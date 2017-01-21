/*********************************************************
 **********		statsheet.c		**********
 ********************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "spsheet.h"
#include "menu.h"
#include "page.h"
#include "error.d"
#include "rsInterface.h"
#include "choices.h"

#include "statsheet.h"

#define MAXITEMS 100
#define EST_YEAR(n)	(((YEAR-1 + n) * 10000) + 1231)

/****** Forward Declarations *******/
PrivateFnDef int getAssumptions (
    char const*			company,
    char const*			type,
    char const*			rules,
    char const*			dataset
);

PrivateFnDef void scanItems (
    char *			line
);

PrivateFnDef int doAnalysis (
    char			type
);

PrivateFnDef void updateAssumption (
    char *			item,
    int				year,
    char *			value
);

/****** Globals *******/
PrivateVarDef char  Type;
PrivateVarDef char *PlugItems[MAXITEMS];
PrivateVarDef int ColCount, FirstEstRow, NotDone;

PrivateVarDef SPRSHEET	*StatSheet, *AssumptSheet;
PrivateVarDef CUR_WINDOW	*StatWin, *AssumptWin, *LineWin;
PrivateVarDef PAGE *ReportPage;

PrivateVarDef void recalc(), printReport(), printReport2();

PrivateVarDef MENU_Choice menuChoices[] = {
 " Calc ",	" Recalculate With New Assumptions",	'c', recalc, ON, 
 " Print ",	" Print Hardcopy Of Report And Assumptions", 'p', 
     printReport, ON, 
 " Report ",	" Print Earnings/Flow of Funds Report", 'r', 
     printReport2, ON, 
 " Worksheet ", " Modify Worksheet Parameters",		'w', NULL, ON, 
 " Globals ",	" View, Modify Stat Sheet Parameters",	'g', NULL, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef void saveAssumptions(), unsaveAssumptions();

PrivateVarDef MENU_Choice saveChoices[] = {
 " Yes ", " Update Assumptions Database With Current Values ",
	     'y', saveAssumptions, ON, 
 " No ",  " Do Not Update the Assumptions Database", 
	    'n', unsaveAssumptions, ON, 
 static_cast<char const*>(NULL), 
};

PublicFnDef void statsheet(char const*company, char const*type, char const*rules, char const*dataset) {
    int i, j, longest;
    CUR_WINDOW *MenuWin;

    Type = type[0];

    MENU::Reference saveMenu (new MENU (saveChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(saveMenu) = " Save Assumptions ? ";
    
/**** create the stat sheet ****/
    ERR_displayStr(" Writing report, please wait...",FALSE);

/**** create assumptions spread sheet object ****/
    if (getAssumptions(company, type, rules, dataset)) return;

    if (doAnalysis(type[0])) return;


/**** create menu object ****/
    MENU::Reference actionMenu (new MENU (menuChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    
/**** create page object ****/
    PAGE_createPage(ReportPage, 3, NULL, actionMenu, NULL, PAGE_menuType, i);
    PAGE_fkey(ReportPage, 1) = NULL;

/*** create report window ***/
    StatWin = CUR_newwin(CUR_LINES - 7, CUR_COLS, 0, 0);
    if( STD_hasInsertDeleteLine )
	CUR_idlok(StatWin,TRUE);
    PAGE_createElement(ReportPage, 0, StatSheet, StatWin, PAGE_Scroll, 
					    SPR_handler, TRUE);
/*** create line window ***/
    LineWin = CUR_newwin(1, CUR_COLS, CUR_LINES - 7, 0);
    WIN_DrawLine(LineWin, 0, '-', i);
    PAGE_createElement(ReportPage, 1, NULL, LineWin, PAGE_Init, NULL, FALSE);
    
/*** create assumptions form ****/
    AssumptWin = CUR_newwin(5, CUR_COLS, CUR_LINES - 6, 0);
    if( STD_hasInsertDeleteLine )
	CUR_idlok(AssumptWin,TRUE);
    PAGE_createElement(ReportPage, 2, AssumptSheet, AssumptWin, PAGE_Input, 
					    SPR_handler, TRUE);

    ERR_displayStr(" Done!", TRUE);
    MenuWin = CUR_newwin(11, 21, 5, 55);

/**** call page handler ****/
    NotDone = TRUE;
    while (NotDone)
    {
	PAGE_handler(ReportPage);
	MENU_handler(saveMenu, MenuWin, PAGE_Input);
    }

    CUR_delwin(MenuWin);
/**** cleanup ****/
    SPR_delete(StatSheet);
    SPR_delete(AssumptSheet);
    CUR_delwin(AssumptWin);
    CUR_delwin(StatWin);
    CUR_delwin(LineWin);
    for (i = 0; i < ColCount; i++)
        free(PlugItems[i]);
	
    PAGE_deletePage(ReportPage, i);
}



/****************************************************
 *****		menu level functions		*****
 ***************************************************/
PrivateFnDef void printReport()
{
    SPR_print(StatSheet, ReportPage);
    SPR_print(AssumptSheet, ReportPage);
}

PrivateFnDef void printReport2()
{
    ERR_displayStr(" Writing report, please wait...",FALSE);
    RS_sendOnly("__tmpCompany displayEarningsHistoryData;");
    RS_sendOnly("__tmpCompany displayEarningsEstimateData;");
    RS_sendOnly("__tmpCompany displayFlowHistoryData;");
    RS_sendLine("__tmpCompany displayFlowEstimateData;");
    RS_printOutput(ReportPage);
}

PrivateFnDef void recalc() {
    int row, col, i, j, year;
    char *cell;
    char *item;
    
    i = 0;
    ERR_displayStr(" Updating assumptions...",FALSE);
    for (row = SPR_fixedRows(AssumptSheet);
	     row < SPR_rowCount(AssumptSheet); row++)
    {
	year = EST_YEAR(i++);
	j = 0;
        for (col = SPR_fixedCols(AssumptSheet);
		 col < SPR_colCount(AssumptSheet); col++)
	{
	    item = PlugItems[j++];
	    cell = SPR_cellPtr(AssumptSheet, row, col);
	    if (SPR_cellStatus(cell) == SPR_Modified)
	        updateAssumption(item, year, SPR_cellValue(cell));
	}
    }
/**** delete estimate data in stat sheet ****/
    for (i = SPR_rowCount(StatSheet) - 1; i >= FirstEstRow; i--)
	SPR_rowCount(StatSheet)--;

#if RSATTACH
    switch (Type)
    {
    case 'E':
	ERR_displayStr(" Calculating earnings...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcEarnings", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating earnings analysis");
	    return;
	}
	ERR_displayStr(" Reading estimate data...",FALSE);
	RS_sendLine("__tmpCompany displayEarningsEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return;
	}
        break;

    case 'F':
        ERR_displayStr (" Calculating funds flow...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcFlow", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating flow of funds");
	    return;
	}
        ERR_displayStr (" Reading estimate data...",FALSE);
	RS_sendLine("__tmpCompany displayFlowEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return;	    
	}
        break;

    case 'R':
        ERR_displayStr (" Calculating reinvestment rate ...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcRR", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating reinvestment rate");
	    return;
	}
        ERR_displayStr (" Reading reinvestment rate data...",FALSE);
	RS_sendLine("__tmpCompany displayRRateEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return;	    
	}
        break;

    case 'D':
        ERR_displayStr (" Calculating dividends ...",FALSE);
        if (FALSE == 
            RS_sendAndCheck("__tmpCompany calcDividends", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating dividends");
	    return;
	}
        ERR_displayStr (" Reading dividend data...",FALSE);
	RS_sendLine("__tmpCompany displayDividendEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return;	    
	}
        break;

    default:
	ERR_displayPause(" Unknown analysis type");
	return;

    }
#endif
    SPR_resetCells(AssumptSheet, cell, row, col);
/*** update the screen ***/
    SPR_paintScreen(StatSheet, StatWin);
    CUR_touchwin(StatWin);
    CUR_wrefresh(StatWin);
}

#if 0
PrivateFnDef int print()
{
}

PrivateFnDef int worksheet()
{
}
 
PrivateFnDef int globals()
{
}
#endif


/****************************************************
 *****		miscelaneous functions		*****
 ***************************************************/

PrivateFnDef int getAssumptions (
    char const*			company,
    char const*			type,
    char const*			rules,
    char const*			dataset
)
{
    char buffer[RS_MaxLine + 1];

#if RSATTACH    
/**** backup assumptions *****/
    if (RS_sendAndCheck("__tmpCompany backupAssumptions", ">>>"))
    {
        ERR_displayPause(" Error Backing Up Assumptions");
	return(TRUE);
    }
    
/**** get item list ****/
    ColCount = 0;
    RS_sendLine("__tmpCompany plugList");
    ERR_displayStr(" Getting plug list...",FALSE);
    while (RS_readLine(buffer, RS_MaxLine))
        scanItems(buffer);

/**** get assumptions spread sheet ****/
    RS_sendLine("__tmpCompany displayAssumptionData");
    SPR_makeSheet(AssumptSheet, 9, (ColCount + 1), 1, 0, 2);
    
    ERR_displayStr(" Reading assumptions...",FALSE);
    if (SPR_readSSheet(AssumptSheet))
    {
	ERR_displayPause(" Error reading assumptions");
	return TRUE;
    }
#endif

#if NOTRSATTACH
    if (NULL == (AssumptSheet = SPR_readSpreadSheet("asspt.sheet")))
    {
	ERR_displayPause(" Error reading assumptions");
	return TRUE;
    }
#endif

    return FALSE;
}

PrivateFnDef void scanItems (char *line) {
    char buffer[RS_MaxLine+1];
    int i, j, len;
    
    len = strlen(line);
    if (line[len - 1] == '\n')
    {
        line[len - 1] = '\0';
	len--;
    }
    j = 0;
    while (line[j] == ' ' && j < len) j++;
    while (j < len)
    {
	i = 0;
	while (line[j] != ' ' && j < len)
	    buffer[i++] = line[j++];
	buffer[i] = '\0';
	if (strlen(buffer) == 0)
	    break;
	PlugItems[ColCount] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
	strcpy(PlugItems[ColCount++], buffer);
	while (line[j] == ' ' && j < len) j++;
    }
}

PrivateFnDef int doAnalysis (char type) {
#if RSATTACH

    SPR_makeSheet(StatSheet, 9, 19, 1, 0, 4);
    SPR_status(StatSheet) = SPR_InitBottom;
    switch (type)
    {
    case 'E':
	ERR_displayStr(" Calculating earnings...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcEarnings", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating earnings analysis");
	    return TRUE;
	}

	ERR_displayStr(" Reading historical data...",FALSE);
	RS_sendLine("__tmpCompany displayEarningsHistoryData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;
	}
	FirstEstRow = SPR_rowCount(StatSheet);
	ERR_displayStr(" Reading estimate data...",FALSE);
	RS_sendLine("__tmpCompany displayEarningsEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;
	}
        break;

    case 'F':
        ERR_displayStr (" Calculating funds flow...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcFlow", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating flow of funds");
	    return TRUE;
	}

        ERR_displayStr (" Reading historical data...",FALSE);
	RS_sendLine("__tmpCompany displayFlowHistoryData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;	    
	}
	FirstEstRow = SPR_rowCount(StatSheet);
	ERR_displayStr(" Reading funds flow data...",FALSE);
	RS_sendLine("__tmpCompany displayFlowEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;
	}
        break;

    case 'R':
        ERR_displayStr (" Calculating reinvestment rate...",FALSE);
        if (FALSE == RS_sendAndCheck("__tmpCompany calcRR", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating reinvestment rate");
	    return TRUE;
	}

        ERR_displayStr (" Reading historical data...",FALSE);
	RS_sendLine("__tmpCompany displayRRateHistoryData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;	    
	}
	FirstEstRow = SPR_rowCount(StatSheet);
	ERR_displayStr(" Reading reinvestment rate data...",FALSE);
	RS_sendLine("__tmpCompany displayRRateEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;
	}
        break;

    case 'D':
        ERR_displayStr (" Calculating dividends...",FALSE);
        if (FALSE == 
              RS_sendAndCheck("__tmpCompany calcDividends", "Calc Done"))
	{
	    ERR_displayPause(" Error calculating dividends");
	    return TRUE;
	}

        ERR_displayStr (" Reading historical data...",FALSE);
	RS_sendLine("__tmpCompany displayDividendHistoryData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;	    
	}
	FirstEstRow = SPR_rowCount(StatSheet);
	ERR_displayStr(" Reading dividend data...",FALSE);
	RS_sendLine("__tmpCompany displayDividendEstimateData");
	if (SPR_readSSheet(StatSheet))
	{
	    ERR_displayPause(" Error reading spread sheet");
	    return TRUE;
	}
        break;

    default:
	ERR_displayPause(" Unknown analysis type");
	return TRUE;

    }
#endif

#if NOTRSATTACH
    if (NULL == (StatSheet = SPR_readSpreadSheet("stat.sheet")))
    {
	ERR_displayPause(" Error reading stat sheet");
	return TRUE;
    }
#endif

    return FALSE;
}

#if RSATTACH

PrivateFnDef void updateAssumption (
    char *			item,
    int				year,
    char *			value
)
{
    char buffer[RS_MaxLine+1];
    sprintf(buffer, "__tmpCompany :%s asOf: %d asDate put: %s", 
			    item, year, value);
/****    ERR_displayPause(buffer);	****/
    ERR_clearMsg();
    if (TRUE == RS_sendAndCheck(buffer, ">>>"))
	ERR_displayPause(" Error updating assumptions database");
}
#endif


PrivateFnDef void saveAssumptions() {
    int i, j, row, col, year;
    char *item;
    char *cell;
    
    NotDone = FALSE;
    i = j = 0;
#if RSATTACH
    for (row = SPR_fixedRows(AssumptSheet);
	     row < SPR_rowCount(AssumptSheet); row++)
    {
	year = EST_YEAR(i++);
	j = 0;
	for (col = SPR_fixedCols(AssumptSheet);
		 col < SPR_colCount(AssumptSheet); col++)
	{
	    item = PlugItems[j++];
	    cell = SPR_cellPtr(AssumptSheet, row, col);
	    if (SPR_cellStatus(cell) == SPR_Modified)
		    updateAssumption(item, year, SPR_cellValue(cell));
	}
    }

    RS_save();
#endif
}

PrivateFnDef void unsaveAssumptions() {
    NotDone = FALSE;
#if RSATTACH
    if (TRUE == RS_sendAndCheck("__tmpCompany rollbackAssumptions", "NA"))
	    ERR_displayPause(" Error rolling back assumptions");
#endif
}

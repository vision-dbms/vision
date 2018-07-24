/****************************************************************************
*****									*****
*****			dataEntry.c					*****
*****									*****
****************************************************************************/

#include "Vk.h"

#include "stdcurses.h"
#include "misc.h"
#include "page.h"
#include "form.h"
#include "rsInterface.h"
#include "choices.h"
#include "error.h"
#include "sps.h"

#include "dataEntry.h"


/*************************************************
 **********	Forward Declarations	**********
 *************************************************/

PrivateFnDef void estDataEntry();

PrivateFnDef void getNewEstObject ();

PrivateFnDef void estExec ();

PrivateFnDef void getObjectAndDate ();

PrivateFnDef void getFundamental ();

PrivateFnDef void fundExec ();

PrivateFnDef void  miscExec ();


PrivateVarDef SPSHEET *MiscSS;
PrivateVarDef FORM *MiscForm;
PrivateVarDef CUR_WINDOW *formWin, *ssWin;

PrivateVarDef SPSHEET *FundSS;
PrivateVarDef FORM *FundForm;
PrivateVarDef CUR_WINDOW *fundformWin, *fundSSWin;

PrivateVarDef	FORM	*Form1, *Form2, *Form3, *Form4;
PrivateVarDef   CUR_WINDOW	*form1Win, *form2Win, *form3Win, *form4Win,
				*ss1Win, *ss2Win, *ss3Win, *ss4Win;
PrivateVarDef	SPSHEET	*SPS1, *SPS2, *SPS3, *SPS4;
PrivateVarDef	FORM	*ObjectForm;
PrivateVarDef	PAGE	*ObjectPage, *CurrPage = NULL;
PrivateVarDef	int	dYear, dMonth, dDay, 
			GotValidObjectAndDate = FALSE;

PrivateVarDef	PAGE	*MiscPage = NULL, *FundPage = NULL, *EstPage = NULL;

#define CompanyObj	0
#define AccountObj	1
#define SecurityObj	2
#define UpdateDate	0
#define EstimateDate	1
PrivateVarDef	int	CurrentObj = 0;
PrivateVarDef	int	DateType = 0;

PrivateVarDef	int	UseAll = 0;
PrivateVarDef	int	UseCalendar = 0;
PrivateVarDef	int	UseFiscal = 0;
PrivateVarDef	int	UseQuarterly = 0;
PrivateVarDef	int	UseDividends = 0;

char	FullObjectName[SPS_maxItemWidth+1];
char	FormattedDate[SPS_maxItemWidth+1];
char	UnFormattedDate[SPS_maxItemWidth+1];
char	CategoryName[SPS_maxItemWidth+1];
char	CompanyFundamentalName[SPS_maxItemWidth+1];
char	FrequencyLetter;

/*************************************************
 **********	Menu Declarations	**********
 *************************************************/

PrivateFnDef void MiscDataEntry ();

PrivateFnDef void companyDataEntry ();
PrivateFnDef void estimateDataEntry ();
PrivateFnDef void companyMiscDataEntry ();
PrivateFnDef void accountDataEntry ();
PrivateFnDef void securityDataEntry ();
PrivateFnDef void fundamentalDataEntry ();
PrivateFnDef void allDataEntry ();
PrivateFnDef void calendarDataEntry ();
PrivateFnDef void fiscalDataEntry ();
PrivateFnDef void quarterlyDataEntry ();
PrivateFnDef void dividendsDataEntry ();
PrivateFnDef void allErase ();

PrivateVarDef MENU_Choice menuChoices1[] = {
    " Company              ",
	    " Scroll To Choice and Push F2; F9 To Quit",
	    'c', companyDataEntry, ON,
    " Security             ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   's', securityDataEntry, ON,
    " Account              ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   'a', accountDataEntry, ON,
    " HouseKeeping         ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   'h',    NULL, ON,
    static_cast<char const*>(NULL),
};

PrivateVarDef MENU_Choice menuChoices2[] = {
    " Estimates            ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   'e',    estimateDataEntry, ON,
    " Fundamental          ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   'f', fundamentalDataEntry, ON,
    " Miscellaneous        ",
	   " Scroll To Choice and Push F2; F9 To Quit",
	   'm',    companyMiscDataEntry, ON,
    static_cast<char const*>(NULL),
};

PrivateVarDef MENU_Choice menuChoices3[] = {
    " Calendar EPS         ",
	   " Calendar Year Earnings per Share", 'c',    calendarDataEntry, ON,
    " Fiscal EPS ",
	   " Fiscal Year Earnings per Share", 'f', fiscalDataEntry, ON,
    " Quarterly EPS        ",
	   " Quarterly Earnings per Share", 'q',    quarterlyDataEntry, ON,
    " DPS (Annual) ",
	   " Annual Dividends per Share", 'd',    dividendsDataEntry, ON,
    " All ",
	   " All of the Above", 'a',    allDataEntry, ON,
    static_cast<char const*>(NULL),
};

PrivateVarDef MENU_Choice dataChoices[] = {
 " Change Company ",	" Change the current Company                       ",
     'c', NULL, ON, 
 " Erase          ",	" Erases All Input Fields of the Worksheet",
     'e', SPS_clearInput, ON, 
 " Blank Cell",	" Erases the Current Cell of the Worksheet",
     'b', SPS_clearCell, ON, 
 " All Erase ",	" Erases All Input Fields of All 4 Worksheet",
     'a', allErase, ON, 
 " Transpose ",	" Transpose the X (columns) and Y (rows) Axes (Also ^T) ",
     't', SPS_transposeAxes, ON, 
 " Flip    ",	" Flip (rotate) the X , Y and Z Axes (Also ^F) ",
     'f', SPS_rotateAxes, ON, 
 " Next ",	" Increment Z to the Next Value (Also ^N) ",
     'n', SPS_incrementZ, ON, 
 " Previous ",	" Decrement Z to the Previous Value (Also ^P) ",
     'p', SPS_decrementZ, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef FORM_Field formFields1[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 31, (CUR_A_UNDERLINE | CUR_A_BOLD), 17, 0, 'a', "Data Entry Module",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 4, 35, (CUR_A_UNDERLINE | CUR_A_BOLD), 9, 0, 'a', "Main Menu",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};

PrivateVarDef FORM_Field formFields2[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 31, (CUR_A_UNDERLINE | CUR_A_BOLD), 17, 0, 'a', "Data Entry Module",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 4, 31, (CUR_A_UNDERLINE | CUR_A_BOLD), 17, 0, 'a', "Company Main Menu",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};

PrivateVarDef FORM_Field formFields3[] = {
 1, 20, CUR_A_NORMAL, 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 31, (CUR_A_UNDERLINE | CUR_A_BOLD), 17, 0, 'a', "Data Entry Module",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 4, 29, (CUR_A_UNDERLINE | CUR_A_BOLD), 21, 0, 'a', "Company Estimate Menu",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};

PublicVarDef int	inDE = FALSE;

PublicFnDef void dataEntryModule() {
    FORM *form;
    PAGE *page;
    PAGE *OldPage;
    CUR_WINDOW *MenuWin, *FormWin;
    int i, j, cols, startcol, longest;
    static int alreadyCentered = FALSE;

    if( inDE )
    {
    	ERR_displayPause("Data Entry already running");
    	return;
    }
    inDE = TRUE;
        
/*** create menu object ***/
    MENU::Reference menu (new MENU (menuChoices1, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));

/*** create menu window ***/
    cols = longest + 4;
    startcol = (CUR_COLS - cols) / 2;
    MenuWin =CUR_newwin(10, cols, 8, startcol);

/**** create form object ****/
    FORM_makeForm(form, formFields1, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form, CUR_COLS);
    }
    strcpy(form->field[0]->value, RS_CompanyName);

/*** create menu window ***/
    FormWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
    OldPage = NULL;
    CurrPage = page;
    PAGE_createElement(page, 0, form, FormWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(page, 1, menu, MenuWin, PAGE_Input,
						 MENU_handler, TRUE);

/**** call page handler ****/
    PAGE_handler(page);

/**** cleanup page ****/
    CUR_delwin(MenuWin);
    CUR_delwin(FormWin);
    free(form);
    PAGE_deletePage(page, i);
    CurrPage = OldPage;

    inDE = FALSE;
    return;
}


/******************************************************
 **********	Exec Functions		***************
 *****************************************************/
PrivateFnDef void companyDataEntry() {
    FORM *form;
    PAGE *page;
    PAGE *OldPage;
    CUR_WINDOW *MenuWin, *FormWin;
    int i, j, cols, startcol, longest;
    static int alreadyCentered = FALSE;

/*** create menu object ***/
    MENU::Reference menu (new MENU (menuChoices2, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));

/*** create menu window ***/
    cols = longest + 4;
    startcol = (CUR_COLS - cols) / 2;
    MenuWin =CUR_newwin(10, cols, 8, startcol);

/**** create form object ****/
    FORM_makeForm(form, formFields2, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form, CUR_COLS);
    }
    strcpy(form->field[0]->value, RS_CompanyName);

/*** create menu window ***/
    FormWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
    OldPage = CurrPage;
    CurrPage = page;
    PAGE_createElement(page, 0, form, FormWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(page, 1, menu, MenuWin, PAGE_Input,
						 MENU_handler, TRUE);

/**** call page handler ****/
    PAGE_handler(page);

/**** cleanup page ****/
    CUR_delwin(MenuWin);
    CUR_delwin(FormWin);
    free(form);
    PAGE_deletePage(page, i);
    CurrPage = OldPage;
}

PublicFnDef void estimateDataEntry() {
    FORM *form;
    PAGE *page;
    PAGE *OldPage;
    CUR_WINDOW *MenuWin, *FormWin;
    int i, j, cols, startcol, longest;
    static int alreadyCentered = FALSE;

/*** create menu object ***/
    MENU::Reference menu (new MENU (menuChoices3, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));

/*** create menu window ***/
    cols = longest + 4;
    startcol = (CUR_COLS - cols) / 2;
    MenuWin =CUR_newwin(10, cols, 8, startcol);

/**** create form object ****/
    FORM_makeForm(form, formFields3, i);
    if( !alreadyCentered )
    {
    	alreadyCentered = TRUE;
    	FORM_centerFormElts(form, CUR_COLS);
    }
    strcpy(form->field[0]->value, RS_CompanyName);

/*** create menu window ***/
    FormWin = CUR_newwin(CUR_LINES - 1, CUR_COLS, 0, 0);

/**** create page object ****/
    PAGE_createPage(page, 2, NULL, NULL, NULL, PAGE_noType, i);
    OldPage = CurrPage;
    CurrPage = page;
    PAGE_createElement(page, 0, form, FormWin, PAGE_Input,
						 FORM_handler, FALSE);
    PAGE_createElement(page, 1, menu, MenuWin, PAGE_Input,
						 MENU_handler, TRUE);

/**** call page handler ****/
    PAGE_handler(page);

/**** cleanup page ****/
    CUR_delwin(MenuWin);
    CUR_delwin(FormWin);
    free(form);
    PAGE_deletePage(page, i);
    CurrPage = OldPage;
}

PrivateVarDef FORM_Field fund1Fields[] = {
 0, 25, (CUR_A_BOLD | CUR_A_UNDERLINE), 29, 0, 'a', "Company Fundamental Worksheet",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 0, (CUR_A_NORMAL), 12, 0, 'a', "Company Name",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 13, (CUR_A_REVERSE), 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 60, (CUR_A_NORMAL), 9, 0, 'a', "Frequency",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 70, (CUR_A_REVERSE), 3, 0, 'a', "   ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 4, (CUR_A_NORMAL), 8, 0, 'a', "Category",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 13, (CUR_A_REVERSE), 25, 0, 'a', "                         ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};
PrivateVarDef FORM_Field est1Fields[] = {
 0, 27, (CUR_A_BOLD | CUR_A_UNDERLINE), 26, 0, 'a', "Company Estimate Worksheet",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 0, (CUR_A_NORMAL), 12, 0, 'a', "Company Name",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 13, (CUR_A_REVERSE), 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 55, CUR_A_NORMAL, 13, 0, 'a', "Estimate Date",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 69, (CUR_A_REVERSE), 10, 0, 'a', "          ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};
PrivateVarDef FORM_Field est2Fields[] = {
 0, 0, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 0, 40, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};
PrivateVarDef FORM_Field est3Fields[] = {
 0, 0, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 0, 40, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};
PrivateVarDef FORM_Field est4Fields[] = {
 0, 0, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 0, 40, CUR_A_NORMAL, 40, 0, 'a', "----------------------------------------",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};

PrivateFnDef SPSHEET *createFundSS()
{
	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet(CompanyFundamentalName));
}
PrivateFnDef SPSHEET *createEstCSS()
{

	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("CompanyUpdateStructureEstC"));
}
PrivateFnDef SPSHEET *createEstFSS()
{

	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("CompanyUpdateStructureEstF"));
}
PrivateFnDef SPSHEET *createEstQSS()
{

	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("CompanyUpdateStructureEstQ"));
}
PrivateFnDef SPSHEET *createEstDSS()
{

	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("CompanyUpdateStructureEstD"));
}

PublicFnDef void calendarDataEntry () {
    UseAll = FALSE;
    UseCalendar = TRUE;
    UseFiscal = FALSE;
    UseQuarterly = FALSE;
    UseDividends = FALSE;
    estDataEntry();
}
PrivateFnDef void fiscalDataEntry () {
    UseAll = FALSE;
    UseCalendar = FALSE;
    UseFiscal = TRUE;
    UseQuarterly = FALSE;
    UseDividends = FALSE;
    estDataEntry();
}
PrivateFnDef void quarterlyDataEntry () {
    UseAll = FALSE;
    UseCalendar = FALSE;
    UseFiscal = FALSE;
    UseQuarterly = TRUE;
    UseDividends = FALSE;
    estDataEntry();
}
PrivateFnDef void dividendsDataEntry () {
    UseAll = FALSE;
    UseCalendar = FALSE;
    UseFiscal = FALSE;
    UseQuarterly = FALSE;
    UseDividends = TRUE;
    estDataEntry();
}
PrivateFnDef void allDataEntry () {
    UseAll = TRUE;
    UseCalendar = TRUE;
    UseFiscal = TRUE;
    UseQuarterly = TRUE;
    UseDividends = TRUE;
    estDataEntry();
}

PrivateFnDef void estDataEntry() {
    SPSHEET *spr;
    PAGE *OldPage;
    int i, longest, j;

    CurrentObj = CompanyObj;
    DateType = EstimateDate;
    getObjectAndDate();
    if( !GotValidObjectAndDate )
	return;

    if( UseCalendar )
    {
	if( (spr = createEstCSS()) == NULL )
	{
	    ERR_displayStr(" Could Not Create Calendar Worksheet",TRUE);
	    return;
	}
	if( UseAll )	SPS1 = spr;
	else		SPS1 = spr;
    }
    else
	SPS1 = NULL;
    if( UseFiscal )
    {
	if( (spr = createEstFSS()) == NULL )
	{
	    ERR_displayStr(" Could Not Create Fiscal Worksheet",TRUE);
	    return;
	}
	if( UseAll )	SPS2 = spr;
	else		SPS1 = spr;
    }
    else
	SPS2 = NULL;
    if( UseQuarterly )
    {
	if( (spr = createEstQSS()) == NULL )
	{
	    ERR_displayStr(" Could Not Create Quarterly Worksheet",TRUE);
	    return;
	}
	if( UseAll )	SPS3 = spr;
	else		SPS1 = spr;
    }
    else
	SPS3 = NULL;
    if( UseDividends )
    {
	if( (spr = createEstDSS()) == NULL )
	{
	    ERR_displayStr(" Could Not Create Dividends Worksheet",TRUE);
	    return;
	}
	if( UseAll )	SPS4 = spr;
	else		SPS1 = spr;
    }
    else
	SPS4 = NULL;
/**** create form objects ****/
    FORM_makeForm(Form1, est1Fields, i);
    if( UseAll )
    {
	FORM_makeForm(Form2, est2Fields, i);
	FORM_makeForm(Form3, est3Fields, i);
	FORM_makeForm(Form4, est4Fields, i);
    }

    strcpy(FORM_fieldValue(FORM_field(Form1,2)),FullObjectName);
    strcpy(FORM_fieldValue(FORM_field(Form1,4)),FormattedDate);
/*** create form windows ****/
    form1Win = CUR_newwin(CUR_LINES-1, CUR_COLS, 0, 0);
    if( UseAll )
    {
	form2Win = CUR_newwin(1, CUR_COLS, 7, 0);
	form3Win = CUR_newwin(1, CUR_COLS, 12, 0);
	form4Win = CUR_newwin(1, CUR_COLS, 17, 0);
    }

/*** create spreadsheets and their windows ***/
    if( UseAll )
    {
	ss1Win = CUR_newwin(4, CUR_COLS, 3, 0);
	ss2Win = CUR_newwin(4, CUR_COLS, 8, 0);
	ss3Win = CUR_newwin(4, CUR_COLS, 13, 0);
	ss4Win = CUR_newwin(4, CUR_COLS, 18, 0);
    }
    else
	ss1Win = CUR_newwin(15, CUR_COLS, 3, 0);

    MENU::Reference actionMenu (new MENU (dataChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(actionMenu) = " Data Entry Options";
    MENU_choiceHandler(actionMenu,0) = getNewEstObject;
    MENU_choiceLabel(actionMenu,0)=" Change Company ";
    MENU_choiceHelp(actionMenu,0)=" Change the Current Company";
    if( UseAll )
	MENU_choiceActive(actionMenu,3) = ON;
    else
	MENU_choiceActive(actionMenu,3) = OFF;
/**** create page object ****/
    if( UseAll )
    {
	PAGE_createPage(EstPage, 8, NULL, actionMenu, NULL, PAGE_menuType, i);
    }
    else
    {
	PAGE_createPage(EstPage, 2, NULL, actionMenu, NULL, PAGE_menuType, i);
    }
    OldPage = CurrPage;
    CurrPage = EstPage;
    PAGE_fkey(EstPage, 1) = estExec;

    PAGE_createElement(EstPage, 0, Form1, form1Win,
					 PAGE_Input, FORM_handler, FALSE);
    PAGE_createElement(EstPage, 1, SPS1, ss1Win,
					 PAGE_Input, SPS_handler, TRUE);
    if( UseAll )
    {
	PAGE_createElement(EstPage, 2, Form2, form2Win,
					     PAGE_Input, FORM_handler, FALSE);
	PAGE_createElement(EstPage, 3, SPS2, ss2Win,
					     PAGE_Input, SPS_handler, TRUE);
	PAGE_createElement(EstPage, 4, Form3, form3Win,
					     PAGE_Input, FORM_handler, FALSE);
	PAGE_createElement(EstPage, 5, SPS3, ss3Win,
					     PAGE_Input, SPS_handler, TRUE);
	PAGE_createElement(EstPage, 6, Form4, form4Win,
					     PAGE_Input, FORM_handler, FALSE);
	PAGE_createElement(EstPage, 7, SPS4, ss4Win,
					     PAGE_Input, SPS_handler, TRUE);
    }

    if( !UseAll )
	SPS_transposeAxes();

/**** call page handler ****/

    PAGE_handler(EstPage);

    if( UseAll )
    {
	if( SPS_sheetStatus(SPS1) != SPS_Normal ||
	    SPS_sheetStatus(SPS2) != SPS_Normal ||
	    SPS_sheetStatus(SPS3) != SPS_Normal ||
	    SPS_sheetStatus(SPS4) != SPS_Normal )
	{
	    if( ERR_promptYesNo("Worksheet(s) have been modified.  Do you want to save") )
		estExec();
	}
    }
    else
    {
	if( SPS_sheetStatus(SPS1) != SPS_Normal )
	{
	    if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
		estExec();
	}
    }
/*****	cleanup  *****/
    free (Form1);
    SPS_delete(SPS1);
    CUR_delwin (ss1Win);
    CUR_delwin (form1Win);
    if( UseAll )
    {
	free (Form2);
	free (Form3);
	free (Form4);
	SPS_delete(SPS2);
	SPS_delete(SPS3);
	SPS_delete(SPS4);
	CUR_delwin (ss2Win);
	CUR_delwin (ss3Win);
	CUR_delwin (ss4Win);
	CUR_delwin (form2Win);
	CUR_delwin (form3Win);
	CUR_delwin (form4Win);
    }
    PAGE_deletePage (EstPage, i)
    EstPage = NULL;
    CurrPage = OldPage;
}

PrivateVarDef FORM_Field miscFields[] = {
 0, 22, (CUR_A_BOLD | CUR_A_UNDERLINE), 36, 0, 'a', "                                    ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 2, 0, (CUR_A_NORMAL), 13, 0, 'a', " Company Name",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 2, 14, (CUR_A_REVERSE), 40, 0, 'a', "                                        ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 2, 57, CUR_A_NORMAL, 11, 0, 'a', "Update Date",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 2, 69, (CUR_A_REVERSE), 10, 0, 'a', "          ",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 -1,
};

#define IOBJLABEL	(ObjectForm->field[0])
#define IOBJECT		(ObjectForm->field[1])
#define IDATELABEL	(ObjectForm->field[2])
#define IFREQ		(ObjectForm->field[3])
#define IMONTH		(ObjectForm->field[4])
#define ICATEGORY	(ObjectForm->field[5])
#define IDAY		(ObjectForm->field[6])
#define IYEAR		(ObjectForm->field[8])
#define ISOURCE		(ObjectForm->field[10])

PrivateFnDef SPSHEET *createAccountMiscSS()
{
	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("AccountUpdateStructureMisc"));
}
PrivateFnDef SPSHEET *createSecurityMiscSS()
{
	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("SecurityUpdateStructureMisc"));
}
PrivateFnDef SPSHEET *createCompanyMiscSS()
{
	ERR_displayStr(" Creating worksheet. Please wait...",FALSE);

	return(SPS_readSSheet("CompanyUpdateStructureMisc"));
}

PrivateFnDef void allErase() {
    SPS_doClearInput(SPS1,ss1Win);
    if( UseAll )
    {
	SPS_doClearInput(SPS2,ss2Win);
	SPS_doClearInput(SPS3,ss3Win);
	SPS_doClearInput(SPS4,ss4Win);
    }
    SPS_refreshPage();
}

PrivateFnDef void getNewEstObject () {
    SPSHEET *spr;

    if( UseAll )
    {
	if( SPS_sheetStatus(SPS1) != SPS_Normal ||
	    SPS_sheetStatus(SPS2) != SPS_Normal ||
	    SPS_sheetStatus(SPS3) != SPS_Normal ||
	    SPS_sheetStatus(SPS4) != SPS_Normal )
	{
	    if( ERR_promptYesNo("Worksheet(s) have been modified.  Do you want to save") )
		estExec();
	    PAGE_status(EstPage) = PAGE_Normal;
	}
    }
    else
    {
	if( SPS_sheetStatus(SPS1) != SPS_Normal )
	{
	    if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
		estExec();
	    PAGE_status(EstPage) = PAGE_Normal;
	}
    }

    getObjectAndDate();
    if( !GotValidObjectAndDate )
	return;

    if( UseCalendar )
    {
	spr = ( UseAll ? SPS1 : SPS1 );
	SPS_readHeadings(spr,"CompanyUpdateStructureEstC",PARTIAL);
	SPS_readCells(spr,"CompanyUpdateStructureEstC");
    }
    if( UseFiscal )
    {
	spr = ( UseAll ? SPS2 : SPS1 );
	SPS_readHeadings(spr,"CompanyUpdateStructureEstF",PARTIAL);
	SPS_readCells(spr,"CompanyUpdateStructureEstF");
    }
    if( UseQuarterly )
    {
	spr = ( UseAll ? SPS3 : SPS1 );
	SPS_readHeadings(spr,"CompanyUpdateStructureEstQ",PARTIAL);
	SPS_readCells(spr,"CompanyUpdateStructureEstQ");
    }
    if( UseDividends )
    {
	spr = ( UseAll ? SPS4 : SPS1 );
	SPS_readHeadings(spr,"CompanyUpdateStructureEstD",PARTIAL);
	SPS_readCells(spr,"CompanyUpdateStructureEstD");
    }
    strcpy(FORM_fieldValue(FORM_field(Form1,2)),FullObjectName);
    strcpy(FORM_fieldValue(FORM_field(Form1,4)),FormattedDate);
    FORM_handler(Form1,form1Win,PAGE_Init);
    SPS_handler(SPS1, ss1Win, PAGE_Init);
    if( UseAll )
    {
	FORM_handler(Form2,form2Win,PAGE_Init);
	SPS_handler(SPS2, ss2Win, PAGE_Init);
	FORM_handler(Form3,form3Win,PAGE_Init);
	SPS_handler(SPS3, ss3Win, PAGE_Init);
	FORM_handler(Form4,form4Win,PAGE_Init);
	SPS_handler(SPS4, ss4Win, PAGE_Init);
    }
    /*
    SPS_refreshPage();
    */
}

PrivateFnDef void getNewFundamental()
{
    SPSHEET *spr;
    char freqBuf[5];

    if( SPS_sheetStatus(FundSS) != SPS_Normal )
    {
	if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
	    fundExec();
	PAGE_status(FundPage) = PAGE_Normal;
    }

    getFundamental();
    if( !GotValidObjectAndDate )
	return;
    if( (spr = createFundSS()) == NULL )
    {
	ERR_displayStr(" Could Not Create New Worksheet",TRUE);
	SPS_refreshPage();
    }
    else
    {
	SPS_delete(FundSS);
	FundSS = spr;
	PAGE_object(FundPage, 1) = (char *)FundSS;
	strcpy(FORM_fieldValue(FORM_field(FundForm,2)),FullObjectName);
	sprintf(freqBuf," %c",(FrequencyLetter == ' ' ? 'A' : FrequencyLetter));
	strcpy(FORM_fieldValue(FORM_field(FundForm,4)),freqBuf);
	strcpy(FORM_fieldValue(FORM_field(FundForm,6)),CategoryName);
	FORM_handler(FundForm,fundformWin,PAGE_Init);
	SPS_handler(FundSS, fundSSWin, PAGE_Init);
    }
}

PrivateFnDef void getNewMiscObject()
{
    if( SPS_sheetStatus(MiscSS) != SPS_Normal )
    {
	if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
	    miscExec();
	PAGE_status(MiscPage) = PAGE_Normal;
    }

    getObjectAndDate();
    if( !GotValidObjectAndDate )
	return;

    switch(CurrentObj)
    {
	case SecurityObj:
	    SPS_readHeadings(MiscSS,"SecurityUpdateStructureMisc",PARTIAL);
	    SPS_readCells(MiscSS,"SecurityUpdateStructureMisc");
	    break;
	case AccountObj:
	    SPS_readHeadings(MiscSS,"AccountUpdateStructureMisc",PARTIAL);
	    SPS_readCells(MiscSS,"AccountUpdateStructureMisc");
	    break;
	case CompanyObj:
	default:
	    SPS_readHeadings(MiscSS,"CompanyUpdateStructureMisc",PARTIAL);
	    SPS_readCells(MiscSS,"CompanyUpdateStructureMisc");
	    break;
    }
    strcpy(FORM_fieldValue(FORM_field(MiscForm,2)),FullObjectName);
    strcpy(FORM_fieldValue(FORM_field(MiscForm,4)),FormattedDate);
    FORM_handler(MiscForm,formWin,PAGE_Init);
    SPS_handler(MiscSS, ssWin, PAGE_Init);
    /*
    SPS_refreshPage();
    */
}

PrivateFnDef void securityDataEntry() {
    CurrentObj = SecurityObj;
    DateType = UpdateDate;
    MiscDataEntry();
}
PrivateFnDef void accountDataEntry() {
    CurrentObj = AccountObj;
    DateType = UpdateDate;
    MiscDataEntry();
}
PrivateFnDef void companyMiscDataEntry () {
    CurrentObj = CompanyObj;
    DateType = UpdateDate;
    MiscDataEntry();
}

PrivateFnDef void estExec () {
    if( UseAll )
    {
	if( SPS_sheetStatus(SPS1) == SPS_Normal )
	    ERR_displayStr("Worksheet has not been modified (since last save)",FALSE);
	else
	    SPS_writeSSheet(SPS1,"CompanyUpdateStructureEstC");
	if( SPS_sheetStatus(SPS2) == SPS_Normal )
	    ERR_displayStr("Worksheet has not been modified (since last save)",FALSE);
	else
	    SPS_writeSSheet(SPS2,"CompanyUpdateStructureEstF");
	if( SPS_sheetStatus(SPS3) == SPS_Normal )
	    ERR_displayStr("Worksheet has not been modified (since last save)",FALSE);
	else
	    SPS_writeSSheet(SPS3,"CompanyUpdateStructureEstQ");
	if( SPS_sheetStatus(SPS4) == SPS_Normal )
	    ERR_displayStr("Worksheet has not been modified (since last save)",FALSE);
	else
	    SPS_writeSSheet(SPS4,"CompanyUpdateStructureEstD");
    }
    else
    {
	if( SPS_sheetStatus(SPS1) == SPS_Normal )
	    ERR_displayStr("Worksheet has not been modified (since last save)",FALSE);
	else if( UseCalendar )
	    SPS_writeSSheet(SPS1,"CompanyUpdateStructureEstC");
	else if( UseFiscal )
	    SPS_writeSSheet(SPS1,"CompanyUpdateStructureEstF");
	else if( UseQuarterly )
	    SPS_writeSSheet(SPS1,"CompanyUpdateStructureEstQ");
	else if( UseDividends )
	    SPS_writeSSheet(SPS1,"CompanyUpdateStructureEstD");
    }
    PAGE_status(EstPage) = PAGE_ExitOnExec;
}

PrivateFnDef void miscExec () {
    if( SPS_sheetStatus(MiscSS) == SPS_Normal )
	ERR_displayPause("Worksheet has not been modified (since last save)");
    else
    {
	switch(CurrentObj)
	{
	    case SecurityObj:
		SPS_writeSSheet(MiscSS,"SecurityUpdateStructureMisc");
		break;
	    case AccountObj:
		SPS_writeSSheet(MiscSS,"AccountUpdateStructureMisc");
		break;
	    case CompanyObj:
	    default:
		SPS_writeSSheet(MiscSS,"CompanyUpdateStructureMisc");
		break;
	}
    }
    PAGE_status(MiscPage) = PAGE_ExitOnExec;
}

PrivateFnDef void fundExec () {
    if( SPS_sheetStatus(FundSS) == SPS_Normal )
	ERR_displayPause("Worksheet has not been modified (since last save)");
    else
	SPS_writeSSheet(FundSS,"CompanyUpdateStructureFundamental");
    PAGE_status(FundPage) = PAGE_ExitOnExec;
}

PrivateFnDef void MiscDataEntry()
{
    PAGE *OldPage;
    int i, longest, j;

    getObjectAndDate();
    if( !GotValidObjectAndDate )
	return;

/*** create spreadsheets and their windows ***/
    switch(CurrentObj)
    {
	case SecurityObj:
	    MiscSS = createSecurityMiscSS();
	    break;
	case AccountObj:
	    MiscSS = createAccountMiscSS();
	    break;
	case CompanyObj:
	default:
	    MiscSS = createCompanyMiscSS();
	    break;
    }
    if( MiscSS == (SPSHEET *)NULL )
    {
	ERR_displayStr(" Could Not Create Worksheet",TRUE);
	return;
    }
    ssWin = CUR_newwin(CUR_LINES-6, CUR_COLS, 4, 0);

/**** create form objects ****/
    FORM_makeForm(MiscForm, miscFields, i);
    strcpy(FORM_fieldValue(FORM_field(MiscForm,2)),FullObjectName);
    strcpy(FORM_fieldValue(FORM_field(MiscForm,4)),FormattedDate);
    switch(CurrentObj)
    {
	case SecurityObj:
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,0)),
		"Security Miscellaneous Worksheet");
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,1)),"Security Name");
	    break;
	case AccountObj:
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,0)),
		"Account Miscellaneous Worksheet");
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,1))," Account Name");
	    break;
	case CompanyObj:
	default:
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,0)),
		"Company Miscellaneous Worksheet");
	    strcpy(FORM_fieldValue(FORM_field(MiscForm,1))," Company Name");
	    break;
    }
    FORM_fieldLen(FORM_field(MiscForm,0)) =
	strlen(FORM_fieldValue(FORM_field(MiscForm,0)));
    FORM_fieldX(FORM_field(MiscForm,0)) =
	(CUR_COLS - FORM_fieldLen(FORM_field(MiscForm,0))) / 2;

/*** create form windows ****/
    formWin = CUR_newwin(CUR_LINES-1, CUR_COLS, 0, 0);

/**** create menu object ****/
    MENU::Reference actionMenu (new MENU (dataChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(actionMenu) = " Data Entry Options";
    MENU_choiceHandler(actionMenu,0) = getNewMiscObject;
    MENU_choiceActive(actionMenu,3) = OFF;
    switch(CurrentObj)
    {
	case SecurityObj:
	    MENU_choiceLabel(actionMenu,0)=" Change Security";
	    MENU_choiceHelp(actionMenu,0)=" Change the Current Security";
	    break;
	case AccountObj:
	    MENU_choiceLabel(actionMenu,0)=" Change Account ";
	    MENU_choiceHelp(actionMenu,0)=" Change the Current Account";
	    break;
	case CompanyObj:
	default:
	    MENU_choiceLabel(actionMenu,0)=" Change Company ";
	    MENU_choiceHelp(actionMenu,0)=" Change the Current Company";
	    break;
    }

/**** create page object ****/
    PAGE_createPage(MiscPage, 2, NULL, actionMenu, NULL, PAGE_menuType, i);
    OldPage = CurrPage;
    CurrPage = MiscPage;
    PAGE_fkey(MiscPage, 1) = miscExec;

    PAGE_createElement(MiscPage, 0, MiscForm, formWin,
					 PAGE_Input, FORM_handler, FALSE);
    PAGE_createElement(MiscPage, 1, MiscSS, ssWin,
					 PAGE_Input, SPS_handler, TRUE);

/**** call page handler ****/

    PAGE_handler(MiscPage);

    if( SPS_sheetStatus(MiscSS) != SPS_Normal )
    {
	if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
	    miscExec();
    }

/*****	cleanup  *****/
    free (MiscForm);
    SPS_delete(MiscSS);
    CUR_delwin (ssWin);
    CUR_delwin (formWin);
    PAGE_deletePage (MiscPage, i)
    MiscPage = NULL;
    CurrPage = OldPage;
}

PrivateVarDef FORM_Field objUpdFields[] = {
 1, 4, CUR_A_NORMAL, 9, 0, 'a', " Company:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 14, (CUR_A_BOLD | CUR_A_REVERSE), 20, 1, 'a', "                    ",
	" Enter Company Ticker Symbol                      ", MENU::Reference(), NULL,
 3, 3, CUR_A_NORMAL, 10, 0, 'a', "    Update",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 14, CUR_A_NORMAL, 5, 0, 'a', "Date:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 20, (CUR_A_BOLD | CUR_A_REVERSE), 2, 1, 'n', "  ",
	" Enter Month", MENU::Reference(), NULL,
 3, 22, CUR_A_NORMAL, 1, 0, 'a', "/",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 23, (CUR_A_BOLD | CUR_A_REVERSE), 2, 1, 'n', "  ",
	" Enter Day", MENU::Reference(), NULL,
 3, 25, CUR_A_NORMAL, 1, 0, 'a', "/",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 26, (CUR_A_BOLD | CUR_A_REVERSE), 4, 1, 'n', "    ",
	" Enter Year", MENU::Reference(), NULL,
 5, 5, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" ,
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
-1,
};
PrivateVarDef FORM_Field objEstFields[] = {
 1, 4, CUR_A_NORMAL, 9, 0, 'a', " Company:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 14, (CUR_A_BOLD | CUR_A_REVERSE), 20, 1, 'a', "                    ",
	" Enter Company Ticker Symbol ", MENU::Reference(), NULL,
 3, 3, CUR_A_NORMAL, 10, 0, 'a', "  Estimate",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 14, CUR_A_NORMAL, 5, 0, 'a', "Date:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 20, (CUR_A_BOLD | CUR_A_REVERSE), 2, 1, 'n', "  ",
	" Enter Month", MENU::Reference(), NULL,
 3, 22, CUR_A_NORMAL, 1, 0, 'a', "/",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 23, (CUR_A_BOLD | CUR_A_REVERSE), 2, 1, 'n', "  ",
	" Enter Day", MENU::Reference(), NULL,
 3, 25, CUR_A_NORMAL, 1, 0, 'a', "/",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 26, (CUR_A_BOLD | CUR_A_REVERSE), 4, 1, 'n', "    ",
	" Enter Year", MENU::Reference(), NULL,
 5, 6, CUR_A_NORMAL, 7, 0, 'a', "Source:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 5, 14, (CUR_A_BOLD | CUR_A_REVERSE), 20, 1, 'm', "                    ",
        " Use Arrow Keys to Select Source Type, or F1 For Menu", MENU::Reference(), NULL, 
 7, 7, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" ,
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
-1,
};
PrivateVarDef MENU_Choice sourceChoices[] = {
 "INT",	   "Use internal name",	'i', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateVarDef FORM_Field fund2Fields[] = {
 1, 6, CUR_A_NORMAL, 8, 0, 'a', "Company:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 1, 15, (CUR_A_BOLD | CUR_A_REVERSE), 20, 1, 'a', "                    ",
	" Enter Company Ticker Symbol ", MENU::Reference(), NULL,
 3, 4, CUR_A_NORMAL, 10, 0, 'a', "Frequency:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 3, 15, (CUR_A_BOLD | CUR_A_REVERSE), 3, 1, 'm', "   ",
        " Use Arrow Keys to Select Frequency Type, or F1 For Menu", MENU::Reference(), NULL, 
 5, 5, CUR_A_NORMAL, 9, 0, 'a', "Category:",
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
 5, 15, (CUR_A_BOLD | CUR_A_REVERSE), 25, 1, 'm', "                         ",
        " Use Arrow Keys to Select Source Type, or F1 For Menu", MENU::Reference(), NULL, 
 7, 7, CUR_A_NORMAL, 29, 0, 'a', "Execute(F2)  Quit(F9)" ,
	static_cast<char const*>(NULL), MENU::Reference(), NULL,
-1,
};
PrivateVarDef MENU_Choice freqChoices[] = {
 " A",	   " Annual",	'a', FORM_menuToForm, ON, 
 " Q",	   " Quarterly", 'q', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};
PrivateVarDef MENU_Choice categoryChoices[] = {
 "BalanceSheet",   " Balance Sheet", 'b', FORM_menuToForm, ON, 
 static_cast<char const*>(NULL), 
};

PrivateFnDef int validateAccount()
{
    char buffer[128];
    ERR_displayStr(" Validating Account and Date...",FALSE);

    if (isBlank(FORM_fieldValue(IOBJECT)))
    {
	ERR_displayPause(" Please Enter Account Code");
	return(TRUE);
    }

    strToUpper(FORM_fieldValue(IOBJECT));
    snprintf(buffer, sizeof(buffer), "!__tmpAccount <- Named Account \\%s", FORM_fieldValue(IOBJECT));
    RS_dumpOutput();
    if (RS_sendAndCheck(buffer, ">>>"))
    {
	ERR_displayPause(" Invalid Account");
	return(TRUE);
    }

    sprintf(buffer, "__tmpAccount name");
    RS_sendLine(buffer);
    if( !RS_readLine(buffer,80) )
	buffer[0] = '\0';
    strcpy(FullObjectName,eatLeadingAndTrailingSpaces(buffer));
    while( RS_readLine(buffer,80) ) ;
    return(FALSE);
}
PrivateFnDef int validateCompany(char const *str) {
    char buffer[128];
    ERR_displayStr(str,FALSE);

    if (isBlank(FORM_fieldValue(IOBJECT)))
    {
	ERR_displayPause(" Please Enter Company Ticker Symbol (or Cusip)");
	return(TRUE);
    }

    strToUpper(FORM_fieldValue(IOBJECT));
    snprintf(
        buffer,
	sizeof(buffer),
	"!__tmpCompany <- Named Company \\%s", FORM_fieldValue(IOBJECT)
    );
    RS_dumpOutput();
    if (RS_sendAndCheck(buffer, ">>>"))
    {
	ERR_displayPause(" Invalid Company");
	return(TRUE);
    }

    snprintf(buffer, sizeof(buffer), "__tmpCompany name");
    RS_sendLine(buffer);
    if( !RS_readLine(buffer,80) )
	buffer[0] = '\0';
    strcpy(FullObjectName,eatLeadingAndTrailingSpaces(buffer));
    while( RS_readLine(buffer,80) ) ;
    return(FALSE);
}
PrivateFnDef int validateSecurity() {
    char buffer[128];
    ERR_displayStr(" Validating Security and Date...",FALSE);

    if (isBlank(FORM_fieldValue(IOBJECT)))
    {
	ERR_displayPause(" Please Enter Security Ticker Symbol (or Cusip)");
	return(TRUE);
    }

    strToUpper(FORM_fieldValue(IOBJECT));
    snprintf(
	buffer,
	sizeof(buffer),
	"!__tmpSecurity <- Named Security \\%s", FORM_fieldValue(IOBJECT)
    );
    RS_dumpOutput();
    if (RS_sendAndCheck(buffer, ">>>"))
    {
	ERR_displayPause(" Invalid Security");
	return(TRUE);
    }

    snprintf(buffer, sizeof(buffer), "__tmpSecurity name");
    RS_sendLine(buffer);
    if( !RS_readLine(buffer,80) )
	buffer[0] = '\0';
    strcpy(FullObjectName,eatLeadingAndTrailingSpaces(buffer));
    while( RS_readLine(buffer,80) ) ;
    return(FALSE);
}

PrivateFnDef void validateInitial() {
    char buffer[128];

    GotValidObjectAndDate = FALSE;

    switch(CurrentObj)
    {
	case SecurityObj:
	    if( validateSecurity() ) return;
	    break;
	case AccountObj:
	    if( validateAccount() ) return;
	    break;
	case CompanyObj:
	default:
	    if( validateCompany(" Validating Company and Date...") ) return;
	    break;
    }
    dYear = atoi(FORM_fieldValue(IYEAR));
    if (dYear <= 100) dYear += 1900;

    dMonth = atoi(FORM_fieldValue(IMONTH));
    if (dMonth < 1 || dMonth > 12)
    {
	ERR_displayPause(" Invalid Month");
	return;
    }

    dDay = atoi(FORM_fieldValue(IDAY));
    if (dDay < 1 || dDay > 31)
    {
	ERR_displayPause(" Invalid Day");
	return;
    }

    snprintf(buffer, sizeof(buffer), "!__tmpDate <- %04d%02d%02d asDate", dYear,dMonth,dDay);
    if (RS_sendAndCheck(buffer, "       NA"))
    {
	ERR_displayPause(" Invalid Date");
	return;
    }
    snprintf(FormattedDate, sizeof(FormattedDate),"%02d/%02d/%04d",dMonth,dDay,dYear);
    snprintf(UnFormattedDate, sizeof(UnFormattedDate), "%04d%02d%02d", dYear,dMonth,dDay);

    if( DateType == EstimateDate )
    {
	strToUpper(FORM_fieldValue(IOBJECT));
	snprintf(
	    buffer,
	    sizeof(buffer),
	    "!__tmpSource <- Named EstimateSource \\%s", FORM_fieldValue(ISOURCE)
	);
	RS_dumpOutput();
	if (RS_sendAndCheck(buffer, ">>>"))
	{
	    ERR_displayPause(" Invalid Source");
	    return;
	}
    }
    PAGE_status(ObjectPage) = PAGE_ExitOnExec;
    GotValidObjectAndDate = TRUE;
}

PrivateVarDef MENU::Reference sourceMenu;

PrivateFnDef void getObjectAndDate () {
	CUR_WINDOW	*w1, *w2;
	int		i, j, longest;
	char		obuf[81], dbuf[81], tbuf[81];

	GotValidObjectAndDate = FALSE;

	switch(DateType)
	{
	    case EstimateDate:
		w1 = CUR_newwin(11,50,10,1);
		w2 = CUR_newwin(9,48,11,2);
		FORM_makeForm(ObjectForm,objEstFields,i);
		strcpy(dbuf,"Estimate");
		break;
	    case UpdateDate:
	    default:
		w1 = CUR_newwin(9,50,10,1);
		w2 = CUR_newwin(7,48,11,2);
		FORM_makeForm(ObjectForm,objUpdFields,i);
		strcpy(dbuf,"Update");
		break;
	}
	switch(CurrentObj)
	{
	    case SecurityObj:
		strcpy(obuf,"Security:");
		strcpy(tbuf," Enter Security Ticker Name");
		WIN_ReverseBox(w1," Security Update Input Form");
		break;
	    case AccountObj:
		strcpy(obuf,"Account:");
		strcpy(tbuf," Enter Account Code");
		WIN_ReverseBox(w1," Account Update Input Form");
		break;
	    case CompanyObj:
	    default:
		strcpy(obuf,"Company:");
		strcpy(tbuf," Enter Company Ticker Symbol");
		if( DateType == EstimateDate )
		    WIN_ReverseBox(w1," Company Estimate Input Form");
		else
		    WIN_ReverseBox(w1," Company Update Input Form");
		break;
	}
	sprintf(FORM_fieldValue(IOBJLABEL),"%*.*s",
		FORM_fieldLen(IOBJLABEL),
		FORM_fieldLen(IOBJLABEL),
		obuf);
	sprintf(FORM_fieldValue(IDATELABEL),"%*.*s",
		FORM_fieldLen(IDATELABEL),
		FORM_fieldLen(IDATELABEL),
		dbuf);
	sprintf(FORM_fieldValue(IOBJECT),"%*.*s",
		FORM_fieldLen(IOBJECT),
		FORM_fieldLen(IOBJECT),
		" ");
	if( DateType == UpdateDate )
	{
	    FORM_fieldHelp(IOBJECT).printf ("%-*.*s", 50, 50, tbuf);
	}

	if( DateType == EstimateDate )
	{
	    if( sourceMenu.isNil () ) {
		sourceMenu.setTo (MENU_getMenu("EstimateSource getMenu"));
		if( sourceMenu.isNil () ) {
		    CUR_beep();
		    sourceMenu.setTo (new MENU (sourceChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
		} else 	{
		    for( i=0 ; i<MENU_choiceCount(sourceMenu) ; i++ ) {
			MENU_choiceHandler(sourceMenu,i) = FORM_menuToForm;
			MENU_choiceActive(sourceMenu,i) = ON;
		    }
		}
		MENU_title(sourceMenu) = " Source:";
	    }
	    ObjectForm->field[10]->menu.setTo (sourceMenu);
	}

	if( CurrPage == NULL )
	    j = 0;
	else
	    j = PAGE_windowCount(CurrPage);
	PAGE_createPage(ObjectPage,(j+2),NULL,NULL,NULL,PAGE_noType,i);
	for( i=0 ; i<j ; i++ )
	{
	    PAGE_createElement(ObjectPage, i, NULL, 
			    PAGE_window(CurrPage,i), PAGE_Input, NULL, FALSE);
	}
	PAGE_createElement(ObjectPage, j, NULL, w1, PAGE_Input, NULL, FALSE);
	PAGE_createElement(ObjectPage, (j+1), ObjectForm, w2, PAGE_Input,
			FORM_handler, TRUE);
	PAGE_fkey(ObjectPage,1) = validateInitial;
	PAGE_handler(ObjectPage);

	CUR_delwin(w2);
	CUR_delwin(w1);
	free(ObjectForm);
	PAGE_deletePage(ObjectPage,i);
}

PublicFnDef void fundamentalDataEntry () {
    PAGE *OldPage;
    int i, longest, j;
    char freqBuf[5];

    CurrentObj = CompanyObj;
    DateType = UpdateDate;
    getFundamental();
    if( !GotValidObjectAndDate )
	return;

    if( (FundSS = createFundSS()) == NULL )
    {
	ERR_displayStr(" Could Not Create Calendar Worksheet",TRUE);
	return;
    }
/**** create form objects ****/
    FORM_makeForm(FundForm, fund1Fields, i);

    strcpy(FORM_fieldValue(FORM_field(FundForm,2)),FullObjectName);
    sprintf(freqBuf," %c",(FrequencyLetter == ' ' ? 'A' : FrequencyLetter));
    strcpy(FORM_fieldValue(FORM_field(FundForm,4)),freqBuf);
    strcpy(FORM_fieldValue(FORM_field(FundForm,6)),CategoryName);
/*** create form windows ****/
    fundformWin = CUR_newwin(CUR_LINES-1, CUR_COLS, 0, 0);

/*** create spreadsheets and their windows ***/
    fundSSWin = CUR_newwin(15, CUR_COLS, 5, 0);

    MENU::Reference actionMenu (new MENU (dataChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
    MENU_title(actionMenu) = " Data Entry Options";
    MENU_choiceHandler(actionMenu,0) = getNewFundamental;
    MENU_choiceLabel(actionMenu,0)=" Change Company ";
    MENU_choiceHelp(actionMenu,0)=" Change the Current Company";
    MENU_choiceActive(actionMenu,3) = OFF;
/**** create page object ****/
    PAGE_createPage(FundPage, 2, NULL, actionMenu, NULL, PAGE_menuType, i);
    OldPage = CurrPage;
    CurrPage = FundPage;
    PAGE_fkey(FundPage, 1) = fundExec;

    PAGE_createElement(FundPage, 0, FundForm, fundformWin,
					 PAGE_Input, FORM_handler, FALSE);
    PAGE_createElement(FundPage, 1, FundSS, fundSSWin,
					 PAGE_Input, SPS_handler, TRUE);

/**** call page handler ****/

    PAGE_handler(FundPage);

    if( SPS_sheetStatus(FundSS) != SPS_Normal )
    {
	if( ERR_promptYesNo("Worksheet has been modified.  Do you want to save") )
	    fundExec();
    }
/*****	cleanup  *****/
    free (FundForm);
    SPS_delete(FundSS);
    CUR_delwin (fundSSWin);
    CUR_delwin (fundformWin);
    PAGE_deletePage (FundPage, i)
    FundPage = NULL;
    CurrPage = OldPage;
}
PrivateFnDef void validateFundamental()
{
    char buffer[81], c;
    int compustat, error, exists, none;

    GotValidObjectAndDate = FALSE;
    compustat = error = exists = none = FALSE;

    if( validateCompany(" Validating Company...") ) return;

    sprintf(buffer, "CompanyUpdateStructureFundamental getPrivateDataStatus");
    RS_sendLine(buffer);
    while( RS_readLine(buffer,80) )
    {
	if( strncmp(buffer,">>>",3) == 0 )
	    error = TRUE;
	if( strncmp(buffer,"Compustat",9) == 0 )
	    compustat = TRUE;
	if( strncmp(buffer,"Exists",6) == 0 )
	    exists = TRUE;
	if( strncmp(buffer,"None",4) == 0 )
	    none = TRUE;
    }

    if( compustat )
    {
	ERR_displayPause("Compustat data already exists for company");
	return;
    }
    else if( exists )
    {
	ERR_clearMsg();
    }
    else if( none )
    {
	if( !ERR_promptYesNo("Private Data Store Not Setup:  Create it") )
	    return;
	while( TRUE )
	{
	    if( ERR_promptForInt("Enter month (1 - 12) for fiscal year-end (F9 to abort): ", &dMonth) ) 
		return;
	    if (dMonth < 1 || dMonth > 12)
		CUR_beep();
	    else
		break;
	}
	sprintf(buffer, "CompanyUpdateStructureFundamental createNewCompanyFYR: %d",dMonth);
	if (RS_sendAndCheck(buffer, ">>>"))
	{
	    ERR_displayPause(" Error creating new company FYR");
	    return;
	}
    }
    else if( error )
    {
	ERR_displayPause("Error validating Company");
	return;
    }

    V::VString s (eatLeadingAndTrailingSpaces(FORM_fieldValue(IFREQ)));
    c = *s;
    if( c == 'A' )
	FrequencyLetter = ' ';
    else
	FrequencyLetter = c;
	
    strcpy(CategoryName,eatLeadingAndTrailingSpaces(FORM_fieldValue(ICATEGORY)));

    sprintf(CompanyFundamentalName,"CompanyFundamentalStructure%s%c",CategoryName,FrequencyLetter);

    PAGE_status(ObjectPage) = PAGE_ExitOnExec;
    GotValidObjectAndDate = TRUE;
}

PrivateVarDef MENU::Reference categoryMenu;

PrivateFnDef void getFundamental () {
	CUR_WINDOW	*w1, *w2;
	int		i, j, longest;

	GotValidObjectAndDate = FALSE;

	w1 = CUR_newwin(11,50,10,1);
	WIN_ReverseBox(w1," Company Fundamental Input Form");
	w2 = CUR_newwin(9,48,11,2);
	FORM_makeForm(ObjectForm,fund2Fields,i);

	MENU::Reference freqMenu (new MENU (freqChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	MENU_title(freqMenu) = " Frequency:";
	ObjectForm->field[3]->menu.setTo (freqMenu);

	if( categoryMenu.isNil () ) {
	    categoryMenu.setTo (MENU_getMenu("CompanyUpdateStructureFundamental categoryList"));
	    if( categoryMenu.isNil () ) {
		CUR_beep();
		categoryMenu.setTo (new MENU (categoryChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j));
	    } else {
		for( i=0 ; i<MENU_choiceCount(categoryMenu) ; i++ ) {
		    MENU_choiceHandler(categoryMenu,i) = FORM_menuToForm;
		    MENU_choiceActive(categoryMenu,i) = ON;
		}
	    }
	    MENU_title(categoryMenu) = " Category:";
	}
	ObjectForm->field[5]->menu = categoryMenu;

	if( CurrPage == NULL )
	    j = 0;
	else
	    j = PAGE_windowCount(CurrPage);
	PAGE_createPage(ObjectPage,(j+2),NULL,NULL,NULL,PAGE_noType,i);
	for( i=0 ; i<j ; i++ )
	{
	    PAGE_createElement(ObjectPage, i, NULL, 
			    PAGE_window(CurrPage,i), PAGE_Input, NULL, FALSE);
	}
	PAGE_createElement(ObjectPage, j, NULL, w1, PAGE_Input, NULL, FALSE);
	PAGE_createElement(ObjectPage, (j+1), ObjectForm, w2, PAGE_Input,
			FORM_handler, TRUE);
	PAGE_fkey(ObjectPage,1) = validateFundamental;
	PAGE_handler(ObjectPage);

	CUR_delwin(w2);
	CUR_delwin(w1);
	free(ObjectForm);
	PAGE_deletePage(ObjectPage,i);
}

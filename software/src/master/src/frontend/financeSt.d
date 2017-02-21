/***************************************************
 **********	financeSt.d	********************
 **************************************************/

#ifndef FINANCEsT_D
#define FINANCEsT_D

#define STARTDATE 0
#define ENDDATE 19991231

/*---------------------------------------------------------------------------
 *	Note: the following defines determine which fields in the form
 *	      are used in the analysis, these MUST correspond to the 
 *	      correct fields
 *___________________________________________________________________________
 */

#define CompanyField	    (Form1->field[1]->value)
#define ReportField	    (&Form1->field[3]->value[1])
#define FreqField	    (&Form1->field[5]->value[1])
#define DateField	    (Form1->field[7]->value)
#define NumberField	    (Form1->field[9]->value)
#define DirectionField	    (&Form1->field[11]->value[1])
#define Direction	    (Form1->field[11]->value[1])

/***** fields menus *****/

#define ReportMenu	(Form1->field[3]->menu)
#define FreqMenu	(Form1->field[5]->menu)
#define DirMenu    	(Form1->field[11]->menu)

#define determineReport()\
{\
    switch (MENU_currChoice (ReportMenu) )\
    {\
    case 0:	/* Balance Sheet */\
	if (*FreqField == 'A')\
             ReportType = "bsheet:";\
	else ReportType = "bsheetQ:";\
	break;\
    case 1:	/* Income Statement */\
	if (*FreqField == 'A')\
             ReportType = "istmt:";\
	else ReportType = "istmtQ:";\
	break;\
    case 2:	/* Sources and Uses */\
	if (*FreqField == 'A')\
             ReportType = "changes:";\
	else ReportType = "changesQ:";\
	break;\
    case 3:	/* Ratio Analysis */\
	if (*FreqField == 'A')\
             ReportType = "ratios:";\
	else ReportType = "ratiosQ:";\
	break;\
    case 4:	/* Credit Analysis */\
	if (*FreqField == 'A')\
             ReportType = "credit:";\
	else ReportType = "creditQ:";\
	break;\
    case 5:	/* Balance Sheet - Percentage */\
	if (*FreqField == 'A')\
             ReportType = "bsheetPCT:";\
	else ReportType = "bsheetPCTQ:";\
	break;\
    case 6:	/* Balance Sheet - Trend */\
	if (*FreqField == 'A')\
             ReportType = "bsheetT:";\
	else ReportType = "bsheetTQ:";\
	break;\
    case 7:	/* Income Statement - Percentage */\
	if (*FreqField == 'A')\
             ReportType = "istmtPCT:";\
	else ReportType = "istmtPCTQ:";\
	break;\
    case 8:	/* Income Statement - Trend */\
	if (*FreqField == 'A')\
             ReportType = "istmtT:";\
	else ReportType = "istmtTQ:";\
	break;\
    }\
}

#endif

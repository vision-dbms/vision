#include "Vk.h"

#include "stdcurses.h"
#include "menu.h"
#include "form.h"
#include "page.h"
#include "edit.h"
#include "gopt.h"

#include "vars.h"

PublicFnDecl void statmenu(), queries(), financeSt(), dataEntryModule();
PublicFnDecl void MAIN_getCompany(), MAIN_getUniverse(), timeSeries(), browser();

PublicVarDef char	StartupKit[VARS_maxLen];
PublicVarDef void 	(*StartupModule)(void) = NULL;
PublicVarDef void 	(*DefaultModule)(void) = EDIT_main;

#define	STARTUPKIT	0
#define STARTUPMODULE	1
#define DEFAULTMODULE	2

#define LASTVARIABLE	2

PrivateVarDef MENU_Choice VarChoices[] = {
 "StartupKit",	  " Name Of Kit Which Contains List Of Startup Modules", '\0', FORM_menuToForm, ON,
 "StartupModule", " Name of module to execute if no StartupKit exists",  '\0', FORM_menuToForm, ON,
 "DefaultModule", " Name of module to execute when F4 is pressed",  '\0', FORM_menuToForm, ON,
 NULL,
};

PrivateVarDef VARS_Type VarNames[] = {
 "StartupKit",	  VARS_stringType,	(caddr_t)StartupKit,	 NULL,	NULL,
 "StartupModule", VARS_funcType,	(caddr_t)&StartupModule, NULL,	NULL,
 "DefaultModule", VARS_funcType,	(caddr_t)&DefaultModule, NULL,	NULL,
 NULL,
};

#define EDITMODULE	9
#define NOMODULE	0

PrivateVarDef MENU_Choice moduleChoices[] = {
 "none",	 " No Module",					'n', FORM_menuToForm, ON,
 "FinanceSt",    " Financial Statement Analysis",		'f', FORM_menuToForm, ON, 
 "Profile",      " Summary Information",			'c', FORM_menuToForm, ON, 
 "Models",       " Stat Sheet and Other Modeling Applications",	'm', FORM_menuToForm, ON,
 "Screening",    " Ad Hoc Screening",				's', FORM_menuToForm, ON, 
 "ReportWriter", " User Defined Reports",			'r', FORM_menuToForm, ON, 
 "TimeSeries",   " User Defined Reports Over Time",		't', FORM_menuToForm, ON, 
 "DataEntry",    " Data Entry module",				'd', FORM_menuToForm, ON,
 "Browser",      " Browse Through Objects in the System",	'b', FORM_menuToForm, ON,
 "Editor",	 " System Editor",				'e', FORM_menuToForm, ON,
 NULL,
};

PrivateVarDef MENU_Choice booleanChoices[] = {
 "FALSE",	" FALSE booloean value",	'f', FORM_menuToForm, ON,
 "TRUE",	" TRUE boolean value",		't', FORM_menuToForm, ON,
 NULL,
};

#define VariableName	2
#define	VariableValue	3

PrivateVarDef FORM_Field formFields[] = {
 1, 1, CUR_A_NORMAL, 9, 0, 'a', "Variable:",
 	NULL, NULL, NULL,
 3, 1, CUR_A_NORMAL, 6, 0, 'a', "Value:",
 	NULL, NULL, NULL,
 1, 11, CUR_A_REVERSE, 20, 1, 'S', "                    ",
 	" Use Arrow Keys to Select Variable Name", NULL, NULL,
 3, 11, CUR_A_REVERSE, 30, 1, 'X', "                              ",
 	NULL, NULL, NULL,
 3, 11, CUR_A_REVERSE, 30, FORM_ScrollFlag, 'A', "                              ",
 	" Enter Startup Kit Name", NULL, NULL,
 3, 11, CUR_A_REVERSE, 30, 0, 'M', "                              ",
 	" Use Arrow Keys to Select Startup Module", NULL, NULL,
 3, 11, CUR_A_REVERSE, 30, 0, 'M', "                              ",
 	" Use Arrow Keys to Select Default Module (F4 key)", NULL, NULL,
 -1,
};

PublicVarDef VARS_Menu NameAndFunc[] = {
	"FinanceSt",		financeSt,
	"Profile",		MAIN_getCompany,
	"Models",		statmenu,
	"Screening",		queries,
	"ReportWriter",		MAIN_getUniverse,
	"TimeSeries",		timeSeries,
	"DataEntry",		dataEntryModule,
	"Browser",		browser,
	"Editor",		EDIT_main,
	"none",			NULL,
	NULL,
};

PrivateVarDef FORM	*VarsForm = NULL;

PrivateFnDef int getVarIndex(char const *name) {
	int	i = 0;

	if( name == NULL )
		return(-1);

	while( VARS_varName(&VarNames[i]) != NULL )
	{
		if( strcmp(name,VARS_varName(&VarNames[i])) == 0 )
			return(i);
		i++;
	}

	return(-1);
}

PrivateFnDef int setCurrChoice(FORM_Field *f) {
	MENU	*menu;
	char	*value;
	int	i;
	
	if( (menu = FORM_fieldMenu(f)) == NULL )
		return(TRUE);
	value = FORM_fieldValue(f);
	
	for( i=0 ; i<MENU_choiceCount(menu) ; i++ )
	{
		if( strcmp(value, MENU_choiceLabel(menu, i)) == 0 )
		{
			MENU_currChoice(menu) = i;
			return(FALSE);
		}
	}
	return(FALSE);
}

PrivateFnDef void initVarsForm() {
	VARS_Type	*vt;
	FORM_Field	*f;
	int		i, j, longest, idx;
	MENU		*menu;

	if( (menu = VARS_menu(&VarNames[STARTUPMODULE])) == NULL )
	{
	    MENU_makeMenu(
		menu, moduleChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j
	    );
	    MENU_title(menu) = " Modules:";
	    MENU_currChoice(menu) = NOMODULE;
	    VARS_menu(&VarNames[STARTUPMODULE]) = menu;
	}
	FORM_fieldMenu (
	    FORM_field(VarsForm, VariableValue+1+STARTUPMODULE)
	) = menu;

	if( (menu = VARS_menu(&VarNames[DEFAULTMODULE])) == NULL )
	{
		MENU_makeMenu(menu, moduleChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
		MENU_title(menu) = " Modules:";
		MENU_currChoice(menu) = EDITMODULE;
		VARS_menu(&VarNames[DEFAULTMODULE]) = menu;
	}
	FORM_fieldMenu(FORM_field(VarsForm, VariableValue+1+DEFAULTMODULE)) = menu;	
	
	for( i=0 ; i<=LASTVARIABLE ; i++ )
	{
		vt = &VarNames[i];
		f = FORM_field(VarsForm,VariableValue+1+i);
		switch(VARS_valueType(vt))
		{
			case VARS_intType:
				sprintf(FORM_fieldValue(f),"%d",VARS_int(vt));
				setCurrChoice(f);
				break;
			case VARS_stringType:
				strncpy(FORM_fieldValue(f),VARS_string(vt),FORM_ValueMaxLen);
				setCurrChoice(f);
				break;
			case VARS_dateType:
				sprintf(FORM_fieldValue(f),"%d",VARS_date(vt));
				setCurrChoice(f);
				break;
			case VARS_floatType:
				sprintf(FORM_fieldValue(f),"%f",VARS_float(vt));
				setCurrChoice(f);
				break;
			case VARS_funcType:
				idx = VARS_moduleNameFromFunc(VARS_func(vt));
				if( idx == -1 )
					strcpy(FORM_fieldValue(f),"none");
				else
					strcpy(FORM_fieldValue(f),VARS_menuName(&NameAndFunc[idx]));
				setCurrChoice(f);
				break;
			case VARS_boolType:
				if( VARS_boolean(vt) == TRUE )
				{
					strcpy(FORM_fieldValue(f),"TRUE");
					MENU_currChoice(FORM_fieldMenu(f)) = 1;
				}
				else
				{
					strcpy(FORM_fieldValue(f),"FALSE");
					MENU_currChoice(FORM_fieldMenu(f)) = 0;
				}
				break;
			case VARS_noType:
			default:
				break;
		}
	}
}

PrivateFnDef void processValue(VARS_Type *vt, char *buffer) {
	int		i, idx;
	
	switch(VARS_valueType(vt))
	{
		case VARS_intType:
			VARS_int(vt) = atoi(buffer);
			break;
		case VARS_stringType:
			strcpy(VARS_string(vt),buffer);
			break;
		case VARS_dateType:
			VARS_date(vt) = atoi(buffer);
			break;
		case VARS_floatType:
			VARS_float(vt) = atof(buffer);
			break;
		case VARS_funcType:
			idx = VARS_funcFromModuleName(buffer);
			if( idx != -1 )
				VARS_func(vt) = VARS_menuFunc(&NameAndFunc[idx]);
			else
				VARS_func(vt) = NULL;
			break;
		case VARS_boolType:
			if( strncmp("TRUE",buffer,4) == 0 )
				VARS_boolean(vt) = TRUE;
			else
				VARS_boolean(vt) = FALSE;
			break;
		case VARS_noType:
		default:
			break;
	}
}

PrivateFnDef int processLine(char *line) {
	int	len, i, j, idx;
	char	buffer[2*VARS_maxLen];
	
	len = strlen(line);
	if( (len > 0) && (line[len-1] == '\n') )
	{
		len--;
		line[len] = '\0';
	}

	j = 0;
	while (isspace(line[j]) && j < len) j++;
	i = 0;
	while (!isspace(line[j]) && line[j] != ',' && j < len)
		buffer[i++] = line[j++];
	if( line[j] == ',' )
    		j++;
	buffer[i] = '\0';
	if (0 == strlen(buffer))
		return(TRUE);

	if( (idx = getVarIndex(buffer)) == -1 )
		return(TRUE);

	while (isspace(line[j]) && j < len) j++;
	i = 0;
	while (line[j] != '|' && j < len)
		buffer[i++] = line[j++];
	buffer[i] = '\0';

	processValue(&VarNames[idx], buffer);
	
	if( line[j] == '|' )
    		j++;
	while (isspace(line[j]) && line[j] != '\0') j++;
	strcpy(buffer,&line[j]);
	if( (0 == (len = strlen(buffer))) ||
	    (strncmp(buffer,"NULL",4) == 0) ||
	    (strncmp(buffer,"none",4) == 0) ||
	    (strncmp(buffer,"NA",2) == 0) )
	{
		if( VARS_menuString(&VarNames[idx]) != NULL )
		  free((void*)VARS_menuString(&VarNames[idx]));
		VARS_menuString(&VarNames[idx]) = NULL;
		if( VARS_menu(&VarNames[idx]) != NULL )
			MENU_deleteMenu(VARS_menu(&VarNames[idx]), i);
		VARS_menu(&VarNames[idx]) = NULL;
	}
	else
	{
		if( VARS_menuString(&VarNames[idx]) != NULL )
			free((void*)VARS_menuString(&VarNames[idx]));
		VARS_menuString(&VarNames[idx]) = (char const*)calloc((len+1), sizeof(char));
		strcpy(VARS_menuString(&VarNames[idx]), buffer);
		if( VARS_menu(&VarNames[idx]) != NULL )
			MENU_deleteMenu(VARS_menu(&VarNames[idx]), i);
		VARS_menu(&VarNames[idx]) = MENU_getMenu(buffer);
	}
	return(FALSE);
}
	
PrivateFnDef int VARS_readFile(char const *fname) {
	FILE	*fp;
	char	buf[2*VARS_maxLen];
	
	if( (fp = fopen(fname, "r")) == NULL )
		return(TRUE);
	while( fgets(buf, 2*VARS_maxLen, fp) != NULL )
	{
		processLine(buf);
	}

	fclose(fp);
	return(FALSE);
}

PrivateFnDef int VARS_writeFile(char const *fname) {
	FILE		*fp;
	char		buf[2*VARS_maxLen], value[VARS_maxLen+1];
	char const	*menu;
	VARS_Type	*vt;
	int		i, idx;
	
	if( (fp = fopen(fname, "w")) == NULL )
		return(TRUE);
	for( i=0 ; i<=LASTVARIABLE ; i++ )
	{
		vt = &VarNames[i];
		char const *name = VARS_varName(vt);
		if( (menu = VARS_menuString(vt)) == NULL )
			menu = "none";
		buf[0] = '\0';
		switch(VARS_valueType(vt))
		{
			case VARS_intType:
				sprintf(buf,"%s %d| %s",name,VARS_int(vt),menu);
				break;
			case VARS_stringType:
				sprintf(buf,"%s %s| %s",name,VARS_string(vt),menu);
				break;
			case VARS_dateType:
				sprintf(buf,"%s %d| %s",name,VARS_date(vt),menu);
				break;
			case VARS_floatType:
				sprintf(buf,"%s %f| %s",name,VARS_float(vt),menu);
				break;
			case VARS_funcType:
				idx = VARS_moduleNameFromFunc(VARS_func(vt));
				if( idx == -1 )
					sprintf(buf,"%s none| %s",name,menu);
				else
					sprintf(buf,"%s %s| %s",name,VARS_menuName(&NameAndFunc[idx]),menu);
				break;
			case VARS_boolType:
				sprintf(buf,"%s %d| %s",
					name,
					((VARS_boolean(vt) == TRUE) ? "TRUE" : "FALSE"),
					menu);
				break;
			case VARS_noType:
			default:
				break;;
		}
		if( strlen(buf) > (size_t)0 )
		{
			strcat(buf,"\n");
			fputs(buf, fp);
		}
	}

	fclose(fp);
	return(FALSE);
}

PrivateVarDef char	startupfile[VARS_maxLen];

PrivateFnDef int getFileName(char const *pstr /* either "write to" or "read from" */) {
    char string[VARS_maxLen], prompt[VARS_maxLen];
    int	error;

    if (!strlen(startupfile))
    {
	sprintf(prompt, "No profile file, enter file to %s: ",pstr);
	if (error = ERR_promptForString(prompt, string, FALSE))
	        return(error);
	else
	{
	    if (strlen(string))
	        strcpy(startupfile, string);
	    else
		return(ERR_AbortedPrompt);
	}
    }
    else
    {
	sprintf(prompt,
	 "Enter filename or <CR> to %s %s: ", pstr, startupfile);
	if (error = ERR_promptForString(prompt, string, TRUE))
	        return(error);
	if (strlen(string))
	        strcpy(startupfile, string);
    }
    return(FALSE);
}

PublicFnDef int VARS_initProfileVariables() {
	MENU	*menu;
	char const *pf;
	int	i, j, longest;
	
	StartupKit[0] = '\0';
	
	if( (pf = getenv("VisionLibrary")) != NULL )
	{
		strcpy(startupfile,pf);
		strcat(startupfile,"/profile");
	}
	else
		startupfile[0] = '\0';
	if( startupfile[0] != '\0' )
		VARS_readFile(startupfile);
		
	pf = GOPT_GetValueOption ("ProfileFile");
	if( IsNil (pf) )
	{
		if( (pf = getenv("HOME")) != NULL )
		{
			strcpy(startupfile,pf);
			strcat(startupfile,"/.visionrc");
		}
		else
			startupfile[0] = '\0';
	}
	else
		strcpy(startupfile,pf);		
	if( startupfile[0] != '\0' )
		VARS_readFile(startupfile);
		
	FORM_makeForm(VarsForm, formFields, i);

	MENU_makeMenu(menu, VarChoices, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
	MENU_title(menu) = " Variables:";
	FORM_fieldMenu(FORM_field(VarsForm, VariableName)) = menu;

	initVarsForm();
		
	return(FALSE);

}

PublicFnDef int VARS_funcFromModuleName(char const *name) {
 	char	*nm;
	int	i = 0;

	if( name == NULL )
		return(-1);

	nm = eatLeadingAndTrailingSpaces(name);
	
	while( VARS_menuName(&NameAndFunc[i]) != NULL )
	{
		if( strcmp(nm,VARS_menuName(&NameAndFunc[i])) == 0 )
			return(i);
		i++;
	}

	return(-1);
}

PublicFnDef int VARS_moduleNameFromFunc(void (*funcAddr)()) {
	int	i = 0;

	if( funcAddr == NULL )
		return(-1);

	while( VARS_menuName(&NameAndFunc[i]) != NULL )
	{
		if( funcAddr == VARS_menuFunc(&NameAndFunc[i]) )
			return(i);
		i++;
	}

	return(-1);
}

PrivateVarDef PAGE	*vpage;

PrivateFnDef void varsExec() {
	MENU		*nmenu;
	int		i;
	VARS_Type	*vt;
	
	nmenu = FORM_fieldMenu(FORM_field(VarsForm, VariableName));
	for( i=0 ; i<MENU_choiceCount(nmenu) ; i++ )
	{
		vt = &VarNames[i];
		processValue(vt, FORM_fieldValue(FORM_field(VarsForm,VariableValue+1+i)));
	}
	PAGE_status(vpage) = PAGE_ExitOnExec;
}

PrivateVarDef void doWrite() {
	if( getFileName("write to") )
		return;
	if( ((access(startupfile, 00) != -1) && (access(startupfile, 02) == -1)) || 
	    VARS_writeFile(startupfile) )
	{
		ERR_displayPause(" Unable To Write To File");
		return;
	}
}

PrivateVarDef void doRead() {
	if( getFileName("read from") )
		return;
	if( (access(startupfile, 04) == -1) || VARS_readFile(startupfile) )
	{
		ERR_displayPause(" Unable To Read From File");
		return;
	}
	initVarsForm();
	FORM_handler(VarsForm,PAGE_window(vpage,PAGE_windowCount(vpage)-1),PAGE_Init);
}

PrivateVarDef MENU_Choice fileReportMenu[] = {
" Read ",  " Read Profile Information From A File",	 'r', doRead,  ON, 
" Write ", " Write Profile Information To A File",	 'w', doWrite, ON, 
NULL, 
};

PrivateFnDef void fileMenu() {
    int		i, j, longest, sr, sc;
    MENU	*menu;
    CUR_WINDOW	*menuWin;

    MENU_makeMenu(menu, fileReportMenu, CUR_A_NORMAL, CUR_A_REVERSE, longest, i, j);
    MENU_title(menu) =" Interface Menu: ";
    i = MENU_choiceCount(menu) + 4;
    if( i >= CUR_LINES )
	i = CUR_LINES - 1;
    j = longest + 4;
    if( (size_t)j <= strlen(MENU_title(menu)) )
	j = strlen(MENU_title(menu)) + 1;
    sr = ((CUR_LINES - 1) - i) / 2;
    sc = CUR_COLS - 10 - j;
    if( sc < 0 )
    {
	sc = 5;
	j = CUR_COLS - 10;
    }
    menuWin = CUR_newwin(i, j, sr, sc);
    MENU_handler(menu,menuWin,PAGE_Input);
    MENU_deleteMenu(menu, i);
    CUR_delwin(menuWin);
}

PublicVarDef int	inVars = FALSE;

PublicFnDef int VARS_runProfile(PAGE *page) {
	int		i;
	CUR_WINDOW	*win1, *win2;

	if( inVars )
	{
		ERR_displayPause("Profile Variable Editor already running");
		return(TRUE);
	}
	inVars = TRUE;
		
	win1 = CUR_newwin(7, 44, CUR_LINES-4-7, 4);
	WIN_ReverseBox(win1," Profile Variable Editor ");
	win2 = CUR_subwin(win1, 5, 42, CUR_LINES-4-7+1, 5);
	PAGE_createPage(vpage, (PAGE_windowCount(page)+2), NULL, NULL, NULL, PAGE_noType, i);
	for( i=0 ; i<PAGE_windowCount(page) ; i++ )
	{
		PAGE_createElement(vpage, i, NULL, PAGE_window(page, i), PAGE_Init, NULL, FALSE);
	}
	PAGE_createElement(vpage, i, NULL, win1, PAGE_Init, NULL, FALSE);
	PAGE_createElement(vpage, (i+1), VarsForm, win2, PAGE_Input, FORM_handler, TRUE);

	PAGE_fkey(vpage, 1) = varsExec;
	PAGE_fkey(vpage, 4) = fileMenu;

	PAGE_handler(vpage);

	CUR_delwin(win2);
	CUR_delwin(win1);
	PAGE_deletePage(vpage, i);
	initVarsForm();
	inVars = FALSE;
	return(FALSE);
}

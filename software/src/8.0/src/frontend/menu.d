/****************************************************************************
*****									*****
*****			menu.d						*****
*****									*****
****************************************************************************/
#ifndef MENU_D
#define MENU_D

#define MENU_Choices 50
#define MENU_Quote   042
#define MENU_MaxLine 200

#define MENU_Normal	0
#define MENU_ExitOnExec	1

#define	MENU_StaticMenu	 0x00
#define	MENU_BackendMenu 0x01

typedef struct {
    char const *label;
    char const *help;
    char letter;	 
    void (*handler)(void);
    char active;
} MENU_Choice;

typedef struct {
    char const *title;
    int choiceCount;	   
    int currChoice;	  
    int normalAttr;		  
    int hilightAttr;
    int status;
    int flags;
    MENU_Choice **choice;
} MENU;

#define MENU_choiceArray(menu)		(menu->choice)
#define MENU_choice(menu, n)		((menu->choice)[n])
#define MENU_choiceLabel(menu, n)	((menu->choice)[n]->label)
#define MENU_choiceHelp(menu, n)	((menu->choice)[n]->help)
#define MENU_choiceLetter(menu, n)	((menu->choice)[n]->letter)
#define MENU_choiceHandler(menu, n)	((menu->choice)[n]->handler)
#define MENU_choiceActive(menu, n)	((menu->choice)[n]->active)
#define MENU_title(menu)		(menu->title)
#define MENU_normal(menu)	        (menu->normalAttr)
#define MENU_hilight(menu)	        (menu->hilightAttr)
#define MENU_choiceCount(menu)		(menu->choiceCount)
#define MENU_currChoice(menu)		(menu->currChoice)
#define MENU_status(menu)		(menu->status)
#define MENU_flags(menu)		(menu->flags)
#define MENU_IsBackendMenu(menu)	(menu->flags & MENU_BackendMenu)
#define MENU_SetBackendMenu(menu)	(menu->flags |= MENU_BackendMenu)

#define MENU_makeMenu(mptr, choices, norm, high, longest, i, j)\
{\
    mptr = (MENU *)malloc(sizeof(MENU));\
    MENU_title(mptr) = (char *)NULL;\
    MENU_currChoice(mptr) = (int)0;\
    MENU_normal(mptr) = (int)norm;\
    MENU_hilight(mptr) = (int)high;\
    i = 0;\
    while (choices[i].label != NULL) i++;\
    MENU_choiceArray(mptr) = (MENU_Choice **)calloc(i, sizeof(MENU_Choice *));\
    longest = i = 0;\
    while (choices[i].label != NULL)\
    {\
        MENU_choice(mptr, i) = (MENU_Choice *)&choices[i];\
	if ((j = strlen(choices[i].label)) > longest)\
	    longest = j;\
	i++;\
    }\
    MENU_status(mptr) = MENU_Normal;\
    MENU_flags(mptr) = MENU_StaticMenu;\
    MENU_choiceCount(mptr) = (int)i;\
}

#define MENU_deleteMenu(mptr, i)\
{\
    if (mptr != NULL)\
    {\
        if( MENU_IsBackendMenu(mptr) )\
        {\
	    for (i = 0; i < MENU_choiceCount(mptr); i++)\
	    {\
		if (MENU_choiceLabel(mptr, i) != NULL)\
		    free((void*)MENU_choiceLabel(mptr, i));\
		if (MENU_choiceHelp(mptr, i) != NULL)\
		    free((void*)MENU_choiceHelp(mptr, i));\
	    }\
	}\
	if (MENU_choiceArray(mptr) != NULL)\
	    free(MENU_choiceArray(mptr));\
	free(mptr);\
    }\
}

#endif

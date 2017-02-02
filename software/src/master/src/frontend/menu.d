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


#endif

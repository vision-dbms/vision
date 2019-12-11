/****************************************************************************
*****									*****
*****			form.d						*****
*****									*****
****************************************************************************/
#ifndef FORM_D
#define FORM_D

#include "V_VString.h"

#include "menu.h"
#include "choices.d"

#define FORM_Fields  100
#define FORM_Quote   042
#define FORM_MaxLine 200
#define FORM_ValueMaxLen 80

#define	FORM_InputFlag	0x0001
#define FORM_ScrollFlag	0x0002
#define FORM_ExtKeyFlag	0x0004

typedef struct {
    int y;		    /** line on screen relative to form->y   **/
    int x;		    /** column on screen relative to form->x **/
    int attr;		    /** video attribute, see curses.h        **/
    int len;		    /** field length			     **/
    unsigned int flags;     /** flags for input, scrolling, etc.     **/
    char type;		    /** undefined (probably 'a', 'n', 'm')   **/
    char value[FORM_ValueMaxLen+1]; /** default field value 	     **/
    V::VString help;
    MENU::Reference menu;   /** pointer to menu, if type == 'm'	     **/
    CHOICE_MenuChoice **choiceArray;
				    /** pointer to an array of pointers
					to a structure that decides
					which menu choices are active.
			       *****/
} FORM_Field;

typedef struct {
    int fields;		  
    int currField;
    FORM_Field *field[FORM_Fields];
} FORM;

#define FORM_fieldCount(form)	(form->fields)
#define FORM_currField(form)	(form->currField)
#define FORM_field(form, n)     (form->field[n])

#define FORM_fieldY(field)		(field->y)
#define FORM_fieldX(field)		(field->x)
#define FORM_fieldAttr(field)		(field->attr)
#define FORM_fieldLen(field)		(field->len)
#define FORM_fieldInput(field)		((((field->flags) & FORM_InputFlag) == FORM_InputFlag) ? TRUE : FALSE)
#define FORM_fieldSetInput(field)	((field->flags) |= FORM_InputFlag)
#define FORM_fieldClearInput(field)	((field->flags) &= ~FORM_InputFlag)
#define FORM_fieldScroll(field)		((((field->flags) & FORM_ScrollFlag) == FORM_ScrollFlag) ? TRUE : FALSE)
#define FORM_fieldSetScroll(field)	((field->flags) |= FORM_ScrollFlag)
#define FORM_fieldClearScroll(field)	((field->flags) &= ~FORM_ScrollFlag)
#define FORM_fieldExtKey(field)		((((field->flags) & FORM_ExtKeyFlag) == FORM_ExtKeyFlag) ? TRUE : FALSE)
#define FORM_fieldSetExtKey(field)	((field->flags) |= FORM_ExtKeyFlag)
#define FORM_fieldClearExtKey(field)	((field->flags) &= ~FORM_ExtKeyFlag)
#define FORM_fieldType(field)		(field->type)
#define FORM_fieldValue(field)		(field->value)
#define FORM_fieldHelp(field)		(field->help)
#define FORM_fieldMenu(field)		(field->menu)
#define FORM_fieldChoiceArray(field)	(field->choiceArray)

#define FORM_makeForm(fptr, fields, i)\
{\
    fptr = (FORM *)malloc(sizeof(FORM));\
    i = 0;\
    while (fields[i].y != -1) {\
        FORM_field(fptr, i) = (FORM_Field *)&fields[i];\
	i++;\
    }\
    FORM_fieldCount(fptr) = i;\
}

#endif

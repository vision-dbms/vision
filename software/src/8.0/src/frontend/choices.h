/*****  Choice Header File  *****/
#ifndef CHOICES_H
#define CHOICES_H

/*****  Supporting Definitions  *****/
#include "form.d"

/*****  Shared Definitions  *****/
#include "choices.d"

/*****  Function Declarations  *****/

PublicFnDecl void CHOICES_LimitOptions (
    CUR_WINDOW *win, FORM *form, CHOICE_MenuChoice *dependencyL, int *Pos
);

#endif

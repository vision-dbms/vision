/****************************************************************************
*****									*****
*****			misc.h						*****
*****									*****
****************************************************************************/

#ifndef MISC_H
#define MISC_H

#include "Vk.h"

PublicFnDecl int isBlank(char *str);
PublicFnDecl int toUpper (int ch);
PublicFnDecl int toLower (int ch);
PublicFnDecl void strToUpper(char *str);
PublicFnDecl void strToLower(char *str);
PublicFnDecl char *eatLeadingAndTrailingSpaces(char *str);

#endif

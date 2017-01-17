/****************************************************************************
*****									*****
*****			misc.h						*****
*****									*****
****************************************************************************/

#ifndef MISC_H
#define MISC_H

#include "Vk.h"
#include "V_VString.h"

PublicFnDecl bool isBlank(char *str);
PublicFnDecl int toUpper (int ch);
PublicFnDecl int toLower (int ch);
PublicFnDecl void strToUpper(char *str);
PublicFnDecl void strToLower(char *str);
PublicFnDecl VString eatLeadingAndTrailingSpaces(char const *str);

#endif

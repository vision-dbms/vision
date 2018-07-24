#include "Vk.h"

#include "misc.h"

PublicFnDef bool isBlank(char *str) {
    int i, len;

    len = strlen(str);
    for (i = 0; i < len; i++)
        if (str[i] != ' ' && str[i] != '\n') 
	    return false;
    return true;
}

PublicFnDef int toUpper (int ch) {
    return islower (ch) ? toupper (ch) : ch;
}

PublicFnDef int toLower (int ch) {
    return isupper (ch) ? tolower (ch) : ch;
}
    
PublicFnDef void strToUpper(char *str) {
    while (*str = toUpper (*str))
	str++;
}

PublicFnDef void strToLower(char *str) {
    while (*str = toLower (*str))
	str++;
}

PublicFnDef V::VString eatLeadingAndTrailingSpaces(char const *str) {
    V::VString result;

//  If there's no input, or the input is empty, return an empty string, ...
    size_t len = str ? strlen (str) : static_cast<size_t>(0);
    if ( 0 == len)
	return result;

    size_t xExtent = len;
    while( xExtent > 0 && isspace(str[xExtent-1]) )
	xExtent--;

    size_t xOrigin = 0;
    while( xOrigin < xExtent && isspace(str[xOrigin]) )
	xOrigin++;

    if (xExtent > xOrigin)
	result.setTo (str + xOrigin, xExtent - xOrigin);

    return result;
}

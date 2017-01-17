#include "Vk.h"

#include "misc.h"

PublicFnDef int isBlank(char *str) {
    int i, len;

    len = strlen(str);
    for (i = 0; i < len; i++)
        if (str[i] != ' ' && str[i] != '\n') 
	    return FALSE;
    return TRUE;
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

PublicFnDef char const *eatLeadingAndTrailingSpaces(char const *str) {
	char	*ptr;
	int	len, i;

	if( str == NULL )
		return(NULL);
	if( (len = strlen(str)) == 0 )
		return(str);
	i = len - 1;
	while( (i >= 0) && isspace(str[i]) )
		i--;
	if( i < 0 )
	{
		str[0] = '\0';
		return( str );
	}
	str[i+1] = '\0';
	ptr = str;
	while( (*ptr != '\0') && isspace(*ptr) )
		ptr++;
	return(ptr);
}

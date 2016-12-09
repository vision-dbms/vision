#include "Vk.h"

PublicFnDef int isBlank(str)
char *str;
{
    int i, len;

    len = strlen(str);
    for (i = 0; i < len; i++)
        if (str[i] != ' ' && str[i] != '\n') 
	    return FALSE;
    return TRUE;
}

PublicFnDef toUpper (ch)
int ch;
{
    return islower (ch) ? toupper (ch) : ch;
}

PublicFnDef toLower (ch)
int ch;
{
    return isupper (ch) ? tolower (ch) : ch;
}
    
PublicFnDef strToUpper(str)
char *str;
{
    while (*str = toUpper (*str))
	str++;
}

PublicFnDef strToLower(str)
char *str;
{
    while (*str = toLower (*str))
	str++;
}

PublicFnDef char *
eatLeadingAndTrailingSpaces(str)
char	*str;
{
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

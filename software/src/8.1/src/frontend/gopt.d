/*****  Global Option Shared Declaration File  *****/
#ifndef GOPT_D
#define GOPT_D

/*****  Global Option Types  *****/
#define GOPT_SwitchOption   0
#define GOPT_ValueOption    1

/*****  Global Option Description Type Definition  *****/
typedef struct {
    char
	*name,
	*envVar,
	*stringValue;
    char
	argSpecLetter;
    unsigned int
	optionType	: 1,
	switchValue	: 1,
	valueSet	: 1;
} GOPT_OptionDescription;

/*****  Option Description Array Helper Macros  *****/
#define GOPT_BeginOptionDescriptions \
PublicVarDef GOPT_OptionDescription GOPT_OptionDescriptions[] = {

#define GOPT_SwitchOptionDescription(optionName, argLetter, envVar)\
    {optionName, (char *)envVar, NilOf (char *), argLetter,\
     GOPT_SwitchOption, 0, 0},

#define GOPT_ValueOptionDescription(optionName, argLetter, envVar, value)\
    {optionName, (char *)envVar, (char *)value, argLetter,\
     GOPT_ValueOption, 0, 0},

#define GOPT_EndOptionDescriptions\
    {NilOf (char *)} }

/*---------------------------------------------------------------------------
 * Example:
 *---------------------------------------------------------------------------
 */


#endif

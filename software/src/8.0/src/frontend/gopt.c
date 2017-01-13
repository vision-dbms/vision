/*****  Global Option Access Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName GOPT


/*******************************************
 *****  Header and Declarations Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Shared  *****/
#include "gopt.h"


/******************************************
 *****  Required Global Declarations  *****
 ******************************************/

/*****  'getopt' Variables  *****/
PublicVarDecl char *optarg;
PublicVarDecl int optind, opterr;

/*****  Global Option Descriptions  *****/
/*---------------------------------------------------------------------------
 * GOPT assumes that a global variable named 'GOPT_OptionsDescriptions' has
 * been created using the utilities defined in "gopt.d".  The following
 * declaration accesses that variable.
 *---------------------------------------------------------------------------
 */
PublicVarDecl GOPT_OptionDescription
    GOPT_OptionDescriptions[];

/*****  Extra Argument Scanning Variables  *****/
PrivateVarDef int
    LocalArgc = 0,
    ExtraArgOrigin = 0,
    ExtraArgPointer = 0;

PrivateVarDef char
    **LocalArgv = NilOf (char **);


/***********************************************
 *****  Global Option Acquisition Routine  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to acquire the current values of the options defined by the
 *****  variable 'GOPT_OptionDescriptions'
 *
 *  Arguments:
 *	argc			- the value of 'argc' passed to the main
 *				  program of this system.
 *	argv			- the value of 'argv' passed to the main
 *				  program of this system.
 *
 *  Returns:
 *	Nothing.
 *
 *  Note:
 *	The format of 'argc' and 'argv' should be compatible with 'getopt'.
 *
 *****/
PublicFnDef void GOPT_AcquireOptions (int argc, char *argv[]) {
    GOPT_OptionDescription *opt, *optionsIndexedByChar[256];
    char optString[513], *p;
    int i, optChar;

/*****  Initialize Global Variables  *****/
    LocalArgc = argc;
    LocalArgv = argv;
#if FALSE
    opterr = FALSE;
    optind = 0;
#endif

/*****  Initialize a directory of options indexed by argument letter  *****/
    for (i = 0; i < 256; i++)
        optionsIndexedByChar[i] = NilOf (GOPT_OptionDescription *);

    for (opt = GOPT_OptionDescriptions; opt->name; opt++)
        optionsIndexedByChar[opt->argSpecLetter] = opt;

/*****  Construct the 'getopt' 'optstring'  *****/
    for (i = 0, p = optString; i < 256; i++)
	if (opt = optionsIndexedByChar[i])
	{
	    *p++ = i;
	    switch (opt->optionType)
	    {
	    case GOPT_ValueOption:
	        *p++ = ':';
	        break;
	    default:
	        break;
	    }
	}
    *p = '\0';

/*****  Extract those options specified by 'argc' and 'argv'  *****/
    while ((optChar = getopt (argc, argv, optString)) != EOF)
    {
	if (IsNil (opt = optionsIndexedByChar[optChar]))
	    continue;
	switch (opt->optionType)
	{
	case GOPT_SwitchOption:
	    opt->switchValue = opt->valueSet = TRUE;
	    
	    break;
	case GOPT_ValueOption:
	    opt->stringValue = optarg;
	    opt->valueSet = TRUE;
	    
	    break;
	default:
	    break;
	}
    }
/*****  Record the 'optind' for use by the 'ExtraArg' routines  *****/
    ExtraArgOrigin = optind;

/*****  Pick up any uninitialized arguments from the environment  *****/
    for (opt = GOPT_OptionDescriptions; opt->name; opt++)
    {
        if (!opt->valueSet &&
	    !IsNil (opt->envVar) &&
	    !IsNil (p = getenv (opt->envVar)))
	    switch (opt->optionType)
	    {
	    case GOPT_SwitchOption:
	        opt->switchValue = opt->valueSet = TRUE;
	        break;
	    case GOPT_ValueOption:
	        opt->stringValue = p;
		opt->valueSet = TRUE;
	        break;
	    default:
	        break;
	    }
    }
}


/****************************************
 *****  Option Value Query Routine  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to obtain the value of a specified 'value' option.
 *
 *  Argument:
 *	optionName		- the name of the option (as defined in
 *				  GOPT_OptionDescriptions) of the option
 *				  desired.
 *
 *  Returns:
 *	The address of the character string value of the option (look but
 *	do NOT touch) or NULL/Nil.
 *
 *****/
PublicFnDef char const *GOPT_GetValueOption (char const *optionName) {
    GOPT_OptionDescription *opt;

    for (opt = GOPT_OptionDescriptions; opt->name; opt++)
        if (strcmp (optionName, opt->name) == 0)
	    return opt->stringValue;
    
    return NilOf (char *);
}

/*---------------------------------------------------------------------------
 *****  Routine to obtain the value of a specified 'switch' option.
 *
 *  Argument:
 *	optionName		- the name of the option (as defined in
 *				  GOPT_OptionDescriptions) of the option
 *				  desired.
 *
 *  Returns:
 *	0 (zero) if the switch is cleared, 1 (one) if the switch is set, and
 *	-1 if the option isn't declared.
 *
 *****/
PublicFnDef int GOPT_GetSwitchOption (char const *optionName) {
    GOPT_OptionDescription *opt;

    for (opt = GOPT_OptionDescriptions; opt->name; opt++)
        if (strcmp (optionName, opt->name) == 0)
	    return opt->switchValue;
    return -1;
}


/*********************************************
 *****  Extra Argument Access Utilities  *****
 *********************************************/

/*---------------------------------------------------------------------------
 * The following routines can be used to sequentially access any additional
 * arguments passed to the main program of this system not processed by
 * 'getopt'.  These routines should not be called before 'GOPT_AcquireOptions'.
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 *****  Routine to position the 'extra argument' pointer to a particular
 *****  argument.
 *
 *  Arguments:
 *	offset			- the offset relative to the position selected
 *				  by 'whence' at which to position the extra
 *				  argument pointer.
 *	whence			- a selector for a base from which to interpret
 *				  'offset'.  The interpretation is similar to
 *				  'lseek's:
 *					whence == 0 --> go to the 'offset'-th
 *							argument relative to
 *							the first extra
 *							argument.
 *					whence == 1 --> go to the 'offset'-th
 *							argument relative to
 *							the current argument.
 *					whence == 2 --> go to the 'offset'-th
 *							argument relative to
 *							the last extra
 *							argument.
 *					whence == 3 --> go to the 'offset'-th
 *							absolute argument.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void GOPT_SeekExtraArgument (
    int				offset,
    int				whence
)
{
    switch (whence)
    {
    case 0:
        ExtraArgPointer = ExtraArgOrigin + offset;
        break;
    case 1:
        ExtraArgPointer = ExtraArgPointer + offset;
        break;
    case 2:
        ExtraArgPointer = LocalArgc + offset;
        break;
    case 3:
        ExtraArgPointer = offset;
        break;
    default:
        break;
    }
}

/****  Routine to return the current extra argument as defined by the last
 *****  call to 'GOPT_SeekExtraArgument' or 'GOPT_GetExtraArgument'.
 *
 *  Returns:
 *	A pointer to the current extra argument or NULL/Nil if there are no
 *	more extra arguments.  The extra argument pointer will be advanced
 *	to point to the next extra argument.
 *
 *****/
PublicFnDef char const *GOPT_GetExtraArgument () {
    return
	ExtraArgPointer >= 0 && ExtraArgPointer < LocalArgc
	? LocalArgv[ExtraArgPointer++]
	: NilOf (char *);
}

/*---------------------------------------------------------------------------
 *****  Routine to return the total number of extra arguments.
 *
 *  Returns:
 *	The number of extra arguments.
 *
 *****/
PublicFnDef int GOPT_GetExtraArgCnt () {
    return
	(LocalArgc - ExtraArgOrigin); 
}


/*****************************************
 *****  Program Name Access Routine  *****
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the name with which this program was invoked.
 *
 *  Returns:
 *	'argv[0]'
 *
 *****/
PublicFnDef char const *GOPT_ProgramName () {
    return LocalArgv[0];
}

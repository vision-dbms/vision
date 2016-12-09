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


/********************************
 ********************************
 *****  Option Acquisition  *****
 ********************************
 ********************************/

#if defined(_WIN32)

/************************
 *****  Windows/NT  *****
 ************************/

static char*	optarg = 0;
static int	optind = 1;
static int	opterr = 0;

/*--------------------------------------------------------------------------
 *  Emulation of HP-UX getopt().  see GETOPT(3C).
 *****/
PublicFnDef V_API int getopt (int argc, char **argv, char *optstring) {
    static int
	currArg = 1, 	/* duplicates "optind" */
	currCh  = 1;	/* skip '-', assuming it exists */
    int ch;
    char *optChPtr;

    optarg = "";

/*****  get next char from argv option arg (if any exist)  *****/
    if (currArg != argc && *argv [currArg] == '-' &&  argv [currArg] [currCh] != '\0') {
	ch = argv [currArg] [currCh++];

/*****  check special case option '--' (by itself) as opt arg terminator  *****/
	if (currCh == 2 && ch == '-' && argv [currArg] [currCh] == '\0')
	    return EOF;

	if (argv [currArg] [currCh] == '\0') {
	    optind++;  currArg++;  currCh = 1;
	}
    } else return EOF;

/*****  check against option string  *****/
    if ((optChPtr = strchr (optstring, ch)) == NULL) {	/* illegal option...  	      */
	if (opterr != 0)
	    fprintf (stderr, "%s: illegal option -- %c\n", argv[0], ch);
	return '?';
    }
    else {					/* option is legal	      */
	if (*(optChPtr + 1) == ':') { 		/* option requires argument?  */
	    if (currCh == 1) {  		/* white space btwn opt & arg */
		if (currArg == argc) {		/* ...or missing argument?    */
		    if (opterr != 0) fprintf (
			stderr, "%s: option requires an argument -- %c\n", argv[0], ch
		    );
		    return '?';
		}
		optarg = argv[currArg];
	    }
	    else	/* no such white space;  option arg is present        */
		optarg = argv[currArg] + currCh;
	    optind = currArg += 1;
	    currCh = 1;
	}
	return ch;
    }
}
#endif


/******************************************
 *****  Required Global Declarations  *****
 ******************************************/

/*****  Global Option Descriptions  *****/
/*---------------------------------------------------------------------------
 * GOPT assumes that 'GOPT_OptionsDescriptions' has been set via the
 * the construction of the GOPT object created using the utilities in "gopt.h".
 *---------------------------------------------------------------------------
 */
typedef V::GOPT::OptionDescription GOPT_OptionDescription;

PrivateVarDef GOPT_OptionDescription *OptionDescriptions = 0;

V::GOPT::GOPT (OptionDescription *pOptionDescriptions) {
    OptionDescriptions = pOptionDescriptions;
}

/*****  Extra Argument Scanning Variables  *****/
PrivateVarDef int
    LocalArgc = 0,
    ExtraArgOrigin = 0,
    ExtraArgPointer = 0;

PrivateVarDef argv_t LocalArgv = NilOf (argv_t);


/***********************************************
 *****  Global Option Acquisition Routine  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to acquire the current values of the options defined by the
 *****  variable 'OptionDescriptions'
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
PublicFnDef V_API void GOPT_AcquireOptions (int argc, argv_t argv) {
    GOPT_OptionDescription *opt, *optionsIndexedByChar[256];
    int i, optChar;

/*****  Initialize Global Variables  *****/
    LocalArgc = argc;
    LocalArgv = argv;

/*****  Initialize a directory of options indexed by argument letter  *****/
    for (i = 0; i < 256; i++)
        optionsIndexedByChar[i] = NilOf (GOPT_OptionDescription *);

    for (opt = OptionDescriptions; opt->name; opt++)
        optionsIndexedByChar[opt->argSpecLetter] = opt;

/*****  Construct the 'getopt' 'optstring'  *****/
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
    char optBuffer[513], *p;
    char *const optString = static_cast<char*>(optBuffer);
#ifdef __VMS
#pragma __pointer_size __restore
#endif
    for (i = 0, p = optString; i < 256; i++) {
	if (opt = optionsIndexedByChar[i]) {
	    *p++ = (char)i;
	    switch (opt->optionType) {
	    case GOPT_ValueOption:
	        *p++ = ':';
	        break;
	    default:
	        break;
	    }
	}
    }
    *p = '\0';

/*****  Extract those options specified by 'argc' and 'argv'  *****/
    while ((optChar = getopt (argc, argv, optString)) != EOF) {
	if (IsNil (opt = optionsIndexedByChar[optChar]))
	    continue;
	switch (opt->optionType) {
	case GOPT_SwitchOption:
	    opt->switchValue = opt->valueSet = true;
	    
	    break;
	case GOPT_ValueOption:
	    opt->stringValue = optarg;
	    opt->valueSet = true;
	    
	    break;
	default:
	    break;
	}
    }
/*****  Record the 'optind' for use by the 'ExtraArg' routines  *****/
    ExtraArgOrigin = optind;

/*****  Pick up any uninitialized arguments from the environment  *****/
    for (opt = OptionDescriptions; opt->name; opt++) {
        if (!opt->valueSet &&
	    !IsNil (opt->envVar) &&
	    !IsNil (p = getenv (opt->envVar)))
	    switch (opt->optionType) {
	    case GOPT_SwitchOption:
	        opt->switchValue = opt->valueSet = true;
	        break;
	    case GOPT_ValueOption:
	        opt->stringValue = p;
		opt->valueSet = true;
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
 *				  OptionDescriptions) of the option
 *				  desired.
 *
 *  Returns:
 *	The address of the character string value of the option (look but
 *	do NOT touch) or NULL/Nil.
 *
 *****/
PublicFnDef V_API char const *GOPT_GetValueOption (char const * optionName) {
    GOPT_OptionDescription *opt;

    for (opt = OptionDescriptions; opt->name; opt++)
        if (strcmp (optionName, opt->name) == 0)
	    return opt->stringValue;
    
    return NilOf (char const *);
}

/*---------------------------------------------------------------------------
 *****  Routine to obtain the value of a specified 'switch' option.
 *
 *  Argument:
 *	optionName		- the name of the option (as defined in
 *				  OptionDescriptions) of the option
 *				  desired.
 *
 *  Returns:
 *	0 (zero) if the switch is cleared, 1 (one) if the switch is set, and
 *	-1 if the option isn't declared.
 *
 *****/
PublicFnDef V_API int GOPT_GetSwitchOption (char const *optionName) {
    GOPT_OptionDescription *opt;

    for (opt = OptionDescriptions; opt->name; opt++)
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
PublicFnDef V_API void GOPT_SeekExtraArgument (
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
PublicFnDef V_API char const *GOPT_GetExtraArgument () {
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
PublicFnDef V_API int GOPT_GetExtraArgCnt () {
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
PublicFnDef V_API char const* GOPT_ProgramName () {
    return LocalArgv[0];
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  gopt.c 1 replace /users/mjc/system
  860226 16:48:33 mjc create

 ************************************************************************/
/************************************************************************
  gopt.c 2 replace /users/mjc/translation
  870524 09:38:46 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  gopt.c 3 replace /users/mjc/system
  870614 20:00:14 mjc Change declarations of 'optarg', 'optind' and 'opterr' to be VAX compatible

 ************************************************************************/
/************************************************************************
  gopt.c 4 replace /users/m2/backend
  890503 15:20:19 m2 Lint fix

 ************************************************************************/

/*****  Global Option Access Facility Exported Declarations  *****/
#ifndef GOPT_Interface
#define GOPT_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VPointer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API GOPT : virtual public VTransient {
	DECLARE_FAMILY_MEMBERS (GOPT, VTransient);

    //  Option Description
    public:
	struct OptionDescription {
	    char const
		*name,
		*envVar,
		*stringValue;
	    char
		argSpecLetter;
	    unsigned int
		optionType	: 1,
		switchValue	: 1,
		valueSet	: 1;
	};

    //  Construction
    public:
	GOPT (OptionDescription *pDescriptionArray);

    //  Destruction
    public:
	~GOPT () {
	}
    };
}

/*****  Global Option Types  *****/
#define GOPT_SwitchOption   0
#define GOPT_ValueOption    1

/*****  Option Description Array Helper Macros  *****/
#define GOPT_BeginOptionDescriptions \
PrivateVarDef V::GOPT::OptionDescription GOPT_OptionDescriptions[] = {

#define GOPT_SwitchOptionDescription(optionName, argLetter, envVar)\
    {optionName, (char *)envVar, NilOf (char *), argLetter,\
     GOPT_SwitchOption, 0, 0},

#define GOPT_ValueOptionDescription(optionName, argLetter, envVar, value)\
    {optionName, (char *)envVar, (char *)value, argLetter,\
     GOPT_ValueOption, 0, 0},

#define GOPT_EndOptionDescriptions\
    {NilOf (char *)} }; PrivateVarDef V::GOPT g_iGOPT (GOPT_OptionDescriptions)

/*****  Function Declarations  *****/
#if defined(_WIN32)
PublicFnDecl V_API int getopt (int argc, char **argv, char *optstring);
#endif

PublicFnDecl V_API void GOPT_AcquireOptions (int argc, argv_t argv);

PublicFnDecl V_API char const *GOPT_GetValueOption (char const *optionName);

PublicFnDecl V_API int GOPT_GetSwitchOption (char const *optionName);

PublicFnDecl V_API void GOPT_SeekExtraArgument (int offset, int whence);

PublicFnDecl V_API int GOPT_GetExtraArgCnt ();

PublicFnDecl V_API char const*GOPT_GetExtraArgument ();

PublicFnDecl V_API char const*GOPT_ProgramName ();


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  gopt.h 1 replace /users/mjc/system
  860226 16:48:38 mjc create

 ************************************************************************/
/************************************************************************
  gopt.h 2 replace /users/mjc/translation
  870524 09:38:54 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

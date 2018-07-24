#ifndef V_Interface_F7CD48E4_3E49_4b5a_841C_E311DCEEA214
#define V_Interface_F7CD48E4_3E49_4b5a_841C_E311DCEEA214

/*************************
 *****  Compilation  *****
 *************************/

#ifdef V_EXPORTS
#define V_API DECLSPEC_DLLEXPORT

#else
#define V_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "v.lib")
#endif

#endif

/*********************
 *****  Support  *****
 *********************/

/***********************
 *****  Namespace  *****
 ***********************/

namespace V {
/**********************************
 *----  Forward Declarations  ----*
 **********************************/

    class VString;

/************************
 *----  Wire Types  ----*
 ************************/

    typedef float		F32;
    typedef double		F64;

    typedef char		S08;
    typedef short		S16;
    typedef int			S32;

    typedef unsigned char	U08;
    typedef unsigned short	U16;
    typedef unsigned int	U32;

    typedef __int64		S64;
    typedef unsigned __int64	U64;

//-----------
//  Environment Access
//-----------

    V_API bool		GetEnvironmentBoolean (char const *pName, bool		bDefaultValue = false);
    V_API int		GetEnvironmentInteger (char const* pName, int		iDefaultValue);
    V_API char const*	GetEnvironmentString  (VString &rResult, char const* pName, char const*	pDefaultValue);
    V_API unsigned int	GetEnvironmentUnsigned(char const *pName, unsigned int	iDefaultValue);

    V_API bool		GetEnvironmentValue (char const* pName, bool		iDefaultValue);
    V_API int		GetEnvironmentValue (char const* pName, int		iDefaultValue);
    V_API double	GetEnvironmentValue (char const* pName, double		iDefaultValue);
    V_API char const*	GetEnvironmentValue (VString &rResult, char const* pName, char const*	pDefaultValue);
    V_API unsigned int	GetEnvironmentValue (char const *pName, unsigned int	iDefaultValue);


//-----------
//  Exit Code Access and Management
//-----------

    V_API int ExitCode ();
    V_API int ExitErrorCode ();

    V_API int SetExitCodeTo (int xExitCode);
    V_API int SetExitCodeToError ();
    V_API int SetExitErrorCodeTo (int xExitCode);

//-----------
//  Utilities
//-----------
    V_API bool SetCloseOnExec (unsigned int fileHandle);
}


#endif

#ifndef Vdb_Interface
#define Vdb_Interface

/*************************
 *****  Compilation  *****
 *************************/

#ifdef Vdb_EXPORTS
#define Vdb_API DECLSPEC_DLLEXPORT

#else
#define Vdb_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vdb.lib")
#endif

#endif

#define Vdb_NAMESPACE_ENTER namespace Vdb {
#define Vdb_NAMESPACE_EXIT }

#define DECLARE_VdbINTERFACE(ifName) DECLARE_API_VINTERFACE(Vdb,ifName)


/***********************
 *****  Namespace  *****
 ***********************/

namespace Vdb {
}


#endif

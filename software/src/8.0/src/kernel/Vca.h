/**
 * @file
 *
 * This file introduces the Vca namespace and declares the public procedural
 * interface to the Vision Connection Architecture.
 *
 * Vca provides a number of routines supporting the development of Vca peers.
 * Vca is object-oriented, with objects exposing interfaces that are passed
 * by reference from one peer to another.  All peers can simultaneously and
 * automatically function as service providers and service consumers -- any
 * peer that passes a reference to one of its objects to another peer is
 * automatically a server for that object; any peer that uses the methods of
 * an object for which it has received a reference is a client of that object.
 *
 * While Vca's client-server computing model is symmetric, Vca must still deal
 * with the problem of exchanging object references between disconnected and
 * possibly unrelated processes.  Vca provides routines to create services
 * that associate objects with addresses (including, but not limited to, TCP
 * service ports) and communication channels.  To access those services, Vca
 * provides routines that connect to addresses and that query connections for
 * their associated objects.  Additionally, there are Vca routines to control
 * Vca's operation (e.g., Shutdown), set tunable execution policy, and provide
 * defaults and custom building blocks for Vca's event dispatcher.
 */


#ifndef Vca_Interface
#define Vca_Interface
#define Vca_Interface_Touch 1

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VCA_EXPORTS
#define Vca_API DECLSPEC_DLLEXPORT

#else
#define Vca_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vca.lib")
#endif

#endif

#define Vca_NAMESPACE_ENTER namespace Vca {
#define Vca_NAMESPACE_EXIT }

#define DECLARE_VcaINTERFACE(ifName) DECLARE_API_VINTERFACE(Vca,ifName)
#define DECLARE_VcaINTERFACE_NOSCOPE(ifName) DECLARE_API_VINTERFACE_NOSCOPE(Vca,ifName)

/**
 * Defines a Vca Interface, for use when defining interfaces that belong to the Vca library. Should be followed by a series of <code>VINTERFACE_METHOD_*</code> lines and finally a VINTERFACE_END.
 * Handles inclusion of appropriate platform-specific import/export tags for use of interfaces inside and outside of the Vca library.
 *
 * @param ifName the interface name.
 * @param ifBase the interface's base's name.
 *
 * @see VINTERFACE_IN_API
 */
#define VcaINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vca)
#define VcaINTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vca)

/**************************
 *****  Declarations  *****
 **************************/

#include "V.h"
#include "V_VCount.h"
#include "V_VSSID.h"

#include "VkDynamicArrayOf.h"

template<class Datum> class IVReceiver;
template<class T> class VReference;

class IVUnknown;

class VString;
class VTypeInfo;

namespace V {
    class VTime;
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

#if defined(_WIN32)
#define Vca_InfiniteWait	INFINITE
#else
#define Vca_InfiniteWait	((size_t)UINT_MAX)
#endif


/*******************************
 *****  The Vca Namespace  *****
 *******************************/

namespace Vca {

/****************************
 *----  Standard Types  ----*
 ****************************/

    typedef ::IVUnknown IVUnknown;
    template<class T> class IDataSource;

    class VBSConsumer;
    class VBSProducer;
    class VMessage;
    class IDirectory;
    class VDirectoryBuilder;

    class VInterfaceQuery;

    class VcaCredentialsProvider;

    class VcaSelf;

    typedef V::VSSID VcaSSID;
    typedef V::VSSID SSID;
    typedef V::ssid_t ssid_t;

    typedef V::unsigned_int32_count_t	counter_t;
    typedef counter_t::value_t		count_t;

/************************
 *----  Wire Types  ----*
 ************************/

    typedef V::F32		F32;
    typedef V::F64		F64;

    typedef V::S08		S08;
    typedef V::S16		S16;
    typedef V::S32		S32;

    typedef V::U08		U08;
    typedef V::U16		U16;
    typedef V::U32		U32;

    typedef V::S64		S64;
    typedef V::U64		U64;


/******************
 *----  Self  ----*
 ******************/

    Vca_API VcaSelf *self ();

/***********************************
 *----  Root Directory Access  ----*
 ***********************************/

    Vca_API void GetRootDirectory (VReference<IDirectory> &rpRootDirectory);

    Vca_API void RegisterDirectoryBuilder (Vca::VDirectoryBuilder *pBuilder);

    Vca_API void GetObjectNames (
	IVReceiver<VkDynamicArrayOf<VString> const&> *pResultReceiver
    );

    Vca_API void GetObjectDescription (
	IVReceiver<VString const&> *pResultReceiver, VString const &rObjectName
    );

    Vca_API void GetObject (VInterfaceQuery const &rQuery, VString const &rObjectName);
  
    Vca_API void GetCredentialsProvider (VReference<Vca::VcaCredentialsProvider> &rpCredentialsProvider);
   

/*********************
 *----  Control  ----*
 *********************/

    Vca_API bool Start (VString *pMessasgeReturn = 0);
    Vca_API bool Stop (VString *pMessageReturn = 0);
    Vca_API void Shutdown ();
    Vca_API bool ShutdownCalled ();

    Vca_API bool EventsDisabled (size_t &rsTimeout, V::VTime const &rtLastEvent);

    Vca_API void Exit (int xExitStatus);  //  calls _exit and never returns.
    Vca_API bool ExitCalled ();

/**********************
 *----  Defaults  ----*
 **********************/
    namespace Default {
	Vca_API bool TcpNoDelaySetting ();
    }
}


#endif

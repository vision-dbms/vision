#ifndef Vca_IProcessInfoSink_Interface
#define Vca_IProcessInfoSink_Interface

#ifndef Vca_IProcessInfoSink
#define Vca_IProcessInfoSink extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IProcessInfo.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IProcessInfoSink)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    VcaINTERFACE (IProcessInfoSink, IClient)
    /**
     *  Receiver for basic information about a process.
     *
     *  @param p1 An IProcessInfo inteface of the object supplying this information.
     *  @param p2 The pid_t associated with the process.
     *  @param p3 The uid_t associated with the process.
     *  @param p4 The gid_t associated with the process.
     *  @param p6 The host name of the system on which the process is running.
     *  @param p6 The user name associated with the value of parameter 3. 
     *  @param p7 The group name associated with the value of parameter 4.
     *
     */
	VINTERFACE_METHOD_7 (
	    OnBasics,
	    IProcessInfo*,
	    ProcessInfo::pid_t,
	    ProcessInfo::uid_t,
	    ProcessInfo::gid_t,
	    VString const&,
	    VString const&,
	    VString const&
	);
    VINTERFACE_END

    VINTERFACE_ROLE (IProcessInfoSink, IClient)
	VINTERFACE_ROLE_7 (
	    OnBasics,
	    IProcessInfo*,
	    ProcessInfo::pid_t,
	    ProcessInfo::uid_t,
	    ProcessInfo::gid_t,
	    VString const&,
	    VString const&,
	    VString const&
	);
    VINTERFACE_ROLE_END
}


#endif

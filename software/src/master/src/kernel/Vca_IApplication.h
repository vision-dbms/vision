#ifndef Vca_IApplication_Interface
#define Vca_IApplication_Interface

#ifndef Vca_IApplication
#define Vca_IApplication extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IApplication)

namespace Vca {
    VINTERFACE_ROLE (IApplication, IVUnknown)
	VINTERFACE_ROLE_1 (GetLogFilePath, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_1 (SetLogFilePath, VString const &);
	VINTERFACE_ROLE_1 (GetLogSwitch, IVReceiver<bool>*);
	VINTERFACE_ROLE_1 (SetLogSwitch, bool);
	VINTERFACE_ROLE_1 (GetLogFileSize, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetLogFileSize, Vca::U32);
	VINTERFACE_ROLE_1 (GetLogFileBackups, IVReceiver<Vca::U32>*);
	VINTERFACE_ROLE_1 (SetLogFileBackups, Vca::U32);
	VINTERFACE_ROLE_1 (GetStatistics, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IApplication, IVUnknown)
	VINTERFACE_METHOD_1 (GetLogFilePath, IVReceiver<VString const &>*);
	VINTERFACE_METHOD_1 (SetLogFilePath, VString const &);
	VINTERFACE_METHOD_1 (GetLogSwitch, IVReceiver<bool>*);
	VINTERFACE_METHOD_1 (SetLogSwitch, bool);
	VINTERFACE_METHOD_1 (GetLogFileSize, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetLogFileSize, Vca::U32);
	VINTERFACE_METHOD_1 (GetLogFileBackups, IVReceiver<Vca::U32>*);
	VINTERFACE_METHOD_1 (SetLogFileBackups, Vca::U32);
	VINTERFACE_METHOD_1 (GetStatistics, IVReceiver<VString const&>*);
    VINTERFACE_END
}


#endif

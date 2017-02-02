/*****  Vca_IApplication Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#define   Vca_IApplication
#include "Vca_IApplication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****			   *****
 *****  Vca::IApplication  *****
 *****			   *****
 *******************************
 *******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IApplication)

namespace Vca {
// {a75cefd4-966c-4c86-b010-717d8e541ff0}
    VINTERFACE_TYPEINFO_DEFINITION (
	IApplication,
	0xa75cefd4, 0x966c, 0x4c86, 0xb0, 0x10, 0x71, 0x7d, 0x8e, 0x54, 0x1f, 0xf0
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, GetLogSwitch, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, SetLogSwitch, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, GetLogFilePath, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, SetLogFilePath, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, GetLogFileSize, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, SetLogFileSize, 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, GetLogFileBackups, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, SetLogFileBackups, 7);
    VINTERFACE_MEMBERINFO_DEFINITION (IApplication, GetStatistics, 8);
}

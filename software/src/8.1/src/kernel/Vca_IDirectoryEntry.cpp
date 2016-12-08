/*****  Vca_IDirectoryEntry Implementation  *****/

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

#define   Vca_IDirectoryEntry
#include "Vca_IDirectoryEntry.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vca::IDirectoryEntry  *****
 *****                        *****
 **********************************
 **********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IDirectoryEntry)

namespace Vca {
// {E903A844-C50D-42e7-A49D-37C318A0A159}
    VINTERFACE_TYPEINFO_DEFINITION (
	IDirectoryEntry,
	0xe903a844, 0xc50d, 0x42e7, 0xa4, 0x9d, 0x37, 0xc3, 0x18, 0xa0, 0xa1, 0x59
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectoryEntry, GetObject, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectoryEntry, SetObject, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectoryEntry, SetSource, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectoryEntry, SetDescription, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectoryEntry, GetDescription, 4);
}

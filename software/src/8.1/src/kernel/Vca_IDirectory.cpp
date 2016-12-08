/*****  Vca_IDirectory Implementation  *****/

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

#define   Vca_IDirectory
#include "Vca_IDirectory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*****************************
 *****************************
 *****                   *****
 *****  Vca::IDirectory  *****
 *****                   *****
 *****************************
 *****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IDirectory)

namespace Vca {
// {D93B0937-2BD7-48f4-89FB-24E08042101D}
    VINTERFACE_TYPEINFO_DEFINITION (
	IDirectory,
	0xd93b0937, 0x2bd7, 0x48f4, 0x89, 0xfb, 0x24, 0xe0, 0x80, 0x42, 0x10, 0x1d
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, GetObject, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, SetObject, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, SetSource, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, SetDescription, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, GetDescription, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IDirectory, GetEntries, 5);
}

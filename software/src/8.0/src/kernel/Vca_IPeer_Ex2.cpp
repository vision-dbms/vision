/*****  Vca_IPeer_Ex2 Implementation  *****/

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

#define   Vca_IPeer_Ex2
#include "Vca_IPeer_Ex2.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IPeer_Ex2  *****
 *****                  *****
 ****************************
 ****************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPeer_Ex2)

namespace Vca {
// {77E3196C-1529-42e2-B7D2-34EFED999980}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPeer_Ex2, 0x77e3196c, 0x1529, 0x42e2, 0xb7, 0xd2, 0x34, 0xef, 0xed, 0x99, 0x99, 0x80
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPeer_Ex2, ReleaseExportEx, 0);
}

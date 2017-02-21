/*****  Vca_IAdmin Implementation  *****/

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

#define   Vca_Main_IAdmin
#include "Vca_IAdmin.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************
 *************************
 *****               *****
 *****  Vca::IAdmin  *****
 *****               *****
 *************************
 *************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IAdmin)

namespace Vca {
    // {9f08bc39-df2f-4464-9973-eb177340e64d}
    VINTERFACE_TYPEINFO_DEFINITION (
	IAdmin,
	0x9f08bc39, 0xdf2f, 0x4464, 0x99, 0x73, 0xeb, 0x17, 0x73, 0x40, 0xe6, 0x4d
    );
    //VINTERFACE_MEMBERINFO_DEFINITION (IAdmin, foo, 0);
}

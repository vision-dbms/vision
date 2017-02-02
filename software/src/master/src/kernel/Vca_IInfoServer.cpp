/*****  Vca_IInfoServer Implementation  *****/

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

#define   Vca_IInfoServer
#include "Vca_IInfoServer.h"

/************************
 *****  Supporting  *****
 ************************/
#include "Vca_CompilerHappyPill.h"


/*************************************
 *************************************
 *****	   	                 *****
 *****      Vca::IInfoServer     *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IInfoServer)

namespace Vca {
// {B7628804-1002-4f70-AADB-46B57F2CA1E8}
    VINTERFACE_TYPEINFO_DEFINITION (
	IInfoServer,
        0xb7628804, 0x1002, 0x4f70, 0xaa, 0xdb, 0x46, 0xb5, 0x7f, 0x2c, 0xa1, 0xe8
    );
    VINTERFACE_MEMBERINFO_DEFINITION (IInfoServer, Notify, 0);
}

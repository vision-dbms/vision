/*****  Vps_IPublication Implementation  *****/
#define Vps_IPublication

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

#include "Vps_IPublication.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vca::Vps::IPublication  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Vps::IPublication)

namespace Vca {
  namespace Vps {

// {8ABF4F7E-E8CE-4F31-9831-8312C9866190}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPublication,
	0x8abf4f7e, 0xe8ce, 0x4f31, 0x98, 0x31,0x83, 0x12, 0xc9, 0x86, 0x61, 0x90
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPublication, Subscribe, 0);
  }
}

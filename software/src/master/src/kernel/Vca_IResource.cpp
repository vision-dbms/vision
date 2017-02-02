/*****  Vca_IResource Implementation  *****/

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

#define   Vca_IResource
#include "Vca_IResource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IResource  *****
 *****                  *****
 ****************************
 ****************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IResource)

namespace Vca {
// {8498D40E-B9A8-49df-AA1B-E7C5111B41B3}
    VINTERFACE_TYPEINFO_DEFINITION (
	IResource,
	0x8498d40e, 0xb9a8, 0x49df, 0xaa, 0x1b, 0xe7, 0xc5, 0x11, 0x1b, 0x41, 0xb3
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IResource, Close, 0);
}

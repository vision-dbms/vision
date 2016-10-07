/*****  VcaSamples_ISubscription Implementation  *****/
#define VcaSamples_ISubscription

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

#include "vca_samples_isubscription.h"

/************************
 *****  Supporting  *****
 ************************/


/***************************************
 ***************************************
 *****	   	                   *****
 *****  VcaSamples::ISubscription  *****
 *****                             *****
 ***************************************
 ***************************************/

VINTERFACE_TEMPLATE_EXPORTS (VcaSamples::ISubscription)

namespace VcaSamples {
// {9EA263AA-D0F5-4966-9F9E-76E23AE5D482}
    VINTERFACE_TYPEINFO_DEFINITION (
	ISubscription,
	0x9ea263aa, 0xd0f5, 0x4966, 0x9f, 0x9e, 0x76, 0xe2, 0x3a, 0xe5, 0xd4, 0x82
    );

    VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Suspend	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Resume	, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (ISubscription, Cancel	, 2);
}

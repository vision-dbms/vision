/*****  Vsa_IEvaluatorClient Implementation  *****/
#define Vsa_IEvaluatorClient

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

#include "Vsa_IEvaluatorClient.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vsa::IEvaluatorClient  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorClient)

namespace Vsa {
    // {90256AB3-D207-413f-B01D-B067233561BA}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorClient,
	0x90256ab3, 0xd207, 0x413f, 0xb0, 0x1d, 0xb0, 0x67, 0x23, 0x35, 0x61, 0xba
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorClient, OnAccept, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorClient, OnChange, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorClient, OnResult, 2);
}

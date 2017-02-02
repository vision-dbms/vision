/*****  Vsa_IPoolEvaluation Implementation  *****/
#define Vsa_IPoolEvaluation

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

#include "Vsa_IPoolEvaluation.h"

/************************
 *****  Supporting  *****
 ************************/


/**********************************
 **********************************
 *****                        *****
 *****  Vsa::IPoolEvaluation  *****
 *****                        *****
 **********************************
 **********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolEvaluation)

namespace Vsa {
    // {BD40B499-DDCE-4DAB-AC5E-E5BFCCDCF039}

    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolEvaluation,
	0xbd40b499, 0xddce, 0x4dab, 0xac, 0x5e, 0xe5, 0xbf, 0xcc, 0xdc, 0xf0, 0x39
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPoolEvaluation, UseAnyGenerationWorker, 0);
}

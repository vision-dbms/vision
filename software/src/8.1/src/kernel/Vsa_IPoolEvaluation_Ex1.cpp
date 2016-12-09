/*****  Vsa_IPoolEvaluation_Ex1 Implementation  *****/
#define Vsa_IPoolEvaluation_Ex1

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

#include "Vsa_IPoolEvaluation_Ex1.h"

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

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IPoolEvaluation_Ex1)

namespace Vsa {
    // {cbc9154e-97f8-4dbd-a2e4-ac2df76552ed}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPoolEvaluation_Ex1,
        0xcbc9154e, 0x97f8, 0x4dbd, 0xa2, 0xe4, 0xac, 0x2d, 0xf7, 0x65, 0x52, 0xed
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IPoolEvaluation_Ex1, SetRetryable, 0);
}

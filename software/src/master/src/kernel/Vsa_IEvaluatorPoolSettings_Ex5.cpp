/*****  Vsa_IEvaluatorPoolSettings_Ex5 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex5

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

#include "Vsa_IEvaluatorPoolSettings_Ex5.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex5  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex5)

namespace Vsa {

    // {5758aff4-37b7-4839-bf4d-9c40feb13cfe}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex5,
	0x5758aff4, 0x37b7, 0x4839, 0xbf, 0x4d, 0x9c, 0x40, 0xfe, 0xb1, 0x3c, 0xfe
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex5, GetWorkerCreationFailureSoftLimit, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex5, SetWorkerCreationFailureSoftLimit, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex5, GetWorkerPIDQuery, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex5, SetWorkerPIDQuery, 3);
}

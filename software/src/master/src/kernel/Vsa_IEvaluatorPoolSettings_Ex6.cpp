
/*****  Vsa_IEvaluatorPoolSettings_Ex6 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex6

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

#include "Vsa_IEvaluatorPoolSettings_Ex6.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex6  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex6)

namespace Vsa {

    // {86aaf6c0-8e48-4318-8824-6c400c2095e0}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex6,
        0x86aaf6c0, 0x8e48, 0x4318, 0x88, 0x24, 0x6c, 0x40, 0x0c, 0x20, 0x95, 0xe0
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex6, GetGenerationMaximum, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex6, SetGenerationMaximum, 1);
}

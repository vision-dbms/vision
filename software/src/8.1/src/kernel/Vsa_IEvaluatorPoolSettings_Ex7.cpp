/*****  Vsa_IEvaluatorPoolSettings_Ex7 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex7

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

#include "Vsa_IEvaluatorPoolSettings_Ex7.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex7  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex7)

namespace Vsa {

    // {8bcbf8dc-c1ff-4216-ad7e-8cc8785b1feb}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex7,
        0x8bcbf8dc, 0xc1ff, 0x4216, 0xad, 0x7e, 0x8c, 0xc8, 0x78, 0x5b, 0x1f, 0xeb
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex7, GetQueryLogLength, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex7, SetQueryLogLength, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex7, GetResultLogLength, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex7, SetResultLogLength, 3);
}

/*****  Vsa_IEvaluatorPoolSettings_Ex3 Implementation  *****/
#define Vsa_IEvaluatorPoolSettings_Ex3

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

#include "Vsa_IEvaluatorPoolSettings_Ex3.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vsa::IEvaluatorPoolSettings_Ex3  *****
 *****                                   *****
 *********************************************
 *********************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPoolSettings_Ex3)

namespace Vsa {

    // {4740C412-97D6-4312-AF25-FD637F725B65}

    
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPoolSettings_Ex3,
	0x4740C412, 0x97d6, 0x4312, 0xaf, 0x25, 0xfd, 0x63, 0x7f, 0x72, 0x5b, 0x65
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex3, GetStopTimeOut, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPoolSettings_Ex3, SetStopTimeOut, 1);
}

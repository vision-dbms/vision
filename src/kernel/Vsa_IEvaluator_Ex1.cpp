/*****  Vsa_IEvaluatorPool_Ex1 mplementation  *****/
#define Vsa_IEvaluator_Ex1

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

#include "Vsa_IEvaluator_Ex1.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

/*********************************
 *********************************
 *****                       *****
 *****  Vsa::IEvaluator_Ex1  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluator_Ex1)

namespace Vsa {
	
	// {3a308db5-1cb3-4917-a9de-2406b0762d84}


    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluator_Ex1,
        0x3a308db5, 0x1cb3, 0x4917, 0xa9, 0xde, 0x24, 0x06, 0xb0, 0x76, 0x2d, 0x84
    );
    
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluator_Ex1, EvaluateExpression_Ex, 0);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluator_Ex1, EvaluateURL_Ex       , 1);
}



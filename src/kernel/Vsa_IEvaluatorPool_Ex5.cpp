/*****  Vsa_IEvaluatorPool_Ex5 mplementation  *****/
#define Vsa_IEvaluatorPool_Ex5

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

#include "Vsa_IEvaluatorPool_Ex5.h"

/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorPool_Ex5  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool_Ex5)

namespace Vsa {
	
	// {203890d0-cc55-11e1-9b23-0800200c9a66}


    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool_Ex5,
        0x203890d0, 0xcc55, 0x11e1, 0x9b, 0x23, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
    );
    
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatSum, 0);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatDivide, 1);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatMax, 2);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatMin, 3);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatMaxString, 4);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, StatMinString, 5);
	VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex5, AllStatValues, 6);	
}



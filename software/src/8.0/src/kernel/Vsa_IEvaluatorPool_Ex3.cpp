/*****  Vsa_IEvaluatorPool_Ex3 Implementation  *****/
#define Vsa_IEvaluatorPool_Ex3

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

#include "Vsa_IEvaluatorPool_Ex3.h"

/*************************************
 *************************************
 *****                           *****
 *****  Vsa::IEvaluatorPool_Ex3  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vsa::IEvaluatorPool_Ex3)

namespace Vsa {
// {FFA5159B-D27C-4713-95DA-A66D57556F63}

    VINTERFACE_TYPEINFO_DEFINITION (
	IEvaluatorPool_Ex3,
	0xffa5159b, 0xd27c, 0x4713, 0x95, 0xda, 0xa6, 0x6d, 0x57, 0x55, 0x6f, 0x63
    );

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, TakeWorkerOffline, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, MakeWorkerOnline , 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, EvaluateUsingOfflineWorker , 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, RetireOfflineWorker , 3);

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, DumpPoolQueue , 4);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, DumpWorkingWorkers , 5);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, DumpWorker , 6);

    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, GetWorker_Ex , 7);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, GetWorkersStatistics , 8); 
    VINTERFACE_MEMBERINFO_DEFINITION (IEvaluatorPool_Ex3, GetGenerationStatistics , 9);     
}



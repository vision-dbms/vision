#ifndef VEvaluationCall_Interface
#define VEvaluationCall_Interface

/************************
 *****  Components  *****
 ************************/

#include "VUtilityCall.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

class VEvaluationCall : public VUtilityCall {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VEvaluationCall, VUtilityCall);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VEvaluationCall (
	VTask* pCaller, rtLINK_CType* pCallerSubset, unsigned int iParameterCount
    );

//  State
protected:
};


#endif

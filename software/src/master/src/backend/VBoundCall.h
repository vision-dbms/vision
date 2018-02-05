#ifndef VBoundCall_Interface
#define VBoundCall_Interface

/************************
 *****  Components  *****
 ************************/

#include "VUtilityCall.h"
#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VBoundCall : public VUtilityCall {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VBoundCall, VUtilityCall);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VBoundCall (
	VTask *pCaller, rtLINK_CType *pCallerSubset, unsigned int iParameterCount, M_CPD *pBlock
    );

//  Destruction
private:
    ~VBoundCall ();

//  Access
public:
    virtual rtBLOCK_Handle *boundBlock () const OVERRIDE;

//  State
protected:
    VCPDReference const m_pBoundBlock;
};


#endif

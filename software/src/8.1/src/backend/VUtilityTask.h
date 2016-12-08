#ifndef VUtilityTask_Interface
#define VUtilityTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTask.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/
/*---------------------------------------------------------------------------*
 * Field Descriptions:
 *---------------------------------------------------------------------------*/

class VUtilityTask : public VTask {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VUtilityTask, VTask);

//  Types
public:
    typedef void (*Continuation) (VUtilityTask* pTask);

//  Standard Continuations
public:
    static void Bridge	 (VUtilityTask* pTask);
    static void Delegate (VUtilityTask* pTask); 
    static void Evaluate (VUtilityTask* pTask);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VUtilityTask (ConstructionData const& rTCData, Continuation pContinuation);

//  Execution
protected:
    void run ();

//  ... Continuation Control
public:
    void setContinuationTo (Continuation pContinuation) {
	m_pContinuation = pContinuation;
    }

//  Call Construction
protected:
    void beginSecondaryCall (rtLINK_CType* pSubset = 0);

//  Display and Description
public:
    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;

//  Exception and Warning Generation
public:

//  State
protected:
    Continuation m_pContinuation;
};


#endif

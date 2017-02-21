#ifndef VGuardedDoubleGenerator_Interface
#define VGuardedDoubleGenerator_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VCPDReference.h"
#include "VfGuardTool.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VTask.h"


/*************************
 *****  Definitions  *****
 *************************/

class VGuardedDoubleGenerator : public VTransient {
//  Construction
public:
    VGuardedDoubleGenerator (VTask* pTask);

//  Access
public:
    unsigned int cardinality () const {
	return m_pTask->cardinality ();
    }

//  Update
public:
    void append (double iValue) {
	if (isfinite (*m_pCursor++ = iValue))
	    m_iGuard.addNext ();
	else
	    m_iGuard.skipNext ();
    }

    void commit ();

//  State
protected:
    VTask* const	m_pTask;
    VCPDReference const	m_pVector;
    VfGuardTool		m_iGuard;
    double*		m_pCursor;
};


#endif

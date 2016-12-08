#ifndef VPrimitiveDescriptor_Interface
#define VPrimitiveDescriptor_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VMutex.h"

/**************************
 *****  Declarations  *****
 **************************/

class M_CPD;

class rtLINK_CType;

class VCall;

class VPrimitiveTaskBase;
class VPrimitiveTask;

class VTaskConstructionData;

/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * Primitive descriptions are used to define the index, name, and code address
 * of each primitive in the system.
 *
 * Primitive Description Field Definitions:
 *	m_xPrimitive		- the index of the primitive function.
 *	m_pName			- the address of a READ ONLY string
 *				  naming the primitive.
 *	m_pContinuation		- the address of the initial continuation for
 *				  VPrimitiveTask implemented primitives.
 *	m_iMutex		- the address of the computation unit holding
 *				  the mutex for this primitive (provided for
 *				  completeness).
 *	m_iUseCount		- the number of times this primitive has been
 *				  invoked in this session.
 *---------------------------------------------------------------------------
 */
class VPrimitiveDescriptor : public VTransient {
//  Implementation Class Enumeration
public:
    enum IClass {
	IClass_PrimitiveTask,
	IClass_DLGEventWaitController,
	IClass_ListEnumerationController,
	IClass_MutexController,
	IClass_ReadEvalPrintController,
	IClass_TrapController,
	IClass_WhileTrueController
    };

//  Standard Continuation
public:
    typedef void (*Continuation) (VPrimitiveTask*);

//  Construction
public:
    VPrimitiveDescriptor (unsigned int xPrimitive, char const* pName, Continuation pCode)
	: m_xPrimitive		(xPrimitive)
	, m_xIClass		(IClass_PrimitiveTask)
	, m_pName		(pName)
	, m_pContinuation	(pCode)
	, m_iUseCount		(0)
    {
    }

    VPrimitiveDescriptor (unsigned int xPrimitive, char const* pName, IClass xIClass)
	: m_xPrimitive		(xPrimitive)
	, m_xIClass		(xIClass)
	, m_pName		(pName)
	, m_pContinuation	(0)
	, m_iUseCount		(0)
    {
    }

//  Destruction
public:
    ~VPrimitiveDescriptor () {
    }

//  Access
public:
    IClass iclass () const {
	return (IClass)m_xIClass;
    }

    unsigned int index () const {
	return m_xPrimitive;
    }

    char const* name () const {
	return m_pName;
    }

    Continuation continuation () const {
	return m_pContinuation;
    }

    unsigned int useCount () const {
	return m_iUseCount;
    }

//  Mutex Support
public:
    bool AcquireMutex (VMutexClaim& rClaim, VComputationUnit* pSupplicant) {
	return m_iMutex.acquireClaim (rClaim, pSupplicant);
    }

//  Task Instantiation
public:
    VPrimitiveTaskBase* instantiate (
	VTaskConstructionData const& rTCData, unsigned short iFlags
    );

//  State
protected:
    unsigned int const	m_xPrimitive	: 16;
    IClass const	m_xIClass	: 8;
    unsigned int			: 8;
    char const* const	m_pName;
    Continuation const	m_pContinuation;
    VMutex		m_iMutex;
    unsigned int	m_iUseCount;
};

#endif

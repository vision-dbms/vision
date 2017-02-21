#ifndef VPrimitiveTask_Interface
#define VPrimitiveTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTask.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VPrimitiveDescriptor.h"

#include "Vca_VGoferInterface.h"


/*************************
 *****  Definitions  *****
 *************************/

/*************************************
 *----  Maximum Primitive Index  ----*
 *************************************/

#define V_PF_MaxPrimitiveIndex   700

/********************************
 *----  VPrimitiveTaskBase  ----*
 ********************************/

class ABSTRACT VPrimitiveTaskBase : public VTask {
    DECLARE_ABSTRACT_RTT (VPrimitiveTaskBase, VTask);

//  Aliases
public:
    typedef Vca::VGoferInterface<Vxa::ISingleton> object_gofer_t;

//  Meta Maker
protected:
    static void MetaMaker ();

//  Primitive Dispatch Vector...
//  ... Declaration
protected:
    static VPrimitiveDescriptor* g_iPDV[V_PF_MaxPrimitiveIndex];

//  ... Access
public:
    static VPrimitiveDescriptor* PrimitiveDescriptor (unsigned int xPrimitive) {
	xPrimitive %= 65536;
	return xPrimitive < V_PF_MaxPrimitiveIndex ? g_iPDV [xPrimitive] : 0;
    }

//  ... Initialization
public:
    static void InstallPrimitives (VPrimitiveDescriptor* pDescriptors, unsigned int cDescriptors);

//  Task Instantiation
public:
    static VPrimitiveTaskBase* Instantiate (ConstructionData const& rTCData, unsigned int xPrimitive) {
	VPrimitiveDescriptor* pDescriptor = PrimitiveDescriptor (xPrimitive % 65536);

	return (pDescriptor ? pDescriptor : g_iPDV[0])->instantiate (
	    rTCData, (unsigned short)(xPrimitive / 65536)
	);
    }

//  Construction
protected:
    VPrimitiveTaskBase (
	ConstructionData const& rTCData, VPrimitiveDescriptor *pDescriptor, unsigned short iFlags
    ) : VTask (rTCData), m_pDescriptor (pDescriptor), m_iFlags (iFlags) {
	PrimitiveTaskCount++;

//	if (pDescriptor->breakOnEntryIsSet ())
//	    setStepDown ();
    }

//  Access
public:
    VPrimitiveDescriptor* descriptor () const {
	return m_pDescriptor;
    }
    unsigned int flags () const {
	return m_iFlags;
    }
    unsigned int primitive () const {
	return m_pDescriptor->index ();
    }

//  Display
public:
    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;

//  Exception Generation
public:
    void raiseUnimplementedAliasException (char const* pCaller);

//  External Object Access
public:
    void accessClientObject ();
    void accessExternalObject (VString const &rObjectName, bool bUsingDirectory);
private:
    void accessExternalObject (object_gofer_t* pObjectGofer);

    void onExternalError  (Vca::IError *pError, VString const &rMessage);
    void onExternalObject (Vxa::ISingleton *pUnknown);

//  State
protected:
    VPrimitiveDescriptor* const	m_pDescriptor;
    unsigned short const	m_iFlags;
};


/****************************
 *----  VPrimitiveTask  ----*
 ****************************/

class VPrimitiveTask : public VPrimitiveTaskBase {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VPrimitiveTask, VPrimitiveTaskBase);

//  Types
public:
    typedef VPrimitiveDescriptor::Continuation Continuation;

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VPrimitiveTask (
	ConstructionData const& rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    )
    : VPrimitiveTaskBase(rTCData, pDescriptor, iFlags)
    , m_pContinuation	(pDescriptor->continuation ())
    {
    }

//  Access
public:
    Continuation continuation () const {
	return m_pContinuation;
    }

//  Execution
protected:
    void run ();

public:
    void setContinuationTo (Continuation pContinuation) {
	m_pContinuation = pContinuation;
    }

//  State
protected:
    Continuation m_pContinuation;
};


/*******************************
 *******************************
 *****  Primitive Support  *****
 *******************************
 *******************************/

/*******************************************************
 *****  Miscellaneous Supporting Types And Macros  *****
 *******************************************************/

/*******************
 *  String Access  *
 *******************/

#define V_BlockString(stringStore,value) (\
    rtBLOCK_CPD_StringSpace (stringStore) + (value)\
)

#define V_LStoreString(stringStore,charCPD,value) (\
    rtCHARUV_CPD_Array (charCPD) +\
    rtLSTORE_CPD_BreakpointArray (stringStore)[value]\
)

/****************************
 *  8-Byte Type Definition  *
 ****************************/

union V_EightByte {
    double	asDouble;
    struct int_pair_t {
	int msw, lsw;
    }		asIntegers;
};

#define V_EightByte_AsDouble(eb)((eb)->asDouble)
#define V_EightByte_MSW(eb)	((eb)->asIntegers.msw)
#define V_EightByte_LSW(eb)	((eb)->asIntegers.lsw)


/*****************************
 *****  Register Access  *****
 *****************************/

#define ADescriptor		(pTask->ducMonotype	())
#define DucIsAScalar		(pTask->isScalar	())


/*************************
 *****  Task Access  *****
 *************************/

#define V_TOTSC_PToken		(pTask->ptoken		())

/*****  Activation Access Macros  *****/
#define V_TOTSC_Primitive	(pTask->primitive	())
#define V_TOTSC_PrimitiveFlags	(pTask->flags		())


#endif

/*****  VCall Implementation  *****/

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

#include "VCall.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"
#include "vmagic.h"
#include "vprimfns.h"

#include "RTclosure.h"
#include "RTcontext.h"
#include "RTclosure.h"
#include "RTcontext.h"
#include "RTdictionary.h"
#include "RTmethod.h"
#include "RTvstore.h"

#include "VFragment.h"
#include "VBlockTask.h"
#include "VPrimitiveTask.h"
#include "VSNFTask.h"
#include "VUtilityTask.h"
#include "VSelector.h"
#include "V_VString.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Utility Task Continuations  *****
 *****                              *****
 ****************************************
 ****************************************/

static void UTC__ValueForNonConformantKey (VUtilityTask* pTask) {
    pTask->beginMessageCall ("valueForNonConformantKey", 0);
    pTask->commitRecipient ();
    pTask->commitCall ();
}


/************************
 ************************
 *****              *****
 *****  VSteppable  *****
 *****              *****
 ************************
 ************************/

unsigned int g_cMessages = 0;
unsigned int g_cSteppings = 0;
unsigned int g_sSteppings = 0;
unsigned int g_cSteppingsOptimized = 0;
unsigned int g_sSteppingsOptimized = 0;
unsigned int g_cSteppingsSuppressed = 0;
unsigned int g_sSteppingsSuppressed = 0;

class VSteppable : public VTransient {
    DECLARE_FAMILY_MEMBERS (VSteppable, VTransient);

//  Step
public:
    class Step : public VReferenceable {
	DECLARE_CONCRETE_RTT (Step, VReferenceable);

	friend class VSteppable;

    //  Construction
    public:
	Step (DSC_Pointer const &rPointer) {
	    m_iPointer.construct (rPointer);
	}

    //  Destruction
    private:
	~Step () {
	    m_iPointer.clear ();
	}

    //  Access
    public:
	DSC_Pointer &pointer () {
	    return m_iPointer;
	}

    //  State
    private:
	Reference m_pSuccessor;
	DSC_Pointer m_iPointer;
    };

//  Construction
public:
    VSteppable (M_CPD *pDPT, DSC_Descriptor const &rValue) : m_pDPT (pDPT), m_bPointerIsNil (false), m_cSteppings (0) {
	m_iValue.construct (rValue);
	m_iValue.getCanonicalStore (m_pCanonicalStore, m_bPointerMustBeConverted);
    }

//  Destruction
public:
    ~VSteppable () {
	if (m_cSteppings) {
	    unsigned int sSteppings = m_cSteppings * rtPTOKEN_CPD_BaseElementCount (m_pDPT);
	    g_cSteppings += m_cSteppings;
	    g_sSteppings += sSteppings;
	    if (m_bPointerIsNil) {
		g_cSteppingsOptimized += m_cSteppings;
		g_sSteppingsOptimized += sSteppings;
	    } else if (m_pSteppingsHead) {
		g_cSteppingsSuppressed += m_cSteppings;
		g_sSteppingsSuppressed += sSteppings;
	    }
	}
	m_iValue.clear ();
    }

//  Access
public:
    operator DSC_Descriptor& () {
	normalize ();
	return m_iValue;
    }

    M_CPD *canonicalStore () const {
	return m_pCanonicalStore;
    }

//  Update
private:
    void normalize ();
public:
    void setStoreTo (M_CPD *pStore);	//  -> convert pointer if necessary
    void step ();

//  State
private:
    M_CPD *const	m_pDPT;
    DSC_Descriptor	m_iValue;
    Step::Reference	m_pSteppingsHead;
    Step::Reference	m_pSteppingsTail;
    unsigned int	m_cSteppings;
    M_CPD *		m_pCanonicalStore;
    bool		m_bPointerMustBeConverted;
    bool		m_bPointerIsNil;
};

DEFINE_CONCRETE_RTT (VSteppable::Step);

void VSteppable::normalize () {
    if (m_bPointerIsNil) {
	M_CPD *pRPT = m_iValue.RPT ();
	m_iValue.Pointer().constructReferenceConstant (m_pDPT, pRPT, rtPTOKEN_CPD_BaseElementCount (pRPT));
    }
    else if (m_pSteppingsHead) {
	DSC_Pointer iSubscript;
	iSubscript.construct (m_pSteppingsHead->pointer ());
	m_pSteppingsHead.claim (m_pSteppingsHead->m_pSuccessor);

	while (m_pSteppingsHead) {
	    DSC_Pointer iNewSubscript;
	    iNewSubscript.constructComposition (iSubscript, m_pSteppingsHead->pointer ());
	    iSubscript.clear ();
	    iSubscript = iNewSubscript;

	    m_pSteppingsHead.claim (m_pSteppingsHead->m_pSuccessor);
	}
	m_pSteppingsTail.clear ();

	DSC_Descriptor iNewValue;
	iNewValue.constructComposition (iSubscript, m_iValue);
	m_iValue.setToMoved (iNewValue);

	iSubscript.clear ();
    }
}

void VSteppable::setStoreTo (M_CPD *pStore) {
    normalize ();
    if (m_bPointerMustBeConverted)
	m_iValue.convertPointer (m_pCanonicalStore, m_pDPT);
    m_iValue.setStoreTo (pStore);
}

void VSteppable::step () {
    m_cSteppings++;
    if (m_bPointerIsNil) {
    //  we already know the result pointer is reference nil...
    }
    else if (m_bPointerMustBeConverted) {
    //  conversions always change the result pointer to reference nil...
	m_bPointerIsNil = true;
	m_pSteppingsHead.clear ();
	m_pSteppingsTail.clear ();
    } else {
	Step::Reference pNextStep (new Step (m_iValue.Pointer ()));
	if (m_pSteppingsTail)
	    m_pSteppingsTail->m_pSuccessor.setTo (pNextStep);
	else
	    m_pSteppingsHead.setTo (pNextStep);
	m_pSteppingsTail.claim (pNextStep);
    }

    DSC_Descriptor iInheritance; {
	M_CPD *pInheritancePointer = rtVSTORE_CPD_InheritancePointerCPD (m_pCanonicalStore);
	iInheritance.constructMonotype (
	    rtVSTORE_CPD_InheritanceStoreCPD (m_pCanonicalStore), pInheritancePointer
	);
	pInheritancePointer->release ();
    }
    m_iValue.setToMoved (iInheritance);
    m_iValue.getCanonicalStore (m_pCanonicalStore, m_bPointerMustBeConverted);
}


/*******************
 *******************
 *****         *****
 *****  VCall  *****
 *****         *****
 *******************
 *******************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

bool VCall::g_bSendingValueMessages = false;


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VCall);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_ciref_c<VCall, VDescriptor>;
template class Vsi_f0_c<VCall, unsigned int>;
template class Vsi_f0_c<VCall, char const*>;
template class Vsi_getr_c<VCall, VString>;
template class Vsi_p0_c<VCall>;

void VCall::MetaMaker () {
    static Vsi_ciref_c<VCall, VDescriptor> const
	si_recipient		(&VCall::crecipient),
	si_searchOrigin		(&VCall::csearchOrigin);

    static Vsi_f0_c<VCall, unsigned int> const
	si_blockIndent		(&VCall::callerDecompilationIndent),
	si_blockOffset		(&VCall::callerDecompilationOffset);

    static Vsi_f0_c<VCall, char const*> const
	si_recipientSourceName	(&VCall::recipientSourceName);

    static Vsi_getr_c<VCall, VString> const
	si_blockText		(&VCall::callerDecompilation),
	si_blockPrefix		(&VCall::callerDecompilationPrefix),
	si_blockSuffix		(&VCall::callerDecompilationSuffix);

    static Vsi_p0_c<VCall> const
	si_dumpByteCodes	(&VCall::dumpCallerByteCodes);

    CSym ("isACall"		)->set (RTT, &g_siTrue);
    CSym ("isABoundCall"	)->set (RTT, &g_siFalse);
    CSym ("isAnEvaluationCall"	)->set (RTT, &g_siFalse);
    CSym ("isAPrimaryCall"	)->set (RTT, &g_siFalse);
    CSym ("isASecondaryCall"	)->set (RTT, &g_siFalse);
    CSym ("isAUtilityCall"	)->set (RTT, &g_siFalse);

    CSym ("blockText"		)->set (RTT, &si_blockText);
    CSym ("blockPrefix"		)->set (RTT, &si_blockPrefix);
    CSym ("blockSuffix"		)->set (RTT, &si_blockSuffix);

    CSym ("blockIndent"		)->set (RTT, &si_blockIndent);
    CSym ("blockOffset"		)->set (RTT, &si_blockOffset);

    CSym ("dumpByteCodes"	)->set (RTT, &si_dumpByteCodes);

    CSym ("recipient"		)->set (RTT, &si_recipient);
    CSym ("recipientSourceName"	)->set (RTT, &si_recipientSourceName);
    CSym ("searchOrigin"	)->set (RTT, &si_searchOrigin);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*---------------------------------------------------------------------------*
 *  Arguments:
 *	pCallerSubset		- an optional ('Nil' if absent) constructor for
 *				  a link specifying the 'subset' of the caller
 *				  domain to which this call applies.  If Nil,
 *				  this call has the same domain as its caller.
 *				  The reference count of '*pSubset' will
 *				  be incremented if necessary.
 *---------------------------------------------------------------------------*/
VCall::VCall (VTask* pCaller, rtLINK_CType* pCallerSubset, Type xType, unsigned int iParameterCount)
: VComputationUnit	(pCaller, pCaller, 0)
, m_pCallerSubset	(pCallerSubset)
, m_pConsumer		(pCaller->cuc ())
, m_xType		(xType)
, m_xRecipient		(VMagicWord_Unspecified)
, m_xReturnCase		(Return_Unspecified)
, m_fSNFHushed		(false)
, m_xCallerPC		(UINT_MAX)
, m_xDuc		(0)
, m_iParameterArray	(iParameterCount)
, m_pTemporalContext	(0)
{
    if (m_pConsumer.operator-> () != pCaller)
	m_pConsumer->markAsInterior ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VCall::~VCall () {
    if (m_pTemporalContext)
	m_pTemporalContext->release ();
}


/************************
 ************************
 *****  Completion  *****
 ************************
 ************************/

VCall* VCall::commit (ReturnCase xReturnCase, rtINDEX_Key* pTemporalContext, unsigned int xCallerPC) {
//  Record the final arguments to the call, ...
    m_xReturnCase = xReturnCase;
    m_xCallerPC = xCallerPC;

    if (pTemporalContext != m_pTemporalContext) {
	if (pTemporalContext)
	    pTemporalContext->retain ();

	if (m_pTemporalContext)  // ... this case should never happen.
	    m_pTemporalContext->release ();

	m_pTemporalContext = pTemporalContext;
    }

//  ... suspend this call's consumer, ...
    suspendConsumer ();

//  ... resume the call, ...
    resume ();

//  ... and return the consumer for further construction:
    return m_pConsumer.operator->() != m_pCaller ? (VCall*)(VComputationUnit*)m_pConsumer : 0;
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VCall::atOrAbove (VCall const* pCall) const {
    while (pCall) {
	if (this == pCall)
	    return true;

	pCall = pCall->caller ()->call ();
    }

    return false;
}

bool VCall::atOrAbove (VTask const* pTask) const {
    while (pTask) {
	VCall const*pTaskCall = pTask->call ();
	if (this == pTaskCall)
	    return true;

	pTask = pTaskCall->caller ();
    }

    return false;
}

bool VCall::atOrAbove (VComputationUnit const* pUnit) const {
    return pUnit->atOrBelow (this);
}

bool VCall::atOrBelow (VCall const* pCall) const {
    return pCall->atOrAbove (this);
}

bool VCall::atOrBelow (VTask const* pTask) const {
    return pTask->atOrAbove (this);
}

bool VCall::atOrBelow (VComputationUnit const* pUnit) const {
    return pUnit->atOrAbove (this);
}

bool VCall::datumAvailable_ () const {
    return m_xRecipient == VMagicWord_Datum && m_xStage == Stage_Entry;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*********************
 *****  General  *****
 *********************/

rtBLOCK_Handle *VCall::boundBlock () const {
    return 0;
}

unsigned int VCall::cardinality_ () const {
    return rtPTOKEN_CPD_BaseElementCount (ptoken ());
}

IOMDriver* VCall::channel_ () const {
    return m_pCaller->channel ();
}

VComputationUnit* VCall::consumer_ () const {
    return consumer ();
}

void VCall::callerDecompilation (VString& rString) const {
    m_pCaller->decompilation (rString, this);
}

void VCall::callerDecompilationPrefix (VString& rString) const {
    m_pCaller->decompilationPrefix (rString, this);
}

void VCall::callerDecompilationSuffix (VString& rString) const {
    m_pCaller->decompilationSuffix (rString, this);
}

unsigned int VCall::callerDecompilationIndent () const {
    return m_pCaller->decompilationIndent (this);
}

unsigned int VCall::callerDecompilationOffset () const {
    return m_pCaller->decompilationOffset (this);
}

void VCall::dumpCallerByteCodes () const {
    m_pCaller->dumpByteCodes (this);
}

M_CPD* VCall::ptoken () const {
    return m_pCallerSubset ? m_pCallerSubset->PPT () : m_pCaller->ptoken ();
}

bool VCall::recipientAvailable_ () const {
    return m_xRecipient != VMagicWord_Datum || m_xStage == Stage_Entry;
}

char const* VCall::recipientSourceName () const {
    return MAGIC_MWIToString (recipientSource ());
}

rtINDEX_Key* VCall::subtaskTemporalContext (rtLINK_CType* pSubset, M_CPD* pReordering) const {
    return m_pTemporalContext ? m_pTemporalContext->NewFutureKey (
	subtaskSubset (pSubset), pReordering
    ) : m_pCaller->temporalContext ()->NewFutureKey (
	pSubset, pReordering
    );
}


/************************
 *****  Datum/Path  *****
 ************************/

bool VCall::getParameter (unsigned int xParameter, VDescriptor& rDatum) {
    if (xParameter < parameterCount ()
	&& m_xStage >= Stage_Entry
	&& !m_iParameterArray[xParameter].isEmpty ()
    ) {
	rDatum.setToCopied (m_iParameterArray[xParameter]);
	return true;
    }

    return false;
}

bool VCall::csearchOrigin (VDescriptor& rDatum) const {
    switch (m_xRecipient) {
    case VMagicWord_Here:
    case VMagicWord_Super:
	rDatum.setToCopied (m_pCaller->getCurrent ());
	return true;

    default:
	return crecipient (rDatum);
    }
}


bool VCall::crecipient (VDescriptor& rDatum) const {
    M_CPD* pPToken = m_pCaller->ptoken ();
    bool fRecipientAvailable = true;

    switch (m_xRecipient) {

    case VMagicWord_Self:
    case VMagicWord_Super:
    case VMagicWord_Here:
    case VMagicWord_Ground:
	rDatum.setToCopied (m_pCaller->getSelf ());
	break;

    case VMagicWord_Current:
	rDatum.setToCopied (m_pCaller->getCurrent ());
	break;

    case VMagicWord_My:
	rDatum.setToCopied (m_pCaller->getMy ());
	break;

    case VMagicWord_Local:
	rDatum.setToCopied (m_pCaller->getLocal ());
	break;

    case VMagicWord_Global:
	rDatum.setToCopied (ENVIR_KDsc_TheTLE);
	rDatum.contentAsMonotype ().coerce (pPToken);
	break;

    case VMagicWord_Tmp:
	rDatum.setToCopied (ENVIR_KDsc_TheTmpTLE);
	rDatum.contentAsMonotype ().coerce (pPToken);
	break;

    case VMagicWord_Date:
	m_pCaller->temporalContext ()->RealizeSequence ();
	rDatum.setToCopied (rtINDEX_Key_Sequence (m_pCaller->temporalContext ()));
	break;

    case VMagicWord_Today:
	ENVIR_KTemporalContext_Today->RealizeSequence ();
	rDatum.setToCopied (rtINDEX_Key_Sequence (ENVIR_KTemporalContext_Today));
	rDatum.contentAsMonotype ().coerce (pPToken);
	break;

    case VMagicWord_Datum:
	if (m_xStage == Stage_Entry)
	    rDatum.setToCopied (m_rDatum);
	else
	    fRecipientAvailable = false;
	break;

    default:
	fRecipientAvailable = false;
	break;
    }

    return fRecipientAvailable;
}

bool VCall::getPathToCaller (
    M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VTask*& rpCaller
) const {
    rpCaller = m_pCaller;
    rpRestriction = m_pCallerSubset;
    rpReordering = 0;

    return true;
}

bool VCall::getPathToConsumer (
    M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpConsumer
) const {
    rpConsumer = m_pConsumer;
    rpRestriction = rpConsumer == m_pCaller ? m_pCallerSubset.operator->() : 0;
    rpReordering = 0;

    return true;
}

bool VCall::getPathToCreator (
    M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpCreator
) const {
    VTask* pCreator;
    if (getPathToCaller (rpReordering, rpRestriction, pCreator)) {
	rpCreator = pCreator;
	return true;
    }
    return false;
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****************
 *****  Run  *****
 *****************/

void VCall::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	switch (m_xRecipient) {

	case VMagicWord_Self: {
		DSC_Descriptor& rSelf = m_pCaller->getSelf ();
		evaluate (rSelf, rSelf);
	    }
	    break;

	case VMagicWord_Current: {
		DSC_Descriptor& rCurrent = m_pCaller->getCurrent ();
		evaluate (rCurrent, rCurrent);
	    }
	    break;

	case VMagicWord_Here:
	    evaluate (m_pCaller->getSelf (), m_pCaller->getCurrent ());
	    break;

	case VMagicWord_Super:
	    evaluate (m_pCaller->getSelf (), m_pCaller->getCurrent (), 1);
	    break;

	case VMagicWord_My:
	/*---->  '^my' is not link equivalent...  <----*/
	    evaluate (m_pCaller->getMy ());
	    break;

	case VMagicWord_Local:
	/*---->  '^local' is not link equivalent...  <----*/
	    evaluate (m_pCaller->getLocal ());
	    break;

	case VMagicWord_Global:
	    evaluate (ENVIR_KDsc_TheTLE, true);
	    break;

	case VMagicWord_Tmp:
	    evaluate (ENVIR_KDsc_TheTmpTLE, true);
	    break;

	case VMagicWord_Date:
	    m_pCaller->temporalContext ()->RealizeSequence ();
	    evaluate (rtINDEX_Key_Sequence (m_pCaller->temporalContext ()));
	    break;

	case VMagicWord_Today:
	    ENVIR_KTemporalContext_Today->RealizeSequence ();
	    evaluate (rtINDEX_Key_Sequence (ENVIR_KTemporalContext_Today), true);
	    break;

	case VMagicWord_Ground:
	    evaluate (m_pCaller->getSelf (), m_pCaller->getCurrent (), UINT_MAX);
	    break;

	case VMagicWord_Datum:
	    evaluate (m_pCallerSubset);
	    break;

	default:
	    raiseException (
		EC__InternalInconsistency,
		"VCall::run: Unrecognized recipient '%s'",
		MAGIC_MWIToString (m_xRecipient)
	    );
	    break;
	}

	if (pausedOnExit ())
	    return;

	/*****  NO BREAK  *****/

    default:
	resumeConsumer ();

	terminate ();
    }
}


/******************
 *****  Fail  *****
 ******************/

void VCall::fail () {
    if (terminatedNot ()) {
	terminate ();

	failConsumer ();
    }
}


/*******************************
 *******************************
 *****  Execution Support  *****
 *******************************
 *******************************/

/*********************************
 *****  Monotype Evaluators  *****
 *********************************/

/*---------------------------------------------------------------------------
 *  Evaluation of a normalized, coerced, link-equivalent monotype.
 *
 *  Note:
 *	The 'fDelegationAllowed' boolean may be overridden after the first
 *	pass through the evaluation loop.  For consistency, if an evaluation
 *	rule delegates the evaluation to a component of the object it is
 *	evaluating, that delegation must be allowed to delegate to Vision
 *	application code if necessary.  Currently, only time-series do this.
 *---------------------------------------------------------------------------
 */
void VCall::evaluate (
    rtLINK_CType*		pSubset,
    M_CPD*			pReordering,
    DSC_Descriptor const&	rEvaluable,
    bool			fDelegationAllowed
)
{
    VTask* pSubtask = 0;

    bool fReorderingRewritten = false;
    bool fDelegationRequired = false;
    bool notDone = true;

    DSC_Descriptor& rMonotype = ducMonotype ();
    if (&rMonotype != &rEvaluable)
	loadDucWithCopied (rEvaluable);

    if (m_xReturnCase != Return_Intension) do {
	RTYPE_Type xMonotypeStoreRType = rMonotype.storeRType ();

	switch (xMonotypeStoreRType) {
	case RTYPE_C_Closure: {
	    //  Decode the closure, ...
		VReference<rtBLOCK_Handle> pBlock; unsigned int xPrimitive; rtCONTEXT_Constructor *pContext;
		rMonotype.decodeClosure (pBlock, xPrimitive, &pContext);

		unsigned int iAttentionMask = rMonotype.storeMask ();

	    //  ... construct a new context if this one can't be used directly, ...
		if (pBlock || !rMonotype.holdsAnIdentity ()) {
		    rMonotype.setStoreTo (pContext);

		    DSC_Descriptor iEmpty;
		    iEmpty.construct ();

		    pContext = new rtCONTEXT_Constructor (iEmpty, iEmpty, iEmpty, rMonotype);
		}

	    //  ... and create the task:
		VTask::ConstructionData iTCData (
		    this, pSubset, pReordering, pContext, iAttentionMask
		);
		if (pBlock) {
		    pSubtask = new VBlockTask (iTCData, pBlock);
		}
		else {
		    pSubtask = VPrimitiveTask::Instantiate (iTCData, xPrimitive);
		}
	    }
	    notDone = false;
	    break;

	case RTYPE_C_Dictionary: {
		M_CPD* pMonotypeStore = rMonotype.storeCPD ();
		rMonotype.setStoreTo (rtDICTIONARY_CPD_ValuesCPD (pMonotypeStore));
	    }
	    break;

	case RTYPE_C_Index: {
		M_CPD* pMonotypeStore = rMonotype.storeCPD ();

		if (!rtINDEX_CPD_IsATimeSeries (pMonotypeStore)) {
		/*****  Return non-time-series without further evaluation, ...  *****/
		    fDelegationRequired = fDelegationAllowed;
		    notDone = false;
		}
		else if (
		    (m_pTemporalContext ? m_pTemporalContext : m_pCaller->temporalContext()
		    )->ConformsToIndex (pMonotypeStore)
		) {
		/*****  ... do the lookup for conformant time-series, ...  *****/
		    rtINDEX_Key* pTemporalContext = subtaskTemporalContext (
			pSubset, pReordering
		    );

		    DSC_Descriptor iLookupResult;
		    rtINDEX_TimeSeriesLookup (
			&iLookupResult, &rMonotype, pTemporalContext
		    );

		/***************************************************************
		 *****  ... index lookup results must be re-factored, ...  *****
		 ***************************************************************/
		    M_CPD* pReorderingReordering = iLookupResult.factor ();
		    if (pReorderingReordering) {
			if (IsNil (pReordering))
			    pReordering = pReorderingReordering;
			else {
			    M_CPD* pRewrittenReordering = rtREFUV_UVExtract (
				pReordering, pReorderingReordering
			    );
			    pReorderingReordering->release ();
			    if (fReorderingRewritten)
				pReordering->release ();
			    pReordering = pRewrittenReordering;
			}
			fReorderingRewritten = true;
		    }
		    loadDucWithMoved (iLookupResult);

		    pTemporalContext->release ();

		/*****  ... and application delegation must be enabled:  *****/
		    fDelegationAllowed = true;
		}
		else {
		/*****  ... and delegate non-conformant time-series to Vision.  *****/
		    VTask::ConstructionData iTCData (this, pSubset, pReordering);
		    pSubtask = new VUtilityTask (iTCData, UTC__ValueForNonConformantKey);
		    notDone = false;
		}
	    }
	    break;

	case RTYPE_C_Vector: {
		M_CPD* pMonotypeStore = rMonotype.storeCPD ();

		if (rMonotype.holdsALink ()) {
		    VDescriptor iResult;
		    rtVECTOR_Extract (
			pMonotypeStore, DSC_Descriptor_Link (rMonotype), iResult
		    );
		    loadDucWithMoved (iResult);
		}
		else if (rMonotype.holdsAScalarReference ()) {
		    DSC_Descriptor iResult;
		    rtVECTOR_RefExtract	(
			pMonotypeStore, &DSC_Descriptor_Scalar (rMonotype), &iResult
		    );
		    loadDucWithMoved (iResult);
		}
		else if (rMonotype.holdsAnIdentity ()) {
		    pMonotypeStore->retain ();
		    loadDucWithVector (pMonotypeStore);
		    ConvertDucVectorsToMonotypeIfPossible ();
		}
		else {
		    VCPDReference pPPT;
		    if (pReordering)
			pPPT.claim (UV_CPD_PosPTokenCPD (pReordering));
		    else if (pSubset)
			pPPT.setTo (pSubset->PPT ());
		    else
			pPPT.setTo (m_pCaller->ptoken ());

		    m_pDuc->setToNA (pPPT, m_pCaller->codKOT ());
		}
	    }
	    notDone = false;
	    break;

	default:
	    fDelegationRequired = fDelegationAllowed;
	    notDone = false;
	    break;
	}
    } while (notDone);

    if (fDelegationRequired) {
	VTask::ConstructionData iTCData (this, pSubset, pReordering);
	start (new VUtilityTask (iTCData, VUtilityTask::Delegate));
    }
    else if (pSubtask)
	start (pSubtask);
    else if (pReordering)
	m_pDuc->distribute (pReordering);

    if (fReorderingRewritten)
	pReordering->release ();
}


/*---------------------------------------------------------------------------
 *  Evaluation using a pair of factored monotypes.
 *---------------------------------------------------------------------------
 */
void VCall::evaluate (
    rtLINK_CType*		pSubset,
    M_CPD*			pReordering,
    DSC_Descriptor const&	rRecipient,
    DSC_Descriptor const&	rSearchOrigin,
    unsigned int		iSkipCount
)
{
    switch (m_xType) {
    default:
	raiseException (
	    EC__InternalInconsistency,
	    "VCall::evaluate: Unrecognized Call Type %u", m_xType
	);
	break;

    case Type_Bound: {
	    DSC_Descriptor iSelf;
	    iSelf.construct (rRecipient);

	    DSC_Descriptor iCurrent;
	    iCurrent.construct (rSearchOrigin);

	    DSC_Descriptor iMy;
	    iMy.construct (iSelf);

	    VTask::ConstructionData iTCData (
		this, pSubset, pReordering, new rtCONTEXT_Constructor (
		    iSelf, iCurrent, iMy
		)
	    );
	    start (new VBlockTask (iTCData, boundBlock (), true));
	}
	break;

    case Type_Evaluation:
	evaluate (pSubset, pReordering, rSearchOrigin, g_bSendingValueMessages);
	break;


    case Type_Message: {
	g_cMessages++;

	/*****  Cache the selector being bound, ...  *****/
	    VSelector const& rSelector = selector_ ();

	/*****  ... and the skip count, ...  *****/
	    unsigned int const iInitialSkipCount = iSkipCount;

	/*****  ... obtain the search origin's PPT, ...  *****/
	    VCPDReference pDispatchPToken;
	    if (pReordering)
		pDispatchPToken.claim (UV_CPD_PosPTokenCPD (pReordering));
	    else if (pSubset)
		pDispatchPToken.setTo (pSubset->PPT ());
	    else
		pDispatchPToken.setTo (m_pCaller->ptoken ());

	/*****  ... initialize the search cursor, ... *****/
	    VSteppable iCurrent (pDispatchPToken, rSearchOrigin);

	/*****  ... and for each level of inheritance, ...  *****/
	    rtDICTIONARY_LookupResult xSelectorLookupResult;
	    do {
	    /*****  ... access the store to search, ...  *****/
		M_CPD *pCanonicalStore = iCurrent.canonicalStore ();

	    /*****  ... look for the selector, ...  *****/
		DSC_Descriptor iSelectorBehavior;
		xSelectorLookupResult = rSelector.lookup (
		    pCanonicalStore, &iSelectorBehavior
		);

	    /*****  ... handling 'super' as though all dictionaries define it, ...  *****/
		if (rtDICTIONARY_LookupResult_FoundNothing == xSelectorLookupResult && rSelector == KS__Super) {
		    iSelectorBehavior.constructScalar (
			m_pCaller->codKOT()->ThePrimitiveClass, (int)V_KnownPrimitive_Super
		    );
		    xSelectorLookupResult = rtDICTIONARY_LookupResult_FoundOther;
		}

	    /*****  ... ignoring finds if the skip count is still positive, ...  *****/
		if (iSkipCount > 0) {
		    switch (xSelectorLookupResult) {
		    case rtDICTIONARY_LookupResult_FoundProperty:
		    case rtDICTIONARY_LookupResult_FoundOther:
			iSelectorBehavior.clear ();
			xSelectorLookupResult = rtDICTIONARY_LookupResult_FoundNothing;
			iSkipCount--;
			break;

		    default:
			break;
		    }
		}

	    /*****  ... and processing found selectors:  *****/
		M_CPD *pSelectorBehavior;
		switch (xSelectorLookupResult) {
		case rtDICTIONARY_LookupResult_FoundProperty:
		    pSelectorBehavior = iSelectorBehavior.storeCPD (); {
			M_CPD* pProperty;
			if (pCanonicalStore->ReferenceIsNil (rtVSTORE_CPx_Column))
			    pProperty = NilOf (M_CPD*);
			else if (
			    !rtVSTORE_AreBehavioriallyEquivalent (
				pProperty = rtVSTORE_CPD_ColumnCPD (pCanonicalStore), pSelectorBehavior
			    )
			) {
			    pProperty->release ();
			    pProperty = NilOf (M_CPD*);
			}

		//  Create a new property store of the right type if one does not exist...
			if (IsNil (pProperty)) {
			    M_CPD* pCanonicalStorePToken = rtVSTORE_CPD_RowPTokenCPD (
				pCanonicalStore
			    );
			    pProperty = rtVSTORE_NewCluster (
				pCanonicalStorePToken, pSelectorBehavior
			    );
			    pCanonicalStore->StoreReference (rtVSTORE_CPx_Column, pProperty);
			    pCanonicalStorePToken->release ();
			}

		//  ... construct a descriptor for the property, ...
			iCurrent.setStoreTo (pProperty);

		//  ...  and evaluate the result (note the delegation):
			evaluate (pSubset, pReordering, iCurrent, true);
		    }
		    iSelectorBehavior.clear ();
		    break;

		case rtDICTIONARY_LookupResult_FoundOther:
		    pSelectorBehavior = iSelectorBehavior.storeCPD ();
		    if (RTYPE_C_Method == pSelectorBehavior->RType ()) {
		    //  Retain the method CPD for use in creating the closure, ...
			pSelectorBehavior->retain ();

		    //  Create a block context for the method, ...
			rtCONTEXT_Constructor* pContext; {
			    DSC_Descriptor iSelf;
			    iSelf.construct (rRecipient);

			    iSelectorBehavior.coerce (pDispatchPToken);
			    iSelectorBehavior.setStoreTo (
				rtMETHOD_CPD_OriginCPD (pSelectorBehavior)
			    );

			    pContext = new rtCONTEXT_Constructor (iSelf, iCurrent, iSelectorBehavior);
			}

		    //  ... and evaluate the method, ...
			if (Return_Intension == m_xReturnCase) {
			    pDispatchPToken->retain ();

			    DSC_Descriptor iClosure;
			    iClosure.constructIdentity (
				new rtCLOSURE_Constructor (
				    pContext, static_cast<rtBLOCK_Handle*> (
					pSelectorBehavior->GetContainerHandle (rtMETHOD_CPx_Block, RTYPE_C_Block)
				    ), pSelectorBehavior->attentionMask ()
				), pDispatchPToken
			    );

			    loadDucWithMoved (iClosure);

			    if (pReordering)
				m_pDuc->distribute (pReordering);
			}
			else {
			    VTask::ConstructionData iTCData (
				this, pSubset, pReordering, pContext,
				pSelectorBehavior->attentionMask ()
			    );
			    start (
				new VBlockTask (
				    iTCData, static_cast<rtBLOCK_Handle*>(
					pSelectorBehavior->GetContainerHandle (rtMETHOD_CPx_Block, RTYPE_C_Block)
				    )
				)
			    );
			    pSelectorBehavior->release ();
			}
		    }
		    else if (pSelectorBehavior->NamesThePrimitiveClass ()) {
		    //  Obtain the primitive's index, ...
			int xPrimitive = DSC_Descriptor_Scalar_Int (iSelectorBehavior);

		    //  Create a block context for the primitive, ...
			rtCONTEXT_Constructor* pContext; {
			    DSC_Descriptor iSelf;
			    iSelf.construct (rRecipient);

			    DSC_Descriptor iEmpty;
			    iEmpty.construct ();

			    pContext = new rtCONTEXT_Constructor (iSelf, iCurrent, iEmpty, iEmpty);
			}

		    //  ... and evaluate the primitive:
			if (Return_Intension == m_xReturnCase) {
			    pDispatchPToken->retain ();

			    DSC_Descriptor iClosure;
			    iClosure.constructIdentity (
				new rtCLOSURE_Constructor (
				    pContext, xPrimitive, pSelectorBehavior->attentionMask ()
				), pDispatchPToken
			    );

			    loadDucWithMoved (iClosure);

			    if (pReordering)
				m_pDuc->distribute (pReordering);
			}
			else {
			    VTask::ConstructionData iTCData (
				this, pSubset, pReordering, pContext,
				pSelectorBehavior->attentionMask ()
			    );
			    start (VPrimitiveTask::Instantiate (iTCData, xPrimitive));
			}
		    }
		    else {
		    //  Otherwise, coerce, ...
			iSelectorBehavior.coerce (pDispatchPToken);

		    //  ... and evaluate the selector definition as a class constant:
			evaluate (pSubset, pReordering, iSelectorBehavior, g_bSendingValueMessages);
		    }
		    iSelectorBehavior.clear ();
		    break;

		default:
		//  Selector not found at this level, check the inheritance, ...
		    if (pCanonicalStore->ReferenceIsntNil (rtVSTORE_CPx_InheritanceStore))
			iCurrent.step ();
		//  ...  or, if no more remain to be searched, fail with a selector not found:
		    else {
			xSelectorLookupResult = rtDICTIONARY_LookupResult_FoundOther;

			rtCONTEXT_Constructor* pContext; {
			    DSC_Descriptor iSelf;
			    iSelf.construct (rRecipient);

			    DSC_Descriptor iOrigin;
			    iOrigin.construct (rSearchOrigin);

			    pContext = new rtCONTEXT_Constructor (iSelf, iCurrent, iOrigin);
			}

			VTask::ConstructionData iTCData (this, pSubset, pReordering, pContext);
			start (new VSNFTask (iTCData, iInitialSkipCount));
		    }
		    break;
		}
	    } while (rtDICTIONARY_LookupResult_FoundNothing == xSelectorLookupResult);
	}
	break;
    }
}


/*---------------------------------------------------------------------------
 *  General evaluation of a single, un-factored monotype.
 *---------------------------------------------------------------------------
 */
void VCall::evaluate (rtLINK_CType* pSubset, DSC_Descriptor const& rRecipient) {
    DSC_Descriptor iRecipient;
    iRecipient.construct (rRecipient);

    M_CPD* pReordering = iRecipient.factor ();

    evaluate (pSubset, pReordering, iRecipient, iRecipient);

    if (pReordering)
	pReordering->release ();

    iRecipient.clear ();
}


/***************************************
 *****  Magic Monotype Evaluators  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *  Evaluation using an un-factored and possibly un-coerced recipient monotype.
 *  'm_pCallerSubset' MUST be nil.
 *---------------------------------------------------------------------------
*/
void VCall::evaluate (DSC_Descriptor const& rRecipient, bool fCoercionRequired) {
    if (m_pCaller->isScalar ())
	evaluate (0, 0, rRecipient, rRecipient);
    else if (!fCoercionRequired)
	evaluate (0, rRecipient);
    else {
	DSC_Descriptor iRecipient;
	iRecipient.construct (rRecipient);
	iRecipient.coerce (m_pCaller->ptoken ());

	//  Coerced monotypes are factored, so this is safe:
	evaluate (0, 0, iRecipient, iRecipient);

	iRecipient.clear ();
    }
}


/*******************************
 *****  General Evaluator  *****
 *******************************/

void VCall::evaluate (rtLINK_CType* pSubset) {
    //  Evaluate...
    if (ducIsAMonotype ()) {
	DSC_Descriptor const& rMonotype = ducMonotype ();
	//  ... scalar monotypes, ...
	if (IsNil (pSubset) && m_pCaller->isScalar ()) {
	    //  ... as scalars:
	    evaluate (0, 0, rMonotype, rMonotype);
	    return;
	}

	//  ... non-expanding and small monotypes, ...
	if (IsNil (pSubset) || !m_pDuc->convertMonotypeToFragmentation (largeTaskSize ())) {
	    //  ... as monotypes:
	    normalizeDuc ();
	    evaluate (pSubset, rMonotype);
	    return;
	}
    }

//  ... and everything else as polytypes, ...
#if 0
    M_CPD* pReordering = m_pDuc->claimedDistribution ();
    if (pSubset || pReordering) {
	VTask::ConstructionData iTCData (this, pSubset, pReordering);
	start (new VUtilityTask (iTCData, VUtilityTask::Bridge));
	if (pReordering)
	    pReordering->release ();
    }
    else {
	VFragmentation& rFragmentation = convertDucToFragmentation ();

	VFragment* pFragment;
	rFragmentation.goToFirstFragment ();
	while (pFragment = rFragmentation.currentFragment ()) {
	    m_pDuc = &pFragment->datum ();
	    evaluate (pFragment->subset ());
	    rFragmentation.goToNextFragment ();
	}
	m_pDuc = &m_rDatum;
    }
#else
    if (pSubset) {
	VTask::ConstructionData iTCData (this, pSubset, NilOf (M_CPD*));
	start (new VUtilityTask (iTCData, VUtilityTask::Bridge));
    }
    else {
	VFragmentation& rFragmentation = convertDucToFragmentation ();
	normalizeDuc ();

	VFragment* pFragment;
	rFragmentation.goToFirstFragment ();
	while (pFragment = rFragmentation.currentFragment ()) {
	    m_pDuc = &pFragment->datum ();
	    evaluate (pFragment->subset ());
	    rFragmentation.goToNextFragment ();
	}
	m_pDuc = &m_rDatum;
    }
#endif
}


/**********************
 **********************
 *****            *****
 *****  rtVSTORE  *****
 *****            *****
 **********************
 **********************/

/******************************
 *****  Selector Lookup   *****
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to look for a selector in a value store.
 *
 *  Arguments:
 *	pVStoreCPD		- the address of a CPD for the value store.  If a property
 *				  was found, the column pointer of this store will be set
 *				  to slot associated with the property.
 *	pSelector		- the address of a 'VSelector'.
 *	pValueReturn		- an optional (Nil to omit) address of a descriptor which
 *				  will be initialized to either the value of the selector
 *				  for non-properties or to the property prototype for
 *				  properties.
 *
 *  Returns:
 *	The appropriate rtDICTIONARY_LookupResult enumeration value.
 *
 *****/
PublicFnDef rtDICTIONARY_LookupResult rtVSTORE_Lookup (
    M_CPD* pVStoreCPD, VSelector const* pSelector, DSC_Descriptor* pValueReturn
)
{
//    LookupCount++;

    M_CPD* pDictionaryCPD = rtVSTORE_CPD_DictionaryCPD (pVStoreCPD);

    int propertySlotOffset;
    rtDICTIONARY_LookupResult xLookupResult = rtDICTIONARY_Lookup (
	pDictionaryCPD, pSelector, pValueReturn, &propertySlotOffset
    );

    if (rtDICTIONARY_LookupResult_FoundProperty == xLookupResult)
    {
	rtVSTORE_Align (pVStoreCPD);
	rtVSTORE_CPD_Column (pVStoreCPD) = rtVSTORE_CPD_ColumnArray (
	    pVStoreCPD
	) + propertySlotOffset;
    }

    pDictionaryCPD->release ();

    return xLookupResult;
}


/**************************
 **************************
 *****                *****
 *****  rtDICTIONARY  *****
 *****                *****
 **************************
 **************************/

/*****************************
 *****************************
 *****  Link Extensions  *****
 *****************************
 *****************************/

inline unsigned int rtLINK_RRD_ReferenceLimit (
    rtLINK_RRDType const *pRRD, rtLINK_RRDType const *pSuccessorRRD
) {
    return rtLINK_RRD_RepeatedRef (
	pRRD
    ) ? rtLINK_RRD_ReferenceOrigin (pRRD) : rtLINK_RRD_ReferenceOrigin (
	pRRD
    ) + rtLINK_RRD_N (pRRD, rtLINK_RRD_LinkCumulative (pSuccessorRRD)) - 1;
}

inline unsigned int rtLINK_RRD_ReferenceLimit (rtLINK_RRDType const *pRRD) {
    return rtLINK_RRD_ReferenceLimit (pRRD, pRRD + 1);
}

PrivateFnDef int __cdecl CompareRRDReferences (
    rtLINK_RRDType const *pRRD1, rtLINK_RRDType const *pRRD2
) {
    return rtLINK_RRD_ReferenceLimit (pRRD1) < rtLINK_RRD_ReferenceOrigin (pRRD2)
    ? -1 : rtLINK_RRD_ReferenceLimit (pRRD2) < rtLINK_RRD_ReferenceOrigin (pRRD1)
    ?  1 : 0;
}

PublicFnDef bool rtLINK_Locate (
    M_CPD *pLink, unsigned int xCoDomainReference, rtREFUV_Type_Reference &iDomainReference
) {
/*****  Align the link, ...  *****/
    rtLINK_AlignLink (pLink);

/*****  ... build the key, ...  *****/
    rtLINK_RRDType  iKey[2];
    rtLINK_RRDType* pResult;

    rtLINK_RRD_D_LinkCumulative (iKey[0]) = 0;
    rtLINK_RRD_D_RepeatedRef	(iKey[0]) = false;
    rtLINK_RRD_D_RefOrigin	(iKey[0]) = xCoDomainReference;
    rtLINK_RRD_D_LinkCumulative	(iKey[1]) = 1;

/*****  ... run the query, ...  *****/
    if (IsNil (
	    pResult = (rtLINK_RRDType*)bsearch (
		(void *)iKey,
		(void *)rtLINK_CPD_RRDArray (pLink),
		(size_t)rtLINK_CPD_RRDCount (pLink),
		sizeof (rtLINK_RRDType),
		(VkComparator)CompareRRDReferences
	    )
	)
    ) return false;

/*****  ... and decode a successful query, ...  *****/
    DSC_InitReferenceScalar (
	iDomainReference,
	rtLINK_CPD_PosPTokenCPD (pLink),
	xCoDomainReference - rtLINK_RRD_ReferenceOrigin (
	    pResult
	) + rtLINK_RRD_LinkCumulative (pResult)
    );

    return true;
}


/*---------------------------------------------------------------------------
 *****  Routine to extract a method for the given selector
 *
 *  Arguments:
 *	pDictionaryCPD		- the address of a CPD for the dictionary.
 *	pSelector		- the address of a 'VSelector'.
 *	pValueReturn		- an optional (Nil to omit) address of a descriptor which
 *				  will be initialized to either the value of the selector
 *				  for non-properties or to the property prototype for
 *				  properties.
 *	pOffsetReturn		- an optional (Nil to omit) address which will be set to
 *				  the property slot offset when this routine returns
 *				  rtDICTIONARY_LookupResult_FoundProperty.
 *
 *  Returns:
 *	The appropriate rtDICTIONARY_LookupResult enumeration value.
 *
 *****/
PublicFnDef rtDICTIONARY_LookupResult rtDICTIONARY_Lookup (
    M_CPD* pDictionaryCPD, VSelector const* pSelector, DSC_Descriptor* pValueReturn, int* pOffsetReturn
)
{
    rtDICTIONARY_LookupResult xLookupResult;

    if (rtDICTIONARY_UsingCache)
	xLookupResult = rtDICTIONARY_Lookup (
	    M_CPD_CPCC (pDictionaryCPD), pSelector, pValueReturn, pOffsetReturn
	);
    else {
	rtSELUV_Set iSet (pDictionaryCPD, rtDICTIONARY_CPx_Selectors);

	unsigned int xSelectorReference;
	if (pSelector->locateIn (iSet, xSelectorReference)) {
	    M_CPD* pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD (
		pDictionaryCPD
	    );

	    DSC_Scalar propertyReference;
	    if (rtLINK_Locate (pPropertySubsetCPD, xSelectorReference, propertyReference)) {
		xLookupResult = rtDICTIONARY_LookupResult_FoundProperty;

		if (IsntNil (pValueReturn)) {
		    M_CPD* pPrototypeVectorCPD = rtDICTIONARY_CPD_PropertyPrototypesCPD (
			pDictionaryCPD
		    );
		    rtVECTOR_RefExtract (pPrototypeVectorCPD, &propertyReference, pValueReturn);
		    pPrototypeVectorCPD->release ();
		}

		if (IsntNil (pOffsetReturn))
		    *pOffsetReturn = DSC_Scalar_Int (propertyReference);

		DSC_ClearScalar (propertyReference);
	    }
	    else if (IsntNil (pValueReturn)) {
	    /*****  ... for everything else:  *****/
		xLookupResult = rtDICTIONARY_LookupResult_FoundOther;

		DSC_Scalar iSelectorReference;
		M_CPD* pSetPToken = iSet.ptoken ();
		pSetPToken->retain ();
		DSC_InitReferenceScalar (iSelectorReference, pSetPToken, xSelectorReference);

		M_CPD* pSelectorValuesCPD = rtDICTIONARY_CPD_ValuesCPD (pDictionaryCPD);
		rtVECTOR_RefExtract (pSelectorValuesCPD, &iSelectorReference, pValueReturn);

		pSelectorValuesCPD->release ();

		DSC_ClearScalar (iSelectorReference);
	    }
	    else {
		xLookupResult = rtDICTIONARY_LookupResult_FoundOther;
	    }
	    pPropertySubsetCPD->release ();
	}
	else xLookupResult = rtDICTIONARY_LookupResult_FoundNothing;
    }

    return xLookupResult;
}


/*---------------------------------------------------------------------------
 *****  Routine to extract a method for the given selector
 *
 *  Arguments:
 *	pDictionaryCPD		- the address of a CPD for the dictionary.
 *	pSelector		- the address of a 'VSelector'.
 *	pValueReturn		- an optional (Nil to omit) address of a descriptor which
 *				  will be initialized to either the value of the selector
 *				  for non-properties or to the property prototype for
 *				  properties.
 *	pOffsetReturn		- an optional (Nil to omit) address which will be set to
 *				  the property slot offset when this routine returns
 *				  rtDICTIONARY_LookupResult_FoundProperty.
 *
 *  Returns:
 *	The appropriate rtDICTIONARY_LookupResult enumeration value.
 *
 *****/
PublicFnDef rtDICTIONARY_LookupResult rtDICTIONARY_Lookup (
    VContainerHandle* pDictionary, VSelector const* pSelector, DSC_Descriptor* pValueReturn, int* pOffsetReturn
) {
    VReference<rtDICTIONARY_Cache> pCache (rtDICTIONARY_Cache::GetCacheOf (pDictionary));
    rtSELUV_Set &rSet = pCache->selectors ();

    rtDICTIONARY_LookupResult	xLookupResult;
    unsigned int		xSelectorReference;
    if (pSelector->locateIn (rSet, xSelectorReference)) {
	M_CPD* pPropertySubsetCPD = pCache->propertySubset ();

	DSC_Scalar propertyReference;
	if (rtLINK_Locate (pPropertySubsetCPD, xSelectorReference, propertyReference)) {
	    xLookupResult = rtDICTIONARY_LookupResult_FoundProperty;

	    if (IsntNil (pValueReturn)) {
		rtVECTOR_RefExtract (
		    pCache->propertyPrototypes (), &propertyReference, pValueReturn
		);
	    }

	    if (IsntNil (pOffsetReturn))
		*pOffsetReturn = DSC_Scalar_Int (propertyReference);

	    DSC_ClearScalar (propertyReference);
	}
	else if (IsntNil (pValueReturn)) {
	/*****  ... for everything else:  *****/
	    xLookupResult = rtDICTIONARY_LookupResult_FoundOther;

	    DSC_Scalar iSelectorReference;
	    M_CPD* pSetPToken = rSet.ptoken ();
	    pSetPToken->retain ();
	    DSC_InitReferenceScalar (iSelectorReference, pSetPToken, xSelectorReference);

	    rtVECTOR_RefExtract (pCache->bindings (), &iSelectorReference, pValueReturn);

	    DSC_ClearScalar (iSelectorReference);
	}
	else xLookupResult = rtDICTIONARY_LookupResult_FoundOther;
    }
    else xLookupResult = rtDICTIONARY_LookupResult_FoundNothing;

    return xLookupResult;
}


/***********************
 ***********************
 *****             *****
 *****  VSelector  *****
 *****             *****
 ***********************
 ***********************/

bool VSelector::locateIn (rtSELUV_Set &rSet, unsigned int& rxElement) const {
    switch (m_xType) {
    case Known:
    case Value:
	return rSet.Locate (m_iValue.as_xMessageName, rxElement);
    case Block:
	return rSet.Locate (
	    m_iValue.as_iBlockMessage.messageName (), rxElement
	);
    case String:
	return rSet.Locate (m_iValue.as_pMessageName, rxElement);
    default:
	raiseSelectorTypeException ();
	break;
    }

    return false;
}


/*********************
 *********************
 *****           *****
 *****  rtSELUV  *****
 *****           *****
 *********************
 *********************/

/*---------------------------------------------------------------------------
 *****  Search comparison utility.
 *
 *  Arguments:
 *	xElement	- the index of the set element to test, relative to
 *			  the current element.
 *	[px]Selector	- the selector being sought.
 *
 *  Returns:
 *	<  0	if m_pElement[xElement] <  the selector being sought,
 *	== 0	if m_pElement[xElement] == the selector being sought,
 *	>  0	if m_pElement[xElement] >  the selector being sought.
 *
 *****/
int rtSELUV_Set::compare (unsigned int xElement, char const* pSelector)
{
    rtSELUV_Type_Element const* pElement = rtSELUV_CPD_Array (m_pSetCPD) + xElement;
    int iComparisonResult = rtSELUV_Element_Type (pElement) - SELECTOR_C_DefinedSelector;

    accessStringSpaceIfNecessary ();
    return iComparisonResult ? iComparisonResult : strcmp (
	m_pStringStorage + rtSELUV_Element_Index (pElement), pSelector
    );
}

int rtSELUV_Set::compare (unsigned int xElement, unsigned int xSelector) {
    rtSELUV_Type_Element const* pElement = rtSELUV_CPD_Array (m_pSetCPD) + xElement;
    int iComparisonResult = rtSELUV_Element_Type (pElement) - SELECTOR_C_KnownSelector;

    return iComparisonResult
	? iComparisonResult
	: (int)rtSELUV_Element_Index (pElement) - (int)xSelector;
}

bool rtSELUV_Set::Locate (char const* pSelector, unsigned int& rxElement) {
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Locate (pSelector, rxElement);
}

bool rtSELUV_Set::Locate (unsigned int xSelector, unsigned int& rxElement) {
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    return iSetCursor.Locate (xSelector, rxElement);
}

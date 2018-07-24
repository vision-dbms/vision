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

#include "VAssociativeCursor.h"

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
    VSteppable (rtPTOKEN_Handle *pDPT, DSC_Descriptor const &rValue) : m_pDPT (pDPT), m_bPointerIsNil (false), m_cSteppings (0) {
	m_iValue.construct (rValue);
	m_iValue.getCanonicalStore (m_pCanonicalStore, m_bPointerMustBeConverted);
    }

//  Destruction
public:
    ~VSteppable () {
	if (m_cSteppings) {
	    unsigned int sSteppings = m_cSteppings * m_pDPT->cardinality ();
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

    rtVSTORE_Handle *canonicalStore () const {
	return m_pCanonicalStore;
    }

    bool isSteppable () const {
	return m_pCanonicalStore->hasAnInheritance ();
    }

    bool getProperty (unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype) {
	normalize ();
	return m_iValue.getProperty (xPropertySlot, pPropertyPrototype);
    }

    Vdd::Store::DictionaryLookup lookup (
	VSelector const &rSelector, unsigned int &rxPropertySlot, DSC_Descriptor *pValueReturn = 0
    ) {
	return m_pCanonicalStore->lookup (rSelector, rxPropertySlot, pValueReturn);
    }

//  Update
private:
    void normalize ();
public:
    void step ();

//  State
private:
    rtPTOKEN_Handle::Reference const	m_pDPT;
    DSC_Descriptor			m_iValue;
    Step::Reference			m_pSteppingsHead;
    Step::Reference			m_pSteppingsTail;
    unsigned int			m_cSteppings;
    rtVSTORE_Handle::Reference		m_pCanonicalStore;
    bool				m_bPointerMustBeConverted;
    bool				m_bPointerIsNil;
};

DEFINE_CONCRETE_RTT (VSteppable::Step);

void VSteppable::normalize () {
    if (m_bPointerIsNil) {
	rtPTOKEN_Handle *pRPT = m_iValue.RPT ();
	m_iValue.Pointer().constructReferenceConstant (m_pDPT, pRPT, pRPT->cardinality ());
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
	Vdd::Store::Reference pInheritanceStore; M_CPD *pInheritancePointer;
	m_pCanonicalStore->getInheritanceStore (pInheritanceStore);
	m_pCanonicalStore->getInheritancePointer (pInheritancePointer);
	iInheritance.constructMonotype (pInheritanceStore, pInheritancePointer);
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
template class Vsi_getr_c<VCall, V::VString>;
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
    return ptoken ()->cardinality ();
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

rtPTOKEN_Handle *VCall::ptoken () const {
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
    rtPTOKEN_Handle * pPToken = m_pCaller->ptoken ();
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
		rtBLOCK_Handle::Reference pBlock; unsigned int xPrimitive; rtCONTEXT_Handle::Reference pContext;
		rMonotype.decodeClosure (pBlock, xPrimitive, &pContext);

		unsigned int iAttentionMask = rMonotype.storeMask ();

	    //  ... construct a new context if this one can't be used directly, ...
		if (pBlock || !rMonotype.holdsAnIdentity ()) {
		    pContext.setTo (new rtCONTEXT_Handle (pContext, rMonotype.Pointer ()));
		}

	    //  ... and create the task:
		VTask::ConstructionData iTCData (this, pSubset, pReordering, pContext, iAttentionMask);
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
		rtVECTOR_Handle::Reference pResultStore;
		static_cast<rtDICTIONARY_Handle*>(rMonotype.store ())->getValues (pResultStore);
		rMonotype.setStoreTo (pResultStore);
	    }
	    break;

	case RTYPE_C_Index: {
		rtINDEX_Handle::Reference pMonotypeStore (
		    static_cast<rtINDEX_Handle*>(rMonotype.store ())
		);

		if (!pMonotypeStore->isATimeSeries ()) {
		/*****  Return non-time-series without further evaluation, ...  *****/
		    fDelegationRequired = fDelegationAllowed;
		    notDone = false;
		}
		else if (
		    (m_pTemporalContext ? m_pTemporalContext : m_pCaller->temporalContext())->ConformsToIndex (
			pMonotypeStore
		    )
		) {
		/*****  ... do the lookup for conformant time-series, ...  *****/
		    rtINDEX_Key *pTemporalContext = subtaskTemporalContext (pSubset, pReordering);

		    DSC_Descriptor iLookupResult;
		    rtINDEX_TimeSeriesLookup (&iLookupResult, &rMonotype, pTemporalContext);

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
		rtVECTOR_Handle::Reference pMonotypeStore (static_cast<rtVECTOR_Handle*> (rMonotype.store ()));

		if (rMonotype.holdsALink ()) {
		    VDescriptor iResult;
		    pMonotypeStore->getElements (iResult, DSC_Descriptor_Link (rMonotype));
		    loadDucWithMoved (iResult);
		}
		else if (rMonotype.holdsAScalarReference ()) {
		    DSC_Descriptor iResult;
		    pMonotypeStore->getElements (iResult, DSC_Descriptor_Scalar (rMonotype));
		    loadDucWithMoved (iResult);
		}
		else if (rMonotype.holdsAnIdentity ()) {
		    loadDucWithVector (pMonotypeStore);
		    ConvertDucVectorsToMonotypeIfPossible ();
		}
		else {
		    rtPTOKEN_Handle::Reference pPPT;
		    if (pReordering)
			pPPT.setTo (static_cast<rtUVECTOR_Handle*>(pReordering->containerHandle ())->pptHandle ());
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
		this, pSubset, pReordering, new rtCONTEXT_Handle (iSelf, iCurrent, iMy)
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
	    rtPTOKEN_Handle::Reference pDispatchPToken;
	    if (pReordering)
		pDispatchPToken.setTo (static_cast<rtUVECTOR_Handle*>(pReordering->containerHandle ())->pptHandle ());
	    else if (pSubset)
		pDispatchPToken.setTo (pSubset->PPT ());
	    else
		pDispatchPToken.setTo (m_pCaller->ptoken ());

	/*****  ... initialize the search cursor, ... *****/
	    VSteppable iCurrent (pDispatchPToken, rSearchOrigin);

	/*****  ... and for each level of inheritance, ...  *****/
	    Vdd::Store::DictionaryLookup xSelectorLookupResult;
	    do {
	    /*****  Look for the selector, ...  *****/
		unsigned int xPropertySlot; DSC_Descriptor iSelectorBehavior;
		xSelectorLookupResult = iCurrent.lookup (rSelector, xPropertySlot, &iSelectorBehavior);

	    /*****  ... handling 'super' as though all dictionaries define it, ...  *****/
		if (Vdd::Store::DictionaryLookup_FoundNothing == xSelectorLookupResult && rSelector == KS__Super) {
		    iSelectorBehavior.constructScalar (
			m_pCaller->codKOT()->ThePrimitiveClass, (int)V_KnownPrimitive_Super
		    );
		    xSelectorLookupResult = Vdd::Store::DictionaryLookup_FoundOther;
		}

	    /*****  ... ignoring finds if the skip count is still positive, ...  *****/
		if (iSkipCount > 0) {
		    switch (xSelectorLookupResult) {
		    case Vdd::Store::DictionaryLookup_FoundProperty:
		    case Vdd::Store::DictionaryLookup_FoundOther:
			iSelectorBehavior.clear ();
			xSelectorLookupResult = Vdd::Store::DictionaryLookup_FoundNothing;
			iSkipCount--;
			break;

		    default:
			break;
		    }
		}

	    /*****  ... and processing found selectors:  *****/
		switch (xSelectorLookupResult) {
		case Vdd::Store::DictionaryLookup_FoundProperty:
		    iCurrent.getProperty (xPropertySlot, iSelectorBehavior.store ());
		    evaluate (pSubset, pReordering, iCurrent, true);
		    iSelectorBehavior.clear ();
		    break;

		case Vdd::Store::DictionaryLookup_FoundOther: {
			Vdd::Store::Reference pSelectorBehavior (iSelectorBehavior.store ());
			if (RTYPE_C_Method == pSelectorBehavior->rtype ()) {
			//  Retain the method CPD for use in creating the closure, ...
			    rtMETHOD_Handle::Reference pMethod (
				static_cast<rtMETHOD_Handle*>(pSelectorBehavior.referent ())
			    );

			//  Create a block context for the method, ...
			    rtCONTEXT_Handle::Reference pContext; {
				DSC_Descriptor iSelf;
				iSelf.construct (rRecipient);

				iSelectorBehavior.coerce (pDispatchPToken);
				Vdd::Store::Reference pMyStore;
				pMethod->getMyStore (pMyStore);
				iSelectorBehavior.setStoreTo (pMyStore);

				pContext.setTo (new rtCONTEXT_Handle (iSelf, iCurrent, iSelectorBehavior));
			    }

			//  ... and evaluate the method, ...
			    rtBLOCK_Handle::Reference pBlock;
			    pMethod->getBlock (pBlock);
			    if (Return_Intension == m_xReturnCase) {
				DSC_Descriptor iClosure;
				iClosure.constructIdentity (
				    new rtCLOSURE_Handle (
					pContext, pBlock, pMethod->attentionMask ()
				    ), pDispatchPToken
				);

				loadDucWithMoved (iClosure);

				if (pReordering)
				    m_pDuc->distribute (pReordering);
			    }
			    else {
				VTask::ConstructionData iTCData (
				    this, pSubset, pReordering, pContext, pMethod->attentionMask ()
				);
				start (new VBlockTask (iTCData, pBlock));
			    }
			}
			else if (pSelectorBehavior->NamesThePrimitiveClass ()) {
			//  Obtain the primitive's index, ...
			    int xPrimitive = DSC_Descriptor_Scalar_Int (iSelectorBehavior);

			//  Create a block context for the primitive, ...
			    rtCONTEXT_Handle::Reference pContext; {
				DSC_Descriptor iSelf;
				iSelf.construct (rRecipient);

				DSC_Descriptor iEmpty;
				iEmpty.construct ();

				pContext.setTo (new rtCONTEXT_Handle (iSelf, iCurrent, iEmpty));
			    }

			//  ... and evaluate the primitive:
			    if (Return_Intension == m_xReturnCase) {
				DSC_Descriptor iClosure;
				iClosure.constructIdentity (
				    new rtCLOSURE_Handle (
					pContext, xPrimitive, pSelectorBehavior->attentionMask ()
				    ), pDispatchPToken
				);

				loadDucWithMoved (iClosure);

				if (pReordering)
				    m_pDuc->distribute (pReordering);
			    }
			    else {
				VTask::ConstructionData iTCData (
				    this, pSubset, pReordering, pContext, pSelectorBehavior->attentionMask ()
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
		    }
		    iSelectorBehavior.clear ();
		    break;

		default:
		//  Selector not found at this level, check the inheritance, ...
		    if (iCurrent.isSteppable ())
			iCurrent.step ();
		    else {
			xSelectorLookupResult = Vdd::Store::DictionaryLookup_FoundOther;

			rtCONTEXT_Handle::Reference pContext; {
			    DSC_Descriptor iSelf;
			    iSelf.construct (rRecipient);

			    DSC_Descriptor iOrigin;
			    iOrigin.construct (rSearchOrigin);

			    pContext.setTo (new rtCONTEXT_Handle (iSelf, iCurrent, iOrigin));
			}

			VTask::ConstructionData iTCData (this, pSubset, pReordering, pContext);
			start (new VSNFTask (iTCData, iInitialSkipCount));
		    }
		    break;
		}
	    } while (Vdd::Store::DictionaryLookup_FoundNothing == xSelectorLookupResult);
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
 *****  Routine to obtain a selector's dictionary definition.
 *
 *  Arguments:
 *	rSelector		- the address of a 'VSelector'.
 *	xPropertySlot		- a reference to the return location of the property slot
 *				  for 'DictionaryLookup_FoundProperty' results.
 *	pPropertyPrototype	- an optional (Nil to omit) address of a descriptor which
 *				  will be initialized to either the value of the selector
 *				  for non-properties or to the property prototype for
 *				  properties.
 *
 *  Returns:
 *	The appropriate Vdd::Store::DictionaryLookup enumeration value.
 *
 *****/
bool rtVSTORE_Handle::getProperty (
    Vdd::Store::Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
) {
    align ();

    xPropertySlot += rtVSTORE_CPx_Column;
    rpResult.clear ();
    if (ReferenceIsntNil (xPropertySlot)) {
	VContainerHandle::Reference pPropertyHandle (elementHandle (xPropertySlot));
	if (pPropertyHandle->getStore (rpResult) && rpResult->isntACloneOf (pPropertyPrototype))
	    rpResult.clear ();
    }

//  Create a new property store of the right type if one does not exist...
    if (rpResult.isNil ()) {
	rtPTOKEN_Handle::Reference pPPT (getPToken ());
	pPropertyPrototype->clone (rpResult, pPPT);
	EnableModifications ();
	StoreReference (xPropertySlot, rpResult);
    }

    return true;
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
    rtLINK_Align (pLink);

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
    iDomainReference.constructReference (
	static_cast<rtLINK_Handle*>(pLink->containerHandle ())->pptHandle (),
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
 *				  Vdd::Store::DictionaryLookup_FoundProperty.
 *
 *  Returns:
 *	The appropriate Vdd::Store::DictionaryLookup enumeration value.
 *
 *****/
Vdd::Store::DictionaryLookup rtDICTIONARY_Handle::getElement (
    VSelector const &rSelector, DSC_Descriptor *pValueReturn, unsigned int *pOffsetReturn
) {
    DictionaryLookup xResult = DictionaryLookup_FoundNothing;

    if (rtDICTIONARY_UsingCache) {
	rtDICTIONARY_Cache::Reference pCache (cache ());
	rtSELUV_Set &rSet = pCache->selectors ();

	unsigned int xSelectorReference;
	if (rSelector.locateIn (rSet, xSelectorReference)) {
	    M_CPD *pPropertySubsetCPD = pCache->propertySubset ();

	    DSC_Scalar propertyReference;
	    if (rtLINK_Locate (pPropertySubsetCPD, xSelectorReference, propertyReference)) {
		xResult = Vdd::Store::DictionaryLookup_FoundProperty;

		if (pValueReturn) {
		    pCache->propertyPrototypes ()->getElements (*pValueReturn, propertyReference);
		}

		if (pOffsetReturn)
		    *pOffsetReturn = DSC_Scalar_Int (propertyReference);

		propertyReference.destroy ();
	    }
	    else if (pValueReturn) {
	    /*****  ... for everything else:  *****/
		xResult = Vdd::Store::DictionaryLookup_FoundOther;

		DSC_Scalar iSelectorReference;
		iSelectorReference.constructReference (rSet.ptoken (), xSelectorReference);

		pCache->bindings ()->getElements (*pValueReturn, iSelectorReference);

		iSelectorReference.destroy ();
	    }
	    else xResult = Vdd::Store::DictionaryLookup_FoundOther;
	}
    }
    else {
	rtSELUV_Set iSet (selectors ());

	unsigned int xSelectorReference;
	if (rSelector.locateIn (iSet, xSelectorReference)) {
	    M_CPD *pPropertySubsetCPD;
	    getPropertySubset (pPropertySubsetCPD);

	    DSC_Scalar propertyReference;
	    if (rtLINK_Locate (pPropertySubsetCPD, xSelectorReference, propertyReference)) {
		xResult = Vdd::Store::DictionaryLookup_FoundProperty;

		if (pValueReturn) {
		    rtVECTOR_Handle::Reference pPropertyPrototypes;
		    getPropertyPrototypes (pPropertyPrototypes);
		    pPropertyPrototypes->getElements (*pValueReturn, propertyReference);
		}

		if (pOffsetReturn)
		    *pOffsetReturn = DSC_Scalar_Int (propertyReference);

		propertyReference.destroy ();
	    }
	    else if (pValueReturn) {
	    /*****  ... for everything else:  *****/
		xResult = Vdd::Store::DictionaryLookup_FoundOther;

		DSC_Scalar iSelectorReference;
		iSelectorReference.constructReference (iSet.ptoken (), xSelectorReference);

		rtVECTOR_Handle::Reference pSelectorValues;
		getValues (pSelectorValues);
		pSelectorValues->getElements (*pValueReturn, iSelectorReference);

		iSelectorReference.destroy ();
	    }
	    else {
		xResult = Vdd::Store::DictionaryLookup_FoundOther;
	    }
	    pPropertySubsetCPD->release ();
	}
    }

    return xResult;
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

/*****  VComputationUnit Implementation  *****/

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

#include "VComputationUnit.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTvstore.h"

#include "IOMDriver.h"

#include "VControlPointSubscription.h"

#include "VFragment.h"

#include "VSelector.h"
#include "VSimpleFile.h"
#include "VSymbol.h"
#include "VSymbolImplementation.h"


/******************************
 ******************************
 *****                    *****
 *****  VComputationList  *****
 *****                    *****
 ******************************
 ******************************/

VComputationList::VComputationList (VComputationList const& rOther) : VReference<VComputationUnit> (rOther) {
}

VComputationList::VComputationList () {
}

VComputationList::~VComputationList () {
}


/*********************************
 *********************************
 *****                       *****
 *****  VComputationTrigger  *****
 *****                       *****
 *********************************
 *********************************/

VComputationTrigger::~VComputationTrigger () {
    triggerAll ();
}

void VComputationTrigger::suspend (VComputationUnit* pSuspendable) {
    pSuspendable->suspend ();
    pSuspendable->m_pSuccessor.claim (*this);
    setTo (pSuspendable);
}

void VComputationTrigger::triggerFirst () {
    if (isntEmpty ()) {
	VReference<VComputationUnit> pSuspendable;
	pop (pSuspendable);
	pSuspendable->resume ();
    }
}

void VComputationTrigger::triggerAll () {
    while (isntEmpty ()) {
	VReference<VComputationUnit> pSuspendable;
	pop (pSuspendable);
	pSuspendable->resume ();
    }
}


/******************************
 ******************************
 *****  Parameter Access  *****
 ******************************
 ******************************/

class VComputationUnitParameterAccessor : public VTransient {
    DECLARE_FAMILY_MEMBERS (VComputationUnitParameterAccessor, VTransient);

//  ParameterData
public:
    class PData : public VTransient {
	DECLARE_FAMILY_MEMBERS (PData, VTransient);

    //  Construction
    public:
	PData ();

    //  Destruction
    public:
	~PData ();

    //  Map Generation
    public:
	void append (
	    VComputationUnitParameterAccessor *pAccessor, unsigned int xCodElement
	);
	void commit (VComputationUnitParameterAccessor *pAccessor);

    //  Output Generation
    public:
	bool output (	//  returns true if output produced
	    VComputationUnitParameterAccessor *pAccessor, unsigned int xParameter
	);

    //  State
    private:
	unsigned int		 m_cElements;
	VReference<rtLINK_CType> m_pCodSubset;
	VReference<rtLINK_CType> m_pDomSubset;
    };
    friend class PData;

//  Construction
public:
    VComputationUnitParameterAccessor (VComputationUnit *pCodTask, VSNFTask *pDomTask)
	: m_pCodTask	(pCodTask)	//  ... accessed computation unit
	, m_pDomTask	(pDomTask)	//  ... accessing task
	, m_pDomPPT	(pDomTask->ptoken ())
	, m_xDomElement	(0)
	, m_cDomSubsets	(0)
	, m_sNASubset	(0)
	, m_iPDataArray (pCodTask->parameterCount_())
    {
    }

//  Destruction
public:
    ~VComputationUnitParameterAccessor () {
    }

//  Access
public:
    unsigned int domElement () const {
	return m_xDomElement;
    }
    unsigned int parameterIndex () const {
	return m_pParameterIndices[m_xDomElement] - 1;
    }

//  Map Generation
protected:
    void append (unsigned int xCodElement);

    void appendNA (unsigned int sRegion);
    void appendNA () {
	appendNA (m_pDomTask->cardinality ());
    }

    void commit ();

    rtLINK_CType *newCodSubset ();
    rtLINK_CType *newDomSubset (unsigned int sPrefix);

    rtPTOKEN_Handle *subsetPPT (unsigned int cElements);

//  Output Generation
private:
    void output (
	unsigned int xParameter, rtLINK_CType *pCodSubset, rtLINK_CType *pDomSubset
    );

//  Implementation
public:
    void implement ();

private:
    void handleNil (
	unsigned int xDomOrigin, unsigned int sRegion, unsigned int xCodOrigin
    ) {
	appendNA (sRegion);
    }
    void handleRepeat (
	unsigned int xDomOrigin, unsigned int sRegion, unsigned int xCodOrigin
    ) {
	while (sRegion-- > 0) {
	    append (xCodOrigin);
	}
    }
    void handleRange (
	unsigned int xDomOrigin, unsigned int sRegion, unsigned int xCodOrigin
    ) {
	while (sRegion-- > 0) {
	    append (xCodOrigin++);
	}
    }

//  State
private:
    VReference<VComputationUnit>const	m_pCodTask;
    VReference<VSNFTask>	const	m_pDomTask;
    rtPTOKEN_Handle*		const	m_pDomPPT;
    unsigned int			m_xDomElement;
    unsigned int			m_cDomSubsets;
    VReference<rtLINK_CType>		m_pNASubset;
    unsigned int			m_sNASubset;
    unsigned int const*			m_pParameterIndices;
    VCachedArray<PData, 1>		m_iPDataArray;
};


/**************************
 *****  Construction  *****
 **************************/

VComputationUnitParameterAccessor::PData::PData () : m_cElements (0) {
}

/*************************
 *****  Destruction  *****
 *************************/

VComputationUnitParameterAccessor::PData::~PData () {
}


/****************************
 *****  Map Generation  *****
 ****************************/

void VComputationUnitParameterAccessor::PData::append (
    VComputationUnitParameterAccessor *pAccessor, unsigned int xCodElement
) {
//  Maintain the source (cod) subset, ...
    if (m_pCodSubset.isNil ())
	m_pCodSubset.claim (pAccessor->newCodSubset ());
    m_pCodSubset->AppendRange (xCodElement, 1);

//  ... and the result (dom) subset, ...
    if (m_pDomSubset)
	m_pDomSubset->AppendRange (pAccessor->domElement (), 1);
    else if (m_cElements != pAccessor->domElement ()) {
	m_pDomSubset.claim (pAccessor->newDomSubset (m_cElements));
	m_pDomSubset->AppendRange (pAccessor->domElement (), 1);
    }

    m_cElements++;
}

void VComputationUnitParameterAccessor::append (unsigned int xCodElement) {
    unsigned int xParameter = parameterIndex ();
    if (xParameter >= m_iPDataArray.count ())
	appendNA (1);
    else {
	m_iPDataArray[xParameter].append (this, xCodElement);
	m_xDomElement++;
    }
}

void VComputationUnitParameterAccessor::appendNA (unsigned int sRegion) {
    if (m_pNASubset)
	m_pNASubset->AppendRepeat (m_xDomElement, sRegion);
    else if (m_sNASubset != m_xDomElement) {
	m_pNASubset.claim (newDomSubset (m_sNASubset));
	m_pNASubset->AppendRepeat (m_xDomElement, sRegion);
    }
    m_sNASubset += sRegion;
    m_xDomElement += sRegion;
}

void VComputationUnitParameterAccessor::PData::commit (VComputationUnitParameterAccessor *pAccessor) {
    if (m_pDomSubset.isNil () && m_cElements > 0 && m_cElements != pAccessor->domElement ())
	m_pDomSubset.claim (pAccessor->newDomSubset (m_cElements));
}

void VComputationUnitParameterAccessor::commit () {
    for (unsigned int xParameter = 0; xParameter < m_iPDataArray.count (); xParameter++)
	m_iPDataArray[xParameter].commit (this);
    if (m_pNASubset.isNil () && m_sNASubset > 0 && m_sNASubset != m_xDomElement)
	m_pNASubset.claim (newDomSubset (m_sNASubset));
}

rtLINK_CType *VComputationUnitParameterAccessor::newCodSubset () {
    return rtLINK_RefConstructor (m_pCodTask->ptoken_());
}

rtLINK_CType *VComputationUnitParameterAccessor::newDomSubset (unsigned int sPrefix) {
    m_cDomSubsets++;
    return rtLINK_RefConstructor (m_pDomTask->ptoken ())->AppendRange (0, sPrefix);
}

rtPTOKEN_Handle *VComputationUnitParameterAccessor::subsetPPT (unsigned int cElements) {
    return cElements < m_pDomPPT->cardinality () ? new rtPTOKEN_Handle (
	m_pDomPPT->ScratchPad (), cElements
    ) : m_pDomPPT;
}


/*******************************
 *****  Result Generation  *****
 *******************************/

//  Return true if some of the result was generated, ...
bool VComputationUnitParameterAccessor::PData::output (
    VComputationUnitParameterAccessor *pAccessor, unsigned int xParameter
) {
    if (m_cElements > 0) {
	rtPTOKEN_Handle::Reference pPPT (pAccessor->subsetPPT (m_cElements));
	m_pCodSubset->Close (pPPT);
	if (m_pDomSubset)
	    m_pDomSubset->Close (pPPT);

	pAccessor->output (xParameter, m_pCodSubset, m_pDomSubset);

	return true;
    }

    return false;
}

void VComputationUnitParameterAccessor::output (
    unsigned int xParameter, rtLINK_CType *pCodSubset, rtLINK_CType *pDomSubset
) {
    VDescriptor *pResult = &m_pDomTask->duc ();
    if (pDomSubset) {
	pDomSubset->retain ();
	pResult = &pResult->contentAsFragmentation ().createFragment (
	    pDomSubset
	)->datum ();
    }

    VDescriptor iParameter;
    m_pCodTask->getParameter (xParameter, iParameter);
    pResult->setToSubset (pCodSubset, iParameter);
}


/****************************
 *****  Implementation  *****
 ****************************/

void VComputationUnitParameterAccessor::implement () {
    if (!m_pDomTask->loadDucWithNextParameterAsMonotype ()) {
	m_pDomTask->loadDucWithNA ();
	return;
    }

    m_pDomTask->normalizeDuc ();
    DSC_Descriptor &rIndices = m_pDomTask->duc ().contentAsMonotype ();
    if (rIndices.store ()->DoesntName (&M_KOT::TheIntegerClass)) {
	m_pDomTask->loadDucWithNA ();
	return;
    }

    DSC_Descriptor &rCurrent = m_pDomTask->getCurrent ();

    m_pParameterIndices = rIndices;
    if (rIndices.holdsAScalarValue ()) {
	VDescriptor iParameter;
	if (!m_pCodTask->getParameter (parameterIndex (), iParameter))
	    m_pDomTask->loadDucWithNA ();
	else {
	    m_pDomTask->loadDucWithMoved (iParameter);
	    m_pDomTask->restrictDuc (rCurrent.Pointer ());
	}
	return;
    }

    rtLINK_CType *pLink = DSC_Descriptor_Link (rCurrent)->Align();

    rtLINK_TraverseRRDCList (pLink, handleNil, handleRepeat, handleRange);

    commit ();

    if (m_cDomSubsets > 0) {
	VFragmentation &rDucFragmentation = m_pDomTask->loadDucWithFragmentation ();
	if (m_pNASubset) {
	    m_pNASubset->Close (new rtPTOKEN_Handle (m_pDomPPT->ScratchPad (), m_sNASubset));
	    m_pNASubset->retain ();
	    rDucFragmentation.createFragment (m_pNASubset)->datum().setToNA (
		m_pNASubset->PPT (), m_pDomTask->codKOT ()
	    );
	    m_cDomSubsets--;
	}
	for (
	    unsigned int xParameter = 0;
	    xParameter < m_iPDataArray.count () && m_cDomSubsets > 0;
	    xParameter++
	) {
	    if (m_iPDataArray[xParameter].output (this, xParameter))
		m_cDomSubsets--;
	}
    }
    else if (m_sNASubset > 0)
	m_pDomTask->loadDucWithNA ();
    else for (
	unsigned int xParameter = 0;
	xParameter < m_iPDataArray.count () && !m_iPDataArray[xParameter].output (
	    this, xParameter
	);
	xParameter++
    );
}

void VComputationUnit::parameter (VSNFTask *pDomTask) {
    VComputationUnitParameterAccessor iImplementation (this, pDomTask);
    iImplementation.implement ();
}


/******************************************************
 ******************************************************
 *****                                            *****
 *****  VComputationUnit::Context::ControlPoints  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationUnit::Context::ControlPoints::ControlPoints (
    VComputationUnit* pController, ControlPoints* pPredecessor
) : m_pController (pController), m_pPredecessor (pPredecessor) {
    for (unsigned int xCP = 0; xCP < ControlPoint__COUNT; xCP++)
	m_iControlPoints[xCP].setControllerTo (this);
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

char const* VComputationUnit::Context::ControlPoints::NameFor (ControlPoint xCP) {
    switch (xCP) {
    case ControlPoint_Entry:
	return "Entry";
    case ControlPoint_Exit:
	return "Exit";
    case ControlPoint_Step:
	return "Step";
    case ControlPoint_Access:
	return "Access";
    case ControlPoint_Update:
	return "Update";
    case ControlPoint_Attention:
	return "Attention";
    case ControlPoint_Fault:
	return "Fault";
    case ControlPoint_Interrupt:
	return "Interrupt";
    case ControlPoint_SNF:
	return "SNF";
    default:
	return "-";
    }
}

char const* VComputationUnit::Context::ControlPoints::description () const {
    return "Context";
}


/***************************
 ***************************
 *****  Control Point  *****
 ***************************
 ***************************/

/********************
 *****  Access  *****
 ********************/

VComputationUnit::Context::ControlPoints::ControlPoint VComputationUnit::Context::ControlPoints::controlPoint (VControlPoint const* pCP) const {
    return pCP >= &m_iControlPoints[0] && pCP < &m_iControlPoints[ControlPoint__COUNT]
	? (ControlPoint)(pCP - m_iControlPoints)
	: ControlPoint__COUNT;
}

/*************************
 *****  Description  *****
 *************************/

char const* VComputationUnit::Context::ControlPoints::descriptionOf (VControlPoint const* pCP) const {
    return NameFor (controlPoint (pCP));
}

/*******************
 *****  Query  *****
 *******************/

bool VComputationUnit::Context::ControlPoints::canTrigger (VControlPoint const* pCP) const {
    return m_pController->canTrigger (controlPoint (pCP));
}

VControlPointFiringType VComputationUnit::Context::ControlPoints::firingType (VControlPoint const* pCP) const {
    return m_pController->firingType (controlPoint (pCP));
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  VComputationUnit::Context::GoferOrder  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

/*************************
 *************************
 *****  Fulfillment  *****
 *************************
 *************************/

bool VComputationUnit::Context::GoferOrder::fulfillFromClient (Context *pContext, Query *pQuery) const {
    return unfulfill ();
}

bool VComputationUnit::Context::GoferOrder::fulfillFromServer (Context *pContext) const {
    return unfulfill ();
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  VComputationUnit::Context::Query  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationUnit::Context::Query::Query (Vsa::VEvaluation *pEvaluation) : m_pEvaluation (pEvaluation), m_bEOL (false) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VComputationUnit::Context::Query::~Query () {
}

/**********************************
 **********************************
 *****  Client Object Gofers  *****
 **********************************
 **********************************/

bool VComputationUnit::Context::Query::getClientObjectGofer (icollection_gofer_t::Reference& rpGofer, Context *pContext) {
    return cachedClientObjectGofer (rpGofer, m_pICollectionGofer, pContext);
}

bool VComputationUnit::Context::Query::getClientObjectGofer (isingleton_gofer_t::Reference& rpGofer, Context *pContext) {
    return cachedClientObjectGofer (rpGofer, m_pISingletonGofer, pContext);
}

void VComputationUnit::Context::Query::setEOL () {
    m_bEOL = true;
    m_pICollectionGofer.clear ();
    m_pISingletonGofer.clear ();
}

bool VComputationUnit::Context::Query::failEOL (Vca::VRolePlayer *pGofer, char const *pWhere) const {
    VString iGoferType;
    if (pGofer)
        iGoferType.setTo (pGofer->rttName());
    std::cerr << this << ": " << pWhere << ", " << iGoferType << std::endl;
    return pGofer;
}


/***************************************
 ***************************************
 *****                             *****
 *****  VComputationUnit::Context  *****
 *****                             *****
 ***************************************
 ***************************************/

/*******************************
 *******************************
 *****  Tuning Parameters  *****
 *******************************
 *******************************/

bool VComputationUnit::Context::g_fVerboseSelectorNotFound	= getenv ("VisionVerboseSNF") ? true : false;
unsigned int VComputationUnit::Context::g_iLargeTaskSize	= 500000;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationUnit::Context::Context (ControlPoints& rControlPoints, Context* pParent)
: m_pParent		(pParent)
, m_rControlPoints	(rControlPoints)
, m_pLargeTaskSize	(pParent ? pParent->m_pLargeTaskSize : &g_iLargeTaskSize)
{
}

VComputationUnit::Context::Context (ThisClass *pParent)
: m_pParent		(pParent)
, m_rControlPoints	(pParent->m_rControlPoints)
, m_pLargeTaskSize	(pParent->m_pLargeTaskSize)
{
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VComputationUnit::Context::verboseSelectorNotFound () const {
    return m_pParent ? m_pParent->verboseSelectorNotFound () : g_fVerboseSelectorNotFound;
}

/*************************
 *************************
 *****  Fulfillment  *****
 *************************
 *************************/

bool VComputationUnit::Context::fulfill (GoferOrder const &rOrder) {
    return fulfillPrerequisite (rOrder);
}

bool VComputationUnit::Context::fulfillPrerequisite (GoferOrder const &rOrder) {
    return m_pParent ? m_pParent->fulfill (rOrder) : rOrder.fulfillFromServer (this);
}

bool VComputationUnit::Context::objectComesFromClient () const {
    static bool const bUsingEvaluator = IsNil (getenv ("SuppressEvaluatorPump"));
    return bUsingEvaluator;
}

bool VComputationUnit::Context::objectComesFromServer (VString& rServerName) const {
    static char const *const s_pServerName = getenv ("FASTBridgeServer");
    if (!s_pServerName)
	return false;

    static VString const s_iServerName (s_pServerName, false);
    rServerName.setTo (s_iServerName);
    return true;
}


/****************************************
 ****************************************
 *****                              *****
 *****  VComputationUnit::Profiler  *****
 *****                              *****
 ****************************************
 ****************************************/

class VComputationUnit::Profiler : public VBenderenceable {
    DECLARE_CONCRETE_RTT (Profiler, VBenderenceable);

//  Construction
private:
    static unsigned int g_xNextProfile;
    static pid_t const g_xThisProcess;
public:
    Profiler ();

//  Destruction
private:
    ~Profiler ();

//  Profiling
public:
    void beginProfiling (VComputationUnit *pSource, VkRunTimes &rProfile);
    void endProfiling (VComputationUnit *pSource, VkRunTimes &rProfile);

//  State
private:
    unsigned int const	m_xThisProfile;
    VSimpleFile		m_iOutputFile;
};

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VComputationUnit::Profiler);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

unsigned int VComputationUnit::Profiler::g_xNextProfile = 0;
pid_t const VComputationUnit::Profiler::g_xThisProcess = getpid ();

VComputationUnit::Profiler::Profiler () : m_xThisProfile (g_xNextProfile++) {
    char iOutputFileName[64];
    sprintf (iOutputFileName, "p_%u_%03u", g_xThisProcess, m_xThisProfile);
    m_iOutputFile.OpenForTextWrite (iOutputFileName);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VComputationUnit::Profiler::~Profiler () {
}

/***********************
 ***********************
 *****  Profiling  *****
 ***********************
 ***********************/

void VComputationUnit::Profiler::beginProfiling (VComputationUnit *pSource, VkRunTimes &rProfile) {
    rProfile.refresh ();
}

void VComputationUnit::Profiler::endProfiling (VComputationUnit *pSource, VkRunTimes &rProfile) {
    VkRunTimes iRunTimes;
    iRunTimes -= rProfile;

    unsigned int xSource = pSource->tuid32 ();

    VTask *pCaller = pSource->caller ();
    VComputationUnit *pCreator = pSource->creator ();

    m_iOutputFile.printf (
	"S\t%u\t%u\t%u\t%g\t%g\t%g\t%u\t%u\t%u\t%u\t%u\t%s\t%s\n",
	xSource,				// me
	pCaller ? pCaller->tuid32 () : 0,	// my caller (a task)
	pCreator ? pCreator->tuid32 () : 0,	// my creator
	iRunTimes.elapsed () * 1e3,		// convert seconds to milliseconds
	iRunTimes.user () * 1e3,		// convert seconds to milliseconds
	iRunTimes.system () * 1e3,		// convert seconds to milliseconds
	iRunTimes.pageflts (),
	iRunTimes.dirio (),
	iRunTimes.bufio (),
	iRunTimes.filcnt (),
	pSource->cardinality_(),
	pSource->rttName ().content (),
	pSource->selector_().messageName ()
    );
    VCall *pSourceAsCall = dynamic_cast<VCall*>(pSource);
    if (pSourceAsCall) {
	VString iCallDescription;
	pSourceAsCall->callerDecompilation (iCallDescription);

	char const *pRest = iCallDescription;
	while (*pRest) {
	    size_t sLine = strcspn (pRest, "\n");
	    m_iOutputFile.printf ("C\t%u %*.*s\n", xSource, sLine, sLine, pRest);

	    pRest += sLine;
	    if (pRest[0] == '\n')
		pRest++;
	}
    }
}


/*****************************************
 *****************************************
 *****                               *****
 *****  VComputationUnit::Extension  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationUnit::Extension::Extension (
    VComputationUnit *pController
) : m_pController (pController), m_pProfiler (
    pController->creatorDescendentProfiler ()
), m_pDescendentProfiler (m_pProfiler) {
    for (unsigned int xCP = 0; xCP < ControlPoint__COUNT; xCP++)
	m_iControlPoints[xCP].setControllerTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VComputationUnit::Extension::~Extension () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

char const* VComputationUnit::Extension::NameFor (ControlPoint xCP) {
    switch (xCP) {
    case ControlPoint_ChildEntry:
	return "Child Entry";
    case ControlPoint_ExteriorChildEntry:
	return "Exterior Child Entry";
    case ControlPoint_InteriorChildEntry:
	return "Interior Child Entry";
    case ControlPoint_Entry:
	return "Entry";
    case ControlPoint_Exit:
	return "Exit";
    case ControlPoint_Step:
	return "Step";
    default:
	return "-";
    }
}

char const* VComputationUnit::Extension::description () const {
    return "Unit";
}


/***************************
 ***************************
 *****  Control Point  *****
 ***************************
 ***************************/

/********************
 *****  Access  *****
 ********************/

VComputationUnit::Extension::ControlPoint VComputationUnit::Extension::controlPoint (VControlPoint const* pCP) const {
    return pCP >= &m_iControlPoints[0] && pCP < &m_iControlPoints[ControlPoint__COUNT]
	? (ControlPoint)(pCP - m_iControlPoints)
	: ControlPoint__COUNT;
}

/*************************
 *****  Description  *****
 *************************/

char const* VComputationUnit::Extension::descriptionOf (VControlPoint const* pCP) const {
    return NameFor (controlPoint (pCP));
}

/*******************
 *****  Query  *****
 *******************/

bool VComputationUnit::Extension::canTrigger (VControlPoint const* pCP) const {
    return m_pController->canTrigger (controlPoint (pCP));
}

VControlPointFiringType VComputationUnit::Extension::firingType (VControlPoint const* pCP) const {
    return m_pController->firingType (controlPoint (pCP));
}

/**********************
 *****  Shutdown  *****
 **********************/

void VComputationUnit::Extension::shutdownIfUntriggerable (ControlPoint xCP) {
    if (m_pController->cantTrigger (xCP))
	m_iControlPoints[xCP].shutdown ();
}

void VComputationUnit::Extension::sendTerminationEvents () {
    for (unsigned int xCP = 0; xCP < ControlPoint__COUNT; xCP++)
	shutdownIfUntriggerable ((ControlPoint)xCP);

    m_iTerminationTrigger.triggerAll ();
}


/***********************
 ***********************
 *****  Profiling  *****
 ***********************
 ***********************/

void VComputationUnit::Extension::newDescendentProfiler () {
    setDescendentProfilerTo (new Profiler ());
}

void VComputationUnit::Extension::setDescendentProfilerTo (Profiler *pProfiler) {
    m_pDescendentProfiler.setTo (pProfiler);
}

void VComputationUnit::Extension::beginProfiling (VComputationUnit *pSource) {
    if (m_pProfiler)
	m_pProfiler->beginProfiling (pSource, m_iProfile);
}

void VComputationUnit::Extension::endProfiling (VComputationUnit *pSource) {
    if (m_pProfiler)
	m_pProfiler->endProfiling (pSource, m_iProfile);
}


/******************************
 ******************************
 *****                    *****
 *****  VComputationUnit  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VComputationUnit);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_cipath_c<VComputationUnit, VTask*>;
template class Vsi_cipath_c<VComputationUnit, VComputationUnit*>;
template class Vsi_ciref_c<VComputationUnit, VDescriptor>;
template class Vsi_setDucFeathers<VComputationUnit>;
template class Vsi_b0_c<VComputationUnit>;
template class Vsi_f0_c<VComputationUnit, char const*>;
template class Vsi_f0_c<VComputationUnit, VSelector const&>;
template class Vsi_property<VComputationUnit, unsigned int>;
template class Vsi_f0_c<VComputationUnit, unsigned int>;
template class Vsi_r0<VComputationUnit,VControlPointSubscription*,VGroundStore*>;
template class Vsi_tb<VComputationUnit>;
template class Vsi_tpFullDuc<VComputationUnit>;

void VComputationUnit::MetaMaker () {
    static Vsi_cipath_c<VComputationUnit, VTask*> const
	si_caller			(&VComputationUnit::getPathToCaller);

    static Vsi_cipath_c<VComputationUnit, VComputationUnit*> const
	si_consumer			(&VComputationUnit::getPathToConsumer);

    static Vsi_cipath_c<VComputationUnit, VComputationUnit*> const
	si_creator			(&VComputationUnit::getPathToCreator);

    static Vsi_ciref_c<VComputationUnit, VDescriptor> const
	si_datum			(&VComputationUnit::cdatum);

    static Vsi_setDucFeathers<VComputationUnit> const
	si_setDucFeathers;

    static Vsi_b0_c<VComputationUnit> const
	si_blocked			(&VComputationUnit::blocked),
	si_damaged			(&VComputationUnit::damaged),
	si_datumAlterable		(&VComputationUnit::datumAlterable_),
	si_datumAvailable		(&VComputationUnit::datumAvailable_),
	si_failed			(&VComputationUnit::failed),
	si_interrupted			(&VComputationUnit::interrupted),
	si_isInterior			(&VComputationUnit::isInterior),
	si_recipientAvailable		(&VComputationUnit::recipientAvailable_),
	si_runnable			((bool (VComputationUnit::*) () const)&VComputationUnit::runnable),
	si_terminated			(&VComputationUnit::terminated);

    static Vsi_f0_c<VComputationUnit, char const*> const
	si_returnCaseName		(&VComputationUnit::returnCaseName),
	si_stateName			(&VComputationUnit::stateName),
	si_stageName			(&VComputationUnit::stageName);

    static Vsi_f0_c<VComputationUnit, VSelector const&> const
	si_selector			(&VComputationUnit::selector_);

    static Vsi_property<VComputationUnit, unsigned int> const
	si_suspendCount			(&VComputationUnit::m_iSuspendCount),
	si_unrunChildCount		(&VComputationUnit::m_cUnrunChildren);

    static Vsi_f0_c<VComputationUnit, unsigned int> const
	si_channelIndex			(&VComputationUnit::channelIndex),
	si_parameterCount		(&VComputationUnit::parameterCount_),
	si_unrunExteriorChildCount	(&VComputationUnit::unrunExteriorChildren),
	si_unrunInteriorChildCount	(&VComputationUnit::unrunInteriorChildren);

    static Vsi_r0<VComputationUnit,VControlPointSubscription*,VGroundStore*> const
	si_contextEntrySubscription	(&VComputationUnit::contextEntrySubscription),
	si_contextExitSubscription	(&VComputationUnit::contextExitSubscription),
	si_contextStepSubscription	(&VComputationUnit::contextStepSubscription),
	si_contextAccessSubscription	(&VComputationUnit::contextAccessSubscription),
	si_contextUpdateSubscription	(&VComputationUnit::contextUpdateSubscription),
	si_contextAttentionSubscription	(&VComputationUnit::contextAttentionSubscription),
	si_contextFaultSubscription	(&VComputationUnit::contextFaultSubscription),
	si_contextInterruptSubscription	(&VComputationUnit::contextInterruptSubscription),
	si_contextSNFSubscription	(&VComputationUnit::contextSNFSubscription),
	si_unitChildEntrySubscription	(&VComputationUnit::unitChildEntrySubscription),
	si_unitExteriorChildEntrySubscription
					(&VComputationUnit::unitExteriorChildEntrySubscription),
	si_unitInteriorChildEntrySubscription
					(&VComputationUnit::unitInteriorChildEntrySubscription),
	si_unitEntrySubscription	(&VComputationUnit::unitEntrySubscription),
	si_unitExitSubscription		(&VComputationUnit::unitExitSubscription),
	si_unitStepSubscription		(&VComputationUnit::unitStepSubscription);

    static Vsi_tb<VComputationUnit> const si_waitForTermination (
	(bool (VComputationUnit::*) (VSNFTask*))&VComputationUnit::untilCompletionSuspend
    );

    static Vsi_tpFullDuc<VComputationUnit> const
	si_parameter			(&VComputationUnit::parameter);

    CSym ("isACall"			)->set (RTT, &g_siFalse);
    CSym ("isATask"			)->set (RTT, &g_siFalse);

    CSym ("caller"			)->set (RTT, &si_caller);
    CSym ("consumer"			)->set (RTT, &si_consumer);
    CSym ("creator"			)->set (RTT, &si_creator);

    CSym ("channelIndex"		)->set (RTT, &si_channelIndex);

    CSym ("datum"			)->set (RTT, &si_datum);
    CSym ("datumAlterable"		)->set (RTT, &si_datumAlterable);
    CSym ("datumAvailable"		)->set (RTT, &si_datumAvailable);

    CSym ("parameter:"			)->set (RTT, &si_parameter);
    CSym ("parameterCount"		)->set (RTT, &si_parameterCount);

    CSym ("recipientAvailable"		)->set (RTT, &si_recipientAvailable);

    CSym ("blocked"			)->set (RTT, &si_blocked);
    CSym ("damaged"			)->set (RTT, &si_damaged);
    CSym ("failed"			)->set (RTT, &si_failed);
    CSym ("interrupted"			)->set (RTT, &si_interrupted);
    CSym ("runnable"			)->set (RTT, &si_runnable);
    CSym ("terminated"			)->set (RTT, &si_terminated);

    CSym ("isInterior"			)->set (RTT, &si_isInterior);

    CSym ("returnCaseName"		)->set (RTT, &si_returnCaseName);

    CSym ("selector"			)->set (RTT, &si_selector);

    CSym ("stageName"			)->set (RTT, &si_stageName);
    CSym ("stateName"			)->set (RTT, &si_stateName);

    CSym ("suspendCount"		)->set (RTT, &si_suspendCount);

    CSym ("unrunChildCount"		)->set (RTT, &si_unrunChildCount);
    CSym ("unrunExteriorChildCount"	)->set (RTT, &si_unrunExteriorChildCount);
    CSym ("unrunInteriorChildCount"	)->set (RTT, &si_unrunInteriorChildCount);

    CSym ("setDatumTo:"			)->set (RTT, &si_setDucFeathers);

    CSym ("contextEntrySubscription"	)->set (RTT, &si_contextEntrySubscription);
    CSym ("contextExitSubscription"	)->set (RTT, &si_contextExitSubscription);
    CSym ("contextStepSubscription"	)->set (RTT, &si_contextStepSubscription);
    CSym ("contextAccessSubscription"	)->set (RTT, &si_contextAccessSubscription);
    CSym ("contextUpdateSubscription"	)->set (RTT, &si_contextUpdateSubscription);
    CSym ("contextAttentionSubscription")->set (RTT, &si_contextAttentionSubscription);
    CSym ("contextFaultSubscription"	)->set (RTT, &si_contextFaultSubscription);
    CSym ("contextInterruptSubscription")->set (RTT, &si_contextInterruptSubscription);
    CSym ("contextSNFSubscription"	)->set (RTT, &si_contextSNFSubscription);
    CSym ("unitChildEntrySubscription"	)->set (RTT, &si_unitChildEntrySubscription);
    CSym ("unitExteriorChildEntrySubscription")->set (
	RTT, &si_unitExteriorChildEntrySubscription
    );
    CSym ("unitInteriorChildEntrySubscription")->set (
	RTT, &si_unitInteriorChildEntrySubscription
    );
    CSym ("unitEntrySubscription"	)->set (RTT, &si_unitEntrySubscription);
    CSym ("unitExitSubscription"	)->set (RTT, &si_unitExitSubscription);
    CSym ("unitStepSubscription"	)->set (RTT, &si_unitStepSubscription);

    CSym ("waitForTermination"		)->set (RTT, &si_waitForTermination);
}


/***********************************
 ***********************************
 *****  Description Utilities  *****
 ***********************************
 ***********************************/

char const* VComputationUnit::NameFor (ReturnCase xElement) {
    switch (xElement) {
    case Return_Unspecified:
	return "Unspecified";
    case Return_Intension:
	return "Intension";
    case Return_Value:
	return "Value";
    case Return_Current:
	return "Current";
    default:
	return string ("#%d???", xElement);
    }
}

char const* VComputationUnit::NameFor (Stage xElement) {
    switch (xElement) {
    case Stage_Unrun:
	return "Unrun";
    case Stage_Entry:
	return "Entry";
    case Stage_Running:
	return "Running";
    case Stage_Exit:
	return "Exit";
    case Stage_Completed:
	return "Completed";
    default:
	return string ("#%d???", xElement);
    }
}

char const* VComputationUnit::NameFor (State xElement) {
    switch (xElement) {
    case State_Runnable:
	return "Runnable";
    case State_Interrupted:
	return "Interrupted";
    case State_Damaged:
	return "Damaged";
    case State_Failed:
	return "Failed";
    case State_Completed:
	return "Terminated";
    default:
	return string ("#%d???", xElement);
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VComputationUnit::VComputationUnit (VComputationUnit *pCreator, VTask *pCaller, unsigned int iAttentionMask)
: m_pCaller			(pCaller)
, m_pCreator			(pCreator)
, m_pContext			(pCreator->context ())
, m_rDatum			(pCreator->duc ())
, m_pDuc			(&m_rDatum)
, m_pExtension			(pCreator->profilingDescendents () ? new Extension (this) : 0)
, m_pScheduler			(pCreator->scheduler ())
, m_iSuspendCount		(1)
, m_cUnrunChildren		(0)
, m_cUnrunInteriorChildren	(0)
, m_xState			(State_Runnable)
, m_xStage			(Stage_Unrun)
, m_iAttentionMask		(iAttentionMask)
, m_fAtStepPoint		(false)
, m_fIsInterior			(false)
{
    m_pCreator->m_cUnrunChildren++;
}

VComputationUnit::VComputationUnit (Context* pContext, VDescriptor& rDatum, VComputationScheduler* pScheduler)
: m_pCaller			(0)
, m_pContext			(pContext)
, m_rDatum			(rDatum)
, m_pDuc			(&m_rDatum)
, m_pExtension			(0)
, m_pScheduler			(pScheduler)
, m_iSuspendCount		(1)
, m_cUnrunChildren		(0)
, m_cUnrunInteriorChildren	(0)
, m_xState			(State_Runnable)
, m_xStage			(Stage_Unrun)
, m_iAttentionMask		(0)
, m_fAtStepPoint		(false)
, m_fIsInterior			(false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

void VComputationUnit::cancelUnrunChild (VComputationUnit const* pChild) {
    if (pChild->isInterior ())
	m_cUnrunInteriorChildren--;

    if (0 == --m_cUnrunChildren && m_pExtension) {
	m_pExtension->shutdownIfUntriggerable (Extension::ControlPoint_ChildEntry);
	m_pExtension->shutdownIfUntriggerable (Extension::ControlPoint_ExteriorChildEntry);
	m_pExtension->shutdownIfUntriggerable (Extension::ControlPoint_InteriorChildEntry);
    }
}

VComputationUnit::~VComputationUnit () {
    if (m_pExtension)
	delete m_pExtension;
    if (m_pCreator && m_xStage == Stage_Unrun)
	m_pCreator->cancelUnrunChild (this);
}


/***********************
 ***********************
 *****  Extension  *****
 ***********************
 ***********************/

VComputationUnit::Extension* VComputationUnit::extension () {
    return m_pExtension ? m_pExtension : (m_pExtension = new VComputationUnit::Extension (this));
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VComputationUnit::consumesResultOf (VComputationUnit const* pUnit) const {
    while (pUnit) {
	if (this == pUnit)
	    return true;

	pUnit = pUnit->consumer_ ();
    }

    return false;
}

bool VComputationUnit::datumAlterable_ () const {
    return datumAvailable_ ();
}

bool VComputationUnit::datumAvailable_ () const {
    return false;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VComputationUnit::cdatum (VDescriptor& rDatum) const {
    if (datumAvailable_ ()) {
	rDatum.setToCopied (m_rDatum);
	return true;
    }

    return false;
}

unsigned int VComputationUnit::channelIndex () const {
    return channel_()->index () + 1;
}

char const *VComputationUnit::selectorName () const {
    return selector_().messageName ();
}


/************************
 ************************
 *****  Duc Access  *****
 ************************
 ************************/

/************************
 ************************
 *****  Duc Update  *****
 ************************
 ************************/

/**************************
 *****  Modification  *****
 **************************/

bool VComputationUnit::setDucFeathers (DSC_Pointer &rFeathers, VDescriptor &rValues) const {
    if (!datumAlterable_ ())
	return false;

    if (rFeathers.holdsAnIdentity ())
	loadDucWithCopied (rValues);
    else {
	if (m_pDuc->isEmpty ())
	    loadDucWithVector (new rtVECTOR_Handle (ptoken_()));
	else
	    normalizeDuc ();
	rValues.assignTo (rFeathers, convertDucToVector ());
	ConvertDucVectorsToMonotypeIfPossible ();
    }

    return true;
}


/****************************
 *****  Transformation  *****
 ****************************/

/*---------------------------------------------------------------------------
 *****  Utility to load the duc with a reordered restriction of itself.
 *
 *  Arguments:
 *	pReordering		- an optional ('Nil' if absent) reference
 *				  uvector specifying the reordering to be
 *				  applied to the elements selected by
 *				  'restriction'.
 *	pRestriction		- an optional ('Nil' if absent) constructor
 *				  for a link specifying the restriction of
 *				  accumulator positions to be selected.
 *
 *  Returns:
 *	Nothing - Executed for side effect on the accumulator only.
 *
 *****/
void VComputationUnit::restrictAndReorderDuc (rtLINK_CType* pRestriction, M_CPD *pReordering) const {
    if (pRestriction || pReordering) {
	VDescriptor iReorderedSubsetOfDuc;

	iReorderedSubsetOfDuc.setToReorderedSubset (pReordering, pRestriction, *m_pDuc);

	m_pDuc->setToMoved (iReorderedSubsetOfDuc);
    }
}

/*---------------------------------------------------------------------------
 *****  Utility to load the duc with a restriction of itself.
 *
 *  Arguments:
 *	pRestriction		- an optional ('Nil' if absent) constructor
 *				  for a link specifying the restriction of
 *				  accumulator positions to be selected.
 *
 *  Returns:
 *	Nothing - Executed for side effect on the accumulator only.
 *
 *****/
void VComputationUnit::restrictDuc (rtLINK_CType* pRestriction) const {
    if (pRestriction) {
	VDescriptor iNewDuc;

	iNewDuc.setToSubset (pRestriction, *m_pDuc);

	m_pDuc->setToMoved (iNewDuc);
    }
}

void VComputationUnit::restrictDuc (DSC_Pointer& rRestriction) const {
    if (!rRestriction.holdsAnIdentity ()) {
	VDescriptor iNewDuc;

	iNewDuc.setToSubset (rRestriction, *m_pDuc);

	m_pDuc->setToMoved (iNewDuc);
    }
}


/********************************************
 ********************************************
 *****  Control Point/Event Management  *****
 ********************************************
 ********************************************/

/*******************
 *****  Query  *****
 *******************/

bool VComputationUnit::canTrigger (Context::ControlPoints::ControlPoint xCP) const {
    switch (xCP) {
    case Context::ControlPoints::ControlPoint_Entry:
    case Context::ControlPoints::ControlPoint_Exit:
    case Context::ControlPoints::ControlPoint_Step:
    case Context::ControlPoints::ControlPoint_Access:
    case Context::ControlPoints::ControlPoint_Update:
    case Context::ControlPoints::ControlPoint_Attention:
    case Context::ControlPoints::ControlPoint_Fault:
    case Context::ControlPoints::ControlPoint_Interrupt:
    case Context::ControlPoints::ControlPoint_SNF:
	return m_xStage < Stage_Exit;

    default:
	return false;
    }
}

bool VComputationUnit::canTrigger (Extension::ControlPoint xCP) const {
    switch (xCP) {
    case Extension::ControlPoint_ChildEntry:
	return m_xStage < Stage_Exit || m_cUnrunChildren > 0;

    case Extension::ControlPoint_ExteriorChildEntry:
	return m_xStage < Stage_Exit || unrunExteriorChildren () > 0;

    case Extension::ControlPoint_InteriorChildEntry:
	return m_xStage < Stage_Exit || unrunInteriorChildren () > 0;

    case Extension::ControlPoint_Entry:
	return m_xStage == Stage_Unrun;

    case Extension::ControlPoint_Exit:
    case Extension::ControlPoint_Step:
	return m_xStage < Stage_Exit;

    default:
	return false;
    }
}


VControlPointFiringType VComputationUnit::firingType (Context::ControlPoints::ControlPoint /*xCP*/) const {
    return m_xStage < Stage_Exit
	? VControlPoint::Controller::FiringType_Normal
	: VControlPoint::Controller::FiringType_Final;
}

VControlPointFiringType VComputationUnit::firingType (Extension::ControlPoint xCP) const {
    VControlPointFiringType xBaseFiringType = m_xStage < Stage_Exit
	? VControlPoint::Controller::FiringType_Normal
	: VControlPoint::Controller::FiringType_Final;

    switch (xCP) {
    case Extension::ControlPoint_ChildEntry:
	return m_cUnrunChildren > 0 ? VControlPoint::Controller::FiringType_Partial : xBaseFiringType;

    case Extension::ControlPoint_ExteriorChildEntry:
	return unrunExteriorChildren () > 0 ? VControlPoint::Controller::FiringType_Partial : xBaseFiringType;

    case Extension::ControlPoint_InteriorChildEntry:
	return unrunInteriorChildren () > 0 ? VControlPoint::Controller::FiringType_Partial : xBaseFiringType;

    case Extension::ControlPoint_Entry:
	return VControlPoint::Controller::FiringType_Final;

    default:
	return xBaseFiringType;
    }
}


/**************************
 *****  Subscription  *****
 **************************/

VControlPointSubscription* VComputationUnit::subscriptionTo (Context::ControlPoints::ControlPoint xCP) {
    return m_pContext->subscriptionTo (xCP);
}

VControlPointSubscription* VComputationUnit::subscriptionTo (Extension::ControlPoint xCP) {
    return canTrigger (xCP) ? extension()->subscriptionTo (xCP) : 0;
}


/************************
 *****  Triggering  *****
 ************************/

void VComputationUnit::triggerChildEntrySubscriptions_ (VComputationUnit* pChild) {
    m_pExtension->trigger (Extension::ControlPoint_ChildEntry, pChild);
    if (pChild->isInterior ()) {
	m_cUnrunInteriorChildren--;
	m_pExtension->trigger (Extension::ControlPoint_InteriorChildEntry, pChild);
    }
    else {
	m_pExtension->trigger (Extension::ControlPoint_ExteriorChildEntry, pChild);
    }
}

/************************
 *****  Event Wait  *****
 ************************/

bool VComputationUnit::suspendOnQueue (
    VComputationUnit* pSuspendee, VComputationTrigger& (VComputationUnit::*pQueueAccessor)()
) {
    if (canSafelySuspend (pSuspendee)) {
	(this->*pQueueAccessor)().suspend (pSuspendee);
	return true;
    }

    return false;
}

/*****************************
 *****  Completion Wait  *****
 *****************************/

VComputationTrigger& VComputationUnit::completionQueue () {
    return extension()->m_iTerminationTrigger;
}

bool VComputationUnit::untilCompletionSuspend (VSNFTask* pSuspendee) {
    return untilCompletionSuspend ((VComputationUnit*)pSuspendee);
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/************************
 *****  Scheduling  *****
 ************************/

void VComputationUnit::yield () {
    scheduler ()->yield ();
}


/***************************
 *****  State Control  *****
 ***************************/

bool VComputationUnit::setStateTo (State xState) {
    bool stateSet = false;
    switch (m_xState) {
    case State_Runnable:
    case State_Interrupted:
	m_xState = xState;
	stateSet = true;
	break;

    case State_Damaged:
    case State_Failed:
	switch (xState) {
	case State_Damaged:
	case State_Failed:
	case State_Completed:
	    m_xState = xState;
	    stateSet = true;
	    break;

	default:
	    break;
	}
	break;

    case State_Completed:
	switch (xState) {
	case State_Completed:
	    stateSet = true;
	    break;

	default:
	    break;
	}
	break;

    default:
	break;
    }

    return stateSet;
}

void VComputationUnit::resume (State xState) {
    setStateTo (xState);
    resume ();
}

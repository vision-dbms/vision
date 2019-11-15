/*****  VSNFTask Implementation  *****/

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

#include "VSNFTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VFragment.h"

#include "VOutputGenerator.h"

#include "VSelector.h"

#include "VSNFTaskHolder.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"
#include "Vxa_ICollection.h"


/****************************************
 ****************************************
 *****                              *****
 *****  OnPopulateCompleted Assign  *****
 *****                              *****
 ****************************************
 ****************************************/

/*------------------------------------------------------------------------------------*
 *  Monotype Case:
 *------------------------------------------------------------------------------------*/

static void Assign (
    VkDynamicArrayOf<Vxa::ISingleton::Reference> &rTarget, rtLINK_CType *pSubset, DSC_Descriptor const &rSource
) {
    VExternalGroundStore *pXGS = static_cast<VExternalGroundStore*> (
	rSource.storeTransientExtensionIfA(VExternalGroundStore::RTT)
    );
    Vxa::ISingleton *pISingleton = pXGS ? pXGS->getInterface () : 0;

    if (pSubset) {
#       define handleNil(c,n,r)
#       define handleRepeat(c,n,r) {\
            rTarget[r] = pISingleton;\
        }
#       define handleRange(c,n,r) {\
            while (n-- > 0)\
            rTarget[r++] = pISingleton;\
        }

        rtLINK_TraverseRRDCList (pSubset, handleNil, handleRepeat, handleRange);

#       undef handleNil
#       undef handleRepeat
#       undef handleRange
    }
    else {
	unsigned int nElements = rTarget.cardinality ();
/*
        if (pISingleton == NULL) printf ("pIVB in VSNFTask::OnPopulateCompleted is NULL...");
        else printf ("pIVB in VSNFTask::OnPopulateCompleted is not NULL...%d/n", nElements);
*/
	for (unsigned int xElement = 0; xElement < nElements; xElement++)
	    rTarget[xElement] = pISingleton;
    }
}

/*------------------------------------------------------------------------------------*
 *  Polytype Case:
 *------------------------------------------------------------------------------------*/

static void Assign (
    VkDynamicArrayOf<Vxa::ISingleton::Reference> &rTarget, rtLINK_CType *pSubset, VDescriptor &rSource
) {
    if (rSource.convertVectorsToMonotypeIfPossible ())
	Assign (rTarget, pSubset, rSource.contentAsMonotype ());
    else {
	VFragmentation &rFragmentation = rSource.convertToFragmentation ();
	rSource.normalize ();

	VFragment* pFragment;
	rFragmentation.goToFirstFragment ();
	while (pFragment = rFragmentation.currentFragment ()) {
	    rtLINK_CType::Reference pFragmentSubsetOfTarget;
	    if (pSubset)
		pFragmentSubsetOfTarget.claim (pSubset->Extract (pFragment->subset ()));
	    else
		pFragmentSubsetOfTarget.setTo (pFragment->subset ());
	    
	    Assign (rTarget, pFragmentSubsetOfTarget, pFragment->datum ());
	    rFragmentation.goToNextFragment ();
	}
    }
}


/**********************
 **********************
 *****            *****
 *****  VSNFTask  *****
 *****            *****
 **********************
 **********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSNFTask);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_f0_c<VSNFTask, unsigned int>;
template class Vsi_p0<VSNFTask>;
template class Vsi_tpFullDuc<VSNFTask>;

void VSNFTask::MetaMaker () {
    static Vsi_f0_c<VSNFTask, unsigned int> const
	si_skipCount	(&VSNFTask::skipCount);

    static Vsi_p0<VSNFTask> const
	si_dismiss	(&VSNFTask::dismiss);

    static Vsi_tpFullDuc<VSNFTask> const
	si_resend	(&VSNFTask::resendIn);

    CSym ("isASNFTask"	)->set (RTT, &g_siTrue);

    CSym ("skipCount"	)->set (RTT, &si_skipCount);

    CSym ("dismiss"	)->set (RTT, &si_dismiss);

    CSym ("resendTo:"	)->set (RTT, &si_resend);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSNFTask::VSNFTask (ConstructionData const& rTCData, unsigned int const iSkipCount)
: VTask			(rTCData)
, m_iParameters		(rTCData.call()->parameterCount ())
, m_iSkipCount		(iSkipCount)
, m_pContinuation	(&VSNFTask::DelegateToRecipient)
, m_pGroundContinuation	(0)
, m_cSegmentsExpected	(0)
, m_cSegmentsReceived	(0)
, m_bRunning		(false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSNFTask::~VSNFTask () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VSNFTask::getParameter (unsigned int xParameter, VDescriptor& rDatum) {
    if (xParameter < parameterCount () && (
	    !m_iParameters[xParameter].isEmpty () || BaseClass::getParameter (
		xParameter, m_iParameters[xParameter]
	    )
	)
    ) {
	rDatum.setToCopied (m_iParameters[xParameter]);
	return true;
    }

    return false;
}

bool VSNFTask::getSelfReference (object_reference_t &rSelfReference) {
    bool bGood = false;

    DSC_Descriptor &rSelf = getCurrent ();
    if (rSelf.isScalar ()) {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rSelf));
	rSelfReference = DSC_Descriptor_Scalar_Int (rSelf);
	bGood = true;
    } else {
	rtLINK_CType *const pSelfLink = DSC_Descriptor_Link (rSelf)->Align ();
	rSelfReference = rtLINK_RRDC_ReferenceOrigin (rtLINK_LC_ChainHead (pSelfLink));
	bGood = true;
    }
    return bGood;
}

bool VSNFTask::getSelfReference (object_reference_array_t &rSelfReferences) {
    bool bGood = false;

    rSelfReferences.DeleteAll ();

    DSC_Descriptor &rSelf = getCurrent ();
    if (rSelf.isScalar ()) {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rSelf));
	rSelfReferences.Append (DSC_Descriptor_Scalar_Int (rSelf));
	bGood = true;
    } else {
	rSelfReferences.Guarantee (cardinality ());
	rtLINK_CType *const pSelfLink = DSC_Descriptor_Link (rSelf)->Align ();

#define GetSelfReferenceRepeat(position,count,reference) {\
	    while (count-- > 0)\
		rSelfReferences[position++] = reference;\
	}
#define GetSelfReferenceRange(position,count,reference) {\
	    while (count-- > 0)\
		rSelfReferences[position++] = reference++;\
	}
	rtLINK_TraverseRRDCList (pSelfLink, GetSelfReferenceRepeat, GetSelfReferenceRepeat, GetSelfReferenceRange);
#undef GetSelfReferenceRepeat
#undef GetSelfReferenceRange

	bGood = true;
    }

    return bGood;
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VSNFTask::datumAlterable_ () const {
    return m_xStage == Stage_Running || VTask::datumAlterable_ ();
}


/********************
 ********************
 *****  Resend  *****
 ********************
 ********************/

void VSNFTask::resendIn (VSNFTask *pResendTask) {
    unsigned int cParameters = parameterCount ();

//  Using the selector of this task, begin a message call in the resend task, ...
    //  (New 'VTask::beginMessageCall (VSelector const&)' overload?):
    VSelector const &rSelector = selector_(); unsigned int xSelector;
    if (rSelector.getKSData (xSelector))
	pResendTask->beginMessageCall (xSelector);
    else
	pResendTask->beginMessageCall (rSelector, cParameters);

//  ... using the parameter of the resend task as the message recipient, ...
    pResendTask->loadDucWithNextParameter ();
    pResendTask->commitRecipient ();

//  ... and the parameters of this task as the parameters of the message:
    DSC_Pointer &rParameterRestriction = pResendTask->getCurrent ().Pointer ();
    VDescriptor iParameter;
    for (unsigned int xParameter = 0; xParameter < cParameters; xParameter++) {
	getParameter (xParameter, iParameter);
	pResendTask->loadDucWithMoved (iParameter);
	pResendTask->restrictDuc (rParameterRestriction);
	pResendTask->commitParameter ();
    }
    pResendTask->commitCall (returnCase ());
}


/***************************
 ***************************
 *****  Continuations  *****
 ***************************
 ***************************/

void VSNFTask::DelegateToRecipient () {
    DSC_Descriptor const &rCurrent = current();

    VGroundStore* pGroundStore = (VGroundStore*)rCurrent.storeTransientExtensionIfA (VGroundStore::RTT);

    if (pGroundStore && pGroundStore->groundImplementationSucceeded (this))
	return;

    if (call()->snfHushed ()) {
	loadDucWithNA ();
	return;
    }

    m_pContinuation = &VSNFTask::TriggerControlPoint;

    beginHushedMessageCall ("handleSNF:", 1);

    commitRecipient (VMagicWord_Self);

    loadDucWith (this);
    commitParameter ();

    commitCall ();
}

void VSNFTask::ContinueFromReturnError () {
    VString errMsg = "Implementation Failure Error: <<<\n>>> ";
    issueWarningMessage ("External Object Selector", &selector (), errMsg.append(m_pErrorString.content()).content());
}

void VSNFTask::ContinueOnTheGround () {
    GroundContinuation pGroundContinuation = m_pGroundContinuation;
    m_pGroundContinuation = 0;

    (*pGroundContinuation) (this);
}

void VSNFTask::TriggerControlPoint () {
    m_pContinuation = &VSNFTask::DoAnOldFashionedSNF;

    triggerSNFSubscriptions ();
}

void VSNFTask::DoAnOldFashionedSNF () {
    issueWarningMessage ("Selector", &selector (), "Not Found");

    if (verboseSelectorNotFound ()) {
	VContainerHandle::Reference pRecipientStore;
	self().store ()->getContainerHandle (pRecipientStore);

	report (
	    "\n    Selector Recipient: #%s[%u:%u]\n\n",
	    pRecipientStore->RTypeName (),
	    pRecipientStore->spaceIndex (),
	    pRecipientStore->containerIndex ()
	);

	unsigned int xLevel = 1;
	VCall const* pCall = 0;
	VTask* pCaller = this;
	while (IsntNil (pCaller)) {
	    pCaller->reportInfo (xLevel++, pCall);
	    pCall = pCaller->call ();
	    pCaller = pCaller->caller ();
	}
    }
}


void VSNFTask::OnPopulateCompleted () {
    VkDynamicArrayOf<ISingleton::Reference> iTargetArray(cardinality());
    Assign (iTargetArray, NilOf (rtLINK_CType*), duc ());
    m_pSNFTaskImplementation->SetToObjects (m_xParameter, iTargetArray);

    if (m_iQueue.isEmpty ()) {
	m_bRunning = false;
	suspend ();
    }
    else {
	VSNFTaskParameters::Reference rpSNFT;
	m_iQueue.dequeue (rpSNFT);
	m_xParameter = rpSNFT->getxParameter ();

	VDescriptor  rVDatum;
	getParameter (m_xParameter, rVDatum);

	m_pContinuation = &VSNFTask::OnPopulateCompleted;

	beginMessageCall ("copyToClientListAt:", 1);
	loadDucWith (rVDatum);
	commitRecipient ();
	
	loadDucWithRepresentative (remoteFactoryGS ());
	commitParameter ();
	commitCall ();
    }
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void VSNFTask::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    if (m_pContinuation) {
		Continuation pContinuation = m_pContinuation;
		m_pContinuation = NilOf (Continuation);

		(this->*pContinuation) ();
	    }
	    else if (pausedOnExit ())
		return;
	    else
		exit ();
	}
	break;

    default:
	exit ();
	break;
    }
}


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

void VSNFTask::reportInfo (unsigned int xLevel, VCall const* Unused(pContext)) const {
    reportInfoHeader (xLevel);

    report ("SNFer\n");
}

void VSNFTask::reportTrace () const {
    reportTraceHeader ("SNF");
    report ("\n");
}

char const* VSNFTask::description () const {
    return "<---SNFer--->";
}


/*********************************
 *********************************
 *****  External Invocation  *****
 *********************************
 *********************************/

void VSNFTask::startExternalInvocation (ISingleton *pISingleton) {
//  ICE == ICollection Enabled
//  (enabled by default unless the 'VxaNoICE' environment variable is set)
    static bool const bICE = getenv ("VxaICE") || !getenv("VxaNoICE");
    suspend ();

    if (Vxa::ICollection *const pICollection = bICE ? dynamic_cast<Vxa::ICollection*>(pISingleton) : 0) {
	VSNFTaskHolder::Reference const pSNFTaskHolder (new VSNFTaskHolder (this, pICollection));
    } else {
	VSNFTaskHolder::Reference const pSNFTaskHolder (new VSNFTaskHolder (this, pISingleton));
    }
}

VExternalGroundStore *VSNFTask::remoteFactoryGS () {
    if (m_pRemoteFactoryGS.isNil ())
	m_pRemoteFactoryGS.setTo (new VExternalGroundStore (m_pRemoteFactory));
    return m_pRemoteFactoryGS;
}

void VSNFTask::createAndCopyToVariant (unsigned int xParameter) {
    if (!m_bRunning) {
	m_xParameter = xParameter;
	m_bRunning = true;
	VDescriptor  rVDatum;
	getParameter (xParameter, rVDatum);

	m_pContinuation = &VSNFTask::OnPopulateCompleted;

	beginMessageCall ("copyToClientListAt:", 1);
	loadDucWith (rVDatum);
	commitRecipient ();

	loadDucWithRepresentative (remoteFactoryGS ());
	commitParameter ();
	commitCall ();
	resume ();
    }
    else {
	VSNFTaskParameters * pParameters = new VSNFTaskParameters(xParameter);
	m_iQueue.enqueue (pParameters);
    }
}

void VSNFTask::returnImplementationHandle (
    IVSNFTaskImplementation *pImplementation,
    VkDynamicArrayOf<ISingleton::Reference> const & rRemoteFactoryArray
) {
    m_pSNFTaskImplementation = pImplementation;
    m_pRemoteFactory = rRemoteFactoryArray[0];
}


/********************************************
 ********************************************
 *****  External Result Return Helpers  *****
 ********************************************
 ********************************************/

/*****************************************
 *----  Polytype Segment Management  ----*
 *****************************************/

VFragment *VSNFTask::createSegment (object_reference_array_t const &rInjector) {
    rtLINK_CType *const pInjector = rtLINK_RefConstructor (ptoken ());
    unsigned int const sInjector = rInjector.cardinality ();
    if (sInjector > 0) {
	unsigned int xRange = rInjector[0];
	unsigned int sRange = 1;

	for (unsigned int xElement = 1; xElement < sInjector; xElement++) {
	    if (sRange + xRange == rInjector[xElement])
		sRange++;
	    else {
		pInjector->AppendRange (xRange, sRange);
		xRange = rInjector[xElement];
		sRange = 1;
	    }
	}
	if (sRange > 0)
	    pInjector->AppendRange (xRange,sRange);
    }
    pInjector->Close (NewDomPToken (sInjector));

    return (
	0 == m_cSegmentsReceived++ ? loadDucWithFragmentation () : duc().contentAsFragmentation ()
    ).createFragment (pInjector);
}

bool VSNFTask::wrapupSegment () {
    if (allSegmentsReceived ()) {
	dismiss ();
	resume ();
	return true;
    }
    return false;
}

bool VSNFTask::SetSegmentCountTo (unsigned int cSegments) {
    m_cSegmentsExpected = cSegments;
    return wrapupSegment ();
}

/******************************************************
 *----  VSNFTask::ProcessArray<Source_T,Result_T> ----*
 ******************************************************/

template <typename Source_T, typename Result_T> void VSNFTask::ProcessArray (
    VDescriptor &rResult, rtPTOKEN_Handle *pPPT, VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
) const {
    unsigned int const count = rSourceArray.cardinality ();
    if(count==1) {
	rResult.setToConstant (pPPT, codKOT (), static_cast<Result_T>(rSourceArray[0]));
	rpResultArray = 0;
    } else {
	Result_T *pResultArray;
	M_CPD *const pResultUV = NewUV (pPPT, pResultArray);
	for (unsigned int xElement = 0; xElement<count; xElement++) {
	    pResultArray[xElement] = static_cast<Result_T>(rSourceArray[xElement]);
	}
	rResult.setToMonotype (pResultUV);
	pResultUV->release ();
	rpResultArray = pResultArray;
    }
}
template void VSNFTask::ProcessArray<bool,int>           (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<bool> const&, int const*&) const;

template void VSNFTask::ProcessArray<char,int>           (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<char> const&, int const*&) const;
template void VSNFTask::ProcessArray<short,int>          (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<short> const&, int const*&) const;
template void VSNFTask::ProcessArray<int,int>            (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<int> const&, int const*&) const;

template void VSNFTask::ProcessArray<unsigned char,int>  (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<unsigned char> const&, int const*&) const;
template void VSNFTask::ProcessArray<unsigned short,int> (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<unsigned short> const&, int const*&) const;
template void VSNFTask::ProcessArray<unsigned int,int>   (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<unsigned int> const&, int const*&) const;

template void VSNFTask::ProcessArray<float,float>        (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<float> const&, float const*&) const;
template void VSNFTask::ProcessArray<double,double>      (VDescriptor&, rtPTOKEN_Handle*, VkDynamicArrayOf<double> const&, double const*&) const;


static char const * DynamicArrayStringAccessFn(bool reset, va_list pArgs) {
    V::VArgList iArgList (pArgs);

    static VkDynamicArrayOf<V::VString> const  *pStrings;
    static unsigned int xString;
    if (reset) {
        pStrings = iArgList.arg<VkDynamicArrayOf<V::VString> const*>();
        xString = 0;
    }
    else if (xString < pStrings->cardinality())
        return (*pStrings)[xString++];
    return NilOf (char const*);
}

template<> void VSNFTask::ProcessArray<V::VString,V::VString> (
    VDescriptor &rResult, rtPTOKEN_Handle *pPPT, VkDynamicArrayOf<VString> const &rSourceArray, VString const *&rpResultArray
) const {
    rResult.setToCorrespondence (
	pPPT, rtLSTORE_NewStringStore (
            codScratchPad (), DynamicArrayStringAccessFn, &rSourceArray
        )
    );
    rpResultArray = 0;
}

template<> void VSNFTask::ProcessArray<Vxa::ISingleton::Reference,Vxa::ISingleton::Reference> (
    VDescriptor &rResult, rtPTOKEN_Handle *pPPT, VkDynamicArrayOf<Vxa::ISingleton::Reference> const &rSourceArray, Vxa::ISingleton::Reference const *&rpResultArray
) const {
    unsigned int const count = rSourceArray.cardinality ();
    if (count==1) {
//	loadDucWithRepresentative (new VExternalGroundStore (rSourceArray[0]));
	VExternalGroundStore::Reference const pXGS (new VExternalGroundStore (rSourceArray[0]));
	rtVSTORE_Handle::Reference pSurrogate;
	pXGS->getSurrogate (pSurrogate);

	// Return reference to object
	rResult.setToReferenceConstant (pPPT, pSurrogate, pXGS->ptoken_(), 0);
    } else {
	VFragmentation &rFragmentation = loadDucWithFragmentation ();
	for (unsigned int xElement=0; xElement<count; xElement++) {
            // Create a task subset of "1"
	    rtLINK_CType *const pSubset = rtLINK_RefConstructor (pPPT);
	    pSubset->AppendRange(xElement,1);
	    rtPTOKEN_Handle::Reference pSubsetDomain (NewDomPToken (1));
	    pSubset->Close (pSubsetDomain);

            // Create the external ground store
	    VExternalGroundStore::Reference const pXGS (new VExternalGroundStore (rSourceArray[xElement]));
	    rtVSTORE_Handle::Reference pSurrogate;
	    pXGS->getSurrogate (pSurrogate);

            // Return reference to object
	    rFragmentation.createFragment (pSubset)->datum ().setToReferenceConstant (
		pSubsetDomain, pSurrogate, pXGS->ptoken_(), 0
	    );
        }
    }
    rpResultArray = 0;
}


/*****************************************************
 *----  VSNFTask::ReturnArray<Source_T,Result_T> ----*
 *****************************************************/

template <typename Source_T, typename Result_T> void VSNFTask::ReturnArray (
    VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
) {
    ProcessArray (duc (), ptoken (), rSourceArray, rpResultArray);
    dismiss ();
    resume ();
}
template void VSNFTask::ReturnArray<bool,int>          (VkDynamicArrayOf<bool> const&, int const*&);

template void VSNFTask::ReturnArray<char,int>          (VkDynamicArrayOf<char> const&, int const*&);
template void VSNFTask::ReturnArray<short,int>         (VkDynamicArrayOf<short> const&, int const*&);
template void VSNFTask::ReturnArray<int,int>           (VkDynamicArrayOf<int> const&,int const*&);

template void VSNFTask::ReturnArray<unsigned char,int> (VkDynamicArrayOf<unsigned char> const&, int const*&);
template void VSNFTask::ReturnArray<unsigned short,int>(VkDynamicArrayOf<unsigned short> const&, int const*&);
template void VSNFTask::ReturnArray<unsigned int,int>  (VkDynamicArrayOf<unsigned int> const&, int const*&);

template void VSNFTask::ReturnArray<float,float>       (VkDynamicArrayOf<float> const&,float const*&);
template void VSNFTask::ReturnArray<double,double>     (VkDynamicArrayOf<double> const&,double const*&);

template void VSNFTask::ReturnArray<V::VString,V::VString> (VkDynamicArrayOf<VString> const&,VString const*&);

template void VSNFTask::ReturnArray<Vxa::ISingleton::Reference,Vxa::ISingleton::Reference>(
    VkDynamicArrayOf<Vxa::ISingleton::Reference> const&,Vxa::ISingleton::Reference const*&
);


/*******************************************************
 *----  VSNFTask::ReturnSegment<Source_T,Result_T> ----*
 *******************************************************/

template <typename Source_T, typename Result_T> bool VSNFTask::ReturnSegment (
    object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
) {
    VFragment *const pFragment = createSegment (rInjector);
    ProcessArray (pFragment->datum (), pFragment->subset ()->PPT (), rSourceArray, rpResultArray);
    return wrapupSegment ();
}
template bool VSNFTask::ReturnSegment<bool,int>           (object_reference_array_t const &rInjector, VkDynamicArrayOf<bool> const&, int const*&);

template bool VSNFTask::ReturnSegment<char,int>           (object_reference_array_t const &rInjector, VkDynamicArrayOf<char> const&, int const*&);
template bool VSNFTask::ReturnSegment<short,int>          (object_reference_array_t const &rInjector, VkDynamicArrayOf<short> const&, int const*&);
template bool VSNFTask::ReturnSegment<int,int>            (object_reference_array_t const &rInjector, VkDynamicArrayOf<int> const&, int const*&);

template bool VSNFTask::ReturnSegment<unsigned char,int>  (object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned char> const&, int const*&);
template bool VSNFTask::ReturnSegment<unsigned short,int> (object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned short> const&, int const*&);
template bool VSNFTask::ReturnSegment<unsigned int,int>   (object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned int> const&, int const*&);

template bool VSNFTask::ReturnSegment<double,double>      (object_reference_array_t const &rInjector, VkDynamicArrayOf<double> const&, double const*&);
template bool VSNFTask::ReturnSegment<float,float>        (object_reference_array_t const &rInjector, VkDynamicArrayOf<float> const&, float const*&);

template bool VSNFTask::ReturnSegment<V::VString,V::VString>    (object_reference_array_t const &rInjector, VkDynamicArrayOf<VString> const&, VString const*&);


/************************************************
 *----  VSNFTask::ReturnSingleton<Source_T> ----*
 ************************************************/

template <typename Source_T> void VSNFTask::ReturnSingleton (Source_T iSingleton) {
    loadDucWith (iSingleton);
    dismiss ();
    resume ();
}
template void VSNFTask::ReturnSingleton<int>         (int);
template void VSNFTask::ReturnSingleton<double>      (double);
template void VSNFTask::ReturnSingleton<float>       (float);
template void VSNFTask::ReturnSingleton<char const*> (char const*);


/********************************
 *----  VSNFTask::ReturnNA  ----*
 ********************************/

bool VSNFTask::ReturnNA (object_reference_array_t const &rInjector) {
    VFragment *const pFragment = createSegment (rInjector);
    pFragment->datum ().setToNA (pFragment->subset ()->PPT (), codKOT ());
    return wrapupSegment ();
}

/***************************************
 *----  VSNFTask::ReturnObject(s)  ----*
 ***************************************/

void VSNFTask::ProcessObjects (
    VDescriptor &rResult, rtPTOKEN_Handle *pPPT, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects
) const {
    VExternalGroundStore::Reference const pXGS (new VExternalGroundStore (codSpace (), pCluster, sCluster));
    rtVSTORE_Handle::Reference pSurrogate;
    pXGS->getSurrogate (pSurrogate);

    unsigned int const count = rxObjects.cardinality ();
    if (count == 1) {
//	loadDucWithRepresentative (pXGS, rxObjects[0]);
	rResult.setToReferenceConstant (pPPT, pSurrogate, pXGS->ptoken_(), rxObjects[0]);
    } else {
	M_CPD *const pResultUV = rtREFUV_New (pPPT, pXGS->ptoken_());
	rtREFUV_ElementType *const pResultArray = rtREFUV_CPD_Array (pResultUV);
	for (unsigned int xElement = 0; xElement<count; xElement++) {
	    pResultArray[xElement] = rxObjects[xElement];
	}
	rResult.setToMonotype (pSurrogate, pResultUV);
	pResultUV->release ();
    }
}

bool VSNFTask::ReturnObjects (
    object_reference_array_t const &rInjector, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects
) {
    VFragment *const pFragment = createSegment (rInjector);
    ProcessObjects (pFragment->datum (), pFragment->subset ()->PPT (), pCluster, sCluster, rxObjects);
    return wrapupSegment ();
    }

void VSNFTask::ReturnObjects (ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects) {
    ProcessObjects (duc (), ptoken (), pCluster, sCluster, rxObjects);
    dismiss ();
    resume ();
}

void VSNFTask::ReturnObject (ICollection *pCluster, object_reference_t sCluster, object_reference_t xObject) {
    loadDucWithRepresentative (new VExternalGroundStore (codSpace (), pCluster, sCluster), xObject);
    dismiss ();
    resume ();
}

/***********************************
 *----  VSNFTask::ReturnError  ----*
 ***********************************/

void VSNFTask::ReturnError (VString const &rMessage) {
    m_pErrorString.setTo(rMessage);
    setContinuationTo(&VSNFTask::ContinueFromReturnError);
    loadDucWithNA ();
    resume();
}

			     
/*******************
 *----  Other  ----*
 *******************/

void VSNFTask::SetOutput (VkDynamicArrayOf<VString> const & rArray){
    VOutputGenerator iOutputGenerator (this);
    for(unsigned int xElement=0; xElement< rArray.cardinality (); xElement++) {
        iOutputGenerator.putString (rArray[xElement]);
        iOutputGenerator.advance ();
    }
}

void VSNFTask::TurnBackSNFTask () {
    DoAnOldFashionedSNF ();		
    loadDucWithNA ();
    dismiss ();
    resume ();
}

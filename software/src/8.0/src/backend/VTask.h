#ifndef VTask_Interface
#define VTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VComputationUnit.h"

#include "VLinkCReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "RTcontext.h"
#include "RTindex.h"
#include "RTlstore.h"
#include "RTptoken.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTundefuv.h"

#include "VCall.h"
#include "VOutputBuffer.h"
#include "VReferenceableMonotype.h"
#include "VTaskDomain.h"

class VByteCodeScanner;
class VNumericBinary;
class VString;

class VfGuardTool;


/*************************
 *****  Definitions  *****
 *************************/

/***********************************
 *----  VTaskConstructionData  ----*
 ***********************************/

class VTaskConstructionData {
//  Construction
public:
    VTaskConstructionData (
	VCall			*pCall,
	rtLINK_CType		*pSubset,
	M_CPD			*pReordering,
	rtCONTEXT_Constructor	*pContext = 0,
	unsigned int		iAttentionMask = 0
    );

//  Access
public:
    VCall *call () const {
	return m_pCall;
    }
    VTask *caller () const {
	return m_pCall->caller ();
    }
    rtLINK_CType *subset () const {
	return m_pSubset;
    }
    M_CPD *reordering () const {
	return m_pReordering;
    }
    rtCONTEXT_Constructor *context () const {
	return m_pContext;
    }
    unsigned int attentionMask () const {
	return m_iAttentionMask;
    }

    unsigned int parameterCount () const {
	return m_pCall->parameterCount ();
    }

    rtINDEX_Key *subtaskTemporalContext () const {
	return m_pCall->subtaskTemporalContext (m_pSubset, m_pReordering);
    }

//  State
protected:
    VCall			* const m_pCall;
    rtLINK_CType		* const m_pSubset;
    M_CPD			* const m_pReordering;
    rtCONTEXT_Constructor	* const m_pContext;
    unsigned int		  const m_iAttentionMask;
};


/*******************
 *----  VTask  ----*
 *******************/

class ABSTRACT VTask : public VComputationUnit {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VTask, VComputationUnit);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Counters, Parameters, and Switches
public:
    static unsigned int BlockTaskCount;
    static unsigned int PrimitiveTaskCount;
    static unsigned int LargeTaskCount;
    static unsigned int MaxTaskSize;

    static bool TracingExecution;

//  Constants
protected:
    static VSelector const g_iTopSelector;

//  Construction Data
public:
    typedef VTaskConstructionData ConstructionData;

//  Construction
protected:
    VTask (ConstructionData const& rTCData);
    VTask (
	Context*		pContext,
	VDescriptor&		rDatum,
	IOMDriver*		pChannel,
	VComputationScheduler*	pScheduler,
	VTask*			pSpawningTask
    );

private:
    M_ASD *getCodSpace ();

//  Destruction
protected:
    ~VTask ();

//  Query
public:
    bool atOrAbove (VCall const* pCall) const;
    bool atOrAbove (VTask const* pTask) const;
    bool atOrAbove (VComputationUnit const* pUnit) const;

    bool atOrBelow (VCall const* pCall) const;
    bool atOrBelow (VTask const* pTask) const;
    bool atOrBelow (VComputationUnit const* pUnit) const;

    bool domainIs (VTaskDomain const* pDomain) const {
	return m_pDomain == pDomain;
    }
    bool domainIsnt (VTaskDomain const* pDomain) const {
	return m_pDomain != pDomain;
    }

    bool isDivertingOutput () const {
	return m_pOutputBuffer->isDivertingOutput ();
    }

    bool isScalar () const {
	return m_pDomain->isScalar ();
    }

    bool myAvailable () const;

    bool outputBufferDomainIsValid () const {
	return domainIs (m_pOutputBuffer->domain ());
    }
    bool outputBufferDomainIsntValid () const {
	return domainIsnt (m_pOutputBuffer->domain ());
    }

    bool recipientAvailable_ () const;

//  Access
public:
    rtCONTEXT_Constructor *blockContext () const {
	return m_pBlockContext;
    }

    VCall *call () const {
	return static_cast<VCall*>(m_pCreator.referent ());
    }
    VTask *caller () const {
	return m_pCaller;
    }
    VTaskDomain *callerDomain () const {
	return m_pCaller ? m_pCaller->domain () : m_pDomain.operator->();
    }
    rtLINK_CType *callerSubset () const {
	return callerDomain () != m_pDomain ? m_pDomain->subset () : 0;
    }
    M_CPD *callerReordering () const {
	return callerDomain () != m_pDomain ? m_pDomain->reordering () : 0;
    }

    unsigned int cardinality () const {
	return m_pDomain->cardinality ();
    }
    unsigned int cardinality_() const {
	return cardinality ();
    }

    IOMDriver* channel_() const;
    IOMDriver* channel () const {
	return m_pOutputBuffer->channel ();
    }

    M_ASD *codSpace () const {
	return m_pCodSpace ? m_pCodSpace : ((VTask*)(this))->getCodSpace ();
    }

    M_AND *codDatabase () const {
	return codSpace ()->Database ();
    }
    M_KOT *codKOT () const {
	return codSpace ()->KOT ();
    }

    M_ASD *codScratchPad () const {
	return codSpace ()->ScratchPad ();
    }

    VComputationUnit* consumer_ () const;
    VComputationUnit* consumer  () const {
	return m_pCaller ? call ()->consumer () : 0;
    }

    VComputationUnit* cuc () {
	return m_pCuc ? (VComputationUnit*)m_pCuc.operator->() : this;
    }

    bool datumAvailable_ () const;

    VTaskDomain *domain () const {
	return m_pDomain;
    }

    M_ASD *domSpace () const {
	return ptoken ()->Space ();
    }

    M_AND *domDatabase () const {
	return domSpace ()->Database ();
    }
    M_KOT *domKOT () const {
	return domSpace ()->KOT ();
    }

    M_ASD *domScratchPad () const {
	return domSpace ()->ScratchPad ();
    }

    unsigned int outputBufferCharacterCount (char iDelimiter) const {
	return m_pOutputBuffer->characterCount (iDelimiter);
    }

    unsigned int parameterCount	() const {
	return m_pCaller ? call()->parameterCount () : 0;
    }
    unsigned int parameterCount_() const {
	return parameterCount ();
    }

    M_CPD *ptoken () const {
	return m_pDomain->ptoken ();
    }
    M_CPD *ptoken_() const {
	return ptoken ();
    }

    ReturnCase returnCase () const {
	return m_pCaller ? call()->returnCase () : Return_Value;
    }
    ReturnCase returnCase_ () const {
	return returnCase ();
    }

    VSelector const& selector () const {
	return m_pCaller ? call()->selector_ () : g_iTopSelector;
    }
    VSelector const& selector_ () const;

    rtINDEX_Key *temporalContext () const {
	return m_pTemporalContext;
    }

//  Path Access
public:
    void getCallerSubsetAndReordering (rtLINK_CType*& rpRestriction, M_CPD*& rpReordering) const {
	if (callerDomain () != m_pDomain) {
	    rpRestriction = m_pDomain->subset ();
	    rpReordering  = m_pDomain->reordering ();
	}
	else {
	    rpRestriction = 0;
	    rpReordering  = 0;
	}
    }
    bool getPathToCall (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VCall*& rpCall
    ) const;
    bool getPathToCaller (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VTask*& rpCaller
    ) const;
    bool getPathToConsumer (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpConsumer
    ) const;
    bool getPathToCreator (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpCreator
    ) const;

//  Parameter Access
public:
    bool getParameter (unsigned int xParameter, VDescriptor& rDatum);

//  Execution ...
protected:
    void setStepPoint () {
	m_fAtStepPoint = m_pCuc.isNil ();
    }

    void exit ();
    void fail ();

//  Call Construction
public:
    /*---------------------------------------------------------------------------*
     *  'begin...Call' Notes:
     *
     *  The domain of a call may be an ordered multi-subset of the domain of the
     *  task that creates it (i.e., related by a link).  That multi-subset is
     *  specified by the 'pSubset' arguments to the following calls.  In all cases,
     *  all parameters to a call (including its recipient and optional temporal
     *  context (specified below) must have the same domain as the call or an
     *  error will occur.
     *---------------------------------------------------------------------------*/
    void beginBoundCall (
	M_CPD *pBlock, unsigned int iParameterCount = 0, rtLINK_CType *pSubset = 0
    );

    void beginMessageCall (
	VByteCodeScanner const& rMessageSource, rtLINK_CType *pSubset = 0
    );
    void beginMessageCall (
	char const* pMessageName, unsigned int iParameterCount, rtLINK_CType *pSubset = 0
    );
    void beginMessageCall (unsigned int xMessageName, rtLINK_CType *pSubset = 0);

    void beginHushedMessageCall (
	VByteCodeScanner const& rMessageSource, rtLINK_CType *pSubset = 0
    ) {
	beginMessageCall (rMessageSource, pSubset);
	m_pCuc->hushSNF ();
    }
    void beginHushedMessageCall (
	char const* pMessageName, unsigned int iParameterCount, rtLINK_CType *pSubset = 0
    ) {
	beginMessageCall (pMessageName, iParameterCount, pSubset);
	m_pCuc->hushSNF ();
    }
    void beginHushedMessageCall (unsigned int xMessageName, rtLINK_CType *pSubset = 0) {
	beginMessageCall (xMessageName, pSubset);
	m_pCuc->hushSNF ();
    }

    void beginValueCall (unsigned int iParameterCount, rtLINK_CType *pSubset = 0);

    void commitRecipient (VMagicWord xRecipient) {
	m_pDuc = m_pCuc->bindRecipient (xRecipient);
    }
    void commitRecipient () {
	commitRecipient (VMagicWord_Datum);
    }

    void commitParameter () {
	m_pDuc = m_pCuc->bindParameter ();
    }

    void commitParameters (unsigned int xFirstParameter = 0);

    void commitCall (ReturnCase xReturnCase, rtINDEX_Key *pTemporalContext, unsigned int xCallerPC = UINT_MAX);
    void commitCall (ReturnCase xReturnCase, unsigned int xCallerPC) {
	commitCall (xReturnCase, NilOf (rtINDEX_Key*), xCallerPC);
    }
    void commitCall (ReturnCase xReturnCase = Return_Value) {
	commitCall (xReturnCase, NilOf (rtINDEX_Key*));
    }
    void commitCall (rtINDEX_Key *pTemporalContext) {
	commitCall (Return_Value, pTemporalContext);
    }

    void sendUnaryMessageToCurrent	(char const * pMessageName);
    void sendUnaryMessageToSelf		(char const * pMessageName);
    void sendBinaryConverseWithSelf	(char const * pMessageName);
    void sendBinaryConverseWithCurrent	(char const * pMessageName);
    void sendBinaryConverseWithSelf	(unsigned int xMessageName);
    void sendBinaryConverseWithCurrent	(unsigned int xMessageName);

//  Domain Prep
public:
    M_CPD *NewDomPToken (unsigned int iCardinality) const {
	return m_pDomain->NewPToken (iCardinality);
    }
    M_CPD *NewDomPToken () const {
	return m_pDomain->NewPToken ();
    }

//  Duc Prep
public:
    M_CPD *NewCodPToken (unsigned int iCardinality) const {
	return rtPTOKEN_New (codScratchPad (), iCardinality);
    }
    M_CPD *NewCodPToken () const {
	return NewCodPToken (cardinality ());
    }

    rtLINK_CType *NewSubset () const {
	return rtLINK_RefConstructor (ptoken (), -1);
    }

    M_CPD *NewUV (M_CPD *pPPT, char *&rpElements) const {
	M_CPD *pUV = NewCharUV (pPPT);
	rpElements = rtCHARUV_CPD_Array (pUV);
	return pUV;
    }
    M_CPD *NewUV (M_CPD *pPPT, double *&rpElements) const {
	M_CPD *pUV = NewDoubleUV (pPPT);
	rpElements = rtDOUBLEUV_CPD_Array (pUV);
	return pUV;
    }
    M_CPD *NewUV (M_CPD *pPPT, float *&rpElements) const {
	M_CPD *pUV = NewFloatUV (pPPT);
	rpElements = rtFLOATUV_CPD_Array (pUV);
	return pUV;
    }
    M_CPD *NewUV (M_CPD *pPPT, int *&rpElements) const {
	M_CPD *pUV = NewIntUV (pPPT);
	rpElements = rtINTUV_CPD_Array (pUV);
	return pUV;
    }

    M_CPD *NewCharUV (M_CPD *pPPT) const {
	return rtCHARUV_New (pPPT, codKOT ()->TheCharacterPTokenCPD ());
    }
    M_CPD *NewDateUV (M_CPD *pPPT) const {
	return rtINTUV_New (pPPT, codKOT ()->TheDatePTokenCPD ());
    }
    M_CPD *NewDoubleUV (M_CPD *pPPT) const {
	return rtDOUBLEUV_New (pPPT, codKOT ()->TheDoublePTokenCPD ());
    }
    M_CPD *NewFloatUV (M_CPD *pPPT) const {
	return rtFLOATUV_New (pPPT, codKOT ()->TheFloatPTokenCPD ());
    }
    M_CPD *NewIntUV (M_CPD *pPPT) const {
	return rtINTUV_New (pPPT, codKOT ()->TheIntegerPTokenCPD ());
    }
    M_CPD *NewUndefUV (M_CPD *pPPT) const {
	return rtUNDEFUV_New (pPPT, codKOT ()->TheNAPTokenCPD ());
    }

    M_CPD *NewCharUV (unsigned int nElements) const {
	M_CPD *pPPT = NewCodPToken (nElements);
	M_CPD *pUV = NewCharUV (pPPT);
	pPPT->release ();
	return pUV;
    }

    M_CPD *NewCharUV () const {
	return NewCharUV (ptoken ());
    }
    M_CPD *NewDateUV () const {
	return NewDateUV (ptoken ());
    }
    M_CPD *NewDoubleUV () const {
	return NewDoubleUV (ptoken ());
    }
    M_CPD *NewFloatUV () const {
	return NewFloatUV (ptoken ());
    }
    M_CPD *NewIntUV () const {
	return NewIntUV (ptoken ());
    }
    M_CPD *NewUndefUV () const {
	return NewUndefUV (ptoken ());
    }

//  Duc Update
public:
    void loadDucWithSelf	() const;
    void loadDucWithCurrent	() const;
    void loadDucWithMy		() const;

    void loadDucWithSuper	() const;

    void loadDucWithDate	() const;
    void loadDucWithLocal	();

    void loadDucWithCoerced	(rtINDEX_Key *pTemporalContext) const;
    void loadDucWithCoerced	(DSC_Descriptor &rValue) const;

    void loadDucWithDouble (M_CPD *pStore, M_CPD *pRPT, double iValue) const {
	m_pDuc->setToConstant (ptoken (), pStore, pRPT, iValue);
    }
    void loadDucWithDouble (M_KOT *pKOT, double iValue) const {
	m_pDuc->setToConstant (ptoken (), pKOT, iValue);
    }
    void loadDucWithDouble (double iValue) const {
	loadDucWithDouble (codKOT (), iValue);
    }

    void loadDucWithFloat (M_CPD *pStore, M_CPD *pRPT, float iValue) const {
	m_pDuc->setToConstant (ptoken (), pStore, pRPT, iValue);
    }
    void loadDucWithFloat (M_KOT *pKOT, float iValue) const {
	m_pDuc->setToConstant (ptoken (), pKOT, iValue);
    }
    void loadDucWithFloat (float iValue) const {
	loadDucWithFloat (codKOT (), iValue);
    }

    void loadDucWithInteger (M_CPD *pStore, M_CPD *pRPT, int iValue) const {
	m_pDuc->setToConstant (ptoken (), pStore, pRPT, iValue);
    }
    void loadDucWithInteger (M_KOT *pKOT, int iValue) const {
	m_pDuc->setToConstant (ptoken (), pKOT, iValue);
    }
    void loadDucWithInteger (int iValue) const {
	loadDucWithInteger (codKOT (), iValue);
    }

    void loadDucWithDate (M_KOT *pKOT, int iValue) const {
	m_pDuc->setToConstant (ptoken (), pKOT->TheDateClass, iValue);
    }
    void loadDucWithDate (int iValue) const {
	loadDucWithDate (codKOT (), iValue);
    }

    void loadDucWithPrimitive (M_KOT *pKOT, unsigned int xPrimitive) const {
	m_pDuc->setToConstant (ptoken (), pKOT->ThePrimitiveClass, (int)xPrimitive);
    }
    void loadDucWithPrimitive (unsigned int xPrimitive) const {
	loadDucWithPrimitive (codKOT (), xPrimitive);
    }

    void loadDucWithSelector (M_CPD *pBlock, unsigned int xSelector) const {
	loadDucWithInteger (pBlock, pBlock->TheSelectorPToken (), xSelector);
    }
    void loadDucWithSelector (M_KOT *pKOT, unsigned int xSelector) const {
	m_pDuc->setToConstant (ptoken (), pKOT->TheSelectorClass, (int)xSelector);
    }
    void loadDucWithSelector (unsigned int xSelector) const {
	loadDucWithSelector (codKOT (), xSelector);
    }

    void loadDucWithReference (M_CPD *pStore, M_CPD *pRPT, unsigned int iValue) const {
	m_pDuc->setToReferenceConstant (ptoken (), pStore, pRPT, iValue);
    }

    void loadDucWithRepresentative (VGroundStore* pStore, unsigned int xRepresentative) const;
    void loadDucWithRepresentative (VGroundStore* pStore) const {
	loadDucWithRepresentative (pStore, 0);
    }

    void loadDucWithUndefined (M_KOTE const &rKOTE) const {
	m_pDuc->setToConstant (ptoken (), rKOTE);
    }
    void loadDucWithUndefined (M_KOTM pKOTM) const {
	loadDucWithUndefined (codKOT ()->*pKOTM);
    }

    void loadDucWithNA (M_KOT *pKOT) const {
	m_pDuc->setToNA (ptoken (), pKOT);
    }
    void loadDucWithNA () const {
	loadDucWithNA (codKOT ());
    }

    void loadDucWithTrue (M_KOT *pKOT) const {
	m_pDuc->setToTrue (ptoken (), pKOT);
    }
    void loadDucWithTrue () const {
	loadDucWithTrue (codKOT ());
    }

    void loadDucWithFalse (M_KOT *pKOT) const {
	m_pDuc->setToFalse (ptoken (), pKOT);
    }	
    void loadDucWithFalse () const {
	loadDucWithFalse (codKOT ());
    }

    void loadDucWithBoolean (M_KOT *pKOT, bool iBoolean) const {
	m_pDuc->setToBoolean (ptoken (), pKOT, iBoolean);
    }
    void loadDucWithBoolean (bool iBoolean) const {
	loadDucWithBoolean (codKOT (), iBoolean);
    }

    void loadDucWithGuardedDouble (double iValue) const {
	if (finite (iValue))
	    loadDucWithDouble (iValue);
	else
	    loadDucWithNA ();
    }

protected:
    void loadDucWithCorrespondence (M_CPD *pStore, unsigned int xRPT, M_CPD *pPPT) const {
	m_pDuc->setToCorrespondence (pPPT, pStore, xRPT);
    }
public:
    void loadDucWithCorrespondence (M_CPD *pStore, unsigned int xRPT) const {
	m_pDuc->setToCorrespondence (ptoken (), pStore, xRPT);
    }

protected:
    void loadDucWithIdentity (M_CPD *pStore, M_CPD *pPPT) const;
public:
    void loadDucWithIdentity (M_CPD *pStore) const {
	loadDucWithIdentity (pStore, ptoken ());
    }

protected:
    void loadDucWithIdentity (VConstructor* pStore, M_CPD *pPPT) const;
public:
    void loadDucWithIdentity (VConstructor* pStore) const {
	loadDucWithIdentity (pStore, ptoken ());
    }

protected:
    void loadDucWithListOrStringStore (M_CPD *pStore, M_CPD *pPPT) const {
	loadDucWithCorrespondence (pStore, rtLSTORE_CPx_RowPToken, pPPT);
    }
public:
    void loadDucWithListOrStringStore (M_CPD *pStore) const {
	loadDucWithCorrespondence (pStore, rtLSTORE_CPx_RowPToken);
    }

    void loadDucWithNewLStore (M_CPD *pContentPrototypeCPD = 0) const;

    void loadDucWithMonotype (M_CPD *pMonotype) const {
	m_pDuc->setToMonotype (pMonotype);
    }
    void loadDucWithMonotype (M_CPD *pStore, M_CPD *pMonotype) const {
	m_pDuc->setToMonotype (pStore, pMonotype);
    }
    void loadDucWithMonotype (M_CPD *pStore, rtLINK_CType *pMonotype) const {
	m_pDuc->setToMonotype (pStore, pMonotype);
    }

protected:
    VFragmentation &loadDucWithFragmentation (M_CPD *pPPT) const;
public:
    VFragmentation &loadDucWithFragmentation () const {
	return loadDucWithFragmentation (ptoken ());
    }

    VDescriptor *loadDucWithGuardedDatum (rtLINK_CType *pGuard) const;

    void loadDucWithBoolean (rtLINK_CType *pTrueSubset) const;

    void loadDucWithPartialCorrespondence (
	M_CPD *pStore, unsigned int xRPT, rtLINK_CType *pSubset
    ) const;

    void loadDucWithPartialFunction (rtLINK_CType *pLink, M_CPD *pUVector) const;
    void loadDucWithPartialFunction (VfGuardTool &rGuard, M_CPD *pUVector) const;

    void loadDucWithPredicateResult (
	M_KOTM pKOTM1, rtLINK_CType *pLink1, M_KOTM pKOTM2, rtLINK_CType *pLink2
    ) const;

    void loadDucWithPredicateResult (
	rtLINK_CType *pTrueSubset, rtLINK_CType *pFalseSubset
    ) const {
	loadDucWithPredicateResult (
	    &M_KnownObjectTable::TheTrueClass , pTrueSubset,
	    &M_KnownObjectTable::TheFalseClass, pFalseSubset
	);
    }

    void loadDucWithParameter (unsigned int xParameter) const;

    void loadDucWithNextParameter (VSelector const *pSelector = 0);
    bool loadDucWithNextParameterAsMonotype () {
	loadDucWithNextParameter ();
	return ConvertDucVectorsToMonotypeIfPossible ();
    }

//  Duc Update (loadDucWith overloads)
public:
    void loadDucWith (double iValue) const {
	loadDucWithDouble (iValue);
    }
    void loadDucWith (float iValue) const {
	loadDucWithFloat (iValue);
    }
    void loadDucWith (int iValue) const {
	loadDucWithInteger (iValue);
    }
    void loadDucWith (unsigned int iValue) const {
	loadDucWithInteger ((int)iValue);
    }

    void loadDucWith (VRunTimeType *pRTT) const;
    void loadDucWith (char const* pString) const;
    void loadDucWith (VString const& rString) const;
    void loadDucWith (VSelector const& rSelector) const;

    void loadDucWith (VDescriptor const& rValue) const {
	loadDucWithCopied (rValue);
    }
    void loadDucWith (DSC_Descriptor const& rValue) const {
	loadDucWithCopied (rValue);
    }
    void loadDucWith (VGroundStore* pGroundStore) const {
	loadDucWithIdentity (pGroundStore->surrogateCPD (), pGroundStore->ptoken_ ());
    }

//  Operator Processing
public:
    void process (VNumericBinary& rOperator);

//  Output Generation
public:
    VOutputBuffer* getOutputBuffer ();

    void __cdecl printf  (size_t size, char const* format, ...);
    void vprintf (size_t size, char const* format, va_list ap);

    void __cdecl outputBufferPrintScalar  (size_t size, char const* format, ...);
    void outputBufferVPrintScalar (size_t size, char const* format, va_list ap);

    void pushOutputDiversion () {
	m_pOutputBuffer.setTo (
	    new VOutputBuffer (m_pDomain, m_pOutputBuffer, true)
	);
    }

    void popOutputDiversion (char* pOutputReturnBuffer, char iDelimiter) {
	m_pOutputBuffer->moveOutputTo (pOutputReturnBuffer, iDelimiter);
	m_pOutputBuffer.setTo (m_pOutputBuffer->parent ());
    }


//  Parameter Management
public:
    void skipParameter () { if (m_xNextParameter > 0) m_xNextParameter--; }

//  Context Management
public:
    //  ^self/^current/^my:
    bool cgetSelf		(VDescriptor &rResultHolder) const;
    bool cgetCurrent	(VDescriptor &rResultHolder) const;
    bool cgetMy		(VDescriptor &rResultHolder) const;

    DSC_Descriptor &getSelf () const {
	return m_pBlockContext->getSelf ();
    }
    DSC_Descriptor &getCurrent () const {
	return m_pBlockContext->getCurrent ();
    }
    DSC_Descriptor &getMy () const {
	return m_pBlockContext->getMy ();
    }

    DSC_Descriptor &self () const {
	return m_pBlockContext->self ();
    }
    DSC_Descriptor &current () const {
	return m_pBlockContext->current ();
    }
    DSC_Descriptor &my () const {
	return m_pBlockContext->my ();
    }

    //  ^date
    DSC_Descriptor &getDate () const {
	m_pTemporalContext->RealizeSequence ();
	return rtINDEX_Key_Sequence (m_pTemporalContext);
    }

    //	^local
    void setLocalContextTo (DSC_Descriptor const& rMonotype) {
	m_pLocalContext.setTo (new VReferenceableMonotype (rMonotype));
    }

    VReferenceableMonotype* getLocalContext ();

    DSC_Descriptor &getLocal () {
	return getLocalContext ()->monotype ();
    }

//  Display and Description
public:
    void __cdecl display (char const* pFormat, ...) const;
    void __cdecl report  (char const* pFormat, ...) const;

    virtual void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const = 0;
    virtual void reportTrace	() const = 0;

    virtual char const* description () const = 0;

    virtual void decompilation		(VString& rString, VCall const* pContext) const;
    virtual void decompilationPrefix	(VString& rString, VCall const* pContext) const;
    virtual void decompilationSuffix	(VString& rString, VCall const* pContext) const;

    virtual unsigned int decompilationIndent (VCall const* pContext) const;
    virtual unsigned int decompilationOffset (VCall const* pContext) const;

    virtual void dumpByteCodes (VCall const* pContext) const;

protected:
    void reportInfoHeader	(unsigned int xLevel) const;
    void reportTraceHeader	(char const* pWhatKindOfTaskAmI) const;

//  Exception and Warning Generation
public:
    void issueWarningMessage (
	char const* pPrefixString, VSelector const* pSelector, char const* pSuffixString
    ) const;

//  State
protected:
    VReference<VTaskDomain> const	m_pDomain;
    rtCONTEXT_Constructor *const	m_pBlockContext;
    rtINDEX_Key *const			m_pTemporalContext;
    VReference<VReferenceableMonotype>	m_pLocalContext;
    M_ASD*				m_pCodSpace;
    VReference<VOutputBuffer>		m_pOutputBuffer;
    VReference<VCall>			m_pCuc;
    unsigned int			m_xNextParameter;
};


#endif

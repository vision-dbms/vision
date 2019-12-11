#ifndef VCall_Interface
#define VCall_Interface

/************************
 *****  Components  *****
 ************************/

#include "VComputationUnit.h"

#include "VCachedArray.h"
#include "VDescriptor.h"
#include "VLinkCReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VMagicWordDescriptor.h"

class rtBLOCK_Handle;
class rtINDEX_Key;

class VTask;


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VCall : public VComputationUnit {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VCall, VComputationUnit);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Types
public:

/*---------------------------------------------------------------------------*
 *	Elements of the 'Type' enumeration denote the implementation strategy
 *	employed by a call.  Valid values are:
 *
 *	Type_Message		- a message call.
 *	Type_Evaluation		- a value call.
 *	Type_Bound		- a bound block call.
 *---------------------------------------------------------------------------*/
    enum Type {
	Type_Message, Type_Evaluation, Type_Bound
    };

//  Switches
public:
    static bool g_bSendingValueMessages;


//  Construction
protected:
    VCall (
	VTask* pCaller, rtLINK_CType* pCallerSubset, Type xType, unsigned int iParameterCount
    );

//  Destruction
protected:
    ~VCall ();

//  Completion
public:
    VDescriptor* bindParameter () {
	m_pDuc = m_xDuc < m_iParameterArray.count ()
	    ? &m_iParameterArray[m_xDuc++]
	    : &m_rDatum;
	return m_pDuc;
    }

    /*---------------------------------------------------------------------------*
     *	'bindRecipient' specifies the set of objects to which the call's selector
     *	is sent when the call's implementation is bound.  Valid recipients are the
     *	valid magic words plus 'MagicWord_Datum' which, in this context, denotes
     *	the value contained in the call's datum descriptor when the call is bound.
     *	Internally, 'MagicWord_Unspecified' is used by this class to denote a call
     *	whose recipient has not yet been specified.
     *---------------------------------------------------------------------------*/
    VDescriptor* bindRecipient (VMagicWord xRecipient) {
	m_xRecipient = xRecipient;
	return bindParameter ();
    }

    VCall* commit (
	ReturnCase xReturnCase, rtINDEX_Key* pTemporalContext, unsigned int xCallerPC
    );

    void hushSNF () {
	m_fSNFHushed = true;
    }

//  Query
public:
    virtual bool atOrAbove (VCall const* pCall) const OVERRIDE;
    virtual bool atOrAbove (VTask const* pTask) const OVERRIDE;
    virtual bool atOrAbove (VComputationUnit const* pUnit) const OVERRIDE;

    virtual bool atOrBelow (VCall const* pCall) const OVERRIDE;
    virtual bool atOrBelow (VTask const* pTask) const OVERRIDE;
    virtual bool atOrBelow (VComputationUnit const* pUnit) const OVERRIDE;

//  Access
public:
    virtual rtBLOCK_Handle *boundBlock () const;

    virtual unsigned int cardinality_ () const OVERRIDE;

    void callerDecompilation		(VString& rString) const;
    void callerDecompilationPrefix	(VString& rString) const;
    void callerDecompilationSuffix	(VString& rString) const;

    unsigned int callerDecompilationIndent () const;
    unsigned int callerDecompilationOffset () const;

    static unsigned int CallerPCOf (VCall const *pCall) {
        return pCall ? pCall->callerPC () : UINT_MAX;
    }
    unsigned int callerPC () const {
	return m_xCallerPC;
    }

    rtLINK_CType* callerSubset () const {
	return m_pCallerSubset;
    }

    virtual IOMDriver* channel_ () const OVERRIDE;

    virtual VComputationUnit* consumer_ () const OVERRIDE;
    VComputationUnit* consumer  () const {
	return m_pConsumer;
    }

    virtual bool datumAvailable_ () const OVERRIDE;

    void dumpCallerByteCodes () const;

    unsigned int parameterCount	() const {
	return m_iParameterArray.count ();
    }
    virtual unsigned int parameterCount_() const OVERRIDE {
	return parameterCount ();
    }

    rtPTOKEN_Handle *ptoken () const;
    virtual rtPTOKEN_Handle *ptoken_() const OVERRIDE {
	return ptoken ();
    }

    virtual bool recipientAvailable_ () const OVERRIDE;

    VMagicWord recipientSource () const {
	return (VMagicWord)m_xRecipient;
    }

    char const* recipientSourceName () const;

    ReturnCase returnCase () const {
	return (ReturnCase)m_xReturnCase;
    }
    virtual ReturnCase returnCase_ () const OVERRIDE {
	return returnCase ();
    }

    bool snfHushed () const {
	return m_fSNFHushed;
    }

    rtLINK_CType* subtaskSubset (rtLINK_CType* pSubset) const {
	return m_pCallerSubset ? 0 : pSubset;
    }

    void subtaskParameter (
	rtLINK_CType*	pSubset,
	M_CPD*		pReordering,
	unsigned int	xParameter,
	VDescriptor&	rDestination
    ) {
	rDestination.setToReorderedSubset (
	    pReordering, subtaskSubset (pSubset), m_iParameterArray[xParameter]
	);
    }

    rtINDEX_Key* subtaskTemporalContext (
	rtLINK_CType* pSubset, M_CPD* pReordering
    ) const;

    rtINDEX_Key* temporalContext () const {
	return m_pTemporalContext;
    }

    Type type () const {
	return (Type)m_xType;
    }

//  Datum/Path Access
public:
    virtual bool getParameter (unsigned int xParameter, VDescriptor& rDatum) OVERRIDE;

    bool crecipient (VDescriptor& rDatum) const;
    bool csearchOrigin (VDescriptor& rDatum) const;

    virtual bool getPathToCaller (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VTask*& rpCaller
    ) const OVERRIDE;

    virtual bool getPathToConsumer (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpConsumer
    ) const OVERRIDE;
    virtual bool getPathToCreator (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, VComputationUnit*& rpCreator
    ) const OVERRIDE;

//  Execution...
protected:
    virtual void fail () OVERRIDE;
    virtual void run () OVERRIDE;

//  ... Consumer Control
public:
    void failConsumer () const {
	m_pConsumer->resumeFailed ();
    }
    void resumeConsumer () const {
	m_pConsumer->resume ();
    }
    void suspendConsumer () {
	m_pConsumer->suspend ();
    }

//  ... Starter
protected:
    void start (VComputationUnit* pSubtask) {
	suspendConsumer ();
	pSubtask->resume ();
    }

//  General Monotype Evaluators:
protected:
    void evaluate (
	rtLINK_CType*		pSubset,
	M_CPD*			pReordering,
	DSC_Descriptor const&	rEvaluable,
	bool		fValueCallbackAllowed
    );

    void evaluate (
	rtLINK_CType*		pSubset,
	M_CPD*			pReordering,
	DSC_Descriptor const&	rRecipient,
	DSC_Descriptor const&	rSearchOrigin,
	unsigned int		iSkipCount = 0
    );

    void evaluate (rtLINK_CType* pSubset, DSC_Descriptor const& rRecipient);

//  Magic Monotype Evaluators:
protected:
    /*---------------------------------------------------------------------------*
     *  Evaluation using a pair of link-equivalent magic monotypes.  For use	 *
     *	by ^self, ^current, ^super, and ^here.  'm_pCallerSubset' MUST be nil.	 *
     *---------------------------------------------------------------------------*/
    void evaluate (
	DSC_Descriptor const&	rRecipient,
	DSC_Descriptor const&	rSearchOrigin,
	unsigned int		iSkipCount = 0
    ) {
	evaluate (0, 0, rRecipient, rSearchOrigin, iSkipCount);
    }

    /*---------------------------------------------------------------------------*
     *  Evaluation using an un-factored and possibly un-coerced magic monotype	 *
     *	recipient.  'm_pCallerSubset' MUST be nil.				 *
     *---------------------------------------------------------------------------*/
    void evaluate (DSC_Descriptor const& rRecipient, bool fCoercionRequired = false);

//  General Evaluator:
protected:
    void evaluate (rtLINK_CType* pSubset);

//  State
protected:
    VLinkCReference const			m_pCallerSubset;
    VReference<VComputationUnit> const		m_pConsumer;
    Type const					m_xType			: 8;
    VMagicWord					m_xRecipient		: 8;
    ReturnCase					m_xReturnCase		: 8;
    unsigned int				m_fSNFHushed		: 1,
									: 7,
						m_xCallerPC,
						m_xDuc;
    VCachedArray<VDescriptor,(unsigned int)1>	m_iParameterArray;
    rtINDEX_Key*				m_pTemporalContext;
};


#endif

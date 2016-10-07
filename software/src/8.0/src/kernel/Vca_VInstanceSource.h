/**
 * @file
 * Once upon a time, the classes you find in this file were all neatly nested within VInstanceSource<val_t>.  All was ordered
 * and organized in the most sensible of ways.  If that wasn't enough, it thrived in both the land of the Microsoft giant
 * and the land of the setting Sun.  Paradise was ours.  But it was too good to last.  In the land of the VMS wizards,
 * access was violated...
 *
 * <pre>
 * %SYSTEM-F-ACCVIO, access violation, reason mask=00, virtual address=0000000000000004, PC=00000000002F65A0, PS=0000001B
 * %TRACE-F-TRACEBACK, symbolic stack dump follows
 *   image    module    routine             line      rel PC           abs PC
 *  CXX$COMPILER  CLASS_DECL  create_member_using_declaration
 *                                         76400 000000000000E490 00000000002F65A0
 *  CXX$COMPILER  CLASS_DECL  member_using_declaration
 *                                         76816 000000000000F644 00000000002F7754
 *  CXX$COMPILER  CLASS_DECL  scan_class_definition
 *                                         79336 00000000000142A4 00000000002FC3B4
 *  CXX$COMPILER  DECL_SPEC  class_specifier
 *                                         77176 00000000000047E4 0000000000341CE4
 *  CXX$COMPILER  DECL_SPEC  process_nontype_identifier
 *                                         80975 0000000000008E34 0000000000346334
 *  CXX$COMPILER  DECL_SPEC                81137 0000000000346334 0000000000000000
 *  CXX$COMPILER  CLASS_DECL  class_member_declaration
 *                                         77645 00000000000101B4 00000000002F82C4
 *  CXX$COMPILER  CLASS_DECL  scan_class_definition
 *                                         79390 00000000000144B8 00000000002FC5C8
 *  CXX$COMPILER  TEMPLATES  instantiate_class_template
 *                                         70262 0000000000015348 000000000049E128
 *  CXX$COMPILER  TEMPLATES                82600 000000000049E128 0000000000000000
 *  CXX$COMPILER  TEMPLATES  wrapup_templ_decl_state
 *                                         83801 00000000000178BC 00000000004A069C
 *  CXX$COMPILER  TEMPLATES                83803 00000000004A069C 0000000000000000
 *  CXX$COMPILER  TEMPLATES  template_directive_or_declaration
 *                                         89150 000000000001EB58 00000000004A7938
 *  CXX$COMPILER  DECLS  declaration       86591 0000000000013A74 00000000003320D4
 *  CXX$COMPILER  DECLS  namespace_declaration
 *                                         85488 0000000000011690 000000000032FCF0
 *  CXX$COMPILER  DECLS  declaration       86603 000000000001372C 0000000000331D8C
 *  CXX$COMPILER  DECLS  translation_unit  88166 00000000000165C8 0000000000334C28
 *  CXX$COMPILER  TRANS_UNIT  process_translation_unit
 *                                         64718 0000000000000A08 00000000004B6D68
 *  CXX$COMPILER  CFE  edg_main            68252 00000000000000FC 00000000002E7FBC
 *  CXX$COMPILER  COMPILE  gem_xx_compile 206952 00000000000024AC 0000000000281EEC
 *  CXX$COMPILER  GEM_CP_VMS  GEM_CP_MAIN   2603 00000000000018CC 0000000000568EFC
 *  CXX$COMPILER                               0 000000000087D2FC 000000000087D2FC
 *                                             0 FFFFFFFF80381CE4 FFFFFFFF80381CE4
 * %TRACE-I-END, end of TRACE stack dump
 * </pre>
 *
 * Alas.  Paradise lost.
 *
 */

#ifndef Vca_VInstanceSource_Interface
#define Vca_VInstanceSource_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VInstanceSink.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IError.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
/*****************************************************************
 *----  template <typename Val_T> class VInstanceSourceBase  ----*
 *****************************************************************/

    template <typename Val_T> class VInstanceSourceElement;

    template <typename Val_T> class VInstanceSourceBase {
	DECLARE_FAMILY_MEMBERS (VInstanceSourceBase<Val_T>, void);

	friend class VInstanceSourceElement<Val_T>;

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef VInstanceSink<Val_T> sink_t;

    //  Construction
    protected:
	VInstanceSourceBase () {
	}

    //  Destruction
    protected:
	~VInstanceSourceBase () {
	}

    //  Query
    public:
	bool validatesDatum (Val_T iValue) const {
	    return validatesDatum_(iValue);
	}
	bool validatesError (IError *pInterface, VString const &rMessage) const {
	    return validatesError_(pInterface, rMessage);
	}
    protected:
	virtual bool validatesDatum_(Val_T iValue) const {
	    return VTypeInfoHolder<Val_T>::validates (iValue);
	}
	virtual bool validatesError_(IError *pInterface, VString const &rMessage) const {
	    return true;
	}

    //  Element Instantiation
    private:
	virtual void newDatum (Val_T iValue) = 0;
	virtual void newError (IError *pInterface, VString const &rMessage) = 0;
	virtual void newFuture(sink_t const &rSink) = 0;
	virtual void reset () = 0;
    };


/********************************************************************
 *----  template <typename Val_T> class VInstanceSourceElement  ----*
 ********************************************************************/

    template <typename Val_T> class VInstanceSourceElement : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VInstanceSourceElement<Val_T>, VReferenceable);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef VInstanceSink<Val_T> sink_t;
	typedef VInstanceSourceBase<Val_T> source_t;

    //  Construction
    protected:
	VInstanceSourceElement () {
	}

    //  Destruction
    protected:
	~VInstanceSourceElement () {
	}

    //  Query
    private:
	virtual bool coversDatum_(Val_T iValue) const {
	    return holdsDatum (iValue);
	}
	virtual bool coversError_(IError *pInterface, VString const &rMessage) const {
	    return holdsError (pInterface, rMessage);
	}

	virtual bool holdsDatum_(Val_T iValue) const {
	    return false;
	}
	virtual bool holdsError_(IError *pInterface, VString const &rMessage) const {
	    return false;
	}

	virtual bool isDatum_() const {
	    return false;
	}
	virtual bool isError_() const {
	    return false;
	}
	virtual bool isFuture_() const {
	    return false;
	}
    public:
	bool coversDatum (Val_T iValue) const {
	    return coversDatum_(iValue);
	}
	bool coversError (IError *pInterface, VString const &rMessage) const {
	    return coversError_(pInterface, rMessage);
	}

	bool holdsDatum (Val_T iValue) const {
	    return holdsDatum_(iValue);
	}
	bool holdsError (IError *pInterface, VString const &rMessage) const {
	    return holdsError_(pInterface, rMessage);
	}

	bool isDatum () const {
	    return isDatum_();
	}
	bool isError () const {
	    return isError_();
	}
	bool isFuture () const {
	    return isFuture_();
	}

    //  Use
    private:
	virtual void setDatum_(source_t *pSource, Val_T iValue) {
	    pSource->newDatum (iValue);
	}
	virtual void setError_(source_t *pSource, IError *pInterface, VString const &rMessage) {
	    pSource->newError (pInterface, rMessage);
	}
	virtual bool supply_(source_t *pSource, sink_t const &rCallback) = 0;
	virtual void unset_(source_t *pSource) {
	    pSource->reset ();
	}
    public:
	void setTo (source_t *pSource, Val_T iValue) {
	    setDatum_(pSource, iValue);
	}
	void setTo (source_t *pSource, IError *pInterface, VString const &rMessage) {
	    setError_(pSource, pInterface, rMessage);
	}
	bool supply (source_t *pSource, sink_t const &rSink) {
	    return supply_(pSource, rSink);
	}
	void unset (source_t *pSource) {
	    unset_(pSource);
	}

    //  Protection Shims
    protected:
	void newDatum (source_t *pSource, Val_T iValue) {
	    pSource->newDatum (iValue);
	}
	void newError (source_t *pSource, IError *pInterface, VString const &rMessage) {
	    pSource->newError (pInterface, rMessage);
	}
	void newFuture(source_t *pSource, sink_t const &rSink) {
	    pSource->newFuture (rSink);
	}
	void reset (source_t *pSource) {
	    pSource->reset ();
	}
    };


/*******************************************************************************************************************
 *----  template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceSource  ----*
 *******************************************************************************************************************/

    /**
     * Value propagation mechanism.
     * Propagates values (including error values) to callbacks, caching values as appropriate. VInstanceSource stores:
     *  - A cached value (which may be a Datum, Error)
     *  - A set of callbacks that will be fired when a value is available (a Future).
     * When a callback is provided, one of two things happen: (1) if a cached value exists, it will be supplied immediately via the given callback; or (2) if a cached value does not exist, the callback will be stored and invoked when a value becomes available.
     */
    template<typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceSource : public VInstanceSourceBase<Val_T> {
	typedef BOGUS_TYPENAME VInstanceSource<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, VInstanceSourceBase<Val_T>);

    //  Aliases
    public:
	typedef Val_T DataType;
	typedef Val_T val_t;
	typedef Var_T var_t;
	typedef VInstanceSink<Val_T> sink_t;
	typedef BaseClass source_t;
	typedef typename sink_t::List CallbackList;

	typedef VInstanceSourceElement<Val_T> Element;
	typedef typename Element::Reference ElementReference;

    //*******************************************************************
    public:
        /**
         * Used to store our actual data value.
         */
	class Datum : public Element {
	public:
	    using Element::RTT;
	    DECLARE_CONCRETE_RTTLITE (Datum, Element);

	//  Construction
	public:
	    Datum (Val_T iValue) : m_iValue (iValue) {
	    }

	//  Destruction
	private:
	    ~Datum () {
	    }

	//  Access
	public:
	    Val_T value () const {
		return m_iValue;
	}

	//  Query
	public:
	    bool holdsDatum_(Val_T iValue) const {
		return iValue == m_iValue;
	    }
	private:
	    virtual bool isDatum_() const {
		return true;
	    }

	//  Use
	private:
	    virtual void setDatum_(source_t *pSource, Val_T iValue) {
		m_iValue = iValue;
	    }
            /**
             * Performs validation and returns false upon failure; otherwise, passes value to rSink and returns true.
             */
	    virtual bool supply_(source_t *pSource, sink_t const &rSink) {
		if (!pSource->validatesDatum (m_iValue))
		    return false;

		rSink (m_iValue);
		return true;
	    }

	//  State
	private:
	    Var_T m_iValue;
	};


    //*******************************************************************
    public:
        /**
         * Used to store the occurrance of an error while attempting datum retrieval.
         */
	class Error : public Element {
	public:
	    using Element::RTT;
	    DECLARE_CONCRETE_RTTLITE (Error, Element);

	//  Construction
	public:
	    Error (
		IError *pInterface, VString const &rMessage
	    ) : m_pInterface (pInterface), m_iMessage (rMessage) {
	    }

	//  Destruction
	private:
	    ~Error () {
	    }

	//  Access
	public:
	    IError *errorInterface () const {
		return m_pInterface;
	    }
	    VString const &errorMessage () const {
		return m_iMessage;
	    }

	//  Query
	public:
	    bool holdsError (IError *pInterface, VString const &rMessage) const {
		return pInterface == m_pInterface && rMessage == m_iMessage;
	    }
	private:
	    virtual bool isError_() const {
		return true;
	    }

	//  Use
	private:
	    virtual void setError_(source_t *pSource, IError *pInterface, VString const &rMessage) {
		m_pInterface.setTo (pInterface);
		m_iMessage.setTo (rMessage);
	    }
	    virtual bool supply_(source_t *pSource, sink_t const &rSink) {
		if (!pSource->validatesError (m_pInterface, m_iMessage))
		    return false;

		rSink (m_pInterface, m_iMessage);
		return true;
	    }

	//  State
	private:
	    IError::Reference	m_pInterface;
	    VString		m_iMessage;
	};


    //********************************************************************
    public:
        /**
         * Used to store callbacks to which we want to provide data when we get it.
         */
	class Future : public Element {
	public:
	    using Element::RTT;
	    DECLARE_CONCRETE_RTTLITE (Future, Element);

	//  Construction
	public:
	    Future (sink_t const &rSink) {
		m_pSinks.push (rSink);
	    }

	//  Destruction
	private:
	    ~Future() {
	    }

	//  Query
	private:
	    virtual bool coversDatum_(Val_T iValue) const {
		return true;
	    }
	    virtual bool coversError_(IError *pInterface, VString const &rMessage) const {
		return true;
	    }
	    virtual bool isFuture_() const {
		return true;
	    }

	//  Use
	private:
	    virtual void setDatum_(source_t *pSource, Val_T iValue) {
		Reference iRetainer (this);
		BaseClass::newDatum (pSource, iValue);
		m_pSinks (iValue);
	    }
	    virtual void setError_(source_t *pSource, IError *pInterface, VString const &rMessage) {
		Reference iRetainer (this);
		BaseClass::newError (pSource, pInterface, rMessage);
		m_pSinks (pInterface, rMessage);
	    }
	    virtual bool supply_(source_t *pSource, sink_t const &rSink) {
		m_pSinks.push (rSink);
		return true;
	    }
	    virtual void unset_(source_t *pSource) {
	    }

	//  State
	private:
	    CallbackList m_pSinks;
	};


    //************************************************************
    //  Construction
    public:
	VInstanceSource(Val_T iValue) {
	    setTo (iValue);
	}
	VInstanceSource(IError *pInterface, VString const &rMessage) {
	    setTo (pInterface, rMessage);
	}
	VInstanceSource() {
	}

    //  Destruction
    public:
	~VInstanceSource () {
	};

    //  Access
    public:
        /**
         * Delegates to our element's supply.
         * If we don't have an element, or if the element is no good, create a future.
         */
	template <typename callback_t> void supply (callback_t const &rCallback) {
	    VInstanceSinkInstance<Val_T,callback_t> iSink(rCallback);
	    if (m_pElement.isNil () || !m_pElement->supply (this, iSink))
		newFuture (iSink);
	}
	template <typename class_t> void supplyMembers (
	    class_t *pSink,
	    void (class_t::*pSinkDataMember)(Val_T),
	    void (class_t::*pSinkErrorMember)(IError*, VString const&)
	) {
	    VInstanceMemberCallback<Val_T,class_t> iCallback (pSink, pSinkDataMember, pSinkErrorMember);
	    supply (iCallback);
	}
	void supplyReceiver (IVReceiver<Val_T> *pReceiver) {
	    VInstanceReceiverCallback<Val_T> iCallback(pReceiver);
	    supply (iCallback);
	}

    //  Query
    public:
	bool coversDatum (Val_T iValue) const {
	    return m_pElement.isNil () || m_pElement->coversDatum (iValue);
	}
	bool coversError (IError *pInterface, VString const &rMessage) const {
	    return m_pElement.isNil () || m_pElement->coversError (pInterface, rMessage);
	}

	bool holdsNothing () const {
	    return m_pElement.isNil ();
	}
	bool holdsSomething () const {
	    return m_pElement.isntNil ();
	}

	bool holdsDatum (Val_T iValue) const {
	    return m_pElement && m_pElement->holdsDatum (iValue);
	}
	bool holdsDatum () const {
	    return m_pElement && m_pElement->isDatum ();
	}

	bool holdsError (IError *pInterface, VString const &rMessage) const {
	    return m_pElement && m_pElement->holdsError (pInterface, rMessage);
	}
	bool holdsError () const {
	    return m_pElement && m_pElement->isError ();
	}

	bool holdsFuture () const {
	    return m_pElement && m_pElement->isFuture ();
	}

	bool validatesDatum (Val_T iValue) const {
	    return coversDatum (iValue) && BaseClass::validatesDatum (iValue);
	}
	bool validatesError (IError *pInterface, VString const &rMessage) const {
	    return coversError (pInterface, rMessage) && BaseClass::validatesError (pInterface, rMessage);
	}

    //  Update
    public:
	void setTo (Val_T iValue) {
	    if (m_pElement)
		m_pElement->setTo (this, iValue);
	    else
		newDatum (iValue);
	}
	void setTo (IError *pInterface, VString const &rMessage) {
	    if (m_pElement)
		m_pElement->setTo (this, pInterface, rMessage);
	    else
		newError (pInterface, rMessage);
	}
	void unset () {
	    if (m_pElement)
		m_pElement->unset (this);
	}

    //  Update Helpers
    private:
	void newDatum (Val_T iValue) {
	    m_pElement.setTo (new Datum (iValue));
	}
	void newError (IError *pInterface, VString const &rMessage) {
	    m_pElement.setTo (new Error (pInterface, rMessage));
	}
	void newFuture(sink_t const &rSink) {
	    reset ();
	    m_pElement.setTo (new Future (rSink));
	    onValueNeeded ();
	}
	void reset () {
	    if (m_pElement) {
		m_pElement.clear ();
		onValueReset ();
	    }
	}

    //  Update Notification
    private:
	virtual void onValueNeeded () {
	}
	virtual void onValueReset () {
	}

    //  State
    private:
	ElementReference m_pElement;
    };
}


#endif

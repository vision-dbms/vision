#ifndef Vca_VInstanceSink_Interface
#define Vca_VInstanceSink_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"
#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IError;

/***********************************************************
 *----  template <typename Val_T> class VInstanceSink  ----*
 ***********************************************************/

    template<typename Val_T> class VInstanceSink {
	DECLARE_NUCLEAR_FAMILY (VInstanceSink<Val_T>);

    //  Aliases
    public:
	typedef ThisClass sink_t;
	typedef Val_T val_t;

    //  List Class
    public:
	class List {
	    DECLARE_NUCLEAR_FAMILY (List);

	public:
	    class Entry : public VReferenceable {
		DECLARE_ABSTRACT_RTTLITE (Entry, VReferenceable);

		friend class List;

	    //  Construction
	    protected:
		Entry () {
		}

	    //  Destruction
	    protected:
		~Entry () {
		}

	    //  Invocation
	    public:
		virtual void operator () (Val_T iValue) const = 0;
		virtual void operator () (IError *pInterface, VString const &rMessage) const = 0;

	    //  State
	    private:
		Reference m_pSuccessor;
	    };
	    typedef typename Entry::Reference EntryReference;

	//  Construction
	public:
	    List () {
	    }

	//  Destruction
	public:
	    ~List () {
	    }

	//  Use
	public:
	    inline void push (sink_t const &rSink);
	private:
	    bool pop (EntryReference &rpCallback) {
		return m_pEntries.interlockedPop (rpCallback, &Entry::m_pSuccessor);
	    }
	public:
	    void operator () (Val_T iValue) {
		EntryReference pEntry;
		while (pop (pEntry))
		    (*pEntry) (iValue);
	    }
	    void operator () (IError *pInterface, VString const &rMessage) {
		EntryReference pEntry;
		while (pop (pEntry))
		    (*pEntry) (pInterface, rMessage);
	    }

	//  State
	private:
	    EntryReference m_pEntries;
	};
	typedef typename List::Entry ListEntry;
	friend class List;

    //  Construction/Destruction
    protected:
	VInstanceSink () {
	}
	~VInstanceSink () {
	}

    //  Use
    private:
	virtual ListEntry *newFuture () const = 0;
    public:
	virtual void operator () (Val_T iValue) const = 0;
	virtual void operator () (IError *pIError, VString const &rMessage) const = 0;
    };

    template<typename Val_T> void VInstanceSink<Val_T>::List::push (sink_t const &rSink) {
	m_pEntries.interlockedPush (rSink.newFuture (), &Entry::m_pSuccessor);
    }


/*******************************************************************
 *----  template <typename Val_T> class VInstanceSinkInstance  ----*
 *******************************************************************/

    template <typename Val_T, typename Callback_T> class VInstanceSinkInstance : public VInstanceSink<Val_T> {
    public:
	typedef BOGUS_TYPENAME VInstanceSinkInstance<Val_T,Callback_T> this_t;
	typedef BOGUS_TYPENAME VInstanceSink<Val_T> base_t;
	DECLARE_FAMILY_MEMBERS (this_t, base_t);

    //  Aliases
    public:
	typedef typename BaseClass::ListEntry	ListEntryBase;
	typedef Val_T 				val_t;
	typedef Callback_T 			callback_t;

	class ConcreteListEntry : public ListEntryBase {
	public:
	    using ListEntryBase::RTT;
	    DECLARE_CONCRETE_RTTLITE (ConcreteListEntry, ListEntryBase);

	//  Construction
	public:
	    ConcreteListEntry (callback_t const &rCallback) : m_iCallback (rCallback) {
	    }

	//  Destruction
	private:
	    ~ConcreteListEntry () {
	    }

	//  Use
	private:
	    void operator () (Val_T iValue) const OVERRIDE {
		m_iCallback (iValue);
	    }
	    void operator () (IError *pIError, VString const &rMessage) const OVERRIDE {
		m_iCallback (pIError, rMessage);
	    }

	//  State
	private:
	    callback_t const m_iCallback;
	};

    //  Construction/Destruction
    public:
	VInstanceSinkInstance (callback_t const &rCallback) : m_rCallback (rCallback) {
	}
	~VInstanceSinkInstance () {
	}

    //  Use
    private:
	virtual ListEntryBase *newFuture () const {
	    return new ConcreteListEntry (m_rCallback);
	}
    public:
	void operator () (Val_T iValue) const {
	    m_rCallback (iValue);
	}
	void operator () (IError *pIError, VString const &rMessage) const {
	    m_rCallback (pIError, rMessage);
	}
		
    //  State
    private:
	callback_t const &m_rCallback;
    };


/*********************************************************************************************************************************************
 *----  template <typename Gofer_T, typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceGoferCallback  -----*
 *********************************************************************************************************************************************/

    template <typename Gofer_T, typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceGoferCallback {
	typedef BOGUS_TYPENAME VInstanceGoferCallback<Gofer_T,Val_T,Var_T> this_t;
	DECLARE_NUCLEAR_FAMILY (this_t);

    //  Aliases
    public:
	typedef Gofer_T gofer_t;
	typedef Val_T val_t;
	typedef Var_T var_t;

    //  Construction
    public:
	VInstanceGoferCallback (gofer_t *pGofer, var_t &rVariable) : m_pGofer (pGofer), m_rVariable (rVariable) {
	}
	VInstanceGoferCallback (ThisClass const &rOther) : m_pGofer (rOther.m_pGofer), m_rVariable (rOther.m_rVariable) {
	}

    //  Destruction
    public:
	~VInstanceGoferCallback () {
	}

    //  Use
    public:
	void operator () (Val_T iValue) const {
	    m_rVariable = iValue;
	    m_pGofer->resume ();
	}
	void operator () (IError *pInterface, VString const &rMessage) const {
	    m_rVariable.setTo (pInterface, rMessage);
	    m_pGofer->onError (pInterface, rMessage);
	}

    //  State
    private:
	typename gofer_t::Reference const m_pGofer;
	var_t &m_rVariable;
    };



/***************************************************************************************
 *----  template <typename Val_T, typename Class_T> class VInstanceMemberCallback  ----*
 ***************************************************************************************/

    template<typename Val_T, typename Class_T> class VInstanceMemberCallback {
	typedef BOGUS_TYPENAME VInstanceMemberCallback<Val_T,Class_T> this_t;
	DECLARE_NUCLEAR_FAMILY (this_t);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Class_T class_t;
	typedef void (Class_T::*data_member_t) (Val_T);
	typedef void (Class_T::*error_member_t) (IError*,VString const&);

    //  Construction
    public:
	VInstanceMemberCallback (
	    class_t *pSink, data_member_t pSinkDataMember, error_member_t pSinkErrorMember
	) : m_pSink (pSink), m_pSinkDataMember (pSinkDataMember), m_pSinkErrorMember (pSinkErrorMember) {
	}
	VInstanceMemberCallback (ThisClass const &rOther)
	    : m_pSink (rOther.m_pSink)
	    , m_pSinkDataMember (rOther.m_pSinkDataMember)
	    , m_pSinkErrorMember (rOther.m_pSinkErrorMember)
	{
	}

    //  Destruction
    public:
	~VInstanceMemberCallback () {
	}

    //  Use
    public:
	void operator () (Val_T iValue) const {
	    if (m_pSink)
		(m_pSink.referent ()->*m_pSinkDataMember) (iValue);
	}
	void operator () (IError *pIError, VString const &rMessage) const {
	    if (m_pSink)
		(m_pSink.referent ()->*m_pSinkErrorMember) (pIError, rMessage);
	}

    //  State
    private:
	typename class_t::Reference const m_pSink;
	data_member_t const m_pSinkDataMember;
	error_member_t const m_pSinkErrorMember;
    };


/***********************************************************************
 *----  template <typename Val_T> class VInstanceReceiverCallback  ----*
 ***********************************************************************/

    template<typename Val_T> class VInstanceReceiverCallback {
	DECLARE_FAMILY_MEMBERS (VInstanceReceiverCallback<Val_T>,void);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef IVReceiver<Val_T> receiver_t;

    //  Construction
    public:
	VInstanceReceiverCallback (receiver_t *pReceiver) : m_pReceiver (pReceiver) {
	}
	VInstanceReceiverCallback (ThisClass const &rOther) : m_pReceiver (rOther.m_pReceiver) {
	}

    //  Destruction
    public:
	~VInstanceReceiverCallback () {
	}

    //  Use
    public:
	void operator () (Val_T iValue) const {
	    if (m_pReceiver)
		m_pReceiver->OnData (iValue);
	}
	void operator () (IError *pIError, VString const &rMessage) const {
	    if (m_pReceiver)
		m_pReceiver->OnError (pIError, rMessage);
	}

    //  State
    private:
	typename receiver_t::Reference const m_pReceiver;
    };
}


#endif

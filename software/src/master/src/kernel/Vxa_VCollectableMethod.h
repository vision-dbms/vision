#ifndef Vxa_VCollectableMethod_Interface
#define Vxa_VCollectableMethod_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMethod.h"
#include "Vxa_VTask.h"
#include "Vxa_VCallType1.h"
#include "Vxa_VCallType2.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollectableCollectionOf.h"
#include "Vxa_VImportable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
/****************************************************************
 *----  template <typename T> class VCollectableMethodBase  ----*
 ****************************************************************/

    template <typename T> class VCollectableMethodBase : public VMethod {
	DECLARE_ABSTRACT_RTTLITE (VCollectableMethodBase<T>, VMethod);

    //  Aliases
    public:
	typedef typename VCollectableTraits<T>::val_t val_t;
	typedef typename VCollectableTraits<T>::var_t var_t;
        typedef typename VCollectableTraits<T>::cluster_t cluster_t;
	typedef VScalar<val_t> self_provider_t;

    //  Implementation
    public:
	class Implementation : public VTask {
	    DECLARE_ABSTRACT_RTTLITE (Implementation, VTask);

	//  Construction
	protected:
	    Implementation (
		VCallData const &rCallData, VCollection *pCollection
	    ) : BaseClass (rCallData), m_pCollection (static_cast<cluster_t*>(pCollection)) {
	    }

	//  Destruction
	protected:
	    ~Implementation () {
	    }

	//  Startup
	protected:
	    template <class importer_t> bool startUsing (importer_t &rImporter) {
		return rImporter.getSelfProviderFor (this, m_pCollection.referent (), m_pSelfProvider)
		    && BaseClass::startUsing (rImporter);
	    }

	//  Use
	protected:
	    val_t self () const {
		return m_pSelfProvider->value ();
	    }

	//  State
	private:
	    typename cluster_t::Reference const m_pCollection;
	    typename self_provider_t::Reference m_pSelfProvider;
	};

    //  Construction
    protected:
	VCollectableMethodBase (VString const &rName) : BaseClass (rName) {
	}

    //  Destruction
    protected:
	~VCollectableMethodBase () {
	}
    };


/**************************************************************************
 *----  template <typename T,P1,...P16> class VCollectableMethodBase  ----*
 **************************************************************************/

    template <typename T,
	      typename P1 =Unused, typename P2 =Unused, typename P3 =Unused, typename P4 =Unused,
	      typename P5 =Unused, typename P6 =Unused, typename P7 =Unused, typename P8 =Unused,
	      typename P9 =Unused, typename P10=Unused, typename P11=Unused, typename P12=Unused,
	      typename P13=Unused, typename P14=Unused, typename P15=Unused, typename P16=Unused
    > class VCollectableMethod : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (
		VCallData const &rCallData, VCollection *pCollection, method_t *pMethod
	    ) : BaseClass (rCallData, pCollection), m_pMethod (pMethod) {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && m_iP12.retrieve (this, rImporter)
		    && m_iP13.retrieve (this, rImporter)
		    && m_iP14.retrieve (this, rImporter)
		    && m_iP15.retrieve (this, rImporter)
		    && m_iP16.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1 , m_iP2 , m_iP3 , m_iP4 ,	m_iP5 , m_iP6 , m_iP7 , m_iP8 ,
			m_iP9 , m_iP10, m_iP11, m_iP12,	m_iP13, m_iP14, m_iP15, m_iP16
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear(); m_iP12.clear();
		m_iP13.clear(); m_iP14.clear(); m_iP15.clear(); m_iP16.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	    typename VImportable<P12>::Instance m_iP12;
	    typename VImportable<P13>::Instance m_iP13;
	    typename VImportable<P14>::Instance m_iP14;
	    typename VImportable<P15>::Instance m_iP15;
	    typename VImportable<P16>::Instance m_iP16;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (pCollection);
	}
    private:
	member_t const m_pMember;
    };


/****************************************************************************************
 *----  template <typename T...> class VCollectableMethodBase<T...> Specializations ----*
 ****************************************************************************************/

    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13, typename P14, typename P15
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && m_iP12.retrieve (this, rImporter)
		    && m_iP13.retrieve (this, rImporter)
		    && m_iP14.retrieve (this, rImporter)
		    && m_iP15.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1 , m_iP2 , m_iP3 , m_iP4 , m_iP5 , m_iP6 , m_iP7 , m_iP8 ,
			m_iP9 , m_iP10, m_iP11, m_iP12,	m_iP13, m_iP14, m_iP15
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear(); m_iP12.clear();
		m_iP13.clear(); m_iP14.clear(); m_iP15.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	    typename VImportable<P12>::Instance m_iP12;
	    typename VImportable<P13>::Instance m_iP13;
	    typename VImportable<P14>::Instance m_iP14;
	    typename VImportable<P15>::Instance m_iP15;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13, typename P14
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && m_iP12.retrieve (this, rImporter)
		    && m_iP13.retrieve (this, rImporter)
		    && m_iP14.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1 , m_iP2 , m_iP3 , m_iP4 ,	m_iP5 , m_iP6 , m_iP7 , m_iP8 ,
			m_iP9 , m_iP10, m_iP11, m_iP12,	m_iP13, m_iP14
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear(); m_iP12.clear();
		m_iP13.clear(); m_iP14.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	    typename VImportable<P12>::Instance m_iP12;
	    typename VImportable<P13>::Instance m_iP13;
	    typename VImportable<P14>::Instance m_iP14;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && m_iP12.retrieve (this, rImporter)
		    && m_iP13.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1 , m_iP2 , m_iP3 , m_iP4 ,	m_iP5 , m_iP6 , m_iP7 , m_iP8 ,
			m_iP9 , m_iP10, m_iP11, m_iP12,	m_iP13
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear(); m_iP12.clear();
		m_iP13.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	    typename VImportable<P12>::Instance m_iP12;
	    typename VImportable<P13>::Instance m_iP13;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && m_iP12.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1 , m_iP2 , m_iP3 , m_iP4 ,	m_iP5 , m_iP6 , m_iP7 , m_iP8 ,
			m_iP9 , m_iP10, m_iP11, m_iP12
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear(); m_iP12.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	    typename VImportable<P12>::Instance m_iP12;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && m_iP11.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6, m_iP7, m_iP8, m_iP9, m_iP10, m_iP11
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear(); m_iP11.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	    typename VImportable<P11>::Instance m_iP11;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && m_iP10.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6, m_iP7, m_iP8, m_iP9, m_iP10
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear (); m_iP10.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	    typename VImportable<P10>::Instance m_iP10;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename  T, typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6, typename P7, typename P8, typename P9 
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && m_iP9.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(
			m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6, m_iP7, m_iP8, m_iP9
		    );
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear ();
		m_iP9.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	    typename VImportable<P9>::Instance m_iP9;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename  T, typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6, typename P7, typename P8 
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7,P8);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && m_iP8.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6, m_iP7, m_iP8);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear (); m_iP8.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	    typename VImportable<P8>::Instance m_iP8;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6,P7);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && m_iP7.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6, m_iP7);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear (); m_iP7.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	    typename VImportable<P7>::Instance m_iP7;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 
    > class VCollectableMethod<T,P1,P2,P3,P4,P5,P6> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5,P6);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && m_iP6.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3, m_iP4, m_iP5, m_iP6);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear (); m_iP6.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	    typename VImportable<P6>::Instance m_iP6;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3, typename P4, typename P5
    > class VCollectableMethod<T,P1,P2,P3,P4,P5> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4,P5);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && m_iP5.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3, m_iP4, m_iP5);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear ();
		m_iP5.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	    typename VImportable<P5>::Instance m_iP5;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

    template <typename T,
	      typename P1, typename P2, typename P3, typename P4 
    > class VCollectableMethod<T,P1,P2,P3,P4> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3,P4> this_t;
	typedef void (T::*member_t)(P1,P2,P3,P4);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && m_iP4.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3, m_iP4);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear (); m_iP4.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	    typename VImportable<P4>::Instance m_iP4;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3
    > class VCollectableMethod<T,P1,P2,P3> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2,P3> this_t;
	typedef void (T::*member_t)(P1,P2,P3);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && m_iP3.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2, m_iP3);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear (); m_iP3.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	    typename VImportable<P3>::Instance m_iP3;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T, typename P1, typename P2> class VCollectableMethod<T,P1,P2> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1,P2> this_t;
	typedef void (T::*member_t)(P1,P2);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && m_iP2.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1, m_iP2);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear (); m_iP2.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	    typename VImportable<P2>::Instance m_iP2;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T, typename P1> class VCollectableMethod<T,P1> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T,P1> this_t;
	typedef void (T::*member_t)(P1);
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (VCallData const &rCallData, VCollection *pCollection, method_t *pMethod)
		: BaseClass (rCallData, pCollection), m_pMethod (pMethod)
	    {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    template <typename ImporterType> bool startUsingImpl (ImporterType &rImporter) {
		return m_iP1.retrieve (this, rImporter)
		    && BaseClass::startUsing (rImporter);
	    }
	    virtual bool startUsing (VCallType1Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool startUsing (VCallType2Importer &rImporter) OVERRIDE {
		return startUsingImpl (rImporter);
	    }
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)(m_iP1);
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		m_iP1.clear();
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	    typename VImportable<P1>::Instance m_iP1;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T> class VCollectableMethod<T> : public VCollectableMethodBase<T> {
    public:
	typedef VCollectableMethodBase<T> base_t;
	typedef VCollectableMethod<T> this_t;
	typedef void (T::*member_t)();
	typedef this_t method_t;
	DECLARE_CONCRETE_RTTLITE (this_t,base_t);
    public:
	class Implementation : public base_t::Implementation {
	    DECLARE_CONCRETE_RTTLITE (Implementation, typename base_t::Implementation);
	public:
	    Implementation (
		VCallData const &rCallData, VCollection *pCollection, method_t *pMethod
	    ) : BaseClass (rCallData, pCollection), m_pMethod (pMethod) {
	    }
	protected:
	    ~Implementation () {
	    }
	private:
	    virtual bool run () OVERRIDE {
		member_t const pMember = m_pMethod->member ();
		while (BaseClass::moreToDo ()) {
		    (BaseClass::self ()->*pMember)();
		    BaseClass::next ();
		}
		if (BaseClass::isAlive ())
		    BaseClass::onSuccess ();
		else {
		    VString iMessage ("Vxa Remote Invocation Failure: ");
		    iMessage<<(m_pMethod->name ());
		    BaseClass::onFailure (0, iMessage);
		}
		return true;
	    }
	private:
	    typename method_t::Reference const m_pMethod;
	};
    public:
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName), m_pMember (pMember) {
	}
    protected:
	~VCollectableMethod () {
	}
    //  Access
    public:
	member_t member () const {
	    return m_pMember;
	}
    private:
	template <class CallHandle> bool invokeImpl (CallHandle rCallHandle, VCollection *pCollection) {
	    typename Implementation::Reference pTask (new Implementation (rCallHandle, pCollection, this));
	    return rCallHandle.start (pTask);
	}
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) OVERRIDE {
	    return invokeImpl (rCallHandle, pCollection);
	}
    private:
	member_t const m_pMember;
    };


/***************************************************************************************************
 *----  template <typename void(T::*)(...)> class VCollectableMethodBase<T...> Specializations ----*
 ***************************************************************************************************/

    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13, typename P14, typename P15, typename P16
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13, typename P14, typename P15 
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13, typename P14
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12,
	      typename P13 
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11, typename P12
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10, typename P11 
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 , typename P10
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1 , typename P2 , typename P3 , typename P4 ,
	      typename P5 , typename P6 , typename P7 , typename P8 ,
	      typename P9 
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8,P9> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1, typename P2, typename P3, typename P4,
	      typename P5, typename P6, typename P7, typename P8
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7,P8)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7,P8> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 
    > class VCollectableMethod<
	void (T::*)(P1,P2,P3,P4,P5,P6,P7)
    > : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6,P7> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T,
	      typename P1, typename P2, typename P3, typename P4,
	      typename P5, typename P6
    > class VCollectableMethod<void(T::*)(P1,P2,P3,P4,P5,P6)> : public VCollectableMethod<T,P1,P2,P3,P4,P5,P6> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5,P6> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3, typename P4, typename P5 
    > class VCollectableMethod<void(T::*)(P1,P2,P3,P4,P5)> : public VCollectableMethod<T,P1,P2,P3,P4,P5> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4,P5> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3, typename P4
    > class VCollectableMethod<void(T::*)(P1,P2,P3,P4)> : public VCollectableMethod<T,P1,P2,P3,P4> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3,P4> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2, typename P3 
    > class VCollectableMethod<void(T::*)(P1,P2,P3)> : public VCollectableMethod<T,P1,P2,P3> {
    public:
	typedef VCollectableMethod<T,P1,P2,P3> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1, typename P2
    > class VCollectableMethod<void(T::*)(P1,P2)> : public VCollectableMethod<T,P1,P2> {
    public:
	typedef VCollectableMethod<T,P1,P2> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <
	typename T, typename P1
    > class VCollectableMethod<void(T::*)(P1)> : public VCollectableMethod<T,P1> {
    public:
	typedef VCollectableMethod<T,P1> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    template <typename T> class VCollectableMethod<void(T::*)()> : public VCollectableMethod<T> {
    public:
	typedef VCollectableMethod<T> base_t;
	typedef typename base_t::member_t member_t;
	VCollectableMethod (VString const &rName, member_t pMember) : base_t (rName, pMember) {
	}
    };
} //  namespace Vxa


#endif

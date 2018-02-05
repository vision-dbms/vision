/*****  Vca_VRolePlayer Implementation  *****/

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

#include "Vca_VRolePlayer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_ITrigger.h"

#include "Vca_VcaSelf.h"
#include "Vca_VcaGofer.h"
#include "Vca_VcaThreadState.h"

#include "Vca_VClassInfoHolder.h"
#include "Vca_VInterfaceEKG.h"
#include "Vca_VStatus.h"

#include "VkRunTimes.h"
#include "VkSocketAddress.h"


/********************************
 *****  Sun Linker Pragmas  *****
 ********************************
 *****
 *****  The Solaris compilers sometimes generate two (or more) manglings for the same symbol.
 *****  When that happens, the linker complains about undefined symbols.  If it happens in
 *****  code used to build a shared library, the linker won't complain until it attempts to
 *****  link a main program using that library.  The workaround for this problem is to make
 *****  the undefined mangled names equivalent to the one and only definition for the symbol.
 *****  That is done using the '#pragma weak "undefined" = "defined"' directive.
 *****
 *****  To find the actual managled names to substitute, run 'nm -Clno LIBRARY'.  Somewhere
 *****  in its output, you should find multiple entries for the symbol, one defined, the
 *****  others not.  Here, for example, are the slightly edited entries for one of the
 *****  symbols the compiler just started mishandling -- 'Vca::VRolePlayer::QueryInterface':
 *****  one of the symbols which the compiler recently started mishandling:
 *****
 *****
 *****     [66797] ...|NOTY |GLOB |0    |UNDEF  |void Vca::VRolePlayer::QueryInterface(IVUnknown*,VTypeInfo*,IVReceiver<IVUnknown*>*)
 *****			       [__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4C3___v_]
 *****     [69596] ...|FUNC |GLOB |0    |8      |void Vca::VRolePlayer::QueryInterface(IVUnknown*,VTypeInfo*,IVReceiver<IVUnknown*>*)
 *****			       [__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4Cp2___v_]
 *****
 *****  If you look carefully, you can see that one of the symbols is UNDEF and the other
 *****  isn't.  The fix is to use the #pragma weak directive to set the value of the
 *****  undefined mangled name to the value of the defined one.  Note the use of "'s in
 *****  these #pragma equivalences.  They surround each of the mangled names and are
 *****  required
 *****
 *****/
#ifdef __sparc
// void Vca::VRolePlayer::QueryRole(Vca::IRoleProvider*,IVReceiver<IVUnknown*>*,VTypeInfo*,IVUnknown*)
#pragma weak "__1cDVcaLVRolePlayerJQdDueryRole6Mpn0ANIRoleProvider_pn0AIITrigger_pnKIVReceiver4CpnJIVUnknown___pnJVTypeInfo_7_v_" = "__1cDVcaLVRolePlayerJQdDueryRole6Mpn0ANIRoleProvider_pn0AIITrigger_pnKIVReceiver4CpnJIVUnknown___pnJVTypeInfo_p6_v_"

// void Vca::VRolePlayer::QueryInterface(IVUnknown*,VTypeInfo*,IVReceiver<IVUnknown*>*)
#pragma weak "__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4C3___v_" = "__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4Cp2___v_"

// void Vca::VRolePlayer::QueryRole2(Vca::IRoleProvider2*,IVReceiver<bool>*,IVReceiver<IVUnknown*>*,VTypeInfo*,IVUnknown*)
#pragma weak "__1cDVcaLVRolePlayerKQdDueryRole26Mpn0AOIRoleProvider2_pnKIVReceiver4Cb__pnKIVReceiver4CpnJIVUnknown___pnJVTypeInfo_7_v_" = "__1cDVcaLVRolePlayerKQdDueryRole26Mpn0AOIRoleProvider2_pnKIVReceiver4Cb__pnKIVReceiver4CpnJIVUnknown___pnJVTypeInfo_p6_v_"
#endif


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VRolePlayer::RoleProvider::Query  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

namespace Vca {
    class VRolePlayer::RoleProvider::Query {
	DECLARE_NUCLEAR_FAMILY (Query);

    /*******************************
     *----  class Transaction  ----*
     *******************************/
    public:
	class Transaction : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Transaction, VRolePlayer);

	//  Aliases
	public:
	    typedef VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;

	//  Construction
	protected:
	    Transaction (RoleProvider* pSuccessor);
	    void startUsing (VGoferInterface<IRoleProvider>* pRoleProviderGofer);

	//  Destruction
	protected:
	    ~Transaction ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  ITrigger
	private:
	    VRole<ThisClass,ITrigger> m_pITrigger;
	public:
	    void getRole (ITrigger::Reference &rpRole) {
		m_pITrigger.getRole (rpRole);
	    }
	//  ...  Methods
	public:
	    void Process (ITrigger *pRole);

	//  IStatusSink
	private:
	    VRole<ThisClass,IStatusSink> m_pIStatusSink;
	public:
	    void getRole (IStatusSink::Reference &rpRole) {
		m_pIStatusSink.getRole (rpRole);
	    }

	//  ... Methods
	public:
	    void OnData (IStatusSink *pRole, bool bGood);
	    void OnEnd_();	// ... when received, interpreted as an indication that interface monitoring should be canceled.
	    void OnError_(IError *pInterface, VString const &rMessage);

	//  Callbacks
	private:
	    /****************************************************************
	     ****************           OPEN ISSUE           ****************
	     ****************************************************************
	     *****
	     *****  Should implementors of this routine abort the query
	     *****  (the current practice) or keep searching?
	     *****
	     ****************************************************************/
	    virtual void onError (IError *pInterface, VString const &rMessage) = 0;

	    void onRoleProvider (IRoleProvider *pRoleProvider);

	//  Delegations
	private:
	    virtual void getRoleFrom (RoleProvider *pProvider) const = 0;
	    virtual void getRoleFrom (IRoleProvider *pProvider, ITrigger *pNotFoundSink) const = 0;
	    virtual void getRoleFrom (IRoleProvider2 *pProvider, IStatusSink *pStatusSink) const = 0;

	    /**
	     * Send a message to the source of this query saying that the requested interface was found.
	     * Possible when this query transaction was initiated by an IRoleProvider2::QueryRole2
	     * message.
	     */
	    virtual void sendAck () const = 0;
	    /**
	     * Send a message to the source of this query saying that it isn't possible to know if the
	     * requested interface was found.  This situation occurs when the query associated with this
	     * transaction is delegated to a peer that does not support sending such acknowledgements
	     * (i.e., does not implement IRoleProvider2).
	     */
	    virtual void sendQuack () const = 0;
	protected:
	    static void sendAckTo (Query const &rQuery) {
		rQuery.sendAck ();
	    }
	    static void sendQuackTo (Query const &rQuery) {
		rQuery.sendQuack ();
	    }

	//  Monitoring
	private:
	    void startInterfaceMonitor (IVUnknown *pInterface);
	    void cancelInterfaceMonitor ();
	    void signalInterfaceMonitor ();

	//  State
	private:
	    RoleProvider::Reference const m_pSuccessor;
	    interface_monitor_t::Reference m_pInterfaceMonitor;
	};
	friend class Transaction;

    /************************************************************
     *----  template <typename Query_T> class Transaction_  ----*
     ************************************************************/
    public:
	template <typename Query_T> class Transaction_ : public Transaction {
	    DECLARE_CONCRETE_RTTLITE (Transaction_, Transaction);

	//  Construction
	public:
	    Transaction_(
		RoleProvider* pSuccessor, Query_T const* pQuery, VGoferInterface<IRoleProvider>* pRoleProviderGofer
	    ) : BaseClass (pSuccessor), m_iQuery (*pQuery) {
		m_iQuery.logAsInteresting ();
		BaseClass::startUsing (pRoleProviderGofer);
	    }

	//  Destruction
	private:
	    ~Transaction_() {
	    }

	//  Callbacks
	private:
	    void onError (IError *pInterface, VString const &rMessage) OVERRIDE {
		m_iQuery.onError (pInterface, rMessage);
	    }

	//  Delegations
	private:
	    void getRoleFrom (RoleProvider *pProvider) const OVERRIDE {
		m_iQuery.getRoleFrom (pProvider);
	    }
	    void getRoleFrom (IRoleProvider *pProvider, ITrigger *pNotFoundSink) const OVERRIDE {
		m_iQuery.getRoleFrom (pProvider, pNotFoundSink);
	    }
	    void getRoleFrom (IRoleProvider2 *pProvider, IStatusSink *pStatusSink) const OVERRIDE {
		m_iQuery.getRoleFrom (pProvider, pStatusSink);
	    }
	    /**
	     * Send a message to the source of this query saying that the requested interface was found.
	     * Possible when this query transaction was initiated by an IRoleProvider2::QueryRole2
	     * message.
	     */
	    void sendAck () const OVERRIDE {
		m_iQuery.sendAck ();
	    }
	    /**
	     * Send a message to the source of this query saying that it isn't possible to know if the
	     * requested interface was found.  This situation occurs when the query associated with this
	     * transaction is delegated to a peer that does not support sending such acknowledgements
	     * (i.e., does not implement IRoleProvider2).
	     */
	    void sendQuack () const OVERRIDE {
		m_iQuery.sendQuack ();
	    }

	//  State
	private:
	    Query_T const m_iQuery;
	};

    /****************************************************************/
    //  Construction
    public:
	Query (
	    IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
	);

    //  Destruction
    public:
	~Query ();

    //  Execution
    private:
	/**
	 * Send a message to the source of this query saying that the requested interface was found.
	 * Possible when this query transaction was initiated by an IRoleProvider2::QueryRole2
	 * message.
	 */
	virtual bool sendAck () const = 0;
	/**
	 * Send a message to the source of this query saying that the requested interface was not found.
	 */
	virtual bool sendNack () const = 0;
	/**
	 * Send a message to the source of this query saying that it isn't possible to know if the
	 * requested interface was found.  This situation occurs when the query associated with this
	 * transaction is delegated to a peer that does not support sending such acknowledgements
	 * (i.e., does not implement IRoleProvider2).
	 */
	virtual bool sendQuack () const = 0;
    public:
	virtual bool acceptedBy (VRolePlayer *pProvider) const = 0;
	virtual bool startTransaction (RoleProvider *pSuccessor, VGoferInterface<IRoleProvider> *pRoleProviderGofer) const = 0;

	bool getRoleFrom (VRolePlayer *pRolePlayer) const;
	bool getRoleFrom (RoleProvider *pProvider) const;
	void getRoleFrom (IRoleProvider *pProvider, ITrigger *pNotFoundSink) const;
	void getRoleFrom (IRoleProvider2 *pProvider, IStatusSink *pStatusSink) const;

	void onError (IError *pInterface, VString const &rMessage) const;

    //  Notification Helpers
    public:
	static bool SendAckTo (ITrigger *pStatusSink) {
	    return false;
	}
	static bool SendNackTo (ITrigger *pStatusSink) {
	    if (pStatusSink) {
		pStatusSink->Process ();
		return true;
	    }
	    return false;
	}
	static bool SendQuackTo (ITrigger *pStatusSink) {
	    return false;
	}

	static bool SendAckTo (IStatusSink *pStatusSink) {
	    if (pStatusSink) {
		pStatusSink->OnData (true);
		return true;
	    }
	    return false;
	}

	static bool SendNackTo (IStatusSink *pStatusSink) {
	    if (pStatusSink) {
		pStatusSink->OnData (false);
		return true;
	    }
	    return false;
	}
	static bool SendQuackTo (IStatusSink *pStatusSink) {
	    if (pStatusSink) {
		pStatusSink->OnEnd ();
		return true;
	    }
	    return false;
	}

    //  Interest
    public:
	virtual void logAsInteresting () const;

    //  State
    protected:
	IObjectSink::Reference	const	m_pInterfaceSink;
	VTypeInfo::Reference	const	m_pInterfaceType;
	IVUnknown::Reference	const	m_pAggregator;
	bool			const	m_bSubquery;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::RoleProvider::Query::Query (
    IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
) : m_pInterfaceSink	(pInterfaceSink)
  , m_pInterfaceType	(pInterfaceType)
  , m_pAggregator	(pAggregator)
  , m_bSubquery		(bSubquery)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::RoleProvider::Query::~Query () {
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*==========================================================================================*
 *
 *  All QueryRole/QueryInterface calls wind up here.  The first four parameters of this
 *  routine are described above this class' definition of QueryRole.  The fifth parameter,
 *  bSubquery, indicates that this is routine is being called synchronously to obtain an
 *  interface from an aggregated local object.  Sub-queries are allowed to send found
 *  interfaces and errors to their client but must never fire the not-found trigger, lest
 *  they prematurely and unpredictably abort the search process.
 *
 *  Note that return from this routine does NOT indicate completion of the requested query.
 *  In particular, aggregated interfaces are obtained in the 'getRoleFrom' calls made by
 *  this routine.  Either of those calls could have initiated an asynchronous query to a
 *  remote object that may ultimately respond to the client or search continuation of this
 *  query.
 *  
 *==========================================================================================*/
bool Vca::VRolePlayer::RoleProvider::Query::getRoleFrom (VRolePlayer *pRolePlayer) const {
    bool bFoundOrWorking = true;

    //  First check our local definitions...
    IVUnknown::Reference pInterface;
    if (pRolePlayer->supplyInterface (pInterface, m_pInterfaceType, m_pAggregator)) {
	if (m_pInterfaceSink)
	    m_pInterfaceSink->OnData (pInterface);
	sendAck ();
    } else if (pRolePlayer->m_pRoleProviders)
	bFoundOrWorking = getRoleFrom (pRolePlayer->m_pRoleProviders);
    else if (m_bSubquery)                       //  If this is a subquery, ...
	bFoundOrWorking = false;                //  ... let our caller know we don't implement the role, ...
    else if (!sendNack () && m_pInterfaceSink)  //  ... if we're not sending our nack to someone else and have a client, ...
	    m_pInterfaceSink->OnData (0);       //  ... let that client know we don't implement the role.

//     if (objectsTrace ()) {
// 	VString iTraceMessage;
// 	iTraceMessage.printf (
// 	    "supplyInterface: %s%s%s%s",
// 	    pInterfaceType->name ().content (), pAggregator ? " A" : "", bSubquery ? " SQ" : "", bFoundOrWorking ? " F/W" : ""
// 	);
// 	traceInfo (iTraceMessage);
//     }
    return bFoundOrWorking;
}

bool Vca::VRolePlayer::RoleProvider::Query::getRoleFrom (RoleProvider *pProvider) const {
//  Check the providers as long as no provider accepts the query and there are more providers to try...
    while (pProvider && !pProvider->accepted (this)) {
	pProvider = pProvider->successor ();
    }

//  If this isn't a subquery and we're out of providers, tell someone that we failed...
    bool bFoundOrWorking = true;
    if (pProvider) {                            //  If we're not still working on the query...
    } else if (m_bSubquery)                     //  ... and this this is subquery, ...
	bFoundOrWorking = false;                //  ... let our caller know we don't implement the role:
    else if (!sendNack () && m_pInterfaceSink)  //  ... if we're not sending our nack to someone else and have a client, ...
	m_pInterfaceSink->OnData (0);           //  ... let that client know we don't implement the role.

    return bFoundOrWorking;
}

void Vca::VRolePlayer::RoleProvider::Query::getRoleFrom (IRoleProvider *pProvider, ITrigger *pNotFoundSink) const {
    pProvider->QueryRole (pNotFoundSink, m_pInterfaceSink, m_pInterfaceType, m_pAggregator);
}

void Vca::VRolePlayer::RoleProvider::Query::getRoleFrom (IRoleProvider2 *pProvider, IStatusSink *pStatusSink) const {
    pProvider->QueryRole2 (pStatusSink, m_pInterfaceSink, m_pInterfaceType, m_pAggregator);
}

void Vca::VRolePlayer::RoleProvider::Query::onError (IError *pInterface, VString const &rMessage) const {
    if (m_pInterfaceSink)
	m_pInterfaceSink->OnError (pInterface, rMessage);
}

/***************************
 ***************************
 *****  Interrogation  *****
 ***************************
 ***************************/

void Vca::VRolePlayer::RoleProvider::Query::logAsInteresting () const {
    if (VcaOIDR *const pOIDR = m_pInterfaceSink ? m_pInterfaceSink->oidr () : 0) {
	pOIDR->logAsInteresting ("AQI-IRP-IVR");
    }
    if (VcaOIDR *const pOIDR = m_pAggregator ? m_pAggregator->oidr () : 0) {
	pOIDR->logAsInteresting ("AQI-IRP-AGR");
    }
}


/****************************************************************
 ****************************************************************
 *****                                                      *****
 *****  Vca::VRolePlayer::RoleProvider::Query::Transaction  *****
 *****                                                      *****
 ****************************************************************
 ****************************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::RoleProvider::Query::Transaction::Transaction (
    RoleProvider* pSuccessor
) : m_pSuccessor (pSuccessor), m_pITrigger (this), m_pIStatusSink (this) {
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::startUsing (VGoferInterface<IRoleProvider>* pRoleProviderGofer) {
    retain (); {
	pRoleProviderGofer->supplyMembers (this, &ThisClass::onRoleProvider, &ThisClass::onError);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::RoleProvider::Query::Transaction::~Transaction () {
}

/*********************** 
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VRolePlayer::RoleProvider::Query::Transaction::Process (ITrigger *pRole) {
    getRoleFrom (m_pSuccessor);
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::OnData (IStatusSink *pRole, bool bGood) {
    cancelInterfaceMonitor ();

    if (bGood)
	sendAck ();
    else {
	getRoleFrom (m_pSuccessor);
    }
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::OnEnd_() {
    cancelInterfaceMonitor ();
    sendQuack ();
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::OnError_(IError *pInterface, VString const &rMessage) {
    onError (pInterface, rMessage);
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::onRoleProvider (IRoleProvider *pRoleProvider) {
//  Did we get a type 2 provider?
    if (IRoleProvider2 *const pRoleProvider2 = dynamic_cast<IRoleProvider2*>(pRoleProvider)) {
	startInterfaceMonitor (pRoleProvider2);

	IStatusSink::Reference pStatusSink;
	getRole (pStatusSink);
	getRoleFrom (pRoleProvider2, pStatusSink);
//  Did we get a type 1 provider?
    } else if (pRoleProvider) {
	sendQuack ();

	ITrigger::Reference pStatusSink;
	getRole (pStatusSink);
	getRoleFrom (pRoleProvider, pStatusSink);
//  Did we get no provider at all?
    } else {
	getRoleFrom (m_pSuccessor); // ... move on to the next provider.
    }
}

/************************ 
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VRolePlayer::RoleProvider::Query::Transaction::startInterfaceMonitor (IVUnknown *pInterface) {
    if (m_pInterfaceMonitor)
	m_pInterfaceMonitor->monitor (pInterface);
    else {
	m_pInterfaceMonitor.setTo (new interface_monitor_t (this, &ThisClass::signalInterfaceMonitor));
	m_pInterfaceMonitor->monitor (pInterface);
    }
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::cancelInterfaceMonitor () {
    if (m_pInterfaceMonitor) {
	m_pInterfaceMonitor->cancel ();
	m_pInterfaceMonitor.clear ();
    }
}

void Vca::VRolePlayer::RoleProvider::Query::Transaction::signalInterfaceMonitor () {
    cancelInterfaceMonitor ();
    onError (0, "Partitioned Aggregate Error");
}


/**********************************************************************************
 **********************************************************************************
 *****                                                                        *****
 *****  template <class StatusSink_T> Vca::VRolePlayer::RoleProvider::Query_  *****
 *****                                                                        *****
 **********************************************************************************
 **********************************************************************************/

namespace Vca {
    template <class StatusSink_T> class VRolePlayer::RoleProvider::Query_ : public Query {
	DECLARE_FAMILY_MEMBERS (Query_<StatusSink_T>,Query);

	friend class Transaction_<ThisClass>;

    /****************************************************************/
    //  Construction
    public:
	Query_(
	    StatusSink_T *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
	) : BaseClass (pInterfaceSink, pInterfaceType, pAggregator, bSubquery), m_pStatusSink (pStatusSink) {
	}

    //  Destruction
    public:
	~Query_() {
	}

    //  Execution
    public:
	bool acceptedBy (VRolePlayer *pProvider) const {
	    return pProvider->supplyInterface (m_pStatusSink, m_pInterfaceSink, m_pInterfaceType, m_pAggregator, true);
	}
	bool startTransaction (RoleProvider *pSuccessor, VGoferInterface<IRoleProvider> *pRoleProviderGofer) const {
	    (new Transaction_<ThisClass>(pSuccessor, this, pRoleProviderGofer))->discard ();
	    return true;
	}
    private:
	/**
	 * Send a message to the source of this query saying that the requested interface was found.
	 * Possible when this query transaction was initiated by an IRoleProvider2::QueryRole2
	 * message.
	 */
	bool sendAck () const {
	    return BaseClass::SendAckTo (m_pStatusSink);
	}
	/**
	 * Send a message to the source of this query saying that the requested interface was not found.
	 */
	bool sendNack () const {
	    return BaseClass::SendNackTo (m_pStatusSink);
	}
	/**
	 * Send a message to the source of this query saying that it isn't possible to know if the
	 * requested interface was found.  This situation occurs when the query associated with this
	 * transaction is delegated to a peer that does not support sending such acknowledgements
	 * (i.e., does not implement IRoleProvider2).
	 */
	bool sendQuack () const {
	    return BaseClass::SendQuackTo (m_pStatusSink);
	}

    //  Interest
    public:
	void logAsInteresting () const  {
	    if (VcaOIDR *const pOIDR = m_pStatusSink ? m_pStatusSink->oidr () : 0) {
		pOIDR->logAsInteresting ("AQI-IRP-NFT");
	    }
	    BaseClass::logAsInteresting ();
	}

    //  State
    private:
	typename StatusSink_T::Reference const m_pStatusSink;
    };
}


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VRolePlayer::RoleProvider  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::RoleProvider::RoleProvider (VRolePlayer *pAggregator) : m_pSuccessor (pAggregator->roleProviders ()) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::RoleProvider::~RoleProvider () {
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vca::VRolePlayer::InterfaceBasedRoleProvider  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

namespace Vca {
    class VRolePlayer::InterfaceBasedRoleProvider : public RoleProvider {
	DECLARE_CONCRETE_RTTLITE (InterfaceBasedRoleProvider, RoleProvider);

    //  Construction
    public:
	InterfaceBasedRoleProvider (VRolePlayer* pAggregator, VGoferInterface<IRoleProvider>* pProvider);

    //  Destruction
    private:
	~InterfaceBasedRoleProvider ();

    //  Use
    private:
	bool accepted (Query const *pQuery) const OVERRIDE;

    //  State
    private:
	VGoferInterface<IRoleProvider>::Reference const m_pRoleProviderGofer;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::InterfaceBasedRoleProvider::InterfaceBasedRoleProvider (
    VRolePlayer* pAggregator, VGoferInterface<IRoleProvider>* pProvider
) : BaseClass (pAggregator), m_pRoleProviderGofer (pProvider) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::InterfaceBasedRoleProvider::~InterfaceBasedRoleProvider () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vca::VRolePlayer::InterfaceBasedRoleProvider::accepted (Query const *pQuery) const {
    return pQuery->startTransaction (successor (), m_pRoleProviderGofer);
}


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  Vca::VRolePlayer::RolePlayerBasedRoleProvider  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

namespace Vca {
    class VRolePlayer::RolePlayerBasedRoleProvider : public RoleProvider {
	DECLARE_CONCRETE_RTTLITE (RolePlayerBasedRoleProvider, RoleProvider);

    //  Construction
    public:
	RolePlayerBasedRoleProvider (VRolePlayer *pAggregator, VRolePlayer *pProvider);

    //  Destruction
    private:
	~RolePlayerBasedRoleProvider ();

    //  Use
    private:
	bool accepted (Query const *pQuery) const OVERRIDE;

    //  State
    private:
	VRolePlayer::Reference const m_pProvider;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::RolePlayerBasedRoleProvider::RolePlayerBasedRoleProvider (
    VRolePlayer *pAggregator, VRolePlayer *pProvider
) : BaseClass (pAggregator), m_pProvider (pProvider) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::RolePlayerBasedRoleProvider::~RolePlayerBasedRoleProvider () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vca::VRolePlayer::RolePlayerBasedRoleProvider::accepted (Query const *pQuery) const {
    return pQuery->acceptedBy (m_pProvider);
}


/******************************
 ******************************
 *****                    *****
 *****  Vca::VRolePlayer  *****
 *****                    *****
 ******************************
 ******************************/

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

#ifdef _WIN32
typedef Vca::ProcessInfo::uid_t uid_t;
typedef Vca::ProcessInfo::gid_t gid_t;
#endif

namespace {
    void MySiteInfo (IVReceiver<VString const&> *pResultSink) {
	Vca::self ()->info ()->supplyProcessInfo (pResultSink);
    }
    VString MySiteInfoForAll () {
        VString sResult;
        Vca::self ()->getInfoForAll (sResult, "");
        return sResult;
    }
    VString MyObjectTable () {
        VString sResult;
        Vca::self ()->getObjectTable (sResult, "");
        return sResult;
    }
    VString MyObjectTableForAll () {
        VString sResult;
        Vca::self ()->getObjectTableForAll (sResult, "");
        return sResult;
    }
    VString MyHostName () {
	char iBuffer[257];
	VString sResult (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));
	return sResult;
    }
    VString MyUserName () {
	char iBuffer[257];
	VString sResult (Vk_username (iBuffer, sizeof (iBuffer)));
	return sResult;
    }
    Vca::count_t MyKeepaliveCount () {
	return Vca::self()->keepaliveCount ();
    }
    unsigned int MyBufIO () {
	return VkRunTimes ().bufio ();
    }
    unsigned int MyDirIO () {
	return VkRunTimes ().dirio ();
    }
    unsigned int MyFilcnt () {
	return VkRunTimes ().filcnt ();
    }
    unsigned int MyPageflts () {
	return VkRunTimes ().pageflts ();
    }
    double MyElapsedTime () {
	return VkRunTimes ().elapsed ();
    }
    double MyUserTime () {
	return VkRunTimes ().user ();
    }

    pid_t MyPID () {
	return getpid ();
    }
    uid_t MyUID () {
	return getuid ();
    }
    gid_t MyGID () {
	return getgid ();
    }
}

namespace Vca {
    VClassInfoHolder &VRolePlayer::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addProperty ("classInfo"                     , &ThisClass::classSummary)
		.addProperty ("help"                          , &ThisClass::classSummary)
		.addProperty ("propertyNames"                 , &ThisClass::classPropertyNames)
		.addProperty ("whatAmI"                       , &ThisClass::rttNameShim)

		.addProperty ("referenceCount"                , &ThisClass::referenceCountShim)
		.addProperty ("exportCount"                   , &ThisClass::exportCount)
		.addProperty ("exportedInterfaceCount"        , &ThisClass::exportedInterfaceCount)
		.addProperty ("instantiatedInterfaceCount"    , &ThisClass::instantiatedInterfaceCount)
		.addProperty ("referencedInterfaceCount"      , &ThisClass::referencedInterfaceCount)
		.addProperty ("roleCount"                     , &ThisClass::roleCount)
		.addProperty ("hasInstantiatedInterfaces"     , &ThisClass::hasInstantiatedInterfaces)
		.addProperty ("hasExportedInterfaces"         , &ThisClass::hasExportedInterfaces)
		.addProperty ("cohortRequestCount"            , &ThisClass::cohortRequestCount)
		.addProperty ("cohortTimerCount"              , &ThisClass::cohortTimerCount)
		.addProperty ("cohortDeviceUseCount"          , &ThisClass::cohortUseCount)

		.addProperty ("siteInfo"                      , &MySiteInfo)
		.addProperty ("siteInfoForAll"                , &MySiteInfoForAll)
		.addProperty ("objectTable"                   , &MyObjectTable)
		.addProperty ("objectTableForAll"             , &MyObjectTableForAll)
		.addProperty ("hostname"                      , &MyHostName)
		.addProperty ("username"                      , &MyUserName)
		.addProperty ("KeepaliveCount"                , &MyKeepaliveCount)
		.addProperty ("BufIO"                         , &MyBufIO)
		.addProperty ("DirIO"                         , &MyDirIO)
		.addProperty ("FilCnt"                        , &MyFilcnt)
		.addProperty ("PageFlts"                      , &MyPageflts)
		.addProperty ("Elapsed"                       , &MyElapsedTime)
		.addProperty ("User"                          , &MyUserTime)

		.addProperty ("pid"                           , &MyPID)
		.addProperty ("uid"                           , &MyUID)
		.addProperty ("gid"                           , &MyGID)

		.markCompleted ();
	}
	return iClassInfoHolder;
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRolePlayer::VRolePlayer (
    VCohort *pCohort
) : m_pCohort (pCohort), m_xSSID (VcaSSID::NewIdentity ()), m_pIVUnknown (this), m_pIRoleProvider (this), m_pIGetter (this), m_pIPeek (this) {
    m_pCohort->incrementMemberCount ();
}

Vca::VRolePlayer::VRolePlayer ()
    : m_pCohort (VcaThreadState::Cohort ()), m_xSSID (VcaSSID::NewIdentity ()), m_pIVUnknown (this), m_pIRoleProvider (this), m_pIGetter (this), m_pIPeek (this)
{
    m_pCohort->incrementMemberCount ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VRolePlayer::~VRolePlayer () {
    m_pCohort->decrementMemberCount ();
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VCohort *Vca::VRolePlayer::CohortOf (VRolePlayer const *pRolePlayer) {
    return pRolePlayer ? pRolePlayer->cohorT () : 0;
}

VString Vca::VRolePlayer::classBaseNames () const {
    return classInfo().baseNames ();
}

VString Vca::VRolePlayer::classPropertyNames () const {
    return classInfo().propertyNames ();
}

VString Vca::VRolePlayer::classSummary () const {
    return classInfo().summary ();
}

Vca::count_t Vca::VRolePlayer::cohortRequestCount () const {
    return cohort()->processorRequestCount();
}

Vca::count_t Vca::VRolePlayer::cohortTimerCount () const {
    return cohort()->pendingTimerCount();
}

Vca::count_t Vca::VRolePlayer::cohortUseCount () const {
    return cohort()->deviceManager() ? cohort()->deviceManager()->useCount() : 0;
}

unsigned int Vca::VRolePlayer::referenceCountShim () const {
    return referenceCount ();
}

VString Vca::VRolePlayer::rttNameShim () const {
    return rttName ();
}


/*************************
 *************************
 *****  Aggregation  *****
 *************************
 *************************/

void Vca::VRolePlayer::appendNew (RoleProvider *pProvider) {
    m_pRoleProviders.setTo (pProvider);
}

void Vca::VRolePlayer::aggregate (VRolePlayer* pRoleProvider) {
    if (pRoleProvider)
	appendNew (new RolePlayerBasedRoleProvider (this, pRoleProvider));
}

void Vca::VRolePlayer::aggregate (VGoferInterface<IRoleProvider>* pRoleProvider) {
    if (pRoleProvider)
	appendNew (new InterfaceBasedRoleProvider (this, pRoleProvider));
}

void Vca::VRolePlayer::aggregate (VGoferInterface<IVUnknown>* pRoleProvider) {
    if (pRoleProvider)
	aggregate (new Gofer::Queried<IRoleProvider>(pRoleProvider));
}

void Vca::VRolePlayer::aggregate (IRoleProvider* pRoleProvider) {
    if (pRoleProvider)
//	aggregate (new Gofer::Cast<IRoleProvider>(pRoleProvider));
	aggregate (new Gofer::Queried<IRoleProvider>(pRoleProvider));
}

void Vca::VRolePlayer::aggregate (IVUnknown* pRoleProvider) {
    if (pRoleProvider)
	aggregate (new Gofer::Queried<IRoleProvider>(pRoleProvider));
}

/***************************
 ***************************
 *****  Communication  *****
 ***************************
 ***************************/

void Vca::VRolePlayer::communicateFailureTo (IClient *pClient) {
    VStatus iStatus;
    iStatus.MakeFailureStatus ();
    iStatus.communicateTo (pClient);
}

/********************
 ********************
 *****  Export  *****
 ********************
 ********************/

Vca::VRoleHolder::count_t Vca::VRolePlayer::exportCount () const {
    VRoleHolder::count_t cResult = 0;
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	cResult += pRoleHolder->exportCount_();
    return cResult;
}

Vca::VRoleHolder::count_t Vca::VRolePlayer::exportedInterfaceCount () const {
    VRoleHolder::count_t cResult = 0;
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	if (pRoleHolder->isExported_())
	    cResult++;
    return cResult;
}

Vca::VRoleHolder::count_t Vca::VRolePlayer::instantiatedInterfaceCount () const {
    VRoleHolder::count_t cResult = 0;
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	if (pRoleHolder->isInstantiated_())
	    cResult++;
    return cResult;
}

Vca::VRoleHolder::count_t Vca::VRolePlayer::referencedInterfaceCount () const {
    VRoleHolder::count_t cResult = 0;
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	if (pRoleHolder->isReferenced_())
	    cResult++;
    return cResult;
}

Vca::VRoleHolder::count_t Vca::VRolePlayer::roleCount () const {
    VRoleHolder::count_t cResult = 0;
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	cResult++;
    return cResult;
}

bool Vca::VRolePlayer::hasInstantiatedInterfaces () const {
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	if (pRoleHolder->isInstantiated_())
	    return true;
    return false;
}

bool Vca::VRolePlayer::hasExportedInterfaces () const {
    for (VRoleHolder::Pointer pRoleHolder (m_pRoleHolders); pRoleHolder; pRoleHolder.setTo (pRoleHolder->successor ()))
	if (pRoleHolder->isExported_())
	    return true;
    return false;
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*********************
 *****  IClient  *****
 *********************/

namespace {
    static VString g_iEODMessage ("No More Data", false);
}

void Vca::VRolePlayer::OnEnd (IClient *pRole) {
//    OnError (pRole, 0, g_iEODMessage);
    OnEnd_();
}

void Vca::VRolePlayer::OnEnd_() {
    OnError (0, 0, g_iEODMessage);
}

void Vca::VRolePlayer::OnError (IClient *pRole, IError *pError, VString const &rMessage) {
    OnError_(pError, rMessage);
}

void Vca::VRolePlayer::OnError_(IError *pError, VString const &rMessage) {
    display ("\n%s<%p>::OnError: %s\n", rttName ().content (), this, rMessage.content ());
}


/*****************************
 *****  Vca::IGetter...  *****
 *****************************/

void Vca::VRolePlayer::GetU64Value (IGetter_Ex2 *pRole, IVReceiver<U64> *pResult, VString const &rKey) {
    if (pResult && !GetU64Value_(pResult, rKey)) pResult->OnError (0, "Key not implemented.");
}

void Vca::VRolePlayer::GetF32Value (IGetter_Ex2 *pRole, IVReceiver<F32> *pResult, VString const &rKey) {
    if (pResult && !GetF32Value_(pResult, rKey)) pResult->OnError (0, "Key not implemented.");
}

void Vca::VRolePlayer::GetF64Value (IGetter_Ex2 *pRole, IVReceiver<F64> *pResult, VString const &rKey) {
    if (pResult && !GetF64Value_(pResult, rKey)) pResult->OnError (0, "Key not implemented.");
}

void Vca::VRolePlayer::GetStringValue (IGetter *pRole, IVReceiver<VString const &> *pResult, VString const &rKey) {
    if (pResult && !GetStringValue_(pResult, rKey)) pResult->OnError (0, "Key not implemented.");
}

void Vca::VRolePlayer::GetU32Value (IGetter *pRole, IVReceiver<U32> *pResult, VString const &rKey) {
    if (pResult && !GetU32Value_(pResult, rKey)) pResult->OnError (0, "Key not implemented.");
}

template <typename Sink_T> bool Vca::VRolePlayer::GetValue (Sink_T *pResultSink, VString const &rKey) {
    return classInfo().getPropertyValue (pResultSink, this, rKey);
}
#ifndef sun
template bool Vca::VRolePlayer::GetValue (IVReceiver<VString const&>*,VString const&);
template bool Vca::VRolePlayer::GetValue (IVReceiver<unsigned int>*,VString const&);
template bool Vca::VRolePlayer::GetValue (IVReceiver<unsigned __int64>*,VString const&);
template bool Vca::VRolePlayer::GetValue (IVReceiver<float>*,VString const&);
template bool Vca::VRolePlayer::GetValue (IVReceiver<double>*,VString const&);
#endif

bool Vca::VRolePlayer::GetStringValue_(
    IVReceiver<VString const &> *pResult, VString const &rKey
) {
    return GetValue (pResult, rKey);
}

bool Vca::VRolePlayer::GetU32Value_(
    IVReceiver<Vca::U32> *pResult, VString const &rKey
) {
    return GetValue (pResult, rKey);
}

bool Vca::VRolePlayer::GetU64Value_(
    IVReceiver<Vca::U64> *pResult, VString const &rKey
) {
    return GetValue (pResult, rKey);
}

bool Vca::VRolePlayer::GetF32Value_(
    IVReceiver<Vca::F32> *pResult, VString const &rKey
) {
    return GetValue (pResult, rKey);
}

bool Vca::VRolePlayer::GetF64Value_(
    IVReceiver<Vca::F64> *pResult, VString const &rKey
) {
    return GetValue (pResult, rKey);
}


/************************
 *****  Vca::IPeek  *****
 ************************/

void Vca::VRolePlayer::GetValue (IPeek *pRole, VString const &rKey, IClient *pValueSink, IVReceiver<IRequest*> *pTicketSink) {
    classInfo().getPropertyValue (pValueSink, pTicketSink, this, rKey);
}


/***************************
 *****  IRoleProvider  *****
 ***************************/

/*==========================================================================================*
 *  Creation of an aggregateable interface from one of the interfaces implemented or
 *  aggregated by this VRolePlayer derived class begin here.  For reference, here's a
 *  description of QueryRole's parameters:
 *
 *    pStatusSink           A pointer to an IStatusSink whose 'OnData' member should
 *                              be called with 'true' if the requested interface was found
 *                              and 'false' if it wasn't.  In practice, implemented by an
 *                              object associated with the query transaction.  Calling this
 *                              interface's OnData with 'false' tells its implementor to
 *                              continue looking elsewhere; calling it with 'true' gives
 *                              the implementing transaction a chance to remove whatever
 *                              monitoring it may have placed on this role provider.
 *
 *    pInterfaceSink            A pointer to an IVReceiver<IVUnknown*> that should be
 *                              fired (i.e., pInterfaceSink->OnData (pInterface))
 *                              if the interface is found.  Implemented by the client
 *                              that wants the interface.  Errors and exceptions are
 *                              also sent to the client so that the client isn't left
 *                              hanging.
 *
 *    pInterfaceType            A pointer to a VTypeInfo identifying the desired
 *                              interface.
 *
 *    pAggregator               A pointer to the IVUnknown aggregating the interface.
 *                              QueryInterface calls made using the interface returned
 *                              by this member are delegated to this aggregator.
 *
 *==========================================================================================*/
void Vca::VRolePlayer::QueryRole2 (
    IRoleProvider2 *pRole, IStatusSink *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
) {
    supplyInterface (pStatusSink, pInterfaceSink, pInterfaceType, pAggregator, false);
}

/*==========================================================================================*
 *  Creation of an aggregateable interface from one of the interfaces implemented or
 *  aggregated by this VRolePlayer derived class begin here.  For reference, here's a
 *  description of QueryRole's parameters:
 *
 *    pNotFoundSink             A pointer to an ITrigger that should be fired (i.e,
 *                              pNotFoundSink->Process()) if the requested interface
 *                              is not found.  In practice, implemented by an object
 *                              associated with the query transaction.  Firing this
 *                              trigger tells the query to continue looking elsewhere.
 *                              If the requested interface is found, abandoning this
 *                              pointer will garbage collect the transaction.
 *
 *    pInterfaceSink            A pointer to an IVReceiver<IVUnknown*> that should be
 *                              fired (i.e., pInterfaceSink->OnData (pInterface))
 *                              if the interface is found.  Implemented by the client
 *                              that wants the interface.  Errors and exceptions are
 *                              also sent to the client so that the client isn't left
 *                              hanging.
 *
 *    pInterfaceType            A pointer to a VTypeInfo identifying the desired
 *                              interface.
 *
 *    pAggregator               A pointer to the IVUnknown aggregating the interface.
 *                              QueryInterface calls made using the interface returned
 *                              by this member are delegated to this aggregator.
 *
 *==========================================================================================*/
void Vca::VRolePlayer::QueryRole (
    IRoleProvider *pRole, ITrigger *pNotFoundSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
) {
    supplyInterface (pNotFoundSink, pInterfaceSink, pInterfaceType, pAggregator, false);
}

/***********************
 *****  IVUnknown  *****
 ***********************/

void Vca::VRolePlayer::QueryInterface (
    IVUnknown *pRole, VTypeInfo *pInterfaceType, IObjectSink *pInterfaceSink
) {
    supplyInterface (pInterfaceSink, pInterfaceType);
}


/*********************************************************
 *********************************************************
 *****  Query Interface / Query Role Implementation  *****
 *********************************************************
 *********************************************************/

bool Vca::VRolePlayer::supplyInterface (
    ITrigger *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
) {
    return supplyInterfaceImplementation (pStatusSink, pInterfaceSink, pInterfaceType, pAggregator, bSubquery);
}

bool Vca::VRolePlayer::supplyInterface (
    IStatusSink *pStatusSink, IObjectSink *pInterfaceSink, VTypeInfo *pInterfaceType, IVUnknown *pAggregator, bool bSubquery
) {
    return supplyInterfaceImplementation (pStatusSink, pInterfaceSink, pInterfaceType, pAggregator, bSubquery);
}

/*==========================================================================================*
 * 
 *  A small, private helper used to perform a LIFO search (most derived to most abstract
 *  class order) the locally implemented interfaces of this class.
 *
 *==========================================================================================*/

bool Vca::VRolePlayer::supplyInterface (
    IVUnknown::Reference &rpInterface, VTypeInfo *pInterfaceType, IVUnknown *pAggregator
) {
    for (VRoleHolder *pHolder = m_pRoleHolders; pHolder; pHolder = pHolder->successor ()) {
	if (pHolder->implements (pInterfaceType)) {
	    pHolder->getRole_(rpInterface, pAggregator);
	    return true;
	}
    }
    return false;
}


/******************
 ******************
 *****  Wait  *****
 ******************
 ******************/

bool Vca::VRolePlayer::waitConditionSatisfied () const {
    return true;
}

bool Vca::VRolePlayer::waitForCondition (size_t sMillisecondTimeout) const {
    // Short-circuit right away if possible.
    bool bConditionSatisfied = waitConditionSatisfied ();
    if (bConditionSatisfied)
	return true;

    // Create an event manager for this object's cohort.
    VCohort::Manager iEM (m_pCohort);
    // Attempt to attach the event stream.
    if (iEM.attachCohortEvents ()) {
	V::VTime iNow;
	V::VTime iLimit (iNow);
	iLimit += static_cast<unsigned __int64>(sMillisecondTimeout) * 1000;
 
        // While the condition isn't satisfied and there's still time left over in the timeout.
	while (!bConditionSatisfied && iLimit > iNow) {
            // Do events until timeout.
	    sMillisecondTimeout = static_cast<size_t>((iLimit - iNow) / 1000);
	    iEM.doEvents (sMillisecondTimeout);
            // Reset loop conditions.
	    iNow.setToNow ();
	    bConditionSatisfied = waitConditionSatisfied ();
	}
    }
    return bConditionSatisfied;
}

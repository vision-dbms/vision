#ifndef Vca_VActivityModel_Interface
#define Vca_VActivityModel_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IActivityProxy.h"
#include "Vca_IActivitySink.h"
#include "Vca_IActivitySource.h"
#include "Vca_IEvent.h"
#include "Vca_IEventSink.h"
#include "Vca_IRequest.h"

#include "Vca_VActivity.h"
#include "Vca_VPublication.h"

#include "Vca_VcaGofer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VActivitySink;

    /******************************
     *----                    ----*
     *----  VSetOf<Record_T>  ----*
     *----                    ----*
     ******************************/

    template <class Record_T> class VSetOf : public VkMapOf<
	typename Record_T::key_holder_t,
	typename Record_T::key_insert_t,
	typename Record_T::key_lookup_t,
	typename Record_T::Reference
    > {
    //  Aliases
    public:
	typedef typename Record_T::key_holder_t key_holder_t;
	typedef typename Record_T::key_insert_t key_insert_t;
	typedef typename Record_T::key_lookup_t key_lookup_t;

	typedef VkMapOf<key_holder_t, key_insert_t, key_lookup_t, typename Record_T::Reference> base_t;

	DECLARE_FAMILY_MEMBERS(VSetOf<Record_T>,base_t);

    //  Construction
    public:
	VSetOf () {
	}

    //  Destruction
    public:
	~VSetOf () {
	}

    //  Access
    public:
	Record_T *operator[] (key_lookup_t xInstance) const {
	    return Instance (xInstance);
	}
	Record_T *Instance (key_lookup_t xInstance) const {
	    unsigned int xRecord;
	    return BaseClass::Locate (xInstance, xRecord) ? BaseClass::operator[] (xRecord).referent () : 0;
	}

	Record_T *InstanceAtPosition (unsigned int xRecord) const {
	    return xRecord < BaseClass::cardinality () ? BaseClass::operator[] (xRecord).referent () : 0;
	}

    //  Query
    public:
	bool Contains (Record_T *pRecord) const {
	    return pRecord && BaseClass::Contains (pRecord->key ());
	}

    //  Update
    public:
	bool Insert (Record_T *pRecord, unsigned int &rxRecord) {
	    if (pRecord && BaseClass::Insert (pRecord->key (), rxRecord)) {
		BaseClass::value (rxRecord).setTo (pRecord);
		return true;
	    }
	    return false;
	}

	bool Insert (Record_T *pRecord) {
	    unsigned int xRecord;
	    return Insert (pRecord, xRecord);
	}

	bool Delete (Record_T *pRecord) {
	    return pRecord && BaseClass::Delete (pRecord->key ());
	}
    };


    /****************************
     *----                  ----*
     *----  VActivityModel  ----*
     *----                  ----*
     ****************************/

    class Vca_API VActivityModel : virtual public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VActivityModel, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<IRequest*> ITicketSink;
	typedef Reference ModelReference;
	typedef V::VTime VTime;

    //********************************************************************************************/
    //  Record
    public:
	class Vca_API Record : virtual public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Record, VRolePlayer);

	//  Aliases
	public:
	    typedef ssid_t key_holder_t;
	    typedef ssid_t key_insert_t;
	    typedef ssid_t key_lookup_t;
	    typedef VSetOf<ThisClass> Set;

	//********************************************************************************************/
	//  Timestamps
	public:
	    class Vca_API Timestamps {
		DECLARE_NUCLEAR_FAMILY (Timestamps);

	    //  Construction
	    public:
		Timestamps (VTime const &rStartTime, VTime const &rEndTimeEstimate);
		Timestamps (VTime const &rStartTime, Timestamps const &rOther);

	    //  Destruction
	    public:
		~Timestamps ();

	    //  Access
	    public:
		VTime const &startTime () const {
		    return m_iStartTime;
		}
		VTime const &endTimeEstimate () const {
		    return m_iEndTimeEstimate;
		}
		VTime const &endTime () const {
		    return m_iEndTime;
		}

		void getStartTime (VString &rResult) const {
		    m_iStartTime.asString (rResult);
		}
		void getEndTime (VString &rResult) const {
		    m_iEndTime.asString (rResult);
		}
		void getEndTimeEstimate (VString &rResult) const {
		    m_iEndTimeEstimate.asString (rResult);
		}

	    //  Update
	    public:
		void setEndTimeEstimateTo (VTime const &rNewValue);
		void setEndTimeTo (VTime const &rNewValue);

	    //  State
	    private:
		S64	const	m_sAdjustment;
		VTime	const	m_iStartTime;
		VTime		m_iEndTime;
		VTime		m_iEndTimeEstimate;
	    };

	//********************************************************************************************/
	//  Construction
	protected:
	    Record (VActivityModel *pModel, VTime const &rStartTime, VTime const &rExpectedEndTime, VString const &rDescription);

	//  Destruction
	protected:
	    ~Record ();

	//  Access
	public:
	    key_insert_t key () const {
		return ssid ();
	    }

	    VActivityModel *model() const {
		return m_pModel;
	    }

	    VString const &description () const {
		return m_iDescription;
	    }

	    Timestamps const &remoteTimes () const {
		return m_iRemoteTimes;
	    }
	    Timestamps const &localTimes () const {
		return m_iLocalTimes;
	    }

	    template <class Interface_T> VGoferInterface<Interface_T> *getGofer (
		VReference<VGoferInterface<Interface_T> > &rpGofer, bool bAuthoritative = true
	    ) const {
		return rpGofer = new Gofer::Queried<Interface_T> (bAuthoritative ? authority_() : proxy_());
	    }

	//  Access Helpers
	private:
	    virtual IVUnknown *authority_() const = 0;
	    virtual IVUnknown *proxy_() const = 0;
	    
	//  Update
	protected:
	    void setEndTimeTo (VTime const &rEndTime);

	//  State
	private:
	    ModelReference		const	m_pModel;
	    VString			const	m_iDescription;
	    Timestamps				m_iRemoteTimes;
	    Timestamps				m_iLocalTimes;
	};

    //********************************************************************************************/
    //  template <class Key_T> class Record_<Key_T>
    public:
	template <class Key_T> class Record_ : public Record {
	    DECLARE_ABSTRACT_RTTLITE (Record_<Key_T>, Record);

	//  Aliases
	public:
	    typedef typename Key_T::Reference key_holder_t;
	    typedef Key_T                    *key_insert_t;
	    typedef Key_T const              *key_lookup_t;

	//  Construction
	protected:
	    Record_(
		VActivityModel *pModel, Key_T *pAuthority, Key_T *pProxy, VTime const &rStartTime, VTime const &rExpectedEndTime, VString const &rDescription
	    ) : BaseClass (pModel, rStartTime, rExpectedEndTime, rDescription), m_pAuthority (pAuthority), m_pProxy (pProxy) {
	    }

	//  Destruction
	protected:
	    ~Record_() {
	    }

	//  Access
	public:
	    key_insert_t key () const {
		return authority ();
	    }
	    key_insert_t authority () const {
		return m_pAuthority;
	    }
	    key_insert_t proxy () const {
		return m_pProxy;
	    }

	//  Gofer Helpers
	private:
	    IVUnknown *authority_() const {
		return authority ();
	    }
	    IVUnknown *proxy_() const {
		return proxy ();
	    }

	//  State
	private:
	    typename Key_T::Reference const m_pAuthority;
	    typename Key_T::Reference const m_pProxy;
	};

    //********************************************************************************************/
    //  ActivityRecord
    public:
	class Vca_API ActivityRecord : public Record_<IActivity>, public VActivity {
	    DECLARE_CONCRETE_RTTLITE (ActivityRecord, Record_<IActivity>);

	//  Aliases
	public:
	    typedef VSetOf<ThisClass> Set;

#if 0
	//********************************************************************************************/
	//  EventRecord
	public:
	    class Vca_API EventRecord : public Record_<IEvent> {
		DECLARE_CONCRETE_RTTLITE (EventRecord, Record_<IEvent>);

	    //  Aliases
	    public:
		typedef VSetOf<ThisClass> Set;
	    };

	//********************************************************************************************/
	//  EndRecord
	public:
	    class Vca_API EndEventRecord : public EventRecord, public VActivity::FinishEvent {
		DECLARE_CONCRETE_RTTLITE (EndEventRecord, EventRecord);

	    //  Construction
	    //  Destruction
	    };

	//********************************************************************************************/
	//  ErrRecord
	public:
	    class Vca_API ErrEventRecord : public EventRecord, public VActivity::ErrorEvent {
		DECLARE_CONCRETE_RTTLITE (ErrEventRecord, EventRecord);

	    //  Construction
	    //  Destruction
	    };
#endif

	//********************************************************************************************/
	//  Construction
	public:
	    ActivityRecord (
		VActivityModel *pModel, IActivity *pInterface, IActivity *pProxy, VTime const &rStartTime, VTime const &rExpectedEndTime, VString const &rDescription
	    );

	//  Destruction
	private:
	    ~ActivityRecord ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IActivityProxy Role
	private:
	    VRole<ThisClass,IActivityProxy> m_pIActivityProxy;
	public:
	    void getRole (IActivityProxy::Reference &rpRole) {
		return m_pIActivityProxy.getRole (rpRole);
	    }

	//  IActivity Callbacks
	public:
	    void SupplyEvents (
		IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
	    );
	    void SupplyEndErr (
		IActivity *pRole, IVReceiver<IRequest*> *pRequestSink, IEventSink *pObserver
	    );
	    void GetSSID (IActivity *pRole, IVReceiver<ssid_t> *pClient);
	    void GetParent (IActivity *pRole, IVReceiver<IActivity*> *pClient);
	    void GetDescription (IActivity *pRole, IVReceiver<VString const&> *pClient);

	//  Access
	protected:
	    void getAuthority_(IActivity::Reference &rpAuthority);
	    void getDescription_(VString &rDescription) const;

	    bool getLocalStartTime_(VTime &rResult) const;
	    bool getLocalEndTime_(VTime &rResult) const;
	    bool getLocalEndTimeEstimate_(VTime &rResult) const;

	    bool getRemoteStartTime_(VTime &rResult) const;
	    bool getRemoteEndTime_(VTime &rResult) const;
	    bool getRemoteEndTimeEstimate_(VTime &rResult) const;
	public:
	    IActivity* activity () const {
		return authority ();
	    }
	    IActivity* activityProxy () const {
		return proxy ();
	    }

	//  Query
	public:
	    bool hasCompleted () const {
		return m_bCompleted;
	    }
	    bool hasntCompleted () const {
		return !hasCompleted ();
	    }

	//  Update
	protected:
	    void onMonitorCompletion_(VActivityMonitor *pMonitor, VTime const &rEndTime);

	//  Distribution
	public:
	    void sendTo (IActivitySink *pActivitySink);

	//  Monitoring
	protected:
	    virtual void onMonitorCreation_(VActivityMonitor *pMonitor);
	    virtual void onMonitorDeletion_(VActivityMonitor *pMonitor);
	    virtual void sendTo_(VActivityMonitor *pMonitor, IActivitySink *pActivitySink);
	    virtual void supplyEvents_(VActivityMonitor *pMonitor, ITicketSink *pTicketSink);
	    virtual void supplyEndErrEvents_(VActivityMonitor *pMonitor, ITicketSink *pTicketSink);

	//  State
	private:
	    VPublication<IEventSink>	m_pubEndErrSink;
	    VPublication<IEventSink>	m_pubEventSink;
	    counter_t			m_cMonitors;
	    bool			m_bCompleted;
	}; // ... class ActivityRecord
	friend class ActivityRecord;

    //********************************************************************************************/
    //  Construction
    public:
	VActivityModel ();

    //  Destruction
    protected:
	~VActivityModel ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IActivitySource Role
    private:
	VRole<ThisClass,IActivitySource> m_pIActivitySource;
    public:
	void getRole (IActivitySource::Reference &rpRole) {
	    m_pIActivitySource.getRole (rpRole);
	}

    //  IActivitySource Callbacks
    public:
	void SupplyActivities (IActivitySource *pRole, ITicketSink *pTicketSink, IActivitySink *pActivitySink);

    //  Access
    public:
	ActivityRecord *operator[] (ActivityRecord::key_lookup_t xActivity) const {
	    return activity (xActivity);
	}
	ActivityRecord *operator[] (ssid_t xActivity) const {
	    return activity (xActivity);
	}

	ActivityRecord *activity (ActivityRecord::key_lookup_t xActivity) const;
	ActivityRecord *activity (ssid_t xActivity) const;
	ActivityRecord *activityAtPosition (unsigned int xActivity) const;

	count_t activityCount () const {
	    return m_iActivitySet.cardinality ();
	}
	count_t activitiesCreated () const {
	    return m_cActivitiesCreated;
	}
	count_t activitiesDeleted () const {
	    return m_cActivitiesDeleted;
	}

	Record *record (Record::key_lookup_t xRecord) const;
	Record *recordAtPosition (unsigned int xRecord) const;

	template <class Interface_T> VGoferInterface<Interface_T> *getGoferFor (
	    VReference<VGoferInterface<Interface_T> >&rpGofer, Record::key_lookup_t xRecord, bool bAuthoritative
	) const {
	    Record::Reference const pRecord (record (xRecord));
	    return pRecord ? pRecord->getGofer (rpGofer, bAuthoritative) : 0;
	}
	    
    //  Accounting
    private:
	void onActivityDeleted () {
	    m_cActivitiesDeleted++;
	}

    //  Update
    private:
	virtual void createActivity_(
	    ActivityRecord::Reference &rpActivity,
	    IActivity                 *pInterface,
	    IActivity                 *pProxy,
	    VTime const               &rStartTime,
	    VTime const               &rExpectedEndTime,
	    VString const             &rDescription
	);
    public:
	void createActivity (
	    ActivityRecord::Reference &rpActivity,
	    VActivitySink             *pController,
	    IActivity                 *pInterface,
	    IActivity                 *pProxy,
	    VTime const               &rStartTime,
	    VTime const               &rExpectedEndTime,
	    VString const             &rDescription
	);
    private:
	void attach (ActivityRecord *pRecord);
	void detach (ActivityRecord *pRecord);

    //  State
    private:
	VPublication<IActivitySink> m_pubIActivitySink;
	ActivityRecord::Set m_iActivitySet;
	Record::Set m_iRecordSet;
	count_t	m_cActivitiesCreated;
	count_t	m_cActivitiesDeleted;
    };
}


#endif

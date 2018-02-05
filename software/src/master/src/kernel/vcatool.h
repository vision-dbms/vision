#ifndef VcaTool_Interface
#define VcaTool_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VClientApplication.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VReceiver.h"

#include "Vca_VActivityModel.h"
#include "Vca_VInterfaceEKG.h"

#include <iostream>

using namespace std;

/*************************
 *****  Definitions  *****
 *************************/
 
namespace Vca {
    class IDie;
    class IPauseResume;
    class IStop;

    class Tool : public VClientApplication {
	DECLARE_CONCRETE_RTTLITE (Tool, VClientApplication);

    //  Aliases
    public:
	typedef VActivityModel ActivityModel;

	typedef VGoferInstance<IRequest*> IRequestGofer;

    //****************************************************************/
    //  ActivitySink
    public:
	class ActivitySink;

    //****************************************************************/
    //  Interpreter
    public:
	class Interpreter;

    //****************************************************************/
    //  Receivers
    private:
	typedef VReceiver<ThisClass, U32> U32Receiver;
	typedef VReceiver<ThisClass, U64> U64Receiver;
	typedef VReceiver<ThisClass, F32> F32Receiver;
	typedef VReceiver<ThisClass, F64> F64Receiver;
	typedef VReceiver<ThisClass, VString const &> VStringReceiver;

    //****************************************************************/
    //  Key
    public:
	class Key {
	    DECLARE_NUCLEAR_FAMILY (Key);

	//  Aliases
	public:
	    typedef unsigned int value_t;

	//  Construction
	public:
	    Key (value_t iValue) : m_iValue (iValue) {
	    }
	    Key () : m_iValue (0) {
	    }

	//  Destruction
	public:
	    ~Key () {
	    }

	//  Access
	public:
	    value_t value () const {
		return m_iValue;
	    }

	//  Comparison
	public:
	    bool operator < (ThisClass const &rOther) const {
		return m_iValue < rOther.value ();
	    }
	    bool operator <= (ThisClass const &rOther) const {
		return m_iValue <= rOther.value ();
	    }
	    bool operator == (ThisClass const &rOther) const {
		return m_iValue == rOther.value ();
	    }
	    bool operator != (ThisClass const &rOther) const {
		return m_iValue >= rOther.value ();
	    }
	    bool operator > (ThisClass const &rOther) const {
		return m_iValue > rOther.value ();
	    }

	//  Update
	public:
	    ThisClass &operator= (ThisClass const &rOther) {
		setTo (rOther);
		return *this;
	    }
	    void setTo (ThisClass const &rOther) {
		m_iValue = rOther.value ();
	    }

	//  State
	private:
	    value_t m_iValue;
	};

    //****************************************************************/
    public:
	template <class Monitored_T> class Monitored {
	    DECLARE_NUCLEAR_FAMILY (Monitored<Monitored_T>);

	//  Aliases
	public:
	    typedef Monitored_T monitored_t;
	    typedef typename monitored_t::monitored_interface_t monitored_interface_t;

	//****************************************************************/
	//  Monitor
	public:
	    class Monitor : public VInterfaceEKG<monitored_t,monitored_interface_t> {
		typedef VInterfaceEKG<monitored_t,monitored_interface_t> base_t;
		DECLARE_CONCRETE_RTTLITE (Monitor, base_t);

	    //  Construction
	    public:
		Monitor (monitored_t *pMonitored) : BaseClass (pMonitored, &monitored_t::onMonitor) {
		}

	    //  Destruction
	    private:
		~Monitor () {
		    BaseClass::cancel ();
		}

	    //  Maintenance
	    public:
		void incrementKeepaliveCount () {
		    m_cKeepalives.increment ();
		}
		void decrementKeepaliveCount () {
		    if (m_cKeepalives.decrementIsZero ())
			BaseClass::cancel ();
		}

	    //  State
	    private:
		counter_t m_cKeepalives;
	    };

	//  Construction
	public:
	    Monitored (ThisClass const &rOther) {
		setTo (rOther);
	    }
	    Monitored () {
	    }

	//  Destruction
	public:
	    ~Monitored () {
		clear ();
	    }

	//  Access
	public:
	    operator monitored_t* () const {
		return monitored ();
	    }
	    monitored_t *operator-> () const {
		return monitored ();
	    }
	    monitored_t &operator* () const {
		return *monitored ();
	    }
	    monitored_t *monitored () const {
		return m_pMonitor ? m_pMonitor->stakeHolder () : static_cast<monitored_t*>(0);
	    }

	//  Comparison
	public:
	    bool operator < (ThisClass const &rOther) const {
		return operator* () < rOther.operator* ();
	    }
	    bool operator <= (ThisClass const &rOther) const {
		return operator* () <= rOther.operator* ();
	    }
	    bool operator == (ThisClass const &rOther) const {
		return operator* () == rOther.operator* ();
	    }
	    bool operator != (ThisClass const &rOther) const {
		return operator* () >= rOther.operator* ();
	    }
	    bool operator > (ThisClass const &rOther) const {
		return operator* () > rOther.operator* ();
	    }

	//  Update
	public:
	    ThisClass &operator= (ThisClass const &rOther) {
		setTo (rOther);
		return *this;
	    }
	    ThisClass &operator= (monitored_t *pOther) {
		setTo (pOther);
		return *this;
	    }
	    void setTo (ThisClass const &rOther) {
		setTo (rOther.m_pMonitor);
	    }
	    void setTo (monitored_t *pOther) {
		if (!pOther)
		    clear ();
		else {
		    setTo (new Monitor (pOther));
		    m_pMonitor->monitor (pOther->monitoredInterface ());
		}
	    }
	private:
	    void clear () {
		if (m_pMonitor) {
		    m_pMonitor->decrementKeepaliveCount ();
		    m_pMonitor.clear ();
		}
	    }
	    void setTo (Monitor *pMonitor) {
		if (pMonitor != m_pMonitor) {
		    if (m_pMonitor)
			m_pMonitor->decrementKeepaliveCount ();
		    m_pMonitor.setTo (pMonitor);
		    if (m_pMonitor)
			m_pMonitor->incrementKeepaliveCount ();
		}
	    }

	//  State
	private:
	    typename Monitor::Reference m_pMonitor;
	};

    //****************************************************************/
    //  Tracker
    public:
	class Tracker : public Activity {
	    DECLARE_CONCRETE_RTTLITE (Tracker, Activity);

	//  Construction
	public:
	    Tracker (Tool *pTool, IRequest *pTicket, VString const &Description);

	//  Destruction
	private:
	    ~Tracker ();

	//  Access
	public:
	    Key const &key () const {
		return m_iKey;
	    }
	    Key::value_t keyValue () const {
		return m_iKey.value ();
	    }
	protected:
	    IRequest *ticket () const {
		return m_pTicket;
	    }
	public:
	    Tool *tool () const {
		return static_cast<Tool*>(application ());
	    }

	//  Display
	public:
	    void showSelf () const;

	//  Monitor Support
	public:
	    typedef IRequest monitored_interface_t;

	    monitored_interface_t *monitoredInterface () const {
		return m_pTicket;
	    }
	    void onMonitor ();

	//  State
	private:
	    Key                 const m_iKey;
	    VString             const m_iDescription;
	    IRequest::Reference const m_pTicket;
	};
	typedef VkMapOf<Key,Key const&,Key const&,Monitored<Tracker> > SetOfTrackers;

    //****************************************************************/
    //  Construction Helpers
    private:
	IRequestGofer *ActivityGofer ();

    //  Construction
    public:
	Tool (Context *pContext);

    //  Destruction
    private:
	~Tool ();

    //  Access
    public:
	ActivityModel *activityModel () const {
	    return m_pActivityModel;
	}

    //  Transitions
    private:
	bool start_() OVERRIDE;
	bool getDefaultServerName (VString &rServerName) const OVERRIDE;

    //  Callbacks
    private:
	void onStandardInput (VBSProducer *pStdin) OVERRIDE;
	void onStandardChannelAccessError (IError *pInterface, VString const &rMessage) OVERRIDE;

	void sendPause		(IPauseResume *pInterface);
	void sendResume		(IPauseResume *pInterface);
	void sendStop		(IStop *pInterface);
	void sendHardstop	(IStop *pInterface);
	void sendDie		(IDie *pInterface);

    //  Monitoring
    public:
	void startActivityMonitor ();
    private:
	void onActivityRequestValue (IRequest *pRequest);
	void onActivityRequestError (IError *pInterface, VString const &rMessage);

    //  Usage
    private:
	void displayUsage () const;

    private:
        template< typename Val_T > bool launchActions (char const * key) ;

    //  Trackers
    public:
	void createTracker (IRequest *pTicket, VString const &rDescription);
	bool deleteTracker (Key const &rKey);
	void deleteTrackers ();
	void showTrackers () const;

    //  State
    private:
	ActivityModel::Reference const	m_pActivityModel;
	IRequestGofer::Reference const	m_pActivityGofer;
	IRequest::Reference		m_pActivityRequest;
	SetOfTrackers                   m_sTrackers;
    };
}

#endif

// Local Variables:
// c-basic-offset: 4
// c-comment-only-line-offset: -4
// c-file-offsets: ((arglist-intro . +) (arglist-close . 0))
// End:

#ifndef Vps_VRelayPublication_Interface
#define Vps_VRelayPublication_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "Vps_VPublication.h"

#include "Vca_ITrigger.h"
#include "Vca_VTriggerTicket.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDataSource.h"

/*************************
 *****  Definitions  *****
 *************************/

#if defined(__VMS)
#include "cpu_stat_time.h"
#else
namespace Vca {
    namespace Vps {
	struct ProcessStats {
	};
    }
}
#endif

namespace Vca {
    namespace Vps {
        class RelayPublicationRegistrar;
	class Vps_API VRelayPublication : public VPublication {
	    DECLARE_CONCRETE_RTT (VRelayPublication, VPublication);

	//  Aliases
	public:
	    typedef IDataSource<IPublication*> IPublicationSource;
	    typedef IVReceiver<IPublication *> IPublicationReceiver;
	    typedef IVReceiver<ISubscription*> ISubscriptionReceiver;
            
	//  Construction
	public:
	    VRelayPublication (VString const &rSubject, IPublicationSource *pSource, RelayPublicationRegistrar* = NULL);

	//  Destruction
	protected:
	    ~VRelayPublication ();

	//  Base Roles
	public:
	    using BaseClass::getRole;

	//--->  IWatermarkedDataSink
	private:
	    VRole<ThisClass,IWatermarkedDataSink> m_pIStringReceiver;
	public:
	    void getRole (IWatermarkedDataSink::Reference &rpRole) {
		m_pIStringReceiver.getRole (rpRole);
	    }
	//  Role Callbacks
	public:         
	    void OnWatermarkedData (IWatermarkedDataSink *pRole, VString const &rWatermark, VString const &rMessage);

	//--->  IVReceiver<VString const&>
	private:
//	    VRole<ThisClass,IVReceiver<VString const&> > m_pIStringReceiver;
	public:
	    void getRole (IVReceiver<VString const&>::Reference &rpRole) {
//		m_pIStringReceiver.getRole (rpRole);
		IWatermarkedDataSink::Reference pWatermarkedDataSinkRole;
		getRole (pWatermarkedDataSinkRole);
		rpRole.setTo (pWatermarkedDataSinkRole);
	    }
	//  Role Callbacks
	public:
	    void OnData (IVReceiver<VString const&> *pRole, VString const &rString);

	//--->  IVReceiver<IPublication*>
	private:
	    VRole<ThisClass,IPublicationReceiver> m_pIPublicationReceiver;
	public:
	    void getRole (IPublicationReceiver::Reference &rpRole) {
		m_pIPublicationReceiver.getRole (rpRole);
	    }
	//  Role Callbacks
	public:
	    void OnData (IPublicationReceiver*pRole, IPublication *pPublication);

	//--->  IVReceiver<ISubscription*>
	private:
	    VRole<ThisClass,ISubscriptionReceiver> m_pISubscriptionReceiver;
	public:
	    void getRole (ISubscriptionReceiver::Reference &rpRole) {
		m_pISubscriptionReceiver.getRole (rpRole);
	    }
	//  Role Callbacks
	public:
	    void OnData (ISubscriptionReceiver *pRole, ISubscription *pSubscription);

	//--->  ITrigger
	private:
	    VRole<ThisClass,ITrigger> m_pITrigger;
	    void getRole (ITrigger::Reference &rpRole) {
		m_pITrigger.getRole (rpRole);
	    }
	//  Role Callbacks
	public:
	    void Process (ITrigger *pRole);

	//  Control
	protected:
	    bool start () OVERRIDE;
	    bool stop () OVERRIDE;
	private:
	    void cancelSubscription ();
	    void cancelSubscriptionMonitor ();
            void publishData(VString const & rString);

        public:
            unsigned long getMessageCount() const {
              return m_iMessageCount;
            }
            unsigned long getMessagesWanted() const {
              return m_iMessagesWanted;
            }

            const ProcessStats&  getEfficiencyStats() const {
              return m_EffStats;
            }

	//  State
	protected:
	    IPublicationSource::Reference const	m_pPublicationSource;
	    ISubscription::Reference		m_pSubscription;
	    VTriggerTicket::Reference		m_pDisconnectedSourceTriggerTicket;
	    bool				m_bRestarted;
            unsigned long                       m_iMessageCount;
            unsigned long                       m_iMessagesWanted;
            ProcessStats                        m_EffStats;
            bool                                m_bCollectStats;
	};
    }
}


#endif

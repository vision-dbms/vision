#ifndef Vps_VPublication_Interface
#define Vps_VPublication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vps_IPublication.h"
#include "Vps_IMessage.h"
#include "Vps_IMessageSink.h"
#include "Vps_ISubscription.h"

#include "Vps_IWatermarkedDataSink.h"
#include "Vps_IWatermarkablePublication.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

  namespace Vps {

    class Vps_API VPublication : public VRolePlayer {
	DECLARE_ABSTRACT_RTT (VPublication, VRolePlayer);

    //  Aliases
    public:
	typedef Reference PublicationReference;

    //  enum
    public:
	enum State {
	    State_Started, State_Stopped
	};

    //  Watermark
    public:
	class Watermark : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (Watermark, VReferenceable);

	//  Construction
	public:
	    Watermark (VString const &rWatermarkData);
	private:
	    static VString WatermarkPreamble (VString const &rWatermarkData);

	//  Destruction
	private:
	    ~Watermark () {
	    }

	//  Access
	public:
	    VString const &watermarkPreamble () const {
		return m_iWatermarkPreamble;
	    }
	    VString watermark () const;

	//  State
	private:
	    VString const m_iWatermarkPreamble;
	};

    // Message
    public:
        class Message : public VRolePlayer {
          DECLARE_CONCRETE_RTT (Message, VRolePlayer);
          // Contruction
          public:
            Message(const VPublication *pPublication);

          //  Destruction
          private:
            ~Message ();

          // Roles
          public:
            using BaseClass::getRole;
          public:
            VRole<ThisClass,IMessage> m_pIMessage;
            void getRole (IMessage::Reference &rpRole) {
              m_pIMessage.getRole (rpRole);
            }
          // Access
          public:
            void SetWanted(IMessage*, bool wanted) { 
              m_bWanted = wanted;
            }
            bool Wanted() {
              return m_bWanted;
            }
          private:
            PublicationReference	const	m_pPublication;
            bool 	                        m_bWanted;
        };

    //  Subscription
    public:
	class Vps_API Subscription : public VRolePlayer {
	    DECLARE_CONCRETE_RTT (Subscription, VRolePlayer);
    
        //  Aliases
        public:
	    typedef IVReceiver<VString const &> IRecipient;

	//  enum
        public:
	    enum State {
		State_Active,
		State_Suspended,
		State_Canceled
	    };

        //  Construction
	public:
	    Subscription (
		VPublication *pPublication, ISubscriber *pSubscriber, IRecipient *pRecipient, Watermark *pWatermark, bool bSuspended
	    );
    
        //  Destruction
	private:
	    ~Subscription ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//--->  ISubscription Role
	private:
	    VRole<ThisClass,ISubscription> m_pISubscription;
	public:
	    void getRole (ISubscription::Reference &rpRole) {
		m_pISubscription.getRole (rpRole);
	    }

	//  Role Callbacks
	//--->  ISubscription
	public:
	    void Suspend (ISubscription *pRole);
	    void Resume (ISubscription *pRole);
	    void Cancel (ISubscription *pRole);

	//  Publication
	public:
	    ThisClass *publish (VString const &rMessage);
	    ThisClass *publish (Message *pMessage, VString const &rMessage);
	    ThisClass *publishError (IError *pError, VString const &rMessage);

	//  Maintenance
	private:
	    void unlink ();
	
	//  State
	private:
	    PublicationReference	const	m_pPublication;
	    IRecipient::Reference	const	m_pRecipient;
	    IWatermarkedDataSink::Reference const m_pWatermarkableRecipient;
	    Watermark::Reference	const	m_pWatermark;
            IMessageSink::Reference             m_pMessageSink;
	    Pointer				m_pSuccessor;
	    Pointer	        		m_pPredecessor;
            State               		m_xState;
	};
	friend class Subscription;

    //  Construction
    protected:
	VPublication (VString const &rSubject);

    //  Destruction
    protected:
	~VPublication ();

    //  IWatermarkablePublication Role
    private:
	VRole<ThisClass,IWatermarkablePublication> m_pIPublication;
    public:
	void getRole (IWatermarkablePublication::Reference &rpRole) {
	    m_pIPublication.getRole (rpRole);
        };
    //  IWatermarkablePublication Methods
    public:
	void SubscribeWithWatermark (
	    IWatermarkablePublication *pRole, ISubscriber *pSubscriber, IWatermarkedDataSink *pRecipient, VString const &rWatermark,  bool bSuspended
	);

    //  IPublication Role
    private:
//	VRole<ThisClass,IPublication> m_pIPublication;
    public:
	void getRole (IPublication::Reference &rpRole) {
//	    m_pIPublication.getRole (rpRole);
	    IWatermarkablePublication::Reference pWatermarkablePublicationRole;
	    getRole (pWatermarkablePublicationRole);
	    rpRole.setTo (pWatermarkablePublicationRole);
        };
    //  IPublication Methods
    public:
	void Subscribe (IPublication *pRole, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended);

    //  Overrides
    private:
	virtual void OnError_(IError *pInterface, VString const &rMessage) OVERRIDE;

    //  Access
    protected:
	State state () const {
	    return m_xState;
	}
	VString const &subject () const {
	    return m_iSubject;
	}
	counter_t::value_t subscriptionCount () const {
	    return m_cSubscriptions;
	}
	Subscription *subscriptionListHead () const {
	    return m_pSubscriptions;
	}

    //  Query
    public:
	bool started () const {
	    return m_xState == State_Started;
	}
	bool stopped () const {
	    return m_xState == State_Stopped;
	}

    //  Control
    protected:
	virtual bool start ()=0;
        virtual bool stop ()=0;

    //  Update
    private:
	void addSubscriber (ISubscriber *pSubscriber, IRecipient *pRecipient, Watermark *pWatermark, bool bSuspended);
	void incrementSubscriptions ();
	void decrementSubscriptions ();

    //  Use
    protected:
	void publish (VString const &rMessage) const;
	void publish (Message *pMessage, VString const &rMessage) const;

    //  State
    private:
	VString const		m_iSubject;
        State                   m_xState;
	counter_t		m_cSubscriptions;
	Subscription::Pointer	m_pSubscriptions;

    };
  }
}


#endif

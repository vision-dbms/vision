#ifndef Vca_VMessage_Interface
#define Vca_VMessage_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VSchedulable.h"
#include "Vca_VMessageManager.h"
#include "Vca_VMessageStarter.h"

/**************************
 *****  Declarations  *****
 **************************/

class IVUnknown;

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaOID;
    class VcaOIDR;
    class VcaSite;
    class VcaSerializer;
    class VcaSerializerForMessage;

    class VCohort;
    class VInterfaceMember;
    class VTuple;

    class Vca_API VMessage : public VReferenceable,  protected V::VSchedulable {
	DECLARE_VIRTUAL_RTTLITE (VMessage, VReferenceable);

    //  Friends
	friend class VcaPeer;
	friend class VcaSerializerForMessage;
	friend class VCohort;
	friend class VInterfaceMember;

    //  Aliases
    public:
	typedef VMessageStarter	Starter;
	typedef VMessageManager	Manager;

    //  SubManager 
    /*****
     *  Cross-cohort messages are handled by scheduling a copy of the
     *  original message with the destination cohort.  A SubManager is
     *  used to manage that copy, delegating events to the manager of
     *  the original message.
     *****/
    public:
	typedef VMessageMonitor<Manager> SubManager;

    //  Status
    public:
	enum Status {
	    Status_New,
	    Status_Sent,
	    Status_Arriving,
	    Status_Received,
	    Status_Run,
	    Status_Confirmed,
	    Status_Error
	};

    //  Construction
    protected:
	VMessage (Starter *pStarter, Manager *pMgr=0);
	VMessage (unsigned int xMessage, Manager *pMgr=0);

    //  Destruction
    protected:
	~VMessage ();

    //  Access
    public:
	virtual VInterfaceMember const *applicable_() const = 0;
	virtual VTuple const &arguments_() const = 0;
	virtual VTuple &arguments_() = 0;

    //  Access
    public:
	virtual IVUnknown *recipient () const = 0;

	static count_t SequenceNumber (ThisClass const *pMessage) {
	    return pMessage ? pMessage->sequenceNumber () : UINT_MAX;
	}
	count_t sequenceNumber () const {
	    return m_xMessage;
	}
	Status status () const {
	    return m_xStatus;
	}
	Manager* manager () const {
	    return m_pManager;
	}

    //  Evaluation
    public:
	void evaluate (Reference *&rppSuccessor);
    private:
	void evaluate ();

	virtual void evaluate_() = 0;
	void evaluateIncomingFrom (VcaSite *pSite);

	virtual void run_() {
	    evaluate ();
	}

    //  Linkage
    public:
	void addToList (Reference *ppListHead);

	static Reference* Successor (ThisClass *pMessage, Reference &rpSuccessorContainer);

    //  Scheduling
    public:
	void resumeUsing (Starter::Reference const &rStarter) {
	    m_pStarter.setTo (rStarter);
	    resume ();
	}
	void resumeUsing (Starter *pStarter) {
	    m_pStarter.setTo (pStarter);
	    resume ();
	}
    private:
	virtual void schedule_();

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xParameter);

    //  Status callbacks
    public:
	void onError (Vca::IError *pError, VString const &rMsg);
	void onSent ();

    //  Manager
    private:
	bool isManaged () const;

    //  Create SubManager
    public:
	bool subManager (Manager::Reference &rpManager) const;

    //  Status
    private:
	bool setStatusToSent ();
	bool setStatusToReceived ();
	bool setStatusToRun ();
	bool setStatusToConfirmed ();
	bool setStatusToError ();

    //  State
    private:
	count_t const		m_xMessage;
	Starter::Reference	m_pStarter;
	Status			m_xStatus;
	Reference		m_pSuccessor;
	Manager::Reference	m_pManager;
    };
}


#endif

#ifndef Vca_VOutputSequencer_Interface
#define Vca_VOutputSequencer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vca_VActivity.h"

#include "Vca_IBSClient.h"

#include "Vca_VTriggerSet.h"

#include "V_VBlob.h"
#include "V_VFifoLite.h"
#include "V_VScheduler.h"
#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSConsumer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VOutputSequencer : public VRolePlayer, protected V::VSchedulable {
	DECLARE_CONCRETE_RTTLITE (VOutputSequencer, VRolePlayer);

    //  Aliases
    public:
	typedef V::VBlob	Blob;
	typedef V::VScheduler	Scheduler;
	typedef ThisClass	Sequencer;
	typedef Reference	SequencerReference;

    /*------------------*
     *----  Output  ----*
     *------------------*/
    public:
	class Vca_API Output : public VActivity {
	    DECLARE_ABSTRACT_RTTLITE (Output, VActivity);

	    friend class VOutputSequencer;

	//  Aliases
	public:
	    typedef V::VFifoLite Fifo;

	//  Status
	public:
	    enum Status {
		Status_Available, Status_Blocked, Status_Done, Status_Final
	    };

	//  Construction
	protected:
	    Output (Sequencer *pSequencer, Status xInitialStatus)
		: m_pSequencer (pSequencer), m_xStatus (xInitialStatus)
	    {
	    }

	//  Destruction
	protected:
	    ~Output () {
	    }

	//  Access
	public:
	    VOutputSequencer *sequencer () const {
		return m_pSequencer;
	    }

	//  Status
	public:
	    Status status () const {
		return m_xStatus;
	    }
	    bool statusIsAvailable () const {
		return m_xStatus == Status_Available;
	    }
	    bool statusIsBlocked () const {
		return m_xStatus == Status_Blocked;
	    }
	    bool statusIsDone () const {
		return m_xStatus == Status_Done;
	    }
	    bool statusIsFinal () const {
		return m_xStatus == Status_Final;
	    }

	protected:
	    void setStatusToAvailable ();
	    void setStatusToDone ();

	//  Use
	private:
	    virtual void load (Fifo &rOutputFifo) = 0;
	    void onSequencerShutdown ();

	protected:
	    void startThis ();	    // typically called from constructor of concrete class

	//  State
	private:
	    SequencerReference const	m_pSequencer;
	    Reference			m_pSuccessor;
	    Status			m_xStatus;
	};
	friend class Output;

    /*------------------------*
     *----  Output Pause  ----*
     *------------------------*/
    public:
	class OutputPause : public Output {
	    DECLARE_CONCRETE_RTTLITE (OutputPause, Output);

	//  Construction
	public:
	    OutputPause (Sequencer *pSequencer);

	//  Destruction
	protected:
	    ~OutputPause () {
	    }

	//  Use
	private:
	    virtual void load (Fifo &rOutputFifo) OVERRIDE;
	};

    /*-----------------------------*
     *----  Output Terminator  ----*
     *-----------------------------*/
    public:
	class OutputTerminator : public Output {
	    DECLARE_CONCRETE_RTTLITE (OutputTerminator, Output);

	//  Construction
	public:
	    OutputTerminator (Sequencer *pSequencer);

	//  Destruction
	protected:
	    ~OutputTerminator () {
	    }

	//  Use
	private:
	    virtual void load (Fifo &rOutputFifo) OVERRIDE;
	};

    /*-----------------------*
     *----  Blob Output  ----*
     *-----------------------*/
    public:
	class Vca_API BlobOutput : public Output {
	    DECLARE_CONCRETE_RTTLITE (BlobOutput, Output);

	//  Construction
	public:
	    BlobOutput (Sequencer *pSequencer, Blob const &rBlob);

	//  Destruction
	private:
	    ~BlobOutput () {
	    }

	//  Use
	private:
	    virtual void load (Fifo &rOutputFifo) OVERRIDE;

	//  State
	private:
	    Blob const m_iBlob;
	};

    /*-------------------------*
     *----  String Output  ----*
     *-------------------------*/
    public:
	class Vca_API StringOutput : public Output {
	    DECLARE_CONCRETE_RTTLITE (StringOutput, Output);

	//  Construction
	public:
	    StringOutput (Sequencer *pSequencer, VString const &rString);

	//  Destruction
	private:
	    ~StringOutput () {
	    }

	//  Use
	private:
	    virtual void load (Fifo &rOutputFifo) OVERRIDE;

	//  State
	private:
	    VString const m_iString;
	};

    /*--------------------------*
     *----  The Main Event  ----*
     *--------------------------*/

    //  Construction
    public:
	VOutputSequencer (VBSConsumer *pBSConsumer);

    //  Destruction
    private:
	~VOutputSequencer ();

    //  IBSClient Role
    private:
	VRole<ThisClass,IBSClient> m_pBSClient;
	void getRole (IBSClient::Reference &rpRole) {
	    m_pBSClient.getRole (rpRole);
	}

    //  IBSClient Methods
    public/*private*/:
	void OnTransfer (IBSClient *pRole, size_t sTransfer);

    //  IClient Methods
    public/*private*/:
	virtual void OnError (IClient *pRole, IError *pError, VString const &rMessage) OVERRIDE;

    //  Access/Query
    public:
	IError *finalError () const {
	    return m_pFinalError;
	}
	VString const &finalErrorMessage () const {
	    return m_iFinalErrorMessage;
	}

	bool isClosed () const {
	    return m_pBS.isNil ();
	}

    //  Control
    public:

    //  Output Generation
    public:
	void outputBlob (BlobOutput::Reference &rpBlobOutput, Blob const &rBlob);
	void outputBlob (Blob const &rBlob);

	void outputString (StringOutput::Reference &rpStringOutput, VString const &rString);
	void outputString (VString const &rString);

	void outputPause (OutputPause::Reference &rpOutputPause);

	void outputTerminator (OutputTerminator::Reference &rpOutputTerminator);
	void outputTerminator ();

    private:
	void queue (Output *pOutput);
	void start (Output *pOutput);

	virtual void schedule_() OVERRIDE;
	virtual void run_() OVERRIDE;

    //  Event Management
    public:
	void onEndTrigger (ITrigger *pTrigger);

    //  State
    private:
	Scheduler		m_iScheduler;
	VBSConsumer::Reference	m_pBS;
	VTriggerSet		m_pOnEndTriggers;
	V::VFifoLite		m_iOutputFifo;
	Output::Reference	m_pOutputQueueHead;
	Output::Reference*	m_ppOutputQueueTail;
	IError::Reference	m_pFinalError;
	VString			m_iFinalErrorMessage;
    };
}


#endif

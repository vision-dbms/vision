#ifndef Vca_VBS_Interface
#define Vca_VBS_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "V_VFifoLite.h"
#include "V_VQueue.h"
#include "V_VSchedulable.h"

#include "Vca_IBSClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSManager.h"

#include "V_VBlob.h"


/*************************
 *****  Definitions  *****
 *************************/

/**********************
 *----  Vca::VBS  ----*
 **********************/

namespace Vca {
    class IResource;
    class ITrigger;

    class VCheckPoint;
    class VStatus;

    /**
     * Abstract class representing a byte stream.
     *
     * @see VBSConsumer
     * @see VBSProducer
     */
    class ABSTRACT Vca_API VBS : public VRolePlayer, protected V::VSchedulable {
	DECLARE_ABSTRACT_RTTLITE (VBS, VRolePlayer);

    //  Blob/Data/Void
    public:
	typedef V::VBlob Blob;
	typedef const Blob Data;
	typedef Blob Void;

    //  Fifo
    public:
	typedef V::VFifoLite Fifo;

    //  ExportStub
    public:
	class ABSTRACT ExportStub : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (ExportStub, VRolePlayer);

	//  Construction
	public:
	    ExportStub (VBS *pBS);

	//  Destruction
	protected:
	    ~ExportStub () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IResource Methods
	public/*protected*/:
	    void Close (IResource *pRole) {
		m_pBS->Close ();
	    }

	//  IBSClient Role
	private:
	    VRole<ThisClass,IBSClient> m_pIBSClient;
	public:
	    void getRole (VReference<IBSClient>&rpRole) {
		m_pIBSClient.getRole (rpRole);
	    }

	//  IBSClient Methods
	public/*private*/:
	    void OnTransfer (IBSClient *pRole, size_t sTransfer) {
		onTransfer (sTransfer);
	    }

	    void OnEnd (IClient *pRole) {
		onEnd ();
	    }

	    void OnError (IClient *pRole, IError *pError, VString const &rMessage) {
		onError (pError, rMessage);
	    }

	//  Communication
	private:
	    virtual void onEnd () = 0;
	    virtual void onError (IError *pError, VString const &rMessage) = 0;
	    virtual void onTransfer (size_t sTransfer) = 0;

	//  Data Transfer
	protected:
	    void get (Void &rVoid, size_t sMinimum);
	    void put (Data &rData);

	//  State
	protected:
	    VReference<VBS> const m_pBS;
	};
	friend class ExportStub;

    //  Request
    public:
	class Request {
	//  Construction
	public:
	    Request (
		IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum
	    ) : m_pClient (pClient), m_sMinimum (sMinimum)
	    {
		m_iFifo.setTo (pVoid, sMaximum);
	    }
	    Request (IBSClient *pClient, void const *pData, size_t sData)
		: m_pClient (pClient), m_sMinimum (sData)
	    {
		m_iFifo.setTo (pData, sData);
	    }
	    Request (Request const &rOther)
		: m_pClient	(rOther.m_pClient)
		, m_iFifo	(rOther.m_iFifo)
		, m_sMinimum	(rOther.m_sMinimum)
	    {
	    }
	    Request () : m_sMinimum (0) {
	    }

	//  Destruction
	public:
	    ~Request () {
	    }

	//  Access/Query
	public:
	    Fifo const &fifo () const {
		return m_iFifo;
	    }
	    Fifo &fifo () {
		return m_iFifo;
	    }
	    size_t fifoConsumerAreaSize () const {
		return m_iFifo.consumerAreaSize ();
	    }
	    size_t fifoProducerAreaSize () const {
		return m_iFifo.producerAreaSize ();
	    }

	    bool fifoSatisfiesGet () const {
		return m_iFifo.consumerAreaSize () >= m_sMinimum;
	    }
	    bool fifoSatisfiesPut () const {
		return m_iFifo.consumerAreaIsEmpty ();
	    }
	    bool satisfied () const {
		return m_pClient.isNil ();
	    }
	    bool unsatisfied  () const {
		return m_pClient.isntNil ();
	    }

	//  Communication
	public:
	    void onEnd () const;
	    void onError (IError *pError, VString const &rMessage) const;
	    bool onStatus (VStatus const &rStatus) const;

	    bool wrapupGet (size_t sOriginalArea, size_t &rsTransfer) const;
	    bool wrapupPut (size_t sOriginalArea, size_t &rsTransfer) const;

	    bool wrapupGet (
		size_t sOriginalArea, size_t &rsTransfer, VReference<IBSClient>&rpClient
	    ) const;
	    bool wrapupPut (
		size_t sOriginalArea, size_t &rsTransfer, VReference<IBSClient>&rpClient
	    ) const;

	//  Data Transfer
	public:
	    void moveDataFrom (Fifo &rFifo) {
		m_iFifo.moveDataFrom (rFifo);
	    }
	    void moveDataTo (Fifo &rFifo) {
		m_iFifo.moveDataTo (rFifo);
	    }

	//  Update
	public:
	    void clear () {
		m_pClient.clear ();
	    }
	    void setTo (
		IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum
	    ) {
		m_pClient.setTo (pClient);
		m_iFifo.setTo (pVoid, sMaximum);
		m_sMinimum = sMinimum;
	    }
	    void setTo (IBSClient *pClient, void const *pData, size_t sData) {
		m_pClient.setTo (pClient);
		m_iFifo.setTo (pData, sData);
		m_sMinimum = sData;
	    }
	    void setTo (Request const &rOther) {
		m_pClient.setTo (rOther.m_pClient);
		m_iFifo.setTo (rOther.m_iFifo);
		m_sMinimum = rOther.m_sMinimum;
	    }

	    Request &operator= (Request const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  State
	public:
	    VReference<IBSClient>	m_pClient;
	    Fifo			m_iFifo;
	    size_t			m_sMinimum;
	};

    //  Globals
    public:
	static void setGlobalBufferSizeTo (size_t sLimit);
	static void setGlobalProxyTransferSizeTo (size_t sLimit);
    private:
	static size_t g_sBufferLimit;
	static size_t g_sProxyTransfer;

    //  Construction
    protected:
	VBS (VCohort *pCohort);

    //  Destruction
    protected:
	~VBS ();

    //  Access/Query
    protected:
	bool active () const {
	    return m_iQueue.active ();
	}

	Request const &activeRequest () const {
	    return m_iQueue.activeRequest ();
	}
	Request &activeRequest () {
	    return m_iQueue.activeRequest ();
	}

	Fifo const &activeRequestFifo () const {
	    return activeRequest ().fifo ();
	}
	Fifo &activeRequestFifo () {
	    return activeRequest ().fifo ();
	}
	size_t activeRequestConsumerAreaSize () const {
	    return activeRequest ().fifoConsumerAreaSize ();
	}
	size_t activeRequestProducerAreaSize () const {
	    return activeRequest ().fifoProducerAreaSize ();
	}

	bool activeRequestFifoSatisfiesGet () const {
	    return activeRequest ().fifoSatisfiesGet ();
	}
	bool activeRequestFifoSatisfiesPut () const {
	    return activeRequest ().fifoSatisfiesPut ();
	}

	bool buffering (size_t sTransfer);

	bool busy () const {
	    return IsntNil (m_pActiveFifo);
	}
	bool isManaged () const {
	    return m_pManager.isntNil ();
	}

	virtual bool closeDeferred () const = 0;

    public:
	size_t bufferSize () const {
	    return V_Min (m_sBufferLimit, g_sBufferLimit);
	}
	size_t proxyTransferSize () const {
	    return g_sProxyTransfer;
	}
	virtual size_t transferPending () const = 0;

	bool unbuffered () const {
	    return m_bUnbuffered;
	}
	Vca::U32 bytesRequested () const {
	  return m_sBytesRequested;
	}
	Vca::U32 bytesAcked () const {
	  return m_sBytesAcked;
	}
	
    //  Update
    private:
	void incrementBytesRequested (Vca::U32 sTransfer);
	void incrementBytesAcked (Vca::U32 sAcked);

	void checkForByteCountOverflow (Vca::U32 sCurrent, Vca::U32 sBytes);

    //  Queuing
    private:
	virtual void run_() = 0;
	virtual void schedule_();

	bool startRequest (VCohortClaim const &rClaim);

        /**
         * Starts a read request.
         *
         * @param rClaim the cohort claim that must be held in order for the generated request to be processed (as opposed to just queued).
         * @param pClient the client requesting the write.
         * @param pVoid the buffer to read data into.
         * @param sMinimum the minimum amount of data to read before this read operation terminates.
         * @param sMaximum the maximum amount of data to read before this read operation terminates. In most cases, this will be equal to the size of the pVoid buffer.
         *
         * @return true if the generated request is the next one to be processed, and can be processed.
         */
	bool startRequest (
	    VCohortClaim const &rClaim, IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum
	);

        /**
         * Starts a write request.
         *
         * @param rClaim the cohort claim that must be held in order for the generated request to be processed (as opposed to just queued).
         * @param pClient the client requesting the write.
         * @param pData the data to be written.
         * @param sData the size of the data to be written.
         *
         * @return true if the generated request is the next one to be processed, and can be processed.
         */
	bool startRequest (
	    VCohortClaim const &rClaim, IBSClient *pClient, void const *pData, size_t sData
	);

	bool nextRequest();

    ///  Data Transfer
    //@{
    protected:
        /**
         * Requests a read on this byte stream.
         *
         * @param pClient interface that should be notified when the I/O operation completes. Only used when the operation is asynchronous (no notifications to this interface are sent on synchronous/immediate I/O).
         * @param pVoid the buffer that should be filled with the requested bytes. This buffer must remain valid until the operation has been completed.
         * @param sMinimum the minimum number of bytes to be read from this stream before returning successfully. If at least this many bytes can be read immediately from the stream upon invocation of this method, the I/O operation will occur synchronously/immediatley. Otherwise, the I/O operation will continue to run asynchronously and @p pClient will be notified upon completion.
         * @param sMaximum the maximum number of bytes to be read from this stream. If more than this many bytes exist in the stream when this I/O operation completes, the remaining bytes will remain in the stream for the next I/O operation to consume.
         * @return the number of bytes read if the read was able to complete synchronously/immediately; if zero was returned, the read is deferred until the minimum required bytes are read, or an error occurs.
         *
         * @todo Document overloads of get(), put().
         */
	size_t get (IBSClient *pClient, void *pVoid, size_t sMinimum, size_t sMaximum);
	size_t get (IBSClient *pClient, void *pVoid, size_t sVoid) {
	    return get (pClient, pVoid, sVoid, sVoid);
	}
	size_t get (IBSClient *pClient, Fifo const &rVoid, size_t sMinimum) {
	    return get (
		pClient, rVoid.producerArea (), sMinimum, rVoid.producerAreaSize ()
	    );
	}
	size_t get (IBSClient *pClient, Fifo const &rVoid) {
	    return get (pClient, rVoid.producerArea (), rVoid.producerAreaSize ());
	}
	size_t get (IBSClient *pClient, Void &rVoid) {
	    return get (pClient, rVoid.postContentArea (), rVoid.postContentSize ());
	}
	size_t get (IBSClient *pClient, Void &rVoid, size_t sMinimum) {
	    return get (
		pClient, rVoid.postContentArea (), sMinimum, rVoid.postContentSize ()
	    );
	}

	size_t put (IBSClient *pClient, void const *pData, size_t sData);
	size_t put (IBSClient *pClient, Fifo const &rData) {
	    return put (pClient, rData.consumerArea (), rData.consumerAreaSize ());
	}
	size_t put (IBSClient *pClient, Data &rData) {
	    return put (pClient, rData.contentArea (), rData.contentSize ());
	}

        /**
         * Attempts to purge the output buffer, sending anything that's inside it.
         *
         * @return true if data was transferred, false otherwise.
         */
	bool putBufferedData ();

	bool getArea (char       *&rpVoid, size_t &rsVoid);
	bool getArea (char const *&rpData, size_t &rsData);

	void onGetCompletion (size_t sTransfer);
	void onPutCompletion (size_t sTransfer);

	size_t onGetContinuation ();
	size_t onPutContinuation ();

    private:
	bool onGetContinuation (size_t sOriginalArea, size_t &rsTotalTransfer);
	bool onPutContinuation (size_t sOriginalArea, size_t &rsTotalTransfer);

    private:
	void wrapupGet (size_t sOriginalArea, size_t &rsTransfer);
	void wrapupPut (size_t sOriginalArea, size_t &rsTransfer);
    //@}

    //  Communication
    protected:
	void onEnd ();
	void onError (IError *pError, VString const &rMessage);
	void onStatus (VStatus const &rStatus);

    //  Control
    public:
	void Close ();
	void Abort ();

    //  Manager Operations
    public:
	void registerManager (VBSManager *pManager);
	bool unregisterManager (VBSManager *pManager);


    //  CheckPoint Operations
    public:
	bool  isCheckPointed		    () const;
	bool  isCheckPointed		    (size_t &rCheckPointSize) const; 
	bool  isBlocked			    () const;
	bool  isntBlocked                   () const;

	void  setCheckPoint		    (VCheckPoint *pCheckPoint);
	void  releaseBlockingCheckPoint	    (); 

	void  queueCheckPoint		    (VCheckPoint *pCheckPoint);
	void  dequeueCheckPoint		    ();

	void  updateCheckPoints             (size_t sDataTransferred);
	void  processCheckPoint		    ();

    //  Subclass Responsibility
    private:
	virtual void transferData () = 0;
	virtual void endTransfers () = 0;

    //  Testing
    protected:
	void displayDataAtTransferPoint (
	    char const *pData, size_t sData, char const *pDir
	) const;

    //  State
    protected/*private*/:
	V::VQueue<Request>	m_iQueue;
	void*			m_pBuffer;
	size_t			m_sBuffer;
	size_t			m_sBufferLimit;
	Fifo			m_iBufferFifo;
	Fifo*			m_pActiveFifo;
	
	Vca::U32                m_sBytesRequested;
	Vca::U32                m_sBytesAcked;

	VReference<VCheckPoint> m_pCheckPointHead;	//  CheckPoint (CP) Queue
	VReference<VCheckPoint> m_pCheckPointTail;
	count_t			m_cNumCheckpoints;

	VBSManager::Reference	m_pManager;
	bool			m_bUnbuffered;
	bool			m_bWaitingToClose;
    };

    /**
     * Used in conjunction with the VBSConsumer class. Its
     * used to stall the bytestream with this VCheckPoint object inserted
     * in the VCheckPoint queue maintained by VBS and then restart the bytestream 
     * operations by releasing/removing this VCheckPoint object from the queue.
     *
     *	    VCheckPoints can be either BLOCKING or NONBLOCKING. Blocking Checkpoints in
     * a VBS CheckPoint queue implies that the operation of the bytestream
     * cant proceed unless the blocking checkpoint is released. A nonblocking
     * checkpoint on the other hand doesnt require it to be released by an external
     * "release" call, it is released automatically when it is being triggered.
     *	    Each VCheckPoint has a trigger object associated with it which is
     * used to perform operations on reaching the checkpoint say for e.g. releasing
     * another blocking checkpoint which is dependant on the completion of this
     * checkpoint.
     */
    class Vca_API VCheckPoint: public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VCheckPoint, VRolePlayer);

    //  friends
	friend class VBS;

    //  Construction
    public:
	VCheckPoint (size_t sDataSize, bool isBlocking, ITrigger *pTrigger);

    //  Destruction
    protected:
	~VCheckPoint (); 

    //  Query 
    public:
	size_t size () const {
	    return m_sCheckPoint;
	}
	bool isBlocking () const {
	    return m_bBlocking;
	}
	bool isntBlocking () const {
	    return !isBlocking ();
	}

    //  Update
    public:
	void decrementSize (size_t sTransfer);

    //  Trigger 
    public:
	void trigger ();

    //  State
    protected:
	size_t			   m_sCheckPoint;
	VReference<ITrigger>	   m_pTrigger;
	bool			   m_bBlocking;
	VReference<VCheckPoint>    m_pSuccessor;
	VReference<VCheckPoint>    m_pPredecessor;
    };
}


#endif

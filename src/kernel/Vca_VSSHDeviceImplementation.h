#if !defined(Vca_VSSHDeviceImplementation_Interface) && defined(HAS_LIBSSH2)
#define Vca_VSSHDeviceImplementation_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDeviceImplementation.h"
#include "V_VPointer.h"
#include "Vca_IPipeSourceClient.h"
#include <libssh2.h>

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VSSHDeviceImplementation : public VDeviceImplementation {
	DECLARE_FAMILY_MEMBERS (VSSHDeviceImplementation, VDeviceImplementation);

    //  Data
    public:
	typedef ThisClass Data;

    //  Device Reference
    public:
	typedef VReference<ThisClass> DeviceReference;

    //  Use
    public:
	class Use : public VDeviceImplementation::AbstractUse {
	    DECLARE_FAMILY_MEMBERS (Use, VDeviceImplementation::AbstractUse);

	//  Friends
	    friend class VSSHDeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Construction
	protected:
	    Use (
		VReferenceable *pContainer, VSSHDeviceImplementation *pDevice
	    ) : BaseClass (pContainer), m_pDevice (pDevice), m_xSubStreamID (0), m_iIndex (g_iIndexNext++) {
	    }

	//  Destruction
	protected:
	    ~Use () {
	    }

	//  Access
	public:
	    VSSHDeviceImplementation *device () const {
		return m_pDevice;
	    }
            int subStreamID () const {
                return m_xSubStreamID;
            }
            int index() const {
                return m_iIndex;
            }

        //  Update
        public:
            void setSubStreamID (int xSubStreamID) {
                m_xSubStreamID = xSubStreamID;
            }

	//  State
	protected:
	    DeviceReference const m_pDevice;
            /** Substream ID. */
            int m_xSubStreamID;
            static int g_iIndexNext;
            int m_iIndex;
	};
	friend class Use;

    //  BSGet
    public:
	class BSGet : public Use {
	    DECLARE_FAMILY_MEMBERS (BSGet, Use);

	//  Friends
	    friend class VSSHDeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Area
	public:
	    typedef VDeviceBSReadArea Area;

	//  Construction
	protected:
	    BSGet (
		VReferenceable *pContainer, VSSHDeviceImplementation *pDevice
	    ) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSGet () {
	    }

	//  Access
	public:
	    Area const &area () const {
		return m_iArea;
	    }
	    void *areaBase () const {
		return m_iArea.base ();
	    }
	    size_t areaSize () const {
		return m_iArea.size ();
	    }
            Reference next () const {
                return m_pNext;
            }

        //  Update
        public:
            void setNext (ThisClass* pNext) {
                m_pNext = pNext;
            }

	//  Startup
	public:
	    bool start (VkStatus &rStatus, Area const &rArea) {
		m_iArea.setTo (rArea);
		return doTransfer (rStatus);
	    }
            /**
             * Attempts the transfer.
             *
             * @param[out] the return status.
             * @param bRetrying true if this doTransfer call originated from a socket event callback (poll that came back from a previously blocked attempt) or false if this dotransfer call originated from the consumer.
             */
            bool doTransfer (VkStatus &rStatus, bool bRetrying = false) {
		return m_pDevice->doRead (rStatus, this, bRetrying) || onError (rStatus);
            }

	//  State
	private:
	    Area m_iArea;
            /** Linked list. */
            Reference m_pNext;
	};
	friend class BSGet;

    //  BSPut
    public:
	class BSPut : public Use {
	    DECLARE_FAMILY_MEMBERS (BSPut, Use);

	//  Friends
	    friend class VSSHDeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Area
	public:
	    typedef VDeviceBSWriteArea Area;

	//  Construction
	protected:
	    BSPut (
		VReferenceable *pContainer, VSSHDeviceImplementation *pDevice
	    ) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSPut () {
	    }

	//  Access
	public:
	    Area const &area () const {
		return m_iArea;
	    }
	    void const *areaBase () const {
		return m_iArea.base ();
	    }
	    size_t areaSize () const {
		return m_iArea.size ();
	    }
            Reference next () const {
                return m_pNext;
            }

        //  Update
        public:
            void setNext (ThisClass* pNext) {
                m_pNext = pNext;
            }

	//  Startup
	public:
	    bool start (VkStatus &rStatus, Area const &rArea) {
		m_iArea.setTo (rArea);
		return doTransfer (rStatus);
	    }
            /**
             * Attempts the transfer.
             *
             * @param[out] the return status.
             * @param bRetrying true if this doTransfer call originated from a socket event callback (poll that came back from a previously blocked attempt) or false if this dotransfer call originated from the consumer.
             */
            bool doTransfer (VkStatus &rStatus, bool bRetrying = false) {
		return m_pDevice->doWrite (rStatus, this, bRetrying) || onError (rStatus);
            }


	//  State
	protected:
	    Area m_iArea;
            /** Linked list. */
            Reference m_pNext;
	};
	friend class BSPut;

    //  ReadPoller
    public:
        class ReadPoller : private VDevice::BSReader {
            DECLARE_FAMILY_MEMBERS (ReadPoller, VDevice::BSReader);

        //  Constructors
        protected:
            ReadPoller (VReferenceable *pContainer, VDevice::BSReadFace *pFace) : BaseClass(pContainer, pFace) {}

        //  Polling
        protected:
            bool startReadPoll (VkStatus &rStatus) { return start (rStatus); }

        //  Poll Callbacks
        private:
            virtual void onReadPoll () = 0;
            void trigger_ () {
                onReadPoll ();
            }

        //  Bogus Transfer Callbacks.
        protected:
            void onTransfer_ (size_t) {}

        //  Closing
        protected:
            using BaseClass::close;
        };

    //  WritePoller
    public:
        class WritePoller : private VDevice::BSWriter {
            DECLARE_FAMILY_MEMBERS (WritePoller, VDevice::BSWriter);

        //  Constructors
        protected:
            WritePoller (VReferenceable *pContainer, VDevice::BSWriteFace *pFace) : BaseClass(pContainer, pFace) {}

        //  Polling
        protected:
            bool startWritePoll (VkStatus &rStatus) { return start (rStatus); }

        //  Poll Callbacks
        private:
            virtual void onWritePoll () = 0;
            void trigger_ () {
                onWritePoll ();
            }

        //  Bogus Transfer Callbacks.
        protected:
            void onTransfer_ (size_t) {}

        //  Closing
        protected:
            using BaseClass::close;
        };

    //  Channel
    public:
        class Channel : public VReferenceable, private ReadPoller, private WritePoller {
            DECLARE_CONCRETE_RTTLITE (Channel, VReferenceable);

        public:
            /** States for this Channel for use when keeping track of half-finished blocked operations. */
            enum State {
                State_New, /**< New, un-initialized channel. */
                State_Initialized, /**< Initialized session descriptor. */
                State_Started, /**< Started session, ready to authenticate. */
                State_Authenticated, /**< Authenticated session ready for execution. */
                State_Opened, /**< Channel has been opened. */
                State_Executed, /**< Remote program has been executed. */
                State_Connected, /**< Connected and able to process I/O. */
                State_Closing, /**< Close in progress. */
                State_Closed, /**< Channel closed. */
                State_Disconnected, /**< Session disconnected. */
                State_Cleaned /**< Internal libssh2 structures cleaned up. */
            };

        public:
        //  Construction
            Channel (VDevice* pDevice, VString const &rUserName, VString const &rPassword, VString const &rCommand) :
                ReadPoller (this, *pDevice),
                WritePoller (this, *pDevice),
                m_pDevice (pDevice),
                m_pSession (NULL),
                m_pChannel (NULL),
                m_iUserName (rUserName),
                m_iPassword (rPassword),
                m_iCommand (rCommand),
                m_cReaderDevices (0),
                m_cWriterDevices (0),
                m_xState (State_New)
            {
            }

        //  Destruction
        protected:
            ~Channel () {
            }

        //  Connecting
        public:
            /**
             * Called to start (or resume) the connection process.
             *
             * @param pClient the client to whom pipes should be sent. If null, m_pClient will be used.
             * @return false on error, true otherwise (including when it would block).
             */
            bool connect (IPipeSourceClient *pClient = NULL);
        public:
            bool supplyPipes (IPipeSourceClient *pClient);

        //  Disconnecting/Closing
        public:
            /** Disconnects this session, picking up in the disconnect process where it left off as necessary. */
            void disconnect ();
            /** Calls disconnect() only if there are no reader or writer devices using this Channel. */
            void disconnectAsNecessary();

        //  Device Use
        public:
            /** Attempts a read. If the read would block, enqueues the read for later processing. */
            bool doRead (VkStatus &rStatus, BSGet *pGet, bool bRetrying);
            /** Attempts a write. If the write would block, enqueues the write for later processing. */
            bool doWrite (VkStatus &rStatus, BSPut *pPut, bool bRetrying);
            /** Retries any reads that were previously blocked. */
            void retryReads ();
            /** Retries any writes that were previously blocked. */
            void retryWrites ();
            /** Indicates error status to all queued operations. */
        private:
            /** Enqueues a blocked read. */
            void enqueue (BSGet *pGet);
            /** Enqueues a blocked write. */
            void enqueue (BSPut *pPut);
            /** Dequeues a blocked read. */
            bool dequeue (BSGet::Reference &rGet);
            /** Dequeues a blocked write. */
            bool dequeue (BSPut::Reference &rPut);

        //  Poll Callbacks
        private:
            void onWritePoll ();
            void onReadPoll ();

        //  Device Accounting
        public:
            /** Decrements reader device count, calling onFinalReader() as necessary. */
            void decrReaderDevices () {
                if (--m_cReaderDevices < 1) onFinalReader ();
            }
            /** Increments reader device count. */
            void incrReaderDevices () {
                m_cReaderDevices++;
            }
            /** Decrements reader device count, calling onFinalWriter() as necessary. */
            void decrWriterDevices () {
                if (--m_cWriterDevices < 1) onFinalWriter ();
            }
            /** Increments writer device count. */
            void incrWriterDevices () {
                m_cWriterDevices++;
            }
        private:
            /** Called when there are no more reader devices using this Channel. Disconnects as necessary. */
            void onFinalReader ();
            /** Called when there are no more writer devices using this Channel. Sends an EOF to the remote peer (closing its STDIN) and then calls disconnectAsNecessary(). If sending an EOF would block, a poll is started. */
            void onFinalWriter ();

        //  Access
        public:
            State getState () { return m_xState; }

        //  State
        private:
            /** Underlying socket device. We keep a reference here for lifetime management. */
            const VDevice::Reference  m_pDevice;
            /** The underlying libssh2 session object. */
            LIBSSH2_SESSION*    m_pSession;
            /** The underlying libssh2 channel object. */
            LIBSSH2_CHANNEL*    m_pChannel;
            /** The username used to connect. */
            VString             m_iUserName;
            /** The password used to connect. */
            VString             m_iPassword;
            /** The command to execute on the remote side. */
            VString             m_iCommand;
            /** The private key file used. */
            VString             m_iKeyFilePriv;
            /** The public key file used. */
            VString             m_iKeyFilePub;
            /** The client to whom pipes should be sent after successful connection. */
            IPipeSourceClient::Reference m_pClient;
            /** The number of reader devices using this Channel. */
            U32                 m_cReaderDevices;
            /** The number of writer devices using this Channel. */
            U32                 m_cWriterDevices;
            /** Queue of blocked writes. */
            BSPut::Reference    m_pBlockedWritesHead;
            /** End of queue of blocked writes. */
            BSPut::Reference    m_pBlockedWritesTail;
            /** Queue of blocked reads. */
            BSGet::Reference    m_pBlockedReadsHead;
            /** End of queue of blocked reads. */
            BSGet::Reference    m_pBlockedReadsTail;
            /** Current State of this Channel. */
            State               m_xState;
        };

    //  Construction
    public:
	VSSHDeviceImplementation (VReferenceable *pContainer, VSSHDeviceImplementation &rOther)
	    : BaseClass (pContainer)
            , m_pChannel (rOther.m_pChannel)
            , m_xSubStreamID (rOther.m_xSubStreamID)
            , m_bHasReaders (rOther.m_bHasReaders)
            , m_bHasWriters (rOther.m_bHasWriters)
	{
            rOther.m_pChannel.clear ();
            rOther.m_bHasReaders = false;
            rOther.m_bHasWriters = false;
	}

	VSSHDeviceImplementation (VReferenceable *pContainer, Channel* pChannel, int xSubStreamID)
	    : BaseClass (pContainer)
            , m_pChannel (pChannel)
            , m_xSubStreamID (xSubStreamID)
            , m_bHasReaders (false)
            , m_bHasWriters (false)
	{
	}

    //  Destruction
    public:
	~VSSHDeviceImplementation () {
	}

    //  Tracing
    public:
        /** Tracing state; true if tracing is enabled, false otherwise. */
        static bool g_bTracing;
        /** Sets tracing state. */
        static void setTracingTo (bool bTracing) {
            g_bTracing = bTracing;
        }
        /** Used to report tracing information. */
        static void TraceSSH (char const *pInfo, ...);

    //  Device Use
    protected:
        /** Delegates to the underlying Channel, providing the appropriate SubStream ID. */
	bool doRead (VkStatus &rStatus, BSGet *pGet, bool bRetrying) {
            if (pGet) pGet->setSubStreamID (m_xSubStreamID);
            if (m_pChannel) return m_pChannel->doRead(rStatus, pGet, bRetrying);
            return rStatus.MakeClosedStatus();
        }

        /** Delegates to the underlying Channel, providing the appropriate SubStream ID. */
	bool doWrite (VkStatus &rStatus, BSPut *pPut, bool bRetrying) {
            if (pPut) pPut->setSubStreamID (m_xSubStreamID);
            if (m_pChannel) return m_pChannel->doWrite(rStatus, pPut, bRetrying);
            return rStatus.MakeClosedStatus();
        }

    //  User Accounting
    public:
        void onFirstReader ();
        void onFirstWriter ();
	void onFinalReader ();
	void onFinalWriter ();

    //  State
    private:
        /** The underlying Channel. */
        Channel::Reference  m_pChannel;
        /** The substream ID for the underlying Channel. */
        const int           m_xSubStreamID;
        /** True if there are readers on this device. */
        bool                m_bHasReaders;
        /** True if there are writers on this device. */
        bool                m_bHasWriters;
    };
}

#endif

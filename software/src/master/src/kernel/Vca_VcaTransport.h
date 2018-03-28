#ifndef Vca_VcaTransport_Interface
#define Vca_VcaTransport_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

#include "V_VFifoLite.h"
#include "V_VScheduler.h"

#include "VkMapOf.h"

#include "Vca_IBSClient.h"
#include "Vca_ITrigger.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VBSManager.h"

#include "Vca_VcaSite.h"

#include "V_VTime.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTimer.h"

class VTypeInfo;


/*************************
 *****  Definitions  *****
 *************************/

/**************************
 *----  VcaTransport  ----*
 **************************/

namespace Vca {
    class IError;

    class VcaPeer;
    class VcaConnection;
    class VcaSerializer;
    class VcaSerializerForMessage;
    class VcaSerializerForPlumbing;

    class VcaTransportInbound;
    class VcaTransportOutbound;

    class VMessage;

    class ABSTRACT Vca_API VcaTransport : public VActivity {
        DECLARE_ABSTRACT_RTTLITE (VcaTransport, VActivity);
	DECLARE_ROLEPLAYER_CLASSINFO ();

    //  Friends
	friend class VcaConnection;
	friend class VcaPeer;
	friend class VcaSerializer;
	friend class VcaSerializerForMessage;

    //  Aliases
    public:
        typedef V::VScheduler Scheduler;
        typedef Pointer TransportPointer;

    //  Enum
    protected:
        enum {
            HEARTBEAT_VALUE = 6000000 // Microseconds
        };

    //  BSManager
    public:
        class Vca_API BSManager : public VBSManager {
            DECLARE_CONCRETE_RTTLITE (BSManager, VBSManager);

        //  Construction
        public:
            BSManager (VcaTransport *pTransport);

        //  Destruction
        protected:
            ~BSManager ();

        //  Callbacks
        public:
            virtual void onEnd      () OVERRIDE {}
            virtual void onError    (IError *pError, VString const &rMessage) OVERRIDE {}
            virtual void onTransfer (size_t sTransfer) OVERRIDE;
            virtual bool onStatus   (VStatus const &rStatus) OVERRIDE;
            virtual void onChange   (U32 sChange) OVERRIDE;

        //  Query
        public:
            VcaTransport *transport () const {
                return m_pTransport;
            }

        //  State
        private:
            TransportPointer const m_pTransport; // maintain a soft reference, to prevent cyclic referencing
        };
        friend class BSManager;


    //  InTransitMessage
    public:
        /**
         * Wrapper class for VMessage that VcaTransport maintains.
         * Wraps the VMessage with VBS's start byte offset, so that later as VBS acknowledges
         * the actual byte transfer, these messages could be dequeued from the VcaTransports queue.
         */
        class Vca_API InTransitMessage : public VReferenceable {
            DECLARE_CONCRETE_RTTLITE (InTransitMessage, VReferenceable);

        //  Friend
            friend class VcaTransport;
            friend class VcaTransportOutbound;

        //  Construction
        public:
            InTransitMessage (VcaTransport *pTransport, VMessage *pMessage, U32 xStartByteOffset);

        //  Destruction
        protected:
            ~InTransitMessage ();

        /// @name Query
        //@{
        public:
            U32 size () const {
                return m_sSize;;
            }
            bool isSizeSet () const {
                return m_sSize != UINT_MAX;
            }
            U32 startByteOffset () const {
                return m_xStartByteOffset;
            }
            VMessage* message () const {
                return m_pMessage;
            }
        //@}

        /// @name Update
        //@{
        public:
          void setSize (U32 sSize) {
                m_sSize = sSize;
          }
          void decrementStartByteOffset (U32 sSize) {
                m_xStartByteOffset -= sSize;
          }
        //@}

        //  State
        private:
            /** The transport within which we're operating. A soft reference to prevent cyclic referencing. */
            TransportPointer    const   m_pTransport;
            /** The message that we're wrapping. */
            VMessage::Reference const   m_pMessage;
            U32                         m_xStartByteOffset;
            U32                         m_sSize;
            Reference                   m_pSuccessor;
        };

    protected:
        typedef InTransitMessage Message;


    //  Trace Control
    private:
        enum TransportTracing {
          Tracing_NotInitialized,
          Tracing_On,
          Tracing_Off
        };
        static TransportTracing g_bTracingTransfers;

        static bool TracingTransfers ();

    public:
        static void setTracingAllTransfersTo (bool bTrace) {
            g_bTracingTransfers = bTrace ? Tracing_On : Tracing_Off;
        }

        void setTracingTransfersTo (bool fTrace) {
            m_bTracingTransfers = fTrace;
        }

        bool tracingTransfers () const {
            return m_bTracingTransfers || TracingTransfers ();
        }


    //  Construction
    protected:
        VcaTransport (VcaConnection *pConnection);

    //  Destruction
    protected:
        ~VcaTransport ();

    //  Roles
    public:
        using BaseClass::getRole;

    /// @name IBSClient Role
    //@{
    private:
        VRole<ThisClass,IBSClient> m_pIBSClient;
    protected:
        void getRole (IBSClient::Reference& rpRole) {
            m_pIBSClient.getRole (rpRole);
        }

    //  IBSClient Methods
    public/*private*/:
        virtual void OnTransfer (IBSClient *pRole, size_t sTransfer) = 0;
    //@}

    /// @name Access
    //@{
    protected:
	virtual void getDescription_(VString &rResult) const OVERRIDE;
    public:
        char const *directionCode () const;

        VcaSite *peer () const {
            return m_pPeer;
        }

        virtual VcaTransportInbound  *transportIfInbound  ();
        virtual VcaTransportOutbound *transportIfOutbound ();

        U32 inTransitMessageSize () const {
            return m_sInTransitMessage;
        }

	void getPeerSiteInfo (IVReceiver<VString const&> *pResultSink) const;
    //@}

    /// @name Query
    //@{
    public:
        virtual bool isIncoming () const {
            return false;
        }
        virtual bool isOutgoing () const {
            return false;
        }
        bool plumbed () const {
            return m_pPeer.isntNil ();
        }
        bool shutdownInProgress () const {
            return m_bShutdownInProgress;
        }
    //@}

    /// @name Data Display
    //@{
    protected:
        void displayDataAtTransferPoint (char const *pData, size_t sData, FILE *pFile) const;
        void displayDataAtTransferPoint (char const *pData, size_t sData) const;
    //@}

    /// @name Data Transfer
    //@{
    public:
        virtual void getData (VcaSerializer *pSerializer, void *pVoid, size_t sVoid);
        virtual void putData (VcaSerializer *pSerializer, void const *pData, size_t sData);

        virtual void transferDataFor (VcaSerializer *pSerializer) = 0;
    //@}

    /// @name Plumbing
    //@{
    private:
	void plumbThyself ();
    public:
        virtual void setPeerFrom (VcaSerializerForPlumbing *pSerializer) = 0;
    //@}

    /// @name Processing
    //@{
    protected:
        void resumeSerialization (bool notSuspended);
        void abortSerialization  ();
    //@}

    /// @name Scheduling
    //@{
    private:
        void schedule (VcaSerializer *pSerializer);
        virtual void startNextSerializer () = 0;
        virtual void wrapup (VcaSerializer *pSerializer) = 0;
    //@}

    /// @name InTransit Message Bookkeeping
    //@{
    protected:
        void enqueue (Message *pMsg);
        bool dequeue (Message::Reference &rpMsg);
    //@}

    /// @name BSManager Calls
    //@{
    protected:
        virtual void onBytesTransferred () {}
        virtual void onChange (U32 sChange) {}
        virtual void onFailedOrClosed () {}
    //@}

    /// @name Message Serialization Completion
    //@{
    protected:
        virtual void onWrapupSerialization () {}
    //@}

    /// @name Shutdown
    //@{
    protected:
        void onShutdownInProgress ();
    public:
        virtual void Close () = 0;
        virtual void Abort () = 0;
    //@}

    // BSManager is not a reference but a direct pointer, to avoid cyclic reference chain
    //  State
    protected:
        VReference<VcaConnection> const m_pConnection;
        VcaSite::Reference              m_pPeer;

        VcaSerializer::Reference        m_pInTransitSerializer;
        V::VFifoLite                    m_pInTransitDatum;

        Scheduler                       m_iScheduler;

        BSManager::Reference            m_pBSManager;

        U32                             m_sInTransitMessage;
        Message::Reference              m_pInTransitMessage;

        Message::Reference              m_pInTransitMsgHead;
        Message::Reference              m_pInTransitMsgTail;
    private:
        U64 mutable                     m_cBytesTransfered;
        bool                            m_bTracingTransfers;
        bool                            m_bShutdownInProgress;
    };


    /*********************************
     *----  VcaTransportInbound  ----*
     *********************************/

    class VcaTransportInbound : public VcaTransport {
        DECLARE_CONCRETE_RTTLITE (VcaTransportInbound, VcaTransport);

    //  Friends
        friend class VcaConnection;
        friend class VcaPeer;

    //  Byte Stream Types
    protected:
        typedef VBSProducer BS;

    //  Table Types
    public:
        typedef VkMapOf<SSID,SSID const&,SSID const&,VcaSite::Reference> SiteTable;
        typedef VkMapOf<SSID,SSID const&,SSID const&,VTypeInfo::Reference> TypeTable;

    //  Construction
    public:
        VcaTransportInbound (VcaConnection *pConnection, BS *pBS);

    //  Destruction
    protected:
        ~VcaTransportInbound ();

    //  Access
    public:
        size_t transferDeficit () const {
            return m_pInTransitDatum.producerAreaSize ();
        }

    public:
        virtual VcaTransportInbound *transportIfInbound () OVERRIDE {
            return this;
        }

    //  Query
    public:
	bool isConnected () const {
	    return m_pBS.isntNil ();
	}
        virtual bool isIncoming () const OVERRIDE {
            return true;
        }

    //  Import Management - for Optimization
    public:
        /*****  Update  *****/
        bool addImport (SSID const &rSSID, VTypeInfo *pObject);

        /*****  Access  *****/
        bool getImport (SSID const &rSSID, VTypeInfo *&rpObject) const;

        // Peer Import Update
        bool addImport (SSID const &rSSID, VcaSite *pPeer);

        // Peer Import Access
        bool getImport (SSID const &rSSID, VcaSite *&rpPeer) const;

    //  Callbacks
    public/*private*/:
        virtual void OnTransfer (IBSClient *pRole, size_t sTransfer) OVERRIDE;

        virtual void OnError (IClient *pRole, IError *pError, VString const &rMessage) OVERRIDE;

    //  Data Transfer
    public:
        virtual void getData (VcaSerializer *pSerializer, void *pVoid, size_t sVoid) OVERRIDE;

        virtual void transferDataFor (VcaSerializer *pSerializer) OVERRIDE;

    //  Plumbing
    private:
	void onAttach (VcaPeer *pPeer);
	void onDetach (VcaPeer *pPeer);
    public:
        virtual void setPeerFrom (VcaSerializerForPlumbing *pSerializer) OVERRIDE;

    //  Scheduling
    private:
        void startMessage ();

        virtual void startNextSerializer () OVERRIDE;

        virtual void wrapup (VcaSerializer *pSerializer) OVERRIDE;

    //  Shutdown
    public:
        virtual void Close () OVERRIDE;
        virtual void Abort () OVERRIDE;

    //  State
    private:
        BS::Reference           m_pBS;
        SiteTable               m_iSiteTable;
        TypeTable               m_iTypeTable;
        Reference               m_pSuccessor;
    };


    /**********************************
     *----  VcaTransportOutbound  ----*
     **********************************/

    class VcaTransportOutbound : public VcaTransport {
        DECLARE_CONCRETE_RTTLITE (VcaTransportOutbound, VcaTransport);

    //  Friends
        friend class VcaConnection;
        friend class VcaPeer;

    //  Byte Stream Types
    protected:
        typedef VBSConsumer BS;

    //  Table Types
    public:
        typedef VkMapOf<
            VcaSite::Reference, VcaSite*, VcaSite const*, SSID
        > SiteTable;
        typedef VkMapOf<
            VTypeInfo::Reference, VTypeInfo*, VTypeInfo const*, SSID
        > TypeTable;


    //  Construction
    public:
        VcaTransportOutbound (VcaConnection *pConnection, BS *pBS);

    //  Destruction
    protected:
        ~VcaTransportOutbound ();

    //  Access
    public:
        size_t transferDeficit () const {
            return m_pInTransitDatum.consumerAreaSize ();
        }

    public:
        virtual VcaTransportOutbound *transportIfOutbound () OVERRIDE {
            return this;
        }

    //  Query
    public:
	bool isConnected () const {
	    return m_pBS.isntNil ();
	}
        virtual bool isOutgoing () const OVERRIDE {
            return true;
        }

    //  Export Management
    public:
        //  returns true if object added to the export set, false if already present...
        bool addExport (VTypeInfo *pObject, SSID &rSSID);
        bool addExport (VcaSite   *pObject, SSID &rSSID);

    //  Callbacks
    public/*private*/:
        virtual void OnTransfer (IBSClient *pRole, size_t sTransfer) OVERRIDE;

        virtual void OnError (IClient *pRole, IError *pError, VString const &rMessage) OVERRIDE;

    //  Data Transfer
    public:
        virtual void putData (VcaSerializer *pSerializer, void const *pData, size_t sData) OVERRIDE ;

        virtual void transferDataFor (VcaSerializer *pSerializer) OVERRIDE;

    //  Plumbing
    private:
	void onAttach (VcaPeer *pPeer);
	void onDetach (VcaPeer *pPeer);
    public:
        virtual void setPeerFrom (VcaSerializerForPlumbing *pSerializer) OVERRIDE;

    //  Scheduling
    private:
        void startMessage (VMessage *pMessage);

        virtual void startNextSerializer () OVERRIDE;

        virtual void wrapup (VcaSerializer *pSerializer) OVERRIDE;

    //  BSManager Calls
    protected:
        virtual void onBytesTransferred () OVERRIDE;
        virtual void onChange (U32 sChange) OVERRIDE;
        virtual void onFailedOrClosed () OVERRIDE;

    //  Message Serialization
    protected:
        virtual void onWrapupSerialization () OVERRIDE;

    //  HeartBeat
    private:
         void startHeartBeat ();
         void stopHeartBeat ();

    //  Base Roles
    public:
        using BaseClass::getRole;


    //  ITrigger Role
    private:
         VRole<ThisClass,ITrigger> m_pITrigger;
    public:
         void getRole (ITrigger::Reference& rpRole) {
            m_pITrigger.getRole (rpRole);
        }

    //  ITrigger Method
    public:
         void Process (ITrigger *pRole);

    //  Shutdown
    public:
        virtual void Close () OVERRIDE;
        virtual void Abort () OVERRIDE;

    //  State
    private:
        BS::Reference           m_pBS;
        SiteTable               m_iSiteTable;
        TypeTable               m_iTypeTable;

        V::VTime                m_iLastMessageTick;
        VTimer::Reference       m_pHeartBeatTimer;
        unsigned int            m_sHeartBeatStall;

        Reference               m_pSuccessor;
    };
}


#endif

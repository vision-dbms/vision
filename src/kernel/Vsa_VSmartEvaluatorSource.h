#ifndef Vsa_VSmartEvaluatorSource_Interface
#define Vsa_VSmartEvaluatorSource_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "VkMapOf.h"
#include "VReceiver.h"
#include "Vsa_VEvaluatorSource.h"


/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDataSource.h"
#include "Vsa_ISubscription.h"
#include "Vsa_IUpDownSubscriber.h"
#include "Vsa_IUpDownPublisher.h"
#include "Vca_IGetter.h"


/*************************
 *****  Definitions  *****
 *************************/
namespace Vca {
  class VDirectory;
}

namespace Vsa {

    class Vsa_API VSmartEvaluatorSource : public VEvaluatorSource {
    //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (VSmartEvaluatorSource, VEvaluatorSource);

    /// @name Aliases
    //@{
    public:
        typedef Vca::IDataSource<IVUnknown*>  ObjectSource;
        typedef IVReceiver<IVUnknown*>   ObjectSink;
        typedef VReceiver<ThisClass, VkDynamicArrayOf<VString> const &> StringArraySink;
        typedef Pointer SmartEvaluatorSourcePointer;
		typedef VkMapOf<VString, char const*, char const*, Vca::U32> BusynessOverrideMap; 
		typedef BusynessOverrideMap::Iterator Iterator;
    //@}

      //  Class
    class Request;
    class ObjectHolder : public Vca::VRolePlayer {

        //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (ObjectHolder, Vca::VRolePlayer);

        //  Typedefs
        typedef VReceiver<ThisClass, ISubscription*> ISubscriptionSink;
        typedef VReceiver<ThisClass, IVUnknown*> IVUnknownSink;

        //  Friend
        friend class Vsa::VSmartEvaluatorSource;

        /** Possible states for this ObjectHolder. */
        enum Status {
          STATUS_BEING_PROBED,
          STATUS_NOT_SUPPORTED,
          STATUS_UP,
          STATUS_DOWN,
          STATUS_NO_CONNECTION
        };

        //  Constructor
        public:
            ObjectHolder (VString const &rName, VSmartEvaluatorSource *pSource,
                          IVUnknown *pObject, IUpDownPublisher *pPublisher, bool bIsPrimarySource
            );

        //  Destructor
        protected:
            ~ObjectHolder ();

        /// @name IUpDownSubscriber Implementation
        //@{
        private:
            Vca::VRole<ThisClass,IUpDownSubscriber> m_pIUpDownSubscriber;
        public:
            void getRole (VReference<IUpDownSubscriber>&rpRole) {
                m_pIUpDownSubscriber.getRole (rpRole);
            }
            void OnUp (IUpDownSubscriber *pRole);
            void OnDown (IUpDownSubscriber *pRole);
        //@}

        /// @name Sinks
        //@{
        public:
            void onSubscription (ISubscriptionSink *pSink, ISubscription *pSubscription);
        //@}


        // Start
        private:
            void start (IUpDownPublisher *pPublisher);

        /// @name Access
        //@{
        public:
            VString const& getName () const {
                return m_iName;
            }
            IVUnknown* getObject () const {
                return m_pObject;
            }
        //@}

        /// @name Query
        //@{
        public:
            bool isObjectUp () const {
                return m_iStatus == STATUS_UP || m_iStatus == STATUS_NOT_SUPPORTED;
            }
            bool isObjectDown () const {
                return m_iStatus == STATUS_DOWN;
            }
            bool isObjectNotConnected () const {
                return m_iStatus == STATUS_NO_CONNECTION;
            }
            bool isObjectReadyForRetry ();
            void getStatus (VString &iStatus) const;
        //@}

        /// @name Update
        //@{
        public:
            void setStatus (Status iStatus) {
                m_iStatus = iStatus;
            }
            void refreshConnectivity ();
        //@}

        //  Unsubscribe
        public:
            void unsubscribe ();

        //  State
        protected:
            VString const                       m_iName;
            IVUnknown::Reference                m_pObject;
            SmartEvaluatorSourcePointer         m_pSmartSource; // soft reference to prevent circular reference
            ISubscription::Reference            m_pSubscription;
            Status                              m_iStatus;
            bool                                m_bIsPrimarySource;
            bool                                m_bReadyForRetry;
            Reference                           m_pSuccessor;
            Reference                           m_pPredecessor;
    };

    //  Friend
    friend class Request;

    //  Construction
    public:
        VSmartEvaluatorSource (Vca::IDirectory *pDirectory, Vca::U32 busynessThreshold, Vca::U32 maxBusynessChecks); 

    //  Destruction
    protected:
        ~VSmartEvaluatorSource ();

    //  Startup
    public:
        void init ();
		
    /// @name IEvaluatorSource Implementation
    //@{
    public:
        virtual void Supply (IEvaluatorSource *pRole, IEvaluatorSink *pSink) OVERRIDE;

    private:
        void supply (IEvaluatorSink *pSink);
		void addBusynessMapEntry (VString &iName, Vca::U32 iBusynessOverride);
    //@}

    /// @name Update
    //@{
    public:
        void setActiveObjectHolder (ObjectHolder *pObjectHolder) {
          m_pActiveObjectHolder.setTo (pObjectHolder);
        }
        void clearActiveObjectHolder () {
          m_pActiveObjectHolder.clear ();
        }
        
        void setBusynessMap(BusynessOverrideMap busynessOverrides);
        
        void refreshConnectivity ();
    //@}

    /// @name Access
    //@{
    public:
        ObjectHolder* getActiveObjectHolder () const {
          return m_pActiveObjectHolder;
        }
        
   		bool getBusynessOverride (VString entry, Vca::U32 &busynessValue);
        Vca::U32 GetBusynessThreshold () { return m_iBusynessThreshold; }
        Vca::U32 GetMaxBusynessChecks () { return m_iMaxBusynessChecks; }
        
    //@}

    /// @name Query
    //@{
    public:
        static void getHostname (VString &rHostname);
    private:
        bool getObjectHolder (VString const &rName, VReference<ObjectHolder> &rpHolder) const;
    //@}

    public:
        /** Callback. */
        void onEntries (StringArraySink*, VkDynamicArrayOf<VString> const &);

    /// @name Linking
    //@{
    private:
        void link (ObjectHolder *pHolder);
        void unlink (ObjectHolder *pHolder);
    //@}

    //  State
    protected:
        VReference<Vca::IDirectory> const m_pDirectory;
        ObjectHolder::Reference           m_pActiveObjectHolder;
        ObjectHolder::Reference           m_pObjectHolderList;
        VkDynamicArrayOf<VString>         m_iEntries;
		Vca::U32						  m_iBusynessThreshold;
		Vca::U32						  m_iMaxBusynessChecks;
		BusynessOverrideMap 			  m_iBusynessOverrides;
    };
}


#endif

#ifndef Vsa_VEvaluatorPool_Interface
#define Vsa_VEvaluatorPool_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluator.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDataSource.h"
#include "Vca_VTimer.h"
#include "Vsa_IEvaluatorPool_Ex5.h"
#include "Vsa_IEvaluatorPoolSettings_Ex8.h"
#include "Vsa_IEvaluatorSourceControl.h"
#include "Vsa_VPoolBroadcast.h"
#include "Vsa_VPoolWorker.h"
#include "Vsa_VPoolWorkerGeneration.h"
#include "Vsa_VOdometer.h"

#include "Vca_VTrigger.h"


/*************************
 *****  Definitions  *****
 *************************/

#ifdef __VMS
#define PoolSettings Settings
#endif

namespace Vsa {
    class VPoolEvaluation;

    class Vsa_API VEvaluatorPool : public VEvaluator {
    //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (VEvaluatorPool, VEvaluator);

    //  Friends
        friend class VPoolWorker;
        friend class VPoolWorker::WorkInProgress;
        friend class VPoolWorkerGeneration;
        friend class VPoolEvaluation;
        friend class VPoolApplication;
        friend class VPoolBroadcast;

    //  Typedef
        typedef VPoolWorker                 Worker;
        typedef VPoolWorker::WorkInProgress WIP;
        typedef VPoolWorkerGeneration       WorkerGeneration;
        typedef Vca::VTrigger<ThisClass>    FlushTrigger;
        typedef IVReceiver<VString const&>  VStringReceiver;
        typedef IVReceiver<bool>            BoolReceiver;
	typedef VkMapOf<VString, char const*, char const*, VOdometer<Vca::U32>::Reference> OdometerTable32; 
	typedef VkMapOf<VString, char const*, char const*, VOdometer<Vca::U64>::Reference> OdometerTable64; 

    //  PoolSettings
    public:
	class Vsa_API OdometerSettings : public VEvaluator::Settings {
	    DECLARE_CONCRETE_RTTLITE (OdometerSettings, VEvaluator::Settings);
	public:
	    Vca::U64 timerTime;
	    Vca::U64 compressedBucketSize;
	    Vca::U64 totalTimeRange;
	    Vca::U64 uncompressedTotalTimeRange;
	    VString name;

	    OdometerSettings() {
		timerTime = 5;					//  5 seconds
		compressedBucketSize = 30;			// 30 seconds
		totalTimeRange = 8 * 60 * 60;			//  8 hours
		uncompressedTotalTimeRange = 8 * 60 * 60;	//  8 hours
	    }
	};
	
        /**
         * Settings object abstracts pool configuration.
         */
        class Vsa_API PoolSettings : public VEvaluator::Settings {
            DECLARE_CONCRETE_RTTLITE (PoolSettings, VEvaluator::Settings);

        //  Globals
        public:
            /** Default workers-in-creation limit. */
            static Vca::U32 g_iDefaultWorkersBeingCreatedMaximum;
            /** Default worker maximum. */
            static Vca::U32 g_iDefaultWorkerMaximum;
            /** Default generation maximum (limit of old generation metadata to keep). */
            static Vca::U32 g_iDefaultGenerationMaximum;

        //  Construction
        public:
            /**
             * Copy constructor.
             *
             * @param rSettings the source from which pool settings should be copied.
             */
            PoolSettings (PoolSettings const &rSettings);

            /**
             * Default constructor initializes values to defaults.
             */
            PoolSettings ();

        //  Destruction
        private:
            /**
             * Empty default destructor restricts scope.
             */
            ~PoolSettings () {
                traceInfo ("Destroying VEvaluatorPool::PoolSettings");
            }
			
		public:
			void AddOdometerSetting(OdometerSettings *settings); 
			
        /// @name IObjectSource Role
        //@{
        private:
          Vca::VRole<ThisClass,Vca::IObjectSource> m_pIObjectSource;
        public:
          void getRole (Vca::IObjectSource::Reference &rpRole) {
            m_pIObjectSource.getRole (rpRole);
          }

        //  IObjectSource Methods
        public:
          void Supply (Vca::IObjectSource *pRole, Vca::IObjectSink *pClient);
		
		/// @name IEvaluatorPoolSettings_Ex8 Role
        //@{
        private:
            Vca::VRole<ThisClass,IEvaluatorPoolSettings_Ex8> m_pIEvaluatorPoolSettings;
        public:
            void getRole (IEvaluatorPoolSettings_Ex8::Reference &rpRole);	
			
        //  IEvaluatorPoolSettings Methods
        public:
            void MakeControl (
                IEvaluatorPoolSettings *pRole, IVReceiver<IEvaluatorPool*> *pClient
            );

            void GetWorkerSource (
                IEvaluatorPoolSettings *pRole, IVReceiver<IEvaluatorSource*> *pClient
            );
            void SetWorkerSource (
                IEvaluatorPoolSettings *pRole, IEvaluatorSource *pWorkerSource
            );

            void GetWorkersBeingCreatedMaximum (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkersBeingCreatedMaximum (
                IEvaluatorPoolSettings *pRole, Vca::U32 iValue
            );

            void GetWorkerMaximum (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerMaximum (IEvaluatorPoolSettings *pRole, Vca::U32 iValue);

            void GetWorkerMinimum (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerMinimum (IEvaluatorPoolSettings *pRole, Vca::U32 iValue);

			void GetEvaluationAttemptMaximum (
				IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
			);
			
			void SetEvaluationAttemptMaximum (
				IEvaluatorPoolSettings *pRole, Vca::U32 iValue
			);
			
            void GetWorkerGenerationLimit (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerGenerationLimit (
                 IEvaluatorPoolSettings *pRole, Vca::U32 iValue
            );
            void GetWorkerMinimumAvailable (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerMinimumAvailable (
                 IEvaluatorPoolSettings *pRole, Vca::U32 iValue
            );
            void GetWorkerStartupQuery (
                IEvaluatorSettings *pRole, IVReceiver<VString const &>*
            );
            void SetWorkerStartupQuery (
                IEvaluatorSettings *pRole, VString const &
            );
            void GetWorkerCreationFailureLimit (
                IEvaluatorPoolSettings *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerCreationFailureLimit (
                IEvaluatorPoolSettings *pRole, Vca::U32 iValue
            );
            void GetDebugFilePathName (
                IEvaluatorPoolSettings *pRole, IVReceiver<VString const &> *pClient
            );
            void SetDebugFilePathName (
                IEvaluatorPoolSettings *pRole, VString const& iValue
            );
            void GetDebugSwitch (
                IEvaluatorPoolSettings *pRole, IVReceiver<bool> *pClient
            );
            void SetDebugSwitch (
                IEvaluatorPoolSettings *pRole, bool iValue
            );

        //  IEvaluatorPoolSettings_Ex1 Methods
        public:
            void GetWorkerMaximumAvailable (
                IEvaluatorPoolSettings_Ex1 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerMaximumAvailable (
                IEvaluatorPoolSettings_Ex1 *pRole, Vca::U32 iValue
            );
            void GetShrinkTimeOut (
                IEvaluatorPoolSettings_Ex1 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetShrinkTimeOut (
                IEvaluatorPoolSettings_Ex1 *pRole, Vca::U32 iValue
            );

        //  IEvaluatorPoolSettings_Ex2 Methods
        public:
            void GetBCEvaluationsInProgressLimit (
                IEvaluatorPoolSettings_Ex2 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetBCEvaluationsInProgressLimit (
                IEvaluatorPoolSettings_Ex2 *pRole, Vca::U32 iValue
            );

        //  IEvaluatorPoolSettings_Ex3 Methods
        public:
            void GetStopTimeOut (
                IEvaluatorPoolSettings_Ex3 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetStopTimeOut (IEvaluatorPoolSettings_Ex3 *pRole, Vca::U32 iValue);

        //  IEvaluatorPoolSettings_Ex4 Methods
        public:
            void GetWorkerQueryHistoryLimit (
                IEvaluatorPoolSettings_Ex4 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerQueryHistoryLimit (IEvaluatorPoolSettings_Ex4 *pRole, Vca::U32 iValue);

        //  IEvaluatorPoolSettings_Ex5 Methods
        public:
            void GetWorkerCreationFailureSoftLimit (
                IEvaluatorPoolSettings_Ex5 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetWorkerCreationFailureSoftLimit (IEvaluatorPoolSettings_Ex5 *pRole, Vca::U32 iValue);
            void GetWorkerPIDQuery (
                IEvaluatorPoolSettings_Ex5 *pRole, IVReceiver<VString const &> *pClient
            );
            void SetWorkerPIDQuery (IEvaluatorPoolSettings_Ex5 *pRole, VString const &iValue);

        //  IEvaluatorPoolSettings_Ex6 Methods
        public:
            void GetGenerationMaximum (
                IEvaluatorPoolSettings_Ex6 *pRole, IVReceiver<Vca::U32> *pClient
            );
            void SetGenerationMaximum (IEvaluatorPoolSettings_Ex6 *pRole, Vca::U32 iValue);
        //@}

        //  IEvaluatorPoolSettings_Ex7 Methods
        public:
            void GetQueryLogLength (IEvaluatorPoolSettings_Ex7 *pRole, IVReceiver<Vca::U32> *pClient);
            void SetQueryLogLength (IEvaluatorPoolSettings_Ex7 *pRole, Vca::U32 iValue);
            void GetResultLogLength (IEvaluatorPoolSettings_Ex7 *pRole, IVReceiver<Vca::U32> *pClient);
            void SetResultLogLength (IEvaluatorPoolSettings_Ex7 *pRole, Vca::U32 iValue);
        //@}
		
		//  IEvaluatorPoolSettings_Ex8 Methods
        public:
            void GetEvaluationOnErrorAttemptMaximum (IEvaluatorPoolSettings_Ex8 *pRole, IVReceiver<Vca::U32> *pClient);
            void SetEvaluationOnErrorAttemptMaximum (IEvaluatorPoolSettings_Ex8 *pRole, Vca::U32 iValue);
            void GetEvaluationTimeOutAttemptMaximum (IEvaluatorPoolSettings_Ex8 *pRole, IVReceiver<Vca::U32> *pClient);
            void SetEvaluationTimeOutAttemptMaximum (IEvaluatorPoolSettings_Ex8 *pRole, Vca::U32 iValue);
        //@}
		
		public:
			VkDynamicArrayOf<OdometerSettings::Reference> GetOdometerSettings () const;
		
        /// @name Query
        //@{
        public:
            bool workerCreationSupported () const {
                return m_pWorkerSource.isntNil ();
            }
        //@}

        /// @name Access
        //@{
        public:
            IEvaluatorSource *workerSource () const {
                return m_pWorkerSource;
            }
            IEvaluatorSourceControl *workerSourceControl () const {
                return m_pWorkerSourceControl;
            }
            Vca::U32 workersBeingCreatedMaximum () const {
                return m_iWorkersBeingCreatedMaximum;
            }
            Vca::U32 workerMaximum () const {
                return m_iWorkerMaximum;
            }
            Vca::U32 workerMinimum () const {
                return m_iWorkerMinimum;
            }
			Vca::U32 evaluationAttemptMaximum () const {
				return m_cEvaluationAttemptMaximum;
			}
			
            Vca::U32 evaluationOnErrorAttemptMaximum () const {
                return m_cEvaluationOnErrorAttemptMaximum;
            }
            Vca::U32 evaluationTimeOutAttemptMaximum () const {
                return m_cEvaluationTimeOutAttemptMaximum;
            }

            Vca::U32 workerMinimumAvailable () const {
                return m_iWorkerMinimumAvailable;
            }
            Vca::U32 workerMaximumAvailable () const {
                return m_iWorkerMaximumAvailable;
            }
            void workerStartupQuery (VString &rQuery) const {
                rQuery = m_iWorkerStartupQuery;
            }
            Vca::U32 workerCreationFailureHardLimit () const {
                return m_iWorkerCreationFailureHardLimit;
            }
            Vca::U32 workerCreationFailureSoftLimit () const {
                return m_iWorkerCreationFailureSoftLimit;
            }
            Vca::U32 shrinkTimeOut () const {
                return m_iShrinkTimeOutMinutes;
            }
            Vca::U32 getStopTimeOut () const {
                return m_iStopTimeOutMins;
            }
            Vca::U32 BCEvaluationsInProgressLimit () const {
                return m_iBCEvaluationsInProgressLimit;
            }
            Vca::U32 workerQueryHistoryLimit () const {
                return m_iWorkerQueryHistoryLimit;
            }
            void workerPIDQuery (VString &rQuery) const {
                rQuery.setTo (m_iWorkerPIDQueryString);
            }
            Vca::U32 generationMaximum () const {
                return m_iGenerationMaximum;
            }
            Vca::U32 queryLogLength () const {
                return m_iQueryLogLength;
            }
            Vca::U32 resultLogLength () const {
                return m_iResultLogLength;
            }
        //@}
		
        /// @name Update
        //@{
        public:
            void setWorkerSourceTo (IEvaluatorSource *pWorkerSource) {
                m_pWorkerSource.setTo (pWorkerSource);
            }
            void setWorkerSourceControlTo (IEvaluatorSourceControl *pWorkerSourceControl) {
                m_pWorkerSourceControl.setTo (pWorkerSourceControl);
            }
            void setWorkersBeingCreatedMaximumTo (Vca::U32 iValue) {
                m_iWorkersBeingCreatedMaximum = iValue;
            }
            void setWorkerMaximumTo (Vca::U32 iValue) {
                m_iWorkerMaximum = iValue;
            }
            void setWorkerMinimumTo (Vca::U32 iValue) {
                m_iWorkerMinimum = iValue;
            }
	    void setEvaluationAttemptMaximumTo (Vca::U32 cMax) {
		m_cEvaluationAttemptMaximum = cMax;
	    }
            void setEvaluationOnErrorAttemptMaximumTo (Vca::U32 cMax) {
                m_cEvaluationOnErrorAttemptMaximum = cMax;
            }
            void setEvaluationTimeOutAttemptMaximumTo (Vca::U32 cMax) {
                // m_cEvaluationTimeOutAttemptMaximum = cMax;
		//    Disallow Timeout retries. A timeout generates a cancel which generates an error. 
		//      in the present design, the cancel error cannot be distinguished from a true error
		//      and the timeout retry may be abandoned or duplicated depending on the onErrorAttempt setting.

                m_cEvaluationTimeOutAttemptMaximum = 0;
            }
            void setWorkerMinimumAvailableTo (Vca::U32 iValue) {
                 m_iWorkerMinimumAvailable = iValue;
            }
            void setWorkerMaximumAvailableTo (Vca::U32 iValue) {
                if (iValue <= m_iWorkerMaximum)
                    m_iWorkerMaximumAvailable = iValue;
                else
                   m_iWorkerMaximumAvailable = m_iWorkerMaximum;
            }
            void setWorkerStartupQueryTo (VString const &rQuery) {
                m_iWorkerStartupQuery = rQuery;
                if (m_pWorkerSourceControl)
                    m_pWorkerSourceControl->SetStartupQuery (rQuery);
            }
            void setWorkerCreationFailureHardLimitTo (Vca::U32 iValue) {
                m_iWorkerCreationFailureHardLimit = iValue;
            }
            void setWorkerCreationFailureSoftLimitTo (Vca::U32 iValue) {
                m_iWorkerCreationFailureSoftLimit = iValue;
            }
            void setWorkerQueryHistoryLimitTo (Vca::U32 iValue) {
                m_iWorkerQueryHistoryLimit = iValue;
            }
            void setShrinkTimeOutTo (Vca::U32 iValue) {
                m_iShrinkTimeOutMinutes = iValue;
            }
            void setStopTimeOutTo (Vca::U32 iValue) {
                m_iStopTimeOutMins = iValue;
            }
            void setBCEvaluationsInProgressLimit (Vca::U32 iLimit)  {
                m_iBCEvaluationsInProgressLimit = iLimit;
            }
            void setWorkerPIDQueryString (VString const &rQuery) {
                m_iWorkerPIDQueryString.setTo (rQuery);
            }
            void setGenerationMaximumTo (Vca::U32 iValue) {
                m_iGenerationMaximum = iValue;
            }
            void setQueryLogLengthTo (Vca::U32 iValue) {
                m_iQueryLogLength = iValue;
            }
            void setResultLogLengthTo (Vca::U32 iValue) {
                m_iResultLogLength = iValue;
            }
        //@}

        public:
            void getStatistics (VString &rResult) const;
            void dumpSettings (VString &rResult) const;

        //  State
        protected:
            /** IEvaluatorSource to be used by the pool in order to obtain workers. */
            IEvaluatorSource::Reference        m_pWorkerSource;
            /** IEvaluatorSourceControl governing m_pWorkerSource. */
            IEvaluatorSourceControl::Reference m_pWorkerSourceControl;
            /** Maximum number of workers to be permitted in the pool. */
            Vca::U32                    m_iWorkerMaximum;
            /** Minimum number of workers that the pool should have. */
            Vca::U32                    m_iWorkerMinimum;
            /** Maximum number of workers to simultaneously spawn. */
            Vca::U32                    m_iWorkersBeingCreatedMaximum;
			/** Maximum number of retries that a query should go through before an error should be returned. */
			Vca::U32                    m_cEvaluationAttemptMaximum;
            /** Maximum number of retries that a query should go through before an error should be returned. */
            Vca::U32                    m_cEvaluationOnErrorAttemptMaximum;
			/** Maximum number of retries that a query should go through before an time out should be returned. */
            Vca::U32                    m_cEvaluationTimeOutAttemptMaximum;

            /** Minimum number of workers that should be available at all times. */
            Vca::U32                    m_iWorkerMinimumAvailable;
            /** Maximum number of available workers before firing should occur. */
            Vca::U32                    m_iWorkerMaximumAvailable;
            /** Startup query for workers, run automatically during worker hiring. */
            VString                     m_iWorkerStartupQuery;
            /** Number of worker hire failures that are allowed to occur before worker hiring is never attempted again. */
            Vca::U32                    m_iWorkerCreationFailureHardLimit;
            /** Number of worker hire failures that are alloewd to occur before worker hiring is not re-attempted until a restart has occurred. */
            Vca::U32                    m_iWorkerCreationFailureSoftLimit;
            /** Maximum number of queries (and results) to keep, per worker, in the worker history metadata. */
            Vca::U32                    m_iWorkerQueryHistoryLimit;
            /** Periodicity of the shrink operation, expressed in minutes. */
            Vca::U32                    m_iShrinkTimeOutMinutes;
            /** Stop timer, expressed in minutes. */
            Vca::U32                    m_iStopTimeOutMins;
            /** Maximum number of broadcast evaluations to simultaneously process. */
            Vca::U32                    m_iBCEvaluationsInProgressLimit;
            /** Query sent to each worker whose result is used as the PID of said worker. */
            VString                     m_iWorkerPIDQueryString;
            /** Maximum number of old generations to keep for metadata/history purposes. */
            Vca::U32                    m_iGenerationMaximum;
            /** Maximum length of query string in log messages (controls truncation of query strings in logs). */
            Vca::U32                    m_iQueryLogLength;
            /** Maximum length of result string in log messages (controls truncation of result strings in logs). */
            Vca::U32                    m_iResultLogLength;
			/** Table of odometer settings for each individual odometer */
			VkDynamicArrayOf<OdometerSettings::Reference> 	m_Odometers;
        };
        friend class PoolSettings;


    //  Construction
    public:
        /**
         * Constructor expects a PoolSettings from which settings are retrieved.
         *
         * @param pSettings the settings to be used for the newly constructed pool.
         */
        VEvaluatorPool (PoolSettings *pSettings);

    //  Destruction
    private:
        ~VEvaluatorPool ();
	
    //  Base Class Roles
    public:
        using BaseClass::getRole;

    /// @name IEvaluatorControl
    //@{
    public:
        void Suspend (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver);
        void Resume  (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver);
        void Restart (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver);
        void Stop    (IEvaluatorControl *pRole, IVReceiver<bool> *pReceiver);
        void GetStatistics (
            IEvaluatorControl *pRole, IVReceiver<VString const &> *pReceiver
        );
        void GetWorker     (IEvaluatorControl*, IVReceiver<IVUnknown*>*, Vca::U32);
    //@}

    /// @name IEvaluatorPool
    //@{ 
    public:
        void GetSettings   (IEvaluatorPool *pRole, IVReceiver<IEvaluatorPoolSettings*>*pClient);
        void AddEvaluator  (IEvaluatorPool *pRole, IEvaluator *pEvaluator);
        void FlushWorkers  (IEvaluatorPool *pRole, IVReceiver<bool>*pClient);
    //@}

    /// @name IEvaluatorPool_Ex1
    //@{
    public:
        void HardRestart (IEvaluatorPool_Ex1 *pRole, IVReceiver<bool> *pReceiver);
        void IsWorkerCreationFailureLmtReached (IEvaluatorPool_Ex1*, IVReceiver<bool>*);
        void GetCurrentGenerationTotalWorkers  (IEvaluatorPool_Ex1*, IVReceiver<Vca::U32>*);
        void GetWorkerInterface  (IEvaluatorPool_Ex1*, IVReceiver<IVUnknown*>*, Vca::U32);
        void GetTotalWorkers (IEvaluatorPool_Ex1*, IVReceiver<Vca::U32>*);
        void HardStop (IEvaluatorPool_Ex1 *pRole, IVReceiver<bool> *pReceiver);
    //@}

    /// @name IEvaluatorPool_Ex2
    //@{
    public:
        void BroadcastExpression (IEvaluatorPool_Ex2*, IEvaluatorClient*, VString const&, VString const&);
        void BroadcastURL (IEvaluatorPool_Ex2*, IEvaluatorClient*, VString const&, VString const&, VString const&);
    //@}

    /// @name IEvaluatorPool_Ex3
    //@{
    public:
        void TakeWorkerOffline (IEvaluatorPool_Ex3*, VString const &, bool, IVReceiver<VString const &>*);
        void MakeWorkerOnline (IEvaluatorPool_Ex3*, VString const &, bool, IVReceiver<VString const &>*);
        void EvaluateUsingOfflineWorker (IEvaluatorPool_Ex3*, VString const &, VString const &, bool, IEvaluatorClient *pClient);
        void RetireOfflineWorker (IEvaluatorPool_Ex3*, VString const &, bool, IVReceiver<VString const &>*);
        void DumpWorker (IEvaluatorPool_Ex3*, VString const &, bool, VString const &, IVReceiver<VString const &>*);
        void DumpPoolQueue (IEvaluatorPool_Ex3*, VString const &, Vca::U32, IVReceiver<VString const &>*);
        void DumpWorkingWorkers (IEvaluatorPool_Ex3*, VString const &, IVReceiver<VString const &>*);
        void GetWorker_Ex (IEvaluatorPool_Ex3*, VString const&, bool, IVReceiver<IVUnknown *>*);
        void GetWorkersStatistics (IEvaluatorPool_Ex3*, IVReceiver<VString const &>*);
        void GetGenerationStatistics (IEvaluatorPool_Ex3*, Vca::U32 iID, IVReceiver<VString const &>*);
    //@}

    /// @name IEvaluatorPool_Ex4
    //@{
    public:
        void QueryDetails (Vsa::IEvaluatorPool_Ex4 *pRole, Vca::U32 iID, IVReceiver<const VString&>* pClient);
        void CancelQuery (Vsa::IEvaluatorPool_Ex4 *pRole, Vca::U32 iID, BoolReceiver *pClient);
    //@}
    
    /// @name IEvaluatorPool_Ex5
    // @{
	private:
		Vca::VRole<ThisClass,IEvaluatorPool_Ex5> m_pIEvaluatorPool;
	public:
        void getRole (IEvaluatorPool_Ex5::Reference &rpRole);
	
        void StatSum (IEvaluatorPool_Ex5 *pRole, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
        void StatDivide (IEvaluatorPool_Ex5 *pRole, VString, VString, Vca::U64, Vca::U64, IVReceiver<Vca::F64>*);
        void StatMax (IEvaluatorPool_Ex5 *pRole, VString, Vca::U64, Vca::U64, IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*);
        void StatMin (IEvaluatorPool_Ex5 *pRole, VString, Vca::U64, Vca::U64, IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*);
        void StatMaxString (IEvaluatorPool_Ex5 *pRole, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
        void StatMinString (IEvaluatorPool_Ex5 *pRole, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
		
        void AllStatValues (IEvaluatorPool_Ex5 *pRole, VString, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, 
                            IVReceiver<VkDynamicArrayOf<V::VTime> const &>*, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, 
                            IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, IVReceiver<VkDynamicArrayOf<Vca::F64> const &>*); 
		

    /// @name Evaluation Creation
    //@{
    private:
        VEvaluation *createEvaluation (
            IEvaluatorClient*, VString const&, VString const&
        );
        VEvaluation *createEvaluation (
            IEvaluatorClient*, VString const&, VString const&, VString const&
        );
    //@}

    /// @name Query
    //@{
    public:
        bool workerCreationSupported () const {
            return m_pSettings->workerCreationSupported ();
        }
        Vca::U32 workersBeingCreatedMaximum () const {
            return m_pSettings->workersBeingCreatedMaximum ();
        }
        Vca::U32 workerMaximum () const {
            return m_pSettings->workerMaximum ();
        }
        Vca::U32 workerMinimum () const {
            return m_pSettings->workerMinimum () > m_cTempWorkerMinimum ? m_pSettings->workerMinimum () : m_cTempWorkerMinimum;
        }
        Vca::U32 workerMinimumAvailable () const {
            return m_pSettings->workerMinimumAvailable ();
        }
        Vca::U32 workerMaximumAvailable () const {
            return m_pSettings->workerMaximumAvailable ();
        }
        IEvaluatorSource *workerSource () const {
            return m_pSettings->workerSource ();
        }
        void workerStartupQuery (VString &rQuery) const {
            m_pSettings->workerStartupQuery (rQuery);
        }
        void workerPIDQuery (VString &rQuery) const {
            m_pSettings->workerPIDQuery (rQuery);
        }
        Vca::U32 workerQueryHistoryLimit () const {
            return m_pSettings->workerQueryHistoryLimit ();
        }
        Vca::U32 workerCreationFailureHardLimit () const {
            return m_pSettings->workerCreationFailureHardLimit ();
        }
        Vca::U32 workerCreationFailureSoftLimit () const {
            return m_pSettings->workerCreationFailureSoftLimit ();
        }
		Vca::U32 evaluationAttemptMaximum () const {
			return m_pSettings->evaluationAttemptMaximum ();
		}
        Vca::U32 evaluationOnErrorAttemptMaximum () const {
            return m_pSettings->evaluationOnErrorAttemptMaximum ();
        }
        Vca::U32 evaluationTimeOutAttemptMaximum () const {
            return m_pSettings->evaluationTimeOutAttemptMaximum ();
        }
        Vca::U64 evaluationTimeOut () const {
            return m_pSettings->evaluationTimeOut ();
        }
        Vca::U32 shrinkTimeOut () const {
            return m_pSettings->shrinkTimeOut ();
        }
        Vca::U32 getStopTimeOut () const {
            return m_pSettings->getStopTimeOut ();
        }
        Vca::U32 BCEvaluationsInProgressLimit () const {
            return m_pSettings->BCEvaluationsInProgressLimit ();
        }
        Vca::U32 queryLogLength () const {
            return m_pSettings->queryLogLength ();
        }
        Vca::U32 resultLogLength () const {
            return m_pSettings->resultLogLength ();
        }
        bool hasMaximumWorkers () const {
            return m_cOnlineWorkers >= m_pSettings->workerMaximum ();
        }
        bool exceedsMaximumWorkers () const {
            return m_cOnlineWorkers > m_pSettings->workerMaximum ();
        }
        bool exceedsIdleWorkers () const {
            return excessIdleWorkers() > 0;
        }
        /**
         * If the number of available workers is greater than the number
         * specified by the tuning parameter, then we may have an excess number
	 * of idle workers, and be in a state where the pool can think about
	 * shrinking to maxAvailable. The number of excess workers is the smaller value of
	 * (1) the difference between the number of available workers and the maxAvailable setting and
	 * (2) the difference between the total number of workers and the minWorker setting.
	 *
	 * @return the number of excess idle workers
         */
        Vca::U32 excessIdleWorkers () const {
            return onlineAvailWorkers () > workerMaximumAvailable () ?
		V_Min (onlineAvailWorkers () - workerMaximumAvailable (), m_cOnlineWorkers - workerMinimum ()) :
		0;
        }
        bool hasMaxBCEvaluationsInProgress () const {
            return m_cBCEvaluationsInProgress >= BCEvaluationsInProgressLimit ();
        }

        Vca::U32 currentGenerationId () const;
        Vca::U32 oldestGenerationId  () const;

        //  activeWIPs - WIPs which havent timed out
        bool hasActiveWIPs () const;
	//  startedWorkers - workers ready to be made available
        bool hasStartedWorkers () const;

        /**
         * Minutes remaining on shrink timer. Returns UINT_MAX if no timer is set.
         */
        Vca::U32 shrinkTimeRemaining () {
            if (m_pShrinkTimer.isNil ()) return UINT_MAX;
            return static_cast<Vca::U32>(m_pShrinkTimer->timeToExpire () / 1000 / 1000 / 60);
        }
    //@}

	
	/// @name OdometerManagement
	//@{
	protected:
		void setupOdometers ();   
	//@}
	
    /// @name Access
    //@{
    public:
        Vca::U32 onlineWorkerCount () const {
            return m_cOnlineWorkers;
        }
        Vca::U32 onlineAvailWorkers () const {
            return m_cOnlineAvailWorkers;
        }
        Vca::U32 onlineWIPs () const {
            return m_cOnlineWIPs;
        }
        Vca::U32 onlineExcessWorkers () const {
            return m_cOnlineExcessWorkers;
        }
        Vca::U32 retiredWorkers () const {
            return m_cWorkersRetired;
        }
        Vca::U32 offlineWorkerCount () const {
            return m_cOfflineWorkers;
        }
        Vca::U32 offlineAvailWorkers () const {
            return m_cOfflineAvailWorkers;
        }
        Vca::U32 offlineWIPs () const {
            return m_cOfflineWIPs;
        }
        Vca::U32 offlineExcessWorkers () const {
            return m_cOfflineExcessWorkers;
        }
        Vca::U32 workersBeingCreated () const {
            return m_cWorkersBeingCreated;
        }
        Vca::U32 lastToBeFlushedWorker () const {
            return m_iLastToBeFlushedWorker;
        }
        Vca::U32 toBeFlushedWorkers () const {
            return m_cToBeFlushedWorkers;
        }
        Vca::U32 BCEvaluationsInProgress () const {
            return m_cBCEvaluationsInProgress;
        }
    //@}

    /// @name Update
    //@{
    public:
        void setWorkerSourceTo (IEvaluatorSource *pWorkerSource) {
            m_pSettings->setWorkerSourceTo (pWorkerSource);
        }
		void setEvaluationAttemptMaxTo (Vca::U32 cMax) {
			m_pSettings->setEvaluationAttemptMaximumTo (cMax);	
		}
        void setEvaluationOnErrorAttemptMaxTo (Vca::U32 cMax) {
            m_pSettings->setEvaluationOnErrorAttemptMaximumTo (cMax);
        }
		void setEvaluationTimeOutAttemptMaxTo (Vca::U32 cMax) {
            m_pSettings->setEvaluationTimeOutAttemptMaximumTo (cMax);
        }

        void setTempWorkerMinimumTo (Vca::U32 cMin) {
	    // Should never be less than the configured minimum .. may be greater..
            m_cTempWorkerMinimum = V_Max (cMin, m_pSettings->workerMinimum ());
        }
        void incrOnlineWorkers () {
            m_cOnlineWorkers++;
            // Reset m_cTempWorkerMinimum to zero if it's been reached.
            if (m_pGenerationHead->onlineWorkerCount() >= m_cTempWorkerMinimum) setTempWorkerMinimumTo (0);
        }
        void decrOnlineWorkers () {
            m_cOnlineWorkers--;
        }

        /**
         * Should be called when online available worker count needs to be incremented.
         * Sets a shrink timer as necessary to trim excess idle workers.
         */
        void incrOnlineAvailWorkers () {
            m_cOnlineAvailWorkers++;
            if (exceedsIdleWorkers ()) setShrinkTimer ();
        }
        void decrOnlineAvailWorkers () {
            m_cOnlineAvailWorkers--;
            if (! exceedsIdleWorkers ()) {
                stopShrinkTimer ();
            }
        }
        void incrOnlineWIPs () {
            m_cOnlineWIPs++;
        }
        void decrOnlineWIPs () {
            m_cOnlineWIPs--;
        }
        void incrOfflineWorkers () {
            m_cOfflineWorkers++;
        }
        void decrOfflineWorkers () {
            m_cOfflineWorkers--;
        }
        void incrOfflineAvailWorkers () {
            m_cOfflineAvailWorkers++;
        }
        void decrOfflineAvailWorkers () {
            m_cOfflineAvailWorkers--;
        }
        void incrOfflineWIPs () {
            m_cOfflineWIPs++;
        }
        void decrOfflineWIPs () {
            m_cOfflineWIPs--;
        }
        void decrOfflineExcessWorkers () {
            m_cOfflineExcessWorkers--;
        }
        void incrOfflineExcessWorkers (Vca::U32 cWorkers = 1) {
            m_cOfflineExcessWorkers += cWorkers;
        }
        void incrWorkersBeingCreated () {
            m_cWorkersBeingCreated++;
        }
        void decrWorkersBeingCreated () {
            m_cWorkersBeingCreated--;
        }
        void decrOnlineExcessWorkers () {
            m_cOnlineExcessWorkers--;
            if (! exceedsIdleWorkers ()) stopShrinkTimer ();
        }
        void incrOnlineExcessWorkers (Vca::U32 cWorkers = 1) {
            if (cWorkers == 0) return;
            m_cOnlineExcessWorkers += cWorkers;
            if (exceedsIdleWorkers ()) setShrinkTimer ();
        }
        void decrementToBeFlushedWorkers () {
            if (--m_cToBeFlushedWorkers == 0)
                triggerOnFlush ();
        }
        void incrementBCEvaluationsInProgress () {
            m_cBCEvaluationsInProgress++;
        }
        void decrementBCEvaluationsInProgress () {
            m_cBCEvaluationsInProgress--;
        }
        void incrWorkersRetired () {
            m_cWorkersRetired++;
        }
        void decrWorkersRetired () {
            m_cWorkersRetired--;
        }
    //@}

    private:
    /// @name Query Execution
    //@{
        /**
         * Pool-specific request queue handling, including hiring of new workers as necessary.
         * Also handles hard stop trigger processing.
         */
        void onQueue_ ();
        void onRequest (VEvaluation *pRequest);

        /**
         * Attempt query processing.
         * Wrapper for processBCEvaluations and processQueryEvaluations, in that order.
         *
         * @return true if either of the delegate functions return true, false otherwise.
         */
        bool processQueries ();

        /**
         * Attempt to allocate pool queries to the available workers.
         *
         * The queries are present in the queue in the order in which they arrived, with their specific
         * preferences of which generation worker they are to be evaluated with. This routine
         * tries to allocate as many latest queries as possible with the latest workers.
         * If there are still queries pending, iterates through the queue and tries to
         * evaluate the ones present after the blocking (pending) query, so that queries
         * which are ready to use old workers can be evaluated.
         *
         * @return true if any queries were evaluated.
         */
        bool processQueryEvaluations ();
        bool processBCEvaluations    ();
        void onAnyDataUpdateFor (VPoolEvaluation *pEvaluation);
        void onRetryableUpdateFor (VPoolEvaluation *pEvaluation);
    //@}

    /// @name Pool Control
    //@{
        void restart ();
        void stop    ();
        void suspend ();
        void suspend (bool);
        void resume  ();
        void resume  (bool);
        void flushWorkers (Vca::VTrigger<VEvaluatorPool> *pTrigger=0);
        void hardRestart       ();
		void resumeHardRestartSuspended (Vca::VTrigger<ThisClass> *pTrigger);
        void resumeHardRestart (Vca::VTrigger<ThisClass> *pTrigger);
        void add     (IEvaluator *pEvaluator);
        void hardstop (Vca::ITrigger *pTrigger=0);
        void triggerOnFlush ();
        void cancelQuery (Vca::U32 iID, BoolReceiver *pClient = 0);
        virtual bool cancel_ (VEvaluation* pEvaluation);
    //@}

    /// @name Offline Worker Management
    //@{
        void takeWorkerOffline (VString const &rId, bool bIsPID, VStringReceiver *pClient);
        void makeWorkerOnline  (VString const &rId, bool bIsPID, VStringReceiver *pClient);
        void retireOfflineWorker  (VString const &rId, bool bIsPID, VStringReceiver *pClient);
        void evaluateUsingOfflineWorker (VString const &rQuery, VString const &rId, bool bIsPID, IEvaluatorClient *pClient);
    //@}

    /// @name Snapshots
    //@{
        void dumpWorker (
            VString const &rId, bool bIsPID, VString const &rFile, VStringReceiver* pClient
        ) const;
        void dumpPoolQueue (VString const &rFile, Vca::U32 cQueries, VStringReceiver* pClient);
        void queueDumpItem (VReference<VPoolEvaluation> &pRequest, VString &rResult);
        void queryDetails (VReference<VPoolEvaluation> &pRequest, VString &rResult, WIP *pWIP = NULL);
        void queryDetails (Vca::U32 xRequest, VString &rResult);
        void queueDump (VString &rResult);
        void queueDumpHeader (VString &rResult);
        void dumpWorkingWorkers (VString const &rFile, VStringReceiver* pClient) const;
    //@}

    /// @name GenerationList
    //@{
        void attach (WorkerGeneration *pGeneration);
        void detach (WorkerGeneration *pGeneration);
        void detachAllGenerations ();
    //@}

    /// @name Worker Operations
    //@{
        /**
         * Initiates worker hiring (as necessary) for the current generation.
         */
        void hire () {
            if (m_pGenerationHead.isntNil ()) m_pGenerationHead->hire ();
        }
        /** Gets a free worker for the evaluation specified from the pool. */
        bool supplyOnlineWorkerFor (VPoolEvaluation *pEvaluation, VPoolWorker::Reference &rpWorker) const;

	/** Makes a worker that has been created available for use. */
	void finishHiringWorker ();

        /** Gets just the worker object reference for the worker id. */
        bool getWorkerReference (VString const & iWorkerId, bool bIsPID, Worker::Reference &rpWorker) const;
        /** Gets just the worker object interface for the worker id. */
        void getWorkerInterface (IVReceiver<IVUnknown*>* pReceiver, Vca::U32 workerIdentifier) const;
    //@}


    /// @name Worker Trim Operations
    //@{
        void trimWorkers         (Vca::U32 cWorkers, bool bOldOnly = true);
        void setShrinkTimer      ();
        void onShrinkTimeOut     (Vca::VTrigger<ThisClass> *pTrigger);
        void stopShrinkTimer     ();
    //@}

        /**
         * Pool assertions check. Runs through a list of assertions and logs assertion failures as appropriate.
         *
         * @return the number of assertions that failed.
         */
        Vca::U32 checkAssertions () const;

    /// @name Broadcast
    //@{
        void processBCObject           (VPoolBroadcast *pBroadcast);
        void appendBCObject            (VPoolBroadcast *pBroadcast);
        void removeBCObject            (VPoolBroadcast *pBroadcast);
        void enqueueBCEvaluation       (VPoolEvaluation *pEvaluation);
        void removeAllBCEvaluationsFor (Vca::U32 iWorkerId);
    //@}

    /// @name Worker Creation Failure Limit
    //@{
        void flushQueue (VString const &rErrorMsg);
        bool isWorkerCreationFailureHardLmtReached () const;
        bool isWorkerCreationFailureSoftLmtReached () const;
        void onWorkerCreationFailureHardLmt ();
        void onWorkerCreationFailureSoftLmt ();
    //@}

    /// @name Client Messaging
    //@{
        void sendWorkerNotAvailableMsg (
            VString const &rWorkerId, bool bIsPID, IVReceiver<VString const &> *pClient
        ) const;
    //@}

    /// @name Statistics & Config
    //@{
        /**
         * Retrieves long, human-readable statistics output for this pool.
         *
         * @param rResult the string in which to store output.
         */
        void getStatistics (VString &rResult) const;

        /**
         * Retrieves long, human-readable statistics output for all online workers.
         *
         * @param rResult the string in which to store output.
         */
        void getWorkersStatistics (VString &rResult) const;

        /**
         * Retrieves human-readable statistics on a given generation.
         *
         * @param iID the generation ID to retrieve statistics on.
         * @param rResult the string in which to store output.
         */
        void getGenerationStatistics (Vca::U32 iID, VString &rResult) const;

        /**
         * Dumps statistics in a machine-readable format.
         *
         * @param rResult the string in which to store output.
         */
        void dumpStats (VString &rResult) const;

        /**
         * Dumps settings in a machine-readable format.
         *
         * @param rResult the string in which to store output.
         */
        void dumpSettings (VString &rResult) const;

        /**
         * Retrieves a header corresponding to quickStats() below.
         *
         * @param rResult the string in which to store output.
         */
        static void quickStatsHeader (VString &rResult);

        /**
         * Retrieves a one-line statistics dump for this pool. Useful when quickly querying multiple pools for statistics.
         * A header for this line can be retrieved via quickStatsHeader().
         *
         * @param rResult the string in which to store output.
         */
        void quickStats (VString &rResult) const;

        /**
         * Retrieves a header corresponding to dumpWorkerStats() below.
         *
         * @param rResult the string in which to store output.
         */
        static void dumpWorkerStatsHeader (VString &rResult);

        /**
         * Retrieves a one-line statistics dump for each worker in this pool.
         * A header for this line can be retrieved via dumpWorkerStatsHeader().
         *
         * @param rResult the string in which to store output.
         */
        void dumpWorkerStats (VString &rResult) const;

        /**
         * Retrieves a header corresponding to dumpGenerationStats() below.
         *
         * @param rResult the string in which to store output.
         */
        static void dumpGenerationStatsHeader (VString &rResult);

        /**
         * Retrieves a one-line statistics dump for each generation in this pool.
         * A header for this line can be retrieved via dumpGenerationStatsHeader().
         *
         * @param rResult the string in which to store output.
         */
        void dumpGenerationStats (VString &rResult) const;
    //@}

    //  State
    protected:	
        /**
         * Count of currently online workers across all generations.
         */
        VOdometer<Vca::U32>            m_cOnlineWorkers; 

        /**
         * Count of currently online available workers across all generations.
         */
        VOdometer<Vca::U32>           m_cOnlineAvailWorkers;

        /**
         * Count of currently online works in progress across all generations.
         */
        VOdometer<Vca::U32>           m_cOnlineWIPs;

        /**
         * Count of workers currently being created across all generations.
         */
        Vca::U32            m_cWorkersBeingCreated;

        /**
         * Count of excess online workers across all generations.
         */
        Vca::U32            m_cOnlineExcessWorkers;

        /**
         * Count of retired workers across all generations.
         */
        VOdometer<Vca::U32>          m_cWorkersRetired;

        /**
         * Count of offline workers across all generations.
         */
        Vca::U32            m_cOfflineWorkers;

        /**
         * Count of offline available workers across all generations.
         */
        Vca::U32            m_cOfflineAvailWorkers;

        /**
         * Count of currently offline works in progress across all generations.
         */
        Vca::U32            m_cOfflineWIPs;

        /**
         * Count of offline excess workers across all generations.
         */
        Vca::U32            m_cOfflineExcessWorkers;

        Vca::U32            m_iLastToBeFlushedWorker;

        /**
         * Count of workers that are about to be flushed.
         */
        Vca::U32            m_cToBeFlushedWorkers;

        /**
         * Temporary minimum worker count used to rehire workers across restarts.
         * Used to make sure that as many workers are rehired after a restart as existed before the restart.
         * Automatically reset to zero when this many workers exist in the current generation.
         */
        Vca::U32            m_cTempWorkerMinimum;

        /** Trigger fired when workers are to be flushed. */
        FlushTrigger::Reference    m_pOnFlushTrigger;

        /**
         * PoolSettings object for this pool, encapsulating all settings.
         */
        PoolSettings::Reference    m_pSettings;

        /**
         * The head of our WorkerGeneration list.
         */
        WorkerGeneration::Reference    m_pGenerationHead;

        /**
         * The tail of our WorkerGeneration list.
         */
        WorkerGeneration               *m_pGenerationTail;

        /**
         * Timer used for shrinking this generation.
         */
        Vca::VTimer::Reference         m_pShrinkTimer;

        /**
         * Count of the number of times the shrink timer has expired --
         * An indication of the number of times shrinking has occurred.
         */
        Vca::U32                       m_cShrinkTimerExpirations;

        /**
         * Track whether this pool has been explicitly suspended
         */
        bool m_bExplicitlySuspended;

        /**
         * Count of broadcast evaluations currently in progress.
         */
        Vca::U32                        m_cBCEvaluationsInProgress;

        /**
         * Broadcast evaluation queue.
         */
        Queue                           m_iBCEvaluationQueue;

        /**
         * Broadcast object list.
         */
        VPoolBroadcast::Reference       m_pBCObjectListHead;

        /** Trigger used to defer hard stop events. */
        Vca::ITrigger::Reference        m_pHardStopTrigger;
	
		OdometerTable32					m_iOdometers32;
		OdometerTable64					m_iOdometers64;
    };
}

#endif

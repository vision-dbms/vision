#ifndef V_VThreadedProcessor__Interface
#define V_VThreadedProcessor__Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VManagedThread.h"
#include "V_VQueue.h"
#include "V_VSemaphore.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /**
     * Thread pool manager, responsible for lifetime and employment management of threads.
     *
     * @tparam Request Object whose instances are requests to be processed by threads in this thread pool. Must have a copy and default constructor and support assignment operators given an instance of its own type as the assignment value.
     */
    template<class Request> class VThreadedProcessor_ : public VReferenceable {
        DECLARE_CONCRETE_RTT (VThreadedProcessor_<Request>, VReferenceable);

    //  Aliases
    public:
        typedef unsigned_int32_count_t  counter_t;
        typedef ThisClass               processor_t;

    //  State
    public:
        /**
         * State information for this thread pool.
         */
        enum State {
            State_Running, /**< The thread pool is running, processing requests as necessary. Incoming requests queued and processed as soon as possible. */
            State_Paused, /**< The thread pool is paused, no new requests will be processed until it has been resumed. Incoming requests are queued. */
            State_Stopped /**< The thread pool is stopped, no new requests will be accepted. */
        };

    //  Worker
    private:
        /** Descriptor for one thread. */
        class Worker : public VManagedThread {
            DECLARE_CONCRETE_RTTLITE (Worker, VManagedThread);

        //  Construction
        public:
            /**
             * Constructor expects a thread pool manager.
             *
             * @param pProcessor the thread pool manager to which this worker belongs.
             */
            Worker (processor_t *pProcessor) : m_pProcessor (pProcessor) {
            }

        //  Destruction
        protected:
            /** Empty default destructor restricts scope. */
            ~Worker () {
            }

        //  Execution
        private:
            /**
             * Override for V::VThread::adjustStackSize() delegates to thread pool manager.
             *
             * @see V::VThread::adjustStackSize()
             */
            void adjustStackSize (size_t &rsSize) const {
                m_pProcessor->adjustStackSize (rsSize);
            }

            /**
             * Delegates to VThreadedProcessor::employ().
             */
            pthread_procedure_result_t run_() { // ... virtual called by the thread procedure to do this thread's business
                m_pProcessor->employ (this);
                return 0;
            }

        //  State
        private:
            typename processor_t::Reference const m_pProcessor;
        };
        friend class Worker;

    //  Construction
    public:
        VThreadedProcessor_()
            : m_xState (State_Running)
            , m_sThreadStack (0) /* -> use default stack size */
            , m_sWorkerLimit (32) /* -> limit to 32 unless set explicitly by application for some special reason */
        {
        }

    //  Destruction
    private:
        ~VThreadedProcessor_() {
        }

    //  Access/Query
    public:
        void adjustStackSize (size_t &rsSize) const;

        State state () const {
            return m_xState;
        }

    //  Settings
    public:
        /**
         * Set the thread stack size. If set to zero, the default stack size is used.
         *
         * @param sThreadStack the new thread stack size to use.
         */
        void setThreadStackSize (size_t sThreadStack) {
            m_sThreadStack = sThreadStack;
        }
        void setWorkerLimit (unsigned int sWorkerLimit) {
            m_sWorkerLimit = sWorkerLimit;
        }
    //  Control
    public:
        void pause () {
            if (State_Running == m_xState)
                m_xState = State_Paused;
        }
        void resume () {
            if (State_Paused == m_xState) {
                m_xState = State_Running;
            // Access and reset (to zero) the number of paused requests:
                unsigned int cPausedRequests = m_cPausedRequests.claim ();
                while (cPausedRequests--)               //  for each paused request...
                    m_iQueueSemaphore.produce ();       //  ... tell a worker to rise and shine -- nap time is over.
            }
        }
        void stop () {
            if (m_xState != State_Stopped) {
                m_xState = State_Stopped;
            // signal the workers to stop...
                counter_t::value_t cWorkers = m_cWorkersAvailable;
                while (cWorkers-- > 0)
                    m_iQueueSemaphore.produce ();
            }
        }

    //  Request Processing
    public:
        /**
         * Handles incoming requests. Starts new workers as necessary, inserts incoming requests into request queue as necessary.
         */
        void process (Request const &rRequest) {
            switch (m_xState) {
            case State_Running:
                if (m_cWorkersAvailable.isZero () && m_cWorkers < m_sWorkerLimit) {
		    m_cWorkers.increment ();    // incrementing worker's count here to ensure it won't exceed the limit
                    typename Worker::Reference pNewWorker (new Worker (this));
                    pNewWorker->start ();       // this better succeed or we'll have a lot of impatient customers :-)
                }
                m_iQueue.enqueue (rRequest);
                m_iQueueSemaphore.produce ();   // send a wake up a call to our sleeping workers
                break;
            case State_Paused:
                m_cPausedRequests.increment ();
                m_iQueue.enqueue (rRequest);
                break;
            default: // State_Stopped
                break;
            }
        }
    private:
        /**
         * Handles incoming workers. Newly created workers (threads) call this from their run_() procedure once their initialization is complete. This is the heart of a worker's thread procedure, pulling requests off this thread pool's queue and processing them.
         *
         * @see Worker::run_()
         */
        void employ (Worker *pWorker) {  // this is heart of a worker's thread procedure (see run_() above for details):
            m_cWorkersAvailable.increment ();
            do {
                m_iQueueSemaphore.consume ();  // ... wait here for something to do
                if (State_Running == m_xState) {
                    Request iRequest;
                    if (m_iQueue.dequeue (iRequest)) {
                        m_cWorkersAvailable.decrement ();
                        iRequest.process ();
                        m_cWorkersAvailable.increment ();
                    }
                }
            } while (m_xState != State_Stopped);
            m_cWorkersAvailable.decrement ();
            m_cWorkers.decrement ();
        }

    //  State
    private:
        /** Request queue. */
        VQueue<Request> m_iQueue;
        /** Semaphore for worker/queue interaction. */
        VSemaphore      m_iQueueSemaphore;
        /** Number of workers. */
        counter_t       m_cWorkers;
        /** Number of available workers. */
        counter_t       m_cWorkersAvailable;
        /** Number of paused requests. */
        counter_t       m_cPausedRequests;
        /** State of this thread pool. */
        State           m_xState;
        /** The size of each thread stack. */
        size_t          m_sThreadStack;
        /** Maximum worker limit. */
        U32             m_sWorkerLimit;
    };
    template <class Request> typename DEFINE_CONCRETE_RTT(VThreadedProcessor_<Request>);

    template <class Request> void VThreadedProcessor_<Request>::adjustStackSize (size_t &rsSize) const {
        if (rsSize < m_sThreadStack)
            rsSize = m_sThreadStack;
    }
}


#endif

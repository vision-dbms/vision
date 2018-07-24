#ifndef V_VThread_Interface
#define V_VThread_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "V_VThreadSpecific.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VThread : public VThreadSpecific {
        DECLARE_ABSTRACT_RTT (VThread, VThreadSpecific);

    //  Aliases
    public:
        typedef Reference ThreadReference;

    //  Reaper
    public:
        class Reaper {
            DECLARE_FAMILY_MEMBERS (Reaper,void);

            friend class VThread;

        //  Construction
        public:
            Reaper () : m_pThread (Here ()) {
                m_pThread->attach (this);
            }

        //  Destruction
        public:
            ~Reaper () {
                m_pThread->detach (this);
            }

        //  Use
        public:
            void schedule (VReferenceableBase *pObject) {
                m_pReclamationList.add (pObject);
            }
            void reap () {
                m_pReclamationList.reap ();
            }

        //  State
        private:
            ThreadReference const m_pThread;
            VReferenceableBase::ReclamationList m_pReclamationList;
        };
        friend class Reaper;

    //  Key
    private:
        typedef V::VEternal<Key const> EternalKey;
        static EternalKey g_iTLSKey;

    //  State
    public:
        enum State {
            State_Set, /**< Thread has been created but not started. */
            State_Startable, /**< Thread has not yet been created. */
            State_Started /**< Thread has been started. */
        };

    //  StackSize
    private:
        enum {
            STACKSIZE_MIN = 65536  // bytes
        };

    //  Construction
    protected:
        VThread (pthread_t hThread, pthread_id_t xThread);
        VThread ();

    //  Destruction
    protected:
        /**
         * Closes/detaches the thread for this descriptor, reclaiming all thread resources upon completion.
         *
         * Stop should first be called on any VThread descriptors before destruction!
         */
        ~VThread ();

    //  Access
    private:
        virtual Thread *thread_() const OVERRIDE {
            return const_cast<VThread*>(this);
        }
    public:
        static pthread_t CurrentThread ();
        static pthread_id_t CurrentThreadId ();

        static Reference Here ();

        bool isInitialized () const {
            return isSet () || isStarted ();
        }
        bool isntInitialized () const {
            return isntSet () && isntStarted ();
        }

        bool isSet () const {
            return m_xState == State_Set;
        }
        bool isStartable () const {
            return m_xState == State_Startable;
        }
        bool isStarted () const {
            return m_xState == State_Started;
        }

        bool isntSet () const {
            return m_xState != State_Set;
        }
        bool isntStartable () const {
            return m_xState != State_Startable;
        }
        bool isntStarted () const {
            return m_xState != State_Started;
        }

    /**
     * @name Control
     */
    //@{
    private:
        /**
         * Called on thread construction/initialization to adjust the stack size appropriately.
         *
         * @param rsRequested the new stack size (absolute value, not relative).
         */
        virtual void adjustStackSize (size_t &rsRequested) const {
        }
    protected:
        /**
         * Starts (spawns) a thread for this descriptor.
         *
         * @param pThreadProcedure the procedure that the new thread should begin execution with.
         * @param pThreadData any thread data for the new thread.
         * @return true on success, false otherwise.
         */
        bool start (pthread_routine_t *pThreadProcedure, void *pThreadData);

        /**
         * Stops execution of the thread for this descriptor.
         *
         * @return true on success, false otherwise.
         */
        bool stop ();
    //@}

    //  Object Reclamation
    public:
        static void ReclaimObject (VReferenceableBase *pObject) {
            Here ()->reclaimObject (pObject);
        }
        void reclaimObject (VReferenceableBase *pObject) {
            if (m_pReaper)
                m_pReaper->schedule (pObject);
            else {
                Reaper iReaper;
                pObject->die ();
                iReaper.reap ();
            }
        }
    private:
        bool attach (Reaper *pReaper) {
            return m_pReaper.setIfNil (pReaper);
        }
        bool detach (Reaper *pReaper) {
            return m_pReaper.clearIf (pReaper);
        }

    //  Thread Attachment
    protected:
        /**
         * Attaches this descriptor to the thread-local storage map.
         */
        bool becomeSpecific () {
            return g_iTLSKey->setSpecific (this);
        }

    //  State
    private:
        /** State of the thread. */
        State           m_xState;
        pthread_t       m_hThread;
        pthread_id_t    m_xThread;
        Reaper::Pointer m_pReaper;
    };
}


#endif

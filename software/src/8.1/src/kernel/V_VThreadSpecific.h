#ifndef V_VThreadSpecific_Interface
#define V_VThreadSpecific_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VPointer.h"

extern "C" {

#if defined(_WIN32)
#define V_THREAD_ENABLED
#define V_OS_THREAD_API WINAPI

typedef HANDLE pthread_t;
typedef DWORD pthread_id_t;
typedef DWORD pthread_key_t;

typedef DWORD pthread_procedure_result_t;

#elif !defined(sun) || defined(_REENTRANT)
#define V_THREAD_ENABLED
#include <pthread.h>

#if defined(__VMS)
typedef __pthreadLongUint_t pthread_id_t;
#else
typedef unsigned int pthread_id_t;
#endif

typedef void *pthread_procedure_result_t;

#else
typedef unsigned int pthread_t;
typedef unsigned int pthread_id_t;
typedef unsigned int pthread_key_t;
typedef unsigned int pthread_procedure_result_t;

#endif

#ifndef V_OS_THREAD_API
#define V_OS_THREAD_API
#endif

typedef void V_OS_THREAD_API pthread_key_destructor_t (void*);
typedef pthread_procedure_result_t V_OS_THREAD_API pthread_routine_t(void*);

}

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class VThread;

  /******************************************************
   *----  Thread Specific Referenceable Base Class  ----*
   ******************************************************/

    class V_API VThreadSpecific : public VReferenceable {
	DECLARE_ABSTRACT_RTT (VThreadSpecific, VReferenceable);

    //  Aliases
    public:
	typedef VThread Thread;
	typedef ThisClass ThreadSpecific;
	typedef void (ThreadProcedure)(void *);

    //  Key
    public:
        /**
         * Serves as an identifier for thread-specific variables.
         * Exaclty one or zero value instances exist per Key per thread.
         * Depending on the thread in context, Key objects will retrieve different value instances.
         * Unlike conventional map models, VThreadSpecific::Key objects are thought of as _containing_ their value, which can be set or retrieved through methods in Key.
         */
	class V_API Key {
	//  Aliases
	public:
	    typedef VPointer<Key> Pointer;

	//  Globals
	private:
	    static Pointer &KeyListHead ();

	//  Construction
	public:
	    Key ();

	//  Destruction
	public:
	    ~Key ();

	//  Thread Association
	public:
	    static void OnThreadTermination ();

            /**
             * Retrieves the value specified by this Key.
             * 
             * @param rpSpecific reference to a ThreadSpecific that will be set to the value specified by this Key.
             * @return true on success, false otherwise.
             */
	    bool getSpecific (VReference<ThreadSpecific> &rpSpecific) const;
            /**
             * Sets the value specified by this Key.
             * 
             * @param pSpecific pointer to a ThreadSpecific that will be used as the new value for this Key.
             * @return true on success, false otherwise.
             */
	    bool setSpecific (ThreadSpecific *pSpecific) const;

	//  State
	private:
	    Pointer		m_pSuccessor;
	    Pointer		m_pPredecessor;
	    pthread_key_t	m_hKey;
	    bool		m_bKey;
	};
	friend class Key;

    //  Construction
    protected:
	VThreadSpecific (Thread *pThread = 0);

    //  Destruction
    protected:
	~VThreadSpecific ();

    //  Thread Association
    private:
	static void OnThreadTermination (ThisClass *pThis);
    protected:
	virtual void onThreadAttach ();	// ... must be called first from override
	virtual void onThreadDetach ();	// ... must be called last  from override

    //  Access
	virtual Thread *thread_() const {
	    return m_pThread;
	}
    public:
	Thread *thread () const {
	    return thread_();
	}

    //  State
    private:
	VReference<Thread> const m_pThread;
    };


    /**
     * Thread specific datum template (use sparingly).
     * Should be used sparingly because the number of Keys allowed per platform vary.
     */
    template <typename T> class VThreadSpecificBinding_ : public VThreadSpecific {
	DECLARE_CONCRETE_RTT (VThreadSpecificBinding_<T>, VThreadSpecific);

    //  Construction
    public:
	VThreadSpecificBinding_(T iT) : m_iT (iT) {
	}

    //  Destruction
    private:
	~VThreadSpecificBinding_() {
	}

    //  Access
    public:
	T value () const {
	    return m_iT;
	}

    //  Update
    public:
	void setTo (T iT) {
	    m_iT = iT;
	}

    //  State
    private:
	T m_iT;
    };
    template <typename T> typename DEFINE_CONCRETE_RTT (VThreadSpecificBinding_<T>);

    /**
     * Convenience class for using thread-specific static variables of any given type.
     * Provides a convenient and syntactically familiar method for getting and setting thread-specific static variables of the templated type.
     * Typical usage will include: declaration/construction; usage of the operator=; pseudocasting (usage as though it were an object of templated type).
     */
    template <typename T> class VThreadSpecific_ : VThreadSpecific::Key {
	DECLARE_FAMILY_MEMBERS (VThreadSpecific_<T>, VThreadSpecific::Key);

    //  Binding
    public:
	typedef VThreadSpecificBinding_<T> Binding;

    //  Construction
    public:
	VThreadSpecific_() {
	}

    //  Destruction
    public:
	~VThreadSpecific_() {
	}

    //  Access
    public:
	operator T () const {
	    return value ();
	}
	T value () const {
	    VReference<VThreadSpecific> pBinding;
	    return getSpecific (pBinding) ? static_cast<Binding*>(pBinding.referent ())->value () : T ();
	}

    //  Update
    public:
	ThisClass &operator= (ThisClass const &rOther) {
	    setTo (rOther);
	    return *this;
	}
	ThisClass &operator= (T iT) {
	    setTo (iT);
	    return *this;
	}
	void setTo (T iT) {
	    VReference<VThreadSpecific> pBinding;
	    if (getSpecific (pBinding))
		static_cast<Binding*>(pBinding.referent ())->setTo (iT);
	    else {
		pBinding.setTo (new Binding (iT));
		setSpecific (pBinding);
	    }
	}
    };
}


#endif

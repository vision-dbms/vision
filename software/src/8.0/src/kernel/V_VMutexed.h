#ifndef V_VMutexed_Interface
#define V_VMutexed_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VSpinlock.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /**************************************************
     *----  template <typename T> class VMutexed  ----*
     **************************************************/

    template <typename T> class VMutexed {
	DECLARE_FAMILY_MEMBERS (VMutexed<T>, void);

    //  Aliases
    public:
	typedef ThisClass Instance;
	typedef VSpinlock mutex_t;
	typedef T value_t;

    //  Access
    public:
	class Access {
	    DECLARE_FAMILY_MEMBERS (Access, void);

	//  Construction
	public:
	    Access (VMutexed &rInstance) : m_iMutexClaim (rInstance.m_iMutex), m_rValue (rInstance.m_iValue) {
	    }

	//  Destruction
	public:
	    ~Access () {
	    }

	//  Access
	public:
	    T const* operator-> () const {
		return &m_rValue;
	    }
	    T const& operator* () const {
		return m_rValue;
	    }

	    T* operator-> () {
		return &m_rValue;
	    }
	    T& operator* () {
		return m_rValue;
	    }

	//  State
	private:
	    mutex_t::Claim m_iMutexClaim;
	    T &m_rValue;
	};
	friend class Access;

    //  Construction
    public:
	template <typename t1,typename t2,typename t3,typename t4> VMutexed (t1 v1,t2 v2,t3 v3,t4 v4) : m_iValue (v1,v2,v3,v4) {
	}
	template <typename t1,typename t2,typename t3> VMutexed (t1 v1,t2 v2,t3 v3) : m_iValue (v1,v2,v3) {
	}
	template <typename t1,typename t2> VMutexed (t1 v1,t2 v2) : m_iValue (v1,v2) {
	}
	template <typename t1> VMutexed (t1 v1) : m_iValue (v1) {
	}
	VMutexed () {
	}

    //  Destruction
    public:
	~VMutexed () {
	}

    //  State
    private:
	T m_iValue;
	mutex_t m_iMutex;
    };
}


#endif

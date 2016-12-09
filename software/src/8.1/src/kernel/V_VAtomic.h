#ifndef V_VAtomic_Interface
#define V_VAtomic_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAtomicMemoryOperations_.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /***************************************************************
     *----  template <typename Val_T> class VAtomicEquivalent  ----*
     ***************************************************************/

    template <size_t Size> struct VAtomicEquivalentForSize {
    };
    template <> struct VAtomicEquivalentForSize<static_cast<size_t>(1)> {
	typedef int var_t;
    };
    template <> struct VAtomicEquivalentForSize<static_cast<size_t>(2)> {
	typedef int var_t;
    };
    template <> struct VAtomicEquivalentForSize<static_cast<size_t>(4)> {
	typedef int var_t;
    };
    template <> struct VAtomicEquivalentForSize<static_cast<size_t>(8)> {
	typedef __int64 var_t;
    };

    template <typename Val_T> class VAtomicEquivalent : public VAtomicEquivalentForSize <sizeof (Val_T)> {
    };

    /****************************************************************************************************************
     *----  template <typename Val_T, typename Var_T = typename VAtomicEquivalent<Val_T>::var_t> class VAtomic  ----*
     ****************************************************************************************************************/

    template <typename Val_T, typename Var_T = typename VAtomicEquivalent<Val_T>::var_t> class VAtomic {
	typedef BOGUS_TYPENAME VAtomic<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, void);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;

	typedef V::VAtomicMemoryOperations_<sizeof (var_t)> atomic_operations_t;
	typedef typename atomic_operations_t::value_t atomic_memory_t;

    //  Construction
    public:
	VAtomic (ThisClass const &rOther) : m_iValue (rOther.m_iValue) {
	}
	VAtomic (val_t iValue) : m_iValue (iValue) {
	}

    //  Destruction
    public:
	~VAtomic () {
	}

    //  Access
    public:
	operator val_t () const {
	    return value ();
	}
	val_t value () const {
	    return static_cast<val_t>(m_iValue);
	}

    //  Update
    public:
	ThisClass& operator= (val_t iNewValue) {
	    setTo (iNewValue);
	    return *this;
	}
	void setTo (val_t iNewValue) {
	    m_iValue = static_cast<atomic_memory_t>(iNewValue);
	}
	bool setIf (val_t iOldValue, val_t iNewValue) {
	    return atomic_operations_t::interlockedSetIf (&m_iValue, iOldValue, iNewValue);
	}

    //  State
    private:
	atomic_memory_t m_iValue;
    };


    /*********************
     *----  Aliases  ----*
     *********************/

    typedef VAtomic<bool>		VAtomic_bool;
    typedef VAtomic<int>		VAtomic_s32;
    typedef VAtomic<unsigned int>	VAtomic_u32;
    typedef VAtomic<__int64>		VAtomic_s64;
    typedef VAtomic<unsigned __int64>	VAtomic_u64;
}


#endif

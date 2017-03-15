#ifndef V_VCounter_Interface
#define V_VCounter_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicOperations_.h"
#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template <class Manager_T> class VCounter {
	DECLARE_NUCLEAR_FAMILY (VCounter<Manager_T>);

    //  Aliases
    public:
	typedef Manager_T manager_t;
	typedef typename Manager_T::storage_t storage_t;
	typedef typename Manager_T::value_t value_t;

    //  Construction
    public:
	VCounter (value_t iValue) : m_iValue (iValue) {
	}
	VCounter () : m_iValue (Manager_T::zero ()) {
	}

    //  Destruction
    public:
	~VCounter () {
	}

    //  Access/Query
    public:
	operator value_t () const {
	    return m_iValue;
	}
	value_t value () const {
	    return m_iValue;
	}
	bool isZero () {  // return true if result is zero
	    return Manager_T::isZero (m_iValue);
	}
        bool isntZero() {
            return !isZero();
        }
    protected:
	storage_t const &storage () const {
	    return m_iValue;
	}
	storage_t &storage () {
	    return m_iValue;
	}

    //  Update
    public:
	ThisClass &operator= (value_t iValue) {
	    m_iValue = iValue;
	    return *this;
	}

	value_t operator++ () {
	    return Manager_T::incrementAndFetch (m_iValue);
	}
	value_t operator++ (int) {
	    return Manager_T::fetchAndIncrement (m_iValue);
	}

	value_t operator-- () {
	    return Manager_T::decrementAndFetch (m_iValue);
	}
	value_t operator-- (int) {
	    return Manager_T::fetchAndDecrement (m_iValue);
	}

	void increment () {
	    Manager_T::increment (m_iValue);
	}
	void decrement () {
	    Manager_T::decrement (m_iValue);
	}
	bool decrementIsZero () {  // return true if result is zero
	    return Manager_T::decrementIsZero (m_iValue);
	}

	value_t claim () {
	    return setTo (Manager_T::zero ());
	}
	value_t setTo (value_t iValue) {
	    return Manager_T::interlockedExchange (&m_iValue, iValue);
	}

    private:
	storage_t m_iValue;
    };

    /****************************************************************/
    template <> class VCounter<unsigned __int32> : public VCounter<VAtomicOperations_<unsigned __int32> > {
	DECLARE_FAMILY_MEMBERS (VCounter<value_t>, VCounter<VAtomicOperations_<unsigned __int32> >);

	//  Construction
    public:
	VCounter (value_t iValue) : BaseClass (iValue) {
	}
	VCounter () {
	}
    };

    /****************************************************************/
    template <> class VCounter<unsigned __int64> : public VCounter<VAtomicOperations_<unsigned __int64> > {
	DECLARE_FAMILY_MEMBERS (VCounter<value_t>, VCounter<VAtomicOperations_<unsigned __int64> >);

	//  Construction
    public:
	VCounter (value_t iValue) : BaseClass (iValue) {
	}
	VCounter () {
	}
    };

    /****************************************************************/
    typedef VCounter<unsigned __int32> unsigned_int32_count_t;
    typedef unsigned_int32_count_t counter32_t;

    typedef VCounter<unsigned __int64> unsigned_int64_count_t;
    typedef unsigned_int64_count_t counter64_t;

    typedef VCounter<VAtomicNilOperations_<unsigned __int32> > unsigned_int32_nilcount_t;
    typedef unsigned_int32_nilcount_t counter32nil_t;

    typedef VCounter<VAtomicNilOperations_<unsigned __int64> > unsigned_int64_nilcount_t;
    typedef unsigned_int64_nilcount_t counter64nil_t;
}


#endif

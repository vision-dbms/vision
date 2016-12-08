#ifndef Vca_VTrackableCounter_Interface
#define Vca_VTrackableCounter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTrackable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VCounter.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /************************************************************************************
     *----  template <class CounterOperations_T> class VTrackableCounterOperations  ----*
     ************************************************************************************/

    template <class CounterOperations_T> class VTrackableCounterOperations {
	DECLARE_NUCLEAR_FAMILY (VTrackableCounterOperations<CounterOperations_T>);

    //  Aliases
    public:
	typedef typename CounterOperations_T::value_t value_t;

    //  Storage
    public:
	class storage_t : public VTrackable<value_t> {
	    DECLARE_FAMILY_MEMBERS (storage_t, VTrackable<value_t>);

	//  Construction
	public:
	    storage_t (value_t iValue) : BaseClass (iValue) {
	    }
	    storage_t () : BaseClass (CounterOperations_T::zero ()) {
	    }

	//  Access
	public:
	    bool isZero () const {
		return CounterOperations_T::isZero (BaseClass::m_iValue);
	    }

	//  Update
	public:
	    void increment () {
		incrementAndFetch ();
	    }
	    void decrement () {
		decrementAndFetch ();
	    }
	    bool decrementIsZero () {
		return 0 == decrementAndFetch ();
	    }

	    value_t fetchAndIncrement () {
		return onChangeToSuccessorOf (CounterOperations_T::fetchAndIncrement (BaseClass::m_iValue));
	    }
	    value_t fetchAndDecrement () {
		return onChangeToPredecessorOf (CounterOperations_T::fetchAndDecrement (BaseClass::m_iValue));
	    }

	    value_t incrementAndFetch () {
		return onChangeTo (CounterOperations_T::incrementAndFetch (BaseClass::m_iValue));
	    }
	    value_t decrementAndFetch () {
		return onChangeTo (CounterOperations_T::decrementAndFetch (BaseClass::m_iValue));
	    }

	//  Tracking
	private:
	    using BaseClass::onChangeTo;
	    value_t onChangeToSuccessorOf (value_t iValue) {
		onChangeTo (iValue + 1);
		return iValue;
	    }
	    value_t onChangeToPredecessorOf (value_t iValue) {
		onChangeTo (iValue - 1);
		return iValue;
	    }
	};

    //  Access
    public:
	static bool isZero (storage_t const &rValue) {
	    return rValue.isZero ();
	}
	static value_t zero () {
	    return CounterOperations_T::zero ();
	}

    //  Operations
    public:
	static void increment (storage_t &rValue) {
	    rValue.increment ();
	}
	static void decrement (storage_t &rValue) {
	    rValue.decrement ();
	}
	static bool decrementIsZero (storage_t &rValue) {
	    return rValue.decrementIsZero ();
	}

	static value_t fetchAndIncrement (storage_t &rValue) {
	    return rValue.fetchAndIncrement ();
	}
	static value_t fetchAndDecrement (storage_t &rValue) {
	    return rValue.fetchAndDecrement ();
	}

	static value_t incrementAndFetch (storage_t &rValue) {
	    return rValue.incrementAndFetch ();
	}
	static value_t decrementAndFetch (storage_t &rValue) {
	    return rValue.decrementAndFetch ();
	}
    };


    /****************************************************************
     *----  template <class Counter_T> class VTrackableCounter  ----*
     ****************************************************************/

    template <class Counter_T> class VTrackableCounter
	: public V::VCounter<VTrackableCounterOperations<typename Counter_T::manager_t> >
    {
	DECLARE_FAMILY_MEMBERS (VTrackableCounter<Counter_T>, V::VCounter<VTrackableCounterOperations<typename Counter_T::manager_t> >);

    //  Aliases
    public:
	typedef typename BaseClass::value_t value_t;
	typedef VTrackable<value_t> trackable_t;

    //  Construction
    public:
	VTrackableCounter (value_t iValue) : BaseClass (iValue) {
	}
	VTrackableCounter () {
	}

    //  Access
    public:
	trackable_t const &trackable () const {
	    return BaseClass::storage ();
	}
    };
}


#endif

#ifndef Vca_VInstanceOf_Interface
#define Vca_VInstanceOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInstance.h"

#include "V_VAtomic.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /***************************************************************************************************************
     *----  template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceOf  ----*
     ***************************************************************************************************************/

    /**
     * Represents a cached value and potentially a provider (Gofer) for it.
     */
    template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VInstanceOf {
	typedef BOGUS_TYPENAME VInstanceOf<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, void);

    //  Status Enumeration
    public:
	enum status_t {
	    Status_Unset,       /**< Uninitialized value. */
            Status_Triggered,   /**< Value being computed. */
            Status_Locked,      /**< Thread-safety lock for when value is being validated. */
            Status_Set          /**< Value is cached. */
	};

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;
	typedef VGoferInstance<val_t,var_t> gofer_t;
	typedef typename gofer_t::Reference gofer_reference_t;

	typedef V::VAtomic<status_t> atomic_status_t;

    //  Construction
    public:
	VInstanceOf (ThisClass const &rOther) : m_pGofer (rOther.m_pGofer), m_iValue (rOther.m_iValue), m_xStatus (
	    m_pGofer.isNil () ? Status_Set : Status_Unset
	) {
	}
	VInstanceOf (gofer_t *pGofer) : m_pGofer (pGofer), m_xStatus (Status_Unset) {
	}
	VInstanceOf (val_t iValue) : m_pGofer (0), m_iValue (iValue), m_xStatus (Status_Set) {
	}

    //  Destruction
    public:
	~VInstanceOf () {
	}

    //  Materialization
    public:
        /**
         * Materializes an instance of val_t.
         * 
         * If such an instance has not already materialized, m_pGofer is used to retrieve one and return.
         * Otherwise, we validate the cached value using Gofer::validatesDatum. If validation succeeds, we return. If validation fails, we clear our cache (m_xStatus = Status_Unset) and retry materialization.
         */
	void materializeFor (VGofer *pGofer) {
	    bool bNotDone = false;
            // This loop only ever runs twice (neglecting crazy threaded conditions).
	    do {
		bNotDone = false;
		if (m_xStatus.setIf (Status_Unset, Status_Triggered)) {
		//  If our value isn't set, 'gofer' it...
		    pGofer->suspend ();
		    m_pGofer->supplyVariable (pGofer, *this);
		} else if (m_pGofer && m_xStatus.setIf (Status_Set, Status_Locked)) {
		//  If our 'gofered' value is no longer valid, 'gofer' it again...
		    bNotDone = !m_pGofer->validatesDatum (m_iValue);
		    m_xStatus = bNotDone ? Status_Unset : Status_Set;
		}
	    } while (bNotDone);
	}

    //  Access
    public:
	operator val_t () const {
	    return m_iValue;
	}
	val_t value () const {
	    return m_iValue;
	}

    //  Query
    public:
	bool isSet () const {
	    return m_xStatus == Status_Set;
	}
	bool isntSet () const {
	    return m_xStatus != Status_Set;
	}

    //  Update
    public:
	template <typename value_t> ThisClass& operator= (value_t iValue) {
	    setTo (iValue);
	    return *this;
	}
	bool setTo (val_t iValue) {
	    if (m_xStatus.setIf (Status_Triggered, Status_Locked)) {
		m_iValue = iValue;
		m_xStatus = Status_Set;
	    }
	    return isSet ();
	}
	bool setTo (IError* pInterface, VString const& rMessage) {
	    return m_xStatus.setIf (Status_Triggered, Status_Unset)
		|| m_xStatus.setIf (Status_Locked   , Status_Unset)
		|| m_xStatus.setIf (Status_Set      , Status_Unset);
	}

    //  State
    private:
	gofer_reference_t const	m_pGofer;
	var_t			m_iValue;
	atomic_status_t		m_xStatus;
    };


    /*********************
     *----  Aliases  ----*
     *********************/

    typedef VInstanceOf<bool>	VInstanceOf_bool;

    typedef VInstanceOf<F32>	VInstanceOf_F32;
    typedef VInstanceOf<F64>	VInstanceOf_F64;

    typedef VInstanceOf<S08>	VInstanceOf_S08;
    typedef VInstanceOf<S16>	VInstanceOf_S16;
    typedef VInstanceOf<S32>	VInstanceOf_S32;

    typedef VInstanceOf<U08>	VInstanceOf_U08;
    typedef VInstanceOf<U16>	VInstanceOf_U16;
    typedef VInstanceOf<U32>	VInstanceOf_U32;

    typedef VInstanceOf<VString const&> VInstanceOf_String;
}


#endif

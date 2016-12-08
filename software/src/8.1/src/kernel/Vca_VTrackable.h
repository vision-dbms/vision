#ifndef Vca_VTrackable_Interface
#define Vca_VTrackable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPublication.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "IVReceiver.h"
#include "Vca_IRequest.h"

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /************************************************************************
     *----  template <typename Val_T, typename Var_T> class VTrackable  ----*
     ************************************************************************/

    template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VTrackable {
    public:
	typedef BOGUS_TYPENAME VTrackable<Val_T,Var_T> this_t;
	DECLARE_NUCLEAR_FAMILY (this_t);

    //  Aliases
    public:
	typedef Val_T value_t;
	typedef IVReceiver<IRequest*> ITicketSink;
        typedef IVReceiver<Val_T> IValueSink;

    //  Construction
    public:
	VTrackable (Val_T iValue) : m_iValue (iValue) {
	}

    //  Destruction
    public:
	~VTrackable () {
	}

    //  Access
    public:
	operator value_t () const {
	    return value ();
	}

	bool hasSubscribers () const {
	    return m_pubValueSink.hasSubscribers ();
	}
	value_t value () const {
	    return m_iValue;
	}

    //  Update
    public:
	ThisClass &operator= (Val_T iValue) {
	    setTo (iValue);
	    return *this;
	}
	void setTo (Val_T iValue) {
	    m_iValue = iValue;
	    onChangeTo (iValue);
	}

    //  Tracking
    public:
	bool supplyAndTrack (IValueSink* pValueSink, ITicketSink *pTicketSink) const {
	    if (pValueSink) {
		pValueSink->OnData (m_iValue);
		if (pTicketSink)
		    m_pubValueSink.addSubscriber (pTicketSink, pValueSink);
		return true;
	    }
	    return false;
	}
    protected:
	Val_T onChangeTo (Val_T iValue) const {
	    if (m_pubValueSink.hasSubscribers ())
		m_pubValueSink->OnData (iValue);
	    return iValue;
	}

    //  State
    private:
	VPublication<IValueSink> mutable m_pubValueSink;
    protected:
	Var_T volatile m_iValue;
    };


    /*********************
     *----  Aliases  ----*
     *********************/

    typedef VTrackable<count_t> VTrackable_count_t;

    typedef VTrackable<F32> VTrackable_F32;
    typedef VTrackable<F64> VTrackable_F64;

    typedef VTrackable<S08> VTrackable_S08;
    typedef VTrackable<S16> VTrackable_S16;
    typedef VTrackable<S32> VTrackable_S32;
    typedef VTrackable<S64> VTrackable_S64;

    typedef VTrackable<U08> VTrackable_U08;
    typedef VTrackable<U16> VTrackable_U16;
    typedef VTrackable<U32> VTrackable_U32;
    typedef VTrackable<U64> VTrackable_U64;

    typedef VTrackable<VString> VTrackable_String;
}


#endif

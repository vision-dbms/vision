#ifndef Vca_VClassInfoHolder_Interface
#define Vca_VClassInfoHolder_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "VkMapOf.h"
#include "VkSetOf.h"

#include "V_VRTTI.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "IVReceiver.h"
#include "Vca_IRequest.h"
#include "Vca_VTrackable.h"
#include "Vca_VTypePattern.h"

#include "V_VString.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

/**************************
 *----  class Unused  ----*
 **************************/

    class Unused {};

/************************************
 *----  class VClassInfoHolder  ----*
 ************************************/

    class Vca_API VClassInfoHolder {
	DECLARE_NUCLEAR_FAMILY (VClassInfoHolder);

    //  Aliases
    public:
	typedef IVReceiver<IRequest*> ITicketSink;

    /**********************************************
     *----  class VClassInfoHolder::Property  ----*
     **********************************************/

    public:
	class Vca_API Property : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Property, VRolePlayer);

	//  Construction
	protected:
	    Property ();

	//  Destruction
	protected:
	    ~Property ();

	//  Access
	private:
	    template <typename sink_t> bool getTrackableImpl (sink_t *pValueSink, VRolePlayer *pContainer) const {
		return pValueSink && getValue (pValueSink, pContainer);
	    }
	    bool sayNoToSubscription (ITicketSink *pTicketSink) const;
	public:
	    virtual bool getTrackableValue (IClient *pValueSink, ITicketSink *pTicketSink, VRolePlayer *pContainer) const;

	    virtual bool getValue (IVReceiver<VString const&  > *pValueSink, VRolePlayer *pContainer) const = 0;
	    virtual bool getValue (IVReceiver<unsigned int    > *pValueSink, VRolePlayer *pContainer) const = 0;
	    virtual bool getValue (IVReceiver<unsigned __int64> *pValueSink, VRolePlayer *pContainer) const = 0;
	    virtual bool getValue (IVReceiver<float           > *pValueSink, VRolePlayer *pContainer) const = 0;
	    virtual bool getValue (IVReceiver<double          > *pValueSink, VRolePlayer *pContainer) const = 0;
	protected:
	    template <class Container_T> static bool GetContainer (Container_T *&rpContainer, VRolePlayer *pContainer) {
		return (rpContainer = dynamic_cast<Container_T*>(pContainer)) ? true : false;
	    }
	protected:
	    template <typename sink_t, typename src_t> static bool SetResult (sink_t *pResultSink, src_t iSource) {
		typename VTypePattern<typename sink_t::DataType>::var_t iValue;
		if (!SetResult (iValue, iSource))
		    return false;
		pResultSink->OnData (iValue);
		return true;
	    }
	    template <typename dst_t, typename src_t> static bool SetResult (dst_t &rResult, src_t iSource) {
		return false;
	    }

	    //  Target F32
	    static bool SetResult (F32 &rResult, F32 iSource) {
		rResult = iSource;
		return true;
	    }
 	    static bool SetResult (F32 &rResult, F64 iValue) {
		rResult = static_cast<F32>(iValue);
		return true;
	    }
 	    static bool SetResult (F32 &rResult, S32 iValue) {
		rResult = static_cast<F32>(iValue);
		return true;
	    }
 	    static bool SetResult (F32 &rResult, S64 iValue) {
		rResult = static_cast<F32>(iValue);
		return true;
	    }
 	    static bool SetResult (F32 &rResult, U32 iValue) {
		rResult = static_cast<F32>(iValue);
		return true;
	    }
 	    static bool SetResult (F32 &rResult, U64 iValue) {
		rResult = static_cast<F32>(iValue);
		return true;
	    }

	    //  Target F64
	    static bool SetResult (F64 &rResult, F32 iValue) {
		rResult = iValue;
		return true;
	    }
	    static bool SetResult (F64 &rResult, F64 iSource) {
		rResult = iSource;
		return true;
	    }
 	    static bool SetResult (F64 &rResult, S32 iValue) {
		rResult = iValue;
		return true;
	    }
 	    static bool SetResult (F64 &rResult, S64 iValue) {
		rResult = static_cast<F64>(iValue);
		return true;
	    }
	    static bool SetResult (F64 &rResult, U32 iValue) {
		rResult = iValue;
		return true;
	    }
	    static bool SetResult (F64 &rResult, U64 iValue) {
		rResult = static_cast<F64>(iValue);
		return true;
	    }

	    //  Target U32
	    static bool SetResult (U32 &rResult, bool iValue) {
		rResult = iValue;
		return true;
	    }
	    static bool SetResult (U32 &rResult, S32 iValue) {
		if (iValue < 0)
		    return false;
		rResult = static_cast<U32>(iValue);
		return true;
	    }
	    static bool SetResult (U32 &rResult, S64 iValue) {
		if (iValue < 0 || iValue > UINT_MAX)
		    return false;
		rResult = static_cast<U32>(iValue);
		return true;
	    }
	    static bool SetResult (U32 &rResult, U32 iSource) {
		rResult = iSource;
		return true;
	    }
	    static bool SetResult (U32 &rResult, U64 iValue) {
		if (iValue > UINT_MAX)
		    return false;
		rResult = static_cast<U32>(iValue);
		return true;
	    }

	    //  Target U64
	    static bool SetResult (U64 &rResult, bool iValue) {
		rResult = iValue;
		return true;
	    }
	    static bool SetResult (U64 &rResult, S32 iValue) {
		if (iValue < 0)
		    return false;
		rResult = static_cast<U64>(iValue);
		return true;
	    }
	    static bool SetResult (U64 &rResult, S64 iValue) {
		if (iValue < 0)
		    return false;
		rResult = static_cast<U64>(iValue);
		return true;
	    }
	    static bool SetResult (U64 &rResult, U32 iValue) {
		rResult = iValue;
		return true;
	    }
	    static bool SetResult (U64 &rResult, U64 iSource) {
		rResult = iSource;
		return true;
	    }

	    //  Target VString
	    static bool SetResult (VString &rResult, VString iSource) {
		rResult = iSource;
		return true;
	    }
	};


    /***********************************************************************************
     *----  template <typename T1, typename T2> class VClassInfoHolder::Property_  ----*
     ***********************************************************************************/

    public:
	template <typename T1, typename T2 = Unused> class Property_ : public Property {
	};


    /********************************************************************************************************************************************
     *----  template <class Container_T, typename Value_T> class VClassInfoHolder::Property_<VTrackable<Value_T> const&(Container_T::*)()>  ----*
     *----                                                                                                                                  ----*
     *                                                                                                                                          *
     *      AKA: Property_ specialization for pointers to Container_T member functions returning a reference to a trackable Value_T.            *
     *                                                                                                                                          *
     ********************************************************************************************************************************************/
    public:
	template <class Container_T, typename Value_T>
	    class Property_<VTrackable<Value_T> const&(Container_T::*)()const> : public Property {
	public:
	    typedef VTrackable<Value_T> const&(Container_T::*accessor_t)()const;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Aliases
	public:
	    typedef IVReceiver<Value_T> IValueSink;

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer) && SetResult (pResultSink, (pCastContainer->*m_pAccessor)().value ());
	    }
	public:
	    bool getTrackableValue (IClient *pValueSink, ITicketSink *pTicketSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer)
		    && (pCastContainer->*m_pAccessor)().supplyAndTrack (dynamic_cast<IValueSink*>(pValueSink), pTicketSink);
	    }
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /**********************************************************************************************************************************
     *----  template <class Container_T, typename Value_T> class VClassInfoHolder::Property_<VTrackable<Value_T> Container_T::*>  ----*
     *----                                                                                                                        ----*
     *                                                                                                                                *
     *      AKA: Property_ specialization for Container_T members of type VTrackable<Value_T>.                                        *
     *                                                                                                                                *
     **********************************************************************************************************************************/
    public:
	template <typename Container_T, typename Value_T>
	    class Property_<VTrackable<Value_T> Container_T::*> : public Property {
	public:
	    typedef VTrackable<Value_T> Container_T::*accessor_t;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Aliases
	public:
	    typedef IVReceiver<Value_T> IValueSink;

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer) && SetResult (pResultSink, pCastContainer->*m_pAccessor);
	    }
	public:
	    bool getTrackableValue (IClient *pValueSink, ITicketSink *pTicketSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer)
		    && (pCastContainer->*m_pAccessor).supplyAndTrack (dynamic_cast<IValueSink*>(pValueSink), pTicketSink);
	    }
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /************************************************************************************************************
     *----  template <typename Value_T> class VClassInfoHolder::Property_<VTrackable<Value_T> const&(*)()>  ----*
     *----                                                                                                  ----*
     *                                                                                                          *
     *      AKA: Property_ specialization for pointers to functions returning a reference to a trackable        *
     *           Value_T.                                                                                       *
     *                                                                                                          *
     ************************************************************************************************************/
    public:
	template <typename Value_T> class Property_<VTrackable<Value_T> const&(*)()> : public Property {
	public:
	    typedef VTrackable<Value_T> const&(*accessor_t)();
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Aliases
	public:
	    typedef IVReceiver<Value_T> IValueSink;

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		return SetResult (pResultSink, (*m_pAccessor)().value ());
	    }
	public:
	    bool getTrackableValue (IClient *pValueSink, ITicketSink *pTicketSink, VRolePlayer *pContainer) const {
		return (*m_pAccessor)().supplyAndTrack (dynamic_cast<IValueSink*>(pValueSink), pTicketSink);
	    }
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /**************************************************************************************************************************
     *----  template <class Container_T, typename Value_T> class VClassInfoHolder::Property_<Value_T (Container_T::*)()>  ----*
     *----                                                                                                                ----*
     *                                                                                                                        *
     *      AKA: Property_ specialization for pointers to Container_T member functions returning a Value_T.                   *
     *                                                                                                                        *
     **************************************************************************************************************************/
    public:
	template <class Container_T, typename Value_T>
	    class Property_<Value_T(Container_T::*)()const> : public Property {
	public:
	    typedef Value_T (Container_T::*accessor_t)() const;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer) && SetResult (pResultSink, (pCastContainer->*m_pAccessor)());
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /*******************************************************************************************
     *----  template <typename Value_T> class VClassInfoHolder::Property_<Value_T (*)()>   ----*
     *----                                                                                 ----*
     *                                                                                         *
     *      AKA: Property_ specialization for pointers to functions returning a Value_T.       *
     *                                                                                         *
     *******************************************************************************************/
    public:
	template <typename Value_T> class Property_<Value_T(*)()> : public Property {
	public:
	    typedef Value_T (*accessor_t)();
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		return SetResult (pResultSink, (*m_pAccessor)());
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /**********************************************************************************************************************
     *----  template <class Container_T, typename Value_T> class VClassInfoHolder::Property_<Value_T Container_T::*>  ----*
     *----                                                                                                            ----*
     *                                                                                                                    *
     *      AKA: Property_ specialization for Container_T members of type Value_T.                                        *
     *                                                                                                                    *
     **********************************************************************************************************************/
    public:
	template <typename Container_T, typename Value_T>
	    class Property_<Value_T Container_T::*> : public Property {
	public:
	    typedef Value_T Container_T::*accessor_t;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		return GetContainer (pCastContainer, pContainer) && SetResult (pResultSink, pCastContainer->*m_pAccessor);
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /*************************************************************************************
     *----  template <typename Value_T> class VClassInfoHolder::Property_<Value_T&>  ----*
     *----                                                                           ----*
     *                                                                                   *
     *      AKA: Property_ specialization for references to objects of type Value_T.     *
     *                                                                                   *
     *************************************************************************************/
    public:
	template <typename Value_T> class Property_<Value_T*> : public Property {
	public:
	    typedef Value_T &accessor_t;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		return SetResult (pResultSink, m_pAccessor);
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t m_pAccessor;
	};


    /*******************************************************************************************************************************************
     *----  template <class Container_T, typename Value_T> class VClassInfoHolder::Property_<void (Container_T::*)(IVReceiver<Value_T>*)>  ----*
     *----                                                                                                                                 ----*
     *                                                                                                                                         *
     *      AKA: Property_ specialization for Container_T member functions taking a pointer to an IVReceiver<Value_T>.                         *
     *                                                                                                                                         *
     *******************************************************************************************************************************************/

    public:
	template <typename Container_T, typename Value_T>
	    class Property_<void (Container_T::*)(IVReceiver<Value_T>*) const> : public Property {
	public:
	    typedef void (Container_T::*accessor_t)(IVReceiver<Value_T>*) const;
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		return false;
	    }
	    bool getValueImpl (IVReceiver<Value_T> *pResultSink, VRolePlayer *pContainer) const {
		Container_T *pCastContainer = 0;
		if (!GetContainer (pCastContainer, pContainer))
		    return false;
		(pCastContainer->*m_pAccessor) (pResultSink);
		return true;
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /***********************************************************************************************************
     *----  template <typename Value_T> class VClassInfoHolder::Property_<void (*)(IVReceiver<Value_T>*)>  ----*
     *----                                                                                                 ----*
     *                                                                                                         *
     *      AKA: Property_ specialization for functions taking a pointer to an IVReceiver<Value_T>.            *
     *                                                                                                         *
     ***********************************************************************************************************/

    public:
	template <typename Value_T> class Property_<void (*)(IVReceiver<Value_T>*)> : public Property {
	public:
	    typedef void (*accessor_t)(IVReceiver<Value_T>*);
	    typedef Property_<accessor_t> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, Property);

	//  Construction
	public:
	    Property_(accessor_t pAccessor) : m_pAccessor (pAccessor) {
	    }

	//  Destruction
	protected:
	    ~Property_() {
	    }

	//  Access
	private:
	    template <typename sink_t> bool getValueImpl (sink_t *pResultSink, VRolePlayer *pContainer) const {
		return false;
	    }
	    bool getValueImpl (IVReceiver<Value_T> *pResultSink, VRolePlayer *pContainer) const {
		(*m_pAccessor) (pResultSink);
 		return true;
	    }
	public:
	    bool getValue (IVReceiver<VString const&> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned int> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<unsigned __int64> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<float> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }
	    bool getValue (IVReceiver<double> *pResultSink, VRolePlayer *pContainer) const {
		return getValueImpl (pResultSink, pContainer);
	    }

	//  State
	private:
	    accessor_t const m_pAccessor;
	};


    /********************************************
     *----  class VClassInfoHolder::MapKey  ----*
     ********************************************/

    public:
        /**
         * A VString subclass whose comparison operators are case-insensitive.
         */
        class MapKey : public VString {
            DECLARE_FAMILY_MEMBERS (MapKey, VString);

	    //  Construction
        public:
            /** Copy constructor. */
            MapKey (VString const &rName) : BaseClass (rName) {
            }
            /** Empty default constructor. */
            MapKey () {
            }

	    //  Destruction
        public:
            /** Empty destructor. */
            ~MapKey () {
            }

	    // Update
        public:
            ThisClass &operator= (ThisClass const &rOther) {
                setTo (rOther);
                return *this;
            }
            ThisClass &operator= (VString const &rOther) {
                setTo (rOther);
                return *this;
            }
            ThisClass &operator= (char const *pOther) {
                setTo (pOther);
                return *this;
            }

	    //  Comparison
        public:
            /** Case-insensitive comparison operator. */
            bool operator<  (char const *pOther) const {
                return strcasecmp (*this, pOther) <  0;
            }
            /** Case-insensitive comparison operator. */
            bool operator<= (char const *pOther) const {
                return strcasecmp (*this, pOther) <= 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator== (char const *pOther) const {
                return strcasecmp (*this, pOther) == 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator!= (char const *pOther) const {
                return strcasecmp (*this, pOther) != 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator>= (char const *pOther) const {
                return strcasecmp (*this, pOther) >= 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator>  (char const *pOther) const {
                return strcasecmp (*this, pOther) >  0;
            }
        };


    /********************************************
     *----  class VClassInfoHolder (con't)  ----*
     ********************************************/

    //  Aliases
    public:
	typedef VkSetOf<ThisClass*,ThisClass*,ThisClass const*> class_map_t;
	typedef VkMapOf<MapKey,char const*,char const*,Property::Reference> property_map_t;

    //  Construction
    public:
	VClassInfoHolder (std::type_info const &rTypeInfo);

    //  Destruction
    public:
	~VClassInfoHolder ();

    //  Access
    public:
	template <typename sink_t> bool getPropertyValue (sink_t *pResultSink, ITicketSink *pTicketSink, VRolePlayer *pContainer, char const *pName) const {
	    Property::Reference pProperty;
	    return getProperty (pProperty, pName) && pProperty->getTrackableValue (pResultSink, pTicketSink, pContainer);
	}
	template <typename sink_t> bool getPropertyValue (sink_t *pResultSink, VRolePlayer *pContainer, char const *pName) const {
	    Property::Reference pProperty;
	    return getProperty (pProperty, pName) && pProperty->getValue (pResultSink, pContainer);
	}
	bool getProperty (Property::Reference &rpProperty, char const *pName) const;

	void getProperties (property_map_t &rResultSink) const;

	bool isComplete () const {
	    return !isntComplete ();
	}
	bool isntComplete () const {
	    return m_bIncomplete;
	}

	VString baseNames () const;
	VString propertyNames () const;
	VString summary () const;
	char const *typeName () const {
	    return m_iRTTI.name ();
	}

    //  Update
    public:
	ThisClass &addBase (ThisClass &rBase) {
	    return addBase (&rBase);
	}
	ThisClass &addBase (ThisClass *pBase);

	template <typename T> ThisClass &addProperty (VString const &rName, T iT) {
	    Property::Reference const pProperty (new Property_<T>(iT));
	    return addProperty (rName, pProperty.referent ());
	}
	ThisClass &addProperty (VString const &rName, Property *pProperty);

	void markCompleted ();

    //  State
    private:
	V::VRTTI       const   m_iRTTI;
	class_map_t    mutable m_mBases;
	property_map_t mutable m_mProperties;
	bool                   m_bIncomplete;
    };

    /*****************************************************************/
    template <typename T> class VClassInfoHolder_ : public VClassInfoHolder {
	DECLARE_FAMILY_MEMBERS (VClassInfoHolder_<T>, VClassInfoHolder);

    //  Construction
    public:
	VClassInfoHolder_() : BaseClass (typeid(T)) {
	}
    };
}


#endif

/**
 * @file
 * Defines, amongst other things, macros that are used for declaration of runtime type (RTT) members.
 * 
 * The RTT macros defined herein come in six flavors: both lite and non-lite (heavy) versions of abstract, virtual and concrete RTT macros exist. 
 * The heavy RTT macros should only be used when working with batchvision-bound classes. In all other cases, you want the lite version of a given macro.
 * 
 * For Vca class definitions, the following macros are useful:
 *   - For abstract classes, use DECLARE_ABSTRACT_RTTLITE
 *   - For concrete classes, use DECLARE_CONCRETE_RTTLITE
 */


#ifndef VReferenceable_Interface
#define VReferenceable_Interface

#ifndef V_VReferenceable
#define V_VReferenceable extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VRunTimeType.h"

#include "V_VCount.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAllocator.h"

#define ENABLE_MEMORY_MANAGER_EXCEPTIONS
#include "VConfig.h"

template <class R> class VReference;
namespace V { class VString; }


/*****************************************
 *****  Reference Monitoring Macros  *****
 *****************************************/

#if defined(_DEBUG) && !defined(_WIN32)
#define ON_DEBUG_DISCARD() onDebugDiscard ()
#define ON_DEBUG_RELEASE() onDebugRelease ()
#define ON_DEBUG_RETAIN()  onDebugRetain  ()
#define ON_DEBUG_UNTAIN()  onDebugUntain  ()

#else
#define ON_DEBUG_DISCARD()
#define ON_DEBUG_RELEASE()
#define ON_DEBUG_RETAIN()
#define ON_DEBUG_UNTAIN()

#endif


/**************************************
 *****  Run Time Type D&D Macros  *****
 **************************************/

#if defined(_WIN32)
#define ABSTRACT __declspec(novtable)
#else
#define ABSTRACT
#endif

/**
 * Used for RTT tagging a class as abstract and includes an RTT object member.
 * Should only be used when working with batchvision-bound classes. In all other cases, you really want the lite version of this macro.
 *
 * @see DECLARE_ABSTRACT_RTTLITE
 */
#define DECLARE_ABSTRACT_RTT(thisClass,baseClass)\
    DECLARE_ABSTRACT_RTTLITE(thisClass,baseClass);\
    DECLARE_RTT_MEMBERS()

/**
 * Used for RTT tagging a class as abstract.
 */
#define DECLARE_ABSTRACT_RTTLITE(thisClass,baseClass)\
    DECLARE_ABSTRACT_RTTLITE_MEMBERS(thisClass,baseClass)

/**
 * NEVER call this directly.
 * @private
 */
#define DECLARE_ABSTRACT_RTTLITE_MEMBERS(thisClass,baseClass)\
    DECLARE_FAMILY_MEMBERS(thisClass,baseClass);\
    DECLARE_ABSTRACT_RTT_PARENT_MEMBERS()\
public:\
    /** Convenience define for a VReference templated for this class. */\
    typedef VReference<ThisClass> Reference

/**
 * NEVER call this directly.
 * @private
 */
#define DECLARE_ABSTRACT_RTT_PARENT_MEMBERS()

#define DECLARE_VIRTUAL_RTT(thisClass,baseClass)\
    DECLARE_VIRTUAL_RTTLITE_MEMBERS(thisClass,baseClass,RTT);\
    DECLARE_RTT_MEMBERS()
#define DECLARE_VIRTUAL_RTTLITE(thisClass,baseClass)\
    DECLARE_VIRTUAL_RTTLITE_MEMBERS(thisClass,baseClass,BaseClass::RTT)
#define DECLARE_VIRTUAL_RTTLITE_MEMBERS(thisClass,baseClass,whichRTT)\
    DECLARE_ABSTRACT_RTTLITE_MEMBERS(thisClass,baseClass)

/**
 * Used for RTT tagging a class as concrete and includes an RTT object member.
 * Should only be used when working with batchvision-bound classes. In all other cases, you really want the lite version of this macro.
 *
 * @see DECLARE_CONCRETE_RTTLITE
 */
#define DECLARE_CONCRETE_RTT(thisClass,baseClass)\
    DECLARE_CONCRETE_RTTLITE_MEMBERS(thisClass,baseClass,RTT);\
    DECLARE_RTT_MEMBERS()

/**
 * Used for RTT tagging a class as concrete.
 */
#define DECLARE_CONCRETE_RTTLITE(thisClass,baseClass)\
    DECLARE_CONCRETE_RTTLITE_MEMBERS(thisClass,baseClass,BaseClass::RTT)
/**
 * NEVER call this directly.
 * @private
 *
 * Expands the DECLARE_CONCRETE_RTTLITE_MEMBERS_NODT macro and additionally adds a deleteThis() method definition for destruction of VReferenceable objects.
 */
#define DECLARE_CONCRETE_RTTLITE_MEMBERS(thisClass,baseClass,whichRTT)\
    DECLARE_CONCRETE_RTTLITE_MEMBERS_NODT(thisClass,baseClass,whichRTT);\
private:\
    virtual void deleteThis () OVERRIDE {\
	if (this->onDeleteThis ())\
	    delete this;\
    }

/**
 * Used for RTT tagging a class as concrete, including an RTT object member but not including a deleteThis() method definition.
 * Should only be used when working with batchvision-bound classes. In all other cases, you really want the lite version of this macro.
 *
 * @see DECLARE_CONCRETE_RTTLITE_NODT
 */
#define DECLARE_CONCRETE_RTT_NODT(thisClass,baseClass)\
    DECLARE_CONCRETE_RTTLITE_MEMBERS_NODT(thisClass,baseClass,RTT);\
    DECLARE_RTT_MEMBERS()

/**
 * Used for RTT tagging a class as concrete but not including a deleteThis() method definition.
 * This macro should NEVER be used directly unless the consumer provides their own deleteThis() implementation. Unless you've defined your own deleteThis, you probably want the non-no-delete version of this macro.
 *
 * @see DECLARE_CONCRETE_RTTLITE
 */
#define DECLARE_CONCRETE_RTTLITE_NODT(thisClass,baseClass)\
    DECLARE_CONCRETE_RTTLITE_MEMBERS_NODT(thisClass,baseClass,BaseClass::RTT)

/**
 * NEVER call this directly.
 * @private
 */
#define DECLARE_CONCRETE_RTTLITE_MEMBERS_NODT(thisClass,baseClass,whichRTT)\
    DECLARE_VIRTUAL_RTTLITE_MEMBERS(thisClass,baseClass,whichRTT)

/**
 * NEVER call this directly.
 * @private
 */
#define DECLARE_RTT_MEMBERS()\
public:\
    virtual VRunTimeType *rtt () const OVERRIDE {\
	return &RTT;\
    }\
    /** Convenience define for a V::VRunTimeType_ templated for this class. */\
    typedef V::VRunTimeType_<ThisClass,BaseClass> RunTimeTypeType;\
    friend class V::VRunTimeType_<ThisClass,BaseClass>;\
    static RunTimeTypeType RTT

/**
 * NEVER call this directly.
 * @private
 */
#define DEFINE_ABSTRACT_RTT(thisClass)  DEFINE_RTT_MEMBER(thisClass)

/**
 * NEVER call this directly.
 * @private
 */
#define DEFINE_VIRTUAL_RTT(thisClass) DEFINE_ABSTRACT_RTT(thisClass)

/**
 * NEVER call this directly.
 * @private
 */
#define DEFINE_CONCRETE_RTT(thisClass) DEFINE_VIRTUAL_RTT(thisClass)

#define DEFINE_ABSTRACT_RTTLITE(thisClass)
#define DEFINE_VIRTUAL_RTTLITE(thisClass) DEFINE_ABSTRACT_RTTLITE(thisClass)
#define DEFINE_CONCRETE_RTTLITE(thisClass) DEFINE_VIRTUAL_RTTLITE(thisClass)

#define DEFINE_RTT_MEMBER(thisClass) thisClass::RunTimeTypeType thisClass::RTT

#if defined(WIN32_COM_SERVER)
#define DECLARE_COM_WRAPPABLE(interface_) \
public:\
    void* comWrapper () const {\
	return m_iComWrapperCache.comWrapper ();\
    }\
    void setComWrapperTo (void *pComWrapper) {\
	m_iComWrapperCache.setComWrapperTo (pComWrapper);\
    }\
    void clearComWrapper () {\
	m_iComWrapperCache.clearComWrapper ();\
    }\
    HRESULT QueryInterface (interface interface_ **ppInterface);\
protected:\
    ComWrapperCache m_iComWrapperCache

#else
#define DECLARE_COM_WRAPPABLE(interface_)

#endif


/*************************
 *****  Definitions  *****
 *************************/

namespace V {

    class VThread;

    /*---------------------------------------*
     *----  class V::VReferenceableBase  ----*
     *---------------------------------------*/

    class V_API VReferenceableBase : public virtual VTransient {
	DECLARE_FAMILY_MEMBERS (VReferenceableBase, VTransient);

	friend class VThread;

    //  Aliases
    public:
	typedef Pointer ReferenceableBasePointer;

    //  ReclamationList
    public:
	class ReclamationList {
	    DECLARE_NUCLEAR_FAMILY (ReclamationList);

	//  Construction
	public:
	    ReclamationList () {
	    }

	//  Destruction
	public:
	    ~ReclamationList () {
	    }

	//  Update
	public:
	    void add (VReferenceableBase *pObject) {
		m_pListHead.push (pObject, &VReferenceableBase::m_pReclamationLink);
	    }
	    void reap () {
		ReferenceableBasePointer pObject;
		while (m_pListHead.pop (pObject, &VReferenceableBase::m_pReclamationLink))
		    pObject->die ();
	    }

	//  State
	private:
	    ReferenceableBasePointer m_pListHead;
	};
	friend class ReclamationList;

    //  Construction
    protected:
	VReferenceableBase () {
	}

    //  Destruction
    protected:
	~VReferenceableBase ();

    //  Meta Maker
    protected:
	static void MetaMaker ();

    //  Reclamation
    private:
	virtual void die_() = 0;
	void die () {
	    die_();
	}
    protected:
	void reclaimThis ();

    //  Run Time Type
    public:
	virtual VRunTimeType *rtt () const = 0;
	virtual V::VString rttName () const;

    //  Display
    public:
	virtual void displayInfo (char const *pWhat) const;
	virtual void displayInfo () const;
        virtual void getInfo (V::VString &rResult, const V::VString &rPrefix) const;

    //  State
    private:
	Pointer m_pReclamationLink;
    };


    /*-----------------------------------------------------------------------------*
     *----  template <class Counter_T> class V::VReferenceableImplementation_  ----*
     *-----------------------------------------------------------------------------*/

    template <class Counter_T> class V_API VReferenceableImplementation_ : public VReferenceableBase {
    public:
	typedef VReferenceableImplementation_<Counter_T> Referenceable;
	DECLARE_ABSTRACT_RTT (Referenceable, VReferenceableBase);

    //  Aliases
    public:
	typedef Counter_T model_counter_t;

//  RTTI
    public:
	class RTTI : public VRTTI {
	public:
	    RTTI (Referenceable *pObject) : VRTTI (typeid(*pObject)) {
	    }
	    ~RTTI () {
	    }
	};


//  Construction
    protected:
	VReferenceableImplementation_(unsigned int iReferenceCount);
	VReferenceableImplementation_();

    //  Destruction
    protected:
	~VReferenceableImplementation_();
	/**
	 *  Override this function in any subclass to gain control of the
	 *  deletion process.  If your override returns true, this object
	 *  will be deleted (i.e., 'delete this' will be executed.  To
	 *  suppress deletion, return false from your override.
	 *
	 *  This function is non-virtual by design.  There is absolutely
	 *  no value in making it virtual since 'deleteThis' is already
	 *  virtual and explicitly overridden in all concrete derived
	 *  classes.
	 *
	 *  Special Note For Overrides In Concrete Classes - Declare
	 *  concrete classes that need to explicitly override this member
	 *  using 'DECLARE_CONCRETE_RTT_NODT' to suppress the automatic
	 *  inclusion of a 'using BaseClass::onDeleteThis' statement in
	 *  your class definition.
	 */
	bool onDeleteThis () {
	    return true;
	}

//  Access
    public:
	unsigned int referenceCount () const {
	    return m_iReferenceCount;
	}

//  Reclamation
    private:
	virtual void deleteThis () = 0;
	virtual void die_() OVERRIDE;
	void die () {
	    if (m_iReferenceCount.decrementIsZero ())
		deleteThis ();
	}
	void reclaimThis () {
	    m_iReferenceCount = 1;
	    BaseClass::reclaimThis ();
	}

//  Reference Monitoring
    protected:
/*------------------------------------------------------------------------------*
 *  Override these routines in a subclass to observe references to instances of	*
 *  that subclass or modify these implementations as appropriate.               *
 *------------------------------------------------------------------------------*/
#ifdef _DEBUG
	virtual void onDebugDiscard	() {
	}
	virtual void onDebugRelease	() {
	}
	virtual void onDebugRetain	() {
	}
	virtual void onDebugUntain	() {
	}
#endif

//  Reference Maintenance
    public:
	void discard () {
	    ON_DEBUG_DISCARD ();
	    if (m_iReferenceCount.isZero ())
		reclaimThis ();
	}
	void release () {
	    ON_DEBUG_RELEASE ();
	    if (m_iReferenceCount.decrementIsZero ())
		reclaimThis ();
	}
	void retain () {
	    ON_DEBUG_RETAIN ();
	    m_iReferenceCount.increment ();
	}
	void untain () {
	    ON_DEBUG_UNTAIN ();
	    m_iReferenceCount.decrement ();
	}

//  State
    protected:
	model_counter_t	m_iReferenceCount;
    };

/********************************
 *****  Member Definitions  *****
 ********************************/

    template <class Counter_T> typename DEFINE_ABSTRACT_RTT (VReferenceableImplementation_<Counter_T>);

    template <class Counter_T> VReferenceableImplementation_<Counter_T>::VReferenceableImplementation_(unsigned int iReferenceCount) : m_iReferenceCount (iReferenceCount) {
    }
    template <class Counter_T> VReferenceableImplementation_<Counter_T>::VReferenceableImplementation_() : m_iReferenceCount (0) {
    }
    template <class Counter_T> VReferenceableImplementation_<Counter_T>::~VReferenceableImplementation_() {
    }
    template <class Counter_T> void V::VReferenceableImplementation_<Counter_T>::die_() {
	die ();
    }
}


/**************************************************
 *----  Standard Typedefs and Instantiations  ----*
 **************************************************/

/// Interlocked (thread-safe) Reference Count Base Class Implementation
typedef V::VReferenceableImplementation_<V::counter32_t> VReferenceable;

/// Non-interlocked (single-threaded) Reference Count Base ClassImplementation
typedef V::VReferenceableImplementation_<V::counter32nil_t> VReferenceableNIL;

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(V_VReferenceable_Instantiations)
V_VReferenceable template class V_API V::VReferenceableImplementation_<V::counter32_t>;
V_VReferenceable template class V_API V::VReferenceableImplementation_<V::counter32nil_t>;
#endif

/*************************************
 *****  Mandatory Redefinitions  *****
 *************************************/

#undef DECLARE_ABSTRACT_RTT_PARENT_MEMBERS
#define DECLARE_ABSTRACT_RTT_PARENT_MEMBERS()\
public:\
    using BaseClass::referenceCount;\
    using BaseClass::retain;\
    using BaseClass::untain;\
    using BaseClass::release;\
    using BaseClass::discard;

/***********************************
 *****  Courtesy Declarations  *****
 ***********************************/

#include "VReference.h"


#endif

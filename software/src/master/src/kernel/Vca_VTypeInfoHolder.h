#ifndef Vca_VTypeInfoHolder_Interface
#define Vca_VTypeInfoHolder_Interface

/************************
 *****  Components  *****
 ************************/

#include "VStaticTransient.h"

#include "V_VRTTI.h"

#include "VTypeInfo.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VBlob.h"
#include "V_VTime.h"

#include "Vca_VcaPeerCharacteristics.h"
#include "Vca_VcaRouteStatistics.h"

#include "VPeerDataArray.h"
#include "VTypeTraits.h"

namespace Vca {
    class VBSConsumer;
    class VBSProducer;
    class VInterfaceMember;

    class VcaOIDX;

    class VcaSerializer;
    class VcaSite;
}


/*************************
 *****  Definitions  *****
 *************************/

/******************************************
 *----  Vca::VTypeInfoHolderInstance  ----*
 ******************************************/

namespace Vca {
    /**
     * Base class for all VTypeInfo holders and is intended to be instantiated statically.
     */
    class Vca_API VTypeInfoHolderInstance : public VStaticTransient {
	DECLARE_FAMILY_MEMBERS (VTypeInfoHolderInstance, VStaticTransient);

    //  Friends
	friend class VInterfaceMember;

    //  SlackerTracker
    public:
	class Vca_API SlackerTracker {
	    DECLARE_NUCLEAR_FAMILY (SlackerTracker);

	//  Aliases
	public:
	    typedef bool (*snitch_t)();

	//  Construction
	public:
	    SlackerTracker (snitch_t pSnitch);

	//  Destruction
	public:
	    ~SlackerTracker ();

	//  Access
	public:
	    snitch_t snitch () const {
		return m_pSnitch;
	    }
	    ThisClass* successor () const {
		return m_pSuccessor;
	    }

	//  Snitching
	public:
	    static unsigned int ShowSlackers ();

	//  State
	private:
	    Pointer	const m_pSuccessor;
	    snitch_t	const m_pSnitch;
	};
	friend class SlackerTracker;

    //  SlackerTrackers
    private:
	static SlackerTracker* g_pSlackerTrackers;

    //  Construction
    protected:
	VTypeInfoHolderInstance ();

    //  Destruction
    protected:
	~VTypeInfoHolderInstance ();

    //  Static Transient Initialization
    protected:
	void initialize ();

    //  Member Access
    private:
	bool memberIndicesNotInitialized () const {
	    return IsNil (m_pMemberArray);
	}
	void buildMemberIndices ();
    public:
	VInterfaceMember const *interfaceMember (unsigned int xMember);
	VInterfaceMember const *interfaceMember (VString const &rMemberName);

    //  Member Registration
    protected:
	void registerMember (VInterfaceMember *pInterfaceMember);

    //  Type Info Instantiation
    protected:
	void raiseExistingTypeInfoHolderException (VTypeInfoHolderInstance *pExistingInstance);
	static void raiseMissingTypeInfoHolderException (std::type_info const &rTypeInfo, void const *pTTIHG);

    private:
	virtual VTypeInfo *typeInfoInstantiation_() = 0;
    public:
	VTypeInfo *typeInfo_() {
	    if (m_pTypeInfo.isNil ())
		initialize ();
	    return m_pTypeInfo;
	}

    //  Proxy Creation
    public:
	virtual IVUnknown *proxy (VcaOIDX *pOIDX) const;

    //  Type Description
    private:
	virtual V::VRTTI const &rtti_() const = 0;
    public:
	V::VRTTI const &rtti () const {
	    return rtti_();
	}
	char const *rttiName_() {
	    return rtti ().name ();
	}
    protected:
	void showTypeInfoDescription (std::type_info const &rTypeInfo) const;

    //  State
    protected:
	VTypeInfo::Reference	m_pTypeInfo;
	unsigned int		m_sMemberArray;
	VInterfaceMember const**m_pMemberArray;
	VInterfaceMember const *m_pMemberListHead;
    };
}


/**********************************
 *----  Vca::VTypeInfoHolder  ----*
 **********************************/

namespace Vca {
    /**
     * Provides a means to learn about a type at runtime, given said type.
     */
    template <class T> class VTypeInfoHolder : public VTypeInfoHolderInstance {
	DECLARE_FAMILY_MEMBERS (VTypeInfoHolder<T>, VTypeInfoHolderInstance);

    //  Reference Type
    public:
	typedef typename VTypeTraits<T>::ReferenceType ReferenceType;

    //  Factory Instance
    protected://private:
	static ThisClass* g_pThisTypeInfoHolder;

    //  SlackerTracker
    private:
	static BaseClass::SlackerTracker const g_iSlackerTracker;
	static bool SlackerSnitch ();

    //  Construction
    protected:
	VTypeInfoHolder ();

    //  Destruction
    protected:
	~VTypeInfoHolder ();

    //  Access
    private:
	virtual V::VRTTI const &rtti_() const OVERRIDE;
    public:
	static VTypeInfo *typeInfo ();
	static VTypeInfo *typeInfoNoException ();

    //  Description
    public:
	static void describe ();

    //  Instance Management
    public:
	virtual void construct_(ReferenceType rT) = 0;
	static  void construct (ReferenceType rT);

	virtual void preserve_(ReferenceType rT) = 0;
	static  void preserve (ReferenceType rT);

	virtual void destroy_(ReferenceType rT) = 0;
	static  void destroy (ReferenceType rT);

	virtual bool validates_(T iT) const;
	static  bool validates (T iT);

    //  Serialization
    public:
	virtual VcaSerializer *serializer_(VcaSerializer *pCaller, ReferenceType rT) const = 0;
	static  VcaSerializer *serializer (VcaSerializer *pCaller, ReferenceType rT);
    };

/********************************
 *----  Member Definitions  ----*
 ********************************/

    template <typename T> bool VTypeInfoHolder<T>::SlackerSnitch () {
	if (g_pThisTypeInfoHolder)
	    return false;
	raiseMissingTypeInfoHolderException (typeid (ThisClass),&g_pThisTypeInfoHolder);
	return true;
    }

    template <typename T> VTypeInfoHolder<T>::VTypeInfoHolder () {
	if (g_pThisTypeInfoHolder == 0)
	    g_pThisTypeInfoHolder = this;
	else {
	    raiseExistingTypeInfoHolderException (g_pThisTypeInfoHolder);
	}
    }

    template <typename T> VTypeInfoHolder<T>::~VTypeInfoHolder () {
	if (g_pThisTypeInfoHolder == this)
	    g_pThisTypeInfoHolder = 0;
    }

    template <typename T> V::VRTTI const &VTypeInfoHolder<T>::rtti_() const {
	static V::VRTTI s_iRTTI (typeid (T));
	return s_iRTTI;
    }

    template <typename T> VTypeInfo *VTypeInfoHolder<T>::typeInfo () {
	if (g_pThisTypeInfoHolder)
	    return g_pThisTypeInfoHolder->typeInfo_();

	raiseMissingTypeInfoHolderException (typeid (ThisClass),&g_pThisTypeInfoHolder);
	return 0;
    }
    template <typename T> VTypeInfo *VTypeInfoHolder<T>::typeInfoNoException () {
	return g_pThisTypeInfoHolder ? g_pThisTypeInfoHolder->typeInfo_() : 0;
    }

    template <typename T> void VTypeInfoHolder<T>::describe () {
	if (g_pThisTypeInfoHolder)
	    g_pThisTypeInfoHolder->showTypeInfoDescription (typeid (T));
    }

    template <typename T> void VTypeInfoHolder<T>::construct (ReferenceType rT) {
	if (g_pThisTypeInfoHolder)
	    g_pThisTypeInfoHolder->construct_(rT);
    }
    template <typename T> void VTypeInfoHolder<T>::destroy (ReferenceType rT) {
	if (g_pThisTypeInfoHolder)
	    g_pThisTypeInfoHolder->destroy_(rT);
    }
    template <typename T> void VTypeInfoHolder<T>::preserve (ReferenceType rT) {
	if (g_pThisTypeInfoHolder)
	    g_pThisTypeInfoHolder->preserve_(rT);
    }
    template <typename T> bool VTypeInfoHolder<T>::validates (T iT) {
	return !g_pThisTypeInfoHolder || g_pThisTypeInfoHolder->validates_(iT);
    }
    template <typename T> bool VTypeInfoHolder<T>::validates_(T iT) const {
	return true;
    }

    template <typename T> VcaSerializer *VTypeInfoHolder<T>::serializer (VcaSerializer *pCaller, ReferenceType rT) {
	if (g_pThisTypeInfoHolder)
	    return g_pThisTypeInfoHolder->serializer_(pCaller, rT);
	raiseMissingTypeInfoHolderException (typeid (ThisClass),&g_pThisTypeInfoHolder);
	return 0;
    }
}


/*********************
 *****  Members  *****
 *********************/

/**********************************
 *****  Vca::VTypeInfoHolder  *****
 **********************************/

namespace Vca {
    template <class T> VTypeInfoHolder<T>* VTypeInfoHolder<T>::g_pThisTypeInfoHolder = 0;
    template <class T> VTypeInfoHolderInstance::SlackerTracker const VTypeInfoHolder<T>::g_iSlackerTracker (
	&VTypeInfoHolder<T>::SlackerSnitch
    );
}



/****************************
 *****  Instantiations  *****
 ****************************/

#ifndef Vca_VTypeInfoHolder
#define Vca_VTypeInfoHolder extern
#endif

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vca_VTypeInfoHolder_Instantiations)

//  Instantiates type information for call-by-value (a.k.a. Plain-Old-Data-Type) types and their arrays:
#define Vca_VTypeInfoHolder_InstantiationsForPODT(t)\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t >;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<t >*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<t >*>;\
    Vca_VTypeInfoHolder_InstantiationsForArraysOf (t)

#define Vca_VTypeInfoHolder_InstantiationsForPODTX(t)\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t >;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<t >*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<Vca::IDataSource<t >*>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<Vca::IDataSource<t >*>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<t >*>;\
    Vca_VTypeInfoHolder_InstantiationsForArraysOf (t)

//  Instantiates type information for call-by-'const&' data types and their arrays:
#define Vca_VTypeInfoHolder_InstantiationsForREFT(t)\
    Vca_VTypeInfoHolder_InstantiationsForSREF(t);\
    Vca_VTypeInfoHolder_InstantiationsForSREF(VkDynamicArrayOf<t >)

#define Vca_VTypeInfoHolder_InstantiationsForREFTX(t)\
    Vca_VTypeInfoHolder_InstantiationsForSREFX(t);\
    Vca_VTypeInfoHolder_InstantiationsForSREFX(VkDynamicArrayOf<t >)

//  Instantiates type information for VReference<VReferenceable> and their arrays:
#define Vca_VTypeInfoHolder_InstantiationsForVREF(t)\
    Vca_VTypeInfoHolder_InstantiationsForREFT (VReference<t >)

//  Instantiates type information for arrays of a type:
#define Vca_VTypeInfoHolder_InstantiationsForArraysOf(t)\
    Vca_VTypeInfoHolder_InstantiationsForSREF(VkDynamicArrayOf<t >)

//  Instantiates type information for call-by-'const&' data types (but NOT their arrays):
#define Vca_VTypeInfoHolder_InstantiationsForSREF(t)\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t >;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t const&>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<t const&>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<t const&>*>

#define Vca_VTypeInfoHolder_InstantiationsForSREFX(t)\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t >;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<t const&>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<t const&>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<IVReceiver<Vca::IDataSource<t const&>*>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<Vca::IDataSource<t const&>*>*>;\
    Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<Vca::IDataSource<t const&>*>

Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<char const*>;
Vca_VTypeInfoHolder template class Vca_API Vca::VTypeInfoHolder<VReferenceable*>;

Vca_VTypeInfoHolder_InstantiationsForPODT (bool);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::F32);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::F64);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::S08);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::S16);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::S32);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::S64);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::U08);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::U16);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::U32);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::U64);

Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::VBSConsumer*);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::VBSProducer*);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::VcaSite*);
Vca_VTypeInfoHolder_InstantiationsForPODT (Vca::VInterfaceMember const*);
Vca_VTypeInfoHolder_InstantiationsForPODT (VTypeInfo*);

Vca_VTypeInfoHolder_InstantiationsForREFT (V::uuid_t);
Vca_VTypeInfoHolder_InstantiationsForREFT (V::VBlob);
Vca_VTypeInfoHolder_InstantiationsForREFTX(VString);
Vca_VTypeInfoHolder_InstantiationsForREFT (V::VTime);

Vca_VTypeInfoHolder_InstantiationsForSREF (Vca::VcaPeerCharacteristics);
Vca_VTypeInfoHolder_InstantiationsForSREF (Vca::VPeerData);
Vca_VTypeInfoHolder_InstantiationsForSREF (Vca::VPeerDataArray);
Vca_VTypeInfoHolder_InstantiationsForSREF (Vca::VcaRouteStatistics);
Vca_VTypeInfoHolder_InstantiationsForSREF (Vca::VcaSSID);

Vca_VTypeInfoHolder_InstantiationsForVREF (IVUnknown);


#endif


#endif

#ifndef VRunTimeType_Interface
#define VRunTimeType_Interface

/************************
 *****  Components  *****
 ************************/

#include "VStaticTransient.h"

#include "V_VRTTI.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /**************************
     *----  VRunTimeType  ----*
     **************************/

    class V_API VRunTimeType : public VStaticTransient {
    //  MetaMaker Type
    public:
	typedef void (*MetaMaker)();

    //  Aliases
    public:
	typedef unsigned short index_t;
	typedef unsigned short level_t;

    //  Construction
    protected:
	VRunTimeType (std::type_info const &rRTTI, MetaMaker pMeta, VRunTimeType *pSuper);

    //  Destruction
    protected:
	~VRunTimeType ();

    //  Initialization
    protected:
	void initialize ();
	void addSubtype (VRunTimeType *pRTT);

    //  Access
    protected:
	MetaMaker meta () const {
	    return m_pMeta;
	}

    public:
	index_t index () const {
	    return m_xType;
	}
	level_t level () const {
	    return m_xLevel;
	}
	char const *name () const {
	    return m_iRTTI.name ();
	}
	index_t subtypeCount () const {
	    return m_cSubtypes;
	}
	VRunTimeType *supertype () const {
	    return m_pSuper;
	}

    //  Query
    public:
	bool isA (VRunTimeType const& rRTT) const;

    //  State
    protected:
	V::VRTTI		m_iRTTI;
	MetaMaker const		m_pMeta;
	VRunTimeType *const	m_pSuper;
	index_t			m_xType;
	level_t			m_xLevel;
	index_t			m_cSubtypes;
    };


    /******************************
     *----  V::VRunTimeType_  ----*
     ******************************/
    /*------------------------------------------------------------------------------------*
     *  In MSVC6, the obvious specialization:
     *
     *	template <> class VRunTimeType_<VReferenceable,VTransient> : public VRunTimeType {
     *	//  Construction
     *	public:
     *	    VRunTimeType_();
     *	};
     *
     *	doesn't work, so the following indirect specializations are used instead:
     *
     *------------------------------------------------------------------------------------*/
    template<typename T> class VRunTimeTypeTraits_ {
    public:
	static VRunTimeType *rtt () {
	    return &T::RTT;
	}
    };

    class VReferenceableBase;

    template<> class VRunTimeTypeTraits_<VReferenceableBase> {
    public:
	static VRunTimeType *rtt () {
	    return 0;
	}
    };

    template<typename T,typename B> class VRunTimeType_ : public VRunTimeType {
    //  Construction
    public:
	VRunTimeType_() : VRunTimeType (typeid (T), &T::MetaMaker, VRunTimeTypeTraits_<B>::rtt ()) {
	}
    };
}
using V::VRunTimeType;


#endif

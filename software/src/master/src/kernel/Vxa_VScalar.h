#ifndef Vxa_VScalar_Interface
#define Vxa_VScalar_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"
#include "Vxa_VType.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    /*****************************************************
     *----  template <typename Val_T> class VScalar  ----*
     *****************************************************/

    template <typename Val_T> class VScalar : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VScalar<Val_T>, VReferenceable);

    //  Aliases
    public:
	typedef Val_T value_t;

    //  Construction
    protected:
	VScalar (VType *pType) : m_pType (pType) {
	}

    //  Destruction
    protected:
	~VScalar () {
	}

    //  Access
    public:
	virtual Val_T value () = 0;

    //  State
    private:
	VType::Pointer const m_pType;
    };

    typedef VScalar<bool>::Reference		bool_scalar_return_t;
    typedef VScalar<short>::Reference		short_scalar_return_t;
    typedef VScalar<unsigned short>::Reference	unsigned_short_scalar_return_t;
    typedef VScalar<int>::Reference		int_scalar_return_t;
    typedef VScalar<unsigned int>::Reference	unsigned_int_scalar_return_t;
    typedef VScalar<float>::Reference		float_scalar_return_t;
    typedef VScalar<double>::Reference		double_scalar_return_t;
    typedef VScalar<VString const&>::Reference	VString_scalar_return_t;

    /*****************************************************************************
     *----  template <typename Val_T, typename Var_T> class VScalarInstance  ----*
     *****************************************************************************/

    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VScalarInstance : public VScalar<Val_T> {
	typedef VScalarInstance<Val_T,Var_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VScalar<Val_T>);

    //  Aliases
    public:
	typedef Var_T variable_t;

    //  Construction
    public:
	template <typename P1,typename P2,typename P3,typename P4> VScalarInstance (
	    VType *pType, P1 iP1,P2 iP2,P3 iP3,P4 iP4
	) : BaseClass (pType), m_iValue (iP1,iP2,iP3,iP4) {
	}
	template <typename P1,typename P2,typename P3> VScalarInstance (
	    VType *pType, P1 iP1,P2 iP2,P3 iP3
	) : BaseClass (pType), m_iValue (iP1,iP2,iP3) {
	}
	template <typename P1,typename P2> VScalarInstance (
	    VType *pType, P1 iP1,P2 iP2
	) : BaseClass (pType), m_iValue (iP1,iP2) {
	}
	template <typename P1> VScalarInstance (
	    VType *pType, P1 iP1
	) : BaseClass (pType), m_iValue (iP1) {
	}

    //  Destruction
    protected:
	~VScalarInstance () {
	}

    //  Access
    public:
	virtual Val_T value () OVERRIDE {
	    return m_iValue;
	}

    //  State
    private:
	Var_T m_iValue;
    };
}


#endif

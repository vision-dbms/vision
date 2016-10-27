#ifndef Vxa_VSerializer2DynArray_Interface
#define Vxa_VSerializer2DynArray_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VcaSerializer.h"
#include "Vca_VTypeInfoHolder.h"
#include "Vca_VTypePattern.h"
#include "Vca_VTrigger.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {

    /*****************************************************************************************
     *----  template<typename Val_T,typename Var_T> class VSerializer2DynArrayOfGeneric  ----*
     *****************************************************************************************/

    template <typename Val_T, typename Var_T> class VSerializer2DynArrayOfGeneric {
    public:
	typedef VSerializer2DynArrayOfGeneric<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t,void);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;
	typedef VkDynamicArrayOf<var_t> dynarray_t;
	typedef ThisClass serializer2_t;

    //  Serializer
    public:
	class Serializer : public Vca::VcaSerializer {
	    DECLARE_CONCRETE_RTTLITE (Serializer, Vca::VcaSerializer);

	//  Aliases
	public:
	    typedef Vca::VTrigger<ThisClass> sequencer_t;

	public:
	    Serializer(
		Vca::VcaSerializer *pCaller, serializer2_t &rDatum
	    ) : BaseClass (pCaller), m_rDatum (rDatum) {
		m_pSequencer.setTo (new sequencer_t (this, &ThisClass::doData));
	    }

	//  Destruction
	protected:
	    ~Serializer() {
	    }

	//  Sequencer Actions
	protected:
	    void doData (sequencer_t *pSequencer) {
		start (this, Vca::VTypeInfoHolder<dynarray_t>::serializer (this, m_rDatum.operator dynarray_t& ()));
		clearSequencer ();
	    }

	//  State
	protected:
	    serializer2_t &m_rDatum;
	};

    //  Construction
    public:
	VSerializer2DynArrayOfGeneric (dynarray_t const &rValues) : m_iDynArray (rValues) {
	}
	VSerializer2DynArrayOfGeneric (ThisClass const &rOther) : m_iDynArray (rOther.m_iDynArray) {
	}
	VSerializer2DynArrayOfGeneric () {
	}

    //  Destruction
    public:
	~VSerializer2DynArrayOfGeneric () {
	}

    //  Access
    public:
	operator dynarray_t const& () const {
	    return m_iDynArray;
	}
	operator dynarray_t& () {
	    return m_iDynArray;
	}

    //  State
    private:
	dynarray_t m_iDynArray;
    };


    /*************************************************
     *----  class VSerializer2DynArrayOfBoolean  ----*
     *************************************************/

    class Vxa_API VSerializer2DynArrayOfBoolean {
	DECLARE_FAMILY_MEMBERS (VSerializer2DynArrayOfBoolean,void);

    //  Aliases
    public:
	typedef bool val_t;
	typedef bool var_t;
	typedef VkDynamicArrayOf<bool> dynarray_t;
	typedef ThisClass serializer2_t;

    //  Serializer
    public:
	class Serializer;

    //  Construction
    public:
	VSerializer2DynArrayOfBoolean (dynarray_t const &rValues) : m_iDynArray (rValues) {
	}
	VSerializer2DynArrayOfBoolean (ThisClass const &rOther) : m_iDynArray (rOther.m_iDynArray) {
	}
	VSerializer2DynArrayOfBoolean () {
	}

    //  Destruction
    public:
	~VSerializer2DynArrayOfBoolean () {
	}

    //  Access
    public:
	operator dynarray_t const& () const {
	    return m_iDynArray;
	}
	operator dynarray_t& () {
	    return m_iDynArray;
	}

    //  State
    private:
	dynarray_t m_iDynArray;
    };


    /*************************************************
     *----  class VSerializer2DynArrayOfInteger  ----*
     *************************************************/

    class Vxa_API VSerializer2DynArrayOfInteger {
	DECLARE_FAMILY_MEMBERS (VSerializer2DynArrayOfInteger,void);

    //  Aliases
    public:
	typedef int val_t;
	typedef int var_t;
	typedef VkDynamicArrayOf<int> dynarray_t;
	typedef ThisClass serializer2_t;

    //  Serializer
    public:
	class Serializer;

    //  Construction
    public:
	VSerializer2DynArrayOfInteger (dynarray_t const &rValues) : m_iDynArray (rValues) {
	}
	VSerializer2DynArrayOfInteger (ThisClass const &rOther) : m_iDynArray (rOther.m_iDynArray) {
	}
	VSerializer2DynArrayOfInteger () {
	}

    //  Destruction
    public:
	~VSerializer2DynArrayOfInteger () {
	}

    //  Access
    public:
	operator dynarray_t const& () const {
	    return m_iDynArray;
	}
	operator dynarray_t& () {
	    return m_iDynArray;
	}

    //  State
    private:
	dynarray_t m_iDynArray;
    };


    /*************************************
     *----  Implementation Policies  ----*
     *************************************/
 
    namespace Policy {
	namespace Serializer2DynArrayPolicy {
	//  Generic Implementation
	    template <typename Val_T, typename Var_T> struct Implementation {
		typedef VSerializer2DynArrayOfGeneric<Val_T,Var_T> Type;
	    };

	//  Boolean Implementation
	    template <> struct Implementation<bool,bool> {
		typedef VSerializer2DynArrayOfBoolean Type;
	    };

	//  Integer Implementation
	    template <> struct Implementation<int,int> {
		typedef VSerializer2DynArrayOfInteger Type;
	    };
	}

    }


    /*********************************************************************************
     *----  template<template Val_T, template Var_T> class VSerializer2DynArray  ----*
     *********************************************************************************/

    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VSerializer2DynArray : public Policy::Serializer2DynArrayPolicy::Implementation<Val_T,Var_T>::Type {
    public:
	typedef VSerializer2DynArray<Val_T,Var_T> this_t;
	typedef typename Policy::Serializer2DynArrayPolicy::Implementation<Val_T,Var_T>::Type base_t;

	DECLARE_FAMILY_MEMBERS (this_t,base_t);

    //  Aliases
    public:
	typedef typename BaseClass::dynarray_t dynarray_t;

    //  Construction
    public:
	VSerializer2DynArray (dynarray_t const &rValues) : BaseClass (rValues) {
	}
	VSerializer2DynArray (ThisClass const &rOther) : BaseClass (rOther) {
	}
	VSerializer2DynArray () {
	}

    //  Destruction
    public:
	~VSerializer2DynArray () {
	}
    };

    /*********************
     *----  Aliases  ----*
     *********************/

    typedef VSerializer2DynArray<bool> bool_s2array_t;
    typedef VSerializer2DynArray<int > i32_s2array_t;
}


/****************************
 *----  Instantiations  ----*
 ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VSerializer2DynArray)

#ifndef Vxa_VSerializer2DynArray
#define Vxa_VSerializer2DynArray extern
#endif

Vxa_VSerializer2DynArray template class Vxa_API Vca::VTypeInfoHolder<Vxa::bool_s2array_t>;
Vxa_VSerializer2DynArray template class Vxa_API Vca::VTypeInfoHolder<Vxa::i32_s2array_t>;

Vxa_VSerializer2DynArray template class Vxa_API Vca::VTypeInfoHolder<Vxa::bool_s2array_t const&>;
Vxa_VSerializer2DynArray template class Vxa_API Vca::VTypeInfoHolder<Vxa::i32_s2array_t const&>;

#endif

#endif

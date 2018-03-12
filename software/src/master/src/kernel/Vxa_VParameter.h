#ifndef Vxa_VParameter_Interface
#define Vxa_VParameter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VScalar.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCursor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {

   /********************************************************************************
    *----  template <typename scalar_return_t, typename storage_t> VParameter  ----*
    ********************************************************************************
    *
    *  Type Equivalences (Pseudo-C++):
    *      template <typename T> typename VScalar<T>::Reference :== scalar_return_t;
    *      template <typename T> VScalar<T> :== typename scalar_return_t::ReferencedClass;
    *      template <typename T> T :== typename scalar_return_t::ReferencedClass::value_t;
    *
    ***********************/
    template <
	typename scalar_return_t, typename storage_t
    > class VParameter : public scalar_return_t::ReferencedClass /* == VScalar<scalar_value_t> */{
	typedef VParameter<scalar_return_t,storage_t> this_t;
	typedef typename scalar_return_t::ReferencedClass base_t;
	DECLARE_CONCRETE_RTTLITE (this_t, base_t);

    //  Aliases
    public:
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	template <typename source_data_t> VParameter (
	    VImportableType *pType, VCursor *pCursor, source_data_t const &rData
	) : BaseClass (pType), m_iStorage (pCursor, rData) {
	}

    //  Destruction
    private:
	~VParameter () {
	}

    //  Access
    public:
	virtual value_t value () OVERRIDE {
	    return m_iStorage;
	}

    //  State
    private:
        storage_t const m_iStorage;
    };


   /*******************************************************************
    *----  template <typename storage_t> class VBooleanParameter  ----*
    *******************************************************************/

    template <typename storage_t> class VBooleanParameter : public VScalar<bool> {
	typedef VBooleanParameter<storage_t> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VScalar<bool>);

    //  Aliases
    public:
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	template <typename source_data_t> VBooleanParameter (
	    VImportableType *pType, VCursor *pCursor, source_data_t const &rData
	) : BaseClass (pType), m_iStorage (pCursor, rData) {
	}

    //  Destruction
    private:
	~VBooleanParameter () {
	}

    //  Access
    public:
	virtual bool value () OVERRIDE {
	    return m_iStorage ? true : false;
	}

    //  State
    private:
	storage_t const m_iStorage;
    };
}


#endif

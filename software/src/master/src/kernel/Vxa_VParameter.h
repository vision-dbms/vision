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
   /*************************************************************************************
    *----  template <typename scalar_return_t, typename data_storage_t> VParameter  ----*
    *************************************************************************************
    *
    *  Type Equivalences (Pseudo-C++):
    *      template <typename T> typename VScalar<T>::Reference :== scalar_return_t;
    *      template <typename T> VScalar<T> :== typename scalar_return_t::ReferencedClass;
    *      template <typename T> T :== typename scalar_return_t::ReferencedClass::value_t;
    *
    ***********************/
    template <
	typename scalar_return_t, typename data_storage_t
    > class VParameter : public scalar_return_t::ReferencedClass /* == VScalar<scalar_value_t> */{
	typedef VParameter<scalar_return_t,data_storage_t> this_t;
	typedef typename scalar_return_t::ReferencedClass base_t;
	DECLARE_CONCRETE_RTTLITE (this_t, base_t);

    //  Aliases
    public:
	typedef VCursor cursor_t;
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	VParameter (
	    VImportableType *pType, data_storage_t const &rDataStorage, cursor_t *pCursor
	) : BaseClass (pType), m_iDataStorage (rDataStorage), m_pCursor (pCursor) {
	}

    //  Destruction
    private:
	~VParameter () {
	}

    //  Access
    public:
	virtual value_t value () OVERRIDE {
	    return static_cast<value_t>(m_iDataStorage[m_pCursor->position()]);
	}

    //  State
    private:
	data_storage_t		const m_iDataStorage;
	cursor_t::Reference	const m_pCursor;
    };


   /************************************************************************
    *----  template <typename data_storage_t> class VBooleanParameter  ----*
    ************************************************************************/

    template <typename data_storage_t> class VBooleanParameter : public VScalar<bool> {
	typedef VBooleanParameter<data_storage_t> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VScalar<bool>);

    //  Aliases
    public:
	typedef VCursor cursor_t;
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	VBooleanParameter (
	    VImportableType *pType, data_storage_t const &rDataStorage, cursor_t *pCursor
	) : BaseClass (pType), m_iDataStorage (rDataStorage), m_pCursor (pCursor) {
	}

    //  Destruction
    private:
	~VBooleanParameter () {
	}

    //  Access
    public:
	virtual value_t value () OVERRIDE {
	    return m_iDataStorage[m_pCursor->position()] ? true : false;
	}

    //  State
    private:
	data_storage_t		const m_iDataStorage;
	cursor_t::Reference	const m_pCursor;
    };
}


#endif

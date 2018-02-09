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
     *----  template <typename value_t, typename storage_t> class VParameterStorage  ----*
     *************************************************************************************/

    template <typename storage_t,typename value_t> class VParameterStorage {
    //  Construction
    public:
        VParameterStorage (
            VCursor *pCursor, storage_t const &rDataStorage
        ) : m_pCursor (pCursor), m_iDataStorage (rDataStorage) {
        }

    //  Destruction
    public:
        ~VParameterStorage () {
        }

    //  Access
    public:
	value_t value () const {
	    return static_cast<value_t>(m_iDataStorage[m_pCursor->position()]);
	}

    //  State
    private:
	VCursor::Reference	const m_pCursor;
	storage_t		const m_iDataStorage;
    };

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
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	VParameter (
	    VImportableType *pType, VCursor *pCursor, data_storage_t const &rDataStorage
	) : BaseClass (pType), m_iDataStorage (pCursor, rDataStorage) {
	}

    //  Destruction
    private:
	~VParameter () {
	}

    //  Access
    public:
	virtual value_t value () OVERRIDE {
	    return static_cast<value_t>(m_iDataStorage.value ());
	}

    //  State
    private:
        VParameterStorage<data_storage_t,value_t> const m_iDataStorage;
    };


   /************************************************************************
    *----  template <typename data_storage_t> class VBooleanParameter  ----*
    ************************************************************************/

    template <typename data_storage_t> class VBooleanParameter : public VScalar<bool> {
	typedef VBooleanParameter<data_storage_t> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VScalar<bool>);

    //  Aliases
    public:
	typedef typename BaseClass::value_t value_t;

    //  Construction
    public:
	VBooleanParameter (
	    VImportableType *pType, VCursor *pCursor, data_storage_t const &rDataStorage
	) : BaseClass (pType), m_iDataStorage (pCursor, rDataStorage) {
	}

    //  Destruction
    private:
	~VBooleanParameter () {
	}

    //  Access
    public:
	virtual bool value () OVERRIDE {
	    return m_iDataStorage.value () ? true : false;
	}

    //  State
    private:
	VParameterStorage<data_storage_t,value_t> const m_iDataStorage;
    };
}


#endif

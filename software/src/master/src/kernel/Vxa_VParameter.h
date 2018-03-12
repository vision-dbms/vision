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

/************************************************************************************
 *----  template <typename storage_t,typename scalar_value_t> class VParameter  ----*
 ************************************************************************************
 *
 *  Type Equivalences (Pseudo-C++):
 *     template <typename scalar_value_t>
 *        typename VScalar<scalar_value_t>::Reference <==> scalar_return_t;
 *        VScalar<scalar_value_t> <==> typename scalar_return_t::ReferencedClass;
 *        scalar_value_t <==> typename scalar_return_t::ReferencedClass::value_t;
 *
 ***********************/
    template <
	typename storage_t, typename scalar_value_t
    > class VParameter : public VScalar<scalar_value_t> {
	typedef VParameter<storage_t,scalar_value_t> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VScalar<scalar_value_t>);

    //  Aliases
    public:
	typedef scalar_value_t value_t;

    //  Construction
    public:
	template <typename source_data_t> VParameter (
	    VImportableType *pType, VCursor *pCursor, source_data_t const &rData
	) : BaseClass (pType), m_iStorage (pCursor, rData) {
	}
        template <typename source_data_t> VParameter (
            VImportableType *pType, source_data_t const &rData
        ) : BaseClass (pType), m_iStorage (rData) {
        }

    //  Destruction
    protected:
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
}


#endif

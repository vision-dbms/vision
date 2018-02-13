#ifndef Vxa_PPack_Interface
#define Vxa_PPack_Interface

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

#include "Vxa_VScalar.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VTask;

/************************
 *----  Vxa::VPack  ----*
 ************************/

    template <typename T> class VPack {
    //  Aliases
    public:
        typedef VPack<T> this_t;
        typedef this_t const &value_t;
        typedef typename VScalar<value_t>::Reference return_t;

    //  Construction
    public:
        VPack () {
        }

    //  Destruction
    public:
        ~VPack () {
        }

    //  Access
    public:
        virtual cardinality_t parameterCount () const = 0;
        virtual T parameterValue (cardinality_t xParameter) const = 0;

    //  State
    private:
    };
} //  namespace Vxa


#endif

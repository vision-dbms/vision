#ifndef Vca_Namespace_VTraits_Interface
#define Vca_Namespace_VTraits_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Namespace {
	/**
	 * Traits describing any namespace indexed by strings and implemented by an interface overload of Vca::Gofer::Supply.
	 */
	template <class Namespace_T> struct VTraits {
	    typedef Namespace_T interface_t;
	    typedef VString const &name_t;
	};
    } // namespace Namespace
} // namespace Vca


#endif

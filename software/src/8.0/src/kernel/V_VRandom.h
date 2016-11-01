#ifndef V_VRandom_Interface
#define V_VRandom_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
/** (Pseudo)Random number generation tools.
 */
    namespace VRandom {
    //  Aliases
	typedef double fraction_t;
	typedef unsigned int number_t;

    //  Generators
    /**
     * Generator for a floating point number uniformly distributed between 0.0 and 1.0 such that 0.0 <= random_result < 1.0.
     *
     *  @return a floating point number uniformly distributed between 0.0 and 1.0 such that 0.0 <= random_result < 1.0.
     */
	V_API fraction_t Fraction ();

    /**
     * Generator for a random unsigned integer.
     *
     *  @return an unsigned integer uniformly distributed betwen 0 and RAND_MAX such that 0 <= random_result <= RAND_MAX.
     */
	V_API number_t Number ();

    /**
     * Generator for a bounded random value.
     *
     *  @tparam value_t the deduced argument type.  Must support operator* (fraction_t).
     *
     *  @param xBound a bound for generated value.
     *
     *  @return a uniformly distributed random value selected from the range [0...xBound) or 0 if xBound is zero.
     */
	template <typename value_t> value_t BoundedValue (value_t xBound) {
	    return static_cast<value_t>(xBound * Fraction ());
	}

    /**
     * Generator for a bounded random value
     *
     *  @tparam value_t the deduced argument type.  Must support operator+, operator-, operator* (fraction_t).
     *
     *  @param xBound1 a bound for generated value.
     *  @param xBound2 the other bound for the generated value.
     *
     *  @return a uniformly distributed random value selected from the range [min(xBound1,xBound2)...max(xBound1,xBound2)) or xBound1 if xBound1 == xBound2.
     */
	template <typename value_t> value_t BoundedValue (value_t xBound1, value_t xBound2) {
	    return static_cast<value_t>(
		xBound2 > xBound1 ? (xBound2 - xBound1) * Fraction () + xBound1 : (xBound1 - xBound2) * Fraction () + xBound2
	    );
	}

    }  // namespace VRandom
}  //  namespace V


/*************************************
 *****  Template Instantiations  *****
 *************************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(V_VRandom_Instantiations)
namespace V {
    template V_API U32 VRandom::BoundedValue (U32,U32);
    template V_API U32 VRandom::BoundedValue (U32);

    template V_API U64 VRandom::BoundedValue (U64,U64);
    template V_API U64 VRandom::BoundedValue (U64);
}  // namespace V
#endif

#endif

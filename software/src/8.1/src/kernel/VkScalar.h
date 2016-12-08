#ifndef VkScalar_Interface
#define VkScalar_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkLongUnsigneds.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/***************************
 *****  Pointer Types  *****
 ***************************/
/*---------------------------------------------------------------------------
 * Pointer Types:
 *	Empty			- the nil pointer - no positional relationship
 *				  is defined or implied by this pointer type.
 *	Scalar			- an optimization of 'Link/Reference' and
 *				  'Values' for singletons.  Scalars of R-Type
 *				  'RefUV' behave like a 'Link/Reference' of
 *				  one element.  Scalars of all other R-Types
 *				  behave like one element 'Value's.  Scalars
 *				  are link equivalent.
 *	Value			- an ordered collection of information bearing
 *				  values.  Values typically represent ordered
 *				  collections of integers, reals, dates, and
 *				  other infinite types.  All elements of a
 *				  'Value' translate into reference 'nil'
 *				  when interpreted as references; consequently,
 *				  'Value's are link equivalent.
 *	Identity		- a collection in one-to-one correspondence to
 *				  another collection.  'Identity's are link
 *				  equivalent.
 *	Link			- an sorted collection of positions.  Links are
 *				  link equivalent (surprise!).
 *	Reference		- an ordered collection of positions.
 *				  References are NOT link equivalent.
 *---------------------------------------------------------------------------
 */

enum DSC_PointerType {
    DSC_PointerType_Empty,
    DSC_PointerType_Scalar,
    DSC_PointerType_Value,
    DSC_PointerType_Identity,
    DSC_PointerType_Link,
    DSC_PointerType_Reference
};

union DSC_ScalarValue {
    char			asChar;
    double			asDouble;
    float			asFloat;
    int				asInt;
    VkUnsigned64		asUnsigned64;
    VkUnsigned96		asUnsigned96;
    VkUnsigned128		asUnsigned128;
};

#define DSC_ScalarValue_Char(sv)	((sv).asChar)
#define DSC_ScalarValue_Double(sv)	((sv).asDouble)
#define DSC_ScalarValue_Float(sv)	((sv).asFloat)
#define DSC_ScalarValue_Int(sv)		((sv).asInt)
#define DSC_ScalarValue_Unsigned64(sv)	((sv).asUnsigned64)
#define DSC_ScalarValue_Unsigned96(sv)	((sv).asUnsigned96)
#define DSC_ScalarValue_Unsigned128(sv)	((sv).asUnsigned128)


#endif

#ifndef _H_V_VFAMILYVALUES
#define _H_V_VFAMILYVALUES

/****************************
 *****  Family Members  *****
 ****************************/

namespace V {
    template<class T> class VAggregatePointer;
}

/**
 * Defines ThisClass, BaseClass and Pointer typedefs.
 */
#define DECLARE_FAMILY_MEMBERS(thisClass,baseClass)\
DECLARE_NUCLEAR_FAMILY(thisClass);\
public:\
    /** Convenience definition for the type of this class' (primary) base class. */\
    typedef baseClass BaseClass

/**
 * Defines ThisClass and Pointer typedefs.
 */
#define DECLARE_NUCLEAR_FAMILY(thisClass)\
public:\
    /** Convenience definition for the type of this class. */\
    typedef thisClass ThisClass;\
    /** Convenience definition for a V::VAggregatePointer templated for this class. */\
    typedef V::VAggregatePointer<ThisClass> Pointer

#endif /* _H_V_VFAMILYVALUES */


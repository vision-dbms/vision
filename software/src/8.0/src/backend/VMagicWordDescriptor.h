#ifndef VMagicWordDescriptor_Interface
#define VMagicWordDescriptor_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCodeDescriptor.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/*****************************
 *****************************
 *****  The Magic Words  *****
 *****************************
 *****************************/
/*---------------------------------------------------------------------------
 * The following enumerated type defines the set of magic words known to the
 * virtual machine.  Because magic word indices are compiled into programs,
 * new magic words must only be added IMMEDIATELY before the list end marker,
 * 'MagicWord_MaximumMagic'.  Associated with each magic word is the
 * language's name for the magic word. Magic words added here must also be
 * added to the initialization list for the 'Descriptions' variable in
 * 'VMagicWordDescriptor.c[pp]'.
 *---------------------------------------------------------------------------
 */
enum VMagicWord {
    VMagicWord_Current,	/*  ^current	*/
    VMagicWord_Self,	/*  ^self	*/
    VMagicWord_Here,	/*  ^here	*/
    VMagicWord_Super,	/*  ^super	*/
    VMagicWord_Global,	/*  ^global	*/
    VMagicWord_My,	/*  ^my		*/
    VMagicWord_Date,	/*  ^date	*/
    VMagicWord_Today,	/*  ^today	*/
    VMagicWord_Tmp,	/*  ^tmp	*/
    VMagicWord_Local,	/*  ^local	*/
    VMagicWord_Ground,	/*  ^ground	*/

    VMagicWord_Datum,
    VMagicWord_Unspecified,

    VMagicWord_MaximumMagic = VMagicWord_Datum
};


class VMagicWordDescriptor : public VCodeDescriptor {
//  Construction
public:
    VMagicWordDescriptor (VMagicWord xCode, char const* pName);

//  Query
public:
    VMagicWord magicWord () const {
	return (VMagicWord)m_xCode;
    }

//  State
protected:

//  Globals
public:
    static inline VMagicWordDescriptor const *Description (
	unsigned int xCode, VMagicWordDescriptor const *pNotFoundResult
    );
    static inline VMagicWordDescriptor const *Description (
	char const* pName, VMagicWordDescriptor const *pNotFoundResult
    );

    static inline VMagicWordDescriptor const *Description (VMagicWord xCode);
    static inline VMagicWordDescriptor const *Description (char const* pName);

    static VMagicWordDescriptor const DefaultDescription;
};


/****************************
 ****************************
 *****  The Code Table  *****
 ****************************
 ****************************/

typedef VCodeTable<
    VMagicWord,
    VMagicWordDescriptor,
    (unsigned int)VMagicWord_MaximumMagic
> VMagicWordDescriptorCodeTable;

extern VMagicWordDescriptorCodeTable const VMagicWordDescriptor__CodeTable;


/******************************
 ******************************
 *****  InLine Functions  *****
 ******************************
 ******************************/

VMagicWordDescriptor const *VMagicWordDescriptor::Description (
    unsigned int xCode, VMagicWordDescriptor const *pNotFoundResult
) {
    return VMagicWordDescriptor__CodeTable.entry (xCode, pNotFoundResult);
}

VMagicWordDescriptor const *VMagicWordDescriptor::Description (
    char const* pName, VMagicWordDescriptor const *pNotFoundResult
) {
    return VMagicWordDescriptor__CodeTable.entry (pName, pNotFoundResult);
}

VMagicWordDescriptor const *VMagicWordDescriptor::Description (VMagicWord xCode) {
    return Description (xCode, &DefaultDescription);
}

VMagicWordDescriptor const *VMagicWordDescriptor::Description (char const* pName) {
    return Description (pName, &DefaultDescription);
}


#endif

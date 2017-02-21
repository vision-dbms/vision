/*****  Descriptor Representation Type Shared Declarations  *****/
#ifndef rtDSC_D
#define rtDSC_D

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

#include "VkScalar.h"


/**************************************
 *****  Packed Descriptor Format  *****
 **************************************/
/*---------------------------------------------------------------------------
 * Packed Descriptor Field Descriptions:
 *	pointerType		- the pointer type of this descriptor.
 *	rType			- the pointer r-type of this descriptor.
 *	store			- a POP for the store associated with this
 *				  descriptor.
 *	pointer			- a POP for whatever container needs to be
 *				  associated with this descriptor.  For
 *				  scalars and identities, this the reference
 *				  P-Token of the pointer; for values, links,
 *				  and references, this is the u-vector or
 *				  link containing the elements of the pointer.
 *	scalarValue		- the value of a scalar pointer.
 *---------------------------------------------------------------------------
 */
struct rtDSC_Descriptor {
    DSC_PointerType		pointerType;
    RTYPE_Type			rType;
    M_POP			store;
    M_POP			pointer;
    DSC_ScalarValue		scalarValue;
};

/*****  Access Macros  *****/
#define rtDSC_Descriptor_PointerType(dsc)	((dsc)->pointerType)
#define rtDSC_Descriptor_RType(dsc)		((dsc)->rType)
#define rtDSC_Descriptor_Store(dsc)		((dsc)->store)
#define rtDSC_Descriptor_Pointer(dsc)		((dsc)->pointer)
#define rtDSC_Descriptor_ScalarValue(dsc)	((dsc)->scalarValue)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTdsc.d 1 replace /users/mjc/system
  861002 17:59:37 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  RTdsc.d 2 replace /users/mjc/translation
  870524 09:32:37 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

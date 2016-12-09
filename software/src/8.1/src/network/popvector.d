/*****  POP-Vector Shared Definitions  *****/
#ifndef POPVECTOR_D
#define POPVECTOR_D

/*****  Memory Manager Shared Definitions  *****/
#include "m.d"

/**********************************
 *****  POP-Vector Structure  *****
 **********************************/
/*---------------------------------------------------------------------------
 * POP-Vector Field Descriptions:
 *	elementCount		- the number of pops in this POP-Vector.
 *	isInconsistent		- a flag set during operations when the
 *				  popvector may be in an inconsistent state.
 *				  Turned off when the operation is complete.
 *	array			- the array of pops contained in this
 *				  POP-Vector.  This field is implicitly present
 *				  and is not declared directly.
 *---------------------------------------------------------------------------
 */

struct POPVECTOR_PVType {
    unsigned int
	isInconsistent : 1, 
	elementCount   :31;
};

/*****  Access Macros  *****/
#define POPVECTOR_PV_ElementCount(p)\
    (p)->elementCount

#define POPVECTOR_PV_IsInconsistent(p)\
    (p)->isInconsistent

#define POPVECTOR_PV_Array(p)\
    ((M_POP*)((POPVECTOR_PVType *)(p) + 1))

/*****  Standard POP-Vector Sizing Macro  *****/
#define POPVECTOR_SizeofPVector(nelements)\
    (sizeof (POPVECTOR_PVType) + (nelements * (sizeof (M_POP))))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  popvector.d 1 replace /users/jck/system
  860526 23:25:54 jck Introducing popvectors as a support type 
for the subset of rtypes that is represented as a sequence of POP's

 ************************************************************************/
/************************************************************************
  popvector.d 2 replace /users/jck/system
  860605 16:25:42 jck Release in support of RTvstore

 ************************************************************************/
/************************************************************************
  popvector.d 3 replace /users/mjc/translation
  870524 09:40:25 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  popvector.d 4 replace /users/jck/system
  890222 15:28:00 jck Implemented corruption detection mechanism

 ************************************************************************/

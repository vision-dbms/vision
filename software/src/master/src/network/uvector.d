/*****  U-Vector Shared Definitions  *****/
#ifndef UV_D
#define UV_D

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

#include "m.d"

/*****************************************
 *****  U-Vector Preamble Structure  *****
 *****************************************/
/*---------------------------------------------------------------------------
 * Field Descriptions:
 *	pToken			- the POP for a positional generation token
 *				  for this U-Vector.  Because of the way
 *				  standard U-Vector CPD's are managed, this
 *				  field MUST be the first field in the
 *				  preamble.
 *	refPToken		- the POP for the positional state referenced
 *				  by this U-Vector.  Conceptually, the values
 *				  of all u-vectors are relative to a positional
 *				  state somewhere.  This slot holds the value
 *				  of that P-Token.
 *	auxiliaryPOP		- a POP available for specific u-vector use
 *				  and interpretation.
 *	elementCount		- the number of elements in this U-Vector.
 *	granularity		- the number of bytes in an element of this
 *				  U-Vector.
 *	isInconsistent		- a boolean which when true signifies that
 *				  the U-Vector is not consistent with its
 *				  P-Token.  This is FATAL!
 *---------------------------------------------------------------------------
 */

struct UV_PreambleType {
    M_POP			pToken,
				refPToken,
				auxiliaryPOP;
    unsigned int		elementCount;
    unsigned short		granularity;

#if defined(_AIX)
    unsigned int 
#else
    unsigned short
#endif
				isInconsistent	: 1,
				isASetUV	: 1,
				padding		: 14;
};

/*****  Access Macros  *****/
#define UV_Preamble_D_PToken(preamble)		((preamble).pToken)
#define UV_Preamble_D_RefPT(preamble)		((preamble).refPToken)
#define UV_Preamble_D_AuxillaryPOP(preamble)	((preamble).auxiliaryPOP)
#define UV_Preamble_D_ElementCount(preamble)	((preamble).elementCount)
#define UV_Preamble_D_Granularity(preamble)	((preamble).granularity)
#define UV_Preamble_D_IsInconsistent(preamble)	((preamble).isInconsistent)
#define UV_Preamble_D_IsASetUV(preamble)	((preamble).isASetUV)


/********************************
 *****  U-Vector Structure  *****
 ********************************/
/*---------------------------------------------------------------------------
 * U-Vector Field Descriptions:
 *	preamble		- the descriptive preamble of this U-Vector.
 *				  The elements of the U-Vector immediately
 *				  follow this preamble.
 *	array			- the array of elements contained in this
 *				  U-Vector.  This field is implicitly present
 *				  and is not declared directly.  The array is
 *				  always allocated with space for ONE EXTRA
 *				  element to enable assignment operations to
 *				  ignore 'nil' references which, by definition,
 *				  address the last element of the u-vector.
 *				  This element is NOT included in the element
 *				  count for the u-vector.
 *---------------------------------------------------------------------------
 */

struct UV_UVType {
    UV_PreambleType		preamble;
};

/*****  Access Macros  *****/
#define UV_UV_Preamble(p)	((p)->preamble)

#define UV_UV_PToken(p) UV_Preamble_D_PToken (\
    UV_UV_Preamble (p)\
)

#define UV_UV_RefPToken(p) UV_Preamble_D_RefPT (\
    UV_UV_Preamble (p)\
)

#define UV_UV_AuxillaryPOP(p) UV_Preamble_D_AuxillaryPOP (\
    UV_UV_Preamble (p)\
)

#define UV_UV_ElementCount(p) UV_Preamble_D_ElementCount (\
    UV_UV_Preamble (p)\
)

#define UV_UV_Granularity(p) UV_Preamble_D_Granularity (\
    UV_UV_Preamble (p)\
)

#define UV_UV_IsInconsistent(p) UV_Preamble_D_IsInconsistent (\
    UV_UV_Preamble (p)\
)

#define UV_UV_IsASetUV(p) UV_Preamble_D_IsASetUV (\
    UV_UV_Preamble (p)\
)

#define UV_UV_Array(p) (\
    (pointer_t)(&UV_UV_Preamble (p) + 1)\
)

#define UV_UV_ArrayAsType(p, type) (\
    (type *)UV_UV_Array (p)\
)

/*****  Standard U-Vector Sizing Macro  *****/
#define UV_SizeofUVector(nelements, granularity) (\
    sizeof (UV_PreambleType) + ((nelements) + 1) * (granularity)\
)


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define UV_HeaderSize sizeof (UV_PreambleType)

#define UV_Size(dataSize)\
    M_SizeOfPreambleType + M_SizeOfEndMarker + UV_HeaderSize + (dataSize)

#define UV_Content(preamblePtr)\
    ((UV_UVType *)((M_CPreamble*)(preamblePtr) + 1))

#define UV_SetElementCount(preamblePtr, count)\
    UV_UV_ElementCount (UV_Content (preamblePtr)) = (count)

#define UV_ElementCount(preamblePtr)\
    UV_UV_ElementCount (UV_Content (preamblePtr))

#define UV_DataPtr(preamblePtr)\
    UV_UV_Array (UV_Content (preamblePtr))

#define UV_DataSize(preamblePtr, size)\
    (size) * UV_UV_Granularity (UV_Content (preamblePtr))

#define UV_SetPToken(preamblePtr, ptokenPOP)\
    (M_POP_D_AsInt\
        (UV_UV_PToken\
	    (UV_Content (preamblePtr))) = M_POP_D_AsInt (ptokenPOP))

#define UV_SetRefPToken(preamblePtr, ptokenPOP)\
    (M_POP_D_AsInt\
        (UV_UV_RefPToken\
	    (UV_Content (preamblePtr))) = ptokenPOP)

#define UV_SetContainerSize(preamblePtr)\
    M_SetContainerSize\
	(preamblePtr,\
	 UV_SizeofUVector\
	    (UV_UV_ElementCount (UV_Content (preamblePtr)),\
	     UV_UV_Granularity (UV_Content (preamblePtr))))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  uvector.d 1 replace /users/mjc/system
  860323 17:20:59 mjc New U-Vector facility

 ************************************************************************/
/************************************************************************
  uvector.d 2 replace /users/mjc/system
  860410 19:20:09 mjc Added P-Token support

 ************************************************************************/
/************************************************************************
  uvector.d 3 replace /users/mjc/system
  860414 12:43:14 mjc Release version of 'uvector' consistent with RT{int,float,double}uv.c

 ************************************************************************/
/************************************************************************
  uvector.d 4 replace /users/mjc/system
  860420 18:49:07 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  uvector.d 5 replace /users/mjc/system
  860422 22:57:06 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  uvector.d 6 replace /users/mjc/system
  860504 18:46:10 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  uvector.d 7 replace /users/jck/system
  860519 01:16:38 jck Fixed spelling of auxiliaryPOP message

 ************************************************************************/
/************************************************************************
  uvector.d 8 replace /users/mjc/system
  860523 00:57:43 mjc Added 'reference P-Token' field, fixed print CPD initialization bug

 ************************************************************************/
/************************************************************************
  uvector.d 9 replace /users/mjc/system
  860530 17:49:50 mjc Added method definition macros for 'atref:' and 'atref:put:'

 ************************************************************************/
/************************************************************************
  uvector.d 10 replace /users/jad/system
  860723 16:13:21 jad added Copy routines

 ************************************************************************/
/************************************************************************
  uvector.d 11 replace /users/jck/system
  860723 16:41:29 jck Added Utility Macros for data base updates

 ************************************************************************/
/************************************************************************
  uvector.d 12 replace /users/jad/system
  860803 18:45:32 jad added a container size define for the
data base update

 ************************************************************************/
/************************************************************************
  uvector.d 13 replace /users/jad/system
  860805 16:59:25 jad Change to doc only- to make it very clear that
there must be an extra element in uvectors

 ************************************************************************/
/************************************************************************
  uvector.d 14 replace /users/mjc/system
  861002 18:37:46 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  uvector.d 15 replace /users/mjc/system
  861016 18:32:26 jad changed the uvector type conversion routines to accept the new
ref ptoken as an argument

 ************************************************************************/
/************************************************************************
  uvector.d 16 replace /users/jad/system
  861216 17:38:57 jad added set uvectors

 ************************************************************************/
/************************************************************************
  uvector.d 17 replace /users/jck/updates
  870129 13:58:48 jck Macros added for the incorporator

 ************************************************************************/
/************************************************************************
  uvector.d 18 replace /users/jck/updates
  870219 07:47:02 jck Added a macro for the incorporator

 ************************************************************************/
/************************************************************************
  uvector.d 19 replace /users/mjc/translation
  870524 09:42:45 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  uvector.d 20 replace /users/jck/system
  880120 20:12:01 jck Removed unwind traps from debugger methods

 ************************************************************************/
/************************************************************************
  uvector.d 21 replace /users/m2/backend
  890828 17:16:25 m2 Added padding to make sure preamble ends on int boundary

 ************************************************************************/

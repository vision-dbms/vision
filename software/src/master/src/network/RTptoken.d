/*****  Positional Generation Token Shared Declarations  *****/
#ifndef rtPTOKEN_D
#define rtPTOKEN_D

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

#include "m.d"

/***************************
 *****  P-Token Types  *****
 ***************************/
/*---------------------------------------------------------------------------
 * The following symbolic constants define the different P-Token types.
 * Briefly, those types are:
 *	Base			- the type of the P-Token associated with a
 *				  newly created INDEPENDENT positionally
 *				  accessible object.
 *	Shift			- the type of a P-Token describing a collection
 *				  of positional shifts.
 *---------------------------------------------------------------------------
 */

#define rtPTOKEN_TT_Base	0
#define rtPTOKEN_TT_Shift	1
#define rtPTOKEN_TT_Cartesian	2


/*************************************
 *****  Shift Descriptor Format  *****
 *************************************/
/*---------------------------------------------------------------------------
 * A shift descriptor defines a collection of tail shifts of a positionally
 * accessible object.  Because they describe tail shifts, a shift descriptor
 * is used to describe insertions and deletions.  The 'origin' and 'shift'
 * values are in units of vector elements.
 *
 * Shift Descriptor Field Descriptions:
 *	origin			- the origin of the tail being shifted.
 *	shift			- the number of positions the tail is to be
 *				  shifted.  Negative values shift the tail
 *				  left and correspond to deletions; positive
 *				  values shift the tail right and correspond
 *				  to insertion.
 *---------------------------------------------------------------------------
 */
struct rtPTOKEN_SDType {
    int
	origin,
	shift;
};

/*****  Access Macros  *****/
#define rtPTOKEN_SD_D_Origin(p)		((p).origin)
#define rtPTOKEN_SD_D_Shift(p)		((p).shift)
#define rtPTOKEN_SD_Origin(p)		((p)->origin)
#define rtPTOKEN_SD_Shift(p)		((p)->shift)


/****************************************
 ***** Cartesian Descriptor Format  *****
 ****************************************/
/*---------------------------------------------------------------------------
 * A cartesian descriptor contains the POP's for the row and column ptokens
 * which define its positional state.
 *---------------------------------------------------------------------------
 */
struct rtPTOKEN_CartesianType {
    M_POP rowPToken;
    M_POP colPToken;
};

/*****  Access Macros  *****/
#define rtPTOKEN_CT_D_RowPToken(p)	((p).rowPToken)
#define rtPTOKEN_CT_D_ColPToken(p)	((p).colPToken)
#define rtPTOKEN_CT_RowPToken(p)	((p)->rowPToken)
#define rtPTOKEN_CT_ColPToken(p)	((p)->colPToken)


/************************************************
 *****  Positional Generation Token Format  *****
 ************************************************/
/*---------------------------------------------------------------------------
 * Positional generation tokens provide two services to vectors and u-vectors.
 * The existence of a positional generation token uniquely defines a particular
 * version of the positional state for an associated vector or u-vector.  The
 * content of a positional generation token provides the specific alterations
 * which produced this version of the vector or u-vector from the last.
 *
 * A positional generation token contains three pieces of information.  A
 * counted alteration descriptor array describes the changes made to arrive at
 * this generation of the vector or u-vector associated with this generation
 * token, a pointer to another positional generation token identifies the next
 * positional generation of the vector or u-vector, and a 'baseElementCount'
 * describes the length of the positional structure described by this P-Token.
 *
 * Field Descriptions:
 *	nextGeneration		- the POP for the generation token associated
 *				  with the next generation of the vector or
 *				  u-vector.  This POP is always 'Nil' for the
 *				  generation token pointed to by the vector or
 *				  u-vector.  This field must be first.
 *	tokenType		- the type of this P-Token.  Values of this
 *				  field are chosen from the 'rtPTOKEN_TT_...'
 *				  symbolic constants described elsewhere in
 *				  this file.
 *	closed			- a boolean flag which when true signals that
 *				  this P-Token is not to be modified further.
 *	count			- the number of alteration descriptors in this
 *				  generation token.
 *	baseElementCount	- the number of elements in the positional
 *				  structure described by this P-Token.
 *	changes			- the array of change descriptors associated
 *				  with this P-Token.  This field does not exist
 *				  specifically since its presence and structure
 *				  are 'tokenType' specific.
 *---------------------------------------------------------------------------
 */

struct rtPTOKEN_Type {
    M_POP			nextGeneration;
    unsigned int		independent	:  1,
				tokenType	:  6,
				closed		:  1,
				count		: 24,
				baseElementCount;
};


/*****  Sizing Macros  *****/
#define rtPTOKEN_PT_SizeofBasePToken (sizeof (rtPTOKEN_Type))

#define rtPTOKEN_PT_SizeofShiftPToken(count) (\
    sizeof (rtPTOKEN_Type) + ((count) * sizeof (rtPTOKEN_SDType))\
)

#define rtPTOKEN_PT_SizeofCartesianPT (\
    sizeof (rtPTOKEN_Type) + sizeof (rtPTOKEN_CartesianType)\
)

/*****  Access Macros  *****/
#define rtPTOKEN_PT_nextGeneration(p)	((p)->nextGeneration)
#define rtPTOKEN_PT_TokenType(p)	((p)->tokenType)
#define rtPTOKEN_PT_Independent(p)	((p)->independent)
#define rtPTOKEN_PT_Closed(p)		((p)->closed)
#define rtPTOKEN_PT_Count(p)		((p)->count)
#define rtPTOKEN_PT_BaseElementCount(p)	((p)->baseElementCount)

#define rtPTOKEN_PT_ChangeArray(p,changeDescriptorType) ((changeDescriptorType *)((p) + 1))

#define rtPTOKEN_PT_CartesianPTokens(p) ((rtPTOKEN_CartesianType *)((p) + 1))

#define rtPTOKEN_PT_RowPToken(p) rtPTOKEN_CT_RowPToken (rtPTOKEN_PT_CartesianPTokens (p))

#define rtPTOKEN_PT_ColPToken(p) rtPTOKEN_CT_ColPToken (rtPTOKEN_PT_CartesianPTokens (p))


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define rtPTOKEN_Content(preamblePtr) (\
    (rtPTOKEN_Type *)((M_CPreamble*)(preamblePtr) + 1)\
)

#define rtPTOKEN_ElementCount(ptoken) (\
    rtPTOKEN_PT_BaseElementCount (rtPTOKEN_Content (ptoken))\
)

#define rtPTOKEN_SetElementCount(ptoken, count) (\
    rtPTOKEN_PT_BaseElementCount (rtPTOKEN_Content (ptoken)) = (count)\
)

#define rtPTOKEN_SetAltCount(ptoken, count) (\
    rtPTOKEN_PT_Count (rtPTOKEN_Content (ptoken)) = (count)\
)

#define rtPTOKEN_SetContainerSize(preamblePtr) M_SetContainerSize (\
    preamblePtr,\
    rtPTOKEN_PT_SizeofShiftPToken (rtPTOKEN_PT_Count (rtPTOKEN_Content (preamblePtr)))\
)

#define rtPTOKEN_Size(altCount) (\
    rtPTOKEN_ContainerSize + altCount * sizeof (rtPTOKEN_SDType)\
)

#define rtPTOKEN_ContainerSize (\
    M_SizeOfPreambleType + M_SizeOfEndMarker + rtPTOKEN_PT_SizeofBasePToken\
)

#define rtPTOKEN_HeaderSize (\
    M_SizeOfPreambleType + sizeof (rtPTOKEN_Type)\
)

#define rtPTOKEN_IsntTerminal(preamblePtr) (\
    rtPTOKEN_PT_TokenType (rtPTOKEN_Content (preamblePtr)) != rtPTOKEN_TT_Base\
)

#define rtPTOKEN_ConvertToShift(ptoken, nextGeneration)\
{\
    rtPTOKEN_Type *ptokenPtr = rtPTOKEN_Content (ptoken);\
    M_POP_D_AsInt (rtPTOKEN_PT_nextGeneration (ptokenPtr)) = M_POP_D_AsInt (nextGeneration);\
    rtPTOKEN_PT_TokenType (ptokenPtr) = rtPTOKEN_TT_Shift;\
}


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTptoken.d 1 replace /users/mjc/system
  860404 20:14:06 mjc Create Positional Generation Token Representation Type

 ************************************************************************/
/************************************************************************
  RTptoken.d 1 replace /users/mjc/system
  860409 08:00:26 mjc Create positional generation token type

 ************************************************************************/
/************************************************************************
  RTptoken.d 2 replace /users/mjc/system
  860420 18:49:36 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTptoken.d 3 replace /users/mjc/system
  860422 22:56:10 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTptoken.d 4 replace /users/mjc/system
  860501 23:48:25 mjc Return P-Token's for further development by jck

 ************************************************************************/
/************************************************************************
  RTptoken.d 5 replace /users/jck/system
  860514 09:53:25 jck PTConstructors and Collapse Algorithm Added

 ************************************************************************/
/************************************************************************
  RTptoken.d 6 replace /users/jck/system
  860515 10:06:52 jck Bug in FTraverseAdjustments macro fixed

 ************************************************************************/
/************************************************************************
  RTptoken.d 7 replace /users/jck/system
  860521 12:39:33 jck Added rtPTOKEN_ToPTokenNoDiscard

 ************************************************************************/
/************************************************************************
  RTptoken.d 8 replace /users/jck/system
  860523 12:50:09 jck Added rtPTOKEN_FTraverseInstructions macro

 ************************************************************************/
/************************************************************************
  RTptoken.d 9 replace /users/jck/system
  860723 12:33:27 jck Added DataBase Update Utility Macros

 ************************************************************************/
/************************************************************************
  RTptoken.d 10 replace /users/jad/system
  860814 14:15:36 jad added rtPTOKEN_IsntCurrent and
rtPTOKEN_CPDCumAdjustments which used together eliminate a CPD in Aligns

 ************************************************************************/
/************************************************************************
  RTptoken.d 11 replace /users/jad/system
  860814 17:53:26 jad added counters and a displayCounts debug method

 ************************************************************************/
/************************************************************************
  RTptoken.d 12 replace /users/mjc/system
  860817 20:15:42 mjc Converted CPD initializer/instantiator/gc routines to expect nothing of new CPDs

 ************************************************************************/
/************************************************************************
  RTptoken.d 13 replace /users/mjc/system
  860818 12:24:25 mjc Correct loss of 'rtPTOKEN_IsntCurrent'

 ************************************************************************/
/************************************************************************
  RTptoken.d 14 replace /users/mjc/system
  860819 12:33:35 mjc Converted 'BaseElementCount' and 'Is/IsntCurrent' to use fast preamble access

 ************************************************************************/
/************************************************************************
  RTptoken.d 15 replace /users/jad/system
  870109 13:38:29 jad implemented cartesian ptokens

 ************************************************************************/
/************************************************************************
  RTptoken.d 16 replace /users/jck/updates
  870129 13:58:38 jck Macros added for the incorporator

 ************************************************************************/
/************************************************************************
  RTptoken.d 17 replace /users/jck/system
  870202 13:29:18 jck Broke sizing macro into three; one for each type of ptoken

 ************************************************************************/
/************************************************************************
  RTptoken.d 18 replace /users/jad/system
  870303 16:13:44 jad added a cartesian ptoken verification routine

 ************************************************************************/
/************************************************************************
  RTptoken.d 19 replace /users/mjc/translation
  870524 09:34:45 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTptoken.d 20 replace /users/jck/system
  870706 08:15:24 jck Added an independent of vstore flag to ptokens which will be unset at time of vstore creation

 ************************************************************************/
/************************************************************************
  RTptoken.d 21 replace /users/jck/updates
  870824 14:56:31 jck Added Macros for the incorporator's use

 ************************************************************************/
/************************************************************************
  RTptoken.d 22 replace /users/jck/system
  880120 20:14:19 jck Removed the unnecessary inclusion of unwind.h

 ************************************************************************/

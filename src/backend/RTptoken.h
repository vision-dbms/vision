/*****  Positional Generation Token Representation Type Header File  *****/
#ifndef rtPTOKEN_H
#define rtPTOKEN_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VContainerHandle.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTptoken.d"

/************************
 *****  Supporting  *****
 ************************/

class M_CPD;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtPTOKEN_CPD_StdPtrCount	3

#define rtPTOKEN_CPx_NextGeneration	(unsigned int)0
#define rtPTOKEN_CPx_ColPToken		(unsigned int)1
#define rtPTOKEN_CPx_RowPToken		(unsigned int)2

/*****  Standard CPD Access Macros  *****/
#define rtPTOKEN_CPD_Base(cpd)\
    ((rtPTOKEN_Type *)M_CPD_ContainerBase (cpd))

#define rtPTOKEN_CPD_NextGeneration(cpd)\
    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_NextGeneration)

#define rtPTOKEN_CPD_NextGenerationCPD(cpd) (\
    (cpd)->GetCPD (rtPTOKEN_CPx_NextGeneration, RTYPE_C_PToken)\
)

#define rtPTOKEN_CPD_ChangeArray(cpd, changeDescriptorType)\
    rtPTOKEN_PT_ChangeArray(rtPTOKEN_CPD_Base (cpd), changeDescriptorType)

#define rtPTOKEN_CPD_TokenType(cpd)\
    rtPTOKEN_PT_TokenType (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_Independent(cpd)\
    rtPTOKEN_PT_Independent (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_IsDependent(cpd)\
    !rtPTOKEN_PT_Independent (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_Closed(cpd)\
    rtPTOKEN_PT_Closed (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_Count(cpd)\
    rtPTOKEN_PT_Count (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_BaseElementCount(cpd)\
    rtPTOKEN_PT_BaseElementCount (rtPTOKEN_CPD_Base (cpd))

/*****  Cartesian PToken Macros  *****/
#define rtPTOKEN_CPD_CartesianPTokens(cpd)\
    rtPTOKEN_PT_CartesianPTokens (rtPTOKEN_CPD_Base (cpd))

#define rtPTOKEN_CPD_RowPToken(cpd)\
    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_RowPToken)

#define rtPTOKEN_CPD_ColPToken(cpd)\
    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_ColPToken)

#define rtPTOKEN_CPD_RowPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtPTOKEN_CPx_RowPToken, RTYPE_C_PToken)\
)

#define rtPTOKEN_CPD_ColPTokenCPD(cpd) (\
    (cpd)->GetCPD (rtPTOKEN_CPx_ColPToken, RTYPE_C_PToken)\
)

/*****  Query Macros  *****/
#define rtPTOKEN_CPD_IsShift(cpd)\
    (rtPTOKEN_CPD_TokenType (cpd) == rtPTOKEN_TT_Shift)

#define rtPTOKEN_CPD_IsntShift(cpd)\
    (!rtPTOKEN_CPD_IsShift (cpd))

#define rtPTOKEN_CPD_IsCartesian(cpd)\
    (rtPTOKEN_CPD_TokenType (cpd) == rtPTOKEN_TT_Cartesian)

/* This macro returns true only if the ptoken is a base ptoken or is a current
   cartesian ptoken.  It also forces non current cartesian ptokens to update.
   Warning: if used improperly, this can cause an infinite update recursive
   loop. */
#define rtPTOKEN_CPD_IsTerminal(cpd) (\
    rtPTOKEN_CPD_TokenType (cpd) == rtPTOKEN_TT_Base\
    ? true\
    : rtPTOKEN_CPD_TokenType (cpd) == rtPTOKEN_TT_Shift\
    ? false\
    : rtPTOKEN__CartesianPTIsCurrent (cpd, -1, true))


/*************************************************
 *************************************************
 *****  PToken Constructor Type Definitions  *****
 *************************************************
 *************************************************/
/*---------------------------------------------------------------------------
 * PToken Constructors are transient computational structures for use in
 * building Shift type PTokens and in collapsing a chain of PTokens into a
 * single set of editting instructions.
 *---------------------------------------------------------------------------
 */

/*********************************
 *****  SD Constructor Type  *****
 *********************************/
/*---------------------------------------------------------------------------
 * An SD Constructor is a cell in a doubly linked list of shift descriptors
 *
 * SD Constructor field descriptions:
 *	prevSDC		-  a pointer to SDC's predecessor.
 *	nextSDC		-  a pointer to SDC's successor.
 *	currentOrigin	-  the origin in terms of the current base's
 * 			   coordinate system (as opposed to the coordinate
 *			   system of the base being edited.
 *	sd		_  this constructors SD.
 *---------------------------------------------------------------------------
 */
struct rtPTOKEN_SDCType {
    rtPTOKEN_SDCType		*prevSDC,
				*nextSDC;
    int				currentOrigin;
    rtPTOKEN_SDType		sd;
};

/*****  Access Macros  *****/
#define rtPTOKEN_SDC_PrevSDC(sdcp)	((sdcp)->prevSDC)
#define rtPTOKEN_SDC_NextSDC(sdcp)	((sdcp)->nextSDC)
#define rtPTOKEN_SDC_CurrentOrigin(sdcp)((sdcp)->currentOrigin)
#define rtPTOKEN_SDC_SD(sdcp)		((sdcp)->sd)

#define rtPTOKEN_SDC_Origin(sdcp) rtPTOKEN_SD_D_Origin (rtPTOKEN_SDC_SD(sdcp))

#define rtPTOKEN_SDC_Shift(sdcp)  rtPTOKEN_SD_D_Shift  (rtPTOKEN_SDC_SD(sdcp))

/********************************
 *****  Order Field Values  *****
 ********************************/

#define rtPTOKEN_Order_Undefined    0
#define rtPTOKEN_Order_Backward	    1
#define rtPTOKEN_Order_Forward	    2


/*************************************
 *****  PToken Constructor Type  *****
 *************************************/
/*---------------------------------------------------------------------------
 * A P-Token constructor is a header which contains a standard CPD for the next
 * generation P-Token, some bookkeeping information and a head and tail for a
 * chain of SD constructors which contain the editting instructions for this
 * P-Token constructor.
 *
 *  P-Token Constructor field descriptions:
 *	m_pNextGeneration	-  A standard CPD for the next generation
 *				   P-Token. While constructing a new P-Token,
 *    				   this field represents the positional state
 * 			   	   of the most recent generation. While
 * 				   collapsing a chain of P-Tokens, this field
 * 				   represents the next P-Token to be
 * 				   assimilated.
 *	m_pRowPToken		-  A standard CPD for the cartesian row ptoken.
 *	                           This field may be nil.
 *	m_pColPToken		-  A standard CPD for the cartesian column
 *	                           ptoken.  This field may be nil.
 *	m_xOrder		-  either: rtPTOKEN_Order_Backward,
 *                                         rtPTOKEN_Order_Forward,  or
 *                                         rtPTOKEN_Order_Undefined.
 *	m_cAlterations		-  The number of SD's in this P-Token
 * 				   constructor.
 *	m_iCardinality		-  The number of elements in the positional
 * 				   structure defined by this P-Token
 * 				   constructor.
 *	m_pSDChainHead		-  The head of a chain of SDC's containing
 *				   this P-Token constructor's editting info.
 *	m_pSDChainTail		-  The tail of a chain of SDC's containing
 *				   this P-Token constructor's editting info.
 *---------------------------------------------------------------------------
 */
class rtPTOKEN_CType : public VTransient {
//  Construction
public:
    rtPTOKEN_CType (M_CPD *pPToken);

private:
    void InsertSDC (int origin, int shift);

public:
    rtPTOKEN_CType *AppendAdjustment (int origin, int shift);

//  Destruction
protected:
    ~rtPTOKEN_CType ();
public:
    void discard () {
	delete this;
    }

//  Access
public:
    M_CPD *NextGeneration () const {
	return m_pNextGeneration;
    }

//  Query
public:
    bool OrderIsUndefined () const {
	return m_xOrder == rtPTOKEN_Order_Undefined;
    }
    bool OrderIsForward () const {
	return m_xOrder == rtPTOKEN_Order_Forward;
    }
    bool OrderIsBackward () const {
	return m_xOrder == rtPTOKEN_Order_Backward;
    }

//  Use
private:
    void ReverseOrder ();

public:
    M_CPD *ToPToken ();

//  State
public:
    M_CPD*			m_pNextGeneration;
    M_CPD*			m_pRowPToken;
    M_CPD*			m_pColPToken;
    unsigned int		m_xOrder	:   2,
				m_cAlterations	:  30,
				m_iCardinality;
    rtPTOKEN_SDCType		*m_pSDChainHead,
				*m_pSDChainTail;
};

/*****  Access Macros  *****/
#define rtPTOKEN_PTC_NextGenCPD(ptcp)		((ptcp)->m_pNextGeneration)
#define rtPTOKEN_PTC_CartesianRowPTCPD(ptcp)	((ptcp)->m_pRowPToken)
#define rtPTOKEN_PTC_CartesianColPTCPD(ptcp)	((ptcp)->m_pColPToken)
#define rtPTOKEN_PTC_Order(ptcp)		((ptcp)->m_xOrder)
#define rtPTOKEN_PTC_AltCount(ptcp)		((ptcp)->m_cAlterations)
#define rtPTOKEN_PTC_BaseCount(ptcp)		((ptcp)->m_iCardinality)
#define rtPTOKEN_PTC_ChainHead(ptcp)		((ptcp)->m_pSDChainHead)
#define rtPTOKEN_PTC_ChainTail(ptcp)		((ptcp)->m_pSDChainTail)

/*****  Order Query  *****/
#define rtPTOKEN_PTC_OrderIsUndefined(ptcp) (\
    rtPTOKEN_PTC_Order (ptcp) == rtPTOKEN_Order_Undefined\
)

#define rtPTOKEN_PTC_OrderIsForward(ptcp) (\
    rtPTOKEN_PTC_Order (ptcp) == rtPTOKEN_Order_Forward\
)

#define rtPTOKEN_PTC_OrderIsBackward(ptcp) (\
    rtPTOKEN_PTC_Order (ptcp) == rtPTOKEN_Order_Backward\
)


/**********************************
 *****  Normalization Macros  *****
 **********************************/
/*---------------------------------------------------------------------------
 * The following macros are used to traverse a P-Token constructor for the
 * purpose of updating the positional or referential state of an object.
 *
 * Macro Argument Descriptions:
 *	constructor		- The P-Token constructor to be traversed.
 *	insertMacro		- the name of a macro of two arguments -
 *				  'origin', 'shift' - which will
 *				  be invoked to insert positions into the
 *				  object being updated.  'origin' will be
 *				  the positional (NOT byte) origin of the
 *				  tail to be moved, 'shift' will be the number
 *				  of positions to be moved. Once scaled and
 *				  shifted, 'origin' and 'shift' are suitable
 *				  arguments to 'M_ShiftContainerTail'.
 *	deleteMacro		- the name of a macro defined identically to
 *				  'insertMacro' which will be invoked to
 *				  delete positions from the object being
 *				  updated.
 *
 *  Syntactic Context:
 *	STATEMENT
 *
 *****/


#define rtPTOKEN_BTraverseAdjustments(constructor, insertMacro, deleteMacro) {\
    rtPTOKEN_SDCType*		sdcp;\
    int				origin,\
				shift,\
				altIndex,\
				altCount = rtPTOKEN_PTC_AltCount (constructor);\
\
    for (\
	altIndex = 0, sdcp = rtPTOKEN_PTC_ChainHead (constructor);\
	altIndex < altCount;\
	altIndex++  , sdcp = rtPTOKEN_SDC_NextSDC (sdcp)\
    ) {\
	shift  = rtPTOKEN_SDC_Shift  (sdcp);\
	origin = rtPTOKEN_SDC_Origin (sdcp);\
	if (shift < 0) {\
	    deleteMacro (origin, shift);\
	}\
	else {\
	    insertMacro (origin, shift);\
	}\
    }\
}

#define rtPTOKEN_BTraverseAdjustments2(constructor, insertMacro, deleteMacro)\
{\
    rtPTOKEN_SDCType*		sdcp;\
    int				origin,\
				shift,\
				altIndex,\
				altCount = rtPTOKEN_PTC_AltCount (constructor);\
\
    for (\
	altIndex = 0, sdcp = rtPTOKEN_PTC_ChainHead (constructor);\
	altIndex < altCount;\
	altIndex++  , sdcp = rtPTOKEN_SDC_NextSDC (sdcp)\
    ) {\
	shift  = rtPTOKEN_SDC_Shift  (sdcp);\
	origin = rtPTOKEN_SDC_Origin (sdcp);\
	if (shift < 0) {\
	    deleteMacro (origin, shift);\
	}\
	else {\
	    insertMacro (origin, shift);\
	}\
    }\
}


/*****  The following macro forward traverses the ptoken instruction set
 *****  using origins of the original base positional object.  It thus
 *****  differs from _FTraverseAdjustments in that it assumes that the
 *****  instructions have not been executed and that the resulting state of
 *****  the object after looking at the instruction is unchanged.
 *****/
#define rtPTOKEN_FTraverseInstructions(constructor, insertMacro, deleteMacro) {\
    rtPTOKEN_SDCType*		sdcp;\
    int				origin,\
				shift,\
				altIndex,\
				altCount = rtPTOKEN_PTC_AltCount (constructor);\
\
    for (\
	altIndex = 0, sdcp = rtPTOKEN_PTC_ChainTail (constructor);\
	altIndex < altCount;\
	altIndex++	 , sdcp = rtPTOKEN_SDC_PrevSDC (sdcp)\
    ) {\
	shift  = rtPTOKEN_SDC_Shift  (sdcp);\
	origin = rtPTOKEN_SDC_Origin (sdcp);\
	if (shift < 0) {\
	    deleteMacro (origin, shift);\
	}\
	else {\
	    insertMacro (origin, shift);\
	}\
    }\
}


/*****  Normalization Chain Mapper Macro  *****/
#define rtPTOKEN_FTraverseAdjustments(constructor, insertMacro, deleteMacro)\
{\
    rtPTOKEN_SDCType*		sdcp;\
    int				origin,\
				shift,\
				altIndex,\
				altCount = rtPTOKEN_PTC_AltCount (constructor);\
\
    for (\
	altIndex = 0, sdcp = rtPTOKEN_PTC_ChainTail (constructor);\
	altIndex < altCount;\
	altIndex++	 , sdcp = rtPTOKEN_SDC_PrevSDC (sdcp)\
    ) {\
	shift  = rtPTOKEN_SDC_Shift (sdcp);\
	origin = rtPTOKEN_SDC_CurrentOrigin (sdcp) - (shift < 0 ? shift : 0);\
	if (shift < 0) {\
	    deleteMacro (origin, shift);\
	}\
	else {\
	    insertMacro (origin, shift);\
	}\
    }\
}


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

/**********************
 *****  Routines  *****
 **********************/

PublicFnDecl M_CPD *rtPTOKEN__CartsianPTIsntCurrent (
    M_CPD *pPTokenRef, unsigned int xPTokenRef, bool update
);

PublicFnDecl bool rtPTOKEN__CartesianPTIsCurrent (
    M_CPD *pPTokenRef, int xPTokenRef, bool update
);

PublicFnDecl void rtPTOKEN_CartesianVerification (
    M_CPD *cartesianPT, M_CPD *pRowPToken, M_CPD *cPTokenRefCPD, int cPTokenRefIndex
);

PublicFnDecl unsigned int rtPTOKEN_BaseElementCount (
    M_CPD *pPTokenRef, int xPTokenRef
);

PublicFnDecl bool rtPTOKEN_IsCurrent (
    M_CPD *pPTokenRef, int xPTokenRef
);

PublicFnDecl bool rtPTOKEN_IsntCurrent (
    M_CPD *pPTokenRef, unsigned int xPTokenRef, M_CPD*&rpPTokenReturn
);

PublicFnDecl M_CPD *rtPTOKEN_BasePToken (
    M_CPD *pPTokenRef, int xPTokenRef
);

PublicFnDecl bool rtPTOKEN_AllAlterationsBeyondEnd (
    M_CPD *pPTokenRef, int xPTokenRef
);

PublicFnDecl M_CPD *rtPTOKEN_New (
    M_ASD *pContainerSpace, unsigned int nelements
);

PublicFnDecl M_CPD *rtPTOKEN_New (
    M_ASD *pContainerSpace, M_CPD *pRowRef, int xRowRef, M_CPD *pColRef, int xColRef
);

PublicFnDecl rtPTOKEN_CType *rtPTOKEN_NewShiftPTConstructor (
    M_CPD *currentPTokenRefCPD, int currentPTokenRefIndex
);

PublicFnDecl rtPTOKEN_CType *rtPTOKEN_CPDCumAdjustments (
    M_CPD *ptokenCPD
);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtPTOKEN_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtPTOKEN_Handle, VContainerHandle);

//  Construction
protected:
    rtPTOKEN_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtPTOKEN_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
public:
    void CheckConsistency ();

protected:
    bool PersistReferences ();

//  State
protected:
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTptoken.h 1 replace /users/mjc/system
  860404 20:14:07 mjc Create Positional Generation Token Representation Type

 ************************************************************************/
/************************************************************************
  RTptoken.h 1 replace /users/mjc/system
  860409 08:00:28 mjc Create positional generation token type

 ************************************************************************/
/************************************************************************
  RTptoken.h 2 replace /users/mjc/system
  860420 18:49:39 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTptoken.h 3 replace /users/mjc/system
  860422 22:56:19 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTptoken.h 4 replace /users/mjc/system
  860501 23:48:28 mjc Return P-Token's for further development by jck

 ************************************************************************/
/************************************************************************
  RTptoken.h 5 replace /users/jck/system
  860514 09:53:28 jck PTConstructors and Collapse Algorithm Added

 ************************************************************************/
/************************************************************************
  RTptoken.h 6 replace /users/jck/system
  860521 12:39:36 jck Added rtPTOKEN_ToPTokenNoDiscard

 ************************************************************************/
/************************************************************************
  RTptoken.h 7 replace /users/mjc/system
  860708 13:06:54 mjc Added 'rtPTOKEN_BasePToken', removed some UNWIND traps, changed internal method names

 ************************************************************************/
/************************************************************************
  RTptoken.h 8 replace /users/jad/system
  860814 14:15:43 jad added rtPTOKEN_IsntCurrent and 
rtPTOKEN_CPDCumAdjustments which used together eliminate a CPD in Aligns

 ************************************************************************/
/************************************************************************
  RTptoken.h 9 replace /users/jad/system
  860814 17:53:31 jad added counters and a displayCounts debug method

 ************************************************************************/
/************************************************************************
  RTptoken.h 10 replace /users/jad/system
  870109 13:38:33 jad implemented cartesian ptokens

 ************************************************************************/
/************************************************************************
  RTptoken.h 11 replace /users/jad/system
  870303 16:13:48 jad added a cartesian ptoken verification routine

 ************************************************************************/
/************************************************************************
  RTptoken.h 12 replace /users/mjc/translation
  870524 09:34:49 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

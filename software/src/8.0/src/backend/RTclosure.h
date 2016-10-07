/*****  Closure Representation Type Header File  *****/
#ifndef rtCLOSURE_H
#define rtCLOSURE_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VConstructor.h"
#include "VContainerHandle.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTclosure.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTblock.h"
#include "RTcontext.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtCLOSURE_CPD_StdPtrCount	2

#define rtCLOSURE_CPx_Context		(unsigned int)0
#define rtCLOSURE_CPx_Block		(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtCLOSURE_CPD_Base(cpd)\
    ((rtCLOSURE_Closure*) M_CPD_ContainerBase (cpd))

#define rtCLOSURE_CPD_Context(cpd)\
    M_CPD_PointerToPOP (cpd, rtCLOSURE_CPx_Context)

#define rtCLOSURE_CPD_Block(cpd)\
    M_CPD_PointerToPOP (cpd, rtCLOSURE_CPx_Block)

#define rtCLOSURE_CPD_Primitive(cpd)\
    rtCLOSURE_Closure_Primitive (rtCLOSURE_CPD_Base (cpd))

#define rtCLOSURE_CPD_ContextCPD(cpd) (\
    (cpd)->GetCPD (rtCLOSURE_CPx_Context, RTYPE_C_Context)\
)

#define rtCLOSURE_CPD_BlockCPD(cpd) (\
    (cpd)->GetCPD (rtCLOSURE_CPx_Block, RTYPE_C_Block)\
)

/*****  Content Query Predicates  *****/
#define rtCLOSURE_CPD_IsABlockClosure(cpd) (\
    (cpd)->ReferenceIsntNil (rtCLOSURE_CPx_Block)\
)

#define rtCLOSURE_CPD_IsAPrimClosure(cpd) (\
    (cpd)->ReferenceIsNil (rtCLOSURE_CPx_Block)\
)


/*************************
 *************************
 *****  Constructor  *****
 *************************
 *************************/

/*---------------------------------------------------------------------------
 * Closures associate a context with a function (e.g., block or primitive) to
 * create a runnable object.
 *
 *  Closure Field Descriptions:
 *	header			- a psuedo object header for this closure.
 *				  THIS FIELD MUST ALWAYS BE THE FIRST FIELD
 *				  IN THE CLOSURE.
 *	m_pBlock		- an optional ('Nil' if absent) block handle.
 *				  If specified, this is a block closure; if
 *				  absent, a primitive closure.
 *	m_xPrimitive		- the index of a primitive for primitive closures.
 *				  Ignored for block closures.
 *	m_pContext		- the context associated with this closure.
 *---------------------------------------------------------------------------
 */
class rtCLOSURE_Constructor : public VConstructor {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtCLOSURE_Constructor, VConstructor);

//  CPD Initialization
public:
    static void InitStdCPD (M_CPD* pCPD);

//  Construction
public:
    rtCLOSURE_Constructor (
	rtCONTEXT_Constructor *context, rtBLOCK_Handle *pBlockHandle, unsigned int iAttentionMask = 0
    );
    rtCLOSURE_Constructor (
	rtCONTEXT_Constructor *context, unsigned int xPrimitive, unsigned int iAttentionMask = 0
    );

//  Destruction
private:
    ~rtCLOSURE_Constructor ();

//  Query
public:
    bool isABlockClosure () const {
	return m_pBlock.isntNil ();
    }
    bool isAPrimitiveClosure () const {
	return m_pBlock.isNil ();
    }

//  Access
public:
    rtBLOCK_Handle *block () const {
	return m_pBlock;
    }
    rtCONTEXT_Constructor* context () const {
	return m_pContext;
    }
    unsigned int primitive () const {
	return m_xPrimitive;
    }

    RTYPE_Type RType_() const;

    M_ASD *Space_() const;

//  Realization
protected:
    M_CPD* newRealization ();

//  State
protected:
    rtCONTEXT_Constructor::Reference const	m_pContext;
    rtBLOCK_Handle::Reference const		m_pBlock;
    unsigned int const				m_xPrimitive;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl bool rtCLOSURE_Align (M_CPD *closure);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtCLOSURE_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtCLOSURE_Handle, VContainerHandle);

//  Construction
protected:
    rtCLOSURE_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtCLOSURE_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
protected:
    bool PersistReferences ();

//  State
protected:
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTclosure.h 1 replace /users/mjc/system
  861002 17:55:45 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTclosure.h 2 replace /users/mjc/translation
  870524 09:32:03 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

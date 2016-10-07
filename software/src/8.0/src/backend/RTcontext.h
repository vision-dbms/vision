/*****  Context Representation Type Header File  *****/
#ifndef rtCONTEXT_H
#define rtCONTEXT_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "popvector.h"

#include "DSC_Descriptor.h"

#include "VConstructor.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTcontext.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtCONTEXT_CPD_StdPtrCount (rtCONTEXT_Context_POPCount + POPVECTOR_CPD_XtraPtrCount)

#define rtCONTEXT_CPx_Current	(unsigned int)0
#define rtCONTEXT_CPx_Self	(unsigned int)1
#define rtCONTEXT_CPx_Origin	(unsigned int)2

#define rtCONTEXT_CPD_CurrentCPD(cpd) (\
    (cpd)->GetCPD (rtCONTEXT_CPx_Current, RTYPE_C_Descriptor)\
)

#define rtCONTEXT_CPD_SelfCPD(cpd) (\
    (cpd)->GetCPD (rtCONTEXT_CPx_Self	, RTYPE_C_Descriptor)\
)

#define rtCONTEXT_CPD_OriginCPD(cpd) (\
    (cpd)->GetCPD (rtCONTEXT_CPx_Origin	, RTYPE_C_Descriptor)\
)


/*********************************************
 *********************************************
 *****  rtCONTEXT_Constructor Interface  *****
 *********************************************
 *********************************************/

/*---------------------------------------------------------------------------
 * Contexts define the values of '^current', '^self', and '^my' for a task.
 * The following type defines the structure of a context psuedo-object.
 *
 *  Context Constructor Field Descriptions:
 *	header			- a psuedo-object header for this context.
 *				  THIS FIELD MUST ALWAYS BE THE FIRST FIELD
 *				  IN THE CONTEXT.
 *	parent			- an optimal descriptor for the parent of this
 *				  context.  This descriptor is either empty or
 *				  references another context.
 *	current			- an optimal descriptor for this context's
 *				  value of '^current'.
 *	self			- an optimal descriptor for this context's
 *				  value of '^self'.  If this descriptor is
 *				  empty, its value can be derived from the
 *				  value of 'self' in the 'parent' context.
 *	origin			- a general descriptor for this context's
 *				  value of '^my'.  If this descriptor is
 *				  empty, its value can be derived from the
 *				  value of 'origin' in the 'parent' context.
 *---------------------------------------------------------------------------
 */


/*---------------------------*
 *----  Type Definition  ----*
 *---------------------------*/

class rtCONTEXT_Constructor : public VConstructor {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtCONTEXT_Constructor, VConstructor);

//  Construction
public:
    rtCONTEXT_Constructor (
	DSC_Descriptor& rSelf, DSC_Descriptor& rCurrent, DSC_Descriptor& rMy, DSC_Descriptor& rParent
    );
    rtCONTEXT_Constructor (
	DSC_Descriptor& rSelf, DSC_Descriptor& rCurrent, DSC_Descriptor& rMy
    );
    rtCONTEXT_Constructor (DSC_Descriptor const& rSelfCurrentAndMy);
    rtCONTEXT_Constructor (M_CPD*);

//  Destruction
protected:
    ~rtCONTEXT_Constructor ();

//  Access
public:
    RTYPE_Type RType_() const;

    M_ASD *Space_() const;

protected:
    DSC_Descriptor& Get (DSC_Descriptor rtCONTEXT_Constructor::*pMemberComponent);

public:
    DSC_Descriptor& getSelf () {
	return Get (&rtCONTEXT_Constructor::m_iSelf);
    }
    DSC_Descriptor& getCurrent () {
	return Get (&rtCONTEXT_Constructor::m_iCurrent);
    }
    DSC_Descriptor& getMy () {
	return Get (&rtCONTEXT_Constructor::m_iMy);
    }

    DSC_Descriptor& self () {
	return m_iSelf;
    }
    DSC_Descriptor& current () {
	return m_iCurrent;
    }
    DSC_Descriptor& my () {
	return m_iMy;
    }

//  Realization
protected:
    M_CPD* newRealization ();

//  State
protected:
    DSC_Descriptor m_iSelf;
    DSC_Descriptor m_iCurrent;
    DSC_Descriptor m_iMy;
    DSC_Descriptor m_iParent;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl bool rtCONTEXT_Align (M_CPD* context);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtCONTEXT_Handle : public rtPOPVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtCONTEXT_Handle, rtPOPVECTOR_Handle);

//  Construction
protected:
    rtCONTEXT_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtCONTEXT_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:

//  Query
public:

//  Callbacks
protected:

//  State
protected:
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTcontext.h 1 replace /users/mjc/system
  861002 17:55:38 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTcontext.h 2 replace /users/mjc/translation
  870524 09:32:16 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

/*****  Selector U-Vector Representation Type Header File  *****/
#ifndef rtSELUV_H
#define rtSELUV_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "uvector.h"

#include "VSet.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTseluv.d"

/************************
 *****  Supporting  *****
 ************************/

#include "selector.h"

#include "VAssociativeCursor.h"
#include "VCollectionOf.h"
#include "VCollectionOfStrings.h"
#include "VOrdered.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/
/*---------------------------------------------------------------------------
 * Selector U-Vectors inherit a basic standard CPD format from UV which
 * the following definitions customize.
 *---------------------------------------------------------------------------
 */

/*****  Standard CPD Pointer Assignments  ****/
#define rtSELUV_CPx_StringSpace    UV_CPx_AuxillaryPOP

/*****  Standard CPD Access Macros  *****/
#define rtSELUV_CPD_StringSpace(cpd) UV_CPD_AuxillaryPOP (cpd)

#define rtSELUV_CPD_StringSpaceCPD(cpd) (\
    (cpd)->GetCPD (rtSELUV_CPx_StringSpace, RTYPE_C_CharUV)\
)

#define rtSELUV_CPD_Array(cpd) UV_CPD_Array (cpd, rtSELUV_Type_Element)

#define rtSELUV_CPD_Element(cpd) M_CPD_PointerToType (\
    cpd, UV_CPx_Element, rtSELUV_Type_Element *\
)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtSELUV_New (M_CPD *pPPT, M_CPD *pRPT);

PublicFnDecl M_CPD *rtSELUV_Align (M_CPD *cpd);

PublicFnDecl M_CPD *rtSELUV_Contents (M_ASD *pContainerSpace, M_CPD *selectorSet);


/*******************************
 *******************************
 *****  Types and Classes  *****
 *******************************
 *******************************/

/*************************
 *----  rtSELUV_Set  ----*
 *************************/

class rtSELUV_Set : public VSet {
//  Friends
    friend class VAssociativeCursor<rtSELUV_Set, VCollectionOfStrings, char const*>;
    friend class VAssociativeCursor<rtSELUV_Set, VCollectionOfUnsigned32, unsigned int>;

//  Construction
public:
    rtSELUV_Set (M_CPD *pSetRef, int xSetRef);
    rtSELUV_Set (M_CPD *pSelectorSetCPD);

//  Destruction
public:
    ~rtSELUV_Set ();

//  Deferred Initialization
protected:
    virtual void initializeDPT ();

//  String Space Management
protected:
    void accessStringSpaceIfNecessary () {
	if (m_pStringSpaceCPD.isNil ())
	    accessStringSpace ();
	else
	    resyncStringSpace ();
    }
    void accessStringSpace ();
    void resyncStringSpace ();

    unsigned int growStringSpace (char const* pString);
    unsigned int growStringSpace (unsigned int sGrowth, unsigned int xOrigin = 0);

//  Alignment
public:
    void align () {
	rtSELUV_Align (m_pSetCPD);
    }

//  Access
public:
    unsigned int cardinality () const { return UV_CPD_ElementCount (m_pSetCPD); }

    int compare (unsigned int xElement, char const*  pSelector);
    int compare (unsigned int xElement, unsigned int xSelector);

    void Locate (
	VCollectionOfStrings	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Locate (
	VCollectionOfUnsigned32	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    bool Locate (char const*  pSelector, unsigned int& rxElement);
    bool Locate (unsigned int xSelector, unsigned int& rxElement);

//  Update
public:
    void Insert (
	VCollectionOfStrings	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Insert (
	VCollectionOfUnsigned32	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    void Delete (
	VCollectionOfStrings	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Delete (
	VCollectionOfUnsigned32	*pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    bool Insert (char const*  pSelector, unsigned int& rxElement);
    bool Insert (unsigned int xSelector, unsigned int& rxElement);

    bool Delete (char const*  pSelector);
    bool Delete (unsigned int xSelector);

//  Update Utilities
protected:
    void install (unsigned int xElement, unsigned int xSelectorType, unsigned int xSelector) {
	rtSELUV_Type_Element* pElement  = rtSELUV_CPD_Array (m_pSetCPD) + xElement;
	rtSELUV_Element_Type (pElement) = xSelectorType;
	rtSELUV_Element_Index(pElement) = xSelector;
    }

    void install (unsigned int xElement, char const* pSelector) {
	install (xElement, SELECTOR_C_DefinedSelector, growStringSpace (pSelector));
    }
    void install (unsigned int xElement, unsigned int xSelector) {
	install (xElement, SELECTOR_C_KnownSelector, xSelector);
    }

    void install (
	rtLINK_CType* pAdditions, VOrdered<VCollectionOfStrings,char const*> &rAdditions
    );
    void install (
	rtLINK_CType* pAdditions, VOrdered<VCollectionOfUnsigned32,unsigned int> &rAdditions
    );

//  State
protected:
    VCPDReference const		m_pSetCPD;
    VCPDReference		m_pStringSpaceCPD;
    char const *		m_pStringStorage;
};


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtSELUV_Handle : public rtUVECTOR_Handle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtSELUV_Handle, rtUVECTOR_Handle);

//  Construction
protected:
    rtSELUV_Handle (M_CTE &rCTE) : rtUVECTOR_Handle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtSELUV_Handle (rCTE);
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
  RTseluv.h 1 replace /users/mjc/system
  860411 22:02:10 mjc Create 'selector' u-vectors

 ************************************************************************/
/************************************************************************
  RTseluv.h 2 replace /users/mjc/system
  860420 18:49:48 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTseluv.h 3 replace /users/jck/system
  860521 12:37:22 jck First Cut Implementation with enough functionallity for Method Dictionary

 ************************************************************************/
/************************************************************************
  RTseluv.h 4 replace /users/jck/system
  860522 12:04:57 jck Added New Scalar Assignment routines

 ************************************************************************/
/************************************************************************
  RTseluv.h 5 replace /users/mjc/maketest
  860708 12:45:43 mjc Deleted references to 'RTselector' and unneeded routines

 ************************************************************************/
/************************************************************************
  RTseluv.h 6 replace /users/mjc/system
  870104 22:52:13 lcn Added parallel lookup and addition routines

 ************************************************************************/
/************************************************************************
  RTseluv.h 7 replace /users/jad/system
  870128 19:05:20 jad implemented delete and contents routines

 ************************************************************************/
/************************************************************************
  RTseluv.h 8 replace /users/mjc/translation
  870524 09:35:15 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

/*****  Physical Object Pointer Array R-Type Header File  *****/
#ifndef rtPARRAY_H
#define rtPARRAY_H

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

#include "RTparray.d"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtPARRAY_CPD_StdPtrCount	1

#define rtPARRAY_CPx_Element		(unsigned int)0


/*****  Access Macros  *****/
#define rtPARRAY_CPD_ElementCount(cpd) (M_CPD_Size (cpd) / sizeof (M_POP))

#define rtPARRAY_CPD_ElementArray(cpd) ((M_POP*)(M_CPD_PreamblePtr (cpd) + 1))

#define rtPARRAY_CPD_Element(cpd) M_CPD_PointerToPOP (cpd, rtPARRAY_CPx_Element)

/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl M_CPD *rtPARRAY_New (M_ASD *pContainerSpace, unsigned int elementCount);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtPARRAY_Handle : public VContainerHandle {
    DECLARE_CONCRETE_RTT (rtPARRAY_Handle, VContainerHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtPARRAY_Handle (rCTE);
    }
private:
    rtPARRAY_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

//  Destruction
private:
    ~rtPARRAY_Handle () {
    }

//  Access
private:
    M_POP *element (unsigned int xElement) const {
	return elementArray () + xElement;
    }
    M_POP *elementArray () const {
	return reinterpret_cast<M_POP*>(containerContent ());
    }
public:
    unsigned int elementCount () const {
	return containerSize () / sizeof (M_POP);
    }

//  Query
public:

//  Callbacks
protected:
    bool PersistReferences ();

//  Display and Inspection
public:
    virtual /*override*/ bool getPOP (M_POP *pResult, unsigned int xPOP) const;
    virtual /*override*/ unsigned int getPOPCount () const {
	return elementCount ();
    }
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTparray.h 1 replace /users/mjc/system
  860323 17:20:20 mjc Physical object pointer array R-Type

 ************************************************************************/
/************************************************************************
  RTparray.h 2 replace /users/mjc/system
  860412 18:05:17 mjc Changed name from 'RTpvector'

 ************************************************************************/
/************************************************************************
  RTparray.h 3 replace /users/mjc/translation
  870524 09:34:23 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

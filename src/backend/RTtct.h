/*****  Transient Container Table Representation Type Header File  *****/
#ifndef rtTCT_H
#define rtTCT_H

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

#include "RTtct.d"

/************************
 *****  Supporting  *****
 ************************/

class M_CT;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Count  *****/
#define rtTCT_CPD_StandardPtrCount	1

#define rtTCT_CPx_Entry			(unsigned int)0

/*****  Access Macros  *****/
#define rtTCT_CPD_Base(cpd)\
    ((M_CT*) M_CPD_ContainerBase (cpd))

#define rtTCT_CPD_EntryCount(cpd)\
    (rtTCT_CPD_Base (cpd)->entryCount ())

#define rtTCT_CPD_Entries(cpd)\
    (rtTCT_CPD_Base (cpd)->entries ())

#define rtTCT_CPD_Entry(cpd)\
    M_CPD_PointerToType (cpd, rtTCT_CPx_Entry, M_DCTE*)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtTCT_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtTCT_Handle, VContainerHandle);

//  Construction
protected:
    rtTCT_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtTCT_Handle (rCTE);
    }

//  Destruction
protected:

//  Access
public:
    M_CT *ct () const {
	return M_CPreamble_ContainerBaseAsType (m_pContainer, M_CT);
    }

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
  RTtct.h 1 replace /users/mjc/system
  870215 22:56:45 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  RTtct.h 2 replace /users/mjc/translation
  870524 09:35:40 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

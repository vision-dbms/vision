/*****  Persistent Container Table Representation Type Header File  *****/
#ifndef rtPCT_H
#define rtPCT_H

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

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Definitions  *****/
#define rtPCT_CPD_StandardPtrCount	1

#define rtPCT_CPx_Entry			(unsigned int)0

/*****  Access Macros  *****/
#define rtPCT_CPD_Base(cpd) ((PS_CT*) M_CPD_ContainerBase (cpd))

#define rtPCT_CPD_SpaceIndex(cpd)	PS_CT_SpaceIndex (rtPCT_CPD_Base (cpd))
#define rtPCT_CPD_Segment(cpd)		PS_CT_Segment (rtPCT_CPD_Base (cpd))
#define rtPCT_CPD_EntryCount(cpd)	PS_CT_EntryCount (rtPCT_CPD_Base (cpd))
#define rtPCT_CPD_FreeList(cpd)		PS_CT_FreeList (rtPCT_CPD_Base (cpd))
#define rtPCT_CPD_Entries(cpd)		PS_CT_Entries (rtPCT_CPD_Base (cpd))

#define rtPCT_CPD_Entry(cpd) M_CPD_PointerToType (cpd, rtPCT_CPx_Entry, PS_CTE*)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtPCT_Handle : public VContainerHandle {
//  Run Time Type
    DECLARE_CONCRETE_RTT (rtPCT_Handle, VContainerHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtPCT_Handle (rCTE);
    }
protected:
    rtPCT_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

//  Destruction
private:
    ~rtPCT_Handle () {
    }

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
  RTpct.h 1 replace /users/mjc/system
  870215 22:56:39 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  RTpct.h 2 replace /users/mjc/translation
  870524 09:34:36 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

/*****  Fault Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfFAULT

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vfault.h"
#include "vstdio.h"
#include "vprimfns.h"

#include "RTlink.h"
#include "RTlstore.h"
#include "RTptoken.h"

#include "RTintuv.h"
#include "RTvector.h"

/*****  Self  *****/
#include "PFfault.h"


/*******************************
 *******************************
 *****  Primitive Indices  *****
 *******************************
 *******************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that assign indices to the primitives.
 *
 * The numeric indices associated in this section with each of the primitives
 * are the system wide 'indices' by which the primitives are known.  These
 * indices are stored in permanent object memory;  consequently, once assigned,
 * they must not be changed or deleted.  To avoid an fatal error at system
 * startup, the largest index assigned in this section must be less than
 * 'V_PF_MaxPrimitiveIndex' (see "vprimfns.h").
 * If necessary this maximum can be increased.
 *---------------------------------------------------------------------------
 */
/*======================================*
 *  This module currently has indices:  *
 *	371-390                         *
 *======================================*/


/******************************
 *****  Fault Primitives  *****
 ******************************/

#define XFaultSeverities 		371
#define XFaultTimes			372
#define XFaultDescriptions		373
#define XMoreFaultInfoIsAvail		374

#define XFaultErrorCodes		375
#define XFaultHandlerErrorCodes		376
#define XFaultFacilities		377
#define XFaultFileNames			378
#define XFaultLineNumbers		379


/***********************************
 ***********************************
 *****  Primitive Definitions  *****
 ***********************************
 ***********************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that define the code bodies of the primitives.
 *---------------------------------------------------------------------------
 */


/******************************
 *****  Fault Primitives  *****
 ******************************/

/*---------------------------------------------------------------------------
 *****  Function to Load a nil referenced store into the accumulator.
 *
 *  Arguments:
 *	store		- the store to reference from the accumulator, it
 *			  may be either a lstore or a vector.  As of 6.1.x,
 *			  the accumulator will add a reference to this CPD.
 *	ptokenRefIndex	- the index to the positional ptoken of the store.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LoadAWithNilRefedStore (
    VPrimitiveTask *pTask, M_CPD *store, int ptokenRefIndex
) {
    rtLINK_CType* linkc = rtLINK_PosConstructor (V_TOTSC_PToken, -1);
    rtLINK_AppendRepeat (
	linkc, rtPTOKEN_BaseElementCount (store, ptokenRefIndex), pTask->cardinality ()
    );
    linkc->Close (store, ptokenRefIndex);
    pTask->loadDucWithMonotype (store, linkc);
    linkc->release ();
}


V_DefinePrimitive (AccessFaultStringField) {
    M_CPD *charuv;
    switch (V_TOTSC_Primitive) {
    case XFaultSeverities:
	charuv = FAULT_AccessSeverity (pTask);
	break;
    case XFaultTimes:
	charuv = FAULT_AccessTimestamp (pTask);
	break;
    case XFaultDescriptions:
	charuv = FAULT_AccessDescription (pTask);
	break;
    case XFaultErrorCodes:
	charuv = FAULT_AccessErrorCode (pTask);
	break;
    case XFaultHandlerErrorCodes:
	charuv = FAULT_AccessHandlerErrorCode (pTask);
	break;
    case XFaultFacilities:
	charuv = FAULT_AccessFacility (pTask);
	break;
    case XFaultFileNames:
	charuv = FAULT_AccessFileName (pTask);
	break;
    default:
        pTask->raiseUnimplementedAliasException ("AccessFaultStringField");
	break;
    }

/*****  Make the lstore ... *****/
    M_CPD *lstore = rtLSTORE_NewStringStoreFromUV (charuv);
    charuv->release ();

/*****  Load the accumulator with the store .....,  *****/
    LoadAWithNilRefedStore (pTask, lstore, rtLSTORE_CPx_RowPToken);
    lstore->release ();
}


V_DefinePrimitive (AccessFaultIntegerField) {
    M_CPD *intuv;
    switch (V_TOTSC_Primitive) {
    case XFaultLineNumbers:
	intuv = FAULT_AccessLineNumber (pTask);
	break;
    case XMoreFaultInfoIsAvail:
	intuv = FAULT_AccessMoreInfoAvail (pTask);
	break;
    default:
        pTask->raiseUnimplementedAliasException ("AccessFaultIntegerField");
	break;
    }

/*****  Create a vector containing the integer uvector ...  *****/
    M_CPD *ptoken = pTask->NewCodPToken (UV_CPD_ElementCount (intuv));
    M_CPD *vector = rtVECTOR_New (ptoken);
    ptoken->release ();

    DSC_Descriptor sourceDsc;
    sourceDsc.constructMonotype (intuv);
    intuv->release ();

/*	ref = pos of vector
	pos = pos of src	*/
    rtLINK_CType *linkc = rtLINK_RefConstructor (vector, rtVECTOR_CPx_PToken);
    rtLINK_AppendRange (
	linkc, 0, rtPTOKEN_BaseElementCount (intuv, UV_CPx_PToken)
    );
    linkc->Close (intuv, UV_CPx_PToken);

    rtVECTOR_Assign (vector, linkc, &sourceDsc);
    linkc->release ();
    sourceDsc.clear ();

/*****  Load the accumulator with the result ...  *****/
    LoadAWithNilRefedStore (pTask, vector, rtVECTOR_CPx_PToken);
    vector->release ();
}


/************************************
 ************************************
 *****  Primitive Descriptions  *****
 ************************************
 ************************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that describe the primitives to the virtual machine and
 * debugger.
 *---------------------------------------------------------------------------
 */

BeginDescriptions

/******************************
 *****  Fault Primitives  *****
 ******************************/

    PD (XFaultSeverities,
	"FaultSeverities",
	AccessFaultStringField)
    PD (XFaultTimes,
	"FaultTimes",
	AccessFaultStringField)
    PD (XFaultDescriptions,
	"FaultDescriptions",
	AccessFaultStringField)
    PD (XMoreFaultInfoIsAvail,
	"MoreFaultInfoIsAvail",
	AccessFaultIntegerField)

    PD (XFaultErrorCodes,
	"FaultErrorCodes",
	AccessFaultStringField)
    PD (XFaultHandlerErrorCodes,
	"FaultHandlerErrorCodes",
	AccessFaultStringField)
    PD (XFaultFacilities,
	"FaultFacilities",
	AccessFaultStringField)
    PD (XFaultFileNames,
	"FaultFileNames",
	AccessFaultStringField)
    PD (XFaultLineNumbers,
	"FaultLineNumbers",
	AccessFaultIntegerField)

EndDescriptions


/*******************************************************
 *****  The Primitive Dispatch Vector Initializer  *****
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a portion of the primitive function
 *****	dispatch vector at system startup.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void pfFAULT_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfFAULT);
    FAC_FDFCase_FacilityDescription ("Fault Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFfault.c 1 replace /users/jad/system
  880711 17:08:05 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/

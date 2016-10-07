/*****  Associative List Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfALIST

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTindex.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTptoken.h"
#include "RTvector.h"

/*****  Self  *****/
#include "PFalist.h"


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
 *	281 - 299                       *
 *======================================*/


/*****************************************
 *****  Associative List Primitives  *****
 *****************************************/

#define XLocateItem			281
#define XDefineItem			282

#define XDeleteItem			283
#define XNewAList			284
#define XNewAListInStore		285

#define XDeleteColumn			286

#define XGetKeyMapType			293
#define XSetKeyMapType			294

#define XGetDefaultKeyMapType		298
#define XSetDefaultKeyMapType		299


/***********************************************************
 *****  Cluster Creation -- Replaced by 'NewPrototype' *****
 ***********************************************************/

V_DefinePrimitive (NewAList) {
/*****  Create the index cluster, ...  *****/
    M_CPD *pContentPrototypeCPD = NilOf (M_CPD*);
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
        break;

    case 2:
	pContentPrototypeCPD = pTask->getSelf ().storeCPD ();
	break;

    default:
	pTask->raiseException (
	    EC__UsageError, "NewAList: Unsupported Flag Value: %u", V_TOTSC_PrimitiveFlags
	);
        break;
    }
    M_CPD *pIndexCPD; {
	M_CPD* pInstancePTokenCPD = pTask->NewCodPToken ();
	pIndexCPD = rtINDEX_NewCluster (pInstancePTokenCPD, pContentPrototypeCPD, -1);
	pInstancePTokenCPD->release ();
    }

/*****  ... and load the accumulator with it.  *****/
    pTask->loadDucWithListOrStringStore (rtINDEX_CPD_ListStoreCPD (pIndexCPD));

    ADescriptor.setStoreTo (pIndexCPD);
}


/***********************************************************
 *****  Instance Creation -- Replaced By 'NewInstance' *****
 ***********************************************************/

V_DefinePrimitive (NewAListInStore) {
/*****  Obtain '^current'...  *****/
    pTask->loadDucWithCurrent ();

/*****  Validate that its store is an Index...  *****/
    M_CPD* index = pTask->ducStore ();
    RTYPE_MustBeA ("NewAListInStore", index->RType (), RTYPE_C_Index);

/*****  Manufacture a new collection of instances...  *****/
    rtLINK_CType* newALists = rtINDEX_AddLists (index, V_TOTSC_PToken);

/*****  Return them...  *****/
    DSC_Descriptor_Pointer (ADescriptor).setTo (newALists);

/*****  And clean up.  *****/
    newALists->release ();
}


/*******************************************
 *****  Associative Access and Update  *****
 *******************************************/

V_DefinePrimitive (AccessAList) {
/*****
 *  Send the binary converse of this primitive if the operand is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	switch (V_TOTSC_Primitive) {
	case XLocateItem:
	    pTask->sendBinaryConverseWithCurrent (KS__LocateInAList);
	    break;
	case XDefineItem:
	    pTask->sendBinaryConverseWithCurrent (KS__DefineInAList);
	    break;
	case XDeleteItem:
	    pTask->sendBinaryConverseWithCurrent (KS__DeleteFromAList);
	    break;
	case XDeleteColumn:
	    pTask->sendBinaryConverseWithCurrent (KS__DeleteColumnFromStore);
	    break;
	default:
	    break;
	}
	return;
    }

    pTask->normalizeDuc ();
    rtINDEX_Key *key = new rtINDEX_Key (V_TOTSC_PToken, &ADescriptor);

    pTask->loadDucWithCurrent ();

    DSC_Descriptor observationVectorDsc;
    switch (V_TOTSC_Primitive) {
    case XLocateItem:
	rtINDEX_AListLookup (
	    &observationVectorDsc, &ADescriptor, key
	);
	pTask->loadDucWithMoved (observationVectorDsc);
        break;

    case XDefineItem:
	rtINDEX_Define (
	    &observationVectorDsc, &ADescriptor, key
	);
	pTask->loadDucWithMoved (observationVectorDsc);
        break;

    case XDeleteItem:
	rtINDEX_Delete (&ADescriptor, key);
	break;

    case XDeleteColumn:
	rtINDEX_KeyDelete (&ADescriptor, key);

    default:
        break;
    }

    key->release ();
}


/***************************************
 *****  Key Map Type Query/Update  *****
 ***************************************/

V_DefinePrimitive (GetKeyMapType) {
    M_CPD *pStoreCPD = pTask->getCurrent ().storeCPD ();

    RTYPE_Type xKeyMapType;
    rtINDEX_GetKeyMapType (pStoreCPD, xKeyMapType);

    int result;
    switch (xKeyMapType) {
    default:
	result = 0;
	break;
    case RTYPE_C_Link:
	result = 1;
	break;
    case RTYPE_C_RefUV:
	result = 2;
	break;
    }

    pTask->loadDucWithInteger (result);
}


V_DefinePrimitive (SetKeyMapType) {
    RTYPE_Type xKeyMapType;
    switch (V_TOTSC_PrimitiveFlags) {
    default:
	xKeyMapType = RTYPE_C_Undefined;
	break;
    case 1:
	xKeyMapType = RTYPE_C_Link;
	break;
    case 2:
	xKeyMapType = RTYPE_C_RefUV;
	break;
    }

    rtINDEX_SetKeyMapType (pTask->getCurrent ().storeCPD (), xKeyMapType);

    pTask->loadDucWithSelf ();
}


/***********************************************
 *****  Default Key Map Type Query/Update  *****
 ***********************************************/

V_DefinePrimitive (GetDefaultKeyMapType) {
    RTYPE_Type xKeyMapType;
    bool fKeyMapAutoConversionEnabled;
    rtINDEX_GetDefaultKeyMapType (xKeyMapType, fKeyMapAutoConversionEnabled);

    int result;
    switch (xKeyMapType) {
    default:
	result = 0;
	break;
    case RTYPE_C_Link:
	result = 1;
	break;
    case RTYPE_C_RefUV:
	result = 2;
	break;
    }
    if (fKeyMapAutoConversionEnabled)
	result += 256;

    pTask->loadDucWithInteger (result);
}


V_DefinePrimitive (SetDefaultKeyMapType) {
    RTYPE_Type xKeyMapType;
    bool fKeyMapAutoConversionEnabled = V_TOTSC_PrimitiveFlags / 256 ? true : false;
    switch (V_TOTSC_PrimitiveFlags % 256) {
    default:
	xKeyMapType = RTYPE_C_Undefined;
	break;
    case 1:
	xKeyMapType = RTYPE_C_Link;
	break;
    case 2:
	xKeyMapType = RTYPE_C_RefUV;
	break;
    }

    rtINDEX_SetDefaultKeyMapType (xKeyMapType, fKeyMapAutoConversionEnabled);

    pTask->loadDucWithSelf ();
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

/************************************
 *****  Associative List Primitives  *****
 ************************************/

    PD (XLocateItem,
	"LocateItem",
	AccessAList)
    PD (XDefineItem,
	"DefineItem",
	AccessAList)
    PD (XDeleteItem,
	"DeleteItem",
	AccessAList)
    PD (XDeleteColumn,
	"DeleteColumn",
	AccessAList)
    PD (XNewAList,
	"NewAList",
	NewAList)
    PD (XNewAListInStore,
	"NewAListInStore",
	NewAListInStore)

    PD (XGetKeyMapType,
	"GetKeyMapType",
	GetKeyMapType)
    PD (XSetKeyMapType,
	"SetKeyMapType",
	SetKeyMapType)

    PD (XGetDefaultKeyMapType,
	"GetDefaultKeyMapType",
	GetDefaultKeyMapType)
    PD (XSetDefaultKeyMapType,
	"SetDefaultKeyMapType",
	SetDefaultKeyMapType)

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
PublicFnDef void pfALIST_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfALIST);
    FAC_FDFCase_FacilityDescription ("Associative List Primitives");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFalist.c 1 replace /users/jck/system
  870605 16:05:11 jck New primitive functions for associative lists

 ************************************************************************/
/************************************************************************
  PFalist.c 2 replace /users/jck/system
  870612 13:31:30 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  PFalist.c 3 replace /users/mjc/system
  870701 22:37:23 mjc Changed 'V_SendBinaryConverse' to 'V_SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  PFalist.c 4 replace /users/jck/system
  870810 14:12:36 jck Implemented deletion for Associative lists

 ************************************************************************/
/************************************************************************
  PFalist.c 5 replace /users/jck/system
  890117 13:22:07 jck Implemented Indexed List entire column deletion

 ************************************************************************/

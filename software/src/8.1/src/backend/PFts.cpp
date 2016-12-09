/*****  Time Series Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfTS

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

/*****  Self  *****/
#include "PFts.h"


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
 *	140 - 159                       *
 *======================================*/


/************************************
 *****  Time Series Primitives  *****
 ************************************/

#define XLocateCurrentDate			140
#define XDefineCurrentDate			141

#define XEffectiveDate				142
#define XNextEffectiveDate			143

#define XDeleteCurrentDate			144

#define XNewTimeSeries				145

#define XDeleteDateFromTimeSeries		146

#define XTimeZero				147


/********************************************
 *****  Time Series Creation Primitive  *****
 ********************************************/

V_DefinePrimitive (NewTimeSeries) {
    rtPTOKEN_Handle::Reference pInstancePToken (pTask->NewCodPToken ());
    rtINDEX_Handle *pIndex;

    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	pIndex = rtINDEX_NewTimeSeries (pInstancePToken);
        break;

    case 1:
	pIndex = rtINDEX_NewCluster (
	    pInstancePToken, &pTask->getCurrent (), NilOf (Vdd::Store*)
	);
        break;

    case 2: {
	    Vdd::Store *pStore = pTask->getSelf ().store ();
	    M_KOTE const &rDateKOTE = pStore->kot()->TheDateClass;
	    pIndex = rtINDEX_NewCluster (
		pInstancePToken,
		RTYPE_C_IntUV,
		rDateKOTE.PTokenHandle (),
		rDateKOTE.store (),
		pStore
	    );
	}
	break;

    case 3:
    /*****  ... send a converse message if appropriate, ...  *****/
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent (
		"asContentPrototypeForNewTimeSeriesWithKeyPrototype:"
	    );
	    return;
	}
	pTask->normalizeDuc ();

	pIndex = rtINDEX_NewCluster (
	    pInstancePToken, &pTask->getCurrent (), pTask->ducMonotype ().store ()
	);
	break;

    default:
	pTask->raiseException (
	    EC__UsageError,
	    "NewTimeSeries: Unsupported Control Value: %u",
	    V_TOTSC_PrimitiveFlags
	);
        break;
    }

    pTask->loadDucWithListOrStringStore (pIndex);
}


/*******************************************
 *****  Time Series Access Primitives  *****
 *******************************************/

V_DefinePrimitive (LocateCurrentDate) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->temporalContext ()->ConformsToIndex (rCurrent)) {
	DSC_Descriptor iResult;

	rtINDEX_TimeSeriesLookup (
	    &iResult, &rCurrent, pTask->temporalContext ()
	);

	pTask->loadDucWithMoved (iResult);
    }
    else pTask->sendUnaryMessageToSelf ("valueCellForNonConformantKey");
}


V_DefinePrimitive (DetermineIntervalLimit) {
    rtLINK_LookupCase xLookupCase;
    char const *pDelegationMessage;

    switch (V_TOTSC_Primitive) {
    case XEffectiveDate:
	xLookupCase = rtLINK_LookupCase_LE;
	pDelegationMessage = "intervalStartForNonConformantKey";
        break;

    case XNextEffectiveDate:
	xLookupCase = rtLINK_LookupCase_GT;
	pDelegationMessage = "intervalEndForNonConformantKey";
        break;

    default:
        pTask->raiseUnimplementedAliasException ("DetermineIntervalLimit");
        break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->temporalContext ()->ConformsToIndex (rCurrent)) {
	DSC_Descriptor iResult;

	rtINDEX_IntervalLimits (
	    &iResult, &rCurrent, pTask->temporalContext (), xLookupCase
	);

	pTask->loadDucWithMoved (iResult);
    }
    else pTask->sendUnaryMessageToSelf (pDelegationMessage);
}


V_DefinePrimitive (TimeZero) {
    DSC_Descriptor iResult;

    static_cast<rtINDEX_Handle*>(pTask->getCurrent ().store ())->getTimeZero (
	iResult, pTask->ptoken ()
    );

    pTask->loadDucWithMoved (iResult);
}


/*******************************************
 *****  Time Series Update Primitives  *****
 *******************************************/

V_DefinePrimitive (DefineCurrentDate) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->temporalContext ()->ConformsToIndex (rCurrent)) {
	DSC_Descriptor iResult;

	rtINDEX_Define (
	    &iResult, &rCurrent, pTask->temporalContext ()
	);

	pTask->loadDucWithMoved (iResult);
    }
    else pTask->sendUnaryMessageToSelf ("newValueCellForNonConformantKey");
}

V_DefinePrimitive (DeleteCurrentDate) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->temporalContext ()->ConformsToIndex (rCurrent)) {
	rtINDEX_Delete (&rCurrent, pTask->temporalContext ());
	pTask->loadDucWithCopied (rCurrent);
    }
    else pTask->sendUnaryMessageToSelf ("deletePointForNonConformantKey");
}

V_DefinePrimitive (DeleteDateFromTimeSeries) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->temporalContext ()->ConformsToIndex (rCurrent)) {
	rtINDEX_KeyDelete (&rCurrent, pTask->temporalContext ());
	pTask->loadDucWithCopied (rCurrent);
    }
    else pTask->sendUnaryMessageToSelf ("deleteColumnForNonConformantKey");
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
 *****  Time Series Primitives  *****
 ************************************/

    PD (XNewTimeSeries,
	"NewTimeSeries",
	NewTimeSeries)

    PD (XLocateCurrentDate,
	"LocateCurrentDate",
	LocateCurrentDate)

    PD (XEffectiveDate,
	"EffectiveDate",
	DetermineIntervalLimit)
    PD (XNextEffectiveDate,
	"NextEffectiveDate",
	DetermineIntervalLimit)

    PD (XTimeZero,
	"TimeZero",
	TimeZero)

    PD (XDefineCurrentDate,
	"DefineCurrentDate",
	DefineCurrentDate)

    PD (XDeleteCurrentDate,
	"DeleteCurrentDate",
	DeleteCurrentDate)
    PD (XDeleteDateFromTimeSeries,
	"DeleteDateFromTimeSeries",
	DeleteDateFromTimeSeries)

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
PublicFnDef void pfTS_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfTS);
    FAC_FDFCase_FacilityDescription ("Time Series Primitives");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFts.c 1 replace /users/jad/system
  861116 16:06:09 jad new time series primitive functions

 ************************************************************************/
/************************************************************************
  PFts.c 2 replace /users/jck/system
  870420 13:34:47 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  PFts.c 3 replace /users/jck/system
  870420 14:39:55 jck  Fixed bug in the 'new' primitive

 ************************************************************************/
/************************************************************************
  PFts.c 4 replace /users/jck/system
  870511 15:28:59 jck Added Interval limit determination primitives

 ************************************************************************/
/************************************************************************
  PFts.c 5 replace /users/jck/system
  870513 11:37:56 jck Corrected implementation of 'nextDate:'

 ************************************************************************/
/************************************************************************
  PFts.c 6 replace /users/mjc/translation
  870524 09:31:11 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFts.c 7 replace /users/jck/system
  870605 16:08:04 jck Generalized interface to RTindex

 ************************************************************************/
/************************************************************************
  PFts.c 8 replace /users/jck/system
  890117 13:25:04 jck Implemented Indexed List entire column deletion

 ************************************************************************/

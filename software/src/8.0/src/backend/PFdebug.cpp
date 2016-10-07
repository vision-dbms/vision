/*****  Vision Debugger Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfDEBUG

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"

/*****  Self  *****/
#include "PFdebug.h"


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
 *	  630 - 63?                     *
 *======================================*/

#define XControlBreakpoint		630

#define XGetTask			635


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

/********************************
 *****  Breakpoint Control  *****
 ********************************/

V_DefinePrimitive (ControlBreakpoint) {
    DSC_Descriptor& rSelf = pTask->getSelf ();

    unsigned int iOrigMask = rSelf.storeMask ();
    unsigned int iKeepMask = V_TOTSC_PrimitiveFlags & 07;
    unsigned int iSetMask  = V_TOTSC_PrimitiveFlags >> 3 & 07;

    rSelf.setStoreAttentionMaskTo (iOrigMask & iKeepMask | iSetMask);

    pTask->loadDucWithInteger (iOrigMask);
}


/*************************
 *****  Task Access  *****
 *************************/

V_DefinePrimitive (GetTask) {
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	pTask->loadDucWith (pTask);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("GetTask");
	break;
    }
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

    PD (XControlBreakpoint,
	"ControlBreakpoint",
	ControlBreakpoint)

    PD (XGetTask,
	"GetTask",
	GetTask)

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
PublicFnDef void pfDEBUG_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (pfDEBUG);
    FAC_FDFCase_FacilityDescription ("Vision Debugger Primitive Function Services");
    default:
        break;
    }
}

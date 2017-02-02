/*****  Persistent Container Table Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtPCT

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"

/*****  Self  *****/
#include "RTpct.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtPCT_Handle);


/**************************************
 *****  Standard CPD Initializer  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize an instance.
 *
 *  Argument:
 *	cpd			- the address of the standard CPD to be
 *				  initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD* cpd) {
    rtPCT_CPD_Entry (cpd) = rtPCT_CPD_Entries (cpd);
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/*********************
 *  Print Functions  *
 *********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a scratch pad container table entry.
 *
 *  Arguments:
 *	cpd			- the address of a standard CPD pointing to the
 *				  entry to be displayed.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void PrintUserCTEntry (M_CPD* cpd) {
    PS_CTE *cte = rtPCT_CPD_Entry (cpd);
    int			cteIndex = cte - rtPCT_CPD_Entries (cpd),
			st = PS_CTE_Segment (*cte),
			ot = PS_CTE_Offset  (*cte);

    if (!PS_CTE_IsFree  (*cte))
	IO_printf ("I:%6d, SG:%5d, OF:%6d\n", cteIndex, st, ot);
    else
	IO_printf ("I:%6d, NF:%d\n", cteIndex, PS_CTE_NextFree (*cte));
}


/*---------------------------------------------------------------------------
 *****  Routine to display a scratch pad container table.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the UserCT to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintUserCT (M_CPD* cpd, int recursive) {
    int	i, n;

    IO_printf ("UserOS{ %u\n", rtPCT_CPD_SpaceIndex (cpd));
    for (i = 0, n = rtPCT_CPD_EntryCount (cpd);
    	 i < n;
	 i++, rtPCT_CPD_Entry (cpd)++)
	if (recursive || !PS_CTE_IsFree (*rtPCT_CPD_Entry (cpd)))
	    PrintUserCTEntry (cpd);
    IO_printf ("}");
}


/***************************************************
 *****  Internal Debugger Method Dictionaries  *****
 ***************************************************/

/********************
 *  'Type' Methods  *
 ********************/

/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (VisitEntries) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);
    M_ASD *pSpace = cpd->Space ();

    PS_CT *pCT = rtPCT_CPD_Base (cpd);
    unsigned int xLimit = PS_CT_EntryCount (pCT);

    for (unsigned int x = 1; x < xLimit; x++)
        if (!PS_CTE_IsFree (PS_CT_Entry (pCT, x)))
	    pSpace->GetCPD (x)->release ();

    return self;
}

IOBJ_DefineUnaryMethod (PrintFreeList) {
    M_CPD*		cpd = RTYPE_QRegisterCPD (self);
    PS_CT*		pCT = rtPCT_CPD_Base (cpd);
    int			f = PS_CT_FreeList (pCT);

    for (;
	 IO_printf ("  %d  ", f), f != -1;
	 f = PS_CTE_NextFree (PS_CT_Entries (pCT)[f]));

    return self;
}

IOBJ_DefineUnaryMethod (PrintMisc) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);
    PS_CT* pCT = rtPCT_CPD_Base (cpd);
    int freeListCount, f = PS_CT_FreeList (pCT);

    for (freeListCount = 0;
	 f != -1;
	 freeListCount++, f = PS_CTE_NextFree (PS_CT_Entries (pCT)[f]));

    IO_printf ("FL: %d, FLC: %u, SI: %u, EC: %u Entries: %08X\n",
	PS_CT_FreeList (pCT), freeListCount, PS_CT_SpaceIndex (pCT),
        PS_CT_EntryCount (pCT), PS_CT_Entries (pCT));

    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtPCT_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("quickPrint"	    , RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	    , RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	IOBJ_MDE ("qprint"	    , RTYPE_QPrintIObject)
	IOBJ_MDE ("print"	    , RTYPE_PrintIObject)
	IOBJ_MDE ("rprint"	    , RTYPE_RPrintIObject)
	IOBJ_MDE ("visitEntries"    , VisitEntries)
	IOBJ_MDE ("prFree"	    , PrintFreeList)
	IOBJ_MDE ("pmisc"	    , PrintMisc)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtPCT_CPD_StandardPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtPCT_Handle::Maker;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
        PrintUserCT (iArgList.arg<M_CPD*>(), false);
	break;
    case RTYPE_RPrintObject:
        PrintUserCT (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtPCT);
    FAC_FDFCase_FacilityDescription ("Persistent Container Table Representation Type Handler");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTpct.c 1 replace /users/mjc/system
  870215 22:56:35 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  RTpct.c 2 replace /users/jck/system
  870415 09:53:34 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTpct.c 3 replace /users/jck/system
  870420 13:31:14 jck Added a debugger method to visit all the containers in a container table

 ************************************************************************/
/************************************************************************
  RTpct.c 4 replace /users/mjc/translation
  870524 09:34:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTpct.c 5 replace /users/jck/system
  871007 13:40:33 jck Augmented the 'pmisc' debugger method

 ************************************************************************/

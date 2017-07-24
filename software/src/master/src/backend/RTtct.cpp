/*****  Transient Container Table Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtTCT

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
#include "RTtct.h"


/*****************************
 *****************************
 *****  Container Table  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 *  Transient Container Table Field Descriptions:
 *	m_cEntries		- the total number of entries in the container
 *				  table.
 *	m_xFreeList		- the index of the first free entry in the
 *				  container table entry free list.
 *	m_iEntries		- the array of container table entries.
 *---------------------------------------------------------------------------
 */
class M_CT {
//  Access
public:
    unsigned int entryCount () const {
	return m_cEntries;
    }
    M_DCTE *entries () {
	return m_iEntries;
    }

private:
    unsigned int	m_cEntries;
public:
    int			m_xFreeList;
    M_DCTE		m_iEntries[1];
};


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

DEFINE_CONCRETE_RTT (rtTCT_Handle);


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard transient container table CPD.
 *
 *  Argument:
 *	cpd			- the address of the standard CPD
 *				  to initialize.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *cpd) {
    rtTCT_CPD_Entry (cpd) = rtTCT_CPD_Entries (cpd);
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
PrivateFnDef void PrintEntry (M_CPD* Unused(cpd), int Unused(full)) {
#if 0
    M_DCTE *cte = rtTCT_CPD_Entry (cpd);
    char
	FPorRTYPE [32];
    int
	forwarding = rtTCT_Entry_ForwardingPointer (cte),
	cteIndex = cte - rtTCT_CPD_Entries (cpd),
	rc = rtTCT_Entry_ReferenceCount (cte),
	cpcc = rtTCT_Entry_ConnectedPointers (cte);

    if (rc > 0 || forwarding) {
	M_CPreamble* cp = rtTCT_Entry_MemoryAddress (cte);
	if (forwarding) sprintf (
	    FPorRTYPE, "FP:[%d:%d],\n",
	    rtTCT_Entry_POPObjectSpace (cte),
	    rtTCT_Entry_POPContainerIndex (cte)
	);
	else sprintf (
	    FPorRTYPE, "RT:  %-18.18s[%2d],\n",
	    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPreamble_RType (cp)),
	    M_CPreamble_RType (cp)
	);
        IO_printf (
	    "I:%6d, RC:%5d, CPCC:%5d, MA:%08X, %s         IRC:%5d, S:%8d, FC:%d\n",
	     cteIndex,
	     rc,
	     cpcc,
	     cp,
	     FPorRTYPE,
	     rtTCT_Entry_IRC (cte),
	     forwarding ? (-1) :M_CPreamble_Size (cp),
	     rtTCT_Entry_FreeContainer (cte)
	);
	_M_DisplayCPCCCPDChain (cpcc);
    }
    else if (full) IO_printf (
	"I:%6d, NF:%d\n", cteIndex, rtTCT_Entry_NextFree (cte)
    );
#endif
}


/*---------------------------------------------------------------------------
 *****  Routine to display a scratch pad container table.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the container table to be
 *				  printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintCT (
    M_CPD*			cpd,
    int				full
)
{
    int	i, n;

    IO_printf ("#%s{\n", cpd->RTypeName ());
    for (i = 0, n = rtTCT_CPD_EntryCount (cpd);
	 i < n;
	 i++, rtTCT_CPD_Entry (cpd)++)
	PrintEntry (cpd, full);
    IO_printf ("}");
}


/***************************************************
 *****  Internal Debugger Method Dictionaries  *****
 ***************************************************/

/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (CTSizeDM) {
    return IOBJ_IntIObject (rtTCT_CPD_EntryCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (PrintCTEDM) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);
    PrintEntry (cpd, true);
    return self;
}

IOBJ_DefineUnaryMethod (CTEBrowserDM) {
    M_CPD *ct = RTYPE_QRegisterCPD (self);
    M_CPD* result = ct->Space ()->GetCPD (rtTCT_CPD_Entry (ct) - rtTCT_CPD_Entries (ct));
    return result ? RTYPE_QRegister (result) : IOBJ_TheNilIObject;
}


IOBJ_DefineUnaryMethod (CTEPositionDM) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);
    return IOBJ_IntIObject (rtTCT_CPD_Entry (cpd) - rtTCT_CPD_Entries (cpd));
}

IOBJ_DefineUnaryMethod (NextCTEDM) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    if (++rtTCT_CPD_Entry (cpd) >= rtTCT_CPD_Entries (cpd) + rtTCT_CPD_EntryCount (cpd)) {
	IO_printf (">>>  End of Table  <<<\n");
	rtTCT_CPD_Entry (cpd)--;
    }

    return self;
}

IOBJ_DefineUnaryMethod (PreviousCTEDM) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    if (rtTCT_CPD_Entry (cpd) <= rtTCT_CPD_Entries (cpd)) {
	IO_printf (">>>  Beginning of Table  <<<\n");
	rtTCT_CPD_Entry (cpd) = rtTCT_CPD_Entries (cpd);
    }
    else rtTCT_CPD_Entry (cpd)--;

    return self;
}

IOBJ_DefineMethod (GoToCTEDM) {
    M_CPD*	cpd = RTYPE_QRegisterCPD (self);
    unsigned int i = IOBJ_NumericIObjectValue (parameterArray[0], unsigned int);

    if (i < rtTCT_CPD_EntryCount (cpd))
	rtTCT_CPD_Entry (cpd) = rtTCT_CPD_Entries (cpd) + i;

    return self;
}


IOBJ_DefineUnaryMethod (VisitEntriesDM) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);
    cpd->Space ()->EnumerateContainers (&VContainerHandle::DoNothing);
    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtTCT_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"	, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	, RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
    	IOBJ_MDE ("qprint"	, RTYPE_QPrintIObject)
	IOBJ_MDE ("print"	, RTYPE_PrintIObject)
	IOBJ_MDE ("rprint"	, RTYPE_RPrintIObject)
	IOBJ_MDE ("size"	, CTSizeDM)
	IOBJ_MDE ("eprint"	, PrintCTEDM)
	IOBJ_MDE ("position"	, CTEPositionDM)
	IOBJ_MDE ("browser"	, CTEBrowserDM)
	IOBJ_MDE ("next"	, NextCTEDM)
	IOBJ_MDE ("previous"	, PreviousCTEDM)
	IOBJ_MDE ("goTo:"	, GoToCTEDM)
	IOBJ_MDE ("visitEntries", VisitEntriesDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtTCT_CPD_StandardPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtTCT_Handle::Maker;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	PrintCT (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	PrintCT (iArgList.arg<M_CPD*>(), true);
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
    FAC_FDFCase_FacilityIdAsString (rtTCT);
    FAC_FDFCase_FacilityDescription (
	"Transient Container Table Representation Type Handler"
    );
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTtct.c 1 replace /users/mjc/system
  870215 22:56:42 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  RTtct.c 2 replace /users/jck/system
  870415 09:53:44 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTtct.c 3 replace /users/mjc/translation
  870524 09:35:32 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

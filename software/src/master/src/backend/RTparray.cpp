/*****  Physical Object Pointer Array Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtPARRAY

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
#include "RTparray.h"


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

DEFINE_CONCRETE_RTT (rtPARRAY_Handle);


/********************************************
 *****  P-Array Instantiation Routines  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new P-Array.
 *
 *  Argument:
 *	elementCount		- the number of physical object pointers
 *				  this P-Array is to have.
 *
 *  Returns:
 *	A CPD for the array created.
 *
 *****/
PublicFnDef M_CPD *rtPARRAY_New (M_ASD *pContainerSpace, unsigned int elementCount) {
    M_CPD *cpd = pContainerSpace->CreateContainer (
	RTYPE_C_PArray, elementCount * sizeof (M_POP)
    )->NewCPD ();

    rtPARRAY_CPD_Element (cpd) = rtPARRAY_CPD_ElementArray (cpd);

    cpd->constructReferences (rtPARRAY_CPx_Element, rtPARRAY_CPD_ElementCount (cpd));

    return cpd;
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/*************
 *  Printer  *
 *************/

/*---------------------------------------------------------------------------
 *****  Routine to display a P-Array.
 *
 *  Arguments:
 *	cpd			- the address of a CPD for the P-Array to be
 *				  printed.
 *	recursive		- a boolean which, when true, requests the
 *				  recursive display of the elements of the
 *				  P-Array.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void PrintPArray (M_CPD *cpd, int recursive) {
    unsigned  int i, n;

    IO_printf ("%s{", RTYPE_TypeIdAsString (RTYPE_C_PArray));
    for (i = 0, n = rtPARRAY_CPD_ElementCount (cpd);
	 i < n;
	 i++, rtPARRAY_CPD_Element (cpd)++
    ) {
	IO_printf ("\n    ");
	if (recursive)
	    RTYPE_RPrint (cpd, rtPARRAY_CPx_Element);
	else
	    RTYPE_Print (cpd, rtPARRAY_CPx_Element);
    }
    IO_printf ("\n}\n");
}


/***************************
 *  Container Reclamation  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PrivateFnDef void ReclaimContainer (
    M_ASD *ownerASD, M_CPreamble const *preambleAddress
) {
    ownerASD->Release (
	(M_POP const *)(preambleAddress + 1),
	M_CPreamble_Size (preambleAddress) / sizeof (M_POP)
    );
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'save' a P-Array
 *****/
bool rtPARRAY_Handle::PersistReferences () {
    return Persist ((M_POP*)containerContent (), containerSize () / sizeof (M_POP));
}


/*********************************************
 *****  Standard PArray Marking Routine  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'mark' containers referenced by a P-Array
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD* pSpace, M_CPreamble const *pContainer) {
    pGCV->Mark (
	pSpace, (M_POP const*)(pContainer + 1), (M_CPreamble_Size (pContainer) / sizeof (M_POP))
    );
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (New_DM) {
    return RTYPE_QRegister (
	rtPARRAY_New (IOBJ_ScratchPad, IOBJ_NumericIObjectValue (parameterArray[0], int))
    );
}

IOBJ_DefineNewaryMethod (NewInitializedTo_DM) {
    int		i, n = IOBJ_NumericIObjectValue (parameterArray[0], int);
    M_CPD*	source = RTYPE_QRegisterCPD (parameterArray[1]);
    M_CPD*	result = rtPARRAY_New (IOBJ_ScratchPad, n);

    for (i = 0; i < n; i++, rtPARRAY_CPD_Element (result)++)
        result->StoreReference (rtPARRAY_CPx_Element, source);

    rtPARRAY_CPD_Element (result) = rtPARRAY_CPD_ElementArray (result);

    return RTYPE_QRegister (result);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (QPrintPAElement)
{
    RTYPE_QPrint (RTYPE_QRegisterCPD (self), rtPARRAY_CPx_Element);
    return self;
}

IOBJ_DefineUnaryMethod (PrintPAElement)
{
    RTYPE_Print (RTYPE_QRegisterCPD (self), rtPARRAY_CPx_Element);
    return self;
}

IOBJ_DefineUnaryMethod (RPrintPAElement)
{
    RTYPE_RPrint (RTYPE_QRegisterCPD (self), rtPARRAY_CPx_Element);
    return self;
}


IOBJ_DefineUnaryMethod (PASize)
{
    M_CPD*
	cpd = RTYPE_QRegisterCPD (self);

    return IOBJ_IntIObject (rtPARRAY_CPD_ElementCount (cpd));
}

IOBJ_DefineUnaryMethod (CurrentPosition) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);

    return IOBJ_IntIObject (rtPARRAY_CPD_Element (cpd) - rtPARRAY_CPD_ElementArray (cpd));
}

IOBJ_DefineUnaryMethod (AtCurrent) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtPARRAY_CPx_Element);
}

IOBJ_DefineMethod (AtCurrentPut) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtPARRAY_CPx_Element, RTYPE_QRegisterCPD (parameterArray[0])
    );

    return self;
}


IOBJ_DefineUnaryMethod (NextPAElement) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);

    if (++rtPARRAY_CPD_Element (cpd) >=
        rtPARRAY_CPD_ElementArray (cpd) + rtPARRAY_CPD_ElementCount (cpd))
    {
	IO_printf (">>>  End of Vector  <<<\n");
	rtPARRAY_CPD_Element (cpd)--;
    }

    return self;
}

IOBJ_DefineUnaryMethod (PreviousPAElement) {
    M_CPD *cpd = RTYPE_QRegisterCPD (self);

    if (rtPARRAY_CPD_Element (cpd) <= rtPARRAY_CPD_ElementArray (cpd)) {
	IO_printf (">>>  Beginning of Vector  <<<\n");
	rtPARRAY_CPD_Element (cpd) = rtPARRAY_CPD_ElementArray (cpd);
    }
    else
	rtPARRAY_CPD_Element (cpd)--;

    return self;
}

IOBJ_DefineMethod (GoToPAElement)
{
    M_CPD*		cpd = RTYPE_QRegisterCPD (self);
    unsigned int	i = IOBJ_NumericIObjectValue (parameterArray[0], unsigned int);

    if (i < rtPARRAY_CPD_ElementCount (cpd))
	rtPARRAY_CPD_Element (cpd) = rtPARRAY_CPD_ElementArray (cpd) + i;

    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtPARRAY_Handler) {
    M_RTD* rtd;

    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"		, New_DM)
	IOBJ_MDE ("new:initializedTo:"	, NewInitializedTo_DM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
    	IOBJ_MDE ("qprint"		, RTYPE_QPrintIObject)
	IOBJ_MDE ("print"		, RTYPE_PrintIObject)
	IOBJ_MDE ("rprint"		, RTYPE_RPrintIObject)
	IOBJ_MDE ("eqprint"		, QPrintPAElement)
	IOBJ_MDE ("eprint"		, PrintPAElement)
	IOBJ_MDE ("erprint"		, RPrintPAElement)
	IOBJ_MDE ("size"		, PASize)
	IOBJ_MDE ("at"			, AtCurrent)
	IOBJ_MDE ("put:"		, AtCurrentPut)
	IOBJ_MDE ("position"		, CurrentPosition)
	IOBJ_MDE ("next"		, NextPAElement)
	IOBJ_MDE ("previous"		, PreviousPAElement)
	IOBJ_MDE ("goTo:"		, GoToPAElement)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData:
        rtd = iArgList.arg<M_RTD*>();
	M_RTD_HandleMaker	(rtd) = &rtPARRAY_Handle::Maker;
	M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	M_RTD_MarkFn		(rtd) = MarkContainers;
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	PrintPArray (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	PrintPArray (iArgList.arg<M_CPD*>(), true);
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
    FAC_FDFCase_FacilityIdAsString (rtPARRAY);
    FAC_FDFCase_FacilityDescription ("Physical Object Pointer Array R-Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTparray.c 1 replace /users/mjc/system
  860323 17:20:16 mjc Physical object pointer array R-Type

 ************************************************************************/
/************************************************************************
  RTparray.c 2 replace /users/mjc/system
  860404 21:54:43 mjc Deleted : from unary messages

 ************************************************************************/
/************************************************************************
  RTparray.c 3 replace /users/mjc/system
  860410 19:22:49 mjc Added print services

 ************************************************************************/
/************************************************************************
  RTparray.c 4 replace /users/mjc/system
  860412 18:05:11 mjc Changed name from 'RTpvector'

 ************************************************************************/
/************************************************************************
  RTparray.c 5 replace /users/mjc/system
  860414 12:30:38 mjc Eliminate use of POP Cache

 ************************************************************************/
/************************************************************************
  RTparray.c 6 replace /users/mjc/system
  860414 15:31:22 mjc Changed print methods

 ************************************************************************/
/************************************************************************
  RTparray.c 7 replace /users/hjb/system
  860423 00:39:26 hjb done updating calls

 ************************************************************************/
/************************************************************************
  RTparray.c 8 replace /users/mjc/system
  860423 11:05:15 mjc Eliminated special handling for Nil POP's

 ************************************************************************/
/************************************************************************
  RTparray.c 9 replace /users/mjc/system
  860506 17:35:48 mjc Added methods useful for timing the memory manager

 ************************************************************************/
/************************************************************************
  RTparray.c 10 replace /users/hjb/system
  860517 17:56:41 hjb added save to user space function

 ************************************************************************/
/************************************************************************
  RTparray.c 11 replace /users/mjc/system
  860617 15:10:52 mjc Moved and documented container save function

 ************************************************************************/
/************************************************************************
  RTparray.c 12 replace /users/mjc/system
  860627 13:56:45 mjc Release new CPD definitions

 ************************************************************************/
/************************************************************************
  RTparray.c 13 replace /users/mjc/system
  860817 20:15:45 mjc Converted CPD initializer/instantiator/gc routines to expect nothing of new CPDs

 ************************************************************************/
/************************************************************************
  RTparray.c 14 replace /users/jad/system
  860914 11:14:36 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  RTparray.c 15 replace /users/mjc/translation
  870524 09:34:14 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTparray.c 16 replace /users/jck/system
  871007 13:37:56 jck Added a marking function for the global garbage collector

 ************************************************************************/

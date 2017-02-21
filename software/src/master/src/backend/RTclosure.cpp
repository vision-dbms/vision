/*****  Closure Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtCLOSURE

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTblock.h"
#include "RTcontext.h"

/*****  Self  *****/
#include "RTclosure.h"


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

DEFINE_CONCRETE_RTT (rtCLOSURE_Handle);


/***********************************
 ***********************************
 *****                         *****
 *****  rtCLOSURE_Constructor  *****
 *****                         *****
 ***********************************
 ***********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtCLOSURE_Constructor);

/********************************
 ********************************
 *****  CPD Initialization  *****
 ********************************
 ********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a standard closure CPD.
 *
 *  Argument:
 *	cpd			- the CPD to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
void rtCLOSURE_Constructor::InitStdCPD (M_CPD* cpd) {
    rtCLOSURE_Closure* closure = rtCLOSURE_CPD_Base (cpd);

    rtCLOSURE_CPD_Context	(cpd) = &rtCLOSURE_Closure_Context (closure);
    rtCLOSURE_CPD_Block		(cpd) = &rtCLOSURE_Closure_Block   (closure);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*---------------------------------------------------------------------------
 *****  Closure constructor constructor.
 *
 *  Arguments:
 *	pContext		- the context associated with this closure.
 *				  One of the outstanding references to this
 *				  context is claimed by this object.
 *	pBlock			- a block handle for block closures.  One of
 *				  the outstanding references to this block
 *				  handle is claimed by this object.
 *				  have an additional reference which will be
 *				  claimed by this object.
 *	xPrimitive		- the index of a primitive for primitive closures.
 *
 *****/
rtCLOSURE_Constructor::rtCLOSURE_Constructor (
    rtCONTEXT_Constructor *pContext, rtBLOCK_Handle *pBlock, unsigned int iAttentionMask
) : VConstructor (iAttentionMask), m_pBlock (pBlock), m_xPrimitive (0), m_pContext (pContext) {
    pContext->release ();
}

rtCLOSURE_Constructor::rtCLOSURE_Constructor (
    rtCONTEXT_Constructor *pContext, unsigned int xPrimitive, unsigned int iAttentionMask
) : VConstructor (iAttentionMask), m_xPrimitive (xPrimitive), m_pContext (pContext) {
    pContext->release ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

rtCLOSURE_Constructor::~rtCLOSURE_Constructor () {
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

RTYPE_Type rtCLOSURE_Constructor::RType_() const {
    return RTYPE_C_Closure;
}

M_ASD *rtCLOSURE_Constructor::Space_() const {
    return m_pContext->Space_();
}


/*************************
 *************************
 *****  Realization  *****
 *************************
 *************************/

/*---------------------------------------------------------------------------
 *****  Routine to realize a closure constructor.
 *
 *  Returns:
 *	A standard CPD for the realized closure.
 *
 *****/
M_CPD *rtCLOSURE_Constructor::newRealization () {
/*****  Create the realized closure...  *****/
    M_CPD *result = ScratchPad_()->CreateContainer (
	RTYPE_C_Closure, sizeof (rtCLOSURE_Closure)
    );

    InitStdCPD (result);

    result->constructReference (rtCLOSURE_CPx_Context);
    result->constructReference (rtCLOSURE_CPx_Block);

/*****  ... realize and record the closure's context, ...  *****/
    result->StoreReference (rtCLOSURE_CPx_Context, m_pContext->realization ());

/*****  ... decode the function, ...  *****/
    if (isABlockClosure ())
	result->StoreReference (rtCLOSURE_CPx_Block, m_pBlock);
    else
        rtCLOSURE_CPD_Primitive (result) = m_xPrimitive;

/*****  ... and return the realized closure.  *****/
    return result;
}


/*********************************
 *********************************
 *****                       *****
 *****  rtCLOSURE Container  *****
 *****                       *****
 *********************************
 *********************************/

/***************************************
 *****  Closure Alignment Routine  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a closure.
 *
 *  Arguments:
 *	closure			- a standard CPD for the closure to be
 *				  aligned.
 *
 *  Returns:
 *	true if the closure required alignment. false if not.
 *
 *****/
PublicFnDef bool rtCLOSURE_Align (M_CPD* closure) {
/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtCLOSURE_Align", M_CPD_RType (closure), RTYPE_C_Closure);

    M_CPD* context = rtCLOSURE_CPD_ContextCPD (closure);

    bool result = rtCONTEXT_Align (context);

    context->release ();

    if (rtCLOSURE_CPD_IsABlockClosure (closure)) {
	M_CPD* block = rtCLOSURE_CPD_BlockCPD (closure);
	result |= rtBLOCK_Align (block);
	block->release ();
    }

    return result;
}


/********************************************************************
 *****  Standard Representation Type Services Handler Routines  *****
 ********************************************************************/

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
    rtCLOSURE_Closure const *closure = (rtCLOSURE_Closure const*)M_CPreamble_ContainerBase (
	preambleAddress
    );

    ownerASD->Release (&rtCLOSURE_Closure_Context (closure));
    ownerASD->Release (&rtCLOSURE_Closure_Block   (closure));
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'save' a Closure.
 *****/
bool rtCLOSURE_Handle::PersistReferences () {
    rtCLOSURE_Closure *closure = (rtCLOSURE_Closure*)ContainerContent ();

    return Persist (&rtCLOSURE_Closure_Context (closure))
	&& Persist (&rtCLOSURE_Closure_Block   (closure));
}


/**********************************************
 *****  Standard Closure Marking Routine  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'mark' containers referenced by a Closure.
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
PrivateFnDef void MarkContainers (M_ASD* pSpace, M_CPreamble const *pContainer) {
    pSpace->Mark ((M_POP const*) M_CPreamble_ContainerBase (pContainer), 2);
}


/*************
 *  Printer  *
 *************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a closure.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the closure to be printed.
 *	full			- a boolean which, when true, requests
 *				  recursive display of the closure.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintClosure (M_CPD* cpd, int full) {
    IO_printf ("#%s{\nFunction: ", RTYPE_TypeIdAsString (RTYPE_C_Closure));
    if (rtCLOSURE_CPD_IsABlockClosure (cpd)) {
	RTYPE_Print (cpd, rtCLOSURE_CPx_Block);
    }
    else IO_printf (
	"<%s>", PRIMFNS_PrimitiveName (rtCLOSURE_CPD_Primitive (cpd))
    );

    IO_printf ("\nContext:  ");
    if (full)
        RTYPE_Print (cpd, rtCLOSURE_CPx_Context);
    else
	RTYPE_QPrint (cpd, rtCLOSURE_CPx_Context);

    IO_printf ("}");
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

/************************
 *  'Instance' Methods  *
 ************************/


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtCLOSURE_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtCLOSURE_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = &rtCLOSURE_Constructor::InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtCLOSURE_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD*>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD*>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	PrintClosure (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	PrintClosure (iArgList.arg<M_CPD*>(), true);
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
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtCLOSURE);
    FAC_FDFCase_FacilityDescription
        ("Closure Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTclosure.c 1 replace /users/mjc/system
  861002 17:55:41 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTclosure.c 2 replace /users/mjc/system
  861028 22:54:27 mjc Fixed non-reclaimation of constructor storage

 ************************************************************************/
/************************************************************************
  RTclosure.c 3 replace /users/mjc/system
  870204 17:16:19 mjc Add '#include saver.h' statement

 ************************************************************************/
/************************************************************************
  RTclosure.c 4 replace /users/jck/system
  870415 09:53:06 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTclosure.c 5 replace /users/mjc/translation
  870524 09:31:55 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTclosure.c 6 replace /users/jck/system
  871007 13:34:04 jck Added a marking function for the global garbage collector

 ************************************************************************/

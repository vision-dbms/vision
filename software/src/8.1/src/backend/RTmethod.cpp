/*****  Method Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtMETHOD

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "popvector.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTblock.h"
#include "RTvstore.h"

/*****  Self  *****/
#include "RTmethod.h"


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

DEFINE_CONCRETE_RTT (rtMETHOD_Handle);

/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool rtMETHOD_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.setTo (static_cast<rtVSTORE_Handle*>(KOT ()->TheMethodClass.ObjectHandle ()));
    return true;
}


/**************************
 *****  Instantiator  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new 'Method' object.
 *
 *  Arguments:
 *	pContainerSpace		- the space in which to create the container.
 *	pBlock			- a reference to a POP for the block to be
 *				  installed in this method.
 *	pMy			- a CPD for the store which will become ^my
 *				  when this method is invoked.
 *
 *  Returns:
 *	A CPD for the method created.
 *
 *****/
rtMETHOD_Handle::rtMETHOD_Handle (
    M_ASD *pContainerSpace, rtBLOCK_Handle *pBlock, Vdd::Store *pMy
) : BaseClass (pContainerSpace, RTYPE_C_Method, rtMETHOD_Method_POPCount) {
    StoreReference (rtMETHOD_CPx_Block, static_cast<VContainerHandle*>(pBlock));
    StoreReference (rtMETHOD_CPx_Origin, pMy);
}


/*---------------------------------------------------------------------------
 *****  Routine to align a method
 *
 *  Returns:
 *	true if an alignment was required, false otherwise
 *
 *****/
bool rtMETHOD_Handle::align () {
    rtBLOCK_Handle::Reference pBlock;
    getBlock (pBlock);
    return pBlock->align ();
}


/********************************************************************
 *****  Standard Representation Type Services Handler Routines  *****
 ********************************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a short version of a method.
 *
 *  Argument:
 *	method			- a standard CPD for the method to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintMethod (M_CPD *method) {
    IO_printf ("#%s{\nBlock: ", RTYPE_TypeIdAsString (RTYPE_C_Method));
    RTYPE_Print (method, rtMETHOD_CPx_Block);
    IO_printf ("\nOrigin: ");
    RTYPE_QPrint (method, rtMETHOD_CPx_Origin);
    IO_printf ("}");
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewWithOriginDM) {
    VContainerHandle *pBlock = RTYPE_QRegisterHandle (parameterArray[0]);
    RTYPE_MustBeA ("'new:withOrigin:'", pBlock->RType (), RTYPE_C_Block);

    Vdd::Store::Reference pMyStore (RTYPE_QRegisterHandle (parameterArray[1])->getStore ());
    rtMETHOD_Handle::Reference pMethod (
	new rtMETHOD_Handle (IOBJ_ScratchPad, static_cast<rtBLOCK_Handle*>(pBlock), pMyStore)
    );
    return RTYPE_QRegister (pMethod);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (BlockDM) {
    return RTYPE_QRegister (RTYPE_QRegisterCPD (self)->GetCPD (rtMETHOD_CPx_Block, RTYPE_C_Block));
}

IOBJ_DefineUnaryMethod (OriginDM) {
    return RTYPE_QRegister (RTYPE_QRegisterCPD (self)->GetCPD (rtMETHOD_CPx_Origin));
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtMETHOD_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:withOrigin:"	, NewWithOriginDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("block"		, BlockDM)
	IOBJ_MDE ("origin"		, OriginDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtMETHOD_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = POPVECTOR_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtMETHOD_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = POPVECTOR_ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = POPVECTOR_MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
        PrintMethod (iArgList.arg<M_CPD*>());
        break;
    case RTYPE_RPrintObject:
        POPVECTOR_Print (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtMETHOD);
    FAC_FDFCase_FacilityDescription
        ("Method Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTmethod.c 1 replace /users/mjc/system
  861002 17:55:26 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTmethod.c 2 replace /users/mjc/translation
  870524 09:33:49 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTmethod.c 3 replace /users/jck/system
  871007 13:33:45 jck Added a marking function for the global garbage collector

 ************************************************************************/

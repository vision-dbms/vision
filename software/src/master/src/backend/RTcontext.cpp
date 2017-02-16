/*****  Context Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtCONTEXT

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "popvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTdsc.h"

/*****  Self  *****/
#include "RTcontext.h"


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

DEFINE_CONCRETE_RTT (rtCONTEXT_Handle);


/***********************************
 ***********************************
 *****                         *****
 *****  rtCONTEXT_Constructor  *****
 *****                         *****
 ***********************************
 ***********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtCONTEXT_Constructor);


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

rtCONTEXT_Constructor::rtCONTEXT_Constructor (
    DSC_Descriptor &rSelf, DSC_Descriptor &rCurrent, DSC_Descriptor &rMy, DSC_Descriptor &rParent
)
{
    m_iSelf = rSelf;
    rSelf.construct ();

    m_iCurrent = rCurrent;
    rCurrent.construct ();

    m_iMy = rMy;
    rMy.construct ();

    m_iParent = rParent;
    rParent.construct ();
}

rtCONTEXT_Constructor::rtCONTEXT_Constructor (
    DSC_Descriptor &rSelf, DSC_Descriptor &rCurrent, DSC_Descriptor &rMy
)
{
    m_iSelf = rSelf;
    rSelf.construct ();

    m_iCurrent = rCurrent;
    rCurrent.construct ();

    m_iMy = rMy;
    rMy.construct ();

    m_iParent.construct ();
}

rtCONTEXT_Constructor::rtCONTEXT_Constructor (DSC_Descriptor const &rSelfCurrentAndMy) {
    m_iSelf	.construct (rSelfCurrentAndMy);
    m_iCurrent	.construct (rSelfCurrentAndMy);
    m_iMy	.construct (rSelfCurrentAndMy);
    m_iParent	.construct ();
}

rtCONTEXT_Constructor::rtCONTEXT_Constructor (M_CPD *pContainer) : VConstructor (pContainer) {
/*****  Unpack the components of this container...  *****/
    M_CPD *dscCPD;
    
    rtDSC_Unpack (dscCPD = rtCONTEXT_CPD_CurrentCPD (pContainer), &m_iCurrent);
    dscCPD->release ();

    rtDSC_Unpack (dscCPD = rtCONTEXT_CPD_SelfCPD    (pContainer), &m_iSelf);
    dscCPD->release ();

    rtDSC_Unpack (dscCPD = rtCONTEXT_CPD_OriginCPD  (pContainer), &m_iMy);
    dscCPD->release ();

    m_iParent.construct ();
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

rtCONTEXT_Constructor::~rtCONTEXT_Constructor () {
    m_iSelf	.clear ();
    m_iCurrent  .clear ();
    m_iMy	.clear ();
    m_iParent   .clear ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

RTYPE_Type rtCONTEXT_Constructor::RType_() const {
    return RTYPE_C_Context;
}

M_ASD *rtCONTEXT_Constructor::Space_() const {
    return ((rtCONTEXT_Constructor*)(this))->getCurrent ().codSpace ();
}


/*---------------------------------------------------------------------------
 *****  Utility macro to obtain the value of a context component.
 *
 *  Arguments:
 *	pThis			- the address of the context psuedo-object
 *				  (i.e., constructor) whose component is
 *				  to be accessed.
 *	rComponent		- the constructor field name for the component
 *				  to be accessed
 *	pAccessRoutine		- the name of the routine to obtain the
 *				  value of the component from a constructor.
 *
 *  Returns:
 *	rComponent
 *
 *****/
DSC_Descriptor &rtCONTEXT_Constructor::Get (DSC_Descriptor rtCONTEXT_Constructor::*pMemberComponent) {
    DSC_Descriptor &rComponent = this->*pMemberComponent;

    rtCONTEXT_Constructor* pParentContext;
    if (rComponent.isEmpty () &&
	IsntNil (pParentContext = static_cast<rtCONTEXT_Constructor*> (m_iParent.storePOIfAvailable ()))
    ) {
	DSC_Descriptor &rParentComponent = pParentContext->Get (pMemberComponent);

	if (rParentComponent.isntEmpty ()) {
	    rComponent.constructComposition (
		DSC_Descriptor_Pointer (m_iParent), rParentComponent
	    );
	}
    }

    return rComponent;
}


/*************************
 *************************
 *****  Realization  *****
 *************************
 *************************/

/*---------------------------------------------------------------------------
 *****  Routine to realize a context constructor.
 *
 *  Arguments:
 *	constructor		- the context psuedo-object (i.e., constructor)
 *				  to be realized.
 *
 *  Returns:
 *	A standard CPD for the realized context.
 *
 *****/
M_CPD *rtCONTEXT_Constructor::newRealization () {
    M_ASD *pContainerSpace = ScratchPad_();

/*****  Create the container, ...  *****/
    M_CPD *pContainer = POPVECTOR_New (
	pContainerSpace, RTYPE_C_Context, rtCONTEXT_Context_POPCount
    );

/*****  ... pack and store its components, ...  *****/
    M_CPD *dscCPD = rtDSC_Pack (pContainerSpace, &getCurrent ());
    pContainer->StoreReference (rtCONTEXT_CPx_Current, dscCPD);
    dscCPD->release ();

    dscCPD = rtDSC_Pack (pContainerSpace, &getSelf ());
    pContainer->StoreReference (rtCONTEXT_CPx_Self, dscCPD);
    dscCPD->release ();

    dscCPD = rtDSC_Pack (pContainerSpace, &getMy ());
    pContainer->StoreReference (rtCONTEXT_CPx_Origin, dscCPD);
    dscCPD->release ();

/*****  ... release the now unneeded parent, ...  *****/
    m_iParent.clear ();

/*****  ... and return.  *****/
    return pContainer;
}


/*********************************
 *********************************
 *****                       *****
 *****  rtCONTEXT_Container  *****
 *****                       *****
 *********************************
 *********************************/

/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a context
 *
 *  Arguments:
 *	context			- a standard CPD for the context to be aligned.
 *
 *  Returns:
 *	true if the closure required alignment. false if not.
 *
 *****/
PublicFnDef bool rtCONTEXT_Align (M_CPD *context) {
    bool result = false;

/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtCONTEXT_Align", M_CPD_RType (context), RTYPE_C_Context);

/*****  Align Current  *****/
    M_CPD *dscCPD = rtCONTEXT_CPD_CurrentCPD (context);
    if (rtDSC_Align (dscCPD))
	result = true;
    dscCPD->release ();

/*****  Align Self  *****/
    dscCPD = rtCONTEXT_CPD_SelfCPD (context);
    if (rtDSC_Align (dscCPD))
	result = true;
    dscCPD->release ();

/*****  Align Origin  *****/
    dscCPD = rtCONTEXT_CPD_OriginCPD (context);
    if (rtDSC_Align (dscCPD))
	result = true;
    dscCPD->release ();

    return result;
}


/********************************************************************
 ********************************************************************
 *****  Standard Representation Type Services Handler Routines  *****
 ********************************************************************
 ********************************************************************/

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

RTYPE_DefineHandler (rtCONTEXT_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	POPVECTOR_StdDMDEPackage
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtCONTEXT_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = POPVECTOR_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtCONTEXT_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = POPVECTOR_ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = POPVECTOR_MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD*>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD*>() = instanceMD;
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
    FAC_FDFCase_FacilityIdAsString (rtCONTEXT);
    FAC_FDFCase_FacilityDescription ("Context Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTcontext.c 1 replace /users/mjc/system
  861002 17:55:33 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTcontext.c 2 replace /users/mjc/system
  861003 11:55:23 mjc Fixed segmentation fault realizing primitive contexts

 ************************************************************************/
/************************************************************************
  RTcontext.c 3 replace /users/mjc/system
  861007 17:26:35 mjc Fixed omission of 'parent' slot in constructor garbage collect

 ************************************************************************/
/************************************************************************
  RTcontext.c 4 replace /users/mjc/system
  861028 22:54:24 mjc Fixed non-reclaimation of constructor storage

 ************************************************************************/
/************************************************************************
  RTcontext.c 5 replace /users/mjc/translation
  870524 09:32:07 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTcontext.c 6 replace /users/jck/system
  871007 13:33:56 jck Added a marking function for the global garbage collector

 ************************************************************************/

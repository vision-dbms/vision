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
#include "RTvstore.h"


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

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

rtCONTEXT_Handle::rtCONTEXT_Handle (
    rtCONTEXT_Handle *pParentContext, DSC_Pointer &rParentPointer
) : BaseClass (pParentContext->ScratchPad (), RTYPE_C_Context), m_pParentContext (pParentContext) {
    m_iSelf	.construct ();
    m_iCurrent	.construct ();
    m_iMy	.construct ();

    m_iParentPointer = rParentPointer;
    rParentPointer.construct ();
}

rtCONTEXT_Handle::rtCONTEXT_Handle (
    DSC_Descriptor &rSelf, DSC_Descriptor &rCurrent, DSC_Descriptor &rMy
) : BaseClass (rCurrent.codScratchPad (), RTYPE_C_Context) {
    m_iSelf = rSelf;
    rSelf.construct ();

    m_iCurrent = rCurrent;
    rCurrent.construct ();

    m_iMy = rMy;
    rMy.construct ();

    m_iParentPointer.construct ();
}

rtCONTEXT_Handle::rtCONTEXT_Handle (
    DSC_Descriptor const &rSelfCurrentAndMy
) : BaseClass (rSelfCurrentAndMy.codScratchPad (), RTYPE_C_Context) {
    m_iSelf	.construct (rSelfCurrentAndMy);
    m_iCurrent	.construct (rSelfCurrentAndMy);
    m_iMy	.construct (rSelfCurrentAndMy);
    m_iParentPointer.construct ();
}

rtCONTEXT_Handle::rtCONTEXT_Handle (M_CTE &rCTE) : rtPOPVECTOR_Handle (rCTE) {
/*****  Unpack the components of this container...  *****/
    rtDSC_Handle::Reference pDescriptor;

    getCurrent (pDescriptor);
    pDescriptor->getValue (m_iCurrent);

    getSelf (pDescriptor);
    pDescriptor->getValue (m_iSelf);

    getMy (pDescriptor);
    pDescriptor->getValue (m_iMy);

    m_iParentPointer.construct ();
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

rtCONTEXT_Handle::~rtCONTEXT_Handle () {
    m_iSelf	.clear ();
    m_iCurrent  .clear ();
    m_iMy	.clear ();
    m_iParentPointer.clear ();
}

void rtCONTEXT_Handle::deleteThis () {
    if (onDeleteThis ())
	delete this;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

rtPTOKEN_Handle *rtCONTEXT_Handle::getPToken () const {
    return m_iCurrent.isntEmpty () ? m_iCurrent.PPT ()
	 : m_iSelf   .isntEmpty () ? m_iSelf   .PPT ()
	 : m_iMy     .isntEmpty () ? m_iMy     .PPT ()
	 : m_iParentPointer.PPT ();
}

/*---------------------------------------------------------------------------
 *****  Utility to obtain a context component.
 *
 *  Arguments:
 *	rComponent		- the constructor field name for the component
 *				  to be accessed
 *	pAccessRoutine		- the name of the routine to obtain the
 *				  value of the component from a constructor.
 *
 *  Returns:
 *	rComponent
 *
 *****/
DSC_Descriptor &rtCONTEXT_Handle::Get (DSC_Descriptor rtCONTEXT_Handle::*pMemberComponent) {
    DSC_Descriptor &rComponent = this->*pMemberComponent;

    if (rComponent.isEmpty () && m_pParentContext.isntNil ()) {
	DSC_Descriptor &rParentComponent = m_pParentContext->Get (pMemberComponent);
	if (rParentComponent.isntEmpty ())
	    rComponent.constructComposition (m_iParentPointer, rParentComponent);
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
void rtCONTEXT_Handle::createContainer () {
    if (hasNoContainer ()) {
	CreateContainer (rtCONTEXT_Context_POPCount);

	rtDSC_Handle::Reference pDescriptor;

	pDescriptor.setTo (new rtDSC_Handle (m_pASD, m_iCurrent));
	StoreReference (rtCONTEXT_CPx_Current, pDescriptor);

	pDescriptor.setTo (new rtDSC_Handle (m_pASD, m_iSelf));
	StoreReference (rtCONTEXT_CPx_Self, pDescriptor);

	pDescriptor.setTo (new rtDSC_Handle (m_pASD, m_iMy));
	StoreReference (rtCONTEXT_CPx_Origin, pDescriptor);

    /*****  ... release the now unneeded parent, ...  *****/
	m_iParentPointer.clear ();
    }
}

bool rtCONTEXT_Handle::PersistReferences () {
    createContainer ();
    return BaseClass::PersistReferences ();
}


/********************************
 ********************************
 *****  Garbage Collection  *****
 ********************************
 ********************************/

void rtCONTEXT_Handle::visitReferencesUsing (Visitor *visitor) {
    BaseClass::visitReferencesUsing (visitor);
    if (m_pParentContext)
	m_pParentContext->visitUsing (visitor);

    m_iMy.visitReferencesUsing (visitor);
    m_iSelf.visitReferencesUsing (visitor);
    m_iCurrent.visitReferencesUsing (visitor);
    // traverse m_iParentPointer in the future?

}


void rtCONTEXT_Handle::generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const {
    BaseClass::generateReferenceReport (rOutputFile, xLevel++);
    if (m_pParentContext.isntEmpty())
	m_pParentContext->generateReferenceReport (rOutputFile, xLevel);

    m_iMy.generateReferenceReport (rOutputFile, xLevel);
    m_iSelf.generateReferenceReport (rOutputFile, xLevel);
    m_iCurrent.generateReferenceReport (rOutputFile, xLevel);
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to align a context
 *
 *  Returns:
 *	true if the closure required alignment. false if not.
 *
 *****/
bool rtCONTEXT_Handle::align () {
    bool bAlignmentsDone = false;

/*****  Align Current  *****/
    rtDSC_Handle::Reference pDescriptor;
    getCurrent (pDescriptor);
    if (pDescriptor->align ())
	bAlignmentsDone = true;

/*****  Align Self  *****/
    getSelf (pDescriptor);
    if (pDescriptor->align ())
	bAlignmentsDone = true;

/*****  Align Origin  *****/
    getMy (pDescriptor);
    if (pDescriptor->align ())
	bAlignmentsDone = true;

    return bAlignmentsDone;
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

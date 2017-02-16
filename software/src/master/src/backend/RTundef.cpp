/*****  The Undefined Object Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtUNDEF

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"

/*****  Self  *****/
#include "RTundef.h"


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

DEFINE_CONCRETE_RTT (rtUNDEF_Handle);


/**************************
 *****  Instantiator  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new undefined object.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	A standard CPD for the undefined object created.
 *
 *  Notes:
 *	This routine will be called once to create the original undefined
 *	physical object.  After that, there is probably no reason to ever
 *	call it again.
 *
 *****/
PublicFnDef M_CPD *rtUNDEF_New (M_ASD *pContainerSpace) {
    return pContainerSpace->CreateContainer (RTYPE_C_Undefined, 0)->NewCPD ();
}


/********************************************************************
 *****  Standard Representation Type Services Handler Routines  *****
 ********************************************************************/

/***************
 *  Converter  *
 ***************/

PrivateFnDef M_CPreamble* ConvertUndefined0 (
    M_ASD *pASD, M_CPreamble const *oldPreamble
) {
    return pASD->AllocateContainer (RTYPE_C_Undefined, 0, M_CPreamble_POPContainerIndex (oldPreamble));
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNilaryMethod (New_DM) {
    return RTYPE_QRegister (rtUNDEF_New (IOBJ_ScratchPad));
}

/************************
 *  'Instance' Methods  *
 ************************/


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtUNDEF_ConversionHandler) {
    M_RTD* rtd;
    switch (handlerOperation) {
    case RTYPE_InitializeMData:
        rtd = iArgList.arg<M_RTD*>();
	M_RTD_ConvertFn (rtd) = ConvertUndefined0;
        break;
    default:
        return -1;
    }
    return 0;
}


RTYPE_DefineHandler(rtUNDEF_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"	    , RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	    , RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new"		    , New_DM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(0);
	    M_RTD_HandleMaker		(rtd) = &rtUNDEF_Handle::Maker;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
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
    FAC_FDFCase_FacilityIdAsString (rtUNDEF);
    FAC_FDFCase_FacilityDescription ("The Undefined Object Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTundef.c 1 replace /users/mjc/system
  860321 18:08:05 mjc Container Table Representation Types

 ************************************************************************/
/************************************************************************
  RTundef.c 2 replace /users/mjc/system
  860423 11:02:44 mjc Added 'New' routine

 ************************************************************************/
/************************************************************************
  RTundef.c 3 replace /users/mjc/translation
  870524 09:35:57 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

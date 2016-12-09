/*****  Run Time Environment Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName ENVIR

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "gopt.h"
#include "m.h"
#include "ps.h"

#include "vdates.h"
#include "vdsc.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"

#include "RTdictionary.h"
#include "RTdsc.h"
#include "RTindex.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTparray.h"
#include "RTptoken.h"
#include "RTrefuv.h"
#include "RTundef.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "M_KnownObjectTable.h"

#include "VDatabaseFederatorForBatchvision.h"

/*****  Self  *****/
#include "venvir.h"


/***************************
 ***************************
 *****                 *****
 *****  ENVIR_Session  *****
 *****                 *****
 ***************************
 ***************************/

PrivateVarDef VSession *g_pThisSession = 0;

PublicFnDef VSession *ENVIR_Session () {
    if (!g_pThisSession)
	g_pThisSession = new VSession ();
    return g_pThisSession;
}

PublicFnDef double ENVIR_SessionMemoryUse () {
    return g_pThisSession ? g_pThisSession->CurrentTransientAllocationLevel() : 0;
}

/*********************
 *********************
 *****           *****
 *****  Globals  *****
 *****           *****
 *********************
 *********************/

/**************************
 *  Flags and Path Names  *
 **************************/

PrivateVarDef bool		AdministrativeSystem,
				BootStrapingSystem;
PrivateVarDef char const*	NDFPathName;
PrivateVarDef char const*	OSDPathName;

/**********************
 *  Built-In Objects  *
 **********************/

ENVIR_BuiltInObjectPArray *M_KOT::ContainerArray () const {
    return (ENVIR_BuiltInObjectPArray *) rtPARRAY_CPD_ElementArray (m_pKOTC);
}

/*************************************************************
 *  Top Level Task, Environment, and Dictionary Definitions  *
 *************************************************************/

PublicVarDef DSC_Descriptor	ENVIR_KDsc_TheTLE,
				ENVIR_KDsc_TheTmpTLE;

/*****************
 *  Known Dates  *
 *****************/

PublicVarDef rtINDEX_Key*	ENVIR_KTemporalContext_Today = 0;


/**********************
 **********************
 *****            *****
 *****  VSession  *****
 *****            *****
 **********************
 **********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSession::VSession () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSession::~VSession () {
}


/*********************************
 *********************************
 *****  Database Activation  *****
 *********************************
 *********************************/

M_AND *VSession::Boot (char const *pNDFPathName) {
    m_pInitialDatabase.setTo (Activate (pNDFPathName));
    M_AttachedNetwork.setTo (m_pInitialDatabase);

    /*****  Create the built-in objects required for system boot strap...  *****/
    m_pInitialDatabase->CreateBootStrapObjects ();

    /*****  Create surrogates for ^global, ^tmp, and ^today, ...  *****/
    ENVIR_KDsc_TheTLE.constructNA (m_pInitialDatabase->KOT ());
    ENVIR_KDsc_TheTmpTLE.construct (ENVIR_KDsc_TheTLE);

    ENVIR_KTemporalContext_Today = new rtINDEX_Key (
	m_pInitialDatabase->TheScalarPTokenHandle (), &ENVIR_KDsc_TheTLE
    );

    return m_pInitialDatabase;
}

M_AND *VSession::Open (
    char const *	pNDFPathName,
    char const *	pVersionSpec,
    char const *	pXUSpecPath,
    char const *	pObjectSpaceName,
    bool		makingNewNDF
) {
    m_pInitialDatabase.setTo (Activate (pNDFPathName, pVersionSpec, makingNewNDF));
    M_AttachedNetwork.setTo (m_pInitialDatabase);

    if (pXUSpecPath)
	m_pInitialDatabase->IncorporateExternalUpdate (pXUSpecPath);

/*****  Access ^global (along with the database's built-in objects)...  *****/
    m_pInitialDatabase->AccessTheTLE (
	atoi (pObjectSpaceName), ENVIR_KDsc_TheTLE, &m_pTLEDescriptor
    );

/*****  ... initialize ^tmp. *****/
    M_ASD *pScratchPad = m_pInitialDatabase->ScratchPad ();

    rtDICTIONARY_Handle *pDictionary = new rtDICTIONARY_Handle (pScratchPad);
    rtPTOKEN_Handle::Reference pInstances (new rtPTOKEN_Handle (pScratchPad, 1));
    M_CPD *pSuperPointer = ENVIR_KDsc_TheTLE.pointerCPD (pInstances);

    ENVIR_KDsc_TheTmpTLE.constructCorrespondence (
	m_pInitialDatabase->TheScalarPTokenHandle (), rtVSTORE_NewCluster (
	    pInstances, pDictionary, ENVIR_KDsc_TheTLE.store (), pSuperPointer
	)
    );
    pSuperPointer->release ();

/*****  ...initialize ^today...  *****/
    DSC_Descriptor today; int y, m, d;
    today.constructScalar (
	m_pInitialDatabase->TheDateClass (), DATES_FindTodaysDate (&y, &m, &d)
    );
    ENVIR_KTemporalContext_Today = new rtINDEX_Key (
	m_pInitialDatabase->TheScalarPTokenHandle (), &today
    );
    today.clear ();

    return m_pInitialDatabase;
}

M_AND *VSession::Activate (
    char const *pNDFPathName, char const *pVersionSpec, bool makingNewNDF
) {
    if (m_pDatabaseFederator.isNil ())
	m_pDatabaseFederator.setTo (new VDatabaseFederatorForBatchvision ());

    return static_cast<M_AND*>(
	m_pDatabaseFederator->Activate (pNDFPathName, pVersionSpec, makingNewNDF)
    );
}

unsigned int VSession::DatabaseActivationCount () const {
    return m_pDatabaseFederator->ActivationCount ();
}

/****************************************
 ****************************************
 *****  Resource Utilization Query  *****
 ****************************************
 ****************************************/

void VSession::AccumulateAllocationStatistics (
    unsigned __int64 *pAllocationTotal, unsigned __int64 *pMappingTotal
) const {
    if (m_pDatabaseFederator)
	m_pDatabaseFederator->AccumulateAllocationStatistics (
	    pAllocationTotal, pMappingTotal
	);
}

double VSession::CurrentTransientAllocationLevel () const {
    if (m_pDatabaseFederator)
	return m_pDatabaseFederator->CurrentTransientAllocationLevel ();
    else
	return 0;
}

double VSession::TransientAllocationHighWaterMark () const {
    if (m_pDatabaseFederator)
	return m_pDatabaseFederator->TransientAllocationHighWaterMark ();
    else
	return 0;
}

/*********************************************
 *********************************************
 *****  Resource Utilization Management  *****
 *********************************************
 *********************************************/

bool VSession::DisposeOfSessionGarbage (bool bAggressive) const {
    return m_pDatabaseFederator.isntNil ()
	&& m_pDatabaseFederator->DisposeOfSessionGarbage (bAggressive);
}

void VSession::FlushCachedResources () const {
    if (m_pDatabaseFederator)
	m_pDatabaseFederator->FlushCachedResources ();
}

unsigned int VSession::ReclaimSegments () const {
    return m_pDatabaseFederator ? m_pDatabaseFederator->ReclaimSegments () : 0;
}

unsigned int VSession::ReclaimAllSegments () const {
    return m_pDatabaseFederator ? m_pDatabaseFederator->ReclaimAllSegments () : 0;
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Known Object Table Management  *****
 *****                                 *****
 *******************************************
 *******************************************/

/*****************************************
 *****  Known CPD Recording Utility  *****
 *****************************************/

void M_KOT::ForwardAndLock (M_CPD* pObject) const {
    if (pObject->isInTheScratchPad () && m_pKOTC->isntInTheScratchPad ())
	pObject->ForwardToSpace (m_pKOTC);

    pObject->SetGCLock ();
}

void M_KOTE::RegisterObject (M_KOT *pKOT, M_CPD *pObject) {
    pKOT->ForwardAndLock (pObject);

    m_pObjectCPD = pObject;
    m_pObjectHandle = pObject->containerHandle ();

    if (m_pObjectHandle->getStore (m_pStore)) {
	m_pPTokenHandle.setTo (m_pStore->getPToken ());
    }
}


/***********************************************
 *****  Known Object Definition Utilities  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Macro to define a known object.
 *
 *  Arguments:
 *	objectName		- the name of the known object as defined in
 *				  'ENVIR_KOTEs'.
 *	pObject			- a CPD for the new value of the object.  This
 *				  CPD will be assigned into the array of known
 *				  CPD.  Duplication of it or its pointer is the
 *				  responsibility of the user of this macro.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DefineKnownObject(objectName, pObject) RegisterObject (\
    (pObject), &M_KOT::objectName, &ENVIR_BuiltInObjectPArray::objectName\
)

void M_KOT::RegisterObject (
    M_CPD *pObject, M_KOTE M_KOT::*pKOTEM, M_POP ENVIR_BuiltInObjectPArray::*pKOTCM
) {
    (this->*pKOTEM).RegisterObject (this, pObject);

    rtPARRAY_CPD_Element (m_pKOTC) = &(ContainerArray()->*pKOTCM);
    m_pKOTC->EnableModifications ();
    m_pKOTC->StoreReference (rtPARRAY_CPx_Element, pObject);
}

void M_KOT::RegisterObject (
    VContainerHandle *pObject, M_KOTE M_KOT::*pKOTEM, M_POP ENVIR_BuiltInObjectPArray::*pKOTCM
) {
    RegisterObject (pObject->GetCPD (), pKOTEM, pKOTCM);
}


/***************************************
 *****  Built-In Objects Creation  *****
 ***************************************/

/*******************************************
 *----  M_AND::CreateBootStrapObjects  ----*
 *******************************************/

void M_AND::CreateBootStrapObjects () {
/*****  Create 'Nil', ...  *****/
    display ("BEGIN bootstrap creation:\n");

    M_CPD* nil = rtUNDEF_New (ScratchPad ());
    display (
	"%s", M_UpdateStatusDescription (nil->CreateSpace (), "...'Nil' Created")
    );
    display (
	"%s\n", M_UpdateStatusDescription (UpdateNetwork (true), " and Saved")
    );
    nil->release ();

/*****  ... and create the built-in object vector, ...  *****/
    m_pKnownObjectTable.setTo (new M_KOT (this));
    m_pKnownObjectTable->completeInitialization ();
}


/**************************
 *----  M_KOT::M_KOT  ----*
 **************************/

M_KOT::M_KOT (M_AND *pAND) : m_pKOTC (rtPARRAY_New (pAND->ScratchPad (), ENVIR_BuiltInObjectPArraySize)) {
/*****  Create and store the built-in p-array:  *****/
    display (
	"%s\n", M_UpdateStatusDescription (
	    m_pKOTC->CreateSpace (), "...'BuiltInObjectPArray' Created"
	)
    );
    M_ASD *pKOTSpace = m_pKOTC->Space ();

/*****  Create the scalar P-Token, ...  *****/
    DefineKnownObject (TheScalarPToken, (new rtPTOKEN_Handle (pKOTSpace, 1))->GetCPD ());
    display ("...'Scalar P-Token' Created\n");

/*****  ... 'SelectorPToken', ...  *****/
    DefineKnownObject (TheSelectorPToken, (new rtPTOKEN_Handle (pKOTSpace, 0))->GetCPD ());
    display ("...'Selector P-Token' Created\n");
}

void M_KOT::completeInitialization () {
    M_ASD *pKOTSpace = m_pKOTC->Space ();
/*****  ... 'ObjectPrototype', ...  *****/
    rtDICTIONARY_Handle::Reference pObjectDictionary (new rtDICTIONARY_Handle (pKOTSpace));
    rtPTOKEN_Handle::Reference pObjectPToken (new rtPTOKEN_Handle (pKOTSpace, 0));
    DefineKnownObject (
	TheObjectPrototype, rtVSTORE_NewCluster (
	    pObjectPToken, pObjectDictionary, NilOf (Vdd::Store*), NilOf (M_CPD*)
	)
    );
    display ("...'Object' created\n");

/*****  ... 'TheNAClass', ...  *****/
    rtDICTIONARY_Handle::Reference pNADictionary (new rtDICTIONARY_Handle (pKOTSpace));
    rtPTOKEN_Handle::Reference pNAPToken (new rtPTOKEN_Handle (pKOTSpace, 0));
    M_CPD* pNAInheritance = rtLINK_RefConstructor (pObjectPToken)->Close (
	pNAPToken
    )->ToLink ();

    DefineKnownObject (
        TheNAClass, rtVSTORE_NewCluster (
	    pNAPToken, pNADictionary, TheObjectPrototype.store (), pNAInheritance
	)
    );
    display ("...'Undefined' Created\n");

/*****  ...cleanup, and return.  *****/
    pNAInheritance	->release ();
    display ("...after Cleanup\n");

    display ("DONE\n");
}


/*********************************************
 *****  Built-In Objects Access Routine  *****
 *********************************************/

/**************************
 *----  M_KOT::M_KOT  ----*
 **************************/

M_KOT::M_KOT (M_CPD *pKOTC) : m_pKOTC (pKOTC) {
    static M_KOTE M_KOT::* const P2TKOTEMembers[] = {
	ENVIR_KOTEs(ENVIR_KOTE_IncludeP2TKOTEM, ENVIR_AKOE_IncludeZero)
    };

/*****  Obtain standard CPDs for the objects contained in the KOTC, ...  *****/
    for (unsigned int i = 0; i < ENVIR_BuiltInObjectPArraySize; i++) {
	M_KOTE M_KOT::*pKOTEM = P2TKOTEMembers[i];
	if (pKOTEM != (M_KOTM)0 && m_pKOTC->ReferenceIsntNil((unsigned int)rtPARRAY_CPx_Element))
	    (this->*pKOTEM).RegisterObject (this, m_pKOTC->GetCPD (rtPARRAY_CPx_Element));
	rtPARRAY_CPD_Element (m_pKOTC)++;
    }

/*****  ...special case certain CPD initializations, ...  *****/
/*----------------------------------------------------------*
 *  I have no idea why this is here.  TheSelectorPToken
 *  is the second object registered in the bootstrap.
 *
 *  -mjc
 *----------------------------------------------------------*/
    if (TheSelectorPToken.IsntSet ()) {
	TheSelectorPToken.RegisterObject (
	    this, TheSelectorClass.PTokenHandle ()->GetCPD ()
	);
    }
}


/******************************************************
 *****  The Top Level Environment Access Routine  *****
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to access the top level environment at startup.
 *
 *  Arguments:
 *	xSpace			- the index of the object space to be opened
 *	rTLE			- an l-val of type DSC_Descriptor in which a
 *				  descriptor for ^global will be constructed.
 *	ppTLEDescriptor		- the optional address of a M_CPD* that, when
 *				  non-null, will be set to the packed version
 *				  of 'rTLE'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void M_AND::AccessTheTLE (
    unsigned int xSpace, DSC_Descriptor &rTLE, rtDSC_Handle::Reference *ppTLEDescriptor
) {
//  Access the known object table if it hasn't been loaded yet, ...
    if (m_pKnownObjectTable.isNil ()) m_pKnownObjectTable.setTo (
	new M_KOT (GetCPD (M_KnownSpace_SystemRoot, M_KnownCTI_SpaceRoot, RTYPE_C_PArray))
    );

/*****  If a space was specified, ...  *****/
    if (xSpace > 0) {
    /*****  ... access it, ...  *****/
	rtDSC_Handle::Reference pTLEDescriptor (
	    xSpace > 2 ? static_cast<rtDSC_Handle*>(
		GetContainerHandle (xSpace, M_KnownCTI_SpaceRoot, RTYPE_C_Descriptor)
	    ) : static_cast<rtDSC_Handle*>(m_pKnownObjectTable->TheRootEnvironment.ObjectHandle ())
	);

    /*****  ... unpack it, ...  *****/
	pTLEDescriptor->getValue (rTLE);

    /*****  ... and validate it:  *****/
	if (rTLE.isntScalar ()) ERR_SignalFault (
	    EC__UsageError, "The Top Level Environment is Non-Scalar"
	);

	if (RTYPE_C_ValueStore != rTLE.storeRType ()) ERR_SignalFault (
	    EC__UsageError, "The Top Level Environment is Non-Tabular"
	);

	if (ppTLEDescriptor)
	    ppTLEDescriptor->claim (pTLEDescriptor);
    }
    else {
    /*****  ... otherwise, access and unpack the 'user' root,  *****/
	rtDSC_Handle::Reference pOS3Descriptor (
	    static_cast<rtDSC_Handle*>(
		GetContainerHandle (M_KnownSpace_UserRoot, M_KnownCTI_SpaceRoot, RTYPE_C_Descriptor)
	    )
	);
        pOS3Descriptor->getValue (rTLE);

    /*****  ... create a new dictionary, ...  *****/
	rtDICTIONARY_Handle::Reference pTLEDictionary (new rtDICTIONARY_Handle (ScratchPad ()));

    /*****
     *  ... and activate that dictionary as the user's top level environment.
     *****/
	Vdd::Store::Reference pTLEStore (rTLE.store ());
	rtPTOKEN_Handle::Reference pTLEInstances (new rtPTOKEN_Handle (ScratchPad (), 1));
	M_CPD *pTLEPointer = rTLE.pointerCPD (pTLEInstances);

	rTLE.clear ();

	rTLE.constructCorrespondence (
	    m_pKnownObjectTable->TheScalarPTokenHandle (), rtVSTORE_NewCluster (
		pTLEInstances, pTLEDictionary, pTLEStore, pTLEPointer
	    )
	);

	if (ppTLEDescriptor)
	    ppTLEDescriptor->setTo (new rtDSC_Handle (ScratchPad (), rTLE));

	pTLEPointer->release ();
    }
}


/*********************************************
 *****  The Top Level Environment Saver  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to save the top level environment
 *
 *  Arguments:
 *	pOutputHandler		- the address of a 'fprintf' like routine to
 *				  handle the output produced by this routine.
 *	pOutputHandlerData	- an address passed as the first argument to
 *				  the output handler to supply it with extra
 *				  data.
 *
 *  Returns:
 *	The PS_UpdateStatus of the save.
 *
 *****/
PS_UpdateStatus M_AND::SaveTheTLE (
    void *pOutputHandlerData, M_AND::UpdateOutputHandler pOutputHandler
) {
/*****  Disallow save in boot strap systems, ...  *****/
    if (BootStrapingSystem) ERR_SignalFault (
	EC__UsageError, "Environment Update Not Permitted During Bootstrap"
    );

/*****  Determine the top level environment, ...  *****/
    rtDSC_Handle::Reference pTLEDescriptor;
    if (this == ENVIR_Session ()->InitialDatabase ())
	pTLEDescriptor.setTo (ENVIR_Session ()->TheTLEDescriptor ());
    else {
	DSC_Descriptor iTLE;
	AccessTheTLE (m_pASDRing->Index (), iTLE, &pTLEDescriptor);
	iTLE.clear ();
    }

/*****  Attempt to make the top level environment persistent, ...  *****/
    PS_UpdateStatus xUpdateStatus = PS_UpdateStatus_Incomplete;
    if (pTLEDescriptor->isInTheScratchPad ()) {
	if (AdministrativeSystem)
	    xUpdateStatus = pTLEDescriptor->CreateSpace ();
	else {
	    xUpdateStatus = PS_UpdateStatus_NotPermitted;
	    SetUpdateStatusTo (PS_UpdateStatus_NotPermitted);
	}
	pOutputHandler (
	    pOutputHandlerData, ">>> %s <<<\n", M_UpdateStatusDescription (
		xUpdateStatus, string (
		    "Object Space %u Created", pTLEDescriptor->spaceIndex ()
		)
	    )
	);
    }

/*****
 *  ...  and update the network if the top level environment is persistent.
 *****/
    if (pTLEDescriptor->isntInTheScratchPad ()) {
	pTLEDescriptor->Space ()->Distinguish ();
	xUpdateStatus = UpdateNetwork (AdministrativeSystem);
	pOutputHandler (
	    pOutputHandlerData, ">>> %s <<<\n", M_UpdateStatusDescription (
		xUpdateStatus,
		AdministrativeSystem ? "Object Network Updated" : "User Space Updated"
	    )
	);
    }

    return xUpdateStatus;
}


/***************************************
 *****  Facility Debugger Methods  *****
 ***************************************/

/************
 *  Access  *
 ************/

IOBJ_DefineNilaryMethod (BuiltInObjectPArrayDM) {
    return RTYPE_QRegister (M_DuplicateCPDPtr (IOBJ_ScratchPad->KOT()->ContainerCPD ()));
}

IOBJ_DefineNilaryMethod (TheTLEDM) {
    return RTYPE_QRegister (ENVIR_Session ()->TheTLEDescriptor ());
}

/************
 *  Update  *
 ************/

IOBJ_DefineUnaryMethod (SaveTheTLEDM) {
    IOBJ_ScratchPad->SaveTheTLE (stdout, (M_AND::UpdateOutputHandler)IO_fprintf);
    return self;
}

/***************
 *  Utilities  *
 ***************/

#define DefineBuiltInObjectGetDM(objectName)\
IOBJ_DefineNilaryMethod (objectName##DM) {\
    M_CPD *pObject = IOBJ_ScratchPad->KOT()->objectName;\
    return pObject ? RTYPE_QRegister (M_DuplicateCPDPtr (pObject)) : IOBJ_TheNilIObject;\
}

#define DefineBuiltInObjectGetAndSetDM(objectName)\
DefineBuiltInObjectGetDM (objectName)\
IOBJ_DefineMethod (Set##objectName##DM) {\
    IOBJ_ScratchPad->Database()->DefineKnownObject (\
	objectName, M_DuplicateCPDPtr (RTYPE_QRegisterCPD (parameterArray[0]))\
    );\
    return self;\
}


/*************
 *  Get/Set  *
 *************/

DefineBuiltInObjectGetDM	(TheNAClass)

DefineBuiltInObjectGetAndSetDM	(TheNoValueClass)

DefineBuiltInObjectGetAndSetDM	(TheSelectorClass)

DefineBuiltInObjectGetAndSetDM	(TheTrueClass)
DefineBuiltInObjectGetAndSetDM	(TheFalseClass)

DefineBuiltInObjectGetAndSetDM	(TheIntegerClass)
DefineBuiltInObjectGetAndSetDM	(TheFloatClass)
DefineBuiltInObjectGetAndSetDM	(TheDoubleClass)

DefineBuiltInObjectGetAndSetDM	(TheDateClass)
DefineBuiltInObjectGetAndSetDM	(TheCharacterClass)

DefineBuiltInObjectGetAndSetDM	(ThePrimitiveClass)
DefineBuiltInObjectGetAndSetDM	(TheClosureClass)
DefineBuiltInObjectGetAndSetDM	(TheMethodClass)
DefineBuiltInObjectGetAndSetDM	(TheFixedPropertyClass)
DefineBuiltInObjectGetAndSetDM	(TheTimeSeriesClass)

DefineBuiltInObjectGetDM	(TheObjectPrototype)

DefineBuiltInObjectGetAndSetDM	(TheFixedPropertySpecificationClass)
DefineBuiltInObjectGetAndSetDM	(TheTimeSeriesPropertySpecificationClass)

DefineBuiltInObjectGetAndSetDM	(TheFixedPropertyPrototype)
DefineBuiltInObjectGetAndSetDM	(TheTimeSeriesPropertyPrototype)

DefineBuiltInObjectGetAndSetDM	(TheListClass)
DefineBuiltInObjectGetAndSetDM	(TheIndexedListClass)
DefineBuiltInObjectGetAndSetDM	(TheStringClass)

DefineBuiltInObjectGetAndSetDM	(TheClosureClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheMethodClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheFixedPropertyClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheTimeSeriesClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheListClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheIndexedListClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheStringClassDictionary)
DefineBuiltInObjectGetAndSetDM	(TheSelectorClassDictionary)

DefineBuiltInObjectGetDM	(TheScalarPToken)
DefineBuiltInObjectGetDM	(TheSelectorPToken)

DefineBuiltInObjectGetAndSetDM	(TheDefaultProperty)
DefineBuiltInObjectGetAndSetDM	(TheDefaultListProperty)
DefineBuiltInObjectGetAndSetDM	(TheDefaultAListProperty)
DefineBuiltInObjectGetAndSetDM	(TheDefaultStringProperty)

DefineBuiltInObjectGetAndSetDM	(TheDefaultTVProperty)
DefineBuiltInObjectGetAndSetDM	(TheDefaultTVListProp)
DefineBuiltInObjectGetAndSetDM	(TheDefaultTVAListProp)
DefineBuiltInObjectGetAndSetDM	(TheDefaultTVStringProp)

DefineBuiltInObjectGetAndSetDM	(TheRootEnvironment)


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    static bool alreadyInitialized = false;
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("builtInObjectPArray"		, BuiltInObjectPArrayDM)
	IOBJ_MDE ("theUserEnvironment"		, TheTLEDM)
	IOBJ_MDE ("saveTheUserEnvironment"	, SaveTheTLEDM)
	IOBJ_MDE ("undefinedBehavior"		, TheNAClassDM)
	IOBJ_MDE ("noValueBehavior"		, TheNoValueClassDM)

	IOBJ_MDE ("selectorBehavior"		, TheSelectorClassDM)

	IOBJ_MDE ("trueBehavior"		, TheTrueClassDM)
	IOBJ_MDE ("falseBehavior"		, TheFalseClassDM)

	IOBJ_MDE ("integerBehavior"		, TheIntegerClassDM)
	IOBJ_MDE ("floatBehavior"		, TheFloatClassDM)
	IOBJ_MDE ("doubleBehavior"		, TheDoubleClassDM)

	IOBJ_MDE ("dateBehavior"		, TheDateClassDM)

	IOBJ_MDE ("characterBehavior"		, TheCharacterClassDM)

	IOBJ_MDE ("primitiveFnBehavior"		, ThePrimitiveClassDM)
	IOBJ_MDE ("closureBehavior"		, TheClosureClassDM)
	IOBJ_MDE ("methodBehavior"		, TheMethodClassDM)
	IOBJ_MDE ("valueBehavior"		, TheFixedPropertyClassDM)
	IOBJ_MDE ("timeVaryingValueBehavior"	, TheTimeSeriesClassDM)

	IOBJ_MDE ("objectPrototype"		, TheObjectPrototypeDM)

	IOBJ_MDE ("propertyPrototype"		, TheFixedPropertySpecificationClassDM)
	IOBJ_MDE ("timeVaryingPropertyPrototype", TheTimeSeriesPropertySpecificationClassDM)

	IOBJ_MDE ("fixedPropertyPrototype"	, TheFixedPropertyPrototypeDM)
	IOBJ_MDE ("timeSeriesPrototype"		, TheTimeSeriesPropertyPrototypeDM)

	IOBJ_MDE ("listPrototype"		, TheListClassDM)
	IOBJ_MDE ("aListPrototype"		, TheIndexedListClassDM)
	IOBJ_MDE ("stringPrototype"		, TheStringClassDM)

	IOBJ_MDE ("closureDictionary"		, TheClosureClassDictionaryDM)
	IOBJ_MDE ("methodDictionary"		, TheMethodClassDictionaryDM)
	IOBJ_MDE ("valueDictionary"		, TheFixedPropertyClassDictionaryDM)
	IOBJ_MDE ("timeVaryingValueDictionary"	, TheTimeSeriesClassDictionaryDM)
	IOBJ_MDE ("listDictionary"		, TheListClassDictionaryDM)
	IOBJ_MDE ("aListDictionary"		, TheIndexedListClassDictionaryDM)
	IOBJ_MDE ("stringDictionary"		, TheStringClassDictionaryDM)
	IOBJ_MDE ("selectorDictionary"		, TheSelectorClassDictionaryDM)

	IOBJ_MDE ("scalarPToken"		, TheScalarPTokenDM)
	IOBJ_MDE ("theSelectorPToken"		, TheSelectorPTokenDM)

	IOBJ_MDE ("theDefaultProperty"		, TheDefaultPropertyDM)
	IOBJ_MDE ("theDefaultListProperty"	, TheDefaultListPropertyDM)
	IOBJ_MDE ("theDefaultAListProperty"	, TheDefaultAListPropertyDM)
	IOBJ_MDE ("theDefaultStringProperty"	, TheDefaultStringPropertyDM)

	IOBJ_MDE (
	    "theDefaultTimeVaryingProperty"	, TheDefaultTVPropertyDM
	)
	IOBJ_MDE (
	    "theDefaultTimeVaryingListProperty"	, TheDefaultTVListPropDM
	)
	IOBJ_MDE (
	    "theDefaultTimeVaryingAListProperty", TheDefaultTVAListPropDM
	)
	IOBJ_MDE (
	    "theDefaultTimeVaryingStringProperty",TheDefaultTVStringPropDM
	)

	IOBJ_MDE ("theRootEnvironment"		, TheRootEnvironmentDM)

	IOBJ_MDE ("setNoValueBehavior:"		, SetTheNoValueClassDM)

	IOBJ_MDE ("setSelectorBehavior:"	, SetTheSelectorClassDM)

	IOBJ_MDE ("setTrueBehavior:"		, SetTheTrueClassDM)
	IOBJ_MDE ("setFalseBehavior:"		, SetTheFalseClassDM)

	IOBJ_MDE ("setIntegerBehavior:"		, SetTheIntegerClassDM)
	IOBJ_MDE ("setFloatBehavior:"		, SetTheFloatClassDM)
	IOBJ_MDE ("setDoubleBehavior:"		, SetTheDoubleClassDM)

	IOBJ_MDE ("setDateBehavior:"		, SetTheDateClassDM)

	IOBJ_MDE ("setCharacterBehavior:"	, SetTheCharacterClassDM)

	IOBJ_MDE ("setPrimitiveFnBehavior:"	, SetThePrimitiveClassDM)
	IOBJ_MDE ("setClosureBehavior:"		, SetTheClosureClassDM)
	IOBJ_MDE ("setMethodBehavior:"		, SetTheMethodClassDM)
	IOBJ_MDE ("setValueBehavior:"		, SetTheFixedPropertyClassDM)
	IOBJ_MDE ("setTimeVaryingValueBehavior:", SetTheTimeSeriesClassDM)

	IOBJ_MDE ("setPropertyPrototype:"	, SetTheFixedPropertySpecificationClassDM)
	IOBJ_MDE (
	    "setTimeVaryingPropertyPrototype:"	, SetTheTimeSeriesPropertySpecificationClassDM
	)

	IOBJ_MDE ("setFixedPropertyPrototype:"	, SetTheFixedPropertyPrototypeDM)
	IOBJ_MDE ("setTimeSeriesPrototype:"	, SetTheTimeSeriesPropertyPrototypeDM)

	IOBJ_MDE ("setListPrototype:"		, SetTheListClassDM)
	IOBJ_MDE ("setAListPrototype:"		, SetTheIndexedListClassDM)
	IOBJ_MDE ("setStringPrototype:"		, SetTheStringClassDM)

	IOBJ_MDE ("setClosureDictionary:"	, SetTheClosureClassDictionaryDM)
	IOBJ_MDE ("setMethodDictionary:"	, SetTheMethodClassDictionaryDM)
	IOBJ_MDE ("setValueDictionary:"		, SetTheFixedPropertyClassDictionaryDM)
	IOBJ_MDE (
	    "setTimeVaryingValueDictionary:"	, SetTheTimeSeriesClassDictionaryDM
	)
	IOBJ_MDE ("setListDictionary:"		, SetTheListClassDictionaryDM)
	IOBJ_MDE ("setAListDictionary:"		, SetTheIndexedListClassDictionaryDM)
	IOBJ_MDE ("setStringDictionary:"	, SetTheStringClassDictionaryDM)
	IOBJ_MDE ("setSelectorDictionary:"	, SetTheSelectorClassDictionaryDM)

	IOBJ_MDE ("setTheDefaultProperty:"	, SetTheDefaultPropertyDM)
	IOBJ_MDE ("setTheDefaultListProperty:"	, SetTheDefaultListPropertyDM)
	IOBJ_MDE ("setTheDefaultAListProperty:"	, SetTheDefaultAListPropertyDM)
	IOBJ_MDE ("setTheDefaultStringProperty:", SetTheDefaultStringPropertyDM)

	IOBJ_MDE (
	    "setTheDefaultTimeVaryingProperty:"	, SetTheDefaultTVPropertyDM
	)
	IOBJ_MDE (
	    "setTheDefaultTimeVaryingListProperty:", SetTheDefaultTVListPropDM
	)
	IOBJ_MDE (
	    "setTheDefaultTimeVaryingAListProperty:", SetTheDefaultTVAListPropDM
	)
	IOBJ_MDE (
	    "setTheDefaultTimeVaryingStringProperty:", SetTheDefaultTVStringPropDM
	)

	IOBJ_MDE ("setTheRootEnvironment:"	, SetTheRootEnvironmentDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (ENVIR);
    FAC_FDFCase_FacilityDescription ("Run Time Environment");
    FAC_FDFCase_FacilityMD (methodDictionary);

    case FAC_DoStartupInitialization:
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}
/*****  Obtain initial database access/creation options, ...  *****/
	AdministrativeSystem	= GOPT_GetSwitchOption ("Administrator") > 0;
	BootStrapingSystem	= AdministrativeSystem && GOPT_GetSwitchOption ("BootStrap") > 0;
	NDFPathName		= GOPT_GetValueOption ("NDFPathName");
	OSDPathName		= GOPT_GetValueOption ("OSDPathName");

/*****  If the boot strap option was specified...  *****/
	if (BootStrapingSystem) {

	    /*****  Create and access the object network...  *****/
	    PS_CreateNetwork (NDFPathName, OSDPathName, false);
	    ENVIR_Session ()->Boot (NDFPathName);

	    IO_printf ("\n\
\n\
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\
>>>  WARNING: Normal System Operation Not Available in BootStrap Mode  <<<\n\
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\
\n");
	}

/*****  Otherwise, access the object network...  *****/
	else {
	/*****  Network Rebuild?  *****/
	    bool makeNewNDF = false;
	    char const* rebuildOption	= GOPT_GetValueOption ("RebuildNDF");
	    if (AdministrativeSystem && IsntNil (rebuildOption)) {
		if (strcmp (rebuildOption, "fromOSD") == 0)
		    PS_CreateNetwork (NDFPathName, OSDPathName, true);
		else if (strcmp (rebuildOption, "fromNDF") == 0)
		    makeNewNDF = true;
		else IO_printf (
		    "Unrecognized Rebuild Option: '%s'\n", rebuildOption
		);
	    }

	    ENVIR_Session ()->Open (
		NDFPathName,
		GOPT_GetValueOption ("NetworkVersion"),
		GOPT_GetValueOption ("XUSpecPathName"),
		GOPT_GetValueOption ("UserObjectSpaceIndex"),
		makeNewNDF
	    );
	}

	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  envir.c 1 replace /users/mjc/system
  860531 10:41:39 mjc Added run time 'envir' facility

 ************************************************************************/
/************************************************************************
  envir.c 2 replace /users/mjc/system
  860531 17:57:04 mjc Release new known CPD names

 ************************************************************************/
/************************************************************************
  envir.c 3 replace /users/mjc/system
  860606 13:28:07 mjc Release numeric/primitive function method dictionary initialization

 ************************************************************************/
/************************************************************************
  envir.c 4 replace /users/mjc/system
  860607 12:13:52 mjc Fix a few bugs

 ************************************************************************/
/************************************************************************
  envir.c 5 replace /users/mjc/system
  860610 11:19:06 mjc Added code to create 'NoValueBehavior'

 ************************************************************************/
/************************************************************************
  envir.c 6 replace /users/mjc/system
  860623 10:23:49 mjc Converted to use new value descriptor macros/R-Type

 ************************************************************************/
/************************************************************************
  envir.c 7 replace /users/mjc/system
  860709 10:19:26 mjc Release new format value descriptors and boot strap environment

 ************************************************************************/
/************************************************************************
  envir.c 8 replace /users/mjc/system
  860713 17:13:28 mjc Release new format environment structure

 ************************************************************************/
/************************************************************************
  envir.c 9 replace /users/mjc/system
  860728 13:55:40 mjc Added new property types

 ************************************************************************/
/************************************************************************
  envir.c 10 replace /users/mjc/system
  860803 00:18:33 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  envir.c 11 replace /users/mjc/system
  860807 18:05:20 mjc Release basic time operations

 ************************************************************************/
/************************************************************************
  envir.c 12 replace /users/mjc/system
  860826 14:21:50 mjc Added default list and string properties and character behavior, deleted class, metaclass, and date offset

 ************************************************************************/
/************************************************************************
  envir.c 13 replace /users/jad/system
  860914 11:15:01 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  envir.c 14 replace /users/mjc/system
  861002 18:02:49 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  envir.c 15 replace /users/mjc/system
  861112 09:35:27 mjc Increase system paranoia level by locking all known CPDs

 ************************************************************************/
/************************************************************************
  envir.c 16 replace /users/mjc/system
  870215 23:06:51 lcn Changed startup and save to accommodate the new memory manager

 ************************************************************************/
/************************************************************************
  envir.c 17 replace /users/mjc/system
  870326 19:27:44 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  envir.c 18 replace /users/jck/system
  870420 13:34:22 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  envir.c 19 replace /users/mjc/system
  870519 23:28:47 mjc Merged compactor and time series changes

 ************************************************************************/
/************************************************************************
  envir.c 20 replace /users/mjc/translation
  870524 09:37:38 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  envir.c 21 replace /users/jck/system
  870605 16:06:36 jck Added known objects to support associative lists

 ************************************************************************/
/************************************************************************
  envir.c 22 replace /users/mjc/system
  870607 03:08:05 mjc Shorten 'ENVIR_KCPD_TheDefaultTVAListProp' to <= 31 characters

 ************************************************************************/
/************************************************************************
  envir.c 23 replace /users/cmm/system
  870821 11:02:33 cmm compactor: added omitlist, minseg formula

 ************************************************************************/
/************************************************************************
  envir.c 24 replace /users/cmm/system
  871006 14:43:02 cmm added stats-only option to compactor, initial MSS back to orig rt. seg of previous SVD, mjc's MSS override

 ************************************************************************/
/************************************************************************
  envir.c 25 replace /users/m2/backend
  890503 15:06:09 m2 VMS/UNIX pathname fixes for OSDPathName

 ************************************************************************/

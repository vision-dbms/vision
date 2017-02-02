/*****  Dictionary Primitives Facility Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfDICTIONARY

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "VFragment.h"

#include "selector.h"
#include "uvector.h"

#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"

#include "RTblock.h"
#include "RTdictionary.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTptoken.h"
#include "RTvector.h"

#include "RTcharuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"

#include "VCollectionOf.h"
#include "VCollectionOfStrings.h"

/*****  Self  *****/
#include "PFdictionary.h"


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
 *	  77 -  79			*
 *	 111 - 114			*
 *	 151 - 153                      *
 *======================================*/

/***************************************************
 *****  Property Update and Access Primitives  *****
 ***************************************************/

#define XLocateOrAddDictionaryPropertySlot	 77
#define XLocateDictionaryPropertySlot		 78
#define XDeleteDictionaryPropertySlot		 79

/***************************************************
 *****  Selector Update and Access Primitives  *****
 ***************************************************/

#define XLocateOrAddEntryInDictionaryOf		111
#define XLocateEntryInDictionaryOf		112
#define XDeleteFromDictionaryOf			113
#define XDictionaryContents			114

/*******************************************************
 *****  String Store Update and Access Primitives  *****
 *******************************************************/

#define XLocateOrAddToStringStore		151
#define XLocateInStringStore			152
#define XDeleteFromStringStore			153


/***********************************
 ***********************************
 *****  Primitive Definitions  *****
 ***********************************
 ***********************************/

/************************************************
 *****  Property Slot Definition Primitive  *****
 ************************************************/

V_DefinePrimitive (LocateOrAddDictionaryPropertySlot) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pDictionary = rCurrent.storeCPD ();

    if (RTYPE_C_Dictionary != pDictionary->RType ()) {
	pTask->loadDucWithNA ();
	return;
    }

    DSC_Descriptor propertySlotDescriptor;

    M_CPD *pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD (pDictionary);
    if (rCurrent.holdsAScalarReference ()) {
	rtREFUV_Type_Reference propertySlotReference;

	rtLINK_LocateOrAddFromRef (
	    pPropertySubsetCPD,
	    &DSC_Descriptor_Scalar (rCurrent),
	    &propertySlotReference
	);

	propertySlotDescriptor.constructConstant (
	    V_TOTSC_PToken,
	    rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary),
	    propertySlotReference
	);
    }
    else if (rCurrent.holdsALink ()) {
	propertySlotDescriptor.constructLink (
	    rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary), rtLINK_LocateOrAddFromLC (
		pPropertySubsetCPD, DSC_Descriptor_Link (rCurrent), NilOf (rtLINK_CType**)
	    )
	);
    }
    else {
	rCurrent.complainAboutBadPointerType ("LocateOrAddDictionaryPropertySlot");
	propertySlotDescriptor.construct ();
    }
    rtDICTIONARY_Align (pDictionary);

    pPropertySubsetCPD->release ();

    pTask->loadDucWithMoved (propertySlotDescriptor);
}


/********************************************
 *****  Property Slot Access Primitive  *****
 ********************************************/

V_DefinePrimitive (LocateDictionaryPropertySlot) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pDictionary = rCurrent.storeCPD ();

    if (RTYPE_C_Dictionary != pDictionary->RType ()) {
	pTask->loadDucWithNA ();
	return;
    }

    M_CPD *pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD (pDictionary);
    if (rCurrent.holdsAScalarReference ()) {
	rtREFUV_Type_Reference propertySlotReference;

	if (rtLINK_LookupUsingRefKey (
		pPropertySubsetCPD,
		&DSC_Descriptor_Scalar (rCurrent),
		rtLINK_LookupCase_EQ,
		INT_MAX,
		&propertySlotReference
	    )
	)
	{
	    DSC_Descriptor propertySlotDescriptor;
	    propertySlotDescriptor.constructConstant (
		V_TOTSC_PToken,
		rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary),
		propertySlotReference
	    );
	    pTask->loadDucWithMoved (propertySlotDescriptor);
	}
	else
	    pTask->loadDucWithNA ();
    }
    else if (rCurrent.holdsALink ()) {
	rtLINK_CType *pLocationsLC;
	rtLINK_CType *pLocatedLC;
	rtLINK_LookupUsingLCKey (
	    pPropertySubsetCPD,
	    DSC_Descriptor_Link (rCurrent),
	    rtLINK_LookupCase_EQ,
	    INT_MAX,
	    &pLocationsLC,
	    &pLocatedLC
	);

	VDescriptor* pGuardedDatum = pTask->loadDucWithGuardedDatum (pLocatedLC);
	if (pGuardedDatum) {
	    DSC_Descriptor iGuardedDatum;
	    iGuardedDatum.constructLink (
		rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary), pLocationsLC
	    );
	    pGuardedDatum->setToMoved (iGuardedDatum);
	}
	else
	    pLocationsLC->release ();
    }
    else rCurrent.complainAboutBadPointerType ("LocateOrAddDictionaryPropertySlot");

    pPropertySubsetCPD->release ();
}


/**********************************************
 *****  Property Slot Deletion Primitive  *****
 **********************************************/

V_DefinePrimitive (DeleteDictionaryPropertySlot) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pDictionary = rCurrent.storeCPD ();

    if (RTYPE_C_Dictionary != pDictionary->RType ()) {
	pTask->loadDucWithNA ();
	return;
    }

    M_CPD *pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD (pDictionary);
    if (rCurrent.holdsAScalarReference ()) {
	pTask->loadDucWithBoolean (
	    rtLINK_DeleteRefSelectedElement (
		pPropertySubsetCPD, &DSC_Descriptor_Scalar (rCurrent)
	    )
	);
    }
    else if (rCurrent.holdsALink ()) {
	rtLINK_CType *pDeletedLC;
	rtLINK_DeleteLCSelectedElements (
	    pPropertySubsetCPD, DSC_Descriptor_Link (rCurrent), &pDeletedLC
	);
	pTask->loadDucWithPredicateResult (pDeletedLC, pDeletedLC->Complement ());
    }
    else {
	rCurrent.complainAboutBadPointerType ("LocateOrAddDictionaryPropertySlot");
    }
    pPropertySubsetCPD->release ();
}


/*****************************************************
 *****  Associative Result Generation Utilities  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Utility to return a 'Locate' primitive scalar result.
 *
 *  Arguments:
 *	pDictionary		- a standard CPD for the dictionary searched.
 *	rAssociativeResult	- a reference to an associative result object.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LoadAWithScalarLocateResult (
    VPrimitiveTask *pTask, M_CPD *pDictionary, VAssociativeResult &rAssociation
) {
    if (!rAssociation.targetReferenceIsValid ())
	pTask->loadDucWithNA ();
    else {
	M_CPD *pDictionaryPToken = rAssociation.targetPToken ();
	pDictionary->retain ();
	pDictionaryPToken->retain ();
	pTask->loadDucWithReference (
	    pDictionary, pDictionaryPToken, rAssociation.targetReferenceScalar ()
	);
    }
}


/*---------------------------------------------------------------------------
 *****  Utility to return a 'Locate' primitive collection result.
 *
 *  Arguments:
 *	pDictionary		- a standard CPD for the dictionary searched.
 *	pDistribution		- a standard CPD for the distribution u-vector
 *				  to be applied to the result.  'distribution'
 *				  will be ASSIGNED into the result.
 *	pLocatedLC		- an optional ('Nil' if absent) address of a
 *				  link constructor specifying the selectors
 *				  (relative to the positional state of
 *				  'distribution') actually found.  'pLocatedLC'
 *                                will be ASSIGNED into the result.
 *	pLocationsLC		- the address of a link constructor specifying
 *				  the dictionary positions of the found
 *				  selectors.  'pLocationsLC' will be ASSIGNED
 *				  into the result.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LoadAWithGeneralLocateResult (
    VPrimitiveTask*		pTask,
    M_CPD*			pDictionary,
    M_CPD*			pDistribution,
    rtLINK_CType*		pLocatedLC,
    rtLINK_CType*		pLocationsLC
)
{
    VDescriptor* pGuardedDatum = pTask->loadDucWithGuardedDatum (pLocatedLC);
    if (pGuardedDatum) {
	pDictionary->retain ();

	DSC_Descriptor iGuardedDatum;
	iGuardedDatum.constructLink (pDictionary, pLocationsLC);

	pGuardedDatum->setToMoved (iGuardedDatum);

	if (pDistribution) {
	    pTask->distributeDuc (pDistribution);
	    pDistribution->release ();
	}
    }
    else {
	if (pDistribution)
	    pDistribution->release ();
	pLocationsLC->release ();
    }
}


/*************************************
 *****  Set Insertion Primitive  *****
 *************************************/

V_DefinePrimitive (LocateOrAddToSet) {
    static char const *const pThisPrimitive = "LocateOrAddToSet";

/*****  Acquire and decode the argument...  *****/
    M_CPD *pSet;
    switch (V_TOTSC_Primitive) {
    case XLocateOrAddEntryInDictionaryOf:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent (KS__LocateOrAddDictionaryEntry4);
	    return;
	}
	pSet = pTask->ducDictionary ();
	break;

    case XLocateOrAddToStringStore:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent ("clusterInsert:");
	    return;
	}
	pSet = pTask->ducStore ();
	pSet->retain ();
	break;

    default:
	pTask->raiseUnimplementedAliasException (pThisPrimitive);
	break;
    }

/*****  ...decode the key type, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pKeyStore = rCurrent.storeCPD ();

    RTYPE_Type xKeyPointerRType = rCurrent.pointerRType ();

/*****  ... perform the operation, ...  *****/
    VAssociativeResult iAssociation (true);
    M_CPD *pDistribution;
    switch (pKeyStore->RType ()) {
    case RTYPE_C_ValueStore:
	if (RTYPE_C_IntUV == xKeyPointerRType) {
	    VCollectionOfUnsigned32 iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.insertInto (pSet, pDistribution, iAssociation);
	}
	break;

    case RTYPE_C_Block:
    case RTYPE_C_ListStore: {
	    VCollectionOfStrings iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.insertInto (pSet, pDistribution, iAssociation);
	}
	break;

    default:
	pSet->release ();
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "%s: Not A String/Selector Store: '%s'",
	    pThisPrimitive,
	    pKeyStore->RTypeName ()
	);
	break;
    }

/*****  ... and return the result:  *****/
    if (!iAssociation.isValid ())
	pTask->loadDucWithNA ();
    else if (iAssociation.isScalar ())
	LoadAWithScalarLocateResult (pTask, pSet, iAssociation);
    else LoadAWithGeneralLocateResult (
	pTask,
	pSet,
	pDistribution,
	NilOf (rtLINK_CType*),
	iAssociation.claimedTargetReference ()
    );
    pSet->release ();
}


/**************************************************
 *****  String/Selector Set Locate Primitive  *****
 **************************************************/

V_DefinePrimitive (LocateInSet) {
    static char const *const pThisPrimitive = "LocateInSet";

/*****  Acquire and decode the argument...  *****/
    M_CPD *pSet;
    switch (V_TOTSC_Primitive) {
    case XLocateEntryInDictionaryOf:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent (KS__LocateDictionaryEntryFor);
	    return;
	}
	pSet = pTask->ducDictionary ();
	break;
    case XLocateInStringStore:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent ("clusterLocate:");
	    return;
	}
	pSet = pTask->ducStore ();
	pSet->retain ();
	break;
    default:
	pTask->raiseUnimplementedAliasException (pThisPrimitive);
	break;
    }

/*****  ...decode the selector type presented to this routine, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pKeyStore = rCurrent.storeCPD ();

    RTYPE_Type xKeyPointerRType = rCurrent.pointerRType ();

/*****  ... perform the operation, ...  *****/
    M_CPD *pDistribution;
    VAssociativeResult iAssociation (true);
    switch (pKeyStore->RType ()) {
    case RTYPE_C_ValueStore:
	if (RTYPE_C_IntUV == xKeyPointerRType) {
	    VCollectionOfUnsigned32 iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.locateIn (pSet, pDistribution, iAssociation);
	}
	break;

    case RTYPE_C_Block:
    case RTYPE_C_ListStore: {
	    VCollectionOfStrings iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.locateIn (pSet, pDistribution, iAssociation);
	}
	break;

    default:
	pSet->release ();
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "%s: Not A String/Selector Store: '%s'",
	    pThisPrimitive,
	    pKeyStore->RTypeName ()
	);
	break;
    }

/*****  ... and return the result:  *****/
    if (!iAssociation.isValid ())
	pTask->loadDucWithNA ();
    else if (iAssociation.isScalar ())
	LoadAWithScalarLocateResult (pTask, pSet, iAssociation);
    else LoadAWithGeneralLocateResult (
	pTask,
	pSet,
	pDistribution,
	iAssociation.claimedSourceReference (),
	iAssociation.claimedTargetReference ()
    );
    pSet->release ();
}


/**************************************************
 *****  String/Selector Set Delete Primitive  *****
 **************************************************/

V_DefinePrimitive (DeleteFromSet) {
    static char const *const pThisPrimitive = "DeleteFromSet";

/*****  Acquire and decode the argument...  *****/
    M_CPD *pSet;
    switch (V_TOTSC_Primitive) {
    case XDeleteFromDictionaryOf:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent (KS__DeleteDictionaryEntryFor);
	    return;
	}
	pSet = pTask->ducDictionary ();
	break;
    case XDeleteFromStringStore:
	if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	    pTask->sendBinaryConverseWithCurrent ("clusterDelete:");
	    return;
	}
	pSet = pTask->ducStore ();
	pSet->retain ();
	break;
    default:
	pTask->raiseUnimplementedAliasException (pThisPrimitive);
	break;
    }

/*****  ...decode the selector type presented to this routine, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pKeyStore = rCurrent.storeCPD ();

    RTYPE_Type xKeyPointerRType = rCurrent.pointerRType ();

/*****  ... perform the operation, ...  *****/
    M_CPD *pDistribution;
    VAssociativeResult iAssociation (true);
    switch (pKeyStore->RType ()) {
    case RTYPE_C_ValueStore:
	if (RTYPE_C_IntUV == xKeyPointerRType) {
	    VCollectionOfUnsigned32 iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.deleteFrom (pSet, pDistribution, iAssociation);
	}
	break;

    case RTYPE_C_Block:
    case RTYPE_C_ListStore: {
	    VCollectionOfStrings iKeys (V_TOTSC_PToken, rCurrent);
	    iKeys.deleteFrom (pSet, pDistribution, iAssociation);

	}
	break;

    default:
	pSet->release ();
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "%s: Not A String/Selector Store: '%s'",
	    pThisPrimitive,
	    pKeyStore->RTypeName ()
	);
	break;
    }

/*****  ... and return the result:  *****/
    if (!iAssociation.isValid ())
	pTask->loadDucWithNA ();
    else if (iAssociation.isScalar ())
	pTask->loadDucWithBoolean (iAssociation.sourceReferenceIsValid ());
    else {
	rtLINK_CType *pDeletedLC = iAssociation.claimedSourceReference ();
	pTask->loadDucWithPredicateResult (pDeletedLC, pDeletedLC->Complement ());
	if (pDistribution) {
	    pTask->distributeDuc (pDistribution);
	    pDistribution->release ();
	}
    }
    pSet->release ();
}


/********************************************
 *****  Selector Enumeration Primitive  *****
 ********************************************/

V_DefinePrimitive (DictionaryContents) {
/*****  Obtain the result's container space, ... *****/
    M_ASD *pContainerSpace = pTask->codScratchPad ();

/*****  Obtain the dictionary's contents, ... *****/
    M_CPD *pContentVector; {
	M_CPD *pDictionary = pTask->getSelf ().dictionaryCPD ();
	pContentVector = rtDICTIONARY_Contents (pContainerSpace, pDictionary);
	pDictionary->release ();
    }

/*****  Create an l-store around the vector, ... *****/
    M_CPD *pLStorePTokenCPD = rtPTOKEN_New (pContainerSpace, 1);
    rtLINK_CType *pLStoreDefinitionLC = rtLINK_RefConstructor (pLStorePTokenCPD, -1);
    rtLINK_AppendRepeat (
	pLStoreDefinitionLC, 0, rtVECTOR_CPD_ElementCount (pContentVector)
    );
    pLStoreDefinitionLC->Close (pContentVector, rtVECTOR_CPx_PToken);

    M_CPD *pLStoreCPD = rtLSTORE_NewCluster (pLStoreDefinitionLC, pContentVector);

    pContentVector->release ();
    pLStoreDefinitionLC->release ();

/*****  Load the Duc and return ... *****/
    pTask->loadDucWithReference (pLStoreCPD, pLStorePTokenCPD, 0);
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

/***************************************************
 *****  Property Update and Access Primitives  *****
 ***************************************************/

    PD (XLocateOrAddDictionaryPropertySlot,
	"LocateOrAddDictionaryPropertySlot",
	LocateOrAddDictionaryPropertySlot)
    PD (XLocateDictionaryPropertySlot,
	"LocateDictionaryPropertySlot",
	LocateDictionaryPropertySlot)
    PD (XDeleteDictionaryPropertySlot,
	"DeleteDictionaryPropertySlot",
	DeleteDictionaryPropertySlot)

/***************************************************
 *****  Selector Update and Access Primitives  *****
 ***************************************************/

    PD (XLocateOrAddEntryInDictionaryOf,
	"LocateOrAddEntryInDictionaryOf",
	LocateOrAddToSet)
    PD (XLocateEntryInDictionaryOf,
	"LocateEntryInDictionaryOf",
	LocateInSet)
    PD (XDeleteFromDictionaryOf,
	"DeleteFromDictionaryOf",
	DeleteFromSet)

    PD (XDictionaryContents,
	"DictionaryContents",
	DictionaryContents)


/*******************************************************
 *****  String Store Update and Access Primitives  *****
 *******************************************************/

    PD (XLocateOrAddToStringStore,
	"XLocateOrAddToStringStore",
	LocateOrAddToSet)
    PD (XLocateInStringStore,
	"XLocateInStringStore",
	LocateInSet)
    PD (XDeleteFromStringStore,
	"XDeleteFromStringStore",
	DeleteFromSet)

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
PublicFnDef void pfDICTIONARY_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfDICTIONARY);
    FAC_FDFCase_FacilityDescription ("Dictionary Primitives Implementation");
    default:
        break;
    }
}

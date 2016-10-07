/*****  MethodDictionary Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtDICTIONARY

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "popvector.h"
#include "ts.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"

#include "RTdsc.h"
#include "RTindex.h"
#include "RTlink.h"
#include "RTptoken.h"
#include "RTrefuv.h"
#include "RTvector.h"

#include "VSelector.h"

/*****  Self  *****/
#include "RTdictionary.h"


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

DEFINE_CONCRETE_RTT (rtDICTIONARY_Handle);


/*******************************************
 *******************************************
 *****  rtDICTIONARY_Cache Use Switch  *****
 *******************************************
 *******************************************/

PublicVarDef bool rtDICTIONARY_UsingCache = true;

/********************************
 ********************************
 *****  rtDICTIONARY_Cache  *****
 ********************************
 ********************************/

/***************************
 *****  Run Time Type  *****
 ***************************/

DEFINE_CONCRETE_RTT (rtDICTIONARY_Cache);

/**************************
 *****  Construction  *****
 **************************/

rtDICTIONARY_Cache::rtDICTIONARY_Cache (M_CPD *pCPD)
: m_iSelectors		(pCPD, rtDICTIONARY_CPx_Selectors)
, m_pBindings		(pCPD, rtDICTIONARY_CPx_Values		  , RTYPE_C_Vector)
, m_pPropertySubset	(pCPD, rtDICTIONARY_CPx_PropertySubset	  , RTYPE_C_Link)
, m_pPropertyPrototypes (pCPD, rtDICTIONARY_CPx_PropertyPrototypes, RTYPE_C_Vector)
{
    pCPD->SetTransientExtensionTo (this);
}

/*****************************************
 *****  Transient Extension Support  *****
 *****************************************/

void rtDICTIONARY_Cache::FlushCacheAsTransientExtensionOf (VContainerHandle *pContainerHandle) {
    pContainerHandle->ClearTransientExtension ();
}


/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new, empty method dictionary.
 *
 *  Arguments:
 *	pContainerSpace		- the ASD for the space in which to create
 *				  the dictionary.
 *
 *  Returns:
 *	The address of a CPD for the method dictionary created.
 *
 *  Notes:
 *	This routine uses TheSelectorPToken known object so that it can be
 *	safely called during the bootstrap.  The alternative used elsewhere,
 *	TheSelectorClass.PToken(), is obtained from the Selector class and
 *	thus isn't valid until that class has been created and registered.
 *	In contrast, TheSelectorPToken known object is the second object created
 *	and registered by the bootstrap.  If both of these known CPDs are
 *	set, they should refer to the same object since the bootstrap uses
 *	TheSelectorPToken known object to build the Selector class.
 *
 *****/
PublicFnDef M_CPD *rtDICTIONARY_New (M_ASD *pContainerSpace) {
    M_CPD *pSelectorPToken = rtPTOKEN_New (pContainerSpace, 0);
    M_CPD *pSelectorUVector = rtSELUV_New (
	pSelectorPToken, pContainerSpace->TheSelectorPToken ()
    );
    M_CPD *pSelectorValues = rtVECTOR_New (pSelectorPToken);

    M_CPD *pPropertyPToken = rtPTOKEN_New (pContainerSpace, 0);
    M_CPD *pPropertySubset = rtLINK_NewEmptyLink (pPropertyPToken, pSelectorPToken);
    M_CPD *pPropertyPrototypes = rtVECTOR_New (pPropertyPToken);

    M_CPD *pDictionary = POPVECTOR_New (
	pContainerSpace, RTYPE_C_Dictionary, rtDICTIONARY_FixedElementCount
    );
    pDictionary->StoreReference (rtDICTIONARY_CPx_Selectors		, pSelectorUVector);
    pDictionary->StoreReference (rtDICTIONARY_CPx_Values		, pSelectorValues);
    pDictionary->StoreReference (rtDICTIONARY_CPx_PropertyPToken	, pPropertyPToken);
    pDictionary->StoreReference (rtDICTIONARY_CPx_PropertySubset	, pPropertySubset);
    pDictionary->StoreReference (rtDICTIONARY_CPx_PropertyPrototypes	, pPropertyPrototypes);

    pPropertyPrototypes ->release ();
    pPropertySubset	->release ();
    pPropertyPToken	->release ();
    pSelectorValues	->release ();
    pSelectorUVector	->release ();
    pSelectorPToken	->release ();

    return pDictionary;
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/********************
 *****  Simple  *****
 ********************/

PublicFnDef M_CPD *rtDICTIONARY_Align (M_CPD *pDictionary) {
    M_CPD *pPropertySubset = rtDICTIONARY_CPD_PropertySubsetCPD (pDictionary);

    rtLINK_AlignLink (pPropertySubset);
    if (pDictionary->ReferenceDoesntName (
	    rtDICTIONARY_CPx_PropertyPToken, pPropertySubset, rtLINK_CPx_PosPToken
	)
    ) {
	pDictionary->StoreReference (
	    rtDICTIONARY_CPx_PropertyPToken, pPropertySubset, rtLINK_CPx_PosPToken
       );
    }
    pPropertySubset->release ();

    return pDictionary;
}

/***********************
 *****  Recursive  *****
 ***********************/

PublicFnDecl bool rtDICTIONARY_AlignAll (
    M_CPD *pDictionary, bool deletingEmptyUSegments
) {
    rtDICTIONARY_Align (pDictionary);

    M_CPD *pVector = rtDICTIONARY_CPD_ValuesCPD (pDictionary);
    bool result = rtVECTOR_AlignAll (pVector, deletingEmptyUSegments);
    pVector->release ();

    pVector = rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary);
    result = rtVECTOR_AlignAll (pVector, deletingEmptyUSegments) || result;
    pVector->release ();

    return result;
}


/**************************
 **************************
 *****  Value Update  *****
 **************************
 **************************/

/*****************************************************
 *****  Compatibility Mode Property Maintenance  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Property Subset Maintenance
 *
 *  Argument:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pSelectorValues		- the address of a CPD for the dictionary's
 *				  value vector (optimization).
 *
 *  Returns:
 *	Nothing
 *
 *****/
PrivateFnDef void MaintainPropertySubset (M_CPD *pDictionary, M_CPD *pSelectorValues) {
    class WiredPropertyDescriptor {
    //  Type Definitions
    public:
	typedef M_CPD *(*Factory)(M_CPD*);
	typedef M_KOTE M_KnownObjectTable::*KOTMM;  //  Known Object Table Mutable Member


    //  Construction
    public:
	WiredPropertyDescriptor (M_KOTM pSpecification, KOTMM pPrototype, Factory pFactory)
	: m_pSpecification (pSpecification), m_pPrototype (pPrototype), m_pFactory (pFactory)
	{
	}

    //  Access
    public:
	M_KOTM specification () const {
	    return m_pSpecification;
	}
	KOTMM prototype () const {
	    return m_pPrototype;
	}
	Factory factory () const {
	    return m_pFactory;
	}

    //  Properties
    private:
	M_KOTM	const	m_pSpecification;
	KOTMM	const	m_pPrototype;
	Factory	const	m_pFactory;
    };
    
    static WiredPropertyDescriptor iFixedProperty (
	&M_KnownObjectTable::TheFixedPropertySpecificationClass,
	&M_KnownObjectTable::TheFixedPropertyPrototype,
	rtVECTOR_New
    );
    static WiredPropertyDescriptor iTimeSeriesProperty (
	&M_KnownObjectTable::TheTimeSeriesPropertySpecificationClass,
	&M_KnownObjectTable::TheTimeSeriesPropertyPrototype,
	rtINDEX_NewTimeSeries
    );
    static WiredPropertyDescriptor const *WiredProperties[] = {
	&iFixedProperty, &iTimeSeriesProperty
    };
    enum {
	WiredPropertyCount = sizeof (WiredProperties) / sizeof (WiredProperties[0])
    };

    M_KnownObjectTable *pKOT = pDictionary->KnownObjectTable ();
/*---------------------------------------------------------------------------*
 *  This routine will be called during the construction of the known object  *
 *  table when pre-6.x database versions are opened.  Since the KOT isn't    *
 *  available at that point, the following test is required:                 *
 *---------------------------------------------------------------------------*/
    if (IsNil (pKOT))
	return;

    VCPDReference pPropertySubset;
    VCPDReference pPropertyPrototypes;

    for (unsigned int i = 0; i < WiredPropertyCount; i++) {
	WiredPropertyDescriptor const *pWPD = WiredProperties[i];

	rtLINK_CType *pPropertySubsetLC = rtVECTOR_SubsetOfType (
	    pSelectorValues, pDictionary->ScratchPad (), pWPD->specification ()
	);
/*****  If this property type exists in the dictionary, ...  *****/
	if (pPropertySubsetLC) {
	    if (pPropertySubset.isEmpty ())
		pPropertySubset.claim (rtDICTIONARY_CPD_PropertySubsetCPD (pDictionary));

	    rtLINK_CType *pAdditionsLC;
	    rtLINK_CType *pLocationsLC = rtLINK_LocateOrAddFromLC (
		pPropertySubset, pPropertySubsetLC, &pAdditionsLC
	    );
/*****  ... and new occurences of it were added, ...  *****/
	    if (pAdditionsLC) {
/*****  ... fix the dictionary's property p-token, ...  *****/
		pDictionary->StoreReference (
		    rtDICTIONARY_CPx_PropertyPToken, pLocationsLC->RPT ()
		);

/*****  ... identify the new property prototype slots, ...  *****/
		rtLINK_CType *pPrototypeAdditionsLC = pLocationsLC->Extract (pAdditionsLC);

/*****  ... construct an appropriate prototype if one doesn't exist, ...  *****/
		M_KOTE *pPrototypeKOTE = &(pKOT->*(pWPD->prototype ()));
		if (pPrototypeKOTE->IsntSet ()) {
		    M_CPD *pPTokenCPD = rtPTOKEN_New (pKOT->Space (), 1);
		    pPrototypeKOTE->RegisterObject (pKOT, (pWPD->factory ())(pPTokenCPD));
		    pPTokenCPD->release ();
		}

/*****  ... construct a descriptor containing their initial value, ...  *****/
		DSC_Descriptor prototypeDescriptor;
		prototypeDescriptor.constructReferenceConstant (
		    pPrototypeAdditionsLC->PPT (),
		    pPrototypeKOTE->RetainedObjectCPD (),
		    pPrototypeKOTE->RetainedPTokenCPD (),
		    0
		);

/*****  ... initialize the slots, ...  *****/
		if (pPropertyPrototypes.isEmpty ()) pPropertyPrototypes.claim (
		    rtDICTIONARY_CPD_PropertyPrototypesCPD (pDictionary)
		);
		rtVECTOR_Assign (
		    pPropertyPrototypes, pPrototypeAdditionsLC, &prototypeDescriptor
		);

/*****  ... release the addition structures, ...  *****/
		prototypeDescriptor.clear ();
		pPrototypeAdditionsLC->release ();
		pAdditionsLC->release ();
	    }

/*****  ... and release the search structures.  *****/
	    pLocationsLC->release ();
	    pPropertySubsetLC->release ();
	}
    }
}


/***********************************
 *****  Assignment Operations  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pElementSelector	- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *	pValues			- a pointer to a vector constructor supplying
 *				  the values being assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source;
 *
 *	  for scalar source values and:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *	  for u-vector source values.
 *
 *****/
PublicFnDef void rtDICTIONARY_Assign (
    M_CPD *pDictionary, rtLINK_CType *pElementSelector, rtVECTOR_CType *pValues
) {
    M_CPD *pSelectorValues = rtDICTIONARY_CPD_ValuesCPD (pDictionary);

    rtVECTOR_Assign		(pSelectorValues, pElementSelector, pValues);
    MaintainPropertySubset	(pDictionary, pSelectorValues);

    pSelectorValues->release ();
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pElementSelector	- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *	pValues			- a pointer to a Descriptor supplying
 *				  the values being assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source;
 *
 *	  for scalar source values and:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *	  for u-vector source values.
 *
 *****/
PublicFnDef void rtDICTIONARY_Assign (
    M_CPD *pDictionary, rtLINK_CType *pElementSelector, DSC_Descriptor *pValues
) {
    M_CPD *pSelectorValues = rtDICTIONARY_CPD_ValuesCPD (pDictionary);
 
    rtVECTOR_Assign		(pSelectorValues, pElementSelector, pValues);
    MaintainPropertySubset	(pDictionary, pSelectorValues);

    pSelectorValues->release ();
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pElementSelector	- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *	pValues			- a pointer to a vector constructor supplying
 *				  the values being assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source;
 *
 *	  for scalar source values and:
 *
 *	    for (i = 0; i < ElementCount (link); i++)
 *		target[link[i]] = source[i];
 *
 *	  for u-vector source values.
 *
 *****/
PublicFnDef void rtDICTIONARY_Assign (
    M_CPD *pDictionary, rtREFUV_TypePTR_Reference pElementSelector, rtVECTOR_CType *pValues
) {
    M_CPD *pSelectorValues = rtDICTIONARY_CPD_ValuesCPD (pDictionary);

    rtVECTOR_Assign		(pSelectorValues, pElementSelector, pValues);
    MaintainPropertySubset	(pDictionary, pSelectorValues);

    pSelectorValues->release ();
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pElementSelector	- the address of a reference identifying the
 *				  element being changed.
 *	pValues			- a pointer to a Descriptor supplying
 *				  the values to be assigned.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the target vector.
 *
 *  Notes:
 *	- This routine performs the equivalent of:
 *
 *		target[reference] = source
 *
 *	  for scalar source values and:
 *
 *		target[reference] = source[elementCount (source) - 1];
 *
 *	  for u-vector sources values.
 *
 *****/
PublicFnDef void rtDICTIONARY_Assign (
    M_CPD *pDictionary, rtREFUV_TypePTR_Reference pElementSelector, DSC_Descriptor *pValues
) {
    M_CPD *pSelectorValues = rtDICTIONARY_CPD_ValuesCPD (pDictionary);

    rtVECTOR_Assign		(pSelectorValues, pElementSelector, pValues);
    MaintainPropertySubset	(pDictionary, pSelectorValues);

    pSelectorValues->release ();
}


/**********************************
 **********************************
 *****  Bound Name Insertion  *****
 **********************************
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to define a (known-selector, value) pair in a dictionary.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pSelector		- the address of a 'VSelector'.
 *	pValue			- the address of the value being assigned to the
 *				  selector.
 *
 *  Returns:
 *	Nothing
 *
 *****/
PublicFnDef void rtDICTIONARY_Define (
    M_CPD *pDictionary, VSelector const *pSelector, DSC_Descriptor *pValue
)
{
/*****  Locate the specified selector, ...  *****/
    unsigned int xElement;
    M_CPD *pSetPToken;
    if (rtDICTIONARY_UsingCache) {
	VReference<rtDICTIONARY_Cache> pCache (
	    rtDICTIONARY_Cache::GetCacheOf (pDictionary)
	);
	rtSELUV_Set &rSet = pCache->selectors ();

	pSelector->insertIn (rSet, xElement);

	pSetPToken = rSet.ptoken ();
	pSetPToken->retain ();
    }
    else {
	rtSELUV_Set iSet (pDictionary, rtDICTIONARY_CPx_Selectors);

	pSelector->insertIn (iSet, xElement);

	pSetPToken = iSet.ptoken ();
	pSetPToken->retain ();
    }
    rtREFUV_Type_Reference assignmentRef;
    DSC_InitReferenceScalar (assignmentRef, pSetPToken, xElement);

/*****  ... update the vector, ...  *****/
    rtDICTIONARY_Assign (pDictionary, &assignmentRef, pValue);
    DSC_ClearScalar (assignmentRef);
}


/***************************
 ***************************
 *****  Other Queries  *****
 ***************************
 ***************************/

PublicFnDef M_CPD *rtDICTIONARY_SelectorPToken (M_CPD *pDictionary) {
    M_CPD *pSelectorUVector = rtDICTIONARY_CPD_SelectorsCPD (pDictionary);
    M_CPD *pSelectorPToken = UV_CPD_PosPTokenCPD (pSelectorUVector);
    pSelectorUVector->release ();

    return pSelectorPToken;
}


/*---------------------------------------------------------------------------
 *****  Routine to produce a vector containing all of the names in the
 *****	method dictionary.
 *
 *  Arguments:
 *	pDictionary    - a standard CPD for the method dictionary to be
 *			      queried.
 *
 *  Returns:
 *	A standard CPD for a vector containing all of the names in
 *      'pDictionary'.
 *
 *****/
PublicFnDef M_CPD *rtDICTIONARY_Contents (M_ASD *pContainerSpace, M_CPD *pDictionary) {
    M_CPD *pSelectorUVector = rtDICTIONARY_CPD_SelectorsCPD (pDictionary);
    M_CPD *result = rtSELUV_Contents (pContainerSpace, pSelectorUVector);
    pSelectorUVector->release ();

    return result;
}


/***************************************
 ***************************************
 *****  Internal Debugger Methods  *****
 ***************************************
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNilaryMethod (NewDM) {
    return RTYPE_QRegister (rtDICTIONARY_New (IOBJ_ScratchPad));
}

IOBJ_DefineNilaryMethod (UsingCacheDM) {
    return IOBJ_SwitchIObject (&rtDICTIONARY_UsingCache);
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nMethod Dictionary Routines Usage Counts\n");
    IO_printf (  "-----------------------------------------\n");

    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (AlignDM) {
    return RTYPE_QRegister (rtDICTIONARY_Align (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (SelectorsDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_CPD_SelectorsCPD (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (ValuesDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_CPD_ValuesCPD (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (SelectorPTokenDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_SelectorPToken (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (PropertyPTokenDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_CPD_PropertyPTokenCPD (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (PropertySubsetDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_CPD_PropertySubsetCPD (RTYPE_QRegisterCPD (self))
    );
}

IOBJ_DefineUnaryMethod (PropertyPrototypesDM) {
    return RTYPE_QRegister (
	rtDICTIONARY_CPD_PropertyPrototypesCPD (RTYPE_QRegisterCPD (self))
    );
}


IOBJ_DefineMethod (AtPutDM) {
    DSC_Descriptor value;
    if (rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[1], &value)) {
	VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray[0]));
	rtDICTIONARY_Define (RTYPE_QRegisterCPD (self), &selector, &value);
	value.clear ();
    }

    return self;
}

IOBJ_DefineMethod (AtDM) {
    VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray[0]));
    M_CPD *pDictionary = RTYPE_QRegisterCPD (self);
    DSC_Descriptor value;
    if (rtDICTIONARY_LookupResult_FoundNothing != rtDICTIONARY_Lookup (
	    pDictionary, &selector, &value, NilOf (int *)
	)
    ) {
        M_CPD *result = rtDSC_Pack (IOBJ_ScratchPad, &value);
	value.clear ();
	return RTYPE_QRegister (result);
    }
    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (IsDefinedDM) {
    VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray [0]));
    DSC_Descriptor value;
    if (rtDICTIONARY_LookupResult_FoundNothing != rtDICTIONARY_Lookup (
	    RTYPE_QRegisterCPD (self), &selector, &value, NilOf (int *)
	)
    ) {
 	value.clear ();
	return IOBJ_IntIObject (true);
    }
    return IOBJ_IntIObject (false);
}


/******************************************
 ******************************************
 *****  Representation Type Handlers  *****
 ******************************************
 ******************************************/

/************************
 *  CPD Initialization  *
 ************************/

PublicFnDef void InitStdCPD (M_CPD *pDictionary) {
    POPVECTOR_InitStdCPD (pDictionary);

    if (pDictionary->ReferenceIsNil (rtDICTIONARY_CPx_PropertyPToken)) {
	M_CPD *pSelectorValues = rtDICTIONARY_CPD_ValuesCPD (pDictionary);
	M_CPD *pSelectorPToken = rtDICTIONARY_SelectorPToken (pDictionary);

	// The property ptoken is shared with some number of value stores.
	// We want to ensure that it resides in the same space as the dictionary
	// responsible for its modifications ...
	M_CPD *pPropertyPToken = rtPTOKEN_New (pDictionary->Space (), 0);

	M_CPD *pPropertySubset = rtLINK_NewEmptyLink (pPropertyPToken, pSelectorPToken);
	M_CPD *pPropertyPrototypes = rtVECTOR_New (pPropertyPToken);

	pDictionary->StoreReference (
	    rtDICTIONARY_CPx_PropertyPToken	, pPropertyPToken
	);
	pDictionary->StoreReference (
	    rtDICTIONARY_CPx_PropertySubset	, pPropertySubset
	);
	pDictionary->StoreReference (
	    rtDICTIONARY_CPx_PropertyPrototypes , pPropertyPrototypes
	);

	MaintainPropertySubset (pDictionary, pSelectorValues);

	pPropertyPrototypes->release ();
	pPropertySubset->release ();
	pPropertyPToken->release ();
	pSelectorPToken->release ();
	pSelectorValues->release ();
    }
}


/*************************************
 *  RTYPE_C_MethodD Type Conversion  *
 *************************************/

PrivateFnDef M_CPreamble *ConvertMethodDToDictionary (
    M_ASD *pASD, M_CPreamble const *pOldDictionaryPreamble
)
{
/*****  Allocate a new container, ...  *****/
    M_CPreamble *pNewDictionaryPreamble = TS_AllocateContainer (
	RTYPE_C_Dictionary,
	POPVECTOR_SizeofPVector (rtDICTIONARY_FixedElementCount),
	M_CPreamble_POP (pOldDictionaryPreamble)
    );

/*****  ... initialize its contents, ...  *****/
    memcpy (
	M_CPreamble_ContainerBase (pNewDictionaryPreamble),
	M_CPreamble_ContainerBase (pOldDictionaryPreamble),
	(size_t)M_CPreamble_Size (pOldDictionaryPreamble)
    );

    POPVECTOR_PVType *pNewDictionaryPOPVector = M_CPreamble_ContainerBaseAsType (
	pNewDictionaryPreamble, POPVECTOR_PVType
    );
    POPVECTOR_PV_ElementCount (pNewDictionaryPOPVector) = rtDICTIONARY_FixedElementCount;

    pASD->constructReferences (
	(M_POP*)(
	    M_CPreamble_ContainerBase (
		pNewDictionaryPreamble
	    ) + M_CPreamble_Size (pOldDictionaryPreamble)
	), rtDICTIONARY_FixedElementCount - 2
    );

/*****  ... and return its new contents, ...  *****/
    return pNewDictionaryPreamble;
}


/**************************************
 *  'RTYPE_C_MethodD' R-Type Handler  *
 **************************************/

RTYPE_DefineHandler (rtDICTIONARY_ConversionHandler) {
    M_RTD *rtd;
    switch (handlerOperation) {
    case RTYPE_InitializeMData:
        rtd = iArgList.arg<M_RTD*>();
	M_RTD_ConvertFn	(rtd) = ConvertMethodDToDictionary;
        break;
    default:
        return -1;
    }
    return 0;
}


/*****************************************
 *  'RTYPE_C_Dictionary' R-Type Handler  *
 *****************************************/

RTYPE_DefineHandler (rtDICTIONARY_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new"	    		, NewDM)
	IOBJ_MDE ("usingCache"		, UsingCacheDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	POPVECTOR_StdDMDEPackage
	IOBJ_MDE ("selectors"		, SelectorsDM)
	IOBJ_MDE ("values"		, ValuesDM)
	IOBJ_MDE ("selectorPToken"	, SelectorPTokenDM)
	IOBJ_MDE ("propertyPToken"	, PropertyPTokenDM)
	IOBJ_MDE ("propertySubset"	, PropertySubsetDM)
	IOBJ_MDE ("propertyPrototypes"	, PropertyPrototypesDM)
	IOBJ_MDE ("at:put:"		, AtPutDM)
	IOBJ_MDE ("at:"			, AtDM)
	IOBJ_MDE ("isDefined:"		, IsDefinedDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtDICTIONARY_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtDICTIONARY_Handle::Maker;
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
    case RTYPE_PrintObject:
	POPVECTOR_Print (iArgList.arg<M_CPD*>(), false);
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
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtDICTIONARY);
    FAC_FDFCase_FacilityDescription
        ("MethodDictionary Representation Type Handler");
    default:
        break;
    }
}


/************************************************************************
  RTdictionary.c 2 replace /users/jck/system
  860521 12:35:58 jck First Complete Implementation

 ************************************************************************/
/************************************************************************
  RTdictionary.c 3 replace /users/jck/system
  860522 12:08:57 jck Added New Entry addition routines

 ************************************************************************/
/************************************************************************
  RTdictionary.c 4 replace /users/jck/system
  860523 15:13:35 jck Use of rtVECTOR_USVD_Type parameter revised

 ************************************************************************/
/************************************************************************
  RTdictionary.c 5 replace /users/jck/system
  860605 16:15:51 jck Updated release to support RTvstore

 ************************************************************************/
/************************************************************************
  RTdictionary.c 6 replace /users/mjc/system
  860617 18:44:10 mjc Declared 'save' routine

 ************************************************************************/
/************************************************************************
  RTdictionary.c 7 replace /users/mjc/system
  860623 10:23:35 mjc Converted to use new value descriptor macros/R-Type

 ************************************************************************/
/************************************************************************
  RTdictionary.c 8 replace /users/mjc/system
  860713 11:01:23 mjc Converted 'at:put:' to unpack 'put:' parameter with 'rtVALUEDSC_UnpackIObject'

 ************************************************************************/
/************************************************************************
  RTdictionary.c 9 replace /users/jad/system
  860819 19:29:35 jad added usage counts

 ************************************************************************/
/************************************************************************
  RTdictionary.c 10 replace /users/mjc/system
  861002 19:15:39 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  RTdictionary.c 11 replace /users/mjc/system
  870104 22:52:16 lcn Added parallel lookup and addition routines

 ************************************************************************/
/************************************************************************
  RTdictionary.c 12 replace /users/jad/system
  870128 19:04:42 jad implemented delete and contents routines

 ************************************************************************/
/************************************************************************
  RTdictionary.c 13 replace /users/mjc/system
  870215 23:02:28 lcn Changed selector u-vector code creation code to use a special ref p-token

 ************************************************************************/
/************************************************************************
  RTdictionary.c 14 replace /users/mjc/translation
  870524 09:34:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTdictionary.c 15 replace /users/jck/system
  871007 13:33:51 jck Added a marking function for the global garbage collector

 ************************************************************************/

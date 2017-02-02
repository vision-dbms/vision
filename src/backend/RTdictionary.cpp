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


/********************************
 ********************************
 *****                      *****
 *****  rtDICTIONARY_Cache  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtDICTIONARY_Cache);

/***************************
 ***************************
 *****  Usage Control  *****
 ***************************
 ***************************/

PublicVarDef bool rtDICTIONARY_UsingCache = true;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

rtDICTIONARY_Cache::rtDICTIONARY_Cache (rtDICTIONARY_Handle *pDictionary)
: m_iSelectors		(pDictionary->selectors ())
, m_pBindings		(pDictionary->values ())
, m_pPropertyPrototypes	(pDictionary->propertyPrototypes ())
, m_pPropertySubset	(0, pDictionary->propertySubset ()->GetCPD ())
{
}


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


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

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
rtDICTIONARY_Handle::rtDICTIONARY_Handle (M_ASD *pContainerSpace) : BaseClass (
    pContainerSpace, RTYPE_C_Dictionary, rtDICTIONARY_FixedElementCount
) {
    rtPTOKEN_Handle::Reference pSelectorPToken (new rtPTOKEN_Handle (pContainerSpace, 0));
    M_CPD *pSelectorUVector = rtSELUV_New (pSelectorPToken, pContainerSpace->TheSelectorPTokenHandle ());
    rtVECTOR_Handle::Reference pValues (new rtVECTOR_Handle (pSelectorPToken));

    rtPTOKEN_Handle::Reference pPropertyPToken (new rtPTOKEN_Handle (pContainerSpace, 0));
    M_CPD *pPropertySubset = rtLINK_NewEmptyLink (pPropertyPToken, pSelectorPToken);
    rtVECTOR_Handle::Reference pPropertyPrototypes (new rtVECTOR_Handle (pPropertyPToken));

    StoreReference (rtDICTIONARY_CPx_Selectors		, pSelectorUVector);
    StoreReference (rtDICTIONARY_CPx_Values		, static_cast<VContainerHandle*>(pValues));
    StoreReference (rtDICTIONARY_CPx_PropertyPToken	, pPropertyPToken);
    StoreReference (rtDICTIONARY_CPx_PropertySubset	, pPropertySubset);
    StoreReference (rtDICTIONARY_CPx_PropertyPrototypes	, static_cast<VContainerHandle*>(pPropertyPrototypes));

    pPropertySubset	->release ();
    pSelectorUVector	->release ();
}

rtDICTIONARY_Handle::rtDICTIONARY_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    if (ReferenceIsNil (rtDICTIONARY_CPx_PropertyPToken)) {
	retain ();
	CreatePropertySubset ();
	untain ();
    }
}


/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool rtDICTIONARY_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.setTo (static_cast<rtVSTORE_Handle*>(KOT ()->TheFixedPropertyClass.ObjectHandle ()));
    return true;
}

/*******************
 *******************
 *****  Cache  *****
 *******************
 *******************/

rtDICTIONARY_Cache *rtDICTIONARY_Handle::cache () {
    if (m_pCache.isNil ())
	m_pCache.setTo (new rtDICTIONARY_Cache (this));
    return m_pCache;
}

void rtDICTIONARY_Handle::flushCachedResources_() {
    m_pCache.clear ();
    BaseClass::flushCachedResources_();
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/********************
 *****  Simple  *****
 ********************/

bool rtDICTIONARY_Handle::align () {
    M_CPD *pPropertySubset;
    getPropertySubset (pPropertySubset);

    bool bWorkDone = false;
    rtLINK_Align (pPropertySubset);

    if (ReferenceDoesntName (rtDICTIONARY_CPx_PropertyPToken, pPropertySubset, rtLINK_CPx_PosPToken)) {
	EnableModifications ();
	StoreReference (
	    rtDICTIONARY_CPx_PropertyPToken, pPropertySubset, rtLINK_CPx_PosPToken
	);
	bWorkDone = true;
    }
    pPropertySubset->release ();
    return bWorkDone;
}

bool rtDICTIONARY_Handle::compact () {

    bool bWorkDone = false; 
	
    if (rtDICTIONARY_UsingCache) {
	rtDICTIONARY_Cache::Reference pCache (cache ());
	rtSELUV_Set &rSet = pCache->selectors ();
	bWorkDone = rSet.compact ();
	return bWorkDone;
    } else {
	rtSELUV_Set iSet (selectors ());
	bWorkDone = iSet.compact ();
	return bWorkDone;
    }
}

/***********************
 *****  Recursive  *****
 ***********************/

bool rtDICTIONARY_Handle::alignAll (bool bCleaning) {
    bool bWorkDone = align ();

    M_CPD *pVector;
    getValues (pVector);
    bWorkDone = pVector->alignAll (bCleaning) || bWorkDone;
    pVector->release ();

    getPropertyPrototypes (pVector);
    bWorkDone = pVector->alignAll (bCleaning) || bWorkDone;
    pVector->release ();
    if (bCleaning) {
	bWorkDone = compact() || bWorkDone;
    }
    return bWorkDone;
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
 *	pValues			- the address of a CPD for the dictionary's
 *				  value vector (optimization).
 *
 *  Returns:
 *	Nothing
 *
 *****/
static M_CPD *NewVector (rtPTOKEN_Handle *pPPT) {
    return (new rtVECTOR_Handle (pPPT))->GetCPD ();
}

static M_CPD *NewTimeSeries (rtPTOKEN_Handle *pPPT) {
    return rtINDEX_NewTimeSeries (pPPT)->GetCPD ();
}

void rtDICTIONARY_Handle::MaintainPropertySubset (rtVECTOR_Handle *pValues) {
    class WiredPropertyDescriptor {
    //  Type Definitions
    public:
	typedef M_CPD *(*Factory)(rtPTOKEN_Handle*);
	typedef M_KOTE M_KOT::*KOTMM;  //  Known Object Table Mutable Member

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
	&M_KOT::TheFixedPropertySpecificationClass,
	&M_KOT::TheFixedPropertyPrototype,
	&NewVector
    );
    static WiredPropertyDescriptor iTimeSeriesProperty (
	&M_KOT::TheTimeSeriesPropertySpecificationClass,
	&M_KOT::TheTimeSeriesPropertyPrototype,
	&NewTimeSeries
    );
    static WiredPropertyDescriptor const *WiredProperties[] = {
	&iFixedProperty, &iTimeSeriesProperty
    };
    enum {
	WiredPropertyCount = sizeof (WiredProperties) / sizeof (WiredProperties[0])
    };

    M_KOT *pKOT = KOT ();
/*---------------------------------------------------------------------------*
 *  This routine will be called during the construction of the known object  *
 *  table when pre-6.x database versions are opened.  Since the KOT isn't    *
 *  available at that point, the following test is required:                 *
 *---------------------------------------------------------------------------*/
    if (IsNil (pKOT))
	return;

    VCPDReference pPropertySubset;
    rtVECTOR_Handle::Reference pPropertyPrototypes;

    for (unsigned int i = 0; i < WiredPropertyCount; i++) {
	WiredPropertyDescriptor const *pWPD = WiredProperties[i];

	rtLINK_CType *pPropertySubsetLC = pValues->subsetOfType (
	    ScratchPad (), pWPD->specification ()
	);
/*****  If this property type exists in the dictionary, ...  *****/
	if (pPropertySubsetLC) {
	    if (pPropertySubset.isEmpty ()) {
		M_CPD *pCPD;
		getPropertySubset (pCPD);
		pPropertySubset.claim (pCPD);
	    }

	    rtLINK_CType *pAdditionsLC;
	    rtLINK_CType *pLocationsLC = rtLINK_LocateOrAddFromLC (
		pPropertySubset, pPropertySubsetLC, &pAdditionsLC
	    );
/*****  ... and new occurences of it were added, ...  *****/
	    if (pAdditionsLC) {
/*****  ... fix the dictionary's property p-token, ...  *****/
		EnableModifications ();
		StoreReference (
		    rtDICTIONARY_CPx_PropertyPToken, pLocationsLC->RPT ()
		);

/*****  ... identify the new property prototype slots, ...  *****/
		rtLINK_CType *pPrototypeAdditionsLC = pLocationsLC->Extract (pAdditionsLC);

/*****  ... construct an appropriate prototype if one doesn't exist, ...  *****/
		M_KOTE *pPrototypeKOTE = &(pKOT->*(pWPD->prototype ()));
		if (pPrototypeKOTE->IsntSet ()) {
		    rtPTOKEN_Handle::Reference pPToken (new rtPTOKEN_Handle (pKOT->Space (), 1));
		    pPrototypeKOTE->RegisterObject (pKOT, (pWPD->factory ())(pPToken));
		}

/*****  ... construct a descriptor containing their initial value, ...  *****/
		DSC_Descriptor prototypeDescriptor;
		prototypeDescriptor.constructReferenceConstant (
		    pPrototypeAdditionsLC->PPT (),
		    pPrototypeKOTE->store (),
		    pPrototypeKOTE->PTokenHandle (),
		    0
		);

/*****  ... initialize the slots, ...  *****/
		if (pPropertyPrototypes.isEmpty ()) {
		    getPropertyPrototypes (pPropertyPrototypes);
		}
		pPropertyPrototypes->setElements (pPrototypeAdditionsLC, prototypeDescriptor);

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
 *	pSubscript		- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *	pSource			- a pointer to a vector constructor supplying
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
void rtDICTIONARY_Handle::assign (rtLINK_CType *pSubscript, rtVECTOR_CType *pSource) {
    rtVECTOR_Handle::Reference pValues;
    getValues (pValues);

    pValues->setElements (pSubscript, pSource);
    MaintainPropertySubset (pValues);
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pSubscript		- a pointer to a link constructor specifying
 *				  the elements of 'target' to be updated.
 *	rSource			- a pointer to a Descriptor supplying
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
void rtDICTIONARY_Handle::assign (rtLINK_CType *pSubscript, DSC_Descriptor &rSource) {
    rtVECTOR_Handle::Reference pValues;
    getValues (pValues);

    pValues->setElements (pSubscript, rSource);
    MaintainPropertySubset (pValues);
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pSubscript		- a pointer to a link constructor specifying
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
void rtDICTIONARY_Handle::assign (DSC_Scalar &rSubscript, rtVECTOR_CType *pSource) {
    rtVECTOR_Handle::Reference pValues;
    getValues (pValues);

    pValues->setElements (rSubscript, pSource);
    MaintainPropertySubset (pValues);
}


/*---------------------------------------------------------------------------
 *****  Element Assignment.
 *
 *  Arguments:
 *	pDictionary		- the address of a CPD for the dictionary.
 *	pSubscript		- the address of a reference identifying the
 *				  element being changed.
 *	rSource			- a pointer to a Descriptor supplying
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
void rtDICTIONARY_Handle::assign (DSC_Scalar &rSubscript, DSC_Descriptor &rSource) {
    rtVECTOR_Handle::Reference pValues;
    getValues (pValues);

    pValues->setElements (rSubscript, rSource);
    MaintainPropertySubset (pValues);
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
 *	rValue			- the address of the value being assigned to the
 *				  selector.
 *
 *  Returns:
 *	Nothing
 *
 *****/
void rtDICTIONARY_Handle::define (VSelector const &rSelector, DSC_Descriptor &rValue) {
/*****  Locate the specified selector, ...  *****/
    unsigned int xElement;
    rtPTOKEN_Handle::Reference pSetPToken;
    if (rtDICTIONARY_UsingCache) {
	rtDICTIONARY_Cache::Reference pCache (cache ());
	rtSELUV_Set &rSet = pCache->selectors ();

	rSelector.insertIn (rSet, xElement);

	pSetPToken.setTo (rSet.ptoken ());
    }
    else {
	rtSELUV_Set iSet (selectors ());

	rSelector.insertIn (iSet, xElement);

	pSetPToken.setTo (iSet.ptoken ());
    }
    DSC_Scalar assignmentRef;
    assignmentRef.constructReference (pSetPToken, xElement);

/*****  ... update the vector, ...  *****/
    assign (assignmentRef, rValue);
    assignmentRef.destroy ();
}


/***************************
 ***************************
 *****  Other Queries  *****
 ***************************
 ***************************/

rtPTOKEN_Handle *rtDICTIONARY_Handle::getPToken () const {
    rtSELUV_Handle::Reference pSelectors (selectors ());
    return pSelectors->getPPT ();
}


/*---------------------------------------------------------------------------
 *****  Routine to produce a vector containing all of the names in the
 *****	method dictionary.
 *
 *  Returns:
 *	A standard CPD for a vector containing all of the names in
 *      'pDictionary'.
 *
 *****/
void rtDICTIONARY_Handle::getSelectorVector (rtVECTOR_Handle::Reference &rpResult, M_ASD *pContainerSpace) const {
    M_CPD *pSelectorUV;
    getSelectors (pSelectorUV);

    rtSELUV_Contents (rpResult, pContainerSpace, pSelectorUV);
    pSelectorUV->release ();
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
    return RTYPE_QRegister (new rtDICTIONARY_Handle (IOBJ_ScratchPad));
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
    RTYPE_QRegisterCPD (self)->align ();
    return self;
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
	static_cast<rtDICTIONARY_Handle*>(RTYPE_QRegisterHandle (self))->getPToken ()
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
    if (rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[1], value)) {
	VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray[0]));
	static_cast<rtDICTIONARY_Handle*>(RTYPE_QRegisterHandle (self))->define (
	    selector, value
	);
	value.clear ();
    }

    return self;
}

IOBJ_DefineMethod (AtDM) {
    DSC_Descriptor value;
    VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray[0]));
    Vdd::Store::DictionaryLookup xLookupResult = static_cast<rtDICTIONARY_Handle*> (
	RTYPE_QRegisterHandle (self)
    )->getElement (selector, &value);
    if (Vdd::Store::DictionaryLookup_FoundNothing != xLookupResult) {
	rtDSC_Handle::Reference result (new rtDSC_Handle (IOBJ_ScratchPad, value));
	value.clear ();
	return RTYPE_QRegister (result);
    }
    return IOBJ_TheNilIObject;
}

IOBJ_DefineMethod (IsDefinedDM) {
    VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray [0]));
    Vdd::Store::DictionaryLookup xLookupResult = static_cast<rtDICTIONARY_Handle*> (
	RTYPE_QRegisterHandle (self)
    )->getElement (selector);
    return IOBJ_IntIObject (Vdd::Store::DictionaryLookup_FoundNothing != xLookupResult);
}


/******************************************
 ******************************************
 *****  Representation Type Handlers  *****
 ******************************************
 ******************************************/

/************************
 *  CPD Initialization  *
 ************************/

void rtDICTIONARY_Handle::CreatePropertySubset () {
    rtVECTOR_Handle::Reference pValues;
    getValues (pValues);

    rtPTOKEN_Handle::Reference pSelectorPToken (getPToken ());

    // The property ptoken is shared with some number of value stores.
    // We want to ensure that it resides in the same space as the dictionary
    // responsible for its modifications ...
    rtPTOKEN_Handle::Reference pPropertyPToken (new rtPTOKEN_Handle (Space (), 0));

    M_CPD *pPropertySubset = rtLINK_NewEmptyLink (pPropertyPToken, pSelectorPToken);
    rtVECTOR_Handle::Reference pPropertyPrototypes (new rtVECTOR_Handle (pPropertyPToken));

    EnableModifications ();
    StoreReference (rtDICTIONARY_CPx_PropertyPToken, pPropertyPToken);
    StoreReference (rtDICTIONARY_CPx_PropertySubset, pPropertySubset);
    StoreReference (
	rtDICTIONARY_CPx_PropertyPrototypes , static_cast<VContainerHandle*>(pPropertyPrototypes)
    );
    MaintainPropertySubset (pValues);

    pPropertySubset->release ();
}


/*************************************
 *  RTYPE_C_MethodD Type Conversion  *
 *************************************/

PrivateFnDef M_CPreamble *ConvertMethodDToDictionary (
    M_ASD *pASD, M_CPreamble const *pOldDictionaryPreamble
)
{
/*****  Allocate a new container, ...  *****/
    M_CPreamble *pNewDictionaryPreamble = pASD->AllocateContainer (
	RTYPE_C_Dictionary,
	POPVECTOR_SizeofPVector (rtDICTIONARY_FixedElementCount),
	M_CPreamble_POPContainerIndex (pOldDictionaryPreamble)
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
	    M_RTD_CPDInitFn		(rtd) = POPVECTOR_InitStdCPD;
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

/*****  Value Store Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtVSTORE

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "popvector.h"
#include "uvector.h"

#include "vdbupdate.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTdictionary.h"
#include "RTlink.h"
#include "RTptoken.h"
#include "RTrefuv.h"
#include "RTvector.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTseluv.h"
#include "RTundefuv.h"

#include "RTclosure.h"
#include "RTindex.h"
#include "RTlstore.h"
#include "RTmethod.h"

#include "VGroundStore.h"
#include "VSelector.h"

/*****  Self  *****/
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

DEFINE_CONCRETE_RTT (rtVSTORE_Handle);

/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

void rtVSTORE_Handle::canonicalize (DSC_Pointer &rPointer) const {
    rtPTOKEN_Handle::Reference pRPT (getPToken ());

    rtPTOKEN_Handle::Reference pPPT (rPointer.PPT ());
    rPointer.clear ();
    rPointer.constructReferenceConstant (pPPT, pRPT, pRPT->cardinality ());
}

bool rtVSTORE_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.setTo (this);
    return false;
}

/*****************************************
 *****************************************
 *****  Transient Extension Support  *****
 *****************************************
 *****************************************/

void rtVSTORE_Handle::setTransientExtensionTo (VGroundStore *pExtension) {
    m_pExtension.setTo (pExtension);
    setPreciousTo (m_pExtension.isntNil ());
}

transientx_t *rtVSTORE_Handle::transientExtensionIfA_(VRunTimeType const &rRTT) const {
    return transientExtensionIsA_(rRTT) ? transientExtension_() : static_cast<transientx_t*>(0);
}

bool rtVSTORE_Handle::transientExtensionIsA_(VRunTimeType const &rRTT) const {
    VRunTimeType *const pRTT = m_pExtension ? m_pExtension->rtt () : static_cast<VRunTimeType*>(0);
    return pRTT && pRTT->isA (rRTT);
}


/****************************
 ****************************
 *****  Usage Counters  *****
 ****************************
 ****************************/

PrivateVarDef unsigned int
    NewCount			    = 0,
    AlignCount			    = 0,

    LookupCount			    = 0,

    AddRowsCount		    = 0,
    CloneCount			    = 0,
    ForwardCount		    = 0;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new, inheriting value store.
 *
 *  Arguments:
 *	pPPT			- the domain p-token for the new store.  If
 *				  Nil, will be obtained from 'pSuperPointer'.
 *	pDictionary		- the associated method dictionary defining the
 *				  column state of the value store.
 *	pSuperStore		- a standard CPD for a value store defining the
 *				  class level inheritance for this value store.
 *	pSuperPointer		- a standard CPD for a Link, U-Vector, or
 *				  P-Token defining the rows of the new value
 *				  store. Ignored if 'pSuperStore' is Nil.
 *
 *  Returns:
 *	The address of a CPD for the value store created.
 *
 *****/
PublicFnDef rtVSTORE_Handle *rtVSTORE_NewCluster (
    rtPTOKEN_Handle *pPPT, rtDICTIONARY_Handle *pDictionary, Vdd::Store *pSuperStore, M_CPD *pSuperPointer
) {
    NewCount++;

/*****  Decode the instance inheritance, ...  *****/
    rtPTOKEN_Handle::Reference pLocalPPT;
    if (pSuperStore) {
	int xRowPTokenRef = -1;
	switch (pSuperPointer->RType ()) {
	case RTYPE_C_Link:
	    xRowPTokenRef = rtLINK_CPx_PosPToken;
	    break;
	case RTYPE_C_CharUV:
	case RTYPE_C_DoubleUV:
	case RTYPE_C_FloatUV:
	case RTYPE_C_IntUV:
	case RTYPE_C_UndefUV:
	case RTYPE_C_Unsigned64UV:
	case RTYPE_C_Unsigned96UV:
	case RTYPE_C_Unsigned128UV:
	    xRowPTokenRef = UV_CPx_PToken;
	    break;
	default:
	    ERR_SignalFault (
		EC__UsageError, UTIL_FormatMessage (
		    "rtVSTORE_NewCluster: Invalid instance inheritance R-Type: %s",
		    pSuperPointer->RTypeName ()
		)
	    );
	    break;
	}

    /*****  ... and obtain a PPT for the new cluster, if necessary:  *****/
	if (!pPPT) {
	    pLocalPPT.setTo (
		static_cast<rtPTOKEN_Handle*>(pSuperPointer->GetContainerHandle (xRowPTokenRef, RTYPE_C_PToken))
	    );
	    pPPT = pLocalPPT;
	}
    }

/*****  ... Mark the ptoken as contolled by the value store  ... *****/
    pPPT->clearIsIndependent ();

/*****  ... Create the new V-Store ...  *****/
    rtPTOKEN_Handle::Reference pColumnPToken;
    pDictionary->getPropertyPToken (pColumnPToken);
    rtVSTORE_Handle *pResult = new rtVSTORE_Handle (
	pPPT, pDictionary, pColumnPToken, pSuperStore, pSuperPointer
    );

/*****  ... and return:  *****/
    return pResult;
}

rtVSTORE_Handle::rtVSTORE_Handle (
    rtPTOKEN_Handle *pPPT, rtDICTIONARY_Handle *pDictionary, rtPTOKEN_Handle *pColumnPToken, Vdd::Store *pSuperStore, M_CPD *pSuperPointer
) : BaseClass (
    pPPT->Space (), RTYPE_C_ValueStore, rtVSTORE_CPD_StdPtrCount + pColumnPToken->cardinality () - 1
) {
    StoreReference (rtVSTORE_CPx_RowPToken   , pPPT);
    StoreReference (rtVSTORE_CPx_Dictionary  , static_cast<VContainerHandle*>(pDictionary));
    StoreReference (rtVSTORE_CPx_ColumnPToken, pColumnPToken);

    if (pSuperStore) {
	StoreReference (rtVSTORE_CPx_InheritanceStore, pSuperStore);
	StoreReference (rtVSTORE_CPx_InheritancePtr  , pSuperPointer);
    }
}


/*****************************
 *****************************
 *****  Cluster Cloning  *****
 *****************************
 *****************************/

bool rtVSTORE_Handle::isACloneOfValueStore (rtVSTORE_Handle const *pOther) const {
    if (Names (pOther))
	return true;				//  ... same objects
    if (ReferenceDoesntName (rtVSTORE_CPx_Dictionary, pOther, pOther->element (rtVSTORE_CPx_Dictionary)))
	return false;				//  ... mismatched dictionaries
    if (hasNoInheritance ())
	return pOther->hasNoInheritance ();	//  ... end of inheritance chain for both?
    if (pOther->hasNoInheritance ())
	return false;				//  ... end of inheritance chain for other but not this.

    Vdd::Store::Reference pThisSuper;
    getInheritanceStore (pThisSuper);

    Vdd::Store::Reference pOtherSuper;
    pOther->getInheritanceStore (pOtherSuper);

    return pThisSuper->isACloneOf (pOtherSuper);//  ... keep checking.
}

void rtVSTORE_Handle::clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const {
    CloneCount++;

/*****  Create a new empty v-store using the prototype's dictionary... *****/
    rtDICTIONARY_Handle::Reference pDictionary (getDictionary ());
    rpResult.setTo (rtVSTORE_NewCluster (pPPT, pDictionary, NilOf (Vdd::Store*), NilOf (M_CPD*)));


/*****  Return if this is the end of the inheritance chain, ...  *****/
    if (hasAnInheritance ()) {
    /*****  Otherwise, construct an inheritance for the clone, ...  *****/
	Vdd::Store::Reference pPrototypeInheritanceStore;
	getInheritanceStore (pPrototypeInheritanceStore);

	M_CPD *pPrototypeInheritancePointer;
	getInheritancePointer (pPrototypeInheritancePointer);

	switch (pPrototypeInheritancePointer->RType ()) {
    /*****  ... as a reference to the current inheritance for values, ...  *****/
	case RTYPE_C_CharUV:
	case RTYPE_C_DoubleUV:
	case RTYPE_C_FloatUV:
	case RTYPE_C_IntUV:
	case RTYPE_C_UndefUV:
	case RTYPE_C_Unsigned64UV:
	case RTYPE_C_Unsigned96UV:
	case RTYPE_C_Unsigned128UV: {
		M_CPD* pCloneInheritancePointer = UV_New (
		    pPrototypeInheritancePointer->RType (),
		    pPPT,
		    pPrototypeInheritancePointer,
		    UV_CPx_RefPToken,
		    (size_t)UV_CPD_Granularity (pPrototypeInheritancePointer),
		    NilOf (Ref_UV_Initializer),
		    0
		);

		rpResult->StoreReference (rtVSTORE_CPx_InheritanceStore, pPrototypeInheritanceStore);
		rpResult->StoreReference (rtVSTORE_CPx_InheritancePtr, pCloneInheritancePointer);

		pCloneInheritancePointer->release ();
	    }
	    break;

    /*****  ... as a reference to a clone of the current inheritance for instances,  ...  *****/
	case RTYPE_C_Link: {
		rtPTOKEN_Handle::Reference pCloneInheritancePPT (new rtPTOKEN_Handle (pPPT->Space (), 0)); {
		    Vdd::Store::Reference pCloneInheritanceStore;
		    pPrototypeInheritanceStore->clone (pCloneInheritanceStore, pCloneInheritancePPT);
		    rpResult->StoreReference (rtVSTORE_CPx_InheritanceStore, pCloneInheritanceStore);
		} {
		    M_CPD *pCloneInheritancePointer = rtLINK_NewEmptyLink (
			pPPT, pCloneInheritancePPT
		    );
		    rpResult->StoreReference (rtVSTORE_CPx_InheritancePtr, pCloneInheritancePointer);
		    pCloneInheritancePointer->release ();
		}
	    }
	    break;

	default:
	    RTYPE_ComplainAboutType (
		"rtVSTORE_NewCluster:Pointer Cloning",
		pPrototypeInheritancePointer->RType (),
		RTYPE_C_ValueStore
	    );
	}
	pPrototypeInheritancePointer->release ();
    }
}


/********************************
 ********************************
 *****  Cluster Forwarding  *****
 ********************************
 ********************************/

bool rtINDEX_Handle::forwardToSpace_(M_ASD *pSpace) {
//  Forward the top level container, ...
    if (HandleBase::forwardToSpace_(pSpace)) {
	ForwardCount++;

//  and its components:
	rtLSTORE_Handle::Reference pListStore;
	getListStore (pListStore);
	if (pListStore->forwardToSpace (pSpace))
	    StoreReference (rtINDEX_CPx_Lists, static_cast<VContainerHandle*>(pListStore));

	VContainerHandle::Reference pKeySet;
	getKeySet (pKeySet);
	if (pKeySet->forwardToSpace (pSpace))
	    StoreReference (rtINDEX_CPx_KeyValues, pKeySet);

	return true;
    }
    return false;
}

bool rtLINK_Handle::forwardToSpace_(M_ASD *pSpace) {
//  Forward the top level container, ...
    if (BaseClass::forwardToSpace_(pSpace)) {

//  and its components:
	rtPTOKEN_Handle::Reference pPPT (getPPT ());
	if (pPPT->forwardToSpace (pSpace))
	    StoreReference (pptPOP (), pPPT);

	return true;
    }
    return false;
}

bool rtUVECTOR_Handle::forwardToSpace_(M_ASD *pSpace) {
//  Forward the top level container, ...
    if (BaseClass::forwardToSpace_(pSpace)) {

//  and its components:
	rtPTOKEN_Handle::Reference pPPT (getPPT ());
	if (pPPT->forwardToSpace (pSpace))
	    StoreReference (pptPOP (), pPPT);

	return true;
    }
    return false;
}

bool rtLSTORE_Handle::forwardToSpace_(M_ASD *pSpace) {
//  Forward the top level container, ...
    if (HandleBase::forwardToSpace_(pSpace)) {
	ForwardCount++;

//  and its components:
    /*----------------------------------------------------------------------*
     *  Keep the ptoken in the same space as the lstore if the lstore is
     *  independent of a vstore, and if the ptoken is transient ...
     *----------------------------------------------------------------------*/
	rtPTOKEN_Handle::Reference pPToken (getPToken ());
	if ((pPToken->isIndependent () || isAStringStore ()) && pPToken->forwardToSpace (pSpace))
	    StoreReference (rowPTokenPOP (), pPToken);

	getContentPToken (pPToken);
	if (pPToken->forwardToSpace (pSpace))
	    StoreReference (contentPTokenPOP (), pPToken);

	VContainerHandle::Reference pContent;
	getContent (pContent);
	if (pContent->forwardToSpace (pSpace))
	    StoreReference (contentPOP (), pContent);

	return true;
    }
    return false;
}

bool rtVSTORE_Handle::forwardToSpace_(M_ASD *pSpace) {
//  Forward the top level container, ...
    if (HandleBase::forwardToSpace_(pSpace)) {
	ForwardCount++;

//  and its components:
	rtPTOKEN_Handle::Reference pRowPToken (getPToken ());
	if (pRowPToken->forwardToSpace (pSpace))
	    StoreReference (rtVSTORE_CPx_RowPToken,pRowPToken);

	if (hasAnInheritance ()) {
	    Vdd::Store::Reference pInheritanceStore;
	    getInheritanceStore (pInheritanceStore);
	    if (pInheritanceStore->forwardToSpace (pSpace))
		StoreReference (rtVSTORE_CPx_InheritanceStore, pInheritanceStore);

	    //  Update instance inheritance to reference the forwarded ptokens
	    VContainerHandle::Reference pInheritancePointer;
	    getInheritancePointer (pInheritancePointer);
	    if (pInheritancePointer->forwardToSpace (pSpace))
		StoreReference (rtVSTORE_CPx_InheritancePtr, pInheritancePointer);
	}
	return true;
    }

    return false;
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/*****************************
 *****  Simple Alignment *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to align the columns of a value store
 *
 *  Returns:
 *	'true' if alignments performed, 'false' otherwise.
 *
 *****/
bool rtVSTORE_Handle::align () {
    bool bAlignmentsPerformed = false;

    AlignCount++;

    /****  Align the row dimension of the v-store, ...  ****/
    {
	rtPTOKEN_Handle::Reference pRowPToken;
	if (isntTerminal (element(rtVSTORE_CPx_RowPToken), pRowPToken)) {
	    pRowPToken.setTo (pRowPToken->basePToken ());

	    EnableModifications ();
	    StoreReference (rtVSTORE_CPx_RowPToken, pRowPToken);

	    bAlignmentsPerformed = true;
	}
    }

    /****  Align the column dimension of the v-store, ...  ****/
    bAlignmentsPerformed = HandleBase::align (
	rtVSTORE_CPx_ColumnPToken, rtVSTORE_CPx_Column, columnCount ()
    ) || bAlignmentsPerformed;

    /****  Compress the v-store property structure if necessary, ...  ****/
    {
	rtDICTIONARY_Handle::Reference pDictionary (getDictionary ());

	/*****  Align the dictionary if a property slot compression is suspected, ...  *****/
	if (pDictionary->propertyPTokenDoesntName (this, element (rtVSTORE_CPx_ColumnPToken)))
	    pDictionary->align ();

	/*****  If a property slot compression is needed, ...  *****/
	if (pDictionary->propertyPTokenDoesntName (this, element (rtVSTORE_CPx_ColumnPToken))) {
	    /*****  Get the property subset, ...  *****/
	    rtLINK_CType *pNonPropertySubsetLC; {
		M_CPD *pPropertySubsetCPD;
		pDictionary->getPropertySubset (pPropertySubsetCPD);

		rtLINK_CType *pPropertySubsetLC = rtLINK_ToConstructor (pPropertySubsetCPD);
		pNonPropertySubsetLC = pPropertySubsetLC->Complement ();
		pPropertySubsetLC->release ();

		pPropertySubsetCPD->release ();
	    }

	    /*****  ... temporarily change the v-store's column p-token, ...  *****/
	    EnableModifications ();
	    rtPTOKEN_CType *pPTConstructor; {
		rtPTOKEN_Handle::Reference pNewColumnPToken (
		    new rtPTOKEN_Handle (
			ScratchPad (), pNonPropertySubsetLC->RPT ()->cardinality ()
		    )
		);
		StoreReference (rtVSTORE_CPx_ColumnPToken, pNewColumnPToken);
		pPTConstructor = pNewColumnPToken->makeAdjustments ();
	    } {
	    /*****  ... delete non-properties from the column set, ...  *****/
#define		handleNil(position, count, refNil)
#define		handleRepeat(position, count, origin) {\
		    pPTConstructor->AppendAdjustment (origin + 1 - totalDeleted++, -1);\
		}
#define		handleRange(position, count, origin) {\
		    pPTConstructor->AppendAdjustment (\
			origin + count - totalDeleted, -count\
		    );\
		    totalDeleted += count;\
		}

		unsigned int totalDeleted = 0;
		rtLINK_TraverseRRDCList (
		    pNonPropertySubsetLC, handleNil, handleRepeat, handleRange
		);

#undef		handleNil
#undef		handleRepeat
#undef		handleRange
	    }

	    pPTConstructor->ToPToken ()->discard ();
	    pNonPropertySubsetLC->release ();

	    /*****  ... re-align the column dimension of the v-store, ...  ****/
	    HandleBase::align (rtVSTORE_CPx_ColumnPToken, rtVSTORE_CPx_Column, columnCount ());

	    /*****
	     *  ... and change the column p-token to the dictionary's property p-token.
	     *****/
	    StoreReference (
		rtVSTORE_CPx_ColumnPToken, pDictionary, pDictionary->element (rtDICTIONARY_CPx_PropertyPToken)
	    );
	}
    }

    return bAlignmentsPerformed;
}


/*********************************
 *****  Recursive Alignment  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to fully align a vstore.
 *
 *  Arguments:
 *	deletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	True if any alignments were done, false otherwise.
 *
 *****/
bool rtVSTORE_Handle::alignAll (bool deletingEmptyUSegments) {
    bool result = false;

/*****
 *  Check for the secret code that this is an Incorporator Updated
 *  store. If it is, turn off usegment deletion. Deleting usegments
 *  is likely to break the incorporator....
 *****/
    if (deletingEmptyUSegments && RTYPE_C_UndefUV == (RTYPE_Type)M_CPreamble_RType (
	    GetContainerAddress (element (rtVSTORE_CPx_InheritancePtr))
	)
    ) deletingEmptyUSegments = false;

/*****  V-Store MUST Be Aligned !!!!!!!!!!!!!!! *****/
    align ();

/*****  ... and align each column ... *****/
    unsigned int const xColumnLimit = rtVSTORE_CPx_Column + rtPTOKEN_BaseElementCount (
	this, element (rtVSTORE_CPx_ColumnPToken)
    );
    for (unsigned int xColumn = rtVSTORE_CPx_Column; xColumn < xColumnLimit; xColumn++) {
	if (ReferenceIsntNil (xColumn)) {
	    VContainerHandle::Reference pColumnHandle (elementHandle (xColumn));
	    if (pColumnHandle->alignAll (deletingEmptyUSegments))
		result = true;
	}
    }

    return result;
}


/***********************************
 ***********************************
 *****  Row Addition Routines  *****
 ***********************************
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to add a specified number of rows to the end of a value store.
 *
 *  Arguments:
 *	this			- a handle for the head of an inheritance
 *				  chain.  The number of rows specified by the
 *				  'newRowsPToken' will be appended to each
 *				  value store in the chain through the end of
 *				  the chain or the first value store which
 *				  inherits its state via a u-vector.
 *	newRowsPToken		- a handle for a P-Token specifying the
 *				  rows to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the value store.  The
 *	positional P-Token of this link constructor will be 'newRowsPToken'.
 *
 *****/
rtLINK_CType *rtVSTORE_Handle::addInstances_(rtPTOKEN_Handle *newRowsPToken) {
    AddRowsCount++;

/*****  If this is the end of the inheritance chain, ...  *****/
    M_CPD *pInheritancePointer = 0;
    bool bTheEndOfTheLine = false;
    if (hasNoInheritance ())
	bTheEndOfTheLine = true;
    else {
	getInheritancePointer (pInheritancePointer);
	bTheEndOfTheLine = pInheritancePointer->RType () != RTYPE_C_Link;
    }

/*****  ... then add rows to this value store's row P-Token:  *****/
    if (bTheEndOfTheLine) {
	rtLINK_CType *pAdditions = 0;
	if (m_pExtension.isNil () || m_pExtension->allowsInsert ()) {
	    rtPTOKEN_CType *ptc = getPToken ()->makeAdjustments ();

	    unsigned int origin = rtPTOKEN_PTC_BaseCount (ptc);
	    unsigned int numRows = newRowsPToken->cardinality ();
	    rtPTOKEN_Handle::Reference pPToken (ptc->AppendAdjustment (origin, numRows)->ToPToken ());

/*****  ... replace the original ptoken, ...  *****/
	    EnableModifications ();
	    StoreReference (rtVSTORE_CPx_RowPToken, pPToken);

/*****  ... create a link constructor for the added rows, ...  *****/
	    pAdditions = rtLINK_AppendRange (
		rtLINK_RefConstructor (pPToken), origin, numRows
	    )->Close (newRowsPToken);
	}

/*****  ...  cleanup, ...  *****/
	if (pInheritancePointer)
	    pInheritancePointer->release ();

/*****  ... and return.  *****/
	return pAdditions;
    }

/*****  ... otherwise, try to add rows to the 'super' of this store:  *****/
    rtLINK_CType *pInheritedAdditions = 0; {
	Vdd::Store::Reference pInheritanceStore;
	getInheritanceStore (pInheritanceStore);
	pInheritedAdditions = pInheritanceStore->addInstances (newRowsPToken);
    }

/*****  If the addition succeeded, update this store...:  *****/
    rtLINK_CType *pAdditions = 0;
    if (pInheritedAdditions) {
	pAdditions = rtLINK_LCAdd (pInheritancePointer, pInheritedAdditions);
	pInheritedAdditions->release ();

	EnableModifications ();
	StoreReference (rtVSTORE_CPx_RowPToken, pAdditions->RPT ());
    }

/*****  ... cleanup, ...  *****/
    pInheritancePointer->release ();

/*****  ... and return.  *****/
    return pAdditions;
}


rtLINK_CType* rtVSTORE_Handle::addExtensions_(
    Vdd::Store::ExtensionType	xOperation,
    Vdd::Store*			pInheritanceStore,
    M_CPD*			pInheritancePointer,
    rtLINK_CType*&		rpExtensionGuard,
    rtLINK_CType**		ppLocateOrAddAdditions	// optional locate or add result, nil if not wanted
) {
//  Initialize the extension guard return lval:
    rpExtensionGuard = NilOf (rtLINK_CType*);

//  ... the descendent has no inheritance:
    if (hasNoInheritance ())
	return NilOf (rtLINK_CType*);

//  ... or the descendent doesn't inherit through a link:
    M_CPD *pThisInheritancePointer;
    getInheritancePointer (pThisInheritancePointer);
    if (pThisInheritancePointer->RTypeIsnt (RTYPE_C_Link)) {
	pThisInheritancePointer->release ();
	return NilOf (rtLINK_CType*);
    }

//  If the descendent inherits directly from the inheritance store, ...
    Vdd::Store::Reference pThisInheritanceStore;
    getInheritanceStore (pThisInheritanceStore);

    rtLINK_CType *pExtensionInheritance = pThisInheritanceStore->Names (
	pInheritanceStore
    ) ? rtLINK_ToConstructor (pInheritancePointer) : pThisInheritanceStore->addExtensions (
	xOperation, pInheritanceStore, pInheritancePointer, rpExtensionGuard
    );

//  If we have an extension inheritance to process, process it:
    rtLINK_CType *pExtension = NilOf (rtLINK_CType*);
    if (pExtensionInheritance) {
	bool bLocateOrAdd = false;

	switch (xOperation) {
	case ExtensionType_Locate: {
		rtLINK_CType* pExtensionGuard;
		rtLINK_LookupUsingLCKey (
		    pThisInheritancePointer,
		    pExtensionInheritance,
		    rtLINK_LookupCase_EQ,
		    INT_MAX,
		    &pExtension,
		    &pExtensionGuard
		);
		if (pExtensionGuard && rpExtensionGuard) {
		    rtLINK_CType* pRightGuard = rpExtensionGuard->Extract (pExtensionGuard);
		    pExtensionGuard->release ();
		    rpExtensionGuard->release ();
		    rpExtensionGuard = pRightGuard;
		}
		else if (pExtensionGuard)
		    rpExtensionGuard = pExtensionGuard;
	    }
	    break;

	case ExtensionType_LocateOrAdd:
	    bLocateOrAdd = true;
	    /***** NO BREAK  *****/

	default:    // ExtensionType_Add
	    pExtension = rtLINK_LCAddLocate (
		pThisInheritancePointer, pExtensionInheritance, bLocateOrAdd, ppLocateOrAddAdditions
	    );
	    EnableModifications ();
	    StoreReference (rtVSTORE_CPx_RowPToken, pExtension->RPT ());
	    break;
	}

	pExtensionInheritance->release ();
    }

    pThisInheritancePointer->release ();

    return pExtension;
}


/*******************************
 *******************************
 *****  Instance Deletion  *****
 *******************************
 *******************************/

bool rtVSTORE_Handle::doVStoreInstanceDeletionSetup (rtPTOKEN_CType::Reference &rpPTC) {
    return this != ENVIR_KDsc_TheTLE.store () && (
	m_pExtension.isNil () || m_pExtension->allowsDelete ()
    ) && doInstanceDeletionSetup (rpPTC, false);
}

bool rtVSTORE_Handle::deleteInstances_(DSC_Scalar &pInstances) {
    rtPTOKEN_CType::Reference pPTC; rtPTOKEN_Handle::Reference pPPT;
    if (!doVStoreInstanceDeletionSetup (pPTC) || !doInstanceDeletion (pInstances, pPTC, pPPT))
	return false;

    /*****  'Align' the store along its vertical dimension  *****/
    /*****   ... but only if necessary (hence the POP comparison) *****/
    if (ReferenceDoesntName (rtVSTORE_CPx_RowPToken, pPPT)) {
	EnableModifications ();
	StoreReference (rtVSTORE_CPx_RowPToken, pPPT);
    }
    return true;
}

bool rtVSTORE_Handle::deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) {
    rtPTOKEN_CType::Reference pPTC; rtPTOKEN_Handle::Reference pPPT;
    if (!doVStoreInstanceDeletionSetup (pPTC) || !doInstanceDeletion (pInstances, pPTC, pPPT, rpTrues, rpFalses))
	return false;

    /*****  'Align' the store along its vertical dimension  *****/
    /*****   ... but only if necessary (hence the POP comparison) *****/
    if (ReferenceDoesntName (rtVSTORE_CPx_RowPToken, pPPT)) {
	EnableModifications ();
	StoreReference (rtVSTORE_CPx_RowPToken, pPPT);
    }
    return true;
}


/****************************************
 ****************************************
 *****  Data Base Update Utilities  *****
 ****************************************
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to write the a Data Base Dump file the vstore records.
 *
 *  Arguments:
 *	pVStoreCPD	- a standard CPD for the vstore to dump.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtVSTORE_WriteDBUpdateInfo (M_CPD *pVStoreCPD) {
/*****  V-Store MUST Be Aligned !!!!!!!!!!!!!!! *****/
    pVStoreCPD->align ();

/***** Setup to get the item name out of the method dictionary *****/
    rtPTOKEN_Handle::Reference pColumnPToken;
    static_cast<rtVSTORE_Handle*>(pVStoreCPD->containerHandle ())->getColumnPToken (pColumnPToken);

    M_CPD* pDictionary		= rtVSTORE_CPD_DictionaryCPD		(pVStoreCPD);
    M_CPD* pPropertySubsetCPD	= rtDICTIONARY_CPD_PropertySubsetCPD	(pDictionary);
    M_CPD* pSelectorsCPD	= rtDICTIONARY_CPD_SelectorsCPD		(pDictionary);
    M_CPD* pStringSpaceCPD	 = rtSELUV_CPD_StringSpaceCPD		(pSelectorsCPD);

/***** Output each column ... *****/
    VContainerHandle *pVStoreHandle = pVStoreCPD->containerHandle ();
    M_POP const *pColumnArray = rtVSTORE_CPD_ColumnArray (pVStoreCPD);
    for (unsigned int  i = 0; i < pColumnPToken->cardinality (); i++) {
	M_POP const *pColumn = &pColumnArray[i];
	if (pVStoreHandle->ReferenceIsntNil (pColumn)) {
	    /*** Get the column cpd ... ***/
	    M_CPD* pColumnVectorCPD = pVStoreHandle->GetCPD (pColumn);

	    /*** Get the item name ***/
	    rtREFUV_Type_Reference propertySetReference;
	    propertySetReference.constructReference (pColumnPToken, i);

	    rtREFUV_Type_Reference selectorSetReference;
	    rtLINK_RefExtract (&selectorSetReference, pPropertySubsetCPD, &propertySetReference);

	    rtSELUV_CPD_Element (pSelectorsCPD) = rtSELUV_CPD_Array (
		pSelectorsCPD
	    ) + rtREFUV_Ref_D_Element (selectorSetReference);

	    selectorSetReference.destroy ();
	    propertySetReference.destroy ();

	    char const *itemName;
	    switch (rtSELUV_Element_Type (rtSELUV_CPD_Element (pSelectorsCPD)))
	    {
	    case SELECTOR_C_DefinedSelector:
		itemName = rtCHARUV_CPD_Array (pStringSpaceCPD) + rtSELUV_Element_Index  (
		    rtSELUV_CPD_Element (pSelectorsCPD)
		);
		break;
	    case SELECTOR_C_KnownSelector:
	        itemName = KS__ToString (
		    rtSELUV_Element_Index  (rtSELUV_CPD_Element (pSelectorsCPD))
		);
		break;
	    default:
		ERR_SignalFault (
		    EC__InternalInconsistency,
		    "rtVSTORE_WriteDBUpdateInfo: Bad element type in Selector UVector."
		);
		break;
	    }
	    
	    /*** Now call the apropriate routine to output the DB Info ...***/
	    switch (pColumnVectorCPD->RType ())
	    {
	    case RTYPE_C_Vector:
		rtVECTOR_WriteDBUpdateInfo (pColumnVectorCPD, itemName);
		break;
	    default:
		ERR_SignalFault (
		    EC__InternalInconsistency,
		    "rtVSTORE_WriteDBUpdateInfo: Unknown column type."
		);
		break;
	    }  /* end switch */

	    pColumnVectorCPD->release ();
	}  /* end of if IsntNilPOP */
    }  /* end of for loop */

/***** Cleanup and return ... *****/
    pStringSpaceCPD->release ();
    pSelectorsCPD->release ();
    pPropertySubsetCPD->release ();
    pDictionary->release ();
}


/************************************************************
 ************************************************************
 *****  Standard Representation Type Services Routines  *****
 ************************************************************
 ************************************************************/

/**********************
 *****  Printing  *****
 **********************/

/*---------------------------------------------------------------------------
 *****  Routine to display a VStore without aligning it.
 *
 *  Arguments:
 *	cpd			- the address of a standard CPD for the VStore
 *				  to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void DumpVStore (M_CPD *cpd) {
    IO_printf ("%s{\n", RTYPE_TypeIdAsString (RTYPE_C_ValueStore));

    IO_printf ("    Row PToken: ");
    RTYPE_RPrint (cpd, rtVSTORE_CPx_RowPToken);

    IO_printf ("\n    InheritanceStore: ");
    RTYPE_QPrint (cpd, rtVSTORE_CPx_InheritanceStore);

    IO_printf ("\n    InheritancePointer: ");
    if (cpd->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore))
	IO_printf ("NIL");
    else RTYPE_QPrint (cpd, rtVSTORE_CPx_InheritancePtr);

    IO_printf ("\n    ColumnPToken: ");
    RTYPE_RPrint (cpd, rtVSTORE_CPx_ColumnPToken);

    IO_printf ("\n    Dictionary: ");
    RTYPE_QPrint (cpd, rtVSTORE_CPx_Dictionary);

    IO_printf (
	"\n    Column Ptr(%x)-> [%u :%u ]",
	rtVSTORE_CPD_Column(cpd),
	M_POP_ObjectSpace (rtVSTORE_CPD_Column(cpd)),
	M_POP_ContainerIndex (rtVSTORE_CPD_Column (cpd))
    );

    IO_printf ("\n}");
}

/*---------------------------------------------------------------------------
 *****  Routine to display a VStore.
 *
 *  Arguments:
 *	cpd			- the address of a standard CPD for the VStore
 *				  to be printed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintVStore (M_CPD *cpd) {
    cpd->align ();
    DumpVStore (cpd);
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewWithMethodsDM) {
    static char const *const pLocation = "'new:withMethods:'";
    VContainerHandle *pPPT = RTYPE_QRegisterHandle (parameterArray[0]);
    RTYPE_MustBeA (pLocation, pPPT->RType (), RTYPE_C_PToken);

    VContainerHandle *pDictionary = RTYPE_QRegisterHandle (parameterArray[1]);
    RTYPE_MustBeA (pLocation, pDictionary->RType (), RTYPE_C_Dictionary);

    return RTYPE_QRegister (
	rtVSTORE_NewCluster (
	    static_cast<rtPTOKEN_Handle*>(pPPT), static_cast<rtDICTIONARY_Handle*>(pDictionary), 0, 0)
    );
}

IOBJ_DefineNewaryMethod (NewInWithMethodsDM) {
    Vdd::Store::Reference pSuperStore (RTYPE_QRegisterHandle(parameterArray[1])->getStore ());

    rtDICTIONARY_Handle::Reference pDictionary (
	static_cast<rtDICTIONARY_Handle*>(RTYPE_QRegisterHandle (parameterArray[2]))
    );
    return RTYPE_QRegister (
	rtVSTORE_NewCluster (
	    0,
	    pDictionary,				/* pDictionary */
	    pSuperStore,				/* pSuperStore */
	    RTYPE_QRegisterCPD (parameterArray[0])	/* pSuperPointer */
	)
    );
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nValue Store Routines Usage Counts\n");
    IO_printf (  "---------------------------------\n");

    IO_printf ("NewCount			= %u\n", NewCount);
    IO_printf ("AlignCount			= %u\n", AlignCount);
    IO_printf ("AddRowsCount			= %u\n", AddRowsCount);
    IO_printf ("CloneCount			= %u\n", CloneCount);
    IO_printf ("ForwardCount			= %u\n", ForwardCount);
    IO_printf ("\n");

    IO_printf ("LookupCount			= %u\n", LookupCount);
    IO_printf ("\n");

    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (AlignDM) {
    RTYPE_QRegisterCPD (self)->align ();
    return self;
}

IOBJ_DefineUnaryMethod (DictionaryDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_Dictionary);
}

IOBJ_DefineUnaryMethod (InheritanceStoreDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_InheritanceStore);
}

IOBJ_DefineUnaryMethod (InheritancePointerDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_InheritancePtr);
}

IOBJ_DefineUnaryMethod (RowPTokenDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_RowPToken);
}

IOBJ_DefineUnaryMethod (ColumnPTokenDM) {
    return RTYPE_Browser(RTYPE_QRegisterCPD (self), rtVSTORE_CPx_ColumnPToken);
}

IOBJ_DefineUnaryMethod (AuxiliaryColumnDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_AuxiliaryColumn);
}

IOBJ_DefineUnaryMethod (ColumnDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_Column);
}


IOBJ_DefineMethod (SetAuxiliaryColumnDM) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtVSTORE_CPx_AuxiliaryColumn, RTYPE_QRegisterCPD (parameterArray[0])
    );

    return self;
}

IOBJ_DefineMethod (SetInheritanceStoreDM) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtVSTORE_CPx_InheritanceStore, RTYPE_QRegisterCPD (parameterArray[0])
    );

    return self;
}

IOBJ_DefineMethod (SetInheritancePointerDM) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtVSTORE_CPx_InheritancePtr, RTYPE_QRegisterCPD (parameterArray[0])
    );

    return self;
}

IOBJ_DefineMethod (SetColumnDM) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtVSTORE_CPx_Column, RTYPE_QRegisterCPD (parameterArray[0])
    );

    return self;
}

IOBJ_DefineMethod (SetColumnPtrDM) {
    VSelectorGenerale selector (RTYPE_QRegisterCPD (parameterArray[0]));

    M_CPD *pStoreCPD = RTYPE_QRegisterCPD (self);
    rtVSTORE_Handle *pStore = static_cast<rtVSTORE_Handle*>(pStoreCPD->containerHandle ());

    unsigned int xPropertySlot;
    if (pStore->lookup (selector, xPropertySlot) != Vdd::Store::DictionaryLookup_FoundProperty)
	IO_printf ("Warning: Property not found, column pointer not set.\n");
    else {
	rtVSTORE_CPD_Column (pStoreCPD) = rtVSTORE_CPD_ColumnArray (pStoreCPD) + xPropertySlot;
    }

    return self;
}

IOBJ_DefineMethod (AddRowsDM) {
    return RTYPE_QRegister (
	static_cast<rtVSTORE_Handle*>(RTYPE_QRegisterHandle (self))->addInstances (
	    RTYPE_QRegisterPToken (parameterArray[0])
	)
    );
}

IOBJ_DefineUnaryMethod (CloneDM) {
    rtPTOKEN_Handle::Reference pClonePPT (new rtPTOKEN_Handle (IOBJ_ScratchPad, 0));

    rtVSTORE_Handle::Reference pClone;
    static_cast<rtVSTORE_Handle*>(RTYPE_QRegisterHandle (self))->clone (pClone, pClonePPT);

    return RTYPE_QRegister (pClone);
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtVSTORE_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:withMethods:"	, NewWithMethodsDM)
	IOBJ_MDE ("new:in:withMethods:"	, NewInWithMethodsDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	POPVECTOR_StdDMDEPackage
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("dictionary"		, DictionaryDM)
	IOBJ_MDE ("methodDictionary"	, DictionaryDM)
	IOBJ_MDE ("rowPToken"		, RowPTokenDM)
	IOBJ_MDE ("inheritanceStore"	, InheritanceStoreDM)
	IOBJ_MDE ("inheritancePointer"	, InheritancePointerDM)
	IOBJ_MDE ("vStoreInheritance"	, InheritanceStoreDM)
	IOBJ_MDE ("valueInheritance"	, InheritancePointerDM)
	IOBJ_MDE ("columnPToken"	, ColumnPTokenDM)
	IOBJ_MDE ("auxiliaryColumn"	, AuxiliaryColumnDM)
	IOBJ_MDE ("column"		, ColumnDM)
	IOBJ_MDE ("setInheritanceStore:", SetInheritanceStoreDM)
	IOBJ_MDE (
	    "setInheritancePointer:"	, SetInheritancePointerDM
	)
	IOBJ_MDE ("setAuxiliaryColumn:"	, SetAuxiliaryColumnDM)
	IOBJ_MDE ("setColumn:"		, SetColumnDM)
	IOBJ_MDE ("defineColumn:"	, SetColumnDM)
//	IOBJ_MDE ("setColumnPtr:"	, SetColumnPtrDM)
	IOBJ_MDE ("addRows:"		, AddRowsDM)
	IOBJ_MDE ("clone"		, CloneDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
//	    rtd->SetCPDReusability	();	// Probably safe except for ?d debugging
	    rtd->SetCPDPointerCountTo	(rtVSTORE_CPD_StdPtrCount + POPVECTOR_CPD_XtraPtrCount);
	    M_RTD_CPDInitFn		(rtd) = POPVECTOR_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtVSTORE_Handle::Maker;
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
	PrintVStore (iArgList.arg<M_CPD*>());
        break;
    case RTYPE_RPrintObject: {
	    M_CPD *const pInstance = iArgList.arg<M_CPD*>();
	    pInstance->align ();
	    POPVECTOR_Print (pInstance, true);
	}
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
    FAC_FDFCase_FacilityIdAsString (rtVSTORE);
    FAC_FDFCase_FacilityDescription ("Value Store Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvstore.c 1 replace /users/mjc/system
  860404 20:15:15 mjc Create Value Store Representation Type

 ************************************************************************/
/************************************************************************
  RTvstore.c 2 replace /users/jad/system
  860422 15:16:12 jad use new io module

 ************************************************************************/
/************************************************************************
  RTvstore.c 3 replace /users/mjc/system
  860523 01:32:06 mjc Return to library for completion by 'jck'

 ************************************************************************/
/************************************************************************
  RTvstore.c 4 replace /users/jck/system
  860530 10:44:40 jck Release partial implementation for mjc

 ************************************************************************/
/************************************************************************
  RTvstore.c 5 replace /users/jck/system
  860605 16:22:22 jck Creation and Search operations functional

 ************************************************************************/
/************************************************************************
  RTvstore.c 6 replace /users/jck/system
  860611 11:12:03 jck AddRows and Align routines implemented

 ************************************************************************/
/************************************************************************
  RTvstore.c 7 replace /users/mjc/system
  860617 18:44:13 mjc Declared 'save' routine

 ************************************************************************/
/************************************************************************
  RTvstore.c 8 replace /users/mjc/system
  860623 10:23:39 mjc Converted to use new value descriptor macros/R-Type

 ************************************************************************/
/************************************************************************
  RTvstore.c 9 replace /users/mjc/system
  860709 10:18:52 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTvstore.c 10 replace /users/mjc/system
  860713 17:13:58 mjc Release new version of 'rtVSTORE_AddRows'

 ************************************************************************/
/************************************************************************
  RTvstore.c 11 replace /users/jck/system
  860725 16:45:38 jck Added methods to assist in data base building

 ************************************************************************/
/************************************************************************
  RTvstore.c 12 replace /users/mjc/system
  860729 12:02:14 mjc Added alignment to 'FindMethod'

 ************************************************************************/
/************************************************************************
  RTvstore.c 13 replace /users/jad/system
  860731 17:46:00 jad added DBUPDATE utilities

 ************************************************************************/
/************************************************************************
  RTvstore.c 14 replace /users/mjc/system
  860803 00:18:30 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  RTvstore.c 15 replace /users/mjc/system
  860805 18:46:59 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  RTvstore.c 16 replace /users/jad/system
  860808 13:33:59 jad fixed setColumnPtr debug method to set up a full valuedsc

 ************************************************************************/
/************************************************************************
  RTvstore.c 17 replace /users/jck/system
  860820 11:42:24 jck Return for jad's use

 ************************************************************************/
/************************************************************************
  RTvstore.c 18 replace /users/jad/system
  860820 13:35:20 jad added clone routine and counts

 ************************************************************************/
/************************************************************************
  RTvstore.c 19 replace /users/jad/system
  860903 12:53:50 jad made Clone routine use new EmptyLink routine

 ************************************************************************/
/************************************************************************
  RTvstore.c 20 replace /users/mjc/system
  861002 18:02:37 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  RTvstore.c 21 replace /users/mjc/system
  870119 10:30:18 mjc Reflected 'rtSELUV' name changes

 ************************************************************************/
/************************************************************************
  RTvstore.c 22 replace /users/jad/system
  870120 15:00:19 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  RTvstore.c 23 replace /users/mjc/system
  870219 00:04:32 mjc Align V-Store prior to DB update dump

 ************************************************************************/
/************************************************************************
  RTvstore.c 24 replace /users/mjc/translation
  870524 09:36:34 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvstore.c 25 replace /users/jck/system
  870706 08:15:29 jck Added an independent of vstore flag to ptokens which will be unset at time of vstore creation

 ************************************************************************/
/************************************************************************
  RTvstore.c 26 replace /users/jck/system
  870824 15:01:37 jck Added routine to align columns

 ************************************************************************/
/************************************************************************
  RTvstore.c 27 replace /users/jck/system
  871007 13:33:34 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTvstore.c 28 replace /users/mjc/Software/system
  871214 16:25:59 mjc Added comprehensive alignment routines for all property types

 ************************************************************************/
/************************************************************************
  RTvstore.c 29 replace /users/jad/system
  880427 16:54:07 jad Added two debug routines: PrintVStore and DumpVStore

 ************************************************************************/
/************************************************************************
  RTvstore.c 30 replace /users/jck/system
  890222 15:30:16 jck Implemented corruption detection mechanism

 ************************************************************************/
/************************************************************************
  RTvstore.c 31 replace /users/jck/system
  890411 13:49:48 jck Fixed a bug in rtVSTORE_New

 ************************************************************************/
/************************************************************************
  RTvstore.c 32 replace /users/mjc/system/workbench/4.4/Originals
  890912 08:58:11 jck removed dependency on RTtstore

 ************************************************************************/

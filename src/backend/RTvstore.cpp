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


/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

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
PublicFnDef M_CPD* rtVSTORE_NewCluster (
    M_CPD *pPPT, M_CPD *pDictionary, M_CPD *pSuperStore, M_CPD *pSuperPointer
) {
    NewCount++;

/*****  Decode the instance inheritance, ...  *****/
    bool bFreePPT = false;
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
	    pPPT = pSuperPointer->GetCPD (xRowPTokenRef, RTYPE_C_PToken);
	    bFreePPT = true;
	}
    }

/*****  ... Mark the ptoken as contolled by the value store  ... *****/
    pPPT->EnableModifications ();
    rtPTOKEN_CPD_Independent (pPPT) = false;

/*****  ... Create the new V-Store ...  *****/
    M_CPD *pColumnPToken = rtDICTIONARY_CPD_PropertyPTokenCPD (pDictionary);

    M_CPD *pResult = POPVECTOR_New (
	pPPT->Space (),
	RTYPE_C_ValueStore,
	rtVSTORE_CPD_StdPtrCount - 1 + rtPTOKEN_CPD_BaseElementCount (pColumnPToken)
    )->StoreReference (
	rtVSTORE_CPx_Dictionary, pDictionary
    )->StoreReference (
	rtVSTORE_CPx_RowPToken, pPPT
    )->StoreReference (
	rtVSTORE_CPx_ColumnPToken, pColumnPToken
    );
    pColumnPToken->release ();

/*****  ... Install the instance and class inheritance, ...  *****/
    if (pSuperStore) pResult->StoreReference (
	rtVSTORE_CPx_InheritanceStore, pSuperStore
    )->StoreReference (
	rtVSTORE_CPx_InheritancePtr, pSuperPointer
    );

/*****  ... cleanup, ...  *****/
    if (bFreePPT)
	pPPT->release ();

/*****  ... and return:  *****/
    return pResult;
}


PublicFnDef M_CPD *rtVSTORE_NewCluster (M_CPD *pPPT, M_CPD *pPrototype) {
    CloneCount++;

/*****  Process other store types, ...  *****/
    switch (M_CPD_RType (pPrototype)) {
    case RTYPE_C_CharUV:
	return rtCHARUV_New (pPPT, pPrototype, UV_CPx_RefPToken);
    case RTYPE_C_Index:
	return rtINDEX_NewCluster (pPPT, pPrototype);
    case RTYPE_C_ListStore:
	return rtLSTORE_NewCluster (pPPT, pPrototype);
    case RTYPE_C_ValueStore:
	break;
    case RTYPE_C_Vector:
        return rtVECTOR_New (pPPT);
    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtVSTORE_NewCluster(Clone): Unexpected R-Type: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pPrototype))
	    )
	);
	break;
    }

/*****  Create a new empty v-store using the prototype's dictionary... *****/
    M_CPD* pNewVStoreCPD; {
	M_CPD* pDictionary = rtVSTORE_CPD_DictionaryCPD (pPrototype);

	pNewVStoreCPD = rtVSTORE_NewCluster (
	    pPPT, pDictionary, NilOf (M_CPD*), NilOf (M_CPD*)
	);

	pDictionary->release ();
    }

/*****  Return if this is the end of the inheritance chain, ...  *****/
    if (pPrototype->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore))
	return pNewVStoreCPD;

/*****  Otherwise, construct an inheritance for the clone, ...  *****/
    M_CPD* pPrototypeInstanceInheritanceCPD = rtVSTORE_CPD_InheritancePointerCPD (
	pPrototype
    );
    switch ((RTYPE_Type)M_CPD_RType (pPrototypeInstanceInheritanceCPD)) {
/*****  ... as a reference to the current inheritance for values, ...  *****/
    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV: {
	    M_CPD* pNewInstanceInheritanceCPD = UV_New (
		(RTYPE_Type)M_CPD_RType (pPrototypeInstanceInheritanceCPD),
		pPPT,
		pPrototypeInstanceInheritanceCPD,
		UV_CPx_RefPToken,
		(size_t)UV_CPD_Granularity (pPrototypeInstanceInheritanceCPD),
		NilOf (Ref_UV_Initializer),
		0
	    );

	    pNewVStoreCPD->StoreReference (
		rtVSTORE_CPx_InheritanceStore, pPrototype, rtVSTORE_CPx_InheritanceStore
	    );
	    pNewVStoreCPD->StoreReference (rtVSTORE_CPx_InheritancePtr, pNewInstanceInheritanceCPD);

	    pNewInstanceInheritanceCPD->release ();
	}
	break;

/*****  ... as a reference to a clone of the current inheritance for instances,  ...  *****/
    case RTYPE_C_Link: {
	    M_CPD* pNextVStorePTokenCPD = rtPTOKEN_New (pPPT->Space (), 0); {
		M_CPD* pPrototypeBehaviorInheritanceCPD = rtVSTORE_CPD_InheritanceStoreCPD (
		    pPrototype
		);
		M_CPD* pNextVStoreCPD = rtVSTORE_NewCluster (
		    pNextVStorePTokenCPD, pPrototypeBehaviorInheritanceCPD
		);

		pNewVStoreCPD->StoreReference (rtVSTORE_CPx_InheritanceStore, pNextVStoreCPD);

		pNextVStoreCPD->release ();
		pPrototypeBehaviorInheritanceCPD->release ();
	    } {
		M_CPD* pNewInstanceInheritanceCPD = rtLINK_NewEmptyLink (
		    pPPT, pNextVStorePTokenCPD
		);

		pNewVStoreCPD->StoreReference (
		    rtVSTORE_CPx_InheritancePtr, pNewInstanceInheritanceCPD
		);

		pNewInstanceInheritanceCPD->release ();
	    }
	    pNextVStorePTokenCPD->release ();
	}
	break;

    default:
	RTYPE_ComplainAboutType (
	    "rtVSTORE_NewCluster:Pointer Cloning",
	    (RTYPE_Type)M_CPD_RType (pPrototypeInstanceInheritanceCPD),
	    RTYPE_C_ValueStore
	);
    }
    pPrototypeInstanceInheritanceCPD->release ();

    return pNewVStoreCPD;
}


PublicFnDef bool rtVSTORE_ForwardCluster (
    M_CPD* pTransientStoreCPD, M_CPD* pPersistentStoreCPD
) {
//  Check request validity. Only forward from scratch pad to persistent ...
    if (pTransientStoreCPD->IsntInTheScratchPad () || pPersistentStoreCPD->IsInTheScratchPad ())
	return false;

    ForwardCount++;


//  Forward components based on store type, ... 
    //  After components are forwarded, the new POP is copied into place.
    //  This is done to reduce the chance that forwarded POP entries will
    //  remain in the scratch pad container table. Forwarded POPs in the
    //  scratch pad CT can make some vector operations less efficient.
    M_CPD* pPTokenCPD;
    switch (M_CPD_RType (pTransientStoreCPD)) {
    case RTYPE_C_Index: {
	//  Forward Top Level Container ...
	    pTransientStoreCPD->ForwardToSpace (pPersistentStoreCPD);
	    M_CPD* pListsCPD = rtINDEX_CPD_ListStoreCPD (pTransientStoreCPD);
	//  Keep the ptoken in the same space with the index if the index is
	//  independent of a vstore, and if the ptoken is transient ...
	    pPTokenCPD = rtLSTORE_CPD_RowPTokenCPD (pListsCPD);
	    if (pPTokenCPD->IsInTheScratchPad () && rtPTOKEN_CPD_Independent (pPTokenCPD)) {
		pPTokenCPD->ForwardToSpace (pPersistentStoreCPD);
		pListsCPD->StoreReference (rtLSTORE_CPx_RowPToken, pPTokenCPD);
	    }
	    pPTokenCPD->release ();
	    pListsCPD->release ();
	}
	break;

    case RTYPE_C_ListStore:
    //  Forward Top Level Container ... 
	pTransientStoreCPD->ForwardToSpace (pPersistentStoreCPD);
    //  Keep the ptoken in the same space with the lstore if the lstore is
    //  independent of a vstore, and if the ptoken is transient ...
	pPTokenCPD = rtLSTORE_CPD_RowPTokenCPD (pTransientStoreCPD);
	if (pPTokenCPD->IsInTheScratchPad () &&
	    (rtPTOKEN_CPD_Independent (pPTokenCPD) || rtLSTORE_CPD_StringStore (pTransientStoreCPD)))
	{
	    pPTokenCPD->ForwardToSpace (pPersistentStoreCPD);
	    pTransientStoreCPD->StoreReference (rtLSTORE_CPx_RowPToken, pPTokenCPD);
	}
	pPTokenCPD->release ();
	break;

    case RTYPE_C_ValueStore: {
	//  Forward Top Level Container ...
	    pTransientStoreCPD->ForwardToSpace (pPersistentStoreCPD);
	    M_CPD* pInheritanceStoreCPD = rtVSTORE_CPD_InheritanceStoreCPD (
		pTransientStoreCPD
	    );
	//  Forward the Inheritance Chain ...
	    rtVSTORE_ForwardCluster (pInheritanceStoreCPD, pPersistentStoreCPD);
	    pTransientStoreCPD->StoreReference (rtVSTORE_CPx_InheritanceStore, pInheritanceStoreCPD);
	//  Forward the Row PToken and update the store's reference to it ....
	    pPTokenCPD = rtVSTORE_CPD_RowPTokenCPD (pTransientStoreCPD);
	    if (pPTokenCPD->IsInTheScratchPad ()) {
		pPTokenCPD->ForwardToSpace (pPersistentStoreCPD);
		pTransientStoreCPD->StoreReference (rtVSTORE_CPx_RowPToken, pPTokenCPD);
	    }
	    pPTokenCPD->release ();
	//  Update instance inheritance to reference the forwarded ptokens
	    M_CPD* pInheritancePtrCPD = rtVSTORE_CPD_InheritancePointerCPD (
		pTransientStoreCPD
	    );
	    switch (M_CPD_RType (pInheritancePtrCPD))
	    {
	    case RTYPE_C_Link:
		switch (M_CPD_RType (pInheritanceStoreCPD))
		{
		case RTYPE_C_ValueStore:
		    pInheritancePtrCPD->StoreReference (
			rtLINK_CPx_RefPToken, pInheritanceStoreCPD, rtVSTORE_CPx_RowPToken
		    );
		    break;
		case RTYPE_C_ListStore:
		    pInheritancePtrCPD->StoreReference (
			rtLINK_CPx_RefPToken, pInheritanceStoreCPD, rtLSTORE_CPx_RowPToken
		    );
		    break;
		default:
		    break;
		}
		pInheritancePtrCPD->StoreReference (
		    rtLINK_CPx_PosPToken, pTransientStoreCPD, rtVSTORE_CPx_RowPToken
		);
		break;
	    case RTYPE_C_CharUV:
	    case RTYPE_C_DoubleUV:
	    case RTYPE_C_FloatUV:
	    case RTYPE_C_IntUV:
	    case RTYPE_C_UndefUV:
	    case RTYPE_C_Unsigned64UV:
	    case RTYPE_C_Unsigned96UV:
	    case RTYPE_C_Unsigned128UV:
		pInheritancePtrCPD->StoreReference (
		    UV_CPx_PToken, pTransientStoreCPD, rtVSTORE_CPx_RowPToken
		);
		break;
	    default:
		break;
	    }
	    pInheritancePtrCPD->release ();
	    pInheritanceStoreCPD->release ();
	}
	break;

    case RTYPE_C_Vector:
    case RTYPE_C_CharUV:
    case RTYPE_C_Method:
    case RTYPE_C_Closure:
    case RTYPE_C_Block:
    //  Forward Top Level Container ...
	pTransientStoreCPD->ForwardToSpace (pPersistentStoreCPD);
    //  No components need be forwarded ...
        break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtVSTORE_ForwardCluster: Unexpected R-Type: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pTransientStoreCPD))
	    )
	);
	break;
    }

    return true;
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
 *  Argument:
 *	cpd		- a CPD for the Value Store to align.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD* rtVSTORE_Align (M_CPD *pVStoreCPD) {
    AlignCount++;

    /****  Validate RType of parameter  ****/
    RTYPE_MustBeA ("rtVSTORE_Align", M_CPD_RType (pVStoreCPD), RTYPE_C_ValueStore);

    /****  Align the row dimension of the v-store, ...  ****/
    {
	M_CPD* pRowPTokenCPD;
	rtPTOKEN_IsntCurrent (pVStoreCPD, rtVSTORE_CPx_RowPToken, pRowPTokenCPD);
	if (IsntNil (pRowPTokenCPD))
	{
	    M_CPD* pBaseRowPTokenCPD = rtPTOKEN_BasePToken (pRowPTokenCPD, -1);
	    pVStoreCPD->StoreReference (rtVSTORE_CPx_RowPToken, pBaseRowPTokenCPD);
	    pBaseRowPTokenCPD->release ();
	    pRowPTokenCPD->release ();
	}
    }

    /****  Align the column dimension of the v-store, ...  ****/
    rtVSTORE_CPD_Column (pVStoreCPD) = rtVSTORE_CPD_ColumnArray (pVStoreCPD);
    POPVECTOR_Align (
	pVStoreCPD,
	rtVSTORE_CPx_ColumnPToken,
	rtVSTORE_CPx_Column,
	rtVSTORE_CPD_ColumnCount (pVStoreCPD)
    );
    rtVSTORE_CPD_Column (pVStoreCPD) = rtVSTORE_CPD_ColumnArray (pVStoreCPD);

    /****  Compress the v-store property structure if necessary, ...  ****/
    {
	M_CPD* pDictionary = rtVSTORE_CPD_DictionaryCPD (pVStoreCPD);

	/*****  Align the dictionary if a property slot compression is suspected, ...  *****/
	if (pVStoreCPD->ReferenceDoesntName (
		rtVSTORE_CPx_ColumnPToken, pDictionary, rtDICTIONARY_CPx_PropertyPToken
	    )
	) rtDICTIONARY_Align (pDictionary);

	/*****  If a property slot compression is needed, ...  *****/
	if (pVStoreCPD->ReferenceDoesntName (
		rtVSTORE_CPx_ColumnPToken, pDictionary, rtDICTIONARY_CPx_PropertyPToken
	    )
	)
	{
	    /*****  Get the property subset, ...  *****/
	    rtLINK_CType* pNonPropertySubsetLC; {
		M_CPD* pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD (
		    pDictionary
		);
		rtLINK_CType *pPropertySubsetLC = rtLINK_ToConstructor (
		    pPropertySubsetCPD
		);

		pNonPropertySubsetLC = pPropertySubsetLC->Complement ();

		pPropertySubsetLC->release ();
		pPropertySubsetCPD->release ();
	    }

	    /*****  ... temporarily change the v-store's column p-token, ...  *****/
	    rtPTOKEN_CType *pPTConstructor; {
		M_CPD *pNewColumnPTokenCPD = rtPTOKEN_New (
		    pVStoreCPD->ScratchPad (),
		    rtPTOKEN_CPD_BaseElementCount (pNonPropertySubsetLC->RPT ())
		);

		pVStoreCPD->StoreReference (rtVSTORE_CPx_ColumnPToken, pNewColumnPTokenCPD);
		pPTConstructor = rtPTOKEN_NewShiftPTConstructor (
		    pNewColumnPTokenCPD, -1
		);

		pNewColumnPTokenCPD->release ();
	    }
	    
	    /*****  ... delete non-properties from the column set, ...  *****/
	    {
		unsigned int totalDeleted = 0;

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

		rtLINK_TraverseRRDCList (
		    pNonPropertySubsetLC, handleNil, handleRepeat, handleRange
		);

#undef		handleNil
#undef		handleRepeat
#undef		handleRange
	    }

	    pPTConstructor->ToPToken ()->release ();
	    pNonPropertySubsetLC->release ();

	    /*****  ... re-align the column dimension of the v-store, ...  ****/
	    rtVSTORE_CPD_Column (pVStoreCPD) = rtVSTORE_CPD_ColumnArray (pVStoreCPD);
	    POPVECTOR_Align (
		pVStoreCPD,
		rtVSTORE_CPx_ColumnPToken,
		rtVSTORE_CPx_Column,
		rtVSTORE_CPD_ColumnCount (pVStoreCPD)
	    );
	    rtVSTORE_CPD_Column (pVStoreCPD) = rtVSTORE_CPD_ColumnArray (pVStoreCPD);

	    /*****
	     *  ... and change the column p-token to the dictionary's property p-token.
	     *****/
	    pVStoreCPD->StoreReference (
		rtVSTORE_CPx_ColumnPToken, pDictionary, rtDICTIONARY_CPx_PropertyPToken
	    );
	}
	pDictionary->release ();
    }
    return pVStoreCPD;
}


/*********************************
 *****  Recursive Alignment  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to fully align a vstore.
 *
 *  Arguments:
 *	pVStoreCPD		- a standard CPD for the vstore to align.
 *	deletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	True if any alignments were done, false otherwise.
 *
 *****/
PublicFnDef bool rtVSTORE_AlignAll (M_CPD *pVStoreCPD, bool deletingEmptyUSegments) {
    bool result = false;

    switch ((RTYPE_Type)M_CPD_RType (pVStoreCPD)) {
    case RTYPE_C_ValueStore: {
	/*****
	 *  Check for the secret code that this is an Incorporator Updated
	 *  store. If it is, turn off usegment deletion. Deleting usegments
	 *  is likely to break the incorporator....
	 *****/
	    if (deletingEmptyUSegments &&
		RTYPE_C_UndefUV == (RTYPE_Type)M_CPreamble_RType (
		    pVStoreCPD->GetContainerAddress (rtVSTORE_CPx_InheritancePtr)
		)
	    ) deletingEmptyUSegments = false;

	/*****  V-Store MUST Be Aligned !!!!!!!!!!!!!!! *****/
	    rtVSTORE_Align (pVStoreCPD);

	    unsigned int size = rtPTOKEN_BaseElementCount (
		pVStoreCPD, rtVSTORE_CPx_ColumnPToken
	    );

	/*****  Go to the first column ...  *****/
	    M_POP const *pColumnArray = rtVSTORE_CPD_ColumnArray (pVStoreCPD);

	/*****  ... and align each column ... *****/
	    VContainerHandle *pVStoreHandle = pVStoreCPD->containerHandle ();
	    for (unsigned int i=0; i<size; i++) {
		M_POP const *pColumn = &pColumnArray[i];
		if (pVStoreHandle->ReferenceIsntNil (pColumn)) {
		    M_CPD* columnCPD = pVStoreHandle->GetCPD (pColumn);

		    if (rtVSTORE_AlignAll (columnCPD, deletingEmptyUSegments))
			result = true;

		    columnCPD->release ();
		}
	    }
	}
        break;

    case RTYPE_C_ListStore:
	result = rtLSTORE_AlignAll (pVStoreCPD, deletingEmptyUSegments);
        break;

    case RTYPE_C_Index:
	result = rtINDEX_AlignAll  (pVStoreCPD, deletingEmptyUSegments);
        break;

    case RTYPE_C_Vector:
	result = rtVECTOR_AlignAll (pVStoreCPD, deletingEmptyUSegments);
        break;

    case RTYPE_C_Method:
	result = rtMETHOD_Align    (pVStoreCPD);
	break;

    case RTYPE_C_Closure:
	result = rtCLOSURE_Align   (pVStoreCPD);
	break;

    default:
	result = false;
        break;
    }

    return result;
}


/***********************************
 ***********************************
 *****  Row Addition Routines  *****
 ***********************************
 ***********************************/

PrivateFnDef rtLINK_CType *AddRowsToPToken (
    M_CPD*			pTokenRefCPD,
    int				pTokenRefIndex,
    M_CPD*			newRowsPToken,
    int				replacingOriginalPToken
) {
    int origin, numRows;

/*****  Add the requested number of 'rows' to the p-token...  *****/
    rtPTOKEN_CType *ptc = rtPTOKEN_NewShiftPTConstructor (pTokenRefCPD, pTokenRefIndex);
    M_CPD *vStoreRowPToken = ptc->AppendAdjustment (
	origin = rtPTOKEN_PTC_BaseCount (ptc),
	numRows = rtPTOKEN_CPD_BaseElementCount (newRowsPToken)
    )->ToPToken ();

/*****  ... replace the original if appropriate, ...  *****/
    if (replacingOriginalPToken)
	pTokenRefCPD->StoreReference (pTokenRefIndex, vStoreRowPToken);

/*****  ... create a link constructor for the added rows, ...  *****/
    rtLINK_CType *addedRows = rtLINK_AppendRange (
	rtLINK_RefConstructor (vStoreRowPToken, -1), origin, numRows
    )->Close (newRowsPToken);

/*****  ... cleanup, ...  *****/
    vStoreRowPToken->release ();

/*****  ... and return.  *****/
    return addedRows;
}


PrivateFnDef rtLINK_CType *AddRowsToIndex (
    M_CPD*			vStore,
    M_CPD*			newRowsPToken
)
{
    int instantiable;

    if (rtINDEX_CPD_IsATimeSeries (vStore))
    {
	M_CPD* lstore = rtINDEX_CPD_ListStoreCPD (vStore);
	M_CPD* ptoken = rtLSTORE_CPD_RowPTokenCPD (lstore);

	instantiable = rtPTOKEN_CPD_Independent (ptoken);

	lstore->release ();
	ptoken->release ();

    }
    else instantiable = true;

    return
	instantiable
	? rtINDEX_AddLists (vStore, newRowsPToken)
	: NilOf (rtLINK_CType *);
}


/*---------------------------------------------------------------------------
 *****  Routine to add a specified number of rows to the end of a value store.
 *
 *  Arguments:
 *	vStore			- a standard CPD for the head of a value store
 *				  chain.  The number of rows specified by the
 *				  'newRowsPToken' will be appended to each
 *				  value store in the chain through the end of
 *				  the chain or the first value store which
 *				  inherits its state via a u-vector.
 *	newRowsPToken		- a standard CPD for a P-Token which
 *				  specifies the number of rows to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the value store.  The
 *	positional P-Token of this link constructor will be 'newRowsPToken'.
 *
 *****/
PublicFnDef rtLINK_CType *rtVSTORE_AddRows (
    M_CPD*			vStore,
    M_CPD*			newRowsPToken
)
{
    AddRowsCount++;

/*****  Verify that 'newRowsPToken' is a ptoken, ...  *****/
    RTYPE_MustBeA (
	"rtVSTORE_AddRows", M_CPD_RType (newRowsPToken), RTYPE_C_PToken
    );

/*****  Process non-value-stores specially, ...  *****/
    switch (M_CPD_RType (vStore)) {
    default:
	RTYPE_ComplainAboutType (
	    "rtVSTORE_AddRows", M_CPD_RType (vStore), RTYPE_C_ValueStore
	);
    case RTYPE_C_ValueStore:
	break;
    case RTYPE_C_ListStore:
	return rtLSTORE_AddLists (vStore, newRowsPToken);
    case RTYPE_C_Index:
	return AddRowsToIndex (vStore, newRowsPToken);
    }

/*****  If this is the end of the V-Store chain, ...   *****/
    M_CPD* valueInheritance = NilOf (M_CPD*);
    if (vStore->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore) ||
/*****  ... or the value inheritance is represented by a U-Vector, ...  *****/
	(RTYPE_Type)M_CPD_RType (
	    valueInheritance = rtVSTORE_CPD_InheritancePointerCPD (vStore)
	) != RTYPE_C_Link
    ) {
/*****  ... then add rows to this value store's row P-Token:  *****/
	rtLINK_CType *addedRows = vStore->TransientExtensionIsA (VGroundStore::RTT) &&
	    !((VGroundStore*)vStore->TransientExtension ())->allowsInsert ()
	    ? NilOf (rtLINK_CType*)
	    : AddRowsToPToken (vStore, rtVSTORE_CPx_RowPToken, newRowsPToken, true);

	if (IsntNil (valueInheritance))
	    valueInheritance->release ();

	return addedRows;
    }

/*****  ... otherwise, try to add rows to the 'super' of this store:  *****/
    M_CPD* vStoreInheritance = rtVSTORE_CPD_InheritanceStoreCPD (vStore);
    rtLINK_CType *inheritedRows = rtVSTORE_AddRows (vStoreInheritance, newRowsPToken);
    vStoreInheritance->release ();

/*****  If the addition failed, return:  *****/
    if (IsNil (inheritedRows))
	return inheritedRows;

/*****  ... otherwise, update this store, ...  *****/
    rtLINK_CType *addedRows = rtLINK_LCAdd (valueInheritance, inheritedRows);
    vStore->StoreReference (rtVSTORE_CPx_RowPToken, addedRows->RPT ());

/*****  ... cleanup, ...  *****/
    valueInheritance->release ();
    inheritedRows->release ();

/*****  ... and return.  *****/
    return addedRows;
}


PublicFnDef rtLINK_CType* rtVSTORE_ExtendRowsTo (
    rtVSTORE_Extension		xOperation,
    M_CPD*			pInheritanceStore,
    M_CPD*			pInheritancePointer,
    M_CPD*			pDescendentStore,
    rtLINK_CType*&		rpExtensionGuard,
    rtLINK_CType**		ppLocateOrAddAdditions	// optional locate or add result, nil if not wanted
)
{
//  Initialize the extension guard return lval:
    rpExtensionGuard = NilOf (rtLINK_CType*);

//  The extension can't be determined if ...
//  ... the desendent isn't a v-store:
    if (pDescendentStore->RTypeIsnt (RTYPE_C_ValueStore))
	return NilOf (rtLINK_CType*);

//  ... the descendent has no inheritance:
    if (pDescendentStore->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore))
	return NilOf (rtLINK_CType*);

//  ... or the descendent doesn't inherit through a link:
    M_CPD* pDescendentInheritance = rtVSTORE_CPD_InheritancePointerCPD (pDescendentStore);
    if (pDescendentInheritance->RTypeIsnt (RTYPE_C_Link)) {
	pDescendentInheritance->release ();
	return NilOf (rtLINK_CType*);
    }

    rtLINK_CType* pExtensionInheritance;
//  If the descendent inherits directly from the inheritance store, ...
    if (pDescendentStore->ReferenceNames (rtVSTORE_CPx_InheritanceStore, pInheritanceStore))
	//  ... look here:
	pExtensionInheritance = rtLINK_ToConstructor (pInheritancePointer);
    else {
	//  ... otherwise, recurse:
	M_CPD* vStoreInheritance = rtVSTORE_CPD_InheritanceStoreCPD (pDescendentStore);

	pExtensionInheritance = rtVSTORE_ExtendRowsTo (
	    xOperation,
	    pInheritanceStore,
	    pInheritancePointer,
	    vStoreInheritance,
	    rpExtensionGuard,
	    NilOf (rtLINK_CType**)
	);

	vStoreInheritance->release ();
    }

//  If we have an extension inheritance to process, process it:
    rtLINK_CType* pExtension = NilOf (rtLINK_CType*);
    if (pExtensionInheritance) {
	bool fLocateOrAdd = false;

	switch (xOperation) {
	case rtVSTORE_Extension_Locate: {
		rtLINK_CType* pExtensionGuard;
		rtLINK_LookupUsingLCKey (
		    pDescendentInheritance,
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

	case rtVSTORE_Extension_LocateOrAdd:
	    fLocateOrAdd = true;
	    /***** NO BREAK  *****/

	default:    // rtVSTORE_Extension_Add
	    pExtension = rtLINK_LCAddLocate (
		pDescendentInheritance, pExtensionInheritance, fLocateOrAdd, ppLocateOrAddAdditions
	    );

	    pDescendentStore->StoreReference (rtVSTORE_CPx_RowPToken, pExtension->RPT ());
	    break;
	}

	pExtensionInheritance->release ();
    }

    pDescendentInheritance->release ();

    return pExtension;
}


/*************************************
 *************************************
 *****  Behaviorial Equivalence  *****
 *************************************
 *************************************/

PublicFnDef int rtVSTORE_AreBehavioriallyEquivalent (
    M_CPD*			pStore1CPD,
    M_CPD*			pStore2CPD
)
{
    bool behavioriallyEquivalent = true;
    bool moreToDo;

    pStore1CPD->retain ();
    pStore2CPD->retain ();

    do {
	moreToDo = false;
	if (pStore1CPD->Names (pStore2CPD))
	    behavioriallyEquivalent = true;
	else if (M_CPD_RType (pStore1CPD) != M_CPD_RType (pStore2CPD))
	    behavioriallyEquivalent = false;
	else switch ((RTYPE_Type)M_CPD_RType (pStore1CPD)) {
	case RTYPE_C_Block:		/* need a p-token to tell for sure for a block */
	case RTYPE_C_Closure:
	case RTYPE_C_Dictionary:
	case RTYPE_C_ListStore:
	case RTYPE_C_Method:
	case RTYPE_C_Vector:
	    behavioriallyEquivalent = true;
	    break;

	case RTYPE_C_Index:
	    behavioriallyEquivalent = pStore1CPD->ReferenceNames (
		rtINDEX_CPx_KeyStore, pStore2CPD, rtINDEX_CPx_KeyStore
	    );
	    break;
	
	case RTYPE_C_ValueStore:
	    if (pStore1CPD->ReferenceDoesntName (
		    rtVSTORE_CPx_Dictionary, pStore2CPD, rtVSTORE_CPx_Dictionary
		)
	    ) behavioriallyEquivalent = false;
	    else if (
		pStore1CPD->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore) &&
		pStore2CPD->ReferenceIsNil (rtVSTORE_CPx_InheritanceStore)
	    ) behavioriallyEquivalent = true;
	    else {
		M_CPD* pInheritance1CPD = rtVSTORE_CPD_InheritanceStoreCPD (pStore1CPD);
		M_CPD* pInheritance2CPD = rtVSTORE_CPD_InheritanceStoreCPD (pStore2CPD);

		pStore1CPD->release ();
		pStore2CPD->release ();

		pStore1CPD = pInheritance1CPD;
		pStore2CPD = pInheritance2CPD;

		moreToDo = true;
	    }
	    break;

	default:
	    ERR_SignalFault (
		EC__InternalInconsistency,
		UTIL_FormatMessage (
		    "rtVSTORE_BehaviorallyEquivalent: Unexpected Store R-Type: %s",
		    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pStore1CPD))
		)
	    );
	    break;
	}
    } while (moreToDo);

    pStore1CPD->release ();
    pStore2CPD->release ();

    return behavioriallyEquivalent;
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
PublicFnDef void rtVSTORE_WriteDBUpdateInfo (
    M_CPD*			pVStoreCPD
)
{
/*****  V-Store MUST Be Aligned !!!!!!!!!!!!!!! *****/
    rtVSTORE_Align (pVStoreCPD);

/***** Setup to get the item name out of the method dictionary *****/
    M_CPD* pColumnPTokenCPD	 = rtVSTORE_CPD_ColumnPTokenCPD		(pVStoreCPD);

    M_CPD* pDictionary	 = rtVSTORE_CPD_DictionaryCPD		(pVStoreCPD);
    M_CPD* pPropertySubsetCPD = rtDICTIONARY_CPD_PropertySubsetCPD	(pDictionary);
    M_CPD* pSelectorsCPD	 = rtDICTIONARY_CPD_SelectorsCPD	(pDictionary);

    M_CPD* pStringSpaceCPD	 = rtSELUV_CPD_StringSpaceCPD		(pSelectorsCPD);

/***** Output each column ... *****/
    VContainerHandle *pVStoreHandle = pVStoreCPD->containerHandle ();
    M_POP const *pColumnArray = rtVSTORE_CPD_ColumnArray (pVStoreCPD);
    for (unsigned int  i = 0; i < rtPTOKEN_CPD_BaseElementCount (pColumnPTokenCPD); i++) {
	M_POP const *pColumn = &pColumnArray[i];
	if (pVStoreHandle->ReferenceIsntNil (pColumn)) {
	    /*** Get the column cpd ... ***/
	    M_CPD* pColumnVectorCPD = pVStoreHandle->GetCPD (pColumn);

	    /*** Get the item name ***/
	    rtREFUV_Type_Reference propertySetReference;
	    pColumnPTokenCPD->retain ();
	    DSC_InitReferenceScalar (propertySetReference, pColumnPTokenCPD, i);

	    rtREFUV_Type_Reference selectorSetReference;
	    rtLINK_RefExtract (&selectorSetReference, pPropertySubsetCPD, &propertySetReference);

	    rtSELUV_CPD_Element (pSelectorsCPD) = rtSELUV_CPD_Array (
		pSelectorsCPD
	    ) + rtREFUV_Ref_D_Element (selectorSetReference);

	    DSC_ClearScalar (selectorSetReference);
	    DSC_ClearScalar (propertySetReference);

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
	    switch ((RTYPE_Type)M_CPD_RType (pColumnVectorCPD))
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
    pColumnPTokenCPD->release ();
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
PrivateFnDef void DumpVStore (
    M_CPD*			cpd
)
{
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
PrivateFnDef void PrintVStore (
    M_CPD*			cpd
)
{
    rtVSTORE_Align (cpd);

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
    M_CPD *pPPT = RTYPE_QRegisterCPD (parameterArray[0]);
    RTYPE_MustBeA (pLocation, pPPT->RType (), RTYPE_C_PToken);

    M_CPD *pDictionary = RTYPE_QRegisterCPD (parameterArray[1]);
    RTYPE_MustBeA (pLocation, pDictionary->RType (), RTYPE_C_Dictionary);

    return RTYPE_QRegister (
	rtVSTORE_NewCluster (pPPT, pDictionary, NilOf (M_CPD*), NilOf (M_CPD*))
    );
}

IOBJ_DefineNewaryMethod (NewInWithMethodsDM) {
    return RTYPE_QRegister (
	rtVSTORE_NewCluster (
	    NilOf (M_CPD*),
	    RTYPE_QRegisterCPD (parameterArray[2]), /* pDictionary */
	    RTYPE_QRegisterCPD (parameterArray[1]), /* pSuperStore */
	    RTYPE_QRegisterCPD (parameterArray[0])  /* pSuperPointer */
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
    rtVSTORE_Align (RTYPE_QRegisterCPD (self));
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
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_AuxillaryColumn);
}

IOBJ_DefineUnaryMethod (ColumnDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtVSTORE_CPx_Column);
}


IOBJ_DefineMethod (SetAuxiliaryColumnDM) {
    RTYPE_QRegisterCPD (self)->StoreReference (
	rtVSTORE_CPx_AuxillaryColumn, RTYPE_QRegisterCPD (parameterArray[0])
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

    switch (
	rtVSTORE_Lookup (RTYPE_QRegisterCPD (self), &selector, NilOf (DSC_Descriptor*))
    ) {
    case rtDICTIONARY_LookupResult_FoundProperty:
	break;

    default:
	IO_printf ("Warning: Property not found, column pointer not set.\n");
	break;
    }

    return self;
}


IOBJ_DefineMethod (AddRowsDM) {
    return RTYPE_QRegister (
	rtVSTORE_AddRows (
	    RTYPE_QRegisterCPD (self), RTYPE_QRegisterCPD (parameterArray[0])
	)
    );
}

IOBJ_DefineUnaryMethod (CloneDM) {
    M_CPD* pClonePToken = rtPTOKEN_New (IOBJ_ScratchPad, 0);
    M_CPD* pClonedStore = rtVSTORE_NewCluster (pClonePToken, RTYPE_QRegisterCPD (self));
    pClonePToken->release ();

    return RTYPE_QRegister (pClonedStore);
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
	IOBJ_MDE ("setColumnPtr:"	, SetColumnPtrDM)
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
    case RTYPE_RPrintObject:
	POPVECTOR_Print (rtVSTORE_Align (iArgList.arg<M_CPD*>()), true);
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

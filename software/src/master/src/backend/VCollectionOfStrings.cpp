/*****  VCollectionOfStrings Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VCollectionOfStrings.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTblock.h"
#include "RTdictionary.h"
#include "RTlstore.h"

#include "RTcharuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"

#include "VLinkCReference.h"


/********************************
 ********************************
 *****  rtLSTORE_StringSet  *****
 ********************************
 ********************************/

/*************************************************
 *****  Construction Precondition Predicate  *****
 *************************************************/

bool rtLSTORE_StringSet::IsAStringSet (M_CPD* pLStore) {
    return rtLSTORE_CPD_StringStore (pLStore) && !rtPTOKEN_PT_Independent (
	M_CPreamble_ContainerBaseAsType (
	    pLStore->GetContainerAddress (rtLSTORE_CPx_RowPToken), rtPTOKEN_Type
	)
    );
}


/**************************
 *****  Construction  *****
 **************************/

rtLSTORE_StringSet::rtLSTORE_StringSet (M_CPD* pSet)
: m_pStringSet		(pSet)
, m_pStringSpace	(pSet, rtLSTORE_CPx_Content, RTYPE_C_CharUV)
{
    align ();
}

/*************************
 *****  Destruction  *****
 *************************/

rtLSTORE_StringSet::~rtLSTORE_StringSet ()
{
}

/*************************************
 *****  Deferred Initialization  *****
 *************************************/

void rtLSTORE_StringSet::initializeDPT ()
{
    m_pDPT.claim (rtLSTORE_CPD_RowPTokenCPD (m_pStringSet));
}


/********************
 *****  Access  *****
 ********************/

void rtLSTORE_StringSet::Locate (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Locate (pKeys, rpReordering, rResult);
}

bool rtLSTORE_StringSet::Locate (char const* pKey, unsigned int& rxElement) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Locate (pKey, rxElement);
}


/********************
 *****  Update  *****
 ********************/

void rtLSTORE_StringSet::Insert (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Insert (pKeys, rpReordering, rResult);
}

void rtLSTORE_StringSet::Delete (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Delete (pKeys, rpReordering, rResult);
}

/*---------------------------------------------------------------------
 *  The scalar 'Insert' and 'Delete' routines return true if the set was
 *  updated and false if no changes were made.
 *---------------------------------------------------------------------
 */
unsigned int rtLSTORE_StringSet::Insert (char const* pKey, unsigned int& rxElement)
{
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Insert (pKey, rxElement);
}

unsigned int rtLSTORE_StringSet::Delete (char const* pKey)
{
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Delete (pKey);
}


/******************************
 *****  Update Utilities  *****
 ******************************/

void rtLSTORE_StringSet::install (unsigned int xElement, char const* pKey) {
//  Determine the required string space expansion, ...
    size_t sAddedElement = strlen (pKey) + 1;
    rtPTOKEN_NewShiftPTConstructor (
	m_pStringSet, rtLSTORE_CPx_ContentPToken
    )->AppendAdjustment (origin (xElement), sAddedElement)->ToPToken ()->release ();

//  ... grow the string space, ...
    rtREFUV_Type_Reference iReference; {
	M_CPD* pReferenceCod = ptoken ();
	pReferenceCod->retain ();
	DSC_InitReferenceScalar (iReference, pReferenceCod, xElement);
    }

    rtLSTORE_AlignUsingRefSelctList (m_pStringSet, &iReference, sAddedElement);
    rtCHARUV_Align (m_pStringSpace);
    refreshCachedPointers ();

//  ... install the strings, ...
    strcpy (storage (xElement), pKey);

//  ... and return:
    DSC_ClearScalar (iReference);
}


void rtLSTORE_StringSet::install (
    rtLINK_CType* pElements, VOrdered<VCollectionOfStrings,char const*>& rAdditions
)
{
//  Determine the required string space expansion, ...
    VCPDReference pStringLengths (
	0, rtINTUV_New (
	    pElements->PPT (), m_pStringSet->KOT ()->TheIntegerPTokenCPD ()
	)
    );
    {
	size_t sPreviousAdditions = 0;
	int *pStringLength = rtINTUV_CPD_Array (pStringLengths);

	rtPTOKEN_CType* pNewContentDPTConstructor = rtPTOKEN_NewShiftPTConstructor (
	    m_pStringSet, rtLSTORE_CPx_ContentPToken
	);
	rAdditions.resetCursor ();

#	define handleNil(c,n,r)
#	define outputAdjustment(r) {\
	    size_t sAddedElement = strlen (rAdditions.element ()) + 1;\
	    pNewContentDPTConstructor->AppendAdjustment (\
		origin (r) + sPreviousAdditions, sAddedElement\
	    );\
	    sPreviousAdditions += sAddedElement;\
	    *pStringLength++ = sAddedElement;\
	}
#	define handleRepeat(c, n, r) {\
	    outputAdjustment (r);\
	    rAdditions.skip (n);\
	}
#	define handleRange(c, n, r) {\
	    while (n-- > 0) {\
		outputAdjustment (r++);\
		rAdditions.goToNextElement ();\
	    }\
	}

	rtLINK_TraverseRRDCList (pElements, handleNil, handleRepeat, handleRange);
	pNewContentDPTConstructor->ToPToken ()->release ();
    }

#   undef handleRange
#   undef handleRepeat

//  ... grow the string space, ...
    rtLSTORE_AlignUsingLCSelctLists (m_pStringSet, pElements, pStringLengths);
    rtCHARUV_Align (m_pStringSpace);
    refreshCachedPointers ();

//  ... install the strings, ...
#   define handleRepeat(c, n, r) {\
	strcpy (storage (r), rAdditions.element ());\
	rAdditions.skip (n);\
    }
#   define handleRange(c, n, r) {\
	while (n-- > 0) {\
	    strcpy (storage (r++), rAdditions.element ());\
	    rAdditions.goToNextElement ();\
	}\
    }

    rAdditions.resetCursor ();
    rtLINK_TraverseRRDCList (
	pElements,
	handleNil,
	handleRepeat,
	handleRange
    );

#   undef handleRange
#   undef handleRepeat
#   undef handleNil

//  ... and return:
}



/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*>;

template void rtDICTIONARY_Insert<VCollectionOfStrings> (
    M_CPD *pDictionary, VCollectionOfStrings *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);
template void rtDICTIONARY_Delete<VCollectionOfStrings> (
    M_CPD *pDictionary, VCollectionOfStrings *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);
template void rtDICTIONARY_Locate<VCollectionOfStrings> (
    M_CPD *pDictionary, VCollectionOfStrings *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);

template void rtDICTIONARY_Insert<VCollectionOfUnsigned32> (
    M_CPD *pDictionary, VCollectionOfUnsigned32 *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);
template void rtDICTIONARY_Delete<VCollectionOfUnsigned32> (
    M_CPD *pDictionary, VCollectionOfUnsigned32 *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);
template void rtDICTIONARY_Locate<VCollectionOfUnsigned32> (
    M_CPD *pDictionary, VCollectionOfUnsigned32 *pKeys, M_CPD *&rpReordering, VAssociativeResult &rAssociation
);


/************************************
 ************************************
 *****  Construction Utilities  *****
 ************************************
 ************************************/

M_CPD* VCollectionOfStrings::StringOriginsContainer (DSC_Descriptor& rSource) {
    M_CPD* pStringOriginsContainer = NilOf (M_CPD*);

    switch ((RTYPE_Type)M_CPD_RType (m_pStringStore)) {
    case RTYPE_C_Block:
	switch (rSource.pointerType ()) {
	case DSC_PointerType_Scalar:
	    DSC_DuplicateScalar (
		m_iStringOriginsReference, DSC_Descriptor_Scalar (rSource)
	    );
	    break;

	case DSC_PointerType_Value:
	    pStringOriginsContainer = DSC_Descriptor_Value (rSource);
	    pStringOriginsContainer->retain ();
	    break;

	default:
	    raiseUnimplementedOperationException (
		"VCollectionOfStrings: Unexpected B-String Pointer Type %u.",
		rSource.pointerType ()
	    );
	    break;
	}
	break;

    case RTYPE_C_ListStore: {
	    rtLSTORE_Align (m_pStringStore);
	    M_CPD* pStoragePToken = rtLSTORE_CPD_ContentPTokenCPD (m_pStringStore);

	    switch (rSource.pointerType ()) {
	    case DSC_PointerType_Scalar: {
		    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rSource));

		    unsigned int xString = DSC_Descriptor_Scalar_Int (rSource);
		    unsigned int xOrigin = xString < rtPTOKEN_CPD_BaseElementCount (
			DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (rSource))
		    ) ? rtLSTORE_CPD_BreakpointArray (m_pStringStore)[xString]
		    : rtPTOKEN_CPD_BaseElementCount (pStoragePToken);

		    pStoragePToken->retain ();
		    DSC_InitReferenceScalar (
			m_iStringOriginsReference, pStoragePToken, xOrigin
		    );
		}
		break;

	    case DSC_PointerType_Link: {
		    rtLINK_CType* pStrings = DSC_Descriptor_Link (rSource)->Align ();

		    pStringOriginsContainer = rtREFUV_New (
			pStrings->PPT (), pStoragePToken
		    );
		    unsigned int const* pSourceOrigins = rtLSTORE_CPD_BreakpointArray (
			m_pStringStore
		    );
		    unsigned int* pTargetOrigins = UV_CPD_Array (
			pStringOriginsContainer, unsigned int
		    );

#		    define handleRange(c, n, r) {\
			memcpy (pTargetOrigins, pSourceOrigins + r, n * sizeof (unsigned int));\
			pTargetOrigins += n;\
		    }
#		    define handleRepetition(c, n, r) {\
			unsigned int xOrigin = pSourceOrigins[r];\
			while (n-- > 0)\
			    *pTargetOrigins++ = xOrigin;\
		    }
#		    define handleNil(c, n, r) {\
			unsigned int xOrigin = rtPTOKEN_CPD_BaseElementCount (pStoragePToken);\
			while (n-- > 0)\
			    *pTargetOrigins++ = xOrigin;\
		    }

		    rtLINK_TraverseRRDCList (
			pStrings, handleNil, handleRepetition, handleRange
		    );

#		    undef handleRange
#		    undef handleRepetition
#		    undef handleNil
		}
		break;

	    default:
		raiseUnimplementedOperationException (
		    "VCollectionOfStrings: Unexpected L-String Pointer Type %u.",
		    rSource.pointerType ()
		);
		break;
	    }
	    pStoragePToken->release ();
	}
	break;

    default:
	raiseComponentTypeException ("String Store", m_pStringStore);
	break;
    }

    return pStringOriginsContainer;
}


M_CPD* VCollectionOfStrings::StringStorageContainer (DSC_Descriptor& rSource) {
    M_CPD* pStorageContainer = rSource.storeCPD ();

    switch ((RTYPE_Type)M_CPD_RType (pStorageContainer))
    {
    case RTYPE_C_ListStore:
	pStorageContainer = rtLSTORE_CPD_ContentStringsCPD (pStorageContainer);
	break;

    default:
	pStorageContainer->retain ();
	break;
    }

    return pStorageContainer;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VCollectionOfStrings::VCollectionOfStrings (M_CPD* pDPT, DSC_Descriptor& rSource)
: VCollectionOfOrderables	(pDPT)
, m_pStringStore		(rSource.storeCPD ())
, m_pStringOriginsContainer	(0, StringOriginsContainer (rSource))
, m_pStringStorageContainer	(0, StringStorageContainer (rSource))
{
    refreshCache ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VCollectionOfStrings::~VCollectionOfStrings ()
{
    if (isScalar ())
    {
	DSC_ClearScalar (m_iStringOriginsReference);
    }
}


/*******************************
 *******************************
 *****  Cache Maintenance  *****
 *******************************
 *******************************/

void VCollectionOfStrings::refreshCache ()
{
    VCollectionOfOrderables::refreshCache ();

//  Refresh the cached origins array pointer, ...
    if (isScalar ())
	m_pStringOriginsArray = (unsigned int const*)&DSC_Scalar_Int (m_iStringOriginsReference);
    else switch ((RTYPE_Type)M_CPD_RType (m_pStringOriginsContainer))
    {
    case RTYPE_C_RefUV:
	rtREFUV_Align (m_pStringOriginsContainer);
	/*****  NO BREAK  *****/

    case RTYPE_C_IntUV:
	m_pStringOriginsArray = UV_CPD_Array (m_pStringOriginsContainer, unsigned int);
	break;

    default:
	raiseComponentTypeException ("Origins Container", m_pStringOriginsContainer);
	break;
    }

//  Refresh cached storage array pointer, ...
    switch ((RTYPE_Type)M_CPD_RType (m_pStringStorageContainer))
    {
    case RTYPE_C_CharUV:
	rtCHARUV_Align (m_pStringStorageContainer);
	m_pStringStorageArray = rtCHARUV_CPD_Array (m_pStringStorageContainer);
	m_sStringStorageArray = rtPTOKEN_BaseElementCount (
	    m_pStringStorageContainer, UV_CPx_PToken
	);
	if (isScalar ())
	    rtREFUV_AlignReference (&m_iStringOriginsReference);
	break;

    case RTYPE_C_Block:
	m_pStringStorageArray = rtBLOCK_CPD_StringSpace (m_pStringStorageContainer);
	m_sStringStorageArray = UINT_MAX;
	break;

    default:
	raiseComponentTypeException ("Storage Container", m_pStringStorageContainer);
	break;
    }
}


/********************************
 ********************************
 *****  Element Comparison  *****
 ********************************
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to compare two elements given their indices.
 *
 *  Arguments:
 *	xElement1, xElement2	- the indices of the elements to compare.
 *
 *  Returns:
 *	A value
 *		 < 0 if 'Elements (xElement1) <  Elements (xElement2)'
 *		== 0 if 'Elements (xElement1) == Elements (xElement2)'
 *		 > 0 if 'Elements (xElement1) >  Elements (xElement2)'.
 *
 *  Notes:
 *	This predicate sorts un-real strings after all real strings.
 *
 *****/
int VCollectionOfStrings::compare (unsigned int xElement1, unsigned int xElement2) const
{
    char const* pString1;
    char const* pString2;

    return !GetElement (xElement1, pString1)
	// e1 isn't real => if e2 is real, then e1 > e2, else e1 == e2
	? GetElement (xElement2, pString2)
	: GetElement (xElement2, pString2)
	? strcmp (pString1, pString2)
	// e1 is real, e2 isn't => e1 < e2
	: -1;
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VCollectionOfStrings::insertInto (
    M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
	rtDICTIONARY_Insert (pSet, this, rpReordering, rAssociation);
	break;
    case RTYPE_C_ListStore:
	if (rtLSTORE_StringSet::IsAStringSet (pSet)) {
	    rtLSTORE_StringSet iSet (pSet);
	    iSet.Insert (this, rpReordering, rAssociation);
	}
	break;
    default:
	break;
    }
}

void VCollectionOfStrings::locateIn (
    M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
	rtDICTIONARY_Locate (pSet, this, rpReordering, rAssociation);
	break;
    case RTYPE_C_ListStore:
	if (rtLSTORE_StringSet::IsAStringSet (pSet)) {
	    rtLSTORE_StringSet iSet (pSet);
	    iSet.Locate (this, rpReordering, rAssociation);
	}
	break;
    default:
	break;
    }
}

void VCollectionOfStrings::deleteFrom (
    M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
	rtDICTIONARY_Delete (pSet, this, rpReordering, rAssociation);
	break;
    case RTYPE_C_ListStore:
	if (rtLSTORE_StringSet::IsAStringSet (pSet)) {
	    rtLSTORE_StringSet iSet (pSet);
	    iSet.Delete (this, rpReordering, rAssociation);
	}
	break;
    default:
	break;
    }
}

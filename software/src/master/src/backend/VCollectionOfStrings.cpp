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

#include "VAssociativeCursor.h"
#include "VLinkCReference.h"


/********************************
 ********************************
 *****  rtLSTORE_StringSet  *****
 ********************************
 ********************************/

/*************************************************
 *****  Construction Precondition Predicate  *****
 *************************************************/

bool rtLSTORE_Handle::isAStringSet () const {
    if (!isAStringStore ())
	return false;

    M_CPreamble *pRowPTokenCP = GetContainerAddress (rowPTokenPOP ());
    if (pRowPTokenCP)
	return !rtPTOKEN_PT_Independent (M_CPreamble_ContainerBaseAsType (pRowPTokenCP, rtPTOKEN_Type));

    rtPTOKEN_Handle::Reference pRowPToken (rowPTokenHandle ());
    return pRowPToken->isDependent ();
}


/**************************
 *****  Construction  *****
 **************************/

rtLSTORE_StringSet::rtLSTORE_StringSet (rtLSTORE_Handle *pSet) {
    m_pStrings.setTo (pSet);
}

/*************************
 *****  Destruction  *****
 *************************/

rtLSTORE_StringSet::~rtLSTORE_StringSet () {
}

/*************************************
 *****  Deferred Initialization  *****
 *************************************/

void rtLSTORE_StringSet::initializeDPT () {
    m_pDPT.setTo (m_pStrings.store ()->getPToken ());
}


/********************
 *****  Access  *****
 ********************/

void rtLSTORE_StringSet::Locate (
    VCollectionOfStrings* pKeys, M_CPD *&rpReordering, VAssociativeResult &rResult
) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Locate (pKeys, rpReordering, rResult);
}

bool rtLSTORE_StringSet::Locate (char const* pKey, unsigned int &rxElement) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Locate (pKey, rxElement);
}


/********************
 *****  Update  *****
 ********************/

void rtLSTORE_StringSet::align () {
    refreshCachedPointers ();
}

void rtLSTORE_StringSet::refreshCachedPointers () {
    m_pStrings.align ();
}

void rtLSTORE_StringSet::Insert (
    VCollectionOfStrings* pKeys, M_CPD *&rpReordering, VAssociativeResult &rResult
) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Insert (pKeys, rpReordering, rResult);
}

void rtLSTORE_StringSet::Delete (
    VCollectionOfStrings* pKeys, M_CPD *&rpReordering, VAssociativeResult &rResult
) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Delete (pKeys, rpReordering, rResult);
}

/*---------------------------------------------------------------------
 *  The scalar 'Insert' and 'Delete' routines return true if the set was
 *  updated and false if no changes were made.
 *---------------------------------------------------------------------
 */
unsigned int rtLSTORE_StringSet::Insert (char const* pKey, unsigned int &rxElement) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Insert (pKey, rxElement);
}

unsigned int rtLSTORE_StringSet::Delete (char const* pKey) {
    VAssociativeCursor<rtLSTORE_StringSet,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Delete (pKey);
}


/******************************
 *****  Update Utilities  *****
 ******************************/

void rtLSTORE_StringSet::install (unsigned int xElement, char const* pKey) {
//  Determine the required string space expansion, ...
    size_t sAddedElement = strlen (pKey) + 1;

    rtPTOKEN_Handle::Reference pContentPToken;
    m_pStrings.store()->getContentPToken (pContentPToken);
    pContentPToken->makeAdjustments ()->AppendAdjustment (origin (xElement), sAddedElement)->ToPToken ()->discard ();

//  ... grow the string space, ...
    rtREFUV_Type_Reference iReference;
    iReference.constructReference (ptoken (), xElement);

    m_pStrings.store()->alignUsingSelectedLists (iReference, sAddedElement);
    refreshCachedPointers ();

//  ... install the strings, ...
    strcpy (storage (xElement), pKey);

//  ... and return:
    iReference.destroy ();
}


void rtLSTORE_StringSet::install (
    rtLINK_CType* pElements, VOrdered<VCollectionOfStrings,char const*> &rAdditions
)
{
//  Determine the required string space expansion, ...
    VCPDReference pStringLengths (
	0, rtINTUV_New (pElements->PPT (), m_pStrings.store()->KOT ()->TheIntegerPTokenHandle ())
    );
    {
	size_t sPreviousAdditions = 0;
	int *pStringLength = rtINTUV_CPD_Array (pStringLengths);

	rtPTOKEN_Handle::Reference pContentDPT;
	m_pStrings.store ()->getContentPToken (pContentDPT);
	rtPTOKEN_CType* pNewContentDPTConstructor = pContentDPT->makeAdjustments ();
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
	pNewContentDPTConstructor->ToPToken ()->discard ();
    }

#   undef handleRange
#   undef handleRepeat

//  ... grow the string space, ...
    m_pStrings.store ()->alignUsingSelectedLists (pElements, pStringLengths);
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


/************************************
 ************************************
 *****  Construction Utilities  *****
 ************************************
 ************************************/

M_CPD *VCollectionOfStrings::StringIndexContainer (DSC_Descriptor &rSource) {
    M_CPD *pStringIndexContainer = NilOf (M_CPD*);

    switch (m_pStringStore->rtype ()) {
    case RTYPE_C_Block:
	switch (rSource.pointerType ()) {
	case DSC_PointerType_Scalar:
	    m_iStringIndexReference.constructFrom (DSC_Descriptor_Scalar (rSource));
	    break;

	case DSC_PointerType_Value:
	    pStringIndexContainer = DSC_Descriptor_Value (rSource);
	    pStringIndexContainer->retain ();
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
	    m_pStringStore->align ();
	    switch (rSource.pointerType ()) {
	    case DSC_PointerType_Scalar: {
		    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rSource));
		    m_iStringIndexReference.constructFrom (DSC_Descriptor_Scalar (rSource));
		}
		break;

	    case DSC_PointerType_Link:
		pStringIndexContainer = DSC_Descriptor_Link (rSource)->ToRefUV ();
		break;

	    default:
		raiseUnimplementedOperationException (
		    "VCollectionOfStrings: Unexpected L-String Pointer Type %u.",
		    rSource.pointerType ()
		);
		break;
	    }
	}
	break;

    default:
	raiseComponentTypeException ("String Store", m_pStringStore->rtype ());
	break;
    }

    return pStringIndexContainer;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VCollectionOfStrings::VCollectionOfStrings (rtPTOKEN_Handle *pDPT, DSC_Descriptor &rSource)
: VCollectionOfOrderables	(pDPT)
, m_pStringStore		(rSource.store ())
, m_pStringIndexContainer	(0, StringIndexContainer (rSource))
{
    refreshCache ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VCollectionOfStrings::~VCollectionOfStrings () {
    if (isScalar ()) {
	m_iStringIndexReference.destroy ();
    }
}


/*******************************
 *******************************
 *****  Cache Maintenance  *****
 *******************************
 *******************************/

void VCollectionOfStrings::refreshCache () {
    VCollectionOfOrderables::refreshCache ();

//  Refresh the indices array pointers, ...
    if (isScalar ())
	m_pStringIndexArray = (unsigned int const*)&DSC_Scalar_Int (m_iStringIndexReference);
    else switch (m_pStringIndexContainer->RType ()) {
    case RTYPE_C_RefUV:
	m_pStringIndexContainer->align ();
	/*****  NO BREAK  *****/

    case RTYPE_C_IntUV:
	m_pStringIndexArray = UV_CPD_Array (m_pStringIndexContainer, unsigned int);
	break;

    default:
	raiseComponentTypeException ("Origins Container", m_pStringIndexContainer);
	break;
    }

//  Refresh cached storage array pointer, ...
    if (!m_pBlockStrings.setTo (m_pStringStore) && !m_pLStoreStrings.setTo (m_pStringStore))
	raiseComponentTypeException ("Storage Container", m_pStringStore->rtype ());
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
int VCollectionOfStrings::compare (unsigned int xElement1, unsigned int xElement2) const {
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
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeInsert (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeInsert_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Insert (pElements, rpReordering, rAssociation);
}

void rtLSTORE_Handle::associativeInsert_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    if (isAStringSet ()) {
	rtLSTORE_StringSet iSet (this);
	iSet.Insert (pElements, rpReordering, rAssociation);
    }
}

void VCollectionOfStrings::locateIn (
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeLocate (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeLocate_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Locate (pElements, rpReordering, rAssociation);
}

void rtLSTORE_Handle::associativeLocate_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    if (isAStringSet ()) {
	rtLSTORE_StringSet iSet (this);
	iSet.Locate (pElements, rpReordering, rAssociation);
    }
}

void VCollectionOfStrings::deleteFrom (
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeDelete (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeDelete_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Delete (pElements, rpReordering, rAssociation);
}

void rtLSTORE_Handle::associativeDelete_(
    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    if (isAStringSet ()) {
	rtLSTORE_StringSet iSet (this);
	iSet.Delete (pElements, rpReordering, rAssociation);
    }
}

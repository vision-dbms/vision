/*****  Identity Primitives Facility Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfIDENTITY

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"

#include "RTdictionary.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTptoken.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

/*****  Self  *****/
#include "PFidentity.h"


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
 *	  10 -  12                      *
 *	  20 -  34			*
 *	  90 -  98			*
 *	 100 - 108			*
 *======================================*/


/************************
 *****  Path Access *****
 ************************/

#define XReturnSelf				10
#define XReturnCurrent				11
#define XReturnSuper				12

/******************************
 *****  Identity Testing  *****
 ******************************/

#define XIsIdenticalTo				20
#define XIsntIdenticalTo			21

/**********************************************
 *****  Long Value Encoding and Decoding  *****
 **********************************************/

#define XLongValueGetPart			27
#define XLongValueSetPart			28

/**********************************
 *****  Pointer Construction  *****
 **********************************/

#define XZeroPointer				29

/********************************
 *****  Pointer Conversion  *****
 ********************************/

#define XIntegerAsReferenceTo			30
#define XValueAsPointerTo			31
#define XSelfPointerAsValue			32
#define XCurrentPointerAsValue			33


/***************************
 *****  Instantiation  *****
 ***************************/

#define XNewInstance				90
#define XNewPrototype				91

#define XExtendTo				92

/**********************
 *****  Deletion  *****
 **********************/

#define XDeleteInstance				95

/****************************
 *****  Specialization  *****
 ****************************/

#define XSpecialize				100
#define XSpecializeClass			101

#define XSpecializeBy				102

/*******************************
 *****  Cluster Migration  *****
 *******************************/

#define XEstablishResidence			103

/***************************
 *****  Cluster Query  *****
 ***************************/

#define XInstanceListCount			104

#define XInstanceList				105
#define XSelectedInstanceList			106


/***********************************
 ***********************************
 *****  Helpers and Utilities  *****
 ***********************************
 ***********************************/

/***********************************
 *****  Specialization Helper  *****
 ***********************************/

PrivateFnDef void loadDucWithSelfSpecialization (
    VTask* pTask, M_CPD *pDictionary, bool bPrototypeOnly
)
{
/*****  Obtain a descriptor for the objects to be specialized...  *****/
    DSC_Descriptor& rSelf = pTask->getSelf ();

/*****  ...obtain the store of and a pointer to those objects...  *****/
    M_CPD *pInstances;
    M_CPD *pSuperPointer;
    if (bPrototypeOnly) {
	pInstances = pTask->NewCodPToken (0);

	rtLINK_CType *emptyLinkc = rtLINK_PosConstructor (pInstances, -1)->Close (
	    V_TOTSC_PToken
	);

	DSC_Pointer iSuperPointer;
	iSuperPointer.constructComposition (emptyLinkc, rSelf.Pointer ());
	pSuperPointer = iSuperPointer.pointerCPD (pInstances);

	iSuperPointer.clear ();
	emptyLinkc->release ();
    }
    else {
	pInstances = pTask->NewCodPToken ();
	pSuperPointer = rSelf.pointerCPD (pInstances);
    }

/*****  ...create a new value store...  *****/
    M_CPD *pSpecialization = rtVSTORE_NewCluster (
	pInstances, pDictionary, rSelf.storeCPD (), pSuperPointer
    );

/*****  ...load the accumulator with a descriptor for it...  *****/
    if (bPrototypeOnly)
	pTask->loadDucWithReference (
	    pSpecialization, rtVSTORE_CPD_RowPTokenCPD (pSpecialization), 0
	);
    else
	pTask->loadDucWithCorrespondence (pSpecialization, rtVSTORE_CPx_RowPToken);

/*****  ... and clean up:  *****/
    pSuperPointer->release ();
    pInstances->release ();
}


/***********************************
 ***********************************
 *****  Primitive Definitions  *****
 ***********************************
 ***********************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that define the code bodies of the primitives.
 *---------------------------------------------------------------------------
 */

/*************************
 *****  Path Access  *****
 *************************/

/*---------------------------------------------------------------------------
 *****  '^self/^current' returning primitive.
 *
 *  Invocation Synopsis:
 *	anObject anyMessageOfAnyValence
 *
 *  Returns:
 *	'anObject'
 *
 *****/
V_DefinePrimitive (ReturnSelfCurrentOrSuper) {
    switch (V_TOTSC_Primitive) {
    case XReturnSelf:
	pTask->loadDucWithSelf ();
	break;
    case XReturnCurrent:
	pTask->loadDucWithCurrent ();
        break;
    case XReturnSuper:
	pTask->loadDucWithSuper ();
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ReturnSelfCurrentOrSuper");
	break;
    }
}


/******************************
 *****  Identity Testing  *****
 ******************************/

/****************************************
 *  Identity Testing Support Utilities  *
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Utility to compare the elements of two u-vectors for identity.
 *
 *  Arguments:
 *	elementType		- the element type of the u-vector elements.
 *	uv1/uv2			- standard CPDs for the u-vectors whose
 *				  elements are to be compared.
 *	identityLink		- an open link constructor which will be
 *				  initialized to identify the identical
 *				  positions in the two u-vectors.
 *	nonIdentityLinkC	- an open link constructor which will be
 *				  initialized to identify the non-identical
 *				  positions in the two u-vectors.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define FindIdenticalElementsInUVectors(elementType,uv1,uv2,identityLink,nonIdentityLink)\
{\
    elementType\
	*uv1p, *uv1l, *uv2p;\
    int\
	origin, count, currentCase, newCase;\
\
    uv1p = UV_CPD_Array (uv1, elementType);\
    uv2p = UV_CPD_Array (uv2, elementType);\
\
    uv1l = uv1p + UV_CPD_ElementCount (uv1);\
\
    origin = count = 0;\
    currentCase = true;\
    rtLINK_CType *currentLink = identityLink;\
    rtLINK_CType *otherLink = nonIdentityLink;\
\
    while (uv1p < uv1l) {\
        newCase = *uv1p++ == *uv2p++;\
	if (newCase == currentCase)\
	    count++;\
	else {\
	    rtLINK_AppendRange (currentLink, origin, count);\
\
	    origin += count;\
	    count = 1;\
\
	    rtLINK_CType *tempLink = currentLink;\
	    currentLink = otherLink;\
	    otherLink = tempLink;\
	    currentCase = newCase;\
	}\
    }\
    rtLINK_AppendRange (currentLink, origin, count);\
}

PrivateFnDef void FindIdenticalElementsInRefUVectors (
    M_CPD *refuv1, M_CPD *refuv2, rtLINK_CType *identityLink, rtLINK_CType *nonIdentityLink
) {
    FindIdenticalElementsInUVectors (
	rtREFUV_ElementType, refuv1, refuv2, identityLink, nonIdentityLink
    );
}


/*---------------------------------------------------------------------------
 *****  Internal routine to find the correspondences (i.e., positions whose
 *****  values match their position number) in a reference u-vector.
 *
 *  Arguments:
 *	uv			- a standard CPD for a reference u-vector.
 *	identityLink		- an open link constructor which will be
 *				  initialized to identify the correspondences
 *				  in the u-vector.
 *	nonIdentityLinkC	- an open link constructor which will be
 *				  initialized to identify the non-
 *				  correspondences in the u-vector.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on its parameters only.
 *
 *****/
PrivateFnDef void FindCorrespondencesInRefUV (
    M_CPD *uv, rtLINK_CType *identityLink, rtLINK_CType *nonIdentityLink
) {
    rtREFUV_ElementType
	*uvp, *uvl;
    int
	i, origin, count, currentCase, newCase;

    rtREFUV_Align (uv);

    uvp = rtREFUV_CPD_Array (uv);
    uvl = uvp + UV_CPD_ElementCount (uv);

    i = origin = count = 0;
    currentCase = true;
    rtLINK_CType *currentLink = identityLink;
    rtLINK_CType *otherLink = nonIdentityLink;

    while (uvp < uvl) {
        newCase = *uvp++ == i++;
	if (newCase == currentCase)
	    count++;
	else {
	    rtLINK_AppendRange (currentLink, origin, count);

	    origin += count;
	    count = 1;

	    rtLINK_CType *tempLink = currentLink;
	    currentLink = otherLink;
	    otherLink = tempLink;
	    currentCase = newCase;
	}
	rtLINK_AppendRange (currentLink, origin, count);
    }
}


/*---------------------------------------------------------------------------
 *****  Internal utility to find the correspondences in a link constructor.
 *
 *  Arguments:
 *	linkc			- the link constructor in which the
 *				  correspondences are to be identified.
 *	identityLink		- an open link constructor which will be
 *				  initialized to identify the correspondences
 *				  in the link.
 *	nonIdentityLinkC	- an open link constructor which will be
 *				  initialized to identify the non-
 *				  correspondences in the link.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on its parameters only.
 *
 *****/
PrivateFnDef void FindCorrespondencesInLinkC (
    rtLINK_CType *linkc, rtLINK_CType *identityLink, rtLINK_CType *nonIdentityLink
) {
    M_CPD *refuv = linkc->ToRefUV ();
    FindCorrespondencesInRefUV (refuv, identityLink, nonIdentityLink);
    refuv->release ();
}


/*---------------------------------------------------------------------------
 *****  Internal utility to find the identical elements in two link
 *****  constructors.
 *
 *  Arguments:
 *	lc1,lc2			- the link constructors to be compared.
 *	identityLink		- an open link constructor which will be
 *				  initialized to identify the identical
 *				  positions in the two link constructors.
 *	nonIdentityLinkC	- an open link constructor which will be
 *				  initialized to identify the non-identical
 *				  positions in the two link constructors.
 *
 *  Returns:
 *	NOTHING - Executed for side effects on its parameters only.
 *
 *****/
PrivateFnDef void FindIdenticalElementsInLinkCs (
    rtLINK_CType *lc1, rtLINK_CType *lc2, rtLINK_CType *identityLink, rtLINK_CType *nonIdentityLink
)
{
    M_CPD *refuv1 = lc1->ToRefUV ();
    M_CPD *refuv2 = lc2->ToRefUV ();

    FindIdenticalElementsInRefUVectors (
	refuv1, refuv2, identityLink, nonIdentityLink
    );

    refuv1->release ();
    refuv2->release ();
}


/*---------------------------------------------------------------------------
 *****  Internal utility to find the identical elements in a link and
 *****  reference u-vector.
 *
 *  Arguments:
 *	linkc			- the link constructor to be tested.
 *	refuv			- the reference u-vector to be tested.
 *	identityLink		- an open link constructor which will be
 *				  initialized to identify the identical
 *				  positions in the constructor and refuv.
 *	nonIdentityLinkC	- an open link constructor which will be
 *				  initialized to identify the non-identical
 *				  positions in the constructor and refuv.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on its parameter only.
 *
 *****/
PrivateFnDef void FindIdentElementsInLCAndRefUV (
    rtLINK_CType *linkc, M_CPD *refuv, rtLINK_CType *identityLink, rtLINK_CType *nonIdentityLink
) {
    M_CPD *tempuv = linkc->ToRefUV ();
    rtREFUV_Align (refuv);

    FindIdenticalElementsInRefUVectors (
	tempuv, refuv, identityLink, nonIdentityLink
    );

    tempuv->release ();
}


/********************************
 *  Identity Testing Primitive  *
 ********************************/

V_DefinePrimitive (TestForIdentity) {
/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse of this primitive if the operand is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	int xConverseSelector;
	switch (V_TOTSC_Primitive) {
	case XIsIdenticalTo:
	    xConverseSelector = KS__Equivalent;
	    break;
	case XIsntIdenticalTo:
	    xConverseSelector = KS__NotEquivalent;
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("TestForIdentity");
	    break;
	}

        pTask->sendBinaryConverseWithSelf (xConverseSelector);
	return;
    }

/*****  Otherwise, acquire both operands...  *****/
    pTask->normalizeDuc ();
    DSC_Descriptor& rSelf = pTask->getSelf ();

/*****  ...  decode the alias used to invoke the test...  *****/
    M_KOTM pIdentityKOTM, pNonIdentityKOTM;
    switch (V_TOTSC_Primitive) {
    case XIsIdenticalTo:
	pIdentityKOTM		= &M_KnownObjectTable::TheTrueClass;
	pNonIdentityKOTM	= &M_KnownObjectTable::TheFalseClass;
	break;
    case XIsntIdenticalTo:
	pIdentityKOTM		= &M_KnownObjectTable::TheFalseClass;
	pNonIdentityKOTM	= &M_KnownObjectTable::TheTrueClass;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("TestForIdentity");
        break;
    }

/*****  ... and perform the primitive:  *****/

/*****
 *  If the operands don't live in the same store or their pointer R-Types
 *  differ, they're NOT equivalent...
 *****/
    M_CPD *selfStore = rSelf.storeCPD ();
    M_CPD *parameterStore = pTask->ducStore ();

    RTYPE_Type selfPointerRType = rSelf.pointerRType ();
    if (RTYPE_C_Link == selfPointerRType)
        selfPointerRType = RTYPE_C_RefUV;

    RTYPE_Type parameterPointerRType = ADescriptor.pointerRType ();
    if (RTYPE_C_Link == parameterPointerRType)
        parameterPointerRType = RTYPE_C_RefUV;

    if (selfStore->DoesntName (parameterStore) || selfPointerRType != parameterPointerRType) {
        pTask->loadDucWithUndefined (pNonIdentityKOTM);
	return;
    }

/*****
 *  If the pointers are of type 'UndefUV', all elements are identical...
 *****/
    if (RTYPE_C_UndefUV == selfPointerRType) {
	pTask->loadDucWithUndefined (pIdentityKOTM);
	return;
    }

/*****
 *  If the operand stores and pointer R-Types match and the pointers have
 *  information content, the test must examine more detail...
 *****/
    int testResult;
    rtLINK_CType *identityLink, *nonIdentityLink;

    switch (rSelf.pointerType ()) {
    default:
        rSelf.complainAboutBadPointerType ("'TestForIdentity' Recipient");
        break;


/*---------------------------------------------------------------------------
 * 'TestForIdentity': ^self 'Scalar' Pointer Case...
 *---------------------------------------------------------------------------
 */
    case DSC_PointerType_Scalar:
	switch (selfPointerRType) {
	case RTYPE_C_CharUV:
	    testResult =
	        DSC_Descriptor_Scalar_Char (ADescriptor) ==
		DSC_Descriptor_Scalar_Char (rSelf);
	    break;
	case RTYPE_C_DoubleUV:
	    testResult =
	        DSC_Descriptor_Scalar_Double (ADescriptor) ==
		DSC_Descriptor_Scalar_Double (rSelf);
	    break;
	case RTYPE_C_FloatUV:
	    testResult =
	        DSC_Descriptor_Scalar_Float (ADescriptor) ==
		DSC_Descriptor_Scalar_Float (rSelf);
	    break;
	case RTYPE_C_IntUV:
	    testResult =
	        DSC_Descriptor_Scalar_Int (ADescriptor) ==
		DSC_Descriptor_Scalar_Int (rSelf);
	    break;
	case RTYPE_C_RefUV:
	    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rSelf));
	    if (DSC_PointerType_Identity == ADescriptor.pointerType ()) {
		testResult = 0 == DSC_Descriptor_Scalar_Int (rSelf);
		break;
	    }
	    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (ADescriptor));
	    testResult =
	        DSC_Descriptor_Scalar_Int (ADescriptor) ==
		DSC_Descriptor_Scalar_Int (rSelf);
	    break;
	case RTYPE_C_Unsigned64UV:
	    testResult =
	        DSC_Descriptor_Scalar_Unsigned64 (ADescriptor) ==
		DSC_Descriptor_Scalar_Unsigned64 (rSelf);
	    break;
	case RTYPE_C_Unsigned96UV:
	    testResult =
	        DSC_Descriptor_Scalar_Unsigned96 (ADescriptor) ==
		DSC_Descriptor_Scalar_Unsigned96 (rSelf);
	    break;
	case RTYPE_C_Unsigned128UV:
	    testResult =
	        DSC_Descriptor_Scalar_Unsigned128 (ADescriptor) ==
		DSC_Descriptor_Scalar_Unsigned128 (rSelf);
	    break;
	default:
	    pTask->raiseException (
		EC__UnknownUVectorRType,
		"TestForIdentity: Unrecognized Scalar Pointer R-Type %s",
		RTYPE_TypeIdAsString (selfPointerRType)
	    );
	    break;
	}
	pTask->loadDucWithUndefined (testResult ? pIdentityKOTM : pNonIdentityKOTM);
        break;


/*---------------------------------------------------------------------------
 * 'TestForIdentity': ^self 'Value' Pointer Case...
 *---------------------------------------------------------------------------
 */
    case DSC_PointerType_Value:
	identityLink	= pTask->NewSubset ();
	nonIdentityLink	= pTask->NewSubset ();
	switch (selfPointerRType) {
	case RTYPE_C_CharUV:
	    FindIdenticalElementsInUVectors (
		rtCHARUV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_DoubleUV:
	    FindIdenticalElementsInUVectors (
		rtDOUBLEUV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_FloatUV:
	    FindIdenticalElementsInUVectors (
		rtFLOATUV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_IntUV:
	    FindIdenticalElementsInUVectors (
		rtINTUV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_Unsigned64UV:
	    FindIdenticalElementsInUVectors (
		rtU64UV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_Unsigned96UV:
	    FindIdenticalElementsInUVectors (
		rtU96UV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	case RTYPE_C_Unsigned128UV:
	    FindIdenticalElementsInUVectors (
		rtU128UV_ElementType,
		DSC_Descriptor_Value (rSelf),
		DSC_Descriptor_Value (ADescriptor),
		identityLink,
		nonIdentityLink
	    )
	    break;
	default:
	    pTask->raiseException (
		EC__UnknownUVectorRType,
		"TestForIdentity: Unrecognized Value Pointer R-Type %s",
		RTYPE_TypeIdAsString (selfPointerRType)
	    );
	    break;
	}
	pTask->loadDucWithPredicateResult (
	    pIdentityKOTM, identityLink, pNonIdentityKOTM, nonIdentityLink
	);
        break;


/*---------------------------------------------------------------------------
 * 'TestForIdentity': ^self 'Identity' Pointer Case...
 *---------------------------------------------------------------------------
 */
    case DSC_PointerType_Identity:
	switch (ADescriptor.pointerType ()) {
	case DSC_PointerType_Scalar:
	    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (ADescriptor));
	    if (0 != DSC_Descriptor_Scalar_Int (ADescriptor)) {
		pTask->loadDucWithUndefined (pNonIdentityKOTM);
		break;
	    }
	case DSC_PointerType_Identity:
	    pTask->loadDucWithUndefined (pIdentityKOTM);
	    break;
	case DSC_PointerType_Link:
	    identityLink = pTask->NewSubset ();
	    nonIdentityLink = pTask->NewSubset ();
	    FindCorrespondencesInLinkC (
		DSC_Descriptor_Link (ADescriptor), identityLink, nonIdentityLink
	    );
	    pTask->loadDucWithPredicateResult (
		pIdentityKOTM, identityLink, pNonIdentityKOTM, nonIdentityLink
	    );
	    break;
	case DSC_PointerType_Reference:
	    identityLink    = pTask->NewSubset ();
	    nonIdentityLink = pTask->NewSubset ();
	    FindCorrespondencesInRefUV (
		DSC_Descriptor_Reference (ADescriptor), identityLink, nonIdentityLink
	    );
	    pTask->loadDucWithPredicateResult (
		pIdentityKOTM, identityLink, pNonIdentityKOTM, nonIdentityLink
	    );
	    break;
	default:
	    pTask->raiseException (
		EC__UnknownUVectorRType,
		"TestForIdentity: Unrecognized Reference Pointer R-Type %s[%d]",
		RTYPE_TypeIdAsString (selfPointerRType),
		ADescriptor.pointerType ()
	    );
	    break;
	}
        break;


/*---------------------------------------------------------------------------
 * 'TestForIdentity': ^self 'Link' Pointer Case...
 *---------------------------------------------------------------------------
 */
    case DSC_PointerType_Link:
	identityLink	= pTask->NewSubset ();
	nonIdentityLink = pTask->NewSubset ();
	switch (ADescriptor.pointerType ()) {
	case DSC_PointerType_Identity:
	    FindCorrespondencesInLinkC (
		DSC_Descriptor_Link (rSelf), identityLink, nonIdentityLink
	    );
	    break;
	case DSC_PointerType_Link:
	    FindIdenticalElementsInLinkCs (
		DSC_Descriptor_Link (rSelf),
		DSC_Descriptor_Link (ADescriptor),
		identityLink, nonIdentityLink
	    );
	    break;
	case DSC_PointerType_Reference:
	    FindIdentElementsInLCAndRefUV (
		DSC_Descriptor_Link (rSelf),
		DSC_Descriptor_Reference (ADescriptor),
		identityLink, nonIdentityLink
	    );
	    break;
	default:
	    pTask->raiseException (
		EC__UnknownUVectorRType,
		"TestForIdentity: Unrecognized Reference Pointer R-Type %s",
		 RTYPE_TypeIdAsString (selfPointerRType)
	    );
	    break;
	}
	pTask->loadDucWithPredicateResult (
	    pIdentityKOTM, identityLink, pNonIdentityKOTM, nonIdentityLink
	);
        break;
    }
}


/***********************************
 *****  Long Pointer Decoding  *****
 ***********************************/

V_DefinePrimitive (LongValueGetPart) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    RTYPE_Type xValueRType = rCurrent.pointerRType ();

    switch (xValueRType) {
    case RTYPE_C_Unsigned64UV:
	if (rCurrent.isScalar ()) {
	    pTask->loadDucWithDouble (
		DSC_Descriptor_Scalar_Unsigned64 (rCurrent)[V_TOTSC_PrimitiveFlags]
	    );
	}
	else {
	    M_CPD *pResultCPD = pTask->NewDoubleUV ();
	    double* pResultCursor = rtDOUBLEUV_CPD_Array (pResultCPD);
	    double* pResultLimit = pResultCursor + UV_CPD_ElementCount (pResultCPD);

	    rtU64UV_ElementType const* pSourceCursor = rtU64UV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );

	    while (pResultCursor < pResultLimit)
		*pResultCursor++ = (*pSourceCursor++)[V_TOTSC_PrimitiveFlags];

	    pTask->loadDucWithMonotype (pResultCPD);

	    pResultCPD->release ();
	}
	break;

    case RTYPE_C_Unsigned96UV:
	if (rCurrent.isScalar ()) {
	    pTask->loadDucWithDouble (
		DSC_Descriptor_Scalar_Unsigned96 (rCurrent)[V_TOTSC_PrimitiveFlags]
	    );
	}
	else {
	    M_CPD *pResultCPD = pTask->NewDoubleUV ();
	    double* pResultCursor = rtDOUBLEUV_CPD_Array (pResultCPD);
	    double* pResultLimit = pResultCursor + UV_CPD_ElementCount (pResultCPD);

	    rtU96UV_ElementType const* pSourceCursor = rtU96UV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );

	    while (pResultCursor < pResultLimit)
		*pResultCursor++ = (*pSourceCursor++)[V_TOTSC_PrimitiveFlags];

	    pTask->loadDucWithMonotype (pResultCPD);

	    pResultCPD->release ();
	}
	break;

    case RTYPE_C_Unsigned128UV:
	if (rCurrent.isScalar ()) {
	    pTask->loadDucWithDouble (
		DSC_Descriptor_Scalar_Unsigned128 (rCurrent)[V_TOTSC_PrimitiveFlags]
	    );
	}
	else {
	    M_CPD *pResultCPD = pTask->NewDoubleUV ();
	    double* pResultCursor = rtDOUBLEUV_CPD_Array (pResultCPD);
	    double* pResultLimit = pResultCursor + UV_CPD_ElementCount (pResultCPD);

	    rtU128UV_ElementType const* pSourceCursor = rtU128UV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );

	    while (pResultCursor < pResultLimit)
		*pResultCursor++ = (*pSourceCursor++)[V_TOTSC_PrimitiveFlags];

	    pTask->loadDucWithMonotype (pResultCPD);

	    pResultCPD->release ();
	}
	break;

    default:
	pTask->raiseException (
	    EC__UsageError,
	    "LongValueGetPart: Unrecognized Recipient Value Type: %s",
	    RTYPE_TypeIdAsString (xValueRType)
	);
	break;
    }
}


/***********************************
 *****  Long Pointer Encoding  *****
 ***********************************/

PrivateFnDef void SendLongValueSetPartConverse (VPrimitiveTask* pTask) {
    char const *pMessageName;
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	pMessageName = "__becomeLongValuePart0In:";
	break;
    case 1:
	pMessageName = "__becomeLongValuePart1In:";
	break;
    case 2:
	pMessageName = "__becomeLongValuePart2In:";
	break;
    case 3:
	pMessageName = "__becomeLongValuePart3In:";
	break;
    default:
	pMessageName = "__becomeLongValuePartXIn:";
	break;
    }
    pTask->sendBinaryConverseWithCurrent (pMessageName);
}


V_DefinePrimitive (LongValueSetPartContinuation) {
/*****  Acquire and inspect the operands again, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... send a converse message if appropriate, ...  *****/
    if (!pTask->ConvertDucVectorsToMonotypeIfPossible ()) {
	SendLongValueSetPartConverse (pTask);
	return;
    }
    pTask->normalizeDuc ();

/*****  ... confirm that the argument is now a double, ...  *****/
    if (RTYPE_C_DoubleUV != ADescriptor.pointerRType ())
	pTask->loadDucWithCurrent ();
    else {
/*****  ... determine the result's behavior, ...  *****/
	M_CPD *pRefStoreCPD = rCurrent.storeCPD ();

/*****  ... determine the result's reference p-token, ...  *****/
	M_CPD *pRPT; {
	    M_CPD *pRPTReference; int xRPTReference;
	    rCurrent.getRPTReference (pRPTReference, xRPTReference);
	    pRPT = rtPTOKEN_BasePToken (pRPTReference, xRPTReference);
	    pRPTReference->release ();
	}

/*****  ... decode the destination type, ...  *****/
	RTYPE_Type xLongValueRType = rCurrent.pointerRType ();

/*****  ... and update its pointer:  *****/
	switch (xLongValueRType) {
	case RTYPE_C_Unsigned64UV:
	    if (DucIsAScalar) {
		VkUnsigned64 iLongValue = DSC_Descriptor_Scalar_Unsigned64 (
		    rCurrent
		);
		iLongValue[
		    V_TOTSC_PrimitiveFlags
		] = (unsigned int)DSC_Descriptor_Scalar_Double (ADescriptor);
		DSC_Descriptor resultDescriptor;
		pRefStoreCPD->retain ();
		resultDescriptor.constructConstant (
		    V_TOTSC_PToken, pRefStoreCPD, pRPT, iLongValue
		);
		pTask->loadDucWithMoved (resultDescriptor);
	    }
	    else {
		M_CPD *pResultUVCPD = rtU64UV_New (V_TOTSC_PToken, pRPT);

		VkUnsigned64* pCurrentCursor = rtU64UV_CPD_Array (
		    DSC_Descriptor_Value (rCurrent)
		);
		double *pPartCursor = rtDOUBLEUV_CPD_Array (
		    DSC_Descriptor_Value (ADescriptor)
		);
		VkUnsigned64* pResultCursor = rtU64UV_CPD_Array (pResultUVCPD);
		VkUnsigned64* pResultLimit  = pResultCursor + UV_CPD_ElementCount (
		    pResultUVCPD
		);

		while (pResultCursor < pResultLimit) {
		    *pResultCursor = *pCurrentCursor++;
		    (*pResultCursor++)[
			V_TOTSC_PrimitiveFlags
		    ] = (unsigned int)*pPartCursor++;
		}

		pTask->loadDucWithMonotype (pRefStoreCPD, pResultUVCPD);
		pResultUVCPD->release ();
		pRPT->release ();
	    }
	    break;
	case RTYPE_C_Unsigned96UV:
	    if (DucIsAScalar) {
		VkUnsigned96 iLongValue = DSC_Descriptor_Scalar_Unsigned96 (
		    rCurrent
		);
		iLongValue[
		    V_TOTSC_PrimitiveFlags
		] = (unsigned int)DSC_Descriptor_Scalar_Double (ADescriptor);
		DSC_Descriptor resultDescriptor;
		pRefStoreCPD->retain ();
		resultDescriptor.constructConstant (
		    V_TOTSC_PToken, pRefStoreCPD, pRPT, iLongValue
		);
		pTask->loadDucWithMoved (resultDescriptor);
	    }
	    else {
		M_CPD *pResultUVCPD = rtU96UV_New (V_TOTSC_PToken, pRPT);

		VkUnsigned96* pCurrentCursor = rtU96UV_CPD_Array (
		    DSC_Descriptor_Value (rCurrent)
		);
		double *pPartCursor = rtDOUBLEUV_CPD_Array (
		    DSC_Descriptor_Value (ADescriptor)
		);
		VkUnsigned96* pResultCursor = rtU96UV_CPD_Array (pResultUVCPD);
		VkUnsigned96* pResultLimit  = pResultCursor + UV_CPD_ElementCount (
		    pResultUVCPD
		);

		while (pResultCursor < pResultLimit) {
		    *pResultCursor = *pCurrentCursor++;
		    (*pResultCursor++)[
			V_TOTSC_PrimitiveFlags
		    ] = (unsigned int)*pPartCursor++;
		}

		pTask->loadDucWithMonotype (pRefStoreCPD, pResultUVCPD);
		pResultUVCPD->release ();
		pRPT->release ();
	    }
	    break;
	case RTYPE_C_Unsigned128UV:
	    if (DucIsAScalar) {
		VkUnsigned128 iLongValue = DSC_Descriptor_Scalar_Unsigned128 (
		    rCurrent
		);
		iLongValue[
		    V_TOTSC_PrimitiveFlags
		] = (unsigned int)DSC_Descriptor_Scalar_Double (ADescriptor);
		DSC_Descriptor resultDescriptor;
		pRefStoreCPD->retain ();
		resultDescriptor.constructConstant (
		    V_TOTSC_PToken, pRefStoreCPD, pRPT, iLongValue
		);
		pTask->loadDucWithMoved (resultDescriptor);
	    }
	    else {
		M_CPD *pResultUVCPD = rtU128UV_New (V_TOTSC_PToken, pRPT);

		VkUnsigned128* pCurrentCursor = rtU128UV_CPD_Array (
		    DSC_Descriptor_Value (rCurrent)
		);
		double *pPartCursor = rtDOUBLEUV_CPD_Array (
		    DSC_Descriptor_Value (ADescriptor)
		);
		VkUnsigned128* pResultCursor = rtU128UV_CPD_Array (pResultUVCPD);
		VkUnsigned128* pResultLimit  = pResultCursor + UV_CPD_ElementCount (
		    pResultUVCPD
		);

		while (pResultCursor < pResultLimit) {
		    *pResultCursor = *pCurrentCursor++;
		    (*pResultCursor++)[
			V_TOTSC_PrimitiveFlags
		    ] = (unsigned int)*pPartCursor++;
		}

		pTask->loadDucWithMonotype (pRefStoreCPD, pResultUVCPD);
		pResultUVCPD->release ();
		pRPT->release ();
	    }
	    break;
	default:
	    pRPT->release ();
	    rCurrent.complainAboutBadPointerType ("LongValueSetPart");
	    break;
	}
    }
}


V_DefinePrimitive (LongValueSetPart) {
/*****  Acquire the operand, ...  *****/
/*****  ... send a converse message if appropriate, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	SendLongValueSetPartConverse (pTask);
	return;
    }

/*****  ... prepare to invoke the continuation, ...  *****/
    pTask->setContinuationTo (LongValueSetPartContinuation);

/*****  ... sending the 'asDouble' message to the argument if necessary:  *****/
    if (RTYPE_C_DoubleUV != ADescriptor.pointerRType ()) {
	pTask->beginMessageCall ("asDouble", 0);
	pTask->commitRecipient	();
	pTask->commitCall	();
    }
}


/**********************************
 *****  Pointer Construction  *****
 **********************************/

V_DefinePrimitive (ZeroPointer) {
    DSC_Descriptor& rSelf = pTask->getSelf ();

    M_CPD *pRefStoreCPD = rSelf.storeCPD ();
    pRefStoreCPD->retain ();

    M_CPD *pRPT; {
	M_CPD *pRPTReference; int xRPTReference;
	rSelf.getRPTReference (pRPTReference, xRPTReference);
	pRPT = rtPTOKEN_BasePToken (pRPTReference, xRPTReference);
	pRPTReference->release ();
    }

/*************************************************************************************/
/*  Flag Encoding:
 *
 *	    Unsigned	    Signed	Floating	Object	
 *	    Integer	    Integer	Point		Reference	
 *	   =================================================================
 *	+ |   0		    8192	16384		24576
 * =========================================================================
 *	0 |   -		    char	-		-
 *	1 |   -		    -		-		-
 *	2 |   -		    long	float		Ref32
 *	3 |   Unsigned64    -		double		-
 *	4 |   Unsigned96    -		-		-
 *	5 |   Unsigned128   -		-		-
 *
 */
/*************************************************************************************/
    bool notProcessed = false;
    DSC_Descriptor result;
    switch (V_TOTSC_PrimitiveFlags / 8192) {
    case 0:	/* Unsigned Integers */
	switch (V_TOTSC_PrimitiveFlags % 8192) {
	case 3:		/* Unsigned-64 (unsigned long long) */
	    result.constructConstant (
		V_TOTSC_PToken, pRefStoreCPD, pRPT, VkUnsigned64::Zero ()
	    );
	    break;
	case 4:		/* Unsigned-96 */
	    result.constructConstant (
		V_TOTSC_PToken, pRefStoreCPD, pRPT, VkUnsigned96::Zero ()
	    );
	    break;
	case 5:		/* Unsigned-128 */
	    result.constructConstant (
		V_TOTSC_PToken, pRefStoreCPD, pRPT, VkUnsigned128::Zero ()
	    );
	    break;
	default:
	    notProcessed = true;
	    break;
	}
	break;

    case 1:	/* Signed Integers */
	switch (V_TOTSC_PrimitiveFlags % 8192) {
	case 0:		/* Signed-8 (char) */
	    result.constructConstant (V_TOTSC_PToken, pRefStoreCPD, pRPT, '\0');
	    break;
	case 2:		/* Signed-32 (int) */
	    result.constructConstant (V_TOTSC_PToken, pRefStoreCPD, pRPT, 0);
	    break;
	default:
	    notProcessed = true;
	    break;
	}
	break;

    case 2:	/* Floating Point Numbers */
	switch (V_TOTSC_PrimitiveFlags % 8192) {
	case 2:		/* Real-32 (float) */
	    result.constructConstant (V_TOTSC_PToken, pRefStoreCPD, pRPT, (float)0.0);
	    break;
	case 3:		/* Real-64 (double) */
	    result.constructConstant (V_TOTSC_PToken, pRefStoreCPD, pRPT, 0.0);
	    break;
	default:
	    notProcessed = true;
	    break;
	}
	break;

    case 3:	/* Object References */
	switch (V_TOTSC_PrimitiveFlags % 8192) {
	case 2:		/* Ref-32  */
	    result.constructReferenceConstant (V_TOTSC_PToken, pRefStoreCPD, pRPT, 0);
	    break;
	default:
	    notProcessed = true;
	    break;
	}
	break;

    default:
	notProcessed = true;
	break;
    }

    if (notProcessed) {
	pRPT->release ();
	pRefStoreCPD->release ();
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "ZeroPointer: Unrecognized Pointer Type Code %u",
	    V_TOTSC_PrimitiveFlags
	);
    }

    pTask->loadDucWithMoved (result);
}


/********************************
 *****  Pointer Conversion  *****
 ********************************/

V_DefinePrimitive (IntegerAsReferenceTo) {
/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (KS__ReferencedBy);
	return;
    }

/*****
 *  Obtain '^current' and punt if its pointer is not of R-Type integer...
 *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    RTYPE_Type currentPointerRType = rCurrent.pointerRType ();
    if (RTYPE_C_IntUV != currentPointerRType)
        return;

/*****
 *  Otherwise, construct references to the objects in the accumulator...
 *****/
    if (DucIsAScalar) {
    /*****
     *  Obtain the reference p-token and value of the new reference...
     *****/
	M_CPD *pRPT = ADescriptor.RPT ();
	int element = DSC_Descriptor_Scalar_Int (rCurrent);
	if (element < 0 || (size_t)element > rtPTOKEN_CPD_BaseElementCount (pRPT))
	    element = rtPTOKEN_CPD_BaseElementCount (pRPT);

    /*****  ... and initialize the reference.  *****/
	DSC_Descriptor_Pointer (ADescriptor).clear ();
	DSC_Descriptor_Pointer (ADescriptor).constructReferenceScalar (pRPT, element);
    }
    else {
    /*****  Normalize the accumulator...  *****/
	pTask->normalizeDuc ();

    /*****
     *  Obtain the reference P-Token and reference Nil of the reference
     *  target...
     *****/

    	M_CPD *refPTokenRefCPD; int refPTokenRefIndex;
	ADescriptor.getRPTReference (refPTokenRefCPD, refPTokenRefIndex);
	int refNil = rtPTOKEN_BaseElementCount(refPTokenRefCPD, refPTokenRefIndex);

    /*****  Obtain the integers to be converted to references...  *****/
	M_CPD *iuv = DSC_Descriptor_Value (rCurrent);

    /*****  Manufacture a new reference u-vector...  *****/
	M_CPD *ruv = rtREFUV_New (V_TOTSC_PToken, refPTokenRefCPD, refPTokenRefIndex);
	int *iuvp, *iuvl, *ruvp;
	for (
	    iuvl = UV_CPD_ElementCount (iuv)+(iuvp = rtINTUV_CPD_Array (iuv)),
	    ruvp = rtREFUV_CPD_Array (ruv);
	    iuvp < iuvl;
	)
	{
	    int element = *iuvp++;
	    *ruvp++ = element < 0 || element > refNil ? refNil : element;
	}

    /*****  ...install it in the descriptor to be returned.  *****/
	DSC_Descriptor_Pointer (ADescriptor).setTo (ruv);
	ruv->release ();

    /*****  ...and clean up.  *****/
	refPTokenRefCPD->release ();
    }
}


V_DefinePrimitive (ValueAsPointerTo) {
/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (KS__PointedToBy);
	return;
    }

/*****  Obtain '^current'...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... and create a new descriptor:  *****/
    if (rCurrent.holdsAScalarValue ()) {
    /*****
     *  Stuff '^current's scalar VALUE into the accumulator, preserving
     *  the reference P-Token of the accumulator...
     *****/
	M_CPD *pRPT;

	if (ADescriptor.holdsAScalarIdentity ()) {
	    /* Prepare for conversion to Scalar Descriptor */
	    pRPT = DSC_Descriptor_Identity (ADescriptor).PToken ();
	    DSC_Descriptor_Type (ADescriptor) = DSC_PointerType_Scalar;
	}
	else
	    pRPT = DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (ADescriptor));

	DSC_Descriptor_Scalar (ADescriptor) = DSC_Descriptor_Scalar (rCurrent);
	DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (ADescriptor)) = pRPT;
    }
    else if (rCurrent.holdsNonScalarValues ()) {
    /*****
     *  Stuff a copy of '^current's u-vector into the accumulator, preserving
     *  the reference P-Token of the accumulator...
     *****/
	pTask->normalizeDuc ();

	M_CPD *uvectorCopy = UV_CopyWithNewPToken (
	    DSC_Descriptor_Value (rCurrent), V_TOTSC_PToken
	);

	M_CPD *refPTokenRefCPD; int refPTokenRefIndex;
	ADescriptor.getRPTReference (refPTokenRefCPD, refPTokenRefIndex);
	uvectorCopy->StoreReference (UV_CPx_RefPToken, refPTokenRefCPD, refPTokenRefIndex);
	refPTokenRefCPD->release ();

	DSC_Descriptor_Pointer (ADescriptor).setTo (uvectorCopy);
	uvectorCopy->release ();
    }
}


V_DefinePrimitive (PointerAsValue) {
/*****  Obtain either '^self' or '^current' as appropriate...  *****/
    switch (V_TOTSC_Primitive) {
    case XSelfPointerAsValue:
	pTask->loadDucWithSelf ();
	break;
    case XCurrentPointerAsValue:
	pTask->loadDucWithCurrent ();
	break;
    default:
	pTask->raiseUnimplementedAliasException ("PointerAsValue");
        break;
    }
    DSC_Descriptor& rResult = pTask->ducMonotype ();

/*****  Decode the pointer R-Type into an object class...  *****/
    M_KOT *pKOT = pTask->codKOT ();
    M_KOTE const *pKOTE;
    switch (rResult.pointerRType ()) {
    case RTYPE_C_CharUV:
	pKOTE = &pKOT->TheCharacterClass;
	break;
    case RTYPE_C_DoubleUV:
	pKOTE = &pKOT->TheDoubleClass;
	break;
    case RTYPE_C_FloatUV:
	pKOTE = &pKOT->TheFloatClass;
	break;
    case RTYPE_C_IntUV:
	pKOTE = &pKOT->TheIntegerClass;
	break;
    case RTYPE_C_UndefUV:
	pKOTE = &pKOT->TheNAClass;
	break;
    default:
	pTask->raiseException (EC__UnknownUVectorRType, "PointerAsValue");
	break;
    }

    if (rResult.holdsAScalarValue ()) {
	rResult.setStoreTo (pKOTE->RetainedObjectCPD ());

	M_CPD *pOldRPT = DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (rResult));
	DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (rResult)) = pKOTE->RetainedPTokenCPD ();
	pOldRPT->release ();
    }

    else if (rResult.holdsNonScalarValues ()) {
	M_CPD *uvectorCopy = UV_Copy (DSC_Descriptor_Value (rResult));
	uvectorCopy->StoreReference (UV_CPx_RefPToken, pKOTE->PTokenCPD ());
	pTask->loadDucWithMonotype (pKOTE->ObjectCPD (), uvectorCopy);
	uvectorCopy->release ();
    }
}


/***************************
 *****  Instantiation  *****
 ***************************/

V_DefinePrimitive (NewInstance) {
/*****  Obtain '^self'...  *****/
    pTask->loadDucWithSelf ();

/*****  Validate that its store is a V-Store...  *****/
    M_CPD *selfStore = pTask->ducStore ();

/*****  Do not manufacture a new instance of a value...  *****/
    if (pTask->ducMonotype ().holdsValuesInAnyForm ())
	return;

/*****  Manufacture a new collection of instances...  *****/
    rtLINK_CType* newInstances = rtVSTORE_AddRows (selfStore, V_TOTSC_PToken);

/*****  and return them.  *****/
    if (IsNil (newInstances))
	pTask->loadDucWithNA ();
    else {
	DSC_Descriptor_Pointer (pTask->ducMonotype ()).setTo (newInstances);
	newInstances->release ();
    }
}


V_DefinePrimitive (ExtendTo) {
/*****  Decode the flags used to invoke this primitive, ...  *****/
    char const*		pConverseName = "addExtensionOf:";
    rtVSTORE_Extension	xExtensionOp = rtVSTORE_Extension_Add;
    switch (pTask->flags ()) {
    case 0:	//  ... add instance extension
	break;
    case 2:	//  ... locate or add instance extension
	pConverseName = "locateOrAddExtensionOf:";
	xExtensionOp = rtVSTORE_Extension_LocateOrAdd;
	break;
    case 3:	//  ... locate instance extension
	pConverseName = "locateExtensionOf:";
	xExtensionOp = rtVSTORE_Extension_Locate;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ExtendTo");
	break;
    }

/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithSelf (pConverseName);
	return;
    }

/*****  Do not manufacture a new instance of a value...  *****/
    if (pTask->ducMonotype ().holdsValuesInAnyForm ()) {
	pTask->loadDucWithNA ();
	return;
    }

/*****  Obtain the descendent store, ...  *****/
    M_CPD *pDescendentStore = pTask->ducMonotype ().storeCPD ();

    DSC_Descriptor& rSelf = pTask->getSelf ();
    rtLINK_CType *pInstances, *pInstanceGuard; {
/*****  Obtain ^self's store and pointer, ...  *****/
	M_CPD *pInheritanceStore = rSelf.storeCPD ();
	M_CPD *pInheritancePointer = rSelf.pointerCPD (pTask->ptoken ());

/*****  ... locate or create the requested extensions, ...  *****/
	pInstances = rtVSTORE_ExtendRowsTo (
	    xExtensionOp,
	    pInheritanceStore,
	    pInheritancePointer,
	    pDescendentStore,
	    pInstanceGuard,
	    NilOf (rtLINK_CType**)
	);

	pInheritancePointer->release ();
    }

/*****  and return them.  *****/
    if (pInstances) {
	pDescendentStore->retain (); // ... must retain store since duc may change
	VDescriptor *pGuardedDatum = pTask->loadDucWithGuardedDatum (pInstanceGuard);
	if (pGuardedDatum)
	    pGuardedDatum->setToMonotype (pDescendentStore, pInstances);
	pDescendentStore->release ();
	pInstances->release ();
    }
    else if (pInstanceGuard) {
	pInstanceGuard->release ();
	pTask->loadDucWithNA ();
    }
    else
	pTask->loadDucWithNA ();
}


V_DefinePrimitive (NewPrototype) {
/*****  Obtain '^self'...  *****/
    DSC_Descriptor& rSelf = pTask->getSelf ();
    M_CPD *pPrototypeStoreCPD = rSelf.storeCPD ();

/*****  ...and clone it:  *****/
    M_CPD *pClonedStorePTokenCPD = pTask->NewCodPToken (0);
    if (pTask->flags () % 2)
	rtPTOKEN_CPD_Independent (pClonedStorePTokenCPD) = false;

    pTask->loadDucWithReference (
	rtVSTORE_NewCluster (pClonedStorePTokenCPD, pPrototypeStoreCPD),
	pClonedStorePTokenCPD,
	0
    );
}


V_DefinePrimitive (EstablishResidence) {
/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithSelf ("provideResidenceFor:");
	return;
    }

/*****  Obtain destination ... *****/
    M_CPD *pPersistentStore = pTask->ducStore ();

/*****  Obtain '^self'...  *****/
    M_CPD *pTransientStore = pTask->getSelf ().storeCPD ();

/*****  ...and forward it:  *****/
    pTask->loadDucWithBoolean (rtVSTORE_ForwardCluster (pTransientStore, pPersistentStore));
}


/**********************
 *****  Deletion  *****
 **********************/

V_DefinePrimitive (DeleteInstance) {
#define handleNil(position, count, refNil) {\
    rtLINK_AppendRange (undeletedLinkC, position, count);\
}

#define handleRepeat(position, count, origin) {\
    rtLINK_AppendRepeat (deletedLinkC, position, count);\
    ptConstructor->AppendAdjustment (origin + 1 - totalDeleted++, -1);\
}

#define handleRange(position, count, origin) {\
    rtLINK_AppendRange (deletedLinkC, position, count);\
    ptConstructor->AppendAdjustment (origin + count - totalDeleted, -count);\
    totalDeleted += count;\
}

    unsigned int	elementPos,
			totalDeleted = 0;
    RTYPE_Type		rtype;

/*****  Obtain '^self'...  *****/
    pTask->loadDucWithSelf ();

/*****  Validate its store  *****/
    M_CPD *store = pTask->ducStore ();

    /***  Disallow deletion in ground stores that disallow deletions  ***/
    if (store->TransientExtensionIsA (VGroundStore::RTT) &&
	!((VGroundStore*)store->TransientExtension ())->allowsDelete ()
    ) {
	pTask->loadDucWithFalse ();
	return;
    }

    switch (rtype = (RTYPE_Type)M_CPD_RType (store)) {
    case RTYPE_C_ValueStore:
    /***  Disallow deletion of top level environment  ***/
	if (store->Names (ENVIR_KDsc_TheTLE.storeCPD ())) {
	    pTask->loadDucWithFalse ();
	    return;
	}

    case RTYPE_C_Dictionary:
    case RTYPE_C_ListStore:
    case RTYPE_C_Index:
    case RTYPE_C_Vector:
        break;

    default:
    /***  Indicate via the accumulator that deletion failed ***/
	pTask->loadDucWithFalse ();
        return;
    }

/*****  Get the ptoken to be edited *****/
    M_CPD *ptokenRefCPD; int ptokenRefIndex;
    ADescriptor.getRPTReference (ptokenRefCPD, ptokenRefIndex);
    M_CPD *pRPT = rtPTOKEN_BasePToken (ptokenRefCPD, ptokenRefIndex);
    rtPTOKEN_CType *ptConstructor = rtPTOKEN_NewShiftPTConstructor (pRPT, -1);

/*****  Ensure that V-Stores remain in control of their properties  *****/
    if (rtPTOKEN_CPD_IsDependent (pRPT) && rtype != RTYPE_C_ValueStore) {
        ptConstructor->discard ();
	ptokenRefCPD->release ();
	pRPT->release ();
    /***  Indicate via the accumulator that deletion failed ***/
	pTask->loadDucWithFalse ();
	return;
    }

    ptokenRefCPD->release ();
    pRPT->release ();

/*****  Do the editing ...  *****/
    if (ADescriptor.holdsAScalarReference () && rtPTOKEN_PTC_BaseCount (ptConstructor) > (
	    elementPos = DSC_Scalar_Int (
		*rtREFUV_AlignReference (&DSC_Descriptor_Scalar (ADescriptor))
	    )
	)
    )
    {
	ptConstructor->AppendAdjustment (elementPos + 1, -1);
	pTask->loadDucWithTrue ();
    }
    else if (ADescriptor.holdsALink ()) {
        rtLINK_CType *linkc = DSC_Descriptor_Link(ADescriptor)->Align();
	rtLINK_CType *deletedLinkC = pTask->NewSubset ();
	rtLINK_CType *undeletedLinkC = pTask->NewSubset ();

	rtLINK_TraverseRRDCList (linkc, handleNil, handleRepeat, handleRange);

	pTask->loadDucWithPredicateResult (deletedLinkC, undeletedLinkC);
    }
    else {
    /*****  It is a value and should not be deleted  *****/
        ptConstructor->discard ();
    /***  Indicate via the accumulator that deletion failed ***/
	pTask->loadDucWithFalse ();
	return;
    }

    pRPT = ptConstructor->ToPToken ();

    if (rtype == RTYPE_C_ValueStore &&
	store->ReferenceDoesntName (rtVSTORE_CPx_RowPToken, pRPT))
    {
    /*****  'Align' the store along its vertical dimension  *****/
    /*****   ... but only if necessary (hence the POP comparison) *****/
	store->StoreReference (rtVSTORE_CPx_RowPToken, pRPT);
    }
    pRPT->release ();

#undef handleNil
#undef handleRepeat
#undef handleRange
}


/****************************
 *****  Specialization  *****
 ****************************/

V_DefinePrimitive (Specialize) {
/*****  Decode the alias by which this primitive was invoked...  *****/
    bool bPrototypeOnly;
    switch (V_TOTSC_Primitive) {
    case XSpecialize:
	bPrototypeOnly = false;
	break;
    case XSpecializeClass:
	bPrototypeOnly = true;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("Specialize");
	break;
    }

/*****  ...create a new method dictionary...  *****/
    M_CPD *pDictionary = rtDICTIONARY_New (pTask->codScratchPad ());

/*****  ...create the specialization...  *****/
    loadDucWithSelfSpecialization (pTask, pDictionary, bPrototypeOnly);

/*****  ...and clean up.  *****/
    pDictionary->release ();
}

V_DefinePrimitive (SpecializeBy) {
/*****  Decode the flags used to invoke this primitive, ...  *****/
    bool bPrototypeOnly;
    switch (pTask->flags ()) {
    case 0:	//  instance extension
	bPrototypeOnly = false;
	break;
    case 1:	//  cluster extension
	bPrototypeOnly = true;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("SpecializeBy");
	break;
    }

/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithSelf (
	    bPrototypeOnly ? "specializePrototype:" : "specializeInstance:"
	);
	return;
    }

/*****  Do not manufacture a new instance of a value or special class...  *****/
    if (ADescriptor.holdsValuesInAnyForm() || ADescriptor.storeRType() != RTYPE_C_ValueStore) {
	pTask->loadDucWithNA ();
	return;
    }

//  Otherwise, obtain the specialization's dictionary, ...
    M_CPD *pDictionary = ADescriptor.dictionaryCPD ();

//  ... create the extension, ...
    loadDucWithSelfSpecialization (pTask, pDictionary, bPrototypeOnly);

//  ... and clean up:
    pDictionary->release ();
}


/***************************
 *****  Cluster Query  *****
 ***************************/

V_DefinePrimitive (InstanceListCount) {
/***** Obtain the store ... *****/
    DSC_Descriptor& rSelf = pTask->getSelf ();

/*****  Obtain the referential ptoken ... *****/
    M_CPD *pRPT; {
	M_CPD *pRPTReference; int xRPTReference;
	rSelf.getRPTReference (pRPTReference, xRPTReference);
	pRPT = rtPTOKEN_BasePToken (pRPTReference, xRPTReference);
	pRPTReference->release ();
    }

/*****  Return the instance list cardinality ... *****/
    RTYPE_Type referenceType = rSelf.pointerRType ();

    pTask->loadDucWithInteger (
	0 == rtPTOKEN_CPD_BaseElementCount (pRPT) ||
	    RTYPE_C_Link != referenceType && RTYPE_C_RefUV != referenceType
	? 0 : rtPTOKEN_CPD_BaseElementCount (pRPT)
    );

/*****  And cleanup...  *****/
    pRPT->release ();
}


V_DefinePrimitive (InstanceList) {
/***** Obtain the store ... *****/
    DSC_Descriptor& rSelf = pTask->getSelf ();
    M_CPD *pStoreCPD = rSelf.storeCPD ();

/*****  Obtain the referential ptoken ... *****/
    M_CPD *pRPT; {
	M_CPD *pRPTReference; int xRPTReference;
	rSelf.getRPTReference (pRPTReference, xRPTReference);
	pRPT = rtPTOKEN_BasePToken (pRPTReference, xRPTReference);
	pRPTReference->release ();
    }

/*****  Special case the Block case ... *****/
    if ((RTYPE_Type)M_CPD_RType (pStoreCPD) == RTYPE_C_Block)
	pStoreCPD = pRPT->GetBlockEquivalentClassFromPToken ();

/***** Make the content vector ... *****/
    M_CPD *vectorPToken, *vector;

    RTYPE_Type referenceType = rSelf.pointerRType ();
    if (0 == rtPTOKEN_CPD_BaseElementCount (pRPT) ||
	RTYPE_C_Link != referenceType && RTYPE_C_RefUV != referenceType
    )
    {
	vectorPToken = pTask->NewCodPToken (0);
	vector = rtVECTOR_New (vectorPToken);
	pRPT->release ();
    }
    else {
	vectorPToken = pTask->NewCodPToken (rtPTOKEN_CPD_BaseElementCount (pRPT));
	vector = rtVECTOR_New (vectorPToken);

	rtLINK_CType *assignmentLinkc = rtLINK_RefConstructor (vectorPToken, -1);
	rtLINK_AppendRange (
	    assignmentLinkc, 0, rtPTOKEN_CPD_BaseElementCount (vectorPToken)
	);
	assignmentLinkc->Close (pRPT);

	pStoreCPD->retain ();
	DSC_Descriptor elementDsc;
	elementDsc.constructIdentity (pStoreCPD, pRPT);

	rtVECTOR_Assign (vector, assignmentLinkc, &elementDsc);

	assignmentLinkc->release ();
	elementDsc.clear ();
    }

/***** Make the lstore ... *****/
    M_CPD *lstorePToken = pTask->NewCodPToken (1);

    rtLINK_CType *definitionLinkc = rtLINK_RefConstructor (lstorePToken, -1);
    rtLINK_AppendRepeat (
	definitionLinkc, 0, rtPTOKEN_CPD_BaseElementCount (vectorPToken)
    );
    definitionLinkc->Close (vectorPToken);

    M_CPD *lstore = rtLSTORE_NewCluster (definitionLinkc, vector);

    definitionLinkc->release ();
    vector->release ();
    vectorPToken->release ();

/****** Load the new lstore into the accumulator ... *****/
    pTask->loadDucWithReference (lstore, lstorePToken, 0);
}


V_DefinePrimitive (SelectedInstanceList) {
    int currentRow, firstRow, numberOfRows, totalSize;

/***** Acquire the operand of this primitive ...  *****/
/*****
 *  Send this primitive's binary converse if the operand is polymorphic ...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (KS__StoreSelectRows);
	return;
    }

/***** Obtain the store ... *****/
    M_CPD *pStoreCPD = pTask->ducStore ();

/*****  Obtain the referential ptoken ... *****/
    M_CPD *pRPT; {
	M_CPD *pRPTReference; int xRPTReference;
	ADescriptor.getRPTReference (pRPTReference, xRPTReference);
	pRPT = rtPTOKEN_BasePToken (pRPTReference, xRPTReference);
	pRPTReference->release ();
    }
    totalSize = rtPTOKEN_CPD_BaseElementCount (pRPT);

/*****  Special case the Block case ... *****/
    if ((RTYPE_Type)M_CPD_RType (pStoreCPD) == RTYPE_C_Block)
	pStoreCPD = pRPT->GetBlockEquivalentClassFromPToken ();

/*****  Access ^current ... *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Determine the row range ... *****/
    if (rCurrent.isScalar ()) {
	numberOfRows = DSC_Descriptor_Scalar_Int (rCurrent);
	currentRow = ADescriptor.holdsAScalarReference ()
	    ? DSC_Descriptor_Scalar_Int (ADescriptor) : totalSize;
    }
    else pTask->raiseUnimplementedOperationException (
	"SelectedInstanceList: Non-Scalar Operation Not Supported"
    );

    if (numberOfRows < 0) {
	numberOfRows *= -1;
	firstRow = currentRow - numberOfRows;
	if (firstRow < 0) {
	    numberOfRows += firstRow;
	    firstRow = 0;
	}
    }
    else {
	firstRow = currentRow;
	if ((firstRow + numberOfRows - 1) >= totalSize)
	    numberOfRows = totalSize - firstRow;
    }

/***** Make the content vector ... *****/
    M_CPD *vectorPToken = pTask->NewCodPToken (numberOfRows);
    M_CPD *vector = rtVECTOR_New (vectorPToken);

    RTYPE_Type referenceType = ADescriptor.pointerRType ();
    if (totalSize != 0 && (
	    RTYPE_C_Link == referenceType || RTYPE_C_RefUV == referenceType
	)
    )
    {
	rtLINK_CType* sourceLinkc = rtLINK_RefConstructor (pRPT, -1);
	rtLINK_AppendRange (sourceLinkc, firstRow, numberOfRows);
	sourceLinkc->Close (vectorPToken);

	pStoreCPD->retain ();

	DSC_Descriptor elementDsc;
	elementDsc.constructMonotype (pStoreCPD, sourceLinkc);
	sourceLinkc->release ();

	rtLINK_CType* assignmentLinkc = rtLINK_RefConstructor (vectorPToken, -1);
	rtLINK_AppendRange (assignmentLinkc, 0, numberOfRows);
	assignmentLinkc->Close (vectorPToken);

	rtVECTOR_Assign (vector, assignmentLinkc, &elementDsc);

	assignmentLinkc->release ();

	elementDsc.clear ();
    }
    pRPT->release ();

/***** Make the lstore ... *****/
    M_CPD *lstorePToken = pTask->NewCodPToken (1);

    rtLINK_CType *definitionLinkc = rtLINK_RefConstructor (lstorePToken, -1);
    rtLINK_AppendRepeat (
	definitionLinkc, 0, rtPTOKEN_CPD_BaseElementCount (vectorPToken)
    );
    definitionLinkc->Close (vectorPToken);

    M_CPD *lstore = rtLSTORE_NewCluster (definitionLinkc, vector);

    definitionLinkc->release ();
    vector->release ();
    vectorPToken->release ();

/****** Load the new lstore into the accumulator ... *****/
    pTask->loadDucWithReference (lstore, lstorePToken, 0);
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


/*************************
 *****  Path Access  *****
 *************************/

    PD (XReturnSelf,
	"ReturnSelf",
	ReturnSelfCurrentOrSuper)
    PD (XReturnCurrent,
	"ReturnCurrent",
	ReturnSelfCurrentOrSuper)
    PD (XReturnSuper,
	"ReturnSuper",
	ReturnSelfCurrentOrSuper)

/******************************
 *****  Identity Testing  *****
 ******************************/

    PD (XIsIdenticalTo,
	"IsIdenticalTo",
	TestForIdentity)
    PD (XIsntIdenticalTo,
	"IsntIdenticalTo",
	TestForIdentity)

/************************************************
 *****  Long Pointer Encoding and Decoding  *****
 ************************************************/

    PD (XLongValueGetPart,
	"LongValueGetPart",
	LongValueGetPart)
    PD (XLongValueSetPart,
	"LongValueSetPart",
	LongValueSetPart)

/**********************************
 *****  Pointer Construction  *****
 **********************************/

    PD (XZeroPointer,
	"ZeroPointer",
	ZeroPointer)

/********************************
 *****  Pointer Conversion  *****
 ********************************/

    PD (XIntegerAsReferenceTo,
	"IntegerAsReferenceTo",
	IntegerAsReferenceTo)
    PD (XValueAsPointerTo,
	"ValueAsPointerTo",
	ValueAsPointerTo)
    PD (XSelfPointerAsValue,
	"SelfPointerAsValue",
	PointerAsValue)
    PD (XCurrentPointerAsValue,
	"CurrentPointerAsValue",
	PointerAsValue)


/***************************
 *****  Instantiation  *****
 ***************************/

    PD (XNewInstance,
	"NewInstance",
	NewInstance)
    PD (XNewPrototype,
	"NewPrototype",
	NewPrototype)

    PD (XExtendTo,
	"ExtendTo",
	ExtendTo)

/**********************
 *****  Deletion  *****
 **********************/

    PD (XDeleteInstance,
	"DeleteInstance",
	DeleteInstance)

/****************************
 *****  Specialization  *****
 ****************************/

    PD (XSpecialize,
	"Specialize",
	Specialize)
    PD (XSpecializeClass,
	"SpecializeClass",
	Specialize)

    PD (XSpecializeBy,
	"SpecializeBy",
	SpecializeBy)

/*******************************
 *****  Cluster Migration  *****
 *******************************/

    PD (XEstablishResidence,
	"EstablishResidence",
	EstablishResidence)

/***************************
 *****  Cluster Query  *****
 ***************************/

    PD (XInstanceListCount,
	"InstanceListCount",
	InstanceListCount)

    PD (XInstanceList,
	"InstanceList",
	InstanceList)
    PD (XSelectedInstanceList,
	"SelectedInstanceList",
	SelectedInstanceList)


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
PublicFnDef void pfIDENTITY_InitDispatchVector () {
/*****  Install the primitives declared in this module...  *****/
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);

/*****  Make 'ReturnSelfCurrentOrSuper' known to the virtual machine...  *****/
    V_KnownPrimitive_Super = XReturnSuper;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfIDENTITY);
    FAC_FDFCase_FacilityDescription ("Identity Primitives Implementation");
    default:
        break;
    }
}


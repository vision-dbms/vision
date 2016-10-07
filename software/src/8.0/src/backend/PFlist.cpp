/*****  List Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfLIST

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "PFlist.h"

/*****  Supporting  *****/
#include "selector.h"
#include "uvector.h"

#include "vdates.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vprimfns.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTindex.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTvector.h"

#include "RTdoubleuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"

#include "VFragment.h"
#include "VListEnumerationController.h"


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
 *	160 - 209                       *
 *======================================*/


/*****************************
 *****  List Primitives  *****
 *****************************/

#define XNewList			160
#define XAppendObjectToList		161
#define XIndexedListToSequencedList	162
#define XNewListInStore			163
#define XBase0NumericList		164
#define XBase1NumericList		165
#define XIndexedListToSharedList	166

#define XCountListElements		170
#define XBase0CellAtPosition 		171
#define XBase1CellAtPosition 		172

//  See VListEnumerationController for the following primitives...
//#define XIterate			179
//#define XIIC				180
//#define XIICForValue			181
//#define XIICForEnvironment		182
//#define XIICForSubset			183
//#define XIICForGrouping		184
//#define XIICForAscendingSort		185
//#define XIICForDescendingSort		186

//#define XIICForNumericTotal		190
//#define XIICForNumericAverage		191
//#define XIICForNumericMinimum		192
//#define XIICForNumericMaximum		193
//#define XIICForNumericCount		194
//#define XIICForAscendingRank		195
//#define XIICForDescendingRank		196
//#define XIICForNumericProduct		197
//#define XIICForRunningTotal		198


/************************
 ************************
 *****  Primitives  *****
 ************************
 ************************/

/*************************************
 *  List Cluster Creation Primitive  *
 *************************************/

V_DefinePrimitive (NewList) {
    M_CPD *pContentPrototypeCPD = NilOf (M_CPD *);
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	break;

    case 2:
	pContentPrototypeCPD = pTask->getSelf ().storeCPD ();
	break;

    default:
	pTask->raiseException (
	    EC__UsageError,
	    "NewList: Unsupported Control Value: %u",
	    V_TOTSC_PrimitiveFlags
	);
	break;
    }

    pTask->loadDucWithNewLStore (pContentPrototypeCPD);
}


/**************************************
 *  List Instance Creation Primitive  *
 **************************************/

V_DefinePrimitive (NewListInStore) {
/*****  Obtain '^current'...  *****/
    pTask->loadDucWithCurrent ();

/*****  Validate that its store is an L-Store...  *****/
    M_CPD* lstore = pTask->ducStore ();
    RTYPE_MustBeA ("NewListInStore", M_CPD_RType (lstore), RTYPE_C_ListStore);

/*****  Manufacture a new collection of instances...  *****/
    rtLINK_CType* newLists = rtLSTORE_AddLists (lstore, V_TOTSC_PToken);

/*****  Return them...  *****/
    DSC_Descriptor_Pointer (ADescriptor).setTo (newLists);

/*****  And clean up.  *****/
    newLists->release ();
}


/***************************
 *  List Append Primitive  *
 ***************************/

V_DefinePrimitive (AppendObjectToListContinuation) {
    pTask->loadDucWithSelf ();
}


V_DefinePrimitive (AppendObjectToList) {
/*****  Access the lists and their elements...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD* pListCluster = rCurrent.storeCPD ();
    M_CPD* pElementCluster = rtLSTORE_CPD_ContentCPD (pListCluster);

/*****  Create the elements, ...  *****/
    DSC_Descriptor iElementDescriptor;
    if (rCurrent.holdsAScalarReference ()) {
	rtREFUV_Type_Reference	iElementRef;

        rtLSTORE_AppendToRefSelectdList (
	    pListCluster, &DSC_Descriptor_Scalar (rCurrent), &iElementRef
	);

	iElementDescriptor.constructConstant (V_TOTSC_PToken, pElementCluster, iElementRef);
    }
    else if (rCurrent.holdsALink ()) {
	rtLINK_CType *pElementLC;
	rtLSTORE_AppendToLCSelectedList (
	    pListCluster, DSC_Descriptor_Link (rCurrent), &pElementLC
	);
	iElementDescriptor.constructLink (pElementCluster, pElementLC);
    }
    else {
	pElementCluster->release ();
	DSC_Descriptor_Pointer (rCurrent).complainAboutBadPointerType ("AppendObjectToList");
    }

//  If the elements can be updated internally, do it, ...
    if (0 == V_TOTSC_PrimitiveFlags && RTYPE_C_Vector == pElementCluster->RType ()) {
	pTask->loadDucWithNextParameter ();
	pTask->duc ().assignTo (DSC_Descriptor_Pointer (iElementDescriptor), pElementCluster);
	iElementDescriptor.clear ();

	pTask->loadDucWithSelf ();
	return;
    }

/*****  ... load the accumulator with the element descriptor, ...  *****/
    pTask->loadDucWithMoved (iElementDescriptor);

/*****  ... returning immediately in the element creation only case:   *****/
    if (1 == V_TOTSC_PrimitiveFlags)
	return;

/*****  Otherwise, set the continuation, ...  *****/
    pTask->setContinuationTo (AppendObjectToListContinuation);

/*****  ... send an assignment message to the element container, ...  *****/
    pTask->beginMessageCall (KS__LAssign);
    pTask->commitRecipient  ();

/*****  ... with the objects to be appended...  *****/
    pTask->loadDucWithNextParameter ();
    pTask->commitParameter ();

/*****  ... and do the assignment.  *****/
    pTask->commitCall ();
}


/*********************************************************
 *  Indexed List to Sequenced List Conversion Primitive  *
 *********************************************************/

V_DefinePrimitive (IndexedListToSequencedList) {
    pTask->loadDucWithCurrent ();

/*****  Access the l-store...  *****/
    DSC_Descriptor &rResult = pTask->ducMonotype ();
    M_CPD *pLStore = rtINDEX_CPD_ListStoreCPD (rResult.storeCPD ());

/*****  ... and return the requested result:  *****/
    switch (V_TOTSC_Primitive) {
    case XIndexedListToSequencedList: {
	    DSC_Descriptor iCopy;
	    rtLSTORE_Copy (pLStore, pLStore->ScratchPad (), &iCopy);

	    DSC_Descriptor iResult;
	    iResult.constructComposition (rResult.Pointer (), iCopy);

	    rResult.setToMoved (iResult);
	    iCopy.clear ();
	}
	break;
    case XIndexedListToSharedList:
	pLStore->retain ();
	rResult.setStoreTo (pLStore);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("IndexedListToSequenceList");
	break;
    }

    pLStore->release ();
}


V_DefinePrimitive (NumericList) {
    M_CPD		*sizeuv;
    rtINTUV_ElementType	*sizeuvp,
			*sizeuvl;
    size_t		totalSize = 0;
    unsigned int	listNum = 0;
    int			origin,
			i,
			cnt;

#define initializeSizePtrs {\
    if (rCurrent.isScalar ()) {\
	sizeuvp = &DSC_Descriptor_Scalar_Int (rCurrent);\
	sizeuvl = sizeuvp + 1;\
    }\
    else {\
	sizeuv  = DSC_Descriptor_Value (rCurrent);\
	sizeuvp = rtINTUV_CPD_Array (sizeuv);\
	sizeuvl = sizeuvp + UV_CPD_ElementCount (sizeuv);\
    }\
}


/*****  Determine the origin ...  *****/
    switch (V_TOTSC_Primitive) {
    case XBase0NumericList:
	origin = 0;
	break;
    case XBase1NumericList:
	origin = 1;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("NumericList");
	break;
    }

/*****  Access the list sizes ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Calculate the total size ...  *****/
    initializeSizePtrs;
    while (sizeuvp < sizeuvl)
	totalSize += (cnt = *sizeuvp++) >= 0 ? cnt : 0;

/*****  Create the integer uvector to contain the actual list values ... *****/
    M_CPD *seqPToken  = pTask->NewCodPToken (totalSize);
    M_CPD *seqUVector; {
	M_CPD *pRPTRef; int xRPTRef;
	rCurrent.getRPTReference (pRPTRef, xRPTRef);
	seqUVector = rtINTUV_New (seqPToken, pRPTRef, xRPTRef);
	pRPTRef->release ();
    }
    rtINTUV_ElementType	*seqPtr = rtINTUV_CPD_Array (seqUVector);

/*****  Create the linkc which will describe the lstore to create ...  *****/
    M_CPD *pLStorePPT = pTask->NewCodPToken ();
    rtLINK_CType *pPartition = rtLINK_RefConstructor (pLStorePPT, -1);

/*****  Fill both 'pPartition' and 'seqUVector' ... *****/
    initializeSizePtrs;
    while (sizeuvp < sizeuvl) {
	rtLINK_AppendRepeat (pPartition, listNum++, *sizeuvp);

	for (i=origin, cnt=0; cnt < *sizeuvp; i++, cnt++)
	    *seqPtr++ = i;

	sizeuvp++;
    }
    pPartition->Close (seqPToken);

/*****  Create a vector containing the values integer uvector ...  *****/
    M_CPD *pContent = rtVECTOR_NewFromUV (seqPToken, rCurrent.storeCPD (), seqUVector);
    seqPToken->release ();
    seqUVector->release ();

/****** Create and return the new lstore ... *****/
    pTask->loadDucWithListOrStringStore (rtLSTORE_NewCluster (pPartition, pContent));
    pContent->release ();
    pPartition->release ();
    pLStorePPT->release ();

#undef initializeSizePtrs
}


/**********************************
 *  List Element Count Primitive  *
 **********************************/

V_DefinePrimitive (CountListElements) {
/*****  Access the lists and their elements...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *lists = rCurrent.storeCPD ();
    M_CPD *lstore = (RTYPE_Type)M_CPD_RType (lists) == RTYPE_C_ListStore
	? M_DuplicateCPDPtr (lists) : rtINDEX_CPD_ListStoreCPD (lists);

/*****  And return the list element counts...  *****/
    if (rCurrent.holdsAScalarReference ()) {
	pTask->loadDucWithInteger (
	    rtLSTORE_RefSelListElementCount (lstore, &DSC_Descriptor_Scalar (rCurrent))
	);
    }
    else if (rCurrent.holdsALink ()) {
	M_CPD* elementCounts = rtLSTORE_LCSelListElementCount (
	    lstore, DSC_Descriptor_Link (rCurrent)
	);
	pTask->loadDucWithMonotype (elementCounts);
	elementCounts->release ();
    }
    else if (rCurrent.holdsAScalarValue ()) {
/*****
 * A property defined at a 'value' class is by definition empty, so return
 * zero as the count.
 *****/
	pTask->loadDucWithInteger (0);
    }
    else if (rCurrent.holdsNonScalarValues ()) {
	M_CPD* elementCounts = pTask->NewIntUV ();
/*****
 * A property defined at a 'value' class is by definition empty, so return
 * zero as the count. (Integer uvectors are zero-filled at creation)
 *****/
	pTask->loadDucWithMonotype (elementCounts);

	elementCounts->release ();
    }
    else {
	rCurrent.complainAboutBadPointerType ("CountListElements");
    }
/*****  Cleanup  *****/
    lstore->release ();
}


/*****  Primitive to do list positional accesses (subscripting).
 *****/
V_DefinePrimitive (CellAtPosition)
{
    int modifier, converseMessageSelector;

/*****  Determine the origin ...  *****/
    switch (V_TOTSC_Primitive) {
    case XBase0CellAtPosition:
	modifier = 0;
	converseMessageSelector = KS__Base0CellPositionOf;
	break;
    case XBase1CellAtPosition:
	modifier = -1;
	converseMessageSelector = KS__Base1CellPositionOf;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("CellAtPosition");
	break;
    }

/*****  
 *  Acquire the operands ...
 *	rCurrent	- the lists
 *	the DUC		- the integer slots
 *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (converseMessageSelector); 
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();

/*****  Validate that the duc contains integers ...  *****/
    RTYPE_Type rtype = ADescriptor.pointerRType ();
    if (rtype != RTYPE_C_IntUV || pTask->ducStore()->DoesntNameTheIntegerClass ()) {
	pTask->sendBinaryConverseWithCurrent (converseMessageSelector); 
	return;
    }

/*****  Perform the lookup ...  *****/
    DSC_Descriptor resultDsc;
    rtLINK_CType*  locatedLinkc;
    if ((RTYPE_Type)M_CPD_RType (rCurrent.storeCPD ()) == RTYPE_C_ListStore)
	rtLSTORE_Subscript (&rCurrent, &ADescriptor, modifier, &resultDsc, &locatedLinkc);
    else
	rtINDEX_Subscript  (&rCurrent, &ADescriptor, modifier, &resultDsc, &locatedLinkc);

    VDescriptor* pGuardedDatum = pTask->loadDucWithGuardedDatum (locatedLinkc);
    if (pGuardedDatum)
	pGuardedDatum->setToMoved (resultDsc);
    else
	resultDsc.clear ();
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


/*****************************
 *****  List Primitives  *****
 *****************************/

    PD (XNewList,
	"NewList",
	NewList)
    PD (XAppendObjectToList,
	"AppendObjectToList",
	AppendObjectToList)

    PD (XIndexedListToSequencedList,
	"IndexedListToSequencedList",
	IndexedListToSequencedList)
    PD (XIndexedListToSharedList,
	"IndexedListToSharedList",
	IndexedListToSequencedList)

    PD (XNewListInStore,
	"NewListInStore",
	NewListInStore)

    PD (XBase0NumericList, 
	"Base0NumericList", 
	NumericList)
    PD (XBase1NumericList, 
	"Base1NumericList", 
	NumericList)

    PD (XCountListElements,
	"CountListElements",
	CountListElements)

    PD (XBase0CellAtPosition, 
	"Base0CellAtPosition", 
	CellAtPosition)
    PD (XBase1CellAtPosition, 
	"Base1CellAtPosition", 
	CellAtPosition)

    PD (XIterate,
	"Iterate",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIIC,
	"IterateInContext",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForValue,
	"IterateInContextForValue",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForEnvironment,
	"IterateInContextForEnvironment",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForSubset,
	"IterateInContextForSubset",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForGrouping,
	"IterateInContextForGrouping",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForAscendingSort,
	"IterateInContextForAscendingSort",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForDescendingSort,
	"IterateInContextForDescendingSort",
	VPrimitiveDescriptor::IClass_ListEnumerationController)

    PD (XIICForNumericTotal,
	"IterateInContextForNumericTotal",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForNumericAverage,
	"IterateInContextForNumericAverage",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForNumericMinimum,
	"IterateInContextForNumericMinimum",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForNumericMaximum,
	"IterateInContextForNumericMaximum",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForNumericCount,
	"IterateInContextForNumericCount",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForAscendingRank,
	"IterateInContextForAscendingRank",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForDescendingRank,
	"IterateInContextForDescendingRank",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForNumericProduct,
	"IterateInContextForNumericProduct",
	VPrimitiveDescriptor::IClass_ListEnumerationController)
    PD (XIICForRunningTotal,
	"IterateInContextForRunningTotal",
	VPrimitiveDescriptor::IClass_ListEnumerationController)


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
PublicFnDef void pfLIST_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfLIST);
    FAC_FDFCase_FacilityDescription ("List Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFlist.c 1 replace /users/jad/system
  861116 16:06:14 jad new list primitive functions

 ************************************************************************/
/************************************************************************
  PFlist.c 2 replace /users/mjc/system
  861226 17:39:24 mjc Fixed repeated list bug 'groupedBy'

 ************************************************************************/
/************************************************************************
  PFlist.c 3 replace /users/jad/system
  870317 15:15:48 jad implemented product primitive

 ************************************************************************/
/************************************************************************
  PFlist.c 4 replace /users/jck/system
  870325 09:44:28 jck Added 'RunningTotal' primitive (rtDOUBLEUV_PartitndCumulative)

 ************************************************************************/
/************************************************************************
  PFlist.c 5 replace /users/jad/system
  870421 12:36:32 jad inhance the extraction primitive

 ************************************************************************/
/************************************************************************
  PFlist.c 6 replace /users/jck/system
  870511 15:28:31 jck Added special treatment of indexed lists to the 'send:' and 'select:' iterators

 ************************************************************************/
/************************************************************************
  PFlist.c 7 replace /users/jck/system
  870519 16:15:55 jck fixed some lurking bugs

 ************************************************************************/
/************************************************************************
  PFlist.c 8 replace /users/mjc/translation
  870524 09:30:38 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFlist.c 9 replace /users/jck/system
  870605 16:08:00 jck Generalized interface to RTindex

 ************************************************************************/
/************************************************************************
  PFlist.c 10 replace /users/jck/system
  870612 13:31:25 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  PFlist.c 11 replace /users/jad/system
  880108 12:22:46 jad fixed the select from a timeseries containing a single element bug

 ************************************************************************/
/************************************************************************
  PFlist.c 12 replace /users/jck/system
  880226 09:47:54 jck Added Value cases to the count primitive

 ************************************************************************/
/************************************************************************
  PFlist.c 13 replace /users/jck/system
  880229 08:19:11 jck Corrected a typo

 ************************************************************************/
/************************************************************************
  PFlist.c 14 replace /users/jck/system
  880407 09:30:59 jck Changed 'EmptyKeyValueOfType' from macro to function for benifit of VAX C-compiler

 ************************************************************************/
/************************************************************************
  PFlist.c 15 replace /users/jad/system
  880711 16:43:11 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  PFlist.c 16 replace /users/mjc/Workbench/backend
  881031 14:20:39 mjc Corrected bugs in the 'NumericList' primitive

 ************************************************************************/
/************************************************************************
  PFlist.c 17 replace /users/jck/system
  890413 14:25:54 jck Added the 'iterate:by:' primitive

 ************************************************************************/
/************************************************************************
  PFlist.c 18 replace /users/jck/system
  890419 09:40:36 jck Make 'iterate:by:' warn user if it discovers an invalid 2nd paramater

 ************************************************************************/

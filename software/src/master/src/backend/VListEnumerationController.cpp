/*****  VListEnumerationController Implementation  *****/

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

#include "VListEnumerationController.h"

/************************
 *****  Supporting  *****
 ************************/

#include "selector.h"

#include "vstdio.h"

#include "RTintuv.h"
#include "RTdoubleuv.h"

#include "VFragment.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VPrimitiveTaskController<VListEnumerationController>;


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VListEnumerationController);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VListEnumerationController::VListEnumerationController (
    ConstructionData const &rTCData, VPrimitiveDescriptor *pDescriptor, unsigned short iFlags
) : VPrimitiveTaskController<VListEnumerationController> (
    rTCData, pDescriptor, iFlags, &VListEnumerationController::startup
) , m_pTemporalContext	(NilOf (rtINDEX_Key*))
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VListEnumerationController::~VListEnumerationController () {
    if (m_pTemporalContext) {
	m_pTemporalContext->release ();
    }
}


/***************************
 ***************************
 *****  Continuations  *****
 ***************************
 ***************************/

/**********************************
 *****  Startup Continuation  *****
 **********************************/

void VListEnumerationController::startup () {
/*****
 *  Decode the alias by which this primitive was invoked...
 *****/
    unsigned int xBlockInvocationMessage = KS__BasicSend;
    unsigned int iterationFactor = 0;
    switch (primitive ()) {
    case XIterate:
	if (loadDucWithNextParameterAsMonotype ()) {
	    DSC_Descriptor& rDucMonotype = ducMonotype ();

	    RTYPE_Type rtype = rDucMonotype.pointerRType ();
	    if (RTYPE_C_IntUV == rtype && rDucMonotype.isScalar ())
		iterationFactor = DSC_Descriptor_Scalar_Int (rDucMonotype);
	    else if (RTYPE_C_IntUV == rtype) {
		if (rDucMonotype.isACoercedScalar ()) iterationFactor = rtINTUV_CPD_Array (
		    DSC_Descriptor_Value (rDucMonotype)
		)[0];
	    }
	    
	}
	if (iterationFactor < 1) {
	    iterationFactor = 1;
	    IO_printf ("\n>>>  Invalid Iteration Factor.  Default Value(1) used.  <<<\n");
	}
	
	/*****  No Break  *****/
    case XIIC:
	setContinuationTo (&VListEnumerationController::returnSelf);
	break;

    case XIICForValue:
	setContinuationTo (&VListEnumerationController::returnValue);
	break;

    case XIICForEnvironment:
	setContinuationTo (&VListEnumerationController::returnValue);
	xBlockInvocationMessage = KS__BasicExtend;
	break;

    case XIICForSubset:
	setContinuationTo (&VListEnumerationController::returnSubset);
	break;

    case XIICForNumericTotal:
    case XIICForNumericAverage:
    case XIICForNumericMinimum:
    case XIICForNumericMaximum:
    case XIICForNumericCount:
    case XIICForNumericProduct:
	setContinuationTo (&VListEnumerationController::returnSummary);
	break;

    case XIICForGrouping:
    /*****  Access and save collector...  *****/
	loadDucWithNextParameter ();
	m_iCollector.setToMoved (duc ());
    /*****  Set up continuation...  *****/
	setContinuationTo (&VListEnumerationController::returnGroup);
	break;

    case XIICForAscendingSort:
    case XIICForDescendingSort:
	setContinuationTo (&VListEnumerationController::returnSort);
	xBlockInvocationMessage = KS__BasicSendCanonicalized4Sort;
	break;

    case XIICForAscendingRank:
    case XIICForDescendingRank:
    case XIICForRunningTotal:
    /*****  Access and save collector...  *****/
	loadDucWithNextParameter ();
	m_iCollector.setToMoved (duc ());
    /*****  Set up continuation...  *****/
	setContinuationTo (&VListEnumerationController::returnOrder);
	xBlockInvocationMessage = KS__BasicSendCanonicalized4Sort;
	break;

    default:
        raiseUnimplementedAliasException ("IterateInContext");
        break;
    }

/*****  Obtain a CPD for the store...  *****/
    DSC_Descriptor& rCurrent = getCurrent ();

/*****
 *  Obtain the links and u-vectors specifying the elements to be operated
 *  upon...
 *****/
    Vdd::Store::Reference pElementStore;
    rtLINK_CType *pElementSelector, *pExpansionLink; M_CPD *pExpansionReordering;
    bool bDone = rCurrent.store()->getListElements (
	rCurrent.Pointer (), pElementStore, pElementSelector, pExpansionLink, pExpansionReordering, m_pTemporalContext
    );
    if (bDone) {
	m_pExpansionLink.claim (pExpansionLink);
	m_pExpansionReordering.claim (pExpansionReordering);
    }
    else {
	clearContinuation ();
	rCurrent.complainAboutBadPointerType ("Iterate In Context");
    }

    if (RTYPE_C_Vector == pElementStore->rtype ())
	static_cast<rtVECTOR_Handle*>(pElementStore.referent ())->getElements (m_iSelectedElements, pElementSelector);
    else {
	m_iSelectedElements.setToMonotype (pElementStore, pElementSelector);
    }
    pElementSelector->release ();

/*****  Send the block invocation message...  *****/
    beginMessageCall (xBlockInvocationMessage, m_pExpansionLink);
    loadDucWithSelectedElements (iterationFactor);
    commitRecipient ();

/*****  Access and push the iterator block parameter...  *****/
    loadDucWithNextParameter ();
    restrictDuc (m_pExpansionLink);
    commitParameter ();

/*****  And run the tasks created...  *****/
    commitCall (m_pTemporalContext);
}


/*************************************
 *****  returnSelf Continuation  *****
 *************************************/

void VListEnumerationController::returnSelf () {
    loadDucWithSelf ();
}

/**************************************
 *****  returnValue Continuation  *****
 **************************************/

void VListEnumerationController::returnValue() {
    loadDucWithInitializedLStoreOrIndex (m_pExpansionLink);
}


/***************************************
 *****  returnSubset Continuation  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Continuation to collect the list elements for which the iterated
 *****  block returned true.
 *---------------------------------------------------------------------------
 */
void VListEnumerationController::returnSubset () {
/*-----------------------------------------------------------------------*
 *+++++++++++++++++++++++++++  Polytype Result  ++++++++++++++++++++++++++
 *-----------------------------------------------------------------------*/
    if (ducIsAPolytype ()) {
/*****
 *  Obtain a link identifying the subset of 'true's in the result...
 *****/
	rtLINK_CType *pSubset = duc ().subsetOfType (codScratchPad (), &M_KOT::TheTrueClass);

/*****  If 'true's were found, ...  *****/
	if (pSubset) {
    /*****  ... then copy the 'select:'ed subset of each list:  *****/
	/*--------------------------------------------------------------*
	 *  Create the definition link needed to manufacture the new
	 *  L-Store...
	 *--------------------------------------------------------------*/
	    rtLINK_CType *pDefinitionLink = m_pExpansionLink->Extract (pSubset);

	/*--------------------------------------------------------------*
	 *  Load the accumulator with the subset of elements that passed
	 *  the 'select:'...
	 *--------------------------------------------------------------*/
	    loadDucWithSelectedElements ();
	    restrictDuc (pSubset);

	/*--------------------------------------------------------------*
	 *  Load the accumulator with a L-Store containing those
	 *  elements...
	 *--------------------------------------------------------------*/
	    if (m_pTemporalContext) {
		rtINDEX_Key* tempKey = m_pTemporalContext->NewFutureKey (
		    pSubset, NilOf (M_CPD*)
		);

		m_pTemporalContext->release ();
		m_pTemporalContext = tempKey;
	    }
	    loadDucWithInitializedLStoreOrIndex (pDefinitionLink);

	/*--------------------------------------------------------------*
	 *  And cleanup...
	 *--------------------------------------------------------------*/
	    pDefinitionLink->release ();
	    pSubset->release ();
	}

    /*****  ... otherwise, return empty lists:  *****/
	else if (m_pTemporalContext)
	    loadDucWithNewIndex  ();
	else
	    loadDucWithNewLStore ();
    }
    /*****  End of Polytype Case  *****/


/*----------------------------------------------------------------------*
*+++++++++++++++++++++++++++  Monotype Result  ++++++++++++++++++++++++++
*-----------------------------------------------------------------------*/

    else {
/*****  If all results are 'true', return copies of the original lists ...  *****/
	if (ducStore ()->NamesTheTrueClass ()) {
	    loadDucWithCopied (m_iSelectedElements);
	    if (m_pExpansionReordering.isntNil ())
		distributeDuc (m_pExpansionReordering);
	    loadDucWithInitializedLStoreOrIndex (m_pExpansionLink);
	}

/*****  ...otherwise, return empty lists.  *****/
	else if (m_pTemporalContext)
	    loadDucWithNewIndex  ();
	else
	    loadDucWithNewLStore ();
    }
}


/****************************************
 *****  returnSummary Continuation  *****
 ****************************************/

void VListEnumerationController::returnSummary () {
/*****  Determine the numeric statistic to be returned...  *****/
    rtVECTOR_PS_Type	statisticType;
    switch (primitive ()) {
    case XIICForNumericTotal:
	statisticType = rtVECTOR_PS_Sum;
	break;
    case XIICForNumericAverage:
	statisticType = rtVECTOR_PS_Avg;
	break;
    case XIICForNumericMinimum:
	statisticType = rtVECTOR_PS_Min;
	break;
    case XIICForNumericMaximum:
	statisticType = rtVECTOR_PS_Max;
	break;
    case XIICForNumericCount:
	statisticType = rtVECTOR_PS_NumericCount;
	break;
    case XIICForNumericProduct:
	statisticType = rtVECTOR_PS_Product;
	break;
    default:
	raiseUnimplementedAliasException (
	    "IterateInContextNumericStatisticContinuation"
	);
	break;
    }

/*****  Compute the statistic's values...  *****/
    M_CPD *statisticValues = convertDucToVectorC ()->PartitionedStatistics (
	m_pExpansionLink, statisticType, codKOT ()
    );

/*****  Return them...  *****/
    loadDucWithMonotype (statisticValues);

/*****  ... and clean up.  *****/
    statisticValues->release ();
}


/**************************************
 *****  returnGroup Continuation  *****
 **************************************/

void VListEnumerationController::returnGroup () {
/*****  Obtain the grouping partitions and group indices...  *****/
    rtVECTOR_CType::Reference pVectorC (convertDucToVectorC ());
    M_CPD *pSortIndices = pVectorC->PartitionedSortIndices (m_pExpansionLink, false);
    pVectorC.setTo (pVectorC->reorder (pSortIndices));

    rtLINK_CType *majorPartition;
    rtLINK_CType *minorPartition;
    M_CPD *elementDistribution;
    pVectorC->PartitionedPartition (
	m_pExpansionLink, &majorPartition, &minorPartition, &elementDistribution
    );
    loadDucWithVector (pVectorC);
    distributeDuc (elementDistribution);

    rtVECTOR_Handle::Reference groupIndices (new rtVECTOR_Handle (minorPartition->RPT ()));
    duc ().assignTo (minorPartition, groupIndices);

/*****  Set up the post collector continuation...  *****/
    setContinuationTo (&VListEnumerationController::returnValue);

/*****  For now, ensure that the returned value is always an L-Store  *****/
    if (m_pTemporalContext) {
	m_pTemporalContext->release ();
	m_pTemporalContext = NilOf (rtINDEX_Key *);
    }

/*****
 *  Adjust the iterator state to reflect the partitioning of the list
 *  elements...
 *****/
    m_pExpansionLink.claim (majorPartition);

/*****  Send the 'basicSend:with:' message used to invoke the collector...  *****/
    beginMessageCall (KS__BasicSendWith, m_pExpansionLink);
    loadDucWithVector (groupIndices);
    commitRecipient ();

/*****  Access and push the collector block...  *****/
    loadDucWithMoved (m_iCollector);
    restrictDuc (majorPartition);
    commitParameter ();

/*****  Create and push the group lists, ...  *****/
    loadDucWithSelectedElements ();
    reorderDuc (pSortIndices);
    pSortIndices->release ();

    distributeDuc (elementDistribution);
    elementDistribution->release ();

    loadDucWithInitializedLStore (minorPartition);
    minorPartition->release ();

    commitParameter ();

/*****  Schedule and run the tasks created...  *****/
    commitCall ();
} 


/*************************************
 *****  returnSort Continuation  *****
 *************************************/

void VListEnumerationController::returnSort () {
/*****  Determine the sense of the sort...  *****/
    bool descending;
    switch (primitive ()) {
    case XIICForAscendingSort:
	descending = false;
	break;
    case XIICForDescendingSort:
	descending = true;
	break;
    default:
	raiseUnimplementedAliasException (
	    "IterateInContextSortContinuation"
	);
	break;
    }

/*****  Obtain sort indices for those iteration results...  *****/
    M_CPD *sortIndices = convertDucToVectorC ()->PartitionedSortIndices (
	m_pExpansionLink, descending
    );

/*****  Sort the original list elements...  *****/
    loadDucWithSelectedElements ();

    reorderDuc (sortIndices);
    sortIndices->release ();

/*****  Return the sorted lists...  *****/
    loadDucWithInitializedLStore (m_pExpansionLink);
}


/**************************************
 *****  returnOrder Continuation  *****
 **************************************/

void VListEnumerationController::returnOrder () {
/*****  Determine the direction of the rank...  *****/
    bool descending, rank;
    switch (primitive ()) {
    case XIICForAscendingRank:
	rank = true;
	descending = false;
	break;
    case XIICForDescendingRank:
	rank = true;
	descending = true;
	break;
    case XIICForRunningTotal:
	rank = false;
	break;
    default:
	raiseUnimplementedAliasException (
	    "IterateInContextNumericCollectionContinuation"
	);
	break;
    }

/*****  Obtain the duc's double's:  *****/
    VDescriptor iValues;
    rtLINK_CType *pSubset = duc ().subsetOfType (
	ptoken ()->ScratchPad (), &M_KOT::TheDoubleClass, &iValues
    );
    
/*****  Set up the post collector continuation...  *****/
    setContinuationTo (&VListEnumerationController::returnValue);

/*****  Send the 'basicSend:with:' message used to invoke the collector...  *****/
    beginMessageCall (KS__BasicSendWith, m_pExpansionLink);
    loadDucWithSelectedElements ();
    commitRecipient ();

/*****  Access and push the collector...  *****/
    loadDucWithMoved (m_iCollector);
    restrictDuc (m_pExpansionLink);
    commitParameter ();

/*****  Access and push the collector's values...  *****/
    if (IsNil (pSubset))
	m_pDuc->setToNA (m_pExpansionLink->PPT (), codKOT ());
    else {
	VLinkCReference pPartition;
	pPartition.claim (m_pExpansionLink->Extract (pSubset));

	M_CPD *pValues;
	if (iValues.convertVectorsToMonotypeIfPossible ())
	    pValues = iValues.contentAsMonotype ().asUVector ();
	else raiseUnimplementedOperationException (
	    "VListEnumerationController::returnOrder: %s Unsupported",
	    iValues.typeName ()
	);

	VCPDReference pResult;
	if (rank) {
	    VCPDReference pSortIndices;
	    pSortIndices.claim (
		rtDOUBLEUV_PartitndSortIndices (pPartition, pValues, descending)
	    );
	    pValues->release ();

	    pResult.claim (
		rtINTUV_PartitndRanks (
		    pPartition, pSortIndices, codKOT ()->TheIntegerPTokenHandle ()
		)
	    );
	}
	else {
	    pResult.claim (
		rtDOUBLEUV_PartitndCumulative (
		    pPartition, pValues, codKOT ()->TheDoublePTokenHandle ()
		)
	    );
	    pValues->release ();
	}

	loadDucWithFragmentation (pSubset->RPT ())
	    .createFragment (pSubset)->datum()
	    .setToMonotype (pResult);
    }
    commitParameter ();

/*****  Schedule and run the tasks created...  *****/
    commitCall (m_pTemporalContext);
}


/***********************
 *****  Utilities  *****
 ***********************/

/*---------------------------------------------------------------------------
 *****  Utility to load the accumulator with a descriptor for an initialized
 *****  L-Store or Index.
 *
 *  Arguments:
 *	pDefinitionLink		- a link constructor to be used to specify
 *				  the breakpoint array of the L-Store being
 *				  created.
 *	fLStoreDesired		- A flag to state that the option to return an
 *				  index should not be considered.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the accumulator only.
 *
 *  Notes:
 *	This routine will obtain its initial L-Store values from the current
 *	content of the accumulator.
 *
 *****/
void VListEnumerationController::loadDucWithInitializedLists (
    rtLINK_CType *pDefinitionLink, bool fLStoreDesired
) {
/*****  Manufacture a new content vector for the L-Store...  *****/
    rtPTOKEN_Handle *pPToken = pDefinitionLink->PPT ();
    rtVECTOR_Handle::Reference pVector (new rtVECTOR_Handle (pPToken));

    DSC_Pointer assignmentPointer;
    assignmentPointer.constructIdentity (pPToken);

    duc().assignTo (assignmentPointer, pVector);

    DSC_Pointer_Identity (assignmentPointer).clear ();

/*****  Manufacture a new indexed or sequenced L-Store...  *****/
    if (fLStoreDesired || IsNil (m_pTemporalContext)) loadDucWithListOrStringStore (
	new rtLSTORE_Handle (pDefinitionLink, pVector, true), pDefinitionLink->RPT ()
    );
    else loadDucWithListOrStringStore (
	rtINDEX_NewCluster (pDefinitionLink, pVector, m_pTemporalContext), pDefinitionLink->RPT ()
    );
}


/*---------------------------------------------------------------------------
 *****  Utility to load the accumulator with a descriptor for a new Index.
 *
 *  Arguments:
 *	pContentPrototype	- the address of a CPD for the new cluster's
 *				  content prototype (Nil for Vector).
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void VListEnumerationController::loadDucWithNewIndex (Vdd::Store *pContentPrototype) {
    m_pTemporalContext->RealizeSet ();

    rtPTOKEN_Handle::Reference pInstancePToken (NewCodPToken ());
    loadDucWithListOrStringStore (
	rtINDEX_NewCluster (
	    pInstancePToken, &rtINDEX_Key_Set (m_pTemporalContext), pContentPrototype
	)
    );
}


void VListEnumerationController::loadDucWithSelectedElements (unsigned int iterationFactor) {
    loadDucWithCopied (m_iSelectedElements);
    if (iterationFactor > 0) {
	if (m_pDuc->isStandard ())
	    m_pDuc->convertMonotypeToFragmentation (iterationFactor);
	else {
	    VFragmentation& rFragmentation = convertDucToFragmentation ();
	    rFragmentation.goToFirstFragment ();
	    while (IsntNil (rFragmentation.currentFragment ())) {
		rFragmentation.currentFragment ()->datum().convertMonotypeToFragmentation (
		    iterationFactor
		);
		rFragmentation.goToNextFragment ();
	    }
	}
    }
    if (m_pExpansionReordering.isntNil ()) {
        distributeDuc (m_pExpansionReordering);
    }
}

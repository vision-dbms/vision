/*****  VFragmentation Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include "VkRadixListSorter.h"

/******************
 *****  Self  *****
 ******************/

#include "VFragmentation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "vstdio.h"
#include "vutil.h"

#include "VFragment.h"


/************************************************
 ************************************************
 *****  Counters, Parameters, and Switches  *****
 ************************************************
 ************************************************/

/**********************
 *****  Counters  *****
 **********************/

unsigned int VFragmentation::CoalesceAttemptCount	= 0;
unsigned int VFragmentation::CoalesceSuccessCount	= 0;
unsigned int VFragmentation::CoalesceNonDJUCount	= 0;
unsigned int VFragmentation::CoalesceExamineCount	= 0;
unsigned int VFragmentation::CoalesceCandidateCount	= 0;
unsigned int VFragmentation::CoalesceReductionCount	= 0;
unsigned int VFragmentation::FlattenFragCount		= 0;
unsigned int VFragmentation::MakeFragFromDscCount	= 0;
unsigned int VFragmentation::MakeFragFromVCCount	= 0;
unsigned int VFragmentation::MakeVectorFromFragCount	= 0;

/****************************
 *****  Trace Switches  *****
 ****************************/

bool VFragmentation::TracingFragmentationOps = false;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

void VFragmentation::construct (VFragmentation const &rSource) {
    construct (rSource.m_pPPT);

    for (
	VFragment *pFragment = rSource.m_pFragmentList;
	pFragment;
	pFragment = pFragment->next ()
    ) {
	rtLINK_CType *pSubset = pFragment->subset ();
	pSubset->retain ();
	createFragment (pSubset)->datum().setToCopied (pFragment->datum());
    }
}


void VFragmentation::construct (rtVECTOR_CType *vectorc) {
/*---------------------------------------------------------------------------*
 *	vectorc			- the vector constructor from which the
 *				  fragmentation is to be created.
 *---------------------------------------------------------------------------*/
    MakeFragFromVCCount++;

    vectorc->MakeConstructorLinks ();
    construct (vectorc->PPT ());

    rtVECTOR_USDC * const *pUSDCArray = vectorc->USDCArray ();
    rtVECTOR_USDC * const * const pUSDCLimit = vectorc->USDCLimit ();
    while (pUSDCArray < pUSDCLimit) {
	rtVECTOR_USDC *pUSDC = *pUSDCArray++;
	rtLINK_CType *linkc = pUSDC->ClaimedAssociatedLink ();
	if (linkc) {
	    createFragment (linkc)->datum ().setToMonotype (
		pUSDC->store (), pUSDC->pointerCPD ()
	    );
	}
    }
}


void VFragmentation::construct (DSC_Descriptor &rSource, unsigned int iFragmentSize) {
    MakeFragFromDscCount++; {
	rtPTOKEN_Handle::Reference pFragmentationPToken (rSource.PPT ());
	construct (pFragmentationPToken);
    }

    m_fLock = true;  // disable automatic coalescing 

    unsigned int dscSize = m_pPPT->cardinality ();
    unsigned int amtLeft = dscSize;

    for (unsigned int origin = 0; origin < dscSize; origin += iFragmentSize) {
	unsigned int sThisFragment = V_Min (iFragmentSize, amtLeft);
        rtLINK_CType *pFragmentSubset = rtLINK_AppendRange (
	    rtLINK_RefConstructor (m_pPPT), origin, sThisFragment
	)->Close (NewPToken (sThisFragment));

	createFragment (pFragmentSubset)->datum ().setToSubset (pFragmentSubset, rSource);
	amtLeft -= sThisFragment;
    }

    goToFirstFragment ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/****************
 *  'assignTo'  *
 ****************/

void VFragmentation::assignTo (rtLINK_CType *pElementSelector, Vdd::Store *pTarget) {
    Flatten ();
    goToFirstFragment ();
    while (m_pCurrentFragment) {
	rtLINK_CType *pNewLC = pElementSelector->Extract (m_pCurrentFragment->subset ());
	m_pCurrentFragment->datum ().assignTo (pNewLC, pTarget);
	pNewLC->release ();

	goToNextFragment ();
    }
}


/*****************
 *  'getVector'  *
 *****************/

void VFragmentation::getVector (rtVECTOR_Handle::Reference &rpResult) {
    MakeVectorFromFragCount++;

    rpResult.setTo (new rtVECTOR_Handle (m_pPPT));

    goToFirstFragment ();

    while (m_pCurrentFragment) {
	m_pCurrentFragment->datum ().assignTo (m_pCurrentFragment->subset (), rpResult);
	goToNextFragment ();
    }
}


/************************
 *  'goToNextFragment'  *
 ************************/

void VFragmentation::goToNextFragment () {
    m_pCurrentFragment = m_pCurrentFragment->next ();
    m_xCurrentFragment++;
}


/*********************
 *  'subsetInStore'  *
 *********************/

rtLINK_CType *VFragmentation::subsetInStore (Vdd::Store *pStore, VDescriptor *pValueReturn) {
    Flatten ();

    goToFirstFragment ();

    VDescriptor  iDatumContribution;
    VDescriptor *pDatumContribution = pValueReturn ? &iDatumContribution : 0;

    bool bSubsetSharedWithSource = true;
    rtLINK_CType *pSubset = NilOf (rtLINK_CType*);

    while (m_pCurrentFragment) {
	VDescriptor& rFragmentDatum = m_pCurrentFragment->datum ();

	rtLINK_CType *pSubsetContribution = NilOf (rtLINK_CType *);
	if (rFragmentDatum.isStandard ()) {
	/*****  Obtain the 'store' of the result...   *****/
	    if (rFragmentDatum.contentAsMonotype ().store ()->Names (pStore)) {
		pSubsetContribution = m_pCurrentFragment->subset ();
		pSubsetContribution->retain ();

		if (pDatumContribution)
		    pDatumContribution->setToCopied (rFragmentDatum);
	    }
	}
	else {
	    rtLINK_CType *pFragmentSubset = rFragmentDatum.subsetInStore (
		pStore, pDatumContribution
	    );

	    /*****  Convert the fragment's subset to its contribution, ... *****/
	    if (pFragmentSubset) {
		pSubsetContribution = m_pCurrentFragment->subset ()->Extract (
		    pFragmentSubset
		);
		pFragmentSubset->release ();
	    }
	}

/***** If the fragment is contributing, add it to the current result ... *****/
	if (pSubsetContribution) {

/*****  ... by sharing the first contribution, ...  *****/
	    if (IsNil (pSubset)) {
		pSubsetContribution->retain ();
		pSubset = pSubsetContribution;
		if (pValueReturn)
		    pValueReturn->setToMoved (iDatumContribution);
	    }

/*****  ... and by adding subsequent contributions:  *****/
	    else {
	    //  The second contribution must decouple the first, ...
		if (bSubsetSharedWithSource) {
		    rtPTOKEN_Handle::Reference pNewPPT (new rtPTOKEN_Handle (pSubset->PPT ()->Space (), 0));
		    rtLINK_CType *pNewSubset = rtLINK_PosConstructor (pNewPPT)->Close (
			pSubset->RPT ()
		    );

		    if (!pValueReturn) 
			pNewSubset->Add (pSubset, false);
		    else {
			VDescriptor iValueReturn;
			iValueReturn.setToMoved (*pValueReturn);
			pValueReturn->setToFragmentation (pNewPPT).createFragment (
			    pNewSubset->Add (pSubset, true)
			)->datum ().setToMoved (iValueReturn);
		    }

		    pSubset->release ();
		    pSubset = pNewSubset;

		    bSubsetSharedWithSource = false;
		}

		if (!pValueReturn)
		    pSubset->Add (pSubsetContribution, false);
		else {
		    VFragmentation &rResult = pValueReturn->contentAsFragmentation ();
		    rResult.createFragment (
			pSubset->Add (pSubsetContribution, true)
		    )->datum ().setToMoved (iDatumContribution);

		//  patch the value return fragmentation p-token to accomodate the Add...
		    rResult.setPPT (pSubset->PPT ());
		}
	    }

	    pSubsetContribution->release ();
	}
	goToNextFragment ();
    }

    return pSubset;
}


/********************
 *  'subsetOfType'  *
 ********************/

rtLINK_CType *VFragmentation::subsetOfType (
    M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
) {
    if (pValueReturn) {
//  The following would be needed if we did not force the coalesce of 'this' at the top
//  of the function
//
//	if (pValueReturn->IsAFragmentation ()) {
//	    M_CPD *pVectorCPD = pValueReturn->ContentAsFragmentation().asVector ();
//	    pValueReturn->SetToVector (pVectorCPD);
//	    pValueReturn->ConvertVectorsToMonotypeIfPossible ();
//	}
	Flatten ();	 // Flatten first to ensure unlock will 'stick'
	m_fLock = false; // Force a full coalesce so result will be monotype
	Coalesce ();
    }
    else
	Flatten ();

    goToFirstFragment ();

    VDescriptor  iDatumContribution;
    VDescriptor *pDatumContribution = pValueReturn ? &iDatumContribution : 0;

    bool bSubsetSharedWithSource = true;
    rtLINK_CType *pSubset = NilOf (rtLINK_CType*);

    while (m_pCurrentFragment) {
	VDescriptor& rFragmentDatum = m_pCurrentFragment->datum ();

	rtLINK_CType *pSubsetContribution = NilOf (rtLINK_CType *); {
	    rtLINK_CType *pFragmentSubset = rFragmentDatum.subsetOfType (
		pSubsetSpace, pKOTM, pDatumContribution
	    );

	    /*****  Convert the fragment's subset to its contribution, ... *****/
	    if (pFragmentSubset) {
		pSubsetContribution = m_pCurrentFragment->subset ()->Extract (
		    pFragmentSubset
		);
		pFragmentSubset->release ();
	    }
	}

/***** If the fragment is contributing, add it to the current result ... *****/
	if (pSubsetContribution) {

/*****  ... by sharing the first contribution, ...  *****/
	    if (IsNil (pSubset)) {
		pSubsetContribution->retain ();
		pSubset = pSubsetContribution;
		if (pValueReturn)
		    pValueReturn->setToMoved (iDatumContribution);
	    }

/*****  ... and by adding subsequent contributions:  *****/
	    else {
	    //  The second contribution must decouple the first, ...
		if (bSubsetSharedWithSource) {
		    rtPTOKEN_Handle::Reference pNewPPT (new rtPTOKEN_Handle (pSubsetSpace, 0));
		    rtLINK_CType *pNewSubset = rtLINK_PosConstructor (pNewPPT)->Close (
			pSubset->RPT ()
		    );

		    if (!pValueReturn) 
			pNewSubset->Add (pSubset, false);
		    else {
			VDescriptor iValueReturn;
			iValueReturn.setToMoved (*pValueReturn);
			pValueReturn->setToFragmentation (pNewPPT).createFragment (
			    pNewSubset->Add (pSubset, true)
			)->datum ().setToMoved (iValueReturn);
		    }

		    pSubset->release ();
		    pSubset = pNewSubset;

		    bSubsetSharedWithSource = false;
		}

		if (!pValueReturn)
		    pSubset->Add (pSubsetContribution, false);
		else {
		    VFragmentation &rResult = pValueReturn->contentAsFragmentation ();
		    rResult.createFragment (
			pSubset->Add (pSubsetContribution, true)
		    )->datum ().setToMoved (iDatumContribution);

		//  patch the value return fragmentation p-token to accomodate the Add...
		    rResult.setPPT (pSubset->PPT ());
		}
	    }

	    pSubsetContribution->release ();
	}
	goToNextFragment ();
    }
    return pSubset;
}


/***************************
 ***************************
 *****  Normalization  *****
 ***************************
 ***************************/

/****************
 *  'Coalesce'  *
 ****************/

void VFragmentation::Coalesce () {
    Flatten ();
    if (m_iFragmentCount < 2 || m_fLock)
	return;

    CoalesceAttemptCount++;
    CoalesceExamineCount += m_iFragmentCount;

//  Initialize descriptors for the coalesce candidates...
    unsigned int iCandidateCount = 0;
    unsigned int iDescriptorCount = 0;
    VFragment::Descriptor *pDescriptorArray = 0;
    VFragment *pFirstCandidate;

    goToFirstFragment ();
    while (m_pCurrentFragment) {
	if (m_pCurrentFragment->IsACoalesceCandidate ()) {
	    switch (iCandidateCount) {
	    case 0:
		pFirstCandidate = m_pCurrentFragment;
		iCandidateCount = 1;
		break;

	    case 1:
		if (m_iFragmentCount > m_xCurrentFragment) {
		    iDescriptorCount = m_iFragmentCount - m_xCurrentFragment + 1;
		    pDescriptorArray = new VFragment::Descriptor[iDescriptorCount];
		}
		else raiseIncorrectFragmentCountException ();

		pFirstCandidate->initializeDescriptor (pDescriptorArray[0]);
		m_pCurrentFragment->initializeDescriptor (pDescriptorArray[iCandidateCount++]);
		break;

	    default:
		if (iDescriptorCount > iCandidateCount)
		    m_pCurrentFragment->initializeDescriptor (pDescriptorArray[iCandidateCount++]);
		else raiseIncorrectFragmentCountException ();
		break;
	    }
	}
	goToNextFragment ();
    }

//  ... return immediately if there aren't at least two candidates, ...
    if (iCandidateCount < 2)
	return;

//  ... otherwise, sort the candidates, ...
    CoalesceCandidateCount += iCandidateCount;
    qsort (
	(void *)pDescriptorArray, 
	iCandidateCount,
	sizeof (VFragment::Descriptor),
	(VkComparator)VFragment::CompareDescriptors
    );

//  ... and look for mergeable fragments:
    bool mergeDone = false;
    bool nonDJU = false;
    unsigned int xMergeOrigin = 0;
    while (xMergeOrigin + 1 < iCandidateCount) {
	VFragment::Descriptor& rBaseDescriptor = pDescriptorArray[xMergeOrigin];

	unsigned int iFastMergeArraySize = 0;
	bool fFastMergePossible = rBaseDescriptor.getFastMergeData (iFastMergeArraySize);

	unsigned int xMergeLimit = xMergeOrigin + 1;
	while (xMergeLimit < iCandidateCount && rBaseDescriptor.canMergeWith (
		pDescriptorArray[xMergeLimit]
	    )
	) if (fFastMergePossible)
	    fFastMergePossible = pDescriptorArray[xMergeLimit++].getFastMergeData (
		iFastMergeArraySize
	    );
	else xMergeLimit++;

	if (xMergeLimit - xMergeOrigin > 1) {
	    CoalesceReductionCount += xMergeLimit - xMergeOrigin - 1;
	    mergeDone = true;

	    rtLINK_CType *pNewSubset;
	    unsigned int xDescriptor;

	    if (fFastMergePossible) {
		VFragment::Descriptor** pFastMergeDescriptors = (VFragment::Descriptor**)UTIL_Malloc (
		    iFastMergeArraySize * sizeof (VFragment::Descriptor*)
		);
		unsigned int* pFastMergeRangeOrigins = (unsigned int*)UTIL_Malloc (
		    iFastMergeArraySize * sizeof (unsigned int)
		);
		unsigned int* pFastMergeRangeSizes = (unsigned int*)UTIL_Malloc (
		    iFastMergeArraySize * sizeof (unsigned int)
		);
		unsigned int *pFastMergeSortIndices = (unsigned int *)UTIL_Malloc (
		    iFastMergeArraySize * sizeof (unsigned int)
		);

		{
		    VFragment::Descriptor**	ppFastMergeDescriptor	= pFastMergeDescriptors;
		    unsigned int*		pFastMergeRangeOrigin	= pFastMergeRangeOrigins;
		    unsigned int*		pFastMergeRangeSize	= pFastMergeRangeSizes;

		    for (xDescriptor = xMergeOrigin; xDescriptor < xMergeLimit; xDescriptor++)
		    {
			pDescriptorArray[xDescriptor].fastMergeBegin (
			    ppFastMergeDescriptor, pFastMergeRangeOrigin, pFastMergeRangeSize
			);
		    }
		}

		UTIL_RadixListSort (
		    pFastMergeRangeOrigins, pFastMergeSortIndices, iFastMergeArraySize
		);

		pNewSubset = rtLINK_RefConstructor (m_pPPT);

		unsigned int xNewSubsetOrigin = 0;
		for (unsigned int xSortIndex = 0; xSortIndex < iFastMergeArraySize; xSortIndex++) {
		    unsigned int xRange		= pFastMergeSortIndices[xSortIndex];
		    unsigned int iRangeSize	= pFastMergeRangeSizes[xRange];

		    rtLINK_AppendRange (
			pNewSubset, pFastMergeRangeOrigins[xRange], iRangeSize
		    );
		    pFastMergeDescriptors[xRange]->fastMergeAppend (xNewSubsetOrigin, iRangeSize);

		    xNewSubsetOrigin += iRangeSize;
		}

		rtPTOKEN_Handle::Reference pNewSubsetPToken (NewPToken (xNewSubsetOrigin));
		pNewSubset->Close (pNewSubsetPToken);
		for (xDescriptor = xMergeOrigin; xDescriptor < xMergeLimit; xDescriptor++) {
		    pDescriptorArray[xDescriptor].fastMergeEnd (pNewSubsetPToken);
		}

		UTIL_Free (pFastMergeSortIndices);
		UTIL_Free (pFastMergeRangeSizes);
		UTIL_Free (pFastMergeRangeOrigins);
		UTIL_Free (pFastMergeDescriptors);
	    }
	    else {
		nonDJU = true;
		pNewSubset = rtLINK_PosConstructor (NewPToken (0))->Close (m_pPPT);

		for (xDescriptor = xMergeOrigin; xDescriptor < xMergeLimit; xDescriptor++)
		    pDescriptorArray[xDescriptor].buildSubset (pNewSubset);

	    }

	    M_CPD *pNewContent = rBaseDescriptor.createContent (pNewSubset);

	    for (xDescriptor = xMergeOrigin; xDescriptor < xMergeLimit; xDescriptor++)
		pDescriptorArray[xDescriptor].moveContent (pNewContent)->removeFrom (this);

	    rBaseDescriptor.createFragment (pNewSubset, pNewContent, this);
	    pNewContent->release ();
	}

	xMergeOrigin = xMergeLimit;
    }
    if (mergeDone)
	CoalesceSuccessCount++;
    if (nonDJU)
	CoalesceNonDJUCount++;

    delete [] pDescriptorArray;
}


/***************
 *  'Flatten'  *
 ***************/

void VFragmentation::Flatten () {
    FlattenFragCount++;
    if (TracingFragmentationOps) {
	IO_printf ("FlattenFragmentation:%d\n", FlattenFragCount);
    }

    goToFirstFragment ();

    VFragment *pFragment;
    while (pFragment = currentFragment ()) {
/*****  If there is a fragmentation inside a fragmentation ... *****/
	VDescriptor& rFragmentDatum = pFragment->datum ();
	if (rFragmentDatum.isAFragmentation ()) {
	    VFragmentation& rSubFragmentation = rFragmentDatum.contentAsFragmentation ();

	    if (rSubFragmentation.m_fLock)
		m_fLock = true;

/*****  First, must distribute the sub-fragmentation ... *****/
	    rFragmentDatum.normalize ();

/*****  Loop thru this sub-fragmentation changing the subsets ... *****/

	    rSubFragmentation.goToFirstFragment ();

	    VFragment *pLastSubFragment = NilOf (VFragment*);
	    VFragment *pSubFragment;
	    while (pSubFragment = rSubFragmentation.currentFragment ()) {
		pLastSubFragment = pSubFragment;
		pSubFragment->setSubsetTo (
		    pFragment->subset ()->Extract (pSubFragment->subset ())
		);
		rSubFragmentation.goToNextFragment ();
	    }

/*****  Replace the fragment with the sub-fragmentation's fragment list *****/
	    if (pLastSubFragment) {
		rSubFragmentation.fragmentList()->m_pPrevious = pFragment->previous ();
		if (pFragment->previous()) {
		    pFragment->previous()->m_pNext
			= m_pCurrentFragment
			= rSubFragmentation.fragmentList ();
		    pFragment->m_pPrevious = NilOf (VFragment*);
		}
		else {
		    m_pFragmentList
			= m_pCurrentFragment
			= rSubFragmentation.fragmentList ();
		}
		pLastSubFragment->m_pNext = pFragment->next ();
		if (pFragment->next ()) {
		    pFragment->next ()->m_pPrevious = pLastSubFragment;
		    pFragment->m_pNext = NilOf (VFragment*);
		}

    /*****  Now free the unneeded parts of the original fragment ... *****/
		rSubFragmentation.m_pFragmentList	=
		rSubFragmentation.m_pCurrentFragment	= NilOf (VFragment*);
	    }

/*****  Update the fragment count for fragmentation ...  *****/
	    m_iFragmentCount += rSubFragmentation.fragmentCount () - 1;

/*****  And free the replaced fragment ...  *****/
	    pFragment->discard ();
	}
	else {
/***** else, a simple content - go to the next fragment ... *****/
	    goToNextFragment ();
	}
    }
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/*************
 *  'clear'  *
 *************/

void VFragmentation::clear () {
    m_pPPT->release ();
    m_pFragmentList->discard ();
}


/**********************
 *  'createFragment'  *
 **********************/

VFragment *VFragmentation::createFragment (rtLINK_CType *pSubset) {
    m_pFragmentList = new VFragment (m_pFragmentList, pSubset);
    m_iFragmentCount++;

    return m_pFragmentList;
}


/******************
 *  'distribute'  *
 ******************/

void VFragmentation::distribute (M_CPD *pDistribution) {
    if (IsNil (pDistribution))
	return;

    goToFirstFragment ();

    VFragment *pFragment;
    while (pFragment = currentFragment ()) {
	M_CPD *refuv = rtREFUV_LCExtract (pDistribution, pFragment->subset ());

	M_CPD *pSort;
	rtLINK_CType *linkc = rtLINK_RefUVToConstructor (refuv, &pSort);

	refuv->release ();

	if (pSort) {
	    pFragment->datum ().reorder (pSort);
	    pSort->release ();
	}

	pFragment->setSubsetTo (linkc);

	goToNextFragment ();
    }

/*****  Set a new PToken for the fragmentation ... *****/
    setPPT (static_cast<rtUVECTOR_Handle*>(pDistribution->containerHandle ())->rptHandle ());
}


/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void VFragmentation::describe (unsigned int xLevel) {
    IO_printf ("(nf:%d)", m_iFragmentCount);
    IO_printf ("(pt:[%d:%d])", m_pPPT->spaceIndex (), m_pPPT->containerIndex ());
    goToFirstFragment ();
    while (m_pCurrentFragment) {
	IO_printf("\n%*s", xLevel * 2, " "); 
	m_pCurrentFragment->describe (xLevel);
	goToNextFragment ();
    }
    IO_printf ("\n");
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VFragmentation::raiseIncorrectFragmentCountException () {
    goToFirstFragment ();
    while (m_pCurrentFragment)
	goToNextFragment ();

    VTransient iExceptionGenerator;
    iExceptionGenerator.raiseException (
	EC__InternalInconsistency,
	"VFragmentation: Incorrect Fragment Count(recorded:%d, observed:%d)",
	m_iFragmentCount, m_xCurrentFragment
    );
}

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

#include "VWhileTrueController.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VFragment.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VPrimitiveTaskController<VWhileTrueController>;


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VWhileTrueController);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VWhileTrueController::VWhileTrueController (
    ConstructionData const&	rTCData,
    VPrimitiveDescriptor*	pDescriptor,
    unsigned short		iFlags
) : VPrimitiveTaskController<VWhileTrueController> (
    rTCData, pDescriptor, iFlags, &VWhileTrueController::startup
)
{
}


/***************************
 ***************************
 *****  Continuations  *****
 ***************************
 ***************************/

/**********************************
 *****  Startup Continuation  *****
 **********************************/

void VWhileTrueController::startup () {
    m_iCondition.setToCopied (getCurrent ());

    loadDucWithNextParameter ();
    m_iBody.setToMoved (duc ());

    scheduleCondition ();
}


/*******************************
 *****  Body Continuation  *****
 *******************************/

void VWhileTrueController::bodyContinuation () {
    scheduleCondition ();
}


/************************************
 *****  Condition Continuation  *****
 ************************************/

void VWhileTrueController::conditionContinuation () {
    bool bodyShouldBeEvaluated;

    VDescriptor &rDuc = duc ();
    if (rDuc.convertVectorsToMonotypeIfPossible ())
	bodyShouldBeEvaluated = rDuc.contentAsMonotype ().storeCPD ()->NamesTheTrueClass ();
    else {
	rtLINK_CType *pSubset = rDuc.subsetOfType (
	    ptoken ()->ScratchPad (), &M_KnownObjectTable::TheTrueClass
	);
	if (pSubset) {
	    restrict (pSubset);
	    pSubset->release ();
	    bodyShouldBeEvaluated = true;
	}
	else bodyShouldBeEvaluated = false;
    }

    if (bodyShouldBeEvaluated)
	scheduleBody ();
    else
	loadDucWithFalse ();
}


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

void VWhileTrueController::restrict (rtLINK_CType *pSubset) {
    if (IsNil (m_pSubset))
	m_pSubset.setTo (pSubset);
    else
	m_pSubset.claim (m_pSubset->Extract (pSubset));

    restrict (pSubset, m_iBody);
    restrict (pSubset, m_iCondition);
}

void VWhileTrueController::restrict (rtLINK_CType *pSubset, VDescriptor& rDatum) {
    VDescriptor iDatumSubset;
    iDatumSubset.setToSubset (pSubset, rDatum);
    rDatum.setToMoved (iDatumSubset);
}


void VWhileTrueController::schedule (Continuation pContinuation, VDescriptor& rDatum) {
    setContinuationTo (pContinuation);

    beginValueCall	(0, m_pSubset);
    loadDucWithCopied	(rDatum);
    commitRecipient	();
    commitCall		();
}

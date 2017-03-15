/*****  VUtilityTask Implementation  *****/

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

#include "VUtilityTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "selector.h"

#include "IOMDriver.h"

#include "VSecondaryCall.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/************************************
 ************************************
 *****  Standard Continuations  *****
 ************************************
 ************************************/

void VUtilityTask::Bridge (VUtilityTask* pTask) {
    pTask->beginSecondaryCall	();
    pTask->commitParameters	();
    pTask->commitCall		(pTask->returnCase ());
}

void VUtilityTask::Delegate (VUtilityTask* pTask) {
    pTask->beginMessageCall	(KS__Value);
    pTask->commitRecipient	();
    pTask->commitCall		();
}

void VUtilityTask::Evaluate (VUtilityTask* pTask) {
    pTask->beginValueCall	(0);
    pTask->commitRecipient	();
    pTask->commitCall		();
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VUtilityTask);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VUtilityTask::MetaMaker () {
    CSym ("isAUtilityTask")->set (RTT, &g_siTrue);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VUtilityTask::VUtilityTask (ConstructionData const& rTCData, Continuation pContinuation)
: VTask (rTCData), m_pContinuation (pContinuation)
{
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****************
 *****  Run  *****
 *****************/

void VUtilityTask::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    if (m_pContinuation) {
		Continuation pContinuation = m_pContinuation;
		m_pContinuation = NilOf (Continuation);

		(*pContinuation) (this);
	    }
	    else if (pausedOnExit ())
		return;
	    else
		exit ();
	}
	break;

    default:
	exit ();
	break;
    }
}


/*******************************
 *******************************
 *****  Call Construction  *****
 *******************************
 *******************************/

void VUtilityTask::beginSecondaryCall (rtLINK_CType* pSubset) {
    m_pCuc = new VSecondaryCall (this, pSubset);
    commitRecipient ();
}


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

void VUtilityTask::reportInfo (unsigned int xLevel, VCall const* Unused(pContext)) const {
    reportInfoHeader (xLevel);

    report ("Utility\n");
}

void VUtilityTask::reportTrace () const {
    reportTraceHeader ("U");
    report ("\n");
}

char const* VUtilityTask::description () const {
    return "<---Utility--->";
}

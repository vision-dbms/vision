/*****  VTopTask Implementation  *****/

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

#include "VTopTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSelector.h"


/**********************
 **********************
 *****            *****
 *****  VTopTask  *****
 *****            *****
 **********************
 **********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VTopTask);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VTopTask::MetaMaker () {
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTopTask::VTopTask (VTask* pSpawningTask, IOMDriver* pChannel)
: VTopTaskBase (pSpawningTask, pChannel)
{
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VTopTask::datumAvailable_ () const {
    return m_fDatumAvailable || VTopTaskBase::datumAvailable_ ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****************
 *****  Run  *****
 *****************/

void VTopTask::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	beginMessageCall(KS__Value);
	commitRecipient ();
	commitCall ();

	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    m_fDatumAvailable = true;

	    if (pausedOnExit ())
		return;

	    exit ();
	    break;
	}
	break;

    default:
	exit ();
	break;
    }
}


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

void VTopTask::reportInfo (unsigned int xLevel, VCall const* Unused(pContext)) const {
    reportInfoHeader (xLevel);

    report ("%s\n", description ());
}

void VTopTask::reportTrace () const {
    reportTraceHeader ("T");

    report ("%s\n", description ());
}

char const* VTopTask::description () const {
    return "<---Top--->";
}

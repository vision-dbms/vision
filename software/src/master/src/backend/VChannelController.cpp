/*****  VChannelController Implementation  *****/

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

#include "VChannelController.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTclosure.h"


/********************************
 ********************************
 *****                      *****
 *****  VChannelController  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VChannelController);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VChannelController::VChannelController (VTask* pSpawningTask, IOMDriver* pChannel)
: VTopTaskBase		(pSpawningTask, pChannel)
, m_xNextChannelState	(IOMState_Unused)
, m_fWaitingForInput	(false)
, m_fWaitingForMutex	(false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VChannelController::~VChannelController () {
    channel ()->detach (this);
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****************
 *****  Run  *****
 *****************/

void VChannelController::run () {
    IOMDriver* const pChannel = channel ();

    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xLastChannelState = m_xNextChannelState = pChannel->state ();
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    m_iAutoMutexClaim.releaseClaim ();

	    if (m_fWaitingForMutex)
		m_fWaitingForMutex = false;
	    else if (m_xLastChannelState != m_xNextChannelState) {
		m_xLastChannelState = m_xNextChannelState;
		pChannel->SetStateTo (m_xNextChannelState);
	    }

	    DSC_Descriptor iInputHandler;
	    switch (pChannel->state ()) {
	    case IOMState_Starting:
	    case IOMState_Restarting:
		m_xNextChannelState = IOMState_Running;
		/***** NO BREAK *****/

	    case IOMState_Running:
		if (m_fWaitingForInput) {
		    pChannel->untilInputArrivesSuspend (this);

		    m_fWaitingForInput = false;
		}
		else if (pChannel->getInputHandler (iInputHandler)) {
		    if (!pChannel->acquireAutoMutex (m_iAutoMutexClaim, this)) {
			//  At this point, 'this' is blocked (i.e., !runnable ()):
			m_fWaitingForMutex = true;

			m_fWaitingForInput = false;
		    }
		    else {
			beginValueCall (0);
			loadDucWithMoved (iInputHandler);
			commitRecipient ();
			commitCall ();

			m_fWaitingForInput = true;
		    }
		}
		else {
		    switch (pChannel->channelType ()) {
		    case IOMChannelType_Service: {
			    IOMDriver* pNewConnection;
			    while (IsntNil (pNewConnection = pChannel->GetConnection ()))
				pNewConnection->EnableHandler (this);
			}

			m_fWaitingForInput = true;
			break;

		    case IOMChannelType_Stream:
			beginValueCall (0);
			loadDucWithIdentity (
			    new rtCLOSURE_Handle (
				new rtCONTEXT_Handle (ENVIR_KDsc_TheTLE), IOMDriver::XReadEvalPrintController
			    )
			);
			commitRecipient ();
			commitCall ();

			m_xNextChannelState = IOMState_Ready;
//			m_fWaitingForInput = true;
			break;

		    default:
			m_xNextChannelState = IOMState_Ready;
			break;
		    }
		}
		break;

	    default:
		if (pausedOnExit ())
		    return;
		else
		    exit ();
		break;
	    }
	}
	break;

    default:
	exit ();
	break;
    }
}


/******************
 *****  Fail  *****
 ******************/

void VChannelController::fail () {
    if (terminatedNot ()) {
	m_xNextChannelState = IOMState_Restarting;
	m_xState = State_Runnable;
    }
}


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

void VChannelController::reportInfo (unsigned int xLevel, VCall const* Unused(pContext)) const {
    reportInfoHeader (xLevel);

    report ("%s\n", description ());
}

void VChannelController::reportTrace () const {
    reportTraceHeader ("C");

    report ("%s\n", description ());
}

char const* VChannelController::description () const {
    return "<---ChannelController--->";
}

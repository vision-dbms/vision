/*****  VReadEvalPrintController Implementation  *****/

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

#include "VReadEvalPrint.h"

/************************
 *****  Supporting  *****
 ************************/

#include "batchvision.h"

#include "m.h"
#include "rslang.h"
#include "selector.h"

#include "vdebug.h"
#include "vstdio.h"

#include "IOMHandle.h"

#include "RTclosure.h"
#include "RTstring.h"

#include "VChannelController.h"
#include "VTransientServices.h"

#include "VkUUID.h"
#include "V_VString.h"

#include "Vca_VTrigger.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VPrimitiveTaskController<VReadEvalPrintController>;


/**************************************
 **************************************
 *****                            *****
 *****  VReadEvalPrintController  *****
 *****                            *****
 **************************************
 **************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VReadEvalPrintController);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VReadEvalPrintController::VReadEvalPrintController (
    ConstructionData const&	rTCData,
    VPrimitiveDescriptor*	pDescriptor,
    unsigned short		iFlags
) : VPrimitiveTaskController<VReadEvalPrintController> (
    rTCData, pDescriptor, iFlags, &VReadEvalPrintController::REPStart
), m_iStartTime			(0)
, m_iMidTime			(0)
, m_iEndTime			(0)
, m_pInitialOutputBuffer	(m_pOutputBuffer)
, m_pSelfDictionary		(0, getSelf ().dictionaryCPD ())
, m_iThisSource			(channel ()->consumedStartupExpression ())
, m_iExecutionsRemaining	(0)
, m_bDebuggerEnabled		(scheduler()->debuggerEnabled ())
, m_bEchoingInput		(false)
, m_bExecutionLogged		(true) 
, m_bNeedingSetup		(true)
, m_bVerboseStats		(true)
{
    if (IsntNil (getenv ("VisionTimingOnly"))) m_bVerboseStats = false;
    Batchvision::registerREPController (this);
    loadDucWithNA ();
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VReadEvalPrintController::isAController () const {
    return channel()->controller() == caller ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void VReadEvalPrintController::exit () {
    clearContinuation ();
    disableGC ();
    BaseClass::exit ();
}

void VReadEvalPrintController::fail () {
    if (terminatedNot ()) {
	setContinuationTo (&VReadEvalPrintController::REPRestart);
	m_xState = State_Runnable;
    }
}


/*******************************
 *******************************
 *****  Execution Support  *****
 *******************************
 *******************************/

/*******************************
 *****  Command Processor  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to process a '?' command.
 *
 *  Argument:
 *	pLine			- the address of the input line being processed.
 *
 *****/
void VReadEvalPrintController::ProcessCommand (char const *pLine) {
    VComputationScheduler* pScheduler = scheduler ();

    switch (pLine[1]) {
    case 'c':
    case 'C':
    /*****  ...compile and NOT run the input and print the result:  *****/

    /*----  Move 'm_iThisSource' to 'm_iLastSource'...  ----*/
        m_iLastSource.setToMoved (m_iThisSource);

    /*----  ...compile the new program...  ----*/
	m_bExecutionLogged = false;
	pScheduler->getRefreshedCumulativeTime (m_iStartTime);

        m_pBlock.claim (
	    RSLANG_Compile (codScratchPad (), m_iLastSource, m_pSelfDictionary)
	);

	pScheduler->getRefreshedCumulativeTime (m_iMidTime);
	m_iEndTime = m_iMidTime;

    /*----  ... and restart the loop.  ----*/
        break;

    case 'd':
    case 'D':
    /*****  ...enable the debugger:  *****/
        if (m_bDebuggerEnabled)
	    setContinuationTo (&VReadEvalPrintController::REPDebug);
        break;

    case 'e':
    case 'E':
    /****  Toggle echo input ... *****/
	if (m_bEchoingInput)
        {
	    m_bEchoingInput = false;
	    display ("Echo Input turned Off\n");
        }
        else 
        {
	    m_bEchoingInput = true;
	    display ("Echo Input turned On\n");
        }
        break;
	    
    case 'f':
    case 'F':
    /*****  Flush the current input  *****/
        m_iThisSource.clear ();
        break;

    /********************************************************************
     *----                  Begin Connected Cases                   ----*
     ********************************************************************/
    case '>': {
    /*****  ...compile and evaluate the input and print the result to the specified file: *****/

	/*----  Get the filename for redirection...  ----*/
	    char const *fn_ptr;
	    for (fn_ptr = &pLine[2]; *fn_ptr && isspace(*fn_ptr); fn_ptr++);

	    char const *lb_ptr;
	    for (lb_ptr = fn_ptr; *lb_ptr && !isspace(*lb_ptr); lb_ptr++);
	    *(char*)lb_ptr = '\0';
	    channel()->OpenOutputFile (fn_ptr, true);
	}

    /*------------------------------------------------------------------*
     *****************		No Break		*****************
     *------------------------------------------------------------------*/

    case 'g':
    case 'G':
    /*****  ...compile and evaluate the input and print the result:  *****/

    /*----  Move 'm_iThisSource' to 'm_iLastSource'...  ----*/
        m_iLastSource.setToMoved (m_iThisSource);

    /*------------------------------------------------------------------*
     *****************		No Break		*****************
     *------------------------------------------------------------------*/

    case 'l':
    case 'L':
    /*----  ...compile the new program...  ----*/
	m_bExecutionLogged = false;
	pScheduler->getRefreshedCumulativeTime (m_iStartTime);
        m_pBlock.claim (
	    RSLANG_Compile (codScratchPad (), m_iLastSource, m_pSelfDictionary)
	);
	pScheduler->getRefreshedCumulativeTime (m_iMidTime);
	m_iEndTime = m_iMidTime;

    /*----  ...and run it.  ----*/
        if (m_pBlock.isntNil ())
	    setContinuationTo (&VReadEvalPrintController::REPEval);

        break;
    /********************************************************************
     *----                  End of Connected Cases                  ----*
     ********************************************************************/

    case 'o':
	switch (pLine[2]) {
	case 'd':
	    switch (pLine[3]) {
	    case '+':
		m_bDebuggerEnabled = true;
		break;
	    case '-':
		m_bDebuggerEnabled = false;
		break;
	    default:
		break;
	    }
	    break;
	case 'm':
	    switch (pLine[3]) {
	    case '+':
		channel()->SetAutoMutexTo (true);
		break;
	    case '-':
		channel()->SetAutoMutexTo (false);
		break;
	    default:
		break;
	    }
	    break;
	case 't':
	    switch (pLine[3]) {
	    case '+':
		m_bVerboseStats = true;
		break;
	    case '-':
		m_bVerboseStats = false;
		break;
	    default:
		break;
	    }
	    break;
	case 'D':
	    switch (pLine[3]) {
	    case '+':
		scheduler()->setDebuggerEnabledTo (true);
		break;
	    case '-':
		scheduler()->setDebuggerEnabledTo (false);
		break;
	    default:
		break;
	    }
	    m_bDebuggerEnabled = scheduler()->debuggerEnabled ();
	    break;
	case 'E':
	    switch (pLine[3]) {
	    case '+':
		channel()->SetPromptFormatTo (IOMPromptFormat_Editor);
		break;
	    case '-':
		channel()->SetPromptFormatTo (IOMPromptFormat_Normal);
		break;
	    default:
		break;
	    }
	    break;
	default:
	    break;
	}
	break;

    case 'r':
    /*****  Compile and repetitively evaluate the input and print the run 
            times  *****/
    /*----  Get the repeat count...  ----*/
	m_iExecutionsRequested = m_iExecutionsRemaining = atoi (&pLine[2]);
	
    /*----  Move 'm_iThisSource' to 'm_iLastSource'...  ----*/
        m_iLastSource.setToMoved (m_iThisSource);

    /*----  ...compile the new program...  ----*/
	m_bExecutionLogged = false;
	pScheduler->getRefreshedCumulativeTime (m_iStartTime);
        m_pBlock.claim (
	    RSLANG_Compile (codScratchPad (), m_iLastSource, m_pSelfDictionary)
	);
	pScheduler->getRefreshedCumulativeTime (m_iMidTime);
	m_iEndTime = m_iMidTime;

    /*----  ...and run it.  ----*/
        if (m_pBlock.isntNil ())
	    setContinuationTo (&VReadEvalPrintController::REPRepeat);

        break;

    case 's':
    /*****  Print the current input  *****/
        IO_puts (m_iThisSource);
        break;

    case 'S':
    /*****  Print the previous input  *****/
        IO_puts (m_iLastSource);
        break;

    case 't':
      /*****  Display the run times of the last program  *****/
	display ("Compile Statistics:\n");
	UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, elapsed);
	UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, user   );
#if defined(__VMS)
	if (m_bVerboseStats) {
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, bufio  );
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, dirio  );
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, filcnt );
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, pageflts);
	} else {
	    UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, system );
	}
#elif defined(_WIN32)
	UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, system );
#else
	UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, system );
	if (m_bVerboseStats) {
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, majflts);
	    UTIL_PrintStatDelta (m_iStartTime, m_iMidTime, minflts);
	}
#endif

	display ("Execute Statistics:\n");
	UTIL_PrintTimeDelta (m_iMidTime, m_iEndTime, elapsed);
	UTIL_PrintTimeDelta (m_iMidTime, m_iEndTime, user   );
#if defined(__VMS)
	if (m_bVerboseStats) {
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, bufio  );
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, dirio  );
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, filcnt );
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, pageflts);
	} else {
	    UTIL_PrintTimeDelta (m_iStartTime, m_iMidTime, system );
	}
#elif defined(_WIN32)
	UTIL_PrintTimeDelta (m_iMidTime, m_iEndTime, system );
#else
	UTIL_PrintTimeDelta (m_iMidTime, m_iEndTime, system );
	if (m_bVerboseStats) {
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, majflts);
	    UTIL_PrintStatDelta (m_iMidTime, m_iEndTime, minflts);
	}
#endif
        break;

    case 'u': {
	    VkUUIDGenerate iUUID;
	    VString iString;
	    iUUID.GetString (iString);
	    IO_printf ("%s\n", (char const*)iString);
	}
	break;

    case 'v':
	report (
	    ">>> Tracing %s\n", channel()->toggleTracing () ? "Enabled" : "Disabled"
	);
	break;

    case 'w':
    case 'W':
    /*****  Update the network or user space ...  *****/

    /*----  Obtain the update expression...  *****/
        if (IsNil (pLine = getenv ("VisionUpdateExpression")))
	    pLine = "Utility updateNetwork;";

    /*----  ...compile the new program...  ----*/
	m_bExecutionLogged = false;
	pScheduler->getRefreshedCumulativeTime (m_iStartTime);
        m_pBlock.claim (
	    RSLANG_Compile (codScratchPad (), pLine, m_pSelfDictionary)
	);
	pScheduler->getRefreshedCumulativeTime (m_iMidTime);
	m_iEndTime = m_iMidTime;

    /*----  ...and run it.  ----*/
        if (m_pBlock.isntNil ())
	    setContinuationTo (&VReadEvalPrintController::REPEval);

        break;
	
    default:
        break;
    } /* end of 'switch' */
}


/***********************************************
 *****  Evaluation Scheduling and Cleanup  *****
 ***********************************************/

void VReadEvalPrintController::ScheduleEvaluation () {
    disableGC ();
    if (m_iExecutionsRemaining == 0)
	setContinuationTo (&VReadEvalPrintController::REPWrap);
    else {
	setContinuationTo (&VReadEvalPrintController::REPRepeat);
	m_iExecutionsRemaining--;
    }

    beginMessageCall	(KS__Print);

    beginBoundCall	(m_pBlock);
    commitRecipient	(VMagicWord_Self);
    commitCall		();
    commitRecipient	();

    commitCall		();
}

void VReadEvalPrintController::ConcludeEvaluation (bool fDisplayingOutput) {
    if (m_pOutputBuffer.referent () != m_pInitialOutputBuffer) {
	if (fDisplayingOutput)
	    m_pOutputBuffer->moveOutputToChannel ();

	m_pOutputBuffer.setTo (m_pInitialOutputBuffer);
    }

    if (isAController ())
	m_iAutoMutexClaim.releaseClaim ();


    setContinuationTo (&VReadEvalPrintController::REPRead);
    logUsage ();
    enableGC ();
}


/***************************
 ***************************
 *****  Continuations  *****
 ***************************
 ***************************/

/**********************
 *****  REPStart  *****
 **********************/

void VReadEvalPrintController::REPStart () {
    setContinuationTo (&VReadEvalPrintController::REPRead);

    if (m_iThisSource.length () > 0)
	ProcessCommand ("?g");
}

/************************
 *****  REPRestart  *****
 ************************/

void VReadEvalPrintController::REPRestart () {
    ConcludeEvaluation (false);

    m_bNeedingSetup = true;
    m_iExecutionsRemaining = 0;

    ProcessCommand ("?f");
}


/*********************
 *****  REPRead  *****
 *********************/

void VReadEvalPrintController::REPRead () {
    setContinuationTo (&VReadEvalPrintController::REPRead);

    if (m_bNeedingSetup) {
	m_bNeedingSetup = false;
	if (m_bDebuggerEnabled)
	    display (
		"Type:\n"
		"    ?d         ... to invoke the debugger.\n"
		"    ?f         ... to erase the current program.\n"
		"    ?g         ... to compile and run the current program.\n"
		"    ?l         ... to compile and run the last program.\n"
		"    ?r[count]  ... to compile and repeated run the current program.\n"
		"    ?c         ... to compile the current program.\n"
		"    ?s         ... to display the current program.\n"
		"    ?S         ... to display the last program.\n"
		"    ?t         ... to display compile and run times of the last program.\n"
		"    ?e         ... to toggle input echo.\n"
		"    ?w         ... to save the global environment.\n"
		"\n"
		"Any thing else will get appended to the current input buffer.\n"
	    );

	if (isAController ())
	    channel()->SetStateToRunning ();
    }

    IOMHandle handle;
    handle.construct (channel());

    char * pLine;
    size_t sLine;
    VkStatusType xStatus = channel()->GetLine (&handle, "V> ", &pLine, &sLine);
    switch (xStatus) {
    case VkStatusType_Completed:
	if (m_bEchoingInput)
	    display ("V> %s", pLine);

	if (pLine[0] != '?')
	    m_iThisSource.append (pLine);
	else
	    ProcessCommand (pLine);

	deallocate (pLine);
	break;

    case VkStatusType_Blocked:
	channel()->untilInputArrivesSuspend (this);
	break;

    default:
	exit ();
	break;
    }
}


/**********************
 *****  REPDebug  *****
 **********************/

void VReadEvalPrintController::REPDebug () {
    setContinuationTo (&VReadEvalPrintController::REPRead);

    DEBUG_Main (codScratchPad ());

/*-----------------------------------------------------------------------*
 *  DEBUG_Main returns for one of two reasons:
 *	1) it read an EOF and is legitimately done with its work.
 *	2) it encountered a syntax or literal/symbol lookup error.
 *  In the second case, it must be restarted.  Handle that restart here:
 *-----------------------------------------------------------------------*/
    switch (channel()->statusType()) {
    case VkStatusType_Blocked:
    case VkStatusType_Completed:
	setContinuationTo (&VReadEvalPrintController::REPDebug);
	break;
    default:
	break;
    }
}

/*********************
 *****  REPEval  *****
 *********************/

void VReadEvalPrintController::REPEval () {
//  Acquire the global mutex if we're required to do so, ...
    if (isAController () && !channel()->acquireAutoMutex (m_iAutoMutexClaim, this))
	setContinuationTo (&VReadEvalPrintController::REPEval);
    else {
    //  ... create a local output buffer, ...
	m_pOutputBuffer.setTo (new VOutputBuffer (m_pDomain, channel ()));

    //  ... and start the evaluation, ...
	ScheduleEvaluation ();
    }
}


/***********************
 *****  REPRepeat  *****
 ***********************/

void VReadEvalPrintController::REPRepeat () {
    if (m_iExecutionsRemaining > 0)
	ScheduleEvaluation ();
    else {
	ConcludeEvaluation ();

	display (
	    "%8.3g %8.3g %8.3g %6d  ", 
	    UTIL_TimeDelta (m_iMidTime, m_iEndTime, elapsed),
	    UTIL_TimeDelta (m_iMidTime, m_iEndTime, user), 
	    UTIL_TimeDelta (m_iMidTime, m_iEndTime, system),
	    m_iExecutionsRequested
	);
	IO_puts (m_iLastSource);
    }
}

/*********************
 *****  REPWrap  *****
 *********************/

void VReadEvalPrintController::REPWrap () {
    ConcludeEvaluation ();

    if (m_iFlags)
	exit ();
}

void VReadEvalPrintController::logUsage () {
    if (!m_bExecutionLogged) {
	m_bExecutionLogged = true;
	scheduler ()->getRefreshedCumulativeTime (m_iEndTime);
	VString delimiter;
	transientServicesProvider ()->usageLogDelimiter (delimiter);
	char iBuffer[256];
	transientServicesProvider ()->logUsageInfo (
	     "%08x%s%5u%s%8s%s%5u%s%5u%s%8g%s%8g%s%8g",
	     gethostid (), delimiter.content (),
	     getpid (), delimiter.content (),
	     Vk_username (iBuffer, sizeof (iBuffer)), delimiter.content (),
	     getuid (), delimiter.content (),
	     getgid (), delimiter.content (),
	     // following fields are specified to be in units of milliseconds ....
	     UTIL_TimeDelta (m_iMidTime, m_iEndTime, elapsed) * 1000, delimiter.content (),
	     UTIL_TimeDelta (m_iMidTime, m_iEndTime, user) * 1000,  delimiter.content (),
	     UTIL_TimeDelta (m_iMidTime, m_iEndTime, system) * 1000
	);
    }
}


/************************
 *****  GC Control  *****
 ************************/

void VReadEvalPrintController::enableGC () {
    if (!m_bGCEnabled) {
	m_bGCEnabled = true;
	scheduler()->enableGC ();
    }
}

void VReadEvalPrintController::disableGC () {
    if (m_bGCEnabled) {
	m_bGCEnabled = false;
	scheduler()->disableGC ();
    }
}

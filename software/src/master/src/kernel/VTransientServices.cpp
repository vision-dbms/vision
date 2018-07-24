/*****  VTransientServices Implementation  *****/

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

#include "VTransientServices.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "V_VTime.h"
#include "VkDynamicArrayOf.h"

#if defined(_WIN32)
static char const *const g_pNullDeviceName = "NUL:";
#elif defined(__VMS)
static char const *const g_pNullDeviceName = "NL:";
extern "C" {
    void *decc$malloc (size_t sAllocation);
    void *decc$realloc (void *pMemory, size_t sMemory);
    void *decc$free (void *pMemory);
}
#else
static char const *const g_pNullDeviceName = "/dev/null";
#endif

char const* const VTransientServices::g_pRestOfLineBreakSet = "\r\n";
char const* const VTransientServices::g_pWhitespaceBreakSet = " \t\r\n";
char const* VTransientServices::g_iEventNames[24] = {
    "CANCEL", "HARDSTOP", "CLIENTDISCONNECT", "WORKERHARDLIMIT", "WORKERSOFTLIMIT", "EVALUATORDISCONNECT",
    "WIPERROR", "WIPRETRYERROR", "WIPTIMEOUT", "WIPRETRYTIMEOUT", "SEFAILCONNECT", "WINUSELIMIT",
    "WUCLIMIT", "TEFFECTIVEWORKERLIMIT", "NONEWGENERATION", "NOEVALUATOR", "CANCELINQUE", "PROMPTSERVERGONE",
    "BRIDGEGONE", "LASTRESORT", "NDFERROR", "NULLGOFER", "BRIDGECONNERR", "NULLITF"
};


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTransientServices::VTransientServices () 
    : m_bIsBackground   (false) 
{
    attachTransientServicesProvider (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VTransientServices::~VTransientServices () {
    detachTransientServicesProvider (this);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void VTransientServices::updateBackgroundSwitch (bool bIsBackground) {
    if (bIsBackground)
	createDaemon ();
}

/****************************************************
 ****************************************************
 *****  Parse Notification Services Table File  ***** 
 ****************************************************
 ****************************************************/

void VTransientServices::parseNSTFile (char const *pNSTFileName) {
    VSimpleFile iNSTFile;
    if (!pNSTFileName || !iNSTFile.Open (pNSTFileName, "r"))
	return;
    log ("VTransientServices::parseNSTFile '%s'", pNSTFileName);
    VString iBuffer;
    bool bCompleted = false;
    VString iEmail;
    VString iPage;
    VString iLog;
    VString iMonster;
    VString iRPD;
    int xId;
    while (!bCompleted && iNSTFile.GetLine (iBuffer)) {
	ParseState xState = ParseState_ExpectingTag;
	char const* pBreakSet = g_pWhitespaceBreakSet;
	char *pBufferPtr;
	for (
	    char *pToken = strtok_r (iBuffer.storage(), pBreakSet, &pBufferPtr);
	    pToken && ParseState_Error != xState;
	    pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	) {
	    pToken += strspn (pToken, g_pWhitespaceBreakSet);
	    
	    VString iCookedToken;
	    iCookedToken.setTo (pToken);
	    
	    switch (xState) {
	    case ParseState_ExpectingTag:
		xState
		    = 0 == strcasecmp (iCookedToken, "email")
		    ? ParseState_ExpectingEmail
		    
		    : 0 == strcasecmp (iCookedToken, "page")
		    ? ParseState_ExpectingPage

		    : 0 == strcasecmp (iCookedToken, "log")
		    ? ParseState_ExpectingLogPath

		    : 0 == strcasecmp (iCookedToken, "monster")
		    ? ParseState_ExpectingMonster

		    : 0 == strcasecmp (iCookedToken, "rpd")
		    ? ParseState_ExpectingRPD
		    
		    : ParseState_Error;
		
		if (ParseState_Error == xState)
		    log ("VTransientServices::parseNSTFile --> Unknown tag '%s'", static_cast <char const *> (iCookedToken.content ()));
		break;
	    case ParseState_ExpectingLogPath:
		iLog.setTo (iCookedToken);
		xState = ParseState_ExpectingLogEventId;
		//		fprintf(stderr, "Log: %s\n", iCookedToken.content ());
		break;
	    case ParseState_ExpectingLogEventId:
		//		fprintf(stderr, "Id: %s\n", iCookedToken.content ());
		if (1 == sscanf (iCookedToken, " %d", &xId)) {
		    if (xId >= 0 && xId <= 1000)
			m_iNST[xId][0].Insert (iLog);
		}
		break;
	    case ParseState_ExpectingEmail:
		iEmail.setTo (iCookedToken);
		xState = ParseState_ExpectingEmailEventId;
		//		fprintf(stderr, "Email: %s\n", iCookedToken.content ());
		break;
	    case ParseState_ExpectingEmailEventId:
		//		fprintf(stderr, "Id: %s\n", iCookedToken.content ());
		if (1 == sscanf (iCookedToken, " %d", &xId)) {
		    if (xId >= 0 && xId <= 1000)
			m_iNST[xId][1].Insert (iEmail);
		}
		break;
	    case ParseState_ExpectingPage:
		iPage.setTo (iCookedToken);
		xState = ParseState_ExpectingPageEventId;
		break;
	    case ParseState_ExpectingPageEventId:
		if (1 == sscanf (iCookedToken, " %d", &xId)) {
		    if (xId >= 0 && xId <= 1000)
			m_iNST[xId][2].Insert (iPage);
		}
		break;

	    case ParseState_ExpectingMonster:
		iMonster.setTo (iCookedToken);
		xState = ParseState_ExpectingMonsterEventId;
		break;
	    case ParseState_ExpectingMonsterEventId:
		if (1 == sscanf (iCookedToken, " %d", &xId)) {
		    if (xId >= 0 && xId <= 1000)
			m_iNST[xId][3].Insert (iMonster);
		}
		break;

	    case ParseState_ExpectingRPD:
		iRPD.setTo (iCookedToken);
		xState = ParseState_ExpectingRPDEventId;
		break;
	    case ParseState_ExpectingRPDEventId:
		if (1 == sscanf (iCookedToken, " %d", &xId)) {
		    if (xId >= 0 && xId <= 1000)
			m_iNST[xId][4].Insert (iRPD);
		}
		break;

	    default:
		log ("VTransientServices::parseNSTFile --> Error: unknown parse state %d[%s]", xState, iCookedToken.content ());
		xState = ParseState_Error;
		break;
	    }
	    switch (xState) {
	    case ParseState_ExpectingTag:
	    case ParseState_ExpectingLogPath:
	    case ParseState_ExpectingLogEventId:
	    case ParseState_ExpectingEmail:
	    case ParseState_ExpectingEmailEventId:
	    case ParseState_ExpectingPage:
	    case ParseState_ExpectingPageEventId:
	    case ParseState_ExpectingMonster:
	    case ParseState_ExpectingMonsterEventId:
	    case ParseState_ExpectingRPD:
	    case ParseState_ExpectingRPDEventId:
		pBreakSet = g_pWhitespaceBreakSet;
		break;
	    default:
		pBreakSet = g_pRestOfLineBreakSet;
		break;
	    }
	} // for
	iBuffer.clear ();
    }  // while
    /*
    VkDynamicArrayOf<VString> iTargets;
    for (size_t i = 0; i < 100; ++i) {
	fprintf (stderr, "\n");
	for (size_t j = 0; j < 5; ++j) {
	    fprintf (stderr, "| Number of subscribers: %d |", m_iNST[i][j].cardinality ()); 
	    if( transientServicesProvider()->getNSTargets (iTargets, i, j)){
		V::U32 iSize = iTargets.cardinality ();
		for (V::U32 i = 0; i < iSize; i++) {
		    fprintf (stderr, " :%s", iTargets[i].content ());
		}
	    }
	    else
		fprintf (stderr, "no subscribers\n");
	}
    }
    */
    iNSTFile.close ();
}

bool VTransientServices::getNSTargets (VkDynamicArrayOf<VString> &rResult, int xEvent, int xType) const {
    bool bResult = false;
    if (xEvent >= 0 && xEvent <= 1000 && xType >= 0 && xType < 5) {
	V::U32 iSize = m_iNST[xEvent][xType].cardinality ();
	if (iSize > 0) {
	    bResult = true;
	    rResult.DeleteAll ();
	    rResult.Append (iSize);
	    for (V::U32 i=0;i<iSize; i++) {
		rResult[i].setTo (m_iNST[xEvent][xType][i].content ());
	    }
	}
    }
    return bResult;
}

 
/*******************************
 *******************************
 *****  Memory Management  *****
 *******************************
 *******************************/

void *VTransientServices::allocate (size_t cBytes) {
#if !defined(__VMS) || !defined(_DEBUG)
    void *const pResult = ::malloc (cBytes);
#else
    void *const pResult = decc$malloc (cBytes);
#endif

    if (pResult)
	return pResult;

    raiseApplicationException ("Error Allocating %u Bytes", cBytes);

    return pResult;
}

void *VTransientServices::reallocate (void *pMemory, size_t cBytes) {
#if !defined(__VMS) || !defined(_DEBUG)
    void *const pResult = ::realloc (pMemory, cBytes);
#else
    void *const pResult = decc$realloc (pMemory, cBytes);
#endif

    if (pResult)
	return pResult;

    raiseApplicationException (
	"Error Reallocating Memory At %p To %u Bytes", pMemory, cBytes
    );

    return pResult;
}

void VTransientServices::deallocate (void *pMemory) {
#if !defined(__VMS) || !defined(_DEBUG)
    ::free (pMemory);
#else
    decc$free (pMemory);
#endif
}


/********************************
 ********************************
 *****  Message Formatting  *****
 ********************************
 ********************************/

/***********************
 *****  'display'  *****
 ***********************/

int VTransientServices::vdisplay (char const* pMessage, va_list pArgList) {
    V::VArgList iArgList (pArgList);
    int const sWrite = ::vfprintf (stdout, pMessage, iArgList);
    fflush (stdout);
    return sWrite;
}


/*************************************
 *****  Message Buffer Services  *****
 *************************************/
/*---------------------------------------------------------------------------
 * Message Buffer Services provides a pool of message buffers which are
 * automatically recycled by message buffer services.  Message buffers are
 * allocated in two ways - as a request for a buffer of a specific size
 * (GetNextMessageBuffer) and automatically by (VTransientServices::vstring).
 * Message buffers will cannot be larger than 'MessageBufferSize'.  Requests
 * for more than 'MessageBufferSize' bytes will be truncated to
 * 'MaxMessageBufferSize'.  As currently implemented, a 'MaxMessageBufferSize'
 * character buffer will be allocated by 'UTIL_FormatMessage'.
 *
 * Note that message buffers are automatically reused without warning.  The
 * only guarantee associated with their use is that they will be unchanged for
 * some SHORT period of time after their allocation.
 *---------------------------------------------------------------------------
 */

/*****  Message Buffer Pool Declarations  *****/
#define MessageBufferSize   256
#define MessageBufferCount  8
PrivateVarDef char MessageBufferPool[MessageBufferCount][MessageBufferSize];
PrivateVarDef unsigned int NextFreeMessageBuffer = 0;

/*---------------------------------------------------------------------------
 *****  Routine to obtain a temporary character string buffer
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	The address of the buffer.  Note that this buffer will eventually be
 *	reclaimed for other uses.
 *
 *****/
PrivateFnDef char *GetNextMessageBuffer () {
    char *result = MessageBufferPool[NextFreeMessageBuffer];
    NextFreeMessageBuffer = (NextFreeMessageBuffer + 1) % MessageBufferCount;

    return result;
}

/*---------------------------------------------------------------------------
 *****  Routine to format a message in a temporary buffer.
 *
 *  Arguments:
 *	Identical to 'printf'
 *
 *  Returns:
 *	The address of the buffer in which the message was formatted.  Note
 *	that this buffer will eventually be reclaimed for other uses.
 *
 *****/
char const *VTransientServices::vstring (char const* pMessage, va_list pArgList) {
    char *const pBuffer= GetNextMessageBuffer ();
    V::VArgList iArgList (pArgList);
    STD_vsprintf (pBuffer, pMessage, iArgList);
    return pBuffer;
}


/**********************************
 **********************************
 *****  Resource Reclamation  *****
 **********************************
 **********************************/

bool VTransientServices::garbageCollected () {
    return false;
}

bool VTransientServices::mappingsReclaimed () {
    return false;
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VTransientServices::vraiseApplicationException (
    char const *pMessage, va_list pArgList
) {
    fflush (stdout);
    fprintf (stderr, ">>> %s\n", vstring (pMessage, pArgList));
    exit (ErrorExitValue);
}

void VTransientServices::vraiseSystemException (
    char const *pMessage, va_list pArgList
) {
    vraiseApplicationException (pMessage, pArgList);
}

void VTransientServices::vraiseUnimplementedOperationException (
    char const *pMessage, va_list pArgList
) {
    raiseApplicationException (
	"Unimplemented Operation: %s", vstring (pMessage, pArgList)
    );
}

void VTransientServices::vraiseException (
    ERRDEF_ErrorCode xErrorCode, char const *pMessage, va_list pArgList
) {
    raiseApplicationException (
	">>> Error #%d: %s\n", xErrorCode, vstring (pMessage, pArgList)
    );
}

/*********************
 *********************
 *****  Logging  *****
 *********************
 *********************/

void VTransientServices::log (char const *pFormat, ...) {
    V_VARGLIST (ap, pFormat);
    m_iLogFile.vlog (pFormat, ap);
}
void VTransientServices::usageLogFilePath (VString &rLogFilePath) const {
}
void VTransientServices::usageLogDelimiter (VString &rLogDelimiter) const {
}
bool VTransientServices::usageLogSwitch () const {
    return false;
}
unsigned int VTransientServices::usageLogFileSize () const {
    return 0;
}
unsigned int VTransientServices::usageLogFileBackups () const {
    return 0;
}

void VTransientServices::updateUsageLogFile () {
}
void VTransientServices::updateUsageLogSwitch (bool bSwitch) {
}
void VTransientServices::updateUsageLogFilePath (char const *pPath) {
}
void VTransientServices::updateUsageLogSize (unsigned int iSize) {
}
void VTransientServices::updateUsageLogBackups (unsigned int iBackups) {
}
void VTransientServices::updateUsageLogTag (char const *pTag) {
}
void VTransientServices::updateUsageLogDelimiter (char const *pDelimiter) {
}
void VTransientServices::logUsageInfo (char const *pFormat, ...) {
}
void VTransientServices::vlogUsageInfo (char const *pFormat, va_list ap){
}
void VTransientServices::updateCamLogPath (char const *pPath) {
    if (pPath != 0) {
	m_iCamLog.updateLogFilePath (pPath);
    }
}

/**************************
 **************************
 *****  Notification  *****
 **************************
 **************************/

V::VString VTransientServices::getNSMessage () const {
    return m_iNSMessage;
}
void VTransientServices::setNSServer (char const *pServer) {
}
void VTransientServices::setNSMessage (char const *pMsg) {
}


/*****************************
 *****************************
 *****  Daemon Creation  *****
 *****************************
 *****************************/

/********
 * Note: Daemon Creation in Unix 
 *    1. Fork and exit the parent. The child is now guaranteed not to be a 
 *       Process Group Leader 
 *    2. Make this child a session leader/process group leader using setsid () 
 *       to lose the controlling terminal
 *    3. Fork again and exit the parent to make the new child a non-session 
 *       group leader (to prevent the child from reacquiring the controlling 
 *       terminal) 
 *    4. Change the current working directory for the daemon to "/tmp" and unset 
 *       the masking attributes
 *    5. Dup stdin to null device, stderr, stdout to log file 
 ****/

bool VTransientServices::createDaemon () {
    if (m_bIsBackground)
	return true;

#if !defined(__VMS) && !defined (_WIN32)
    switch (fork ()) {
    case 0:
	break;
    case -1:
	return false;
    default:
	_exit (0);
    }
    if (setsid () < 0)
	return false; 

    switch (fork ()) {
    case 0:
//	chdir ("/tmp");
//	umask (0);
	break;
    case -1:
	return false;
    default:
	_exit (0);
    }
    m_bIsBackground = true;
#endif
    redirect_stdin ();
    redirect_stdout_stderr ();
    return true;
}

/****
 * If running in background mode close the stdin and redirect it to 
 * the null device
 ****/

void VTransientServices::redirect_stdin () {
    if (isBackground ()) {
	int xNullDeviceHandle = open (g_pNullDeviceName, O_RDONLY, 0);

	//  close stdin and bind null-device to stdin
	dup2 (xNullDeviceHandle, 0);
	V::SetCloseOnExec (0);

	close (xNullDeviceHandle);
    }
}

/****
 * If running in background mode redirect the stdout and stderr to 
 * the application's current log file (if it is valid)
 ****/

void VTransientServices::redirect_stdout_stderr () {
    if (isBackground ()) {
	m_iLogFile.redirectStdOutToLog ();
	m_iLogFile.redirectStdErrToLog ();
    }
}

/**********************************
 **********************************
 *****  Notification Service  *****
 **********************************
 **********************************/

void VTransientServices::notify (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap) {
}

bool VTransientServices::parseNotificationTemplate (VSimpleFile &rSessionsFile) {

    bool bCompleted = false;

    VString iBuffer;
    PlanEntry *pEntry;
    NSEntry *pNSEntry = new NSEntry ();
    while (!bCompleted && rSessionsFile.GetLine (iBuffer)) {
      ParseState xState = ParseState_ExpectingTag;
      char const *pBreakSet = g_pWhitespaceBreakSet;
      char *pBufferPtr;
      for (
	   char *pToken = strtok_r (iBuffer.storage (), pBreakSet, &pBufferPtr);
	   pToken && ParseState_Error != xState;
	   pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	   ) {
	pToken += strspn (pToken, g_pWhitespaceBreakSet);

	switch (xState) {
	case ParseState_ExpectingTag:
	  xState = 0 == strcasecmp (pToken, "Name")
            ? ParseState_ExpectingNotificationName
	    : 0 == strcasecmp (pToken, "Plan_Template")
	    ? ParseState_ExpectingPlanSentinel
	    : 0 == strcasecmp (pToken, "Notification_Template")
	    ? ParseState_ExpectingNotificationSentinel
	    : 0 == strcasecmp (pToken, "Server")
	    ? ParseState_ExpectingNotificationServerName
	    : ParseState_Error;  
	  if (ParseState_Error == xState)
	      log ("VTransientServices::parseNotificationTemplate --> Unknown tag '%s'", pToken);
	  break;
	case ParseState_ExpectingPlanSentinel:
	  if (strcmp (pToken, "Begin") == 0) {
	    pEntry = new PlanEntry ();
	    bool bResult = parsePlanTemplate (rSessionsFile, pEntry);
	    if (bResult && (0 != pEntry->name ().length ())) {
		pNSEntry->insertEntry (pEntry);
		pEntry = NULL;
	    } else {
		log ("VTransientServices::parseNotificationTemplate --> Error: cannot insert Plan entry '%s'", pEntry->name ().content ());
		delete pEntry;
		pEntry = NULL;
		xState = ParseState_Error;
	    }
          }
	  else {
	    log ("VTransientServices::parseNotificationTemplate --> Error: found '%s' when expecting 'Begin'", pToken);
	    xState = ParseState_Error;
	  }
 	  break;
	case ParseState_ExpectingNotificationSentinel:
	  if (strcmp (pToken, "End") == 0) {
	      unsigned int xIdx;
	      if (m_iNSEntries.Insert (pNSEntry->name (), xIdx)) {
		  m_iNSEntries[xIdx] = pNSEntry;
		  pNSEntry = NULL;
		  bCompleted = true;
	      } else {
		  log ("VTransientServices::parseNotificationTemplate --> Error: cannot insert NS entry '%s'", pNSEntry->name ().content ());
		  delete pNSEntry;
		  pNSEntry = NULL;
		  xState = ParseState_Error;
	      }
	  }
	  else {
	    log ("VTransientServices::parseNotificationTemplate --> Error: found '%s' when expecting 'End'", pToken);
	    xState = ParseState_Error;
	  }
	  break;
	case ParseState_ExpectingNotificationName:
	  pNSEntry->setName (pToken);
	  break;
	case ParseState_ExpectingNotificationServerName:
	  pNSEntry->setServer (pToken);
	  break;
	default:
	  log ("VTransientServices::parseNotificationTemplate --> Error: unknown parse state %d[%s]", xState, pToken);
	  xState = ParseState_Error;
	  break;
	}
        //  set the break set to use for next parsing iteration
	switch (xState) {
	case ParseState_ExpectingTag:
	  pBreakSet = g_pWhitespaceBreakSet;
	  break;
	default:
	  pBreakSet = g_pRestOfLineBreakSet;
	  break;
	}
      }      
      iBuffer.clear ();
    } 
    return bCompleted;
}

bool VTransientServices::parsePlanTemplate (VSimpleFile &rSessionsFile, VTransientServices::PlanEntry *pEntry) {

    int xId;
    bool bCompleted = false;
    bool bEventComment = false;
    VString iPlanName;
    VString iBuffer;
    VString iEmail;
    VString iPage;
    VString iLog;
    VString iMonster;
    VString iRPD;
    VString iPre;
    VString iPost;

    while (!bCompleted && rSessionsFile.GetLine (iBuffer)) {
      bEventComment = false;
      ParseState xState = ParseState_ExpectingTag;
      char const *pBreakSet = g_pWhitespaceBreakSet;
      char *pBufferPtr;
      for (
	   char *pToken = strtok_r (iBuffer.storage (), pBreakSet, &pBufferPtr);
	   pToken && (!bEventComment) && ParseState_Error != xState;
	   pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	   ) {
	pToken += strspn (pToken, g_pWhitespaceBreakSet);
	VString iCookedToken;
	iCookedToken.setTo (pToken);
	switch (xState) {
	case ParseState_ExpectingTag:
	  xState = 0 == strcasecmp (pToken, "Name")
            ? ParseState_ExpectingPlanName
	    : 0 == strcasecmp (pToken, "Event")
	    ? ParseState_ExpectingEventId
	    : 0 == strcasecmp (pToken, "Action")
	    ? ParseState_ExpectingActionTag
	    : 0 == strcasecmp (pToken, "Plan_Template")
	    ? ParseState_ExpectingPlanSentinel
	    : ParseState_Error;  
	  if (ParseState_Error == xState)
	      log ("VTransientServices::parsePlanTemplate --> Unknown tag '%s'", pToken);
	  break;
	case ParseState_ExpectingPlanSentinel:
	  if (strcmp (pToken, "End") == 0) {
	    bCompleted = true;
          }
          else {
	    log ("VTransientServices::parsePlanTemplate --> Error: found '%s' when expecting 'End'", pToken);
	    xState = ParseState_Error;
	  }
	  break;
	case ParseState_ExpectingPlanName:
	  pEntry->setName (pToken);
	  break;
	case ParseState_ExpectingEventId:
	    if (1 == sscanf (iCookedToken, " %d", &xId)) {
		if (xId >= 0 && xId <= 1000)
		    pEntry->updateEvents (xId);
	    } else if (iCookedToken.getPrefix ('#', iPre, iPost)) {
		bEventComment = true;
	    } else {
		bool bValid = false;
		for (int i = 0; i < 24; i++) {
		    if (iCookedToken.equalsIgnoreCase (g_iEventNames[i])) {
			pEntry->updateEvents (i+1);
			bValid = true;
			break;
		    }
		}
		if (!bValid) {
		    log ("VTransientServices::parsePlanTemplate --> Error: found invalid mnemonic '%s'", static_cast <char const *> (iCookedToken.content ()));
		    xState = ParseState_Error;
		}
	    }
	    break;
	case ParseState_ExpectingActionTag:
	    xState
		= 0 == strcasecmp (iCookedToken, "email")
		? ParseState_ExpectingEmail
		
		: 0 == strcasecmp (iCookedToken, "page")
		? ParseState_ExpectingPage

		: 0 == strcasecmp (iCookedToken, "log")
		? ParseState_ExpectingLogPath

		: 0 == strcasecmp (iCookedToken, "monster")
		? ParseState_ExpectingMonster

		: 0 == strcasecmp (iCookedToken, "rpd")
		? ParseState_ExpectingRPD
		    
		: ParseState_Error;
		
	    if (ParseState_Error == xState)
		log ("VTransientServices::parsePlanTemplate --> Unknown action tag '%s'", static_cast <char const *> (iCookedToken.content ()));
	    break;
	case ParseState_ExpectingLogPath:
	    iLog.setTo (iCookedToken);
	    pEntry->updateActions (0, iLog);
	    break;
	case ParseState_ExpectingEmail:
	    iEmail.setTo (iCookedToken);
	    pEntry->updateActions (1, iEmail);
	    break;
	case ParseState_ExpectingPage:
	    iPage.setTo (iCookedToken);
	    pEntry->updateActions (2, iPage);
	    break;
	case ParseState_ExpectingMonster:
	    iMonster.setTo (iCookedToken);
	    pEntry->updateActions (3, iMonster);
	    break;
	case ParseState_ExpectingRPD:
	    iRPD.setTo (iCookedToken);
	    pEntry->updateActions (4, iRPD);
	    break;
	default:
	  log ("VTransientServices::parsePlanTemplate --> Error: unknown parse state %d[%s]", xState, pToken);
	  xState = ParseState_Error;
	  break;
	}
        //  set the break set to use for next parsing iteration
	switch (xState) {
	case ParseState_ExpectingTag:
	case ParseState_ExpectingActionTag:
	case ParseState_ExpectingLogPath:
	case ParseState_ExpectingEmail:
	case ParseState_ExpectingPage:
	case ParseState_ExpectingMonster:
	case ParseState_ExpectingRPD:
	case ParseState_ExpectingEventId:
	  pBreakSet = g_pWhitespaceBreakSet;
	  break;
	default:
	  pBreakSet = g_pRestOfLineBreakSet;
	  break;
	}
      }      
      iBuffer.clear ();
    } 
    return bCompleted;
}
bool VTransientServices::getNSEntry (VString const &rName, NSEntry::Reference &rEntry) const {
    unsigned int xEntry;
    if (m_iNSEntries.Locate (rName, xEntry)){
	rEntry.setTo (m_iNSEntries[xEntry]);
	return true;
    } else
	return false;
}

bool VTransientServices::NSEntry::insertEntry (VTransientServices::PlanEntry *pEntry) {
    unsigned int xIdx;
    if (m_iPlanEntries.Insert (pEntry->name (), xIdx)) {
	m_iPlanEntries[xIdx] = pEntry;
	return true;
    } else
	return false;
}
void VTransientServices::NSEntry::setName (VString const &rName) {
    m_iName.setTo (rName);
}
void VTransientServices::NSEntry::setServer (VString const &rServer) {
    m_iInfoServer.setTo (rServer);
}
void VTransientServices::NSEntry::getActionInfos (int xEvent, ActionInfos_t *pActionInfos) {
    PlanIterator iterator (m_iPlanEntries);
    while (iterator.isNotAtEnd ()) {
        VString iPlan = iterator.key ();
        unsigned int xPlan;
	if (m_iPlanEntries.Locate (iPlan, xPlan))
	    m_iPlanEntries [xPlan]->getActionInfos (xEvent, pActionInfos);
	++iterator;
    }	    
}

bool VTransientServices::PlanEntry::updateEvents (int xEvent) {
    return m_iEvents.Insert (xEvent);
}
bool VTransientServices::PlanEntry::updateActions (int xIdx, VString const &rAction) {
    return m_iActionInfos[xIdx].Insert (rAction);
}
void VTransientServices::PlanEntry::setName (VString const &rName) {
    m_iPlanName.setTo (rName);
}
void VTransientServices::PlanEntry::getActionInfos (int xEvent, ActionInfos_t *pActionInfos) {
    if (m_iEvents.Contains (xEvent)) {
	for (int xType = 0; xType < 5; xType++) {
	    for (int xElement = 0; xElement < m_iActionInfos[xType].cardinality (); xElement++) {
		pActionInfos[xType].Insert (m_iActionInfos[xType][xElement]);
	    }
	}
    }
}

	


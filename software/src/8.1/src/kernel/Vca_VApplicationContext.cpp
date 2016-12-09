/*****  Vca_VApplicationContext Implementation  *****/

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

#include "Vca_VApplicationContext.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBS.h"
#include "Vca_VcaPeer.h"
#include "Vca_VcaSelf.h"
#include "Vca_VcaOIDR.h"
#include "Vca_VcaTransport.h"
#include "Vca_VSSHDeviceImplementation.h"
#include "VTransientServices.h"


/**************************************
 **************************************
 *****                            *****
 *****  Vca::VApplicationContext  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VApplicationContext::VApplicationContext (
    int argc, argv_t argv, ThisClass *pParent
) : m_iCommandLine (argc, argv), m_pParent (pParent), m_xExitStatus (NormalExitValue) {
    if (m_pParent.isNil ()) {
    //  Lifetime Management Policy
	VcaSelf::Reference const pSelf (self());
	pSelf->setKeepalivesManageShutdown ();

    //  Export Management Policy
	if (commandSwitchValue ("keepalivesExcludeExports", "VcaKeepalivesExcludeExports"))
	    pSelf->clearKeepalivesIncludeExports ();
	else if (commandSwitchValue ("keepalivesIncludeExports", "VcaKeepalivesIncludeExports"))
	    pSelf->setKeepalivesIncludeExports ();
	else if (!pParent)
	    pSelf->clearKeepalivesIncludeExports ();

    //  Transport Buffering and Policy
	size_t iOptionValue;
	if (getCommandValue (iOptionValue, "vcaBufferSize", "VcaBufferSize"))
	    VBS::setGlobalBufferSizeTo (iOptionValue);
	if (getCommandValue (iOptionValue, "vcaProxyTransferSize", "VcaProxyTransferSize"))
	    VBS::setGlobalProxyTransferSizeTo (iOptionValue);

	VcaPeer::setGlobalTransportPolicyTo (
	    commandSwitchValue ("vcaFifoTransportPolicy", "VcaFifoTransportPolicy")
	    ? VcaPeer::FIFO : VcaPeer::LIFO
	);

    // TraceObjects
	bool bTraceObjects = commandSwitchValue ("traceObjects", "TraceObjects");
	VTransient::setObjectsTrace (bTraceObjects);


    //  TraceProxyGC
	bool bTraceGC = commandSwitchValue ("traceProxyGC", "TraceProxyGC");
	VcaOIDR::setGCTrace (bTraceGC);


    //  TraceTransports
	bool bTraceTransports = commandSwitchValue ("traceTransports", "TraceTransports");
	VcaTransport::setTracingAllTransfersTo (bTraceTransports);
          
#ifdef HAS_LIBSSH2
    //  TraceSSH
	bool bTraceSSH = commandSwitchValue ("traceSSH", "TraceSSH");
	VSSHDeviceImplementation::setTracingTo (bTraceSSH);
#endif

	VTransientServices *pTSP = transientServicesProvider ();

    //  Logging Information
	bool bLogging = pTSP->logSwitch () || commandSwitchValue ("logSwitchOn", "LogSwitchOn");
	    pTSP->updateLogSwitch (bLogging);

        char const *camPath = commandStringValue ("camLogPath", "VisionCamLogPath");
        pTSP->updateCamLogPath (camPath);
 
	char const *pPath = commandStringValue ("logFilePath", "LogFilePath");
	if (pPath) 
	    pTSP->updateLogFilePath (pPath);
        
	char const *pSize = commandStringValue ("logFileSize", "LogFileSize");
	if (pSize) 
	    pTSP->updateLogFileSize (atoi (pSize));
       
	char const *pBackups = commandStringValue ("logFileBackups", "LogFileBackups");
	if (pBackups) 
	    pTSP->updateLogFileBackups (atoi (pBackups));

        char const *pInfoServer = commandStringValue ("infoServer", "InfoServer");
        if (pInfoServer)
            pTSP->updateInfoServerEntry (pInfoServer);

        char const *pInfoLogger = commandStringValue ("infoLogger", "InfoLogger");
        if (pInfoLogger)
            pTSP->updateInfoLogger (pInfoLogger);

        char const *pInfoSubscriber = commandStringValue ("infoSubscriber", "InfoSubscriber");
        if (pInfoSubscriber)
            pTSP->updateInfoSubscriber (pInfoSubscriber);
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VApplicationContext::~VApplicationContext () {
}


/*********************************
 *********************************
 *****  Command Line Access  *****
 *********************************
 *********************************/

Vca::VApplicationContext::CommandLine const &Vca::VApplicationContext::commandLine () const {
    return m_pParent ? m_pParent->commandLine () : m_iCommandLine;
}

char const *Vca::VApplicationContext::commandStringValue (	// -xyz=StringValue
    char const *pTag, char const *pVariable, char const *pDefaultValue
) const {
//  If we have no parent, ...
    if (m_pParent.isNil ())
    //  ... return the command, environment, or default value, whichever comes first:
	return m_iCommandLine.stringValue (pTag, pVariable, pDefaultValue);

//  If we have a parent, just check our command line, ...
    char const *const pResult = m_iCommandLine.stringValue (pTag);

//  ... leaving the rest to our parent (if our command line doesn't supply a value):
    return pResult ? pResult : m_pParent->commandStringValue (pTag, pVariable, pDefaultValue);
}

bool Vca::VApplicationContext::commandSwitchValue (		// -xyz
    char const *pTag, char const *pVariable
) const {
//  With no parent ...
    return m_pParent.isNil ()
    // ... return the command or environment value, whichever comes first:
	? m_iCommandLine.switchValue (pTag, pVariable)
    // ... otherwise, check just our command line, leaving the rest to our parent:
	: m_iCommandLine.switchValue (pTag) || m_pParent->commandSwitchValue (pTag, pVariable);
}

bool Vca::VApplicationContext::getCommandValue (		// -xyz=size
    size_t &rValue, char const *pTag, char const *pVariable
) const {
//  With no parent ...
    return m_pParent.isNil ()
    // ... return the command or environment value, whichever comes first:
	? m_iCommandLine.getValue (rValue, pTag, pVariable)
    // ... otherwise, check just our command line, leaving the rest to our parent:
	: m_iCommandLine.getValue (rValue, pTag) || m_pParent->getCommandValue (rValue, pTag, pVariable);
}


/************************************
 ************************************
 *****  Application Management  *****
 ************************************
 ************************************/

void Vca::VApplicationContext::incrementApplicationCount () {
    if (0 == m_cApplications++) {
	if (m_pParent)
	    m_pParent->incrementApplicationCount ();
	else
	    self()->incrementKeepaliveCount ();
    }
}
void Vca::VApplicationContext::decrementApplicationCount () {
    if (m_cApplications.decrementIsZero ()) {
	if (m_pParent)
	    m_pParent->decrementApplicationCount ();
	else
	    self ()->decrementKeepaliveCount ();
    }
}

void Vca::VApplicationContext::setExitStatus (int xExitStatus) {
    m_xExitStatus = xExitStatus;
    if (m_pParent)
	m_pParent->setExitStatus (xExitStatus);
}

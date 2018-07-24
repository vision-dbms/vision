/*****  Vision Connection Architecture Query  *****/

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

#include "Vca_VcaQuery.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VMainProgram.h"

#include "Vca_IDirectory.h"
#include "Vca_IRoleProvider.h"

#include "Vsa_IEvaluator.h"
#include "Vsa_IPoolEvaluation.h"


/****************************
 ****************************
 *****                  *****
 *****  Vsa::VQueryApp  *****
 *****                  *****
 ****************************
 ****************************/

/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

V::VString const &Vsa::VQueryApp::Environment (argv_t ppEnvironment) {
    static bool bNotInitialized = true;
    static VString g_iString;

    if (bNotInitialized) {
	g_iString.clear ();
	g_iString.guarantee (65535);

	bool bNotFirst = false;
	for (argv_t ppBinding = ppEnvironment; *ppBinding; ppBinding++) {
	    if (bNotFirst) g_iString.append ("\n");
            else bNotFirst = true;
	    g_iString.append (*ppBinding);
	}
	g_iString.trim ();
	bNotInitialized = false;
    }
    return g_iString; 
}

char const *Vsa::VQueryApp::EnvironmentString (
    char const *pVariableName, char const *pDefaultValue
)  {
    char const *pVariableValue = getenv (pVariableName);
    return pVariableValue ? pVariableValue : pDefaultValue;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VQueryApp::VQueryApp (Context *pContext, argv_t envp)
: BaseClass		(pContext)
, m_iPathInfo		(getPathInfo ())
, m_iQueryString	(getQueryString ()) 
, m_iEnvironment	(Environment (envp))
, m_iCommandCursor	(this)
, m_bUsingAnyData       (commandSwitchValue ("anyData"))
{
    if (char const *const pBridgeServerName = commandStringValue ("bridgeServer", "FASTBridgeServer"))
	aggregate (new Vca::Gofer::Named<Vca::IRoleProvider,Vca::IDirectory>(pBridgeServerName));
}

V::VString Vsa::VQueryApp::getPathInfo () {
    char const *pArgument = m_iCommandCursor.firstToken ();
    VString iResult (EnvironmentString ("PATH_INFO", pArgument ? pArgument : "/"));
    return iResult;
}

V::VString Vsa::VQueryApp::getQueryString () {
    if (isGET ()) {
	char const *pArgument = m_iCommandCursor.nextToken ();
	VString iResult (
	    EnvironmentString ("QUERY_STRING", pArgument ? pArgument : "")
	);
	return iResult;
    }
    else {
	VString iResult (getInput ());
	return iResult;
    }
}

V::VString const &Vsa::VQueryApp::getInput () {
    static bool bNotInitialized = true;
    static VString g_iString;

    if (bNotInitialized) {
	size_t usize = 0;
	size_t asize = 4096;
	char *buffer = g_iString.guarantee (asize);

	size_t bcount;
	while (bcount = read (fileno (stdin), buffer + usize, asize - usize - 1)) {
	    if ((size_t)-1 == bcount)
		break;

	    usize += bcount;
	    if (asize <= usize + 1) {
		asize += 4096;
		buffer = g_iString.guarantee (asize);
	    }
	}
	buffer[usize] = '\0';

	g_iString.trim ();

	bNotInitialized = false;
    }

    return g_iString;    
}


/*********************
 *********************
 *****  Output   *****
 *********************
 *********************/

unsigned int Vsa::VQueryApp::wrighte (
    int fd, char const *pData, unsigned int sData
) const {
    int sTransfer;
    while (sData > 0 && (sTransfer = ::write (fd, pData, sData)) > 0) {
	pData += sTransfer;
	sData -= sTransfer;
    }
    return sData;
}

void Vsa::VQueryApp::report (char const *pMessage) const {
    printf ("Content-Type: text/html\n\n");
    printf ("<HTML>\n");
    printf ("<HEAD> <TITLE> VQuery Error </TITLE> </HEAD>\n");
    printf ("<BODY>\n");
    printf ("<H2> VQuery Error </H2>\n");
    printf ("<HR>\n");
    // printf ("<STRONG>\n");
    printf ("Your request to access %s", m_iPathInfo.content ());
    // printf (strlen (m_iServiceName) > 0 ? " the '%s'" : " a", m_iServiceName.content ());
    printf (" Vision server failed.\n%s\n", pMessage);
    // printf ("</STRONG>\n");
    printf ("<HR>\n");
    printf ("</BODY>\n");
    printf ("</HTML>\n");
}


/*******************************
 *******************************
 *****  Interface Methods  *****
 *******************************
 *******************************/

/****************************************************************************
 * Routine: OnResult
 * Operation:
 *	On getting the result from the service manager, flushes the result to
 * the client. Then terminates the connections created so that this (CGI) 
 * process exists 
 ****************************************************************************/

void Vsa::VQueryApp::OnAccept_(IEvaluation *pEvaluation, Vca::U32 xQueuePosition) {
    IPoolEvaluation::Reference const pPoolEvaluation (dynamic_cast <IPoolEvaluation*> (pEvaluation));
    if (pPoolEvaluation && m_bUsingAnyData) {
	pPoolEvaluation->UseAnyGenerationWorker ();
    }
}

void Vsa::VQueryApp::OnResult_(IEvaluationResult *pResult, VString const &rResult) {
    fflush (stdout);
    wrighte (fileno (stdout), rResult.content (), rResult.length ());

    stop ();
}

void Vsa::VQueryApp::OnError_(Vca::IError *pError, VString const& rErrorMessage) {
    report (rErrorMessage);
    VEvaluatorClient::OnError_(pError, rErrorMessage);
    stop ();
}


/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

/****************************************************************************
 * Routine: onEvaluator
 * Operation:
 *	On receiving the Evaluator interface from the ServiceManager (SM)
 * invokes the EvaluateURL interface method on it to retrieve the results
 ****************************************************************************/

void Vsa::VQueryApp::onEvaluator (IEvaluator *pEvaluator) {
    if (!pEvaluator) {
	report ("Evaluation Not Supported By Server");	
	stop ();
    } else if (m_iPathInfo != "/-") {
	onQuery (pEvaluator, m_iPathInfo, m_iQueryString, m_iEnvironment);
    } else {
	onQuery (pEvaluator, m_iPathInfo, m_iQueryString);
    }
}

void Vsa::VQueryApp::onConnectionError (Vca::IError *pInterface, VString const &rMessage) {
    VString iError;
    iError << "Connection Error: " << rMessage;
    report (iError);
    stop ();
}

/****************************************************************************
 * Routine: start  (SM - ServiceManager)
 * Operation:
 *	This routines connects to the ServiceManager listening at rNetworkName
 * and queries it for Evaluator Interface. 
 ****************************************************************************/

bool Vsa::VQueryApp::start_() {
    if (!BaseClass::start_())
	return false;

    if (!getServerInterface (this, &ThisClass::onEvaluator, &ThisClass::onConnectionError)) {
	report ("No Server To Contact");
    }
    return isStarting ();
}

bool Vsa::VQueryApp::getDefaultServerName (VString &rServerName) const {
    CommandCursor iCommandCursor (this);
    rServerName.setTo (iCommandCursor.firstToken ());

    return rServerName.isntEmpty ();
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[], char *envp[]) {
    Vca::VMainProgram iMain (argc, argv);
    if (!iMain.runnable ())
	return iMain.exitStatus ();

    Vsa::VQueryApp::Reference pApp (new Vsa::VQueryApp (iMain, envp));
    pApp->start ();

    return iMain.processEvents ();
}

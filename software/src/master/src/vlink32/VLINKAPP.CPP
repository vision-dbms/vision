//////////////////////////////////////////////////////////////////////////////
//
// CApplication Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"
#include "resource.h"

#include <afxpriv.h>
#include <ctype.h>
#include <direct.h>

//  Supporting...
#include "aboutdlg.h"

#include "ipframe.h"
#include "mainfrm.h"

#include "vlinkdoc.h"
#include "vlinkvw.h"
#include "vlinkmdi.h"

#include "Agent.h"
#include "Connection.h"
#include "ConnectionTemplate.h"
#include "ForwardingService.h"
#include "Request.h"
#include "TextBuffer.h"

#include "Vca.h"

//  Self...
#include "vlinkapp.h"


/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

const DWORD	CApplication::g_iIdleThreshold	= 1024;
const DWORD	CApplication::g_iUIDBase	= CoGetCurrentProcess();
DWORD		CApplication::g_iUIDSequence 	= 0;

/////////////////////////////////////////////////////////////////////////////
//  The One and Only CApplication object
/////////////////////////////////////////////////////////////////////////////

CApplication theApp;


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CApplication::Dump (CDumpContext& dc) const
{
    CCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CApplication, CWinApp)
    //{{AFX_MSG_MAP(CApplication)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CApplication, CWinApp)
    //{{AFX_DISPATCH_MAP(CApplication)
    DISP_PROPERTY_EX(CApplication, "Top", GetTop, SetTop, VT_R4)
    DISP_PROPERTY_EX(CApplication, "Left", GetLeft, SetLeft, VT_R4)
    DISP_PROPERTY_EX(CApplication, "Width", GetWidth, SetWidth, VT_R4)
    DISP_PROPERTY_EX(CApplication, "Height", GetHeight, SetHeight, VT_R4)
    DISP_PROPERTY_EX(CApplication, "Visible", GetVisible, SetVisible, VT_BOOL)
    DISP_PROPERTY_EX(CApplication, "DefaultFilePath", OAGetDefaultFilePath, OASetDefaultFilePath, VT_BSTR)
    DISP_FUNCTION(CApplication, "ClassName", OAClassName, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CApplication, "Name", OAName, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CApplication, "UID", OAUID, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CApplication, "Application", OAApplication, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "Parent", OAParent, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "FullName", OAFullName, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CApplication, "Path", OAPath, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CApplication, "Agents", OAAgents, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "Connections", OAConnections, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "ConnectionTemplates", OAConnectionTemplates, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "Requests", OARequests, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "ShowMainWindow", OAShowMainWindow, VT_BOOL, VTS_I2)
    DISP_FUNCTION(CApplication, "CreateConnection", OACreateConnection, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I2)
    DISP_FUNCTION(CApplication, "CreateStoredConnection", OACreateStoredConnection, VT_DISPATCH, VTS_BSTR)
    DISP_FUNCTION(CApplication, "GetConnection", OAGetConnection, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I2)
    DISP_FUNCTION(CApplication, "GetStoredConnection", OAGetStoredConnection, VT_DISPATCH, VTS_BSTR)
    DISP_FUNCTION(CApplication, "DefaultConnectTimeout", OADefaultConnectTimeout, VT_I4, VTS_NONE)
    DISP_FUNCTION(CApplication, "DefaultRequestTimeout", OADefaultRequestTimeout, VT_I4, VTS_NONE)
    DISP_FUNCTION(CApplication, "CreateDefaultConnection", OACreateDefaultConnection, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "GetDefaultConnection", OAGetDefaultConnection, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "CreateAgent", OACreateAgent, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "SavedTemplateNames", OASavedTemplateNames, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CApplication, "GetProfileLong", OAGetProfileLong, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION(CApplication, "GetProfileShort", OAGetProfileShort, VT_I2, VTS_BSTR VTS_BSTR VTS_I2)
    DISP_FUNCTION(CApplication, "GetProfileString", OAGetProfileString, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR)
    DISP_FUNCTION(CApplication, "WriteProfileLong", OAWriteProfileLong, VT_BOOL, VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION(CApplication, "WriteProfileShort", OAWriteProfileShort, VT_BOOL, VTS_BSTR VTS_BSTR VTS_I2)
    DISP_FUNCTION(CApplication, "WriteProfileString", OAWriteProfileString, VT_BOOL, VTS_BSTR VTS_BSTR VTS_BSTR)
    DISP_FUNCTION(CApplication, "QuoteString", OAQuoteString, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION(CApplication, "QuoteSymbol", OAQuoteSymbol, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION(CApplication, "TextLimit", OATextLimit, VT_I4, VTS_NONE)
    DISP_FUNCTION(CApplication, "RemoveNamedObject", OARemoveNamedObject, VT_EMPTY, VTS_BSTR)
    DISP_PROPERTY_PARAM(CApplication, "NamedObject", OAGetNamedObject, OASetNamedObject, VT_DISPATCH, VTS_BSTR)
    DISP_DEFVALUE(CApplication, "Name")
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CApplication, CWinApp)

CApplication::CApplication()
: m_guiInitialized	(FALSE)
, m_iIdleThreshold	(g_iIdleThreshold)
, m_pForwardingService	(0)
{
    EnableAutomation();
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CApplication::~CApplication()
{
    while (!m_templateServers.IsEmpty ())
	delete m_templateServers.RemoveHead ();

    CString iKey;
    union {
	LPVOID		pAny;
	LPDISPATCH	pObject;
    };
    POSITION position = m_iNamedObjectMap.GetStartPosition ();
    while (position)
    {
	m_iNamedObjectMap.GetNextAssoc (position, iKey, pAny);
	if (pObject)
	    pObject->Release ();
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Access
/////////////////////////////////////////////////////////////////////////////

void CApplication::GetClassName (CString& s) const
{
    s += "VLink.";
    s += GetRuntimeClass()->m_lpszClassName + 1;
}

void CApplication::GetUID (CString& s) const
{
    GetClassName (s);
    s += ".";
    s += CApplication::g_iUIDBase;
    s += ".";
    s += (long)0;
}

CConnection* CApplication::LookupConnection (LPCSTR pConnectionId) const
{
    return m_pForwardingService ? m_pForwardingService->Get (pConnectionId) : NULL;
}


/////////////////////////////////////////////////////////////////////////////
//  Update
/////////////////////////////////////////////////////////////////////////////

void CApplication::CreateOLEDocumentTemplate (
    REFCLSID		clsid,
    CRuntimeClass *	pDocClass,
    CRuntimeClass *	pViewClass,
    CRuntimeClass *	pFrameClass,
    CRuntimeClass *	pIPFrameClass,
    UINT		nIDResource,
    UINT		nIDSrvrIPFrame,
    UINT		nIDSrvrEmbedded,
    UINT		nIDCntrIP
)
{
    CMultiDocTemplate* pDocTemplate = new CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass);
    pDocTemplate->SetContainerInfo(nIDCntrIP);
    pDocTemplate->SetServerInfo(nIDSrvrEmbedded, nIDSrvrIPFrame, pIPFrameClass);
    AddDocTemplate(pDocTemplate);

    COleTemplateServer* pTemplateServer = new COleTemplateServer ();
    pTemplateServer->ConnectTemplate(clsid, pDocTemplate, FALSE);
    pTemplateServer->UpdateRegistry(OAT_INPLACE_SERVER);
    m_templateServers.AddTail (pTemplateServer);
}

void CApplication::Detach (CForwardingService* pForwardingService)
{
    if (m_pForwardingService == pForwardingService)
	m_pForwardingService = NULL;
}

void CApplication::Register (CConnection* pConnection)
{
    if (pConnection->ForwardingId().IsEmpty ())
	return;

    if (m_pForwardingService)
	m_pForwardingService->Attach (pConnection);
    else
    {
	unsigned int xServicePort = AfxGetApp()->GetProfileInt (
	    "Forwarding Service", "Port", 0
	);
	if (xServicePort > 0)
	{
	    m_pForwardingService = new CForwardingService (xServicePort);
	    m_pForwardingService->Attach  (pConnection);
	    m_pForwardingService->Release ();
	}
    }
}

BOOL CApplication::ShowMainWindow(int showCmd)
{
    BOOL result = m_pMainWnd->ShowWindow(showCmd /* m_nCmdShow */);;
    if (!m_guiInitialized)
    {
	m_pMainWnd->UpdateWindow();
	m_guiInitialized = TRUE;
    }
    return result;
}


/////////////////////////////////////////////////////////////////////////////
//  Initialization Override
/////////////////////////////////////////////////////////////////////////////

BOOL CApplication::InitInstance()
{
// Initialize the OLE 2.0 libraries...
    if (!AfxOleInit())
    {
	AfxMessageBox(IDP_OLE_INIT_FAILED);
	return FALSE;
    }

//  Load standard profile settings, ...
    LoadStdProfileSettings	();

//  Create main MDI Frame window ...
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	return FALSE;
    m_pMainWnd = pMainFrame;

//  Create the WINSOCK event notification window...
    if (!m_iNetwork.Create (
	    NULL,
	    "WinsockNotificationWindow",
	    WS_CHILD, 
	    CRect (0, 0, 10, 10),
	    m_pMainWnd,
	    IDC_WinsockNotificationWindow
	)
    )
    {
	AfxMessageBox(m_iNetwork.ErrorMessage ());
	return FALSE;
    }

//  Register document templates and connect them to the OLE template server...
    CreateOLEDocumentTemplate (
	CVLinkDocument::guid,
	RUNTIME_CLASS(CVLinkDocument),
	RUNTIME_CLASS(CVLinkView),
	RUNTIME_CLASS(CVLinkMDIChild),
	RUNTIME_CLASS(CInPlaceFrame),
	IDR_VLINKTYPE,
	IDR_VLINKTYPE_SRVR_IP,
	IDR_VLINKTYPE_SRVR_EMB,
	IDR_VLINKTYPE_CNTR_IP
    );

//  Register all OLE server factories as running.  This enables the
//		OLE 2.0 libraries to create objects from other applications.
//		Note: MDI applications register all server objects without regard
//		to the /Embedding or /Automation on the command line.
    COleTemplateServer::RegisterAll();

//  Enable file manager drag/drop and DDE Execute open...
    m_pMainWnd->DragAcceptFiles();
    EnableShellOpen();
    RegisterShellFileTypes();

//  Exit if the application was launched as an OLE server...
    if (RunEmbedded() || RunAutomated())
	return TRUE;

//  Update the system registry in case it has been damaged...
    //	m_server.UpdateRegistry(OAT_INPLACE_SERVER);
    COleObjectFactory::UpdateRegistryAll();

//	Open or create initial documents...
    if (m_lpCmdLine[0] == '\0') OnFileNew ();
    else OpenDocumentFile(m_lpCmdLine);

//  Show the main window...
    ShowMainWindow (m_nCmdShow);

//  And return successfully...
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//  Shutdown Override
/////////////////////////////////////////////////////////////////////////////

int CApplication::ExitInstance (void)
{
#ifdef _DEBUG
    afxDump << "Exiting Application Instance\n";
#endif

    return CWinApp::ExitInstance ();
}


/////////////////////////////////////////////////////////////////////////////
//  OnIdle Override
/////////////////////////////////////////////////////////////////////////////

BOOL CApplication::OnIdle (long idleSlot)
{
/*---------------------------------------------------------------------*
 *  This is probably not the right place to put this, but it at least
 *  demonstrates that we can make Vca calls from VLink...
 *---------------------------------------------------------------------*/
// Process Vca events...
//    Vca::DoEvents ();

// Delay idle processing until 'm_iIdleThreshold' messages have been processed:
//    if (m_iIdleThreshold-- > 0)
//	return TRUE;

// Once idle processing begins, pass all idle slots to the framework:
    if (CWinApp::OnIdle (idleSlot)) {
//	m_iIdleThreshold = 0;
	return TRUE;
    }

// When no more idle slots are requested, delay the next idle loop until
// 'g_iIdleThreshold' messages have been processed:
//    m_iIdleThreshold = g_iIdleThreshold;
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

void CApplication::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

BSTR CApplication::OAClassName()
{
    CString s;
    GetClassName (s);
    return s.AllocSysString();
}

BSTR CApplication::OAName()
{
    USES_CONVERSION;
    return ::SysAllocString(T2COLE(m_pszAppName));
}

BSTR CApplication::OAUID()
{
    CString s;
    GetUID (s);
    return s.AllocSysString();
}

LPDISPATCH CApplication::OAApplication()
{
    return GetIDispatch(TRUE);
}

LPDISPATCH CApplication::OAParent()
{
    return GetIDispatch(TRUE);
}

BSTR CApplication::OAFullName()
{
    char moduleFileName[_MAX_PATH];
    moduleFileName[GetModuleFileName (m_hInstance, moduleFileName, sizeof(moduleFileName))] = '\0';

    USES_CONVERSION;
    return ::SysAllocString(T2COLE(moduleFileName));
}

BSTR CApplication::OAPath()
{
    char moduleFileName[_MAX_PATH], *pFinalPathSeparator;
    moduleFileName[GetModuleFileName (m_hInstance, moduleFileName, sizeof(moduleFileName))] = '\0';
    pFinalPathSeparator = strrchr (moduleFileName, '\\');
    if (pFinalPathSeparator)
	pFinalPathSeparator[0] = '\0';
    USES_CONVERSION;
    return ::SysAllocString(T2COLE(moduleFileName));
}

BSTR CApplication::OAGetDefaultFilePath()
{
    char defaultFilePath[_MAX_PATH];
    if (NULL == _getcwd (defaultFilePath, sizeof(defaultFilePath)))
	    defaultFilePath[0] = '\0';
    USES_CONVERSION;
    return ::SysAllocString (T2COLE(defaultFilePath));
}

void CApplication::OASetDefaultFilePath(LPCSTR lpszNewValue)
{
    if (0 == _chdir (lpszNewValue)
	&&  2  <= strlen(lpszNewValue)
	&& ':' == lpszNewValue[1]
	&& 'A' <= toupper(lpszNewValue[0])
	&& 'Z' >= toupper(lpszNewValue[0])
    ) _chdrive (toupper(lpszNewValue[0]) - 'A' + 1);
}

float CApplication::GetTop()
{
    RECT windowRectangle;
    m_pMainWnd->GetWindowRect (&windowRectangle);
    return (float)(windowRectangle.top);
}

void CApplication::SetTop(float iNewValue)
{
}

float CApplication::GetLeft()
{
    RECT windowRectangle;
    m_pMainWnd->GetWindowRect (&windowRectangle);
    return (float)(windowRectangle.left);
}

void CApplication::SetLeft(float iNewValue)
{
}

float CApplication::GetHeight()
{
    RECT windowRectangle;
    m_pMainWnd->GetWindowRect (&windowRectangle);
    return (float)(windowRectangle.bottom - windowRectangle.top);
}

void CApplication::SetHeight(float iNewValue)
{
}

float CApplication::GetWidth()
{
    RECT windowRectangle;
    m_pMainWnd->GetWindowRect (&windowRectangle);
    return (float)(windowRectangle.right - windowRectangle.left);
}

void CApplication::SetWidth(float iNewValue)
{
}

BOOL CApplication::GetVisible()
{
    WINDOWPLACEMENT windowPlacement;
    m_pMainWnd->GetWindowPlacement (&windowPlacement);

    return m_guiInitialized && SW_HIDE != windowPlacement.showCmd;
}

void CApplication::SetVisible(BOOL bNewValue)
{
}

LPDISPATCH CApplication::OAAgents()
{
    return CAgent::Instances.OACollection ();
}

LPDISPATCH CApplication::OAConnections()
{
    return CConnection::Instances.OACollection ();
}

LPDISPATCH CApplication::OAConnectionTemplates()
{
    return CConnectionTemplate::Instances.OACollection ();
}

LPDISPATCH CApplication::OARequests()
{
    return CRequest::Instances.OACollection ();
}

BOOL CApplication::OAShowMainWindow(short showCmd)
{
    return ShowMainWindow (showCmd);
}

LPDISPATCH CApplication::OACreateConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    CConnectionData iConnectionData (hostname, username, password, database, objectSpace);
    CConnection* pConnection = new CConnection (iConnectionData);
    return pConnection->GetIDispatch (FALSE);
}

LPDISPATCH CApplication::OACreateDefaultConnection()
{
    return OACreateStoredConnection (NULL);
}

LPDISPATCH CApplication::OACreateStoredConnection(LPCSTR storedTemplateName)
{
    CConnectionData connectionData;
    CConnection* pConnection = connectionData.Load (storedTemplateName)
	? new CConnection (connectionData)
	: NULL;
    return pConnection ? pConnection->GetIDispatch (FALSE) : NULL;
}

LPDISPATCH CApplication::OAGetConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    CConnection* pConnection = CConnection::GetConnection (hostname, username, password, database, objectSpace);
    return pConnection ? pConnection->GetIDispatch (FALSE) : NULL;
}

LPDISPATCH CApplication::OAGetDefaultConnection()
{
    return OAGetStoredConnection (NULL);
}

LPDISPATCH CApplication::OAGetStoredConnection(LPCSTR storedTemplateName)
{
    CConnectionData connectionData;
    CConnection* pConnection = connectionData.Load (storedTemplateName)
	? CConnection::GetConnection (connectionData)
	: NULL;
    return pConnection ? pConnection->GetIDispatch (FALSE) : NULL;
}

long CApplication::OADefaultConnectTimeout()
{
    return CConnection::g_defaultTimeout;
}

long CApplication::OADefaultRequestTimeout()
{
    return CRequest::g_defaultTimeout;
}

LPDISPATCH CApplication::OACreateAgent()
{
    CAgent* pAgent = new CAgent ();
    return pAgent->GetIDispatch(FALSE);
}

LPDISPATCH CApplication::OASavedTemplateNames()
{
    CTextBuffer* pTextBuffer = new CTextBuffer ();
    int templateIndex;
    int templateCount = CConnectionData::SavedTemplateCount();

    for (templateIndex = 0; templateIndex < templateCount; templateIndex++)
    {
	CString savedTemplateName = CConnectionData::SavedTemplateName (templateIndex);
	pTextBuffer->AppendLines (savedTemplateName);
	pTextBuffer->AppendLines ("\n");
    }

    pTextBuffer->Trim (FALSE);

    return pTextBuffer->GetIDispatch (FALSE);
}

long CApplication::OAGetProfileLong(LPCSTR sectionName, LPCSTR entryName, long defaultValue)
{
    CString defaultValueAsString;
    defaultValueAsString += defaultValue;

    long returnValue;
    return 1 == sscanf (GetProfileString (sectionName, entryName, defaultValueAsString), "%ld", &returnValue) ? returnValue : defaultValue;
}

short CApplication::OAGetProfileShort(LPCSTR sectionName, LPCSTR entryName, short defaultValue)
{
    return GetProfileInt (sectionName, entryName, defaultValue);
}

BSTR CApplication::OAGetProfileString(LPCSTR sectionName, LPCSTR entryName, LPCSTR defaultValue)
{
    return GetProfileString (sectionName, entryName, defaultValue).AllocSysString();
}

BOOL CApplication::OAWriteProfileLong(LPCSTR sectionName, LPCSTR entryName, long iNewValue)
{
    CString newValueAsString;
    newValueAsString += iNewValue;
    return WriteProfileString (sectionName, entryName, newValueAsString);
}

BOOL CApplication::OAWriteProfileShort(LPCSTR sectionName, LPCSTR entryName, short iNewValue)
{
    return WriteProfileInt (sectionName, entryName, iNewValue);
}

BOOL CApplication::OAWriteProfileString(LPCSTR sectionName, LPCSTR entryName, LPCSTR pNewValue)
{
    return WriteProfileString (sectionName, entryName, pNewValue);
}

BSTR CApplication::OAQuoteString(LPCSTR string)
{
    LPCSTR	cursor = string;
    CString result = "\"";

    while (*cursor)
    {
	CString substring (cursor, strcspn (cursor, "\\\""));
	result += substring;
	cursor += substring.GetLength ();
	if (*cursor)
	{
		result += '\\';
		result += *cursor++;
	}
    }
    result += "\"";

    return result.AllocSysString();
}

BSTR CApplication::OAQuoteSymbol(LPCSTR string)
{
    LPCSTR	cursor = string;
    CString result = *cursor && *cursor != '_' && (*cursor < 'a' || *cursor > 'z') && (*cursor < 'A' || *cursor > 'Z')
	? "\\"
	: *cursor
	? ""
	: "\\?";

    while (*cursor)
    {
	CString substring (cursor, strspn (cursor, "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	result += substring;
	cursor += substring.GetLength();
	if (*cursor)
	{
	    result += '\\';
	    result += *cursor++;
	}
    }

    return result.AllocSysString();
}

long CApplication::OATextLimit()
{
    return 0x7fffffff;
}

LPDISPATCH CApplication::OAGetNamedObject(LPCSTR pName)
{
    union {
	LPVOID		pAny;
	LPDISPATCH	pObject;
    };

    if (m_iNamedObjectMap.Lookup (pName, pAny))
	pObject->AddRef ();
    else pObject = NULL;

    return pObject;
}

void CApplication::OASetNamedObject(LPCSTR pName, LPDISPATCH pNewObject)
{
    union {
	LPVOID		pAny;
	LPDISPATCH	pOldObject;
    };

    if (!m_iNamedObjectMap.Lookup (pName, pAny))
	pOldObject = NULL;

    if (pOldObject == pNewObject)
	return;

    if (pNewObject)
	pNewObject->AddRef ();
    if (pOldObject)
	pOldObject->Release ();
    m_iNamedObjectMap[pName] = pNewObject;
}

void CApplication::OARemoveNamedObject(LPCSTR pName) 
{
    union {
	LPVOID		pAny;
	LPDISPATCH	pOldObject;
    };

    if (m_iNamedObjectMap.Lookup (pName, pAny))
    {
	m_iNamedObjectMap.RemoveKey (pName);
	if (pOldObject)
	    pOldObject->Release ();
    }
}

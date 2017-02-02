//////////////////////////////////////////////////////////////////////////////
//
//  CConnectionTemplate Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Agent.h"
#include "Connection.h"

//  Self...
#include "ConnectionTemplate.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

CCollectionKernel CConnectionTemplate::Instances;

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CConnectionTemplate::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConnectionTemplate, CVLinkCmdTarget)
    //{{AFX_MSG_MAP(CConnectionTemplate)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CConnectionTemplate, CVLinkCmdTarget)
    //{{AFX_DISPATCH_MAP(CConnectionTemplate)
    DISP_PROPERTY_EX(CConnectionTemplate, "Database", OAGetDatabase, OASetDatabase, VT_BSTR)
    DISP_PROPERTY_EX(CConnectionTemplate, "Executable", OAGetExecutable, OASetExecutable, VT_BSTR)
    DISP_PROPERTY_EX(CConnectionTemplate, "Hostname", OAGetHostname, OASetHostname, VT_BSTR)
    DISP_PROPERTY_EX(CConnectionTemplate, "ObjectSpace", OAGetObjectSpace, OASetObjectSpace, VT_I2)
    DISP_PROPERTY_EX(CConnectionTemplate, "Options", OAGetOptions, OASetOptions, VT_BSTR)
    DISP_PROPERTY_EX(CConnectionTemplate, "Password", OAGetPassword, OASetPassword, VT_BSTR)
    DISP_PROPERTY_EX(CConnectionTemplate, "Timeout", OAGetTimeout, OASetTimeout, VT_I4)
    DISP_PROPERTY_EX(CConnectionTemplate, "Username", OAGetUsername, OASetUsername, VT_BSTR)
    DISP_FUNCTION(CConnectionTemplate, "Command", OACommand, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "Description", OADescription, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "Agent", OAAgent, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "CreateConnection", OACreateConnection, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "GetConnection", OAGetConnection, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "LoadDefaultTemplate", OALoadDefaultTemplate, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "LoadTemplate", OALoadTemplate, VT_BOOL, VTS_BSTR)
    DISP_FUNCTION(CConnectionTemplate, "StoreDefaultTemplate", OAStoreDefaultTemplate, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnectionTemplate, "StoreTemplate", OAStoreTemplate, VT_BOOL, VTS_BSTR)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
//  OLE UUID Data
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CConnectionTemplate, CVLinkCmdTarget)

CConnectionTemplate::CConnectionTemplate(CAgent* pAgent) : m_pAgent(pAgent)
{
//  Reference the agent, ...
    m_pAgent->Retain ();

//  ... add this instance to the class instance list, ...
    Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CConnectionTemplate::~CConnectionTemplate()
{
//  Release the agent, ...
    m_pAgent->Release ();

//  ... and remove this instance from the class instance list...
    Instances.DeleteItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

CConnection *CConnectionTemplate::CreateConnection ()
{
    return m_pAgent->CreateConnection (m_templateData);
}

CConnection *CConnectionTemplate::GetConnection ()
{
    return m_pAgent->GetConnection (m_templateData);
}

/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

BSTR CConnectionTemplate::OAGetDatabase()
{
    return m_templateData.m_database.AllocSysString();
}

void CConnectionTemplate::OASetDatabase(LPCSTR lpszNewValue)
{
    m_templateData.m_database = lpszNewValue;
}

BSTR CConnectionTemplate::OAGetExecutable()
{
    return m_templateData.m_executable.AllocSysString();
}

void CConnectionTemplate::OASetExecutable(LPCSTR lpszNewValue)
{
    m_templateData.m_executable = lpszNewValue;
}

BSTR CConnectionTemplate::OAGetHostname()
{
    return m_templateData.m_hostname.AllocSysString();
}

void CConnectionTemplate::OASetHostname(LPCSTR lpszNewValue)
{
    m_templateData.m_hostname = lpszNewValue;
}

short CConnectionTemplate::OAGetObjectSpace()
{
    return m_templateData.m_osNumber;
}

void CConnectionTemplate::OASetObjectSpace(short nNewValue)
{
    if (nNewValue >= 1 && nNewValue < 1024) m_templateData.m_osNumber = nNewValue;
}

BSTR CConnectionTemplate::OAGetOptions()
{
    return m_templateData.m_options.AllocSysString();
}

void CConnectionTemplate::OASetOptions(LPCSTR lpszNewValue)
{
    m_templateData.m_options = lpszNewValue;
}

BSTR CConnectionTemplate::OAGetPassword()
{
    return m_templateData.m_password.AllocSysString();
}

void CConnectionTemplate::OASetPassword(LPCSTR lpszNewValue)
{
    m_templateData.m_password = lpszNewValue;
}

long CConnectionTemplate::OAGetTimeout()
{
    return m_templateData.m_timeout;
}

void CConnectionTemplate::OASetTimeout(long nNewValue)
{
    m_templateData.m_timeout = nNewValue;
}

BSTR CConnectionTemplate::OAGetUsername()
{
    return m_templateData.m_username.AllocSysString();
}

void CConnectionTemplate::OASetUsername(LPCSTR lpszNewValue)
{
    m_templateData.m_username = lpszNewValue;
}

BSTR CConnectionTemplate::OACommand()
{
    CString command;
    m_templateData.GetCommandString (command);
    return command.AllocSysString();
}

BSTR CConnectionTemplate::OADescription()
{
    CString command;
    m_templateData.GetDescription (command);
    return command.AllocSysString();
}

LPDISPATCH CConnectionTemplate::OAAgent()
{
    return m_pAgent->GetIDispatch(TRUE);
}

LPDISPATCH CConnectionTemplate::OACreateConnection()
{
    CConnection *pConnection = CreateConnection ();
    return pConnection ? pConnection->GetIDispatch (TRUE) : NULL;
}

LPDISPATCH CConnectionTemplate::OAGetConnection()
{
    CConnection *pConnection = GetConnection ();
    return pConnection ? pConnection->GetIDispatch (TRUE) : NULL;
}

BOOL CConnectionTemplate::OALoadDefaultTemplate()
{
    return OALoadTemplate (NULL);
}

BOOL CConnectionTemplate::OALoadTemplate(LPCSTR templateName)
{
    return m_templateData.Load (templateName);
}

BOOL CConnectionTemplate::OAStoreDefaultTemplate()
{
    return OAStoreTemplate (NULL);
}

BOOL CConnectionTemplate::OAStoreTemplate(LPCSTR templateName)
{
    return m_templateData.Store (templateName);
}

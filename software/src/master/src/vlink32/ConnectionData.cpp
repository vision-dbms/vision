//////////////////////////////////////////////////////////////////////////////
//
//  CConnectionData Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...

//  Self...
#include "ConnectionData.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

static const LPCSTR TemplateIndexSection = "Connection Templates";

LPCSTR CConnectionData::AdjustedTemplateName (LPCSTR templateName)
{
    return templateName && 0 != strcmp (templateName, "") ? templateName : "Default";
}

CString CConnectionData::TemplateSectionName (LPCSTR templateName)
{
    CString sectionName = "Connection Template ";
    sectionName += AdjustedTemplateName (templateName);
    return sectionName;
}

int CConnectionData::SavedTemplateCount ()
{
    return AfxGetApp()->GetProfileInt (TemplateIndexSection, "Count", 0);
}

LPCSTR CConnectionData::SavedTemplateKey (int templateIndex)
{
    static char keyBuffer[20];
    sprintf (keyBuffer, "Template %d", templateIndex);
    return keyBuffer;
}

CString CConnectionData::SavedTemplateName (int templateIndex)
{
    return AfxGetApp()->GetProfileString (TemplateIndexSection, SavedTemplateKey (templateIndex));
}

BOOL CConnectionData::WriteTemplateName (LPCSTR templateName)
{
    templateName = AdjustedTemplateName (templateName);

    int templateIndex;
    int templateCount = SavedTemplateCount ();
    for (templateIndex = 0; templateIndex < templateCount; templateIndex++)
    {
	if (templateName == SavedTemplateName (templateIndex))
	    return TRUE;
    }
    return AfxGetApp()->WriteProfileString (
	TemplateIndexSection, SavedTemplateKey (templateIndex), templateName
    ) && AfxGetApp()->WriteProfileInt (TemplateIndexSection, "Count", templateIndex + 1);
}


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

CConnectionData::CConnectionData (
    LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
)
: m_xServerType		(ServerType_RemoteExec)
, m_hostname		(hostname)
, m_username		(username)
, m_password		(password)
, m_executable		("/vision/bin/batchvision")
, m_database		(database)
, m_osNumber		(objectSpace)
, m_timeout		(-1)
, m_useEPrompt		(TRUE)
{
}

CConnectionData::CConnectionData ()
: m_xServerType		(ServerType_RemoteExec)
, m_executable		("/vision/bin/batchvision")
, m_osNumber		(0)
, m_timeout		(-1)
, m_useEPrompt		(TRUE)
{
}


/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

BOOL CConnectionData::operator== (CConnectionData& operand) const
{
    return m_hostname	== operand.m_hostname
	&& m_username	== operand.m_username
	&& m_password	== operand.m_password
	&& m_executable	== operand.m_executable
	&& m_database	== operand.m_database
	&& m_osNumber	== operand.m_osNumber
	&& m_options	== operand.m_options
	&& m_useEPrompt	== operand.m_useEPrompt;
}

void CConnectionData::GetDescription (CString& string) const
{
    // Identify the user and host, ...
    string = m_username; string += "@"; string += m_hostname;

    // ... executable, ...
    string += ":"; string += m_executable.GetLength() > 0 ? m_executable : "batchvision";

    // ... database, ...
    if (m_database.GetLength() > 0)
    {
	string += " -n";
	string += m_database;
    }

    // ... object space, ...
    if (m_osNumber > 1 && m_osNumber < 1024)
    {
	string += " -U";
	string += m_osNumber;
    }

    // ... and other options.
    if (m_options.GetLength() > 0)
    {
	string += " ";
	string += m_options;
    }
}

void CConnectionData::GetCommandString (CString& string) const
{
    // Identify the 'batchvision' executable, ...
    string = m_executable.GetLength() > 0 ? m_executable : "/vision/bin/batchvision";
    // ... required prompt format option, ...
    if (m_useEPrompt)
	string += " -E";

    // ... and optional arguments:
    if (m_database.GetLength() > 0)
    {
	string += " -n";
	string += m_database;
    }
    if (m_osNumber > 1 && m_osNumber < 1024)
    {
	string += " -U";
	string += m_osNumber;
    }
    if (m_options.GetLength() > 0)
    {
	string += " ";
	string += m_options;
    }
}

void CConnectionData::GetConnectString (CString& string) const
{
    CString command;
    GetCommandString (command);

//    string  = "0"	; string += '\0';	// ... error socket port
    string  = ""	; string += '\0';	// ... error socket port
    string += m_username; string += '\0';	// ... user name
    string += m_password; string += '\0';	// ... password
    string += command	; string += '\0';	// ... command string
}

BOOL CConnectionData::Load (LPCSTR templateName)
{
    CWinApp* const pMain = AfxGetApp ();

//  Construct the section name for the standard connection, ...
    CString sectionName = TemplateSectionName (templateName);

//  ... retrieve and check the required fields of a standard connection, ...
    CString hostname = pMain->GetProfileString (sectionName, "Hostname", m_hostname);
    if ("" == hostname)
	return FALSE;

    CString username = pMain->GetProfileString (sectionName, "Username", m_username);
    if ("" == username)
	return FALSE;

//  ... retrieve the other fields in the connection's definition, ...
    m_xServerType	= (ServerType)pMain->GetProfileInt  (sectionName, "ServerType"	, m_xServerType);
    m_hostname		= hostname;
    m_username		= username;
    m_password		= pMain->GetProfileString	    (sectionName, "Password"	, m_password);
    m_executable	= pMain->GetProfileString	    (sectionName, "Executable"	, m_executable);
    m_database		= pMain->GetProfileString	    (sectionName, "Database"	, m_database);
    m_osNumber		= pMain->GetProfileInt		    (sectionName, "ObjectSpace"	, m_osNumber);
    m_options		= pMain->GetProfileString	    (sectionName, "Options"	, m_options);
    m_forwardingId	= pMain->GetProfileString	    (sectionName, "ForwardingId", m_forwardingId);

//  ... convert 'timeout' to an integer, ...
    CString timeout;
    timeout += m_timeout;
    timeout  = AfxGetApp ()->GetProfileString (sectionName, "Timeout"	, timeout);
    sscanf (timeout, "%ld", &m_timeout);

//  ... convert 'useEPrompt' to a boolean, ...
    CString useEPromptString = AfxGetApp ()->GetProfileString (
    	sectionName, "UseEPrompt", m_useEPrompt ? "1" : "0"
    );
    int useEPromptValue = m_useEPrompt;
    sscanf (useEPromptString, "%d", &useEPromptValue);
    m_useEPrompt = useEPromptValue ? TRUE : FALSE;

//  ... and return successfully.
    return TRUE;
}

BOOL CConnectionData::Store (LPCSTR templateName) const
{
//  Construct the section name for the standard connection, ...
    CString sectionName = TemplateSectionName (templateName);

//  ... convert 'timeout' to a string, ...
    CString timeout;
    timeout += m_timeout;

//  ... convert 'useEPrompt' to a string, ...
    CString useEPrompt = m_useEPrompt ? "1" : "0";

//  ... and save the standard connection definition:
    CWinApp* const pMain = AfxGetApp ();

    return pMain->WriteProfileInt	(sectionName, "ServerType"	, m_xServerType)
	&& pMain->WriteProfileString	(sectionName, "Hostname"	, m_hostname)
	&& pMain->WriteProfileString	(sectionName, "Username"	, m_username)
	&& pMain->WriteProfileString	(sectionName, "Password"	, m_password)
	&& pMain->WriteProfileString	(sectionName, "Executable"	, m_executable)
	&& pMain->WriteProfileString	(sectionName, "Database"	, m_database)
	&& pMain->WriteProfileInt	(sectionName, "ObjectSpace"	, m_osNumber)
	&& pMain->WriteProfileString	(sectionName, "Options"		, m_options)
	&& pMain->WriteProfileString	(sectionName, "ForwardingId"	, m_forwardingId)
	&& pMain->WriteProfileString	(sectionName, "Timeout"		, timeout)
	&& pMain->WriteProfileString	(sectionName, "UseEPrompt"	, useEPrompt)
	&& WriteTemplateName (templateName);
}

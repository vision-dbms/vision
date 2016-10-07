//////////////////////////////////////////////////////////////////////////////
//
//  CVLinkCmdTarget Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

#include <time.h>

//  Supporting...
#include "Agent.h"
#include "TextBuffer.h"
#include "vlinkapp.h"

//  Self...
#include "vlinkcmd.h"

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CVLinkCmdTarget::Dump (CDumpContext& dc) const
{
    CCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Maps
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVLinkCmdTarget, CCmdTarget)
    //{{AFX_MSG_MAP(CVLinkCmdTarget)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CVLinkCmdTarget, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CVLinkCmdTarget)
    DISP_PROPERTY(CVLinkCmdTarget, "Tag", m_tag, VT_BSTR)
    DISP_FUNCTION(CVLinkCmdTarget, "ClassName", OAClassName, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "Name", OAName, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "UID", OAUID, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "Application", OAApplication, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "Parent", OAParent, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "DescriptionBuffer", OADescriptionBuffer, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "DescriptionText", OADescriptionText, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "SynopsisBuffer", OASynopsisBuffer, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CVLinkCmdTarget, "SynopsisText", OASynopsisText, VT_BSTR, VTS_NONE)
    DISP_DEFVALUE(CVLinkCmdTarget, "Name")
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

CApplication* CVLinkCmdTarget::Application () {
    return &theApp;
}


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CVLinkCmdTarget, CCmdTarget)

CVLinkCmdTarget::CVLinkCmdTarget ()
: m_iUIDSequence(CApplication::g_iUIDSequence++)
{
    EnableAutomation();
    AfxOleLockApp();

    m_pNextNewestItem =
    m_pNextOldestItem = NULL;
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CVLinkCmdTarget::~CVLinkCmdTarget()
{
    AfxOleUnlockApp ();
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CVLinkCmdTarget::DumpBuffer (LPCSTR description, LPCSTR buffer, int byteCount) const
{
#ifdef _DEBUG
    int i;
    afxDump << this << description << ":";
    for (i = 0; i < byteCount; i++)
    {
	if (0 == i % 80) afxDump << "\n|*** ";
	if (buffer[i] >= 32 && buffer[i] < 127)
	{
	    CString s(&buffer[i], 1);
	    afxDump << s;
	}
	else afxDump << "\\<" << (int)buffer[i] << ">";
    }
    afxDump << "\n**** " << byteCount << (byteCount != 1 ? "Bytes\n" : "Byte\n");
#endif
}

void CVLinkCmdTarget::GetClassName (CString& result) const
{
    result  = "VLink.";
    result += GetRuntimeClass()->m_lpszClassName + 1;
}

void CVLinkCmdTarget::GetUID (CString& result) const
{
    GetClassName (result);
    result += ".";
    result += CApplication::g_iUIDBase;
    result += ".";
    result += m_iUIDSequence;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

BSTR CVLinkCmdTarget::OAClassName()
{
    CString s;
    GetClassName (s);
    return s.AllocSysString();
}

BSTR CVLinkCmdTarget::OAName()
{
    CString s;
    GetName (s);
    return s.AllocSysString();
}

BSTR CVLinkCmdTarget::OAUID()
{
    CString s;
    GetUID (s);
    return s.AllocSysString();
}

LPDISPATCH CVLinkCmdTarget::OAApplication()
{
    return Application()->GetIDispatch (TRUE);
}

LPDISPATCH CVLinkCmdTarget::OAParent()
{
    return Application()->GetIDispatch (TRUE);
}

LPDISPATCH CVLinkCmdTarget::OADescriptionBuffer()
{
    CString s;
    GetDescription (s);
    CTextBuffer* pTextBuffer = new CTextBuffer (s);
    return pTextBuffer->GetIDispatch (FALSE);
}

BSTR CVLinkCmdTarget::OADescriptionText()
{
    CString s;
    GetDescription (s);
    return s.AllocSysString();
}

LPDISPATCH CVLinkCmdTarget::OASynopsisBuffer()
{
    CString s;
    GetSynopsis (s);
    CTextBuffer* pTextBuffer = new CTextBuffer (s);
    return pTextBuffer->GetIDispatch (FALSE);
}

BSTR CVLinkCmdTarget::OASynopsisText()
{
    CString s;
    GetSynopsis (s);
    return s.AllocSysString();
}

#ifndef CVLinkCmdTarget_Interface
#define CVLinkCmdTarget_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

class CApplication;
class CTextBuffer;

/*************************
 *****  Definitions  *****
 *************************/

class CVLinkCmdTarget : public CCmdTarget
{
    friend class CCollection;

    DECLARE_DYNAMIC(CVLinkCmdTarget)

//  Construction
protected:
    CVLinkCmdTarget ();

//  Destruction
protected:
    ~CVLinkCmdTarget ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Operations
public:
    static CApplication*Application		();

    CVLinkCmdTarget*    NextNewestItem		() const { return m_pNextNewestItem; }
    CVLinkCmdTarget*	NextOldestItem		() const { return m_pNextOldestItem; }

    virtual ULONG	ClassIndex		() const { return 0; }

    void		GetClassName		(CString &result) const;
    void		GetUID			(CString &result) const;
    virtual void	GetName			(CString &result) const { GetUID  (result); };
    virtual void	GetDescription		(CString &result) const { GetName (result); };
    virtual void	GetSynopsis		(CString &result) const { GetName (result); };

    ULONG		Retain			() { return ExternalAddRef  (); }
    ULONG		Release 		() { return ExternalRelease (); }

    void DumpBuffer (LPCSTR description, const CString& string) const {
	DumpBuffer (description, string, string.GetLength ());
    }
    void DumpBuffer (LPCSTR description, LPCSTR buffer, int byteCount) const;

//  State
public:
    DWORD const			m_iUIDSequence;
    CVLinkCmdTarget*		m_pNextNewestItem;
    CVLinkCmdTarget*		m_pNextOldestItem;

// Handlers
protected:
    //{{AFX_MSG(CVLinkCmdTarget)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    //{{AFX_DISPATCH(CVLinkCmdTarget)
    CString m_tag;
    afx_msg BSTR OAClassName();
    afx_msg BSTR OAName();
    afx_msg BSTR OAUID();
    afx_msg LPDISPATCH OAApplication();
    afx_msg LPDISPATCH OAParent();
    afx_msg LPDISPATCH OADescriptionBuffer();
    afx_msg BSTR OADescriptionText();
    afx_msg LPDISPATCH OASynopsisBuffer();
    afx_msg BSTR OASynopsisText();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

#endif

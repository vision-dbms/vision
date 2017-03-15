#ifndef CConnectionData_Interface
#define CConnectionData_Interface

class CConnectionData
{
//  Globals
public:
    static LPCSTR	AdjustedTemplateName	(LPCSTR templateName);
    static CString	TemplateSectionName	(LPCSTR templateName);

    static int		SavedTemplateCount	();
    static LPCSTR	SavedTemplateKey	(int savedTemplateIndex);
    static CString	SavedTemplateName	(int savedTemplateIndex);

private:
    static BOOL		WriteTemplateName	(LPCSTR templateName);

//  Types
public:
    enum ServerType {
	ServerType_LocalExec,
	ServerType_RemoteExec,
	ServerType_SVS
    };

//  Construction
public:
    CConnectionData (
	LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
    );
    CConnectionData ();

//  Operations
public:
    BOOL operator== (CConnectionData& operand) const;

    CString const& ForwardingId	() const { return m_forwardingId; }

    void GetDescription		(CString& string) const;
    void GetCommandString	(CString& string) const;
    void GetConnectString	(CString& string) const;

    BOOL Load			(LPCSTR storedTemplateName);
    BOOL Store			(LPCSTR storedTemplateName) const;

//  State
protected:
    ServerType		m_xServerType;
public:
    CString		m_hostname;
    CString		m_username;
    CString		m_password;
    CString		m_executable;
    CString		m_database;
    short		m_osNumber;
    CString		m_options;
    long		m_timeout;
    BOOL		m_useEPrompt;
    CString		m_forwardingId;
};

#endif

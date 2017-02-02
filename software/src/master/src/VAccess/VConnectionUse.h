/*****  Connection Use Class Definition  *****/
#ifndef VConnectionUse_Interface
#define VConnectionUse_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VConnection.h"

class VFormatSettings;
class VString;


/*************************
 *****  Definitions  *****
 *************************/

class VConnectionUse : public VReferenceable {
    DECLARE_ABSTRACT_RTT (VConnectionUse, VReferenceable);

//  Friends
    friend class VConnection;

//  Construction
protected:
    VConnectionUse (VConnection* pConnection);

//  Destruction
protected:
    ~VConnectionUse ();

//  Initialization
protected:
    void attach () {
	m_pConnection->attach (this);
    }

//  Access
public:
    VConnection* connection () const {
	return m_pConnection;
    }
    VFormatSettings* formatSettings () const {
	return m_pFormatSettings;
    }

//  Query
public:
    int isConnected () const;
    int isntConnected () const;

//  Event Handling
protected:
    virtual void handleConnectionEvent ();
    virtual void handleDisconnectEvent ();

//  Request Execution
public:
    int execute (
	char const*	pRequest,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );

    int submit (
	VString&	iReply,
	char const*	pRequest,
	char const*	pDate = 0,
	char const*	pCurrency = 0,
	size_t		sReplyBufferInitialAllocation	= 4096,
	size_t		sReplyBufferAllocationIncrement	= 4096
    );

//  State
protected:
    VReference<VConnection>	const	m_pConnection;
    VReference<VFormatSettings>	const	m_pFormatSettings;
    VConnectionUse*			m_pNextUse;
    VConnectionUse*			m_pPrevUse;
};

#endif

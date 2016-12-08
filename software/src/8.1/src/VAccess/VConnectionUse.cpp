/*****  VConnectionUse Implementation  *****/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VConnectionUse.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VConnection.h"
#include "VFormatSettings.h"

/**********************
 *****  Standard  *****
 **********************/

DEFINE_ABSTRACT_RTT (VConnectionUse);


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VConnectionUse::VConnectionUse (VConnection* pConnection)
: m_pConnection		(pConnection)
, m_pFormatSettings	(pConnection->formatSettings ()->newChild ())
, m_pNextUse		(0)
, m_pPrevUse		(0)
{
}

/************************
 ************************
 *****  Destruction  ****
 ************************
 ************************/

VConnectionUse::~VConnectionUse () {
    m_pConnection->detach (this);
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

int VConnectionUse::isConnected () const {
    return m_pConnection->isConnected ();
}
int VConnectionUse::isntConnected () const {
    return m_pConnection->isntConnected ();
}

/****************************
 ****************************
 *****  Event Handling  *****
 ****************************
 ****************************/

void VConnectionUse::handleConnectionEvent () {
}

void VConnectionUse::handleDisconnectEvent () {
}

/*******************************
 *******************************
 *****  Request Execution  *****
 *******************************
 *******************************/

int VConnectionUse::execute (
    char const *	pRequest,
    char const*		pDate,
    char const*		pCurrency
)
{
    return m_pConnection->execute (
	pRequest, pDate, pCurrency
    );
}

int VConnectionUse::submit (
    VString&		iReply,
    char const *	pRequest,
    char const*		pDate,
    char const*		pCurrency,
    size_t		sReplyBufferInitialAllocation,
    size_t		sReplyBufferAllocationIncrement
)
{
    return m_pConnection->submit (
	iReply,
	pRequest,
	pDate,
	pCurrency
    );
}

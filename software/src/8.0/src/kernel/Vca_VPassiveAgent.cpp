/*****  Vca_VPassiveAgent Implementation  *****/

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

#include "Vca_VPassiveAgent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VGoferExogenousInterface.h"

#include "Vca_CompilerHappyPill.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VPassiveAgent::Agency  *****
 *****                              *****
 ****************************************
 ****************************************/

namespace Vca {
    class VPassiveAgent::Agency : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Agency, VRolePlayer);

	friend class VPassiveAgent;

    //  Construction
    protected:
	Agency (VPassiveAgent *pAgent) : m_pIPassiveServer (this), m_pAgent (pAgent) {
	}

    //  Destruction
    private:
	~Agency () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IPassiveServer Role
    private:
	VRole<ThisClass,IPassiveServer> m_pIPassiveServer;
    public:
	void getRole (IPassiveServer::Reference &rpRole) {
	    m_pIPassiveServer.getRole (rpRole);
	}

    //  IPassiveServer Callbacks
    public:
	void RequestCallback (
	    IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
	);

    //  State
    private:
	AgentReference m_pAgent;
    };
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VPassiveAgent::Agency::RequestCallback (
    IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
) {
    AgentReference pAgent;
    pAgent.claim (m_pAgent);
    pAgent->getSuccessor (m_pAgent);

    if (pAgent) {
	CallData iCallData (pCall, rCallerAddress, rCallID);
	pAgent->processCallbackRequest (iCallData);
    } else if (pCall) {
	pCall->OnEnd ();
    }
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VPassiveAgent::CallData  *****
 *****                                *****
 ******************************************
 ******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPassiveAgent::CallData::CallData (
    IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
) : m_pCall (pCall), m_iCallerAddress (rCallerAddress), m_iCallID (rCallID) {
}

Vca::VPassiveAgent::CallData::CallData (
    ThisClass const &rOther
) : m_pCall (rOther.m_pCall), m_iCallerAddress (rOther.m_iCallerAddress), m_iCallID (rOther.m_iCallID) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VPassiveAgent::CallData::~CallData () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VPassiveAgent::CallData::OnEnd () const {
    if (m_pCall)
	m_pCall->OnEnd ();
}

void Vca::VPassiveAgent::CallData::OnError (IError *pInterface, VString const &rMessage) const {
    if (m_pCall)
	m_pCall->OnError (pInterface, rMessage);
}


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VPassiveAgent::CallbackRequest  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPassiveAgent::CallbackRequest::CallbackRequest (char const *pRequestData) {
    setTo (pRequestData);
}

Vca::VPassiveAgent::CallbackRequest::CallbackRequest (
    ThisClass const &rOther
) : m_iCallerAddress (rOther.callerAddress ()), m_iCallID (rOther.callID ()), m_bValid (rOther.isValid ()) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VPassiveAgent::CallbackRequest::~CallbackRequest () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VPassiveAgent::CallbackRequest::setTo (char const *pRequestData) {
    m_bValid = false;

//  Skip leading whitespace...
    pRequestData += strspn (pRequestData, " \t");

//  Extract the call's UUID...
    if (char const* const pDelimiter = strchr (pRequestData, '@')) {
	size_t  sUUIDText = pDelimiter - pRequestData;
	VString iUUIDText (sUUIDText);
	iUUIDText.setTo (pRequestData, sUUIDText);
	uuid_t iUUID;
	if (VkUUID::GetUUID (iUUID, iUUIDText)) {
	    pRequestData += sUUIDText + 1;
	    m_iCallerAddress.setTo (pRequestData);
	    m_iCallID.setTo (iUUID);
	    m_bValid = true;
	}
    }
}


/********************************
 ********************************
 *****                      *****
 *****  Vca::VPassiveAgent  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPassiveAgent::VPassiveAgent () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VPassiveAgent::~VPassiveAgent () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VPassiveAgent::supplyPassiveServerGofer (PassiveServerGofer::Reference &rpGofer) {
    IPassiveServer::Reference pIPassiveServer;
    (new Agency (this))->getRole (pIPassiveServer);
    rpGofer.setTo (new VGoferExogenousInterface<IPassiveServer> (pIPassiveServer));
    return rpGofer.isntNil ();
}

/*****  Vca_VListener Implementation  *****/

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

#include "Vca_VListener.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IListenerClient.h"
#include "Vca_IPipeSourceClient.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VConnection.h"

#include "Vca_VStatus.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VListener::Request  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VListener::Request::Request (
    IListenerClient *pClient, size_t cConnections
) : m_xType (Type_Listener), m_pClient (pClient), m_cConnections (cConnections) {
}

Vca::VListener::Request::Request (
    IConnectionSourceClient *pClient, size_t cConnections
) : m_xType (Type_ConnectionSource), m_pClient (pClient), m_cConnections (cConnections) {
}

Vca::VListener::Request::Request (
    IPipeSourceClient *pClient, size_t cConnections
) : m_xType (Type_PipeSource), m_pClient (pClient), m_cConnections (cConnections) {
}

Vca::VListener::Request::Request (Request const &rOther)
    : m_xType (rOther.m_xType)
    , m_pClient (rOther.m_pClient)
    , m_cConnections (rOther.m_cConnections)
{
}

Vca::VListener::Request::Request () : m_xType (Type_None), m_cConnections (0) {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VListener::Request::setTo (IListenerClient *pClient, size_t cConnections) {
    m_xType = Type_Listener;
    m_pClient.setTo (pClient);
    m_cConnections = cConnections;
}

void Vca::VListener::Request::setTo (IConnectionSourceClient *pClient, size_t cConnections) {
    m_xType = Type_ConnectionSource;
    m_pClient.setTo (pClient);
    m_cConnections = cConnections;
}

void Vca::VListener::Request::setTo (IPipeSourceClient *pClient, size_t cConnections) {
    m_xType = Type_PipeSource;
    m_pClient.setTo (pClient);
    m_cConnections = cConnections;
}

void Vca::VListener::Request::setTo (Request const &rOther) {
    m_xType = rOther.m_xType;
    m_pClient.setTo (rOther.m_pClient);
    m_cConnections = rOther.m_cConnections;
}

/**********************************
 **********************************
 *****  Client Communication  *****
 **********************************
 **********************************/

void Vca::VListener::Request::onDevice (VDevice *pDevice) {
    switch (m_xType) {
    case Type_Listener: {
	    IConnection::Reference pInterface;
	    if (!pDevice->supply (pInterface))
		pDevice->communicateFailureTo (m_pClient);
	    else static_cast<IListenerClient*>(m_pClient.referent ())->OnConnection (
		pInterface
	    );
	}
	break;
    case Type_ConnectionSource: {
	    IConnection::Reference pInterface;
	    if (!pDevice->supply (pInterface))
		pDevice->communicateFailureTo (m_pClient);
	    else static_cast<IConnectionSourceClient*>(m_pClient.referent ())->OnData (
		pInterface
	    );
	}
	break;
    case Type_PipeSource: {
	    VBSProducer::Reference pBSProducer; VBSConsumer::Reference pBSConsumer;
	    if (!pDevice->supply (pBSProducer, pBSConsumer))
		pDevice->communicateFailureTo (m_pClient);
	    else {
              static_cast<IPipeSourceClient*>(m_pClient.referent ())->OnData (
		pBSConsumer, pBSProducer, 0
	    );
            }
	}
	break;
    }
    m_cConnections--;
}

//  returns true if an end or error was encountered:
bool Vca::VListener::Request::onStatus (VStatus const &rStatus) const {
    return rStatus.communicateTo (m_pClient);
}


/****************************
 ****************************
 *****                  *****
 *****  Vca::VListener  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VListener::VListener (DeviceFace *pDeviceFace)
: DeviceUser		(this, pDeviceFace)
, m_pIConnectionSource	(this)
, m_pIPipeSource	(this)
, m_pIListener		(this)
{
}


/*******************************
 *******************************
 *****  Interface Methods  *****
 *******************************
 *******************************/

void Vca::VListener::Supply (IConnectionSource *pRole, IConnectionSourceClient *pClient) {
    if (pClient) {
	Request iRequest (pClient);
	if (m_iQueue.enqueue (iRequest))
	    acceptConnections ();
    }
}

void Vca::VListener::Supply (IPipeSource *pRole, IPipeSourceClient *pClient) {
    if (pClient) {
	Request iRequest (pClient);
	if (m_iQueue.enqueue (iRequest))
	    acceptConnections ();
    }
}

void Vca::VListener::SupplyByteStreams (
    IListener *pRole, IPipeSourceClient *pClient, U32 cConnections
) {
    SupplyByteStreams (pClient, cConnections);
}

void Vca::VListener::SupplyConnections (
    IListener *pRole, IListenerClient *pClient, U32 cConnections
) {
    SupplyConnections (pClient, cConnections);
}

void Vca::VListener::GetNamespace (
    IBinding *pRole, IVReceiver<INamespace*> *pClient
) {
    if (pClient)
	pClient->OnData (0);
}

void Vca::VListener::GetName (
    IBinding *pRole, IVReceiver<VString const&> *pClient
) {
    if (pClient) {
	VStatus iStatus; VString iName;
	if (getName (iStatus, iName))
	    pClient->OnData (iName);
	else
	    iStatus.communicateTo (pClient);
    }
}

void Vca::VListener::Close (IResource *pRole) {
    close ();
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

void Vca::VListener::SupplyByteStreams (IPipeSourceClient *pClient, U32 cConnections) {
    if (pClient && cConnections > 0) {
	Request iRequest (pClient, cConnections);
	if (m_iQueue.enqueue (iRequest))
	    acceptConnections ();
    }
}

void Vca::VListener::SupplyConnections (IListenerClient *pClient, U32 cConnections) {
    if (pClient && cConnections > 0) {
	Request iRequest (pClient, cConnections);
	if (m_iQueue.enqueue (iRequest))
	    acceptConnections ();
    }
}

/********************************
 ********************************
 *****  Request Processing  *****
 ********************************
 ********************************/

void Vca::VListener::onDevice_(VDevice *pDevice) {
    activeRequest ().onDevice (pDevice);
}

void Vca::VListener::acceptConnections () {

    Reference iRetainer (this); //  Protective Guard

    VStatus iStatus;
    while (finish (iStatus)
	&& iStatus.isCompleted ()
	&& active ()
	&& (activeRequest ().isntSatisfied () || nextRequest ())
	&& start (iStatus)
    );
    if (active ())
	activeRequest ().onStatus (iStatus);
}

bool Vca::VListener::nextRequest () {
    if (m_iQueue.dequeue ())
	return true;

    activeRequest ().clear ();
    return false;
}

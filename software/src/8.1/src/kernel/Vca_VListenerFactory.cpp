/*****  Vca::VListenerFactory Implementation  *****/

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

#include "Vca_VListenerFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IListenerClient.h"
#include "Vca_IPipeSourceClient.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VDeviceFactory.h"

#include "Vca_VListener.h"
#include "Vca_VStatus.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VListenerFactory  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VListenerFactory::VListenerFactory (
    VDeviceFactory *pDeviceFactory
) : BaseClass (pDeviceFactory->cohort ()), m_pDeviceFactory (pDeviceFactory), m_pIListenerFactory (this) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VListenerFactory::~VListenerFactory () {
    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VListenerFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VListenerFactory::Supply (Reference &rpInstance) {
    rpInstance.setTo (
	reinterpret_cast<ThisClass*> (VCohort::VcaValue (CohortIndex ()))
    );
    if (rpInstance.isNil ()) {
	VDeviceFactory::Reference pDeviceFactory;
	if (VDeviceFactory::Supply (pDeviceFactory))
	    rpInstance.setTo (new ThisClass (pDeviceFactory));
    }
    return rpInstance.isntNil ();
}

bool Vca::VListenerFactory::Supply (ThisInterface::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}

/**************************************
 **************************************
 *****  IListenerFactory Methods  *****
 **************************************
 **************************************/

void Vca::VListenerFactory::SupplyListener (
    IListenerFactory *pRole, IVReceiver<IListener*> *pClient, VString const &rName
) {
    if (pClient) {
	VStatus iStatus; VListener::Reference pListener;
	if (!m_pDeviceFactory->supply (iStatus, pListener, rName, Default::TcpNoDelaySetting ()))
	    iStatus.communicateTo (pClient);
	else {
	    IListener::Reference pInterface;
	    pListener->getRole (pInterface);
	    pClient->OnData (pInterface);
	}
    }
}

void Vca::VListenerFactory::SupplyByteStreams (
    IListenerFactory*	pRole,
    IPipeSourceClient*	pClient,
    VString const&	rName,
    U32			cConnections
) {
    if (pClient) {
	VStatus iStatus; VListener::Reference pListener;
	if (m_pDeviceFactory->supply (iStatus, pListener, rName, Default::TcpNoDelaySetting ()))
	    pListener->SupplyByteStreams (pClient, cConnections);
	else
	    iStatus.communicateTo (pClient);
    }
}

void Vca::VListenerFactory::SupplyConnections (
    IListenerFactory*	pRole,
    IListenerClient*	pClient,
    VString const&	rName,
    U32			cConnections
) {
    if (pClient) {
	VStatus iStatus; VListener::Reference pListener;
	if (!m_pDeviceFactory->supply (iStatus, pListener, rName, Default::TcpNoDelaySetting ()))
	    iStatus.communicateTo (pClient);
	else {
	    IListener::Reference pInterface;
	    pListener->getRole (pInterface);
	    pClient->OnListener (pInterface);
	    pInterface->SupplyConnections (pClient, cConnections);
	}
    }
}

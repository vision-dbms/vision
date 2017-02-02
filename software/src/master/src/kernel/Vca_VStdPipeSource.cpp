/*****  Vca_VStdPipeSource Implementation  *****/

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

#include "Vca_VStdPipeSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IStdPipeSourceClient.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VDeviceFactory.h"

#include "Vca_VStatus.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VStdPipeSource  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VStdPipeSource::VStdPipeSource (
    VDeviceFactory *pDeviceFactory
) : BaseClass (pDeviceFactory->cohort ()), m_pDeviceFactory (pDeviceFactory), m_pIStdPipeSource (this) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VStdPipeSource::~VStdPipeSource () {
    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VStdPipeSource::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}


/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VStdPipeSource::Supply (Reference &rpInstance) {
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

bool Vca::VStdPipeSource::Supply (ThisInterface::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}


/************************************
 ************************************
 *****  IStdPipeSource Methods  *****
 ************************************
 ************************************/

void Vca::VStdPipeSource::Supply (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
) {
    supply (pClient);
}

void Vca::VStdPipeSource::Supply2 (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
) {
    supply (pClient, Std_IO);
}

void Vca::VStdPipeSource::SupplyI (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
) {
    supply (pClient, Std_I);
}

void Vca::VStdPipeSource::SupplyO (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
) {
    supply (pClient, Std_O);
}

void Vca::VStdPipeSource::SupplyE (
    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
) {
    supply (pClient, Std_E);
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

void Vca::VStdPipeSource::supply (IStdPipeSourceClient *pClient, Std xWhat) {
    if (pClient) {
	VStatus iStatus;
	VBSProducer::Reference pStdin;
	VBSConsumer::Reference pStdout, pStderr;
#ifdef __VMS
	bool bTwoPipe = getenv ("VcaStd3PipeSource") ? false : true;
#else
	bool bTwoPipe = getenv ("VcaStd2PipeSource") ? true : false;
#endif
	bool bDone = bTwoPipe ? m_pDeviceFactory->supply (
	    iStatus, pStdin, pStdout
	) : m_pDeviceFactory->supply (iStatus, pStdin, pStdout, pStderr);
	if (bDone)
	    pClient->OnData (pStdin, pStdout, pStderr);
	else
	    iStatus.communicateTo (pClient);
    }
}

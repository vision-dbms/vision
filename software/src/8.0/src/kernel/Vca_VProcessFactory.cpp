/*****  Vca_VProcessFactory Implementation  *****/

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

#include "Vca_VProcessFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPipeSourceClient.h"
#include "Vca_IProcessFactoryClient.h"

#include "Vca_VStatus.h"


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::VProcessFactory::PipeSource  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VProcessFactory::PipeSource::PipeSource (
    VProcessFactory *pFactory, VString const &rCommand, Wants xWants
) : m_pFactory (pFactory), m_iCommand (rCommand), m_xWants (xWants) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VProcessFactory::PipeSource::~PipeSource () {
    traceInfo ("Destroying VProcessFactory::PipeSource");
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VProcessFactory::PipeSource::supply_(IPipeSourceClient *pClient) {
    m_pFactory->supply (pClient, m_iCommand, m_xWants);
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VProcessFactory  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VProcessFactory::VProcessFactory (
    VDeviceFactory *pDeviceFactory
) : BaseClass (pDeviceFactory->cohort ()), m_pDeviceFactory (pDeviceFactory), m_pIProcessFactory (this) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VProcessFactory::~VProcessFactory () {
    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VProcessFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VProcessFactory::Supply (Reference &rpInstance) {
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

bool Vca::VProcessFactory::Supply (ThisInterface::Reference &rpRole) {
    Reference pInstance;
    if (Supply (pInstance))
	pInstance->getRole (rpRole);
    else
	rpRole.clear ();
    return rpRole.isntNil ();
}


/*************************************
 *************************************
 *****  IProcessFactory Methods  *****
 *************************************
 *************************************/

void Vca::VProcessFactory::MakeProcess (
    IProcessFactory *pRole, IProcessFactoryClient *pClient, VString const &rCommand, bool bTwoPipe
) {
    supply (pClient, rCommand, WantsTwoPipe (bTwoPipe));
}


/***********************
 ***********************
 *****  Local Use  *****
 ***********************
 ***********************/

bool Vca::VProcessFactory::Supply (
    IProcessFactoryClient *pClient, VString const &rCommand, Wants xWants
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (pClient, rCommand, xWants);
}

bool Vca::VProcessFactory::supply (
    IProcessFactoryClient *pClient, VString const &rCommand, Wants xWants
) {
    bool bStarted = false;
    if (pClient) {
	VStatus iStatus;
	VProcess::Reference pProcess;
	VBSConsumer::Reference pStdToPeer;
	VBSProducer::Reference pStdToHere;
	VBSProducer::Reference pErrToHere;
	if (supply (
		iStatus, pProcess,
		WantsStdin  (xWants) ? &pStdToPeer : 0,
		WantsStdout (xWants) ? &pStdToHere : 0,
		WantsStderr (xWants) ? &pErrToHere : WantsStdout (xWants) ? &pStdToHere : 0,
		rCommand
	    )
	) {
	    IProcess::Reference pInterface;
	    pProcess->getRole (pInterface);
	    pClient->OnData (pInterface, pStdToPeer, pStdToHere, pErrToHere);
	    bStarted = true;
	}
	else {
	    iStatus.communicateTo (pClient);
	}
    }
    return bStarted;
}

bool Vca::VProcessFactory::Supply (
    IPipeSourceClient *pClient, VString const &rCommand, Wants xWants
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (pClient, rCommand, xWants);
}

bool Vca::VProcessFactory::supply (
    IPipeSourceClient *pClient, VString const &rCommand, Wants xWants
) {
    bool bStarted = false;
    if (pClient) {
	VStatus iStatus;
	VProcess::Reference pProcess;
	VBSConsumer::Reference pStdToPeer;
	VBSProducer::Reference pStdToHere;
	VBSProducer::Reference pErrToHere;
	if (supply (
		iStatus, pProcess,
		WantsStdin  (xWants) ? &pStdToPeer : 0,
		WantsStdout (xWants) ? &pStdToHere : 0,
		WantsStderr (xWants) ? &pErrToHere : WantsStdout (xWants) ? &pStdToHere : 0,
		rCommand
	    )
	) {
	    pClient->OnData (pStdToPeer, pStdToHere, pErrToHere);
	    bStarted = true;
	}
	else {
	    iStatus.communicateTo (pClient);
	}
    }
    return bStarted;
}

bool Vca::VProcessFactory::Supply (
    PipeSource::Reference &rpPPS, VString const &rCommand, Wants xWants
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rpPPS, rCommand, xWants);
}

bool Vca::VProcessFactory::supply (
    PipeSource::Reference &rpPPS, VString const &rCommand, Wants xWants
) {
    rpPPS.setTo (new PipeSource (this, rCommand, xWants));
    return true;
}

bool Vca::VProcessFactory::Supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference&	rpStdBSToPeer,
    VBSProducer::Reference&	rpStdBSToHere,
    VBSProducer::Reference&	rpErrBSToHere,	//  & == &rpStdBSToHere -> send to stdout
    char const*			pCommand
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpProcess, rpStdBSToPeer, rpStdBSToHere, rpErrBSToHere, pCommand
    );
}

bool Vca::VProcessFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference&	rpStdBSToPeer,
    VBSProducer::Reference&	rpStdBSToHere,
    VBSProducer::Reference&	rpErrBSToHere,	//  & == &rpStdBSToHere -> send to stdout
    char const*			pCommand
) {
    return m_pDeviceFactory->supply (
	rStatus, rpProcess, rpStdBSToPeer, rpStdBSToHere, rpErrBSToHere, pCommand
    );
}

bool Vca::VProcessFactory::Supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference*	ppStdBSToPeer,
    VBSProducer::Reference*	ppStdBSToHere,
    VBSProducer::Reference*	ppErrBSToHere,	//  null -> null device, ==ppStdBSToHere -> send to stdout
    char const*			pCommand
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpProcess, ppStdBSToPeer, ppStdBSToHere, ppErrBSToHere, pCommand
    );
}

bool Vca::VProcessFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference*	ppStdBSToPeer,
    VBSProducer::Reference*	ppStdBSToHere,
    VBSProducer::Reference*	ppErrBSToHere,	//  null -> null device, ==ppStdBSToHere -> send to stdout
    char const*			pCommand
) {
    return m_pDeviceFactory->supply (
	rStatus, rpProcess, ppStdBSToPeer, ppStdBSToHere, ppErrBSToHere, pCommand
    );
}

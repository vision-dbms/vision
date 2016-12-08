/*****  Vca_IPassiveServer_Gofers Implementation *****/
#define Vca_IPassiveServer_Gofers

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

#include "Vca_IPassiveServer_Gofers.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPassiveCall.h"

#include "Vca_VProcess.h"
#include "Vca_VProcessFactory.h"
#include "Vca_VStatus.h"


/************************************************************
 ************************************************************
 *****                                                  *****
 *****  Vca::IPassiveServer_Gofer::LaunchAgent::Server  *****
 *****                                                  *****
 ************************************************************
 ************************************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPassiveServer_Gofer::LaunchAgent::Server::~Server () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPassiveServer_Gofer::LaunchAgent::Server::RequestCallback (
    IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
) {
    VString iLaunchCommand (m_iLaunchCommand);
    iLaunchCommand << " -callback="; {
	VString iCallIDString; {
	    VkUUIDHolder iCallID (rCallID);
	    iCallID.GetString (iCallIDString);
	}
	iLaunchCommand << iCallIDString;
    }
    iLaunchCommand << "@" << rCallerAddress;
    VStatus iLaunchStatus; VProcess::Reference pProcess;
    if (!VProcessFactory::Supply (iLaunchStatus, pProcess, 0, 0, 0, iLaunchCommand))
	iLaunchStatus.communicateTo (pCall);
}


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::IPassiveServer_Gofer::LaunchAgent  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPassiveServer_Gofer::LaunchAgent::~LaunchAgent () {
}

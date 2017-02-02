/*****  Vca_IPipeSource_Gofers Implementation *****/
#define Vca_IPipeSource_Gofers

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

#include "Vca_IPipeSource_Gofers.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_VConnectionFactory.h"
#include "Vca_VConnectionPipeSource.h"
#include "Vca_VProcessFactory.h"
#include "Vca_VRexecConnectionFactory.h"
#include "Vca_VSSHConnectionFactory.h"


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::IPipeSource_Gofer::NewConnection  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPipeSource_Gofer::NewConnection::~NewConnection () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPipeSource_Gofer::NewConnection::onData () {
    IPipeSource::Reference pIPipeSource; {
	VConnectionPipeSource::Reference pPipeSource;
	if (VConnectionFactory::Supply (pPipeSource, m_iAddress, m_bAServerFileName, Default::TcpNoDelaySetting ()))
	    pPipeSource->getRole (pIPipeSource);
    }
    setTo (pIPipeSource);
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::IPipeSource_Gofer::NewProcess  *****
 *****                                      *****
 ************************************************
 ************************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPipeSource_Gofer::NewProcess::~NewProcess () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPipeSource_Gofer::NewProcess::onData () {
    IPipeSource::Reference pIPipeSource; {
	VProcessFactory::PipeSource::Reference pPipeSource;
	if (VProcessFactory::Supply (pPipeSource, m_iCommand, VProcessFactory::WantsDescriptors (m_bPlumbingStdin, m_bPlumbingStdout, m_bPlumbingStderr)))
	    pPipeSource->getRole (pIPipeSource);
    }
    setTo (pIPipeSource);
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::IPipeSource_Gofer::NewRexec  *****
 *****                                    *****
 **********************************************
 **********************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPipeSource_Gofer::NewRexec::~NewRexec () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPipeSource_Gofer::NewRexec::onData () {
    IPipeSource::Reference pIPipeSource; {
	VRexecConnectionFactory::PipeSource::Reference pPipeSource;
	if (VRexecConnectionFactory::Supply (pPipeSource, m_iHost, m_iLogin, m_iPassword, m_iCommand, m_b3Pipe, Default::TcpNoDelaySetting ()))
	    pPipeSource->getRole (pIPipeSource);
    }
    setTo (pIPipeSource);
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::IPipeSource_Gofer::NewSSH  *****
 *****                                    *****
 **********************************************
 **********************************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::IPipeSource_Gofer::NewSSH::~NewSSH () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::IPipeSource_Gofer::NewSSH::onData () {
    IPipeSource::Reference pIPipeSource; {
	VSSHConnectionFactory::PipeSource::Reference pPipeSource;
	if (VSSHConnectionFactory::Supply (pPipeSource, m_iHost, m_iLogin, m_iPassword, m_iCommand, m_b3Pipe, Default::TcpNoDelaySetting ()))
	    pPipeSource->getRole (pIPipeSource);
    }
    setTo (pIPipeSource);
}

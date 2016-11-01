/*****  Vca_VConnectionPipeSource Implementation  *****/

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

#include "Vca_VConnectionPipeSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "VSimpleFile.h"
#include "Vca_IPipeSourceClient.h"


/****************************************
 ****************************************
 *****                              *****
 *****  Vca::VConnectionPipeSource  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VConnectionPipeSource::VConnectionPipeSource (
    VConnectionFactory *pFactory, VString const &rDestination, bool bFileName, bool bNoDelay
) : m_pFactory (pFactory), m_iDestination (rDestination), m_bFileName (bFileName), m_bNoDelay (bNoDelay) {
    traceInfo ("Creating VConnectionPipeSource");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VConnectionPipeSource::~VConnectionPipeSource () {
    traceInfo ("Destroying VConnectionPipeSource");
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VConnectionPipeSource::supply_(IPipeSourceClient *pClient) {
    if (!m_bFileName)
	m_pFactory->supply (pClient, m_iDestination, m_bNoDelay);
    else {
	VSimpleFile iFile; VString iDestination;
	if (iFile.OpenForTextRead (m_iDestination) && iFile.GetLine (iDestination))
	    m_pFactory->supply (pClient, iDestination, m_bNoDelay);
	else if (pClient)
	    pClient->OnError (0, "Invalid connection source file");
    }
}

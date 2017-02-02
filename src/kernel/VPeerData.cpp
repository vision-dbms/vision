/*****  Vca_VPeerData Implementation  *****/

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

#include "VPeerData.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void Vca::VPeerData::displayInfo () const {
    unsigned int i;
    VString string;

    VkUUIDHolder uuid(m_iPeer);
    uuid.GetString(string);
    display ("\nPeer ID: %s\n", string.content ());   

    display ("\nInFinalPath %d\n", m_iInFinalPath);

    display ("ListenerDomains\n");
    for(i=0; i<m_iListenerDomains.cardinality(); i++) {
	uuid = m_iListenerDomains[i];
	uuid.GetString(string);
	display ("\tDomain %s\n", string.content ());
    }

    display ("ConnectorDomains\n");
    for(i=0; i<m_iConnectorDomains.cardinality(); i++) {
	uuid =  m_iConnectorDomains[i];
	uuid.GetString(string);
	display ("\tDomain %s\n", string.content ());
    }

    display ("ConnectedPeers\n");
	for(i=0; i<m_iConnectedPeers.cardinality(); i++) {
	uuid = m_iConnectedPeers[i];
	uuid.GetString(string); 
	display ("\tDomain %s\n", string.content ());
    }
}

/*****  Vca_VPeerDataArray Implementation  *****/

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

#include "VPeerDataArray.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "VkSetOf.h"

#include "VPath.h"

#include "Vca_VcaSelf.h"


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VPeerDataArray::append (VPeerData const &rPeerData) {
    unsigned int xElement = m_iPeerDataArray.cardinality ();
    m_iPeerDataArray.Append (1);
    m_iPeerDataArray[xElement] = rPeerData;
}

void Vca::VPeerDataArray::appendLocalData (VcaSelf* pSelf) {
    VPeerData iLocalPeerData;
    pSelf->getLocalPeerData (iLocalPeerData);
    append (iLocalPeerData);
}

void Vca::VPeerDataArray::setFinalPathPeers (
    uuid_t const &sourcePeer, uuid_t const &destinationPeer
) {
        unsigned int i,j,k,l;
        int numListeners, numConnectors;
        unsigned int sourceIndex, destIndex;

        const unsigned int num_nodes = m_iPeerDataArray.cardinality();

        if(!locatePeer(sourcePeer, sourceIndex) || !locatePeer(destinationPeer, destIndex)) {
            printf("\nInvalid Peer");
            exit(0);
            // need to take necessary action
        }

        VReference<VPath> *actualMatrix, *firstMatrix, *secondMatrix;
 
        actualMatrix = new VReference<VPath> [num_nodes * num_nodes];
        firstMatrix  = new VReference<VPath> [num_nodes * num_nodes];
        secondMatrix = new VReference<VPath> [num_nodes * num_nodes];

        for(i=0; i<num_nodes; i++) {
            for(j=0; j<num_nodes; j++) {
                (*index(actualMatrix,i,j)).setTo(new VPath());
                (*index(firstMatrix,i,j)) .setTo(new VPath());
                (*index(secondMatrix,i,j)).setTo(new VPath());
           }
        }

        // Searching the existing information and creating single linked lists 
        // (possible and existing connections) in the created matrix. Traverses 
        // through each peer, for each listener or connection domain looks at all 
        // other peers whether they have an opposite (listener or connector). If 
        // so then creates a link(single path list) in the corresponding cell in the matrix.
        // variables i and k are indices which traverse the set 

        for(i=0; i<num_nodes; i++) {
     
            //Listener Vs Connector -  Connection Possibility Search
            numListeners = m_iPeerDataArray[i].numListenerDomains();

            while(--numListeners >=0) { 
                uuid_t listenerDomain = m_iPeerDataArray[i].listenerDomains()[numListeners];

                for(k=0; k<num_nodes; k++) {

                        if(i==k)  // neglect same peer
                             continue;

                         numConnectors = m_iPeerDataArray[k].numConnectorDomains();
                         while( --numConnectors >= 0) {
                             uuid_t connectorDomain = m_iPeerDataArray[k].connectorDomains()[numConnectors];

                             if(VkUUID::eq(listenerDomain, connectorDomain)) {                                                                                        
                                 VPath::VPathPeer *headPeer = new VPath::VPathPeer(m_iPeerDataArray[i].peerUUID(),m_iPeerDataArray[k].peerUUID());
                                 VPath *headPath     = new VPath(*headPeer);
                                 (*index(actualMatrix,i,k)).setTo(headPath);
                                 headPeer->discard();
                             }
                         }
                 }
            } //end of while
           
            // Connector Vs Listener  Connection Possibility Search
            numConnectors = m_iPeerDataArray[i].numConnectorDomains();
            while(--numConnectors >=0) { 

                 uuid_t connectorDomain = m_iPeerDataArray[i].connectorDomains()[numConnectors];
                 for(k=0; k<num_nodes; k++) {
                        if(i==k)  // neglect same peer
                             continue;
                         numListeners = m_iPeerDataArray[k].numListenerDomains();
                         while( --numListeners >= 0) {
                             uuid_t listenerDomain = m_iPeerDataArray[k].listenerDomains()[numListeners];

                             if(VkUUID::eq(listenerDomain, connectorDomain)) {                                                                                        
                                 VPath::VPathPeer *headPeer = new VPath::VPathPeer(m_iPeerDataArray[i].peerUUID(),m_iPeerDataArray[k].peerUUID()) ;
                                 VPath *headPath = new VPath(*headPeer);
                                 (*index(actualMatrix,i,k)).setTo(headPath);
                                 headPeer->discard();
                             }
                         }
                 }
            }
            
            // Direct Connections
            for(k=0; k<num_nodes; k++) 
                if(k!=i && m_iPeerDataArray[i].isConnectedToPeer(m_iPeerDataArray[k].peerUUID())) {
 
                           VPath::VPathPeer *headPeer = new VPath::VPathPeer(m_iPeerDataArray[i].peerUUID(),m_iPeerDataArray[k].peerUUID());
                           VPath *headPath = new VPath(*headPeer);
                           (*index(actualMatrix,i,k)).setTo(headPath);
                           headPeer->discard();
                    }
          }

    // Displays actual matrix, copies(duplicates) to firstMatrixMatrix and also 
    // makes it (upper or) lower triangular matrix
    
    printf("\nActual Adjacency Matrix\n\n");
    for(i=0;i<num_nodes; i++) {
        for(k=0; k<num_nodes; k++) {
            if(k >= i)
                (*index(actualMatrix,i,k)).setTo(new VPath());

            (*index(actualMatrix,i,k))->display();
            (*index(firstMatrix,i,k)).setTo(new VPath(**index(actualMatrix,i,k)));
        }
        printf("\n");
    }
    printf("\n------------------------\n");

    // Computing Path by doing repititive matrix multiplications
    // do while loop continues till a path is found 
    // need to change may end up in looping forever

    while (!(*index(firstMatrix, sourceIndex, destIndex))->headPath() 
	&& !(*index(firstMatrix, destIndex, sourceIndex))->headPath()
    ) {
        for(unsigned int x=0; x<num_nodes; x++) {
            for(unsigned int y=0; y<num_nodes; y++) {
          
                VReference<VPath> pCellResult(new VPath());
                for(unsigned int z=0;z<num_nodes;z++) {

                    if((*index(firstMatrix, x, z))->headPath() 
                    && (*index(actualMatrix ,z, y))->headPath()) {

                        VReference<VPath> pTemp(new VPath());
                        pTemp.setTo(new VPath(**index(firstMatrix, x, z)));
                        pTemp->concatenate(*index(actualMatrix, z, y));
                        pCellResult->summation(pTemp);
                    }
                }
                (*index(secondMatrix, x, y)).setTo(pCellResult);
            }
        }

    // Display
	for( k=0;k<num_nodes;k++) {
	    for(l=0;l<num_nodes;l++) {
		(*index(secondMatrix, k, l))->display();
		(*index(firstMatrix, k, l)).claim((*index(secondMatrix, k, l)));
	    }
	    printf("\n\n");
	}
	printf("\n------------------------\n");
    } 

    VPath::VPathPeer *peer;

    printf("\nResulting Route:");

    if((*index(firstMatrix, sourceIndex, destIndex))->headPath()) {
        (*index(firstMatrix, sourceIndex, destIndex))->display();
        peer = (*index(firstMatrix, sourceIndex, destIndex))->headPeer();
    }
    
    if((*index(firstMatrix, destIndex, sourceIndex))->headPath()) {
        (*index(firstMatrix, destIndex, sourceIndex))->display();
        peer = (*index(firstMatrix, destIndex, sourceIndex))->headPeer();
    }
    
    // setting the peers in final path
    while(peer) {
        setPeerStatus(peer->uuid(),true); //not checking for error condition bec should be true
        peer = peer->nextPeer();
    }
}

void Vca::VPeerDataArray::displayUUID(uuid_t const &uuid) {
    VkUUIDHolder test1(uuid);
    VString test;
    test1.GetString(test);
    printf("\n%s\n", test.content ());
}

void Vca::VPeerDataArray::populate() {
        uuid_t uuid1 =  VkUUID::ReturnUUID("11111111-52d5-45a4-803e-3c463d5482ba");
        uuid_t uuid2 =  VkUUID::ReturnUUID("22222222-52d5-45a4-803e-3c463d5482bb");
        uuid_t uuid3 =  VkUUID::ReturnUUID("33333333-52d5-45a4-803e-3c463d5482bc");
        uuid_t uuid4 =  VkUUID::ReturnUUID("44444444-52d5-45a4-803e-3c463d5482bd");
        uuid_t uuid5 =  VkUUID::ReturnUUID("55555555-52d5-45a4-803e-3c463d5482be");
        uuid_t uuid6 =  VkUUID::ReturnUUID("66666666-52d5-45a4-803e-3c463d5482b0");
        uuid_t uuid7 =  VkUUID::ReturnUUID("77777777-f7d0-44c7-981b-0b03bb1d3845");
        uuid_t uuid8 =  VkUUID::ReturnUUID("88888888-f7d0-44c7-981b-0b03bb1d3845");
        uuid_t uuid9 =  VkUUID::ReturnUUID("99999999-f7d0-44c7-981b-0b03bb1d3845");

//	uuid_t domainX =  VkUUID::ReturnUUID("4159836c-83bd-498f-ab45-9137d77561fb");
        uuid_t domainY =  VkUUID::ReturnUUID("EA52975C-5BF1-468a-ABB4-50470FA9D4CB");
//	uuid_t domainZ =  VkUUID::ReturnUUID("4E71D382-8EAA-4c4b-98D5-714F70C9A39E");

        m_iPeerDataArray.Append(9);

        VPeerData peer1(uuid1);
        //peer1.addListenerDomain(domainX);
        peer1.addConnectedPeer(uuid2);
        m_iPeerDataArray[0]= peer1;

        VPeerData peer2(uuid2);
        //peer2.addListenerDomain(domainX);
        peer2.addConnectedPeer(uuid1);
        peer2.addConnectedPeer(uuid3);
        m_iPeerDataArray[1] = peer2;

        VPeerData peer3(uuid3);
        //peer3.addConnectorDomain(domainX);
        peer3.addListenerDomain(domainY);
        peer3.addConnectedPeer(uuid2);
        peer3.addConnectedPeer(uuid4);
        m_iPeerDataArray[2] = peer3;

        VPeerData peer4(uuid4);
        //peer4->addConnectorDomain(domainY);
        //peer4->addListenerDomain(domainX);
        peer4.addConnectedPeer(uuid3);
        peer4.addConnectedPeer(uuid5);
        peer4.addConnectedPeer(uuid6);
        peer4.addConnectedPeer(uuid3);

        m_iPeerDataArray[3] = peer4;

        VPeerData peer5(uuid5);
        //peer5.addConnectorDomain(domainY);
        //peer5.addListenerDomain(domainX);
        peer5.addConnectedPeer(uuid4);
        peer5.addConnectedPeer(uuid1);

        m_iPeerDataArray[4] = peer5;

        VPeerData peer6(uuid6);
        //peer6.addConnectorDomain(domainY);
        peer6.addConnectedPeer(uuid7);
        peer6.addConnectedPeer(uuid8);
        peer6.addConnectedPeer(uuid4);

        m_iPeerDataArray[5] = peer6;

        VPeerData peer7(uuid7);
        peer7.addConnectorDomain(domainY);
        peer7.addConnectedPeer(uuid6);
        peer7.addConnectedPeer(uuid8);
        m_iPeerDataArray[6] = peer7;

        VPeerData peer8(uuid8);
        //peer8.addListenerDomain(domainY);
        //peer8.addListenerDomain(domainZ);
        peer8.addConnectedPeer(uuid7);
        peer8.addConnectedPeer(uuid9);
        peer8.addConnectedPeer(uuid6);

        m_iPeerDataArray[7] = peer8;

        VPeerData peer9(uuid9);
        //peer9.addListenerDomain(domainY);
        //peer9.addConnectorDomain(domainZ);
        peer9.addConnectedPeer(uuid8);
        m_iPeerDataArray[8] = peer9;

        for(int i=0;i<9;i++)
            displayUUID(m_iPeerDataArray[i].peerUUID());
        
        setFinalPathPeers(uuid1, uuid8);
}

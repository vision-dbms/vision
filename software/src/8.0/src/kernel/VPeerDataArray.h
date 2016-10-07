#ifndef VPeerDataArray_Interface
#define VPeerDataArray_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkDynamicArrayOf.h"
#include "VPeerData.h"

/**************************
 *****  Declarations  *****
 **************************/

class VPath;

#include "VReference.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaSelf;

    class Vca_API VPeerDataArray : public VTransient {
	DECLARE_FAMILY_MEMBERS (VPeerDataArray, VTransient);

    // Construction 
    public:
	VPeerDataArray (VPeerDataArray const &rOther)
	    : m_iPeerDataArray (rOther.m_iPeerDataArray)
	{
	}

	VPeerDataArray () {
	}

    // Destruction
    public:
	~VPeerDataArray() {
	}

    public:
	VPeerData const &operator[] (unsigned int xElement) const {
	    return m_iPeerDataArray[xElement];
	}
	VPeerData &operator[] (unsigned int xElement) {
	    return m_iPeerDataArray[xElement];
	}

	void append (unsigned int cElements) {
	    m_iPeerDataArray.Append (cElements);
	}
	void append (VPeerData const &rPeerData);
	void appendLocalData (VcaSelf* pSelf);

	unsigned int cardinality () const {
	    return m_iPeerDataArray.cardinality();
	}

	bool locatePeer(uuid_t const &peerUUID, unsigned int &xIndex) const {
	    for(unsigned int i=0; i<m_iPeerDataArray.cardinality(); i++)  {
		if(VkUUID::eq(m_iPeerDataArray[i].peerUUID(),peerUUID)) {
		    xIndex = i;
		    return true;
		}
	    }
	    return false;
	}

	unsigned int pathLength() const {
	    unsigned int length = 0;
	    for(unsigned int i=0; i<m_iPeerDataArray.cardinality(); i++)  {
		if(m_iPeerDataArray[i].inFinalPath()) 
		    length++;
	    }
	    return length;
	}

    // sets inFinalPath  (status) of the peer 
    private:
	bool setPeerStatus (uuid_t const &peerUUID, bool inFinalPath) {
	    for(unsigned int i=0; i<m_iPeerDataArray.cardinality(); i++)  {
		if(VkUUID::eq (m_iPeerDataArray[i].peerUUID(),peerUUID))  {
		    m_iPeerDataArray[i].setInFinalPath(inFinalPath);
		    return true;
		}
	    }
	    return false;
	}

    public:
	bool getPeerUUID (unsigned int xIndex, uuid_t  &rUUID) const {
	    if(xIndex < m_iPeerDataArray.cardinality()) {
		rUUID = m_iPeerDataArray[xIndex].peerUUID();
		return true;
	    }
	    return false;
	}
	// Need to thoroughly  check this function
	// returns the next in array(upwards from currentPeer) which is present in the Final Path list

	bool getNextFinalPathPeer(
	    uuid_t const &currentPeer, uuid_t &nextPeer, uuid_t const &targetPeer
	) const {

	    unsigned int xCurrent,xTarget;
	    locatePeer(currentPeer, xCurrent); 
	    locatePeer(targetPeer,  xTarget); 
	    if(xTarget==xCurrent)
		return false;

	    if(xTarget<xCurrent) {
		for(unsigned int i = --xCurrent; i>=xTarget; i--) {
		    if(m_iPeerDataArray[i].inFinalPath()) {
			nextPeer = m_iPeerDataArray[i].peerUUID();;
			return true;
		    }
		}
	    }
	    else {
		for(unsigned int i = xCurrent+1; i<=xTarget; i++) {
		    if(m_iPeerDataArray[i].inFinalPath()) {
			nextPeer = m_iPeerDataArray[i].peerUUID();;
			return true;
		    }
		}
	    }
	    return false;
	}

	bool getNextPeer(
	    uuid_t const &currentPeer, uuid_t  &nextPeer, uuid_t const &targetPeer
	) const {

	    unsigned int xCurrent,xTarget;
	    locatePeer(currentPeer, xCurrent); 
	    locatePeer(targetPeer,  xTarget); 
        
	    if(xTarget == xCurrent)
		return false;
        
	    if(xTarget < xCurrent)
		nextPeer = m_iPeerDataArray[xCurrent-1].peerUUID();
	    else
		nextPeer = m_iPeerDataArray[xCurrent+1].peerUUID();
	    return true;
	}


	// Is direct connection possible between source and destination
	// checks to see whether the final peers are only the source and destination peers
	// if some other peer is in final peer status then direct connection is not possible
    
	bool isDirectConnectionPossible() const {
	    unsigned int size = cardinality();

	    if(m_iPeerDataArray[0].inFinalPath() && m_iPeerDataArray[size-1].inFinalPath()) {
		for(unsigned int i=1; i<size-1; i++) {
		    if(m_iPeerDataArray[i].inFinalPath())
			return false;
		}
		return true;
	    }
	    return false;
	}

    
	// Returns true if the given peers can be directly connected
    
	bool isDirectConnectionPossible(uuid_t const &uuid1, uuid_t const &uuid2) {
	     unsigned int sourceIndex, destIndex;
	     locatePeer (uuid1,sourceIndex);
	     locatePeer (uuid2,destIndex);
 

	     if(sourceIndex > destIndex) {
		 unsigned int temp = destIndex;
		 destIndex = sourceIndex;
		 sourceIndex = temp;
	     }

	     if(m_iPeerDataArray[sourceIndex].inFinalPath() && m_iPeerDataArray[destIndex].inFinalPath()) {
		for(unsigned int i=sourceIndex+1; i<destIndex; i++) {
		    if(m_iPeerDataArray[i].inFinalPath())
			return false;
		}
		return true;
	    }
	    return false;
	}

    public:
	void setFinalPathPeers(uuid_t const &, uuid_t const &);

    // matrix utility
    public:
	VReference<VPath> *index(VReference<VPath> *pMatrix, int x, int y) const {
	    return pMatrix + (x * m_iPeerDataArray.cardinality()) + y; 
	}

    // Testing
    public:
	void display(char const *pWhat) const {
	    printf ("\n+++ %-20.20s\n", pWhat);
	    for(unsigned int i=0; i<m_iPeerDataArray.cardinality(); i++)
		m_iPeerDataArray[i].displayInfo ();
	}

	void populate ();
	static void displayUUID(uuid_t const &uuid);

    // State
    private:
	VkDynamicArrayOf<VPeerData> m_iPeerDataArray;
    };
}


#endif

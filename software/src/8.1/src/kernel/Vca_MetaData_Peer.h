#ifndef Vca_MetaData_Peer_Interface
#define Vca_MetaData_Peer_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_MetaData_SiteObject.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace MetaData {
        /** Represents another Site that a particular Site knows about. */
	class Vca_MetaData_API Peer : public SiteObject {
	    DECLARE_CONCRETE_RTTLITE (Peer, SiteObject);

	//  Construction
	public:
            /**
             * Constructor expects the 'owner' of this peer as well as the remote site.
             *
             * @param pSite the 'owner' of this peer.
             * @param pPeer the remote site that this Peer represents.
             */
	    Peer (Site* pSite, Site* pPeer);

	//  Destruction
	private:
	    ~Peer ();
        
	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
            /** Returns the remote site. */
	    Site* peer () const {
		return m_pPeer;
	    }

	//  State
	private:
            /** The remote site. */
	    Site::Reference const m_pPeer;
	};
    }
}


#endif

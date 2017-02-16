#ifndef Vca_VConnectionRequest_Interface
#define Vca_VConnectionRequest_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IConnectionRequest.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VConnectionRequest : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VConnectionRequest, VRolePlayer);

    //  Construction
    public:
	VConnectionRequest () : m_bDone (false), m_pIConnectionRequest (this) {
	}

    //  Destruction
    private:
	~VConnectionRequest () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IConnectionRequest Role
    private:
	VRole<ThisClass,IConnectionRequest> m_pIConnectionRequest;
    public:
	void getRole (VReference<IConnectionRequest>&rpRole) {
	    m_pIConnectionRequest.getRole (rpRole);
	}

    //  Interface Methods
    public:
	void onListenerAddress (IConnectionRequest *pRole, VString const &pListener);

	void onDone (IConnectionRequest *pRole);

	void onFailure (IConnectionRequest *pRole);

    //  Query
    public:
	bool isDone () const {
	    return m_bDone;
	}

    //  State
    protected:
	bool m_bDone;
    };
}


#endif

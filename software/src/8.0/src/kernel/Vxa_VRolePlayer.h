#ifndef Vxa_VRolePlayer_Interface
#define Vxa_VRolePlayer_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VRolePlayer : public virtual Vca::VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VRolePlayer, Vca::VRolePlayer);

    //  Construction
    protected:
	VRolePlayer (Vca::VCohort *pCohort) : BaseClass (pCohort) {
	}
	VRolePlayer () {
	}

    //  Destruction
    protected:
	~VRolePlayer () {
	}

    //  State
    private:
    };
} //  namespace Vxa


#endif

#ifndef Vca_VActivitySink_Interface
#define Vca_VActivitySink_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VActivityModel.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

/***************************
 *----                 ----*
 *----  VActivitySink  ----*
 *----                 ----*
 ***************************/

    class Vca_API VActivitySink : public virtual VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (VActivitySink, VRolePlayer);

	friend class VActivityModel;

    //  Aliases
    public:
	typedef VActivityModel::ActivityRecord ActivityRecord;
	typedef V::VTime VTime;

    //  Construction
    public:
	VActivitySink (VActivityModel *pModel = 0);

    //  Destruction
    protected:
	~VActivitySink ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IActivitySink
    private:
	VRole<ThisClass,IActivitySink> m_pIActivitySink;
    public:
	void getRole (IActivitySink::Reference& rpRole) {
	    m_pIActivitySink.getRole (rpRole);
	}

    //  IActivitySink Implementation
    public:
	void OnActivity (
	    IActivitySink* pRole, IActivity* pActivity, IActivity* pProxy, VTime const& rStartTime, VTime const& rExpectedEndTime, VString const& rDescription
	);

    //  Access
    public:
	VActivityModel *model () const {
	    return m_pModel;
	}

    //  Monitoring
    public:
	virtual void createMonitorFor (VActivity *pActivity) const;

    //  State
    private:
	VActivityModel::Reference const m_pModel;
    };
}


#endif

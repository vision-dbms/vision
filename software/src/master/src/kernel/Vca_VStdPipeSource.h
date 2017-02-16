#ifndef Vca_VStdPipeSource_Interface
#define Vca_VStdPipeSource_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IStdPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IStdPipeSourceClient;

    class VDeviceFactory;

    class Vca_API VStdPipeSource : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VStdPipeSource, VRolePlayer);

    //  Aliases
    public:
	typedef IStdPipeSource ThisInterface;

    //  Construction
    private:
	VStdPipeSource (VDeviceFactory *pDeviceFactory);

    //  Destruction
    private:
	~VStdPipeSource ();

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex ();

    //  Cohort Instance
    public:
	static bool Supply (Reference &rpInstance);
	static bool Supply (ThisInterface::Reference &rpRole);

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IStdPipeSource Role
    private:
	VRole<ThisClass,IStdPipeSource> m_pIStdPipeSource;
    public:
	void getRole (IStdPipeSource::Reference &rpRole) {
	    m_pIStdPipeSource.getRole (rpRole);
	}

    //  IStdPipeSource Methods
    public:
	void Supply  (IStdPipeSource *pRole, IStdPipeSourceClient *pClient);
	void Supply2 (IStdPipeSource *pRole, IStdPipeSourceClient *pClient);
	void SupplyI (IStdPipeSource *pRole, IStdPipeSourceClient *pClient);
	void SupplyO (IStdPipeSource *pRole, IStdPipeSourceClient *pClient);
	void SupplyE (IStdPipeSource *pRole, IStdPipeSourceClient *pClient);

    //  Local Use
    public:
	enum Std {
	    Std_I, Std_O, Std_E, Std_IO, Std_IOE
	};
	void supply (IStdPipeSourceClient *pClient, Std xWhat = Std_IOE);

    //  State
    protected:
	VReference<VDeviceFactory> const m_pDeviceFactory;
    };
}            


#endif

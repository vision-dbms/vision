#ifndef Vca_VProcessFactory_Interface
#define Vca_VProcessFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IProcessFactory.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VDeviceFactory.h"
#include "Vca_VPipeSource.h"
#include "Vca_VProcess.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkStatus;

class VString;


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VBSConsumer;
    class VBSProducer;

    class VDeviceFactory;

    class VProcess;

    class Vca_API VProcessFactory : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VProcessFactory, VRolePlayer);

    //  Aliases
    public:
	typedef Reference ProcessFactoryReference;
	typedef IProcessFactory ThisInterface;

    //  Wants
    public:
	enum Wants {
	    Wants_Nothing,
	    Wants_Stdin,
	    Wants_Stdout,
	    Wants_Stdin_Stdout,
	    Wants_Stderr,
	    Wants_Stderr_Stdin,
	    Wants_Stderr_Stdout,
	    Wants_Everything
	};
	static bool WantsStdin (Wants xWants) {
	    return xWants & Wants_Stdin ? true : false;
	}
	static bool WantsStdout (Wants xWants) {
	    return xWants & Wants_Stdout ? true : false;
	}
	static bool WantsStderr (Wants xWants) {
	    return xWants & Wants_Stderr ? true : false;
	}
	static Wants WantsDescriptors (bool bWantsStdin, bool bWantsStdout, bool bWantsStderr) {
	    return static_cast<Wants> (
		(bWantsStdin  ? Wants_Stdin  : Wants_Nothing) +
		(bWantsStdout ? Wants_Stdout : Wants_Nothing) +
		(bWantsStderr ? Wants_Stderr : Wants_Nothing)
	    );
	}
	static Wants WantsTwoPipe (bool bTwoPipe) {
	    return bTwoPipe ? Wants_Stdin_Stdout : Wants_Everything;
	}

    //  PipeSource
    public:
	class Vca_API PipeSource : public VPipeSource {
	//  Run Time Type
	    DECLARE_CONCRETE_RTTLITE (PipeSource, VPipeSource);

	//  Construction
	public:
	    PipeSource (
		VProcessFactory *pFactory, VString const &rCommand, Wants xWants
	    );

	//  Destruction
	private:
	    ~PipeSource ();

	//  Access/Query
	public:
	    VString const &command () const {
		return m_iCommand;
	    }
	    VProcessFactory *factory () const {
		return m_pFactory;
	    }
	    bool twoPipe () const {
		return wantsStdin () && wantsStdout () && !wantsStderr ();
	    }
	    bool wantsStdin () const {
		return WantsStdin (m_xWants);
	    }
	    bool wantsStdout () const {
		return WantsStdout (m_xWants);
	    }
	    bool wantsStderr () const {
		return WantsStderr (m_xWants);
	    }

	//  Implementation
	private:
	    void supply_(IPipeSourceClient *pClient) OVERRIDE;

	//  State
	private:
	    ProcessFactoryReference	const	m_pFactory;
	    VString			const	m_iCommand;
	    Wants			const	m_xWants;
	};

    //  Construction
    private:
	VProcessFactory (VDeviceFactory *pDeviceFactory);

    //  Destruction
    private:
	~VProcessFactory ();

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

    //  IProcessFactory Role
    private:
	VRole<ThisClass,IProcessFactory> m_pIProcessFactory;
    public:
	void getRole (IProcessFactory::Reference &rpRole) {
	    m_pIProcessFactory.getRole (rpRole);
	}

    //  IProcessFactory Methods
    public:
	void MakeProcess (
	    IProcessFactory*		pRole,
	    IProcessFactoryClient*	pClient,
	    VString const&		rCommand,
	    bool			bTwoPipe
	);

    //  Local Use
    public:
	static bool Supply (
	    IProcessFactoryClient *pClient, VString const &rCommand, bool bTwoPipe
	) {
	    return Supply (pClient, rCommand, WantsTwoPipe (bTwoPipe));
	}
	static bool Supply (
	    IProcessFactoryClient *pClient, VString const &rCommand, Wants xWants
	);
	bool supply (
	    IProcessFactoryClient *pClient, VString const &rCommand, Wants xWants
	);

	static bool Supply (
	    IPipeSourceClient *pClient, VString const &rCommand, bool bTwoPipe
	) {
	    return Supply (pClient, rCommand, WantsTwoPipe (bTwoPipe));
	}
	static bool Supply (
	    IPipeSourceClient *pClient, VString const &rCommand, Wants xWants
	);
	bool supply (
	    IPipeSourceClient *pClient, VString const &rCommand, Wants xWants
	);

	static bool Supply (
	    PipeSource::Reference &rpPPS, VString const &rCommand, bool bTwoPipe
	) {
	    return Supply (rpPPS, rCommand, WantsTwoPipe (bTwoPipe));
	}
	static bool Supply (
	    PipeSource::Reference &rpPPS, VString const &rCommand, Wants xWants
	);
	bool supply (
	    PipeSource::Reference &rpPPS, VString const &rCommand, Wants xWants
	);

	static bool Supply (
	    VkStatus&			rStatus,
	    VProcess::Reference&	rpProcess,
	    VBSConsumer::Reference&	rpStdBSToPeer,
	    VBSProducer::Reference&	rpStdBSToHere,
	    VBSProducer::Reference&	rpErrBSToHere,	//  & == &rpStdBSToHere -> send to stdout
	    char const*			pCommandLine
	);
	bool supply (
	    VkStatus&			rStatus,
	    VProcess::Reference&	rpProcess,
	    VBSConsumer::Reference&	rpStdBSToPeer,
	    VBSProducer::Reference&	rpStdBSToHere,
	    VBSProducer::Reference&	rpErrBSToHere,	//  & == &rpStdBSToHere -> send to stdout
	    char const*			pCommandLine
	);

	static bool Supply (
	    VkStatus&			rStatus,
	    VProcess::Reference&	rpProcess,
	    VBSConsumer::Reference*	ppStdBSToPeer,	//  null -> null device
	    VBSProducer::Reference*	ppStdBSToHere,	//  null -> null device
	    VBSProducer::Reference*	ppErrBSToHere,	//  null -> null device, ==ppStdBSToHere -> send to stdout
	    char const*			pCommandLine
	);
	bool supply (
	    VkStatus&			rStatus,
	    VProcess::Reference&	rpProcess,
	    VBSConsumer::Reference*	ppStdBSToPeer,	//  null -> null device
	    VBSProducer::Reference*	ppStdBSToHere,	//  null -> null device
	    VBSProducer::Reference*	ppErrBSToHere,	//  null -> null device, ==ppStdBSToHere -> send to stdout
	    char const*			pCommandLine
	);

    //  State
    protected:
	VDeviceFactory::Reference const m_pDeviceFactory;
    };
}


#endif

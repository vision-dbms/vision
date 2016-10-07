#ifndef Vca_VFilterPipeSourceTransaction__Interface
#define Vca_VFilterPipeSourceTransaction__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPipeSourceClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPipeSource.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template<class Factory> class VFilterPipeSourceTransaction_ : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VFilterPipeSourceTransaction_<Factory>, VRolePlayer);

    //  Factory Data
    public:
	typedef typename Factory::PipeSourceData PipeSourceData;

    //  Construction
    public:
	VFilterPipeSourceTransaction_(
	    IPipeSourceClient *pClient, Factory *pFactory, PipeSourceData const &rData, IPipeSource *pPipeSource
	) : m_pClient (pClient), m_iPipeSourceData (rData), m_pFactory (pFactory), m_pIPipeSourceClient (this) {
	    retain (); {
		VReference<IPipeSourceClient> pRole;
		getRole (pRole);
		pPipeSource->Supply (pRole);
	    } untain ();
	}

    //  Destruction
    private:
	~VFilterPipeSourceTransaction_() {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IPipeSourceClient Role
    private:
	VRole<ThisClass,IPipeSourceClient> m_pIPipeSourceClient;
    public:
	void getRole (VReference<IPipeSourceClient>&rpRole) {
	    m_pIPipeSourceClient.getRole (rpRole);
	}

    //  IPipeSourceClient Methods
    public:
	void OnData (
	    IPipeSourceClient *pRole, VBSConsumer *pBSConsumer, VBSProducer *pBSProducer, VBSProducer *pErrorBSProducer
	) {
	    m_pFactory->supply (m_pClient, m_iPipeSourceData, pBSProducer, pBSConsumer);
	}

    //  IClient Methods
    public:
	void OnError (IClient *pRole, IError *pError, VString const &rMessage) {
	    m_pClient->OnError (pError, rMessage);
	}
	void OnEnd (IClient *pRole) {
	    m_pClient->OnEnd ();
	}

    //  State
    private:
	VReference<IPipeSourceClient>	const m_pClient;
	VReference<Factory>		const m_pFactory;
	PipeSourceData			const m_iPipeSourceData;
    };
}


#endif

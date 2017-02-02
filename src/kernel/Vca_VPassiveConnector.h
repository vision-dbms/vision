#ifndef Vca_VPassiveConnector_Interface
#define Vca_VPassiveConnector_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VcaListener.h"

#include "Vca_Registry_IUpdate.h"
#include "Vca_Registry_VRegistry.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VPassiveConnector : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VPassiveConnector, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<VString const&> INameSink;
	typedef IDataSource<VString const&> INameSource;

    //  Construction
    private:
	VcaListener::Reference newListener (IListener *pListener);
    public:
	VPassiveConnector (IListener *pListener);

    //  Destruction
    public:
	~VPassiveConnector ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  INameSource
    private:
	VRole<ThisClass,INameSource> m_pINameSource;
    public:
	void getRole (INameSource::Reference &rpRole) {
	    m_pINameSource.getRole (rpRole);
	}

    //  INameSource Methods
    public:
	void Supply (INameSource *pRole, INameSink *pSink);

    //  IPassiveConnector
    private:
	VRole<ThisClass,IPassiveConnector> m_pIPassiveConnector;
    public:
	void getRole (IPassiveConnector::Reference &rpRole) {
	    m_pIPassiveConnector.getRole (rpRole);
	}

    //  IPassiveConnector Methods
    public:
	void RequestCallback (
	    IPassiveConnector *pRole,
	    IPassiveCallbackReceiver *pCallbackSink,
	    IPassiveServer *pServer,
	    U64 sMicrosecondTimeout
	);

    //  Registry::IUpdate
    private:
	VRole<ThisClass,Registry::IUpdate> m_pRegistry_IUpdate;
    public:
	void getRole (Registry::IUpdate::Reference &rpRole) {
	    m_pRegistry_IUpdate.getRole (rpRole);
	}

    //  Registry::IUpdate Methods
    public:
	void AddObject (
	    Registry::IUpdate *pRole, Registry::IUpdater* pUpdater, IVUnknown* pObject
	);

    //  Name Query
    public:
	template <typename callback_t> void getName (callback_t const &rCallback) {
	    m_pListener->getName (rCallback);
	}
	template <typename class_t> void getName (
	    class_t *pConsumer,
	    void (class_t::*pDataMember)(VString const&),
	    void (class_t::*pErrorMember) (IError*, VString const&)
	) {
	    m_pListener->getName (pConsumer, pDataMember, pErrorMember);
	}
	void getName (VcaListener::INameSink *pSink) {
	    m_pListener->getName (pSink);
	}

    //  State
    private:
	Registry::VRegistry::Reference const m_pRegistry;
	VcaListener::Reference const m_pListener; // must follow contact manager
    };
}


#endif

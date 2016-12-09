#ifndef Vca_VConnection_Interface
#define Vca_VConnection_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IConnection.h"
#include "Vca_IDataSource.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*************************
 *----  VConnection  ----*
 *************************/

namespace Vca {
    class VDevice;

    class Vca_API VConnection : public VRolePlayer {
    //  Family
	DECLARE_CONCRETE_RTTLITE (VConnection, VRolePlayer);

    //  Byte Stream Source Types
    public:
	typedef IDataSource<VBSConsumer*> IVBSConsumerSource;
	typedef IDataSource<VBSProducer*> IVBSProducerSource;

    //  Byte Stream Sink Types
    public:
	typedef IVReceiver<VBSConsumer*> IVBSConsumerSink;
	typedef IVReceiver<VBSProducer*> IVBSProducerSink;

    //  Construction
    public:
	VConnection (VDevice *pDevice);

    //  Destruction
    protected:
	~VConnection ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IConnection Role
    private:
	VRole<ThisClass,IConnection> m_pIConnection;
    public:
	void getRole (VReference<IConnection>&rpRole) {
	    m_pIConnection.getRole (rpRole);
	}

    //  IVBSConsumerSource Role
    private:
	VRole<ThisClass,IVBSConsumerSource> m_pIVBSConsumerSource;
    public:
	void getRole (VReference<IVBSConsumerSource>&rpRole) {
	    m_pIVBSConsumerSource.getRole (rpRole);
	}

    //  IVBSProducerSource Role
    private:
	VRole<ThisClass,IVBSProducerSource> m_pIVBSProducerSource;
    public:
	void getRole (VReference<IVBSProducerSource>&rpRole) {
	    m_pIVBSProducerSource.getRole (rpRole);
	}

    //  Access
    public:
	VDevice *device () const {
	    return m_pDevice;
	}

    //  Local Suppliers
    public:
	bool supply (
	    VReference<VBSProducer>&rpBSProducer, VReference<VBSConsumer>&rpBSConsumer
	);
	bool supply (VReference<VBSProducer>&rpBSProducer);
	bool supply (VReference<VBSConsumer>&rpBSConsumer);

	bool supply (VReference<IConnection>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}
	bool supply (VReference<IVBSConsumerSource>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}
	bool supply (VReference<IVBSProducerSource>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}

    //  Interface Methods
    public/*private*/:
	void SupplyBSConsumer (IConnection *pRole, IVBSConsumerSink *pSink);
	void SupplyBSProducer (IConnection *pRole, IVBSProducerSink *pSink);

	void Supply (IVBSConsumerSource *pRole, IVBSConsumerSink *pSink);
	void Supply (IVBSProducerSource *pRole, IVBSProducerSink *pSink);

    //  State
    private:
	VReference<VDevice> const m_pDevice;
    };
}


#endif

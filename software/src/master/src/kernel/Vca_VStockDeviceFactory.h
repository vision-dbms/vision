#ifndef Vca_VStockDeviceFactory_Interface
#define Vca_VStockDeviceFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IConnection.h"
#include "Vca_IListener.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VConnection;
    class VListener;

    class VDevice;

    class Vca_API VStockDeviceFactory : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VStockDeviceFactory, VRolePlayer);

    //  Sources and Sinks
    public:
	typedef IDataSource<IConnection*> IConnectionSource;
	typedef IDataSource<VBSConsumer*> IBSConsumerSource;
	typedef IDataSource<VBSProducer*> IBSProducerSource;
	typedef IDataSource<IListener*>   IListenerSource;
	typedef IVReceiver<IConnection*> IConnectionSink;
	typedef IVReceiver<VBSConsumer*> IBSConsumerSink;
	typedef IVReceiver<VBSProducer*> IBSProducerSink;
	typedef IVReceiver<IListener*>   IListenerSink;

    //  Abstract Device
    public:
	class AbstractDevice;

    //  Construction
    protected:
	VStockDeviceFactory ();

    //  Destruction
    protected:
	~VStockDeviceFactory ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IBSConsumerSource Role
    private:
	VRole<ThisClass,IBSConsumerSource> m_pIBSConsumerSource;
    public:
	void getRole (VReference<IBSConsumerSource> &rpRole) {
	    m_pIBSConsumerSource.getRole (rpRole);
	}

    //  IBSProducerSource Role
    private:
	VRole<ThisClass,IBSProducerSource> m_pIBSProducerSource;
    public:
	void getRole (VReference<IBSProducerSource> &rpRole) {
	    m_pIBSProducerSource.getRole (rpRole);
	}

    //  IConnectionSource Role
    private:
	VRole<ThisClass,IConnectionSource> m_pIConnectionSource;
    public:
	void getRole (VReference<IConnectionSource> &rpRole) {
	    m_pIConnectionSource.getRole (rpRole);
	}

    //  IListenerSource Role
    private:
	VRole<ThisClass,IListenerSource> m_pIListenerSource;
    public:
	void getRole (VReference<IListenerSource> &rpRole) {
	    m_pIListenerSource.getRole (rpRole);
	}

    //  IPipeSource Role
    private:
	VRole<ThisClass,IPipeSource> m_pIPipeSource;
    public:
	void getRole (VReference<IPipeSource> &rpRole) {
	    m_pIPipeSource.getRole (rpRole);
	}

    //  IBSConsumerSource Methods
    public/*private*/:
	void Supply (IBSConsumerSource *pRole, IBSConsumerSink *pClient);

    //  IBSProducerSource Methods
    public/*private*/:
	void Supply (IBSProducerSource *pRole, IBSProducerSink *pClient);

    //  IConnectionSource Methods
    public/*private*/:
	void Supply (IConnectionSource *pRole, IConnectionSink *pClient);

    //  IListenerSource Methods
    public/*private*/:
	void Supply (IListenerSource *pRole, IListenerSink *pClient);

    //  IPipeSource Methods
    public/*private*/:
	void Supply (IPipeSource *pRole, IPipeSourceClient *pClient);

    //  Local Use
    private:
	virtual bool supply_(VReference<VDevice>&rpDevice) = 0;
    public:
	bool supply (VReference<VDevice>&rpDevice) {
	    return supply_(rpDevice);
	}
	bool supply (
	    VReference<VBSProducer>&rpBSProducer, VReference<VBSConsumer>&rpBSConsumer
	);
	bool supply (VReference<VBSProducer>&rpBSProducer);
	bool supply (VReference<VBSConsumer>&rpBSConsumer);
	bool supply (VReference<VConnection>&rpConnection);
	bool supply (VReference<IConnection>&rpConnection);
	bool supply (VReference<VListener>&rpListener);
	bool supply (VReference<IListener>&rpListener);

	void supply (IBSConsumerSink *pClient);
	void supply (IBSProducerSink *pClient);
	void supply (IConnectionSink *pClient);
	void supply (IPipeSourceClient *pClient);
	void supply (IListenerSink *pClient);
    };
}


#endif

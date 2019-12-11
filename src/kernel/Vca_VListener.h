#ifndef Vca_VListener_Interface
#define Vca_VListener_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vca_VDevice.h"

#include "Vca_IDataSource.h"
#include "Vca_IListener.h"
#include "Vca_IPipeSource.h"

#include "V_VQueue.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IConnection.h"


/*************************
 *****  Definitions  *****
 *************************/

/***********************
 *----  VListener  ----*
 ***********************/

namespace Vca {
    class VStatus;

    class Vca_API VListener : public VRolePlayer, private VDevice::Listener {
	DECLARE_CONCRETE_RTTLITE (VListener, VRolePlayer);

    //  Device Types
    public:
	typedef VDeviceListenFace DeviceFace;
	typedef VDeviceListener   DeviceUser;

    //  Connection Receiver/Source
    public:
	typedef IVReceiver<IConnection*> IConnectionSourceClient;
	typedef IDataSource<IConnection*> IConnectionSource;

    //  Request
    public:
	class Request : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Request, VTransient);

	//  Type
	public:
	    enum Type {
		Type_None, Type_Listener, Type_ConnectionSource, Type_PipeSource
	    };

	//  Construction
	public:
	    Request (IListenerClient *pClient, size_t cConnections = 1);
	    Request (IConnectionSourceClient *pClient, size_t cConnections = 1);
	    Request (IPipeSourceClient *pClient, size_t cConnections = 1);
	    Request (Request const &rOther);
	    Request ();

	//  Destruction
	public:
	    ~Request () {
	    }

	//  Access
	public:
	    IClient *client () const {
		return m_pClient;
	    }
	    size_t connections () const {
		return m_cConnections;
	    }
	    Type type () const {
		return m_xType;
	    }

	//  Query
	public:
	    bool isSatisfied () const {
		return m_cConnections == 0;
	    }
	    bool isntSatisfied () const {
		return m_cConnections > 0;
	    }

	//  Update
	public:
	    void clear () {
		m_xType = Type_None;
		m_pClient.clear ();
		m_cConnections = 0;
	    }
	    void setTo (IListenerClient *pClient, size_t cConnections = 1);
	    void setTo (IConnectionSourceClient *pClient, size_t cConnections = 1);
	    void setTo (IPipeSourceClient *pClient, size_t cConnections = 1);

	    void setTo (Request const &rOther);

	    Request &operator= (Request const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  Client Communication
	public:
	    void onDevice (VDevice *pDevice);
	    bool onStatus (VStatus const &rStatus) const;

	//  State
	public:
	    Type		m_xType;
	    VReference<IClient> m_pClient;
	    size_t		m_cConnections;
	};

    //  Construction
    public:
	VListener (DeviceFace *pFace);

    //  Destruction
    protected:
	~VListener () {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IConnectionSource Role
    private:
	VRole<ThisClass,IConnectionSource> m_pIConnectionSource;
    public:
	void getRole (VReference<IConnectionSource> &rpRole) {
	    m_pIConnectionSource.getRole (rpRole);
	}

    //  IListener Role
    private:
	VRole<ThisClass,IListener> m_pIListener;
    public:
	void getRole (VReference<IListener> &rpRole) {
	    m_pIListener.getRole (rpRole);
	}

    //  IPipeSource Role
    private:
	VRole<ThisClass,IPipeSource> m_pIPipeSource;
    public:
	void getRole (VReference<IPipeSource> &rpRole) {
	    m_pIPipeSource.getRole (rpRole);
	}

    //  IBinding Methods
    public/*private*/:
	void GetNamespace (IBinding *pRole, IVReceiver<INamespace*> *pNamespaceReceiver);
	void GetName (IBinding *pRole, IVReceiver<VString const&> *pNameReceiver);

    //  IConnectionSource Methods
    public/*private*/:
	void Supply (IConnectionSource *pRole, IConnectionSourceClient *pClient);

    //  IPipeSource Methods
    public/*private*/:
	void Supply (IPipeSource *pRole, IPipeSourceClient *pClient);

    //  IListener Methods
    public/*private*/:
	void SupplyByteStreams (
	    IListener *pRole, IPipeSourceClient *pClient, U32 cConnections
	);
	void SupplyConnections (
	    IListener *pRole, IListenerClient *pClient, U32 cConnections
	);

    //  IResource Methods
    public/*private*/:
	void Close (IResource *pRole);

    //  Access/Query
    private:
	bool active () const {
	    return m_iQueue.active ();
	}
	Request const &activeRequest () const {
	    return m_iQueue.activeRequest ();
	}
	Request &activeRequest () {
	    return m_iQueue.activeRequest ();
	}

    //  Local Use/Interface Supply
    public:
	void SupplyByteStreams (
	    IPipeSourceClient *pClient, U32 cConnections
	);
	void SupplyConnections (
	    IListenerClient *pClient, U32 cConnections
	);
	bool supply (VReference<IConnectionSource>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}
	bool supply (VReference<IListener>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}
	bool supply (VReference<IPipeSource>&rpInterface) {
	    getRole (rpInterface);
	    return true;
	}

    //  Callbacks
    private:
	virtual void trigger_() OVERRIDE {
	    acceptConnections ();
	}

    //  Completion
    private:
	virtual void onDevice_(VDevice *pDevice) OVERRIDE;

    //  Request Processing
    private:
	void acceptConnections ();
	bool nextRequest ();

    //  State
    private:
	V::VQueue<Request> m_iQueue;
    };
}


#endif

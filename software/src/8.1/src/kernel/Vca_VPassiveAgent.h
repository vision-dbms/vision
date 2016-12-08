#ifndef Vca_VPassiveAgent_Interface
#define Vca_VPassiveAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPassiveCall.h"
#include "Vca_IPassiveServer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VGoferInterface.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VPassiveAgent : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VPassiveAgent, VRolePlayer);

    //  Aliases
    public:
	typedef Reference AgentReference;
	typedef VGoferInterface<IPassiveServer> PassiveServerGofer;

    //  Agency
    public:
	class Agency;
	friend class Agency;

    //  CallbackRequest
    public:
	class Vca_API CallbackRequest {
	    DECLARE_NUCLEAR_FAMILY (CallbackRequest);

	//  Construction
	public:
	    CallbackRequest (char const *pRequestData);
	    CallbackRequest (ThisClass const &rOther);

	//  Destruction
	public:
	    ~CallbackRequest ();

	//  Access
	public:
	    VString const &callerAddress () const {
		return m_iCallerAddress;
	    }
	    uuid_t const &callID () const {
		return m_iCallID;
	    }
	    void getCallIDAsString (VString &rString) const {
		m_iCallID.GetString (rString);
	    }
	    bool isValid () const {
		return m_bValid;
	    }
	    bool isntValid () const {
		return !m_bValid;
	    }

	//  Update
	public:
	    void setTo (char const *pRequestData);

	//  State
	private:
	    VString		m_iCallerAddress;
	    VkUUIDHolder	m_iCallID;
	    bool		m_bValid;
	};

    //  CallData
    public:
	class Vca_API CallData {
	    DECLARE_NUCLEAR_FAMILY (CallData);

	    friend class Agency;

	//  Construction
	protected:
	    CallData (IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID);
	public:
	    CallData (ThisClass const &rOther);

	//  Destruction
	public:
	    ~CallData ();

	//  Access
	public:
	    VString const &callerAddress () const {
		return m_iCallerAddress;
	    }
	    uuid_t const &callID () const {
		return m_iCallID;
	    }
	    void getCallIDAsString (VString &rString) const {
		m_iCallID.GetString (rString);
	    }

	//  Use
	public:
	    void OnEnd () const;
	    void OnError (IError *pInterface, VString const &rMessage) const;

	//  State
	private:
	    IPassiveCall::Reference	const m_pCall;
	    VString			const m_iCallerAddress;
	    VkUUIDHolder		const m_iCallID;
	};

    //  Construction
    protected:
	VPassiveAgent ();

    //  Destruction
    protected:
	~VPassiveAgent ();

    //  Access
    protected:
	bool supplyPassiveServerGofer (PassiveServerGofer::Reference &rpGofer);

    //  Use
    private:
	virtual void getSuccessor (AgentReference &rpSuccessor) = 0;
	virtual void processCallbackRequest (CallData const &rCallData) = 0;
    };
}


#endif

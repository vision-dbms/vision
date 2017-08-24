#ifndef Vca_VNotifier_Interface
#define Vca_VNotifier_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_IInfoServer.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkStatus;

#include "VTransientServices.h"
#include "Vca_VGoferInterface.h"

/*************************
 *****  Definitions  *****
 *************************/


namespace Vca {

    class Vca_API InfoDataObject : public VRolePlayer {
	DECLARE_CONCRETE_RTT(InfoDataObject, VRolePlayer);
	
	InfoDataObject (bool bWaitingForResp, VGoferInterface<IInfoServer> *pInfoServerGofer, VString const & rTarget, int iPriority, char const *pFormat, va_list ap);
	
    private:
	~InfoDataObject ();

    public:
    //  IClient Role
    private:
	VRole<ThisClass,IClient> m_pIClient;
    public:
	void getRole (IClient::Reference &rpRole) {
	    m_pIClient.getRole (rpRole);
	}

    private:
//        void OnEnd_();

    public:	
	void onInfoServer (Vca::IInfoServer *pInfoServer);
	void onInfoServerError (Vca::IError *pError, VString const &rMsg) {
	}

    private:
	VString m_iTarget;
	int m_iPriority;
	VString m_pContent;
	bool m_bWaitingForResp;
    };
 
    class Vca_API Notifier : public VReferenceable {
	DECLARE_CONCRETE_RTT (Notifier, VReferenceable);

    public:
	Notifier (VGoferInterface<IInfoServer> *pInfoServerGofer); 

    private:
	~Notifier ();

    public:
	void notify (VString const & rTarget, int iPriority, char const *pFormat, ...) {
	    V_VARGLIST (ap, pFormat);
	    notify (false, rTarget, iPriority, pFormat, ap);
	}
	void notify (bool bWaitingForResp, VString const & rTarget, int iPriority, char const *pFormat, ...) {
	    V_VARGLIST (ap, pFormat);
	    notify (bWaitingForResp, rTarget, iPriority, pFormat, ap);
	}
	void notify (bool bWaitingForResp, VString const & rTarget, int iPriority, char const *pFormat, va_list ap) {
	    InfoDataObject::Reference const pInfoDataObj (new InfoDataObject (bWaitingForResp, m_pGofer, rTarget, iPriority, pFormat, ap));
	}
    private:
	VGoferInterface<IInfoServer>::Reference m_pGofer;
    };


    class Vca_API VTransientServicesForNotification : public VTransientServices {
 
	DECLARE_FAMILY_MEMBERS (VTransientServicesForNotification, VTransientServices);

	//  Construction
    public:
	VTransientServicesForNotification () {
	}

	//  Destruction
    public:
	~VTransientServicesForNotification () {
	}

    public:
    //  Notification
	void notify (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap) {
            notify_ (bWaitingForResp, xEvent, pFormat, ap);
        }

    private:
        void notify_ (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap);
    };

}

#endif

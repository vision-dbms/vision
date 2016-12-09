#ifndef Vca_VBSProducerBitBucket_Interface
#define Vca_VBSProducerBitBucket_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IBSClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSProducer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VBSProducerBitBucket : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VBSProducerBitBucket, VRolePlayer);

    //  Construction
    public:
	VBSProducerBitBucket (VBSProducer *pBS);

    //  Destruction
    private:
	~VBSProducerBitBucket () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IBSClient Role
    private:
	VRole<ThisClass,IBSClient> m_pIBSClient;
    protected:
	void getRole (IBSClient::Reference &rpRole) {
	    m_pIBSClient.getRole (rpRole);
	}

    //  IBSClient Methods
    public:
	void OnTransfer (IBSClient *pRole, size_t sTransfer);

    //  IClient Methods
    public:
	void OnError (IClient *pRole, IError *pError, VString const &rMessage);
	void OnEnd (IClient *pRole);

    //  State
    private:
	VBSProducer::Reference m_pBS;
	char m_iBucketBuffer[1024];
    };
}


#endif

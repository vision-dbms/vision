#ifndef VUtilities_ZlibDecompressor_Interface
#define VUtilities_ZlibDecompressor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBSProducer.h"
#include "V_VBlob.h"

#include "VUtilities.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*****************************************
 *----  VUtlities::ZlibDecompressor  ----*
 *****************************************/

namespace VUtilities {
    
    class VUtilities_API ZlibDecompressor : public Vca::VBSProducer {
	DECLARE_CONCRETE_RTT (ZlibDecompressor, Vca::VBSProducer);

    //  Construction
    public:
	ZlibDecompressor (Vca::VBSProducer *pProducer);

    //  Destruction
    private:
	~ZlibDecompressor ();

    //  IBSClient Role
    private:
	Vca::VRole<ThisClass,Vca::IBSClient> m_pIBSClient;

    public:
	using BaseClass::getRole;
	void getRole (VReference<Vca::IBSClient>&rpRole) {
	    m_pIBSClient.getRole (rpRole);
	}

    //  IBSClient Methods
    public/*private*/:

	void OnTransfer (Vca::IBSClient *pRole, size_t sTransfer) {
	    if (m_pIBSClient.supplies (pRole)) {
		onTransfer (sTransfer);
		onGetContinuation ();
	    }
	}

	void OnEnd (Vca::IClient *pRole) {
	    if (m_pIBSClient.supplies (pRole))
		onEnd ();
	}

	void OnError (Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage) {
	    if (m_pIBSClient.supplies (pRole))
		onError (pError, rMessage);
	}

	void onTransfer (size_t sTransfer);

    //  Control
    private:
	void endTransfers () {
	    m_pProducer->Close ();
	}

    //  Data Transfer
    private:
	void transferData (); 
	void decompressBlobToBuffer (); 

    //  Init
    private:
	void init ();

    //  State
    private:
	VReference<Vca::VBSProducer> const	m_pProducer;

	char*				m_pOriginalArea;
	size_t				m_sOriginalSize;
	
	V::VBlob			m_iBlob;
	z_stream			m_iState;

    };
}

#endif

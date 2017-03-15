#ifndef VUtilities_ZlibCompressor_Interface
#define VUtilities_ZlibCompressor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBSConsumer.h"
#include "V_VBlob.h"

#include "VUtilities.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/***************************************
 *----  VUtilities::ZlibCompressor  ----*
 ***************************************/

namespace VUtilities {

    class VUtilities_API ZlibCompressor : public Vca::VBSConsumer {
    DECLARE_CONCRETE_RTT (ZlibCompressor, Vca::VBSConsumer);

    //  Construction
    public:
	ZlibCompressor (Vca::VBSConsumer *pConsumer)
	    : m_pOriginalArea (0), m_sOriginalSize (0)
	    , m_pData (0), m_sData (0), m_sVoid (0)
	    , m_pConsumer (pConsumer), m_pIBSClient (this)
	    , m_iBlob (4096), m_bPending (false) {
	    init ();
	}

    //  Destruction
    private:
	~ZlibCompressor () {
	}

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
		onPutContinuation ();
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
	    m_pConsumer->Close ();
	}

    //  Data Transfer
    private:
	void transferData ();
	Vca::U32 compressBufferToBlob (); 
	void flushPendingData ();

    //  Init
    private:
	void init ();

    //  State
    private:
	VReference<Vca::VBSConsumer> const	m_pConsumer;
	
	char const*			m_pOriginalArea;
	size_t				m_sOriginalSize;
	
	V::VBlob			m_iBlob;
	char const*			m_pData;    //  ... outgoing from compressed Blob
	size_t				m_sData;    //  ... outgoing from compressed Blob
	size_t				m_sVoid;    //  ... incoming from consumer

	z_stream			m_iState;
	bool				m_bPending;
    };
}

#endif

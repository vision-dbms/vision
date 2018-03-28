/*****  Connection Class Definition  *****/
#ifndef VConnection_Interface
#define VConnection_Interface

/************************
 *****  Components  *****
 ************************/

#include "VAccessString.h"
#include "VTransient.h"
#include "Vsa_IEvaluator.h"
#include "Vca_VInterfaceEKG.h"
#include "VReceiverWaiter.h"
#include "V_VReferenceable_.h"

/**************************
 *****  Declarations  *****
 **************************/

class VFormatSettings;
class VDatum;


/*************************
 *****  Definitions  *****
 *************************/

class VConnection : public VReferenceable {
    DECLARE_CONCRETE_RTTLITE (VConnection, VReferenceable);
    DECLARE_COM_WRAPPABLE (IConnection);

    typedef V::VString VString;
    typedef Vca::IDataSource<Vsa::IEvaluator*> IEvaluatorSource;
    typedef VReceiverWaiter<VkDynamicArrayOf <VString> const &, V::VReferenceable_<VkDynamicArrayOf<VString> > > EntryNamesReceiver;
    typedef VReceiverWaiter<VString const&, V::VReferenceable_<VString> > EntryDescriptionReceiver;

/*****  Friends  *****/
    friend class VSockets;
    friend class VConnectionUse;

/*****  Socket Manager  *****/
public:
    static VSockets SocketManager;

//  Constants
public:
    static Vca::U32 g_cPortNumber;
    static size_t g_sMaxAdaptiveReallocationIncrement;
    static size_t g_sAdaptiveReallocationDelay;
    static size_t g_sAdaptiveReallocationMultiplier;
    enum VConnection_ConnectionTypes {
        ConnectionType_Plain, /**< Connection type for plaintext connections (direct connection via OpenVision). */
        ConnectionType_SSH, /**< Connection type for ssh connections. */
        ConnectionType_Rexec, /** Connection type for Rexec connections. */
        ConnectionType_Session /** Connection type for Session connections. */
    };

/*****  Construction/Destruction  *****/
public:
    VConnection ();

protected:
    ~VConnection ();

/// @name IEvaluator EKG
/// We use an EKG to handle unexpected disconnects.
//@{
public:
	/** Callback invoked when the IEvaluator EKG flatlines. */
	void signalIEvaluatorEKG ();
private:
	/** Cancels the IEValuator EKG. */
	void cancelIEvaluatorEKG ();

	/** IEvaluator EKG template instantiation. */
	typedef Vca::VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;

	/** Keeps track of the IEvaluator interface, notifies us upon disconnect. */
	interface_monitor_t::Reference m_pIEvaluatorEKG;
//@}

/*****  Access  *****/
public:
    bool getSessionNames (VAccessString &rSessionNames);
    VFormatSettings* formatSettings () const {
	return m_pFormatSettings;
    }

/*****  Remote Execution  *****/
public:
    bool login (
	VAccessString & rErrorMessage,
	VAccessString const & rHostName,
	VAccessString const & rUserName,
	VAccessString const & rPassword,
        VAccessString const & rCommand
    );

/*****  Connection  *****/
public:
    bool connect (char const *pHostName, u_short xHostPort);

/*****  Disconnection  *****/
public:
    void disconnect ();

/*****  Request Submission  *****/
/// @name Request Submission
//@{
public:
    bool include (char const* pIncludeFileName);
    bool include (char const* pIncludeFileName, VString& iIncludeReply);

    bool execute (
	char const*	pRequest,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );

    bool submit (
	VString&	iReply,
	char const*	pRequest,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );

    bool getValue (
	VDatum&		iResult,
	char const*	pEntityType,
	char const*	pEntity,
	char const*	pItem,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );

    bool getValue (
	VString&	iResult,
	char const*	pEntityType,
	char const*	pEntity,
	char const*	pItem,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );

    bool getValue (
	double&		iResult,
	char const*	pEntityType,
	char const*	pEntity,
	char const*	pItem,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );
    
    bool getValue (
	int&		iResult,
	char const*	pEntityType,
	char const*	pEntity,
	char const*	pItem,
	char const*	pDate = 0,
	char const*	pCurrency = 0
    );
//@}

/*****  Query  *****/
public:
    bool isntConnected () const {
	return !isConnected ();
    }
    bool isConnected () const {
	return m_pIEvaluator.isntNil ();
    }

/*****  Attachment *****/
protected:
    void attach (VConnectionUse *pUse);
    void detach (VConnectionUse *pUse);

/*****  Setup  *****/
protected:
    void setupConnection ();

/*****  State  *****/
protected:
    VReference<VFormatSettings>	const	m_pFormatSettings;

    VConnectionUse*			m_pUseListHead;

    /** The evaluator interface to use, if there is one. */
    Vsa::IEvaluator::Reference          m_pIEvaluator;
};

#endif

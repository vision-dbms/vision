/*****  Vca_VcaGofer Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VcaGofer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif

#include "Vca_IDirectory.h"
#include "Vca_IPipeSource.h"
#include "Vca_IPipeSourceClient.h"

#include "Vca_VConnectionFactory.h"

#include "Vca_VcaConnection.h"

#ifdef __VMS
#include "find_symbol.h" //  omit:Linux omit:SunOS
#endif


/**************************************
 **************************************
 *****                            *****
 *****  Vca::Gofer::PipeReceiver  *****
 *****                            *****
 **************************************
 **************************************/

namespace Vca {
    namespace Gofer {
	class PipeReceiver : public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (PipeReceiver, VRolePlayer);

	//  Construction
	public:
	    PipeReceiver (VInterfaceQuery const &rQuery, VcaOffer::Offering const &rCounterOffer);

	//  Destruction
	private:
	    ~PipeReceiver () {
	    }

	//  Base Roles
	public:
	    using BaseClass::getRole;

	//  IPipeSourceClient Role
	private:
	    VRole<ThisClass,IPipeSourceClient> m_pIPipeSourceClient;
	public:
	    void getRole (IPipeSourceClient::Reference &rpRole) {
		m_pIPipeSourceClient.getRole (rpRole);
	    }

	//  IPipeSourceClient Methods
	public/*private*/:
	    void OnData (
		IPipeSourceClient *pRole,
		VBSConsumer *pStdBSToPeer,
		VBSProducer *pStdBSToHere,
		VBSProducer *pErrBSToHere
	    );

	//  IClient Methods
	public/*private*/:
	    void OnError (IClient *pRole, IError *pIError, VString const &rMessage) OVERRIDE;
	    void OnEnd (IClient *pRole) OVERRIDE;

	//  Use
	public:
	    void retrieveFrom (IPipeFactory *pFactory, VString const &rObjectName);
	    void retrieveFrom (IPipeSource *pSource);

	//  State
	private:
	    VInterfaceQuery	const m_iQuery;
	    VcaOffer::Offering	const m_iCounter;
	};
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Gofer::PipeReceiver::PipeReceiver (
    VInterfaceQuery const &rQuery, VcaOffer::Offering const &rCounter
) : m_iQuery (rQuery), m_iCounter (rCounter), m_pIPipeSourceClient (this) {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Gofer::PipeReceiver::OnData (
    IPipeSourceClient *pRole,
    VBSConsumer *pBSToPeer,
    VBSProducer *pBSToHere,
    VBSProducer *pErrorBSToHere
) {
    VcaConnection::Reference pConnection (new VcaConnection (pBSToHere, pBSToPeer, m_iCounter));
    pConnection->queryService (m_iQuery);
}

void Vca::Gofer::PipeReceiver::OnError (
    IClient *pRole, IError *pIError, VString const &rMessage
) {
    m_iQuery.onError (pIError, rMessage);
}

void Vca::Gofer::PipeReceiver::OnEnd (IClient *pRole) {
    m_iQuery.onEnd ();
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::Gofer::PipeReceiver::retrieveFrom (
    IPipeFactory *pPipeFactory, VString const &rObjectName
) {
    IPipeSourceClient::Reference pRole;
    getRole (pRole);
    pPipeFactory->MakeConnection (pRole, rObjectName, true);
}

void Vca::Gofer::PipeReceiver::retrieveFrom (IPipeSource *pPipeSource) {
    IPipeSourceClient::Reference pRole;
    getRole (pRole);
    pPipeSource->Supply (pRole);
}


/************************
 ************************
 *****              *****
 *****  Vca::Gofer  *****
 *****              *****
 ************************
 ************************/

namespace {
    bool TracingGofers () {
	return Vca::VGofer::TracingGofers ();
    }
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, offering_t const &rCounter, VString const &rObjectName, IDirectory *pDirectory) {
    if (TracingGofers ()) {
	fprintf (stderr, "+++ Gofer Named Object %s\n", rObjectName.content ());
    }

    IDirectory::Reference pNamespace (pDirectory);
    if (pNamespace.isNil ())
	GetRootDirectory (pNamespace);

    rQuery.getObject (rObjectName, pNamespace);
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, offering_t const &rCounter, VString const &rObjectName, IPipeFactory *pPipeFactory) {
    if (TracingGofers ()) {
	fprintf (stderr, "+++ Gofer Piped Object %s\n", rObjectName.content ());
    }

    IPipeFactory::Reference pNamespace (pPipeFactory);
    if (pNamespace.isNil ())
	VConnectionFactory::Supply (pNamespace);

    PipeReceiver::Reference pPipeReceiver (new PipeReceiver (rQuery, rCounter));
    pPipeReceiver->retrieveFrom (pNamespace, rObjectName);
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, offering_t const &rCounter, VString const &rObjectName, VString const &rLibraryName) {
    typedef IVUnknown::Reference (*routine_t)();
#ifdef __VMS
    routine_t pRoutine = reinterpret_cast<routine_t>(find_symbol_maybe (rLibraryName, rObjectName));
#else
    routine_t pRoutine = 0;
#endif
    if (pRoutine) {
	IVUnknown::Reference const pSource (pRoutine ());
	rQuery.getObject (pSource);
    } else {
	VString iMessage;
	iMessage << "'" << rObjectName << "' not found in '" << rLibraryName << "'";
	rQuery.onError (0, iMessage);
    }
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, offering_t const &rCounter, VString const &rObjectName) {
    if (TracingGofers ()) {
	fprintf (stderr, "+++ Gofer Piped Object %s\n", rObjectName.content ());
    }

    PipeReceiver::Reference pPipeReceiver (new PipeReceiver (rQuery, rCounter));

    IPipeFactory::Reference pPipeFactory;
    VConnectionFactory::Supply (pPipeFactory);

    pPipeReceiver->retrieveFrom (pPipeFactory, rObjectName);
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, offering_t const &rCounter, IPipeSource *pPipeSource) {
    PipeReceiver::Reference pPipeReceiver (new PipeReceiver (rQuery, rCounter));
    pPipeReceiver->retrieveFrom (pPipeSource);
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, VString const &rObjectName) {
    offering_t const iOffering;
    Supply (rQuery, iOffering, rObjectName);
}

void Vca::Gofer::Supply (VInterfaceQuery const &rQuery, IPipeSource *pPipeSource) {
    offering_t const iOffering;
    Supply (rQuery, iOffering, pPipeSource);
}

/*****  Vca_VInterfaceQuery Implementation  *****/

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

#include "Vca_VInterfaceQuery.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "IVReceiver.h"

#include "Vca_IDirectory.h"
#include "Vca_IPassiveCallback.h"
#include "Vca_ITrigger.h"

#include "Vca_VMessageManager.h"
#include "Vca_VRolePlayer.h"
#include "Vca_VTriggerTicket.h"

/********************************
 *****  Sun Linker Pragmas  *****
 ********************************
 *****
 *****  The Solaris compilers sometimes generate two (or more) manglings for the same symbol.
 *****  When that happens, the linker complains about undefined symbols.  If it happens in
 *****  code used to build a shared library, the linker won't complain until it attempts to
 *****  link a main program using that library.  The workaround for this problem is to make
 *****  the undefined mangled names equivalent to the one and only definition for the symbol.
 *****  That is done using the '#pragma weak "undefined" = "defined"' directive.
 *****
 *****  To find the actual managled names to substitute, run 'nm -Clno LIBRARY'.  Somewhere
 *****  in its output, you should find multiple entries for the symbol, one defined, the
 *****  others not.  Here, for example, are the slightly edited entries for one of the
 *****  symbols the compiler just started mishandling -- 'Vca::VRolePlayer::QueryInterface':
 *****
 *****
 *****     [66797] ...|NOTY |GLOB |0    |UNDEF  |void Vca::VRolePlayer::QueryInterface(IVUnknown*,VTypeInfo*,IVReceiver<IVUnknown*>*)
 *****			       [__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4C3___v_]
 *****     [69596] ...|FUNC |GLOB |0    |8      |void Vca::VRolePlayer::QueryInterface(IVUnknown*,VTypeInfo*,IVReceiver<IVUnknown*>*)
 *****			       [__1cDVcaLVRolePlayerOQdDueryInterface6MpnJIVUnknown_pnJVTypeInfo_pnKIVReceiver4Cp2___v_]
 *****
 *****  If you look carefully, you can see that one of the symbols is UNDEF and the other
 *****  isn't.  The fix is to use the #pragma weak directive to set the value of the
 *****  undefined mangled name to the value of the defined one.  Note the use of "'s in
 *****  these #pragma equivalences.  They surround each of the mangled names and are
 *****  required
 *****
 *****/
#ifdef sun
// void Vca::VInterfaceQuery::getMessageParameters(IVUnknown*,VReference<IVReceiver<IVUnknown*> >&,VReference<VTypeInfo>&,VReference<Vca::VMessageManager>&)const
#pragma weak "__1cDVcaPVInterfaceQdDueryUgetMessageParameters6kMpnJIVUnknown_rnKVReference4nKIVReceiver4Cp2____rnKVReference4nJVTypeInfo___rnKVReference4n0APVMessageManager____v_" = "__1cDVcaPVInterfaceQdDueryUgetMessageParameters6kMpnJIVUnknown_rnKVReference4nKIVReceiver4C3____rnKVReference4nJVTypeInfo___rnKVReference4n0APVMessageManager____v_"
#endif


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VInterfaceQuery::Receptionist  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace Vca {
    class VInterfaceQuery::Receptionist : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (Receptionist, VRolePlayer);

    //  Construction
    public:
	Receptionist (receiver_t *pResultReceiver, manager_t *pManager);

    //  Destruction
    private:
	~Receptionist () {
	}

    //  Roles
    private:
	VRole<ThisClass,receiver_t> m_pIReceiver;
    public:
	void getRole (receiver_reference_t &rpRole) {
	    m_pIReceiver.getRole (rpRole);
	}

    private:
	VRole<ThisClass,ITrigger> m_pITrigger;
    public:
	void getRole (ITrigger::Reference &rpRole) {
	    m_pITrigger.getRole (rpRole);
	}

    //  Callbacks
    public:
	void OnData (receiver_t *pRole, IVUnknown *pResult);
	void OnError_(IError *pInterface, VString const &rMessage);
	void Process (ITrigger *pRole);

    //  Use
    public:
	void getMessageParameters (
	    IVUnknown *pQueryRecipient, receiver_reference_t &rpResultReceiver, manager_reference_t &rpManager
	);

    private:
	void cancelNoRouteToPeerCallback ();
	    
    //  State
    private:
	manager_reference_t const m_pManager;
	receiver_reference_t      m_pResultReceiver;
	VTriggerTicket::Reference m_pNoRouteToPeerCallbackTicket;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VInterfaceQuery::Receptionist::Receptionist (
    receiver_t *pResultReceiver, manager_t *pManager
) : m_pResultReceiver (pResultReceiver), m_pManager (pManager), m_pIReceiver (this), m_pITrigger (this) {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VInterfaceQuery::Receptionist::OnData (receiver_t *pRole, IVUnknown *pResult) {
    Reference iRetainer (this);
    cancelNoRouteToPeerCallback ();
    if (m_pResultReceiver) {
	m_pResultReceiver->OnData (pResult);
    }
}

void Vca::VInterfaceQuery::Receptionist::OnError_(IError *pInterface, VString const &rMessage) {
    Reference iRetainer (this);
    cancelNoRouteToPeerCallback ();
    if (m_pResultReceiver) {
	m_pResultReceiver->OnError (pInterface, rMessage);
    }
}

void Vca::VInterfaceQuery::Receptionist::Process (ITrigger *pRole) {
    Reference iRetainer (this);
    cancelNoRouteToPeerCallback ();
    if (m_pResultReceiver) {
	m_pResultReceiver->OnError (0, "No Route To Peer");
    }
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VInterfaceQuery::Receptionist::getMessageParameters (
    IVUnknown *pQueryRecipient, receiver_reference_t &rpResultReceiver, manager_reference_t &rpManager
) {
    ITrigger::Reference pITrigger;
    getRole (pITrigger);
    if (pQueryRecipient->requestNoRouteToPeerCallback (m_pNoRouteToPeerCallbackTicket, pITrigger)) {
    //  Interpose if a callback was successfully registered...
	getRole (rpResultReceiver);
	if (m_pManager)
	    rpManager.setTo (new VMessageMonitor<manager_t> (m_pManager));
    } else {
    //  Otherwise, let the application supplied objects do the work...
	rpResultReceiver.setTo (m_pResultReceiver);
	rpManager.setTo (m_pManager);
    }
}

void Vca::VInterfaceQuery::Receptionist::cancelNoRouteToPeerCallback () {
    VTriggerTicket::Reference pCallbackTicket;
    pCallbackTicket.claim (m_pNoRouteToPeerCallbackTicket);
    if (pCallbackTicket)
	pCallbackTicket->cancel ();
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VInterfaceQuery  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VInterfaceQuery::VInterfaceQuery (
    manager_t *pManager, receiver_t *pReceiver, typeinfo_t *pTypeInfo
) : m_pManager (pManager), m_pReceiver (pReceiver), m_pTypeInfo (pTypeInfo) {
}

Vca::VInterfaceQuery::VInterfaceQuery (
    receiver_t *pReceiver, typeinfo_t *pTypeInfo
) : m_pReceiver (pReceiver), m_pTypeInfo (pTypeInfo) {
}

Vca::VInterfaceQuery::VInterfaceQuery (
    ThisClass const &rOther
) : m_pManager (rOther.m_pManager), m_pReceiver (rOther.m_pReceiver), m_pTypeInfo (rOther.m_pTypeInfo) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VInterfaceQuery::~VInterfaceQuery () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VInterfaceQuery::getMessageParameters (
    IVUnknown *pImplementor, receiver_reference_t &rpResultReceiver, typeinfo_reference_t &rpTypeInfo, manager_reference_t &rpManager
) const {
    if (pImplementor && m_pReceiver) {
	Receptionist::Reference pReceptionist (new Receptionist (m_pReceiver, m_pManager));
	pReceptionist->getMessageParameters (pImplementor, rpResultReceiver, rpManager);
    } else {
	rpResultReceiver.setTo (m_pReceiver);
	rpManager.setTo (m_pManager);
    }
    rpTypeInfo.setTo (m_pTypeInfo);
}

void Vca::VInterfaceQuery::getObject (IVUnknown *pSource) const {
    if (pSource) {
	VInterfaceQuery::receiver_reference_t pReceiver;
	VInterfaceQuery::typeinfo_reference_t pTypeInfo;
	VInterfaceQuery::manager_reference_t pManager;
	getMessageParameters (pSource, pReceiver, pTypeInfo, pManager);

	if (pReceiver)
	    pSource->QueryInterface (pTypeInfo, pReceiver, pManager);
    } else {
	VString iMessage;
	iMessage
	    << "Server does not support "
	    << typeInfo ()->name ()
	    << " interface";
	onError (0, iMessage);
    }
}

void Vca::VInterfaceQuery::getObject (IPassiveCallback *pSource) const {
    if (pSource) {
	VInterfaceQuery::receiver_reference_t pReceiver;
	VInterfaceQuery::typeinfo_reference_t pTypeInfo;
	VInterfaceQuery::manager_reference_t pManager;
	getMessageParameters (pSource, pReceiver, pTypeInfo, pManager);

	if (pReceiver)
	    pSource->QueryService (pReceiver, pTypeInfo, pManager);
    } else {
	VString iMessage;
	iMessage
	    << "Server does not support "
	    << typeInfo ()->name ()
	    << " interface";
	onError (0, iMessage);
    }
}



void Vca::VInterfaceQuery::getObject (VString const &rName, IDirectory *pSource) const {
    if (!pSource)
	onError (0, "No Directory");
    else {
	VInterfaceQuery::receiver_reference_t pReceiver;
	VInterfaceQuery::typeinfo_reference_t pTypeInfo;
	VInterfaceQuery::manager_reference_t pManager;
	getMessageParameters (pSource, pReceiver, pTypeInfo, pManager);

	if (pReceiver)
	    pSource->GetObject (pReceiver, rName, pTypeInfo, pManager);
    }
}

void Vca::VInterfaceQuery::onEnd () const {
    if (m_pReceiver)
	m_pReceiver->OnEnd ();
}

void Vca::VInterfaceQuery::onError (IError *pInterface, VString const &rMessage) const {
    if (m_pReceiver)
	m_pReceiver->OnError (pInterface, rMessage);
}

void Vca::VInterfaceQuery::onZero () const {
    if (m_pReceiver)
	m_pReceiver->OnData (0);
}


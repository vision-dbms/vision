/*****  Vca_VcaDirectoryBuilder Implementation *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VkSocketAddress.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VcaDirectoryBuilder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V.h"
#include "VSimpleFile.h"

#include "Vca_IPassiveServer_Gofers.h"
#include "Vca_IPassiveConnector_Gofers.h"
#include "Vca_IPassiveCallback_Gofers.h"
#include "Vca_IPipeSource_Gofers.h"

#include "Vca_VcaGofer.h"
#include "Vca_VcaGofer_OneOf.h"
#include "Vca_VcaGofer_Retried.h"
#include "Vca_VcaGofer_Timeout.h"

#include "Vca_VcaCredentialsProvider.h"

#include "Vca_VDirectory.h"

#include "Vca_VPassiveProcessAgent.h"

#include "VTransientServices.h" 

#include "Vca_CompilerHappyPill.h"

#define VCA_PASSIVE_DISTRIBUTED_CALLBACK 1
#include "VConfig.h"


/***********************
 ***********************
 *****             *****
 *****  Utilities  *****
 *****             *****
 ***********************
 ***********************/

namespace {
    VString ToLower (VString const& rInput) {
	VString iOutput (rInput);
	for (char* pOutput = iOutput.storage (); *pOutput; pOutput++)
	    *pOutput = tolower (*pOutput);
	return iOutput;
    }

    void ExpandVariables (VString &rResult, char const *pSource, char const *pNamePrefix, size_t sNamePrefix) {
	rResult.clear ();

	while (*pSource) {
	    static char   const iNameSuffix[] = ")";
	    static size_t const sNameSuffix = sizeof (iNameSuffix) - 1;

	    char const *pName = strstr (pSource, pNamePrefix);
	    if (!pName) {
		rResult.append (pSource);
		pSource += strlen (pSource);
	    } else {
		size_t const sPrefix = pName - pSource;

		rResult.append (pSource, sPrefix);

		pName += sNamePrefix;
		size_t const sName = strcspn (pName, iNameSuffix);

		VString iName, iValue;
		iName.setTo (pName, sName);

		V::GetEnvironmentString (iValue, iName, 0);
		if (iValue.length () > 0)
		    rResult.append (iValue);

		pSource += sPrefix + sNamePrefix + sName;
		if (*pSource)
		    pSource += sNameSuffix;
	    }
	}
    }
    void ExpandSDVariables (VString &rResult, char const *pSource) {
	static char   const iNamePrefix[] = "$(";
	static size_t const sNamePrefix = sizeof (iNamePrefix) - 1;
	ExpandVariables (rResult, pSource, iNamePrefix, sNamePrefix);
    }
    void ExpandDDVariables (VString &rResult, char const *pSource) {
	static char   const iNamePrefix[] = "$$(";
	static size_t const sNamePrefix = sizeof (iNamePrefix) - 1;
	ExpandVariables (rResult, pSource, iNamePrefix, sNamePrefix);
    }
}


/*****************************************************************
 *****************************************************************
 *****                                                       *****
 *****  Vca::VcaDirectoryBuilder::Directory::Pattern::Query  *****
 *****                                                       *****
 *****************************************************************
 *****************************************************************/

namespace Vca {
    class VcaDirectoryBuilder::Directory::Pattern::Query : public QueryBase, public Session::Bindings {
	DECLARE_FAMILY_MEMBERS (Query, QueryBase);

    //  Construction
    public:
	Query (VString const& rInput);

    //  Destruction
    public:
	~Query ();

    //  Access
    public:
	bool getBinding (VString& rResult, unsigned int xBinding) const;

	VString const& input () const;

    //  Use
    public:
	unsigned int edit (VString const& rInput, VString& rOutput) const;
	VString edit (VString const& rInput) const;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Directory::Pattern::Query::Query (
    VString const& rInput
) : QueryBase (ToLower (rInput)) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Directory::Pattern::Query::~Query () {
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

unsigned int Vca::VcaDirectoryBuilder::Directory::Pattern::Query::edit (VString const& rInput, VString& rOutput) const {
    unsigned int cChanges = 0;
    rOutput.clear ();

    char const* pSource = rInput;
    while (*pSource) {
	static char   const iNamePrefix[] = "%(";
	static char   const iNameSuffix[] = ")";
	static size_t const sNamePrefix = sizeof (iNamePrefix) - 1;
	static size_t const sNameSuffix = sizeof (iNameSuffix) - 1;

	char const *pName = strstr (pSource, iNamePrefix);
	if (!pName) {
	    rOutput.append (pSource);
	    pSource += strlen (pSource);
	} else {
	    cChanges++;
	    size_t const sPrefix = pName - pSource;

	    rOutput.append (pSource, sPrefix);

	    pName += sNamePrefix;
	    size_t const sName = strcspn (pName, iNameSuffix);

	    VString iName;
	    iName.setTo (pName, sName);

	    unsigned int xBinding;
	    VString iValue;
	    if (1 == sscanf (iName, "%u", &xBinding) && getBinding (iValue, xBinding))
		rOutput.append (iValue);

	    pSource += sPrefix + sNamePrefix + sName;
	    if (*pSource)
		pSource += sNameSuffix;
	}
    }
    return cChanges;
}

VString Vca::VcaDirectoryBuilder::Directory::Pattern::Query::edit (VString const& rInput) const {
    VString iOutput;
    edit (rInput, iOutput);
    return iOutput;
}

bool Vca::VcaDirectoryBuilder::Directory::Pattern::Query::getBinding (VString& rResult, unsigned int xBinding) const {
    return getMatch (rResult, xBinding);
}

VString const& Vca::VcaDirectoryBuilder::Directory::Pattern::Query::input () const {
    return string ();
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vca::VcaDirectoryBuilder::Directory::Pattern  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Directory::Pattern::Pattern (
    VString const& rPattern, Session* pSession
) : m_iPattern (ToLower (rPattern)), m_iPatternRegex (m_iPattern), m_pImplementation (pSession) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Directory::Pattern::~Pattern () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vca::VcaDirectoryBuilder::Directory::Pattern::addToDirectory (Directory* pDirectory, Query& rQuery) const {
    return matches (rQuery) && m_pImplementation->addToDirectory (pDirectory, rQuery);
}

bool Vca::VcaDirectoryBuilder::Directory::Pattern::matches (Query& rQuery) const {
    return m_iPatternRegex.Execute (rQuery);
}


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VcaDirectoryBuilder::Directory  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Directory::Directory () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VcaDirectoryBuilder::Directory::locate (Entry::Reference &rpEntry, VString const &rName) {
    if (BaseClass::locate (rpEntry, rName))
	return true;

    Pattern::Query iQuery (rName);
    for (unsigned int xPattern = 0; xPattern < m_iPatterns.cardinality (); xPattern++) {
	if (m_iPatterns[xPattern]->addToDirectory (this, iQuery) && BaseClass::locate (rpEntry, rName))
	    return true;
    }
    return false;
}

bool Vca::VcaDirectoryBuilder::Directory::hasPattern (VString const& rPattern) const {
    unsigned int xPattern;
    return m_iPatterns.Locate (rPattern, xPattern);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VcaDirectoryBuilder::Directory::setPattern (VString const& rPattern, Session* pSession) {
    unsigned int xPattern;
    m_iPatterns.Insert (rPattern, xPattern);
    m_iPatterns[xPattern].setTo (new Pattern (rPattern, pSession));
}


/***************************************************************************
 ***************************************************************************
 *****                                                                 *****
 *****  Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource  *****
 *****                                                                 *****
 ***************************************************************************
 ***************************************************************************/

namespace {
    using namespace Vca;

    /*****************************************************************/
    class TriggerBase : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (TriggerBase, VRolePlayer);

    //  Construction
    protected:
	TriggerBase () : m_pITrigger (this) {
	}

    //  Destruction
    protected:
	~TriggerBase () {
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  ITrigger Role
    private:
	VRole<ThisClass,ITrigger> m_pITrigger;
    public:
	void getRole (ITrigger::Reference &rpRole) {
	    m_pITrigger.getRole (rpRole);
	}

    //  ITrigger Implementation
    public:
	void Process (ITrigger *pTrigger) {
	    onTrigger_();
	}

    //  Callbacks
    private:
	virtual void onTrigger_() = 0;
    };

    /*****************************************************************/
    template <class Target_T> class Trigger : virtual public TriggerBase {
	DECLARE_CONCRETE_RTTLITE (Trigger<Target_T>, TriggerBase);

    //  Aliases
    public:
	typedef Target_T target_t;
	typedef void (Target_T::*member_t)(ThisClass*);

    //  Construction
    public:
	Trigger (
	    target_t *pTarget, member_t pMember
	) : m_pTarget (pTarget), m_pMember (pMember) {
	}

    //  Destruction
    private:
	~Trigger () {
	}

    //  Callbacks
    private:
	virtual void onTrigger_() OVERRIDE {
	    (m_pTarget.referent()->*m_pMember) (this);
	}

    //  State
    private:
	typename target_t::Pointer const m_pTarget;
	member_t const m_pMember;
    };
}

/************************************
 ************************************
 *                                  *
 *>>>>>>>>>>>>>>>>  <<<<<<<<<<<<<<<<*
 *                                  *
 ************************************
 ***********************************/

namespace Vca {
    class VcaDirectoryBuilder::Order::Session::AuthoritativeSource : public gofer_t {
	DECLARE_CONCRETE_RTTLITE (AuthoritativeSource, gofer_t);

    //  Trigger
    public:
	typedef Trigger<ThisClass> trigger_t;

    //  Construction
    public:
	AuthoritativeSource (Session *pSession, gofer_t *pInitialSource);

    //  Destruction
    private:
	~AuthoritativeSource ();

    //  Access
    protected:
	virtual void getDescription_(VString &rDescription) const OVERRIDE {
	    m_pSession->getDescription (rDescription);
	}
    public:
	VString const &getName () const {
	    return m_pSession->getName ();
	}

    //  Callbacks
    private:
	virtual void onData () OVERRIDE;
	virtual void onReset () OVERRIDE;

	virtual void onError (IError *pInterface, VString const &rMessage) OVERRIDE;
	void onValue (IVUnknown *pObject);

    //  TTL
    private:
	void startTTL ();
	void resetTTL ();
	void onTTL (trigger_t *pTrigger);

    //  State
    private:
	Session::Reference const m_pSession;
	gofer_t::Reference       m_pSource;
	VTimer::Reference        m_pTTLTimer;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::AuthoritativeSource (
    Session *pSession, gofer_t *pInitialSource
) : m_pSession (pSession), m_pSource (pInitialSource) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::~AuthoritativeSource () {
    resetTTL ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::onData () {
    if (m_pSource)
	m_pSource->supplyMembers (this, &ThisClass::onValue, &ThisClass::onError);
    else {
	VString iMessage;
	iMessage << "No source for session '" << m_pSession->getName () << "'";
	onError (0, iMessage);
    }
}

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::onReset () {
    resetTTL ();
    m_pSession->supplyObjectSource (m_pSource);
    BaseClass::onReset ();
}

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::onError (IError *pInterface, VString const &rMessage) {
    BaseClass::onError (pInterface, rMessage);
}

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::onValue (IVUnknown *pObject) {
    setTo (pObject);
    startTTL ();
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::startTTL () {
    if (Vca::VDirectory::TTLIsDisabled ())
	return;
    microseconds_t const sTTL (m_pSession->getTTL ());
    if (sTTL > 0) {
	ITrigger::Reference pTrigger;
	(new trigger_t (this, &ThisClass::onTTL))->getRole (pTrigger);

	m_pTTLTimer.setTo (
	    new VTimer (VString ("TTL for ") << getName (), pTrigger, sTTL)
	);
	m_pTTLTimer->start ();
    }
}

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::resetTTL () {
    if (m_pTTLTimer) {
	m_pTTLTimer->cancel ();
	m_pTTLTimer.clear ();
    }
}

void Vca::VcaDirectoryBuilder::Order::Session::AuthoritativeSource::onTTL (trigger_t *pTrigger) {
    resetGofer ();
}


/****************************************************************
 ****************************************************************
 *****                                                      *****
 *****  Vca::VcaDirectoryBuilder::Order::Session::Fallback  *****
 *****                                                      *****
 ****************************************************************
 ****************************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::Session::Fallback::Fallback (
    VString const &rName, ThisClass *pSuccessor, ThisClass *pNamespace
) : m_iName (rName), m_pSuccessor (pSuccessor), m_pNamespace (pNamespace) {
}

Vca::VcaDirectoryBuilder::Order::Session::Fallback::Fallback (
    ThisClass const* pPattern, Bindings const& rBindings
) : m_iName (rBindings.edit (pPattern->name ()))
  , m_pNamespace (pPattern->namespase () ? new Fallback (pPattern->namespase (), rBindings) : 0)
  , m_pSuccessor (pPattern->successor () ? new Fallback (pPattern->successor (), rBindings) : 0)
{
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VcaDirectoryBuilder::Order::Session::Fallback::GetReversedList (
    Reference &rpReversed, ThisClass *pList
) {
    while (pList) {
	rpReversed.setTo (new ThisClass (pList->name (), rpReversed));
        pList = pList->successor ();
    }
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VcaDirectoryBuilder::Order::Session::Fallback::makeGofer (gofer_t::Reference &rpGofer) const {
    gofer_t::Reference pNewGofer;
    if (m_pNamespace.isNil ())
	pNewGofer.setTo (new Gofer::Named<IVUnknown,IDirectory> (m_iName));
    else {
	gofer_t::Reference pNamespaceGofer;
	m_pNamespace->makeGofer (pNamespaceGofer);
	pNewGofer.setTo (
	    new Gofer::Named<IVUnknown,IDirectory> (m_iName, new Gofer::Queried<IDirectory>(pNamespaceGofer))
	);
    }
    pNewGofer->setFallbackTo (rpGofer);
    rpGofer.claim (pNewGofer);
}


/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vca::VcaDirectoryBuilder::Order::Session  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::Session::Session (
    VString const &rName, SessionType xType
) : m_iName (rName), m_xType (xType), m_sTTL (0), m_bPattern (false) {
}

Vca::VcaDirectoryBuilder::Order::Session::Session (
    ThisClass const* pPattern, Bindings const& rBindings
) : m_iName         (rBindings.input ())
  , m_xType         (pPattern->getType ())
  , m_sTTL          (pPattern->getTTL())
  , m_pFallbackList (pPattern->fallbackListFor (rBindings))
  , m_bPattern      (false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Order::Session::~Session () {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::Session::instantiationFor_(
    Bindings const& rBindings
) const {
    return 0;
}

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::Session::instantiationFor (
    Bindings const& rBindings
) const {
    return isPattern () ? instantiationFor_(rBindings) : 0;
}

Vca::VcaDirectoryBuilder::Session::Fallback* Vca::VcaDirectoryBuilder::Order::Session::fallbackListFor (
    Bindings const& rBindings
) const {
    return isPattern () && m_pFallbackList ? new Fallback (m_pFallbackList, rBindings) : m_pFallbackList.referent ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VcaDirectoryBuilder::Order::Session::getInfo (VString &rInfo) const {
    rInfo << "\r\nSessionName  : " << m_iName;

    //  Reverse the alias list so names are displayed in the correct order:
    Fallback::Reference pReversed;
    Fallback::GetReversedList (pReversed, m_pFallbackList);

    while (pReversed) {
	rInfo << "\r\nElse Use: " << pReversed->name ();
	pReversed.setTo (pReversed->successor ());
    }
}

void Vca::VcaDirectoryBuilder::Order::Session::getDescription (VString &rDesc) const {
    getInfo (rDesc);

    rDesc << "\r\n|";
    getStatusMessage_(rDesc);

    rDesc << "\r\n|" << getTypeCode ();
}

void Vca::VcaDirectoryBuilder::Order::Session::getStatusMessage_(VString &rMessage) const {
}

bool Vca::VcaDirectoryBuilder::Order::Session::supplyObjectSource (gofer_t::Reference &rpObjectGofer) const {
    rpObjectGofer.clear ();
    if (supplyObjectGofer_(rpObjectGofer)) {
    //  If we want timeouts, ...
	if (wantsTimeout ()) { 	//  ... wrap our gofer in a 'Timeout' gofer:
	    rpObjectGofer.setTo (new Gofer::Timeout<IVUnknown>(rpObjectGofer.referent (), timeout ()));
	}

    //  If we have fallbacks, ...
	if (hasFallbacks ()) {  //  ... attach them to the gofer we just got:
	    gofer_t::Reference pFallbackGofer; {
		Fallback::Reference pFallback (m_pFallbackList);
		while (pFallback) {
		    pFallback->makeGofer (pFallbackGofer);
		    pFallback.setTo (pFallback->successor ());
		}
	    }
	    rpObjectGofer->setFallbackTo (pFallbackGofer);
	}

    //  If we want retries, ...
	if (wantsRetries ()) {  //  ... wrap our gofer (complete with its fallback rules) in a 'Retry' gofer:
	    rpObjectGofer.setTo (
		new Gofer::Retried<IVUnknown> (rpObjectGofer.referent (), m_iRetrySettings)
	    );
	}
    }
    return rpObjectGofer.isntNil ();
}

bool Vca::VcaDirectoryBuilder::Order::Session::supplyObjectSource_(IObjectSource::Reference& rpObjectSource) const {
    return rpObjectSource.isntNil ();
}

bool Vca::VcaDirectoryBuilder::Order::Session::supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const {
    IObjectSource::Reference pObjectSource;
    if (supplyObjectSource_(pObjectSource))
	rpObjectGofer.setTo (new Gofer::Sourced<IVUnknown> (pObjectSource.referent ()));
    return rpObjectGofer.isntNil ();
}

Vca::U64 Vca::VcaDirectoryBuilder::Order::Session::timeout_() const {
    return 0;
}
bool Vca::VcaDirectoryBuilder::Order::Session::wantsTimeout_() const {
    return false;
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

bool Vca::VcaDirectoryBuilder::Order::Session::addToDirectory (VDirectory* pDirectory) {
    VString const& rName (getName ());

    gofer_t::Reference pSource;
    if (isPattern ()) {
	Directory::Reference const pPatternDirectory (dynamic_cast<Directory*>(pDirectory));
	if (pPatternDirectory) {
	    pPatternDirectory->setPattern (rName, this);
	    return true;
	}
    } else if (supplyObjectSource (pSource)) {
	VString iDesc;
	getDescription (iDesc); // the description isn't complete until supplyObjectSource() is called
	pDirectory->setSource (rName, new AuthoritativeSource (this, pSource), iDesc); 
	return true;
    }

    //  The add failed, say something ...
    VString iDesc;
    getDescription (iDesc); // the description isn't complete until supplyObjectSource() is called
    log ("VcaDirectoryBuilder::Order::insertSession --> Faulty template specification ... '%s'[%s] ",
	 rName.content(), iDesc.content());

    return false;
}

bool Vca::VcaDirectoryBuilder::Order::Session::addToDirectory (
    VDirectory* pDirectory, Bindings const& rBindings
) const {
    Session::Reference const pInstantiation (instantiationFor (rBindings));
    return pInstantiation && pInstantiation->addToDirectory (pDirectory);
}


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  Vca::VcaDirectoryBuilder::Order::AliasSession  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

namespace Vca {
    /**
     * Session type that supports redirections to other session definitions.
     * Basically, anything that contains a 'Use' statement in a sessions file is handled by this class.
     */
    class VcaDirectoryBuilder::Order::AliasSession : public Session {
	DECLARE_CONCRETE_RTTLITE (AliasSession, Session);

    //  Aliases
    public:
	typedef Reference session_reference_t;
	typedef Gofer::OneOf<IVUnknown> oneof_gofer_t;
	typedef oneof_gofer_t::gofer_array_t gofer_array_t;

    //  OneOfInitializer
    public:
	class OneOfInitializer : public oneof_gofer_t::Initializer {
	    DECLARE_FAMILY_MEMBERS (OneOfInitializer, oneof_gofer_t::Initializer);

	//  Construction
	public:
	    OneOfInitializer (AliasSession* pSession) : m_pSession (pSession) {
	    }

	//  Destruction
	public:
	    ~OneOfInitializer () {
	    }

	//  Use
	public:
	    void initialize (gofer_array_t &rGoferArray) const {
		m_pSession->initialize (rGoferArray);
	    }

	//  State
	private:
	    session_reference_t const m_pSession;
	};

    //  Construction
    public:
	AliasSession (VString const& rSessionName, SessionType xType, Fallback *pAliasList, bool bOneOf);
	AliasSession (ThisClass const* pPattern, Bindings const& rBindings);

    //  Destruction
    private:
	~AliasSession () {
	}

    //  Instantiation
    private:
	virtual Session* instantiationFor_(Bindings const& rBindings) const OVERRIDE;
	Fallback* aliasListFor (Bindings const& rBindings) const;

    //  Access
    public:
	unsigned int aliasCount () const;

    private:
	virtual bool supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const OVERRIDE;

    //  Query
    public:
	void initialize (gofer_array_t &rGoferArray) const;

	virtual void getInfo (VString &rInfo) const OVERRIDE;
	using BaseClass::getInfo;

	bool oneOf () const {
	    return m_bOneOf;
	}

    private:
	virtual void getStatusMessage_(VString &rInfo) const OVERRIDE;

    //  State
    private:
	Fallback::Reference const m_pAliasList;
	bool const m_bOneOf;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::AliasSession::AliasSession (
    VString const &rSessionName, SessionType xType, Fallback *pAliasList, bool bOneOf
) : BaseClass (rSessionName, xType), m_pAliasList (pAliasList), m_bOneOf (bOneOf) {
}

Vca::VcaDirectoryBuilder::Order::AliasSession::AliasSession (
    ThisClass const* pPattern, Bindings const& rBindings
) : BaseClass (pPattern, rBindings)
  , m_pAliasList	(pPattern->aliasListFor (rBindings))
  , m_bOneOf		(pPattern->oneOf ()) {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

Vca::VcaDirectoryBuilder::Session::Fallback* Vca::VcaDirectoryBuilder::Order::AliasSession::aliasListFor (
    Bindings const& rBindings
) const {
    return isPattern () && m_pAliasList ? new Fallback (m_pAliasList, rBindings) : m_pAliasList.referent ();
}

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::AliasSession::instantiationFor_(
    Bindings const& rBindings
) const {
    return new ThisClass (this, rBindings);
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

unsigned int Vca::VcaDirectoryBuilder::Order::AliasSession::aliasCount () const {
    unsigned int sAliasList = 0;
    for (Fallback *pAlias = m_pAliasList; pAlias; pAlias = pAlias->successor ())
	sAliasList++;
    return sAliasList;
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VcaDirectoryBuilder::Order::AliasSession::initialize (gofer_array_t &rGoferArray) const {
    unsigned int nAliases (aliasCount ());
    rGoferArray.Guarantee (nAliases);

    for (Fallback *pAlias = m_pAliasList; pAlias; pAlias = pAlias->successor ())
	pAlias->makeGofer (rGoferArray[--nAliases]);
}

void Vca::VcaDirectoryBuilder::Order::AliasSession::getInfo (VString &rInfo) const {
    BaseClass::getInfo (rInfo);
    if (!m_bOneOf)
	rInfo << "\r\nUse: "<< m_pAliasList->name ();
    else {
    //  Reverse the alias list so names are displayed in the correct order:
	Fallback::Reference pReversed;
	Fallback::GetReversedList (pReversed, m_pAliasList);

	rInfo << "\r\nOne Of:";

	while (pReversed) {
	    rInfo << "\r\n... " << pReversed->name ();
	    pReversed.setTo (pReversed->successor ());
	}
    }
    rInfo << "\r\n";
}

void Vca::VcaDirectoryBuilder::Order::AliasSession::getStatusMessage_(VString &rInfo) const {
    if (!m_bOneOf)
	rInfo << "Use: " << m_pAliasList->name ();
    else {
    //  Reverse the alias list so names are displayed in the correct order:
	Fallback::Reference pReversed;
	Fallback::GetReversedList (pReversed, m_pAliasList);

	char const *pPrefix = "One Of {";

	while (pReversed) {
	    rInfo << pPrefix << pReversed->name ();
	    pPrefix = ", ";

	    pReversed.setTo (pReversed->successor ());
	}
	rInfo << "}";
    }
}

bool Vca::VcaDirectoryBuilder::Order::AliasSession::supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const {
    if (!m_bOneOf)
	m_pAliasList->makeGofer (rpObjectGofer);
    else {
	OneOfInitializer iOneOfInitializer (const_cast<ThisClass*>(this));
	rpObjectGofer.setTo (new oneof_gofer_t (iOneOfInitializer));
    }

    return rpObjectGofer.isntNil ();
}


/************************************************************
 ************************************************************
 *****                                                  *****
 *****  Vca::VcaDirectoryBuilder::Order::LinkedSession  *****
 *****                                                  *****
 ************************************************************
 ************************************************************/

namespace Vca {
    /**
     * Session type that supports redirections to other session definitions.
     * Basically, anything that contains a 'Use' statement in a sessions file is handled by this class.
     */
    class VcaDirectoryBuilder::Order::LinkedSession : public Session {
	DECLARE_CONCRETE_RTTLITE (LinkedSession, Session);

    //  Aliases
    public:
	typedef Reference session_reference_t;
	typedef Gofer::OneOf<IVUnknown> oneof_gofer_t;
	typedef oneof_gofer_t::gofer_array_t gofer_array_t;

    //  Construction
    public:
	LinkedSession (VString const& rSessionName, SessionType xType, VString const &rEntryName, VString const &rImageName);
	LinkedSession (ThisClass const* pPattern, Bindings const& rBindings);

    //  Destruction
    private:
	~LinkedSession () {
	}

    //  Instantiation
    private:
	virtual Session* instantiationFor_(Bindings const& rBindings) const OVERRIDE;

    //  Access
    public:
	VString const &entryName () const {
	    return m_iEntryName;
	}
	VString const imageName () const {
	    return m_iImageName;
	}

    private:
	virtual bool supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const OVERRIDE;

    //  Query
    public:
	void initialize (gofer_array_t &rGoferArray) const;

	virtual void getInfo (VString &rInfo) const OVERRIDE;

	using BaseClass::getInfo;

    private:
	virtual void getStatusMessage_(VString &rInfo) const OVERRIDE;

    //  State
    private:
	VString const m_iEntryName;
	VString const m_iImageName;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::LinkedSession::LinkedSession (
    VString const &rSessionName, SessionType xType, VString const &rEntryName, VString const &rImageName
) : BaseClass (rSessionName, xType), m_iEntryName (rEntryName), m_iImageName (rImageName) {
}

Vca::VcaDirectoryBuilder::Order::LinkedSession::LinkedSession (
    ThisClass const* pPattern, Bindings const& rBindings
) : BaseClass (pPattern, rBindings), m_iEntryName (rBindings.edit (pPattern->entryName ())), m_iImageName (rBindings.edit (pPattern->imageName ())) {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::LinkedSession::instantiationFor_(
    Bindings const& rBindings
) const {
    return new ThisClass (this, rBindings);
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VcaDirectoryBuilder::Order::LinkedSession::getInfo (VString &rInfo) const {
    BaseClass::getInfo (rInfo);
#if 0
    if (!m_bOneOf)
	rInfo << "\r\nUse: "<< m_pAliasList->name ();
    else {
    //  Reverse the alias list so names are displayed in the correct order:
	Fallback::Reference pReversed;
	Fallback::GetReversedList (pReversed, m_pAliasList);

	rInfo << "\r\nOne Of:";

	while (pReversed) {
	    rInfo << "\r\n... " << pReversed->name ();
	    pReversed.setTo (pReversed->successor ());
	}
    }
#endif
    rInfo << "\r\n";
}

void Vca::VcaDirectoryBuilder::Order::LinkedSession::getStatusMessage_(VString &rInfo) const {
#if 0
    if (!m_bOneOf)
	rInfo << "Use: " << m_pAliasList->name ();
    else {
    //  Reverse the alias list so names are displayed in the correct order:
	Fallback::Reference pReversed;
	Fallback::GetReversedList (pReversed, m_pAliasList);

	char const *pPrefix = "One Of {";

	while (pReversed) {
	    rInfo << pPrefix << pReversed->name ();
	    pPrefix = ", ";

	    pReversed.setTo (pReversed->successor ());
	}
	rInfo << "}";
    }
#endif
}

bool Vca::VcaDirectoryBuilder::Order::LinkedSession::supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const {
    rpObjectGofer.setTo (new Gofer::Linked<IVUnknown> (m_iEntryName, m_iImageName));
    return rpObjectGofer.isntNil ();
}


/*************************************************************
 *************************************************************
 *****                                                   *****
 *****  Vca::VcaDirectoryBuilder::Order::PlumbedSession  *****
 *****                                                   *****
 *************************************************************
 *************************************************************/

namespace Vca {
    /**
     * Type of session incurring a connection.
     * A timeout is provided in this class, specified in microseconds. If a connection has not been successfully established by this timeout, this session will fail.
     */
    class VcaDirectoryBuilder::Order::PlumbedSession : public Session {
	DECLARE_ABSTRACT_RTTLITE (PlumbedSession, Session);

    //  Aliases
    public:
	typedef VGoferInterface<IPassiveServer> PassiveServerGofer;

    //  Construction
    public:
        /**
         * @param sTimeout timeout in microseconds.
	 * @param bTimeout boolean which, when true, indicates that sTimeout is valid.
         */
	PlumbedSession (VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, bool bPassive);
	PlumbedSession (ThisClass const* pPattern, Bindings const& rBindings);

    //  Destruction
    protected:
	~PlumbedSession ();

    //  Access
    protected:
	U64 timeout_() const;
	bool wantsTimeout_() const;
    public:
	U64 timeout () const {
	    return m_sTimeout;
	}
	bool passive () const {
	    return m_bPassive;
	}

    //  Query
    public:
	void getInfo (VString &rInfo) const;
	using BaseClass::getInfo;
    protected:
	virtual bool supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const;
	virtual bool supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const;
    private:
	virtual void getStatusMessage_(VString &rInfo) const;
	virtual bool supplyPipeSourceGofer_(PipeSourceGofer::Reference &rpPipeSourceGofer) const = 0;
	virtual bool isAPassiveAgent_() const {
	    return false;
	}

    //  State
    private:
	U64	const	m_sTimeout;
	bool	const	m_bTimeout;
	bool	const	m_bPassive;
    protected:
	VString mutable	m_iStatusMessage;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::PlumbedSession::PlumbedSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, bool bPassive
) : BaseClass (rName, xType), m_sTimeout (sTimeout), m_bTimeout (bTimeout), m_bPassive (bPassive) {
}

Vca::VcaDirectoryBuilder::Order::PlumbedSession::PlumbedSession (
    ThisClass const* pPattern, Bindings const& rBindings
) : BaseClass (pPattern, rBindings), m_sTimeout (pPattern->timeout ()), m_bTimeout (pPattern->m_bTimeout), m_bPassive (pPattern->passive ()) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Order::PlumbedSession::~PlumbedSession () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VcaDirectoryBuilder::Order::PlumbedSession::getInfo (VString &rInfo) const {
    BaseClass::getInfo (rInfo);
    if (m_bPassive) {
	rInfo << "\r\nOption Passive";
	if (isAPassiveAgent_())
	    rInfo << "Agent";
    }
}

void Vca::VcaDirectoryBuilder::Order::PlumbedSession::getStatusMessage_(VString &rInfo) const {
    rInfo << m_iStatusMessage;
}

bool Vca::VcaDirectoryBuilder::Order::PlumbedSession::supplyObjectGofer_(gofer_t::Reference& rpObjectGofer) const {
    if (m_bPassive) {
	PassiveServerGofer::Reference pPassiveServer;
	if (supplyPassiveServerGofer_(pPassiveServer)) {
	    rpObjectGofer.setTo (
		new Gofer::Passive<IVUnknown> (
		    new IPassiveCallback_Gofer::Request (
			IPassiveConnector_Gofer::DefaultConnector (), pPassiveServer, m_bTimeout ? m_sTimeout : 60 * 1000 * 1000 /* 60 seconds */
		    )
		)
	    );
	}
    } else {
	PipeSourceGofer::Reference pPipeSourceGofer;
	if (supplyPipeSourceGofer_(pPipeSourceGofer)) {
	    rpObjectGofer.setTo (new Gofer::Piped<IVUnknown> (pPipeSourceGofer));
	}
    }

    return rpObjectGofer.isntNil ();
}

bool Vca::VcaDirectoryBuilder::Order::PlumbedSession::supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const {
    PipeSourceGofer::Reference pPipeSourceGofer;
    if (supplyPipeSourceGofer_(pPipeSourceGofer))
	rpPassiveServer.setTo (new Gofer::Piped<IPassiveServer> (pPipeSourceGofer));

    return rpPassiveServer.isntNil ();
}

Vca::U64 Vca::VcaDirectoryBuilder::Order::PlumbedSession::timeout_() const {
    return m_sTimeout;
}

bool Vca::VcaDirectoryBuilder::Order::PlumbedSession::wantsTimeout_() const {
    return BaseClass::wantsTimeout_() || (m_bTimeout && !passive ());
}


/*************************************************************
 *************************************************************
 *****                                                   *****
 *****  Vca::VcaDirectoryBuilder::Order::ProcessSession  *****
 *****                                                   *****
 *************************************************************
 *************************************************************/

namespace Vca {
    /**
     * Session descriptor for new processes (remote or local).
     */
    class VcaDirectoryBuilder::Order::ProcessSession: public PlumbedSession {
	DECLARE_CONCRETE_RTTLITE (ProcessSession, PlumbedSession);

    //  Aliases
    public:
	typedef IDataSource<IVUnknown*> IObjectSource;

    //  Construction
    public:
        /**
         * @param bPassiveAgent if true, indicates that this processSession is actually a passive agent.
         */
	ProcessSession (
            VString const &rName,
            SessionType xType,
	    U64 sTimeout,
	    bool bTimeout,
            bool bPassive,
            bool bPassiveAgent,
	    VString const &rHost,
            VString const &rLogin,
            VString const &rPassword,
	    VString const &rProgram,
            VString const &rDatabase,
            bool bThreePipe,
            bool bSSH
        );
	ProcessSession (ThisClass const* pPattern, Bindings const& rBindings);

    //  Destruction
    private:
	~ProcessSession () {
	}

    //  Instantiation
    private:
	virtual Session* instantiationFor_(Bindings const& rBindings) const OVERRIDE;

    //  Access
    public:
	VString const &getHost () const {
	    return m_iHost;
	}
	VString const &getLogin () const {
	    return m_iLogin;
	}
	VString const &getPassword () const {
	    return m_iPassword;
	}
	VString const &getProgram () const {
	    return m_iProgram;
	}
	VString const &getDatabase () const {
	    return m_iDatabase;
	}
	bool getErrorChannelAllowed () const {
	    return m_bThreePipe;
	}

    //  Query
    public:
	virtual void getInfo (VString &rInfo) const OVERRIDE;
	using BaseClass::getInfo;
    protected:
	virtual bool supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const OVERRIDE;
	virtual bool supplyPipeSourceGofer_(PipeSourceGofer::Reference &rpPipeSourceGofer) const OVERRIDE;
    private:

	bool supplyPipeSourceGoferForDirective (
	    PipeSourceGofer::Reference &rpPipeSourceGofer, VString const &rDirective
	) const {
	    VInstanceOf_String const iDirective (rDirective);
	    return supplyPipeSourceGoferForDirective (rpPipeSourceGofer, iDirective);
	}
	bool supplyPipeSourceGoferForDirective (
	    PipeSourceGofer::Reference &rpPipeSourceGofer, VGoferInstance_String *pDirectiveGofer
	) const {
	    VInstanceOf_String const iDirective (pDirectiveGofer);
	    return supplyPipeSourceGoferForDirective (rpPipeSourceGofer, iDirective);
	}
	bool supplyPipeSourceGoferForDirective (
	    PipeSourceGofer::Reference &rpPipeSourceGofer, VInstanceOf_String const &rDirective
	) const;

    private:
	virtual bool isAPassiveAgent_() const OVERRIDE {
	    return isAPassiveAgent ();
	}
    public:
	bool isAPassiveAgent () const {
	    return m_bPassiveAgent;
	}
	bool isntAPassiveAgent () const {
	    return !m_bPassiveAgent;
	}
	bool isSSH () const {
#ifdef HAS_LIBSSH2
	    return m_bSSH;
#endif
	    return false;
	}

    //  State
    private:
	VString	const m_iHost;
	VString const m_iLogin;
	VString const m_iPassword;
	VString const m_iProgram;
	VString const m_iDatabase;
	bool const m_bThreePipe;
	bool const m_bPassiveAgent;
        bool const m_bSSH;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::ProcessSession::ProcessSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, bool bPassive, bool bPassiveAgent,
    VString const &rHost, VString const &rLogin, VString const &rPassword,
    VString const &rProgram, VString const &rDatabase, bool bThreePipe, bool bSSH
) : BaseClass (rName, xType, sTimeout, bTimeout, bPassive), m_bPassiveAgent (bPassiveAgent),
    m_iHost (rHost), m_iLogin (rLogin), m_iPassword (rPassword),
    m_iProgram (rProgram), m_iDatabase (rDatabase), m_bThreePipe (bThreePipe), m_bSSH (bSSH)
{
}

Vca::VcaDirectoryBuilder::Order::ProcessSession::ProcessSession (
    ThisClass const* pPattern, Bindings const& rBindings
) : BaseClass (pPattern, rBindings)
  , m_iHost		(rBindings.edit (pPattern->getHost ()))
  , m_iLogin		(rBindings.edit (pPattern->getLogin ()))
  , m_iPassword		(rBindings.edit (pPattern->getPassword ()))
  , m_iProgram		(rBindings.edit (pPattern->getProgram ()))
  , m_iDatabase		(rBindings.edit (pPattern->getDatabase ()))
  , m_bPassiveAgent	(pPattern->isAPassiveAgent ())
  , m_bThreePipe	(pPattern->getErrorChannelAllowed ())
  , m_bSSH		(pPattern->isSSH ())
{
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::ProcessSession::instantiationFor_(
    Bindings const& rBindings
) const {
    return new ThisClass (this, rBindings);
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VcaDirectoryBuilder::Order::ProcessSession::getInfo (VString &rInfo) const {
    BaseClass::getInfo (rInfo);

    if (m_iHost.isntEmpty ())
	rInfo << "\r\nHost     : '" << m_iHost << "'";
    if (m_iLogin.isntEmpty ())
	rInfo << "\r\nLogin    : '" << m_iLogin << "'"; 
    if (m_iProgram.isntEmpty ())
	rInfo << "\r\nProgram  : '" << m_iProgram << "'";
    if (m_iDatabase.isntEmpty ())
	rInfo << "\r\nDatabase : '" << m_iDatabase << "'";
    if (m_iHost.isntEmpty () && m_iPassword.isEmpty ())
	rInfo << "\r\n" << (isSSH () ? "Info     : " : "Error    : ") << "Password not supplied";
    rInfo << "\r\n";
}

bool Vca::VcaDirectoryBuilder::Order::ProcessSession::supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const {
    if (isntAPassiveAgent ())
	return BaseClass::supplyPassiveServerGofer_(rpPassiveServer);

/****************
 *  Passive directive agents place a call's origin and identity in a directive they pass to another entity for
 *  execution.  For process sessions, that information is added to the command the process will execute...
 ****************/

//  Create a passive command agent...
    VPassiveProcessAgent::Reference const pAgent (new VPassiveProcessAgent (m_iProgram));

//  ... obtain a gofer for a pipe source that will talk to the command as it runs ...
    PipeSourceGofer::Reference pPipeSourceGofer;
    supplyPipeSourceGoferForDirective (pPipeSourceGofer, pAgent->directiveGofer ());

//    rpPassiveServer.setTo (new IPassiveServer_Gofer::LaunchAgent (pAgent->directiveGofer ()));

//  ... and a passive server that will launch the agent:
    return pAgent->supplyPassiveServerGofer (rpPassiveServer, pPipeSourceGofer);
}

bool Vca::VcaDirectoryBuilder::Order::ProcessSession::supplyPipeSourceGofer_(PipeSourceGofer::Reference &rpPipeSourceGofer) const {
    VString iProgram (m_iProgram);
    if (m_iDatabase.isntEmpty ())
	iProgram << " -n " << m_iDatabase;
    iProgram << " -- -plumbed";

    return supplyPipeSourceGoferForDirective (rpPipeSourceGofer, iProgram);
}

bool Vca::VcaDirectoryBuilder::Order::ProcessSession::supplyPipeSourceGoferForDirective (
    PipeSourceGofer::Reference &rpPipeSourceGofer, VInstanceOf_String const &rDirective
) const {
    enum Type {REMOTE_PROCESS, LOCAL_AGENT, LOCAL_PROCESS};
    Type const xProcessType = m_iHost.isntEmpty () ? REMOTE_PROCESS : isAPassiveAgent () ? LOCAL_AGENT : LOCAL_PROCESS;

    VString iPassword (m_iPassword);
    if (xProcessType == REMOTE_PROCESS && iPassword.isEmpty ()) {
	VcaCredentialsProvider::Reference pCredentialsProvider;
	GetCredentialsProvider (pCredentialsProvider);

	if (!pCredentialsProvider->getPassword (m_iHost, m_iLogin, iPassword) && !isSSH ())
	    // if could not locate password for rexec then dont insert session entry... 
	    return false;
    }
  
    switch (xProcessType) {
    case REMOTE_PROCESS:
        if (isSSH ())
	    rpPipeSourceGofer.setTo (new IPipeSource_Gofer::NewSSH (m_iHost, m_iLogin, iPassword, rDirective, m_bThreePipe));
        else
	    rpPipeSourceGofer.setTo (new IPipeSource_Gofer::NewRexec (m_iHost, m_iLogin, iPassword, rDirective, m_bThreePipe));
	m_iStatusMessage << " - " << m_iLogin << "@" << m_iHost;
	break;

    case LOCAL_AGENT:
	rpPipeSourceGofer.setTo (new IPipeSource_Gofer::NewProcess (rDirective, false, false, false));
	m_iStatusMessage << " - Callback Agent";
	break;

    case LOCAL_PROCESS:
	rpPipeSourceGofer.setTo (new IPipeSource_Gofer::NewProcess (rDirective)); {
	    char iBuffer [256]; VString iHost, iLogin;
	    iHost.setTo (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));
	    iLogin.setTo (Vk_username (iBuffer, sizeof (iBuffer)));
	    m_iStatusMessage << " - " << iLogin << "@" << iHost;
	}
	break;
    default:
	break;
    }

    return rpPipeSourceGofer.isntNil ();
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vca::VcaDirectoryBuilder::Order::PortSession  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

namespace Vca {
    /**
     * Descriptor for sessions that are built arround a connection to some existing process.
     * Examples:
     *  * Hostname/Port
     *  * URL
     *  * ServerFile
     */
    class VcaDirectoryBuilder::Order::PortSession: public PlumbedSession {
	DECLARE_CONCRETE_RTTLITE (PortSession, PlumbedSession);

    //  Construction
    public:
	PortSession (
	    VString const &rName,
            SessionType xType,
            U64 sTimeout,
	    bool bTimeout,
            bool bPassive,
            VString const &rHost,
            VString const &rPort
	);
	PortSession (
            VString const &rName,
            SessionType xType,
            U64 sTimeout,
	    bool bTimeout,
            bool bPassive,
            VString const
            &rServerFileName
        );

	PortSession (
	    VString const &rName,
            SessionType xType,
            U64 sTimeout,
	    bool bTimeout,
            VString const &rURLBase,
            VString const &rHost,
            VString const &rPort
	);
	PortSession (
            VString const &rName,
            SessionType xType,
            U64 sTimeout,
	    bool bTimeout,
            VString const &rURLBase,
            VString const &rServerFileName
        );

	PortSession (ThisClass const* pPattern, Bindings const& rBindings);

    //  Destruction
    private:
	~PortSession () {
	}

	//  Instantiation
    private:
	virtual Session* instantiationFor_(Bindings const& rBindings) const OVERRIDE;

    //  Access
    public:
	VString const &getHost () const {
	    return m_iHost;
	}
	VString const &getPort () const {
	    return m_iPort;
	}
	VString const &getURLBase () const {
	    return m_iURLBase;
	}
    //  Query
    public:
	bool hostIsAServerFileName () const {
	    return m_iPort.isEmpty ();
	}

	VCA_PASSIVE_DISTRIBUTED_CALLBACK_HELPERS

    //  Query
    public:
	virtual void getInfo (VString &rInfo) const OVERRIDE;
	using BaseClass::getInfo;
    protected:
	virtual bool supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const OVERRIDE;
	virtual bool supplyPipeSourceGofer_(PipeSourceGofer::Reference &rpPipeSourceGofer) const OVERRIDE;

    //  State
    protected:
	VString const m_iHost;
	VString const m_iPort;
	VString const m_iURLBase;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::PortSession::PortSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, bool bPassive, VString const &rHost, VString const &rPort
) : BaseClass (rName, xType, sTimeout, bTimeout, bPassive), m_iHost (rHost), m_iPort (rPort) {
}

Vca::VcaDirectoryBuilder::Order::PortSession::PortSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, bool bPassive, VString const &rServerFileName
) : BaseClass (rName, xType, sTimeout, bTimeout, bPassive), m_iHost (rServerFileName) {
}

Vca::VcaDirectoryBuilder::Order::PortSession::PortSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, VString const &rURLBase, VString const &rHost, VString const &rPort
) : BaseClass (rName, xType, sTimeout, bTimeout, true), m_iURLBase (rURLBase), m_iHost (rHost), m_iPort (rPort) {
}

Vca::VcaDirectoryBuilder::Order::PortSession::PortSession (
    VString const &rName, SessionType xType, U64 sTimeout, bool bTimeout, VString const &rURLBase, VString const &rServerFileName
) : BaseClass (rName, xType, sTimeout, bTimeout, true), m_iURLBase (rURLBase), m_iHost (rServerFileName) {
}

Vca::VcaDirectoryBuilder::Order::PortSession::PortSession (
    ThisClass const* pPattern, Bindings const& rBindings
) : BaseClass (pPattern, rBindings)
  , m_iHost	(rBindings.edit (pPattern->getHost ()))
  , m_iPort	(rBindings.edit (pPattern->getPort ()))
  , m_iURLBase	(rBindings.edit (pPattern->getURLBase ()))
{
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

Vca::VcaDirectoryBuilder::Session* Vca::VcaDirectoryBuilder::Order::PortSession::instantiationFor_(
    Bindings const& rBindings
) const {
    return new ThisClass (this, rBindings);
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::VcaDirectoryBuilder::Order::PortSession::getInfo (VString &rInfo) const {
    BaseClass::getInfo (rInfo);

    VCA_PASSIVE_DISTRIBUTED_CALLBACK_INFO;

    rInfo << "\r\n";
}

bool Vca::VcaDirectoryBuilder::Order::PortSession::supplyPassiveServerGofer_(PassiveServerGofer::Reference &rpPassiveServer) const {
#ifndef VCA_HAS_PASSIVE_DISTRIBUTED_CALLBACK 
    return BaseClass::supplyPassiveServerGofer_(rpPassiveServer);
#else
    VCA_PASSIVE_DISTRIBUTED_CALLBACK_GOFER;
#endif
}

bool Vca::VcaDirectoryBuilder::Order::PortSession::supplyPipeSourceGofer_(PipeSourceGofer::Reference &rpPipeSourceGofer) const {
    VString iAddress;
    if (hostIsAServerFileName ()) {
	iAddress.setTo (m_iHost);
	m_iStatusMessage << "-" << m_iHost;
    } else if (m_iHost.isntEmpty ()) {
	iAddress << m_iHost << ":" << m_iPort;
	m_iStatusMessage << m_iHost << ":" << m_iPort;
    } else {
	iAddress.setTo (m_iPort);
	char iBuffer[256];
	m_iStatusMessage << VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)) << ":" << m_iPort;
    }

    rpPipeSourceGofer.setTo (new IPipeSource_Gofer::NewConnection (iAddress, hostIsAServerFileName ()));

    return rpPipeSourceGofer.isntNil ();
}


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VcaDirectoryBuilder::Order *****
 *****                                  *****
 ********************************************
 ********************************************/

/********************
 ********************
 ****  Globals  *****
 ********************
 ********************/

char const* const Vca::VcaDirectoryBuilder::Order::g_pRestOfLineBreakSet = "\r\n";
char const* const Vca::VcaDirectoryBuilder::Order::g_pWhitespaceBreakSet = " \t\r\n";

char const *Vca::VcaDirectoryBuilder::Order::SessionTypeCode (SessionType xType) {
    switch (xType) {
    case SESSION_GLOBAL:
	return "Global";
    case SESSION_LOCAL:
	return "Local";
    case SESSION_HIDDEN:
	return "Hidden";
    }
    return "";
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::Order::Order (
    VcaDirectoryBuilder* pBuilder, VDirectory* pDirectory
) : m_pBuilder (pBuilder), m_pDirectory (pDirectory) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::Order::~Order () {
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

void Vca::VcaDirectoryBuilder::Order::getDirectoryDescription (VString &rDescription) const {
    m_pDirectory->getDirectoryDescription (rDescription);
}

bool Vca::VcaDirectoryBuilder::Order::includeIfTagAccepted (char const *pTag) const {
    return strcasecmp (pTag, "any") == 0;
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****
 * Method: buildDirectory
 *   Builds directory out of the following sessions files (the bottom ones overriding the top ones)
 *   1. Global sessions file pointed by VcaGlobalSessionsFile env variable
 *   1. Local sessions file pointed by VcaSessionsFile env variable.
 *   2. $HOME/sessions.cfg 
 *   3. ./sessions.cfg
 *****/

void Vca::VcaDirectoryBuilder::Order::buildDirectory () {
    VString iFilePath;
    parseSessionsFile (V::GetEnvironmentString (iFilePath, "VcaGlobalSessionsFile", 0), SESSION_GLOBAL);
    parseSessionsFile (V::GetEnvironmentString (iFilePath, "VcaSessionsFile", 0), SESSION_GLOBAL);

    //  $HOME/sessions.cfg 
    struct stat iHomeStats;
    bool bHomeStats = false;
    V::GetEnvironmentString (iFilePath, "HOME", 0);
    if (iFilePath.length () > 0) {

#ifdef __VMS
        iFilePath << "sessions.cfg";
#else
        iFilePath << "/sessions.cfg";
#endif
        bHomeStats = stat (iFilePath, &iHomeStats) == 0;
        if (bHomeStats)
	    parseSessionsFile (iFilePath, SESSION_LOCAL);
    }

    // Process ./sessions.cfg (only once if same as $HOME/sessions.cfg)
    iFilePath = "sessions.cfg";
    struct stat iCurrentStats;
    bool bProcessingCurrentFile = !bHomeStats || stat (iFilePath, &iCurrentStats) == 0 && (
	iHomeStats.st_dev != iCurrentStats.st_dev || iHomeStats.st_ino != iCurrentStats.st_ino
    );
    if (bProcessingCurrentFile)
        parseSessionsFile (iFilePath, SESSION_LOCAL);
}


/*********************
 *********************
 *****  Parsing  *****
 *********************
 *********************/

/*****
 * Method: parseSessionsFile
 *   Contains the main loop parsing the input sessions file
 *****/

void Vca::VcaDirectoryBuilder::Order::parseSessionsFile (
    char const *pSessionsFileName, SessionType xType
) {
    VSimpleFile iSessionsFile;
    if (!pSessionsFileName || !iSessionsFile.Open (pSessionsFileName, "r"))
	return;
    log ("VcaDirectoryBuilder::Order::parseSessionsFile '%s'", pSessionsFileName);
    VString iBuffer;
    while (iSessionsFile.GetLine (iBuffer)) {
	ParseState xState = ParseState_ExpectingTag;
	char const*  pBreakSet = g_pWhitespaceBreakSet;
	char *pBufferPtr;
	bool bIncludeIfTagAccepted = false;
	for (
	     char *pToken = strtok_r (iBuffer.storage (), pBreakSet, &pBufferPtr);
	     pToken && ParseState_Error != xState;
	     pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	) {
	    pToken += strspn (pToken, g_pWhitespaceBreakSet);	//  needed to work around a buggy VMS strtok.
	    switch (xState) {
	    case ParseState_ExpectingTag:
		xState
		    = 0 == strcasecmp (pToken, "Connection_Template")
		    ? ParseState_ExpectingSessionSentinel
		    : 0 == strcasecmp (pToken, "Notification_Template")
		    ? ParseState_ExpectingNotificationSentinel
		    : 0 == strcasecmp (pToken, "Include")
		    ? ParseState_ExpectingIncludeTarget
		    : 0 == strcasecmp (pToken, "IncludeIf")
		    ? ParseState_ExpectingIncludeIfTag
		    : ParseState_ExpectingUnknown;  
		break;
	    case ParseState_ExpectingSessionSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    xState = ParseState_Error;
		    log ("VcaDirectoryBuilder::Order::parseSessionsFile --> Error: found '%s' when expecting 'Begin'", static_cast <char const *> (pToken));
		}
		else {
		    Session::Reference pSession;
		    if (parseSessionTemplate (iSessionsFile, xType, pSession))
			insertSession (pSession);
		    xState = ParseState_ExpectingTag;
		}
		break;
	    case ParseState_ExpectingNotificationSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    log ("VcaDirectoryBuilder::Order::parseSessionsFile --> Error: found '%s' when expecting 'Begin' for Notification_Template", pToken);
		    xState = ParseState_Error;
		} else {
		    VTransient::transientServicesProvider ()->parseNotificationTemplate (iSessionsFile);
		    xState = ParseState_ExpectingTag; 
		}
		break;
	    case ParseState_ExpectingIncludeTarget:
		parseSessionsFile (pToken, xType);
		xState = ParseState_ExpectingTag;
		break;
	    case ParseState_ExpectingIncludeIfTag:
		bIncludeIfTagAccepted = includeIfTagAccepted (pToken);
		xState = ParseState_ExpectingIncludeIfTarget;
		break;
	    case ParseState_ExpectingIncludeIfTarget:
		if (bIncludeIfTagAccepted)
		    parseSessionsFile (pToken, xType);
		xState = ParseState_ExpectingTag;
		break;
	    case ParseState_ExpectingUnknown:
		xState = ParseState_ExpectingTag;
		//  ignore the unknown value...
		break;
	    default:
		log ("VcaDirectoryBuilder::Order::parseSessionsFile --> Error: unknown parse state %d[%s]", xState, pToken);
		xState = ParseState_Error;
		break;
	    } 
	    switch (xState) {
	    case ParseState_ExpectingIncludeTarget:
	    case ParseState_ExpectingIncludeIfTarget:
	    case ParseState_ExpectingSessionSentinel:
	    case ParseState_ExpectingNotificationSentinel:
	    case ParseState_ExpectingUnknown:
		pBreakSet = g_pRestOfLineBreakSet;
		break;
	    default:
		pBreakSet = g_pWhitespaceBreakSet;
	    }
	}
	iBuffer.clear ();
    }
    iSessionsFile.close ();
}


bool Vca::VcaDirectoryBuilder::Order::parseSessionTemplate(
	VString &rSessionTemplate, SessionType xType, Session::Reference &rpSession
) const { 
	bool bCompleted = false;

	VString iSessionName, iHostName, iUserName, iPassword, iProgram, iDatabase, iPort, iServerFile, iURLBase, iEntryName, iImageName;
	Session::Fallback::Reference pAliasList, pFallbackList, pNamespace;
	VTimer::microseconds_t sTimeout = 60 * 1000 * 1000; // 60 seconds
	Session::retry_count_t cRetries = UINT_MAX;
	Session::retry_delay_t sRetryDelay1 =  1 * 1000; //  1 second
	Session::retry_delay_t sRetryDelay2 = 90 * 1000; // 90 seconds
	Session::microseconds_t sTTL = 0; // infinite
	bool bRetrying = false, bOneOf = false, bPassive = false, bPassiveAgent = false, bThreePipe = true, bPattern = false, bSSH = false, bTimeout = false;
	double f64; U32 u32;


	char *pSessionPtr;
	char const* pNewLines = g_pRestOfLineBreakSet;
	 for (
		 char *iBufferChar = strtok_r (rSessionTemplate.storage(), pNewLines, &pSessionPtr);
		 iBufferChar;
		 iBufferChar = strtok_r (0, pNewLines, &pSessionPtr)
	    ) {
		VString iBuffer = iBufferChar;
		
		ParseState xState = ParseState_ExpectingTag;
	    char const* pBreakSet = g_pWhitespaceBreakSet;
	    char *pBufferPtr;
	    for (
		 char *pToken = strtok_r (iBuffer.storage(), pBreakSet, &pBufferPtr);
		 pToken && ParseState_Error != xState;
		 pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	    ) {
		pToken += strspn (pToken, g_pWhitespaceBreakSet);	//  needed to work around a buggy VMS strtok.

	    //  Expand...
		VString iCookedToken;
		switch (xState) {
		case ParseState_ExpectingTag:
		//  ... nothing:
		    iCookedToken.setTo (pToken);
		    break;
		case ParseState_ExpectingProgram:
		case ParseState_ExpectingRProgram:
		case ParseState_ExpectingDatabase:
		//  ... $$(name) environment variables:
		    ExpandDDVariables (iCookedToken, pToken);
		    break;
		default:
		//  ... $(name) environment variables:
		    ExpandSDVariables (iCookedToken, pToken);
		    break;
		}
		    
		switch (xState) {
		case ParseState_ExpectingTag:
		    xState
		        = 0 == strcasecmp (iCookedToken, "Connection_Template")
			? ParseState_ExpectingSessionSentinel

			: 0 == strcasecmp (iCookedToken, "Name")
			? ParseState_ExpectingSessionName

			: 0 == strcasecmp (iCookedToken, "Pattern")
			? ParseState_ExpectingSessionPattern

			: 0 == strcasecmp (iCookedToken, "HostName")
			? ParseState_ExpectingHostName

			: 0 == strcasecmp (iCookedToken, "UserName")
			? ParseState_ExpectingUserName

			: 0 == strcasecmp (iCookedToken, "Password")
			? ParseState_ExpectingPassword

			: 0 == strcasecmp (iCookedToken, "Program")
			? ParseState_ExpectingProgram

			: 0 == strcasecmp (iCookedToken, "RProgram")
			? ParseState_ExpectingRProgram

			: 0 == strcasecmp (iCookedToken, "Database")
			? ParseState_ExpectingDatabase

			: 0 == strcasecmp (iCookedToken, "Port")
			? ParseState_ExpectingPortNumber

			: 0 == strcasecmp (iCookedToken, "ServerFile")
			? ParseState_ExpectingServerFile

			: 0 == strcasecmp (iCookedToken, "URL")
			? ParseState_ExpectingURL

			: 0 == strcasecmp (iCookedToken, "Timeout")
			? ParseState_ExpectingTimeout

			: 0 == strcasecmp (iCookedToken, "Option")
			? ParseState_ExpectingOption

			: 0 == strcasecmp (iCookedToken, "UseNamespace")
			? ParseState_ExpectingUseNamespaceTarget

			: 0 == strcasecmp (iCookedToken, "Use")
			? ParseState_ExpectingUseTarget

			: 0 == strcasecmp (iCookedToken, "ElseUse")
			? ParseState_ExpectingElseUseTarget

			: 0 == strcasecmp (iCookedToken, "RetryCount")
			? ParseState_ExpectingRetryCount

			: 0 == strcasecmp (iCookedToken, "RetryDelay")
			? ParseState_ExpectingRetryDelay1

			: 0 == strcasecmp (iCookedToken, "Entry")
			? ParseState_ExpectingEntryName

			: 0 == strcasecmp (iCookedToken, "Library")
			? ParseState_ExpectingImageName

			: 0 == strcasecmp (iCookedToken, "TTL")
			? ParseState_ExpectingTTL

			: 0 == strcasecmp (iCookedToken, "Notify")
			? ParseState_ExpectingInfoServer
			
			: ParseState_Error;
		    if (ParseState_Error == xState)
			log ("VcaDirectoryBuilder::Order::parseSessionTemplate --> Unknown tag '%s'", static_cast <char const *> (iCookedToken.content ()));
		    break;
		case ParseState_ExpectingSessionSentinel:
		    if (strcmp (iCookedToken, "Begin") == 0) {
			xState = ParseState_Error;
			log ("VcaDirectoryBuilder::Order::parseSessionTemplate --> Error: found 'Begin' when expecting 'End'");
		    }
		    else if (strcmp (iCookedToken, "End") == 0) {
			if (pAliasList || bOneOf) {
			    rpSession.setTo (new AliasSession (iSessionName, xType, pAliasList, bOneOf));
			    bCompleted = true;
			} else if (iProgram.isntEmpty ()) {
			    rpSession.setTo (
				new ProcessSession (
				    iSessionName, xType, sTimeout, bTimeout, bPassive, bPassiveAgent, iHostName, iUserName, iPassword,
				    iProgram, iDatabase, bThreePipe, bSSH
				)
			    );
			    bCompleted = true;
			} else if (iURLBase.isntEmpty ()) {
			    static VString const iPort80 ("80", false);
			    rpSession.setTo (
				iPort.isntEmpty () ? new PortSession (
				    iSessionName, xType, sTimeout, bTimeout, iURLBase, iHostName, iPort
				) : iServerFile.isEmpty () ? new PortSession (
				    iSessionName, xType, sTimeout, bTimeout, iURLBase, iHostName, iPort80
				) : new PortSession (iSessionName, xType, sTimeout, bTimeout, iURLBase, iServerFile)
			    );
			    bCompleted = true;
			} else if (iPort.isntEmpty ()) {
			    rpSession.setTo (new PortSession (iSessionName, xType, sTimeout, bTimeout, bPassive, iHostName, iPort));
			    bCompleted = true;
			} else if (iServerFile.isntEmpty ()) {
			    rpSession.setTo (new PortSession (iSessionName, xType, sTimeout, bTimeout, bPassive, iServerFile));
			    bCompleted = true;
			} else if (iEntryName.isntEmpty () && iImageName.isntEmpty ()) {
			    rpSession.setTo (new LinkedSession (iSessionName, xType, iEntryName, iImageName));
			    bCompleted = true;
			}
			if (bCompleted) {
			    rpSession->setFallbackListTo (pFallbackList);
			    if (bRetrying)
				rpSession->setRetrySettings (cRetries, sRetryDelay1, sRetryDelay2);
			    if (sTTL > 0)
				rpSession->setTTL (sTTL);
			    rpSession->setIsPatternTo (bPattern);
			}
		    }
		    break;
		case ParseState_ExpectingSessionName:
		    bPattern = false;
		    iSessionName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingSessionPattern:
		    bPattern = true;
		    iSessionName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingInfoServer:
		    VTransient::transientServicesProvider ()->updateInfoServerEntry (iCookedToken);
		    xState = ParseState_ExpectingNSTemplate;
		    // fprintf(stderr, "InofServer: %s\n", iCookedToken.content ());
		    break;
		case ParseState_ExpectingNSTemplate:
		    // fprintf(stderr, "InfoServer Event Action Map: %s\n", iCookedToken.content ());
		    VTransient::transientServicesProvider ()->parseNSTFile (iCookedToken);
		    break;
		case ParseState_ExpectingHostName:
		    iHostName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingUserName:
		    iUserName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingPassword:
		    iPassword.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingProgram:
		    iProgram.setTo (iCookedToken);
		    bThreePipe = true;
		    break;
		case ParseState_ExpectingRProgram:
		    iProgram.setTo (iCookedToken);
		    bThreePipe = false;
		    break;
		case ParseState_ExpectingDatabase:
		    iDatabase.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingPortNumber:
		    iPort.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingRetryCount:
		    if (1 == sscanf (iCookedToken, " %u", &u32))
			cRetries = u32;
		    break;
		case ParseState_ExpectingRetryDelay1:
		    if (1 == sscanf (iCookedToken, " %lg", &f64) && f64 >= 0.0) {
			sRetryDelay2 = sRetryDelay1 = static_cast<Session::retry_delay_t>(f64 * 1e3);
			xState = ParseState_ExpectingRetryDelay2;
		    }
		    break;
		case ParseState_ExpectingRetryDelay2:
		    if (1 == sscanf (iCookedToken, " %lg", &f64) && f64 >= 0.0) {
			sRetryDelay2 = static_cast<Session::retry_delay_t>(f64 * 1e3);
			xState = ParseState_ExpectingUnknown;
		    }
		    break;
		case ParseState_ExpectingTTL:
		    if (1 == sscanf (iCookedToken, " %lg", &f64) && f64 >= 0.0) {
			sTTL = static_cast<Session::microseconds_t>(f64 * 1e6);
		    }
		    break;
		case ParseState_ExpectingServerFile:
		    iServerFile.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingURL:
		    iURLBase.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingEntryName:
		    iEntryName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingImageName:
		    iImageName.setTo (iCookedToken);
		    break;
		case ParseState_ExpectingTimeout:
		    if (1 == sscanf (iCookedToken, " %lg", &f64) && f64 > 0.0) {
			sTimeout = static_cast<VTimer::microseconds_t>(f64 * 1e6);
			bTimeout = true;
		    }
		    break;
		case ParseState_ExpectingOption:
		    if (0 == strcasecmp (iCookedToken, "choose"))
			bOneOf = true;
		    else if (0 == strcasecmp (iCookedToken, "hidden")) {
			xType = SESSION_HIDDEN;
		    } else if (0 == strcasecmp (iCookedToken, "passive")) {
			bPassive = true;
			bPassiveAgent = false;
		    } else if (0 == strcasecmp (iCookedToken, "passiveAgent")) {
			bPassive = true;
			bPassiveAgent = true;
		    } else if (0 == strcasecmp (iCookedToken, "retry")) {
			bRetrying = true;
                    } else if (0 == strcasecmp (iCookedToken, "rexec")) {
                        bSSH = false;
		    } else if (0 == strcasecmp (iCookedToken, "ssh")) {
                        bSSH = true;
                    }
		    break;
		case ParseState_ExpectingUseNamespaceTarget:
		    pNamespace.setTo (new Session::Fallback (iCookedToken, 0, pNamespace));
		    break;
		case ParseState_ExpectingUseTarget:
		    pAliasList.setTo (new Session::Fallback (iCookedToken, pAliasList, pNamespace));
		    break;
		case ParseState_ExpectingElseUseTarget:
		    pFallbackList.setTo (new Session::Fallback (iCookedToken, pFallbackList, pNamespace));
		    break;
		default:
		    log ("VcaDirectoryBuilder::Order::parseSessionTemplate --> Error: unknown parse state %d[%s]", xState, iCookedToken.content ());
		    xState = ParseState_Error;
		    break;
		}
		switch (xState) {
		case ParseState_ExpectingOption:
		case ParseState_ExpectingRetryDelay1:
		case ParseState_ExpectingRetryDelay2:
		case ParseState_ExpectingTag:
		case ParseState_ExpectingInfoServer:
		    pBreakSet = g_pWhitespaceBreakSet;
		    break;
		default:
		    pBreakSet = g_pRestOfLineBreakSet;
		    break;
		}
	    }
	    iBuffer.clear ();
		
	}
	return bCompleted;
	
}

bool Vca::VcaDirectoryBuilder::Order::parseSessionTemplate (
    VSimpleFile &rSessionsFile, SessionType xType, Session::Reference &rpSession
)  const {
	VString templateString;
	if(extractSessionTemplateString(rSessionsFile, templateString)) {
		return parseSessionTemplate(templateString, xType, rpSession);
	} else {
		return false;
	}
}

bool Vca::VcaDirectoryBuilder::Order::extractSessionTemplateString (
	VSimpleFile &rSessionsFile, VString &templateString
) const { 
	templateString = "";
	VString iBuffer;
	bool bCompleted = false;
        char *pBufferPtr;
	while (!bCompleted && rSessionsFile.GetLine(iBuffer)) {
            char *line = strtok_r (iBuffer.storage (), g_pRestOfLineBreakSet, &pBufferPtr);
            if (line) {
		templateString << line << "\n";
		if (0 == strcasecmp (line, "Connection_Template End"))
			bCompleted = true;
            }
            iBuffer.clear();
	}
	return bCompleted;
}


/******************************
 ******************************
 *****  Directory Update  *****
 ******************************
 ******************************/

/*****
 * Method: insertSession
 *    Inserts entry (Name,<ObjectSource,Description>) into the directory provided the credentials
 *    are present. If for an Entry the provided password is NULL and there is no credentials
 *    in the list of available credentials for this entry, then the entry is not inserted.
 *****/


bool Vca::VcaDirectoryBuilder::Order::insertSession (Session *pSession) {
    return pSession && pSession->addToDirectory (m_pDirectory);
}


/**************************************
 **************************************
 *****                            *****
 *****  Vca::VcaDirectoryBuilder  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaDirectoryBuilder::VcaDirectoryBuilder () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaDirectoryBuilder::~VcaDirectoryBuilder () {
}

/********************************
 ********************************
 *****  Directory Creation  *****
 ********************************
 ********************************/

Vca::VDirectory* Vca::VcaDirectoryBuilder::newDirectory_() const {
    return new Directory ();
}

void Vca::VcaDirectoryBuilder::build (VDirectory *pDirectory) {
    Order::Reference const pOrder (new Order (this, pDirectory));
    pOrder->buildDirectory ();
}

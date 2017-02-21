/*****  Vca_VDirectory Implementation  *****/

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

#include "Vca_VDirectory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaGofer.h"
#include "Vca_VGoferExogenousInterface.h"
#include "Vca_VInterfaceQuery.h"

#include "Vca_VStatus.h"


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VDirectory::Entry::RolePlayer  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace Vca {
    class VDirectory::Entry::RolePlayer : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (RolePlayer, VRolePlayer);

    //  Construction
    public:
	RolePlayer (VDirectory *pDirectory, Entry *pEntry);

    //  Destruction
    private:
	~RolePlayer () {
	    m_pEntry->detach (this);
	}

    //  IDirectoryEntry Role
    private:
	VRole<ThisClass,IDirectoryEntry> m_pIDirectoryEntry;
    public:
	void getRole (IDirectoryEntry::Reference&rpRole) {
	    m_pIDirectoryEntry.getRole (rpRole);
	}

    //  IDirectoryEntry Methods
	public/*private*/:
	void GetObject (
	    IDirectoryEntry *pRole, ObjectSink *pClient, VTypeInfo *pType
	);
	void SetObject		(IDirectoryEntry *pRole, Object *pObject);
	void SetSource		(IDirectoryEntry *pRole, ObjectSource *pSource);
	void SetDescription	(IDirectoryEntry *pRole, VString const &rDescription);
	void GetDescription	(IDirectoryEntry *pRole, IVReceiver<VString const &>* pReceiver);

    //  IBinding Methods
    public:
	void GetNamespace	(IBinding *pRole, IVReceiver<INamespace*> *pClient);
	void GetName		(IBinding *pRole, IVReceiver<VString const&> *pClient);

    //  IResource Methods
    public:
	void Close (IResource *pRole);

    //  Access
    public:
	VDirectory *directory () const {
	    return m_pDirectory;
	}
	Entry *entry () const {
	    return m_pEntry;
	}
	VString const &name () const {
	    return m_pEntry->name ();
	}
    private:
	void getObject (VInterfaceQuery const &rQuery) {
	    m_pEntry->getObject (rQuery);
	}

    //  Update
    public:
	void setObject (Object *pObject) {
	    m_pEntry->setObject (0, m_pDirectory, pObject);
	}
	void setSource (ObjectSource *pSource) {
	    m_pEntry->setSource (0, m_pDirectory, pSource);
	}
	void setDescription (VString const &rDescription) {
	    m_pEntry->setDescription (0, m_pDirectory, rDescription);
	} 

    //  State
    private:
	VDirectory::Reference	const m_pDirectory;
	Entry::Reference	const m_pEntry;
    };

    bool VDirectory::g_TTLDisabled = false;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDirectory::Entry::RolePlayer::RolePlayer (
    VDirectory *pDirectory, Entry *pEntry
) : m_pDirectory (pDirectory), m_pEntry (pEntry), m_pIDirectoryEntry (this) {
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/**********************************
 *****  Vca::IDirectoryEntry  *****
 **********************************/

void Vca::VDirectory::Entry::RolePlayer::GetObject (
    IDirectoryEntry *pRole, ObjectSink *pClient, VTypeInfo *pType
) {
    VInterfaceQuery iQuery (pClient, pType);
    getObject (iQuery);
}

void Vca::VDirectory::Entry::RolePlayer::SetObject (IDirectoryEntry *pRole, Object *pObject) {
    setObject (pObject);
}

void Vca::VDirectory::Entry::RolePlayer::SetSource (
    IDirectoryEntry *pRole, ObjectSource *pSource
) {
    setSource (pSource);
}

void Vca::VDirectory::Entry::RolePlayer::SetDescription (IDirectoryEntry *pRole, VString const &rDescription) {
    setDescription (rDescription);
}

void Vca::VDirectory::Entry::RolePlayer::GetDescription (IDirectoryEntry *pRole, IVReceiver<VString const &>* pReceiver) {
    if (pReceiver)
	pReceiver->OnData (m_pEntry->getDescription ());
}

/***************************
 *****  Vca::IBinding  *****
 ***************************/

void Vca::VDirectory::Entry::RolePlayer::GetNamespace (
    IBinding *pRole, IVReceiver<INamespace*> *pClient
) {
    if (pClient) {
	IDirectory::Reference pIDirectory;
	m_pDirectory->getRole (pIDirectory);
	pClient->OnData (pIDirectory);
    }
}

void Vca::VDirectory::Entry::RolePlayer::GetName (
    IBinding *pRole, IVReceiver<VString const&> *pClient
) {
    if (pClient) {
	pClient->OnData (name ());
    }
}

/****************************
 *****  Vca::IResource  *****
 ****************************/

void Vca::VDirectory::Entry::RolePlayer::Close (IResource *pRole) {
    m_pEntry->deleteFrom (m_pDirectory);
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::VDirectory::Entry  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDirectory::Entry::Entry (VString const &rName) : m_iName (rName) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDirectory::Entry::~Entry () {
}

void Vca::VDirectory::Entry::detach (RolePlayer *pRolePlayer) {
    m_pRolePlayer.clearIf (pRolePlayer);
}


/**************************
 **************************
 *****  Role Support  *****
 **************************
 **************************/

void Vca::VDirectory::Entry::supply (BindingSink *pClient, VDirectory *pDirectory) {
    if (pClient) {
	IDirectoryEntry::Reference pRole;
	supply (pRole, pDirectory);
	pClient->OnData (pRole);
    }
}

void Vca::VDirectory::Entry::supply (EntrySink *pClient, VDirectory *pDirectory) {
    if (pClient) {
	IDirectoryEntry::Reference pRole;
	supply (pRole, pDirectory);
	pClient->OnData (pRole);
    }
}

void Vca::VDirectory::Entry::supply (IDirectoryEntry::Reference &rpRole, VDirectory *pDirectory) {
    if (m_pRolePlayer.isNil ())
	m_pRolePlayer.setTo (new RolePlayer (pDirectory, this));
    m_pRolePlayer->getRole (rpRole);
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VDirectory::Entry::getObject (VInterfaceQuery const &rQuery) {
    if (m_pBinding)
	m_pBinding->supply (rQuery);
    else
	rQuery.onZero ();
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VDirectory::Entry::deleteFrom (VDirectory *pDirectory) {
    pDirectory->detach (this);
}

void Vca::VDirectory::Entry::setObject (EntrySink *pClient, VDirectory *pDirectory, Object *pObject) {
    setSource (pClient, pDirectory, new VGoferExogenousInterface<IVUnknown> (pObject));
}

void Vca::VDirectory::Entry::setObject (EntrySink *pClient, VDirectory *pDirectory, Object *pObject, VString const& rDescription) {
    m_iDescription.setTo (rDescription);
    setObject (pClient, pDirectory, pObject);
}

void Vca::VDirectory::Entry::setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectGofer *pSource) {
    m_pBinding.setTo (pSource);
    supply (pClient, pDirectory);
}

void Vca::VDirectory::Entry::setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectGofer *pSource, VString const& rDescription) {
    m_iDescription.setTo (rDescription);
    setSource (pClient, pDirectory, pSource);
}

void Vca::VDirectory::Entry::setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectSource *pSource) {
    setSource (pClient, pDirectory, new Gofer::Sourced<IVUnknown> (pSource));
}

void Vca::VDirectory::Entry::setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectSource *pSource, VString const& rDescription) {
    m_iDescription.setTo (rDescription);
    setSource (pClient, pDirectory, pSource);
}

void Vca::VDirectory::Entry::setDescription (EntrySink *pClient, VDirectory *pDirectory, VString const &rDescription) {
    m_iDescription.setTo (rDescription);
    supply (pClient, pDirectory);
}


/*****************************
 *****************************
 *****                   *****
 *****  Vca::VDirectory  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDirectory::VDirectory () : m_pIDirectory (this) {
    traceInfo ("Creating Vca::VDirectory");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDirectory::~VDirectory () {
    traceInfo ("Destroying Vca::VDirectory");
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VDirectory::setObject (VString const &rName, IVUnknown *pObject) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setObject(0, this, pObject);
}

void Vca::VDirectory::setObject (VString const &rName, IVUnknown *pObject, VString const& rDescription) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setObject(0, this, pObject, rDescription);
}

void Vca::VDirectory::setSource (VString const &rName, ObjectGofer *pSource) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setSource(0, this, pSource);
}

void Vca::VDirectory::setSource (VString const &rName, ObjectGofer *pSource, VString const& rDescription) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setSource(0, this, pSource, rDescription);
}

void Vca::VDirectory::setSource (VString const &rName, ObjectSource *pSource) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setSource(0, this, pSource);
}

void Vca::VDirectory::setSource (VString const &rName, ObjectSource *pSource, VString const& rDescription) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setSource(0, this, pSource, rDescription);
}

void Vca::VDirectory::setDescription (VString const &rName, VString const &rDescription) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setDescription(0, this, rDescription);  
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VDirectory::hasEntry (VString const &rName) const {
    unsigned int xEntry;
    return m_iEntries.Locate (rName, xEntry);
}

//  Note: Need to implement a VkSetOf::const_iterator to make this method a const method....

void Vca::VDirectory::getDirectoryDescription (VString &rDescription) {
    EntryMap::Iterator iterator (m_iEntries);
    while (iterator.isNotAtEnd ()) {
	Entry::Reference pEntry;
	pEntry.setTo (iterator.value ());
	rDescription << pEntry->getDescription ();
	++iterator;
    }
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/*****************************
 *****  Vca::IDirectory  *****
 *****************************/

void Vca::VDirectory::GetObject (
    IDirectory *pRole, ObjectSink *pClient, VString const &rName, VTypeInfo *pType
) {
    if (pClient) {
	Entry::Reference pEntry;
	if (!locate (pEntry, rName)) {
	    VString sMessage ("Directory has no definition for ");
	    sMessage.quote (rName);
	    pClient->OnError (0, sMessage);
	} else {
	    VInterfaceQuery iQuery (pClient, pType);
	    pEntry->getObject (iQuery);
	}
    }
}

void Vca::VDirectory::SetObject (
    IDirectory *pRole, EntrySink *pClient, VString const &rName, Object *pObject
) {
    if (pClient) {
	Entry::Reference pEntry;
	supply (pEntry, rName);
	pEntry->setObject (pClient, this, pObject);
    }
}

void Vca::VDirectory::SetSource (
    IDirectory *pRole, EntrySink *pClient, VString const &rName, ObjectSource *pSource
) {
    if (pClient) {
	Entry::Reference pEntry;
	supply (pEntry, rName);
	pEntry->setSource (pClient, this, pSource);
    }
}

void Vca::VDirectory::SetDescription (
    IDirectory *pRole, EntrySink *pClient, VString const &rName, VString const &rDescription
) {
    Entry::Reference pEntry;
    supply (pEntry, rName);
    pEntry->setDescription (pClient, this, rDescription);
}

void Vca::VDirectory::GetDescription (
    IDirectory *pRole, IVReceiver<VString const &>* pReceiver, VString const &rName
) {
    if (pReceiver) {
	Entry::Reference pEntry;
	supply (pEntry, rName);
	pReceiver->OnData (pEntry->getDescription ());
    }
}

void Vca::VDirectory::GetEntries (
    IDirectory *pRole, IVReceiver<VkDynamicArrayOf <VString> const & > *pReceiver
) {
    if (pReceiver) {
	VkDynamicArrayOf <VString> iResult (m_iEntries.cardinality ());

	EntryMap::Iterator iterator (m_iEntries);
	unsigned int xElement = 0;

	while (iterator.isNotAtEnd ()) {
	    iResult[xElement++] = iterator.key ();
	    ++iterator;
	}
	pReceiver->OnData (iResult);
    }
}

/*****************************
 *****  Vca::INamespace  *****
 *****************************/

void Vca::VDirectory::GetBinding (
    INamespace *pRole, IVReceiver<IBinding*>*pClient, VString const &rName
) {
    if (pClient) {
	Entry::Reference pEntry;
	if (locate (pEntry, rName))
	    pEntry->supply (pClient, this);
	else
	    pClient->OnData (0);
    }
}


/*************************
 *************************
 *****  Maintenance  *****
 *************************
 *************************/

void Vca::VDirectory::detach (Entry const *pEntry) {
    unsigned int xEntry;
    if (m_iEntries.Locate (pEntry->name (), xEntry) && m_iEntries[xEntry] == pEntry)
	m_iEntries.Delete (pEntry->name ());
}

bool Vca::VDirectory::locate (Entry::Reference &rpEntry, VString const &rName) {
    unsigned int xEntry;
    if (m_iEntries.Locate (rName, xEntry))
        rpEntry.setTo (m_iEntries[xEntry]);
    else
	rpEntry.clear ();

    return rpEntry.isntNil ();
}

bool Vca::VDirectory::supply (Entry::Reference &rpEntry, VString const &rName) {
    unsigned int xEntry;
    bool const bCreated = m_iEntries.Insert (rName, xEntry);
    if (!bCreated)
	rpEntry.setTo (m_iEntries[xEntry]);
    else {
	rpEntry.setTo (new Entry (rName));
	m_iEntries[xEntry].setTo (rpEntry);
    }
    return bCreated;
}

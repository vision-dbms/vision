#ifndef Vca_VRoleHolder_Interface
#define Vca_VRoleHolder_Interface

/**************************
 *****  Declarations  *****
 **************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VCohort;
    class VRolePlayer;

    /*************************
     *----  VRoleHolder  ----*
     *************************/

    /**
     * Non-templated base class for VRole.
     */
    class Vca_API VRoleHolder {
	DECLARE_FAMILY_MEMBERS (VRoleHolder, void);
	friend class VRolePlayer;

    //  Aliases
    public:
	typedef VcaOID::count_t count_t;

    //  Construction
    protected:
	VRoleHolder (VRolePlayer *pActor);

    //  Destruction
    protected:
	~VRoleHolder () {
	}

    //  Access
    public:
	virtual count_t exportCount_() const = 0;
	virtual count_t weakExportCount_() const = 0;
	virtual void getRole_(IVUnknown::Reference &rpRole, IVUnknown *pController) = 0;
	virtual bool isExported_() const = 0;
	virtual bool isInstantiated_() const = 0;
	virtual bool isReferenced_() const = 0;
	virtual VTypeInfo *typeInfo_() const = 0;

    protected:
	VRoleHolder *successor () const {
	    return m_pSuccessor;
	}

    //  Query
    public:
	bool implements (VTypeInfo *pTypeInfo) const;
    private:
	Pointer const m_pSuccessor;
    };

    /*******************
     *----  VRole  ----*
     *******************/

    template <class Actor, class Interface> class VRole : public VRoleHolder {
    public:
	typedef VRole<Actor,Interface> ThisRole;
	DECLARE_FAMILY_MEMBERS (ThisRole, VRoleHolder);

    //  Aliases
    public:
	typedef typename Interface::template Role<Actor,Interface>::Implementation ActorRole;
	typedef typename Interface::Reference InterfaceReference;

    //  Delegate Implementation
    public:
        /**
         * Provides QueryInterface functionality in the case that an interface was acquired from an aggregated VRolePlayer.
         * Delegates QueryInterface functionality to the aggregator.
         * @see InterfaceAggregation
         */
	class DelegateRole : public ActorRole {
	    DECLARE_FAMILY_MEMBERS (DelegateRole,ActorRole);
	    friend class VRole<Actor,Interface>;

	//  Run Time Type
	public:
	    USING (Interface::RTT)
	    V::VRunTimeType *rtt () const {
		return &RTT;
	    }

	//  Construction
	private:
	    DelegateRole (
		ThisRole *pRole, IVUnknown *pController
	    ) : m_pRole (pRole), m_pController (pController) {
		actor()->retain ();
	    }

	//  Destruction
	private:
	    ~DelegateRole () {
		actor()->release ();
	    }
	    void deleteThis () {
		delete this;
	    }

	//  Access
	public:
	    Actor *actor_() const {
		return actor ();
	    }
	    Actor *actor () const {
		return m_pRole->actor ();
	    }
	    VCohort *actorCohort () const {
		return m_pRole->actorCohort ();
	    }

	//  Message Handling
	public:
            /**
             * Delegates to (no pun intended) VMessageScheduler::mustSchedule(VCohort) to decide whether or not the actor's cohort can be claimed locally.
             */
	    bool defersTo (VMessageScheduler &rScheduler) {
		return rScheduler.mustSchedule (actorCohort ());
	    }

	//  RolePlayer
	private:
            /**
             * Delegates to the local non-virtual rolePlayer() implementation.
             */
	    virtual /* override */ VRolePlayer *rolePlayer_() const {
		return rolePlayer();
	    }
	public:
	    VRolePlayer *rolePlayer() const {
		return actor ();
	    }
	    VCohort *rolePlayerCohort () const {
		return actorCohort ();
	    }

	//  QI
	private:
            /**
             * Delegates to the aggregator.
             */
	    void QueryInterface (VTypeInfo *pTypeInfo, IObjectSink *pObjectSink) {
                // Crash resistance: m_pController should always be non-null.
		if (m_pController)
		    m_pController->QueryInterface (pTypeInfo, pObjectSink);
		else
		    BaseClass::QI (pTypeInfo, pObjectSink);
	    }

	//  State
	private:
	    ThisRole*		 const m_pRole;
            /**
             * Aggregator used to acquire this interface.
             */
	    IVUnknown::Reference const m_pController;
	};

    //  Primary Implementation
    public:
        /**
         * Provides direct QueryInterface functionality (no aggregation).
         * Relies on the fact that the default implementation (in VRolePlayer) provides direct QueryInterface functionality.
         */
	class PrimaryRole : public ActorRole {
	    DECLARE_FAMILY_MEMBERS (PrimaryRole,ActorRole);
	    friend class VRole<Actor,Interface>;

	//  Run Time Type
	public:
	    USING (Interface::RTT)
	    V::VRunTimeType *rtt () const {
		return &RTT;
	    }

	//  Construction
	private:
	    PrimaryRole (
		ThisRole *pRole
	    ) : m_pRole (pRole), m_bActorAlive (true), m_bReferencingActor (true) {
		pRole->actor()->retain ();
	    }

	//  Destruction
	private:
	    ~PrimaryRole () {
		if (m_bActorAlive)
		    m_pRole->endRole (this, m_bReferencingActor);
	    }
	    void deleteThis () {
		makeActorReferenceRole ();
		if (m_iReferenceCount.isZero ())
		    delete this;
	    }

	//  Access
	public:
	    Actor *actor_() const {
		return actor ();
	    }
	    Actor *actor () const {
		return m_bActorAlive ? m_pRole->actor () : 0;
	    }
	    VCohort *actorCohort () const {
		return m_bActorAlive ? m_pRole->actorCohort () : 0;
	    }

	//  Query
	public:
	    bool referencesActor () const {
		return m_bReferencingActor;
	    }
	    bool referencedByActor () const {
		return !m_bReferencingActor;
	    }

	//  Message Handling
	public:
	    bool defersTo (VMessageScheduler &rScheduler) {
		return m_bActorAlive && rScheduler.mustSchedule (m_pRole->actorCohort ());
	    }

	//  Reference Management
	public:
	    USING (Interface::release)
	private:
	    USING (Interface::m_iReferenceCount)

	/*-------------------------------------------------------------------------*
	 *  These routines must be called under the protection of a reference to
	 *	the role or unpredictable failures may occur.
	 *-------------------------------------------------------------------------*/
	    void makeRoleReferenceActor () {
		m_bReferencingActor = m_bReferencingActor || m_pRole->makeRoleReferenceActor (this);
	    }
	    void makeActorReferenceRole () {
		m_bReferencingActor = m_bReferencingActor && m_pRole->makeActorReferenceRole (this);
	    }

	    void detach (ThisRole const *pRole) {
		if (m_pRole == pRole) {
		    m_bActorAlive = false;
		    if (referencedByActor ())
			release ();
		}
	    }

	//  RolePlayer
	private:
	    VRolePlayer *rolePlayer_() const {
		return rolePlayer();
	    }
	public:
	    VRolePlayer *rolePlayer() const {
		return actor ();
	    }
	    VCohort *rolePlayerCohort () const {
		return actorCohort ();
	    }

	//  State
	private:
	    ThisRole* const	m_pRole;
	    bool		m_bActorAlive;
	    bool		m_bReferencingActor;	// implies actor isn't referencing role
	};
	friend class PrimaryRole;

    //  Construction
    public:
	VRole (Actor *pActor) : BaseClass (pActor), m_pActor (pActor), m_pRole (0) {
	}

    //  Destruction
    public:
    /*-------------------------------------------------------------------------*
     *  If this holder is being destroyed, it references the implementation
     *  and not the other way around.
     *-------------------------------------------------------------------------*/
	~VRole () {
	    if (m_pRole)
		m_pRole->detach (this);
	}

    //  Access
    public:
	Actor *actor () const {
	    return m_pActor;
	}
	VCohort *actorCohort () const {
	    return m_pActor->cohort ();
	}

	count_t exportCount_() const {
	    return m_pRole ? m_pRole->exportCount () : 0;
	}

	count_t weakExportCount_() const {
	    return m_pRole ? m_pRole->weakExportCount () : 0;
	}

	void getRole (InterfaceReference &rpRole, IVUnknown *pController = 0) {
	    if (pController)
		rpRole.setTo (new DelegateRole (this, pController));
	    else if (m_pRole) {
		rpRole.setTo (m_pRole);
		m_pRole->makeRoleReferenceActor ();
	    } else {
		m_pRole = new PrimaryRole (this);
		rpRole.setTo (m_pRole);
	    }
	}
	void getRole_(IVUnknown::Reference &rpIVUnknown, IVUnknown *pController) {
	    InterfaceReference pRole;
	    getRole (pRole, pController);
	    rpIVUnknown.setTo (pRole);
	}

	bool isExported_() const {
	    return m_pRole && m_pRole->isExported ();
	}
	bool isInstantiated_() const {
	    return m_pRole ? true : false;
	}
	bool isReferenced_() const {
	    return m_pRole && m_pRole->referencesActor();
	}


	VTypeInfo *typeInfo_() const {
	    return Interface::typeInfo ();
	}

    //  Query
    public:
	/**
	 * Returns true if and only if the given interface is the primary role created by this role holder.
         *
         * This method does not recognize delegation interfaces created at the behest of objects that are aggregating the object implementing this role.
	 */
	bool supplies (IVUnknown const *pInterface) const {
	    return m_pRole == pInterface;
	}

    //  Reference Maintenance
    private:
	void endRole (PrimaryRole const *pRole, bool bReferencingActor) {
	    if (m_pRole == pRole) {
		m_pRole = 0;
		if (bReferencingActor)
		    m_pActor->release ();
	    }
	}

/*-------------------------------------------------------------------------*
 *  These routines must be called under the protection of a reference to
 *	the role or unpredictable failures may occur.
 *
 *	These routines return true if the role references the actor on return.
 *-------------------------------------------------------------------------*/
	bool makeRoleReferenceActor (PrimaryRole *pRole) {
	    if (m_pRole != pRole)
		return false;
	    m_pActor->retain ();
	    m_pRole->release ();
	    return true;
	}
	bool makeActorReferenceRole (PrimaryRole *pRole) {
	//  The reference reversal cannot be safely done if the role holds
	//  the last reference to the actor...
	    if (m_pRole != pRole || m_pActor->referenceCount () <= 1)
		return true;
	    m_pRole->retain ();
	    m_pActor->release ();
	    return false;
	}

    //  State
    private:
	Actor *const	m_pActor;
	PrimaryRole*	m_pRole;
    };
}


#endif

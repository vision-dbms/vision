#ifndef Vca_VReceiver_Interface
#define Vca_VReceiver_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "IVReceiver.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <
	class Actor, class Datum, class Base = VRolePlayer
    > class VReceiver : public Base {
    //  Run Time Type
    public:
	typedef VReceiver<Actor,Datum,Base> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, Base);

    //  Aliases
    public:
	typedef VReference<Actor> ActorReference;
	typedef void (Actor::*Action) (ThisClass *pThis, Datum iDatum);
	typedef void (Actor::*EndAction) (ThisClass *pThis);
	typedef void (Actor::*ErrorAction) (
	    ThisClass *pThis, IError *pError, VString const &rText
	);

	typedef IVReceiver<Datum> ReceiverInterface;

    //  Construction
    public:
	VReceiver (Actor *pActor, Action pAction)
	    : m_pActor (pActor), m_pAction (pAction)
	    , m_bEndAction (false), m_bErrorAction (false)
	{
	}
	VReceiver (Actor *pActor, Action pAction, EndAction pEndAction)
	    : m_pActor (pActor), m_pAction (pAction), m_bErrorAction (false)
	    , m_bEndAction (true), m_pEndAction (pEndAction)
	{
	}
	VReceiver (Actor *pActor, Action pAction, ErrorAction pErrorAction)
	    : m_pActor (pActor), m_pAction (pAction), m_bEndAction (false)
	    , m_bErrorAction (true), m_pErrorAction (pErrorAction)
	{
	}
	VReceiver (Actor *pActor, Action pAction, EndAction pEndAction, ErrorAction pErrorAction)
	    : m_pActor (pActor), m_pAction (pAction)
	    , m_bEndAction (true), m_pEndAction (pEndAction)
	    , m_bErrorAction (true), m_pErrorAction (pErrorAction)
	{
	}

    //  Destruction
    private:
	~VReceiver () {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IVReceiver Role
    private:
	VRole<ThisClass,ReceiverInterface> m_pReceiverRole;
    public:
	void getRole (VReference<ReceiverInterface>&rpRole) {
	    m_pReceiverRole.getRole (rpRole);
	}

    //  IVReceiver Methods
    public/*private*/:
	void OnData (ReceiverInterface *pRole, Datum iDatum) {
	    (m_pActor.referent()->*m_pAction) (this, iDatum);
	}

    //  IClient Methods
    public/*private*/:
	void OnEnd (ReceiverInterface *pRole) {
	    if (m_bEndAction)
		(m_pActor.referent()->*m_pEndAction) (this);
	    else if (m_bErrorAction) {
		VString iEOD ("EOD", false);
		OnError (pRole, 0, iEOD);
	    }
	}
	void OnError (ReceiverInterface *pRole, IError *pError, VString const &rText) {
	    if (m_bErrorAction) {
		(m_pActor.referent()->*m_pErrorAction) (this, pError, rText);
	    }
	}

    //  Update
    public:
	void setActorTo (ActorReference const &rActorReference) {
	    m_pActor.setTo (rActorReference);
	}
	void setActorTo (Actor *pActor) {
	    m_pActor.setTo (pActor);
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}
	void setEndActionTo (EndAction pAction) {
	    m_pEndAction = pAction;
	    m_bEndAction = true;
	}
	void setErrorActionTo (ErrorAction pAction) {
	    m_pErrorAction = pAction;
	    m_bErrorAction = true;
	}
	void unsetEndAction () {
	    m_bEndAction = false;
	}
	void unsetErrorAction () {
	    m_bErrorAction = false;
	}

    //  State
    private:
	ActorReference	m_pActor;
	Action		m_pAction;
	EndAction	m_pEndAction;
	ErrorAction	m_pErrorAction;
	bool		m_bEndAction;
	bool		m_bErrorAction;
    };
}


#endif

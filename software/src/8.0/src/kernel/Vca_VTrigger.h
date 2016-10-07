#ifndef Vca_VTrigger_Interface
#define Vca_VTrigger_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_ITrigger.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Actor> class VTrigger : public ITrigger {
	DECLARE_CONCRETE_RTTLITE (VTrigger<Actor>, ITrigger);

    //  Member Types
    public:
	typedef typename Actor::Reference ActorReference;
	typedef void (Actor::*Action) (ThisClass *pThis);
	typedef VCohort::Reference CohortReference;

    //  Construction
    public:
	VTrigger (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction), m_pCohort (VCohort::Here ()) {
	}

    //  Destruction
    protected:
	~VTrigger () {
	};

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

    //  Message Handling
    private:
	virtual bool defersTo (VMessageScheduler &rScheduler) {
	    return rScheduler.mustSchedule (m_pCohort);
	}

    //  IVUnknown Methods
    private:
	void VINTERFACE_MEMBERIMPL(QueryInterface) (VTypeInfo *pTypeInfo, IVReceiver<IVUnknown*> *pReceiver) {
	    BaseClass::QI (pTypeInfo, pReceiver);
	}

    //  ITrigger Methods
    private:
	void VINTERFACE_MEMBERIMPL(Process) () {
	    (m_pActor.referent()->*m_pAction) (this);
	}

    //  State
    protected:
	CohortReference const	m_pCohort;
	ActorReference		m_pActor;
	Action			m_pAction;
    };
}


#endif

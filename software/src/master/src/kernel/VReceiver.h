#ifndef VReceiver_Interface
#define VReceiver_Interface

/************************
 *****  Components  *****
 ************************/

#include "IVReceiver.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

template <class Actor, class Datum> class VReceiver : public IVReceiver<Datum> {
//  Family
public:
    typedef VReceiver<Actor,Datum> this_t;
    DECLARE_CONCRETE_RTTLITE (this_t, IVReceiver<Datum>);

//  Aliases
public:
    typedef Vca::VCohort VCohort;
    typedef VCohort::Reference CohortReference;

//  Member Types
public:
    typedef typename Actor::Reference ActorReference;
    typedef void (Actor::*Action) (ThisClass *pThis, Datum iDatum);
    typedef void (Actor::*EndAction) (ThisClass *pThis);
    typedef void (Actor::*ErrorAction) (
	ThisClass *pThis, Vca::IError *pError, VString const &rText
    );

//  Construction
public:
    VReceiver (Actor *pActor, Action pAction)
	: m_pCohort (VCohort::Here ()), m_pActor (pActor), m_pAction (pAction)
	, m_bEndAction (false), m_bErrorAction (false)
    {
    }
    VReceiver (Actor *pActor, Action pAction, EndAction pEndAction)
	: m_pCohort (VCohort::Here ()), m_pActor (pActor), m_pAction (pAction), m_bErrorAction (false)
	, m_bEndAction (true)
	, m_pEndAction (pEndAction)
    {
    }
    VReceiver (Actor *pActor, Action pAction, ErrorAction pErrorAction)
	: m_pCohort (VCohort::Here ()), m_pActor (pActor), m_pAction (pAction), m_bEndAction (false)
	, m_bErrorAction (true)
	, m_pErrorAction (pErrorAction)
    {
    }
    VReceiver (Actor *pActor, Action pAction, EndAction pEndAction, ErrorAction pErrorAction)
	: m_pCohort (VCohort::Here ()), m_pActor (pActor), m_pAction (pAction)
	, m_bEndAction (true)
	, m_pEndAction (pEndAction)
	, m_bErrorAction (true)
	, m_pErrorAction (pErrorAction)
    {
    }

//  Destruction
protected:
    ~VReceiver () {
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

//  Message Handling
private:
    virtual bool defersTo (Vca::VMessageScheduler &rScheduler) OVERRIDE {
	return rScheduler.mustSchedule (m_pCohort);
    }

//  IVReceiver Methods
private:
    virtual void VINTERFACE_MEMBERIMPL(OnData) (Vca::VMessage *pMessage, Datum iDatum) OVERRIDE {
	(m_pActor.referent()->*m_pAction) (this, iDatum);
    }

//  Vca::IClient Methods
private:
    virtual void VINTERFACE_MEMBERIMPL(OnEnd) (Vca::VMessage *pMessage) OVERRIDE {
	if (m_bEndAction)
	    (m_pActor.referent()->*m_pEndAction) (this);
	else if (m_bErrorAction) {
	    VString iEOD ("EOD", false);
	    VINTERFACE_MEMBERIMPL(OnError) (pMessage, 0, iEOD);
	}
    }
    virtual void VINTERFACE_MEMBERIMPL(OnError) (Vca::VMessage *pMessage, Vca::IError *pError, VString const &rText) OVERRIDE {
	if (m_bErrorAction)
	    (m_pActor.referent()->*m_pErrorAction) (this, pError, rText);
    }

//  IVUnknown Methods
private:
    virtual void VINTERFACE_MEMBERIMPL(QueryInterface) (Vca::VMessage *pMessage, VTypeInfo *pTypeInfo, IVReceiver<IVUnknown*> *pReceiver) OVERRIDE {
	BaseClass::QI (pTypeInfo, pReceiver);
    }

//  State
protected:
    CohortReference const m_pCohort;
    ActorReference	m_pActor;
    Action		m_pAction;
    EndAction		m_pEndAction;
    ErrorAction		m_pErrorAction;
    bool		m_bEndAction;
    bool		m_bErrorAction;
};


#endif

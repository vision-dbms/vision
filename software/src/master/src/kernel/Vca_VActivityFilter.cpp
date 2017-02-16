/*****  Vca_VActivityFilter Implementation  *****/

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

#include "Vca_VActivityFilter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivityMonitor.h"


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VActivityFilter::Harvester::Item  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityFilter::Harvester::Item::Item () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityFilter::Harvester::Item::~Item () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VActivityFilter::Harvester::Item::getDescription_(VString& rResult) const {
}

void Vca::VActivityFilter::Harvester::Item::getDescription (VString& rResult) const {
    getDescription_(rResult);
}

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vca::VActivityFilter::Harvester::Item::detach () {
    if (m_pHarvester) {
	m_pHarvester->detach (this);
    }
}

void Vca::VActivityFilter::Harvester::Item::attachHere (
    Harvester* pHarvester, Reference& rpListHead, Reference& rpPredecessor
) {
    if (m_pHarvester.setIfNil (pHarvester)) {
	m_pPredecessor = rpPredecessor;
	if (m_pPredecessor) {
	    m_pSuccessor = m_pPredecessor->m_pSuccessor;
	    m_pPredecessor->m_pSuccessor = this;
	} else {
	    m_pSuccessor.clear ();
	    rpListHead = this;
	}
	rpPredecessor = this;
    }
}

void Vca::VActivityFilter::Harvester::Item::detachFrom (
    Harvester *pHarvester, Reference &rpListHead, Reference &rpListTail
) {
    if (m_pHarvester.clearIf (pHarvester)) {
    //  Set my predecessor's successor to my successor, leaving my successor unchanged...
	if (m_pPredecessor)
	    m_pPredecessor->m_pSuccessor = m_pSuccessor;
	else {
	    rpListHead = m_pSuccessor;
	}

    //  Set my successor's predecessor to my predecessor, clearing my predecessor...
	if (m_pSuccessor)
	    m_pSuccessor->m_pPredecessor.claim (m_pPredecessor);
	else {
	    rpListTail.claim (m_pPredecessor);
	}

    //  Finally, clear my successor...
	m_pSuccessor.clear ();
    }
}

bool Vca::VActivityFilter::Harvester::Item::added (ThisClass* pNewItem, Reference &rpNextItem) {
    return !ripenedBy (pNewItem, rpNextItem) && ripensAfter (pNewItem) && add (pNewItem);
}

bool Vca::VActivityFilter::Harvester::Item::add (ThisClass* pNewItem) {
    m_pHarvester->insert (pNewItem, m_pPredecessor);
    return true;
}


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vca::VActivityFilter::Harvester  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityFilter::Harvester::Harvester () : m_pITrigger (this) {
}

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void Vca::VActivityFilter::Harvester::attach (Item* pNewItem, VTimer* pHarvestTimer) {
    pNewItem->detach ();

//  Try to place the new item in the existing list, ...
    Item::Reference pNextItem (m_pItemListHead);
    while (pNextItem && !pNextItem->added (pNewItem, pNextItem)); // ... when 'added' returns true, new-item was inserted before old-item

//  If we run out of old items, the new item belongs at the end of the list, ...
    if (pNextItem.isNil ()) {
	pNewItem->attachHere (this, m_pItemListHead, m_pItemListTail);
    }

//  Starting with the new item, drain the list of any ripened items that remain...
    pNextItem.setTo (pNewItem);
    while (pNextItem && pNextItem->ripenedBy (pNewItem, pNextItem));

//  Finally see if we need to start or stop the harvest timer, ...
    if (isntEmpty () && m_pHarvestTimer.setIfNil (pHarvestTimer))
	m_pHarvestTimer->start ();
    else {
	scheduleNextHarvest ();
    }
}

void Vca::VActivityFilter::Harvester::detach (Item *pItem) {
    pItem->detachFrom (this, m_pItemListHead, m_pItemListTail);
    scheduleNextHarvest ();
}

void Vca::VActivityFilter::Harvester::insert (Item* pNewItem, Item::Reference& rpPredecessor) {
    pNewItem->attachHere (this, m_pItemListHead, rpPredecessor);
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityFilter::Harvester::Process (ITrigger* pRole) {
    VTime iNow;

    Item::Reference pNextItem (m_pItemListHead);
    while (pNextItem && pNextItem->ripenedBy (iNow, pNextItem));

    scheduleNextHarvest ();
}

/********************************
 ********************************
 *****  Harvest Management  *****
 ********************************
 ********************************/

void Vca::VActivityFilter::Harvester::scheduleNextHarvest () {
    if (m_pHarvestTimer && isEmpty ()) {
	m_pHarvestTimer->stop ();
	m_pHarvestTimer.clear ();
    }
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VActivityFilter::ActivityMonitor  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

namespace Vca {
    class VActivityFilter::ActivityMonitor : public VActivityMonitor, private virtual Harvester::Item {
	DECLARE_CONCRETE_RTTLITE (ActivityMonitor, VActivityMonitor);

    //  Construction
    public:
	ActivityMonitor (
	    VActivity *pActivity, consumer_gofer_t* pConsumerGofer, Harvester* pHarvester, VTimer* pHarvestTimer
	);

    //  Destruction
    private:
	~ActivityMonitor ();

    //  Access
    protected:
	void getDescription_(VString &rResult) const;

    //  LifespanMonitor Callbacks
    private:
	void OnEndEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp);
	void OnErrorEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage);
	void OnFailureEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage);

	void onCompletion_();

    //  Forwarding
    private:
	void forward ();

	void onActivitySinkGoferValue (IActivitySink* pActivitySink);
	void onActivitySinkGoferError (IError* pInterface, VString const& rMessage);

    //  Ripeness
    private:
	bool ripen ();

	bool ripenedBy (Harvester::Item const* pItem, Harvester::Item::Reference &rpNextItem);
	bool ripenedBy (VTime const& rTime, Harvester::Item::Reference &rpNextItem);

	bool ripensBefore (Item const* pItem) const;
	bool ripensAfter  (Item const* pItem) const;

	bool ripensBefore (VTime const& rTime) const;
	bool ripensAfter  (VTime const& rTime) const;

	bool startedBefore (VTime const& rTime) const;
	bool startedAfter  (VTime const& rTime) const;

    //  State
    private:
	consumer_gofer_t::Reference 	const	m_pConsumerGofer;
	bool					m_bUnforwarded;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityFilter::ActivityMonitor::ActivityMonitor (
    VActivity *pActivity, consumer_gofer_t* pConsumerGofer, Harvester* pHarvester, VTimer* pHarvestTimer
) : BaseClass (pActivity), m_pConsumerGofer (pConsumerGofer), m_bUnforwarded (true) {
    retain (); {
	pHarvester->attach (this, pHarvestTimer);
	requestEndErrEvents ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityFilter::ActivityMonitor::~ActivityMonitor () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VActivityFilter::ActivityMonitor::getDescription_(VString& rResult) const {
    BaseClass::getDescription (rResult);
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityFilter::ActivityMonitor::OnEndEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp) {
    detach ();
}

void Vca::VActivityFilter::ActivityMonitor::OnErrorEvent_(
    IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage
) {
    forward ();
}

void Vca::VActivityFilter::ActivityMonitor::OnFailureEvent_(
    IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage
) {
    forward ();
}

void Vca::VActivityFilter::ActivityMonitor::forward () {
    if (m_bUnforwarded && m_pConsumerGofer) {
	m_bUnforwarded = false;
	m_pConsumerGofer->supplyMembers (this, &ThisClass::onActivitySinkGoferValue, &ThisClass::onActivitySinkGoferError);
    }
}

void Vca::VActivityFilter::ActivityMonitor::onActivitySinkGoferValue (IActivitySink* pActivitySink) {
    activity ()->sendTo (this, pActivitySink);
}

void Vca::VActivityFilter::ActivityMonitor::onActivitySinkGoferError (IError* pInterface, VString const& rMessage) {
}

void Vca::VActivityFilter::ActivityMonitor::onCompletion_() {
}

/**********************
 **********************
 *****  Ripening  *****
 **********************
 **********************/

bool Vca::VActivityFilter::ActivityMonitor::ripen () {
    detach ();
    forward ();
    return true;
}

bool Vca::VActivityFilter::ActivityMonitor::ripenedBy (Harvester::Item const* pItem, Harvester::Item::Reference &rpNextItem) {
    rpNextItem.setTo (successor ());

    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && pItem->startedAfter (iEndTimeEstimate) && ripen ();
}
bool Vca::VActivityFilter::ActivityMonitor::ripenedBy (VTime const& rTime, Harvester::Item::Reference &rpNextItem) {
    rpNextItem.setTo (successor ());

    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && rTime > iEndTimeEstimate && ripen ();
}

bool Vca::VActivityFilter::ActivityMonitor::ripensBefore (Item const* pItem) const {
    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && pItem->ripensAfter (iEndTimeEstimate);
}
bool Vca::VActivityFilter::ActivityMonitor::ripensAfter  (Item const* pItem) const {
    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && pItem->ripensBefore (iEndTimeEstimate);
}

bool Vca::VActivityFilter::ActivityMonitor::ripensBefore (VTime const& rTime) const {
    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && iEndTimeEstimate < rTime;
}
bool Vca::VActivityFilter::ActivityMonitor::ripensAfter  (VTime const& rTime) const {
    VTime iEndTimeEstimate (0);
    return getLocalEndTimeEstimate (iEndTimeEstimate) && iEndTimeEstimate > rTime;
}

bool Vca::VActivityFilter::ActivityMonitor::startedBefore (VTime const& rTime) const {
    VTime iStartTime (0);
    return getLocalStartTime (iStartTime) && iStartTime < rTime;
}
bool Vca::VActivityFilter::ActivityMonitor::startedAfter (VTime const& rTime) const {
    VTime iStartTime (0);
    return getLocalStartTime (iStartTime) && iStartTime > rTime;
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::VActivityFilter  *****
 *****                        *****
 **********************************
 **********************************/

Vca::VTimer::microseconds_t Vca::VActivityFilter::g_sHarvestingInterval = 1000000 * static_cast<Vca::VTimer::microseconds_t>(
    V::GetEnvironmentValue ("VcaActivityHarvestingInterval", 10.0)
); // == 10 seconds

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTimer* Vca::VActivityFilter::HarvestTimer () {
    ITrigger::Reference pTrigger;
    m_pHarvester->getRole (pTrigger);
    return new VTimer ("Activity Filter Harvest Timer", pTrigger, g_sHarvestingInterval, true);
}

Vca::VActivityFilter::VActivityFilter (
    consumer_gofer_t* pConsumerGofer
) : m_pConsumerGofer (pConsumerGofer), m_pHarvester (new Harvester ()), m_pHarvestTimer (HarvestTimer ()) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityFilter::~VActivityFilter () {
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivityFilter::createMonitorFor (VActivity* pActivity) const {
    ActivityMonitor::Reference const pMonitor (
	new ActivityMonitor (pActivity, m_pConsumerGofer, m_pHarvester, m_pHarvestTimer)
    );
}

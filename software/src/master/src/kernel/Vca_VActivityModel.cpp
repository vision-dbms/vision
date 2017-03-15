/*****  Vca_VActivityModel Implementation  *****/

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

#include "Vca_VActivityModel.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivityMonitor.h"
#include "Vca_VActivitySink.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VActivityModel  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityModel::VActivityModel ()
    : m_pIActivitySource (this)
    , m_cActivitiesCreated (0)
    , m_cActivitiesDeleted (0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityModel::~VActivityModel () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VActivityModel::createActivity_(
    ActivityRecord::Reference &rpActivity,
    IActivity                 *pInterface,
    IActivity                 *pProxy,
    VTime const               &rStartTime,
    VTime const               &rExpectedEndTime,
    VString const             &rDescription
) {
    rpActivity.setTo (
	new ActivityRecord (this, pInterface, pProxy, rStartTime, rExpectedEndTime, rDescription)
    );
}

void Vca::VActivityModel::createActivity (
    ActivityRecord::Reference &rpActivity,
    VActivitySink             *pController,
    IActivity                 *pInterface,
    IActivity                 *pProxy,
    VTime const               &rStartTime,
    VTime const               &rExpectedEndTime,
    VString const             &rDescription
) {
    m_cActivitiesCreated++;
    createActivity_(rpActivity, pInterface, pProxy, rStartTime, rExpectedEndTime, rDescription);
    pController->createMonitorFor (rpActivity);
}

void Vca::VActivityModel::attach (ActivityRecord *pRecord) {
    m_iRecordSet.Insert (pRecord);

    if (m_iActivitySet.Insert (pRecord))
	pRecord->sendTo (m_pubIActivitySink);
}

void Vca::VActivityModel::detach (ActivityRecord *pRecord) {
    m_iActivitySet.Delete (pRecord);
    m_iRecordSet.Delete (pRecord);
}

/**************************
 **************************
 *****  Subscription  *****
 **************************
 **************************/

void Vca::VActivityModel::SupplyActivities (
    IActivitySource *pRole, ITicketSink *pTicketSink, IActivitySink *pActivitySink
) {
    if (pActivitySink) {
	m_iActivitySet.SendTo (pActivitySink);
	m_pubIActivitySink.addSubscriber (pTicketSink, pActivitySink);
    } else if (pTicketSink) {
	pTicketSink->OnError (0, "Activity observer not provided");
    }
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VActivityModel::Record  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityModel::Record::Record (
    VActivityModel* pModel, VTime const& rStartTime, VTime const& rExpectedEndTime, VString const& rDescription
) : m_pModel       (pModel)
  , m_iDescription (rDescription)
  , m_iRemoteTimes (rStartTime, rExpectedEndTime)
  , m_iLocalTimes  (VTime(), m_iRemoteTimes)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityModel::Record::~Record () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VActivityModel::Record::setEndTimeTo (VTime const& rEndTime) {
    m_iRemoteTimes.setEndTimeTo (rEndTime);
    m_iLocalTimes.setEndTimeTo (rEndTime);
}


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VActivityModel::Record::Timestamps  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityModel::Record::Timestamps::Timestamps (
    VTime const& rStartTime, VTime const& rEndTimeEstimate
) : m_sAdjustment	(0)
  , m_iStartTime	(rStartTime)
  , m_iEndTime		(rEndTimeEstimate)
  , m_iEndTimeEstimate	(rEndTimeEstimate)
{
}

Vca::VActivityModel::Record::Timestamps::Timestamps (
    VTime const& rStartTime, Timestamps const& rOther
) : m_sAdjustment	(rStartTime - rOther.startTime ())
  , m_iStartTime	(rStartTime)
  , m_iEndTime		(rOther.endTime ())
  , m_iEndTimeEstimate	(rOther.endTimeEstimate ())
{
    if (m_sAdjustment) {
	m_iEndTime += m_sAdjustment;
	m_iEndTimeEstimate += m_sAdjustment;
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityModel::Record::Timestamps::~Timestamps () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VActivityModel::Record::Timestamps::setEndTimeEstimateTo (VTime const& rNewValue) {
    m_iEndTimeEstimate = rNewValue;
    if (m_sAdjustment)
	m_iEndTimeEstimate += m_sAdjustment;
}

void Vca::VActivityModel::Record::Timestamps::setEndTimeTo (VTime const& rNewValue) {
    m_iEndTime = rNewValue;
    if (m_sAdjustment)
	m_iEndTime += m_sAdjustment;
}


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VActivityModel::ActivityRecord  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityModel::ActivityRecord::ActivityRecord (
    VActivityModel* pModel, IActivity* pInterface, IActivity* pProxy, VTime const& rStartTime, VTime const& rExpectedEndTime, VString const& rDescription
) : BaseClass		(pModel, pInterface, pProxy, rStartTime, rExpectedEndTime, rDescription)
  , m_bCompleted	(false)
  , m_pIActivityProxy	(this)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityModel::ActivityRecord::~ActivityRecord () {
    model ()->onActivityDeleted ();
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VActivityModel::ActivityRecord::getAuthority_(IActivity::Reference &rpAuthority) {
    rpAuthority.setTo (authority ());
}

void Vca::VActivityModel::ActivityRecord::getDescription_(VString &rResult) const {
    rResult.setTo (description ());
}

bool Vca::VActivityModel::ActivityRecord::getLocalStartTime_(VTime &rResult) const {
    rResult.setTo (localTimes().startTime ());
    return true;
}

bool Vca::VActivityModel::ActivityRecord::getLocalEndTime_(VTime &rResult) const {
    rResult.setTo (localTimes().endTime ());
    return true;
}

bool Vca::VActivityModel::ActivityRecord::getLocalEndTimeEstimate_(VTime &rResult) const {
    rResult.setTo (localTimes().endTimeEstimate ());
    return true;
}

bool Vca::VActivityModel::ActivityRecord::getRemoteStartTime_(VTime &rResult) const {
    rResult.setTo (remoteTimes().startTime ());
    return true;
}

bool Vca::VActivityModel::ActivityRecord::getRemoteEndTime_(VTime &rResult) const {
    rResult.setTo (remoteTimes().endTime ());
    return true;
}

bool Vca::VActivityModel::ActivityRecord::getRemoteEndTimeEstimate_(VTime &rResult) const {
    rResult.setTo (remoteTimes().endTimeEstimate ());
    return true;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VActivityModel::ActivityRecord::onMonitorCompletion_(VActivityMonitor *pMonitor, VTime const& rEndTime) {
    if (hasntCompleted ()) {
	m_bCompleted = true;
	setEndTimeTo (rEndTime);
    }
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityModel::ActivityRecord::SupplyEvents (
    IActivity* pRole, IVReceiver<IRequest*>* pRequestSink, IEventSink* pObserver
) {
#if 0
    if (pObserver) {
	m_iEventSet.SendTo (pObserver);
	m_pubEventSink.addSubscriber (pRequestSink, pObserver);
    }
#else
    activity()->SupplyEvents (pRequestSink, pObserver);
#endif
}

void Vca::VActivityModel::ActivityRecord::SupplyEndErr (
    IActivity* pRole, IVReceiver<IRequest*>* pRequestSink, IEventSink* pObserver
) {
#if 0
    if (pObserver) {
	m_iEndErrSet.SendTo (pObserver);
	m_pubEndErrSink.addSubscriber (pRequestSink, pObserver);
    }
#else
    activity()->SupplyEndErr (pRequestSink, pObserver);
#endif
}

void Vca::VActivityModel::ActivityRecord::GetSSID (IActivity* pRole, IVReceiver<ssid_t>* pSink) {
#if 0
    if (pSink)
	pSink->OnData (ssid ());
#else
    activity()->GetSSID (pSink);
#endif
}

void Vca::VActivityModel::ActivityRecord::GetParent (IActivity* pRole, IVReceiver<IActivity*>* pSink) {
#if 0
    if (pSink) {
	if (m_pParentGofer)
	    m_pParentGofer->supplyReceiver (pSink);
	else {
	    pSink->OnData (0);
	}
    }
#else
    activity()->GetParent (pSink);
#endif
}

void Vca::VActivityModel::ActivityRecord::GetDescription (IActivity* pRole, IVReceiver<VString const&>* pSink) {
    if (pSink)
	pSink->OnData (description ());
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VActivityModel::ActivityRecord *Vca::VActivityModel::activity (ActivityRecord::key_lookup_t xActivity) const {
    return m_iActivitySet[xActivity];
}

Vca::VActivityModel::ActivityRecord *Vca::VActivityModel::activity (ssid_t xActivity) const {
    return dynamic_cast<ActivityRecord*>(m_iRecordSet[xActivity]);
}

Vca::VActivityModel::ActivityRecord *Vca::VActivityModel::activityAtPosition (unsigned int xActivity) const {
    return m_iActivitySet.InstanceAtPosition (xActivity);
}

Vca::VActivityModel::Record *Vca::VActivityModel::record (Record::key_lookup_t xRecord) const {
    return m_iRecordSet[xRecord];
}

Vca::VActivityModel::Record *Vca::VActivityModel::recordAtPosition (unsigned int xInstance) const {
    return m_iRecordSet.InstanceAtPosition (xInstance);
}

/**************************
 **************************
 *****  Distribution  *****
 **************************
 **************************/

void Vca::VActivityModel::ActivityRecord::sendTo (IActivitySink* pActivitySink) {
    if (pActivitySink) {
	IActivityProxy::Reference pProxy;
	getRole (pProxy);

	VTime iStartTime;
	getRemoteStartTime (iStartTime);

	VTime iEndTime;
	getRemoteEndTimeEstimate (iEndTime);

	pActivitySink->OnActivity (authority (), pProxy, iStartTime, iEndTime, description ());
    }
}


/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivityModel::ActivityRecord::onMonitorCreation_(VActivityMonitor *pMonitor) {
    if (0 == m_cMonitors++)
	model ()->attach (this);
}

void Vca::VActivityModel::ActivityRecord::onMonitorDeletion_(VActivityMonitor *pMonitor) {
    if (m_cMonitors.decrementIsZero ())
	model ()->detach (this);
}

void Vca::VActivityModel::ActivityRecord::sendTo_(VActivityMonitor *pMonitor, IActivitySink *pActivitySink) {
    sendTo (pActivitySink);
}

void Vca::VActivityModel::ActivityRecord::supplyEvents_(VActivityMonitor *pMonitor, ITicketSink* pTicketSink) {
    if (!activity())
	pMonitor->OnEnd_();
    else {
	IEventSink::Reference pEventSink;
	pMonitor->getRole (pEventSink);
	activity()->SupplyEvents (pTicketSink, pEventSink);
    }
}

void Vca::VActivityModel::ActivityRecord::supplyEndErrEvents_(VActivityMonitor *pMonitor, ITicketSink* pTicketSink) {
    if (!activity())
	pMonitor->OnEnd_();
    else {
	IEventSink::Reference pEventSink;
	pMonitor->getRole (pEventSink);
	activity()->SupplyEndErr (pTicketSink, pEventSink);
    }
}

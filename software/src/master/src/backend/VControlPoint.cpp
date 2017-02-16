/*****  VControlPoint Implementation  *****/

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

#include "VControlPoint.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VControlPointSubscription.h"


/***************************************
 ***************************************
 *****                             *****
 *****  VControlPoint::Controller  *****
 *****                             *****
 ***************************************
 ***************************************/

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

char const* VControlPoint::Controller::description () const {
    return "Self";
}

char const* VControlPoint::Controller::descriptionOf (VControlPoint const*) const {
    return "Control Point";
}


/***************************
 ***************************
 *****                 *****
 *****  VControlPoint  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VControlPoint::VControlPoint (VControlPoint::Controller* pController)
: m_pController (pController), m_pSubscriptionListHead (0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VControlPoint::~VControlPoint () {
    shutdown ();
}


/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

char const* VControlPoint::controllerDescription () const {
    return m_pController ? m_pController->description () : "-";
}

char const* VControlPoint::description () const {
    return m_pController ? m_pController->descriptionOf (this) : "-";
}

/**************************
 **************************
 *****  Subscription  *****
 **************************
 **************************/

VControlPointSubscription* VControlPoint::subscription () {
    return canBeTriggered () ? new VControlPointSubscription (this) : 0;
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VControlPoint::trigger_(VComputationUnit* pSource) {
    Controller::FiringType xFiringType = m_pController->firingType (this);
    VControlPointSubscription* pSubscription = m_pSubscriptionListHead;
    while (pSubscription) {
	pSubscription = pSubscription->deliver (pSource, xFiringType);
    }
}

/**********************
 **********************
 *****  Shutdown  *****
 **********************
 **********************/

void VControlPoint::shutdown () {
    while (m_pSubscriptionListHead)
	m_pSubscriptionListHead->cancel ();
}

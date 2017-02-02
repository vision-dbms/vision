#ifndef VControlPoint_Interface
#define VControlPoint_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

class VComputationUnit;
class VControlPointSubscription;

/*************************
 *****  Definitions  *****
 *************************/

class VControlPoint : public VTransient {
    DECLARE_FAMILY_MEMBERS (VControlPoint, VTransient);

//  Controller Class
public:
    class /*ABSTRACT*/ Controller : public VTransient {
	DECLARE_FAMILY_MEMBERS (Controller, VTransient);

    //  Friends
	friend class VControlPoint;

    //  Triggering Reason
    public:
	enum FiringType {
	    FiringType_Normal,
	    FiringType_Partial,
	    FiringType_Final
	};

    //  Construction
    protected:
	Controller () {
	}

    //  Destruction
    protected:
	~Controller () {
	}

    //  Access
    public:
	virtual char const* description () const;
	virtual char const* descriptionOf (VControlPoint const* pControlPoint) const;

    //  Query
    public:
	virtual bool canTrigger (VControlPoint const* pControlPoint) const = 0;
	virtual FiringType firingType (VControlPoint const* pControlPoint) const = 0;
    };

//  Friends
    friend class VControlPointSubscription;

//  Construction
public:
    VControlPoint (Controller* pController = 0);

//  Destruction
public:
    ~VControlPoint ();

//  Query
public:
    bool canBeTriggered () const {
	return m_pController ? m_pController->canTrigger (this) : false;
    }

//  Description
public:
    char const* controllerDescription () const;
    char const* description () const;

//  Control
public:
    void setControllerTo (Controller* pController) {
	if (IsNil (m_pController))
	    m_pController = pController;
    }

//  Subscription
public:
    VControlPointSubscription* subscription ();

//  Delivery
public:
    void trigger_(VComputationUnit* pSource);
    void trigger (VComputationUnit* pSource) {
	if (m_pSubscriptionListHead)
	    trigger_(pSource);
    }

//  Shutdown
public:
    void shutdown ();

//  State
protected:
    Controller*			m_pController;
    VControlPointSubscription*	m_pSubscriptionListHead;
};


typedef VControlPoint::Controller		VControlPointController;
typedef VControlPoint::Controller::FiringType	VControlPointFiringType;


#endif

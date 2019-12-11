#ifndef VControlPointSubscription_Interface
#define VControlPointSubscription_Interface

/************************
 *****  Components  *****
 ************************/

#include "VInternalGroundStore.h"

#include "VComputationTrigger.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VControlPoint.h"

class VComputationUnit;
class VSNFTask;
class VSuspension;

/*************************
 *****  Definitions  *****
 *************************/

class VControlPointSubscription : public VInternalGroundStore {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VControlPointSubscription, VInternalGroundStore);

//  Friends
    friend class VControlPoint;

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VControlPointSubscription (VControlPoint* pControlPoint);

//  Destruction
protected:
    ~VControlPointSubscription ();

//  Query
public:
    bool hasExpired () const {
	return m_pControlPoint ? !m_pControlPoint->canBeTriggered () : true;
    }
    bool hasSuspensions () const {
	return m_pSuspensions.isntNil ();
    }

    bool isSingleUse () const {
	return m_fSingleUse;
    }

//  Access
public:
    unsigned int cardinality () const {
	return 1;
    }
    virtual unsigned int cardinality_() const OVERRIDE {
	return cardinality ();
    }

    char const* controllerDescription () const {
	return m_pControllerDescription;
    }
    char const* controlPointDescription () const {
	return m_pControlPointDescription;
    }

    VSuspension* nextSuspension ();

    rtPTOKEN_Handle *ptoken () const {
	return M_AttachedNetwork->TheScalarPTokenHandle ();
    }
    virtual rtPTOKEN_Handle *ptoken_() const OVERRIDE {
	return ptoken ();
    }

//  Control
public:
    void cancel ();

    bool setSingleUseTo (bool iNewValue);
    bool setSingleUseOff ();
    bool setSingleUseOn  ();

//  Delivery
protected:
    VControlPointSubscription* deliver (VComputationUnit* pSource, VControlPointFiringType xFiringType);

//  Wait
public:
    bool waitForSuspensions (VComputationUnit* pSuspendee);
    bool waitForSuspensions (VSNFTask* pSuspendee);

//  State
protected:
    VControlPoint*		m_pControlPoint;
    char const* const		m_pControllerDescription;
    char const* const		m_pControlPointDescription;
    VControlPointSubscription*	m_pPredecessor;
    VControlPointSubscription*	m_pSuccessor;
    VReference<VSuspension>	m_pSuspensions;
    VComputationTrigger		m_iTrigger;
    bool			m_fSingleUse;
};


#endif

/*****  Vca_VActivityRelay Implementation  *****/

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

#include "Vca_VActivityRelay.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VActivityMonitor.h"


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::VActivityRelay::ActivityMonitor  *****
 *****                                        *****
 **************************************************
 **************************************************/

namespace Vca {
    class VActivityRelay::ActivityMonitor : public VActivityMonitor {
	DECLARE_CONCRETE_RTTLITE (ActivityMonitor, VActivityMonitor);

    //  Construction
    public:
	ActivityMonitor (VActivity *pActivity);

    //  Destruction
    private:
	~ActivityMonitor ();
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityRelay::ActivityMonitor::ActivityMonitor (VActivity *pActivity) : BaseClass (pActivity) {
    retain (); {
	requestEndErrEvents ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityRelay::ActivityMonitor::~ActivityMonitor () {
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VActivityRelay  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityRelay::VActivityRelay (VActivityModel *pModel) : BaseClass (pModel) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityRelay::~VActivityRelay () {
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivityRelay::createMonitorFor (VActivity* pActivity) const {
    ActivityMonitor::Reference const pMonitor (new ActivityMonitor (pActivity));
}

#ifndef Vca_VMessageScheduler_Interface
#define Vca_VMessageScheduler_Interface 

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VCohort.h"

#include "Vca_VMessageStarter.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VMessage;

    class Vca_API VMessageScheduler {
    //  Construction
    public:
	VMessageScheduler ();

    //  Destruction
    public:
	~VMessageScheduler ();

    //  Initialization
    public:
	/**
	 *  Returns true if operations on the cohort's objects must
	 *  be queued (i.e., deferred).  Returns false if the scheduler has
	 *  successfully claimed the cohort for this thread so that operations
	 *  can be performed inline immediately.
	 *
	 *  @param pCohort The cohort that must be claimed to avoid scheduling.
	 *  @param pStarter The object that will ultimately take responsibility for running the message if scheduling is required.
	 *
	 *  @return true if scheduling (deferred execution) is required, false otherwise.
	 */
	bool mustSchedule (VCohort *pCohort, VMessageStarter *pStarter);
        /**
         * @copydoc mustSchedule(VCohort,VMessageStarter)
         */
	bool mustSchedule (VCohort *pCohort) {
	    return mustSchedule (pCohort, pCohort);
	}

    //  Access
    public:
	operator VMessageStarter *() const {
	    return m_pStarter;
	}

    //  Scheduling
    public:
	void schedule (VMessage *pMessage);

    //  State
    private:
	VCohort::Claim			m_pCohort;
	VMessageStarter::Reference	m_pStarter;
    };
}


#endif

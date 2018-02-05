#ifndef Vca_VcaThreadState_Interface
#define Vca_VcaThreadState_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "V_VThreadSpecific.h"

#include "V_VSemaphore.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VCohort.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Thread state/metadata object for Vca.
     * Every thread in which Vca objects live will have exactly one instance of this class. Each VcaThreadState keeps track of the stack of VCohort::Claim instances created in its thread. The 'current' claim is the claim that was most recently instantiated, or at the top of the claim stack.
     *
     * @attention This is a class that helps Vca do its bookkeeping. It isn't usually seen or manipulated by general consumer code.
     */
    class Vca_API VcaThreadState : public V::VThreadSpecific {
	DECLARE_CONCRETE_RTTLITE (VcaThreadState, V::VThreadSpecific);

	friend class VCohort::Claim;

    //  Aliases
    public:
	typedef VCohort::Claim		CohortClaim;
	typedef CohortClaim::Pointer	CohortClaimPointer;
	typedef V::VSemaphore		Semaphore;

    //  Key
    private:
        /**
         * Thread Local Storage (TLS) Key that references this thread's instance of this class.
         * The value referenced by this key is initialized by Here() as needed.
         */
	static Key const g_iTLSKey;

    //  Construction
    private:
        /**
         * Constructs a VcaThreadState for an existing Thread.
         * Never used by consumers, since we have a factory.
         * @see Here()
         */
	VcaThreadState (Thread *pThread);

    //  Destruction
    private:
	~VcaThreadState ();

    //  Cohort Access
    public:
        /**
         * Returns the current VCohort for this thread.
         * Equivelant to Here() -> cohort().
         */
	static VCohort *Cohort ();

        /**
         * Retrieves a value stored in the current Cohort, indexed by a given VCohortIndex.
         *
         * @param rIndex the index of the value to be retrieved.
         * @return the fetched value.
         */
	static void *CohortValue (VCohortIndex const &rIndex);

        /**
         * Returns the current VCohort referenced by this VcaThreadState.
         */
	VCohort *cohort () const {
	    return m_pCohortClaims->cohort ();
	}

    //  Cohort Claims
    protected:
        /**
         * Returns the stack of cohort claims in this thread.
         *
         * @return the top of the cohort claim stack of this thread.
         */
	CohortClaim *cohortClaims () const {
	    return m_pCohortClaims;
	}
    private:
	void attach (CohortClaim *pClaim, bool bWait = true);
	void detach (CohortClaim *pClaim);

	bool acquireCohorts ();
	void releaseCohorts ();

    //  Synchronization
    public:
	void signal ();
	void waitForSignal ();

    //  Thread Association
    public:
        /**
         * Factory for VcaThreadState instances.
         * Retrieves, creating if necessary, the instance of this class associated with the current thread.
         */
	static Reference Here ();
    private:
	void onThreadDetach () OVERRIDE;

    //  State
    private:
	//  'm_pCohortClaims' must precede 'm_pDefaultCohortClaim' so that it
	//  is constructed first.
	CohortClaimPointer	m_pCohortClaims;
	CohortClaim		m_pDefaultCohortClaim;
	Semaphore		m_iControlSemaphore;
    };
}


#endif

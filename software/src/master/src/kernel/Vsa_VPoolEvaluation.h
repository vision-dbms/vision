#ifndef Vsa_VPoolEvaluation_Interface
#define Vsa_VPoolEvaluation_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V_VTime.h"
#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluation.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IPoolEvaluation_Ex1.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VEvaluatorPool;

    /*****************************
     *----  VPoolEvaluation  ----*
     *****************************/

    /**
     * A specialized VEvaluation class having Pool related attributes that is to be used when evaluating the VEvaluation.
     */
    class Vsa_API VPoolEvaluation : public VEvaluation {
    //  Run Time Type
        DECLARE_CONCRETE_RTTLITE (VPoolEvaluation, VEvaluation);

    //  Construction
    public:
        VPoolEvaluation (       //  ... Expression
            VEvaluatorPool*     pEvaluator,
            IEvaluatorClient*   pClient,
            VString const&      rPath,
            VString const&      rQuery,
            Vca::U32            iValidGeneration = 0,
            Vca::U32            iWorkerId = 0
        );
        VPoolEvaluation (       //  ... URL
            VEvaluatorPool*     pEvaluator,
            IEvaluatorClient*   pClient,
            VString const&      rPath,
            VString const&      rQuery,
            VString const&      rEnvironment,
            Vca::U32            iValidGeneration = 0,
            Vca::U32            iWorkerId = 0
        );

    //  Destruction
    protected:
        ~VPoolEvaluation () {
            traceInfo ("Destroying VPoolEvaluation");
        }

    //  IEvaluation Role
    public:
	void getRole (IEvaluation::Reference &rpRole) OVERRIDE;

    //  IPoolEvaluation Role
        void UseAnyGenerationWorker (IPoolEvaluation *pRole);

    //  IPoolEvaluation_Ex1 Role
    private:
        Vca::VRole<ThisClass,IPoolEvaluation_Ex1> m_pIPoolEvaluation;
    public:
	void getRole (IPoolEvaluation_Ex1::Reference &rpRole);

        void SetRetryable (IPoolEvaluation_Ex1 *pRole, bool bRetryable);

    //  Query
    public:
        /**
         * Returns the time when this query arrived at the pool.
         */
	V::VTime    arrivalTime () const {
	    return m_iStartTime;
	}

        /**
         * Returns the minimum generation id to which worker that will evaluate this VEvaluation belongs.
         */
        Vca::U32    validGeneration () const {
            return m_iValidGeneration;
        }

        /**
         * Returns the ID of the worker that will be used to evaluate this VEvaluation.
         */
        Vca::U32    workerId () const {
            return m_iWorker;
        }

        /**
         * Returns true if any data (generation) can be used.
         */
        bool anyData () const {
            return m_bAnyData;
        }

        /**
         * Returns true if retries are enabled.
         */
        virtual bool retryable() const {
            return m_bRetryable && !isCancelled ();
        }

        /**
         * Returns the pool to which this evaluation belongs.
         */
        Vsa::VEvaluatorPool* evaluatorPool () const;

    //  Update
    public:
        /**
         * Sets the ID of the worker that will be used to evaluate this VEvaluation.
         *
         * @param iWorker the new worker id.
         */
        void setWorker (Vca::U32 iWorker) {
             m_iWorker = iWorker;
        }

        /**
         * Sets the minimum generation id to which worker that will evaluate this VEvaluation belongs.
         *
         * @param iGeneration the new minimum generation id.
         */
        void setValidGeneration (Vca::U32 iGeneration) {
            m_iValidGeneration = iGeneration;
        }

	void setRetryable (bool bRetryable) OVERRIDE {
	    m_bRetryable = bRetryable;
	}

    //  State
    protected:
        /**
         * The minimum generation id to which worker that will evaluate this VEvaluation belongs.
         */
        Vca::U32    m_iValidGeneration;

        /**
         * Whether or not we're allowing any data (generation) to be used.
         */
        bool        m_bAnyData;

        /**
         * Whether or not we're allowing retries.
         */
        bool        m_bRetryable;

        /**
         * The ID of the worker that will be used to evaluate this VEvaluation. Defaults to 0.
         */
        Vca::U32    m_iWorker;
    };
}


#endif

#ifndef Vca_VQueryCache_Interface
#define Vca_VQueryCache_Interface

#include "Vsa_VEvaluatorClientWaiter.h"
#include "Vsa_IEvaluator.h"
#include "V_VTime.h"

class VQueryCache : public Vsa::VEvaluatorClientWaiter {
    DECLARE_CONCRETE_RTTLITE (VQueryCache, Vsa::VEvaluatorClientWaiter);

// Construction
public:
    /**
     * Constructor expects query string.
     *
     * @param[in] rQuery the query for which we're caching the result.
     */
    VQueryCache (V::VString const &rQuery) : m_query (rQuery) {}

// Destruction
protected:
    /** Empty default destructor. */
    ~VQueryCache () {}

// VEvaluatorClient Methods
private:
    virtual void OnResult_(Vsa::IEvaluationResult *pResult, V::VString const &rOutput);

// Evaluation
public:
    /**
     * Evaluates a query with a given timeout.
     *
     * @param pEvaluator the evaluator to use.
     * @param limit the timeout limit, or null if no timeout should be used.
     * @return true if the evaluation finished, false if the timeout was reached before the evaluation finished.
     */
    bool evaluateWith (Vsa::IEvaluator *pEvaluator, V::VTime const *limit = 0);

// Query
public:
    /**
     * Retrieves the cached result.
     *
     * @param rResult the reference by which the result should be retrieved.
     */
    void getResult (V::VString &rResult) { rResult = m_result; }

// State
protected:
    /** The cached result. */
    V::VString m_result;
    /** The cached query. */
    V::VString const m_query;
};

#endif
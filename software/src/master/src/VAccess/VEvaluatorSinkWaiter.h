#ifndef VEvaluatorSinkWaiter_Interface
#define VEvaluatorSinkWaiter_Interface

#include "Vca_VRolePlayer.h"
#include "V_VWaiter.h"
#include "Vsa_IEvaluator.h"
#include "Vca_IDataSource.h"
#include "V_VTime.h"

/** IEvaluatorSink implementation with wait functionality. */
class VEvaluatorSinkWaiter : public Vca::VRolePlayer, public V::VWaiter {
    DECLARE_CONCRETE_RTTLITE (VEvaluatorSinkWaiter, Vca::VRolePlayer);

// Construction
public:
    /** Empty default constructor. */
    VEvaluatorSinkWaiter () {}

// Destruction
protected:
    /** Empty default destructor. */
    ~VEvaluatorSinkWaiter () {}

public:
    /**
     * Implements IEvaluatorSink::OnData().
     */
    void OnData (Vsa::IEvaluator *pEvaluator);

    /** Implements IClient::OnError() by signalling the wait lock. */
    void OnError (Vca::IError *pError, const VString &rMessage);

    /**
     * Retrieves cached value.
     *
     * @param pEvaluator the reference by which the cached value should be returned.
     */
    void getEvaluator (Vsa::IEvaluator::Reference &pEvaluator) {
        pEvaluator = m_pEvaluator;
    }

    /**
     * Retrieves cached error message.
     *
     * @param rMessage the reference by which the cached error message should be returned.
     */
    void getErrorMessage (VString &rMessage) {
        rMessage = m_sErrorMessage;
    }
// State
private:
    /** Cached return value. */
    Vsa::IEvaluator::Reference m_pEvaluator;
    /** Cached error message. */
    VString m_sErrorMessage;
};

#endif
#ifndef Vsa_VEvaluatorClientWaiter_Interface
#define Vsa_VEvaluatorClientWaiter_Interface

#include "Vsa_VEvaluatorClient.h"
#include "V_VWaiter.h"

namespace Vsa {
    /** Used to wait for an evaluation result. Subclasses should still override methods as they would when subclassing VEvaluatorClient. */
    class VEvaluatorClientWaiter : public V::VWaiter, public Vsa::VEvaluatorClient {
        DECLARE_ABSTRACT_RTTLITE (VEvaluatorClientWaiter, Vsa::VEvaluatorClient);

    // Construction
    protected:
        /** Empty default constructor. */
        VEvaluatorClientWaiter () {}

    // Destruction
    protected:
        ~VEvaluatorClientWaiter () {}

    // IEvaluatorClient Methods
    public:
        /**
         * Signals the wait lock.
         * Overriding the implementation found in VEvaluatorClient, here we'll provide an OnResult that signals the wait lock after the existing implementation of OnResult() is called (including a call to OnResult_()).
         */
        void OnResult (IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput) {
            BaseClass::OnResult (pRole, pResult, rOutput);
            signal ();
        }
        /**
         * Signals the wait lock.
         */
        void OnError (Vca::IClient *pRole, Vca::IError *pError, const VString &rMessage) {
            BaseClass::OnError (pRole, pError, rMessage);
            signal (false);
        }
    };
}

#endif

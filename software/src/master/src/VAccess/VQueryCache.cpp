/**
 * @file
 * Vsa::VEvaluatorClientWaiter Implementation.
 */

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VQueryCache.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

void VQueryCache::OnResult_ (Vsa::IEvaluationResult *pResult, V::VString const &rOutput) {
    m_result = rOutput;
}

bool VQueryCache::evaluateWith (Vsa::IEvaluator *pEvaluator, V::VTime const *limit) {
    Vsa::IEvaluatorClient::Reference rEvaluatorClient;
    getRole (rEvaluatorClient);
    pEvaluator->EvaluateExpression (rEvaluatorClient, "", m_query);
    if (!limit) {
        return wait ();
    } else {
        return waitUntil (*limit);
    }
}
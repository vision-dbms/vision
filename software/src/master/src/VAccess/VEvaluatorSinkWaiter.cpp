/**
 * @file
 * VEvaluatorSinkWaiter Implementation.
 */

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VEvaluatorSinkWaiter.h"
 
/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

void VEvaluatorSinkWaiter::OnData (Vsa::IEvaluator *pEvaluator) {
    m_pEvaluator.setTo (pEvaluator);
    m_sErrorMessage.clear();
    signal ();
}

void VEvaluatorSinkWaiter::OnError (Vca::IError *pError, const V::VString &rMessage) {
    m_sErrorMessage = rMessage;
    signal (false);
}
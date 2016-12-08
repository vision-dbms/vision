/**
 * @file
 * Implementation of collection of gofers for IEvaluator interface instances.
 */
#define Vsa_IEvaluator_Gofers

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_IEvaluator_Gofers.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vsa_VEvaluatorPump.h"
#include "Vsa_VEvaluatorPumpSource.h"

void Vsa::IEvaluator_Gofer::FromPipeSource::onNeed () {
    // Materialize IPipeSource
    m_pIPipeSource.materializeFor (this);
    
    // All set, let the base class know.
    BaseClass::onNeed();
}

void Vsa::IEvaluator_Gofer::FromPipeSource::onData () {
    // Create Evaluator Pump
    VEvaluatorPump::PumpSettings::Reference pPumpSettings (new VEvaluatorPump::PumpSettings ());
    VEvaluatorPumpSource::Reference pPumpSource (new VEvaluatorPumpSource (pPumpSettings, m_pIPipeSource, "2+2;", false)); // TODO: Allow for Plumbed workers.

    // Create IEvaluatorSource
    IEvaluatorSource::Reference pIEvaluatorSource;
    pPumpSource->getRole (pIEvaluatorSource);

    // Retrieve IEvaluator.
    IEvaluatorSink::Reference pSink;
    getRole (pSink);
    pIEvaluatorSource->Supply (pSink);

}

void Vsa::IEvaluator_Gofer::FromPipeSource::OnData (IEvaluatorSink *pRole, IEvaluator *pEvaluator) {
    setTo (pEvaluator);
}


/*****  VTopTaskBase Implementation  *****/

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

#include "VTopTaskBase.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***********************************
 ***********************************
 *****                         *****
 *****  VTopTaskBase::Context  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTopTaskBase::Context::Context (VTopTaskBase* pController)
: VTask::Context (m_iControlPoints), m_iControlPoints (pController)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VTopTaskBase::Context::~Context () {
}


/**************************
 **************************
 *****                *****
 *****  VTopTaskBase  *****
 *****                *****
 **************************
 **************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VTopTaskBase);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VTopTaskBase::MetaMaker () {
    CSym ("isATopTask")->set (RTT, &g_siTrue);
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTopTaskBase::VTopTaskBase (VTask* pSpawningTask, IOMDriver* pChannel) : VTask (
    &m_iContext, m_iDatum, pChannel, new VComputationScheduler (), pSpawningTask
), m_iContext (this), m_fDatumAvailable (false) {
    m_pScheduler->retain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VTopTaskBase::~VTopTaskBase () {
    m_pScheduler->release ();
}

/***********************
 ***********************
 *****             *****
 *****  Execution  *****
 *****             *****
 ***********************
 ***********************/

void VTopTaskBase::start () {
    m_pScheduler->incrementTopTaskCount ();
    resume ();
}

void VTopTaskBase::exit () {
    m_pScheduler->decrementTopTaskCount ();
    BaseClass::exit ();
}

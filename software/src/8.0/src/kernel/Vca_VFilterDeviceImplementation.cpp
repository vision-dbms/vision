/*****  Vca_VFilterDeviceImplementation Implementation  *****/

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

#include "Vca_VFilterDeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VOutputSequencer.h"


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::VFilterDeviceImplementation  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VFilterDeviceImplementation::VFilterDeviceImplementation (
    VReferenceable *pContainer, FactoryData &rData
) : BaseClass (pContainer) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VFilterDeviceImplementation::~VFilterDeviceImplementation () {
}


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  Vca::VFilterDeviceImplementation::FactoryData  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VFilterDeviceImplementation::FactoryData::FactoryData (
    PipeSourceData const &rData, VBSProducer *pBSProducer, VBSConsumer *pBSConsumer
) : m_pBSProducer (pBSProducer), m_pBSConsumer (pBSConsumer) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VFilterDeviceImplementation::FactoryData::~FactoryData () {
}

/************************
 ************************
 *****  Access/Use  *****
 ************************
 ************************/

Vca::VOutputSequencer *Vca::VFilterDeviceImplementation::FactoryData::newOutputSequencer () const {
    return new VOutputSequencer (m_pBSConsumer);
}

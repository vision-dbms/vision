/*****  Vca_VNullDeviceImplementation Implementation  *****/

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

#include "Vca_VNullDeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerConsumerDevice_.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VNullDeviceImplementation  *****
 *****                                  *****
 ********************************************
 ********************************************/

/********************************************************
 ********************************************************
 ******  BUILDER Mandated Template Instantiations  ******
 ********************************************************
 ********************************************************/

template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VNullDeviceImplementation>::ConsumerTypes>;
template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VNullDeviceImplementation>::ProducerTypes>;
template class Vca_API Vca::VBSPoll_<Vca::VDevice_<Vca::VNullDeviceImplementation>::ConsumerTypes>;
template class Vca_API Vca::VBSPoll_<Vca::VDevice_<Vca::VNullDeviceImplementation>::ProducerTypes>;

template class Vca_API Vca::VBSProducerConsumerDevice_<Vca::VNullDeviceImplementation>;

template class Vca_API Vca::VDevice_<Vca::VNullDeviceImplementation>;

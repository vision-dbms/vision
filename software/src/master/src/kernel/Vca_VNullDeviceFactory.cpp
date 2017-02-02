/*****  Vca_VNullDeviceFactory Implementation  *****/

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

#include "Vca_VNullDeviceFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VNullDeviceFactory  *****
 *****                           *****
 *************************************
 *************************************/

/*************************************
 *************************************
 *****  Template Instantiations  *****
 *************************************
 *************************************/

template class Vca_API Vca::VStockDeviceFactory_<Vca::VNullDeviceImplementation>;

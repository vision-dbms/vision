/*****  Vca_VLoopbackDeviceFactory Implementation  *****/

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

#include "Vca_VLoopbackDeviceFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VLoopbackDeviceFactory  *****
 *****                               *****
 *****************************************
 *****************************************/

/*************************************
 *************************************
 *****  Template Instantiations  *****
 *************************************
 *************************************/

template class Vca_API Vca::VStockDeviceFactory_<Vca::VLoopbackDeviceImplementation>;

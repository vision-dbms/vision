/*****  VDatabaseComponent Implementation  *****/

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

#include "VDatabaseComponent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VDatabase.h"

#include "V_VString.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VDatabaseComponent);


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VDatabase *VDatabaseComponent::database_() const {
    return container_()->database_();
}

/*****  VDatabaseFederatorForBatchvision Implementation  *****/

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

#include "VDatabaseFederatorForBatchvision.h"

/************************
 *****  Supporting  *****
 ************************/

#include "M_AND.h"

#include "VDatabaseNDF.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VDatabaseFederatorForBatchvision);


/*********************************
 *********************************
 *****  Database Activation  *****
 *********************************
 *********************************/

VDatabaseActivation *VDatabaseFederatorForBatchvision::Activate_(
    VDatabaseNDF *pDatabaseNDF, char const *pVersionSpec, bool makingNewNDF
) {
    return new M_AND (this, pDatabaseNDF, pVersionSpec, makingNewNDF);
}

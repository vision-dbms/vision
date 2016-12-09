/*****  Vca_VPublication Implementation  *****/
    
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

#include "Vca_VPublication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VRolePlayer.h"

#include "VTypeInfo.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VPublicationBase  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPublicationBase::VPublicationBase () {
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VPublicationBase::publishes (VTypeInfo *pTypeInfo) const {
    return typeInfo_()->specializes (pTypeInfo);
}

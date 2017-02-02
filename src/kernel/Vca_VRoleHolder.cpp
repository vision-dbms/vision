/*****  Vca_VRoleHolder Implementation  *****/

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

#include "Vca_VRoleHolder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VRolePlayer.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VRoleHolder  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VRoleHolder::VRoleHolder (VRolePlayer *pRolePlayer) : m_pSuccessor (pRolePlayer->m_pRoleHolders) {
    pRolePlayer->m_pRoleHolders = this;
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VRoleHolder::implements (VTypeInfo *pTypeInfo) const {
    return typeInfo_()->specializes (pTypeInfo);
}

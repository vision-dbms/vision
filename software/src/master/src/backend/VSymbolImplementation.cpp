/*****  VSymbolImplementation Implementation  *****/

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

#include "VSymbolImplementation.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************************
 ***********************************
 *****                         *****
 *****  VSymbolImplementation  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************************
 **************************************
 *****  Constant Implementations  *****
 **************************************
 **************************************/

void VsiTrue::implement (VSNFTask *pTask) const {
    pTask->loadDucWithTrue ();
}

void VsiFalse::implement (VSNFTask *pTask) const {
    pTask->loadDucWithFalse ();
}

void VsiNA ::implement (VSNFTask *pTask) const {
    pTask->loadDucWithNA ();
}

VsiTrue		g_siTrue;
VsiFalse	g_siFalse;
VsiNA		g_siNA;

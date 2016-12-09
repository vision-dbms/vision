/*****  VContainerHandle Implementation  *****/

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

#include "VContainerHandle.h"

/************************
 *****  Supporting  *****
 ************************/

#include "M_CPD.h"

#include "Vdd_Store.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

#ifdef VMS_LINUX_EXPLICIT_COMPAT
template class VStoreHandle_<VContainerHandle>;
#endif


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VContainerHandle);

/**********************************
 **********************************
 *****  Enumeration Handlers  *****
 **********************************
 **********************************/

typedef bool (*CallF) (VContainerHandle*, V::VArgList const&);
typedef void (*CallP) (VContainerHandle*, V::VArgList const&);

bool VContainerHandle::CallFunction (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    CallF const pFunction = iArgList.arg<CallF>();
    return pFunction (this, iArgList);
}

bool VContainerHandle::CallProcedure (VArgList const &rArgList) {
    VArgList iArgList (rArgList);
    CallP const pProcedure = iArgList.arg<CallP> ();
    pProcedure (this, iArgList);
    return true;
}

bool VContainerHandle::DoNothing (VArgList const&) {
    return true;
}

/*****************************
 *****************************
 *****  Type Conversion  *****
 *****************************
 *****************************/

Vdd::Store *VContainerHandle::getStore_() {
    return 0;
}

bool VContainerHandle::LocateNameOf (Vdd::Store *pStore, M_TOP &rIdentity) const {
    VContainerHandle::Reference pStoreHandle;
    pStore->getContainerHandle (pStoreHandle);
    return LocateNameOf (pStoreHandle, rIdentity);
}

bool VContainerHandle::LocateOrAddNameOf (Vdd::Store *pStore, M_TOP &rIdentity) const {
    VContainerHandle::Reference pStoreHandle;
    pStore->getContainerHandle (pStoreHandle);
    return LocateOrAddNameOf (pStoreHandle, rIdentity);
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void VContainerHandle::describe_(bool bVerbose) {
    M_CPD *pCPD = GetCPD ();
    if (bVerbose)
	RTYPE_Print (pCPD, -1);
    else
	RTYPE_RPrint (pCPD, -1);
    pCPD->release ();
}

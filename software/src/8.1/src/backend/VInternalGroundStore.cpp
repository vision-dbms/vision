/*****  VInternalGroundStore Implementation  *****/

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

#include "VInternalGroundStore.h"

/************************
 *****  Supporting  *****
 ************************/

#include "venvir.h"

#include "RTvstore.h"

#include "VSelector.h"

#include "VSymbol.h"
#include "VSymbolSpace.h"
#include "VSymbolImplementation.h"


/**********************************
 **********************************
 *****                        *****
 *****  VInternalGroundStore  *****
 *****                        *****
 **********************************
 **********************************/

/***************************
 ***************************
 *****  UID Generator  *****
 ***************************
 ***************************/

unsigned int VInternalGroundStore::g_xUID32 = 0;

/**********************************
 **********************************
 *****  Cluster Symbol Space  *****
 **********************************
 **********************************/

VSymbolSpace VInternalGroundStore::g_iClusterSymbolSpace;

VSymbol *VInternalGroundStore::CSym (char const* pName) {
    return g_iClusterSymbolSpace.locateOrAdd (pName);
}

void VInternalGroundStore::DisplayCSymBindings () {
    g_iClusterSymbolSpace.DisplayBindings ();
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VInternalGroundStore);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_f0_c<VInternalGroundStore, unsigned int>;
template class Vsi_f0<VInternalGroundStore, unsigned int>;
template class Vsi_f0_c<VInternalGroundStore, char const*>;
template class Vsi_f0_c<VInternalGroundStore, VRunTimeType*>;

void VInternalGroundStore::MetaMaker () {
    static Vsi_f0_c<VInternalGroundStore, unsigned int> const
	si_cardinality		(&VInternalGroundStore::cardinality_),
	si_referenceCount	(&VInternalGroundStore::referenceCountMetaMakerWorkaround);

    static Vsi_f0<VInternalGroundStore, unsigned int> const
	si_tuid32		(&VInternalGroundStore::tuid32);

    static Vsi_f0_c<VInternalGroundStore, VRunTimeType*> const
	si_typeName		(&VInternalGroundStore::rtt);

    CSym ("isAControlSubscription"	)->set (RTT, &g_siFalse);

    CSym ("cardinality"			)->set (RTT, &si_cardinality);

    CSym ("referenceCount"		)->set (RTT, &si_referenceCount);

    CSym ("tuid32"			)->set (RTT, &si_tuid32);
    CSym ("typeName"			)->set (RTT, &si_typeName);
}


/*********************************
 *********************************
 *****  Task Implementation  *****
 *********************************
 *********************************/

VSymbolImplementation const *VInternalGroundStore::implementationOf (char const* pName) const {
    VSymbol *pSymbol = g_iClusterSymbolSpace.locate (pName);
    return pSymbol ? pSymbol->implementationAt (rtt ()) : 0;
}

bool VInternalGroundStore::groundImplementationSucceeded (VSNFTask* pTask) {
    VSymbolImplementation const *pImplementation = implementationOf (pTask->selector ());
    if (pImplementation) {
	pImplementation->implement (pTask);
	return true;
    }
    return false;
}

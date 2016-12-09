/*****  Vdd::Store Implementation  *****/

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

#include "Vdd_Store.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdictionary.h"
#include "RTlink.h"
#include "RTptoken.h"
#include "RTvstore.h"

#include "VGroundStore.h"


/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool Vdd::Store::decodeClosure_(
    VReference<rtBLOCK_Handle> &rpBlock, unsigned int &rxPrimitive, VReference<rtCONTEXT_Handle> *ppContext = 0
) const {
    return false;
}

bool Vdd::Store::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.clear ();
    return false;
}

void Vdd::Store::getCanonicalization (rtVSTORE_Handle::Reference &rpStore, DSC_Pointer &rPointer) {
    if (getCanonicalization_(rpStore, rPointer))
	rpStore->canonicalize (rPointer);
}

/*********************
 *********************
 *****  Cloning  *****
 *********************
 *********************/

void Vdd::Store::clone_(Reference &rpResult, rtPTOKEN_Handle *pPPT) const {
    rpResult.clear ();
}

bool Vdd::Store::isACloneOf_(Vdd::Store const *pOther) const {
    return Names (pOther) || rtype () == pOther->rtype ();
}

bool Vdd::Store::isACloneOfIndex_(rtINDEX_Handle const *pOther) const {
    return false;
}

bool Vdd::Store::isACloneOfListStore_(rtLSTORE_Handle const *pOther) const {
    return false;
}

bool Vdd::Store::isACloneOfValueStore_(rtVSTORE_Handle const *pOther) const {
    return false;
}


/*******************************
 *******************************
 *****  Dictionary Access  *****
 *******************************
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to obtain a selector's dictionary definition.
 *
 *  Arguments:
 *	rSelector		- the address of a 'VSelector'.
 *	xPropertySlot		- a reference to the return location of the property slot
 *				  for 'DictionaryLookup_FoundProperty' results.
 *	pValueReturn		- an optional (Nil to omit) address of a descriptor which
 *				  will be initialized to either the value of the selector
 *				  for non-properties or to the property prototype for
 *				  properties.
 *
 *  Returns:
 *	The appropriate Vdd::Store::DictionaryLookup enumeration value.
 *
 *****/
Vdd::Store::DictionaryLookup Vdd::Store::lookup (
    VSelector const &rSelector, DSC_Pointer const &rPointer, unsigned int &rxPropertySlot, DSC_Descriptor *pValueReturn
) {
    rtDICTIONARY_Handle::Reference pDictionary (getDictionary (rPointer));
    return pDictionary->getElement (rSelector, pValueReturn, &rxPropertySlot);
}

Vdd::Store::DictionaryLookup rtVSTORE_Handle::lookup (VSelector const &rSelector, unsigned int &rxPropertySlot, DSC_Descriptor *pValueReturn) {
    rtDICTIONARY_Handle::Reference pDictionary (getDictionary ());
    return pDictionary->getElement (rSelector, pValueReturn, &rxPropertySlot);
}


/*****************************
 *****************************
 *****  Instance Access  *****
 *****************************
 *****************************/

bool Vdd::Store::getInheritance (Vdd::Store::Reference &rpStore, DSC_Pointer &rPointer) {
    rtVSTORE_Handle::Reference pCanonicalStore;
    getCanonicalization (pCanonicalStore, rPointer);

    if (pCanonicalStore && pCanonicalStore->hasAnInheritance ()) {
	DSC_Pointer iInheritance; {
	    M_CPD *pInheritancePointer;
	    pCanonicalStore->getInheritancePointer (pInheritancePointer);
	    iInheritance.construct (pInheritancePointer);
	    pInheritancePointer->release ();
	}

	DSC_Pointer iPointer;
	iPointer.constructComposition (rPointer, iInheritance);
	iInheritance.clear ();

	rPointer.setToMoved (iPointer);

	pCanonicalStore->getInheritanceStore (rpStore);
	return true;
    }
    return false;
}

bool Vdd::Store::getProperty_(
    Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
) {
    rtVSTORE_Handle::Reference pCanonicalStore;
    getCanonicalization (pCanonicalStore, rPointer);
    return pCanonicalStore->getProperty (rpResult, rPointer, xPropertySlot, pPropertyPrototype);
}


/*******************************
 *******************************
 *****  Instance Deletion  *****
 *******************************
 *******************************/

bool Vdd::Store::doInstanceDeletionSetup (rtPTOKEN_CType::Reference &rpPTC, bool bIndependenceRequired) {
    align ();
    rtPTOKEN_Handle::Reference pPPT (getPToken ());
    if (bIndependenceRequired && pPPT->isDependent ())
	return false;

    rpPTC.setTo (pPPT->makeAdjustments ());
    return true;
}

bool Vdd::Store::doInstanceDeletion (DSC_Scalar &pInstances, rtPTOKEN_CType *pPTC, rtPTOKEN_Handle::Reference &rpNewPPT) {
    rtREFUV_AlignReference (&pInstances);
    unsigned int xInstance = DSC_Scalar_Int (pInstances);
    if (xInstance < rtPTOKEN_PTC_BaseCount (pPTC)) {
	pPTC->AppendAdjustment (xInstance + 1, -1);
	rpNewPPT.setTo (pPTC->ToPToken ());
	return true;
    }
    return false;
}

bool Vdd::Store::doInstanceDeletion (
    rtLINK_CType*		pInstances,
    rtPTOKEN_CType*		pPTC,
    rtPTOKEN_Handle::Reference&	rpNewPPT,
    rtLINK_CType*&		rpTrues,
    rtLINK_CType*&		rpFalses
) {
    pInstances->Align ();

#define handleNil(position, count, refNil) {\
    rtLINK_AppendRange (rpFalses, position, count);\
}

#define handleRepeat(position, count, origin) {\
    rtLINK_AppendRepeat (rpTrues, position, count);\
    pPTC->AppendAdjustment (origin + 1 - totalDeleted++, -1);\
}

#define handleRange(position, count, origin) {\
    rtLINK_AppendRange (rpTrues, position, count);\
    pPTC->AppendAdjustment (origin + count - totalDeleted, -count);\
    totalDeleted += count;\
}

    unsigned int totalDeleted = 0;
    rpTrues = rtLINK_RefConstructor (pInstances->PPT ());
    rpFalses = rtLINK_RefConstructor (pInstances->PPT ());

    rtLINK_TraverseRRDCList (pInstances, handleNil, handleRepeat, handleRange);

    rpNewPPT.setTo (pPTC->ToPToken ());

    return true;
}

/***********************************/

bool Vdd::Store::doInstanceDeletion (DSC_Scalar &pInstances) {
    rtPTOKEN_CType::Reference pPTC; rtPTOKEN_Handle::Reference pPPT;
    return doInstanceDeletionSetup (pPTC, true) && doInstanceDeletion (pInstances, pPTC, pPPT);
}

bool Vdd::Store::doInstanceDeletion (rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) {
    rtPTOKEN_CType::Reference pPTC; rtPTOKEN_Handle::Reference pPPT;
    return doInstanceDeletionSetup (pPTC, true) && doInstanceDeletion (pInstances, pPTC, pPPT, rpTrues, rpFalses);
}


/******************************
 ******************************
 *****  Mutex Management  *****
 ******************************
 ******************************/

bool Vdd::Store::AcquireMutex (VMutexClaim &rClaim, VComputationUnit *pSupplicant) {
    return m_iMutex.acquireClaim (rClaim, pSupplicant, referenceable ());
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vdd::Store::describe_(bool bVerbose) {
    VContainerHandle::Reference pHandle;
    getContainerHandle (pHandle);
    pHandle->describe (bVerbose);
}

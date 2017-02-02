/*****  VCollectionOf Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include "VkRadixListSorter.h"

/******************
 *****  Self  *****
 ******************/

#include "VCollectionOf.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdictionary.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VCollectionOf<unsigned int>;


/*************************************
 *************************************
 *****                           *****
 *****  VCollectionOfUnsigned32  *****
 *****                           *****
 *************************************
 *************************************/

/**********************
 **********************
 *****  Ordering  *****
 **********************
 **********************/

void VCollectionOfUnsigned32::sortOrderingArray (
    unsigned int *pOrderingArray, unsigned int sOrderingArray
) const {
    UTIL_RadixListSort (m_pElementArray, pOrderingArray, sOrderingArray);
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VCollectionOfUnsigned32::insertInto (
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeInsert (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeInsert_(
    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Insert (pElements, rpReordering, rAssociation);
}

void VCollectionOfUnsigned32::locateIn (
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeLocate (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeLocate_(
    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Locate (pElements, rpReordering, rAssociation);
}

void VCollectionOfUnsigned32::deleteFrom (
    Store *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    pSet->associativeDelete (this, rpReordering, rAssociation);
}

void rtDICTIONARY_Handle::associativeDelete_(
    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
) {
    Delete (pElements, rpReordering, rAssociation);
}

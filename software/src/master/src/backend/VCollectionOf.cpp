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
    M_CPD *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
        rtDICTIONARY_Insert (pSet, this, rpReordering, rAssociation);
	break;
    default:
	break;
    }
}

void VCollectionOfUnsigned32::locateIn (
    M_CPD *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
	rtDICTIONARY_Locate (pSet, this, rpReordering, rAssociation);
	break;
    default:
	break;
    }
}

void VCollectionOfUnsigned32::deleteFrom (
    M_CPD *pSet, M_CPD *&rpReordering, VAssociativeResult &rAssociation
)
{
    switch (M_CPD_RType (pSet)) {
    case RTYPE_C_Dictionary:
	rtDICTIONARY_Delete (pSet, this, rpReordering, rAssociation);
	break;
    default:
	break;
    }
}

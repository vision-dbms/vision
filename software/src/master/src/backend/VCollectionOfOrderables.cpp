/*****  VCollectionOfOrderables Implementation  *****/

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

#include "VCollectionOfOrderables.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTrefuv.h"

#include "VSorter.h"


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

unsigned int const VCollectionOfOrderables::g_pTrivialOrdering[1] = {0};

/*******************************
 *******************************
 *****  Cache Maintenance  *****
 *******************************
 *******************************/

void VCollectionOfOrderables::refreshCache ()
{
}


/**********************
 **********************
 *****  Ordering  *****
 **********************
 **********************/

M_CPD* VCollectionOfOrderables::orderDPT () {
    if (m_pOrderDPT.isNil ())
	createOrderingObjects ();
    return m_pOrderDPT;
}

M_CPD* VCollectionOfOrderables::ordering () {
    if (m_pOrderDPT.isNil ())
	createOrderingObjects ();
    return m_pOrdering;
}

unsigned int const* VCollectionOfOrderables::orderingArray () {
    if (m_pOrderDPT.isNil ())
	createOrderingObjects ();
    return m_pOrderingArray;
}

void VCollectionOfOrderables::createOrderingObjects () {
    unsigned int nelements = cardinality ();
    if (nelements > 1) {
	m_pOrderDPT.claim (rtPTOKEN_New (M_AttachedNetwork->ScratchPad (), nelements));
	m_pOrdering.claim (
	    rtREFUV_New (m_pOrderDPT, ptoken (), NilOf (Ref_UV_Initializer))
	);

	unsigned int *pOrderingArray = UV_CPD_Array (m_pOrdering, unsigned int);
	for (unsigned int i = 0; i < nelements; i++)
	    pOrderingArray[i] = i;

	sortOrderingArray (pOrderingArray, nelements);

	m_pOrderingArray = pOrderingArray;
    }
    else {
	m_pOrderDPT.setTo (m_pDPT);
	m_pOrderingArray = g_pTrivialOrdering;
    }
}

void VCollectionOfOrderables::sortOrderingArray (
    unsigned int *pOrderingArray, unsigned int sOrderingArray
) const
{
    VSorter iSorter (*this);
    iSorter.hsort (pOrderingArray, sOrderingArray, sizeof (*pOrderingArray));
}

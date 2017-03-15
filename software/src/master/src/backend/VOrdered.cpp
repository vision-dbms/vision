/*****  VOrdered Implementation  *****/

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

#include "VOrdered.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTrefuv.h"

#include "VCollectionOfOrderables.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VOrderedBase::VOrderedBase (VCollectionOfOrderables& rOrderableCollection)
: VCollection		(rOrderableCollection.orderDPT ())
, m_pOrdering		(rOrderableCollection.ordering ())
, m_pOrderingArray	(rOrderableCollection.orderingArray ())
, m_pOrderingLimit	(m_pOrderingArray + cardinality ())
{
    resetCursor (0);
}

VOrderedBase::VOrderedBase (VOrderedBase& rOrderedBase, rtLINK_CType* pRestriction)
: VCollection		(pRestriction->PPT ())
, m_pOrdering		(0, rOrderedBase.restrictedOrdering (pRestriction))
, m_pOrderingArray	(m_pOrdering.isntNil ()
			 ? UV_CPD_Array (m_pOrdering, unsigned int)
			 : rOrderedBase.orderingArray ())
, m_pOrderingLimit	(m_pOrderingArray + cardinality ())
{
    resetCursor (0);
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

M_CPD* VOrderedBase::restrictedOrdering (rtLINK_CType* pRestriction) const
{
    return m_pOrdering.isntNil () ? rtREFUV_LCExtract (m_pOrdering, pRestriction) : 0;
}

#ifndef VCollectionOf_Implementation
#define VCollectionOf_Implementation

/********************************
 ********************************
 *****  Element Comparison  *****
 ********************************
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to compare two elements given their indices.
 *
 *  Arguments:
 *	xElement1, xElement2	- the indices of the elements to compare.
 *
 *  Returns:
 *	A value
 *		 < 0 if 'Elements (xElement1) <  Elements (xElement2)'
 *		== 0 if 'Elements (xElement1) == Elements (xElement2)'
 *		 > 0 if 'Elements (xElement1) >  Elements (xElement2)'.
 *
 *  Notes:
 *	This predicate sorts 'nil' (i.e., negative) origins after all
 *	real origins.
 *
 *****/
template <class ElementType> int VCollectionOf<ElementType>::compare (
    unsigned int xElement1, unsigned int xElement2
) const {
    ElementType const& iElement1 = m_pElementArray[xElement1];
    ElementType const& iElement2 = m_pElementArray[xElement2];

    return
	iElement1 < iElement2 ? -1 :
	iElement1 > iElement2 ?  1 :
	0;
}


#endif

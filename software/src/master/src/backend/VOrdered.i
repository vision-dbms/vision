#ifndef VOrdered_Implementation
#define VOrdered_Implementation

template <class CType, class EType> void VOrdered<CType,EType>::goToNextDifference () {
    if (elementIsValid ())
    {
	unsigned int xInitialElement = *m_pOrderingElement;
	do {
	    goToNextElement ();
	} while (
	    elementIsValid () && m_rOrderableCollection.compare (
		xInitialElement, *m_pOrderingElement
	    ) == 0
	);
    }
}

template <class CType, class EType> void VOrdered<CType,EType>::cacheElement () {
    if (m_pOrderingElement < m_pOrderingLimit)
    {
	m_iCachedElementIsValid = m_rOrderableCollection.GetElement (
	    *m_pOrderingElement, m_iCachedElement
	);
    }
}


#endif

#ifndef VSetOf_Implementation
#define VSetOf_Implementation

/*************************************
 *****  Deferred Initialization  *****
 *************************************/

template <class ELType, class EVType> void VSetOf<ELType,EVType>::initializeDPT () {
    m_pDPT.claim (m_iElementArray.newBasePToken ());
    m_pElementArrayDPT.setTo (m_pDPT);
}

/***********************
 *****  Alignment  *****
 ***********************/

template <class ELType, class EVType> void VSetOf<ELType,EVType>::align () {
    if (m_pElementArrayDPT.isntNil ())
	m_pElementArrayDPT.claim (m_iElementArray.editUsingDPTChain (m_pElementArrayDPT));
}

/************************
 *****  Comparison  *****
 ************************/

template <class ELType, class EVType> int VSetOf<ELType,EVType>::compare (unsigned int xElement, EVType iKey) const {
    ELType const& rElement = m_iElementArray[xElement];
    return rElement < iKey ? -1 : rElement == iKey ? 0 : 1;
}


/********************
 *****  Access  *****
 ********************/

template <class ELType, class EVType> void VSetOf<ELType,EVType>::Locate (
    VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    iSetCursor.Locate (pKeys, rpReordering, rResult);
}

template <class ELType, class EVType> bool VSetOf<ELType,EVType>::Locate (EVType iKey, unsigned int& rxElement) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    return iSetCursor.Locate (iKey, rxElement);
}


/********************
 *****  Update  *****
 ********************/

/********************************
 *----  Insertion/Deletion  ----*
 ********************************/

template <class ELType, class EVType> void VSetOf<ELType,EVType>::Insert (
    VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    iSetCursor.Insert (pKeys, rpReordering, rResult);
}

template <class ELType, class EVType> void VSetOf<ELType,EVType>::Delete (
    VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    iSetCursor.Delete (pKeys, rpReordering, rResult);
}

/*---------------------------------------------------------------------
 *  The scalar 'Insert' and 'Delete' routines return true if the set was
 *  updated and false if no changes were made.
 *---------------------------------------------------------------------
 */
template <class ELType, class EVType>
bool VSetOf<ELType,EVType>::Insert (EVType iKey, unsigned int& rxElement) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    return iSetCursor.Insert (iKey, rxElement);
}

template <class ELType, class EVType> bool VSetOf<ELType,EVType>::Delete (EVType iKey) {
    VAssociativeCursor<VSetOf<ELType,EVType>,VCollectionOf<EVType>,EVType> iSetCursor (this);
    return iSetCursor.Delete (iKey);
}


/**********************************
 *----  Element Installation  ----*
 **********************************/

template <class ELType, class EVType> void VSetOf<ELType,EVType>::install (
    rtLINK_CType* pAdditions, VOrdered<VCollectionOf<EVType>,EVType> &rAdditions
) {
#   define handleNil(c, n, r)
#   define handleRepeat(c, n, r) {\
	m_iElementArray[r] = rAdditions.element ();\
	rAdditions.skip (n);\
    }
#   define handleRange(c, n, r) {\
	while (n-- > 0) {\
	    m_iElementArray[r++] = rAdditions.element ();\
	    rAdditions.goToNextElement ();\
	}\
    }

    rAdditions.resetCursor ();
    rtLINK_TraverseRRDCList (
	pAdditions, handleNil, handleRepeat, handleRange
    );

#   undef handleNil
#   undef handleRepeat
#   undef handleRange
}

#endif

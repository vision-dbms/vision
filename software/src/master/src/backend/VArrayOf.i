#ifndef VArrayOf_Implementation
#define VArrayOf_Implementation

/***********************
 *****  Alignment  *****
 ***********************/

/*--------------------------------------------------------------------------
 *  Notes:
 *	This routine replaces the current element array with one edited
 *	using the instructions in the p-token referenced by 'pDPT' and
 *	returns a handle for the p-token associated with the new set of
 *	elements.  The returned handle carries a reference which must either
 *	be released or claimed by the caller.
 *--------------------------------------------------------------------------
 */
template <class ElementType> rtPTOKEN_Handle *VArrayOf<ElementType>::editUsingDPTChain (rtPTOKEN_Handle *pDPT) {
    if (pDPT->isTerminal ())
    //  If there are no changes, keep the status quo, ...
	pDPT->retain ();
    else {
    //  ... otherwise, obtain the editing instructions, ...
        rtPTOKEN_CType *pDPTC = pDPT->getAdjustments ();

    //  ... allocate a new element array, ...
	ElementType* pNewElementArray = newElementArray (rtPTOKEN_PTC_BaseCount (pDPTC));

    //  ... initialize those elements using the supplied instructions, ...
	unsigned int xTargetElement = 0;
	unsigned int xSourceElement = 0;

#	define copyToSourceElement(xSourceLimit) while (xSourceElement < xSourceLimit) {\
	    pNewElementArray[xTargetElement++] = m_pElementArray[xSourceElement++];\
	}
#	define handleInsert(xSourceOrigin,sSourceShift) {\
	    copyToSourceElement (((unsigned int)xSourceOrigin));\
	    xTargetElement += sSourceShift;\
	}
#	define handleDelete(xSourceOrigin,sSourceShift) {\
	    copyToSourceElement (((unsigned int)(xSourceOrigin - sSourceShift)));\
	    xSourceElement = xSourceOrigin;\
	}
	rtPTOKEN_FTraverseInstructions (pDPTC, handleInsert, handleDelete);
	copyToSourceElement (m_iElementCount);
#	undef handleDelete
#	undef handleInsert
#	undef copyToSourceElement

    //  ... install the new elements, ...
	deleteElementArray ();
	m_pElementArray = pNewElementArray;
	m_iElementCount = rtPTOKEN_PTC_BaseCount (pDPTC);

    //  ... retain the DPT for the edited array, ...
	pDPT = pDPTC->NextGeneration ();
	pDPT->retain ();

    //  ... discard the editing instructions, ...
	pDPTC->discard ();
    }

    //  ... and return the current DPT.
    return pDPT;
}


template <class ElementType> void VArrayOf<ElementType>::insert (unsigned int xInsertionOrigin, unsigned int iElementCount) {
//  Adjust the parameters if the insertion origin is past the current end of the array...
    if (xInsertionOrigin > m_iElementCount) {
	iElementCount += xInsertionOrigin - m_iElementCount;
	xInsertionOrigin = m_iElementCount;
    }

//  ... and reallocate and install the new element array if there is work to do:
    if (iElementCount > 0) {
	ElementType* pNewElementArray = newElementArray (m_iElementCount + iElementCount);

	unsigned int xTargetElement = 0;
	unsigned int xSourceElement = 0;

	while (xSourceElement < xInsertionOrigin)
	    pNewElementArray[xTargetElement++] = m_pElementArray[xSourceElement++];

	xTargetElement += iElementCount;

	while (xSourceElement < m_iElementCount)
	    pNewElementArray[xTargetElement++] = m_pElementArray[xSourceElement++];

	deleteElementArray ();
	m_pElementArray = pNewElementArray;
	m_iElementCount += iElementCount;
    }
}


#endif

#ifndef VSet_Implementation
#define VSet_Implementation

/********************
 ********************
 *****  Insert  *****
 ********************
 ********************/

template <class SetClass, class KeyClass, class KeyValueType>
void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Insert (
    VOrdered<KeyClass,KeyValueType>& rKeys, VAssociativeResult& rResult
)
{
    VfLocateOrAddGenerator iGenerator (rResult);
    initializeGenerator (iGenerator, rKeys.ptoken ());

    resetCursor ();
    rKeys.resetCursor ();

    while (rKeys.elementIsValid ()) {
	bool found = Locate (rKeys.element ());
	unsigned int xSourceRangeOrigin = rKeys.elementIndex ();
	rKeys.goToNextDifference ();
	iGenerator.emitReference (
	    xSourceRangeOrigin,
	    rKeys.elementIndex () - xSourceRangeOrigin,
	    elementIndex (),
	    !found
	);
    }

    unsigned int xSourceRangeOrigin = rKeys.elementIndex ();
    unsigned int iSourceCardinality = rKeys.cardinality ();
    if (xSourceRangeOrigin < iSourceCardinality) iGenerator.emitReference (
	xSourceRangeOrigin, iSourceCardinality - xSourceRangeOrigin
    );
    iGenerator.commit ();

    m_pSet->installDPT (iGenerator.targetPToken ());
    if (iGenerator.additionsGenerated ()) {
	m_pSet->align ();
	if (iGenerator.inScalarMode ()) {
	    m_pSet->install (elementIndex (), rKeys.element ());
	    m_xElementLimit++;
	}
	else {
	    rtLINK_CType* pSourceAdditions = iGenerator.sourceReference ();
	    VOrdered<KeyClass,KeyValueType> iAddedKeys (rKeys, pSourceAdditions);

	    rtLINK_CType* pTargetAdditions = iGenerator.targetReference ()->Extract (
		pSourceAdditions
	    );

	    m_pSet->install (pTargetAdditions, iAddedKeys);

	    m_xElementLimit += pTargetAdditions->ElementCount ();

	    pTargetAdditions->release ();
	}
    }
}

template <class SetClass, class KeyClass, class KeyValueType> void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Insert (
    KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VOrdered<KeyClass,KeyValueType> iOrderedKeys (*pKeys);
    iOrderedKeys.getOrdering (rpReordering);
    Insert (iOrderedKeys, rResult);
}

template <class SetClass, class KeyClass, class KeyValueType>
bool VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Insert (
    KeyValueType iKeyValue
)
{
    if (Locate (iKeyValue))
	return false;

    m_pSet->adjustDomain (m_xElement, 1);
    m_pSet->install (m_xElement, iKeyValue);

    m_xElementLimit++;

    return true;
}


/********************
 ********************
 *****  Delete  *****
 ********************
 ********************/

template <class SetClass, class KeyClass, class KeyValueType> void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Delete (
    VOrdered<KeyClass,KeyValueType>& rKeys, VAssociativeResult& rResult
)
{
    VfDeleteGenerator iGenerator (rResult);
    initializeGenerator (iGenerator, rKeys.ptoken ());

    resetCursor ();
    rKeys.resetCursor ();

    while (rKeys.elementIsValid () && elementIsValid ()) {
	if (Locate (rKeys.element ()))
	    iGenerator.emitDeletion (rKeys.elementIndex (), elementIndex ());
	rKeys.goToNextDifference ();
    }

    iGenerator.commit ();

    m_pSet->installDPT (iGenerator.targetPToken ());
    if (iGenerator.inScalarMode ())
    {
	if (rResult.sourceReferenceIsValid ())
	{
	    m_pSet->align ();
	    m_xElementLimit--;
	}
    }
    else {
	unsigned int iDeletionCount = iGenerator.sourceReference ()->ElementCount ();
	if (iDeletionCount > 0) {
	    m_pSet->align ();
	    m_xElementLimit -= iDeletionCount;
	}
    }
}

template <class SetClass, class KeyClass, class KeyValueType>
void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Delete (
    KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VOrdered<KeyClass,KeyValueType> iOrderedKeys (*pKeys);
    iOrderedKeys.getOrdering (rpReordering);
    Delete (iOrderedKeys, rResult);
}

template <class SetClass, class KeyClass, class KeyValueType>
bool VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Delete (
    KeyValueType iKeyValue
) {
    if (Locate (iKeyValue)) {
	m_pSet->adjustDomain (m_xElement + 1, -1);
	m_xElementLimit--;
	return true;
    }

    return false;
}


/********************
 ********************
 *****  Locate  *****
 ********************
 ********************/

template <class SetClass, class KeyClass, class KeyValueType>
void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Locate (
    VOrdered<KeyClass,KeyValueType>& rKeys, VAssociativeResult& rResult
)
{
    VfLocateGenerator iGenerator (rResult);
    initializeGenerator (iGenerator, rKeys.ptoken ());

    resetCursor ();
    rKeys.resetCursor ();

    while (rKeys.elementIsValid () && elementIsValid ()) {
	bool found = Locate (rKeys.element ());
	unsigned int xSourceRangeOrigin = rKeys.elementIndex ();
	rKeys.goToNextDifference ();
	iGenerator.emitReference (
	    xSourceRangeOrigin,
	    rKeys.elementIndex () - xSourceRangeOrigin,
	    elementIndex (),
	    !found
	);
    }

    iGenerator.commit ();
}

template <class SetClass, class KeyClass, class KeyValueType>
void VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Locate (
    KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VOrdered<KeyClass,KeyValueType> iOrderedKeys (*pKeys);
    iOrderedKeys.getOrdering (rpReordering);
    Locate (iOrderedKeys, rResult);
}


template <class SetClass, class KeyClass, class KeyValueType>
bool VAssociativeCursor<SetClass,KeyClass,KeyValueType>::Locate (
    KeyValueType iKeyValue
) {
    bool found = false;
    int ll = m_xElement - 1;
    int ul = m_xElementLimit;
    while (ul - ll > 1) {
	int index = (ul + ll) / 2;
	int iComparisonResult = m_pSet->compare (index, iKeyValue);
	if (iComparisonResult == 0) {
	    found = true;
	    ul = index;
	    break;
	}
	else if (iComparisonResult < 0)
	    ll = index;
	else
	    ul = index;
    }

    m_xElement = ul;

    return found;
}

#endif

/*****  VArray Template Definitions  *****/
#ifndef VArray_TemplateDefinitions
#define VArray_TemplateDefinitions

/****************************************************************
 *****  VArray::Construction, Destruction & Initialization  *****
 ****************************************************************/

template <class E> VArray<E>::VArray (VArray<E> const& iOther) : VArrayBase (
    iOther.m_sDimensionVector, iOther.m_pDimensionVector
) {
    initialize ();

    unsigned int xElement = arraySize ();
    while (xElement-- > 0)
	m_pElementVector[xElement] = iOther.m_pElementVector[xElement];
}

template <class E> VArray<E>::VArray (size_t sDimensionVector) : VArrayBase (
    sDimensionVector
) {
    initialize ();
}

template <class E> VArray<E>::~VArray () {
    delete[] m_pElementVector;
}

template <class E> void VArray<E>::initialize () {
    m_pElementVector = new E[arraySize () + 1];
}


/********************************
 *****  VArray::Comparison  *****
 ********************************/

template <class E> bool VArray<E>::operator== (VArray<E> const &iOther) const {
    if (!conformsTo (iOther))
	return false;

    unsigned int xElement = arraySize ();
    while (xElement-- > 0)
    {
	if (m_pElementVector[xElement] != iOther.m_pElementVector[xElement])
	    return false;
    }

    return true;
}

template <class E> bool VArray<E>::operator!= (VArray<E> const &iOther) const
{
    if (!conformsTo (iOther))
	return true;

    unsigned int xElement = arraySize ();
    while (xElement-- > 0)
    {
	if (m_pElementVector[xElement] != iOther.m_pElementVector[xElement])
	    return true;
    }

    return false;
}


/****************************
 *****  VArray::Access  *****
 ****************************/

template <class E> E& VArray<E>::element (
    unsigned int xSubscript0, ...
) const
{
    va_list pSubscriptVector;
    va_start (pSubscriptVector, xSubscript0);
    return (*this)[elementOffset (pSubscriptVector, xSubscript0)];
}

template <class E> E& VArray<E>::element (
    unsigned int const* pSubscriptVector
) const
{
    return (*this)[elementOffset (pSubscriptVector)];
}

template <class E> E& VArray<E>::operator[] (unsigned int xElement) const
{
    return m_pElementVector[
	xElement < arraySize () ? xElement : arraySize ()
    ];
}


/****************************
 *****  VArray::Update  *****
 ****************************/

template <class E> VArray<E>& VArray<E>::operator= (VArray<E> const& iOther)
{
    reshape (iOther);
    unsigned int xElement = arraySize ();
    while (xElement-- > 0)
	m_pElementVector[xElement] = iOther.m_pElementVector[xElement];
    return *this;
}

template <class E> VArray<E>& VArray<E>::operator<< (E const& iElement)
{
    newElement() = iElement;
    return *this;
}

template <class E> E& VArray<E>::newElement ()
{
    unsigned int xElement = arraySize ();
    guarantee (xElement);
    return (*this)[xElement];
}


/******************************
 *****  VArray::Resizing  *****
 ******************************/

template <class E> void VArray<E>::fixVector (size_t sOldElementVector)
{
    size_t sNewElementVector = arraySize ();
    if (sNewElementVector == sOldElementVector)
	return;

    size_t sCopy = sOldElementVector;
    if (sCopy > sNewElementVector)
	sCopy = sNewElementVector;

    E* pOldElementVector = m_pElementVector;

    E* pNewElementVector = new E[sNewElementVector + 1];
    for (
	unsigned int xElement = 0; xElement < sCopy; xElement++
    ) pNewElementVector[xElement] = pOldElementVector[xElement];
    m_pElementVector = pNewElementVector;

    delete[] pOldElementVector;
}


/********************************
 *****  VArray::Formatting  *****
 ********************************/

template <class E> void VArray<E>::formatForVision (
    V::VString&	iExpression,
    char const*		pKeyword,
    bool		fQuote,
    bool		fClose,
    char const*		pSeparator
) const {
    size_t sElementVector = arraySize ();

    if (iExpression.guarantee (
	    iExpression.length () + strlen (pKeyword) + 16 * sElementVector
	)
    ) {
	iExpression.append (pKeyword);

	if (0 == sElementVector)
	    iExpression.append (" NA");
	else {
	    char const *pNextSeparator = " ";
	    unsigned int xElement;
	    for (xElement = 0; xElement < sElementVector; xElement++)
	    {
		iExpression.append (pNextSeparator);
		pNextSeparator = pSeparator;

		if (fQuote) iExpression.quote (
		    m_pElementVector [xElement]
		);
		else iExpression.append (
		    m_pElementVector [xElement]
		);
	    }
	}

	iExpression.append (fClose ? ".\n" : "\n");
    }
}


#endif

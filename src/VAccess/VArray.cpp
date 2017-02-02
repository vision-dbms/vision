/*****  VArrayBase Implementation  *****/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VArray.h"

/************************
 *****  Supporting  *****
 ************************/


/************************
 ************************
 *****              *****
 *****  VArrayBase  *****
 *****              *****
 ************************
 ************************/

/**************************************************
 *****  VArrayBase::Construction/Destruction  *****
 **************************************************/

VArrayBase::VArrayBase (
    size_t			sDimensionVector,
    VArrayDimensionData const*	pDimensionVector
)
: m_sDimensionVector (sDimensionVector)
, m_pDimensionVector (new VArrayDimensionData[sDimensionVector])
{
    if (pDimensionVector)
    {
	unsigned int xDimension = sDimensionVector;
	while (xDimension-- > 0)
	    m_pDimensionVector[xDimension] = pDimensionVector[xDimension];
    }
}

VArrayBase::~VArrayBase () {
    delete[] m_pDimensionVector;
}


/************************************
 *****  VArrayBase::Predicates  *****
 ************************************/

bool VArrayBase::conformsTo (VArrayBase const &iOther) const {
    if (m_sDimensionVector != iOther.m_sDimensionVector)
	return false;

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) if (
	m_pDimensionVector[
	    xDimension
	].m_sVector != iOther.m_pDimensionVector[
	    xDimension
	].m_sVector
    ) return false;

    return true;
}


/********************************************
 *****  VArrayBase::Counts and Offsets  *****
 ********************************************/

unsigned int VArrayBase::elementOffset (
    va_list pSubscriptVector, unsigned int xSubscript0
) const
{
    unsigned int xElementPosition = 0;

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) xElementPosition += m_pDimensionVector[xDimension].elementOffset (
	xDimension > 0 ? va_arg (pSubscriptVector, size_t) : xSubscript0
    );

    return xElementPosition;
}

unsigned int VArrayBase::elementOffset (
    unsigned int const* pSubscriptVector
) const
{
    unsigned int xElementPosition = 0;

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) xElementPosition += m_pDimensionVector[xDimension].elementOffset (
	pSubscriptVector[xDimension]
    );

    return xElementPosition;
}


/**************************************************
 *****  VArrayBase::Re-shaping and Re-Sizing  *****
 **************************************************/

void VArrayBase::reshape (VArrayBase const& iOtherArray)
{
    size_t		sOldElementVector = arraySize ();

    resizeDimensionVector (iOtherArray.m_sDimensionVector);

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) m_pDimensionVector[xDimension] = iOtherArray.m_pDimensionVector[
	xDimension
    ];

    fixVector (sOldElementVector);
}

void VArrayBase::reshape (size_t sDimensionVector, va_list pDimensionVector)
{
    size_t sOldElementVector = arraySize ();

    resizeDimensionVector (sDimensionVector);

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) m_pDimensionVector[xDimension].m_sVector = va_arg (
	pDimensionVector, size_t
    );

    fixCounts ();
    fixVector (sOldElementVector);
}

void VArrayBase::reshape (
    size_t sDimensionVector, size_t const *pDimensionVector
)
{
    size_t sOldElementVector = arraySize ();

    resizeDimensionVector (sDimensionVector);

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector; 
	xDimension++
    ) m_pDimensionVector[xDimension].m_sVector = pDimensionVector[
	xDimension
    ];

    fixCounts ();
    fixVector (sOldElementVector);
}


void VArrayBase::resizeDimensionVector (size_t sNewDimensionVector)
{
    if (sNewDimensionVector == m_sDimensionVector)
	return;

    delete[] m_pDimensionVector;
    m_pDimensionVector = new VArrayDimensionData[sNewDimensionVector];
    m_sDimensionVector = sNewDimensionVector;
}

void VArrayBase::clear ()
{
    size_t sOldElementVector = arraySize ();

    for (
	unsigned int xDimension = 0;
	xDimension < m_sDimensionVector;
	xDimension++
    ) m_pDimensionVector[xDimension].m_sVector = 0;

    fixCounts ();
    fixVector (sOldElementVector);
}


void VArrayBase::guarantee (unsigned int xElement)
{
    if (0 == m_sDimensionVector || 0 == m_pDimensionVector[0].m_sMatrix)
	return;

    size_t sElementVector = arraySize ();
    if (xElement >= sElementVector)
    {
	size_t sSubMatrix0 = m_pDimensionVector[0].m_sMatrix;
	size_t sDimension0 = (xElement + sSubMatrix0) / sSubMatrix0 * sSubMatrix0;

	m_pDimensionVector[0].m_sVector = sDimension0;

	fixVector (sElementVector);
    }
}

void VArrayBase::fixCounts ()
{
    size_t sSubMatrix = 1;

    unsigned int xDimension = m_sDimensionVector;
    while (xDimension-- > 0)
    {
	m_pDimensionVector[xDimension].m_sMatrix = sSubMatrix;
	sSubMatrix *= m_pDimensionVector[xDimension].m_sVector;
    }
}


/*************************************
 *****  VArrayBase::COM Support  *****
 *************************************/

#if defined(WIN32_COM_SERVER)

bool VArrayBase::NewSafeArray (VARTYPE xElementType, SAFEARRAY**ppArray, void**ppElementArray) const {
    *ppArray = 0;
    *ppElementArray = 0;

    SAFEARRAY* pSA; {
	unsigned int const nDimensions = dimensionCount ();

	SAFEARRAYBOUND* pSABArray = new SAFEARRAYBOUND[nDimensions];
	if (!pSABArray)
	    return false;

	for (unsigned int xDimension = 0; xDimension < nDimensions; xDimension++) {
	    SAFEARRAYBOUND& rSAB = pSABArray[nDimensions - xDimension - 1];
	    rSAB.cElements = elementCount (xDimension);
	    rSAB.lLbound   = 0;
	}
	pSA = SafeArrayCreate (xElementType, nDimensions, pSABArray);
	delete[] pSABArray;
    }
    if (!pSA)
	return false;

    void* pElementArray;
    if (FAILED (SafeArrayAccessData (pSA, &pElementArray))) {
	SafeArrayDestroy (pSA);
	return false;
    }

    *ppArray = pSA;
    *ppElementArray = pElementArray;

    return true;
}

bool VArrayBase::getElements (VARIANT* pElementArray, bool bReturningUnicode) const {
    unsigned int nElements = arraySize ();
    unsigned int xElement = 0;
    while (xElement < nElements) {
	VariantInit (pElementArray);
	V_VT (pElementArray) = VT_NULL;
	pElementArray++;
    }
    return true;
}

bool VArrayBase::getArrayOfVariants (SAFEARRAY**ppArray, bool bReturningUnicode) const {
    VARIANT* pElementArray;

    if (!NewSafeArray (VT_VARIANT, ppArray, reinterpret_cast<void**>(&pElementArray)))
	return false;

    getElements (pElementArray, bReturningUnicode);

    SafeArrayUnaccessData (*ppArray);

    return true;
}

bool VArrayBase::getArray (VARIANT* pArray, bool bReturningUnicode) const {
    SAFEARRAY* pSA;
    if (!getArrayOfVariants (&pSA, bReturningUnicode))
	return false;

    VariantClear (pArray);
    V_ARRAY (pArray) = pSA;
    V_VT (pArray) = VT_VARIANT | VT_ARRAY;

    return true;
}


#endif

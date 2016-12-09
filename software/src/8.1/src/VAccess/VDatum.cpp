/*****  Datum Class Implementation  ******/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/******************
 *****  Self  *****
 ******************/

#include "VDatum.h"

/************************
 *****  Supporting  *****
 ************************/


/********************
 ********************
 *****          *****
 *****  VDatum  *****
 *****          *****
 ********************
 ********************/

/*********************************
 *****  VDatum::Destruction  *****
 *********************************/

VDatum::~VDatum () {
    clear ();
}


/****************************
 *****  VDatum::Access  *****
 ****************************/

int VDatum::asInteger (bool& isValid) const {
    switch (m_xDatumKind)
    {
    case VDatumKind_Integer:
	isValid = true;
	return m_iInteger;

    case VDatumKind_Real:
	if (m_iReal >= (double)INT_MIN &&
	    m_iReal <= (double)INT_MAX
	)
	{
	    isValid = true;
	    return (int)m_iReal;
	}
	// ... no break here if default case is next.

    default:
	isValid = false;
	return 0;
    }
}

double VDatum::asReal (bool& isValid) const
{
    switch (m_xDatumKind)
    {
    case VDatumKind_Integer:
	isValid = true;
	return (double)m_iInteger;

    case VDatumKind_Real:
	isValid = true;
	return m_iReal;

    default:
	isValid = false;
	return 0.0;
    }
}

char const* VDatum::asString (bool &isValid) const {
    switch (m_xDatumKind) {
    case VDatumKind_String:
	isValid = true;
	return m_iString;

    default:
	isValid = false;
	return 0;
    }
}


/*********************************
 *****  VDatum::COM Support  *****
 *********************************/

#if defined(WIN32_COM_SERVER)
bool VDatum::getValue (VARIANT* pValue, bool bReturningUnicode) const {
    VariantClear (pValue);
    switch (m_xDatumKind)
    {
    case VDatumKind_NA:
	V_VT (pValue) = VT_NULL;
	return true;

    case VDatumKind_Integer:
	V_I4 (pValue) = m_iInteger;
	V_VT (pValue) = VT_I4;
	return true;

    case VDatumKind_Real:
	V_R8 (pValue) = m_iReal;
	V_VT (pValue) = VT_R8;
	return true;

    case VDatumKind_String:
	V_BSTR (pValue) = m_iString.asBSTR (bReturningUnicode);
	V_VT (pValue) = VT_BSTR;
	return true;

    default:
	return false;
    }
}
#endif


/********************************
 *****  VDatum::Comparison  *****
 ********************************/

bool VDatum::operator== (VDatum const & iOther) const
{
    switch (m_xDatumKind)
    {
    case VDatumKind_Integer:
	return iOther == m_iInteger;

    case VDatumKind_Real:
	return iOther == m_iReal;

    case VDatumKind_String:
	return iOther == m_iString;

    default:
	return m_xDatumKind == iOther.m_xDatumKind;
    }
}

bool VDatum::operator== (int iOther) const
{
    switch (m_xDatumKind)
    {
    case VDatumKind_Integer:
	return m_iInteger == iOther;

    case VDatumKind_Real:
	return m_iReal == (double)iOther;

    default:
	return false;
    }
}

bool VDatum::operator== (double iOther) const
{
    switch (m_xDatumKind)
    {
    case VDatumKind_Integer:
	return (double)m_iInteger == iOther;

    case VDatumKind_Real:
	return m_iReal == iOther;

    default:
	return false;
    }
}

bool VDatum::operator== (char const* pOther) const
{
    switch (m_xDatumKind)
    {
    case VDatumKind_String:
	return m_iString == pOther;

    default:
	return false;
    }
}


/****************************
 *****  VDatum::Update  *****
 ****************************/

VDatum& VDatum::operator= (VDatum const& iValue)
{
    clear ();

    VDatumKind xDatumKind = iValue.m_xDatumKind;

    switch (xDatumKind)
    {
    case VDatumKind_Integer:
	m_iInteger = iValue.m_iInteger;
	break;
    case VDatumKind_Real:
	m_iReal = iValue.m_iReal;
	break;
    case VDatumKind_String:
	m_iString = iValue.m_iString;
	break;
    default:
	break;
    }
    m_xDatumKind = xDatumKind;

    return *this;
}


VDatum& VDatum::operator= (char const* pValue)
{
    clear ();
    m_iString = pValue;
    m_xDatumKind = VDatumKind_String;
    return *this;
}

VDatum& VDatum::operator= (double iValue) {
    clear ();
    m_xDatumKind = VDatumKind_Real;
    m_iReal = iValue;
    return *this;
}

VDatum& VDatum::operator= (int iValue) {
    clear ();
    m_xDatumKind = VDatumKind_Integer;
    m_iInteger = iValue;
    return *this;
}


void VDatum::clear ()
{
    switch (m_xDatumKind)
    {
    case VDatumKind_String:
	m_iString.clear ();
	break;
    default:
	break;
    }
    m_xDatumKind = VDatumKind_NA;
}


/*****************************
 *****  VDatum::Parsing  *****
 *****************************/

bool VDatum::parse (char const* pString, VDatumKind xExpectedKind)
{
    char *pEnd;

    pString += strspn (pString, VAccessString::WhiteSpace);

    switch (xExpectedKind)
    {
    default:
    /*****  Look for NAs, ...  *****/
	if (strlen (pString) >= 2 && strncmp (pString, "NA", 2) == 0 && strspn (
		pString + 2, VAccessString::WhiteSpace
	    ) == strlen (pString + 2)
	)
	{
	    clear ();
	    return true;
	}

    /*****  Look for numbers, ...  *****/
	if (parse (
		pString, VDatumKind_Integer
	    ) || parse (
		pString, VDatumKind_Real
	    )
	) return true;

    /*****  otherwise, assume it's a string, ...  *****/
	*this = pString;
	return true;

    case VDatumKind_Integer:
	{
	    int iInteger = (int)strtol (pString, &pEnd, 10);
	    if (pEnd > pString && strlen (pEnd) == strspn (
		    pEnd, VAccessString::WhiteSpace
		)
	    )
	    {
		*this = iInteger;
		return true;
	    }
	}
	clear ();
	return false;

    case VDatumKind_Real:
	{
	    double iReal = strtod (pString, &pEnd);
	    if (pEnd > pString && strlen (pEnd) == strspn (
		    pEnd, VAccessString::WhiteSpace
		)
	    )
	    {
		*this = iReal;
		return true;
	    }
	}
	clear ();
	return false;

    case VDatumKind_String:
	*this = pString;
	return true;
    }
}


/*************************
 *************************
 *****               *****
 *****  VDatumArray  *****
 *****               *****
 *************************
 *************************/

/*********************************
 *****  VDatumArray::Access  *****
 *********************************/

bool VDatumArray::getElements (double* pElementArray, double iNAValue) const {
    unsigned int nElements = arraySize ();
    unsigned int xElement = 0;
    while (xElement < nElements) {
	if ((*this)[xElement++].getValue (*pElementArray))
	    pElementArray++;
	else
	    *pElementArray++ = iNAValue;
    }
    return true;
}

bool VDatumArray::getElements (int* pElementArray, int iNAValue) const {
    unsigned int nElements = arraySize ();
    unsigned int xElement = 0;
    while (xElement < nElements) {
	if ((*this)[xElement++].getValue (*pElementArray))
	    pElementArray++;
	else
	    *pElementArray++ = iNAValue;
    }
    return true;
}

/**************************************
 *****  VDatumArray::COM Support  *****
 **************************************/

#if defined(WIN32_COM_SERVER)

bool VDatumArray::getElements (BSTR* pElementArray, bool bReturningUnicode) const {
    unsigned int nElements = arraySize ();
    unsigned int xElement = 0;
    while (xElement < nElements) {
		VDatum &rDatum = ((*this)[xElement++]);
		VAccessString iString;
		bool isValid;
		switch (rDatum.kind ()) {
		case VDatumKind_Integer:
			iString << rDatum.asInteger (isValid);
		case VDatumKind_Real:
			iString << rDatum.asReal (isValid);
		case VDatumKind_String:
			iString << rDatum.asString (isValid);
		default:
			continue;
		}
		*pElementArray++ = iString.asBSTR ();
    }
    return true;
}

bool VDatumArray::getElements (VARIANT* pElementArray, bool bReturningUnicode) const {
    unsigned int nElements = arraySize ();
    unsigned int xElement = 0;
    while (xElement < nElements) {
	VariantInit (pElementArray);
	(*this)[xElement++].getValue (pElementArray, bReturningUnicode);
	pElementArray++;
    }
    return true;
}

bool VDatumArray::getArray (SAFEARRAY**ppArray, double iNAValue) const {
    double* pElementArray;

    if (!NewSafeArray (VT_R8, ppArray, reinterpret_cast<void**>(&pElementArray)))
	return false;

    getElements (pElementArray, iNAValue);

    SafeArrayUnaccessData (*ppArray);

    return true;
}

bool VDatumArray::getArray (SAFEARRAY**ppArray, int iNAValue) const {
    int* pElementArray;

    if (!NewSafeArray (VT_I4, ppArray, reinterpret_cast<void**>(&pElementArray)))
	return false;

    getElements (pElementArray, iNAValue);

    SafeArrayUnaccessData (*ppArray);

    return true;
}

bool VDatumArray::getArray (SAFEARRAY**ppArray, bool bReturningUnicode) const {
    BSTR* pElementArray;

    if (!NewSafeArray (VT_BSTR, ppArray, reinterpret_cast<void**>(&pElementArray)))
	return false;

    getElements (pElementArray, bReturningUnicode);

    SafeArrayUnaccessData (*ppArray);

    return true;
}


#endif

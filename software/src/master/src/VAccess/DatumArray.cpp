// DatumArray.cpp : Implementation of CDatumArray

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  Global  *****
 ********************/

#include "VStdLib.h"
#include "VAccess.h"

/******************
 *****  Self  *****
 ******************/

#include "DatumArray.h"

/************************
 *****  Supporting  *****
 ************************/

#include "FormatSettings.h"

/**********************
 *****  Standard  *****
 **********************/


/////////////////////////////////////////////////////////////////////////////
// CDatumArray

STDMETHODIMP CDatumArray::InterfaceSupportsErrorInfo(REFIID riid) {
    static const IID* arr[] = {
	&IID_IDatumArray
    };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
	if (InlineIsEqualGUID(*arr[i],riid))
	    return S_OK;
    }
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IDatumArray


STDMETHODIMP CDatumArray::get_FormatSettings(IFormatSettings **pVal) {
    *pVal= 0;
    return m_pFormatSettings->QueryInterface (pVal);
}

STDMETHODIMP CDatumArray::get_DimensionCount(long *pVal) {
    *pVal = m_iDatumArray.dimensionCount ();
    return S_OK;
}

STDMETHODIMP CDatumArray::get_ElementCount(long dimension, long *pVal) {
    if (dimension < 0 || (unsigned int)dimension >= m_iDatumArray.dimensionCount ()) {
	char iErrorMessage[1024];
	sprintf (
	    iErrorMessage,
	    "DatumArray::ElementCount: Dimension index %d out of range 0..%d",
	    dimension, m_iDatumArray.dimensionCount () - 1
	);
	Error (iErrorMessage, IID_IDatumArray);
	return E_INVALIDARG;
    }
    *pVal = m_iDatumArray.elementCount (dimension);
    return S_OK;
}

STDMETHODIMP CDatumArray::GetDoubleArray(SAFEARRAY**ppArray) {
    if (m_iDatumArray.getArray (ppArray, m_pFormatSettings->doubleNaN ()))
	return S_OK;

    Error ("DatumArray::GetDoubleArray: Access Failed");
    return E_FAIL;
}

STDMETHODIMP CDatumArray::GetLongArray(SAFEARRAY**ppArray) {
    if (m_iDatumArray.getArray (ppArray, m_pFormatSettings->longNaN ()))
	return S_OK;

    Error ("DatumArray::GetLongArray: Access Failed");
    return E_FAIL;
}

STDMETHODIMP CDatumArray::GetStringArray(SAFEARRAY**ppArray) {
    if (m_iDatumArray.getArray (ppArray, m_pFormatSettings->returningUnicode ()))
	return S_OK;

    Error ("DatumArray::GetStringArray: Access Failed");
    return E_FAIL;
}

STDMETHODIMP CDatumArray::GetVariantArray(SAFEARRAY**ppArray) {
    if (m_iDatumArray.getArrayOfVariants (ppArray, m_pFormatSettings->returningUnicode ()))
	return S_OK;

    Error ("DatumArray::GetVariantArray: Access Failed");
    return E_FAIL;
}

STDMETHODIMP CDatumArray::AsDoubleArray(SAFEARRAY**ppArray) {
    return GetDoubleArray (ppArray);
}

STDMETHODIMP CDatumArray::AsLongArray(SAFEARRAY**ppArray) {
    return GetLongArray (ppArray);
}

STDMETHODIMP CDatumArray::AsStringArray(SAFEARRAY**ppArray) {
    return GetStringArray (ppArray);
}

STDMETHODIMP CDatumArray::AsVariantArray(SAFEARRAY**ppArray) {
    return GetVariantArray (ppArray);
}

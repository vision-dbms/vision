// FormatSettings.cpp : Implementation of CFormatSettings

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

#include "FormatSettings.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VariantConverter.h"

/**********************
 *****  Standard  *****
 **********************/

DEFINE_V_WRAPPER(VFormatSettings,CFormatSettings,IFormatSettings);


/*************************
 *************************
 *****  Definitions  *****
 *************************
 *************************/

/////////////////////////////////////////////////////////////////////////////
// CFormatSettings

STDMETHODIMP CFormatSettings::InterfaceSupportsErrorInfo(REFIID riid) {
    static const IID* arr[] = {
	&IID_IFormatSettings
    };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
	if (InlineIsEqualGUID(*arr[i],riid))
	    return S_OK;
    }
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IFormatSettings

STDMETHODIMP CFormatSettings::get_GlobalSettings(IFormatSettings **pVal) {
    *pVal = 0;
    return VFormatSettings::GlobalSettings->QueryInterface (pVal);
}

STDMETHODIMP CFormatSettings::get_ParentSettings(IFormatSettings **pVal) {
    *pVal = 0;
    if (m_pImplementation->parent ())
	return m_pImplementation->parent()->QueryInterface (pVal);

    return S_OK;
}

STDMETHODIMP CFormatSettings::get_DoubleNaN(double *pVal) {
    *pVal = m_pImplementation->doubleNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_DoubleNaN(double newVal) {
    m_pImplementation->setDoubleNaNTo (newVal);
    return S_OK;
}

STDMETHODIMP CFormatSettings::get_SetsDoubleNaN(BOOL *pVal) {
    *pVal = m_pImplementation->setsDoubleNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_SetsDoubleNaN(BOOL newVal) {
    if (!newVal)
	m_pImplementation->unsetDoubleNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::get_LongNaN(long *pVal) {
    *pVal = m_pImplementation->longNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_LongNaN(long newVal) {
    m_pImplementation->setLongNaNTo (newVal);
    return S_OK;
}

STDMETHODIMP CFormatSettings::get_SetsLongNaN(BOOL *pVal) {
    *pVal = m_pImplementation->setsLongNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_SetsLongNaN(BOOL newVal) {
    if (!newVal)
	m_pImplementation->unsetLongNaN ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::get_ReturningUnicode(BOOL *pVal) {
    *pVal = m_pImplementation->returningUnicode ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_ReturningUnicode(BOOL newVal) {
    m_pImplementation->setReturningUnicodeTo (newVal ? true : false);
    return S_OK;
}

STDMETHODIMP CFormatSettings::get_SetsReturningUnicode(BOOL *pVal) {
    *pVal = m_pImplementation->setsReturningUnicode ();
    return S_OK;
}

STDMETHODIMP CFormatSettings::put_SetsReturningUnicode(BOOL newVal) {
    if (!newVal)
	m_pImplementation->unsetReturningUnicode ();
    return S_OK;
}

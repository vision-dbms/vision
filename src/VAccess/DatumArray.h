// DatumArray.h : Declaration of the CDatumArray

#ifndef __DATUMARRAY_H_
#define __DATUMARRAY_H_

#include "resource.h"       // main symbols

#include "VATL.h"
#include "VDatum.h"
#include "VFormatSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CDatumArray
class ATL_NO_VTABLE CDatumArray : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public VComErrorImpl<CDatumArray,&CLSID_DatumArray>,
    public ISupportErrorInfo,
    public IConnectionPointContainerImpl<CDatumArray>,
    public IDispatchImpl<IDatumArray, &IID_IDatumArray, &LIBID_VACCESSLib>
{
protected:
    VReference<VFormatSettings> m_pFormatSettings;

public:
    void createFormatSettingsWithParent (VFormatSettings* pParent) {
	m_pFormatSettings.setTo (pParent->newChild ());
    }

public:
    VDatumArray m_iDatumArray;

public:
    CDatumArray() {
    }

DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CDatumArray)
    COM_INTERFACE_ENTRY(IDatumArray)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()
    BEGIN_CONNECTION_POINT_MAP(CDatumArray)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IDatumArray
public:
    STDMETHOD(get_FormatSettings)(/*[out, retval]*/ IFormatSettings* *pVal);
    STDMETHOD(get_DimensionCount)(/*[out, retval]*/ long *pVal);
    STDMETHOD(get_ElementCount)(/*[in]*/ long dimension, /*[out, retval]*/ long *pVal);

    STDMETHOD(GetDoubleArray)(/*[out]*/ SAFEARRAY**ppArray);
    STDMETHOD(GetLongArray)(/*[out]*/ SAFEARRAY**ppArray);
    STDMETHOD(GetStringArray)(/*[out]*/ SAFEARRAY**ppArray);
    STDMETHOD(GetVariantArray)(/*[out]*/ SAFEARRAY**pArray);

    STDMETHOD(AsDoubleArray)(/*[out,retval]*/ SAFEARRAY**ppArray);
    STDMETHOD(AsLongArray)(/*[out,retval]*/ SAFEARRAY**ppArray);
    STDMETHOD(AsStringArray)(/*[out,retval]*/ SAFEARRAY**ppArray);
    STDMETHOD(AsVariantArray)(/*[out,retval]*/ SAFEARRAY**ppArray);
};


#endif //__DATUMARRAY_H_

// FormatSettings.h : Declaration of the CFormatSettings

#ifndef __FORMATSETTINGS_H_
#define __FORMATSETTINGS_H_

#include "resource.h"       // main symbols

#include "VATL.h"
#include "VFormatSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CFormatSettings
class ATL_NO_VTABLE CFormatSettings : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public VComErrorImpl<CFormatSettings, &CLSID_FormatSettings>,
    public ISupportErrorInfo,
    public IDispatchImpl<IFormatSettings, &IID_IFormatSettings, &LIBID_VACCESSLib>
{
public:
    CFormatSettings()
    {
    }

DECLARE_V_WRAPPER(VFormatSettings,CFormatSettings)

DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CFormatSettings)
    COM_INTERFACE_ENTRY(IFormatSettings)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IFormatSettings
public:
    STDMETHOD(get_GlobalSettings)(/*[out, retval]*/ IFormatSettings* *pVal);
    STDMETHOD(get_ParentSettings)(/*[out, retval]*/ IFormatSettings* *pVal);

    STDMETHOD(get_DoubleNaN)(/*[out, retval]*/ double *pVal);
    STDMETHOD(put_DoubleNaN)(/*[in]*/ double newVal);
    STDMETHOD(get_SetsDoubleNaN)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_SetsDoubleNaN)(/*[in]*/ BOOL newVal);

    STDMETHOD(get_LongNaN)(/*[out, retval]*/ long *pVal);
    STDMETHOD(put_LongNaN)(/*[in]*/ long newVal);
    STDMETHOD(get_SetsLongNaN)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_SetsLongNaN)(/*[in]*/ BOOL newVal);

    STDMETHOD(get_ReturningUnicode)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_ReturningUnicode)(/*[in]*/ BOOL newVal);
    STDMETHOD(get_SetsReturningUnicode)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_SetsReturningUnicode)(/*[in]*/ BOOL newVal);
};

#endif //__FORMATSETTINGS_H_

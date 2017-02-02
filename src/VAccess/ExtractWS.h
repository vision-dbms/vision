// ExtractWS.h : Declaration of the CExtractWS

#ifndef __EXTRACTWS_H_
#define __EXTRACTWS_H_

#include "resource.h"       // main symbols
#include "VAccessCP.h"

#include "VATL.h"
#include "VExtractWS.h"

/////////////////////////////////////////////////////////////////////////////
// CExtractWS
class ATL_NO_VTABLE CExtractWS : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public VComErrorImpl<CExtractWS, &CLSID_ExtractWS>,
    public ISupportErrorInfo,
    public IConnectionPointContainerImpl<CExtractWS>,
    public IDispatchImpl<IExtractWS, &IID_IExtractWS, &LIBID_VACCESSLib>,
    public CProxy_IExtractWSEvents< CExtractWS >
{
protected:
    HRESULT InvalidValueError (char const* pWhere, char const* pWhat, unsigned int xValue);
    HRESULT InvalidValueError (char const* pWhere, ExtractOrientation xValue);
    HRESULT InvalidValueError (char const* pWhere, ItemType xValue);

public:
    CExtractWS()
    {
    }

DECLARE_V_WRAPPER(VExtractWS,CExtractWS)

DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CExtractWS)
    COM_INTERFACE_ENTRY(IExtractWS)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CExtractWS)
    CONNECTION_POINT_ENTRY(DIID__IExtractWSEvents)
END_CONNECTION_POINT_MAP()

// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IExtractWS
public:
	STDMETHOD(get_EntityListExpression)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_EntityListExpression)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_EntityListString)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_EntityListString)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_DateListExpression)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_DateListExpression)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_DateListString)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_DateListString)(/*[in]*/ BSTR newVal);
    STDMETHOD(get_ItemListType)(/*[out, retval]*/ ExtractListType *pVal);
    STDMETHOD(get_EntityListType)(/*[out, retval]*/ ExtractListType *pVal);
    STDMETHOD(get_DateListType)(/*[out, retval]*/ ExtractListType *pVal);
    STDMETHOD(get_ItemType)(/*[out, retval]*/ ItemType *pVal);
    STDMETHOD(put_ItemType)(/*[in]*/ ItemType newVal);
    STDMETHOD(get_Item)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Item)(/*[in]*/ BSTR newVal);
    STDMETHOD(Copy)(/*[out,retval]*/IExtractWS** ppExtractWS);
    STDMETHOD(get_Connection)(/*[out, retval]*/ IConnection** ppConnection);
    STDMETHOD(get_IsConnected)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_FormatSettings)(/*[out,retval]*/ IFormatSettings** ppFormatSettings);

    STDMETHOD(get_Orientation)(/*[out, retval]*/ ExtractOrientation *pVal);
    STDMETHOD(put_Orientation)(/*[in]*/ ExtractOrientation newVal);
    STDMETHOD(SetOrientationTo)(/*[in]*/ ExtractOrientation orientation);
    STDMETHOD(SetOrientationToEI)();
    STDMETHOD(SetOrientationToET)();
    STDMETHOD(SetOrientationToIE)();
    STDMETHOD(SetOrientationToIT)();
    STDMETHOD(SetOrientationToTE)();
    STDMETHOD(SetOrientationToTI)();

    STDMETHOD(get_Delimiter)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Delimiter)(/*[in]*/ BSTR newVal);
    STDMETHOD(SetDelimiterTo)(/*[in]*/ BSTR delimiter);

    STDMETHOD(get_ColumnLabelsOn)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_ColumnLabelsOn)(/*[in]*/ BOOL newVal);
    STDMETHOD(SetColumnLabelsOn)();
    STDMETHOD(SetColumnLabelsOff)();

    STDMETHOD(get_RowLabelsOn)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_RowLabelsOn)(/*[in]*/ BOOL newVal);
    STDMETHOD(SetRowLabelsOn)();
    STDMETHOD(SetRowLabelsOff)();

    STDMETHOD(get_ScalarLabelOn)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(put_ScalarLabelOn)(/*[in]*/ BOOL newVal);
    STDMETHOD(SetScalarLabelOn)();
    STDMETHOD(SetScalarLabelOff)();

    STDMETHOD(get_Date)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Date)(/*[in]*/ BSTR newVal);
    STDMETHOD(SetDateTo)(/*[in]*/ BSTR date);

    STDMETHOD(get_Currency)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Currency)(/*[in]*/ BSTR newVal);
    STDMETHOD(SetCurrencyTo)(/*[in]*/ BSTR currency);

    STDMETHOD(get_EntityType)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_EntityType)(/*[in]*/ BSTR newVal);
    STDMETHOD(SetEntityTypeTo)(/*[in]*/ BSTR entityType);

    STDMETHOD(get_Entity)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Entity)(/*[in]*/ BSTR newVal);
    STDMETHOD(SetEntityTo)(/*[in]*/ BSTR entity);

//-----------------------
    STDMETHOD(ClearDateList)();
    STDMETHOD(AddDate)(/*[in]*/ BSTR date);
    STDMETHOD(SetDateListExpressionTo)(/*[in]*/ BSTR dateListExpression);
    STDMETHOD(SetDateListTo)(/*[in]*/ BSTR dateList);

    STDMETHOD(ClearItemList)();
    STDMETHOD(AddTypedItem)(/*[in]*/ BSTR item, /*[in]*/ ItemType itemType);
    STDMETHOD(AddItem)(/*[in]*/ BSTR item);
    STDMETHOD(SetTypedItemTo)(/*[in]*/ BSTR item, /*[in]*/ ItemType itemType);
    STDMETHOD(SetItemTo)(/*[in]*/ BSTR item);

    STDMETHOD(ClearEntityList)();
    STDMETHOD(AddEntity)(/*[in]*/ BSTR entity);
    STDMETHOD(SetEntityListExpressionTo)(/*[in]*/ BSTR entityListExpression);
    STDMETHOD(SetEntityListTo)(/*[in]*/ BSTR entityList);
//-----------------------

    STDMETHOD(Run)(/*[out,retval]*/ IDatumArray** ppResultDatumArray);
};

#endif //__EXTRACTWS_H_

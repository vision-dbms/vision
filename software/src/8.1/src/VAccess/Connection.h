// Connection.h : Declaration of the CConnection

#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include "resource.h"       // main symbols
#include "VAccessCP.h"

#include "VATL.h"
#include "VConnection.h"

/////////////////////////////////////////////////////////////////////////////
// CConnection
class ATL_NO_VTABLE CConnection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnection, &CLSID_Connection>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CConnection>,
	public IDispatchImpl<IConnection, &IID_IConnection, &LIBID_VACCESSLib>,
	public CProxy_IConnectionEvents< CConnection >
{
public:
	CConnection()
	{
	}

DECLARE_V_WRAPPER_WITH_FACTORY(VConnection,CConnection)

DECLARE_REGISTRY_RESOURCEID(IDR_CONNECTION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CConnection)
    COM_INTERFACE_ENTRY(IConnection)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CConnection)
    CONNECTION_POINT_ENTRY(DIID__IConnectionEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IConnection
public:
    STDMETHOD(get_IsConnected)(/*[out, retval]*/ BOOL *pVal);
    STDMETHOD(get_FormatSettings)(/*[out, retval]*/ IFormatSettings**pVal);

    STDMETHOD(Connect)(/*[in]*/BSTR hostname, /*[in]*/ long port);
    STDMETHOD(Login)(/*[in]*/ BSTR hostname, /*[in]*/ BSTR username, /*[in]*/ BSTR password, /*[in,optional]*/ VARIANT command);
    STDMETHOD(Rexec)(/*[in]*/ BSTR hostname, /*[in]*/ BSTR username, /*[in]*/ BSTR password, /*[in,optional]*/ VARIANT command);
    STDMETHOD(Disconnect)();

    STDMETHOD(Submit)(/*[in]*/ BSTR request, /*[in, optional]*/ VARIANT date, /*[in, optional]*/ VARIANT currency, /*[in, optional]*/ VARIANT replyBufferInitialSize, /*[in, optional]*/ VARIANT replyBufferSizeIncrement, /*[out, retval]*/ BSTR *Reply);
    STDMETHOD(Execute)(/*[in]*/ BSTR request, /*[in, optional]*/ VARIANT date, /*[in, optional]*/ VARIANT currency);
    STDMETHOD(Include)(/*[in]*/ BSTR remoteFileName);

    STDMETHOD(GetDouble)(/*[out]*/ double* result, /*[in]*/ BSTR entityType, /*[in]*/ BSTR entity, /*[in]*/ BSTR item, /*[in,optional]*/ VARIANT date, /*[in,optional]*/ VARIANT currency, /*[out,retval]*/ BOOL* pSuccessful);
    STDMETHOD(GetLong)(/*[out]*/ long* result, /*[in]*/ BSTR entityType, /*[in]*/ BSTR entity, /*[in]*/ BSTR item, /*[in,optional]*/VARIANT date, /*[in,optional]*/ VARIANT currency, /*[out,retval]*/ BOOL* pSuccessful);
    STDMETHOD(GetString)(/*[out]*/ BSTR* result, /*[in]*/ BSTR entityType, /*[in]*/ BSTR entity, /*[in]*/ BSTR item, /*[in,optional]*/ VARIANT date, /*[in,optional]*/ VARIANT currency, /*[out,retval]*/ BOOL* pSuccessful);
    STDMETHOD(GetValue)(/*[out]*/ VARIANT* result, /*[in]*/ BSTR entityType, /*[in]*/ BSTR entity, /*[in]*/ BSTR item, /*[in,optional]*/ VARIANT date, /*[in, optional]*/ VARIANT currency, /*[out,retval]*/ BOOL* pSuccessful);

    STDMETHOD(NewExtractWS)(/*[out, retval]*/ IExtractWS** ppExtractWS);
    
    STDMETHOD(get_SessionNames)(/*[out, retval]*/ BSTR *sessionNames);
};


#endif //__CONNECTION_H_

#ifndef VATL_Interface
#define VATL_Interface

/**************************
 *****  Declarations  *****
 **************************/

#include "VReference.h"

/*************************
 *****  Definitions  *****
 *************************/

#if defined(WIN32_COM_SERVER)

//*****************************************************************************
#define DECLARE_V_WRAPPER(VType,CType) \
public:\
    virtual VReference<VType>& rImplementation () {\
	return m_pImplementation;\
    }\
    VComWrapperImplementationReference<VType> m_pImplementation;

#define DEFINE_V_WRAPPER(VType,CType,IType) \
HRESULT VType::QueryInterface (IType** ppInterface) {\
    return VComWrapper<VType, CComObject<CType> >::GetOrCreateInterface (\
	this, __uuidof(IType), reinterpret_cast<void**>(ppInterface)\
    );\
}

//*****************************************************************************
#define DECLARE_V_WRAPPER_WITH_FACTORY(VType,CType)\
    DECLARE_V_WRAPPER(VType,CType)\
public:\
    typedef CComCreator<VComWrapper<VType, VComPolyWrapper<VType, CType> > > _CreatorClass;


#define DEFINE_V_WRAPPER_WITH_FACTORY(VType,CType,IType) \
HRESULT VType::QueryInterface (IType** ppInterface) {\
    return VComWrapper<VType, VComPolyWrapper<VType, CType> >::GetOrCreateInterface (\
	this, __uuidof(IType), reinterpret_cast<void**>(ppInterface)\
    );\
}

//*****************************************************************************

template <class VType> class VComWrapperImplementationReference : public VReference<VType> {
public:
    ~VComWrapperImplementationReference () {
	if (m_pReferent)
	    m_pReferent->clearComWrapper ();
    }
};

//*****************************************************************************

template <class VType, class CType> class VComPolyWrapper : public CComPolyObject<CType> {
public:
    VComPolyWrapper (void* pv) : CComPolyObject<CType> (pv) {
	m_contained.m_pImplementation.setTo (new VType ());
    }

public:
    VReference<VType>& rImplementation () {
	return m_contained.m_pImplementation;
    }
};

//*****************************************************************************

template <class VType, class Base> class VComWrapper : public Base {
public:
    VComWrapper (VType* pImplementation) : Base (NULL) {
	rImplementation().setTo (pImplementation);
    }
    VComWrapper (void* pController) : Base (pController) {
    }

    HRESULT FinalConstruct () {
	if (rImplementation().isNil ())
	    return E_POINTER;
	rImplementation()->setComWrapperTo (this);
	return S_OK;
    }

    static HRESULT GetOrCreateInterface (VType* pImplementation, REFIID iid, void ** ppvObject) {
	*ppvObject = 0;

	VComWrapper<VType, Base>* pWrapper = reinterpret_cast<VComWrapper<VType, Base>*>(
	    pImplementation->comWrapper ()
	);
	if (pWrapper == 0) {
	    HRESULT hRes = E_OUTOFMEMORY;
	    ATLTRY((pWrapper = new VComWrapper<VType,Base> (pImplementation)))
	    if (pWrapper != NULL) {
		pWrapper->SetVoid(NULL);
		pWrapper->InternalFinalConstructAddRef();
		hRes = pWrapper->FinalConstruct();
		pWrapper->InternalFinalConstructRelease();
		if (hRes != S_OK) {
		    delete pWrapper;
		    pWrapper = NULL;
		}
	    }
	    if (hRes != S_OK)
		return hRes;
	}
	return pWrapper->QueryInterface (iid, ppvObject);
    }
};

//*****************************************************************************

template <class T, const CLSID* pclsid = &CLSID_NULL> class VComErrorImpl {
public:
    static const CLSID& WINAPI GetObjectCLSID() {return *pclsid;}
    static HRESULT WINAPI Error(LPCOLESTR lpszDesc, const IID& iid = GUID_NULL, HRESULT hRes = 0) {
	return AtlReportError(GetObjectCLSID(), lpszDesc, iid, hRes);
    }
    static HRESULT WINAPI Error(
	LPCOLESTR lpszDesc, DWORD dwHelpID, LPCOLESTR lpszHelpFile, const IID& iid = GUID_NULL, HRESULT hRes = 0
    )
    {
	return AtlReportError(GetObjectCLSID(), lpszDesc, dwHelpID, lpszHelpFile, iid, hRes);
    }
    static HRESULT WINAPI Error(
	UINT nID, const IID& iid = GUID_NULL, HRESULT hRes = 0, HINSTANCE hInst = _Module.GetResourceInstance()
    )
    {
	return AtlReportError(GetObjectCLSID(), nID, iid, hRes, hInst);
    }
    static HRESULT WINAPI Error(
	UINT nID, DWORD dwHelpID, LPCOLESTR lpszHelpFile, const IID& iid = GUID_NULL,
	    HRESULT hRes = 0, HINSTANCE hInst = _Module.GetResourceInstance()
    )
    {
	return AtlReportError(GetObjectCLSID(), nID, dwHelpID, lpszHelpFile, iid, hRes, hInst);
    }
#ifndef OLE2ANSI
    static HRESULT WINAPI Error(LPCSTR lpszDesc, const IID& iid = GUID_NULL, HRESULT hRes = 0) {
	return AtlReportError(GetObjectCLSID(), lpszDesc, iid, hRes);
    }
    static HRESULT WINAPI Error(
	LPCSTR lpszDesc, DWORD dwHelpID, LPCSTR lpszHelpFile, const IID& iid = GUID_NULL, HRESULT hRes = 0
    )
    {
	return AtlReportError(GetObjectCLSID(), lpszDesc, dwHelpID, lpszHelpFile, iid, hRes);
    }
#endif
};

//*****************************************************************************

#endif


#endif

// ExtractWS.cpp : Implementation of CExtractWS

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

#include "ExtractWS.h"

/************************
 *****  Supporting  *****
 ************************/

#include "DatumArray.h"

#include "VariantConverter.h"

/**********************
 *****  Standard  *****
 **********************/

DEFINE_V_WRAPPER(VExtractWS,CExtractWS,IExtractWS);


/*************************
 *************************
 *****  Definitions  *****
 *************************
 *************************/

static unsigned int Translated (ExtractOrientation xValue, VExtractOrientation *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case ExtractOrientation_EI:
	*pxTranslatedValue = VExtractOrientation_EI;
	break;
    case ExtractOrientation_ET:
	*pxTranslatedValue = VExtractOrientation_ET;
	break;
    case ExtractOrientation_IE:
	*pxTranslatedValue = VExtractOrientation_IE;
	break;
    case ExtractOrientation_IT:
	*pxTranslatedValue = VExtractOrientation_IT;
	break;
    case ExtractOrientation_TE:
	*pxTranslatedValue = VExtractOrientation_TE;
	break;
    case ExtractOrientation_TI:
	*pxTranslatedValue = VExtractOrientation_TI;
	break;
    default:
	fTranslatedSuccessfully = FALSE;
	break;
    }

    return fTranslatedSuccessfully;
}

static unsigned int Translated (VExtractOrientation xValue, ExtractOrientation *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case VExtractOrientation_EI:
	*pxTranslatedValue = ExtractOrientation_EI;
	break;
    case VExtractOrientation_ET:
	*pxTranslatedValue = ExtractOrientation_ET;
	break;
    case VExtractOrientation_IE:
	*pxTranslatedValue = ExtractOrientation_IE;
	break;
    case VExtractOrientation_IT:
	*pxTranslatedValue = ExtractOrientation_IT;
	break;
    case VExtractOrientation_TE:
	*pxTranslatedValue = ExtractOrientation_TE;
	break;
    case VExtractOrientation_TI:
	*pxTranslatedValue = ExtractOrientation_TI;
	break;
    default:
    	fTranslatedSuccessfully = FALSE;
	break;
    }
    return fTranslatedSuccessfully;
}


static unsigned int Translated (ExtractListType xValue, VExtractListType *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case ExtractListType_Unspecified:
	*pxTranslatedValue = VExtractListType_Unspecified;
	break;
    case ExtractListType_Array:
	*pxTranslatedValue = VExtractListType_Array;
	break;
    case ExtractListType_String:
	*pxTranslatedValue = VExtractListType_String;
	break;
    case ExtractListType_Expression:
	*pxTranslatedValue = VExtractListType_Expression;
	break;
    default:
	fTranslatedSuccessfully = FALSE;
	break;
    }

    return fTranslatedSuccessfully;
}

static unsigned int Translated (VExtractListType xValue, ExtractListType *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case VExtractListType_Unspecified:
	*pxTranslatedValue = ExtractListType_Unspecified;
	break;
    case VExtractListType_Array:
	*pxTranslatedValue = ExtractListType_Array;
	break;
    case VExtractListType_String:
	*pxTranslatedValue = ExtractListType_String;
	break;
    case VExtractListType_Expression:
	*pxTranslatedValue = ExtractListType_Expression;
	break;
    default:
    	fTranslatedSuccessfully = FALSE;
	break;
    }
    return fTranslatedSuccessfully;
}


static unsigned int Translated (ItemType xValue, VDatumKind *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case ItemType_Unspecified:
	*pxTranslatedValue = VDatumKind_NA;
	break;
    case ItemType_Integer:
	*pxTranslatedValue = VDatumKind_Integer;
	break;
    case ItemType_Real:
	*pxTranslatedValue = VDatumKind_Real;
	break;
    case ItemType_String:
	*pxTranslatedValue = VDatumKind_String;
	break;
    default:
	fTranslatedSuccessfully = FALSE;
	break;
    }

    return fTranslatedSuccessfully;
}

static unsigned int Translated (VDatumKind xValue, ItemType *pxTranslatedValue) {
    unsigned int fTranslatedSuccessfully = TRUE;
    switch (xValue) {
    case VDatumKind_NA:
	*pxTranslatedValue = ItemType_Unspecified;
	break;
    case VDatumKind_Integer:
	*pxTranslatedValue = ItemType_Integer;
	break;
    case VDatumKind_Real:
	*pxTranslatedValue = ItemType_Real;
	break;
    case VDatumKind_String:
	*pxTranslatedValue = ItemType_String;
	break;
    default:
	fTranslatedSuccessfully = FALSE;
	break;
    }

    return fTranslatedSuccessfully;
}


/////////////////////////////////////////////////////////////////////////////
// CExtractWS

HRESULT CExtractWS::InvalidValueError (char const* pWhere, char const* pWhat, unsigned int xValue) {
    char iErrorMessage[1024];
    sprintf (
	iErrorMessage, "ExtractWS::%s: Invalid %s %u", pWhere, pWhat, xValue
    );
    Error (iErrorMessage, IID_IExtractWS);

    return E_INVALIDARG;
}

HRESULT CExtractWS::InvalidValueError (char const* pWhere, ExtractOrientation xValue) {
    return InvalidValueError (pWhere, "Extract Orientation", (unsigned int)xValue);
}

HRESULT CExtractWS::InvalidValueError (char const* pWhere, ItemType xValue) {
    return InvalidValueError (pWhere, "Item Type", (unsigned int)xValue);
}

STDMETHODIMP CExtractWS::InterfaceSupportsErrorInfo(REFIID riid) {
    static const IID* arr[] = {
	&IID_IExtractWS
    };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
	if (InlineIsEqualGUID(*arr[i],riid))
	    return S_OK;
    }
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IExtractWS
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::Copy(IExtractWS **ppExtractWS) {
    VReference<VExtractWS> pNewExtractWS (new VExtractWS (*m_pImplementation));

    *ppExtractWS = 0;
    return pNewExtractWS->QueryInterface (ppExtractWS);
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CExtractWS::get_FormatSettings(IFormatSettings **ppFormatSettings) {
    *ppFormatSettings = 0;
    return m_pImplementation->formatSettings()->QueryInterface (ppFormatSettings);
}

HRESULT CExtractWS::get_Connection(IConnection **ppConnection) {
    *ppConnection = 0;
    return m_pImplementation->connection()->QueryInterface (ppConnection);
}

STDMETHODIMP CExtractWS::get_IsConnected(BOOL *pVal) {
    *pVal = m_pImplementation->isConnected ();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Orientation(ExtractOrientation *pVal) {
    Translated (m_pImplementation->orientation (), pVal);
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Orientation(ExtractOrientation newVal) {
    VExtractOrientation xTranslatedOrientation;
    if (Translated (newVal, &xTranslatedOrientation)) {
	m_pImplementation->setOrientationTo (xTranslatedOrientation);
	return S_OK;
    }
    return InvalidValueError ("SetOrientationTo (ExtractOrientation)", newVal);
}

STDMETHODIMP CExtractWS::SetOrientationTo (ExtractOrientation newVal) {
    return put_Orientation (newVal);
}

STDMETHODIMP CExtractWS::SetOrientationToEI() {
    m_pImplementation->setOrientationToEI ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetOrientationToET() {
    m_pImplementation->setOrientationToET ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetOrientationToIE() {
    m_pImplementation->setOrientationToIE ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetOrientationToIT() {
    m_pImplementation->setOrientationToIT ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetOrientationToTE() {\
    m_pImplementation->setOrientationToTE ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetOrientationToTI() {
    m_pImplementation->setOrientationToTI ();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_ColumnLabelsOn(BOOL *pVal) {
    *pVal = m_pImplementation->showColumnLabels ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_ColumnLabelsOn(BOOL newVal) {
    if (newVal)
	m_pImplementation->setColumnLabelsOn ();
    else
	m_pImplementation->setColumnLabelsOff ();

    return S_OK;
}

STDMETHODIMP CExtractWS::SetColumnLabelsOn() {
    m_pImplementation->setColumnLabelsOn ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetColumnLabelsOff() {
    m_pImplementation->setColumnLabelsOff ();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_RowLabelsOn(BOOL *pVal) {
    *pVal = m_pImplementation->showRowLabels ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_RowLabelsOn(BOOL newVal) {
    if (newVal)
	m_pImplementation->setRowLabelsOn ();
    else
	m_pImplementation->setRowLabelsOff ();

    return S_OK;
}

STDMETHODIMP CExtractWS::SetRowLabelsOn() {
    m_pImplementation->setRowLabelsOn ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetRowLabelsOff() {
    m_pImplementation->setRowLabelsOff ();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_ScalarLabelOn(BOOL *pVal) {
    *pVal = m_pImplementation->showScalarLabel ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_ScalarLabelOn(BOOL newVal) {
    if (newVal)
	m_pImplementation->setScalarLabelOn ();
    else
	m_pImplementation->setScalarLabelOff ();

    return S_OK;
}

STDMETHODIMP CExtractWS::SetScalarLabelOn() {
    m_pImplementation->setScalarLabelOn ();
    return S_OK;
}

STDMETHODIMP CExtractWS::SetScalarLabelOff() {
    m_pImplementation->setScalarLabelOff ();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Delimiter(BSTR *pVal) {
    *pVal = m_pImplementation->delimiter().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Delimiter(BSTR newVal) {
    VAccessString cDelimiter (newVal);
    if (cDelimiter.length () > 0) {
	m_pImplementation->setDelimiterTo (cDelimiter);
	return S_OK;
    }

    Error ("ExtractWS::put_Delimiter: Delimiter Not Specified");
    return E_INVALIDARG;
}

STDMETHODIMP CExtractWS::SetDelimiterTo (BSTR newVal) {
    return put_Delimiter (newVal);
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Currency(BSTR *pVal) {
    *pVal = m_pImplementation->currency().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Currency(BSTR newVal) {
    VAccessString cCurrency (newVal);
    m_pImplementation->setCurrencyTo (cCurrency);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetCurrencyTo (BSTR newVal) {
    return put_Currency (newVal);
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Date(BSTR *pVal) {
    *pVal = m_pImplementation->date().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Date(BSTR newVal) {
    VAccessString cDate (newVal);
    m_pImplementation->setDateTo (cDate);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetDateTo (BSTR newVal) {
    return put_Date (newVal);
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_EntityType(BSTR *pVal) {
    *pVal = m_pImplementation->entityType().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_EntityType(BSTR newVal) {
    VAccessString cEntityType (newVal);
    m_pImplementation->setEntityTypeTo (cEntityType);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetEntityTypeTo (BSTR newVal) {
    return put_EntityType (newVal);
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Entity(BSTR *pVal) {
    *pVal = m_pImplementation->entity ().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Entity(BSTR newVal) {
    VAccessString cEntity (newVal);
    m_pImplementation->setEntityTo (cEntity);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetEntityTo (BSTR newVal) {
    return put_Entity (newVal);
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_Item (BSTR *pVal) {
    *pVal = m_pImplementation->itemName().asBSTR ();
    return S_OK;
}

STDMETHODIMP CExtractWS::put_Item (BSTR item) {
    VAccessString cItem (item);
    m_pImplementation->setItemTo (cItem, m_pImplementation->itemKind ());
    return S_OK;
}

STDMETHODIMP CExtractWS::get_ItemType (ItemType *pVal) {
    if (!Translated (m_pImplementation->itemKind(), pVal))
	*pVal = ItemType_Unspecified;
    return S_OK;
}

STDMETHODIMP CExtractWS::put_ItemType (ItemType itemType) {
    VDatumKind xTranslatedItemType;

    if (Translated (itemType, &xTranslatedItemType)) {
	m_pImplementation->setItemKindTo (xTranslatedItemType);
	return S_OK;
    }

    return InvalidValueError ("SetTypeItemTo (item, itemType)", itemType);
}

STDMETHODIMP CExtractWS::SetItemTo (BSTR item) {
    VAccessString cItem (item);
    m_pImplementation->setItemTo (cItem);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetTypedItemTo (BSTR item, ItemType itemType) {
    VDatumKind xTranslatedItemType;

    if (Translated (itemType, &xTranslatedItemType)) {
	VAccessString cItem (item);
	m_pImplementation->setItemTo (cItem, xTranslatedItemType);
	return S_OK;
    }

    return InvalidValueError ("SetTypeItemTo (item, itemType)", itemType);
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_EntityListString(BSTR *pVal) {
    // TODO: Add your implementation code here

    return S_OK;
}

STDMETHODIMP CExtractWS::put_EntityListString(BSTR newVal) {
    VAccessString cEntityList (newVal);
    m_pImplementation->setEntityListTo (cEntityList);
    return S_OK;
}

STDMETHODIMP CExtractWS::get_EntityListExpression(BSTR *pVal) {
    // TODO: Add your implementation code here

    return S_OK;
}

STDMETHODIMP CExtractWS::put_EntityListExpression(BSTR newVal) {
    VAccessString cEntityListExpression (newVal);
    m_pImplementation->setEntityListExpressionTo (cEntityListExpression);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetEntityListTo (BSTR entityList) {
    return put_EntityListString (entityList);
}

STDMETHODIMP CExtractWS::SetEntityListExpressionTo(BSTR entityListExpression) {
    return put_EntityListExpression (entityListExpression);
}

STDMETHODIMP CExtractWS::AddEntity(BSTR entity) {
    VAccessString cEntity (entity);
    m_pImplementation->addEntity (cEntity);
    return S_OK;
}

STDMETHODIMP CExtractWS::ClearEntityList() {
    m_pImplementation->clearEntityList ();
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::AddItem(BSTR item) {
    VAccessString cItem (item);
    m_pImplementation->addItem (cItem);
    return S_OK;
}

STDMETHODIMP CExtractWS::AddTypedItem(BSTR item, ItemType itemType) {
    VDatumKind xTranslatedItemType;

    if (Translated (itemType, &xTranslatedItemType)) {
	VAccessString cItem (item);

	m_pImplementation->addItem (cItem, xTranslatedItemType);
	return S_OK;
    }

    return InvalidValueError ("AddTypedItem (item, itemType)", itemType);
}

STDMETHODIMP CExtractWS::ClearItemList() {
    m_pImplementation->clearItemList ();
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::get_DateListString (BSTR *pVal) {
    // TODO: Add your implementation code here

    return S_OK;
}

STDMETHODIMP CExtractWS::put_DateListString(BSTR newVal) {
    VAccessString cDateList (newVal);
    m_pImplementation->setDateListTo (cDateList);
    return S_OK;
}

STDMETHODIMP CExtractWS::get_DateListExpression(BSTR *pVal) {
    // TODO: Add your implementation code here

    return S_OK;
}

STDMETHODIMP CExtractWS::put_DateListExpression(BSTR newVal) {
    VAccessString cDateListExpression (newVal);
    m_pImplementation->setDateListExpressionTo (cDateListExpression);
    return S_OK;
}

STDMETHODIMP CExtractWS::SetDateListTo(BSTR dateList) {
    return put_DateListString (dateList);
}

STDMETHODIMP CExtractWS::SetDateListExpressionTo(BSTR dateListExpression) {
    return put_DateListExpression (dateListExpression);
}

STDMETHODIMP CExtractWS::AddDate(BSTR date) {
    VAccessString cDate (date);
    m_pImplementation->addDate (cDate);
    return S_OK;
}

STDMETHODIMP CExtractWS::ClearDateList () {
    m_pImplementation->clearDateList ();
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CExtractWS::Run(IDatumArray **ppResultDatumArray) {
    CComObject<CDatumArray>* pDatumArray;

    HRESULT hr = CComObject<CDatumArray>::CreateInstance (&pDatumArray);
    if (FAILED (hr)) {
	Error ("ExtractWS::run: Result Allocation Failed.");
	return hr;
    }

    pDatumArray->createFormatSettingsWithParent (m_pImplementation->formatSettings ());

    if (!m_pImplementation->run (pDatumArray->m_iDatumArray)) {
	Error ("ExtractWS::run: Result Evaluation Failed.");
	delete pDatumArray;

	return E_FAIL;
    }

    hr = pDatumArray->QueryInterface (ppResultDatumArray);
    if (FAILED (hr)) {
	Error ("ExtractWS::run: Result Return Failed.");
	delete pDatumArray;

	return hr;
    }

    return hr;
}

STDMETHODIMP CExtractWS::get_DateListType (ExtractListType *pVal) {
    Translated (m_pImplementation->dateListType (), pVal);
    return S_OK;
}

STDMETHODIMP CExtractWS::get_EntityListType (ExtractListType *pVal) {
    Translated (m_pImplementation->entityListType (), pVal);
    return S_OK;
}

STDMETHODIMP CExtractWS::get_ItemListType (ExtractListType *pVal) {
    Translated (m_pImplementation->itemListType (), pVal);
    return S_OK;
}

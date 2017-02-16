// Connection.cpp : Implementation of CConnection

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

#include "Connection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VExtractWS.h"
#include "VFormatSettings.h"
#include "VariantConverter.h"

/**********************
 *****  Standard  *****
 **********************/

DEFINE_V_WRAPPER_WITH_FACTORY(VConnection,CConnection,IConnection);


/*************************
 *************************
 *****  Definitions  *****
 *************************
 *************************/

/////////////////////////////////////////////////////////////////////////////
// CConnection

STDMETHODIMP CConnection::InterfaceSupportsErrorInfo(REFIID riid) {
    static const IID* arr[] = {
	&IID_IConnection
    };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++) {
	if (InlineIsEqualGUID(*arr[i],riid))
	    return S_OK;
    }
    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// IConnection

STDMETHODIMP CConnection::get_IsConnected(BOOL *pVal) {
    *pVal = m_pImplementation->isConnected ();
    return S_OK;
}

STDMETHODIMP CConnection::get_FormatSettings(IFormatSettings **pVal) {
    *pVal= 0;
    return m_pImplementation->formatSettings()->QueryInterface (pVal);
}

STDMETHODIMP CConnection::Connect(BSTR hostname, long port) {
    VAccessString cHostname (hostname);

    if (m_pImplementation->connect (cHostname, (unsigned short)port))
	return S_OK;

    Error ("Connection::Connect: Failed", IID_IConnection);
    return E_FAIL;
}

STDMETHODIMP CConnection::Login(BSTR hostname, BSTR username, BSTR password, VARIANT command) {
    VAccessString cHostname (hostname);
    VAccessString cUsername (username);
    VAccessString cPassword (password);
    CVariantConverter cCommand (command);

    VAccessString sErrorMessage;
    VAccessString sCommand (cCommand.asStringElse ("/vision/bin/batchvision"));
    if (m_pImplementation->login (
	    sErrorMessage,
	    cHostname,
            cUsername,
            cPassword,
	    sCommand
	)
    ) return S_OK;

    // TODO: Check if the conversion to char* is necessary. Is there some reason for the 1280 below?
    char iErrorMessage[1280];
    sprintf (iErrorMessage, "Connection::Login: %s", sErrorMessage.content());
    Error (iErrorMessage, IID_IConnection);
    return E_FAIL;
}

STDMETHODIMP CConnection::Disconnect() {
    m_pImplementation->disconnect ();
    return S_OK;
}


STDMETHODIMP CConnection::Submit(
    BSTR	request,
    VARIANT	date,
    VARIANT	currency,
    VARIANT	replyBufferInitialSize,
    VARIANT	replyBufferSizeIncrement,
    BSTR*	reply
) {
//  Obtain optional argument values...
    CVariantConverter cDate		(date);
    CVariantConverter cCurrency		(currency);

    VAccessString cRequest (request);
    VAccessString iReplyBuffer;
    if (m_pImplementation->submit (
	    iReplyBuffer,
	    cRequest,
	    cDate		.asStringElse	(0),
	    cCurrency		.asStringElse	(0)
	)
    ) {
	*reply = iReplyBuffer.asBSTR ();
	return S_OK;
    }

    Error ("Connection::Submit: Failed", IID_IConnection);
    return E_FAIL;
}

STDMETHODIMP CConnection::Execute(BSTR request, VARIANT date, VARIANT currency) {
    VAccessString	      cRequest	(request);
    CVariantConverter cDate	(date);
    CVariantConverter cCurrency	(currency);

    if (m_pImplementation->execute (cRequest, cDate.asStringElse (0), cCurrency.asStringElse (0)))
	return S_OK;

    Error ("Connection::Execute: Failed", IID_IConnection);
    return E_FAIL;
}

STDMETHODIMP CConnection::Include(BSTR remoteFileName) {
    VAccessString cRemoteFileName (remoteFileName);
    if (m_pImplementation->include (cRemoteFileName))
	return S_OK;

    Error ("Connection::Include: Failed", IID_IConnection);
    return E_FAIL;
}


STDMETHODIMP CConnection::GetDouble(double *result, BSTR entityType, BSTR entity, BSTR item, VARIANT date, VARIANT currency, BOOL* pSuccessful) {
    VAccessString		cEntityType	(entityType);
    VAccessString		cEntity		(entity);
    VAccessString		cItem		(item);
    CVariantConverter	cDate		(date);
    CVariantConverter	cCurrency	(currency);

    double iResult;
    *pSuccessful = m_pImplementation->getValue (
	iResult, cEntityType, cEntity, cItem, cDate.asStringElse (0), cCurrency.asStringElse (0)
    );
    *result = *pSuccessful ? iResult : m_pImplementation->formatSettings()->doubleNaN();
    return S_OK;
}

STDMETHODIMP CConnection::GetLong(long *result, BSTR entityType, BSTR entity, BSTR item, VARIANT date, VARIANT currency, BOOL* pSuccessful) {
    VAccessString		cEntityType	(entityType);
    VAccessString		cEntity		(entity);
    VAccessString		cItem		(item);
    CVariantConverter	cDate		(date);
    CVariantConverter	cCurrency	(currency);

    int iResult;
    *pSuccessful = m_pImplementation->getValue (
	iResult, cEntityType, cEntity, cItem, cDate.asStringElse (0), cCurrency.asStringElse (0)
    );
    *result = *pSuccessful ? iResult : m_pImplementation->formatSettings ()->longNaN ();
    return S_OK;
}

STDMETHODIMP CConnection::GetString(BSTR *result, BSTR entityType, BSTR entity, BSTR item, VARIANT date, VARIANT currency, BOOL* pSuccessful) {
    VAccessString		cEntityType	(entityType);
    VAccessString		cEntity		(entity);
    VAccessString		cItem		(item);
    CVariantConverter	cDate		(date);
    CVariantConverter	cCurrency	(currency);

    VAccessString iResult;
    *pSuccessful = m_pImplementation->getValue (
	iResult, cEntityType, cEntity, cItem, cDate.asStringElse (0), cCurrency.asStringElse (0)
    );
    *result = *pSuccessful ? iResult.asBSTR (
	m_pImplementation->formatSettings()->returningUnicode ()
    ) : 0;
    return S_OK;
}

STDMETHODIMP CConnection::GetValue(VARIANT *result, BSTR entityType, BSTR entity, BSTR item, VARIANT date, VARIANT currency, BOOL* pSuccessful) {
    VAccessString		cEntityType	(entityType);
    VAccessString		cEntity		(entity);
    VAccessString		cItem		(item);
    CVariantConverter	cDate		(date);
    CVariantConverter	cCurrency	(currency);

    VDatum iResult;
    *pSuccessful = m_pImplementation->getValue (
	iResult, cEntityType, cEntity, cItem, cDate.asStringElse (0), cCurrency.asStringElse (0)
    );
    VariantInit (result);
    iResult.getValue (result, m_pImplementation->formatSettings()->returningUnicode ());
    return S_OK;
}

STDMETHODIMP CConnection::NewExtractWS(IExtractWS **ppExtractWS) {
    VReference<VExtractWS> pExtractWS (new VExtractWS (m_pImplementation));
    *ppExtractWS = 0;
    return pExtractWS ? pExtractWS->QueryInterface (
//	__uuidof (IExtractWS), reinterpret_cast<void**>(ppExtractWS)
	ppExtractWS
    ) : E_OUTOFMEMORY;
}

STDMETHODIMP CConnection::Rexec(BSTR hostname, BSTR username, BSTR password, VARIANT command) {
    return Login (hostname, username, password, command);
}

STDMETHODIMP CConnection::get_SessionNames(BSTR* sessionNames) {
    VAccessString cSessionNames;
    if (!m_pImplementation->getSessionNames(cSessionNames)) return E_FAIL;
    *sessionNames = cSessionNames.asBSTR();
    return S_OK;
}

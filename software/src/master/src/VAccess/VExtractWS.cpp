/*****  VExtractWS Implementation  *****/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VExtractWS.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VConnection.h"
#include "VAccessString.h"

/************************
 ************************
 *****              *****
 *****  VExtractWS  *****
 *****              *****
 ************************
 ************************/

/**************************************
 **************************************
 *****  Referenceable Requisites  *****
 **************************************
 **************************************/

DEFINE_CONCRETE_RTT (VExtractWS);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VExtractWS::VExtractWS (VExtractWS const& iOther) : VConnectionUse (iOther.m_pConnection) {
    initialize ();

    attach ();

    *this = iOther;
}

VExtractWS::VExtractWS (VConnection *pConnection) : VConnectionUse (pConnection) {
    initialize ();

    attach ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

//  Note: Disabled destruction of specific ExtractWS instance 
//  as default instance is being used

VExtractWS::~VExtractWS () {
    /*static char const* const pDeleteMessage = "deleteInstance";
    if (m_pConnection) {
	VString iRequest (m_iRemoteName.strlen () + strlen (pDeleteMessage));
	iRequest << m_iRemoteName << pDeleteMessage;
	execute (iRequest);
    }*/
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void VExtractWS::initialize () {
    m_iLocalSettings.m_xOrientation = VExtractOrientation_EI;
    m_iLocalSettings.m_iDelimiter = "\t";
}


/************************
 ************************
 *****  Assignment  *****
 ************************
 ************************/

VExtractWS& VExtractWS::operator= (VExtractWS const& iOther) {
    m_iLocalSettings = iOther.m_iLocalSettings;
    m_iRemoteSettings.reset ();
    
    return *this;
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/*********************
 *****  Context  *****
 *********************/

VExtractWS& VExtractWS::setCurrencyTo (char const *pString) {
    m_iLocalSettings.m_iCurrency = pString;
    return *this;
}

/********************
 *****  Format  *****
 ********************/

VExtractWS& VExtractWS::setOrientationTo (VExtractOrientation xOrientation) {
    m_iLocalSettings.m_xOrientation = xOrientation;
    return *this;
}

VExtractWS& VExtractWS::setColumnLabelsOn () {
    m_iLocalSettings.m_fColumnLabelsOn = true;
    return *this;
}

VExtractWS& VExtractWS::setColumnLabelsOff () {
    m_iLocalSettings.m_fColumnLabelsOn = false;
    return *this;
}

VExtractWS& VExtractWS::setRowLabelsOn () {
    m_iLocalSettings.m_fRowLabelsOn = true;
    return *this;
}

VExtractWS& VExtractWS::setRowLabelsOff () {
    m_iLocalSettings.m_fRowLabelsOn = false;
    return *this;
}

VExtractWS& VExtractWS::setScalarLabelOn () {
    m_iLocalSettings.m_fScalarLabelOn = true;
    return *this;
}

VExtractWS& VExtractWS::setScalarLabelOff () {
    m_iLocalSettings.m_fScalarLabelOn = false;
    return *this;
}

VExtractWS& VExtractWS::setDelimiterTo (char const *pDelimiter) {
    m_iLocalSettings.m_iDelimiter = pDelimiter;
    return *this;
}


/********************
 *****  Scalar  *****
 ********************/

VExtractWS& VExtractWS::setEntityTypeTo (char const *pString) {
    m_iLocalSettings.m_iEntityType = pString;
    return *this;
}

VExtractWS& VExtractWS::setEntityTo (char const *pString) {
    m_iLocalSettings.m_iEntity = pString;
    return *this;
}

VExtractWS& VExtractWS::setItemKindTo (VDatumKind xKind) {
    m_iLocalSettings.m_iItem.kind () = xKind;
    return *this;
}

VExtractWS& VExtractWS::setItemTo (char const *pName, VDatumKind xKind) {
    m_iLocalSettings.m_iItem.name () = pName;
    m_iLocalSettings.m_iItem.kind () = xKind;
    return *this;
}

VExtractWS& VExtractWS::setDateTo (char const *pString) {
    m_iLocalSettings.m_iDate = pString;
    return *this;
}


/*************************
 *****  Entity List  *****
 *************************/

VExtractWS& VExtractWS::setEntityListTo (char const *pString) {
    clearEntityList ();

    m_iLocalSettings.m_iEntityListString = pString;

    m_iLocalSettings.m_fEntityListStringIsAList = true;
    m_iLocalSettings.m_fEntityListStringIsValid = true;

    return *this;
}

VExtractWS& VExtractWS::setEntityListExpressionTo (char const *pString) {
    clearEntityList ();

	if ('[' == pString[strspn (pString, VAccessString::WhiteSpace)])
	m_iLocalSettings.m_iEntityListString = pString;
    else {
	m_iLocalSettings.m_iEntityListString = "[";
	m_iLocalSettings.m_iEntityListString << pString << "]";
    }

    m_iLocalSettings.m_fEntityListStringIsAList	= false;
    m_iLocalSettings.m_fEntityListStringIsValid	= true;

    return *this;
}

VExtractWS& VExtractWS::addEntity (char const *pString) {
    VString iString(pString);

    m_iLocalSettings.m_fEntityListStringIsValid = false;
    m_iLocalSettings.m_iEntityList << iString;

    return *this;
}

VExtractWS& VExtractWS::clearEntityList () {
    m_iLocalSettings.m_fEntityListStringIsValid = false;
    m_iLocalSettings.m_iEntityList.clear ();

    return *this;
}


/***********************
 *****  Item List  *****
 ***********************/

VExtractWS& VExtractWS::addItem (char const *pName, VDatumKind xKind) {
    VExtractItem& rItem = m_iLocalSettings.m_iItemList.newElement ();
    rItem.name () = pName;
    rItem.kind () = xKind;
    return *this;
}

VExtractWS& VExtractWS::clearItemList () {
    m_iLocalSettings.m_iItemList.clear ();
    return *this;
}

/***********************
 *****  Date List  *****
 ***********************/

VExtractWS& VExtractWS::setDateListTo (char const *pString) {
    clearDateList ();

    m_iLocalSettings.m_iDateListString = pString;

    m_iLocalSettings.m_fDateListStringIsAList = true;
    m_iLocalSettings.m_fDateListStringIsValid = true;

    return *this;
}

VExtractWS& VExtractWS::setDateListExpressionTo (char const *pString) {
    clearDateList ();

    if ('(' != pString[strspn (pString, VAccessString::WhiteSpace)])
	m_iLocalSettings.m_iDateListString = pString;
    else {
	m_iLocalSettings.m_iDateListString = "(";
	m_iLocalSettings.m_iDateListString << pString << ")";
    }

    m_iLocalSettings.m_fDateListStringIsAList = false;
    m_iLocalSettings.m_fDateListStringIsValid = true;

    return *this;
}

VExtractWS& VExtractWS::addDate (char const *pString) {
    VString iString(pString);

    m_iLocalSettings.m_fDateListStringIsValid = false;
    m_iLocalSettings.m_iDateList << iString;

    return *this;
}

VExtractWS& VExtractWS::clearDateList () {
    m_iLocalSettings.m_fDateListStringIsValid = false;
    m_iLocalSettings.m_iDateList.clear ();

    return *this;
}


/****************************
 ****************************
 *****  Event Handling  *****
 ****************************
 ****************************/

//  Note: Disabled creation of specific remote ExtractWS instance 
//  to make VAccess pool friendly. Each connection make use of the 
//  default ExtractWS instance

void VExtractWS::handleConnectionEvent ()
{
   /* VUID iUID;

    m_pConnection->getUID (iUID);

    VString iRemoteName ("X");
    iRemoteName << iUID.asString ("");

    VString iRequest ("Interface ExtractWS createInstance: ");
    iRequest.quote (iRemoteName);

    execute (iRequest);
    m_iRemoteName = "Interface Named ExtractWS ";
    m_iRemoteName << iRemoteName << "\n";*/

    m_iRemoteName = "Interface ExtractWS \n";
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

bool VExtractWS::run (VDatumArray& rDatumArray, bool fReset) {
    if (isntConnected ())
	return false;

    VString iRequest (m_iRemoteName);
    if (fReset) {
	m_iRemoteSettings.reset ();
	iRequest << "reset\n";
    }
    m_iLocalSettings.formatForVision (iRequest, m_iRemoteSettings);

    VString iReply;
    if (!submit (iReply, iRequest))
	return false;

    m_iRemoteSettings = m_iLocalSettings;

    return parse (rDatumArray, iReply);
}


/**********************************
 **********************************
 *****                        *****
 *****  VExtractWS::Settings  *****
 *****                        *****
 **********************************
 **********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VExtractWS::Settings::Settings ()
: m_xOrientation		(VExtractOrientation_Unknown)
, m_fColumnLabelsOn		(false)
, m_fRowLabelsOn		(false)
, m_fScalarLabelOn		(false)
, m_fEntityListStringIsAList	(false)
, m_fEntityListStringIsValid	(false)
, m_fDateListStringIsAList	(false)
, m_fDateListStringIsValid	(false)
{
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VExtractListType VExtractWS::Settings::dateListType () const {
    return m_fDateListStringIsValid ? (
	m_fDateListStringIsAList ? VExtractListType_String : VExtractListType_Expression
    ) : m_iDateList.arraySize () ? VExtractListType_Array  : VExtractListType_Unspecified;
}

bool VExtractWS::Settings::getDateListString (VString& rResult) const {
    if (m_fDateListStringIsValid && m_fDateListStringIsAList) {
	rResult.setTo (m_iDateListString);
	return true;
    }
    rResult.clear ();
    return false;
}

bool VExtractWS::Settings::getDateListExpression (VString& rResult) const {
    if (m_fDateListStringIsValid && !m_fDateListStringIsAList) {
	rResult.setTo (m_iDateListString);
	return true;
    }
    rResult.clear ();
    return false;
}

VExtractListType VExtractWS::Settings::entityListType () const {
    return m_fEntityListStringIsValid ? (
	m_fEntityListStringIsAList ? VExtractListType_String : VExtractListType_Expression
    ) : m_iEntityList.arraySize () ? VExtractListType_Array  : VExtractListType_Unspecified;
}

bool VExtractWS::Settings::getEntityListString (VString& rResult) const {
    if (m_fEntityListStringIsValid && m_fEntityListStringIsAList) {
	rResult.setTo (m_iEntityListString);
	return true;
    }
    rResult.clear ();
    return false;
}

bool VExtractWS::Settings::getEntityListExpression (VString& rResult) const {
    if (m_fEntityListStringIsValid && !m_fEntityListStringIsAList) {
	rResult.setTo (m_iEntityListString);
	return true;
    }
    rResult.clear ();
    return false;
}

VExtractListType VExtractWS::Settings::itemListType () const {
    return m_iItemList.arraySize () ? VExtractListType_Array  : VExtractListType_Unspecified;
}


/*******************************
 *******************************
 *****  Vision Formatting  *****
 *******************************
 *******************************/

void VExtractWS::Settings::formatForVision (
    VString& iRequest, Settings const &rRemoteSettings
) const {
    if (m_xOrientation != rRemoteSettings.m_xOrientation)
    {
	VAccessString iOrientationCode (orientationCode ());
	iOrientationCode.formatForVision (iRequest, "setOrientationTo:");
    }

    if (m_iDelimiter != rRemoteSettings.m_iDelimiter)
	m_iDelimiter.formatForVision (iRequest, "setDelimiterTo:");

    if (m_fColumnLabelsOn != rRemoteSettings.m_fColumnLabelsOn)
	iRequest << (
	    m_fColumnLabelsOn ? "setColumnLabelsOn\n" : "setColumnLabelsOff\n"
	);

    if (m_fRowLabelsOn != rRemoteSettings.m_fRowLabelsOn)
	iRequest << (
	    m_fRowLabelsOn ? "setRowLabelsOn\n" : "setRowLabelsOff\n"
	);

    if (m_fScalarLabelOn != rRemoteSettings.m_fScalarLabelOn)
	iRequest << (
	    m_fScalarLabelOn ? "setScalarLabelOn\n" : "setScalarLabelOff\n"
	);

    if (m_iEntityType != rRemoteSettings.m_iEntityType)
	m_iEntityType.formatForVision (iRequest, "setEntityTypeTo:", false);

    if (m_iEntity != rRemoteSettings.m_iEntity)
	m_iEntity.formatForVision (iRequest, "setEntityTo:");

    if (m_iItem != rRemoteSettings.m_iItem)
	m_iItem.formatForVision (iRequest, "setItemTo:");

//  Must resend 'date' and 'currency' every time...
//    if (m_iDate != rRemoteSettings.m_iDate)
	m_iDate.formatForVision (iRequest, "setDateTo:");

//    if (m_iCurrency != rRemoteSettings.m_iCurrency)
	m_iCurrency.formatForVision (iRequest, "setCurrencyTo:");

    if (m_fEntityListStringIsValid) {
	if (m_fEntityListStringIsAList	!= rRemoteSettings.m_fEntityListStringIsAList ||
	    m_iEntityListString		!= rRemoteSettings.m_iEntityListString)
	    m_iEntityListString.formatForVision (
		iRequest, "setEntityListTo:", m_fEntityListStringIsAList
	    );
    }
    else if (m_iEntityList != rRemoteSettings.m_iEntityList)
	m_iEntityList.formatForVision (iRequest, "setEntityListTo:");

    if (m_iItemList != rRemoteSettings.m_iItemList)
	m_iItemList.formatForVision (iRequest, "setItemListTo:");

    if (m_fDateListStringIsValid) {
	if (m_fDateListStringIsAList	!= rRemoteSettings.m_fDateListStringIsAList ||
	    m_iDateListString		!= rRemoteSettings.m_iDateListString) {
	    if (m_fDateListStringIsAList)
		m_iDateListString.formatForVision (iRequest, "setDateListTo:");
	    else {
		VAccessString iDateListString;
		iDateListString << "(" << m_iDateListString << ")";
		iDateListString.formatForVision (iRequest, "setDateListTo:", false);
	    }
	}
    }
    else if (m_iDateList != rRemoteSettings.m_iDateList)
	m_iDateList.formatForVision (iRequest, "setDateListTo:");

    iRequest << "run\n";
}


/*************************
 *************************
 *****  Orientation  *****
 *************************
 *************************/

char const *VExtractWS::Settings::orientationCode () const {
    switch (m_xOrientation) {
    default:
	return "Unknown";
    case VExtractOrientation_EI:
	return "EI";
    case VExtractOrientation_ET:
	return "ET";
    case VExtractOrientation_IE:
	return "IE";
    case VExtractOrientation_IT:
	return "IT";
    case VExtractOrientation_TE:
	return "TE";
    case VExtractOrientation_TI:
	return "TI";
    }
}


/****************************
 ****************************
 *****  Result Parsing  *****
 ****************************
 ****************************/

bool VExtractWS::Settings::parse (
    VDatumArray& rDatumArray, char const* pString
) const {
    VString iCellString;

    char const *pDelimiterString = m_iDelimiter;
    size_t	sDelimiterString = strlen (pDelimiterString);

/*****  Parse the array size, ...  *****/
    size_t s[2];
    for (unsigned int xDimension = 0; xDimension < 2; xDimension++)
    {
	char const* pDelimiter = strstr (pString, pDelimiterString);
	if (0 == pDelimiter)
	    return false;

	char* pEOP;
	s[xDimension] = (size_t)strtol (pString, &pEOP, 10);
	if (pEOP == pString)
	    return false;

	pString = pDelimiter + sDelimiterString;
    }

/*****  ... reshape the array, ...  *****/
    rDatumArray.reshape (2, s);

/*****  ... and do the parsing:  *****/
    unsigned int x[2];
    for (x[0] = 0; x[0] < s[0]; x[0]++)
    {
	pString = strchr (pString, '\n');
	if (pString)
	    pString++;
	else return false;

	for (x[1] = 0; x[1] < s[1]; x[1]++)
	{
	    char const* pDelimiter = strstr (pString, pDelimiterString);
	    if (0 == pDelimiter)
		return false;

	    iCellString.setTo (pString, (size_t)(pDelimiter - pString));
	    controllingItem(x).parse (rDatumArray.element(x), iCellString);

	    pString = pDelimiter + sDelimiterString;
	}
    }

    return true;
}


/******************************
 ******************************
 *****  Controlling Item  *****
 ******************************
 ******************************/

VExtractItem const& VExtractWS::Settings::controllingItem (
    unsigned int const* pSubscriptVector
) const {
    static VExtractItem const iLabelItem ("__LabelItem__", VDatumKind_String);

    size_t sColLabelArea = m_fScalarLabelOn || m_fColumnLabelsOn ? 1 : 0;
    if (sColLabelArea > 0 && 0 == pSubscriptVector[0])
	return iLabelItem;

    size_t sRowLabelArea = m_fScalarLabelOn || m_fRowLabelsOn    ? 1 : 0;
    if (sRowLabelArea > 0 && 0 == pSubscriptVector[1])
	return iLabelItem;

    switch (m_xOrientation) {
    case VExtractOrientation_EI:
	return m_iItemList[pSubscriptVector[1] - sRowLabelArea];

    case VExtractOrientation_IE:
	return m_iItemList[pSubscriptVector[0] - sColLabelArea];

    case VExtractOrientation_IT:
	return m_iItemList[pSubscriptVector[0] - sColLabelArea];

    case VExtractOrientation_TI:
	return m_iItemList[pSubscriptVector[1] - sRowLabelArea];

    default:
	return m_iItem;
    }
}


/*******************
 *******************
 *****  Reset  *****
 *******************
 *******************/

void VExtractWS::Settings::reset () {
    m_xOrientation		= VExtractOrientation_Unknown;
    m_fColumnLabelsOn		=
    m_fRowLabelsOn		=
    m_fEntityListStringIsValid	=
    m_fEntityListStringIsAList	= false;
    m_iDelimiter		.clear ();
    m_iEntityType		.clear ();
    m_iEntity			.clear ();
    m_iItem			.clear ();
    m_iDate			.clear ();
    m_iCurrency			.clear ();
    m_iEntityListString		.clear ();
    m_iEntityList		.clear ();
    m_iItemList			.clear ();
    m_iDateList			.clear ();
}

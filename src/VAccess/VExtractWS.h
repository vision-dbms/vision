/*****  Extract Workspace Class Definition  *****/
#ifndef VExtractWS_Interface
#define VExtractWS_Interface

/************************
 *****  Components  *****
 ************************/

#include "VConnectionUse.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VDatum.h"
#include "VAccessString.h"


/*******************************************
 *****  Extract Orientation Constants  *****
 *******************************************/

enum VExtractOrientation {
    VExtractOrientation_Unknown,
    VExtractOrientation_EI,
    VExtractOrientation_ET,
    VExtractOrientation_IE,
    VExtractOrientation_IT,
    VExtractOrientation_TE,
    VExtractOrientation_TI
};

/*****************************************
 *****  Extract List Type Constants  *****
 *****************************************/

enum VExtractListType {
    VExtractListType_Unspecified,
    VExtractListType_Array,
    VExtractListType_String,
    VExtractListType_Expression
};


/*******************************************
 *****  Extract Item Class Definition  *****
 *******************************************/

class VExtractItem {
/*****  Construction *****/
public:
    VExtractItem (
	char const* pName = "", VDatumKind xKind = VDatumKind_NA
    ) : m_iName (pName), m_xKind (xKind)
    {
    }

/*****  Predicates  *****/
public:
    bool operator== (VExtractItem const &iOther) const {
	return m_xKind == iOther.m_xKind && m_iName == iOther.m_iName;
    }
    bool operator!= (VExtractItem const &iOther) const {
	return m_xKind != iOther.m_xKind || m_iName != iOther.m_iName;
    }

/*****  Access  *****/
public:
    VAccessString&	name ()		{ return m_iName; }
    VAccessString const&name () const	{ return m_iName; }

    VDatumKind&		kind ()		{ return m_xKind; }
    VDatumKind const&	kind () const	{ return m_xKind; }

    operator char const* () const {
	return m_iName;
    }

/*****  Update  *****/
public:
    VExtractItem& operator= (VExtractItem const &iOther) {
	m_iName = iOther.m_iName;
	m_xKind = iOther.m_xKind;
	return *this;
    }

    VExtractItem& operator= (char const* pName) {
	m_iName = pName;
	m_xKind = VDatumKind_NA;
	return *this;
    }

    void clear () {
	m_iName.clear ();
	m_xKind = VDatumKind_NA;
    }

/*****  Parsing  *****/
public:
    bool parse (VDatum& rResult, char const* pString) const {
	return rResult.parse (pString, m_xKind);
    }

/*****  Vision Expression Formatting  *****/
public:
    void formatForVision (
	V::VString&	iExpression,
	char const*	pKeyword,
	bool		fQuote = true,
	bool		fClose = true
    ) const {
		//m_iName.formatForVision (iExpression, pKeyword, fQuote, fClose);
		size_t sGuarantee = iExpression.length() + ::strlen (pKeyword) + 2;
		if (fClose)
			sGuarantee += 1;
		if (fQuote)
			sGuarantee += 2;

		if (iExpression.guarantee (sGuarantee)) {
			iExpression.append (pKeyword);
			if (fQuote)
				iExpression.quote (m_iName);
			else
				iExpression.append (m_iName);
			iExpression.append (fClose ? ".\n" : "\n");
		}
    }

/*****  State  *****/
protected:
    VAccessString	m_iName;
    VDatumKind		m_xKind;
};


/************************************************
 *****  Extract Workspace Class Definition  *****
 ************************************************/

class VExtractWS : public VConnectionUse {
    DECLARE_CONCRETE_RTT (VExtractWS, VConnectionUse);
    DECLARE_COM_WRAPPABLE (IExtractWS);

/*****  Settings  *****/
public:
    class Settings {
	friend class VExtractWS;

    //  Construction
    public:
	Settings ();

    //  Access
    public:
	VExtractListType dateListType () const;
	bool getDateListString (VString& rResult) const;
	bool getDateListExpression (V::VString& rResult) const;

	VExtractListType entityListType	() const;
	bool getEntityListString (V::VString& rResult) const;
	bool getEntityListExpression (V::VString& rResult) const;

	VExtractListType itemListType () const;

    //  Update
    public:
	void reset ();

    //  Vision Formatting
    public:
	void formatForVision (
	    V::VString& iRequest, Settings const &rRemoteSettings
	) const;

    protected:
	char const* orientationCode () const;

    //  Result Parsing
    public:
	bool parse (VDatumArray& rDatumArray, char const* pString) const;

    protected:
	VExtractItem const& controllingItem (
	    unsigned int const* pSubscriptVector
	) const;

    //  State
    protected:
	VExtractOrientation	m_xOrientation;
	unsigned int		m_fColumnLabelsOn		: 1,
				m_fRowLabelsOn			: 1,
				m_fScalarLabelOn		: 1,
				m_fEntityListStringIsAList	: 1,
				m_fEntityListStringIsValid	: 1,
				m_fDateListStringIsAList	: 1,
				m_fDateListStringIsValid	: 1;
	VAccessString			m_iDelimiter;
	VAccessString			m_iEntityType;
	VAccessString			m_iEntity;
	VExtractItem		m_iItem;
	VAccessString			m_iDate;
	VAccessString			m_iCurrency;
	VAccessString			m_iEntityListString;
	VArray<V::VString>		m_iEntityList;
	VArray<VExtractItem>	m_iItemList;
	VAccessString			m_iDateListString;
	VArray<V::VString>		m_iDateList;
    };


/*****  Construction/Destruction/Initialization  *****/
public:
    VExtractWS (VExtractWS const& iOther);
    VExtractWS (VConnection* pConnection);

protected:
    ~VExtractWS ();

private:
    void initialize ();

/*****  Assignment  *****/
    VExtractWS& operator= (VExtractWS const& iOther);


/*****  Access  *****/
public:
    VAccessString const& date () const {
	return m_iLocalSettings.m_iDate;
    }

    VAccessString const& currency () const {
	return m_iLocalSettings.m_iCurrency;
    }

    VExtractOrientation orientation () const {
	return m_iLocalSettings.m_xOrientation;
    }

    bool showColumnLabels () const {
	return m_iLocalSettings.m_fColumnLabelsOn;
    }

    bool showRowLabels () const {
	return m_iLocalSettings.m_fRowLabelsOn;
    }

    bool showScalarLabel () const {
	return m_iLocalSettings.m_fScalarLabelOn;
    }

    VAccessString const& delimiter () const {
	return m_iLocalSettings.m_iDelimiter;
    }


    VAccessString const& entityType () const {
	return m_iLocalSettings.m_iEntityType;
    }

    VAccessString const& entity () const {
	return m_iLocalSettings.m_iEntity;
    }

    VExtractItem const& item () const {
	return m_iLocalSettings.m_iItem;
    }
    VAccessString const& itemName () const {
	return m_iLocalSettings.m_iItem.name ();
    }
    VDatumKind itemKind () const {
	return m_iLocalSettings.m_iItem.kind ();
    }


    VExtractListType dateListType () const {
	return m_iLocalSettings.dateListType ();
    }
    bool getDateListString (V::VString& rResult) const {
	return m_iLocalSettings.getDateListString (rResult);
    }
    bool getDateListExpression (V::VString& rResult) const {
	return m_iLocalSettings.getDateListExpression (rResult);
    }

    VExtractListType entityListType () const {
	return m_iLocalSettings.entityListType ();
    }
    bool getEntityListString (V::VString& rResult) const {
	return m_iLocalSettings.getDateListString (rResult);
    }
    bool getEntityListExpression (V::VString& rResult) const {
	return m_iLocalSettings.getDateListExpression (rResult);
    }

    VExtractListType itemListType () const {
	return m_iLocalSettings.itemListType ();
    }


/*****  Update  *****/
public:
    VExtractWS&	setCurrencyTo	(char const *pName);

    VExtractWS& setOrientationTo(VExtractOrientation xOrientation);
    VExtractWS& setOrientationToEI () {
	return setOrientationTo (VExtractOrientation_EI);
    }
    VExtractWS& setOrientationToET () {
	return setOrientationTo (VExtractOrientation_ET);
    }
    VExtractWS& setOrientationToIE () {
	return setOrientationTo (VExtractOrientation_IE);
    }
    VExtractWS& setOrientationToIT () {
	return setOrientationTo (VExtractOrientation_IT);
    }
    VExtractWS& setOrientationToTE () {
	return setOrientationTo (VExtractOrientation_TE);
    }
    VExtractWS& setOrientationToTI () {
	return setOrientationTo (VExtractOrientation_TI);
    }

    VExtractWS& setColumnLabelsOn ();
    VExtractWS& setColumnLabelsOff ();

    VExtractWS& setRowLabelsOn ();
    VExtractWS& setRowLabelsOff ();

    VExtractWS& setScalarLabelOn ();
    VExtractWS& setScalarLabelOff ();

    VExtractWS& setDelimiterTo (char const *pDelimiter);

    VExtractWS& setEntityTypeTo	(char const *pString);

    VExtractWS& setEntityTo (char const *pString);

    VExtractWS& setEntityListTo (char const *pString);
    VExtractWS& setEntityListExpressionTo (char const *pString);

    VExtractWS& addEntity (char const *pString);
    VExtractWS& clearEntityList	();

    VExtractWS& setItemKindTo (VDatumKind xKind);
    VExtractWS& setItemTo (
	char const *pString, VDatumKind xKind = VDatumKind_NA
    );
    VExtractWS& addItem (
	char const *pString, VDatumKind xKind = VDatumKind_NA
    );
    VExtractWS& clearItemList ();
    
    VExtractWS& setDateTo (char const *pString);

    VExtractWS& setDateListTo (char const *pString);
    VExtractWS& setDateListExpressionTo (char const *pString);

    VExtractWS& addDate (char const *pString);
    VExtractWS& clearDateList ();

/*****  Execution  *****/
public:
    //bool run (VDatumArray& rDataArray, bool fReset = false);
      bool run (VDatumArray& rDataArray, bool fReset = true); 

/*****  Parsing  *****/
protected:
    bool parse (VDatumArray& rDatumArray, char const* pString) const {
	return m_iRemoteSettings.parse (rDatumArray, pString);
    }

    VExtractItem const& item (unsigned int const* pSubscriptVector) const;

/*****  Event Handling  *****/
    virtual void handleConnectionEvent ();

/*****  State  *****/
protected:
    V::VString	m_iRemoteName;
    Settings	m_iLocalSettings;
    Settings	m_iRemoteSettings;
};


#endif

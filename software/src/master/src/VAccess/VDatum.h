/*****  Datum Class Definition  *****/
#ifndef VDatum_Definition
#define VDatum_Definition

/******************************************
 *****  Declarations and Definitions  *****
 ******************************************/
#include "VStdLib.h"
#include "VAccessString.h"

/************************************
 *****  VDatumKind Enumeration  *****
 ************************************/

enum VDatumKind {
    VDatumKind_NA,
    VDatumKind_Integer,
    VDatumKind_Real,
    VDatumKind_String
};


/*************************************
 *****  VDatum Class Definition  *****
 *************************************/

class VDatum {
/*****  Friends  *****/

/*****  Construction/Destruction  *****/
public:
    VDatum (char const* pValue) : m_xDatumKind(VDatumKind_NA) {
	*this = pValue;
    }
    VDatum (double iValue) : m_xDatumKind(VDatumKind_NA) {
	*this = iValue;
    }
    VDatum (int iValue) : m_xDatumKind(VDatumKind_NA) {
	*this = iValue;
    }
    VDatum () : m_xDatumKind(VDatumKind_NA) {
    }

    ~VDatum ();

/*****  Access  *****/
public:
    VDatumKind kind () const {
	return m_xDatumKind;
    }

    bool isNA () const {
	return VDatumKind_NA == m_xDatumKind;
    }
    bool isInteger () const {
	return VDatumKind_Integer == m_xDatumKind;
    }
    bool isReal () const {
	return VDatumKind_Real == m_xDatumKind;
    }
    bool isString () const {
	return VDatumKind_String == m_xDatumKind;
    }

    char const*	asString  (bool &isValid) const;
    double	asReal	  (bool &isValid) const;
    int		asInteger (bool &isValid) const;

#if defined(WIN32_COM_SERVER)
    bool getValue (VARIANT* pVariant, bool bReturningUnicode = true) const;
#endif

    bool getValue (char const*& iValue) const {
	bool isValid;
	iValue = asString (isValid);
	return isValid;
    }
    bool getValue (double& iValue) const {
	bool isValid;
	iValue = asReal (isValid);
	return isValid;
    }
    bool getValue (int& iValue) const {
	bool isValid;
	iValue = asInteger (isValid);
	return isValid;
    }

    operator char const* () const {
	bool isValid;
	return asString (isValid);
    }
    operator double () const {
	bool isValid;
	return asReal (isValid);
    }
    operator int () const {
	bool isValid;
	return asInteger (isValid);
    }

/*****  Comparison  *****/
public:
    bool operator== (VDatum const &iOther) const;
    bool operator== (int iOther) const;
    bool operator== (double iOther) const;
    bool operator== (char const *pOther) const;

    bool operator!= (VDatum const &iOther) const {
	return !(*this == iOther);
    }
    bool operator!= (int iOther) const {
	return !(*this == iOther);
    }
    bool operator!= (double iOther) const {
	return !(*this == iOther);
    }
    bool operator!= (char const *pOther) const {
	return !(*this == pOther);
    }

/*****  Update  *****/
public:
    VDatum& operator= (VDatum const& iValue);
    VDatum& operator= (char const* pValue);
    VDatum& operator= (double iValue);
    VDatum& operator= (int iValue);

    void clear ();

    bool parse (char const* pString, VDatumKind xExpectedKind = VDatumKind_NA);

/*****  State  *****/
protected:
    VDatumKind m_xDatumKind;
	int		   m_iInteger;
	double     m_iReal;
	VAccessString	   m_iString;
};


/******************************************
 *****  VDatumArray Class Definition  *****
 ******************************************/

#include "VArray.h"

class VDatumArray : public VArray<VDatum> {
//  Construction
public:
    VDatumArray (VDatumArray const& rOther) : VArray<VDatum> (rOther) {
    }
    VDatumArray (size_t sDimensionVector = 1) : VArray<VDatum> (sDimensionVector) {
    }

//  Access
public:
    bool getElements (double * pElementArray, double iNAValue) const;
    bool getElements (int    * pElementArray, int    iNAValue) const;

#if defined(WIN32_COM_SERVER)
    bool getElements (BSTR   * pElementArray, bool bReturningUnicode = true) const;
    bool getElements (VARIANT* pElementArray, bool bReturningUnicode = true) const;

    bool getArray (SAFEARRAY**ppArray, double	iNAValue) const;
    bool getArray (SAFEARRAY**ppArray, int	iNAValue) const;
    bool getArray (SAFEARRAY**ppArray, bool	bReturningUnicode) const;
#endif
};


#endif

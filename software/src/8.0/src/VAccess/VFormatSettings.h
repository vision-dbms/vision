#ifndef VFormatSettings_Interface
#define VFormatSettings_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VFormatSettings : public VReferenceable {
    DECLARE_CONCRETE_RTT (VFormatSettings, VReferenceable);
    DECLARE_COM_WRAPPABLE (IFormatSettings);

//  Globals
public:
    static VReference<VFormatSettings> const GlobalSettings;

//  Construction
protected:
    VFormatSettings (VFormatSettings* pParent);

public:
    VFormatSettings* newChild () {
	return new VFormatSettings (this);
    }

//  Destruction
protected:
    ~VFormatSettings () {
    }

//  Access
public:
    VFormatSettings* parent () const {
	return m_pParent;
    }

    double	doubleNaN		() const;
    long	longNaN			() const;
    bool	returningUnicode	() const;

//  Query
public:
    bool setsDoubleNaN () const {
	return m_fSetsDoubleNaN;
    }
    bool setsLongNaN () const {
	return m_fSetsLongNaN;
    }
    bool setsReturningUnicode () const {
	return m_fSetsReturningUnicode;
    }

//  Update
public:
    void setDoubleNaNTo (double iNewValue) {
	m_iDoubleNaN = iNewValue;
	m_fSetsDoubleNaN = true;
    }

    void setLongNaNTo (long iNewValue) {
	m_iLongNaN = iNewValue;
	m_fSetsLongNaN = true;
    }
    void setReturningUnicodeTo (bool iNewValue) {
	m_fReturningUnicode = iNewValue;
	m_fSetsReturningUnicode = true;
    }

    void unsetDoubleNaN () {
	m_fSetsDoubleNaN = false;
    }
    void unsetLongNaN () {
	m_fSetsLongNaN = false;
    }
    void unsetReturningUnicode () {
	m_fSetsReturningUnicode = false;
    }

//  State
protected:
    VReference<VFormatSettings> const	m_pParent;
    double				m_iDoubleNaN;
    long				m_iLongNaN;
    unsigned int			m_fReturningUnicode	: 1;
    unsigned int			m_fSetsDoubleNaN	: 1;
    unsigned int			m_fSetsLongNaN		: 1;
    unsigned int			m_fSetsReturningUnicode	: 1;
};


#endif

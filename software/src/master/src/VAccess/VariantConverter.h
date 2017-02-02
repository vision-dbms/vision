#ifndef VariantConverter_Interface
#define VariantConverter_Interface

/************************
 *****  Components  *****
 ************************/

#include "VAccessString.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class CVariantConverter : public CComVariant {
//  Construction
public:
    CVariantConverter(VARIANT const& rSource);

//  Destruction
public:
    ~CVariantConverter();

//  Access
    VARTYPE vartype () const {
	return V_VT (this);
    }

//  Query
public:
    unsigned int unspecified () const {
	return VT_ERROR == V_VT (this) &&  DISP_E_PARAMNOTFOUND == V_ERROR (this);
    }

//  Use
public:
    unsigned int changeTypeTo (VARTYPE xNewType);

    long	asLongElse	(long	iElse);
    LPCSTR	asStringElse	(LPCSTR	pElse);

//  State
protected:
    VAccessString m_iAnsiStringHolder;
};


#endif

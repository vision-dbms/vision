// VariantConverter.cpp: implementation of the CVariantConverter class.
//
//////////////////////////////////////////////////////////////////////

#include "VStdLib.h"
#include "VariantConverter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariantConverter::CVariantConverter (VARIANT const& rSource)
: CComVariant (V_ISBYREF (&rSource) ? *V_VARIANTREF (&rSource) : rSource)
{
}

CVariantConverter::~CVariantConverter() {
}

unsigned int CVariantConverter::changeTypeTo (VARTYPE xNewType) {
    if (unspecified ())
	return FALSE;

    ChangeType (xNewType);
    return vartype () == xNewType;
}

long CVariantConverter::asLongElse(long iElse) {
    if (!changeTypeTo (VT_I4))
	return iElse;

    return V_I4 (this);
}

LPCSTR CVariantConverter::asStringElse(LPCSTR pElse) {
    if (!changeTypeTo (VT_BSTR))
	return pElse;

    m_iAnsiStringHolder.setTo (V_BSTR (this));

    return m_iAnsiStringHolder;
}

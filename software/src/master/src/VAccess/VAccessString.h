#ifndef VAccessString_Interface
#define VAccessString_Interface

#include "V_VString.h"

class VAccessString : public V::VString {
	DECLARE_FAMILY_MEMBERS (VAccessString, V::VString);

public:
	// Constructors
	VAccessString() : V::VString  () {};
	VAccessString(VString const &rString) : V::VString(rString) {};

	// Operator overloads.
	ThisClass &operator= (VString const &rOther) {
		BaseClass::setTo (rOther);
        return *this;
    }
    ThisClass &operator= (char const *pOther) {
		BaseClass::setTo (pOther);
        return *this;
    }

	// BSTR compatibility.
	VAccessString(BSTR pBSTR);
	BSTR asBSTR (bool bReturningUnicode = true) const;
	void setTo (BSTR pBSTR);

	void formatForVision (
		VString&	rExpression,
		char const*	pKeyword,
		bool		bQuote = true,
		bool		bClose = true) const;

	static char const * const WhiteSpace;
};


#endif

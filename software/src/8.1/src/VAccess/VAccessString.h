#ifndef VAccessString_Interface
#define VAccessString_Interface

#include "V_VString.h"

class VAccessString : public VString {
	DECLARE_FAMILY_MEMBERS (VAccessString, VString);

public:
	// Constructors
	VAccessString() : VString() {};
	VAccessString(VString const &rString) : VString(rString) {};

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

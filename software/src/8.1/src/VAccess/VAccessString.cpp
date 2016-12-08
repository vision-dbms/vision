
#include "VStdLib.h"
#include "VAccessString.h"

char const* const VAccessString::WhiteSpace = " \t\n";

VAccessString::VAccessString(BSTR pBSTR) {
	setTo (pBSTR);
}

BSTR VAccessString::asBSTR (bool bReturningUnicode) const {
    size_t cCharacters =  length();
    if (!bReturningUnicode)
		return ::SysAllocStringByteLen (content(), cCharacters);

    size_t cWideChars = MultiByteToWideChar(CP_ACP, 0, content(), cCharacters, 0, 0);
    BSTR pBSTR = SysAllocStringLen (0, cWideChars);
    if (pBSTR && 0 == MultiByteToWideChar(CP_ACP, 0, content(), cCharacters, pBSTR, cWideChars)) {
        SysFreeString (pBSTR);
        pBSTR = NULL;
    }

    return pBSTR;
}

void VAccessString::setTo (BSTR pBSTR) {
    size_t sBSTR = pBSTR ? ::SysStringLen (pBSTR) : 0;
    if (0 == sBSTR)
        clear ();
    else {
        // Determine number of bytes to be allocated for ANSI string. An
        // ANSI string can have at most 2 bytes per character (for Double
        // Byte Character Strings.)
        size_t cbAnsi = WideCharToMultiByte (CP_ACP, 0, pBSTR, sBSTR + 1, 0, 0, NULL, NULL);
        if (guarantee (cbAnsi) && 0 == WideCharToMultiByte (CP_ACP, 0, pBSTR, sBSTR + 1, storage(), cbAnsi, NULL, NULL))
            clear ();
    }
}


void VAccessString::formatForVision (
		VString&	rExpression,
		char const*	pKeyword,
		bool		bQuote,
		bool		bClose) const
{
	size_t sGuarantee = rExpression.length() + ::strlen (pKeyword) + 2;
	if (bClose)
		sGuarantee += 1;
	if (bQuote)
		sGuarantee += 2;

	if (rExpression.guarantee (sGuarantee)) {
		rExpression.append (pKeyword);
		if (bQuote)
			rExpression.quote (*this);
		else
			rExpression.append (*this);
		rExpression.append (bClose ? ".\n" : "\n");
	}
}
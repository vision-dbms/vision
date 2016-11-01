/*****  V_VCommandLine Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "V_VCommandLine.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  V::VCommandLine::Cursor  *****
 *****                           *****
 *************************************
 *************************************/

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

char const *V::VCommandLine::Cursor::nextString (
    char const *pTag, char const *pVariable, char const *pDefaultValue
) {
    if (pTag) {
	size_t const sTag = strlen (pTag);

	char const *pWord;
	while (pWord = nextTag ()) {
	    if (strncasecmp (pWord, pTag, sTag) == 0 && '=' == pWord[sTag])
		return pWord + sTag + 1;
	}
    }
    char const *pEnvironmentValue = pVariable ? getenv (pVariable) : 0;
    return pEnvironmentValue ? pEnvironmentValue : pDefaultValue;
}

bool V::VCommandLine::Cursor::nextSwitch (char const *pTag, char const *pVariable) {
    if (pTag) {
	char const *pWord;
	while (pWord = nextTag ()) {
	    if (strcasecmp (pWord, pTag) == 0)
		return true;
	}
    }
    return pVariable && getenv (pVariable);
}

// -xyz(etc.)
char const *V::VCommandLine::Cursor::nextTag () {
    char const *pWord;
    while (pWord = nextWord ()) {
	if ('-' == pWord[0])
	    return pWord + 1;
    }
    return 0;
}

// xyz NOT -xyz(etc.)
char const *V::VCommandLine::Cursor::nextToken () {
    char const *pWord;
    while (pWord = nextWord ()) {
	if ('-' != pWord[0])
	    return pWord;
    }
    return 0;
}


/*****************************
 *****************************
 *****                   *****
 *****  V::VCommandLine  *****
 *****                   *****
 *****************************
 *****************************/

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool V::VCommandLine::getValue (
    size_t &rResult, char const *pTag, char const *pVariable
) const {
    Cursor iCursor (this);
    char const *pOptionString = iCursor.nextString (pTag, pVariable);
    if (!pOptionString)
	return false;

    char *pOptionResidue;
    rResult = strtoul (pOptionString, &pOptionResidue, 0);

    return pOptionString != pOptionResidue;
}

char const *V::VCommandLine::stringValue (
    char const *pTag, char const *pVariable, char const *pDefaultValue
) const {
    Cursor iCursor (this);
    return iCursor.nextString (pTag, pVariable, pDefaultValue);
}

bool V::VCommandLine::switchValue (char const *pTag, char const *pVariable) const {
    Cursor iCursor (this);
    return iCursor.nextSwitch (pTag, pVariable);
}

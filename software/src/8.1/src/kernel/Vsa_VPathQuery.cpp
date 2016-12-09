/*****  Vsa_VPathQuery Implementation  *****/

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

#include "Vsa_VPathQuery.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************************
 ******************************************
 *****                                *****
 *****  Vsa::VPathQuery::Environment  *****
 *****                                *****
 ******************************************
 ******************************************/

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vsa::VPathQuery::Environment::getValue (char const *pKey, VString &rValue) {
    if (m_bNotMapped) {
	VString iKey; VString iValue;

	char const *pRest = m_iString;
	char const *pEQ;

	while (pEQ = strchr (pRest, '=')) {
	    iKey.setTo (pRest, pEQ - pRest);
	    pRest = pEQ + 1;

	    char const *pNL = strchr (pRest, '\n');
	    if (pNL) {
		iValue.setTo (pRest, pNL - pRest - ((pNL - 1)[0] != '\r' ? 0 : 1));
		pRest = pNL + 1;
	    }
	    else {
		iValue.setTo (pRest);
		pRest += strlen (pRest);
	    }

	    unsigned int xElement;
	    m_iMap.Insert (iKey, xElement);
	    m_iMap.value (xElement).setTo (iValue);
	}
	m_bNotMapped = false;
    }

    unsigned int xElement;
    bool found = m_iMap.Locate (pKey, xElement);
    if (found)
	rValue.setTo (m_iMap.value (xElement));
    else
	rValue.clear ();
    return found;
}


/****************************************
 ****************************************
 *****                              *****
 *****  Vsa::VPathQuery::Formatter  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPathQuery::Formatter::Formatter (
    char const *pClass, bool bClassCopy, char const *pTemplate, bool bTemplateCopy
) : m_iClass (pClass, bClassCopy), m_iTemplate (pTemplate, bTemplateCopy) {
}

Vsa::VPathQuery::Formatter::Formatter (
    VString const &rClass, VString const &rTemplate
) : m_iClass (rClass), m_iTemplate (rTemplate) {
}

Vsa::VPathQuery::Formatter::Formatter (
    Formatter const &rOther
) : m_iClass (rOther.m_iClass), m_iTemplate (rOther.m_iTemplate) {
}


/*****************************
 *****************************
 *****                   *****
 *****  Vsa::VPathQuery  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPathQuery::VPathQuery (
    VString const &rPath, VString const &rQuery, VString const &rEnvironment
) : m_iPath (rPath), m_iQuery (rQuery), m_iEnvironment (rEnvironment) {
  traceInfo ("Creating Vsa::VPathQuery");
}

Vsa::VPathQuery::VPathQuery (
    VString const &rPath, VString const &rQuery
) : m_iPath (rPath), m_iQuery (rQuery) {
  traceInfo ("Creating Vsa::VPathQuery");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPathQuery::~VPathQuery () {
  traceInfo ("Destroying Vsa::VPathQuery");
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vsa::VPathQuery::getVisionExpression (Formatter const &rFormatter, VString &rResult) {
    rResult.clear ();
//    rResult.guarantee (65535);

    VString iTemplate (rFormatter.getTemplate ());
    char const *pSource = iTemplate.storage ();

    while (*pSource) {
	unsigned int sScan = strcspn (pSource, "$");
	rResult.append (pSource, sScan);

	pSource += sScan;
	if (*pSource) {
	    rResult.append (" ");

	    bool anInternalSymbol = '$' == *++pSource;
	    if (anInternalSymbol)
		++pSource;
	    unsigned int sSymbol = strspn (
		pSource, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
	    );
	    sSymbol += strspn (
		pSource + sSymbol, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
	    );
	    if (sSymbol > 0) {
		VString iSymbol;
		iSymbol.setTo (pSource, sSymbol);
		pSource += sSymbol;

		if (!anInternalSymbol) {
		    VString iSymbolValue;
		    getValue (iSymbol, iSymbolValue);
		    rResult.quote (iSymbolValue);
		}
		else if (0 == strcasecmp (iSymbol, "class"))
		    rResult.append (rFormatter.getClass ());
		else if (0 == strcasecmp (iSymbol, "path"))
		    rResult.quote (m_iPath);
		else if (0 == strcasecmp (iSymbol, "query"))
		    rResult.quote (m_iQuery);
		else if (0 == strcasecmp (iSymbol, "environment"))
		    rResult.quote (environmentString ());
		else
		    rResult.quote ("");
	    }
	    else rResult.quote ("");
	}
    }
}

/*****  VA::PugiXML::Document Implementation  *****/

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

#include "va_pugixml_document.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"

#include "va_pugixml_node.h"


/***********************************
 ***********************************
 *****                         *****
 *****  VA::Pugixml::Document  *****
 *****                         *****
 ***********************************
 ***********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::Document);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::Document::Document (VString const &rFilename)  {
    m_iPugiParseResult = m_iPugiDocument.load_file (rFilename);
    setPugiNodeTo (m_iPugiDocument);
}


/************************************
 ************************************
 *****  Helper Implementations  *****
 ************************************
 ************************************/

/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

/********************
 *----  Status  ----*
 ********************/

void VA::PugiXML::Document::getParsedOK (Vxa::VResultBuilder &rRB) {
    rRB = parsedOK ();
}

void VA::PugiXML::Document::getParseStatus (Vxa::VResultBuilder &rRB) {
    rRB = static_cast<int>(parseStatus ());
}

void VA::PugiXML::Document::getParseDescription (Vxa::VResultBuilder &rRB) {
    rRB = parseDescription ();
}

namespace VA {
    namespace PugiXML {
	class DocumentClass : public Node::Class<Document> {
	    DECLARE_FAMILY_MEMBERS (DocumentClass, Node::Class<Document>);

	public:
	    DocumentClass () {
		defineMethod ("parsedOK", &Document::getParsedOK);
		defineMethod ("parseStatus", &Document::getParseStatus);
		defineMethod ("parseDescription", &Document::getParseDescription);

		defineHelp ("PugiXML::Document");
	    }
	} g_iDocumentClass;
    }
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::Document);

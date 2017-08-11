#pragma once
#ifndef VA_PugiXML_Document_Interface
#define VA_PugiXML_Document_Interface

/************************
 *****  Components  *****
 ************************/

//#include "Vxa_VCollectableObject.h"
#include "va_pugixml_node.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vxa {
    class VResultBuilder;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace PugiXML {
	class Document : public Node {
	    DECLARE_CONCRETE_RTTLITE (Document, Node);

	//  Aliases
	public:
	    typedef Vxa::VResultBuilder VResultBuilder;

	//  Construction
	public:
	    Document (VString const &rFilename);

	//  Destruction
	private:
	    ~Document () {
	    }

	//  Helpers
	public:
	    bool isEmpty () const {
		return m_iPugiDocument.empty ();
	    }
	    bool parsedOK () const {
		return m_iPugiParseResult;
	    }
	    pugi::xml_parse_status parseStatus () const {
		return m_iPugiParseResult.status;
	    }
	    char const *parseDescription () const {
		return m_iPugiParseResult.description ();
	    }

	//  Methods
	public:
	    void getParsedOK (Vxa::VResultBuilder &rRB);
	    void getParseStatus (Vxa::VResultBuilder &rRB);
	    void getParseDescription (Vxa::VResultBuilder &rRB);

	//  State
	private:
	    pugi::xml_document     m_iPugiDocument;
	    pugi::xml_parse_result m_iPugiParseResult;
	};
    }
}


#endif

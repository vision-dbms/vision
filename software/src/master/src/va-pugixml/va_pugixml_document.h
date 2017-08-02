#pragma once
#ifndef VA_PugiXML_Document_Interface
#define VA_PugiXML_Document_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

#include <pugixml.hpp>

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
	class Node;

	class Document : public Vxa::VCollectableObject {
	    DECLARE_CONCRETE_RTTLITE (Document, Vxa::VCollectableObject);

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

	private:
	    Node *asNode ();

	//  Methods
	public:
	    void getIsEmpty (Vxa::VResultBuilder &rRB);
	    void getParsedOK (Vxa::VResultBuilder &rRB);
	    void getParseStatus (Vxa::VResultBuilder &rRB);
	    void getParseDescription (Vxa::VResultBuilder &rRB);
		
	    //  Content
	    void getAttributeCount (Vxa::VResultBuilder &rRB);
	    void getChildCount (Vxa::VResultBuilder &rRB);

	    void getName (Vxa::VResultBuilder &rRB);
	    void getValue (Vxa::VResultBuilder &rRB);

	    void getChildValue (Vxa::VResultBuilder &rRB);
	    void getChildValueOf (Vxa::VResultBuilder &rRB, VString const &rChildName);

	    void getParent (Vxa::VResultBuilder &rRB);

	    void getChild (Vxa::VResultBuilder &rRB, int xChild);

	    void getFirstChild (Vxa::VResultBuilder &rRB);
	    void getLastChild (Vxa::VResultBuilder &rRB);
	    void getNextSibling (Vxa::VResultBuilder &rRB);
	    void getPreviousSibling (Vxa::VResultBuilder &rRB);

	    void getAttribute (Vxa::VResultBuilder &rRB, int xAttribute);

	    void getFirstAttribute (Vxa::VResultBuilder &rRB);
	    void getLastAttribute (Vxa::VResultBuilder &rRB);

	//  State
	private:
	    pugi::xml_document     m_iPugiDocument;
	    pugi::xml_parse_result m_iPugiParseResult;
	};
    }
}


#endif

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
}


/************************************
 ************************************
 *****  Helper Implementations  *****
 ************************************
 ************************************/

VA::PugiXML::Node *VA::PugiXML::Document::asNode () {
    return new Node (this, m_iPugiDocument);
}


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

/********************
 *----  Status  ----*
 ********************/

void VA::PugiXML::Document::getIsEmpty (Vxa::VResultBuilder &rRB) {
    rRB = isEmpty ();
}

void VA::PugiXML::Document::getParsedOK (Vxa::VResultBuilder &rRB) {
    rRB = parsedOK ();
}

void VA::PugiXML::Document::getParseStatus (Vxa::VResultBuilder &rRB) {
    rRB = static_cast<int>(parseStatus ());
}

void VA::PugiXML::Document::getParseDescription (Vxa::VResultBuilder &rRB) {
    rRB = parseDescription ();
}

/*********************
 *----  Content  ----*
 *********************/

void VA::PugiXML::Document::getAttributeCount (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getAttributeCount (rRB);
}
void VA::PugiXML::Document::getChildCount (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getChildCount (rRB);
}

void VA::PugiXML::Document::getName (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getName (rRB);
}
void VA::PugiXML::Document::getValue (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getValue (rRB);
}

void VA::PugiXML::Document::getChildValue (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getChildValue (rRB);
}
void VA::PugiXML::Document::getChildValueOf (Vxa::VResultBuilder &rRB, VString const &rChildName) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getChildValueOf (rRB, rChildName);
}

void VA::PugiXML::Document::getParent (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getParent (rRB);
}

void VA::PugiXML::Document::getChild (Vxa::VResultBuilder &rRB, int xChild) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getChild (rRB, xChild);
}

void VA::PugiXML::Document::getFirstChild (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getFirstChild (rRB);
}
void VA::PugiXML::Document::getLastChild (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getLastChild (rRB);
}
void VA::PugiXML::Document::getNextSibling (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getNextSibling (rRB);
}
void VA::PugiXML::Document::getPreviousSibling (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getPreviousSibling (rRB);
}

void VA::PugiXML::Document::getAttribute (Vxa::VResultBuilder &rRB, int xAttribute) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getAttribute (rRB, xAttribute);
}

void VA::PugiXML::Document::getFirstAttribute (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getFirstAttribute (rRB);
}
void VA::PugiXML::Document::getLastAttribute (Vxa::VResultBuilder &rRB) {
    Node::Reference const pThisNode (asNode ());
    pThisNode->getLastAttribute (rRB);
}

namespace VA {
    namespace PugiXML {
	class DocumentClass : public Vxa::VCollectable<Document> {
	public:
	    DocumentClass () {
		VString iHelpInfo ("The class PugiXML::Document supports the following methods:\nhelp\n");

		defineMethod ("isEmpty", &Document::getIsEmpty);
		
		defineMethod ("parsedOK", &Document::getParsedOK);
		defineMethod ("parseStatus", &Document::getParseStatus);
		defineMethod ("parseDescription", &Document::getParseDescription);

		defineMethod ("attributeCount", &Document::getAttributeCount);
		defineMethod ("childCount", &Document::getChildCount);

		defineMethod ("getName", &Document::getName);
		defineMethod ("getValue", &Document::getValue);

		defineMethod ("childValue", &Document::getChildValue);
		defineMethod ("childValueOf:", &Document::getChildValueOf);

		defineMethod ("parent", &Document::getParent);

		defineMethod ("getChild:", &Document::getChild);

		defineMethod ("firstChild", &Document::getFirstChild);
		defineMethod ("lastChild", &Document::getLastChild);
		defineMethod ("nextSibling", &Document::getNextSibling);
		defineMethod ("previousSibling", &Document::getPreviousSibling);

		defineMethod ("getAttribute:", &Document::getAttribute);

		defineMethod ("firstAttribute", &Document::getFirstAttribute);
		defineMethod ("lastAttribute", &Document::getLastAttribute);

		defineConstant ("help", (iHelpInfo << m_iHelpInfo));
	    }
	} g_iDocumentClass;
    }
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::Document);

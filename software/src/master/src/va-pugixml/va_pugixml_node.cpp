/*****  VA::PugiXML::Node Implementation  *****/

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

#include "va_pugixml_node.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"

#include "va_pugixml_attribute.h"
#include "va_pugixml_document.h"


/*******************************
 *******************************
 *****                     *****
 *****  VA::Pugixml::Node  *****
 *****                     *****
 *******************************
 *******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::Node);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::Node::Node (
    Document *pDocument, pugi::xml_node const &rPugiNode
) : m_pDocument (pDocument), m_iPugiNode (rPugiNode) {
}


/************************************
 ************************************
 *****  Helper Implementations  *****
 ************************************
 ************************************/

VA::PugiXML::count_t VA::PugiXML::Node::attributeCount () const {
    count_t count = 0;
    for (pugi::xml_attribute_iterator it = m_iPugiNode.attributes_begin(); it != m_iPugiNode.attributes_end(); ++it) {
	count++;
    }
    return count;
}

VA::PugiXML::count_t VA::PugiXML::Node::childCount () const {
    count_t count = 0;
    for (pugi::xml_node_iterator it = m_iPugiNode.begin(); it != m_iPugiNode.end(); ++it) {
	count++;
    }
    return count;
}

VA::PugiXML::Attribute *VA::PugiXML::Node::attribute (int const xAttribute) {
    count_t xCurrentAttribute = 0;
    for (pugi::xml_attribute_iterator it = m_iPugiNode.attributes_begin(); it != m_iPugiNode.attributes_end(); ++it, ++xCurrentAttribute) {
	if (xCurrentAttribute == xAttribute)
	    return new Attribute (this, *it);
    }
    pugi::xml_attribute iEmptyAttribute;
    return new Attribute (this, iEmptyAttribute);
}

VA::PugiXML::Node *VA::PugiXML::Node::child (int const xChild) {
    count_t xCurrentChild = 0;
    for (pugi::xml_node_iterator it = m_iPugiNode.begin(); it != m_iPugiNode.end(); ++it, ++xCurrentChild) {
	if (xCurrentChild == xChild)
	    return new Node (m_pDocument, *it);
    }
    pugi::xml_node iEmptyNode;
    return new Node (m_pDocument, iEmptyNode);
}

void VA::PugiXML::Node::returnNonEmpty (Vxa::VResultBuilder &rRB, pugi::xml_attribute const &rPugiAttribute) {
    //    if (!rPugiAttribute.empty ())
	rRB = new Attribute (this, rPugiAttribute);
}

void VA::PugiXML::Node::returnNonEmpty (Vxa::VResultBuilder &rRB, pugi::xml_node const &rPugiNode) {
    //    if (!rPugiNode.empty ())
	rRB = new Node (m_pDocument, rPugiNode);
}


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

/********************
 *----  Status  ----*
 ********************/

void VA::PugiXML::Node::getIsEmpty (Vxa::VResultBuilder &rRB) {
    rRB = isEmpty ();
}

/*********************
 *----  Content  ----*
 *********************/

void VA::PugiXML::Node::getAttributeCount (Vxa::VResultBuilder &rRB) {
    rRB = attributeCount ();
}
void VA::PugiXML::Node::getChildCount (Vxa::VResultBuilder &rRB) {
    rRB = childCount ();
}

void VA::PugiXML::Node::getName (Vxa::VResultBuilder &rRB) {
    rRB = m_iPugiNode.name ();
}
void VA::PugiXML::Node::getValue (Vxa::VResultBuilder &rRB) {
    rRB = m_iPugiNode.value ();
}

void VA::PugiXML::Node::getChildValue (Vxa::VResultBuilder &rRB) {
    rRB = m_iPugiNode.child_value ();
}
void VA::PugiXML::Node::getChildValueOf (Vxa::VResultBuilder &rRB, VString const &rChildName) {
    rRB = m_iPugiNode.child_value (rChildName);
}

void VA::PugiXML::Node::getParent (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.parent ());
}

void VA::PugiXML::Node::getChild (Vxa::VResultBuilder &rRB, int xChild) {
    rRB = child (xChild);
}

void VA::PugiXML::Node::getFirstChild (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.first_child ());
}
void VA::PugiXML::Node::getLastChild (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.last_child ());
}
void VA::PugiXML::Node::getNextSibling (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.next_sibling ());
}
void VA::PugiXML::Node::getPreviousSibling (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.previous_sibling ());
}

void VA::PugiXML::Node::getAttribute (Vxa::VResultBuilder &rRB, int xAttribute) {
    rRB = attribute (xAttribute);
}

void VA::PugiXML::Node::getFirstAttribute (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.first_attribute ());
}
void VA::PugiXML::Node::getLastAttribute (Vxa::VResultBuilder &rRB) {
    returnNonEmpty (rRB, m_iPugiNode.last_attribute ());
}

namespace VA {
    namespace PugiXML {
	class NodeClass : public Vxa::VCollectable<Node> {
	public:
	    NodeClass () {
		VString iHelpInfo ("The class PugiXML::Node supports the following methods:\nhelp\n");

		defineMethod ("isEmpty", &Node::getIsEmpty);
		
		defineMethod ("attributeCount", &Node::getAttributeCount);
		defineMethod ("childCount", &Node::getChildCount);

		defineMethod ("getName", &Node::getName);
		defineMethod ("getValue", &Node::getValue);

		defineMethod ("childValue", &Node::getChildValue);
		defineMethod ("childValueOf:", &Node::getChildValueOf);

		defineMethod ("parent", &Node::getParent);

		defineMethod ("getChild:", &Node::getChild);

		defineMethod ("firstChild", &Node::getFirstChild);
		defineMethod ("lastChild", &Node::getLastChild);
		defineMethod ("nextSibling", &Node::getNextSibling);
		defineMethod ("previousSibling", &Node::getPreviousSibling);

		defineMethod ("getAttribute:", &Node::getAttribute);

		defineMethod ("firstAttribute", &Node::getFirstAttribute);
		defineMethod ("lastAttribute", &Node::getLastAttribute);

		defineConstant ("help", (iHelpInfo << m_iHelpInfo));
	    }
	} g_iNodeClass;
    }
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::Node);

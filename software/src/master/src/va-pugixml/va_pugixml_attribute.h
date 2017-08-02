#pragma once
#ifndef VA_PugiXML_Attribute_Interface
#define VA_PugiXML_Attribute_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_pugixml_node.h"

namespace Vxa {
    class VResultBuilder;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace PugiXML {
	class Attribute : public Vxa::VCollectableObject {
	    DECLARE_CONCRETE_RTTLITE (Attribute, Vxa::VCollectableObject);

	//  Aliases
	public:
	    typedef Vxa::VResultBuilder VResultBuilder;

	//  Construction
	public:
	    Attribute (Node *pNode, pugi::xml_attribute const &rPugiAttribute);

	//  Destruction
	private:
	    ~Attribute () {
	    }

	//  Helpers
	private:
	    void returnNonEmpty (Vxa::VResultBuilder &rRB, pugi::xml_attribute const &rPugiAttribute);

	//  Methods
	public:
	    void getName (Vxa::VResultBuilder &rRB);
	    void getValue (Vxa::VResultBuilder &rRB);

	    void getNextSibling (Vxa::VResultBuilder &rRB);
	    void getPreviousSibling (Vxa::VResultBuilder &rRB);

	//  State
	private:
	    Node::Reference const m_pNode;
	    pugi::xml_attribute   m_iPugiAttribute;
	};
    }
}


#endif

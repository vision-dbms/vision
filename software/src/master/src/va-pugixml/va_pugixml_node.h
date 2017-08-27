#pragma once
#ifndef VA_PugiXML_Node_Interface
#define VA_PugiXML_Node_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"
#include "Vxa_VCollectable.h"

#include "pugixml.hpp"

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
	typedef unsigned int count_t;

	class Attribute; class Document;

	class Node : public Vxa::VCollectableObject {
	    DECLARE_CONCRETE_RTTLITE (Node, Vxa::VCollectableObject);

	//  Class Builder
	public:
	    class ClassBuilder : public Object::ClassBuilder {
	    protected:
		ClassBuilder (Vxa::VClass *pClass);
	    };

	//  Construction
	public:
	    Node (Document *pDocument, pugi::xml_node const &rPugiNode);
	protected:
	    Node ();
	    void setPugiNodeTo (pugi::xml_node const &rPugiNode);

	//  Destruction
	protected:
	    ~Node ();

	//  Helpers
	public:
	    bool isEmpty () const {
		return m_iPugiNode.empty ();
	    }

	    count_t attributeCount () const;
	    count_t childCount () const;

	    Attribute *attribute (int const xAttribute);
	    Node *child (int const xChild);

	private:
	    void returnNonEmpty (Vxa::VResultBuilder &rRB, pugi::xml_attribute const &rPugiAttribute);
	    void returnNonEmpty (Vxa::VResultBuilder &rRB, pugi::xml_node const &iPugiNode);

	//  Methods
	public:
	    void getIsEmpty (Vxa::VResultBuilder &rRB);

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
	    VReference<Document> const m_pDocument;
	    pugi::xml_node             m_iPugiNode;
	};
    }
}


#endif

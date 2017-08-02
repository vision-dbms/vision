/*****  VA::PugiXML::Root Implementation  *****/

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

#include "va_pugixml_root.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/*******************************
 *******************************
 *****                     *****
 *****  VA::Pugixml::Root  *****
 *****                     *****
 *******************************
 *******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::Root);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::Root::Root () {
}


/*******************************
 *******************************
 *****  Document Creation  *****
 *******************************
 *******************************/


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

void VA::PugiXML::Root::loadDocument (VResultBuilder &rRB, VString const &rFilename) {
    rRB = new Document (rFilename);
}

namespace VA {
    namespace PugiXML {
	class RootClass : public Vxa::VCollectable<Root> {
	public:
	    RootClass () {
		VString iHelpInfo ("The class PugiXML::Root supports the following methods:\nhelp\n");

		defineMethod   ("load:", &Root::loadDocument);

		defineConstant ("help", (iHelpInfo << m_iHelpInfo));
	    }
	} g_iRootClass;
    }
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::Root);

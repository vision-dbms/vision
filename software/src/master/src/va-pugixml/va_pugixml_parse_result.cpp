/*****  VA::PugiXML::ParseResult Implementation  *****/

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

#include "va_pugixml_parse_result.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/**************************************
 **************************************
 *****                            *****
 *****  VA::Pugixml::ParseResult  *****
 *****                            *****
 **************************************
 **************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::ParseResult);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::ParseResult::ParseResult () {
}


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

// void VA::PugiXML::ParseResult::getCalendar (VResultBuilder &rRB) {
//     m_pFQLFetchObject->getCalendar (rRB);
// }

// void VA::PugiXML::ParseResult::setCalendar_(VResultBuilder &rRB, VString const &rCalender) {
//     m_pFQLFetchObject->setCalendar_(rRB, rCalender);
// }

namespace VA {
    namespace PugiXML {
	class ParseResultClass : public Vxa::VCollectable<ParseResult> {
	public:
	    ParseResultClass () {
		VString iHelpInfo ("The class PugiXML::ParseResult supports the following methods:\nhelp\n");

		defineConstant ("help", (iHelpInfo << m_iHelpInfo));
	    }
	} g_iParseResultClass;
    }
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::ParseResult);

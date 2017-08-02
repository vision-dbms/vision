#pragma once
#ifndef VA_PugiXML_ParseResult_Interface
#define VA_PugiXML_ParseResult_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

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
	class ParseResult : public Vxa::VCollectableObject {
	    DECLARE_CONCRETE_RTTLITE (ParseResult, Vxa::VCollectableObject);

	//  Aliases
	public:
	    typedef Vxa::VResultBuilder VResultBuilder;

	//  Construction
	public:
	    ParseResult ();

	//  Destruction
	private:
	    ~ParseResult () {
	    }

	//  Methods
	public:
//	    void getCalendar	(VResultBuilder &rRB);
//	    void setCalendar_ 	(VResultBuilder &rRB, VString const & rCalender);

	//  State
	private:
	};
    }
}


#endif

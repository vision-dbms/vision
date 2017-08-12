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

	//  Class Builder
	public:
	    class ClassBuilder : public Object::ClassBuilder {
	    protected:
		ClassBuilder (Vxa::VClass *pClass);
	    };

	//  Construction
	public:
	    ParseResult ();

	//  Destruction
	private:
	    ~ParseResult () {
	    }

	//  Methods
	public:

	//  State
	private:
	};
    }
}


#endif

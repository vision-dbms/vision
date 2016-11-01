#ifndef Vxa_VTaskCursor_Interface
#define Vxa_VTaskCursor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCursor.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VTaskCursor : public VCursor {
	DECLARE_CONCRETE_RTTLITE (VTaskCursor, VCursor);

    //  Construction
    public:
	VTaskCursor (VFiniteSet *pCodomain);

    //  Destruction
    private:
	~VTaskCursor ();

    //  Query
    public:
	bool isAlive () const {
	    return m_bAlive;
	}
	bool moreToDo () const {
	    return m_bAlive && BaseClass::inBounds ();
	}

    //  Update
    public:
	void kill () {
	    m_bAlive = false;
	}

    //  State
    private:
	bool m_bAlive;
    };
}


#endif

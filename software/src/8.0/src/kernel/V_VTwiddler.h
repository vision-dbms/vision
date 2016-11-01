#ifndef V_VTwiddler_Interface
#define V_VTwiddler_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAtomic.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /** A thread-safe boolean. */
    class V_API VTwiddler : public VAtomic_bool {
	DECLARE_FAMILY_MEMBERS (VTwiddler, VAtomic_bool);

    //  Construction
    public:
	VTwiddler (ThisClass const &rOther) : BaseClass (rOther) {
	}
	VTwiddler (bool bValue = false) : BaseClass (bValue) {
	}

    //  Destruction
    public:
	~VTwiddler () {
	}

    //  Access
    public:
	bool isSet () const {
	    return value ();
	}
	bool isClear () const {
	    return !value ();
	}

    //  Update
    public:
	bool clearIfSet () {
	    return setIf (true, false);
	}
	bool setIfClear () { // this isn't atomic but it could be
	    return setIf (false, true);
	}

	ThisClass &operator= (bool bValue) {
	    setTo (bValue);
	    return *this;
	}
	void set () {
	    setTo (true);
	}
	void clear () {
	    setTo (false);
	}
    };
}


#endif

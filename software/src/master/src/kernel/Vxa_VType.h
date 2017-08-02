#ifndef Vxa_VType_Interface
#define Vxa_VType_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VType : virtual public VTransient {
	DECLARE_FAMILY_MEMBERS (VType, VTransient);

    //  Construction
    protected:
	VType ();

    //  Destruction
    protected:
	~VType ();

    //  Description
    protected:
	virtual VString &getDescription_(VString &rResult) const;
    public:
	VString &getDescription (VString &rResult) const {
	    return getDescription_(rResult);
	}
    };
} //  namespace Vxa


#endif

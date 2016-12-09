#ifndef Vxa_VType_Interface
#define Vxa_VType_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VStaticTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VType : virtual public VStaticTransient {
	DECLARE_FAMILY_MEMBERS (VType, VStaticTransient);

    //  Construction
    protected:
	VType ();

    //  Destruction
    protected:
	~VType ();

    //  Initialization
    protected:
	void initialize ();

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

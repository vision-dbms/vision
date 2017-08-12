#ifndef Vxa_VMethod_Interface
#define Vxa_VMethod_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType1;
    class VCallType2;

    class VCollection;

    class Vxa_API VMethod : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VMethod, VReferenceable);

	friend class VCallType1;
	friend class VCallType2;

    //  Construction
    protected:
	VMethod (VString const &rName);
	VMethod (char const *pName);

    //  Destruction
    protected:
	~VMethod ();

    //  Access
    public:
	VString const &name () const {
	    return m_iName;
	}

    //  Invocation
    private:
	virtual bool invoke (VCallType1 const &rCallHandle, VCollection *pCollection) = 0;
	virtual bool invoke (VCallType2 const &rCallHandle, VCollection *pCollection) = 0;

    //  State
    private:
	VString const m_iName;
    };
    typedef VMethod::Reference method_return_t;
} //  namespace Vxa


#endif

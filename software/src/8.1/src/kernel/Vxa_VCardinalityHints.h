#ifndef Vxa_VCardinalityHints_Interface
#define Vxa_VCardinalityHints_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCardinalityHints : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VCardinalityHints, VReferenceable);

    //  Construction
    protected:
	VCardinalityHints ();

    //  Destruction
    protected:
	~VCardinalityHints ();

    //  Access
    protected:
	virtual cardinality_t lowerBound_() const;
	virtual cardinality_t upperBound_() const = 0;
    public:
	cardinality_t lowerBound () const {
	    return lowerBound_();
	}
	cardinality_t upperBound () const {
	    return upperBound_();
	}
	cardinality_t span (cardinality_t sMinimumSpan = 0) const;
    };
}


#endif

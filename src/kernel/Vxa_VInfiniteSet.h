#ifndef Vxa_VInfiniteSet_Interface
#define Vxa_VInfiniteSet_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VInfiniteSet : public VSet {
	DECLARE_CONCRETE_RTTLITE (VInfiniteSet, VSet);

    //  Construction
    public:
	VInfiniteSet ();

    //  Destruction
    protected:
	~VInfiniteSet ();

    //  Cardinality
    private:
	cardinality_t cardinality_() const {
	    return cardinality ();
	}
    public:
	cardinality_t cardinality () const {
	    return ~static_cast<cardinality_t>(0);
	}

    //  Transmission
    protected:
	bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection);
	bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker);
    };
}


#endif

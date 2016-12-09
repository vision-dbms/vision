#ifndef Vxa_VSet_Interface
#define Vxa_VSet_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType2Exporter;
    class VMonotypeMapMaker;

    class Vxa_API VSet : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE(VSet, VRolePlayer);

    //  Construction
    protected:
	VSet ();

    //  Destruction
    protected:
	~VSet ();

    //  Cardinality
    private:
	virtual cardinality_t cardinality_() const = 0;
    public:
	cardinality_t cardinality () const {
	    return cardinality_();
	}

    //  Description
    protected:
	virtual VString &getDescription_(VString &rResult) const;
    public:
	VString &getDescription (VString &rResult) const {
	    return getDescription_(rResult);
	}

    //  Transmission
    protected:
	virtual bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection);
	virtual bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker);
    public:
	virtual bool transmitUsing (VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection);
	virtual bool transmitUsing (VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker);
    };
}


#endif

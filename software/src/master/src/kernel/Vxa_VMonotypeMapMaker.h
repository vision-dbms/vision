#ifndef Vxa_VMonotypeMapMaker_Interface
#define Vxa_VMonotypeMapMaker_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMapMaker.h"
#include "Vxa_VSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCollection;

    class Vxa_API VMonotypeMapMaker : public VMapMaker::Implementation {
	DECLARE_ABSTRACT_RTTLITE (VMonotypeMapMaker,VMapMaker::Implementation);

    //  Construction
    protected:
	VMonotypeMapMaker (VCardinalityHints *pTailHints, VSet *pCodomain);

    //  Destruction
    protected:
	~VMonotypeMapMaker ();

    //  Codomain Access
    public:
	VSet *codomain () const {
	    return m_pCodomain;
	}

    //  Codomain Management
    public:
	VMapMaker &getMapMakerFor (VMapMaker &rParent, cardinality_t xElement, VSet *pCodomain);

    //  Debugging
    protected:
	template <typename val_t> void show (val_t iValue) const {
	}
	void show (bool bValue) const;
	void show (int iValue) const;
	void show (unsigned int iValue) const;
	void show (float iValue) const;
	void show (double iValue) const;
	void show (char const *pValue) const;
	void show (VString iValue) const;

    //  Transmission
    protected:
	bool transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection);
	bool transmitUsing (VCallType2Exporter *pExporter);
    public:
	bool transmitValues (VCallType2Exporter *pExporter, VCollection *pCluster, object_reference_array_t const &rInjection) {
	    return transmitValues_(pExporter, pCluster, rInjection);
	}
	bool transmitValues (VCallType2Exporter *pExporter, VCollection *pCluster) {
	    return transmitValues_(pExporter, pCluster);
	}

	bool transmitValues (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) {
	    return transmitValues_(pExporter, rInjection);
	}
	bool transmitValues (VCallType2Exporter *pExporter) {
	    return transmitValues_(pExporter);
	}
    private:
	virtual bool transmitValues_(VCallType2Exporter *pExporter, VCollection *pCluster, object_reference_array_t const &rInjection) = 0;
	virtual bool transmitValues_(VCallType2Exporter *pExporter, VCollection *pCluster) = 0;

	virtual bool transmitValues_(VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) = 0;
	virtual bool transmitValues_(VCallType2Exporter *pExporter) = 0;

    //  State
    private:
	VCardinalityHints::Reference const m_pTailHints;
	VSet::Reference const m_pCodomain;
    };
}


#endif

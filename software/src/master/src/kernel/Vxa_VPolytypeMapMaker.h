#ifndef Vxa_VPolytypeMapMaker_Interface
#define Vxa_VPolytypeMapMaker_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMapMaker.h"

#include "Vxa_VSet.h"

#include "VkSetOfFactoryReferences.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VMonotypeMapMakerFor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VPolytypeMapMaker : public VMapMaker::Implementation {
	DECLARE_CONCRETE_RTTLITE (VPolytypeMapMaker, VMapMaker::Implementation);

    //  Injector
    public:
	class Injector : public VMonotypeMapMakerFor<cardinality_t> {
	    DECLARE_CONCRETE_RTTLITE (Injector, VMonotypeMapMakerFor<cardinality_t>);

	//  Construction
	public:
	    Injector (VCardinalityHints *pTailHints, cardinality_t xInjectionOrigin, cardinality_t xInjectionLimit);

	//  Destruction
	protected:
	    ~Injector ();

	//  Transmission
	public:
	    bool transmitSegmentUsing (VCallType2Exporter *pExporter, cardinality_t &rcSegments);

	//  Update
	public:
	    void appendRange (cardinality_t xOrigin, cardinality_t xLimit);
	};

    //  Segment
    public:
	class Segment : public Injector {
	    DECLARE_CONCRETE_RTTLITE (Segment, Injector);

	//  Segment Key
	public:
	    typedef VSet const *Key;

	//  Segment Initializer
	public:
	    class Initializer;

	//  Construction
	public:
	    Segment (Initializer const &rInitializer);
	    Segment (VMonotypeMapMaker *pChild);

	//  Destruction
	private:
	    ~Segment ();

	//  Domain Comparision (used by factory reference)
	public:
	    bool operator< (Key iKey) const {
		return m_pCodomain < iKey;
	    }
	    bool operator<= (Key iKey) const {
		return m_pCodomain <= iKey;
	    }
	    bool operator== (Key iKey) const {
		return m_pCodomain == iKey;
	    }
	    bool operator!= (Key iKey) const {
		return m_pCodomain != iKey;
	    }
	    bool operator>= (Key iKey) const {
		return m_pCodomain >= iKey;
	    }
	    bool operator> (Key iKey) const {
		return m_pCodomain > iKey;
	    }

	//  Access
	public:
	    VSet *codomain () const {
		return m_pCodomain;
	    }
	    VMapMaker const &mapMaker () const {
		return m_iMapMaker;
	    }
	    VMapMaker &mapMaker () {
		return m_iMapMaker;
	    }

	//  Update
	public:
	    bool updatedThroughInjectionPoint (cardinality_t xInjectionPoint);
	    void setInjectionPointTo (cardinality_t xInjectionPoint);

	//  Transmission
	public:
	    bool transmitSegmentUsing (VCallType2Exporter *pExporter, cardinality_t &rcSegments);

	//  State
	private:
	    VSet::Reference const m_pCodomain;
	    VMapMaker             m_iMapMaker;
	    cardinality_t         m_xInjectionPoint;
	};

    //  Construction
    public:
	VPolytypeMapMaker (VCardinalityHints *pTailHints, cardinality_t xCommitLimit);
	VPolytypeMapMaker (VMonotypeMapMaker *pChild);

    //  Destruction
    private:
	~VPolytypeMapMaker ();

    //  Implementation
    protected:
	bool commitThrough (cardinality_t xElement) OVERRIDE;
    protected:
	VMapMaker &getMapMakerFor (VMapMaker &rParent, cardinality_t xElement, VSet *pCodomain) OVERRIDE;
    private:
	VMapMaker &getMapMakerFor (cardinality_t xElement, VSet *pCodomain);

    //  Transmission
    protected:
	bool transmitUsing (VCallType2Exporter *pExporter) OVERRIDE;

    //  State
    private:
	VkSetOfFactoryReferences<Segment> m_aSegments;
	Segment::Reference                m_pCurrentSegment;
	Injector::Reference               m_pMissingInjector;
    };
}


#endif

/*****  Vxa_VPolytypeMap Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vxa_VPolytypeMapMaker.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType2Exporter.h"
#include "Vxa_VMonotypeMapMaker.h"

namespace {
    bool const s_bDebugging = getenv ("VxaPolytrace") ? true : false;
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  Vxa::VPolytypeMapMaker::Injector  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VPolytypeMapMaker::Injector::Injector (
    VCardinalityHints *pTailHints, cardinality_t xInjectionOrigin, cardinality_t xInjectionLimit
) : BaseClass (pTailHints, 0, xInjectionOrigin) {
    appendRange (xInjectionOrigin, xInjectionLimit);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VPolytypeMapMaker::Injector::~Injector () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VPolytypeMapMaker::Injector::transmitSegmentUsing (VCallType2Exporter *pExporter, cardinality_t &rcSegments) {
    if (isEmpty ())
	return true;

    if (pExporter->returnSegment (trimmedContainer ())) {
	rcSegments++;
	return true;
    }

    return false;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vxa::VPolytypeMapMaker::Injector::appendRange (cardinality_t xOrigin, cardinality_t xLimit) {
    if (s_bDebugging)
	fprintf (stderr, "VInjectorMapMaker[%p]::appendRange (%u,%u)\n", this, xOrigin, xLimit);
    while (xOrigin < xLimit)
	append (xOrigin++);
}


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vxa::VPolytypeMapMaker::Segment::Initializer  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

namespace Vxa {
    class VPolytypeMapMaker::Segment::Initializer {
	DECLARE_NUCLEAR_FAMILY (Initializer);

    //  Construction
    public:
	Initializer (VCardinalityHints *pTailHints, VSet *pCodomain, cardinality_t xOrigin);
	Initializer (VMonotypeMapMaker *pChild);

    //  Destruction
    public:
	~Initializer ();

    //  Access
    public:
	operator Key () const {
	    return codomain ();
	}
	VMonotypeMapMaker *child () const {
	    return m_pChild;
	}
	VSet *codomain () const {
	    return m_pCodomain;
	}
	cardinality_t origin () const {
	    return m_xOrigin;
	}
	cardinality_t limit () const {
	    return m_pChild ? m_xOrigin + m_pChild->commitLimit () : m_xOrigin;
	}
	VCardinalityHints *tailHints () const {
	    return m_pTailHints;
	}

    //  Query
    public:
	bool withChild () const {
	    return m_pChild.isntNil ();
	}

    //  State
    private:
	VMonotypeMapMaker::Reference const m_pChild;
	VCardinalityHints::Reference const m_pTailHints;
	VSet::Reference              const m_pCodomain;
	cardinality_t                const m_xOrigin;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VPolytypeMapMaker::Segment::Initializer::Initializer (
    VCardinalityHints *pTailHints, VSet *pCodomain, cardinality_t xOrigin
) : m_pTailHints (pTailHints), m_pCodomain (pCodomain), m_xOrigin (xOrigin) {
}

Vxa::VPolytypeMapMaker::Segment::Initializer::Initializer (
    VMonotypeMapMaker *pChild
) : m_pChild (pChild), m_pTailHints (pChild->tailHints ()), m_pCodomain (pChild->codomain ()), m_xOrigin (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VPolytypeMapMaker::Segment::Initializer::~Initializer () {
}


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vxa::VPolytypeMapMaker::Segment  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VPolytypeMapMaker::Segment::Segment (
    Initializer const &rInitializer
) : BaseClass (rInitializer.tailHints (), rInitializer.origin (), rInitializer.limit ()), m_pCodomain (rInitializer.codomain ()) {
    if (rInitializer.withChild ())
	m_iMapMaker.setImplementationTo (rInitializer.child ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VPolytypeMapMaker::Segment::~Segment () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VPolytypeMapMaker::Segment::transmitSegmentUsing (VCallType2Exporter *pExporter, cardinality_t &rcSegments) {
    if (isEmpty ())
	return true;

    if (m_iMapMaker.transmitUsing (pExporter, trimmedContainer ())) {
	rcSegments++;
	return true;
    }

    return false;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vxa::VPolytypeMapMaker::Segment::updatedThroughInjectionPoint (cardinality_t xInjectionPoint) {
    if (xInjectionPoint <= m_xInjectionPoint || m_iMapMaker.isntPending ())
	return false;

    m_iMapMaker.commitUpdate ();

    setCurrentValueTo (m_xInjectionPoint);
    commitUpdate ();

    return true;
}

void Vxa::VPolytypeMapMaker::Segment::setInjectionPointTo (cardinality_t xInjectionPoint) {
    m_iMapMaker.retractUpdate ();
    m_xInjectionPoint = xInjectionPoint;
}


/************************************
 ************************************
 *****                          *****
 *****  Vxa::VPolytypeMapMaker  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VPolytypeMapMaker::VPolytypeMapMaker (
    VCardinalityHints *pTailHints, cardinality_t xCommitLimit
) : BaseClass (pTailHints, xCommitLimit) {
}

Vxa::VPolytypeMapMaker::VPolytypeMapMaker (VMonotypeMapMaker *pChild) : BaseClass (pChild->tailHints (), pChild->commitLimit ()) {
    Segment::Initializer iSegmentInitializer (pChild);
    unsigned int xSegment;
    m_aSegments.Insert (iSegmentInitializer, xSegment);
    m_pCurrentSegment.setTo (m_aSegments[xSegment]);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VPolytypeMapMaker::~VPolytypeMapMaker () {
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

bool Vxa::VPolytypeMapMaker::commitThrough (cardinality_t xElement) {
    if (s_bDebugging)
	fprintf (stderr, "VPolytypeMapMaker[%p]::commitThrough (%u): %u/%u\n", this, xElement, commitLimit (), updateLimit ());

//  If the current segment has an uncommited value at position < xElement, commit it and note the update, ...
    if (m_pCurrentSegment && m_pCurrentSegment->updatedThroughInjectionPoint (xElement))
	noteUpdate ();

//  If this map maker is complete through 'xElement', there's nothing more to do, ...
    if (BaseClass::commitThrough (xElement))
	return true;

//  ... but, if it isn't, create an 'empty' segment to complete it through 'xElement', ...
    if (m_pMissingInjector)
	m_pMissingInjector->appendRange (commitLimit (), xElement);
    else
	m_pMissingInjector.setTo (new Injector (tailHints (), commitLimit (), xElement));

    do {
	noteUpdate ();
	commitUpdate ();
    } while (xElement > commitLimit ());

    return true;
}

Vxa::VMapMaker &Vxa::VPolytypeMapMaker::getMapMakerFor (
    VMapMaker &rParent, cardinality_t xElement, VSet *pCodomain
) {
    return getMapMakerFor (xElement, pCodomain);
}

Vxa::VMapMaker &Vxa::VPolytypeMapMaker::getMapMakerFor (cardinality_t xElement, VSet *pCodomain) {
    if (s_bDebugging)
	fprintf (stderr, "VPolytypeMapMaker[%p]::getMapMakerFor (%u, %p): %u/%u\n", this, xElement, pCodomain, commitLimit (), updateLimit ());

//  Commit this map through 'xElement', ...
    commitThrough (xElement);

//  ... find or create a segment for the requested type, ...
    Segment::Initializer const iSegmentInitializer (tailHints (), pCodomain, xElement); unsigned int xSegment;
    m_aSegments.Insert (iSegmentInitializer, xSegment);
    m_pCurrentSegment.setTo (m_aSegments[xSegment]);

//  ... set its current injection point to the current element, ...
    m_pCurrentSegment->setInjectionPointTo (xElement);

//  ... and return its map maker:
    return m_pCurrentSegment->mapMaker ();
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VPolytypeMapMaker::transmitUsing (VCallType2Exporter *pExporter) {
//  Complete the map, ...
    commitThrough (pExporter->cardinality ());

//  ... send the segments, ...
    cardinality_t cSegmentsSent = 0;
    for (unsigned int xSegment = 0; xSegment < m_aSegments.cardinality (); xSegment++) {
	if (!m_aSegments[xSegment]->transmitSegmentUsing (pExporter, cSegmentsSent))
	    return false;
    }

//  ... send NA's if appropriate, ...
    if (m_pMissingInjector) {
    //  Send the injector...
	if (!m_pMissingInjector->transmitSegmentUsing (pExporter, cSegmentsSent))
	    return false;
    }

//  ... and send the number of segments sent:
    return pExporter->returnSegmentCount (cSegmentsSent);
}

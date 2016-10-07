/*****  Vxa_VMonotypeMapMaker Implementation  *****/

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

#include "Vxa_VMonotypeMapMaker.h"

/************************
 *****  Supporting  *****
 ************************/

namespace {
    bool const s_bDebugging = getenv ("VxaPolytrace") ? true : false;
}


/************************************
 ************************************
 *****                          *****
 *****  Vxa::VMonotypeMapMaker  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VMonotypeMapMaker::VMonotypeMapMaker (
    VCardinalityHints *pTailHints, VSet *pCodomain
) : BaseClass (pTailHints), m_pCodomain (pCodomain) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VMonotypeMapMaker::~VMonotypeMapMaker () {
}

/*********************************
 *********************************
 *****  Codomain Management  *****
 *********************************
 *********************************/

Vxa::VMapMaker &Vxa::VMonotypeMapMaker::getMapMakerFor (
    VMapMaker &rParent, cardinality_t xElement, VSet *pCodomain
) {
    return commitThrough (xElement) && pCodomain == m_pCodomain
	? rParent
	: rParent.getMapMakerFor (xElement, pCodomain, this);
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VMonotypeMapMaker::transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) {
    commitUpdate ();
    return m_pCodomain && m_pCodomain->transmitUsing (pExporter, this, rInjection);
}

bool Vxa::VMonotypeMapMaker::transmitUsing (VCallType2Exporter *pExporter) {
    commitUpdate ();
    return m_pCodomain && m_pCodomain->transmitUsing (pExporter, this);
}

/***********************
 ***********************
 *****  Debugging  *****
 ***********************
 ***********************/

void Vxa::VMonotypeMapMaker::show (bool bValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: (%s) at: %u/%u\n", this, bValue ? "T" : "F", commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (int iValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: (%d) at: %u/%u\n", this, iValue, commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (unsigned int iValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: (%u) at: %u/%u\n", this, iValue, commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (float iValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: F:%g at: %u/%u\n", this, iValue, commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (double iValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: D:%g at: %u/%u\n", this, iValue, commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (char const *pValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: (%s) at: %u/%u\n", this, pValue, commitLimit (), updateLimit ());
}

void Vxa::VMonotypeMapMaker::show (VString iValue) const {
    if (s_bDebugging)
	fprintf (stderr, "VMonotypeMapMaker[%p]: (%s) at: %u/%u\n", this, iValue.content (), commitLimit (), updateLimit ());
}

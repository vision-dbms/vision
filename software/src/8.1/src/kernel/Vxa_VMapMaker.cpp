/*****  Vxa_VMapMaker Implementation  *****/

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

#include "Vxa_VMapMaker.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType2Exporter.h"
#include "Vxa_VPolytypeMapMaker.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vxa::VMapMaker::Implementation  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VMapMaker::Implementation::Implementation (
    VCardinalityHints *pTailHints, cardinality_t xCommitLimit
) : m_pTailHints (pTailHints), m_xCommitLimit (xCommitLimit), m_xUpdateLimit (xCommitLimit) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VMapMaker::Implementation::~Implementation () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VMapMaker::Implementation::transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) {
    return pExporter->returnError ("Unimplemented Map");
}

bool Vxa::VMapMaker::Implementation::transmitUsing (VCallType2Exporter *pExporter) {
    return pExporter->returnError ("Unimplemented Map");
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vxa::VMapMaker::Implementation::commitThrough (cardinality_t xElement) {
    if (m_xCommitLimit < xElement) {
	m_xCommitLimit = xElement < m_xUpdateLimit ? xElement : m_xUpdateLimit;
    }
    return xElement <= m_xCommitLimit;
}


/****************************
 ****************************
 *****                  *****
 *****  Vxa::VMapMaker  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VMapMaker::VMapMaker () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VMapMaker::~VMapMaker () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VMapMaker::transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) const {
    return m_pImplementation && m_pImplementation->transmitUsing (pExporter, rInjection);
}

bool Vxa::VMapMaker::transmitUsing (VCallType2Exporter *pExporter) const {
    return m_pImplementation && m_pImplementation->transmitUsing (pExporter);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vxa::VMapMaker::commitUpdate () const {
    return m_pImplementation && m_pImplementation->commitUpdate ();
}

void Vxa::VMapMaker::retractUpdate () const {
    if (m_pImplementation)
	m_pImplementation->retractUpdate ();
}

Vxa::VMapMaker &Vxa::VMapMaker::getMapMakerFor (cardinality_t xElement, VSet *pCodomain, VMonotypeMapMaker *pChild) {
    m_pImplementation.setTo (new VPolytypeMapMaker (pChild));
    return m_pImplementation->getMapMakerFor (*this, xElement, pCodomain);
}

Vxa::VMapMaker &Vxa::VMapMaker::getMapMakerFor (cardinality_t xElement, VSet *pCodomain, VCardinalityHints *pTailHints) {
    if (m_pImplementation.isNil () && xElement > 0)
	m_pImplementation.setTo (new VPolytypeMapMaker (pTailHints, xElement));
    return m_pImplementation ? m_pImplementation->getMapMakerFor (*this, xElement, pCodomain) : *this;
}

void Vxa::VMapMaker::setImplementationTo (Implementation *pImplementation) {
    m_pImplementation.setIfNil (pImplementation);
}

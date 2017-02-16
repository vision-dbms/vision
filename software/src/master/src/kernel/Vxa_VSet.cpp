/*****  Vxa_VSet Implementation  *****/

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

#include "Vxa_VSet.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType2Exporter.h"


/***********************
 ***********************
 *****             *****
 *****  Vxa::VSet  *****
 *****             *****
 ***********************
 ***********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VSet::VSet () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VSet::~VSet () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

VString &Vxa::VSet::getDescription_(VString &rResult) const {
    V::VRTTI iRTTI (typeid (*this));
    return rResult << iRTTI.name ();
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VSet::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection) {
    return pExporter->raiseUnimplementedOperationException (typeid(*this), "transmitUsing_(segment)");
}

bool Vxa::VSet::transmitUsing (VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection) {
    return transmitUsing_(pExporter, pMapMaker, rInjection);
}

bool Vxa::VSet::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker) {
    return pExporter->raiseUnimplementedOperationException (typeid(*this), "transmitUsing_");
}

bool Vxa::VSet::transmitUsing (VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker) {
    return transmitUsing_(pExporter, pMapMaker);
}

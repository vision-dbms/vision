/*****  Vxa_VInfiniteSet Implementation  *****/

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

#include "Vxa_VInfiniteSet.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VMonotypeMapMaker.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vxa::VInfiniteSet  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VInfiniteSet::VInfiniteSet () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VInfiniteSet::~VInfiniteSet () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VInfiniteSet::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection) {
    return pMapMaker->transmitValues (pExporter, rInjection);
}

bool Vxa::VInfiniteSet::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker) {
    return pMapMaker->transmitValues (pExporter);
}

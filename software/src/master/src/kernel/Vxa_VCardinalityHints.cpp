/*****  Vxa_VCardinalityHints Implementation  *****/

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

#include "Vxa_VCardinalityHints.h"

/************************
 *****  Supporting  *****
 ************************/


/************************************
 ************************************
 *****                          *****
 *****  Vxa::VCardinalityHints  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCardinalityHints::VCardinalityHints () {
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCardinalityHints::~VCardinalityHints () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vxa::cardinality_t Vxa::VCardinalityHints::lowerBound_() const {
    return 0;
}

Vxa::cardinality_t Vxa::VCardinalityHints::span (cardinality_t sMinimumSpan) const {
    cardinality_t const xUB = upperBound ();
    cardinality_t const xLB = lowerBound ();
    return xUB > xLB + sMinimumSpan ? xUB - xLB : sMinimumSpan;
}

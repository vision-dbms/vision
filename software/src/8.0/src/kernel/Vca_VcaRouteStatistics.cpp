/*****  VcaRouteStatistics Implementation  *****/

/************************
 ************************
 *****              *****
 *****  Interfaces  *****
 *****              *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_VcaRouteStatistics.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vca::VcaRouteStatistics  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaRouteStatistics::VcaRouteStatistics () : m_cHops (UINT_MAX) {	 
}

Vca::VcaRouteStatistics::VcaRouteStatistics (unsigned int cHops) : m_cHops (cHops) {
}

Vca::VcaRouteStatistics::VcaRouteStatistics (
    VcaRouteStatistics const &rOther
) : m_cHops (rOther.m_cHops) {
}

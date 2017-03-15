/*****  V_VAdaptiveAllocator Implementation  *****/

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

#include "V_VAdaptiveAllocator.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************************************
 ***********************************************
 *****                                     *****
 *****  V::VAdaptiveAllocator::Parameters  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

size_t V::VAdaptiveAllocator::Parameters::g_sIncrementDelay	= 2;
size_t V::VAdaptiveAllocator::Parameters::g_sIncrementMin	= 4096;
size_t V::VAdaptiveAllocator::Parameters::g_sIncrementMax	= 16 * 1024 * 1024;
size_t V::VAdaptiveAllocator::Parameters::g_sIncrementMultiplier= 4;


/***********************************
 ***********************************
 *****                         *****
 *****  V::VAdaptiveAllocator  *****
 *****                         *****
 ***********************************
 ***********************************/

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void V::VAdaptiveAllocator::reset () {
    m_sCurrentIncrement = m_pParameters->incrementMin ();
    m_sTimeToNextAdaptiveCall = m_pParameters->incrementDelay ();
}

size_t V::VAdaptiveAllocator::nextSize (size_t sRequired) {
    if (0 == m_sTimeToNextAdaptiveCall--) {
	m_sTimeToNextAdaptiveCall = m_pParameters->incrementDelay ();
	m_sCurrentIncrement *= m_pParameters->incrementMultiplier ();
	if (m_sCurrentIncrement > m_pParameters->incrementMax ())
	    m_sCurrentIncrement = m_pParameters->incrementMax ();
    }

    return (sRequired + m_sCurrentIncrement) / m_sCurrentIncrement * m_sCurrentIncrement;
}

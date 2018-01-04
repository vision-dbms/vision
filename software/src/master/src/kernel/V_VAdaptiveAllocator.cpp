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

size_t V::VAdaptiveAllocator::g_sIncrementDelay      = 2;
size_t V::VAdaptiveAllocator::g_sIncrementMin        = 4096;
size_t V::VAdaptiveAllocator::g_sIncrementMax        = 16 * 1024 * 1024;
size_t V::VAdaptiveAllocator::g_sIncrementMultiplier = 4;


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
    m_sCurrentIncrement = incrementMin ();
    m_sTimeToNextAdaptiveCall = incrementDelay ();
}

size_t V::VAdaptiveAllocator::nextSize (size_t sRequired) {
    if (0 == m_sTimeToNextAdaptiveCall--) {
	m_sTimeToNextAdaptiveCall = incrementDelay ();
	m_sCurrentIncrement *= incrementMultiplier ();
	if (m_sCurrentIncrement > incrementMax ())
	    m_sCurrentIncrement = incrementMax ();
    }

    return (sRequired + m_sCurrentIncrement) / m_sCurrentIncrement * m_sCurrentIncrement;
}

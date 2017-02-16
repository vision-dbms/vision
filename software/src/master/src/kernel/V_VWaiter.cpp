/**
 * @file
 * V::VWaiter Implementation.
 */

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "V_VWaiter.h"

bool V::VWaiter::waitUntil (V::VTime const & cLimit) {
    if (hasResult ()) return true;
    m_pWaitLock.waitUntil (cLimit);
    return hasResult ();
}

bool V::VWaiter::wait () {
    if (hasResult ()) return true;
    m_pWaitLock.wait ();
    return hasResult ();
}

void V::VWaiter::signal (bool bWithResult) {
    if (bWithResult) m_bHasResult.set ();
    else reset ();
    m_pWaitLock.signal ();
}

void V::VWaiter::reset () {
    m_bHasResult.clear ();
}

bool V::VWaiter::hasResult () {
    return m_bHasResult.isSet ();
}

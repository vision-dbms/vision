/*****  Vca_VLoopbackDeviceImplementation Implementation  *****/

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

#include "Vca_VLoopbackDeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerConsumerDevice_.h"


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VLoopbackDeviceImplementation  *****
 *****                                      *****
 ************************************************
 ************************************************/

/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

void Vca::VLoopbackDeviceImplementation::doTransfer (BSGet *pGet, BSPut *pPut) {
    size_t sGet = pGet->areaSize ();
    size_t sPut = pPut->areaSize ();
    size_t sTransfer = V_Min (sGet, sPut);
    memcpy (pGet->areaBase (), pPut->areaBase (), sTransfer);
    pGet->onData (sTransfer);
    pPut->onData (sTransfer);
}

bool Vca::VLoopbackDeviceImplementation::doRead  (
    VkStatus &rStatus, BSGet *pGet
) {
    if (m_pPendingPut) {
	BSPut::Pointer pPut;
	pPut.claim (m_pPendingPut);

	doTransfer (pGet, pPut);
	pPut->triggerUser ();

	return true;
    }

    if (m_bNoMorePuts)
	return rStatus.MakeClosedStatus ();

    m_pPendingGet.setTo (pGet);
    return rStatus.MakeBlockedStatus ();
}

bool Vca::VLoopbackDeviceImplementation::doWrite (
    VkStatus &rStatus, BSPut *pPut
) {
    if (m_pPendingGet) {
	BSGet::Pointer pGet;
	pGet.claim (m_pPendingGet);

	doTransfer (pGet, pPut);
	pGet->triggerUser ();

	return true;
    }

    if (m_bNoMoreGets)
	return rStatus.MakeClosedStatus ();

    m_pPendingPut.setTo (pPut);
    return rStatus.MakeBlockedStatus ();
}


/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

void Vca::VLoopbackDeviceImplementation::onFinalReader () {
    m_bNoMoreGets = true;
    if (m_pPendingPut) {
	m_pPendingPut->onEnd ();
	m_pPendingPut->triggerUser ();
	m_pPendingPut.clear ();
    }
    BaseClass::onFinalReader ();
}

void Vca::VLoopbackDeviceImplementation::onFinalWriter () {
    m_bNoMorePuts = true;
    if (m_pPendingGet) {
	m_pPendingGet->onEnd ();
	m_pPendingGet->triggerUser ();
	m_pPendingGet.clear ();
    }
    BaseClass::onFinalWriter ();
}


/********************************************************
 ********************************************************
 ******  BUILDER Mandated Template Instantiations  ******
 ********************************************************
 ********************************************************/

template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VLoopbackDeviceImplementation>::ConsumerTypes>;
template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VLoopbackDeviceImplementation>::ProducerTypes>;

template class Vca_API Vca::VBSProducerConsumerDevice_<Vca::VLoopbackDeviceImplementation>;

template class Vca_API Vca::VDevice_<Vca::VLoopbackDeviceImplementation>;

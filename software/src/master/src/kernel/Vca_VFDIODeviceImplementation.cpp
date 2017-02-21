/*****  Vca_VFDIODeviceImplementation Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#if defined(_WIN32)
#define SSIZE_MAX INT_MAX
#endif

/******************
 *****  Self  *****
 ******************/

#include "Vca_VFDIODeviceImplementation.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSConsumerDevice_.h"

#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerDevice_.h"
#include "Vca_VBSProducerConsumerDevice_.h"


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VFDIODeviceImplementation  *****
 *****                                  *****
 ********************************************
 ********************************************/

/********************************************************
 ********************************************************
 ******  BUILDER Mandated Template Instantiations  ******
 ********************************************************
 ********************************************************/

template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VFDIODeviceImplementation>::ConsumerTypes>;
template class Vca_API Vca::VBSMove_<Vca::VDevice_<Vca::VFDIODeviceImplementation>::ProducerTypes>;

template class Vca_API Vca::VBSConsumerDevice_<Vca::VFDIODeviceImplementation>;
template class Vca_API Vca::VBSProducerDevice_<Vca::VFDIODeviceImplementation>;
template class Vca_API Vca::VBSProducerConsumerDevice_<Vca::VFDIODeviceImplementation>;

template class Vca_API Vca::VDevice_<Vca::VFDIODeviceImplementation>;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

bool Vca::VFDIODeviceImplementation::canGet (int xOpenMode) {
    xOpenMode &= 3;
    return O_RDONLY == xOpenMode || O_RDWR == xOpenMode;
}

bool Vca::VFDIODeviceImplementation::canPut (int xOpenMode) {
    xOpenMode &= 3;
    return O_WRONLY == xOpenMode || O_RDWR == xOpenMode;
}

Vca::VFDIODeviceImplementation::VFDIODeviceImplementation (VReferenceable *pContainer, ThisClass &rOther) 
    : BaseClass		(pContainer)
    , m_hDevice		(rOther.grab ())
    , m_bNoMoreGets	(rOther.m_bNoMoreGets)
    , m_bNoMorePuts	(rOther.m_bNoMorePuts) {
}

Vca::VFDIODeviceImplementation::VFDIODeviceImplementation (VReferenceable *pContainer, Handle &rhDevice, int xOpenMode)
    : BaseClass		(pContainer)
    , m_hDevice		(grab (rhDevice))
    , m_bNoMoreGets	(m_hDevice < 0 || !canGet (xOpenMode))
    , m_bNoMorePuts	(m_hDevice < 0 || !canPut (xOpenMode))
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VFDIODeviceImplementation::~VFDIODeviceImplementation () {
    m_bNoMoreGets = m_bNoMorePuts = true;
    onEndOfFlow ();
}

/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

bool Vca::VFDIODeviceImplementation::doTransfer (VkStatus &rStatus, ssize_t &rsTransfer, BSGet::Area const &rArea) {
    if (m_bNoMoreGets)
	return rStatus.MakeClosedStatus ();

    size_t sRequest = rArea.size ();
    if (sRequest > SSIZE_MAX)
	sRequest = SSIZE_MAX;

    rsTransfer = read (m_hDevice, rArea.base (), sRequest);
    return rsTransfer > 0 || (
	rsTransfer < 0 ? rStatus.MakeErrorStatus () : rStatus.MakeClosedStatus ()
    );
}

bool Vca::VFDIODeviceImplementation::doTransfer (VkStatus &rStatus, ssize_t &rsTransfer, BSPut::Area const &rArea) {
    if (m_bNoMorePuts)
	return rStatus.MakeClosedStatus ();

    size_t sRequest = rArea.size ();
    if (sRequest > SSIZE_MAX)
	sRequest = SSIZE_MAX;

    rsTransfer = write (m_hDevice, rArea.base (), sRequest);

    return rsTransfer >= 0 || rStatus.MakeErrorStatus ();
}

/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

void Vca::VFDIODeviceImplementation::onEndOfFlow () {
    if (m_bNoMoreGets && m_bNoMorePuts && m_hDevice >= 0) {
	close (m_hDevice);
	m_hDevice = -1;
    }
}

void Vca::VFDIODeviceImplementation::onFinalReader () {
    m_bNoMoreGets = true;
    onEndOfFlow ();
    BaseClass::onFinalReader ();
}

void Vca::VFDIODeviceImplementation::onFinalWriter () {
    m_bNoMorePuts = true;
    onEndOfFlow ();
    BaseClass::onFinalWriter ();
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VFDIODeviceImplementation::BSGet  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

bool Vca::VFDIODeviceImplementation::BSGet::start (VkStatus &rStatus, Area const &rArea) {
    m_iArea.setTo (rArea);

    ssize_t sTransfer;
    return m_pDevice->doTransfer (rStatus, sTransfer, rArea) ? onData (sTransfer) : onError (rStatus);
}

/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::VFDIODeviceImplementation::BSPut  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

bool Vca::VFDIODeviceImplementation::BSPut::start (VkStatus &rStatus, Area const &rArea) {
    m_iArea.setTo (rArea);

    ssize_t sTransfer;
    return m_pDevice->doTransfer (rStatus, sTransfer, rArea) ? onData (sTransfer) : onError (rStatus);
}

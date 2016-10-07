/*****  VCOS (Copy Optimized Storage) Implementation  *****/

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

#include "V_VCOS.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  V::VCOS::OwnershipToken  *****
 *****                           *****
 *************************************
 *************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (V::VCOS::OwnershipToken);


/*********************
 *********************
 *****           *****
 *****  V::VCOS  *****
 *****           *****
 *********************
 *********************/

/********************************
 ********************************
 *****  Storage Management  *****
 ********************************
 ********************************/

void V::VCOS::deallocateStorage () {
    if (storageIsPrivate ())
	deallocate (m_pStorage);
    m_pStorageOwnershipToken.clear ();
}

namespace {
    size_t ReportingThreshold () {
	char const *const pReportingThreshold = getenv ("VCOSReportingThreshold");
	return pReportingThreshold ? atoi (pReportingThreshold) : UINT_MAX;
    }
    static size_t const g_sReportingThreshold = ReportingThreshold ();
}

void V::VCOS::guarantee (size_t sNewStorage, bool bExact) {
    if (storageIsShared ()) {
	if (sNewStorage >= g_sReportingThreshold) {
	    fprintf (
		stderr,
		"+++ %llp: Privatized %u byte%s of storage (was %u shared)\n",
		this, sNewStorage, sNewStorage != 1 ? "s" : "",  m_sStorage
	    );
	}
	pointer_t pNewStorage = (pointer_t)allocate (sNewStorage);
	memcpy (pNewStorage, m_pStorage, V_Min (m_sStorage, sNewStorage));
	m_pStorage = pNewStorage;
	m_sStorage = sNewStorage;

	m_pStorageOwnershipToken.setTo (new OwnershipToken ());
    }
    else if (m_sStorage < sNewStorage || (bExact && m_sStorage != sNewStorage)) {
	m_pStorage = (pointer_t)reallocate (m_pStorage, sNewStorage);
	m_sStorage = sNewStorage;
    }
}

void V::VCOS::trim (size_t sStorage) {
    if (m_sStorage != sStorage)
	guarantee (sStorage, true);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VCOS::VCOS (void *pStorage, size_t sStorage, bool bCopy)
: m_pStorage ((pointer_t)pStorage), m_sStorage (sStorage)
{
    if (bCopy)
	guarantee (m_sStorage);
}

V::VCOS::VCOS (size_t sStorage) : m_pStorage (0), m_sStorage (0) {
    if (sStorage > 0)
	guarantee (sStorage);
}

V::VCOS::VCOS (VCOS const& rOther)
: m_pStorageOwnershipToken (rOther.m_pStorageOwnershipToken)
, m_pStorage (rOther.m_pStorage)
, m_sStorage (rOther.m_sStorage)
{
}

V::VCOS::VCOS () : m_pStorage (0), m_sStorage (0) {
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void V::VCOS::setTo (VCOS const &rOther) {
    if (this != &rOther) {
	deallocateStorage ();

	m_pStorageOwnershipToken.setTo (rOther.m_pStorageOwnershipToken);
	m_pStorage = rOther.m_pStorage;
	m_sStorage = rOther.m_sStorage;
    }
}

void V::VCOS::setTo (void *pStorage, size_t sStorage, bool bCopy) {
    if (bCopy)
	memcpy (storage (sStorage), pStorage, sStorage);
    else {
	deallocateStorage ();
	m_pStorage = (pointer_t)pStorage;
	m_sStorage = sStorage;
    }
}

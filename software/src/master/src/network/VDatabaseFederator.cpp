/*****  VDatabaseFederator Implementation  *****/

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

#include "VDatabaseFederator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VDatabaseActivation.h"

#include "VDatabaseNDF.h"

#include "VSimpleFile.h"
#include "V_VString.h"


/********************************
 ********************************
 *****                      *****
 *****  VDatabaseFederator  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VDatabaseFederator);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VDatabaseFederator::VDatabaseFederator ()
: m_pActivationListHead (0)
, m_cActivations (0)
, m_iTransientAllocationLevel (0)
, m_iTransientAllocationHighWaterMark (0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VDatabaseFederator::~VDatabaseFederator () {
}


/*********************************
 *********************************
 *****  Database Activation  *****
 *********************************
 *********************************/

/***********************
 *****  Utilities  *****
 ***********************/

VDatabaseActivation *VDatabaseFederator::Locate (uuid_t const &rDatabaseUUID) {
    VDatabaseActivation *pActivation = m_pActivationListHead;

    while (pActivation && pActivation->databaseUUID_() != rDatabaseUUID)
	pActivation = pActivation->successor ();

    return pActivation;
}


/****************************
 *****  NDF Activation  *****
 ****************************/

VDatabaseActivation *VDatabaseFederator::Activate (
    char const *pNDFPathName, char const *pVersionSpec, bool makingNewNDF
) {
//  Obtain (or create) the UUID for the database in question, ...
    VString iUUIDFileName (pNDFPathName);
    iUUIDFileName.append (".UUID");

    VSimpleFile iUUIDFile;
    VString iUUIDAsString;
    if (access (iUUIDFileName, 0) && ENOENT == errno) {
	VkUUIDGenerate iNewUUID;
	if (iNewUUID.GetString (iUUIDAsString)) {
	    VString iUUIDCreationFileName (pNDFPathName);
	    iUUIDCreationFileName.append (".UUID");
	    if (iUUIDFile.PutLine (iUUIDAsString, iUUIDCreationFileName)) {
		iUUIDFile.close ();
		rename (iUUIDCreationFileName, iUUIDFileName);
		chmod (iUUIDFileName, 0444);
	    }
	    else {
		iUUIDFile.close ();
		remove (iUUIDCreationFileName); //  ... harmless directory clean-up
	    }
	}
	iUUIDAsString.clear ();
    }
    if (!iUUIDFile.GetLine (iUUIDAsString, iUUIDFileName)) {
	raiseApplicationException (
	    "VDatabaseFederator::Activate::'%s' Not Accessible.",
	    (char const*)iUUIDFileName
	);
	return NilOf (VDatabaseActivation*);
    }

    uuid_t iUUID;
    if (!VkUUID::GetUUID (iUUID, iUUIDAsString)) {
	raiseApplicationException (
	    "VDatabaseFederator::Activate::'%s' UUID <%s> Invalid.",
	    (char const*)iUUIDFileName, (char const*)iUUIDAsString
	);
	return NilOf (VDatabaseActivation*);
    }

    VDatabaseActivation *pActivation = Locate (iUUID);
    if (pActivation)
	return pActivation;

    VReference<VDatabaseNDF> pNDF (new VDatabaseNDF (new VDatabase (iUUID), pNDFPathName));

    pActivation = Activate_(pNDF, pVersionSpec, makingNewNDF);

    if (pActivation) {
	m_cActivations++;
	m_pActivationListHead.setTo (pActivation);
    }

    return pActivation;
}


/****************************************
 ****************************************
 *****  Resource Utilization Query  *****
 ****************************************
 ****************************************/

void VDatabaseFederator::AccumulateAllocationStatistics (
    double *pAllocationTotal, double *pMappingTotal
) const {
    VDatabaseActivation *pActivation = m_pActivationListHead;

    while (pActivation) {
	pActivation->AccumulateAllocationStatistics (pAllocationTotal, pMappingTotal);
	pActivation = pActivation->successor ();
    }
}

/*********************************************
 *********************************************
 *****  Resource Utilization Management  *****
 *********************************************
 *********************************************/

void VDatabaseFederator::FlushCachedResources () const {
    VDatabaseActivation *pActivation = m_pActivationListHead;
    while (pActivation) {
	pActivation->FlushCachedResources ();
	pActivation = pActivation->successor ();
    }
}

unsigned int VDatabaseFederator::ReclaimSegments () const {
    FlushCachedResources ();

    unsigned int result = 0;

    VDatabaseActivation *pActivation = m_pActivationListHead;
    while (pActivation) {
	result += pActivation->ReclaimSegments ();
	pActivation = pActivation->successor ();
    }

    return result;
}

unsigned int VDatabaseFederator::ReclaimAllSegments () const {
    unsigned int result = 0;

    VDatabaseActivation *pActivation = m_pActivationListHead;
    while (pActivation) {
	result += pActivation->ReclaimAllSegments ();
	pActivation = pActivation->successor ();
    }

    return result;
}

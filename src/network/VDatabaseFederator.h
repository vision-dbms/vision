#ifndef VDatabaseFederator_Interface
#define VDatabaseFederator_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VkUUID.h"

class VDatabase;
class VDatabaseNDF;

class VDatabaseActivation;

/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VDatabaseFederator : public VReferenceable {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VDatabaseFederator, VReferenceable);

//  Aliases
public:
    typedef V::uuid_t uuid_t;

//  Friends
    friend class VDatabaseActivation;

//  Construction
protected:
    VDatabaseFederator ();

//  Destruction
protected:
    ~VDatabaseFederator ();

//  Database Activation
private:
    virtual VDatabaseActivation *Activate_(
	VDatabaseNDF *pDatabaseNDF, char const *pVersionSpec, bool makingNewNDF
    ) = 0;

public:
    VDatabaseActivation *Activate (
	char const *pNDFPathName, char const *pVersionSpec = 0, bool makingNewNDF = false
    );

    unsigned int ActivationCount () const {
	return m_cActivations;
    }

    VDatabaseActivation *Locate (uuid_t const &rDatabaseUUID);

//  Resource Utilization Query
public:
    void AccumulateAllocationStatistics (
	double *pAllocationTotal, double *pMappingTotal
    ) const;

//  Resource Utilization Management
public:
    void FlushCachedResources () const;

    unsigned int ReclaimSegments () const;
    unsigned int ReclaimAllSegments () const;

//  Statistics Access
 public:
    double CurrentTransientAllocationLevel () const {
	return m_iTransientAllocationLevel;
    }
    double TransientAllocationHighWaterMark () const {
	return m_iTransientAllocationHighWaterMark;
    }
//  Statistics Maintenance
 public:
    void AdjustAllocationLevel (int sAdjustment) {
	m_iTransientAllocationLevel += sAdjustment;
	if (m_iTransientAllocationLevel > m_iTransientAllocationHighWaterMark)
	    m_iTransientAllocationHighWaterMark =
		m_iTransientAllocationLevel;
    }
    void IncrementAllocationLevel (size_t sIncrease) {
	m_iTransientAllocationLevel += sIncrease;
	if (m_iTransientAllocationLevel > m_iTransientAllocationHighWaterMark)
	    m_iTransientAllocationHighWaterMark =
		m_iTransientAllocationLevel;
    }
    void DecrementAllocationLevel (size_t sDecrease) {
	m_iTransientAllocationLevel -= sDecrease;
    }

//  State
protected:
    VReference<VDatabaseActivation> m_pActivationListHead;
    unsigned int		    m_cActivations;
    double			    m_iTransientAllocationLevel;
    double			    m_iTransientAllocationHighWaterMark;
};


#endif

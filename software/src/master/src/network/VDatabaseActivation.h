#ifndef VDatabaseActivation_Interface
#define VDatabaseActivation_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VDatabase.h"

class VDatabaseFederator;

/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VDatabaseActivation : public VReferenceable {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VDatabaseActivation, VReferenceable);

//  Friends
    friend class VDatabaseFederator;

//  Construction
protected:
    VDatabaseActivation (VDatabaseFederator *pFederator);

//  Destruction
protected:
    ~VDatabaseActivation ();

//  Access
public:
    virtual VDatabase *database_() const = 0;

    VkUUID const &databaseUUID_() const {
	return database_()->uuid ();
    }

    VDatabaseFederator *federator () const {
	return m_pFederator;
    }

    VDatabaseActivation *successor () const {
	return m_pActivationListSuccessor;
    }

//  Resource Utilization Query
public:
    virtual void AccumulateAllocationStatistics (
	unsigned __int64 *pAllocationTotal, unsigned __int64 *pMappingTotal
    ) const = 0;

//  Resource Utilization Management
public:
    virtual void FlushCachedResources () = 0;

    virtual unsigned int ReclaimSegments    () = 0;
    virtual unsigned int ReclaimAllSegments () = 0;

//  State
protected:
    VReference<VDatabaseFederator> const	m_pFederator;
    VReference<VDatabaseActivation>		m_pActivationListSuccessor;
};


#endif

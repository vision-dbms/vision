#ifndef VGroundStore_Interface
#define VGroundStore_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"

class VSNFTask;


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VGroundStore : public VBenderenceable {
    DECLARE_ABSTRACT_RTT (VGroundStore, VBenderenceable);

//  Construction
protected:

    VGroundStore () : m_pSurrogateHandle (0) {
    }

//  Destruction
protected:
    ~VGroundStore () {
    }

//  Access
public:
    virtual unsigned int cardinality_ () const = 0;
    virtual M_CPD *ptoken_ () const = 0;

//  Query
public:
    virtual bool allowsInsert () const;
    virtual bool allowsDelete () const;

    bool hasASurrogate () const {
	return m_pSurrogateHandle ? true : false;
    }

//  Surrogate / Transient Extension Management
protected:
    void EndRoleAsTransientExtensionOf (VContainerHandle* pClusterHandle);

private:
    M_CPD *getCPDForNewSurrogate ();

public:
    M_CPD *surrogateCPD () {
	return m_pSurrogateHandle ? m_pSurrogateHandle->GetCPD () : getCPDForNewSurrogate ();
    }

//  Task Implementation

public:
    virtual bool groundImplementationSucceeded (VSNFTask* pTask) = 0;

//  State
private:
    VContainerHandle*	m_pSurrogateHandle;
};


#endif

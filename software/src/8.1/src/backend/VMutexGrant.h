#ifndef VMutexGrant_Interface
#define VMutexGrant_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

#include "VComponentReference.h"
#include "VComputationTrigger.h"

/**************************
 *****  Declarations  *****
 **************************/

class VMutex;
class VMutexClaim;

/*************************
 *****  Definitions  *****
 *************************/

class VMutexGrant : public VBenderenceable {
    DECLARE_CONCRETE_RTT (VMutexGrant, VBenderenceable);

//  Aliases
public:
    typedef VComponentReference<VMutex,transientx_t> MutexReference;

//  Friends
    friend class VMutex;
    friend class VMutexClaim;

//  Construction
protected:
    VMutexGrant (VComputationUnit* pOwner, transientx_t *pResource, VMutex* pMutex);

//  Destruction
private:
    ~VMutexGrant ();

//  Control
protected:
    bool acquireClaim (VMutexClaim& rClaim, VComputationUnit* pSupplicant);

    void releaseClaim ();

//  State
protected:
    VReference<VComputationUnit>const	m_pOwner;
    MutexReference		const	m_pMutex;
    unsigned int			m_cClaims;
    VComputationTrigger			m_iQueue;
};


#endif

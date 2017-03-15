#ifndef VMutex_Interface
#define VMutex_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VBenderenceable.h"

class VComputationUnit;
class VMutexClaim;
class VMutexGrant;

/*************************
 *****  Definitions  *****
 *************************/

class VMutex : public VTransient {
    DECLARE_FAMILY_MEMBERS (VMutex, VTransient);

//  Friends
    friend class VMutexGrant;

//  Construction
public:
    VMutex () : m_pGrant (0) {
    }

//  Destruction
public:
    ~VMutex () {
    }

//  Control
public:
    bool acquireClaim (
	VMutexClaim &rClaim, VComputationUnit *pSupplicant, transientx_t *pResource = 0
    );

protected:
    void release (transientx_t *pResource);

//  State
protected:
    VMutexGrant *m_pGrant;
};


#endif

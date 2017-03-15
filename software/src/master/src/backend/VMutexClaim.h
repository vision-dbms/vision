#ifndef VMutexClaim_Interface
#define VMutexClaim_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VMutexGrant.h"

/*************************
 *****  Definitions  *****
 *************************/

class VMutexClaim : public VTransient {
//  Friends
    friend class VMutex;
    friend class VMutexGrant;

//  Construction
public:
    VMutexClaim ();

//  Destruction
public:
    ~VMutexClaim ();

//  Access
protected:
    VMutexGrant* grant () const {
	return m_pGrant;
    }

//  Control
public:
    void releaseClaim ();

protected:
    void setTo (VMutexGrant* pNewGrant);

//  State
protected:
    VReference<VMutexGrant> m_pGrant;
};


#endif

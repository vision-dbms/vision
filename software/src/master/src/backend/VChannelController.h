#ifndef VChannelController_Interface
#define VChannelController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTopTaskBase.h"

#include "VMutexClaim.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "IOMDriver.h"


/*************************
 *****  Definitions  *****
 *************************/

class VChannelController : public VTopTaskBase {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VChannelController, VTopTaskBase);

//  Construction
public:
    VChannelController (VTask* pSpawningTask, IOMDriver* pChannel);

//  Destruction
protected:
    ~VChannelController ();

//  Execution
protected:
    virtual void run	() OVERRIDE;
    virtual void fail	() OVERRIDE;

//  Display And Description
public:
    virtual void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const OVERRIDE;
    virtual void reportTrace	() const OVERRIDE;

    virtual char const* description () const OVERRIDE;


//  State
protected:
    VMutexClaim		m_iAutoMutexClaim;
    IOMState		m_xLastChannelState;
    IOMState		m_xNextChannelState;
    unsigned int	m_fWaitingForInput	: 1;
    unsigned int	m_fWaitingForMutex	: 1;
};


#endif

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
    void run	();
    void fail	();

//  Display And Description
public:
    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;


//  State
protected:
    VMutexClaim		m_iAutoMutexClaim;
    IOMState		m_xLastChannelState;
    IOMState		m_xNextChannelState;
    unsigned int	m_fWaitingForInput	: 1;
    unsigned int	m_fWaitingForMutex	: 1;
};


#endif

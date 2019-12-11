#ifndef VTopTask_Interface
#define VTopTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTopTaskBase.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VTopTask : public VTopTaskBase {
    DECLARE_CONCRETE_RTT (VTopTask, VTopTaskBase);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VTopTask (VTask* pSpawningTask, IOMDriver* pChannel);

//  Query
public:
    virtual bool datumAvailable_ () const OVERRIDE;

//  Execution
protected:
    virtual void run () OVERRIDE;

//  Display
public:
    virtual void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const OVERRIDE;
    virtual void reportTrace	() const OVERRIDE;

    virtual char const* description () const OVERRIDE;

//  State
protected:
};


#endif

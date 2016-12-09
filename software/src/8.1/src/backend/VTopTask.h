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
    bool datumAvailable_ () const;

//  Execution
protected:
    void run ();

//  Display
public:
    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;

//  State
protected:
};


#endif

#ifndef VDatabaseFederatorForBatchvision_Interface
#define VDatabaseFederatorForBatchvision_Interface

/************************
 *****  Components  *****
 ************************/

#include "VDatabaseFederator.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VDatabaseFederatorForBatchvision : public VDatabaseFederator {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VDatabaseFederatorForBatchvision, VDatabaseFederator);

//  Construction
public:
    VDatabaseFederatorForBatchvision () {
    }

//  Destruction
private:
    ~VDatabaseFederatorForBatchvision () {
    }

//  Database Activation
private:
    virtual VDatabaseActivation *Activate_(
	VDatabaseNDF *pDatabase, char const *pVersionSpec, bool makingNewNDF
    ) OVERRIDE;

//  Resource Utilization Management
private:
    void DoTransientGCSetup	() const;
    void DoTransientGCMarking	() const;
    bool DoTransientGCSweep	() const;

    bool DoGCCycleElimination	() const;
    bool EnqueuePossibleCycles	() const;
    bool TraverseAndDetectCycles() const;
    bool EnqueueOmittingCycles	() const;

public:
    bool DisposeOfSessionGarbage (bool bAggressive = false) const;

//  State
protected:
};


#endif

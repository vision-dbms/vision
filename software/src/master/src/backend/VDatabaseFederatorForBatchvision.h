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
    VDatabaseActivation *Activate_(
	VDatabaseNDF *pDatabase, char const *pVersionSpec, bool makingNewNDF
    );

//  Resource Utilization Management
private:
    void DoTransientGCSetup	() const;
    void DoTransientGCMarking	() const;
    bool DoTransientGCSweep	() const;

public:
    bool DisposeOfSessionGarbage () const;

//  State
protected:
};


#endif

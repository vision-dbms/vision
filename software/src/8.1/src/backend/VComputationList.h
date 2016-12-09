#ifndef VComputationList_Interface
#define VComputationList_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

class VComputationUnit;

/*************************
 *****  Definitions  *****
 *************************/

class VComputationList : public VReference<VComputationUnit> {
//  Construction
public:
    VComputationList (VComputationList const& rOther);
    VComputationList ();

//  Destruction
public:
    ~VComputationList ();

//  Access/Update
public:
    inline void pop (VReference<VComputationUnit>& rpHeadReturn);

//  State
protected:
};



#endif

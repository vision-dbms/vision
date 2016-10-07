#ifndef VComputationTrigger_Interface
#define VComputationTrigger_Interface

/************************
 *****  Components  *****
 ************************/

#include "VComputationList.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VComputationTrigger : protected VComputationList {
//  Construction
public:
    VComputationTrigger () {
    }

//  Destruction
public:
    ~VComputationTrigger ();

//  Query
public:
    bool isEmpty () const {
	return VComputationList::isEmpty ();
    }
    bool isntEmpty () const {
	return VComputationList::isntEmpty ();
    }

//  Access
public:
    operator VComputationUnit* () const {
	return VComputationList::operator VComputationUnit* ();
    }

//  Use
public:
    void suspend (VComputationUnit* pSuspendable);

    void triggerFirst ();

    void triggerAll ();

//  State
protected:
};


#endif

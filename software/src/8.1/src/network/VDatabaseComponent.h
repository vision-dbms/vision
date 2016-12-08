#ifndef VDatabaseComponent_Interface
#define VDatabaseComponent_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VDatabase.h"

class VString;


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VDatabaseComponent : public VReferenceable {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VDatabaseComponent, VReferenceable);

//  Construction
protected:
    VDatabaseComponent () {
    }

//  Access
public:
    virtual VDatabaseComponent *container_() const = 0;

    virtual VDatabase *database_() const;

    VkUUID const &databaseUUID_() const {
	return database_()->uuid ();
    }

//  State
protected:
};


#endif

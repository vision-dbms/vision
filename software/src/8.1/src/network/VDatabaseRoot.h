#ifndef VDatabaseRoot_Interface
#define VDatabaseRoot_Interface

/************************
 *****  Components  *****
 ************************/

#include "VDatabaseComponent.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VDatabaseRoot : public VDatabaseComponent {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VDatabaseRoot, VDatabaseComponent);

//  Construction
protected:
    VDatabaseRoot (VDatabase *pDatabase);

//  Destruction
protected:
    ~VDatabaseRoot ();

//  Access
public:
    VDatabaseComponent *container_() const;

    VDatabase *database_() const;
    VDatabase *database () const {
	return m_pDatabase;
    }

    VkUUID const &databaseUUID () const {
	return m_pDatabase->uuid ();
    }

//  State
protected:
    VReference<VDatabase> const m_pDatabase;
};


#endif

#ifndef VDatabaseNDF_Interface
#define VDatabaseNDF_Interface

/************************
 *****  Components  *****
 ************************/

#include "VDatabaseRoot.h"

#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "ps.d"

/*************************
 *****  Definitions  *****
 *************************/

class VDatabaseNDF : public VDatabaseRoot {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VDatabaseNDF, VDatabaseRoot);

//  Friends
    friend class VDatabaseFederator;

//  Construction
protected:
    VDatabaseNDF (VDatabase *pDatabase, char const *pNDFPathName)
	: VDatabaseRoot (pDatabase), m_iNDFPathName (pNDFPathName) 
    {
    }

//  Access
public:
    VString const &ndfPathName () const {
	return m_iNDFPathName;
    }

//  State
protected:
    VString const m_iNDFPathName;
};


#endif

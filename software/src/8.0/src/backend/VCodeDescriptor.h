#ifndef VCodeDescriptor_Interface
#define VCodeDescriptor_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VCodeTable.h"

/*************************
 *****  Definitions  *****
 *************************/

class VCodeDescriptor : public VTransient {
//  Construction
public:
    VCodeDescriptor (VCodeDescriptor const& rOther);
    VCodeDescriptor (unsigned int xCode, char const* pName);

//  Query
public:
    unsigned int code () const {
	return m_xCode;
    }
    char const* name () const {
	return m_pName;
    }

//  State
protected:
    unsigned int const	m_xCode;
    char const* const	m_pName;
};


#endif

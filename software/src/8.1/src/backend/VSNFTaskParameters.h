#ifndef VSNFTaskParameters_Interface
#define VSNFTaskParameters_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

class VSNFTaskParameters : public VReferenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VSNFTaskParameters, VReferenceable);

//  Construction
public:
    VSNFTaskParameters (unsigned int xParameter);

//  Destruction
private:
    ~VSNFTaskParameters ();

//  Access
public:
    unsigned int getxParameter () const {
	return m_xParameter;
    }

    VSNFTaskParameters *getSuccessor () const {
	return m_pSuccessor;
    }

//  State
private:
    unsigned int const m_xParameter;
public:
    VReference<VSNFTaskParameters> m_pSuccessor;
};


#endif

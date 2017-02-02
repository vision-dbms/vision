#ifndef VSNFTaskParametersList_Interface
#define VSNFTaskParametersList_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

class VSNFTaskParameters;

/*************************
 *****  Definitions  *****
 *************************/

class VSNFTaskParametersList : public VReference<VSNFTaskParameters> {
//  Construction
public:
    VSNFTaskParametersList (VSNFTaskParametersList const& rOther);
    VSNFTaskParametersList ();

//  Destruction
public:
    ~VSNFTaskParametersList ();

//  Access/Update
public:
    inline void pop (VReference<VSNFTaskParameters>& rpHeadReturn);

//  State
protected:
};



#endif

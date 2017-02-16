#ifndef Vca_VManager_Interface
#define Vca_VManager_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

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

namespace Vca {
    class IError;

    class ABSTRACT Vca_API VManager : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VManager, VReferenceable);

    //  Construction
    protected:
	VManager ();

    //  Destruction
    protected:
	~VManager ();
    
    //  Callbacks	    
    public:
	virtual void onEnd () = 0;
	virtual void onError (IError *pError, VString const &rMessage) = 0;
    };
}


#endif

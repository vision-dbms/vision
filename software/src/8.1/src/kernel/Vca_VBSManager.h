#ifndef Vca_VBSManager_Interface
#define Vca_VBSManager_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_VManager.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VStatus.h"

/*************************
 *****  Definitions  *****
 *************************/

/************************
 *----  VBSManager  ----*
 ************************/

namespace Vca {

    class ABSTRACT Vca_API VBSManager : public VManager {
	DECLARE_ABSTRACT_RTTLITE (VBSManager, VManager);

    //  Construction
    protected:
	VBSManager ();

    //  Destruction
    protected:
	~VBSManager ();

    //  Callbacks
    public:
	virtual void onTransfer (size_t sTransfer) = 0;
	virtual bool onStatus (VStatus const &rStatus) = 0;
        virtual void onChange (Vca::U32 sChange) = 0;
    };
}


#endif

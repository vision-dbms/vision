/*****  Vision Kernel Status Interface  *****/
#ifndef Vca_VStatus_Interface
#define Vca_VStatus_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VkStatus.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IClient;

    class Vca_API VStatus : public VkStatus {
    //  Construction
    public:
	VStatus (VkStatus const &rOther) : VkStatus (rOther) {
	}
	VStatus (VStatus const &rOther) : VkStatus (rOther) {
	}
	VStatus () {
	}

    //  Reporting
    public:
	bool communicateTo (IClient *pRequest) const;
    };

}


#endif

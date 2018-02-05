#ifndef Vca_VActivityRelay_Interface
#define Vca_VActivityRelay_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivitySink.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     *  An activity sink that collects activities in-progress.
     */
    class Vca_API VActivityRelay : public VActivitySink {
	DECLARE_CONCRETE_RTTLITE (VActivityRelay, VActivitySink);

    //********************************************************************************************/
    //  ActivityMonitor
    public:
	class ActivityMonitor;

    //********************************************************************************************/
    //  Construction
    public:
	VActivityRelay (VActivityModel *pModel);

    //  Destruction
    protected:
	~VActivityRelay ();

    //  Monitoring
    public:
	void createMonitorFor (VActivity* pActivity) const OVERRIDE;
    };
}


#endif

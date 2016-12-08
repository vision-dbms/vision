/*****  Vca NewsFeed Repeater  *****/

/************************
 *****  Components  *****
 ************************/

#include "Vca_VServerApplication.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDirectory.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace NewsFeed {
	class Repeater : public VServerApplication {
	    DECLARE_CONCRETE_RTTLITE (Repeater, VServerApplication);

	//  Construction
	public:
	    Repeater (Context *pContext);

	//  Destruction
	private:
	    ~Repeater ();

	//  Run State Transition
	private:
	    bool start_();

        //  Usage
        private:
          void displayUsage () const;
	};
    }
}

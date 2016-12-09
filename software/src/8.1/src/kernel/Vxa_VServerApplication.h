#ifndef Vxa_VServerApplication_Interface
#define Vxa_VServerApplication_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VServerApplication.h"
#include "Vxa_ICollection.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VExportable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class ABSTRACT Vxa_Main_API VServerApplication : public Vca::VServerApplication {
	DECLARE_ABSTRACT_RTTLITE (VServerApplication, Vca::VServerApplication);

    //  Construction
    protected:
	VServerApplication (Context *pContext);

    //  Destruction
    protected:
	~VServerApplication ();
      
    //  Root Object
    private:
	//  override this virtual to call one of the two 'offer methods that follow...
	virtual void getRootObject () = 0;
    protected:
	void offer (export_return_t const &rRootExport) {
	    aggregate (rRootExport);
	}
	template <typename T> void offer (T *pT) {
	    aggregate (pT);
	    offer (Export (pT));
	}

    //  Run State
    protected:
	bool start_();
    };
}


#endif

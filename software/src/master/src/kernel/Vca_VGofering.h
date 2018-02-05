#ifndef Vca_VGofering_Interface
#define Vca_VGofering_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VGofer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Represents a gofer's work in progress.
     */
    class Vca_API VGofering : public VActivity {
	DECLARE_CONCRETE_RTTLITE (VGofering, VActivity);

	friend class VGofer;

    //  Construction
    public:
	VGofering (VGofer* pGofer);

    //  Destruction
    protected:
	~VGofering ();

    //  Access
    protected:
	virtual void getDescription_(VString& rResult) const OVERRIDE;

    //  State
    private:
	VGofer::Reference const m_pGofer;
    };
}


#endif

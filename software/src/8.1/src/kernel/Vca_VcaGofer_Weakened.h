#ifndef Vca_VcaGofer_Weakened_Interface
#define Vca_VcaGofer_Weakened_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VRTTI.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Gofer {
	template <class Interface_T> class Weakened : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Weakened<Interface_T>, VGoferInterface<Interface_T>);

	//  Construction
	public:
	    template <typename source_t> Weakened (source_t iSource) : m_iSource (iSource) {
	    }

	//  Destruction
	protected:
	    ~Weakened () {
	    }

	//  Callbacks
	private:
	    void onNeed () {
		m_iSource.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void onData () {
		if (m_iSource)
		    m_iSource->weakenImport ();
		BaseClass::setTo (m_iSource);
	    }

	//  State
	private:
	    VInstanceOfInterface<Interface_T> m_iSource;
	};
    }
}

#endif

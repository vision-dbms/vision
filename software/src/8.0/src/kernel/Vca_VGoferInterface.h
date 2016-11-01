#ifndef Vca_VGoferInterface_Interface
#define Vca_VGoferInterface_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInstance.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

#define DECLARE_GOFER_INTERFACE(storageclass,api,iface)\
    DECLARE_GOFER_INSTANCE(storageclass,api,iface*)\
    DECLARE_TEMPLATE_INSTANCE(storageclass,api,Vca::VGoferInterface<iface >)

namespace Vca {

/********************************************************************
 *----  template <typename Interface_T> class VGoferInterface  -----*
 ********************************************************************/

    template <class Interface_T> class VGoferInterface : public VGoferInstance<Interface_T*> {
	DECLARE_ABSTRACT_RTTLITE (VGoferInterface<Interface_T>,VGoferInstance<Interface_T*>);

    //  Aliases
    public:
	typedef Interface_T interface_t;

    //  Construction
    protected:
	VGoferInterface (interface_t *pInterface) : BaseClass (pInterface), m_bFallbackOnNull (false) {
	}
	VGoferInterface () : m_bFallbackOnNull (false) {
	}

    //  Destruction
    protected:
	~VGoferInterface () {
	}

    //  Fallback on Null
    public:
        void setFallbackOnNull (bool bFallbackOnNull = true) {
            m_bFallbackOnNull = bFallbackOnNull;
        }
        /**
         * Delegates to onError to handle fallback when null pointer is returned as gofer value and fallback-on-null is enabled.
         */
        void setTo (Interface_T* iVal) {
            if (!iVal && m_bFallbackOnNull) BaseClass::onError (0, "Null pointer returned to VGoferInterface.");
            else BaseClass::setTo (iVal);
        }
    private:
        bool m_bFallbackOnNull;
    };
}


#endif

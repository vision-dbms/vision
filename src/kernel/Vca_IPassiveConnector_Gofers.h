#ifndef Vca_IPassiveConnector_Gofers_Interface
#define Vca_IPassiveConnector_Gofers_Interface

#ifndef Vca_IPassiveConnector_Gofers
#define Vca_IPassiveConnector_Gofers extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_IPassiveConnector.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    DECLARE_GOFER_INTERFACE (Vca_IPassiveConnector_Gofers,Vca_API,IPassiveConnector)

    namespace IPassiveConnector_Gofer {
        /** Convenience definition for the Connector base class; VGoferInterface templated for IPassiveConnector interfaces. */
	typedef VGoferInterface<IPassiveConnector> gofer_base_t;

        /**
         * Gofer that returns an IPassiveConnector instance.
         * This is the initiator for callbacks in Vca.
         */
	class Connector : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (Connector, gofer_base_t);

	//  Construction
	public:
            /** Empty constructor. */
	    Connector () {
	    }

	//  Destruction
	private:
            /** Empty private destructor restricts scoping of destruction. */
	    ~Connector () {
	    }

	//  Callbacks
	private:
            /**
             * Manufactures a TCP/IP listener and attaches a new VPassiveConnector to it. Sets this gofer's value to an IPassiveConnector instance for said VPassiveConnecter.
             */
	    void onData ();
	};

        /**
         * Returns a gofer for the IPassiveConnector interface for a global instance of Vca::VPassiveConnector.
         */
	Vca_API gofer_base_t *DefaultConnector ();
    }
}

#endif

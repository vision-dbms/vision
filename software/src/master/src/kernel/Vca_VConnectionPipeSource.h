#ifndef Vca_VConnectionPipeSource_Interface
#define Vca_VConnectionPipeSource_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VConnectionFactory.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VConnectionPipeSource : public VPipeSource {
	DECLARE_CONCRETE_RTTLITE (VConnectionPipeSource, VPipeSource);

    //  Construction
    public:
	VConnectionPipeSource (
	    VConnectionFactory *pFactory, VString const &rDestination, bool bFileName, bool bNoDelay
	);

    //  Destruction
    protected:
	~VConnectionPipeSource ();

    //  Access/Query
    public:
	VConnectionFactory *factory () const {
	    return m_pFactory;
	}
	VString const &destination () const {
	    return m_iDestination;
	}
	bool destinationIsFileName () const {
	    return m_bFileName;
	}
	bool noDelay () const {
	    return m_bNoDelay;
	}

    //  Implementation
    private:
	void supply_(IPipeSourceClient *pClient) OVERRIDE;

    //  State
    private:
	VConnectionFactory::Reference	const	m_pFactory;
	VString				const	m_iDestination;
        bool				const	m_bFileName;
	bool				const	m_bNoDelay;
    };
}


#endif

#ifndef Vca_VFilterPipeSource__Interface
#define Vca_VFilterPipeSource__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPipeSource.h"

#include "Vca_IPipeSourceClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPipeSource.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template<class Factory> class VFilterPipeSource_ : public VPipeSource {
	DECLARE_CONCRETE_RTTLITE (VFilterPipeSource_<Factory>, VPipeSource);

    //  Factory Data
    public:
	typedef typename Factory::PipeSourceData PipeSourceData;

    //  Construction
    public:
	VFilterPipeSource_(
	    Factory *pFactory, PipeSourceData const &rPipeSourceData, IPipeSource *pPipeSource
	) : m_pFactory (pFactory), m_iPipeSourceData (rPipeSourceData), m_pPipeSource (pPipeSource) {
	}

    //  Destruction
    private:
	~VFilterPipeSource_() {
	}

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  Use
    private:
	virtual void supply_(IPipeSourceClient *pClient) {
	    m_pFactory->supply (pClient, m_iPipeSourceData, m_pPipeSource);
	}

    //  State
    private:
	VReference<Factory> const	m_pFactory;
	VReference<IPipeSource>		m_pPipeSource;
	PipeSourceData			m_iPipeSourceData;
    };
}


#endif

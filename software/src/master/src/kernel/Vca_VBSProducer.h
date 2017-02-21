#ifndef Vca_VBSProducer_Interface
#define Vca_VBSProducer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBS.h"

#include "Vca_IBSProducer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArea.h"


/*************************
 *****  Definitions  *****
 *************************/

/******************************
 *----  Vca::VBSProducer  ----*
 ******************************/

namespace Vca {
    /**
     * Abstract class representing an inbound byte stream.
     */
    class ABSTRACT Vca_API VBSProducer : public VBS {
	DECLARE_ABSTRACT_RTTLITE (VBSProducer, VBS);

    //  Area
    public:
	typedef V::VArea<void *> Area;

    //  Byte Stream
    public:
	typedef ThisClass BS;

    //  ProducerExportStub
    public:
	class ProducerExportStub;

    //  Construction
    protected:
	VBSProducer (VCohort *pCohort);

    //  Destruction
    protected:
	~VBSProducer () {
	}

    //  Export
    private:
	void getExportStub (VReference<ProducerExportStub>&rpExport);

    //  Roles
    public:
	using BaseClass::getRole;

    //  IBSProducer Role
    public:
	void getRole (VReference<IBSProducer>&rpRole);

    //  Access
    public:
	size_t transferPending () const; 

	virtual size_t GetByteCount () {
	    return m_iBufferFifo.consumerAreaSize ();
	}

    //  Query    
    public:
	bool closeDeferred () const {
	    return false;
	}

    //  Scheduling
    private:
	virtual void run_();

    //  Transfer
    public:
	using BaseClass::get;

    //  State
    private:
    };

}
#endif

#ifndef Vca_VBSConsumer_Interface
#define Vca_VBSConsumer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBS.h"

#include "Vca_IBSConsumer.h"

#include "V_VAggregatePointer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArea.h"


/*************************
 *****  Definitions  *****
 *************************/

/******************************
 *----  Vca::VBSConsumer  ----*
 ******************************/

namespace Vca {
    /**
     * Abstract class representing an outgoing byte stream.
     */
    class ABSTRACT Vca_API VBSConsumer : public VBS {
	DECLARE_ABSTRACT_RTTLITE (VBSConsumer, VBS);

    //  Aliases
    public:
	typedef V::VArea<void const*>		Area;
	typedef ThisClass			BS;

    //  ConsumerExportStub
    public:
	class ConsumerExportStub;

    //  Globals
    private:
	static Pointer g_pFirstInstance;

    //  Construction
    protected:
	VBSConsumer (VCohort *pCohort);

    //  Destruction
    protected:
	~VBSConsumer ();

    //  Export
    private:
	void getExportStub (VReference<ConsumerExportStub>&rpExport);

    //  Roles
    public:
	using BaseClass::getRole;

    //  IBSConsumer Role
    public:
	void getRole (VReference<IBSConsumer>&rpRole);

    //  Access
    public:
	size_t transferPending () const;

    //  Query
    public:
	bool closeDeferred () const {
	    return transferPending () > 0;
	}

    //  Scheduling
    private:
	virtual void run_();

    //  Transfer
    public:
	using BaseClass::put;
	static void PutBufferedData ();

    //  State
    private:
	Pointer m_pNextInstance;
	Pointer m_pLastInstance;
    };

}


#endif

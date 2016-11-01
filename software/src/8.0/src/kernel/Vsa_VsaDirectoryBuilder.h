#ifndef Vsa_VsaDirectoryBuilder_Interface
#define Vsa_VsaDirectoryBuilder_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/
#include "VSimpleFile.h"
#include "Vca_VcaDirectoryBuilder.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

#ifdef __VMS
#define ThisOrder Order
#endif

namespace Vsa {
    class VsaDirectoryBuilder : public Vca::VcaDirectoryBuilder {
	DECLARE_CONCRETE_RTTLITE (VsaDirectoryBuilder, Vca::VcaDirectoryBuilder);

	class ThisOrder: public Vca::VcaDirectoryBuilder::Order {
	    DECLARE_CONCRETE_RTTLITE (ThisOrder, Vca::VcaDirectoryBuilder::Order);
        
        //  class
	    class SpecialPoolSession;
        
        //  friend
	    friend class VsaDirectoryBuilder;

	    enum ParseState {
		ParseState_Error,
		ParseState_ExpectingTag,
		ParseState_ExpectingUnknown,
		ParseState_ExpectingIncludeTarget,
		ParseState_ExpectingIncludeIfTag,
		ParseState_ExpectingIncludeIfTarget,
		ParseState_ExpectingSessionSentinel,

		ParseState_ExpectingPoolSessionSentinel,
		ParseState_ExpectingPoolSessionName,

		ParseState_ExpectingWorkerMax,
		ParseState_ExpectingWorkerMin,
		ParseState_ExpectingWorkerMaxAvail,
		ParseState_ExpectingWorkerMinAvail,
		ParseState_ExpectingWorkersBeingCreated,
		ParseState_ExpectingEvaluationTimeout,
		ParseState_ExpectingEvaluationAttempts,
		ParseState_ExpectingWorkerCreationFailureHardLmt,
		ParseState_ExpectingWorkerCreationFailureSoftLmt,
		ParseState_ExpectingShrinkTimeOut,
		ParseState_ExpectingStopTimeOut,
		ParseState_ExpectingOption
	    };
	
	//  Constructor
	public:
	    ThisOrder (VcaDirectoryBuilder* pBuilder, Vca::VDirectory *pDirectory);
	
	//  Destructor
	protected:
	    ~ThisOrder ();

        //  Implementation
	protected:
	    virtual void parseSessionsFile (char const *pConfigFile, SessionType iType);
	    virtual bool insertSession (Session *pSession);

	    bool parsePoolSessionTemplate (VSimpleFile &rSessionsFile, SessionType iType, VReference<Session> &rpSession) const;

	//  Query
	protected:
	    virtual bool includeIfTagAccepted (char const *pTag) const;
	};      

    //  Construction
    public:
	VsaDirectoryBuilder ();

    //  Destruction
    protected:
	~VsaDirectoryBuilder ();

    public:
	virtual void build (Vca::VDirectory *pDirectory);
    };
}

#endif




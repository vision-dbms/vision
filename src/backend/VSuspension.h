#ifndef VSuspension_Interface
#define VSuspension_Interface

/************************
 *****  Components  *****
 ************************/

#include "VInternalGroundStore.h"

/**************************
 *****  Declarations  *****
 **************************/

class DSC_Pointer;

class VComputationUnit;
class VDescriptor;


/*************************
 *****  Definitions  *****
 *************************/

class VSuspension : public VInternalGroundStore {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VSuspension, VInternalGroundStore);

//  Friends
    friend class VControlPointSubscription;

//  Meta Maker
protected:
    static void MetaMaker ();

//  State Enumeration
public:
    enum State {
	Aborted, Resumed, MustAbort, WaitingToAbort, WaitingToResume
    };
    static char const* NameFor (State xState);

    static State AppropriateInitialStateFor (VComputationUnit* pSuspendee);

//  Construction
public:
    VSuspension (VComputationUnit* pSuspendee, VSuspension* pSuccessor = 0);

//  Destruction
protected:
    ~VSuspension ();

//  Query
public:
    bool datumAlterable () const;
    bool datumAvailable () const;

    bool committed () const {
	return m_xState == Aborted || m_xState == Resumed;
    }
    bool uncommitted () const {
	return m_xState != Aborted && m_xState != Resumed;
    }

    bool canAbort () const {
	return uncommitted ();
    }

    bool canResume () const {
	return uncommitted () && m_xState != MustAbort;
    }

    bool mustAbort () const {
	return m_xState == MustAbort;
    }

    bool waitingToAbort () const {
	return m_xState == WaitingToAbort;
    }
    bool waitingToResume () const {
	return m_xState == WaitingToResume;
    }

//  Mandated Access
public:
    virtual unsigned int cardinality_() const OVERRIDE;

    virtual rtPTOKEN_Handle *ptoken_() const OVERRIDE;

//  Access
public:
    bool cdatum (VDescriptor& rDatum) const;

    State state () const {
	return m_xState;
    }
    char const* stateName () const {
	return NameFor (state ());
    }

    VComputationUnit* suspendee () const {
	return m_pSuspendee;
    }

//  Use
public:
    bool commit ();

    bool abort ();
    bool resume ();

    bool setStateToWaitingToAbort ();
    bool setStateToWaitingToResume ();

    bool setDucFeathers (DSC_Pointer& rFeathers, VDescriptor& rValues) const;

//  State
protected:
    VReference<VComputationUnit> const	m_pSuspendee;
    VReference<VSuspension>		m_pSuccessor;
    State				m_xState;
};


#endif

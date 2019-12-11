#ifndef Vxa_VResultBuilder_Interface
#define Vxa_VResultBuilder_Interface

/*******************************************************************************
 *>>>>  ResultReturn: Introduction
 *
 *  To follow the trail of result return, follow the '>>>>  ResultReturn: Step n'
 *  markers. You'll find '>>>>  ResultReturn: Step 1' in this file.
 *
 *******************************************************************************/

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VTask.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICaller2.h"
#include "Vxa_VExportableDatum.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VResultBuilder {
	DECLARE_NUCLEAR_FAMILY (VResultBuilder);

    //****************************************************************
    //  RemoteControl
    public:
        typedef VTask::RemoteControl RemoteControl;

    //****************************************************************
    //  TailHints
    public:
	class Vxa_API TailHints : public VCursor::TailHints {
	    DECLARE_CONCRETE_RTTLITE (TailHints, VCursor::TailHints);

	//  Construction
	public:
	    TailHints (VResultBuilder const *pResultBuilder) : BaseClass (pResultBuilder->cursor ()) {
		pResultBuilder->attach (this);
	    }
	//  Destruction
	private:
	    ~TailHints () {
	    }
	};
	friend class TailHints;

    //****************************************************************
    //  Construction
    protected:
	VResultBuilder (VTask *pTask);

    //  Destruction
    protected:
	~VResultBuilder ();

    //  Access
    public:
        VTask *task () const {
            return m_pTask;
        }
	VTaskCursor *cursor () const {
	    return m_pTask->cursor ();
	}
	cardinality_t cursorPosition () const {
	    return m_pTask->cursorPosition ();
	}
	TailHints *tailHints () const {
	    return m_pTailHints ? m_pTailHints : new TailHints (this);
	}

        bool invokedIntensionally () const {
            return m_pTask->invokedIntensionally ();
        }
        cardinality_t parameterCount () const {
            return m_pTask->parameterCount ();
        }
        VString const &selectorName () const {
            return m_pTask->selectorName ();
        }
        VString const &selectorComponent (cardinality_t xComponent) const {
            return m_pTask->selectorComponent (xComponent);
        }
        bool selectorComponent (VString &rComponent, cardinality_t xComponent) const {
            return m_pTask->selectorComponent (rComponent, xComponent);
        }
        cardinality_t taskCardinality () const {
            return m_pTask->cardinality ();
        }

    //  Remote Control
    public:
        RemoteControl *getRemoteControl () const;

    private:
        virtual ICaller2 *getRemoteControlInterface () const {
            return 0;
        }

    //  Query
    public:
	bool isAlive () const {
	    return m_pTask->isAlive ();
	}

    //  Maintenance
    private:
	void attach (TailHints *pTailHints) const {
	    m_pTailHints.setIfNil (pTailHints);
	}

    //  Interface
    public:
/*******************************************************************************
 *>>>>  ResultReturn: Step 1
 *
 *  Result return begins here.  Vxa::VClass methods return values by accepting
 *  a parameter of type 'Vxa::VResultBuilder&' to which they assign values.
 *  The result builder does the work of associating and delivering, if possible,
 *  those values to the appropriate Vision peer task element.  Because the set
 *  of types is open-ended, these assignment operators are defined as templates.
 *  Vxa::VResultBuilder is an abstract class, however, that knows nothing about
 *  the particular capabilities of the interfaces obtained from Vision.  That
 *  information is captured in the appropriate concrete VResultBuilder subclass.
 *  Since templated member functions cannot be virtual, the next stop in the
 *  value return process is the VExportable<T> traits template.  Using its own
 *  virtual members, that template embeds the value in a concrete subclass of
 *  abstract class VExportableDatum and returns the now encapsulated datum
 *  to the result builder via the pure virtual 'returnResult' VResultBuilder
 *  member.  Implemented by the concrete result builder subclass, the result
 *  builder and the datum to be returned can now interact with each other using
 *  virtual and subclass specific members.
 *
 *  Next Stop: Vxa_VExportable.h
 *
 *******************************************************************************/
	template <typename T> ThisClass& operator= (T const &rT) {
	    setResultTo (rT);
	    return *this;
	}
	template <typename T, size_t N> ThisClass& operator= (T (&raT)[N]) {
	    setResultTo (raT);
	    return *this;
	}
        template <typename T, size_t N> void setResultTo (T (&raT)[N]) {
            VExportable<T*>::ReturnResult (this, raT);
        }
	template <typename T> void setResultTo (T const &rT) {
	    VExportable<T>::ReturnResult (this, rT);
	}

    //  Implementation
    public:
/*******************************************************************************
 *>>>>  ResultReturn: Step 3
 *
 *  Last Stop: Vxa_VExportable.h
 *  Next Stop:
 *
 *******************************************************************************/
	virtual bool returnResult (VExportableDatum const &rDatum) = 0;

    //  State
    private:
        VTask::Reference       const m_pTask;
	TailHints::Reference mutable m_pTailHints;
    };
}


#endif

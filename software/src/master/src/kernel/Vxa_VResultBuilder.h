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

#include "Vxa_VTaskCursor.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VExportableDatum.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VResultBuilder {
	DECLARE_NUCLEAR_FAMILY (VResultBuilder);

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
	VResultBuilder (VTaskCursor *pCursor) : m_pCursor (pCursor) {
	}

    //  Destruction
    protected:
	~VResultBuilder () {
	}

    //  Access
    public:
	VTaskCursor *cursor () const {
	    return m_pCursor;
	}
	cardinality_t cursorPosition () const {
	    return m_pCursor->position ();
	}
	TailHints *tailHints () const {
	    return m_pTailHints ? m_pTailHints : new TailHints (this);
	}

    //  Query
    public:
	bool isAlive () const {
	    return m_pCursor->isAlive ();
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
	VTaskCursor::Reference const m_pCursor;
	TailHints::Reference mutable m_pTailHints;
    };
}


#endif

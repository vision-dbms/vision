#ifndef Vca_VcaGofer_OneOf_Interface
#define Vca_VcaGofer_OneOf_Interface


/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "VkDynamicArrayOf.h"

#include "V_VRandom.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VRTTI.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Gofer {

    /*********************************************************************************************
     *  Can be generalized to any object, not just interfaces, provided interfaces continue to
     *  inherit from VGoferInterface<T> (non-interfaces must inherit from VGoferInstance<T>).
     *********************************************************************************************/

        /**
         * Gofer used to obtain a value from a collection of candidates (gofer array), sampling without replacement (keeping track of failures) and automatically trying a new candidate upon failure until either a candidate succeeds or all candidates have been exhausted.
         */
	template <typename Interface_T> class OneOf : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (OneOf<Interface_T>, VGoferInterface<Interface_T>);

	//  Aliases
	public:
	    typedef typename BaseClass::IDataReceiver IObjectReceiver;
	    typedef typename BaseClass::IDataSupplier IObjectSupplier;

	    typedef BaseClass gofer_t;
	    typedef typename gofer_t::Reference gofer_reference_t;
	    typedef VkDynamicArrayOf<gofer_reference_t> gofer_array_t;

	//  Globals
	public:
	    using BaseClass::TracingGofers;

	//  Initializer
	public:
            /**
             * Helper class used to initialize alternatives list for OneOf gofers.
             */
	    class Initializer {
		DECLARE_NUCLEAR_FAMILY (Initializer);

	    //  Construction
	    protected:
                /** Empty constructor restricts scope. */
		Initializer () {
		}

	    //  Destruction
	    protected:
                /** Empty destructor restricts scope. */
		~Initializer () {
		}

	    //  Use
	    public:
                /**
                 * Populates an array with gofers to be used as candidates for a OneOf.
                 */
		virtual void initialize (gofer_array_t &rGoferArray) const = 0;
	    };

	//  Construction
	public:
            /**
             * Constructor for OneOf gofers expects an Initializer subclass instance.
             * Initializes our candidate list using the provided Initializer.
             *
             * @param rInitializer the Initializer to use to retrieve candidates.
             */
	    OneOf (Initializer const &rInitializer) : m_xCandidateArray (0) {
		rInitializer.initialize (m_aCandidates);
	    }

	//  Destruction
	private:
            /** Empty destructor restricts scope. */
	    ~OneOf () {
	    }

	//  Access/Query
	public:
            /** Returns the number of candidates that we have. */
	    unsigned int cardinality () const {
		return m_aCandidates.cardinality ();
	    }

            /** Returns the number of candidates that we have left to try. */
	    unsigned int candidateCount () const {
		return cardinality () - m_xCandidateArray;
	    }

            /**
             * Used to determine if there are any remaining candidates to try.
             *
             * @return true if there are more than zero candidates left to try, false otherwise.
             */
	    bool moreCandidates () const {
		return candidateCount () > 0;
	    }

            /**
             * Used to determine if there are any remaining candidates to try.
             *
             * @return true if there are no candidates left to try, false otherwise.
             */
	    bool noCandidates () const {
		return candidateCount () == 0;
	    }

	//  Callbacks and Triggers
	private:
            /**
             * Samples without replacement from our candidate list.
             */
	    void onData () {
		if (TracingGofers ()) {
		    VString iDesc;
		    BaseClass::getDescription (iDesc);
		    printf ("+++ %s[%llp]::onData ()\n", iDesc.content (), this);
		}

	    //  Reset the randomizer, ...
		resetElementSelector ();

	    //  ... and pretend our last 'candidate' didn't work:
		onCandidateError (0, "No Choice");
	    }

            /**
             * Marks a candidate as 'errored' so that it will not be returned (again) by this OneOf, and automatically chooses a next candidate as possible.
             * Called when the last candidate attempt has failed.
             *
             * @param pInterface the error interface for the candidate error.
             * @param rMessage the error message.
             */
	    void onCandidateError (IError *pInterface, VString const &rMessage) {
		if (TracingGofers ()) {
		    VString iDesc;
		    BaseClass::getDescription (iDesc);
		    printf ("+++ %s[%llp]::onCandidateError (%s)\n", iDesc.content (), this, rMessage.content ());
		}

		gofer_reference_t pChoice;
		while (moreCandidates () && pChoice.isNil ())
		    chooseNext (pChoice);

		if (pChoice)
                    // Since each candidate is a gofer, we attempt retrieveal using said gofer. The gofer then indicates errors by another call to this method.
		    pChoice->supplyMembers (this, &ThisClass::onCandidateValue, &ThisClass::onCandidateError);
		else {
		    BaseClass::onError (pInterface, rMessage);
		}
	    }

            /**
             * Called by candidate retrieval gofers upon successful retrieval; sets this gofer's result to the retrieved result.
             *
             * @param pValue the retrieved candidate result to be stored as this gofer's result.
             */
	    void onCandidateValue (Interface_T *pValue) {
		BaseClass::setTo (pValue);
	    }

	//  Randomization
	private:
            /**
             * Randomly selects candidates from our candidate array.
             */
	    void chooseNext (gofer_reference_t &rpChoice) {
	    //  If the candidate array is exhausted, start over:
		if (noCandidates ())
		    resetElementSelector ();

	    //  For non-trivial candidate arrays ...
		unsigned int const sCandidateArray = candidateCount ();
		if (TracingGofers ()) {
		    VString iDesc;
		    BaseClass::getDescription (iDesc);
		    printf (
			"+++ %s[%llp]: Choosing one of [%u..%u)...", iDesc.content (), this, m_xCandidateArray, m_xCandidateArray + sCandidateArray
		    );
		}
		if (sCandidateArray > 0) {
		//  ... and non-trivial choices, ...
		    if (sCandidateArray > 1) {
		    //  ... choose a random element from the candidate array, ...
			unsigned int const xChoice = m_xCandidateArray + V::VRandom::BoundedValue (sCandidateArray);

			if (TracingGofers ())
			    printf ("%u", xChoice);

		    //  ... swap it with the first element of the candidate array, ...
			gofer_reference_t const pOldFirst (m_aCandidates[m_xCandidateArray]);
			m_aCandidates[m_xCandidateArray].setTo (m_aCandidates[xChoice]);
			m_aCandidates[xChoice].setTo (pOldFirst);
		    }

		//  ... return the chosen candidate, ...
		    rpChoice.setTo (m_aCandidates[m_xCandidateArray]);

		//  ... and shrink the candidate array by one.
		    m_xCandidateArray++;
		}
		if (TracingGofers ())
		    printf ("\n");
	    }

            /**
             * Resets our candidate selection state so that all candidates once again become available (as though none had errored).
             */
	    void resetElementSelector () {
	    //  ... and start with all alternates available for selection:
		m_xCandidateArray = 0;
	    }

	//  Gofer Callbacks
	protected:
            /**
             * Resets candidate availability; delegates to resetElementSelector().
             */
	    void onReset () { // onGofeReset
		if (TracingGofers ()) {
		    VString iDesc;
		    BaseClass::getDescription (iDesc);
		    printf ("+++ %s[%llp]: Choice 'onReset'\n", iDesc.content (), this);
		}
		resetElementSelector ();
		BaseClass::onReset ();
	    }

	//  State
	private:
            /** Array of candidates. */
	    gofer_array_t m_aCandidates;
            /** Index of the first available candidate within m_aCandidates. */
	    unsigned int m_xCandidateArray;
	};
    }
}


#endif /* Vca_VcaGofer_OneOf_Interface */

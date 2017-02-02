#ifndef Vxa_VCursor_Interface
#define Vxa_VCursor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_VCardinalityHints.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VFiniteSet.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCursor : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VCursor, VRolePlayer);

    //  Aliases
    public:
	typedef Reference CursorReference;

    //  class CardinalityHints
    public:
	class Vxa_API CardinalityHints : public VCardinalityHints {
	    DECLARE_ABSTRACT_RTTLITE (CardinalityHints,VCardinalityHints);

	//  Construction
	protected:
	    CardinalityHints (VCursor *pCursor);

	//  Destruction
	protected:
	    ~CardinalityHints ();

	//  Access
	public:
	    VCursor *cursor () const {
		return m_pCursor;
	    }

	//  State
	private:
	    CursorReference const m_pCursor;
	};

    //  template <typename WhichEnd> class CardinalityHintsFor
    public:
	template <typename WhichEnd> class CardinalityHintsFor : public CardinalityHints {
	    DECLARE_CONCRETE_RTTLITE (CardinalityHintsFor<WhichEnd>,CardinalityHints);

	//  Construction
	public:
	    CardinalityHintsFor (VCursor *pCursor) : BaseClass (pCursor) {
	    }

	//  Destruction
	protected:
	    ~CardinalityHintsFor () {
	    }

	//  Access
	private:
	    cardinality_t upperBound_() const {
		return upperBound ();
	    }
	public:
	    cardinality_t upperBound () const {
		return WhichEnd::cardinality (cursor ());
	    }
	};
	struct Vxa_API Head {
	    static cardinality_t cardinality (VCursor *pCursor) {
		return pCursor->headCardinality ();
	    }
	};
	struct Vxa_API Tail {
	    static cardinality_t cardinality (VCursor *pCursor) {
		return pCursor->tailCardinality ();
	    }
	};
	typedef CardinalityHintsFor<Head> HeadHints;
	typedef CardinalityHintsFor<Tail> TailHints;

    //  Construction
    protected:
	VCursor (VFiniteSet *pSet);

    //  Destruction
    protected:
	~VCursor ();

    //  Access
    public:
	VFiniteSet *set () const {
	    return m_pSet;
	}

	cardinality_t cardinality () const {
	    return m_pSet->cardinality ();
	}
	cardinality_t headCardinality () const {
	    return m_xPosition;
	}
	cardinality_t tailCardinality () const {
	    return cardinality () - headCardinality ();
	}
	cardinality_t position () const {
	    return headCardinality ();
	}
	cardinality_t remainder () const {
	    return tailCardinality ();
	}

    //  Query
    public:
	bool inBounds () const {
	    return m_xPosition < cardinality ();
	}

    //  Update
    public:
	void first () {
	    m_xPosition = 0;
	}
	void next () {
	    m_xPosition++;
	}

    //  State
    private:
	VFiniteSet::Reference const m_pSet;
	cardinality_t m_xPosition;
    };
}


#endif

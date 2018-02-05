/*****  Vxa_VSerializer2DynArray Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#define   Vxa_VSerializer2DynArray
#include "Vxa_VSerializer2DynArray.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VcaSerializer_.h"
#include "Vca_VTypeInfoHolderFor.h"


/********************
 ********************
 *****  Logger  *****
 ********************
 ********************/

#include "V_VLogger.h"

namespace {
    V::VLogger g_iLogger (getenv ("VxaS2LogFile"), false);
}


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::VTypeInfoHolderForSerializable_  *****
 *****                                        *****
 **************************************************
 **************************************************/

namespace Vca {
    template <typename T> class VTypeInfoHolderForSerializable_ : public VTypeInfoHolderFor<T> {
	DECLARE_FAMILY_MEMBERS (VTypeInfoHolderForSerializable_<T>, VTypeInfoHolderFor<T>);

    //  Reference Type
    public:
	typedef typename BaseClass::ReferenceType ReferenceType;

    //  Construction
    public:
	VTypeInfoHolderForSerializable_(
	    unsigned int  iD1  , unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
	) : BaseClass (
	    iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7
	) {
	}

    //  Serialization
    public:
	VcaSerializer *serializer_(VcaSerializer *pCaller, ReferenceType rT) const {
	    return new typename T::Serializer (pCaller, rT);
	}
    };
}


/*********************************
 *********************************
 *****                       *****
 *****  Vxa::SerializerBase  *****
 *****                       *****
 *********************************
 *********************************/

namespace Vxa {
    class SerializerBase : public Vca::VcaSerializer {
	DECLARE_ABSTRACT_RTTLITE (SerializerBase, Vca::VcaSerializer);

    //  Construction
    protected:
	SerializerBase (Vca::VcaSerializer *pCaller) : BaseClass (pCaller) {
	}

    //  Destruction
    protected:
	~SerializerBase () {
	}

    //  Component Serialization
    protected:
	template <typename T> void serializeComponent (T &rT) {
	    start (this, Vca::VTypeInfoHolder<T>::serializer (this, rT));
	}

    //  I/O
    protected:
	using BaseClass::get;
	template <typename T> void get (T &rT) {
	    get (&rT, sizeof (T));
	}

	using BaseClass::put;
	template <typename T> void put (T const &rT) {
	    put (&rT, sizeof (T));
	}
    };
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vxa::VSerializer2DynArrayOfBoolean  *****
 *****                                      *****
 ************************************************
 ************************************************/

class Vxa::VSerializer2DynArrayOfBoolean::Serializer : public SerializerBase {
    DECLARE_CONCRETE_RTTLITE (Serializer, SerializerBase);

//  Aliases
public:
    typedef Vca::VTrigger<ThisClass> sequencer_t;

public:
    Serializer(
	Vca::VcaSerializer *pCaller, serializer2_t &rDatum
    ) : BaseClass (pCaller), m_rDatum (rDatum) {
	m_pSequencer.setTo (new sequencer_t (this, &ThisClass::doData));
    }

//  Destruction
protected:
    ~Serializer() {
    }

//  Sequencer Actions
protected:
    void doData (sequencer_t *pSequencer) {
	start (this, Vca::VTypeInfoHolder<dynarray_t>::serializer (this, m_rDatum.operator dynarray_t& ()));
	clearSequencer ();
    }

//  State
protected:
    serializer2_t &m_rDatum;
};


/************************************************
 ************************************************
 *****                                      *****
 *****  Vxa::VSerializer2DynArrayOfInteger  *****
 *****                                      *****
 ************************************************
 ************************************************/

class Vxa::VSerializer2DynArrayOfInteger::Serializer : public SerializerBase {
    DECLARE_CONCRETE_RTTLITE (Serializer, SerializerBase);

//  Aliases
public:
    typedef dynarray_t::element_count_t element_count_t;
    typedef dynarray_t::element_index_t element_index_t;

    typedef Vca::VTrigger<ThisClass> sequencer_t;

//  States and Types
public:
    enum State {
	State_Initial,
	State_FindWork,
	State_SerializingArrayNumber,
	State_SerializingElementCount,
	State_SerializingBlockType,
	State_SerializingBlockSize,
	State_SerializingArray,
	State_SerializingBlockOrigin,
	State_SerializingBlockDelta,
	State_SerializingReferenceData
    };
    enum BlockType {
	BlockType_Array,
	BlockType_Repeat,
	BlockType_Plus1,
	BlockType_Minus1,
	BlockType_Sequence
    };

    char const *stateName () const;
    char const *blockTypeName () const;

//  Constants
public:
    static element_count_t const g_sSmallestInterestingRun = 4; // must be > 2

//  Construction
public:
    Serializer (Vca::VcaSerializer *pCaller, serializer2_t &rDatum);

//  Destruction
private:
    ~Serializer() {
	if (m_pReferenceData)
	    deallocate (m_pReferenceData);
    }

//  Access/Query
public:
    bool done () const {
	return elementCount () <= m_xElement;
    }
    static size_t elementAllocation (element_count_t cElements) {
	return cElements * sizeof (val_t);
    }
    size_t elementAllocation () const {
	return elementAllocation (elementCount ());
    }
    element_count_t elementCount () const {
	return m_rDynArray.elementCount ();
    }
    element_count_t remainder () const {
	return elementCount () - m_xElement;
    }


//  State Machine
private:
    void doUntilDone (sequencer_t *pSequencer);

private:
    bool blockable (element_count_t xElement) const {
	return xElement + g_sSmallestInterestingRun <= elementCount ();
    }
    bool blockable () const {
	return blockable (m_xElement);
    }
    bool findBlock ();

    bool inBlock () {
	return isIncoming () ? blockable () : findBlock ();
    }

    void outputRepeat ();
    void outputPlus1 ();
    void outputMinus1 ();
    void outputSequence ();

    int const *putableArray () const {
	return const_cast<dynarray_t const&>(m_rDynArray).elementArray ();
    }
    int const *putables () const {
	return putableArray () + m_xElement;
    }
    int *getables () {
	return m_rDynArray.elementArray () + m_xElement;
    }

//  Serialization Callbacks
private:
    void getData () OVERRIDE;
    void putData () OVERRIDE;
    void localizeData () OVERRIDE;

    void wrapupIncomingSerialization () OVERRIDE;
    void wrapupOutgoingSerialization () OVERRIDE;

//  State
private:
    static bool		const	g_bReferenceData;
    static unsigned int	const	g_vReferenceData = 0x80000000;
    static unsigned int		g_xArray;

    State		m_xState;
    serializer2_t&	m_rDatum;
    dynarray_t&		m_rDynArray;
    unsigned int	m_xArray;
    element_count_t	m_cElements;
    element_index_t	m_xElement;
    val_t		m_iBlockOrigin;
    val_t		m_iBlockDelta;
    unsigned char 	m_xBlockType;
    bool		m_bReferenceData;
    val_t*		m_pReferenceData;
};

unsigned int Vxa::VSerializer2DynArrayOfInteger::Serializer::g_xArray = 0;

bool const Vxa::VSerializer2DynArrayOfInteger::Serializer::g_bReferenceData = (
    getenv ("VxaS2SendingRefData") && atoi (getenv ("VxaS2SendingRefData")) ? true : false
);


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VSerializer2DynArrayOfInteger::Serializer::Serializer (
    Vca::VcaSerializer *pCaller, serializer2_t &rDatum
) : BaseClass		(pCaller),
    m_xState		(State_Initial),
    m_rDatum		(rDatum),
    m_rDynArray		(rDatum),
    m_cElements		(elementCount ()),
    m_xElement		(0),
    m_xArray		(0),
    m_xBlockType	(BlockType_Array),
    m_bReferenceData	(false),
    m_pReferenceData	(0)
{
    m_pSequencer.setTo (new sequencer_t (this, &ThisClass::doUntilDone));
}


/***************************
 ***************************
 *****  State Machine  *****
 ***************************
 ***************************/

void Vxa::VSerializer2DynArrayOfInteger::Serializer::doUntilDone (sequencer_t *pSequencer) {
    bool bNotDone = true;
    while (runnable () && bNotDone) {
	switch (m_xState) {
	case State_Initial:
	    m_xState = State_SerializingArrayNumber;
	    if (isOutgoing ()) {
		m_xArray = g_xArray++;
		if (g_bReferenceData)
		    m_xArray += g_vReferenceData;
	    }
	    transferData ();
	    break;
	case State_SerializingArrayNumber:
	    m_bReferenceData = m_xArray & g_vReferenceData ? true : false;
	    if (m_bReferenceData) {
		m_xArray -= g_vReferenceData;	//  ... remove the reference data serialization bit from 'm_xArray'.
	    }
	    m_xState = State_SerializingElementCount;
	    transferData ();
	    break;
	case State_SerializingElementCount:
	    if (isIncoming ())
		m_rDynArray.Append (m_cElements);
 	    if (g_iLogger.isActive ())
		g_iLogger.printf ("%5u: %s %u Elements\n", m_xArray, (isIncoming () ? "Incoming" : "Outgoing"), elementCount ());
	//  No break;
	case State_FindWork:
	    if (done ()) {
		if (m_bReferenceData) {
		    m_xState = State_SerializingReferenceData;
		    transferData ();
		}
		bNotDone = false;
		clearSequencer ();
	    } else if (inBlock ()) {
		m_xState = State_SerializingBlockType;
		transferData ();
	    } else { // ... too little to bother with.
		m_xState = State_SerializingArray;
		m_cElements = remainder ();
		transferData ();
	    }
	    break;
	case State_SerializingBlockType:
	    m_xState = State_SerializingBlockSize;
	    transferData ();
	    break;
	case State_SerializingBlockSize:
	    switch (m_xBlockType) {
	    case BlockType_Array:
		m_xState = State_SerializingArray;
		transferData ();
		break;
	    case BlockType_Repeat:
	    case BlockType_Plus1:
	    case BlockType_Minus1:
	    case BlockType_Sequence:
		m_xState = State_SerializingBlockOrigin;
		transferData ();
		break;
	    }
	    break;
	case State_SerializingArray:
 	    if (g_iLogger.isActive () && isIncoming ()) {
		VString iWhat;
		iWhat.printf ("Incoming %s at %u", blockTypeName (), m_xElement);
		g_iLogger.log (m_xArray, iWhat, getables (), m_cElements);
	    }
	    m_xElement += m_cElements;
	    m_xState = State_FindWork;
	    break;
	case State_SerializingBlockOrigin:
	    if (BlockType_Sequence == m_xBlockType) {
		m_xState = State_SerializingBlockDelta;
		transferData ();
	    } else {
		m_xState = State_FindWork;
		if (isOutgoing ())
		    m_xElement += m_cElements;
		else switch (m_xBlockType) {
		case BlockType_Repeat:
		    outputRepeat ();
		    break;
		case BlockType_Plus1:
		    outputPlus1 ();
		    break;
		case BlockType_Minus1:
		    outputMinus1 ();
		    break;
		}
	    }
	    break;
	case State_SerializingBlockDelta:
	    m_xState = State_FindWork;
	    if (isOutgoing ())
		m_xElement += m_cElements;
	    else
		outputSequence ();
	    break;
	default:
	    bNotDone = false;
	    abort ();
	    break;
	}
    }
}


/****************************************
 *----  State Machine Block Finder  ----*
 ****************************************/

bool Vxa::VSerializer2DynArrayOfInteger::Serializer::findBlock () {
    int const *const pElements = putables ();

    element_index_t const xElementLimit = remainder ();
    if (xElementLimit >= g_sSmallestInterestingRun) { //  ... only bother if there are enough elements
    //  Look for an initial pattern in the elements ...
	element_index_t xNextElement = 1;
	int iBlockDelta = pElements[xNextElement] - pElements[xNextElement - 1];
	while (
	    ++xNextElement < xElementLimit &&
	    iBlockDelta == pElements[xNextElement] - pElements[xNextElement - 1]
	);

    //  At this point, 'xNextElement' identifies the position of the element that
    //  broke the pattern (or the end of the array).  If the pattern is sufficiently
    //  long, return it:
	if (xNextElement >= g_sSmallestInterestingRun) {
	    m_iBlockOrigin = pElements[0];
	    m_iBlockDelta = iBlockDelta;
	    m_cElements = xNextElement;
	    switch (iBlockDelta) {
	    case -1:
		m_xBlockType = BlockType_Minus1;
		break;
	    case 0:
		m_xBlockType = BlockType_Repeat;
		break;
	    case 1:
		m_xBlockType = BlockType_Plus1;
		break;
	    default:
		m_xBlockType = BlockType_Sequence;
		break;
	    }

	    if (g_iLogger.isActive ()) {
		switch (m_xBlockType) {
		case BlockType_Repeat:
		case BlockType_Plus1:
		case BlockType_Minus1:
		    g_iLogger.printf (
			"%5u: Outgoing %s at %u from %d for %u\n", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements
		    );
		    break;
		case BlockType_Sequence:
		    g_iLogger.printf (
			"%5u: Outgoing %s at %u from %d for %u by %d\n", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements, m_iBlockDelta
		    );
		    break;
		}
	    }
	    return true;
	}

    //  At this point, we're dealing with an array.  If enough elements remain, ...
	element_index_t xLookaheadLimit = xNextElement + g_sSmallestInterestingRun;
	while (xElementLimit >= xLookaheadLimit) {
	//  ... find the start of a pattern that might end it:
	    do {
		iBlockDelta = pElements[xNextElement] - pElements[xNextElement - 1];
	    } while (
		++xNextElement < xElementLimit &&
		iBlockDelta != pElements[xNextElement] - pElements[xNextElement - 1]
	    );

	//  At this point, 'xNextElement' identifies the position of the third element
	//  in a possible pattern (or the end of the list):
	//
	//	pElements[xNextElement] - pElements[xNextElement - 1] ==
	//	pElements[xNextElement - 1] - pElements[xNextElement - 2]
	//
	//  See if the pattern is sufficiently long to stand on its own:

	    xLookaheadLimit = xNextElement + g_sSmallestInterestingRun;
	    if (xLookaheadLimit > xElementLimit)
		xLookaheadLimit = xElementLimit;
	    element_index_t xNextLookahead = xNextElement;
	    while (
		++xNextLookahead < xLookaheadLimit &&
		iBlockDelta == pElements[xNextLookahead] - pElements[xNextLookahead - 1]
	    );

	//  If the pattern checked out.  We've found the limit of our array:
	    if (xNextLookahead >= xLookaheadLimit) {
		m_xBlockType = BlockType_Array;
		m_cElements = xNextElement - 2;
		if (g_iLogger.isActive ()) {
		    VString iWhat;
		    iWhat.printf ("Outgoing %s at %u", blockTypeName (), m_xElement);
		    g_iLogger.log (m_xArray, iWhat, pElements, m_cElements);
		}
		return true;
	    }
	}
    }

//  If we get here, no interesting patterns were found.  Punt...
    m_xBlockType = BlockType_Array;
    m_cElements = xElementLimit;
    if (g_iLogger.isActive ()) {
	VString iWhat;
	iWhat.printf ("Outgoing %s at %u", blockTypeName (), m_xElement);
	g_iLogger.log (m_xArray, iWhat, pElements, m_cElements);
    }
    return false;
}


/********************************************
 *----  State Machine Output Utilities  ----*
 ********************************************/

void Vxa::VSerializer2DynArrayOfInteger::Serializer::outputRepeat () {
    if (g_iLogger.isActive ()) {
	g_iLogger.printf ("%5u: Incoming %s at %u from %d for %u\n", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements);
    }
    int *const pElements = getables ();
    for (unsigned int xElement = 0; xElement < m_cElements; xElement++)
	pElements[xElement] = m_iBlockOrigin;
    m_xElement += m_cElements;
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::outputPlus1 () {
    if (g_iLogger.isActive ()) {
	g_iLogger.printf ("%5u: Incoming %s at %u from %d for %u\n", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements);
    }
    int *const pElements = getables ();
    for (unsigned int xElement = 0; xElement < m_cElements; xElement++)
	pElements[xElement] = m_iBlockOrigin++;
    m_xElement += m_cElements;
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::outputMinus1 () {
    if (g_iLogger.isActive ()) {
	g_iLogger.printf ("%5u: Incoming %s at %u from %d for %u\n", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements);
    }
    int *const pElements = getables ();
    for (unsigned int xElement = 0; xElement < m_cElements; xElement++)
	pElements[xElement] = m_iBlockOrigin--;
    m_xElement += m_cElements;
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::outputSequence () {
    if (g_iLogger.isActive ()) {
	g_iLogger.printf ("%5u: Incoming %s at %u from %d for %u by %d", m_xArray, blockTypeName (), m_xElement, m_iBlockOrigin, m_cElements, m_iBlockDelta);
    }
    int *const pElements = getables ();
    for (unsigned int xElement = 0; xElement < m_cElements; xElement++) {
	pElements[xElement] = m_iBlockOrigin;
	m_iBlockOrigin += m_iBlockDelta;
    }
    m_xElement += m_cElements;
}


/*************************************************
 *----  State Machine Description Utilities  ----*
 *************************************************/

char const *Vxa::VSerializer2DynArrayOfInteger::Serializer::stateName () const {
    switch (m_xState) {
    case State_Initial:
	return "Initial";
    case State_FindWork:
	return "FindWork";
    case State_SerializingArrayNumber:
	return "SerializingArrayNumber";
    case State_SerializingElementCount:
	return "SerializingElementCount";
    case State_SerializingBlockType:
	return "SerializingBlockType";
    case State_SerializingBlockSize:
	return "SerializingBlockSize";
    case State_SerializingArray:
	return "SerializingArray";
    case State_SerializingBlockOrigin:
	return "SerializingBlockOrigin";
    case State_SerializingBlockDelta:
	return "SerializingBlockDelta";
    case State_SerializingReferenceData:
	return "SerializingReferenceData";
    };
    return "???";
}

char const *Vxa::VSerializer2DynArrayOfInteger::Serializer::blockTypeName () const {
    switch (m_xBlockType) {
    case BlockType_Array:
	return "Array";
    case BlockType_Repeat:
	return "Repeat";
    case BlockType_Plus1:
	return "Plus1";
    case BlockType_Minus1:
	return "Minus1";
    case BlockType_Sequence:
	return "Sequence";
    };
    return "???";
}


/*************************************
 *************************************
 *****  Serialization Callbacks  *****
 *************************************
 *************************************/

void Vxa::VSerializer2DynArrayOfInteger::Serializer::getData () {
    switch (m_xState) {
    case State_SerializingArrayNumber:
	get (m_xArray);
	break;
    case State_SerializingElementCount:
    case State_SerializingBlockSize:
	get (m_cElements);
	break;
    case State_SerializingBlockType:
	get (m_xBlockType);
	break;
    case State_SerializingBlockOrigin:
	get (m_iBlockOrigin);
	break;
    case  State_SerializingBlockDelta:
	get (m_iBlockDelta);
	break;
    case State_SerializingArray:
	get (getables (), elementAllocation (m_cElements));
	break;
    case State_SerializingReferenceData:
	m_pReferenceData = reinterpret_cast<int*>(allocate (elementAllocation ()));
	get (m_pReferenceData, elementAllocation ());
	break;
    }
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::putData () {
    switch (m_xState) {
    case State_SerializingArrayNumber:
	put (m_xArray);
	break;
    case State_SerializingElementCount:
    case State_SerializingBlockSize:
	put (m_cElements);
	break;
    case State_SerializingBlockType:
	put (m_xBlockType);
	break;
    case State_SerializingBlockOrigin:
	put (m_iBlockOrigin);
	break;
    case  State_SerializingBlockDelta:
	put (m_iBlockDelta);
	break;
    case State_SerializingArray:
	put (putables (), elementAllocation (m_cElements));
	break;
    case State_SerializingReferenceData:
	put (putableArray (), elementAllocation ());
	break;
    }
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::localizeData () {
    switch (m_xState) {
    case State_SerializingArrayNumber:
	peer ()->localize (m_xArray);
	break;
    case State_SerializingElementCount:
    case State_SerializingBlockSize:
	peer ()->localize (m_cElements);
	break;
    case State_SerializingBlockType:
	peer ()->localize (m_xBlockType);
	break;
    case State_SerializingBlockOrigin:
	peer ()->localize (m_iBlockOrigin);
	break;
    case  State_SerializingBlockDelta:
	peer ()->localize (m_iBlockDelta);
	break;
    case State_SerializingArray:
	peer ()->localize (*getables (), m_cElements);
	break;
    case State_SerializingReferenceData:
	peer ()->localize (*m_pReferenceData, elementCount ());
	break;
    }
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::wrapupIncomingSerialization () {
    if (m_pReferenceData) {
	bool const bReferenceDataMismatch = 0 != memcmp (m_pReferenceData, putableArray (), elementAllocation ());
	if (g_iLogger.isActive ()) {
	    g_iLogger.log (m_xArray, "Result Test", bReferenceDataMismatch ? "FAILED" : "PASSED");
	    if (bReferenceDataMismatch) {
		g_iLogger.log (m_xArray, "Serialized Data", putableArray (), elementCount ());
		g_iLogger.log (m_xArray, "Reference Data", m_pReferenceData, elementCount ());
	    }
	}
	if (bReferenceDataMismatch)
	    fprintf (stderr, "+++ %u[%u] Reference Data Mismatch\n", m_xArray, elementCount ());
    }
    if (g_iLogger.isActive ())
	g_iLogger.flush ();
}

void Vxa::VSerializer2DynArrayOfInteger::Serializer::wrapupOutgoingSerialization () {
    if (g_iLogger.isActive ())
	g_iLogger.flush ();
}


/********************************************************
 ********************************************************
 *****                                              *****
 *****  Vxa::VSerializer2DynArray Type Information  *****
 *****                                              *****
 ********************************************************
 ********************************************************/

using namespace Vca;

namespace {
    // {2E320788-F6B7-47ef-9940-5BCEA251DE7C}
    VTypeInfoHolderForSerializable_<Vxa::bool_s2array_t> const VTypeInfoHolderFor_bool_s2array (
	0x2e320788, 0xf6b7, 0x47ef, 0x99, 0x40, 0x5b, 0xce, 0xa2, 0x51, 0xde, 0x7c
    );

    // {1403CB6B-907D-4449-A1BA-B5AD38B14FFE}
    VTypeInfoHolderForSerializable_<Vxa::i32_s2array_t> const VTypeInfoHolderFor_i32_s2array (
	0x1403cb6b, 0x907d, 0x4449, 0xa1, 0xba, 0xb5, 0xad, 0x38, 0xb1, 0x4f, 0xfe
    );
    
    // {30D04295-ED56-458a-9EDA-210B6ECAB1E4}
    VTypeInfoHolderFor<Vxa::bool_s2array_t const&> const VTypeInfoHolderFor_bool_s2array_const_r (
	0x30d04295, 0xed56, 0x458a, 0x9e, 0xda, 0x21, 0xb, 0x6e, 0xca, 0xb1, 0xe4
    );

    // {2E8FCA7D-6B38-466d-B0E9-B84CF00A15DD}
    VTypeInfoHolderFor<Vxa::i32_s2array_t const&> const VTypeInfoHolderFor_i32_s2array_const_r (
	0x2e8fca7d, 0x6b38, 0x466d, 0xb0, 0xe9, 0xb8, 0x4c, 0xf0, 0xa, 0x15, 0xdd
    );
    
}

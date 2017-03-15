#ifndef VAssociative_Interface
#define VAssociative_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VfLocateOrAddGenerator.h"
#include "VfLocateGenerator.h"
#include "VfDeleteGenerator.h"

/*********************
 *****  Classes  *****
 *********************/

/*---------------------------------------------------------------------
 *  Notes:
 *	- The scalar 'Insert' and 'Delete' routines return true if the set was
 *	  updated and false if no changes were made.
 *	- The scalar 'Locate' routines return true if the element was found
 *	  and false otherwise.
 *---------------------------------------------------------------------
 */

class VAssociativeCursorBase : public VTransient {
//  Construction
protected:
    VAssociativeCursorBase (unsigned int xElementLimit)
	: m_xElementLimit (xElementLimit), m_xElement (0)
    {
    }

//  Destruction
protected:
    ~VAssociativeCursorBase () {
    }

//  Use
public:
    void resetCursor () {
	m_xElement = 0;
    }

    bool elementIsValid () const {
	return m_xElement < m_xElementLimit;
    }
    unsigned int elementIndex () const {
	return m_xElement;
    }

//  State
protected:
    unsigned int m_xElement;
    unsigned int m_xElementLimit;
};


/***********************
 *****  Templates  *****
 ***********************/

template <class SetClass, class KeyClass, class KeyValueType> class VAssociativeCursor : public VAssociativeCursorBase {
//  Construction
public:
    VAssociativeCursor (SetClass* pSet)
	: VAssociativeCursorBase (pSet->cardinality ())
	, m_pSet (pSet)
    {
    }

//  Destruction
public:
    ~VAssociativeCursor () {
    }

//  Operator Initialization
protected:
    void initializeGenerator (VAssociativeOperator& rOperator, rtPTOKEN_Handle *pSourcePToken) {
	rOperator.initialize (m_pSet->ptoken (), pSourcePToken);
    }

//  Use
public:
    void Insert (KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult);
    void Insert (VOrdered<KeyClass,KeyValueType>&rKeys, VAssociativeResult& rResult);

    bool Insert (KeyValueType iKeyValue);
    bool Insert (KeyValueType iKeyValue, unsigned int& rxElement) {
	bool added = Insert (iKeyValue);
	rxElement = elementIndex ();
	return added;
    }

    void Delete (KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult);
    void Delete (VOrdered<KeyClass,KeyValueType>&rKeys, VAssociativeResult& rResult);

    bool Delete (KeyValueType iKeyValue);

    void Locate (KeyClass *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult);
    void Locate (VOrdered<KeyClass,KeyValueType>&rKeys, VAssociativeResult& rResult);

    bool Locate (KeyValueType iKeyValue);
    bool Locate (KeyValueType iKeyValue, unsigned int& rxElement) {
	bool found = Locate (iKeyValue);
	rxElement = elementIndex ();
	return found;
    }

//  State
protected:
    SetClass* m_pSet;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VAssociativeCursor.i")
#else
#include "VAssociativeCursor.i"
#endif


#endif

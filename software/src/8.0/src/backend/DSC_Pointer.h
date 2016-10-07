#ifndef DSC_Pointer_Interface
#define DSC_Pointer_Interface

/************************
 *****  Components  *****
 ************************/

#include "DSC_Scalar.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "RTlink.h"


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * Pointers select elements from a positional structure.  Pointers attempt to
 * consolidate in one abstraction three concepts:
 *
 *	- scalars versus collections
 *	- values versus positions
 *	- ordered versus unordered positional collections
 *
 * (More will be said about these concepts when time pressures are fewer...)
 *
 * Pointers which can be replaced by equivalent ordered positional collections
 * (i.e., links) are termed link equivalent.  The only pointer type which is
 * NOT link equivalent is 'Reference'.  Two different pointer type names are
 * provided to preserve this distinction notionally in the programs which use
 * pointers; however, the use of common macros precludes the creation of
 * physically distinct types.
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * In all discussions of 'Pointers', the terms 'ordered collection' and
 * 'ordered' refer to 'user' defined and controlled orderings.  These
 * orderings impose no implicit content based constraints on the order in
 * which two or more elements can appear in a collection.
 *
 * A pointer is termed link equivalent if it can be translated into an
 * equivalent link for purposes of positional access to a structure.
 *---------------------------------------------------------------------------
 */

/***************************
 *****  Pointer Types  *****
 ***************************/

/*---------------------------------------------------------------------------
 * Pointer Types (Definitions found in 'VkScalar.h'):
 *	Empty			- the nil pointer - no positional relationship
 *				  is defined or implied by this pointer type.
 *	Scalar			- an optimization of 'Link/Reference' and
 *				  'Values' for singletons.  Scalars of R-Type
 *				  'RefUV' behave like a 'Link/Reference' of
 *				  one element.  Scalars of all other R-Types
 *				  behave like one element 'Value's.  Scalars
 *				  are link equivalent.
 *	Value			- an ordered collection of information bearing
 *				  values.  Values typically represent ordered
 *				  collections of integers, reals, dates, and
 *				  other infinite types.  All elements of a
 *				  'Value' translate into reference 'nil'
 *				  when interpreted as references; consequently,
 *				  'Value's are link equivalent.
 *	Identity		- a collection in one-to-one correspondence to
 *				  another collection.  'Identity's are link
 *				  equivalent.
 *	Link			- an sorted collection of positions.  Links are
 *				  link equivalent (surprise!).
 *	Reference		- an ordered collection of positions.
 *				  References are NOT link equivalent.
 *---------------------------------------------------------------------------
 */


/********************
 *****  VALUES  *****
 ********************/
/*---------------------------------------------------------------------------
 * Values represent ordered non-positional collections.  Values are directly
 * represented by u-vectors.
 *---------------------------------------------------------------------------
 */

/*********************************
 *  Value CPD and R-Type Access  *
 *********************************/

/*---------------------------------------------------------------------------
 *****  Macro to return a standard CPD for a 'Value'.
 *
 *  Arguments:
 *	value			- the value to be converted into a container.
 *
 *  Syntactic Context:
 *	M_CPD *Expression	- a CPD for an instance of the least
 *				  general R-Type required to represent this
 *				  value.  This CPD must be freed when no
 *				  longer needed.
 *
 *  Notes:
 *	The semantics of descriptor values requires that this macro
 *	manufacture a new container to hold the value.  With a suitable
 *	change in P-Token, that container may be used to instantiate a new
 *	V-Store (provided the container returned is NOT a reference u-vector).
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of the 'Value' component of a descriptor.
 *
 *  Arguments:
 *	value			- the value whose R-Type is desired.
 *
 *  Syntactic Context:
 *	RType_Type Expression
 *
 *****/

/*************************
 *  Value P-Token Query  *
 *************************/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of a value.
 *
 *  Arguments:
 *	value			- the value whose positional P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'value'.  'pPTokenRef' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of a value.
 *
 *  Arguments:
 *	value			- the value whose referential P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'value'.  'pPTokenRef' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*********************************
 *  Value Initialization Macros  *
 *********************************/

/***** Macros to Initialize a Value from a Value and index *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Value from a link constructor and a source
 *****  Value.
 *
 *  Arguments:
 *	target	    		- the Value to be initialized.
 *	linkc			- a pointer to a link constructor which will be
 *                                used to extract from the source Value.
 *	source			- the Value to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Value from a reference uvector and a source
 *****  Value.
 *
 *  Arguments:
 *	target	    		- the Value to be initialized.
 *	cpd			- a standard CPD for a reference uvector which
 *                                will be used to extract from the source
 *                                Value.
 *	source			- the Value to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*******************************************
 *  Value Duplication and Clearing Macros  *
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Macro to duplicate a 'Value'
 *
 *  Arguments:
 *	target			- a location whose contents will be replaced
 *				  by the duplicated Value.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Value.
 *	source			- a location containing the Value to be
 *				  duplicated.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to clear a 'Value'
 *
 *  Arguments:
 *	value			- the Value to be cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/


/****************************
 *  Value Class Definition  *
 ****************************/

class DSC_Value {
//  Exception Generation
protected:
    enum ExceptionSource {
	ExceptionSource_ConstructComposition
    };

    void raiseTypeException (ExceptionSource xExceptionSource) const;

//  Construction
public:
    void construct (DSC_Value const &rSource) {
	m_pValues = rSource;
	m_pValues->retain ();
    }
    void construct (M_CPD *pValues) {
	m_pValues = pValues;
    }

    void constructComposition (rtLINK_CType *pSubscript, M_CPD *pSource);
    void constructComposition (M_CPD *pSubscript, M_CPD *pSource);

    void constructSet (M_ASD *pContainerSpace, M_CPD *&enumeration, M_CPD *source);

//  Destruction
public:
    void clear () {
	m_pValues->release ();
    }

//  Component Realization
public:
    M_CPD *realizedValues () const {
	return m_pValues;
    }

//  Access
public:
    unsigned int cardinality () const {
	return UV_CPD_ElementCount (m_pValues);
    }

    void getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	m_pValues->retain ();
	pPTokenRef = m_pValues;
	xPTokenRef = UV_CPx_PToken;
    }
    void getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	m_pValues->retain ();
	pPTokenRef = m_pValues;
	xPTokenRef = UV_CPx_RefPToken;
    }

    RTYPE_Type RType () const {
	return m_pValues->RType ();
    }

    operator M_CPD* () const {
	return m_pValues;
    }
    M_CPD *operator-> () const {
	return m_pValues;
    }

//  Query
public:
    bool isACoercedScalar () const;

//  Container Creation
public:
    M_CPD *asContainer (M_CPD *pPPT) const {
	return UV_BasicCopy (m_pValues, pPPT);
    }

//  Modification
public:
    void distribute (M_CPD *pDistribution);

//  State
protected:
    M_CPD *m_pValues;
};


/************************
 *****  IDENTITIES  *****
 ************************/

/*---------------------------------------------------------------------------
 * Identities represent a positional collection in one-to-one correspondence
 * with another collection.  Identities are represented by a p-token.
 *
 *  Identity Field Descriptions:
 *	m_pPToken			- a CPD for the P-Token denoting this identity.
 *---------------------------------------------------------------------------
 */

/**********************************************
 *  Identity: PToken, CPD, and R-Type Access  *
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Macro to return a standard CPD for a 'Identity'.
 *
 *  Arguments:
 *	identity		- the identity to be converted into a
 *				  container.
 *
 *  Syntactic Context:
 *	M_CPD *Expression	- a CPD for an instance of the least
 *				  general R-Type required to represent this
 *				  value.  This CPD must be freed when no
 *				  longer needed.
 *
 *  Notes:
 *	The semantics of descriptor identities requires that this macro
 *	manufacture a new container to hold the identity.  With a suitable
 *	change in P-Token, that container may be used to instantiate a new
 *	V-Store (provided the container returned is NOT a reference u-vector).
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of the 'Identity' component of a descriptor.
 *
 *  Arguments:
 *	identity		- the identity whose R-Type is desired.
 *
 *  Syntactic Context:
 *	RTYPE_Type Expression
 *
 *****/

/****************************
 *  Identity P-Token Query  *
 ****************************/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of an identity.
 *
 *  Arguments:
 *	identity		- the identity whose positional P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'identity'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of an identity.
 *
 *  Arguments:
 *	identity		- the identity whose referential P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'identity'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/************************************
 *  Identity Initialization Macros  *
 ************************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize an 'Identity'
 *
 *  Arguments:
 *	identity		- the Pointer to be initialized.
 *	pToken			- a CPD for the P-Token denoting the identity.
 *	                          ASSIGNED directly into the identity.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a set Reference and an enumeration from a source
 *****  Identity.
 *
 *  Arguments:
 *	target	    		- the set Reference to be initialized.
 *	enumeration		- an 'lval' of type 'M_CPD*' which will
 *				  be set to a standard CPD for a reference
 *				  u-vector specifying the ordering factored
 *				  from 'source'.
 *	source			- the Identity to use as the source of the
 *				  set.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/**********************************************
 *  Identity Duplication and Clearing Macros  *
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Macro to duplicate an 'Identity'
 *
 *  Arguments:
 *	target			- a location whose contents will be replaced
 *				  by the duplicated Identity.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Identity.
 *	source			- a location containing the Identity to be
 *				  duplicated.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to clear an 'Identity'
 *
 *  Arguments:
 *	identity		- the Identity to be cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/


/*******************************
 *  Identity Class Definition  *
 *******************************/

class DSC_Identity {
//  Construction
public:
    void construct (DSC_Identity const &rSource) {
	m_pPToken = rSource.m_pPToken;
	m_pPToken->retain ();
    }

    void construct (M_CPD *pPToken) {
	m_pPToken = pPToken;
    }

//  Destruction
public:
    void clear () {
	m_pPToken->release ();
	m_pPToken = NilOf (M_CPD*);
    }

//  Access
public:
    unsigned int cardinality () const {
	return rtPTOKEN_CPD_BaseElementCount (m_pPToken);
    }

    void getPTokenReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	m_pPToken->retain ();
	pPTokenRef = m_pPToken;
	xPTokenRef = -1;
    }
    void getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	getPTokenReference (pPTokenRef, xPTokenRef);
    }
    void getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	getPTokenReference (pPTokenRef, xPTokenRef);
    }

    M_CPD *PToken () const {
	return m_pPToken;
    }

    static RTYPE_Type RType () {
	return RTYPE_C_Link;
    }

//  Query
public:
    bool isScalar () const {
	return m_pPToken->NamesTheScalarPToken ();
    }

//  Container Creation
public:
    M_CPD *asContainer (M_CPD *pPPT) const {
	return rtLINK_NewRefLink (pPPT, m_pPToken);
    }

    M_CPD *asUVector () const {
	M_CPD *pResult = rtREFUV_New (m_pPToken, m_pPToken);
	unsigned int *pResultArray = (unsigned int*)rtREFUV_CPD_Array (pResult);
	unsigned int cElements = UV_CPD_ElementCount (pResult);
	for (unsigned int xElement = 0; xElement < cElements; xElement++)
	    pResultArray[xElement] = xElement;
	return pResult;
    }

//  State
protected:
    M_CPD *m_pPToken;
};


/*******************
 *****  LINKS  *****
 *******************/

/*---------------------------------------------------------------------------
 * Link are sorted collections of positions.  Links are represented by either
 * a link constructor or a link or both.
 *
 *  Link Field Descriptions:
 *	m_pConstructor	    - the address of a link constructor or Nil.
 *	m_pContainer	    - the address of a link CPD or Nil.
 *
 *---------------------------------------------------------------------------
 */

/********************************
 *  Link CPD and R-Type Access  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Macro to return a standard CPD for a 'Link'.
 *
 *  Arguments:
 *	link			- the link to be converted into a
 *				  container.
 *
 *  Syntactic Context:
 *	M_CPD *Expression	- a CPD for an instance of the least
 *				  general R-Type required to represent this
 *				  value.  This CPD must be freed when no
 *				  longer needed.
 *
 *  Notes:
 *	The semantics of descriptor links requires that this macro
 *	manufacture a new container to hold the link.  With a suitable
 *	change in P-Token, that container may be used to instantiate a new
 *	V-Store (provided the container returned is NOT a reference u-vector).
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of the 'Link' component of a descriptor.
 *
 *  Arguments:
 *	link			- the link whose R-Type is desired.
 *
 *  Syntactic Context:
 *	RTYPE_Type Expression
 *
 *****/

/************************
 *  Link P-Token Query  *
 ************************/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of a link.
 *
 *  Arguments:
 *	link			- the link whose positional P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'link'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of a link.
 *
 *  Arguments:
 *	link			- the link whose referential P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'link'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*****************************
 *  Link Realization Macros  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Macro to make the 'linkc' of the link present.
 *
 *  Arguments:
 *	link			- the link whose 'linkc' is to be
 *				  made present.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to make the 'linkCPD' of the link present.
 *
 *  Arguments:
 *	link			- the link whose 'linkCPD' is to be
 *				  made present.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/********************************
 *  Link Initialization Macros  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a 'Link'.
 *
 *  Arguments:
 *	link			- the link descriptor to initialize.
 *	linkc			- an optional ('Nil' if absent) link
 *                                constructor which will be
 *				  ASSIGNED into the 'Link'.
 *	linkCPD			- an optional ('Nil' if absent) standard CPD
 *                                for the link which will
 *				  be ASSIGNED into the 'link'.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *	Either the 'linkc', the 'linkCPD', or both must not be 'Nil'.
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Link from a link constructor and a source
 *****  Link.
 *
 *  Arguments:
 *	target	    		- the Link to be initialized.
 *	linkc			- a pointer to a link constructor which will be
 *                                used to extract from the source Link.
 *	source			- the Link to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a set Reference and an enumeration from a source
 *****  Link.
 *
 *  Arguments:
 *	target	    		- the set Reference to be initialized.
 *	enumeration		- an 'lval' of type 'M_CPD*' which will
 *				  be set to a standard CPD for a reference
 *				  u-vector specifying the ordering factored
 *				  from 'source'.
 *	source			- the Link to use as the source of the
 *				  set.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/******************************************
 *  Link Duplication and Clearing Macros  *
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Macro to duplicate a 'Link'
 *
 *  Arguments:
 *	target			- a location whose contents will be replaced
 *				  by the duplicated Link.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Link.
 *	source			- a location containing the Link to be
 *				  duplicated.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to clear a 'Link'
 *
 *  Arguments:
 *	link			- the Link to be cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/


/***************************
 *  Link Class Definition  *
 ***************************/

class DSC_Link {
//  Construction
public:
    void construct (DSC_Link const &rSource) {
	if (m_pConstructor = rSource.m_pConstructor)
	    m_pConstructor->retain ();
	if (m_pContainer = rSource.m_pContainer)
	    m_pContainer->retain ();
    }

    void construct (M_CPD *pContainer) {
	m_pConstructor = NilOf (rtLINK_CType*);
	m_pContainer = pContainer;
    }
    void construct (rtLINK_CType *pConstructor) {
	m_pConstructor = pConstructor;
	m_pContainer = NilOf (M_CPD*);
    }

    void constructComposition (rtLINK_CType *pSubscript, DSC_Link const &rSource) {
	m_pConstructor = rSource.m_pConstructor
	    ? rSource.m_pConstructor->Extract (pSubscript)
	    : rtLINK_LCExtract (rSource.m_pContainer, pSubscript);
	m_pContainer = NilOf (M_CPD*);
    }

//  Destruction
public:
    void clear () {
	if (m_pConstructor) {
	    m_pConstructor->release ();
	    m_pConstructor = NilOf (rtLINK_CType*);
	}
	if (m_pContainer) {
	    m_pContainer->release ();
	    m_pContainer = NilOf (M_CPD*);
	}
    }

//  Component Realization
public:
    rtLINK_CType *realizedConstructor () {
	if (!m_pConstructor)
	    m_pConstructor = rtLINK_ToConstructor (rtLINK_AlignLink (m_pContainer));
	return m_pConstructor;
    }
    M_CPD *realizedContainer () {
	if (!m_pContainer)
	    m_pContainer = m_pConstructor->Align ()->ToLink (false);
	return m_pContainer;
    }

//  Access
public:
    unsigned int cardinality () const {
	return m_pConstructor ? m_pConstructor->ElementCount () : rtLINK_CPD_ElementCount (
	    m_pContainer
	);
    }

    void getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	if (m_pConstructor) {
	    pPTokenRef = m_pConstructor->PPT ();
	    xPTokenRef = -1;
	}
	else {
	    pPTokenRef = m_pContainer;
	    xPTokenRef = rtLINK_CPx_PosPToken;
	}
	pPTokenRef->retain ();
    }
    void getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	if (m_pConstructor) {
	    pPTokenRef	= m_pConstructor->RPT ();
	    xPTokenRef	= -1;
	}
	else {
	    pPTokenRef	= m_pContainer;
	    xPTokenRef	= rtLINK_CPx_RefPToken;
	}
	pPTokenRef->retain ();
    }

    static RTYPE_Type RType () {
	return RTYPE_C_Link;
    }

    operator rtLINK_CType* () {
	return realizedConstructor ();
    }
    rtLINK_CType *operator-> () {
	return realizedConstructor ();
    }

//  Container Creation
public:
    M_CPD *asContainer (M_CPD *pPPT) const {
	return m_pConstructor
	    ? m_pConstructor->Align ()->ToLink (pPPT, false)
	    : rtLINK_Copy (m_pContainer, pPPT);
    }

    M_CPD *asUVector () const {
	return m_pConstructor ? m_pConstructor->ToRefUV () : rtLINK_LinkToRefUV (
	    m_pContainer
	);
    }

//  Use
public:
    rtLINK_CType *Extract (rtLINK_CType *pSubscript) const {
	return m_pConstructor ? m_pConstructor->Extract (pSubscript) : rtLINK_LCExtract (
	    m_pContainer, pSubscript
	);
    }
    M_CPD *Extract (M_CPD *pSubscript) const {
	return m_pConstructor ? m_pConstructor->Extract (pSubscript) : rtLINK_UVExtract (
	    m_pContainer, pSubscript
	);
    }

//  State
protected:
    rtLINK_CType	*m_pConstructor;
    M_CPD		*m_pContainer;
};


/************************
 *****  REFERENCES  *****
 ************************/

/*---------------------------------------------------------------------------
 * References are explicitly ordered collections of positions.  References have
 * multiple, concurrently saved representations to afford different algorithms
 * the different perspectives on references they require.  All components of a
 * reference are optional; however, not all components can be missing.
 *
 *  Reference Field Descriptions:
 *	m_pValues		- a standard reference U-Vector CPD enumerating
 *				  the positions referenced by this reference.
 *	m_pReordering		- a standard reference U-Vector CPD specifying
 *				  how the elements selected by 'm_pSubset' should
 *				  be reordered.  In the absence of 'm_pSubset',
 *				  'm_pReordering' and 'positions' are
 *				  synonomous.
 *	m_pRedistribution	- a standard reference U-Vector CPD specifying
 *				  how the elements selected by 'm_pSubset' should
 *				  be redistributed.
 *	m_pSubset			- a link constructor enumerating in optimal
 *				  (i.e., positional) order the positions
 *				  referenced by this reference.
 *---------------------------------------------------------------------------
 */

/*************************************
 *  Reference CPD and R-Type Access  *
 *************************************/

/*---------------------------------------------------------------------------
 *****  Macro to return a standard CPD for a 'Reference'.
 *
 *  Arguments:
 *	reference		- the reference to be converted into a
 *				  container.
 *	cpd			- an 'lval' of type 'M_CPD*' which will
 *				  be set to a CPD for an instance of the least
 *				  general R-Type required to represent this
 *				  reference.  This CPD must be freed when no
 *				  longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *	The semantics of descriptor references requires that this macro
 *	manufacture a new container to hold the reference.  With a suitable
 *	change in P-Token, that container may be used to instantiate a new
 *	V-Store (provided the container returned is NOT a reference u-vector).
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of a 'Reference'.
 *
 *  Arguments:
 *	reference		- the reference whose R-Type is desired.
 *
 *  Syntactic Context:
 *	RType_Type Expression
 *
 *****/

/*****************************
 *  Reference P-Token Query  *
 *****************************/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of a reference.
 *
 *  Arguments:
 *	reference		- the reference whose positional P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'reference'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of a reference.
 *
 *  Arguments:
 *	reference		- the reference whose referential P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'reference'.  'pPTokenRef' must
 *				  be freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/**********************************
 *  Reference Realization Macros  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  Macro to make the 'values' uvector of the reference present.
 *
 *  Arguments:
 *	reference		- the reference whose 'values' are to be
 *				  made present.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to make the 'Redistribution' u-vector of a reference present.
 *
 *  Arguments:
 *	reference		- the reference whose 'm_pRedistribution'
 *				  is to be made present.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*************************************
 *  Reference Initialization Macros  *
 *************************************/

/*---------------------------------------------------------------------------
 *****  General macro to initialize a Reference.
 *
 *  Arguments:
 *	reference		- the reference to be initialized.
 *	values			- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pValues' reference u-vector of the
 *				  reference.  Will be ASSIGNED directly into
 *                                the reference.
 *	reordering		- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pReordering' component of a
 *				  Reference.  Will be ASSIGNED directly into
 *                                the reference.
 *	redistribution		- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pRedistribution' component of a
 *				  Reference.  Will be ASSIGNED directly into
 *                                the reference.
 *	subset			- an optional ('Nil' if absent) link
 *				  constructor pointer for the 'm_pSubset'
 *				  component of the Reference.  Will be ASSIGNED
 *                                directly into the reference.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Reference from a link constructor and a source
 *****  Reference.
 *
 *  Arguments:
 *	target	    		- the Reference to be initialized.
 *	linkc			- a pointer to a link constructor which will be
 *                                used to extract from the source Reference.
 *	source			- the Reference to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Reference from a reference uvector and a source
 *****  Reference.
 *
 *  Arguments:
 *	target	    		- the Reference to be initialized.
 *	cpd			- a standard CPD for a reference uvector
 *                                which will be used to extract from the
 *                                source Reference.
 *	source			- the Reference to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Reference from a reference uvector and a source
 *****  Link.
 *
 *  Arguments:
 *	target	    		- the Reference to be initialized.
 *	cpd			- a standard CPD for a reference uvector which
 *                                will be used to extract from the source Link.
 *	source			- the Link to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a set Reference and an enumeration from a source
 *****  Reference.
 *
 *  Arguments:
 *	target	    		- the set Reference to be initialized.
 *	enumeration		- an 'lval' of type 'M_CPD*' which will
 *				  be set to a standard CPD for a reference
 *				  u-vector specifying the ordering factored
 *				  from 'source'.
 *	source			- the Reference to use as the source of the
 *				  set.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/***********************************************
 *  Reference Duplication and Clearing Macros  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Macro to duplicate a 'Reference'
 *
 *  Arguments:
 *	target			- a location whose contents will be replaced
 *				  by the duplicated Reference.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Reference.
 *	source			- a location containing the Reference to be
 *				  duplicated.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macros to clear parts of a 'Reference'
 *
 *  Argument:
 *	reference		- the reference whose component(s) are to be
 *				  cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to clear a 'Reference'
 *
 *  Arguments:
 *	reference		- the Reference to be cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*************************************************
 *  Reference Factoring and Distribution Macros  *
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Macro to factor a reference into its 'Subset' and 'Redistribution'
 *****  components.
 *
 *  Arguments:
 *	reference		- the reference to be factored.
 *	linkc			- an 'lval' of type 'rtLINK_CType*' which
 *				  will be set to the 'Subset' of the Reference.
 *	cpd			- an 'lval' of type 'M_CPD*' which will
 *				  be set to the 'Redistribution' of the
 *				  Reference.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to distribute a 'Reference'
 *
 *  Arguments:
 *	reference		- the Reference to be 'distributed'.
 *	cpd			- a standard CPD for a reference u-vector
 *				  specifying the 'distribution' to be
 *				  applied to Reference.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/


/********************************
 *  Reference Class Definition  *
 ********************************/

class DSC_Reference {
//  Exception Generation
protected:
    enum ExceptionSource {
	ExceptionSource_Distribute,
	ExceptionSource_Factor,
	ExceptionSource_GetPPTReference,
	ExceptionSource_GetRPTReference,
	ExceptionSource_RealizeValues
    };
    void raiseMissingComponentException (ExceptionSource xExceptionSource) const;

//  Construction/Initialization
public:
    void construct (DSC_Reference const &rSource) {
	*this = rSource;

	if (m_pValues)
	    m_pValues->retain ();
	if (m_pReordering)
	    m_pReordering->retain ();
	if (m_pRedistribution)
	    m_pRedistribution->retain ();
	if (m_pSubset)
	    m_pSubset->retain ();
    }

    void construct (M_CPD *pValues) {
	m_pValues = pValues;
	m_pReordering = m_pRedistribution = NilOf (M_CPD*);
	m_pSubset = NilOf (rtLINK_CType*);
    }
    void construct (M_CPD *pDistribution, rtLINK_CType *pSubset) {
	m_pValues = m_pReordering = NilOf (M_CPD*);
	m_pRedistribution = pDistribution;
	m_pSubset = pSubset;
    }

    void constructComposition (rtLINK_CType *pSubscript, DSC_Reference &rSource) {
	m_pValues = rtREFUV_LCExtract (rSource, pSubscript);
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;
    }
    void constructComposition (M_CPD *pSubscript, DSC_Reference &rSource) {
	m_pValues = rtREFUV_UVExtract (rSource, pSubscript);
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;
    }
    void constructComposition (M_CPD *pSubscript, DSC_Link const &rSource) {
	m_pValues = rSource.Extract (pSubscript);
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;
    }

    void constructSet (
	M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Identity const &rSource
    ) {
	m_pValues = 0;
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;

	M_CPD *refuv = rSource.asUVector ();
	rtREFUV_ToSetUV (pContainerSpace, refuv, &m_pValues, &rpEnumeration);
	refuv->release ();
    }
    void constructSet (
	M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Link const &rSource
    ) {
	m_pValues = 0;
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;

	M_CPD *refuv = rSource.asUVector ();
	rtREFUV_ToSetUV (pContainerSpace, refuv, &m_pValues, &rpEnumeration);
	refuv->release ();
    }
    void constructSet (
	M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Reference &rSource
    ) {
	m_pValues = 0;
	m_pReordering = 0;
	m_pRedistribution = 0;
	m_pSubset = 0;

	rtREFUV_ToSetUV (pContainerSpace, rSource, &m_pValues, &rpEnumeration);
    }

//  Destruction
protected:
    void clearValues () {
	if (m_pValues) {
	    m_pValues->release ();
	    m_pValues = NilOf (M_CPD*);
	}
    }

    void clearReordering () {
	if (m_pReordering) {
	    m_pReordering->release ();
	    m_pReordering = NilOf (M_CPD*);
	}
    }

    void clearRedistribution () {
	if (m_pRedistribution) {
	    m_pRedistribution->release ();
	    m_pRedistribution = NilOf (M_CPD*);
	}
    }

    void clearSubset () {
	if (m_pSubset) {
	    m_pSubset->release ();
	    m_pSubset = NilOf (rtLINK_CType*);
	}
    }

public:
    void clear () {
	clearValues		();
	clearReordering		();
	clearRedistribution	();
	clearSubset		();
    }

//  Component Realization
public:
    M_CPD *realizedValues () {
	if (m_pValues) {
	}
	else if (m_pRedistribution) {
    	    m_pValues = m_pSubset ? m_pSubset->ToRefUV (
		m_pRedistribution
	    ) : rtREFUV_Flip (m_pRedistribution);
	}
	else if (m_pReordering) {
	    m_pRedistribution = rtREFUV_Flip (m_pReordering);
	    m_pValues = m_pSubset->ToRefUV (m_pRedistribution);
	}
	else raiseMissingComponentException (ExceptionSource_RealizeValues);

	return m_pValues;
    }

//  Access
public:
    unsigned int cardinality () const {
	return UV_CPD_ElementCount (
	    m_pValues ? m_pValues : m_pRedistribution ? m_pRedistribution : m_pReordering
	);
    }

    void getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	if (m_pValues) {
	    m_pValues->retain ();
	    pPTokenRef = m_pValues;
	    xPTokenRef = UV_CPx_PToken;
	}
	else if (m_pRedistribution) {
	    m_pRedistribution->retain ();
	    pPTokenRef = m_pRedistribution;
	    xPTokenRef = UV_CPx_RefPToken;
	}
	else if (m_pReordering) {
	    m_pReordering->retain ();
	    pPTokenRef = m_pReordering;
	    xPTokenRef = UV_CPx_PToken;
	}
	else raiseMissingComponentException (ExceptionSource_GetPPTReference);
    }

    void getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
	if (m_pSubset) {
	    pPTokenRef = m_pSubset->RPT ();
	    pPTokenRef->retain ();
	    xPTokenRef = (-1);
	}
	else if (m_pValues) {
	    m_pValues->retain ();
	    pPTokenRef = m_pValues;
	    xPTokenRef = UV_CPx_RefPToken;
	}
	else if (m_pRedistribution) {
	    m_pRedistribution->retain ();
    	    pPTokenRef = m_pRedistribution;
	    xPTokenRef = UV_CPx_PToken;
	}
	else raiseMissingComponentException (ExceptionSource_GetRPTReference);
    }

    static RTYPE_Type RType () {
	return RTYPE_C_RefUV;
    }

    operator M_CPD * () {
	return realizedValues ();
    }
    M_CPD *operator-> () {
	return realizedValues ();
    }

//  Container Creation
public:
    M_CPD *asContainer (M_CPD *pPPT) {
	return UV_BasicCopy (realizedValues (), pPPT);
    }

//  Modification
public:
    M_CPD *factor (rtLINK_CType **ppLink) {
	if (m_pRedistribution) {
	}
	else if (m_pReordering)
	    m_pRedistribution = rtREFUV_Flip (m_pReordering);
	else if (m_pValues) {
	    clearSubset ();
	    m_pSubset = rtLINK_RefUVToConstructor (m_pValues, &m_pRedistribution);
	}
	else
	    raiseMissingComponentException (ExceptionSource_Factor);

	if (m_pSubset)
	    m_pSubset->retain ();
	*ppLink = m_pSubset;

	if (m_pRedistribution)
	    m_pRedistribution->retain ();

	return m_pRedistribution;
    }

    void distribute (M_CPD *pDistribution) {
	if (m_pRedistribution) {
	    M_CPD *newuv = rtREFUV_UVExtract (pDistribution, m_pRedistribution);
	    clearRedistribution ();
	    m_pRedistribution = newuv;

	    clearReordering ();
	    clearValues ();
	}
	else if (m_pReordering) {
	    M_CPD *newuv = rtREFUV_Distribute (m_pReordering, pDistribution);
	    clearRedistribution ();
	    m_pRedistribution = newuv;
	    clearReordering ();
	    clearValues ();
	}
	else if (m_pValues) {
	    M_CPD *newuv = rtREFUV_Distribute (pDistribution, m_pValues);
	    clear ();
	    m_pValues = newuv;
	}
	else raiseMissingComponentException (ExceptionSource_Distribute);
    }

//  State
protected:
    M_CPD		*m_pValues;
    M_CPD		*m_pReordering;
    M_CPD		*m_pRedistribution;
    rtLINK_CType	*m_pSubset;
};


/***********************************
 *****  Pointer Access Macros  *****
 ***********************************/

/*********************************
 *  Pointer Direct State Access  *
 *********************************/

#define DSC_Pointer_Type(pointer) ((pointer).m_xType)

#define DSC_Pointer_Content(pointer) ((pointer).m_iContent)

#define DSC_Pointer_Scalar(pointer) (DSC_Pointer_Content(pointer).as_iScalar)

#define DSC_Pointer_Scalar_Char(pointer)\
    DSC_Scalar_Char (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Double(pointer)\
    DSC_Scalar_Double (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Float(pointer)\
    DSC_Scalar_Float (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Int(pointer)\
    DSC_Scalar_Int (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Unsigned64(pointer)\
    DSC_Scalar_Unsigned64 (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Unsigned96(pointer)\
    DSC_Scalar_Unsigned96 (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Scalar_Unsigned128(pointer)\
    DSC_Scalar_Unsigned128 (DSC_Pointer_Scalar (pointer))

#define DSC_Pointer_Value(pointer) (DSC_Pointer_Content(pointer).as_iValue)

#define DSC_Pointer_Identity(pointer) (DSC_Pointer_Content(pointer).as_iIdentity)

#define DSC_Pointer_Link(pointer) (DSC_Pointer_Content(pointer).as_iLink)

#define DSC_Pointer_Reference(pointer) (DSC_Pointer_Content(pointer).as_iReference)


/****************************************************************
 *  Pointer Content Predicates and Derived State Access Macros  *
 ****************************************************************/

/***********************************
 *  Pointer CPD and R-Type Access  *
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Macro to return a standard CPD for a 'Pointer'.
 *
 *  Arguments:
 *	pointer			- the pointer to be converted into a container.
 *	cpd			- an 'lval' of type 'M_CPD*' which will
 *				  be set to a CPD for an instance of the least
 *				  general R-Type required to represent this
 *				  pointer.  This CPD must be freed when no
 *				  longer needed.
 *
 *  New Identity:
 *	M_CPD *DSC_Pointer::pointerCPD ()
 *
 *  Notes:
 *	The semantics of descriptor pointers requires that this macro return
 *	a container distinct from the one used to internally represent the
 *	pointer.  As a consequence, each application of this macro to a
 *	pointer will produce a new instance representing the descriptor.
 *	With a suitable change in P-Token, that container may be used to
 *	instantiate a new V-Store (provided the container returned is NOT
 *	a reference u-vector).
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of the 'Pointer' component of a descriptor.
 *
 *  Arguments:
 *	pointer			- the descriptor whose store CPD is desired.
 *	rtype			- an 'lval' of type 'RTYPE_Type' which will
 *				  be set to the least general R-Type required
 *				  to represent the pointer component of this
 *				  descriptor.
 *
 *  Syntactic Context:
 *	RTYPE_Type pointerRType () const
 *
 *****/

/***************************
 *  Pointer P-Token Query  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of a pointer.
 *
 *  Arguments:
 *	pointer			- the pointer whose positional P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'pointer'.  'pPTokenRef' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of a pointer.
 *
 *  Arguments:
 *	pointer			- the pointer whose referential P-Token is
 *				  desired.
 *	pPTokenRef/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'pointer'.  'pPTokenRef' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/******************************************
 *****  Pointer Management Utilities  *****
 ******************************************/

/***********************************
 *  Pointer Initialization Macros  *
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Macros to initialize various flavors of value scalar pointer.
 *
 *  Arguments:
 *	pointer			- the Pointer to initialize.
 *	pPPT			- the position P-Token of 'pointer'.
 *	pRPT			- a standard CPD for the reference P-Token of
 *				  the scalar.  Will be ASSIGNED directly into
 *                                the scalar pointer.
 *	value			- the value of the scalar.  This parameter is
 *				  omitted for undefined scalars.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer with a U-Vector element.
 *
 *  Arguments:
 *	pointer			- the pointer to initialize.
 *	uvector			- a standard CPD for the u-vector supplying
 *				  the element.
 *	subscript		- the subscript of the u-vector element to be
 *				  installed in this pointer.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a 'Value' pointer.
 *
 *  Arguments:
 *	pointer			- the Pointer to be initialized.
 *	cpd			- a standard CPD for the U-Vector to be
 *                                installed in the Value.  Will be ASSIGNED
 *                                directly into the value.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize an 'Identity' pointer.
 *
 *  Arguments:
 *	pointer			- the Pointer to be initialized.
 *	pToken			- a CPD for the P-Token denoting the identity.
 *	                          ASSIGNED directly into the identity.
 *
 *  Syntactic Context:
 *	See DSC_Pointer::constructIdentity (M_CPD *pPToken)
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a 'Link' Pointer.
 *
 *  Arguments:
 *	pointer			- the pointer to initialize.
 *	linkc			- an optional ('Nil' if absent) link
 *                                constructor which will be
 *				  ASSIGNED into the 'Link'.
 *	linkCPD			- an optional ('Nil' if absent) standard CPD
 *                                for the link which will
 *				  be ASSIGNED into the 'link'.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *	Either the 'linkc', the 'linkCPD', or both must not be 'Nil'.
 *
 *****/

/*---------------------------------------------------------------------------
 *****  General macro to initialize a Reference Pointer.
 *
 *  Arguments:
 *	pointer			- the Pointer to be initialized.
 *	values			- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pValues' reference u-vector of the
 *				  reference.  Will be ASSIGNED directly into
 *                                the reference pointer.
 *	reordering		- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pReordering' component of a
 *				  Reference.  Will be ASSIGNED directly into
 *                                the reference pointer.
 *	redistribution		- an optional ('Nil' if absent) standard CPD
 *				  for the 'm_pRedistribution' component of a
 *				  Reference.  Will be ASSIGNED directly into
 *                                the reference pointer.
 *	subset			- an optional ('Nil' if absent) link
 *				  constructor pointer for the 'm_pSubset'
 *				  component of the Reference.  Will be ASSIGNED
 *                                directly into the reference pointer.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/

/******************************************************
 *  Macro to Initialize a Pointer from a Pointer CPD  *
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer from a pointer CPD.
 *
 *  Arguments:
 *	pointer			- the pointer to be initialized.
 *	cpd			- a CPD for a pointer container (i.e., link
 *				  or u-vector) which will be used to initialize
 *				  the descriptor.  Unlike other macros which
 *				  always ASSIGN their CPD arguments into a
 *				  descriptor, this macro reserves the right
 *				  NOT to assign this CPD into the descriptor
 *				  it initializes.  As a consequence, this
 *				  macro will duplicate this CPD pointer
 *				  itself if it needs to keep a CPD for this
 *				  container.
 *
 *  New Identity:
 *	DSC_Pointer::construct (M_CPD *pMonotype)
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer from a link constructor and a source
 *****  pointer.
 *
 *  Arguments:
 *	target	    		- the pointer to be initialized.
 *	linkc			- a pointer to a link constructor which will be
 *                                used to extract from the source pointer.
 *	source			- the pointer to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	See DSC_Pointer::constructComposition (rtLINK_CType*, DSC_Pointer const&)
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer from a reference uvector and a source
 *****  pointer.
 *
 *  Arguments:
 *	target	    		- the pointer to be initialized.
 *	cpd			- a pointer to a reference uvector which will
 *                                be used to extract from the source pointer.
 *	source			- the pointer to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	See DSC_Pointer::constructComposition (M_CPD*, DSC_Descriptor const&)
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer from a reference and a source
 *****  pointer.
 *
 *  Arguments:
 *	target	    		- the pointer to be initialized.
 *	ref			- a pointer to a object of type:
 *                                'rtREFUV_Type_Reference' which will be
 *                                used to extract from the source pointer.
 *	source			- the pointer to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	See DSC_Pointer::constructComposition (DSC_Scalar&, DSC_Descriptor const&)
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a pointer from a 'Pointer' and a source
 *****  pointer.
 *
 *  Arguments:
 *	target	    		- the pointer to be initialized.
 *	pointer 		- a Pointer which will be used to extract
 *                                from the source pointer.
 *	source			- the pointer to use as the source of the
 *				  extract.
 *
 *  Syntactic Context:
 *	See DSC_Pointer::constructComposition (DSC_Pointer&, DSC_Descriptor const&)
 *
 ****/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a set pointer and an enumeration from a source
 *****  pointer.
 *
 *  Arguments:
 *	target	    		- the set pointer to be initialized.
 *	enumeration		- an 'lval' of type 'M_CPD*' which will
 *				  be set to a standard CPD for a reference
 *				  u-vector specifying the ordering factored
 *				  from 'source'.
 *	source			- the pointer to use as the source of the
 *				  set.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/

/*********************************************
 *  Pointer Duplication And Clearing Macros  *
 *********************************************/
/*-----------------------------------------------------------------------
 *****  Macro to duplicate a 'Pointer'
 *
 *  Arguments:
 *	target			- a location whose contents will be replace
 *				  by the duplicated Pointer.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Pointer.
 *	source			- a location containing the Pointer to be
 *				  duplicated.
 *
 *  New Identity:
 *	DSC_Pointer::construct (DSC_Pointer const &rSource)
 *
 *****/

/************************************************************
 *  Pointer Factoring, Distribution, and Reordering Macros  *
 ************************************************************/

/*---------------------------------------------------------------------------
 *****  Macro to factor a Pointer.
 *
 *  Arguments:
 *	pointer			- the pointer to factor.
 *	cpd			- an 'lval' of type 'M_CPD*' which
 *				  be set to either 'Nil' if no 'distribution'
 *				  can be factored from this pointer (i.e., the
 *				  Pointer is not a Reference Pointer) or a
 *				  standard CPD for a reference u-vector which
 *				  contains the distribution factored from a
 *				  'Reference' Pointer.
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to distribute a 'Pointer'
 *
 *  Arguments:
 *	pointer			- the Pointer to be 'distributed'.
 *	cpd			- a standard CPD for a reference u-vector
 *				  specifying the 'distribution' to be
 *				  applied to Pointer.
 *
 *  New Identity:
 *	void DSC_Pointer::distribute (M_CPD *distribution)
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to reorder a 'Pointer'
 *
 *  Arguments:
 *	pointer			- the Pointer to be 'reordered'.
 *	reordering		- a standard CPD for a reference u-vector
 *				  specifying the 'reordering' to be
 *				  applied to Pointer.
 *
 *  New Identity:
 *	void DSC_Pointer::reorder (M_CPD *reordering)
 *
 *****/

/*************************
 *  U-Vector Assignment  *
 *************************/

/***** Macro to Assign values to a U-Vector given a link constructor and a
 ***** Pointer.
 *
 *  Arguments:
 *	uvector		   - the address of a standard CPD for the U-Vector
 *			      to be modified.
 *	linkc		    - an address of a link constructor specifying the
 *			      elements in the U-Vector to be modified.
 *	pointer		    - the Pointer supplying the source values for
 *                            the assign.
 *
 *****/

/**************************************
 *  Macro To Coerce a Scalar Pointer  *
 **************************************/

/*---------------------------------------------------------------------------
 *****  Macro to coerce a Scalar Pointer.
 *
 *  Arguments:
 *	pointer			- the pointer to be coerced.  MUST be a
 *                                scalar pointer.
 *	pPPT			- the new positional ptoken (Not Assigned).
 *
 *  New Identity:
 *	void DSC_Pointer::coerce (M_CPD *pPPT)
 *
 *****/

/*---------------------------------------------------------------------------
 *****  Macro to determine if the pointer is a coerced Scalar Pointer.
 *
 *  Arguments:
 *	pointer			- the pointer to be checked.
 *	result			- an integer to be set to either true or false.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/


/**********************
 *  Class Definition  *
 **********************/

class DSC_Pointer {
//  Construction
public:
    void construct () {
	m_xType = DSC_PointerType_Empty;
    }

    void construct (M_CPD *pMonotype);
    void construct (rtLINK_CType *pMonotype);
    void construct (DSC_Pointer const &rSource);

    void constructComposition (rtLINK_CType *pSubscript, DSC_Pointer &rSource);
    void constructComposition (M_CPD	    *pSubscript, DSC_Pointer &rSource);
    void constructComposition (DSC_Scalar   &rSubscript, DSC_Pointer &rSource);
    void constructComposition (DSC_Pointer  &rSubscript, DSC_Pointer &rSource);

    void constructSet (
	M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Pointer &rSource
    );

    void constructCorrespondence (
	M_CPD *pPPT, M_CPD *pRPTReference, unsigned int xRPTReference
    );

    void constructIdentity (M_CPD *pPToken) {
	m_iContent.as_iIdentity.construct (pPToken);
	m_xType = DSC_PointerType_Identity;
    }

    void constructLink (M_CPD *pLink) {
	m_iContent.as_iLink.construct (pLink);
	m_xType = DSC_PointerType_Link;
    }
    void constructLink (rtLINK_CType *pLink) {
	m_iContent.as_iLink.construct (pLink);
	m_xType = DSC_PointerType_Link;
    }

    void constructReference (M_CPD *pValues) {
	m_iContent.as_iReference.construct (pValues);
	m_xType = DSC_PointerType_Reference;
    }
    void constructReference (M_CPD *pDistribution, rtLINK_CType *pSubset) {
	m_iContent.as_iReference.construct (pDistribution, pSubset);
	m_xType = DSC_PointerType_Reference;
    }

    void constructValue (M_CPD *pValue) {
	m_iContent.as_iValue.construct (pValue);
	m_xType = DSC_PointerType_Value;
    }

    void constructScalarComposition (unsigned int xSubscript, M_CPD *pSource) {
        m_iContent.as_iScalar.constructComposition (xSubscript, pSource);
	m_xType = DSC_PointerType_Scalar;
    }

    void constructScalar (DSC_Scalar const &rValue) {
	m_iContent.as_iScalar = rValue;
	m_xType = DSC_PointerType_Scalar;
    }
    void constructConstant (M_CPD *pPPT, DSC_Scalar const &rValue) {
	constructScalar (rValue);
	coerce (pPPT);
    }

    void constructScalar (M_CPD *pRPT) {
	DSC_InitUndefinedScalar (m_iContent.as_iScalar, pRPT);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructScalar (M_KOTE const &rKOTE) {
	constructScalar (rKOTE.RetainedPTokenCPD ());
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pRPT) {
	constructScalar (pRPT);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE) {
	constructScalar (rKOTE);
	coerce (pPPT);
    }

    void constructScalar (M_CPD *pRPT, char iValue) {
	DSC_InitCharScalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructScalar (M_KOTE const &rKOTE, char iValue) {
	constructScalar (rKOTE.RetainedPTokenCPD (), iValue);
    }
    void constructScalar (M_KOT *pKOT, char iValue) {
	constructScalar (pKOT->TheCharacterClass, iValue);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, char iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, char iValue) {
	constructScalar (rKOTE, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, char iValue) {
	constructConstant (pPPT, pKOT->TheCharacterClass, iValue);
    }

    void constructScalar (M_CPD *pRPT, double iValue) {
	DSC_InitDoubleScalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructScalar (M_KOTE const &rKOTE, double iValue) {
	constructScalar (rKOTE.RetainedPTokenCPD (), iValue);
    }
    void constructScalar (M_KOT *pKOT, double iValue) {
	constructScalar (pKOT->TheDoubleClass, iValue);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, double iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, double iValue) {
	constructScalar (rKOTE, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, double iValue) {
	constructConstant (pPPT, pKOT->TheDoubleClass, iValue);
    }

    void constructScalar (M_CPD *pRPT, float iValue) {
	DSC_InitFloatScalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructScalar (M_KOTE const &rKOTE, float iValue) {
	constructScalar (rKOTE.RetainedPTokenCPD (), iValue);
    }
    void constructScalar (M_KOT *pKOT, float iValue) {
	constructScalar (pKOT->TheFloatClass, iValue);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, float iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, float iValue) {
	constructScalar (rKOTE, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, float iValue) {
	constructConstant (pPPT, pKOT->TheFloatClass, iValue);
    }

    void constructScalar (M_CPD *pRPT, int iValue) {
	DSC_InitIntegerScalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructScalar (M_KOTE const &rKOTE, int iValue) {
	constructScalar (rKOTE.RetainedPTokenCPD (), iValue);
    }
    void constructScalar (M_KOT *pKOT, int iValue) {
	constructScalar (pKOT->TheIntegerClass, iValue);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, int iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, int iValue) {
	constructScalar (rKOTE, iValue);
	coerce (pPPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, int iValue) {
	constructConstant (pPPT, pKOT->TheIntegerClass, iValue);
    }

    void constructScalar (M_CPD *pRPT, VkUnsigned64 const &iValue) {
	DSC_InitUnsigned64Scalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, VkUnsigned64 const &iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }

    void constructScalar (M_CPD *pRPT, VkUnsigned96 const &iValue) {
	DSC_InitUnsigned96Scalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, VkUnsigned96 const &iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }

    void constructScalar (M_CPD *pRPT, VkUnsigned128 const &iValue) {
	DSC_InitUnsigned128Scalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructConstant (M_CPD *pPPT, M_CPD *pRPT, VkUnsigned128 const &iValue) {
	constructScalar (pRPT, iValue);
	coerce (pPPT);
    }

    void constructReferenceScalar (M_CPD *pRPT, unsigned int iValue) {
	DSC_InitReferenceScalar (m_iContent.as_iScalar, pRPT, iValue);
	m_xType = DSC_PointerType_Scalar;
    }
    void constructReferenceConstant (M_CPD *pPPT, M_CPD *pRPT, unsigned int iValue) {
	constructReferenceScalar (pRPT, iValue);
	coerce (pPPT);
    }

//  Access
public:
    void assignToUV (rtLINK_CType *pSubscript, M_CPD *pTarget);

    unsigned int cardinality () const {
	switch (m_xType) {
	case DSC_PointerType_Scalar:
	    return 1;
	case DSC_PointerType_Value:
	    return m_iContent.as_iValue.cardinality ();
	case DSC_PointerType_Identity:
	    return m_iContent.as_iIdentity.cardinality ();
	case DSC_PointerType_Link:
	    return m_iContent.as_iLink.cardinality ();
	case DSC_PointerType_Reference:
	    return m_iContent.as_iReference.cardinality ();
	default:
	    return 0;
	}
    }

    void getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const;
    void getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const;

    bool isACoercedScalar () const;

    RTYPE_Type pointerRType () const;

    M_CPD *PPT () const {
	M_CPD* pPTokenRef = 0; int xPTokenRef = 0;
	getPPTReference (pPTokenRef, xPTokenRef);

	M_CPD* pPTokenCPD;
	if (xPTokenRef < 0)
	    pPTokenCPD = pPTokenRef;
	else {
	    pPTokenCPD = pPTokenRef->GetCPD (xPTokenRef, RTYPE_C_PToken);
	    pPTokenRef->release ();
	}

	return pPTokenCPD;
    }

    M_CPD *RPT () const {
	M_CPD* pPTokenRef = 0; int xPTokenRef = 0;
	getRPTReference (pPTokenRef, xPTokenRef);

	M_CPD* pPTokenCPD;
	if (xPTokenRef < 0)
	    pPTokenCPD = pPTokenRef;
	else {
	    pPTokenCPD = pPTokenRef->GetCPD (xPTokenRef, RTYPE_C_PToken);
	    pPTokenRef->release ();
	}

	return pPTokenCPD;
    }

    DSC_PointerType Type () const {
	return m_xType;
    }

protected:
    void const *valueArrayOfType (RTYPE_Type const xExpectedType) const;

public:
    operator char const* () const {
	return (char const*)valueArrayOfType (RTYPE_C_CharUV);
    }
    operator float const* () const {
	return (float const*)valueArrayOfType (RTYPE_C_FloatUV);
    }
    operator double const* () const {
	return (double const*)valueArrayOfType (RTYPE_C_DoubleUV);
    }
    operator int const* () const {
	return (int const*)valueArrayOfType (RTYPE_C_IntUV);
    }
    operator unsigned int const* () const {
	return (unsigned int const*)valueArrayOfType (RTYPE_C_IntUV);
    }
    operator VkUnsigned64 const* () const {
	return (VkUnsigned64 const*)valueArrayOfType (RTYPE_C_Unsigned64UV);
    }
    operator VkUnsigned96 const* () const {
	return (VkUnsigned96 const*)valueArrayOfType (RTYPE_C_Unsigned96UV);
    }
    operator VkUnsigned128 const* () const {
	return (VkUnsigned128 const*)valueArrayOfType (RTYPE_C_Unsigned128UV);
    }

//  Query
public:
    bool isEmpty () const {
	return m_xType == DSC_PointerType_Empty;
    }
    bool isntEmpty () const {
	return m_xType != DSC_PointerType_Empty;
    }

    bool isScalar () const {
	return m_xType == DSC_PointerType_Scalar;
    }
    bool isntScalar () const {
	return m_xType != DSC_PointerType_Scalar;
    }

    bool holdsAScalarValue () const {
	return isScalar () && m_iContent.as_iScalar.holdsAValue ();
    }

    bool holdsAScalarReference () const {
	return isScalar () && m_iContent.as_iScalar.holdsAReference ();
    }

    bool holdsNonScalarValues () const {
	return m_xType == DSC_PointerType_Value;
    }

    bool holdsValuesInAnyForm () const {
	return holdsAScalarValue () || holdsNonScalarValues ();
    }

    bool holdsAnIdentity () const {
	return m_xType == DSC_PointerType_Identity;
    }

    bool holdsAScalarIdentity () const {
	return holdsAnIdentity () && m_iContent.as_iIdentity.isScalar ();
    }

    bool holdsALink () const {
	return m_xType == DSC_PointerType_Link;
    }

    bool holdsNonScalarReferences () const {
	return m_xType == DSC_PointerType_Reference;
    }

//  Update
public:
    void clear ();

    void coerce (M_CPD *pPPT);

    void convert (M_CPD *pEffectiveStoreCPD, M_CPD *pPPT);

    M_CPD *factor ();
    void distribute (M_CPD *distribution);
    void reorder    (M_CPD *reordering);

    void setTo (M_CPD *pMonotype) {
	clear ();
	construct (pMonotype);
    }
    void setTo (rtLINK_CType *pMonotype) {
	clear ();
	construct (pMonotype);
    }

//  Container Creation
public:
    M_CPD *pointerCPD (M_CPD *PPT);

    // Returns true if the u-vector must be freed by the caller
    bool getUVector (M_CPD *&rpUVector);

    M_CPD *asUVector () {
	M_CPD *pUVector;
	if (getUVector (pUVector))
	    return pUVector;

	pUVector->retain ();
	return pUVector;
    }

//  Exception Generation
public:
    void complainAboutBadPointerType (char const* pText) const;

//  State
public:
    DSC_PointerType		m_xType;
    union content_t {
        DSC_Scalar		as_iScalar;
	DSC_Value		as_iValue;
	DSC_Identity		as_iIdentity;
	DSC_Link		as_iLink;
	DSC_Reference		as_iReference;
    } m_iContent;
};


#endif

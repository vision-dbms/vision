/*****  VSymbol Implementation  *****/

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

#include "VSymbol.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSymbolBinding.h"
#include "VSymbolSpace.h"


/*********************
 *********************
 *****           *****
 *****  VSymbol  *****
 *****           *****
 *********************
 *********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSymbol);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSymbol::VSymbol (char const* pName) : m_pName (pName) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSymbol::~VSymbol () {
}


/********************************
 ********************************
 *****  Binding Management  *****
 ********************************
 ********************************/

/*------------------------------------------------------------------------------------*
  The binding management algorithms are based on two principles:
  
  1) The set of all types forms a partial order with a unique least element (i.e.,
     types are organized into a hierachy that inherits from a common root type).
  2) Operation implementations are organized into a tree of bindings associated with
     types in the hierarchy.  The root of the binding tree is attached to the greatest
     lower bound of all types that implement the operation.  That type need not be the
     root of the type tree and need not supply a definition for the operation.  The
     binding tree does not need to contain a node for every type node it spans; rather,
     it only needs nodes that correspond to types that introduce definitions or that
     correspond to greatest lower bounds for types with required nodes (i.e., nodes
     are required for definitions and branch points, but not for every type along a
     linear path).

  Each type in the hierachy is characterized by its distance to the root of the tree
  and a unique index relative to its immediate supertype.  A type's distance to the
  root is the level of the type.  The level of a binding is the level of the unique
  type associated with the binding.  Given two types and/or bindings, T1 and T2, if
  level(T1) > level(T2), then T1 is known to be more specialized than T2, but not
  necessarily a specialization of T2.

  Candidate binding can be nil.  Nil bindings have a level of infinity and are
  interpreted as definitions attached to types more specialized than any existing type.
 *------------------------------------------------------------------------------------*/


/******************************
 *****  Binding Creation  *****
 ******************************/

void VSymbol::createBinding (
    VRunTimeType *pType, VSymbolBindingHolder *&rpCandidateBindingHolder
) {
//  Obtain the level of the current candidate binding...
    unsigned short xTypeLevel = pType->level ();
    unsigned short xCandidateLevel = VSymbolBinding::levelOf (*rpCandidateBindingHolder);

//  If the candidate in hand is more general than this type, ...
    if (xCandidateLevel < xTypeLevel) {
    //  ... ask our supertype to process it:
	createBinding (pType->supertype (), rpCandidateBindingHolder);
	xCandidateLevel = VSymbolBinding::levelOf (*rpCandidateBindingHolder);

    //  If we get a candidate just above us, ...
	if (xTypeLevel /* > 0 */ - 1 == xCandidateLevel /* ...possibly infinite */) {
	// ... grab the subtype binding holder in which we belong:
	    rpCandidateBindingHolder = &(*rpCandidateBindingHolder)->subtypeBindingHolder (
		pType
	    );
	    xCandidateLevel = VSymbolBinding::levelOf (*rpCandidateBindingHolder);
	}
    }

/*------------------------------------------------------------------------------------*
  At this point, the candidate in hand is attached to a type at least as specialized
  as this type (i.e., xCandidateLevel >= xTypeLevel).  If that candidate is attached to
  a more specialized type than this one, delegate its handling to whatever subtype
  may have called us...
 *------------------------------------------------------------------------------------*/
//  If the candidate is more specialized than this type, let a more specialized type process it:
    if (xCandidateLevel > xTypeLevel)
	return;

//  At this point, the candidate is at our level.  Obtain its type, ...
    VRunTimeType *pCandidateType = (*rpCandidateBindingHolder)->type ();

//  If the candidate's type matches this type, return to our caller, ...
    if (pCandidateType == pType)
	return;

/*------------------------------------------------------------------------------------*
  We have just discovered that the binding tree doesn't contain a node for the GLB for
  this type and the candidate subtree, so add it:
 *------------------------------------------------------------------------------------*/
    VRunTimeType *pTypeGLBSubtype = pType;
    VRunTimeType *pBindGLBSubtype = pCandidateType;

    while (pTypeGLBSubtype->supertype () != pBindGLBSubtype->supertype ()) {
	pTypeGLBSubtype = pTypeGLBSubtype->supertype ();
	pBindGLBSubtype = pBindGLBSubtype->supertype ();
    }

    VSymbolBinding *pNewBinding = new VSymbolBinding (pTypeGLBSubtype->supertype ());
    pNewBinding->subtypeBindingHolder (pBindGLBSubtype).setTo (
	*rpCandidateBindingHolder
    );
    rpCandidateBindingHolder->setTo (pNewBinding);
    rpCandidateBindingHolder = &pNewBinding->subtypeBindingHolder (pTypeGLBSubtype);
}


/****************************
 *****  Binding Lookup  *****
 ****************************/

/*------------------------------------------------------------------------------------*
  This routine is initially invoked for the base type of the object in hand and a
  candidate binding equal to the root of a binding tree.  The general flow of the search
  is to initially follow the type graph toward the root of the tree to determine if the
  candidate binding applies to this type or one of its supertypes.  If the candidate
  doesn't apply, then there is no binding associated with this type.  If the candidate
  does apply, however, the path taken to the supertype, when retraced in reverse,
  defines a path from the candidate to the most specialized implementation of the
  operation applicable to this type.

  As always, there are three cases to consider:

  1) The current type is more specialized that the candidate binding in hand:
  
  In this case, we need to ask our supertype if it recognizes the candidate and, if it
  does, to set the candidate to the most specialized candidate that applies to us.  If
  it returns a candidate and that candidate is exactly one level more general than us,
  the candidate must apply to our immedidate supertype.  If that is the case, we further
  adjust the candidate to select the sub-binding of that candidate that corresponds to
  us.  This is where the algorithm does its inheritance directed backtracking through
  the binding tree.  As this point, the candidate is guaranteed to be at least as
  specialized as this type (i.e., xCandidateLevel >= xTypeLevel).

  2) The candidate binding is more specialized than the current type:

  In this case, the candidate doesn't apply to us, although it may apply to one of our
  subtypes.  That determination needs to made by whatever subtypes may have been
  traversed to get here.

  3) The candidate binding is at our level:

  This case handles two paths through this algorithm.  If this type doesn't
  match the type of the associated binding, there is no better definition in the
  candidate binding tree.  If the types do match and this binding actually has a
  value, it is the best value we've found so far, so return it as such.
 *------------------------------------------------------------------------------------*/
VSymbolBinding *VSymbol::locateBinding (
    VRunTimeType const *pType, VSymbolBinding *&rpCandidateBinding
) const {
    VSymbolBinding *pActualBinding = 0;

//  Obtain the level of the current candidate binding...
    unsigned short xTypeLevel = pType->level ();
    unsigned short xCandidateLevel = VSymbolBinding::levelOf (rpCandidateBinding);

//  If the candidate in hand is more general than this type, ...
    if (xCandidateLevel < xTypeLevel) {
    //  ... ask our supertype to process it:
	pActualBinding = locateBinding (pType->supertype (), rpCandidateBinding);
	xCandidateLevel = VSymbolBinding::levelOf (rpCandidateBinding);

    //  If we get a candidate just above us, ...
	if (xTypeLevel /* > 0 */ - 1 == xCandidateLevel /* ...possibly infinite */) {
	// ... grab the specialized binding that may apply to us:
	    rpCandidateBinding = rpCandidateBinding->subtypeBinding (pType);
	    xCandidateLevel = VSymbolBinding::levelOf (rpCandidateBinding);
	}
    }

/*------------------------------------------------------------------------------------*
  At this point, the candidate in hand is attached to a type at least as specialized
  as this type (i.e., xCandidateLevel >= xTypeLevel).  If that candidate is attached to
  a more specialized type than this one, delegate its handling to whatever subtype
  may have called us.  If the candidate in hand is at our level, but not our type,
  drop it from further consideration; otherwise, if the candidate in hand actually
  supplies a definition, it is the most applicable definition to date, so return it
  as the actual definition.
 *------------------------------------------------------------------------------------*/
    if (xCandidateLevel > xTypeLevel) {
    }
    else if (rpCandidateBinding->type () != pType)
	rpCandidateBinding = 0;
    else if (rpCandidateBinding->isBound ())
	pActualBinding = rpCandidateBinding;

    return pActualBinding;
}


/********************************************
 ********************************************
 *****  Symbol Value Update And Access  *****
 ********************************************
 ********************************************/

/********************
 *****  Update  *****
 ********************/

void VSymbol::set (VRunTimeType *pType, VSymbolImplementation const *pValue) {
    unsigned int xTypeLevel = pType->level ();

    VSymbolBindingHolder *pCandidateBindingHolder = &m_pRootBinding;
    createBinding (pType, pCandidateBindingHolder);
    VSymbolBinding *pCandidateBinding = *pCandidateBindingHolder;
    if (VSymbolBinding::levelOf (pCandidateBinding) > xTypeLevel) {
	//  A new binding will be needed, so create one, ...
	VSymbolBinding *pNewBinding = new VSymbolBinding (pType);

	//  ... and insert it at the right place in the tree, ...
	if (IsNil (pCandidateBinding))
	    pCandidateBindingHolder->setTo (pNewBinding);
	else {
	    VRunTimeType *pBindGLBSubtype = pCandidateBinding->type ();
	    while (pBindGLBSubtype->supertype ()->level () > xTypeLevel)
		pBindGLBSubtype = pBindGLBSubtype->supertype ();

	    if (pBindGLBSubtype->supertype () != pType) {
	    /*--------------------------------------------------------------------------*
	      We have just discovered that the current binding tree doesn't contain the
	      GLB of this type and the current binding tree root, so add it:
	     *--------------------------------------------------------------------------*/
		VRunTimeType *pTypeGLBSubtype = pType;
		pBindGLBSubtype = pBindGLBSubtype->supertype ();
		while (pTypeGLBSubtype->supertype () != pBindGLBSubtype->supertype ()) {
		    pTypeGLBSubtype = pTypeGLBSubtype->supertype ();
		    pBindGLBSubtype = pBindGLBSubtype->supertype ();
		}

		VSymbolBinding *pNewGLB = new VSymbolBinding (
		    pTypeGLBSubtype->supertype ()
		);
		pNewGLB->subtypeBindingHolder (pBindGLBSubtype).setTo (pCandidateBinding);
		pNewGLB->subtypeBindingHolder (pTypeGLBSubtype).setTo (pNewBinding);
		pCandidateBindingHolder->setTo (pNewGLB);
	    }
	    else {
	    /*--------------------------------------------------------------------------*
	      Otherwise, the new binding is the GLB of this type and the current
	      binding tree:
	     *--------------------------------------------------------------------------*/
		pNewBinding->subtypeBindingHolder (pBindGLBSubtype).setTo (
		    pCandidateBinding
		);
		pCandidateBindingHolder->setTo (pNewBinding);
	    }
	}
	pCandidateBinding = pNewBinding;
    }
    pCandidateBinding->setImplementationTo (pValue);
}

/********************
 *****  Access  *****
 ********************/

VSymbolImplementation const *VSymbol::implementationAt (VRunTimeType const *pType) const {
    VSymbolBinding *pCandidateBinding = m_pRootBinding;
    VSymbolBinding *pActualBinding = locateBinding (pType, pCandidateBinding);
    return pActualBinding ? pActualBinding->implementation () : 0;
}


/*****************************
 *****************************
 *****  Binding Display  *****
 *****************************
 *****************************/

void VSymbol::DisplayBindings () const {
    printf ("Symbol: %s\n", m_pName);
    rootBinding ()->Display (true, 1);
    printf ("================\n");
}


/**********************************
 **********************************
 *****                        *****
 *****  VSymbolBindingHolder  *****
 *****                        *****
 **********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSymbolBindingHolder::VSymbolBindingHolder (ThisClass const &rOther) : BaseClass (rOther) {
}

VSymbolBindingHolder::VSymbolBindingHolder (VSymbolBinding *pOther) : BaseClass (pOther) {
}

VSymbolBindingHolder::VSymbolBindingHolder () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSymbolBindingHolder::~VSymbolBindingHolder () {
}

/*
 *****  Update  *****
 */

VSymbolBindingHolder &VSymbolBindingHolder::operator= (ThisClass const &rOther) {
    setTo (rOther);
    return *this;
}

VSymbolBindingHolder &VSymbolBindingHolder::operator= (VSymbolBinding *pOther) {
    setTo (pOther);
    return *this;
}

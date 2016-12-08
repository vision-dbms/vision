/*****  VDescriptor Implementation  *****/

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

#include "VDescriptor.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verr.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTdictionary.h"


/************************************************
 ************************************************
 *****  Counters, Parameters, and Switches  *****
 ************************************************
 ************************************************/

/**********************
 *****  Counters  *****
 **********************/

unsigned int VDescriptor::HomogenizeVCCount		= 0;
unsigned int VDescriptor::HomogenizeVectorCount		= 0;

/************************
 *****  Parameters  *****
 ************************/

bool VDescriptor::DefragmentationAttemptEnabled	= true;

/**********************
 *****  Switches  *****
 **********************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VDescriptor::VDescriptor () {
    construct ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VDescriptor::~VDescriptor () {
    clear ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*---------------------------------------------------------------------------*
 *  Arguments:
 *	pElementSelector	- a link constructor specifying the set of
 *				  positions to be modified.
 *	pTarget			- the store being modified.
 *---------------------------------------------------------------------------*/
void VDescriptor::assignTo (rtLINK_CType *pElementSelector, Vdd::Store *pTarget) {
    normalize ();

    switch (m_xType) {
    case Type_Std:
	if (!pTarget->setElements (pElementSelector, contentAsMonotype ()))
	    raiseAssignmentTargetException (pTarget);
	break;

    case Type_Vector:
	convertToVectorC ();
	/***** No break here *****/

    case Type_VectorC:
	if (!pTarget->setElements (pElementSelector, contentAsVectorC ()))
	    raiseAssignmentTargetException (pTarget);
	break;

    case Type_Fragmentation:
	contentAsFragmentation ().assignTo (pElementSelector, pTarget);
	break;

    default:
	raiseTypeException ("assignTo [rtLINK_CType*]");
	break;
    }
}


/*---------------------------------------------------------------------------*
 *  Arguments:
 *	pElementSelector	- the address of a reference scalar specifying
 *				  the position to be modified.
 *	pTarget			- the store being modified.
 *---------------------------------------------------------------------------*/
void VDescriptor::assignTo (rtREFUV_Type_Reference &rElementSelector, Vdd::Store *pTarget) {
    normalize ();

    if (isStandard ()) {
	if (!pTarget->setElements (rElementSelector, contentAsMonotype ()))
	    raiseAssignmentTargetException (pTarget);
    }
    else if (!pTarget->setElements (rElementSelector, convertToVectorC ())) {
	raiseAssignmentTargetException (pTarget);
    }
}


/*---------------------------------------------------------------------------*
 *  Arguments:
 *	rPointer		- the address of a link equivalent descriptor
 *				  pointer specifying the positions to be
 *				  modified.
 *	pTarget			- the store being modified.
 *---------------------------------------------------------------------------*/
void VDescriptor::assignTo (DSC_Pointer &rPointer, Vdd::Store *pTarget) {
    if (rPointer.holdsAScalarReference ())
        assignTo (DSC_Pointer_Scalar (rPointer), pTarget);
    else if (rPointer.holdsALink ()) {
	assignTo (DSC_Pointer_Link (rPointer), pTarget);
    }
    else if (rPointer.holdsAnIdentity ()) {
	rtPTOKEN_Handle::Reference pTargetPToken (pTarget->getPToken ());
        rtLINK_CType *pIdentity = rtLINK_AppendRange (
	    rtLINK_RefConstructor (pTargetPToken), 0, pTargetPToken->cardinality ()
	)->Close (pTargetPToken);
	assignTo (pIdentity, pTarget);
	pIdentity->release ();
    }
    else if (rPointer.holdsNonScalarReferences ()) {
	DSC_Pointer pointer;
	pointer.construct (rPointer);

	M_CPD *pDistribution = pointer.factor ();
	if (IsNil (pDistribution))
	    assignTo (pointer, pTarget);
	else {
	    VDescriptor xdsc;
	    xdsc.setToCopied (*this);

	    xdsc.reorder (pDistribution);
	    pDistribution->release ();

	    xdsc.assignTo (pointer, pTarget);
	}

	pointer.clear ();
    }
}


/******************
 *  Store Subset  *
 ******************/

rtLINK_CType *VDescriptor::subsetInStore (Vdd::Store *pStore, VDescriptor *pValueReturn) {
    normalize ();

    rtLINK_CType *pSubset = NilOf (rtLINK_CType*);
    switch (m_xType) {
    case Type_Fragmentation:
	pSubset = contentAsFragmentation ().subsetInStore (pStore, pValueReturn);
	break;

    case Type_Vector:
	pSubset = contentAsVector ()->subsetInStore (pStore, pValueReturn);
	break;

    case Type_VectorC:
	pSubset = contentAsVectorC ()->subsetInStore (pStore, pValueReturn);
	break;

    case Type_Std:
	if (contentAsMonotype().store ()->Names (pStore)) {
	    if (pValueReturn)
		pValueReturn->setToCopied (*this);
	
	    rtPTOKEN_Handle::Reference pPPT (contentAsMonotype ().PPT ());
	    pSubset = rtLINK_RefConstructor (pPPT)->Append (
		0, pPPT->cardinality (), false
	    )->Close (pPPT);
	}
	break;

    default:
	raiseTypeException ("VDescriptor::subsetInStore");
	pSubset = NilOf (rtLINK_CType*);
	break;
    }

    return pSubset;
}


/*****************
 *  Type Subset  *
 *****************/

rtLINK_CType *VDescriptor::subsetOfType (
    M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *pValueReturn
) {
    normalize ();

    rtLINK_CType *pSubset = NilOf (rtLINK_CType *);
    switch (m_xType) {
    case Type_Fragmentation:
	pSubset = contentAsFragmentation ().subsetOfType (
	    pSubsetSpace, pKOTM, pValueReturn
	);
	break;

    case Type_Vector:
	pSubset = contentAsVector ()->subsetOfType (
	    pSubsetSpace, pKOTM, pValueReturn
	);
	break;

    case Type_VectorC:
	pSubset = contentAsVectorC ()->subsetOfType (pSubsetSpace, pKOTM, pValueReturn);
	break;

    case Type_Std:
	if (contentAsMonotype().store ()->Names (pKOTM)) {
	    rtPTOKEN_Handle::Reference pSubsetRPT (contentAsMonotype ().PPT ());
	    M_ASD *pThisSpace = pSubsetRPT->Space ();

	//  Use the monotype's domain if the result will be indistinguishable...
	    bool okToReuseDomain = pThisSpace == pSubsetSpace
		|| pThisSpace->Database () == pSubsetSpace->Database ();
	    rtPTOKEN_Handle::Reference pSubsetPPT (
		okToReuseDomain ? pSubsetRPT : new rtPTOKEN_Handle (
		    pSubsetSpace, pSubsetRPT->cardinality ()
		)
	    );
	    pSubset = rtLINK_RefConstructor (pSubsetRPT)->Append (
		0, pSubsetRPT->cardinality (), false
	    )->Close (pSubsetPPT);

	    if (pSubsetPPT != pSubsetRPT) {
		if (pValueReturn)
		    pValueReturn->setToSubset (pSubset, *this);
	    }
	    else if (pValueReturn)
		pValueReturn->setToCopied (*this);
	}
	break;

    default:
	raiseTypeException ("VDescriptor::subsetOfType");
	break;
    }

    return pSubset;
}


/****************
 *  'typeName'  *
 ****************/

char const *VDescriptor::typeName () const {
    switch (m_xType) {
    case Type_Empty:
	return "Empty";
    case Type_Std:
	return "Descriptor";
    case Type_Vector:
	return "Vector";
    case Type_VectorC:
	return "VectorC";
    case Type_Fragmentation:
	return "Fragmentation";
    default:
	return string ("#%d", m_xType);
    }
}


/******************************************
 ******************************************
 *****  Conversion and Normalization  *****
 ******************************************
 ******************************************/

/**************************************
 *  'convertMonotypeToFragmentation'  *
 **************************************/

bool VDescriptor::convertMonotypeToFragmentation (unsigned int fragmentSize) {
    if (Type_Std == m_xType && contentAsMonotype ().cardinality () > fragmentSize) {
	DSC_Descriptor iCopy = contentAsMonotype ();

	m_xType = Type_Empty;
	contentAsFragmentation ().construct (iCopy, fragmentSize);
	m_xType = Type_Fragmentation;

	iCopy.clear ();
	return true;
    }
    return false;
}


/******************************************
 *  'convertVectorsToMonotypeIfPossible'  *
 ******************************************/

bool VDescriptor::convertVectorsToMonotypeIfPossible () {
    DSC_Descriptor dsc;

    switch (m_xType) {
    case Type_Std:
	return true;

    case Type_VectorC:
	if (contentAsVectorC ()->SimplifiedToMonotype (&dsc)) {
	    HomogenizeVCCount++;
	    setContentToMoved (dsc);
	    return true;
	}
	break;

    case Type_Vector:
	if (contentAsVector ()->simplifiedToMonotype (dsc)) {
	    HomogenizeVectorCount++;
	    setContentToMoved (dsc);
	    return true;
	}
	break;

    default:
	break;
    }

    return false;
}


/******************************
 *  'convertToFragmentation'  *
 ******************************/

VFragmentation& VDescriptor::convertToFragmentation (bool fCoalescing) {
    switch (m_xType) {
    case Type_Vector:
	setContentToVector (new rtVECTOR_CType (contentAsVector ()));
	/*****  NO BREAK  *****/

    case Type_VectorC: {
	    rtVECTOR_CType* pVectorC = contentAsVectorC ();

	    m_xType = Type_Empty;
	    contentAsFragmentation ().construct (pVectorC);
	    m_xType = Type_Fragmentation;

	    pVectorC->release ();
	}

	if (VFragmentation::TracingFragmentationOps) {
	    IO_printf ("V(%d) to F:(%p)", m_xType, this);
	    contentAsFragmentation ().describe (1);
	    IO_printf ("\n");
	}
        break;

    case Type_Fragmentation:
	if (VFragmentation::TracingFragmentationOps) {
	    IO_printf ("F to F:(%p)", this);
	    contentAsFragmentation ().describe (1);
	    IO_printf ("\n");
	}
	if (fCoalescing) {
//	    if (IsntNil (m_pDistribution))
//	    {
//		contentAsFragmentation ().distribute (m_pDistribution);
//		m_pDistribution->release ();
//		m_pDistribution = NilOf (M_CPD *);
//	    }
    	    contentAsFragmentation ().Coalesce ();
	}
        break;

    default:
	/*---------------------------------------------------------------------------*
	 * Note: An error occurs if this is not polymorphic.
	 *---------------------------------------------------------------------------*/
        raiseTypeException ("convertToFragmentation");
        break;
    }

    return contentAsFragmentation ();
}


/***********************
 *  'convertToVector'  *
 ***********************/

rtVECTOR_Handle *VDescriptor::convertToVector () {
    rtVECTOR_Handle::Reference pVector;
    switch (m_xType) {
    case Type_Vector:
        break;
    case Type_VectorC:
	contentAsVectorC ()->getVector (pVector);
	setContentToVector (pVector);
        break;
    case Type_Fragmentation:
	contentAsFragmentation().getVector (pVector);
	setContentToVector (pVector);
        break;
    case Type_Std:
	setContentToVector (rtVECTOR_Handle::NewFrom (contentAsMonotype ()));
        break;
    default:
        raiseTypeException ("convertToVector");
        break;
    }

    return contentAsVector ();
}


/************************
 *  'convertToVectorC'  *
 ************************/

rtVECTOR_CType *VDescriptor::convertToVectorC () {
    rtVECTOR_Handle::Reference pVector;
    switch (m_xType) {
    case Type_Std:
	setToVector (rtVECTOR_Handle::NewFrom (contentAsMonotype ()));
        break;

    case Type_Fragmentation:
	contentAsFragmentation ().getVector (pVector);
	setContentToVector (pVector);
        break;

    default:
	break;
    }

    switch (m_xType) {
    case Type_Vector:
	setContentToVector (new rtVECTOR_CType (contentAsVector ()));
        break;

    case Type_VectorC:
	break;

    default:
	/*---------------------------------------------------------------------------*
	 * Note: An error occurs if this is not polymorphic.
	 *---------------------------------------------------------------------------*/
	raiseTypeException ("convertToVectorC");
	break;
    }

    return contentAsVectorC ();
}


/*****************
 *  'normalize'  *
 *****************/

void VDescriptor::normalize () {
    if (IsNil (m_pDistribution))
	return;

    switch (m_xType) {
    case Type_Std:
        contentAsMonotype ().distribute (m_pDistribution);
	break;

    case Type_Vector:
	convertToVectorC ();
	/* No break here */

    case Type_VectorC:
	setContentToVector (contentAsVectorC ()->distribute (m_pDistribution));
	break;

    case Type_Fragmentation:
	contentAsFragmentation ().distribute (m_pDistribution);
	break;

    default:
	raiseTypeException ("normalize");
	break;
    }

    m_pDistribution->release ();
    m_pDistribution = NilOf (M_CPD*);
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/*************
 *  'clear'  *
 *************/

void VDescriptor::clearContent () {
    switch (m_xType) {
    case Type_Empty:
        break;
    case Type_Std:
	contentAsMonotype ().clear ();
        break;
    case Type_Vector:
	contentAsVector ()->release ();
        break;
    case Type_VectorC:
	contentAsVectorC ()->release ();
        break;
    case Type_Fragmentation:
	contentAsFragmentation ().clear ();
        break;
    default:
	raiseTypeException ("clearContent");
        break;
    }
    m_xType = Type_Empty;
}

void VDescriptor::clear () {
    clearContent ();

    if (m_pDistribution) {
	m_pDistribution->release ();
	m_pDistribution = NilOf (M_CPD*);
    }
}


/*******************
 *  'setToCopied'  *
 *******************/

void VDescriptor::setToCopied (VDescriptor const& rSource) {
    switch (rSource.type ()) {
    case Type_Empty:
	clear ();
        break;
    case Type_Std:
	setToCopied (rSource.contentAsMonotype ());
        break;
    case Type_Vector:
	setToVector (rSource.contentAsVector ());
        break;
    case Type_VectorC:
	setToVector (rSource.contentAsVectorC ());
        break;
    case Type_Fragmentation:
	clear ();
	contentAsFragmentation ().construct (rSource.contentAsFragmentation ());
	m_xType = Type_Fragmentation;
	break;
    default:
	rSource.raiseTypeException ("setToCopied");
        break;
    }
    m_pDistribution = rSource.distribution ();
    if (m_pDistribution)
	m_pDistribution->retain ();
}


/****************************
 *  'setToReorderedSubset'  *
 ****************************/

/*---------------------------------------------------------------------------*
 *  Arguments:
 *	pReordering		- an optional ('Nil' if absent) standard CPD
 *				  for a reference u-vector specifying the
 *				  reordering of the selected subset.
 *	pSubset			- an optional ('Nil' if absent) constructor
 *				  for a link specifying the subset of
 *				  'source' to be moved.
 *	rSource			- an extended descriptor from which a
 *				  reordered subset will be extracted.  If
 *				  both 'pSubset' and 'pReordering' are 'Nil',
 *				  this macro will move 'rSource' into 'this'.
 *				  If either of the following arguments are
 *				  specified, 'rSource' may be altered in form
 *				  to optimize the operation that must be
 *				  performed.
 *---------------------------------------------------------------------------*/
void VDescriptor::setToReorderedSubset (
    M_CPD *pReordering, rtLINK_CType *pSubset, VDescriptor &rSource
)
{
    if (pSubset)
	setToSubset (pSubset, rSource);
    else
	setToMoved (rSource);

    if (pReordering)
	reorder (pReordering);
}


/*******************
 *  'setToSubset'  *
 *******************/

/*---------------------------------------------------------------------------*
 *  Arguments:
 *	pSubset			- a constructor for a link specifying the
 *				  elements to be extracted from the descriptor.
 *	rSource			- an extended descriptor from which elements
 *				  are to be extracted.
 *---------------------------------------------------------------------------*/
void VDescriptor::setToSubset (rtLINK_CType *pSubset, VDescriptor &rSource) {
    rSource.normalize ();
    if (rSource.isStandard ())
        setToSubset (pSubset, rSource.contentAsMonotype ());
    else
	rSource.convertToVector ()->getElements (*this, pSubset);
}

void VDescriptor::setToSubset (DSC_Pointer &rSubset, VDescriptor &rSource) {
    if (rSubset.holdsAnIdentity ())
	setToCopied (rSource);
    else if (rSubset.holdsAScalarReference ()) {
	rSource.normalize ();
	if (rSource.isStandard ())
	    setToSubset (rSubset, rSource.contentAsMonotype ());
	else {
	    clear ();
	    rSource.convertToVector ()->getElements (
		contentAsMonotype (), DSC_Pointer_Scalar (rSubset)
	    );
	    m_xType = Type_Std;
	}
    }
    else if (rSubset.holdsALink ())
	setToSubset (DSC_Pointer_Link (rSubset), rSource);
    else if (rSubset.holdsNonScalarReferences ()) {
	DSC_Pointer iLink;
	iLink.construct (rSubset);

	M_CPD *pDistribution = iLink.factor ();

	setToSubset (iLink, rSource);
	if (pDistribution) {
	    distribute (pDistribution);
	    pDistribution->release ();
	}

	iLink.clear ();
    }
    else {
	rtPTOKEN_Handle::Reference pPPT (rSubset.PPT ());

    /*-----------------------------------------------------------------------*
     *  The subset is everywhere undefined, making the composition computed
     *	by this routine everywhere undefined.  Implemented 'correctly' for
     *	federations, I suspect this routine should examine the co-domain
     *	structure of its 'source'; however, as implemented, it is simplified
     *	to base its result on the subset domain. (Pre-Jan. 2003, this routine
     *	returned the undefined associated with the initial database, which in
     *	all non-federated and most current federated cases is the database of
     *	the subset's domain).
     *-----------------------------------------------------------------------*/
	setToNA (pPPT, pPPT->KOT());
    }
}


/**********************************
 **********************************
 *****  Consistency Checking  *****
 **********************************
 **********************************/

void VDescriptor::checkDistribution (M_CPD *pDistribution) const {
    rtPTOKEN_Handle::Reference pPPT;
    switch (m_xType) {
    case Type_Std:
	pPPT.setTo (contentAsMonotype ().PPT ());
	break;
    case Type_Vector:
	pPPT.setTo (contentAsVector ()->getPToken ());
	break;
    case Type_VectorC:
	pPPT.setTo (contentAsVectorC ()->PPT ());
	break;
    case Type_Fragmentation:
	pPPT.setTo (contentAsFragmentation ().ptoken ());
	break;
    default:
	raiseTypeException ("distribution checking method");
	break;
    }

    if (pDistribution->ReferenceDoesntName (UV_CPx_PToken, pPPT)) raiseException (
	EC__InternalInconsistency,
	"Inconsistent Distribution: Dom(Descriptor)=[%u:%u]%u Dom(Distribution)=[%u:%u]%u",
	pPPT->spaceIndex (), pPPT->containerIndexNC (), pPPT->cardinality (),
	M_POP_ObjectSpace	(M_CPD_PointerToPOP (pDistribution, UV_CPx_PToken)),
	M_POP_ContainerIndex	(M_CPD_PointerToPOP (pDistribution, UV_CPx_PToken)),
	rtPTOKEN_BaseElementCount (pDistribution, UV_CPx_PToken)
    );
}


/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void VDescriptor::describe (unsigned int xLevel) {
    IO_printf ("%1d-", m_xType);
    switch (m_xType) {
    case Type_Fragmentation:
	contentAsFragmentation ().describe (xLevel + 1);
	break;

    case Type_Std:
	contentAsMonotype ().store ()->describe ();
	break;

    default:
	break;
    }
    if (m_pDistribution)
	m_pDistribution->print ();
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

/*---------------------------------------------------------------------------*
 *  Argument:
 *	pWhere			- an optional ('Nil' is absent) character
 *				  string describing the nature of the
 *				  objection.
 *---------------------------------------------------------------------------*/
void VDescriptor::raiseTypeException (char const *pWhere) const {
    raiseException (
	EC__InternalInconsistency,
	"VDescriptor: Unknown or unexpected %s type%s%s.",
	typeName (),
	pWhere ? " in " : "",
	pWhere ? pWhere : ""
    );
}

void VDescriptor::raiseAssignmentTargetException (Vdd::Store *pTarget) const {
    RTYPE_ComplainAboutType (
	"VDescriptor::assignTo", pTarget->rtype (), RTYPE_C_Vector
    );
}

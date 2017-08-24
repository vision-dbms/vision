/*****  VTaskDomain Implementation  *****/

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

#include "VTaskDomain.h"

/************************
 *****  Supporting  *****
 ************************/

#include "venvir.h"

#include "VSymbolImplementation.h"


/*************************
 *************************
 *****               *****
 *****  VTaskDomain  *****
 *****               *****
 *************************
 *************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VTaskDomain);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VTaskDomain::MetaMaker () {
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

/********************
 *****  Ground  *****
 ********************/

VReference<VTaskDomain> VTaskDomain::g_pGroundDomain;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTaskDomain::VTaskDomain (VTaskDomain *pParent, rtLINK_CType *pSubset, M_CPD *pReordering)
: m_pParent	(pParent)
, m_pSubset	(pSubset)
, m_pReordering	(pReordering)
, m_pDPT	(pReordering
		 ? static_cast<rtPTOKEN_Handle*>(pReordering->GetContainerHandle (UV_CPx_PToken, RTYPE_C_PToken))
		 : pSubset->PPT ())
{
}

VTaskDomain::VTaskDomain () : m_pDPT (M_AttachedNetwork->TheScalarPTokenHandle ()) {
}

VTaskDomain* VTaskDomain::groundDomain  () {
    if (g_pGroundDomain.isNil ())
	g_pGroundDomain.setTo (new VTaskDomain ());

    return g_pGroundDomain;
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*************************
 *****  Path Access  *****
 *************************/

VTaskDomain::PathType VTaskDomain::getPathToAncestor (
    VTaskDomain* pDestination, rtLINK_CType*& rpRestriction, M_CPD*& rpReordering
) const {
/*****  Determine the relationship of this domain to its parent, ... *****/
    VCPDReference	pReordering	(m_pReordering);
    VLinkCReference	pRestriction	(m_pSubset);

    PathType xPathType
	= pReordering && pRestriction ? PathType_SequencedSubset
	: pReordering ? PathType_Sequence
	: pRestriction ? PathType_Subset
	: PathType_Identity;

/*****  ... and locate a task which owns a valid stack context  ...  *****/
    M_CPD* seq1;
    VTaskDomain* pAncestor = m_pParent;
    while (pAncestor != pDestination) {
	rtLINK_CType* pAncestorSubset = pAncestor->subset ();
	M_CPD* pAncestorSequence = pAncestor->reordering ();

	if (pAncestorSequence) {
/***** ... adjust the path type for 'SequencedSubset' tasks:  *****/
	    if (pAncestorSubset) {
		switch (xPathType) {
		case PathType_Identity:		/* I->Ut->Lt => Ut[I]->Lt => Ut->Lt */
		    pReordering.setTo (pAncestorSequence);
		    pRestriction.setTo (pAncestorSubset);
		    xPathType = PathType_SequencedSubset;
		    break;

		case PathType_Sequence:		/* U->Ut->Lt => Ut[U]->Lt */
		    pReordering.claim (rtREFUV_UVExtract (pAncestorSequence, pReordering));
		    pRestriction.setTo (pAncestorSubset);
		    xPathType = PathType_SequencedSubset;
		    break;

		case PathType_Subset:		/* L->Ut->Lt => Ut[L]->Lt  */
		    pReordering.claim (rtREFUV_LCExtract (pAncestorSequence, pRestriction));
		    pRestriction.setTo (pAncestorSubset);
		    xPathType = PathType_SequencedSubset;
		    break;

		case PathType_SequencedSubset:	/* U->L->Ut->Lt
						=> U->Ut[L]->Lt
						=> Ut[L][U]->Lt */
		    seq1 = rtREFUV_LCExtract (pAncestorSequence, pRestriction);

		    pReordering.claim (rtREFUV_UVExtract (seq1, pReordering));
		    pRestriction.setTo (pAncestorSubset);
		    xPathType = PathType_SequencedSubset;

		    seq1->release ();
		    break;
		}
	    }

/***** ... adjust the path type for 'Sequenced' tasks ...  *****/
	    else {
		switch (xPathType) {
		case PathType_Identity:		/* I->Ut => U[I] => Ut */
		    pReordering.setTo (pAncestorSequence);
		    xPathType = PathType_Sequence;
		    break;

		case PathType_Sequence:		/* U->Ut => Ut[U] */
		    pReordering.claim (rtREFUV_UVExtract (pAncestorSequence, pReordering));
		    xPathType = PathType_Sequence;
		    break;

		case PathType_Subset:		/* L->Ut => Ut[L] */
		    pReordering.claim (rtREFUV_LCExtract (pAncestorSequence, pRestriction));
		    pRestriction.clear ();
		    xPathType = PathType_Sequence;
		    break;

		case PathType_SequencedSubset:	/* U->L->Ut => U->Ut[L] => Ut[L][U] */
		    seq1 = rtREFUV_LCExtract (pAncestorSequence, pRestriction);

		    pRestriction.clear ();
		    pReordering.claim (rtREFUV_UVExtract (seq1, pReordering));
		    xPathType = PathType_Sequence;

		    seq1->release ();
		    break;
		}
	    }
	}

/***** ... adjust the relationship for 'Subset' tasks ...  *****/
	else if (pAncestorSubset) {
	    switch (xPathType) {
	    case PathType_Identity:		/* I->Lt => Lt[I] => Lt */
		pRestriction.setTo (pAncestorSubset);
		xPathType = PathType_Subset;
		break;

	    case PathType_Sequence:		/* U->Lt => U->Lt */
		pRestriction.setTo (pAncestorSubset);
		xPathType = PathType_SequencedSubset;
		break;

	    case PathType_Subset:		/* L->Lt => Lt[L] */
	    case PathType_SequencedSubset:	/* U->L->Lt => U->Lt[L] */
		pRestriction.claim (pAncestorSubset->Extract (pRestriction));
		break;
	    }
	}
	pAncestor = pAncestor->parent ();
    }

    if (rpRestriction = pRestriction)
	rpRestriction->retain ();

    if (rpReordering = pReordering)
	rpReordering->retain ();

    return xPathType;
}

/**************************
 *****  Path Cleanup  *****
 **************************/

void VTaskDomain::releasePathMaps (
    PathType xReturnedPathType, rtLINK_CType* pRestriction, M_CPD* pReordering
) {
    switch (xReturnedPathType) {
    case PathType_Subset:
	pRestriction->release ();
	break;

    case PathType_Sequence:
	pReordering->release ();
	break;

    case PathType_SequencedSubset:
	pRestriction->release ();
	pReordering->release ();
	break;

    default:
	break;
    }
}

/*****  POP-Vector Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName POPVECTOR

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "vdsc.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"

/*****  Shared  *****/
#include "popvector.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (rtPOPVECTOR_Handle);


/**********************************************************
 *****  Standard Popvector Consistency Check Routine  *****
 **********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to check a POP-Vector for corruption.
 *****/
void rtPOPVECTOR_Handle::CheckConsistency () {
    if (POPVECTOR_PV_IsInconsistent ((POPVECTOR_PVType*)ContainerContent ())) {
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"Corrupted popvector[%d:%d] detected", SpaceIndex (), ContainerIndex ()
	    )
	);
    }
}


/**********************************************************
 *****  Standard POP-Vector CPD Management Utilities  *****
 **********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard POP-Vector CPD.
 *
 *  Argument:
 *	cpd			- the address of a standard POP-Vector CPD
 *				  which is to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *  Notes:
 *	The number of pointers a cpd will have is defined by the specific
 *	RTYPE of which the cpd's object is an instance. This routine sets
 *	pointers of the cpd to the first n POPs in the vector. Thus,  the
 *	first pointer in the cpd points to the first POP in the vector, the
 *	second to the second, etc.
 *	In addition, the POP-Vector keeps a 'private' pointer for
 *	debugging purposes.
 *
 *****/
PublicFnDef void POPVECTOR_InitStdCPD (M_CPD* cpd) {
    M_POP	*pop = POPVECTOR_CPD_Array (cpd);
    pointer_t	*p = (pointer_t *)M_CPD_Pointers (cpd),
		*pl = p + cpd->PointerCount () - POPVECTOR_CPD_XtraPtrCount;

    while (p < pl)
	*p++ = (pointer_t)pop++;

    *p = (pointer_t)POPVECTOR_CPD_Array (cpd);
}


/********************************************************
 *****  Standard POP-Vector Instantiation Routines  *****
 ********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create and initialize a new standard POP-Vector.
 *
 *  Arguments:
 *	pSpace			- creation space.
 *	xType			- the representation type of the POP-Vector
 *				  being created.
 *	cElements		- the number of POPs to be included.
 *
 *  Returns:
 *	A standard CPD for the POP-Vector created.
 *
 *  Note:
 *	The POPs will all be initialized in the standard way.
 *
 *****/
PublicFnDef M_CPD *POPVECTOR_New (M_ASD *pSpace, RTYPE_Type xType, unsigned int cElements) {
    M_CPD *cpd = pSpace->CreateContainer (xType, POPVECTOR_SizeofPVector (cElements));

    POPVECTOR_InitStdCPD (cpd);

    cpd->constructReferences (0, cElements);

    POPVECTOR_CPD_ElementCount (cpd) = cElements;
    POPVECTOR_CPD_IsInconsistent (cpd) = false;

    return cpd;
}


/********************************************
 *****  Standard POP-Vector GC Routine  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PublicFnDef void POPVECTOR_ReclaimContainer (
    M_ASD * ownerASD, M_CPreamble const *preambleAddress
) {
    POPVECTOR_PVType const *pv = (POPVECTOR_PVType const *)(preambleAddress + 1);
    ownerASD->Release (POPVECTOR_PV_Array (pv), POPVECTOR_PV_ElementCount (pv));
}


/**********************************************
 *****  Standard POP-Vector Save Routine  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'save' a POP-Vector.
 *****/
bool rtPOPVECTOR_Handle::PersistReferences () {
    POPVECTOR_PVType *pv = (POPVECTOR_PVType *)ContainerContent ();
    return Persist (POPVECTOR_PV_Array (pv), POPVECTOR_PV_ElementCount (pv));
}


/**********************************************
 *****  Standard POP-Vector Mark Routine  *****
 **********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'mark' the containers referenced by a POP-Vector.
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void POPVECTOR_MarkContainers (M_ASD *pSpace, M_CPreamble const *pContainer) {
    POPVECTOR_PVType const *pv = (POPVECTOR_PVType const *)(pContainer + 1);

    pSpace->Mark (POPVECTOR_PV_Array (pv), POPVECTOR_PV_ElementCount (pv));
}


/**************************************************************
 *****  Standard POP-Vector Positional Alignment Routine  *****
 **************************************************************/

/****************************
 *  Initialization Utility  *
 ****************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to properly initialize inserted gaps during the
 *****  alignment process
 *
 *  Arguments:
 *	pTail			- a pointer to the region just inserted (sShift > 0)
 *				  or about to be deleted (sShift < 0).
 *	sShift			- the number of bytes in the affected region.
 *				  Values < 0 imply deletions, > 0 insertions.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  the 'shift' and 'pAlignAP' values defined by
 *				  POPVECTOR_Align.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializePOPs (
    VContainerHandle *pContainerHandle, pointer_t pTail, ptrdiff_t Unused(sShift), va_list ap
) {
    V::VArgList iArgList (ap);

    int const shift = iArgList.arg<int>();

    if (shift < 0)
        pContainerHandle->ClearReferences ((M_POP*)pTail, -shift);
    else
	pContainerHandle->constructReferences ((M_POP*)pTail, shift);
}


/********************
 *  Utility Macros  *
 ********************/

#define AlignmentInsertMacro(origin, shift)\
    AlignmentInsertDeleteMacroBody (origin, shift)

#define AlignmentDeleteMacro(origin, shift)\
    AlignmentInsertDeleteMacroBody (origin, shift)

#define AlignmentInsertDeleteMacroBody(origin, shift)\
    M_CPD_PointerToPOP (cpd, alignmentIndex) =\
	    M_CPD_PointerToPOP (cpd, alignmentIndex - 1) + origin + 1;\
    cpd->ShiftContainerTail (\
	alignmentIndex,\
	(elementCount - origin) * sizeof (M_POP),\
	(ptrdiff_t)(shift) * sizeof (M_POP),\
	true,\
	InitializePOPs,\
	shift,\
	origin\
    );\
    elementCount += shift;\
    POPVECTOR_CPD_ElementCount (cpd) += shift


/**********************************
 *  Positional Alignment Routine  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  Standard routine to normalize a pop-vector.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the POP-Vector to be
 *				  normalized.
 *	pTokenIndex		- The index into the cpd pointer array which
 *				  points to the POP for the ptoken which will
 *				  control the alignment.
 *	alignmentIndex		- The index into the cpd pointer array which
 *				  marks the alignable portion of the popvector.
 *	elementCount		- The number of alignable elements in this
 *				  popvector.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD* POPVECTOR_Align (
    M_CPD* cpd, unsigned int xPToken, unsigned int alignmentIndex, unsigned int elementCount
) {
/*****  If POP-Vector's ptoken is a chain terminator, Nothing to do  *****/
    M_CPD* pTokenCPD;
    rtPTOKEN_IsntCurrent (cpd, xPToken, pTokenCPD);
    if (IsNil (pTokenCPD))
        return cpd;

/*****  Obtain the constructor  *****/
    rtPTOKEN_CType *ptConstructor = rtPTOKEN_CPDCumAdjustments (pTokenCPD);

/*****  ...enable modification of the popvector, ...  *****/
    cpd->EnableModifications ();
    cpd->CheckConsistency ();
    POPVECTOR_CPD_IsInconsistent (cpd) = true;

/*****  Do the normalization  *****/
    rtPTOKEN_BTraverseAdjustments (
	ptConstructor, AlignmentInsertMacro, AlignmentDeleteMacro
    );

/*****  Update the the POP-Vector's P-Token  *****/
    cpd->StoreReference (xPToken, ptConstructor->NextGeneration ());

/*****  Cleanup  *****/
    POPVECTOR_CPD_IsInconsistent (cpd) = false;
    pTokenCPD->release ();
    ptConstructor->discard ();

    return cpd;
}


/***********************************************
 *****  Standard POP-Vector Print Routine  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Standard routine to print a pop-vector.
 *
 *  Arguments:
 *	popvectorCPD		- a standard CPD for the u-vector to be
 *				  printed.
 *	recursive		- a boolean which, when true, requests the
 *				  display of the P-Token associated with this
 *				  POP-Vector.
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void POPVECTOR_Print (M_CPD* popvectorCPD, int recursive) {
    IO_printf (
	"********************************************\n%s\n", popvectorCPD->RTypeName ()
    );

    unsigned int index = POPVECTOR_CPx_Cursor (popvectorCPD);

    M_POP* cursor = POPVECTOR_CPD_Cursor (popvectorCPD);
    unsigned int elementCount	= POPVECTOR_CPD_ElementCount (popvectorCPD);
    unsigned int i;

    for (POPVECTOR_CPD_Cursor (popvectorCPD) = POPVECTOR_CPD_Array (popvectorCPD), i = 0;
	 i < elementCount;
	 i++, POPVECTOR_CPD_Cursor (popvectorCPD)++)
    {
	if (recursive)
	    RTYPE_RPrint (popvectorCPD, index);
	else
	    RTYPE_Print (popvectorCPD, index);
	IO_printf ("\n");
    }
    IO_printf ("********************************************\n");
    POPVECTOR_CPD_Cursor(popvectorCPD) = cursor;

}


/*****************************************************
 ***** Standard POP-Vector Manipulation Routines *****
 *****************************************************/

PublicFnDef void POPVECTOR_NextElement (M_CPD* popvectorCPD) {
    int elementCount	    = POPVECTOR_CPD_ElementCount (popvectorCPD);

    if ((POPVECTOR_CPD_Cursor (popvectorCPD) += 1) >=
        POPVECTOR_CPD_Array (popvectorCPD) + elementCount)
    {
        POPVECTOR_CPD_Cursor (popvectorCPD) -= 1;
	IO_printf (">>>  End of POP-Vector  <<<\n");
    }
}

PublicFnDef void POPVECTOR_PreviousElement (M_CPD* popvectorCPD) {
    if ((POPVECTOR_CPD_Cursor (popvectorCPD) -= 1) < POPVECTOR_CPD_Array (popvectorCPD)) {
        POPVECTOR_CPD_Cursor (popvectorCPD) = POPVECTOR_CPD_Array (popvectorCPD);
	IO_printf (">>>  Beginning of POP-Vector  <<<\n");
    }
}

PublicFnDef void POPVECTOR_GoToElement (M_CPD* popvectorCPD, int i) {
    if (i >= 0 && i < (int) POPVECTOR_CPD_ElementCount (popvectorCPD))
	POPVECTOR_CPD_Cursor (popvectorCPD) = POPVECTOR_CPD_Array (popvectorCPD) + i;
}


/**************************************************
 *****  Standard POP-Vector Debugger Methods  *****
 **************************************************/

IOBJ_DefinePublicUnaryMethod (POPVECTOR_DM_Size) {
    return IOBJ_IntIObject (POPVECTOR_CPD_ElementCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefinePublicUnaryMethod (POPVECTOR_DM_NextElement) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    POPVECTOR_NextElement (cpd);

    return self;
}

IOBJ_DefinePublicUnaryMethod (POPVECTOR_DM_PreviousElement) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    POPVECTOR_PreviousElement (cpd);

    return self;
}

IOBJ_DefinePublicMethod (POPVECTOR_DM_GoToElement) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);
    int i = IOBJ_NumericIObjectValue (parameterArray[0], int);

    POPVECTOR_GoToElement (cpd, i);

    return self;
}

IOBJ_DefinePublicUnaryMethod (POPVECTOR_DM_Eprint) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    RTYPE_Print (cpd, POPVECTOR_CPx_Cursor (cpd));

    return self;
}

IOBJ_DefinePublicUnaryMethod (POPVECTOR_DM_Browser) {
    M_CPD* cpd = RTYPE_QRegisterCPD (self);

    return RTYPE_Browser (cpd, POPVECTOR_CPx_Cursor (cpd));
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (POPVECTOR);
    FAC_FDFCase_FacilityDescription ("POP-Vector");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  popvector.c 1 replace /users/jck/system
  860526 23:25:51 jck Introducing popvectors as a support type
for the subset of rtypes that is represented as a sequence of POP's

 ************************************************************************/
/************************************************************************
  popvector.c 2 replace /users/jck/system
  860605 16:25:38 jck Release in support of RTvstore

 ************************************************************************/
/************************************************************************
  popvector.c 3 replace /users/jck/system
  860611 11:08:45 jck Added Alignment Routine

 ************************************************************************/
/************************************************************************
  popvector.c 4 replace /users/mjc/system
  860617 18:43:44 mjc Added 'save' routine

 ************************************************************************/
/************************************************************************
  popvector.c 5 replace /users/mjc/system
  860620 17:46:40 mjc Fixed align routine to GC deleted POPs

 ************************************************************************/
/************************************************************************
  popvector.c 6 replace /users/jad/system
  860814 14:20:17 jad use new ptoken IsntCurrent routines in align

 ************************************************************************/
/************************************************************************
  popvector.c 7 replace /users/jad/system
  860914 11:15:27 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  popvector.c 8 replace /users/mjc/system
  861226 12:26:24 mjc Removed superfluous unwind trap

 ************************************************************************/
/************************************************************************
  popvector.c 9 replace /users/jck/system
  870415 09:54:03 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  popvector.c 10 replace /users/mjc/translation
  870524 09:40:21 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  popvector.c 11 replace /users/jck/system
  871007 13:32:54 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  popvector.c 12 replace /users/mjc/Software/system
  871208 15:36:14 mjc Fixed align deletion cleanup bug, enabled align container shrinkage

 ************************************************************************/
/************************************************************************
  popvector.c 13 replace /users/jck/system
  890222 15:23:13 jck Implemented corruption detection mechanism

 ************************************************************************/

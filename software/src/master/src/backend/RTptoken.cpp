/*****  Positional Generation Token Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtPTOKEN

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "V_VAllocator.h"

/*****  Self  *****/
#include "RTptoken.h"


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

DEFINE_CONCRETE_RTT (rtPTOKEN_Handle);


/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool
    TracingCartesianUpdate	= false,
    TracingCollapse		= false,
    TracingConversion		= false,
    TracingPTCAllocator		= false,
    TracingSDCInsertion		= false,
    TracingSDCAllocator	    	= false;


/****************************
 *****  Usage Counters  *****
 ****************************/

PrivateVarDef unsigned int
    BaseElementCount		= 0,
    BasePTokenCount		= 0,
    IsCurrentCount		= 0,
    IsntCurrentCount		= 0,

    NewBasePTokenCount		= 0,
    NewCartesianPTokenCount	= 0,
    NewShiftPTConstructorCount	= 0,

    AppendAdjustmentCount 	= 0,
    DiscardConstructorCount	= 0,

    BasicToPTokenCount		= 0,
    ToConstructorCount	    	= 0,

    CumulativeAdjustmentsCount  = 0,

    UpdateCartesianPTokenCount  = 0;


/*******************************
 *****  General Utilities  *****
 *******************************/

/******************
 *  Verification  *
 ******************/

/*---------------------------------------------------------------------------
 *****  Internal routine to check a P-Token for consistency
 *****/
void rtPTOKEN_Handle::CheckConsistency () {
    if (isntClosed ()) ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "Corrupted ptoken[%u:%u] detected", spaceIndex (), containerIndex ()
	)
    );
}


/*************
 *  Display  *
 *************/

void rtPTOKEN_Handle::dump () const {
    IO_printf ("PTOKEN DUMP:\n");
    IO_printf (
	"  type:%s, closed=%u, count=%u, baseElementCount=%u\n",
	typeName (), isClosed (), sdCount (), cardinality ()
    );

    if (typeIsntShift ())
	IO_printf ("  next Generation: Nil\n");
    else {
	IO_printf ("  next Generation: \n");
	rtPTOKEN_Handle::Reference pNextGeneration (nextPToken ());
	pNextGeneration->dump ();
    }
}

char const *rtPTOKEN_Handle::typeName () const {
    if (hasNoContainer ())
	return "Base";
    else switch (type ()) {
    case rtPTOKEN_TT_Base:
	return "Base";
    case rtPTOKEN_TT_Shift:
	return "Shift";
    case rtPTOKEN_TT_Cartesian:
	return "Cartesian";
    default:
	return "Unknown";
    }
}

bool rtPTOKEN_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;

    rtPTOKEN_Handle* self = const_cast<rtPTOKEN_Handle*>(this);
    self->createContainer();

    switch (xReference) {
    case 0:
	*pResult = *nextGenerationPOP ();
	break;
    case 1:
	if (typeIsCartesian ())
	    *pResult = *rowPTokenPOP ();
	else
	    bResultValid = false;
	break;
    case 2:
	if (typeIsCartesian ())
	    *pResult = *colPTokenPOP ();
	else
	    bResultValid = false;
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard P-Token CPD.
 *
 *  Argument:
 *	cpd			- the address of the CPD to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *  Behavior:
 *	This routine will initial the pointers of the CPD according to the
 *	description of a standard P-Token CPD given in "RTptoken.d".
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *cpd) {
    rtPTOKEN_Type *base = M_CPD_ContainerBaseAsType (cpd, rtPTOKEN_Type);

    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_NextGeneration) = &rtPTOKEN_PT_nextGeneration	(base);
    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_ColPToken	) = &rtPTOKEN_PT_ColPToken	(base);
    M_CPD_PointerToPOP (cpd, rtPTOKEN_CPx_RowPToken	) = &rtPTOKEN_PT_RowPToken	(base);
}


/************************************************************
 *****  Cartesian P-Token Managment and Query Routines  *****
 ************************************************************/

/*---------------------------------------------------------------------------
 *****  Private routine to modify a cartesian ptoken when its not current.
 *
 *  Arguments:
 *	pRowPToken	- optional (Nil if ommitted).
 *			  a handle for the 'not current' ptoken that
 *                        correspounds to the cartesian ptoken's rows.
 *		          (If this ptoken is current it should be Nil.)
 *	pColPToken	- optional (Nil if ommitted).
 *			  a handle for the 'not current' ptoken that
 *                        correspounds to the cartesian ptoken's columns.
 *		          (If this ptoken is current it should be Nil.)
 *
 *  Returns:
 *	true if the ptoken is now current and false otherwise
 *
 *  Warning:
 *	Be very careful to not call anything that tries to update the
 *      cartesian ptoken during this update !!!
 *
 *****/
bool rtPTOKEN_Handle::normalizationIsTerminal (rtPTOKEN_Handle *pRowPToken, rtPTOKEN_Handle *pColPToken) {
    int
	origNumberOfColumns, newNumberOfColumns,
	currentRow, currentColumn, /* used by oldMatrixPos to determine the
				      row and column to output */
	rowPointer;		   /* goes backwards, the next row to be
				      processed */
    rtPTOKEN_CType
	*rowPTConstructor = NilOf (rtPTOKEN_CType*),
	*colPTConstructor = NilOf (rtPTOKEN_CType*);


/***** Traversal Macros ... *****/
#define oldMatrixPos ((currentRow * origNumberOfColumns) + currentColumn)


/***** Column Traversal Macros ... *****/
#define handleColInsert(origin, shift) {\
    if (TracingCartesianUpdate) IO_printf (\
	"in HandleColInsert: org=%d, sft=%d\n", origin, shift\
    );\
    currentColumn = origin;\
    pTokenConstructor->AppendAdjustment (oldMatrixPos, shift);\
}

#define handleColDelete(origin, shift) {\
    if (TracingCartesianUpdate) IO_printf (\
	"in HandleColDelete: org=%d, sft=%d\n", origin, shift\
    );\
    currentColumn = origin;\
    pTokenConstructor->AppendAdjustment (oldMatrixPos, shift);\
}

#define outputWholeRow {\
    if (TracingCartesianUpdate) IO_printf (\
	"In OutputWholeRow: %d\n", currentRow\
    );\
    /* traverse the columns backwards ... */\
    rtPTOKEN_BTraverseAdjustments2 (\
	colPTConstructor, handleColInsert, handleColDelete\
    );\
}


/***** Both Row and Column Traversal Macros ... *****/
#define locateRow(row) {\
    if (TracingCartesianUpdate) IO_printf (\
	"in LocateRow: %d, rowPointer=%d\n", row, rowPointer\
    );\
    while (rowPointer >= (row)) {\
	currentRow = rowPointer;\
	outputWholeRow;\
	rowPointer--;\
    }\
}

#define addRows(row, count) {\
    if (TracingCartesianUpdate) IO_printf (\
	"In AddRows: Row=%d, cnt=%d, currentRow=%d\n", row, count, currentRow\
    );\
    currentColumn = 0;\
    currentRow = (row);\
    pTokenConstructor->AppendAdjustment (oldMatrixPos, newNumberOfColumns * (count));\
}

#define deleteRows(row, count) {\
    if (TracingCartesianUpdate) IO_printf (\
	"In DeleteRows: Row=%d, cnt=%d, currentRow=%d\n",\
	row, count, currentRow\
    );\
    /*** count is negative for a delete ***/\
    currentColumn = 0;\
    currentRow = (row);\
    pTokenConstructor->AppendAdjustment (oldMatrixPos, origNumberOfColumns * (count));\
}

#define handleRowInsert(origin, shift) {\
    if (TracingCartesianUpdate)	IO_printf (\
	"In HandleRowInsert: org=%d, sht=%d, rowPointer=%d\n",\
	origin, shift, rowPointer\
    );\
    locateRow (origin);\
    addRows (origin, shift);\
}

#define handleRowDelete(origin, shift) {\
    if (TracingCartesianUpdate) IO_printf (\
	"In HandleRowDelete: org=%d, sht=%d, rowPointer=%d\n",\
	origin, shift, rowPointer\
    );\
    locateRow (origin);\
    deleteRows (origin, shift);\
    rowPointer = (origin) + (shift) - 1; /* skip the deleted rows */\
}


/***** Row Only Traversal Macros ... *****/
#define rowOnly_addOrDeleteRows(row, count) {\
    if (TracingCartesianUpdate) IO_printf (\
	"In rowOnly_AddOrDeleteRows: Row=%d, cnt=%d\n", row, count\
    );\
    /*** count is negative for a delete ***/\
    currentColumn = 0;\
    currentRow = (row);\
    pTokenConstructor->AppendAdjustment (oldMatrixPos, origNumberOfColumns * (count));\
}

#define rowOnly_handleInsertDelete(origin, shift) {\
    if (TracingCartesianUpdate) IO_printf (\
	"In RowOnly_HandleInsertDelete: org=%d, sft=%d\n", origin, shift\
    );\
    rowOnly_addOrDeleteRows(origin, shift);\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'normalizationIsTerminal'
 *---------------------------------------------------------------------------
 */

    UpdateCartesianPTokenCount++;

    bool rowsNotCurrent = IsntNil (pRowPToken);
    bool colsNotCurrent = IsntNil (pColPToken);

/*****  If both are current, return ... *****/
    if (!rowsNotCurrent && !colsNotCurrent) {
	return true;
    }

/*****  Make a ptoken constructor for the product  ... *****/
    rtPTOKEN_CType *pTokenConstructor = makeAdjustments ();

/***** Determine the original number of rows ... *****/
    unsigned int origNumberOfRows = rowsNotCurrent
	? pRowPToken->cardinality () : rtPTOKEN_BaseElementCount (this, rowPTokenPOP ());

/***** If the column ptoken isn't current, collapse it ... *****/
    if (colsNotCurrent) {
	/*** Special case empty cartesian ptokens ... ***/
	/*** If there are no rows we don't need to do the traversal,
             we only need to update to the new ptoken ***/
	if (origNumberOfRows == 0) {
	    colsNotCurrent = false;

	    /*** Store the new column ptoken here for the ToPToken routine ***/
	    pTokenConstructor->m_pColPToken.setTo (pColPToken->basePToken ());

	    origNumberOfColumns = pTokenConstructor->m_pColPToken->cardinality ();
	}

	/*** Need to do the traversal ... ***/
	else {
	    colPTConstructor = pColPToken->getAdjustments ();

	    /*** Store the new column ptoken here for the ToPToken routine ***/
	    pTokenConstructor->m_pColPToken.setTo (colPTConstructor->NextGeneration ());

	    origNumberOfColumns = pColPToken->cardinality ();
	    newNumberOfColumns = colPTConstructor->NextGeneration ()->cardinality ();
	}
    }
    else {
	/*** Store the column ptoken here for the ToPToken routine ***/
	pTokenConstructor->m_pColPToken.setTo (colPTokenHandle ());

	origNumberOfColumns = pTokenConstructor->m_pColPToken->cardinality ();
    }

/***** If the row ptoken isnt current, collapse it ... *****/
    if (rowsNotCurrent) {
	/*** Special case empty cartesian ptokens ... ***/
	/*** If the columns are empty then we don't need to do the traversal,
	     we only need to update the row ptoken to the new one ***/
	if (origNumberOfColumns == 0) {
	    rowsNotCurrent = false;

	    /*** Store the new row ptoken here for the ToPToken routine ***/
	    pTokenConstructor->m_pRowPToken.setTo (pRowPToken->basePToken ());

	    /*** currentRow = the last row (the number of rows - 1) ***/
	    currentRow = pTokenConstructor->m_pRowPToken->cardinality () - 1;
	}

	/*** Need to do the traversal ***/
	else {
	    rowPTConstructor = pRowPToken->getAdjustments ();

	    /*** Store the new row ptoken here for the ToPToken routine ***/
	    pTokenConstructor->m_pRowPToken.setTo (rowPTConstructor->NextGeneration ());

	    /*** currentRow = the last row(the original number of rows -1) ***/
	    currentRow = pRowPToken->cardinality () - 1;
	}
    }
    else {
	/*** Store the row ptoken here for the ToPToken routine ***/
	pTokenConstructor->m_pRowPToken.setTo (rowPTokenHandle ());

	/*** currentRow = the last row (the original number of rows - 1) ***/
	currentRow = pTokenConstructor->m_pRowPToken->cardinality () - 1;
    }

/***** Do the traversal ... *****/
    if (rowsNotCurrent && colsNotCurrent) {
	rowPointer = currentRow;

	/*** Traverse the rows from the bottom up ... ***/
	rtPTOKEN_BTraverseAdjustments (
	    rowPTConstructor, handleRowInsert, handleRowDelete
	);

	/*** Finish up the rows ... ***/
	currentRow = rowPointer;
	while (currentRow >= 0) {
	    outputWholeRow;
	    currentRow--;
	}
    }
    else if (rowsNotCurrent) {
	/*** Traverse the rows from the bottom up ... ***/
	rtPTOKEN_BTraverseAdjustments (
	    rowPTConstructor, rowOnly_handleInsertDelete, rowOnly_handleInsertDelete
	);
    }
    else if (colsNotCurrent) {
	while (currentRow >= 0) {
	    outputWholeRow;
	    currentRow--;
	}
    }

/***** If no adjustments were added, the ptoken will now be current *****/
    bool bCurrent = rtPTOKEN_PTC_AltCount (pTokenConstructor) == 0;

/***** Discard the constructors ... *****/
    if (rowPTConstructor)
	rowPTConstructor->discard ();
    if (colPTConstructor)
	colPTConstructor->discard ();

/***** Finish the cartesian ptoken ... *****/
    pTokenConstructor->ToPToken ()->discard ();

    return bCurrent;

/***** undef the macros ... *****/
#undef oldMatrixPos
#undef handleColInsert
#undef handleColDelete
#undef outputWholeRow
#undef locateRow
#undef addRows
#undef deleteRows
#undef handleRowInsert
#undef handleRowDelete
#undef rowOnly_addOrDeleteRows
#undef rowOnly_handleInsertDelete
}


/*---------------------------------------------------------------------------
 ***** Private routine to determine if a cartesian ptoken is terminal.
 *****/
bool rtPTOKEN_Handle::productIsTerminal () {
    bool bTerminal = true;

    rtPTOKEN_Handle::Reference pRowPToken;
    if (isntTerminal (rowPTokenPOP (), pRowPToken))
	bTerminal = false;

    rtPTOKEN_Handle::Reference pColPToken;
    if (isntTerminal (colPTokenPOP (), pColPToken))
	bTerminal = false;

    return bTerminal || normalizationIsTerminal (pRowPToken, pColPToken);
}

void rtPTOKEN_Handle::setProductComponentsTo (rtPTOKEN_Handle *pRowPToken, rtPTOKEN_Handle *pColPToken) {
    EnableModifications ();
    clearIsClosed ();
    StoreReference (rowPTokenPOP (), pRowPToken);
    StoreReference (colPTokenPOP (), pColPToken);
    setIsClosed ();
}


/*---------------------------------------------------------------------------
 *****  Routine to verify that the specified row and column ptokens match
 *****  the cartesian ptoken.
 *
 *  Arguments:
 *	pProduct		- the cartesian ptoken.
 *	pRowPToken		- the row P-Token.
 *	pColPTokenRef/Index	- a reference to a POP for the column P-Token.
 *
 *  Returns:
 *	NOTHING - Executed for side effect and error detection only.
 *
 *  Errors Signalled:
 *	'EC__InternalInconsistency' if either the row or column ptokens do not
 *	match the row and column ptokens of the cartesian ptoken.
 *
 *****/
void rtPTOKEN_Handle::verifyProduct (rtPTOKEN_Handle *pRowPToken, rtPTOKEN_Handle *pColPToken) {
    if (typeIsntCartesian ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_CartesianVerification: Not a Cartesian PToken"
    );

    if (ReferenceDoesntName (rowPTokenPOP (), pRowPToken)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_CartesianVerification: Row Inconsistency"
    );

    if (ReferenceDoesntName (colPTokenPOP (), pColPToken)) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_CartesianVerification: Column Inconsistency"
    );
}


/********************************************************
 *****  POP Reference Based P-Token Query Routines  *****
 ********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the element count of a P-Token given a
 *****  POP reference for that P-Token.
 *
 *  Arguments:
 *	p/xPTokenRef		- a reference to a POP for the P-Token whose
 *				  base element count is desired.
 *
 *  Returns:
 *	The base element count of the P-Token referenced by the POP.  An
 *	error will be signalled if the POP does not reference a P-Token.
 *
 *****/
PublicFnDef unsigned int rtPTOKEN_BaseElementCount (M_CPD const *pPTokenRef, int xPTokenRef) {
    return rtPTOKEN_BaseElementCount (pPTokenRef->containerHandle (), pPTokenRef->OutboundPOP (xPTokenRef));
}

PublicFnDef unsigned int rtPTOKEN_BaseElementCount (VContainerHandle const *pPTokenRef, M_POP const *pPTokenPOP) {
    BaseElementCount++;

    VContainerHandle::Reference pHandle; rtPTOKEN_Type *pCC; RTYPE_Type xRT;
    M_CPreamble const *pCP = 0;
    if (pPTokenPOP) {
	pCP = pPTokenRef->GetContainerAddress (pPTokenPOP);
	if (pCP) {
	    xRT = (RTYPE_Type)(M_CPreamble_RType (pCP));
	    pCC = M_CPreamble_ContainerBaseAsType (pCP, rtPTOKEN_Type);
	}
	else {
	    pHandle.setTo (pPTokenRef->GetContainerHandle (pPTokenPOP));
	    pPTokenRef = pHandle;
	    xRT = pPTokenRef->RType ();
	    pCC = 0;
	}
    }
    else {
	xRT = pPTokenRef->RType ();
	pCC = 0;
    }

    RTYPE_MustBeA ("rtPTOKEN_BaseElementCount", xRT, RTYPE_C_PToken);

    return pCC
	? rtPTOKEN_PT_BaseElementCount (pCC)
	: static_cast<rtPTOKEN_Handle const*>(pPTokenRef)->cardinality ();
}


/*---------------------------------------------------------------------------
 *****  Routine to return true if the P-Token represents the current postional
 *****  state for the base object.
 *
 *  Arguments:
 *	p/xPTokenRef		- a reference to a POP for the P-Token whose
 *				  base element count is desired.
 *
 *  Returns:
 *	true if the P-Token represents the current postional state for its
 *	base object.  false otherwise.
 *
 *****/
bool VContainerHandle::isTerminal (M_POP const *pPTokenPOP) {
    IsCurrentCount++;

    VContainerHandle::Reference pHandle; rtPTOKEN_Type *pCC; RTYPE_Type xRT;
    M_CPreamble const *pCP = 0;
    if (pPTokenPOP) {
	pCP = GetContainerAddress (pPTokenPOP);
	if (pCP) {
	    xRT = (RTYPE_Type)(M_CPreamble_RType (pCP));
	    pCC = M_CPreamble_ContainerBaseAsType (pCP, rtPTOKEN_Type);
	}
	else {
	    pHandle.setTo (GetContainerHandle (pPTokenPOP, RTYPE_C_PToken));
	    xRT = pHandle->RType ();
	    pCC = 0;
	}
    }
    else {
	xRT = RType ();
	pCC = 0;
    }

    RTYPE_MustBeA ("VContainerHandle::isTerminal", xRT, RTYPE_C_PToken);

    unsigned int xTT = pCC
	? rtPTOKEN_PT_TokenType (pCC)
	: static_cast<rtPTOKEN_Handle*>(pHandle ? pHandle.referent () : this)->type ();

    if (rtPTOKEN_TT_Cartesian != xTT)
	return rtPTOKEN_TT_Base == xTT;

    if (pHandle.isNil ())
	pHandle.setTo (pPTokenPOP ? GetContainerHandle (pPTokenPOP, RTYPE_C_PToken) : this);

    return static_cast<rtPTOKEN_Handle*>(pHandle.referent ())->productIsTerminal ();
}

/*---------------------------------------------------------------------------
 *****  Macro to determine if the P-Token represents the current postional
 *****  state for the base object.
 *
 *  Arguments:
 *	pPTokenRef/Index	- a reference to a POP for the P-Token whose
 *				  base element count is desired.
 *	rpResult		- a standard cpd for the ptoken.
 *
 *  Returns:
 *	true if the referenced p-token isn't current, false otherwise.
 *
 *	in 'rpResult:
 *	If the ptoken is current the value: NilOf (M_CPD*) is returned.
 *	If the ptoken isn't current a handle for the PToken is returned.
 *
 *****/
bool VContainerHandle::isntTerminal (M_POP const *pPTokenPOP, rtPTOKEN_Handle::Reference &rpResult) {
    IsntCurrentCount++;

    VContainerHandle::Reference pHandle; rtPTOKEN_Type *pCC; RTYPE_Type xRT; unsigned int xSpace, xCntnr;
    M_CPreamble const *pCP = 0;
    if (pPTokenPOP) {
	pCP = GetContainerAddress (pPTokenPOP);
	if (pCP) {
	    xRT = (RTYPE_Type)(M_CPreamble_RType (pCP));
	    M_POP const *pPOP = &M_CPreamble_POP (pCP);
	    xSpace = M_POP_ObjectSpace (pPOP);
	    xCntnr = M_POP_ContainerIndex (pPOP);
	    pCC = M_CPreamble_ContainerBaseAsType (pCP, rtPTOKEN_Type);
	}
	else {
	    pHandle.setTo (GetContainerHandle (pPTokenPOP));
	    xRT = pHandle->RType ();
	    xSpace = pHandle->spaceIndex ();
	    xCntnr = pHandle->containerIndexNC ();
	    pCC = 0;
	}
    }
    else {
	xRT = RType ();
	xSpace = spaceIndex ();
	xCntnr = containerIndexNC ();
	pCC = 0;
    }

    RTYPE_MustBeA (
	UTIL_FormatMessage ("VContainerHandle::isntTerminal [%d:%d]", xSpace, xCntnr), xRT, RTYPE_C_PToken
    );

    unsigned int xTT = pCC
	? rtPTOKEN_PT_TokenType (pCC)
	: static_cast<rtPTOKEN_Handle*>(pHandle ? pHandle.referent () : this)->type ();

    switch (xTT) {
    case rtPTOKEN_TT_Shift:
	rpResult.setTo (
	    static_cast<rtPTOKEN_Handle*>(
		pHandle ? pHandle.referent () : pPTokenPOP ? GetContainerHandle (pPTokenPOP, RTYPE_C_PToken) : this
	    )
	);
	break;
    case rtPTOKEN_TT_Cartesian:
	rpResult.setTo (
	    static_cast<rtPTOKEN_Handle*>(
		pHandle ? pHandle.referent () : pPTokenPOP ? GetContainerHandle (pPTokenPOP, RTYPE_C_PToken) : this
	    )
	);
	if (rpResult->productIsTerminal ())
	    rpResult.clear ();
	break;
    default:
	rpResult.clear ();
	break;
    }

    return rpResult.isntNil ();
}

bool VContainerHandle::isntTerminal (M_POP const *pPTokenPOP, rtPTOKEN_CType *&rpResult) {
    rtPTOKEN_Handle::Reference pPToken;
    if (isntTerminal (pPTokenPOP, pPToken)) {
	rpResult = pPToken->getAdjustments ();
	return true;
    }
    return false;
}


/*---------------------------------------------------------------------------
 *****  Routine to return a CPD for the base P-Token in a P-Token chain.
 *
 *  Arguments:
 *	pPTokenRef/Index	- a CPD/Index reference to a POP for the
 *				  P-Token whose base P-Token is desired.
 *
 *  Returns:
 *	A handle for the base P-Token in the chain.
 *
 *****/
PublicFnDef rtPTOKEN_Handle *rtPTOKEN_BasePToken (M_CPD *pPTokenRef, int xPTokenRef) {
    BasePTokenCount++;

    rtPTOKEN_Handle *pResult;
    if (xPTokenRef >= 0)
	pResult = static_cast<rtPTOKEN_Handle*>(pPTokenRef->GetContainerHandle (xPTokenRef, RTYPE_C_PToken));
    else {
	RTYPE_MustBeA ("rtPTOKEN_BasePToken", pPTokenRef->RType (), RTYPE_C_PToken);
	pResult = static_cast<rtPTOKEN_Handle*>(pPTokenRef->containerHandle ());
    }
    return pResult->basePToken ();
}

rtPTOKEN_Handle *rtPTOKEN_Handle::basePToken () {
    rtPTOKEN_Handle *pResult = this;
    while (pResult->isntTerminal ()) {
	rtPTOKEN_Handle *pPrior = pResult;
	pResult = pPrior->nextPToken ();
	pPrior->discard ();
    }
    return pResult;
}


bool rtPTOKEN_Handle::tailAdjustmentsOnly (unsigned int xEnd) const {
    bool result = true;
    unsigned int alterationCount = sdCount ();
    rtPTOKEN_SDType *changeDesc = sdArray ();

    while (alterationCount--) {
	if (xEnd > rtPTOKEN_SD_Origin (changeDesc) ||
	    xEnd > rtPTOKEN_SD_Origin (changeDesc) +
	    		  rtPTOKEN_SD_Shift (changeDesc))
	{
	    result = false;
	    break;
	}
	changeDesc++;
    }

    return result;
}

bool rtPTOKEN_Handle::tailAdjustmentsOnly () {
    unsigned int const xEnd = cardinality ();

    rtPTOKEN_Handle::Reference pCurrentPToken (this);

    while (pCurrentPToken->isntTerminal ()) {
	if (!pCurrentPToken->tailAdjustmentsOnly (xEnd))
	    return false;
	pCurrentPToken.setTo (pCurrentPToken->nextPToken ());
    }

    return true;
}


/************************************
 *****  Instantiation Routines  *****
 ************************************/

void rtPTOKEN_Handle::deleteThis () {
    if (onDeleteThis ())
	delete this;
}

/********************************
 *  External Creation Routines  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new 'Base' P-Token for a new INDEPENDENT vector
 *****  or u-vector.
 *
 *  Arguments:
 *	pContainerSpace		- the address of the ASD in which to create
 *				  this p-token.
 *	nelements		- the number of elements in the positional
 *				  structure described by this P-Token.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
rtPTOKEN_Handle::rtPTOKEN_Handle (
    M_ASD *pContainerSpace, unsigned int nelements
) : BaseClass (pContainerSpace, RTYPE_C_PToken), m_iCardinality (nelements), m_bIndependent (true) {
    NewBasePTokenCount++;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new 'Cartesian' P-Token for an INDEPENDENT vector
 *****  or u-vector.
 *
 *  Arguments:
 *	pRowPToken		- a handle for the row ptoken of the product.
 *	pColPToken		- a handle for the column ptoken of the product.
 *
 *****/
rtPTOKEN_Handle::rtPTOKEN_Handle (
    M_ASD *pContainerSpace, rtPTOKEN_Handle *pRowPToken, rtPTOKEN_Handle *pColPToken
) : BaseClass (pContainerSpace, RTYPE_C_PToken, rtPTOKEN_PT_SizeofCartesianPT), m_iCardinality (
    pRowPToken->cardinality () * pColPToken->cardinality ()
), m_bIndependent (true) {
    NewCartesianPTokenCount++;

    rtPTOKEN_Type *pCC = typecastContent ();
    rtPTOKEN_PT_Closed (pCC) = false;

    constructReference (&rtPTOKEN_PT_nextGeneration (pCC));

    constructReference (&rtPTOKEN_PT_RowPToken (pCC));
    StoreReference (&rtPTOKEN_PT_RowPToken (pCC), pRowPToken);

    constructReference (&rtPTOKEN_PT_ColPToken (pCC));
    StoreReference (&rtPTOKEN_PT_ColPToken (pCC), pColPToken);

    rtPTOKEN_PT_TokenType	(pCC) = rtPTOKEN_TT_Cartesian;
    rtPTOKEN_PT_Independent	(pCC) = true;
    rtPTOKEN_PT_Count		(pCC) = 0;
    rtPTOKEN_PT_BaseElementCount(pCC) = pRowPToken->cardinality () * pColPToken->cardinality ();

    rtPTOKEN_PT_Closed		(pCC) = true;
}

void rtPTOKEN_Handle::createContainer () {
    if (hasNoContainer ()) {
	CreateContainer (rtPTOKEN_PT_SizeofBasePToken);

	rtPTOKEN_Type *pCC = typecastContent ();
	rtPTOKEN_PT_Closed (pCC) = false;

	constructReference (&rtPTOKEN_PT_nextGeneration (pCC));

	rtPTOKEN_PT_TokenType       (pCC) = rtPTOKEN_TT_Base;
	rtPTOKEN_PT_Independent     (pCC) = m_bIndependent;
	rtPTOKEN_PT_Count           (pCC) = 0;
	rtPTOKEN_PT_BaseElementCount(pCC) = m_iCardinality;

	rtPTOKEN_PT_Closed (pCC) = true;
    }
}


/************************************************************
 *  Constructor Allocator Definitions and Utility Routines  *
 ************************************************************/

/*****  Free Lists  *****/
PrivateVarDef V::VAllocatorFreeList SDConstructorFreeList (sizeof (rtPTOKEN_SDCType));

/*****  Allocators  *****/

/*---------------------------------------------------------------------------
 *****  Internal routine to allocate a partially initialized SD constructor.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	The address of the SD constructor allocated.
 *
 *****/
PrivateFnDef rtPTOKEN_SDCType* AllocateSDConstructor () {
    rtPTOKEN_SDCType *result = (rtPTOKEN_SDCType*)SDConstructorFreeList.allocate ();

    rtPTOKEN_SDC_PrevSDC (result) =
    rtPTOKEN_SDC_NextSDC (result) = NilOf (rtPTOKEN_SDCType*);

    if (TracingSDCAllocator)
        IO_printf ("...AllocateSDConstructor: SDC:%08X\n", result);

    return result;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to allocate a partially initialized P-Token 
 *****  constructor.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	The address of the P-Token constructor allocated.
 *
 *****/
DEFINE_CONCRETE_RTT (rtPTOKEN_CType);
void rtPTOKEN_CType::deleteThis () {
    delete this;
}

rtPTOKEN_CType::rtPTOKEN_CType (rtPTOKEN_Handle *pNextGeneration)
: m_pNextGeneration	(pNextGeneration)
, m_iCardinality	(pNextGeneration->cardinality ())
, m_pRowPToken		(0)
, m_pColPToken		(0)
, m_pSDChainHead	(0)
, m_pSDChainTail	(0)
, m_cAlterations	(0)
, m_xOrder		(rtPTOKEN_Order_Undefined)
{
    if (TracingPTCAllocator)
        IO_printf ("...AllocatePTokenConstructor: PTC:%08X\n", this);
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new 'Shift' P-Token constructor.
 *
 *  Arguments:
 *	p/xCurrentPTokenRef	- a reference to a POP for the P-Token
 *				  currently associated with the positionally
 *				  accessible object whose editing history is
 *				  to be recorded in this P-Token.
 *
 *  Returns:
 *	A pointer to the P-Token constructor created.
 *
 *****/
PublicFnDef rtPTOKEN_CType *rtPTOKEN_NewShiftPTConstructor (
    M_CPD *pCurrentPTokenRef, int xCurrentPTokenRef
) {
    NewShiftPTConstructorCount++;

    rtPTOKEN_Handle::Reference pPToken;
    if (xCurrentPTokenRef >= 0) pPToken.setTo (
	static_cast<rtPTOKEN_Handle*>(pCurrentPTokenRef->GetContainerHandle (xCurrentPTokenRef, RTYPE_C_PToken))
    );
    else {
	RTYPE_MustBeA (
	    "rtPTOKEN_NewShiftPTConstructor", pCurrentPTokenRef->RType (), RTYPE_C_PToken
	);
	pPToken.setTo (static_cast<rtPTOKEN_Handle*>(pCurrentPTokenRef->containerHandle ()));
    }
    return pPToken->makeAdjustments ();
}

rtPTOKEN_CType *rtPTOKEN_Handle::makeAdjustments () {
    if (typeIsShift ()) ERR_SignalFault (
	EC__InternalInconsistency, "_NewShiftPTConstructor: Attempt to attach to a shift ptoken"
    );
    return new rtPTOKEN_CType (this);
}


/*---------------------------------------------------------------------------
 ***** Internal Routine to determine if current edit instruction lies adjacent
 ***** to the last. If so, it combines the two.
 *
 *  Arguments:
 *	lastInstruction		- A pointer to the SDC that contains the last
 *				  instruction.
 *	origin			- The origin component of the current
 *				  instruction.
 *	shift			- The shift component of the current
 *				  instruction.
 *
 *  Returns:
 *	true if the instructions are combinable, false otherwise.
 *
 *****/
PrivateFnDef int HandleAdjacency (
    rtPTOKEN_SDCType* lastInstruction, int origin, int shift
) {
    int
	lookingBack = false,
	origin2,
	shift2;
    rtPTOKEN_SDCType* next = lastInstruction;

    if ((shift < 0 && rtPTOKEN_SDC_Shift (lastInstruction) > 0 &&
        origin < rtPTOKEN_SDC_Origin (lastInstruction)) ||
	shift > 0 && rtPTOKEN_SDC_Shift (lastInstruction) < 0)
    {
	lookingBack = true;
        lastInstruction = rtPTOKEN_SDC_PrevSDC (lastInstruction);
    }

    if (IsNil (lastInstruction))
        return false;

    shift2 = rtPTOKEN_SDC_Shift (lastInstruction);
    origin2 = rtPTOKEN_SDC_Origin (lastInstruction);

    if (!(shift < 0 && shift2 < 0) &&
	!(shift > 0 && shift2 > 0))
        return false;

    if (shift < 0 &&
       (origin2 + shift2 == origin ||
        origin2 + shift2 == origin + shift))
    {
	if (lookingBack)
	    rtPTOKEN_SDC_Origin (next) += shift;


	rtPTOKEN_SDC_Origin (lastInstruction) = origin - shift2;
	rtPTOKEN_SDC_Shift (lastInstruction) += shift;
	return true;
    }

    if (shift > 0 &&
       (origin2 <= origin &&
           origin2 + shift2 >= origin))
    {
	if (lookingBack)
	    rtPTOKEN_SDC_Origin (next) += shift;

	rtPTOKEN_SDC_Shift (lastInstruction) += shift;
	return true;
    }


    return false;
}


/*---------------------------------------------------------------------------
 *****  Internal Routine to insert a cell in a P-Token Constructor's SDC chain
 *****  and in the originArray.
 *
 *  Arguments:
 *	this			-  a pointer to a P-Token Constructor
 *	origin			-  The origin value to be inserted.
 *	shift			-  The shift value to be inserted.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
void rtPTOKEN_CType::InsertSDC (int origin, int shift) {
    rtPTOKEN_SDCType* newSDC = AllocateSDConstructor();

    m_cAlterations++;

/****  Link into SDC chain  ****/
        rtPTOKEN_SDC_NextSDC (newSDC) = NilOf (rtPTOKEN_SDCType*);
	rtPTOKEN_SDC_PrevSDC (newSDC) = m_pSDChainTail;
        rtPTOKEN_SDC_NextSDC (m_pSDChainTail) = newSDC;
	m_pSDChainTail = newSDC;

/****  Insert values in newSDC  ****/
    rtPTOKEN_SDC_Shift (newSDC) = shift;
    rtPTOKEN_SDC_Origin (newSDC) = origin;
}


/******************************************************
 *****  P-Token Constructor SDC Append Routine  *****
 ******************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to add a shift description constructor to a P-Token constructor
 *
 *  Arguments:
 *	this			-  The P-Token constructor to be modified.
 *	origin			-  The origin of the tail move according to the
 *				   current state of the base object being
 *				   edited.
 *	shift			-  The amount of tail shift. The sign
 *				   indicates direction of shift.
 *
 *  Returns:
 *	The modified constructor.
 *
 *  Notes:
 *	The origin and shift arguments should be the same as those passed to
 *   M_ShiftContainerTail. This routine expects editing instructions to be
 *   collapsed to a minimal set and to be presented for insertion in sorted
 *   order (ascending or descending) with origin as the key.
 *
 *  WARNING:
 *	Because an edit can change the reference frame, calls to this routine
 *   are expected to occur in the SAME order as the actual calls to
 *   M_ShiftContainerTail. Results cannot be guaranteed otherwise.
 *
 *****/


/*---------------------------------------------------------------------
 *	TABLE 1  -  P-Token Adjustments
 *---------------------------------------------------------------------

    where:
	r represents the origin
	s represents the adjustment shift amount
	  (s is positive for inserts and negative for deletes)


   Insert                              Delete

   --------------------------          ---------------------------
   >>>> | >>>>>>>>>>>>>>>>>>>          >>>> | <<<<<<<< | >>>>>>>>>
   --------------------------          ---------------------------
	^                                   ^          ^
	|                                   | <------> |
	r                                   r+s   s    r


	    ||                                   ||
	    VV                                   VV

  ---------------------------          ---------------------------
  >>>> |         | >>>>>>>>>>	       >>>> | >>>>>>>>>>>>>>>>>>>>
  ---------------------------          ---------------------------
       ^         ^                          ^
       | <-----> |                          |
       r    s    r+s                        r+s


*/


/*---------------------------------------------------------------------------
 * 	TABLE 2  -  Testing Consistency
 *---------------------------------------------------------------------------

    Determining if the adjustment is consistent.  Consistent means that
    it is not overlapping or out of order.

    When this table produces a true value the adjustment is inconsistent.


	|	Forward			|	Backward
----------------------------------------------------------------
II	|  r	       <  r + s		| r     >  r
	|   n+1            n   n     	|  n+1      n
----------------------------------------------------------------
ID	|  r    + s    <  r + s 	| r     >  r
	|   n+1    n+1     n   n	|  n+1      n
----------------------------------------------------------------
DD	|  r    + s    <  r + s		| r     >  r + s
	|   n+1    n+1     n   n	|  n+1      n   n
----------------------------------------------------------------
DI	|  r           <  r + s		| r     >  r + s
	|   n+1            n   n        |  n+1      n   n
----------------------------------------------------------------


 Where:

     r		is the previous adjustment origin
      n

     s		is the previous adjustment shift amount
      n

     r		is the new adjustment origin
      n+1

     s		is the new adjustment shift amount
      n+1

     II		means that the nth and n+1th adjustments are inserts
		(that both s's are positive)

     ID		means that the nth adjustment is an insert and the
		n+1th adjustment is a delete

     DD		means that the nth and n+1th adjustments are deletes

     DI		means that the nth adjustment is a delete and the
		n+1th adjustment is an insert


    The test for inconsistency which correspounds to this table is:

	((forward)
	    ?  ((newIsDelete) ? (newAdjustedOrigin < oldAdjustedOrigin)
			      : (newOrigin < oldAdjustedOrigin))

	    :  ((oldIsDelete) ? (newOrigin > oldAdjustedOrigin)
			      : (newOrigin > oldOrigin)))

    Where:
	newOrigin = r				oldOrigin = r
	             n+1             		             n

	newAdjustedOrigin = r    +  s		oldAdjustedOrigin = r  + s
		             n+1     n+1                             n    n

	newIsDelete = s    < 0			oldIsDelete = s  < 0
                       n+1                                     n

*/


/*---------------------------------------------------------------------------
 * 	TABLE 3  -  Determining Direction
 *---------------------------------------------------------------------------

    The following table determines the direction that adjustments are
    going, from two consecutive adjustments.

    The 'Direction' column is determined by the following formula:

	if (newOrigin >= (oldOrigin + oldAdjustedOrigin))
	    then Forward;
	else Backward;

------------------------------------------------------------------------------
    old	    old	    new	    new	    |   Direction	Consistent    Merge
    origin  shift   origin  shift   |
------------------------------------------------------------------------------
    8	    1	    7	    -1	    |	    B		    Y		N
    8	    1	    7	     1	    |	    B		    Y		N
    8	    1	    8	    -1	    |	    B		    Y		N
    8	    1	    8	     1	    |	    B		    Y		Y
------------------------------------------------------------------------------
    8	    1	    9	    -1	    |	    F		    N		-
    8	    1	    9	     1	    |	    F		    Y		Y
    8	    1	    10	    -1	    |	    F		    Y		N
    8	    1	    10	     1	    |	    F		    Y		N
------------------------------------------------------------------------------
    8	    -1	    6	    -1	    |	    B		    Y		N
    8	    -1	    6	     1	    |	    B		    Y		N
    8	    -1	    7	    -1	    |	    F *		    Y		Y
    8	    -1	    7	     1	    |	    F **	    Y		N
------------------------------------------------------------------------------
    8	    -1	    8	    -1	    |	    F		    Y		Y
    8	    -1	    8	     1	    |	    F		    Y		N
    8	    -1	    9	    -1	    |	    F		    Y		N
    8	    -1	    9	     1	    |	    F		    Y		N
------------------------------------------------------------------------------
*   the formula in this case produces the wrong direction, it should be
    backwards.

**  although the formula produced Forward, it is imposible to tell from these
    two adjustments the correct direction.  It could be either.

These two exceptions need to be special cased in the code.

*/


rtPTOKEN_CType *rtPTOKEN_CType::AppendAdjustment (int origin, int shift) {

/*****  Macros to make life easier ... *****/
#define newOrigin	    origin
#define newShift	    shift
#define newAdjustedOrigin   (newOrigin + newShift)
#define newIsDelete	    (newShift < 0)

#define oldOrigin	    (rtPTOKEN_SDC_Origin (chaint))
#define oldShift	    (rtPTOKEN_SDC_Shift (chaint))
#define oldAdjustedOrigin   (oldOrigin + oldShift)
#define oldIsDelete	    (oldShift < 0)

#define forward	 (!OrderIsBackward ())
/* note that 'forward' is defined to be 'rtPTOKEN_Order_Forward' and
   'rtPTOKEN_Order_Undefined' this is for the one exception case whose
   order cannot be determined yet.
*/


/*
 *---------------------------------------------------------------------------
 *  Body of 'rtPTOKEN_AppendAdjustment'
 *---------------------------------------------------------------------------
 */

/*****  Increment the usage count ... *****/
    AppendAdjustmentCount++;


/*****  If the shift is zero, return ... *****/
    if (newShift == 0)
	/***  Erroneous usage, but can be safely ignored  ***/
	return this;


/*****  Check if the adjustment is Out of Bounds ... *****/
    if (newOrigin < 0 || newAdjustedOrigin < 0 || (size_t)newOrigin > m_iCardinality
    ) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "_AppendAdjustment: Instruction Out of Bounds\n*\t\t\
Origin:%d, Shift:%d, ElementCount:%d\n",
	    origin, shift, m_iCardinality
	)
    );

/*****  If this is the first adjustment ... *****/
    rtPTOKEN_SDCType *chainh = m_pSDChainHead;
    rtPTOKEN_SDCType *chaint = m_pSDChainTail;
    if (m_cAlterations == 0) {
	if (IsntNil (chainh) || IsntNil (chaint)) ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"_AppendAdjustment: ChainHead:%08X, ChainTail:%08X, AltCount:%d\n",
		chainh, chaint, m_cAlterations
	    )
	);

	m_pSDChainHead = m_pSDChainTail = AllocateSDConstructor();

	rtPTOKEN_SDC_Origin (m_pSDChainTail) = origin;
	rtPTOKEN_SDC_Shift (m_pSDChainTail) = shift;
	rtPTOKEN_SDC_CurrentOrigin (m_pSDChainTail) =
	    (shift > 0 ? origin : origin + shift);
	if (TracingSDCInsertion) IO_printf (
	    "origin: %d, shift: %d, order: %c, next: %08X, prev: %08X\n",
	    rtPTOKEN_SDC_Origin (m_pSDChainTail),
	    rtPTOKEN_SDC_Shift (m_pSDChainTail),
	    'U',
	    rtPTOKEN_SDC_NextSDC (m_pSDChainTail),
	    rtPTOKEN_SDC_PrevSDC (m_pSDChainTail)
	);

	m_iCardinality += shift;
	m_cAlterations++;
	return this;
    }


/*****  Determine if instructions are being presented in forward or
 *****  backward order  (see TABLE 3) ...
 *****/
    if (OrderIsUndefined ()) {
	if (newOrigin >= oldAdjustedOrigin)
	     m_xOrder = rtPTOKEN_Order_Forward;
	else m_xOrder = rtPTOKEN_Order_Backward;

	/*** special case the two exception cases ... ***/
	if (oldIsDelete && newIsDelete && newOrigin == oldAdjustedOrigin)
	    m_xOrder = rtPTOKEN_Order_Backward;

	if (oldIsDelete && !newIsDelete && newOrigin == oldAdjustedOrigin)
	    m_xOrder = rtPTOKEN_Order_Undefined;
    }

/*****  Check that constraints are being met,
 *****  that it is consistent  (see TABLE 2) ...
 *****/
    if ((forward)
	    ? ((newIsDelete) ? (newAdjustedOrigin < oldAdjustedOrigin)
			     : (newOrigin < oldAdjustedOrigin))
	    : ((oldIsDelete) ? (newOrigin > oldAdjustedOrigin)
			     : (newOrigin > oldOrigin)))
    {
	char order = OrderIsForward () ? '+' : OrderIsBackward () ? '-' : 'U';

        ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
	    "_AppendAdjustment: Instruction Overlapping or Out of Order\n*\t\t\
Origin:%d, Shift:%d, PrevOrigin:%d, PrevShift:%d, Order(%c)\n",
		origin, shift,
		rtPTOKEN_SDC_Origin (chaint),
		rtPTOKEN_SDC_Shift (chaint),
		order
	    )
	);
    }

/*****  Check for adjacency - whether the two adjustments can be merged
 *****  into one ...
 *****/
    if (!HandleAdjacency (chaint, origin, shift))
    /*****  ... if not adjacent, insert the new adjustment ... *****/
	InsertSDC (origin, shift);

    if (TracingSDCInsertion) {
	char order = OrderIsForward () ? '+' : OrderIsBackward () ? '-' : 'U';

	IO_printf (
	    "origin: %d, shift: %d, order: %c, next: %08X, prev: %08X\n",
	    rtPTOKEN_SDC_Origin (m_pSDChainTail),
	    rtPTOKEN_SDC_Shift (m_pSDChainTail),
	    order,
	    rtPTOKEN_SDC_NextSDC (m_pSDChainTail),
	    rtPTOKEN_SDC_PrevSDC (m_pSDChainTail)
	);
    }

    m_iCardinality += newShift;

    return this;

/***** undef the macros ... *****/
#undef newOrigin
#undef newShift
#undef newAdjustedOrigin
#undef newIsDelete
#undef oldOrigin
#undef oldShift
#undef oldAdjustedOrigin
#undef oldIsDelete
#undef forward
}


/**************************************************
 *****  P-Token Constructor Deletion Routine  *****
 **************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to discard a P-Token constructor.
 *
 *  Argument:
 *	this			- the address of a pointer to the P-Token
 *				  constructor to be discarded.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
rtPTOKEN_CType::~rtPTOKEN_CType () {
    DiscardConstructorCount++;

/*****  Free the SD constructor chain  *****/
    rtPTOKEN_SDCType *sdc = m_pSDChainHead;
    while (sdc) {
	rtPTOKEN_SDCType *nextsdc = rtPTOKEN_SDC_NextSDC (sdc);

	if (TracingSDCAllocator)
	    IO_printf ("...~rtPTOKEN_CType: SDC:%08X freed.\n", sdc);

	SDConstructorFreeList.deallocate (sdc);
	sdc = nextsdc;
    }

/*****  Free the P-Token constructor  *****/
    if (TracingPTCAllocator) display (
	"...~rtPTOKEN_CType: PTC:%08X freed.\n", this
    );
}


/******************************************************************
 *****  P-Token-Constructor <-> P-Token Conversion Routines  ******
 ******************************************************************/

/*****************************
 *  Standardization Utility  *
 *****************************/

void rtPTOKEN_CType::ReverseOrder () {
    rtPTOKEN_SDCType
	*chainh	 = m_pSDChainHead,
	*chaint	 = m_pSDChainTail,
	*nextSDC = chainh;
    int adjustment = 0;

    for (unsigned int x = 0; x < m_cAlterations; x++) {
        rtPTOKEN_SDC_Origin (nextSDC) -= adjustment;
	rtPTOKEN_SDCType *tempSDC = rtPTOKEN_SDC_NextSDC (nextSDC);
	rtPTOKEN_SDC_NextSDC (nextSDC) = rtPTOKEN_SDC_PrevSDC (nextSDC);
	rtPTOKEN_SDC_PrevSDC (nextSDC) = tempSDC;
        adjustment += rtPTOKEN_SDC_Shift (nextSDC);
	nextSDC = rtPTOKEN_SDC_PrevSDC (nextSDC);
    }

    m_pSDChainHead = chaint;
    m_pSDChainTail = chainh;
}


/***********************************************
 *  P-Token-Constructor To P-Token Conversion  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a ptoken constructor to a ptoken.
 *
 *  Argument:
 *	this			- the address of a closed ptoken constructor
 *				  to be converted.
 *
 *  Returns:
 *	The address of a handle for the ptoken created.
 *
 *****/
rtPTOKEN_Handle *rtPTOKEN_CType::ToPToken () {
    BasicToPTokenCount++;

/*****  Validate the integrity of the this...  *****/
    m_pNextGeneration->CheckConsistency ();
    if (m_pNextGeneration->typeIsShift ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_CType::ToPToken: Attempt to attach to a shift p-token"
    );

/*****  Validate the new row and col cpd's for cartesian ptokens ... *****/
    if (m_pNextGeneration->typeIsCartesian () && (m_pRowPToken.isNil () || m_pColPToken.isNil ())) ERR_SignalFault (
	EC__InternalInconsistency, "rtPTOKEN_CType::ToPToken: Nil Cartesian PTokens"
    );

/*****  If no alterations were made, ...  *****/
    if (m_cAlterations == 0) {
    /*****  ... return the current generation:  *****/
	/*** If it is a cartesian ptoken, update the pop's ... ***/
	rtPTOKEN_Handle *result = m_pNextGeneration;
	if (result->typeIsCartesian ()) {
	    result->setProductComponentsTo (m_pRowPToken, m_pColPToken);
	}

    //  ... and return the constructed p-token
	result->retain ();
	discard ();
	result->untain ();
	return result;
    }

    if (m_cAlterations >= (unsigned int)(1 << 24)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Requested alteration count %u exceeds maximum %u [%u:%u]",
	    m_cAlterations, (1 << 24) - 1,
	    m_pNextGeneration->spaceIndex (),
	    m_pNextGeneration->containerIndex ()
	)
    );

/*****  ...  otherwise, manufacture a new P-Token:  *****/
    rtPTOKEN_Handle *result = m_pNextGeneration->typeIsCartesian ()
	? new rtPTOKEN_Handle (
	    m_pNextGeneration->Space (), m_pRowPToken, m_pColPToken
	) : new rtPTOKEN_Handle (
	    m_pNextGeneration->Space (), m_iCardinality
	);

    result->setIsIndependentTo (m_pNextGeneration->isIndependent ());

/*---------------------------------------------------------------------------
 * The following function was inserted into the routine originally to keep
 * ptoken chains in one space, basically for reasons of locality of reference.
 * One consequence of this decision has been the possibility of creating
 * dangling references when private (single-space) saves are requested. This
 * consequence has been felt on numerous occasions by clients who have had
 * their private saves disallowed because the "local update would threaten
 * network integrity".
 * A second, positive consequence is not so easily noticed. If a ptoken from
 * another space is edited and a new terminator is created for the end of the
 * chain, and if that ptoken serves as a reference ptoken in a property in the
 * users space, then after a save (if ptokens were not forwarded), the
 * property would contain the new ptoken, but the referenced object would
 * remain unchanged (would continue to reference the ptoken as it existed
 * before editing). The result would be that upon exiting vision and
 * reentering, any access of the affected property would cause a referential
 * inconsistency.
 * The following vision code would produce such a situation:

	!x <- ClassCreatedInAnotherSpace new;
	?g
	?w  #	With VisionAdm NOT set.
	    Exit and re-enter vision ---
	x
	?g
 *---------------------------------------------------------------------------
 *  Call replaced in 7.x with code that creates the new p-token in the right
 *  space in the first place.
 *
    result->ForwardToSpace (m_pNextGeneration);
 *---------------------------------------------------------------------------
 */

/*****  Initialize the SD array  *****/
    if (OrderIsForward ())
        ReverseOrder ();

/*****  Modify the extant ptoken  *****/
    m_pNextGeneration->convertToShift (result, m_pSDChainHead, m_pSDChainTail, m_cAlterations);

    discard ();

    return result;
}

void rtPTOKEN_Handle::convertToShift (
    rtPTOKEN_Handle *pNextGeneration, rtPTOKEN_SDCType const *pSDHead, rtPTOKEN_SDCType const *pSDTail, unsigned int sSDList
) {
    if (hasAContainer ())
	EnableModifications ();
    else
	createContainer ();
    clearIsClosed ();
    StoreReference (nextGenerationPOP (), pNextGeneration);

/*****  If this is a cartesian ptoken, clear its pops and take its larger size into account...  *****/
    unsigned int xSize = sizeof(rtPTOKEN_SDType) * sSDList;
    if (typeIsCartesian ()) {
	ClearReference (rowPTokenPOP ());
	ClearReference (colPTokenPOP ());

	xSize -= sizeof (rtPTOKEN_CartesianType);
    }
    GrowContainer (xSize);

    if (pSDHead) {
	rtPTOKEN_SDType *dest = sdArray ();
	unsigned int x = 0;
	while (pSDHead != pSDTail) {
	    if (TracingConversion) IO_printf (
	    "...InitializePToken: SDiter:%d srcp:%08X dest:%08X srcl:%08X\n",
	        x++, pSDHead, dest, pSDTail
	    );
	    *dest++ = rtPTOKEN_SDC_SD (pSDHead);
	    pSDHead = rtPTOKEN_SDC_NextSDC (pSDHead);
	}
        if (TracingConversion) IO_printf (
	    "...InitializePToken: SDiter:%d srcp:%08X dest:%08X srcl:%08X\n",
	    x, pSDHead, dest, pSDTail
	);
	*dest = rtPTOKEN_SDC_SD (pSDHead);
    }

    rtPTOKEN_Type *pCC = typecastContent ();
    rtPTOKEN_PT_TokenType	(pCC) = rtPTOKEN_TT_Shift;
    rtPTOKEN_PT_Count		(pCC) = sSDList;
    rtPTOKEN_PT_Closed		(pCC) = true;
}


/***********************************************
 *  P-Token to P-Token-Constructor Conversion  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a ptoken into a ptoken constructor.
 *
 *  Returns:
 *	The address of a ptoken constructor.
 *
 *****/
rtPTOKEN_CType *rtPTOKEN_Handle::asConstructor () {
    rtPTOKEN_SDCType *chainHead, *chainTail, *newSDC;
    int adjustment = 0, shift;

    ToConstructorCount++;

/*****  The ptoken must be a shift ptoken  *****/
    if (typeIsntShift ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_ToConstructor:  Must be a shift p-token"
    );

/*****  ...initialize traversal pointers and SD count...  *****/
    unsigned int alt = sdCount ();

    rtPTOKEN_CType *ptokenc = new rtPTOKEN_CType (nextPToken ());

    rtPTOKEN_PTC_Order (ptokenc) = rtPTOKEN_Order_Backward;
    rtPTOKEN_PTC_AltCount (ptokenc) = alt;

/*** Force cartesian ptokens to update themselves ***/
    if (ptokenc->NextGeneration ()->typeIsCartesian ())
	ptokenc->NextGeneration ()->productIsTerminal ();

/*****  ...allocate the SD chain...  *****/
    rtPTOKEN_SDType *sdp = sdArray ();
    rtPTOKEN_SDType *sdl = sdp + alt;

    if (sdp < sdl) {
        rtPTOKEN_SDC_NextSDC (chainHead = newSDC = AllocateSDConstructor()) = 0;
        rtPTOKEN_SDC_PrevSDC (chainHead) = NilOf (rtPTOKEN_SDCType*);
	while (rtPTOKEN_SDC_SD (chainTail = newSDC) = *sdp++, sdp < sdl) {
	    rtPTOKEN_SDC_NextSDC (newSDC = AllocateSDConstructor ()) = 0;
	    rtPTOKEN_SDC_PrevSDC (newSDC) = chainTail;
	    rtPTOKEN_SDC_NextSDC (chainTail) = newSDC;
	}
	rtPTOKEN_PTC_ChainHead (ptokenc) = chainHead;
	rtPTOKEN_PTC_ChainTail (ptokenc) = chainTail;
    }
    unsigned int x;
    for (x = 0, newSDC = chainTail; x < alt; x++, newSDC = rtPTOKEN_SDC_PrevSDC (newSDC)) {
        rtPTOKEN_SDC_CurrentOrigin (newSDC) = rtPTOKEN_SDC_Origin (newSDC)
	    + adjustment
	    + (((shift = rtPTOKEN_SDC_Shift (newSDC)) < 0) ? shift : 0);
	adjustment += shift;
    }

/*****  ...and return the ptoken constructor.  *****/
    return ptokenc;
}


/****************************************
 *****  Chain Collapsing Utilities  *****
 ****************************************/

PrivateFnDef rtPTOKEN_SDCType *Delete (
    rtPTOKEN_CType *ptc, rtPTOKEN_SDCType *sdc, bool free
)
{
    rtPTOKEN_SDCType* prev = rtPTOKEN_SDC_PrevSDC (sdc);
    rtPTOKEN_SDCType* next = rtPTOKEN_SDC_NextSDC (sdc);
    if (TracingCollapse) IO_printf (">Delete\n");

    if (IsNil (prev))
        rtPTOKEN_PTC_ChainHead (ptc) = next;
    else
	rtPTOKEN_SDC_NextSDC (prev) = next;

    if (IsNil (next))
        rtPTOKEN_PTC_ChainTail (ptc) = prev;
    else
	rtPTOKEN_SDC_PrevSDC (next) = prev;

    if (free)
        SDConstructorFreeList.deallocate (sdc);

    if (TracingCollapse)
	IO_printf ("Delete<\n");

    return next;

}


PrivateFnDef rtPTOKEN_SDCType* InsertSDCfromPTC (
    rtPTOKEN_CType* ptc1, rtPTOKEN_SDCType* sdc1,
    rtPTOKEN_CType* ptc2, rtPTOKEN_SDCType* sdc2
)
{
    rtPTOKEN_SDCType* next = Delete (ptc2, sdc2, false);
    int shift;
    if (TracingCollapse) IO_printf (">InsertSDCfromPTC\n");

    if (IsNil (sdc1) && IsntNil (rtPTOKEN_PTC_ChainTail (ptc1)))
    {
        rtPTOKEN_SDC_NextSDC (rtPTOKEN_PTC_ChainTail (ptc1)) = sdc2;
	rtPTOKEN_SDC_PrevSDC (sdc2) = rtPTOKEN_PTC_ChainTail (ptc1);
	rtPTOKEN_SDC_NextSDC (sdc2) = NilOf (rtPTOKEN_SDCType*);
	rtPTOKEN_PTC_ChainTail (ptc1) = sdc2;
        if (TracingCollapse)
	    IO_printf ("InsertSDCfromPTC (IsNil (sdc1) &&)<\n");
	return next;
    }
    else if (IsNil (sdc1))
    {
	rtPTOKEN_SDC_PrevSDC (sdc2) =
	rtPTOKEN_SDC_NextSDC (sdc2) = NilOf (rtPTOKEN_SDCType*);
	rtPTOKEN_PTC_ChainTail (ptc1) =
	rtPTOKEN_PTC_ChainHead (ptc1) = sdc2;
        if (TracingCollapse) IO_printf ("InsertSDCfromPTC (IsNil (sdc1))<\n");
        return next;
    }

    if (IsntNil (rtPTOKEN_SDC_PrevSDC (sdc2) = rtPTOKEN_SDC_PrevSDC (sdc1)))
        rtPTOKEN_SDC_NextSDC (rtPTOKEN_SDC_PrevSDC (sdc2)) = sdc2;
    else
	rtPTOKEN_PTC_ChainHead (ptc1) = sdc2;
    rtPTOKEN_SDC_PrevSDC (sdc1) = sdc2;
    rtPTOKEN_SDC_NextSDC (sdc2) = sdc1;

    shift = ((shift = rtPTOKEN_SDC_Shift (sdc1)) > 0 ? shift : 0);
    rtPTOKEN_SDC_Origin (sdc2) += rtPTOKEN_SDC_Origin (sdc1) -
				  (rtPTOKEN_SDC_CurrentOrigin (sdc1) + shift);
    if (TracingCollapse) IO_printf ("InsertSDCfromPTC<\n");
    return next;

}


PrivateFnDef int Compare (rtPTOKEN_SDCType* sdc1, rtPTOKEN_SDCType* sdc2) {
    int
	rangeTop1,
	rangeTop2,
	rangeBottom1,
	rangeBottom2,
	shift,
	shift2;

    if (TracingCollapse) IO_printf (">Compare\n");
    if (IsNil (sdc1) && IsNil (sdc2)) {
	if (TracingCollapse) IO_printf ("Compare=0(both nil)\n");
        return 0;
    }

    if (IsNil (sdc1)) {
	if (TracingCollapse) IO_printf ("Compare=-1(sdc1 nil)\n");
        return -1;
    }

    if (IsNil (sdc2)) {
	if (TracingCollapse) IO_printf ("Compare=1(sdc2 nil)\n");
        return 1;
    }

    rangeBottom1 = rtPTOKEN_SDC_CurrentOrigin (sdc1);
    rangeTop1 = rangeBottom1 + ((shift = rtPTOKEN_SDC_Shift (sdc1)) > 0 ? shift : 0);

    if ((shift2 = rtPTOKEN_SDC_Shift(sdc2)) > 0) {
        rangeBottom2 = rtPTOKEN_SDC_Origin (sdc2);
	rangeTop2 =  rtPTOKEN_SDC_Origin (sdc2) + rtPTOKEN_SDC_Shift(sdc2);
    }
    else {
        rangeTop2 = rtPTOKEN_SDC_Origin (sdc2);
	rangeBottom2 =  rtPTOKEN_SDC_Origin (sdc2) + rtPTOKEN_SDC_Shift(sdc2);
    }


    if (TracingCollapse) IO_printf (
	"Compare: t1=%d b1=%d s=%d t2=%d b2=%d s2=%d<\n",
	rangeTop1, rangeBottom1, shift, rangeTop2, rangeBottom2, shift2
    );

    if (rangeBottom1 > rangeTop2) {
	if (TracingCollapse) IO_printf ("Compare=1\n");
	return 1;
    }

    if (rangeBottom2 > rangeTop1) {
	if (TracingCollapse) IO_printf ("Compare=-1\n");
        return -1;
    }

    if (shift > 0 && shift2 > 0 && rangeBottom2 < rangeBottom1) {
	if (TracingCollapse) IO_printf ("Compare=1\n");
	return 1;
    }

    if (TracingCollapse) IO_printf ("Compare=0\n");
    return 0;
}


PrivateFnDef void ComputeNewCurrents (rtPTOKEN_CType* ptc) {
    if (TracingCollapse)
	IO_printf (">ComputeNewCurrents\n");

    int adjustment = 0;

    unsigned int x = 0;
    for (
	rtPTOKEN_SDCType* nextSDC = rtPTOKEN_PTC_ChainTail (ptc);
        IsntNil (nextSDC);
	x++, nextSDC = rtPTOKEN_SDC_PrevSDC (nextSDC)
    ) {
	int shift = rtPTOKEN_SDC_Shift (nextSDC);
        rtPTOKEN_SDC_CurrentOrigin (nextSDC) = rtPTOKEN_SDC_Origin (nextSDC) +
				adjustment + (shift < 0 ? shift : 0);
	if (TracingCollapse) IO_printf (
	    "    SDC=%08X, %d %d:%d\n",
	    nextSDC,
	    rtPTOKEN_SDC_CurrentOrigin (nextSDC),
	    rtPTOKEN_SDC_Origin (nextSDC),
	    rtPTOKEN_SDC_Shift (nextSDC)
	);
	adjustment += shift;
    }

    rtPTOKEN_PTC_AltCount (ptc) = x;
    if (TracingCollapse) IO_printf ("ComputeNewCurrents<\n");
}


/*****************************************
 *  Relationship Definition and Utility  *
 *****************************************/

enum InstructionRelationship {
    None,
    InsertionInsertion,
    InsertionDeletion1,
    InsertionDeletion2,
    InsertionDeletion3,
    InsertionDeletion4,
    InsertionDeletion5,
    DeletionInsertion,
    DeletionDeletion
};

PrivateFnDef InstructionRelationship GetRelationship (
    rtPTOKEN_SDCType* sdc1, rtPTOKEN_SDCType* sdc2
)
{
    int
	shift1,
	shift2,
	origin1,
	origin2;
    if (TracingCollapse) IO_printf (">GetRelationship<\n");

    if (IsNil (sdc1) && IsNil (sdc2))
        return None;

    shift1 = rtPTOKEN_SDC_Shift (sdc1);
    shift2 = rtPTOKEN_SDC_Shift (sdc2);

    if (shift1 < 0 && shift2 < 0)
        return DeletionDeletion;

    if (shift1 < 0 && shift2 > 0)
        return DeletionInsertion;

    if (shift1 > 0 && shift2 > 0)
        return InsertionInsertion;

    origin1 = rtPTOKEN_SDC_CurrentOrigin (sdc1);
    origin2 = rtPTOKEN_SDC_Origin (sdc2);

    if (origin1 + shift1 > origin2 && origin2 + shift2 < origin1)
        return InsertionDeletion1;

    if (origin1 + shift1 < origin2 && origin2 + shift2 > origin1)
        return InsertionDeletion2;

    if ((origin1 + shift1 >= origin2 && origin2 + shift2 > origin1) ||
	(origin1 + shift1 > origin2 && origin2 + shift2 >= origin1))
        return InsertionDeletion3;

    if ((origin1 + shift1 < origin2 && origin2 + shift2 <= origin1) ||
	(origin1 + shift1 <= origin2 && origin2 + shift2 < origin1))
        return InsertionDeletion4;

    if (origin1 + shift1 == origin2 && origin2 + shift2 == origin1)
        return InsertionDeletion5;

    ERR_SignalFault (
	EC__InternalInconsistency, "GetRelationship: Unknown Relationship"
    );

    return None;
}


/*---------------------------------------------------------------------------
 ***** Routine to collapse a chain of ptokens into a single ptoken constructor.
 *
 *  Arguments:
 *	pThis		-  The reference to the head of the ptoken
 *				   chain whose instructions are to be
 *				   accumulated in the returned ptoken
 *				   constructor.
 *
 *  Returns:
 *	a pointer to a constructor whose instructions represent the total
 *	adjustments to the base object needed to make it current.
 *
 *****/
rtPTOKEN_CType *rtPTOKEN_Handle::getAdjustments () {
    int rel, adjustment;

    CumulativeAdjustmentsCount++;

/*****  The ptoken chain must start with a shift ptoken  *****/
    if (typeIsntShift ()) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtPTOKEN_CumulativeAdjustments:  Must be a shift p-token"
    );

/*****  Obtain a constructor for the current generation  *****/
    rtPTOKEN_CType *ptokenc = asConstructor ();

/*****  Traverse the ptoken chain, accumulating instructions in ptokenc  *****/
    rtPTOKEN_Handle *pNextGeneration;
    while (IsntNil (pNextGeneration = ptokenc->NextGeneration ()) && pNextGeneration->typeIsShift ()) {
        rtPTOKEN_CType *nextPTC = pNextGeneration->asConstructor ();

	rtPTOKEN_SDCType *sdc1 = rtPTOKEN_PTC_ChainHead (ptokenc);
	rtPTOKEN_SDCType *sdc2 = rtPTOKEN_PTC_ChainHead (nextPTC);
	while (IsntNil (sdc1) || IsntNil (sdc2)) {
	    if (TracingCollapse) IO_printf (
		"sdc1:%08X, sdc2:%08X co1:%d %d:%d, co2:%d %d:%d\n",
		sdc1, sdc2,
		IsNil (sdc1) ? 0 : rtPTOKEN_SDC_CurrentOrigin (sdc1),
		IsNil (sdc1) ? 0 : rtPTOKEN_SDC_Origin (sdc1),
		IsNil (sdc1) ? 0 : rtPTOKEN_SDC_Shift (sdc1),
		IsNil (sdc2) ? 0 : rtPTOKEN_SDC_CurrentOrigin (sdc2),
		IsNil (sdc2) ? 0 : rtPTOKEN_SDC_Origin (sdc2),
		IsNil (sdc2) ? 0 : rtPTOKEN_SDC_Shift (sdc2)
	    );

	    while (Compare (sdc1, sdc2) > 0) {
	        sdc1 = rtPTOKEN_SDC_NextSDC (sdc1);
		if (TracingCollapse) {
		    IO_printf (">\n");
		    IO_printf ("sdc1:%08X, co1:%d, %d:%d\n",
			sdc1,
			IsNil (sdc1) ? 0 : rtPTOKEN_SDC_CurrentOrigin (sdc1),
			IsNil (sdc1) ? 0 : rtPTOKEN_SDC_Origin (sdc1),
			IsNil (sdc1) ? 0 : rtPTOKEN_SDC_Shift (sdc1));
		}
	    }

	    while (Compare (sdc1, sdc2) < 0) {
		if (TracingCollapse)
		    IO_printf ("<\n");
	        sdc2 = InsertSDCfromPTC (ptokenc, sdc1, nextPTC, sdc2);
	    }

	    if (Compare (sdc1, sdc2) == 0) {
		if (TracingCollapse)
		    IO_printf ("=\n");
	        switch (rel = (int)GetRelationship (sdc1, sdc2)) {
		case None:
		    if (TracingCollapse)
			IO_printf (">None<\n");
		    break;

		case InsertionInsertion:
		    if (TracingCollapse)
			IO_printf (">InsertionInsertion\n");
		    rtPTOKEN_SDC_Shift (sdc1) += rtPTOKEN_SDC_Shift (sdc2);
		    sdc2 = Delete (nextPTC, sdc2, true);
		    if (TracingCollapse)
			IO_printf ("InsertionInsertion<\n");
		    break;

		case DeletionInsertion:
		    if (TracingCollapse)
			IO_printf (">DeletionInsertion\n");
		    sdc1 = rtPTOKEN_SDC_NextSDC (sdc1);
/*****
 *  Force it around the loop again.  If the next sdc is greater that this
 *  the the greater test will insert sdc2 then.  If sdc2's origin is the same
 *  as the next sdc, they will be merged correctly in the == case.
 *  This happens with the following sdc's:
 *	6-3
 *	3+2
 *	3+3
 *  This 6-3 and 3+2 get inserted and then the 3+3 tries to get inserted
 *  between the two.  It winds up here in DeleteInsert with 6-3 and 3+2.
 *  Sdc1 (6-3) is processed and sdc2 (3+3) goes back to the top of the
 *  loop.  Then (3+3) gets compared with (3+2) and correctly gets merged
 *  by the InsertInsert case.
 *****/
		    if (TracingCollapse)
			IO_printf ("DeletionInsertion<\n");
		    break;

		case InsertionDeletion1:
	/**** Overlap on low end of insertion  ****/
		    if (TracingCollapse)
			IO_printf (">InsertionDeletion1\n");
		    adjustment = rtPTOKEN_SDC_CurrentOrigin (sdc1) -
				 rtPTOKEN_SDC_Origin (sdc2);
		    rtPTOKEN_SDC_Shift (sdc2) -= adjustment;
		    rtPTOKEN_SDC_Shift (sdc1) += adjustment;
		    rtPTOKEN_SDC_Origin (sdc2) = rtPTOKEN_SDC_CurrentOrigin (sdc1);
		    sdc1 = rtPTOKEN_SDC_NextSDC (sdc1);
		    if (TracingCollapse)
			IO_printf ("InsertionDeletion1<\n");
		    break;

		case InsertionDeletion2:
	/**** Overlap on high end of insertion  ****/
		    if (TracingCollapse)
			IO_printf (">InsertionDeletion2\n");
		    adjustment = rtPTOKEN_SDC_CurrentOrigin (sdc1) +
				 rtPTOKEN_SDC_Shift (sdc1) -
				 rtPTOKEN_SDC_Origin (sdc2) -
				 rtPTOKEN_SDC_Shift (sdc2);
		    rtPTOKEN_SDC_Shift (sdc2) += adjustment;
		    rtPTOKEN_SDC_Shift (sdc1) -= adjustment;
		    rtPTOKEN_SDC_Origin (sdc2) -= adjustment;
		    sdc2 = InsertSDCfromPTC (ptokenc, sdc1, nextPTC, sdc2);
		    if (TracingCollapse)
			IO_printf ("InsertionDeletion2<\n");
		    break;

		case InsertionDeletion3:
	/****  Deletion solely of elements previously inserted  ****/
		    if (TracingCollapse)
			IO_printf (">InsertionDeletion3\n");
		    rtPTOKEN_SDC_Shift (sdc1) += rtPTOKEN_SDC_Shift (sdc2);
		    sdc2 = Delete (nextPTC, sdc2, true);
		    if (TracingCollapse)
			IO_printf ("InsertionDeletion3<\n");
		    break;

		case InsertionDeletion4:
	/****  Entire Insertion Deleted  ****/
		    if (TracingCollapse)
			IO_printf (">InsertionDeletion4\n");
		    rtPTOKEN_SDC_Shift (sdc2) += rtPTOKEN_SDC_Shift (sdc1);
		    rtPTOKEN_SDC_Origin (sdc2) -= rtPTOKEN_SDC_Shift (sdc1);
		    sdc1 = Delete (ptokenc, sdc1, true);
		    if (TracingCollapse)
			IO_printf ("InsertionDeletion4<\n");
		    break;

		case InsertionDeletion5:
	/****  Insertion and deletion cancel exactly  ****/
		    if (TracingCollapse)
			IO_printf (">InsertionDeletion5\n");
		    sdc2 = Delete (nextPTC, sdc2, true);
		    sdc1 = Delete (ptokenc, sdc1, true);
		    if (TracingCollapse)
			IO_printf ("InsertionDeletion5<\n");
		    break;

		case DeletionDeletion:
		    if (TracingCollapse)
			IO_printf (">DeletionDeletion\n");
		    rtPTOKEN_SDC_Origin (sdc2) -= rtPTOKEN_SDC_Shift (sdc1);
		    rtPTOKEN_SDC_Shift (sdc2) += rtPTOKEN_SDC_Shift (sdc1);
		    sdc1 = Delete (ptokenc, sdc1, true);
		    if (TracingCollapse)
			IO_printf ("DeletionDeletion<\n");
		    break;

		default:
	            ERR_SignalFault (
			EC__InternalInconsistency,
			"_CumulativeAdjustments: Unknown Relationship"
		    );
		    break;
		}
		if (TracingCollapse)
		    IO_printf (" rel:%d\n", rel);

	    }/*	end if */

	}/* end for */
	ptokenc->m_pNextGeneration.setTo (nextPTC->NextGeneration ());
	ComputeNewCurrents (ptokenc);
	nextPTC->discard ();

    }/* end while */
    rtPTOKEN_PTC_BaseCount (ptokenc) = pNextGeneration->cardinality ();

    return ptokenc;
}


/************************************************************
 *****  Standard Representation Type Services Routines  *****
 ************************************************************/

/**************
 *  Printers  *
 **************/

void rtPTOKEN_Handle::displayTokenType () const {
    rtPTOKEN_SDType *sdp, *sdl;

    M_CPD *pCPD = 0;
    switch (type ()) {
    case rtPTOKEN_TT_Base:
        IO_printf ("[Base:%d:%s]", cardinality (), isIndependent () ? "I" : "D");
        break;
    case rtPTOKEN_TT_Shift:
        IO_printf ("[Shift:%d:%s]", cardinality (), isIndependent () ? "I" : "D");
	for (sdl = sdCount () + (sdp = sdArray ()); sdp < sdl; sdp++)
	    IO_printf (" %d:%d", rtPTOKEN_SD_Origin (sdp), rtPTOKEN_SD_Shift (sdp));
        break;
    case rtPTOKEN_TT_Cartesian:
        IO_printf ("[Cartesian:%d]", cardinality ());

	pCPD->GetCPD ();
 	IO_printf ("\n  Row PToken: ");
	RTYPE_QPrint (pCPD, rtPTOKEN_CPx_RowPToken);
	IO_printf (" ");
	RTYPE_Print (pCPD, rtPTOKEN_CPx_RowPToken);

 	IO_printf ("\n  Col PToken: ");
	RTYPE_QPrint (pCPD, rtPTOKEN_CPx_ColPToken);
	IO_printf (" ");
	RTYPE_Print (pCPD, rtPTOKEN_CPx_ColPToken);
        break;
    default:
        IO_printf ("[UnknownTT#%d]", type ());
        break;
    }
    if (pCPD)
	pCPD->release ();
}


/*---------------------------------------------------------------------------
 *****  Routine to display a P-Token.
 *
 *  Arguments:
 *	cpd			- the address of a handle for the P-Token
 *				  to be printed.
 *	recursive		- a boolean which when true requests recursive
 *				  display of the P-Token.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintPToken (M_CPD *cpd, bool recursive) {
    IO_printf ("P-Token{");
    if (recursive) {
	RTYPE_QPrint (cpd, -1);
	IO_printf (" ");
    }


    static_cast<rtPTOKEN_Handle*>(cpd->containerHandle ())->displayTokenType ();

    if (recursive) {
	IO_printf ("\n");
	RTYPE_RPrint (cpd, rtPTOKEN_CPx_NextGeneration);
    }
    IO_printf ("}");
}


/*---------------------------------------------------------------------------
 *****  Routine to display a P-Token.
 *
 *  Arguments:
 *	cpd			- the address of a handle for the P-Token
 *				  to be printed.
 *	recursive		- a boolean which when true requests recursive
 *				  display of the P-Token.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *  Note:
 *	This function displays the ptoken without displaying its pop.
 *	This is needed for the testdeck scripts.
 *
 *****/
void rtPTOKEN_Handle::display (bool recursive) const {
    IO_printf ("P-Token{");

    displayTokenType ();

    if (recursive && typeIsShift ()) {
	IO_printf ("\n");
	Reference pNext (nextPToken ());
	pNext->display (true);
    }
    IO_printf ("}");
}


/***************************
 *  Container Reclamation  *
 ***************************/

class VPOPReclaimer : public VBenderenceable {
    DECLARE_CONCRETE_RTT (VPOPReclaimer, VBenderenceable);

//  Construction
public:
    VPOPReclaimer (VPOPReclaimer *pSuccessor, M_POP const &rPOP, M_ASD *pPOPOwner);

//  Destruction
protected:
    ~VPOPReclaimer ();

//  State
protected:
    VReference<VPOPReclaimer>	const m_pSuccessor;
    M_POP			const m_iPOP;
    M_ASD*			const m_pPOPOwner;
};

DEFINE_CONCRETE_RTT (VPOPReclaimer);

VPOPReclaimer::VPOPReclaimer (
    VPOPReclaimer *pSuccessor, M_POP const &rPOP, M_ASD *pPOPOwner
) : m_pSuccessor (pSuccessor), m_iPOP (rPOP), m_pPOPOwner (pPOPOwner) {
}

VPOPReclaimer::~VPOPReclaimer () {
    m_pPOPOwner->Release (&m_iPOP);
}


/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	pOwner			- the ASD of the space which owns the container
 *
 *  Notes:
 *	Reclaiming a P-Token chain can lead to a tail-recursive stack overflow.
 *	To avoid that problem, the POPs referenced from a P-Token are copied to
 *	VPOPReclaimer VReferenceables.  Because VReferenceables are freed using
 *	a non-recursive reaper, the tail recursion is avoided.
 *
 *****/
PrivateFnDef void ReclaimContainer (M_ASD *pOwner, M_CPreamble const *preambleAddress) {
    rtPTOKEN_Type const *ptoken = reinterpret_cast<rtPTOKEN_Type const *>(preambleAddress + 1);

//  Create reclaimer objects for the POPs referenced by this p-token, ...
    VReference<VPOPReclaimer> pPOPReclamationQueueHead;

    if (pOwner->ReferenceIsntNil (&rtPTOKEN_PT_nextGeneration (ptoken))) pPOPReclamationQueueHead.setTo (
	new VPOPReclaimer (
	    pPOPReclamationQueueHead, rtPTOKEN_PT_nextGeneration (ptoken), pOwner
	)
    );

    if (rtPTOKEN_PT_TokenType (ptoken) == rtPTOKEN_TT_Cartesian) {
	if (pOwner->ReferenceIsntNil (&rtPTOKEN_PT_RowPToken (ptoken))) pPOPReclamationQueueHead.setTo (
	    new VPOPReclaimer (
		pPOPReclamationQueueHead, rtPTOKEN_PT_RowPToken (ptoken), pOwner
	    )
	);

	if (pOwner->ReferenceIsntNil (&rtPTOKEN_PT_ColPToken (ptoken))) pPOPReclamationQueueHead.setTo (
	    new VPOPReclaimer (
		pPOPReclamationQueueHead, rtPTOKEN_PT_ColPToken (ptoken), pOwner
	    )
	);
    }

//  ... that will be destroyed here iff this call isn't tail recursive.
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'save' a P-Token
 *****/
bool rtPTOKEN_Handle::PersistReferences () {
    createContainer ();

    rtPTOKEN_Type *ptoken = typecastContent ();

    bool result = Persist (&rtPTOKEN_PT_nextGeneration (ptoken));

    if (result && rtPTOKEN_PT_TokenType (ptoken) == rtPTOKEN_TT_Cartesian)
	result = Persist ((M_POP*)rtPTOKEN_PT_CartesianPTokens (ptoken), 2);

    return result;
}


/******************************************
 *****  Standard PToken Mark Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'save' a P-Token
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
PrivateFnDef void MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD* pSpace, M_CPreamble const *pContainer) {
    rtPTOKEN_Type const *ptoken = reinterpret_cast<rtPTOKEN_Type const *>(pContainer + 1);

    pGCV->Mark (pSpace, &rtPTOKEN_PT_nextGeneration (ptoken));

    if (rtPTOKEN_PT_TokenType (ptoken) == rtPTOKEN_TT_Cartesian)
	pGCV->Mark (pSpace, (M_POP const*)rtPTOKEN_PT_CartesianPTokens (ptoken), 2);
}


/***************************************************
 *****  Internal Debugger Method Dictionaries  *****
 ***************************************************/

/***************
 *  Utilities  *
 ***************/

PrivateFnDef int ReadInt (char const* prompt) {
    char readBuffer[80];
    int i = 0;

    if (IsntNil (IO_pfgets (prompt, readBuffer, 80)))
	STD_sscanf (readBuffer, "%d", &i);

    return i;
}


/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	new rtPTOKEN_Handle (
	    IOBJ_ScratchPad, IOBJ_NumericIObjectValue (parameterArray[0], unsigned int)
	)
    );
}

IOBJ_DefineNewaryMethod (NewCartesianDM) {
    return RTYPE_QRegister (
	new rtPTOKEN_Handle (
	    IOBJ_ScratchPad,
	    RTYPE_QRegisterPToken (parameterArray[0]),	// rowPToken
	    RTYPE_QRegisterPToken (parameterArray[1])	// colPToken
	)
    );
}

IOBJ_DefineUnaryMethod (TracesOnDM) {
    TracingCartesianUpdate  =
    TracingConversion	    =
    TracingPTCAllocator	    =
    TracingSDCInsertion	    =
    TracingSDCAllocator	    = true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM) {
    TracingCartesianUpdate  =
    TracingConversion	    =
    TracingPTCAllocator	    =
    TracingSDCInsertion	    =
    TracingSDCAllocator	    = false;

    return self;
}

IOBJ_DefineNilaryMethod (CartesianTraceDM) {
    return IOBJ_SwitchIObject (&TracingCartesianUpdate);
}

IOBJ_DefineNilaryMethod (CollapseTraceDM) {
    return IOBJ_SwitchIObject (&TracingCollapse);
}

IOBJ_DefineNilaryMethod (ConversionTraceDM) {
    return IOBJ_SwitchIObject (&TracingConversion);
}

IOBJ_DefineNilaryMethod (PTCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingPTCAllocator);
}

IOBJ_DefineNilaryMethod (SDCAllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingSDCAllocator);
}

IOBJ_DefineNilaryMethod (SDCInsertionTraceDM) {
    return IOBJ_SwitchIObject (&TracingSDCInsertion);
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nP-Token Routines Usage Counts\n");
    IO_printf (  "-----------------------------------------\n");

    IO_printf ("BaseElementCount		= %u\n", BaseElementCount);
    IO_printf ("BasePTokenCount			= %u\n", BasePTokenCount);
    IO_printf ("IsCurrentCount			= %u\n", IsCurrentCount);
    IO_printf ("IsntCurrentCount		= %u\n", IsntCurrentCount);
    IO_printf ("\n");

    IO_printf ("NewBasePTokenCount		= %u\n", NewBasePTokenCount);
    IO_printf ("NewCartesianPTokenCount		= %u\n", NewCartesianPTokenCount);
    IO_printf ("NewShiftPTConstructorCount	= %u\n", NewShiftPTConstructorCount);
    IO_printf ("\n");

    IO_printf ("AppendAdjustmentCount		= %u\n", AppendAdjustmentCount);
    IO_printf ("CumulativeAdjustmentsCount      = %u\n", CumulativeAdjustmentsCount);
    IO_printf ("\n");

    IO_printf ("BasicToPTokenCount		= %u\n", BasicToPTokenCount);
    IO_printf ("ToConstructorCount	    	= %u\n", ToConstructorCount);
    IO_printf ("DiscardConstructorCount		= %u\n", DiscardConstructorCount);
    IO_printf ("\n");

    IO_printf ("AllocateSDCCount	    	= %u\n", SDConstructorFreeList.allocationCount ());
    IO_printf ("DiscardSDCCount			= %u\n", SDConstructorFreeList.deallocationCount ());
    IO_printf ("\n");

    IO_printf ("UpdateCartesianPTokenCount	= %u\n", UpdateCartesianPTokenCount);
    IO_printf ("\n");

    IO_printf (
	"Estimated Number of Align Queries  = %u\n",
	IsCurrentCount + IsntCurrentCount
    );
    IO_printf (
	"Estimated Number of Aligns Needed  = %u\n\n",
	CumulativeAdjustmentsCount
    );

    return self;
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (SizeDM) {
    return IOBJ_IntIObject (RTYPE_QRegisterPToken (self)->sdCount ());
}

IOBJ_DefineUnaryMethod (BaseElementCountDM) {
    return IOBJ_IntIObject (RTYPE_QRegisterPToken (self)->cardinality ()
    );
}

IOBJ_DefineUnaryMethod (IsCurrentDM) {
    return IOBJ_IntIObject(RTYPE_QRegisterPToken (self)->isTerminal ());
}

IOBJ_DefineUnaryMethod (NextGenerationDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtPTOKEN_CPx_NextGeneration);
}

IOBJ_DefineUnaryMethod (BasePTokenDM) {
    return RTYPE_QRegister (RTYPE_QRegisterPToken (self)->basePToken ());
}

IOBJ_DefineUnaryMethod (ToggleIndependenceFlagDM) {
    rtPTOKEN_Handle *pPTH = RTYPE_QRegisterPToken (self);
    pPTH->setIsIndependentTo (pPTH->isDependent ());

    return self;
}


IOBJ_DefineUnaryMethod (NewShiftPTokenDM) {
    return RTYPE_QRegister (RTYPE_QRegisterPToken (self)->makeAdjustments ()->ToPToken ());
}

IOBJ_DefineUnaryMethod (AddSDCTestDM) {
    rtPTOKEN_CType *constructor = RTYPE_QRegisterPToken (self)->makeAdjustments ();

    int iter = ReadInt ("Iterations> ");

    for (int i = 0; i < iter; i++) {
	int origin = ReadInt ("origin> ");

	if (origin == -10)
	    break;

	int shift = ReadInt ("shift> ");

        constructor->AppendAdjustment (origin, shift);
    }

    return RTYPE_QRegister (constructor->ToPToken ());
}


#define CollapseMacro(origin, shift)\
    IO_printf (" %d:%d", origin, shift)

IOBJ_DefineUnaryMethod (CollapseDM) {
    rtPTOKEN_CType* constructor = RTYPE_QRegisterPToken (self)->getAdjustments ();

    IO_printf ("P-Token{[Shift:%d]", rtPTOKEN_PTC_BaseCount (constructor));
    rtPTOKEN_BTraverseAdjustments (constructor, CollapseMacro, CollapseMacro);
    IO_printf ("}\n");

    return RTYPE_QRegister (constructor->ToPToken ());
}

IOBJ_DefineUnaryMethod (AddSDCVolumeWCTestDM) {
    rtPTOKEN_CType* constructor = RTYPE_QRegisterPToken (self)->makeAdjustments ();
    int iter = ReadInt ("Iterations> ");

    for (int i = 0; i < iter; i++) {
	if (i % 1000 == 0) IO_printf ("%d\n", i);
        constructor->AppendAdjustment (2 * i, 1);
    }

    IO_printf ("To PToken...\n");

    return RTYPE_QRegister (constructor->ToPToken ());
}

IOBJ_DefineUnaryMethod (AddSDCVolumeBCTestDM) {
    rtPTOKEN_CType* constructor = RTYPE_QRegisterPToken (self)->makeAdjustments ();
    int iter = ReadInt ("Iterations> ");

    for (int i = iter; i > 0; i--) {
	if (i % 1000 == 0)
	    IO_printf ("%d\n", i);
        constructor->AppendAdjustment (i, 1);
    }

    return RTYPE_QRegister (constructor->ToPToken ());
}

IOBJ_DefineUnaryMethod (DisplayDM) {
    RTYPE_QRegisterPToken (self)->display (true);
    return self;
}

IOBJ_DefineUnaryMethod (DumpDM) {
    RTYPE_QRegisterPToken (self)->dump ();
    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtPTOKEN_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"		, NewDM)
	IOBJ_MDE ("newRow:col:"		, NewCartesianDM)
	IOBJ_MDE ("tracesOn"		, TracesOnDM)
	IOBJ_MDE ("tracesOff"		, TracesOffDM)
	IOBJ_MDE ("cartesianTrace"	, CartesianTraceDM)
	IOBJ_MDE ("collapseTrace"	, CollapseTraceDM)
	IOBJ_MDE ("conversionTrace"	, ConversionTraceDM)
	IOBJ_MDE ("ptcAllocatorTrace"	, PTCAllocatorTraceDM)
	IOBJ_MDE ("sdcAllocatorTrace"	, SDCAllocatorTraceDM)
	IOBJ_MDE ("sdcInsertionTrace"	, SDCInsertionTraceDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
    	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("size"		, SizeDM)
	IOBJ_MDE ("baseCount"		, BaseElementCountDM)
	IOBJ_MDE ("isCurrent"		, IsCurrentDM)
	IOBJ_MDE ("nextPToken"		, NextGenerationDM)
	IOBJ_MDE ("basePToken"		, BasePTokenDM)
	IOBJ_MDE ("toggleDependence"	, ToggleIndependenceFlagDM)
	IOBJ_MDE ("shiftPToken"		, NewShiftPTokenDM)
	IOBJ_MDE ("collapse"		, CollapseDM)
	IOBJ_MDE ("addSDCTest"		, AddSDCTestDM)
	IOBJ_MDE ("addSDCVolumeTest"	, AddSDCVolumeBCTestDM)
	IOBJ_MDE ("addSDCWorstCase"	, AddSDCVolumeWCTestDM)
	IOBJ_MDE ("display"		, DisplayDM)
	IOBJ_MDE ("dump"		, DumpDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtPTOKEN_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtPTOKEN_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
        PrintPToken (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
        PrintPToken (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtPTOKEN);
    FAC_FDFCase_FacilityDescription ("Positional Token Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTptoken.c 1 replace /users/mjc/system
  860404 20:14:04 mjc Create Positional Generation Token Representation Type

 ************************************************************************/
/************************************************************************
  RTptoken.c 1 replace /users/mjc/system
  860409 08:00:23 mjc Create positional generation token type

 ************************************************************************/
/************************************************************************
  RTptoken.c 2 replace /users/mjc/system
  860410 19:21:52 mjc Added creation routine

 ************************************************************************/
/************************************************************************
  RTptoken.c 3 replace /users/mjc/system
  860420 18:49:32 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTptoken.c 4 replace /users/mjc/system
  860422 22:56:03 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTptoken.c 5 replace /users/hjb/system
  860423 01:05:29 hjb done updating calls

 ************************************************************************/
/************************************************************************
  RTptoken.c 6 replace /users/mjc/system
  860501 23:48:21 mjc Return P-Token's for further development by jck

 ************************************************************************/
/************************************************************************
  RTptoken.c 8 replace /users/jck/system
  860514 10:56:15 jck Lurking bugs fixed

 ************************************************************************/
/************************************************************************
  RTptoken.c 9 replace /users/jck/system
  860521 12:38:21 jck Relaxed Constraints on Addition of Instructions

 ************************************************************************/
/************************************************************************
  RTptoken.c 10 replace /users/mjc/system
  860529 09:43:11 mjc Eliminated direct access to 'stdin' through 'scanf'

 ************************************************************************/
/************************************************************************
  RTptoken.c 11 replace /users/jck/system
  860604 13:27:57 jck Corrected a bookkeeping bug in collapse algorithm

 ************************************************************************/
/************************************************************************
  RTptoken.c 12 replace /users/mjc/system
  860617 15:09:20 mjc Added container save function

 ************************************************************************/
/************************************************************************
  RTptoken.c 13 replace /users/jck/system
  860623 14:17:32 jck Minor adjustments to _CumulativeAdjustments

 ************************************************************************/
/************************************************************************
  RTptoken.c 14 replace /users/mjc/system
  860626 17:28:58 mjc Deleted 'UNWIND' traps

 ************************************************************************/
/************************************************************************
  RTptoken.c 15 replace /users/jad/system
  860630 16:17:09 jad fixed bug in IsCurrent routine

 ************************************************************************/
/************************************************************************
  RTptoken.c 16 replace /users/mjc/system
  860708 13:06:50 mjc Added 'rtPTOKEN_BasePToken', removed some UNWIND traps, changed internal method names

 ************************************************************************/
/************************************************************************
  RTptoken.c 17 replace /users/jad/system
  860814 14:15:30 jad added rtPTOKEN_IsntCurrent and
rtPTOKEN_CPDCumAdjustments which used together eliminate a CPD in Aligns

 ************************************************************************/
/************************************************************************
  RTptoken.c 18 replace /users/jad/system
  860814 17:53:22 jad added counters and a displayCounts debug method

 ************************************************************************/
/************************************************************************
  RTptoken.c 19 replace /users/mjc/system
  860817 20:15:39 mjc Converted CPD initializer/instantiator/gc routines to expect nothing of new CPDs

 ************************************************************************/
/************************************************************************
  RTptoken.c 20 replace /users/mjc/system
  860819 12:33:31 mjc Converted 'BaseElementCount' and 'Is/IsntCurrent' to use fast preamble access

 ************************************************************************/
/************************************************************************
  RTptoken.c 21 replace /users/jad/system
  860914 11:14:40 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  RTptoken.c 22 replace /users/mjc/mysystem
  861005 02:48:50 mjc Modified 'ToPToken' to create new P-Tokens only when necessary

 ************************************************************************/
/************************************************************************
  RTptoken.c 23 replace /users/jck/system
  861017 21:16:26 jck Added printing of POP's to recursive print

 ************************************************************************/
/************************************************************************
  RTptoken.c 24 replace /users/jad/system
  870109 13:38:25 jad implemented cartesian ptokens

 ************************************************************************/
/************************************************************************
  RTptoken.c 25 replace /users/jad/system
  870109 15:44:45 jad remember to undef macros

 ************************************************************************/
/************************************************************************
  RTptoken.c 26 replace /users/jck/system
  870202 13:29:14 jck Broke sizing macro into three; one for each type of ptoken

 ************************************************************************/
/************************************************************************
  RTptoken.c 27 replace /users/mjc/system
  870215 23:03:16 lcn Changed 'BasicToPToken' to guarantee that p-token chains live in the same space

 ************************************************************************/
/************************************************************************
  RTptoken.c 28 replace /users/jad/system
  870303 16:13:40 jad added a cartesian ptoken verification routine

 ************************************************************************/
/************************************************************************
  RTptoken.c 29 replace /users/jck/system
  870415 10:43:50 jck Change to InitStdCPD to return a cpd. Also taught save and gc functions about Cartesian ptokens

 ************************************************************************/
/************************************************************************
  RTptoken.c 30 replace /users/jck/system
  870420 10:36:15 jck Freed a CPD in normalizationIsTerminal

 ************************************************************************/
/************************************************************************
  RTptoken.c 31 replace /users/mjc/MERGE/vax
  870519 17:14:38 mjc Added omitted 'va_list ap;' declaration

 ************************************************************************/
/************************************************************************
  RTptoken.c 32 replace /users/mjc/translation
  870524 09:34:40 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTptoken.c 33 replace /users/jck/system
  870622 09:19:12 jck Fixed a bug in the DeletionInsertion case of the
cumulative adjustments routine

 ************************************************************************/
/************************************************************************
  RTptoken.c 34 replace /users/jck/system
  870706 08:15:19 jck Added an independent of vstore flag to ptokens which will be unset at time of vstore creation

 ************************************************************************/
/************************************************************************
  RTptoken.c 35 replace /users/jck/system
  870730 09:31:34 jck Added some usage counts and a debugger method to toggle the ptoken's independence flag

 ************************************************************************/
/************************************************************************
  RTptoken.c 36 replace /users/jck/system
  871007 13:38:01 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTptoken.c 37 replace /users/jad/system
  880427 16:52:23 jad Fixed bug in collapse

 ************************************************************************/
/************************************************************************
  RTptoken.c 38 replace /users/jck/system
  890222 15:31:19 jck Implemented corruption detection mechanism

 ************************************************************************/
/************************************************************************
  RTptoken.c 39 replace /users/jck/system
  890823 11:33:28 jck added explanatory comment for call to M_ForwardContainer

 ************************************************************************/
/************************************************************************
  RTptoken.c 40 replace /users/jck/system
  890828 16:36:53 jck Disallowed negative 'nelements' parameter to new routine

 ************************************************************************/
/************************************************************************
  RTptoken.c 41 replace /users/jck/system
  891019 10:14:38 jck Added M_EnableModifications prior to marking ptoken open

 ************************************************************************/

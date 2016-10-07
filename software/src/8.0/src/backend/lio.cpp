/*****  Logical I/O Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName LIO

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "verr.h"
#include "venvir.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTlink.h"
#include "RTptoken.h"
#include "RTrefuv.h"

#include "IOMDriver.h"

#include "VOutputGenerator.h"

#include "VTask.h"

/*****  Self  *****/
#include "lio.h"


/************************************************
 ************************************************
 *****  Counters, Parameters, and Switches  *****
 ************************************************
 ************************************************/

/**********************
 *****  Counters  *****
 **********************/

unsigned int VOutputBuffer::AllocatoR::g_iLocalExtentExpansionCount  = 0;
unsigned int VOutputBuffer::AllocatoR::g_iGlobalExtentExpansionCount = 0;
unsigned int VOutputBuffer::AllocatoR::CellBlock::g_iTotalAllocation = 0;

/************************
 *****  Parameters  *****
 ************************/

size_t VOutputBuffer::AllocatoR::g_sInitialCellBlock = 8 * 1024;	// was 2097152;
size_t VOutputBuffer::AllocatoR::g_sLargestCellBlock = 2 * 1024 * 1024;	// ==  2097152;

/**********************
 *****  Switches  *****
 **********************/

/****************************
 *****  Run Time Types  *****
 ****************************/

DEFINE_CONCRETE_RTT (VOutputBuffer);

DEFINE_CONCRETE_RTT (VOutputBuffer::AllocatoR);


/**************************
 **************************
 *****                *****
 *****  Construction  *****
 *****                *****
 **************************
 **************************/

VOutputBuffer::AllocatoR::CellBlock::CellBlock (
    CellBlock* pPredecessor, size_t sCellBlock
)
: m_pPredecessor(pPredecessor)
, m_pSuccessor	(pPredecessor ? pPredecessor->successor () : 0)
, m_sCellBlock	(sCellBlock)
, m_sFreeArea	(sCellBlock)
, m_pFreeArea	((char*)(&m_pFreeArea + 1))
{
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor = this;
    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor = this;
}

VOutputBuffer::AllocatoR::AllocatoR ()
: m_pActiveBlock	(0)
, m_sAllBlocks		(0)
, m_sNextBlock		(g_sInitialCellBlock)
{
}


//  Subtask/masking/diverting
VOutputBuffer::VOutputBuffer (VTaskDomain* pDomain, VOutputBuffer* pParent, bool fDiverting)
: VTaskDomainedStore	(pDomain)
, m_pParent		(pParent)
, m_pChannel		(pParent->m_pChannel)
, m_pAllocator		(fDiverting ? new AllocatoR () : pParent->m_pAllocator.referent ())
, m_fDiverting		(fDiverting || pParent->m_fDiverting || isntScalar ())
, m_pRingHeads		(0)
{
}

//  Top Task
VOutputBuffer::VOutputBuffer (VTaskDomain* pDomain, IOMDriver* pChannel)
: VTaskDomainedStore	(pDomain)
, m_pChannel		(pChannel)
, m_pAllocator		(new AllocatoR ())
, m_fDiverting		(isntScalar ())
, m_pRingHeads		(0)
{
    pChannel->IncrementUseCount ();
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VOutputBuffer::AllocatoR::~AllocatoR () {
    CellBlock* pBlock = m_pActiveBlock;
    while (pBlock) {
	m_pActiveBlock = pBlock;
	pBlock = pBlock->successor ();
    }

    while (pBlock = m_pActiveBlock) {
	m_pActiveBlock = pBlock->predecessor ();
	CellBlock::g_iTotalAllocation -= pBlock->cellBlockSize ();

	delete pBlock;
    }

//  Make adjustments if this allocator needed a large number of cell blocks, ...
    if (m_sAllBlocks > 8 * g_sInitialCellBlock) {
	g_iGlobalExtentExpansionCount++;
	g_sInitialCellBlock *= 2;
    }
}

VOutputBuffer::~VOutputBuffer () {
    deallocate (m_pRingHeads);
    if (m_pParent.isNil ())
	m_pChannel->DecrementUseCount ();
}


/*******************************
 *******************************
 *****                     *****
 *****  Output Generation  *****
 *****                     *****
 *******************************
 *******************************/

/**********************************
 *----  Output Buffer Access  ----*
 **********************************/

VOutputBuffer* VTask::getOutputBuffer () {
//  Return immediately if the output buffer domain is valid:
    if (outputBufferDomainIsValid ())
	return m_pOutputBuffer;

//  Otherwise, try to find a valid output buffer for this domain, ...
    VTaskDomain* const pThisDomain = m_pDomain;

    VTask* pSourceTask = m_pCaller;
    while (pSourceTask->domainIs (pThisDomain) && pSourceTask->outputBufferDomainIsntValid ())
	pSourceTask = pSourceTask->caller ();

//  ... use it or create a new one, ...
    m_pOutputBuffer.setTo (
	pSourceTask->domainIs (pThisDomain) ? pSourceTask->m_pOutputBuffer.operator-> () : new VOutputBuffer (
	    pThisDomain, pSourceTask->getOutputBuffer ()
	)
    );

//  ... push that valid output buffer up the stack, ...
    VTask* pDestinationTask = m_pCaller;
    while (
	pDestinationTask &&
	pDestinationTask->domainIs (pThisDomain) &&
	pDestinationTask->m_pOutputBuffer.referent () != m_pOutputBuffer
    ) {
	pDestinationTask->m_pOutputBuffer.setTo (m_pOutputBuffer);
	pDestinationTask = pDestinationTask->m_pCaller;
    }

//  ... and return:
    return m_pOutputBuffer;
}


/*********************************
 *----  Ring Array Creation  ----*
 *********************************/

VOutputBuffer::Cell** VOutputBuffer::newRings () {
    unsigned int iRingCount = cardinality ();

    if (iRingCount > 0) {
	size_t sAllocation = sizeof (Cell*) * iRingCount;
	m_pRingHeads = reinterpret_cast<Cell**>(UTIL_Malloc (sAllocation));
	memset (m_pRingHeads, 0, sAllocation);
    }
    return m_pRingHeads;
}


/*****************************
 *----  Cell Allocation  ----*
 *****************************/

VOutputBuffer::AllocatoR::CellBlock* VOutputBuffer::AllocatoR::allocate (
    Cell** ppRing, size_t sAllocation
)
{
//  Adjust the allocation to include the cell and alignment padding, ...
//  sAllocation = Cell::sizeFor (sAllocation);		// ... more accurate...
    sAllocation = Cell::legacySizeFor (sAllocation);	// ... but this is more forgiving.

//  ... find, ...
    CellBlock* pAllocatingBlock;
    for (
	pAllocatingBlock = m_pActiveBlock;
	pAllocatingBlock && pAllocatingBlock->cantAllocate (sAllocation);
	pAllocatingBlock = pAllocatingBlock->predecessor ()
    );

    if (IsNil (pAllocatingBlock) && IsntNil (m_pActiveBlock)) {
	for (
	    pAllocatingBlock = m_pActiveBlock->successor ();
	    pAllocatingBlock && pAllocatingBlock->cantAllocate (sAllocation);
	    pAllocatingBlock = pAllocatingBlock->successor ()
	);

	if (pAllocatingBlock)
	    m_pActiveBlock = pAllocatingBlock;
    }

//  ... or create, ...
    if (IsNil (pAllocatingBlock)) {
	// ... cap the default size of the next extent, ...
	if (m_sNextBlock > g_sLargestCellBlock)
	    m_sNextBlock = g_sLargestCellBlock;

	//  ... but create an extent large enough to fill the current need, ...
	size_t sExtent = V_Max (m_sNextBlock, sAllocation);
	pAllocatingBlock = m_pActiveBlock = new (sExtent) CellBlock (
	    m_pActiveBlock, sExtent
	);

	//  ... and make adjustments for next time, ...
	m_sAllBlocks += sExtent;
	if (m_sAllBlocks > 8 * m_sNextBlock) {
	    g_iLocalExtentExpansionCount++;
	    m_sNextBlock *= 2;
	}
    }

//  ... a cell block capable of allocating the cell:
    return pAllocatingBlock->allocate (ppRing, sAllocation);
}


/***************************************
 *----  Expand-On-Overflow Format  ----*
 ***************************************/

/*---------------------------------------------------------------------------
 *****  '<stdarg.h>' based routine to print a collection of arguments to a
 *****  message ring.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  '*m_ppRingHead'.
 *	size			- an integer containing the number of
 *                                characters to print.  It may be a guess,
 *				  if so it must be larger than necessary.
 *	format			- a C style format string.
 *	pArgList		- a <stdarg.h> argument pointer positioned at
 *				  the first parameter to the format string.
 *
 *****/
void VOutputBuffer::vprintf (Cell** ppRing, size_t size, char const* format, va_list pArgList) {
//  Allocate and initialize the cell, ...  *****/
    AllocatoR::CellBlock* pCellBlock = m_pAllocator->allocate (ppRing, size + 1);
    size_t length = STD_vsprintf ((*ppRing)->content (), format, pArgList);

//  ... check for an invalid size which can cause buffer overflow, ...
    if (size < length)
	raiseSizeException (size, length);

//  ... return any excess allocation, ...
    pCellBlock->reclaim (size + 1, length + 1);
}

/*---------------------------------------------------------------------------
 *****  Routine to print a collection of arguments to a message ring.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  m_ppRingHead.
 *	size			- an integer containing the number of
 *                                characters to print.  It may be a guess,
 *				  if so it must be larger than necessary.
 *	format			- a C style format string.
 *	...			- appropriate arguments to the format string.
 *
 *****/
void __cdecl VOutputGenerator::printf (size_t size, char const* format, ...) const {
    V_VARGLIST (pArgList, format);
    vprintf (size, format, pArgList);
}


/*---------------------------------------------------------------------------
 *****  Routine to print a collection of arguments to a message ring.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  m_ppRingHead.
 *	size			- an integer containing the number of
 *                                characters to print.  It may be a guess,
 *				  if so it must be larger than necessary.
 *	format			- a C style format string.
 *	...			- appropriate arguments to the format string.
 *
 *****/
void VTask::outputBufferVPrintScalar (size_t size, char const* format, va_list pArgList) {
    if (isDivertingOutput ()) {
	VOutputGenerator iOutputGenerator (this);
	iOutputGenerator.vprintf (size, format, pArgList);
    } else {
	channel()->VPrint (size, format, pArgList);
    }
}

void __cdecl VTask::outputBufferPrintScalar (size_t size, char const* format, ...) {
    V_VARGLIST (pArgList, format);
    outputBufferVPrintScalar (size, format, pArgList);
}


/*************************************
 *----  Fill-On-Overflow Format  ----*
 *************************************/

/*---------------------------------------------------------------------------
 *****  '<stdarg.h>' based routine to print a collection of arguments to a
 *****  message ring.  If the outputted string is longer than than the
 *****  specified size ('fieldWidth') then the output will be replaced by
 *****  the 'overflowChar's.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  '*m_ppRingHead'.
 *	size			- an integer containing the maximum number of
 *                                characters that could be produced. It may
 *                                be a guess, if so it must be larger than
 *                                necessary.
 *	fieldWidth		- integer specifying the size that the
 *                                outputted string must fit in (not counting
 *                                the terminating '\0').
 *                                If the outputted string is longer than
 *                                'fieldWidth', the output will be replaced
 *                                by 'fieldWidth' 'overflowChar's.
 *	overflowChar		- the character to replace the output with
 *                                on overflow.
 *	format			- a C style format string.
 *	pArgList		- a <stdarg.h> argument pointer positioned at
 *				  the first parameter to the format string.
 *
 *****/
void VOutputBuffer::vprintf (
    Cell**		ppRing,
    size_t		size,
    int			fieldWidth,
    int			overflowChar,
    char const*		format,
    va_list		pArgList
) {
//  Allocate and initialize the cell, ...  *****/
    AllocatoR::CellBlock* pCellBlock = m_pAllocator->allocate (ppRing, size + 1);
    char *pContent = (*ppRing)->content ();
    size_t length = STD_vsprintf (pContent, format, pArgList);

//  ... check for an invalid size which can cause buffer overflow, ...
    if (size < length)
	raiseSizeException (size, length);

//  ... replace the output with the overflow character if necessary, ...
    if (length > (size_t)fieldWidth) {
	memset (pContent, overflowChar, fieldWidth);
        pContent [fieldWidth] = '\0';
	length = fieldWidth;
    }

//  ... return any excess allocation, ...
    pCellBlock->reclaim (size + 1, length + 1);
}


/*---------------------------------------------------------------------------
 *****  Routine to print a collection of arguments to a message ring.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  m_ppRingHead.
 *	size			- an integer containing the number of
 *                                characters to print.  It may be a guess,
 *				  if so it must be larger than necessary.
 *	fieldWidth		- integer specifying the size that the
 *                                outputted string but fit in (not counting
 *                                the terminating '\0').
 *                                If the outputted string is longer than
 *                                'fieldWidth', the output will be replaced
 *                                by 'fieldWidth' 'overflowChar's.
 *	overflowChar		- the character to replace the output with
 *                                on overflow.
 *	format			- a C style format string.
 *	...			- appropriate arguments to the format string.
 *
 *****/
void __cdecl VOutputGenerator::printf (
    size_t size, int fieldWidth, int overflowChar, char const* format, ...
) const {
    V_VARGLIST (pArgList, format);
    vprintf (size, fieldWidth, overflowChar, format, pArgList);
}


/**************************
 *----  Comma Format  ----*
 **************************/

/*---------------------------------------------------------------------------
 *****  Formats with commas and prints a number to a message ring.
 *****  If the outputted string is longer than than the specified
 *****  size the output will be replaced by the 'overflowChar's.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  '*m_ppRingHead'.
 *	size			- an integer specifying the maximum number of
 *                                characters that could be output using a
 *                                format made up of fieldSpecification and
 *                                precision to format number. This number is
 *                                used to allocate a temporary internal buffer.
 *				  This number may be a guess, if so it must be
 *                                larger than necessary.
 *	overflowChar		- the character to replace the output with
 *                                on overflow.
 *	fieldSpecification	- an integer specifing the format's
 *                                field width and justification.
 *	precision		- an integer specifing the number of decimals
 *                                to be printed.
 *	number 			- the number to output.
 *
 *  Returns:
 *	false if the number overflowed, true otherwise.
 *
 *  Notes:
 *	The number of characters output is always the abs (fieldSpecification).
 *	(Plus the '\0')
 *
 *****/
void VOutputBuffer::printWithCommas (
    Cell**		ppRing,
    size_t		size,
    int			overflowChar,
    int			fieldSpecification,
    int			precision,
    double		number
)
{
//  Allocate and initialize the cell, ...  *****/
    AllocatoR::CellBlock* pCellBlock = m_pAllocator->allocate (ppRing, size + 1);
    char *pContent = (*ppRing)->content ();
    UTIL_FormatNumberWithCommas (
	pContent, size, overflowChar, fieldSpecification, precision, number
    );

//  ... return any excess allocation, ...
    pCellBlock->reclaim (size + 1, strlen (pContent) + 1);
}


/***************************
 *----  String Format  ----*
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to print a string to a message ring.
 *
 *  Arguments:
 *	buffer			- the address of a task output buffer.  Output
 *				  will be appended to the ring identified by
 *				  '*m_ppRingHead'.
 *	string	    		- a C style format string.
 *
 *  Returns:
 *	The number of characters 'printed'.
 *
 *****/
void VOutputBuffer::putString (Cell** ppRing, char const* string)
{
//  Allocate and initialize the cell, ...  *****/
    m_pAllocator->allocate (ppRing, strlen (string) + 1);
    strcpy ((*ppRing)->content (), string);
}


/**************************
 *----  Broadcasting  ----*
 **************************/

/*---------------------------------------------------------------------------
 *****  '<stdarg.h>' based routine to print a collection of arguments to every
 *****  message ring in a task output buffer.
 *
 *  Arguments:
 *	size			- an integer containing the number of
 *                                characters to be printed - per ring.
 *				  It may be a guess, if so it must be
 *                                larger than necessary.
 *	format			- a C style format string.
 *	pArgList		- a <stdarg.h> argument pointer positioned at
 *				  the first parameter to the format string.
 *
 *****/
void VTask::vprintf (size_t size, char const* format, va_list pArgList) {
    if (isScalar ())
	outputBufferVPrintScalar (size, format, pArgList);
    else {
	/*****  initialize the cursor  *****/
	VOutputGenerator iOutputGenerator (this);

	/*****  send output to every pcell ring  *****/
	unsigned int elementsRemaining = cardinality ();
	while (elementsRemaining-- > 0) {
	    V::VArgList pArgListCopy (pArgList);
	    iOutputGenerator.vprintf (size, format, pArgListCopy);
	    iOutputGenerator.advance ();
	}
    }
}

/*---------------------------------------------------------------------------
 *****  Routine to print a collection of arguments to EVERY message ring in a
 *****  task output buffer.
 *
 *  Arguments:
 *	format			- a C style format string.
 *	size			- an integer containing the number of
 *                                characters to be printed - per ring.
 *                                It may be a guess, if so it must be
 *                                larger than necessary.
 *	...			- appropriate arguments to the format string.
 *
 *  Returns:
 *	The number of characters 'printed' per ring(from 'vsprintf') -
 *      this must be <= 'size'.
 *
 *****/
void __cdecl VTask::printf (size_t size, char const* format, ...) {
    V_VARGLIST (pArgList, format);
    vprintf (size, format, pArgList);
}


/***************************
 ***************************
 *****                 *****
 *****  Output Access  *****
 *****                 *****
 ***************************
 ***************************/

/*****************************
 *****  Character Count  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to count the number of characters contained in a task buffer.
 *
 *  Argument:
 *	buffer			- the address of the task output buffer to be
 *				  analyzed.
 *	delimiter		- a character which, when non-null, will be
 *				  used to mark the end of each output cell
 *				  in the task output buffer.
 *
 *  Returns:
 *	The number of characters contained in 'buffer'.
 *
 *****/
unsigned int VOutputBuffer::characterCount (char iDelimiter) const {
//  Minimally, each ring contributes a terminating null character, ...
    unsigned int iCharacterCount = cardinality ();

//  If there is output, count the cells and characters, ...
    if (hasOutput ())
    {
	unsigned int iCellCount = 0;
	unsigned int iRingCount = iCharacterCount;

	for (unsigned xRing = 0; xRing < iRingCount; xRing++)
	{
	    Cell const* const pStartingCell = m_pRingHeads[xRing];

	    if (pStartingCell)
	    {
		Cell const* pCurrentCell = pStartingCell;
		do {
		    iCellCount++;
		    iCharacterCount += strlen (pCurrentCell->content ());

		    pCurrentCell = pCurrentCell->successor ();
		} while (pStartingCell != pCurrentCell);
	    }
	}

	if (iDelimiter)
	    iCharacterCount += iCellCount;
    }

    return iCharacterCount;
}


/************************************
 *****  Copy Output To Channel  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to print the rings of a task output buffer in the proper order.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
void VOutputBuffer::copyOutputTo (IOMDriver* pOutputChannel) const {
//  If there is output, ...
    if (hasOutput ())
    {
    //  ... copy their delimited content to the result string, ...
	unsigned int iRingCount = cardinality ();

	for (unsigned int xRing = 0; xRing < iRingCount; xRing++)
	{
	    if (m_pRingHeads[xRing])
	    {
		Cell const* const pStartingCell = m_pRingHeads[xRing]->successor ();
		Cell const* pCurrentCell = pStartingCell;

		do {
		    if (SIGNAL_InInterruptState)
			return;

		    pOutputChannel->PutString (pCurrentCell->content ());

		    pCurrentCell = pCurrentCell->successor ();
		} while (pStartingCell != pCurrentCell);

	    }
	}
    }
}


/***********************************
 *****  Copy Output To String  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to print the rings of a task output buffer in the proper order.
 *
 *  Arguments:
 *	string			- the address of the string to receive the
 *				  output.
 *	delimiter		- a character which, when non-null, will be
 *				  used to mark the end of each output cell
 *				  in the task output buffer.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
void VOutputBuffer::copyOutputTo (char *pTarget, char iDelimiter) const {
//  If there are rings, then, ...
    if (hasOutput ())
    {
    //  ... copy their delimited content to the result string, ...
	unsigned int iRingCount = cardinality ();

	for (unsigned int xRing = 0; xRing < iRingCount; xRing++)
	{
	    if (m_pRingHeads[xRing])
	    {
		Cell const* const pStartingCell = m_pRingHeads[xRing]->successor ();
		Cell const* pCurrentCell = pStartingCell;

		do {
		    strcpy (pTarget, pCurrentCell->content ());
		    pTarget += strlen (pTarget);
		    if (iDelimiter)
			*pTarget++ = iDelimiter;

		    pCurrentCell = pCurrentCell->successor ();
		} while (pStartingCell != pCurrentCell);

	    }
	    *pTarget++ = '\0';
	}
    }

//  ... otherwise, just return a terminating null for each ring:
    else
	memset (pTarget, '\0', cardinality ());
}


/*********************************
 *****  Copy Output To File  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to print the rings of a task output buffer in the proper order.
 *
 *  Arguments:
 *	pStream			- the address of the FILE to receive the
 *				  output.
 *	pOmitString		- a set of characters which should not be 
 *				  copied to the output file.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
void VOutputBuffer::copyOutputTo (FILE *pStream, char const *pOmitString) const {
//  If there are rings, then, ...
    if (hasOutput ())
    {
    //  ... copy their content to the FILE sans occurrences of characters in omitString, ...
	unsigned int iRingCount = cardinality ();

	for (unsigned int xRing = 0; xRing < iRingCount; xRing++)
	{
	    if (m_pRingHeads[xRing])
	    {
		Cell const* const pStartingCell = m_pRingHeads[xRing]->successor ();
		Cell const* pCurrentCell = pStartingCell;

		do {
		    /*****  Check for the interrupt signal ... *****/
		    if (SIGNAL_InInterruptState)
			return;

		    char* pOutput = (char *)pCurrentCell->content ();
		    
		    for (unsigned int length = strlen (pOutput),
			     span = 0;
			 length > 0;
			 length -= span)
		    {
			pOutput += span;
			span = strcspn (pOutput, pOmitString);
			pOutput [span] = '\0';
			if (span < length) span++;
			fputs (pOutput, pStream);
		    }

		    pCurrentCell = pCurrentCell->successor ();
		} while (pStartingCell != pCurrentCell);

	    }
	}
    }
}


/**************************
 *****  Ring Splicer  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Macro used by sendOutputToCaller to splice a source P-Cell ring
 *        into a target P-Cell ring.
 *
 *  Arguments:
 *	rpTargetRingHead		- the address of the target ring head.
 *	pSourceRing		- the address of the source ring head.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *****/
#define ROSpliceRings(rpTargetRingHead, pSourceRing) {\
    /*****  If there is a source ring, ...  *****/\
    if (pSourceRing) {\
	Cell* pTargetRingHead_ = rpTargetRingHead;\
    /*****  and a target ring, append the source ring to the target, ...  *****/\
	if (pTargetRingHead_) {\
	    Cell* pFirstCellInTargetRing = pTargetRingHead_->successor ();\
	    pTargetRingHead_->m_pSuccessor = pSourceRing->successor ();\
	    pSourceRing->m_pSuccessor = pFirstCellInTargetRing;\
	}\
	rpTargetRingHead = pSourceRing;\
    }\
}


/***********************************
 *****  Move Output To Caller  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the output accumulated in one buffer to another
 *****	buffer.
 *
 *  Arguments:
 *	pTarget			- the address of the receiving task output
 *				  buffer.
 *	pCallerSubset		- an optional ('Nil' if absent) link
 *				  constructor specifying the message rings of
 *				  the target to be updated.  If not specified,
 *				  all message rings of target will be updated
 *				  from their positionally corresponding source
 *				  rings.
 *	pCallerReordering	- an optional ('Nil' if absent) CPD for the
 *				  reference u-vector specifying the distribution
 *				  re-ordering of the message rings in source.
 *				  If both 'pCallerSubset' and 'pCallerReordering'
 *				  are specified, 'pCallerReordering' is applied
 *				  before 'pCallerSubset'.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void VOutputBuffer::moveOutputTo (
    VOutputBuffer* pTarget, rtLINK_CType* pCallerSubset, M_CPD* pCallerReordering
)
{
#   define nilReferenceHandler(c, n, r)

//  If there is nothing to return, do nothing, ...
    if (this == pTarget || IsNil (m_pRingHeads)) {
    }
//  moving across allocatoRs is not allowed ...
    else if (m_pAllocator.referent () != pTarget->m_pAllocator) ERR_SignalFault (
	EC__InternalInconsistency,
	"VOutputBuffer::moveOutputTo:  Attempt to move output across allocators!!"
    );
//  ... otherwise, if both an order and subset were specified, ...
    else if (IsntNil (pCallerSubset) && IsntNil (pCallerReordering)) {
    //  ... make sure that the target has rings, ...
	Cell** pTargetRings = pTarget->rings ();

    //  ... align and validate the subset and order, ...
	pCallerSubset->Align ();
	rtREFUV_AlignForAssign (
	    pCallerSubset->PPT (), -1, pCallerReordering, ptoken (), -1
	);

    //  ...  invert the order, ...
	VCPDReference pOrderInverse (0, rtREFUV_Flip (pCallerReordering));

    //  ...  initialize cached state, ...
	rtREFUV_ElementType* pOrderInverseArray = rtREFUV_CPD_Array (pOrderInverse);

    //  ...traverse the subset, ...
#	define repeatedReferenceHandler(c, n, r) {\
	    while (n-- > 0) {\
		ROSpliceRings (\
		    pTargetRings[r], m_pRingHeads[pOrderInverseArray[c]]\
		);\
		c++;\
	    }\
	}
#	define rangeReferenceHandler(c, n, r) {\
	    while (n-- > 0) {\
		ROSpliceRings (\
		    pTargetRings[r], m_pRingHeads[pOrderInverseArray[c]]\
		);\
		r++;\
		c++;\
	    }\
	}
	rtLINK_TraverseRRDCList (
	    pCallerSubset,
	    nilReferenceHandler,
	    repeatedReferenceHandler,
	    rangeReferenceHandler
	);
#	undef repeatedReferenceHandler
#	undef rangeReferenceHandler

    //  ... and clean-up:
	discardRings ();
    }

//  ... otherwise, if just the subset was specified, ...
    else if (IsntNil (pCallerSubset)) {
    //  ... make sure that the target has rings, ...
	Cell** pTargetRings = pTarget->rings ();

    //  ... align and validate the subset, ...
	pCallerSubset->Align ();

    //  ...initialize the source cursor, ...
	unsigned int xRing = 0;

    //  ...traverse the subset, ...
#	define repeatedReferenceHandler(c, n, r) {\
	    while (n-- > 0) {\
		Cell* pSourceRing = m_pRingHeads[xRing++];\
		ROSpliceRings (pTargetRings[r], pSourceRing);\
	    }\
	}

#	define rangeReferenceHandler(c, n, r) {\
	    while (n-- > 0) {\
		Cell* pSourceRing = m_pRingHeads[xRing++];\
		ROSpliceRings (pTargetRings[r], pSourceRing);\
		r++;\
	    }\
	}
	rtLINK_TraverseRRDCList (
	    pCallerSubset,
	    nilReferenceHandler,
	    repeatedReferenceHandler,
	    rangeReferenceHandler
	);
#	undef repeatedReferenceHandler
#	undef rangeReferenceHandler

    //  ... and clean-up:
	discardRings ();
    }

//  ... otherwise, if just the order was specified...  *****/
    else if (IsntNil (pCallerReordering)) {
    //  ... make sure that the target has rings, ...
	Cell** pTargetRings = pTarget->rings ();

    //  ... align and validate the order, ...
	rtREFUV_AlignForAssign (
	    pTarget->ptoken (), -1, pCallerReordering, ptoken (), -1
	);

    //  ... initialize cached state, ...
	rtREFUV_ElementType* arrayp = rtREFUV_CPD_Array (pCallerReordering);

    //  ... traverse the ordering, ...
	for (
	    unsigned int xRing = 0;
	    xRing < UV_CPD_ElementCount (pCallerReordering);
	    arrayp++, xRing++
	)
	{
	    ROSpliceRings (pTargetRings[*arrayp], m_pRingHeads[xRing]);
	}

    //  ... and clean-up:
	discardRings ();
    }

//  ... otherwise, if just a non-empty target was specified, ...
    else if (pTarget->hasOutput ()) {
    //  ... get the target rings, ...
	Cell** pTargetRings = pTarget->rings ();

    //  ... splice the source rings into their corresponding target rings, ...
	unsigned int xRing = cardinality ();
	while (xRing-- > 0) {
	    Cell* pSourceRingHead = m_pRingHeads[xRing];
	    ROSpliceRings (pTargetRings[xRing], pSourceRingHead);
	}

    //  ... and clean-up:
	discardRings ();
    }

//  ... otherwise, nothing but the source is present.
    else {
    //  ... so just move the source rings to the target.
	pTarget->m_pRingHeads = m_pRingHeads;
	m_pRingHeads = 0;
    }

//  Finally, if the target isn't diverting output, display its output:
    if (pTarget->isntDivertingOutput ())
	pTarget->moveOutputToChannel ();

#   undef nilReferenceHandler
}


/**********************************
 **********************************
 *****			      *****
 *****  Exception Generation  *****
 *****                        *****
 **********************************
 **********************************/

void VOutputBuffer::raiseSizeException (size_t size, size_t length) const {
    raiseException (
	EC__InternalInconsistency,
	"LIO_vprintf size problem: size=%d but real size=%d",
	size,
	length
    );
}


/***************************************
 ***************************************
 *****                             *****
 *****  Facility Debugger Methods  *****
 *****                             *****
 ***************************************
 ***************************************/

IOBJ_DefineMethod (SetInitialExtentSizeDM)
{
    int input = IOBJ_NumericIObjectValue (parameterArray[0], int);

    if (input > 0)
	VOutputBuffer::setCellAllocatorInitialExtentSizeTo ((size_t)input);
    else {
        IO_printf ("Invalid input parameter: (%d)\n", input);
	IO_printf ("CellBlock Size not changed\n");
    }

    return self;
}

IOBJ_DefineMethod (SetMaximumExtentSizeDM)
{
    int input = IOBJ_NumericIObjectValue (parameterArray[0], int);

    if (input > 0)
	VOutputBuffer::setCellAllocatorMaximumExtentSizeTo ((size_t)input);
    else {
        IO_printf ("Invalid input parameter: (%d)\n", input);
	IO_printf ("CellBlock Size not changed\n");
    }

    return self;
}

IOBJ_DefineUnaryMethod (DisplayStateDM)
{
    UTIL_DisplayExpression (%u, VOutputBuffer::cellAllocatorInitialExtentSize ());
    UTIL_DisplayExpression (%u, VOutputBuffer::cellAllocatorMaximumExtentSize ());
    return self;
}

IOBJ_DefineUnaryMethod (DisplayCountsDM)
{
    UTIL_DisplayExpression (%u, VOutputBuffer::cellAllocatorLocalExtentExpansionCount  ());
    UTIL_DisplayExpression (%u, VOutputBuffer::cellAllocatorGlobalExtentExpansionCount ());
    UTIL_DisplayExpression (%u, VOutputBuffer::cellAllocatorCurrentAllocationAmount ());
    return self;
}


/*********************************
 *********************************
 *****                       *****
 *****  Facility Definition  *****
 *****                       *****
 *********************************
 *********************************/

FAC_DefineFacility
{
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("displayState"		, DisplayStateDM)
	IOBJ_MDE ("displayCounts"		, DisplayCountsDM)
	IOBJ_MDE ("setInitialExtentSize:"	, SetInitialExtentSizeDM)
	IOBJ_MDE ("setMaximumExtentSize:"	, SetMaximumExtentSizeDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (LIO);
    FAC_FDFCase_FacilityDescription ("Logical I/O");
    FAC_FDFCase_FacilityMD (methodDictionary);
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  lio.c 1 replace /users/mjc/rsystem
  860717 17:44:44 mjc Add 'Logical I/O facility

 ************************************************************************/
/************************************************************************
  lio.c 2 replace /users/mjc/rsystem
  860718 14:12:33 mjc Return to library for 'spe'

 ************************************************************************/
/************************************************************************
  lio.c 3 replace /users/mjc/system
  860720 22:21:57 mjc Changed definition of task output buffers

 ************************************************************************/
/************************************************************************
  lio.c 4 replace /users/mjc/system
  860721 09:52:18 mjc Made 'LIO_v{printf,broadcast}' return a value

 ************************************************************************/
/************************************************************************
  lio.c 5 replace /users/spe/system
  860729 16:13:56 spe Added pcell, pcellSpace and buffer functions to the system.

 ************************************************************************/
/************************************************************************
  lio.c 6 replace /users/spe/system
  860801 13:58:02 spe Made UVAppend more efficient.

 ************************************************************************/
/************************************************************************
  lio.c 7 replace /users/mjc/system
  861002 18:02:59 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  lio.c 7 replace /users/mjc/system
  861002 19:11:43 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  lio.c 8 replace /users/mjc/system
  861016 12:56:41 mjc Fixed bug in 'SpliceRings' that wiped target ring when no source was present

 ************************************************************************/
/************************************************************************
  lio.c 9 replace /users/mjc/system
  861125 17:33:55 mjc Added metrics to return last and max size of P-Cell space, added code to free P-Cell space on reset

 ************************************************************************/
/************************************************************************
  lio.c 10 replace /users/jad/system
  870317 12:30:12 jad check interrupt flag in print loop

 ************************************************************************/
/************************************************************************
  lio.c 11 replace /users/mjc/translation
  870524 09:39:23 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  lio.c 12 replace /users/jad/system
  880706 11:31:59 jad Fix Lio Print Buffer Size

 ************************************************************************/
/************************************************************************
  lio.c 13 replace /users/jad/system
  880805 13:38:55 jad util.h

 ************************************************************************/
/************************************************************************
  lio.c 14 replace /users/jck/system
  880915 12:19:46 jck Fixed a non-portable use of stdarg

 ************************************************************************/
/************************************************************************
  lio.c 15 replace /users/jck/system
  881104 10:07:21 jck Modified internal management of buffers

 ************************************************************************/
/************************************************************************
  lio.c 16 replace /users/m2/backend
  890503 15:23:32 m2 IO_puts(), STD_printf(), and va_end() fixes

 ************************************************************************/

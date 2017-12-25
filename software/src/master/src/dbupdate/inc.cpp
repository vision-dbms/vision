/******************************************************
 ************		inc.c	    *******************
 ******************************************************/

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "VkMemory.h"

#include "readlayout.h"
#include "sigHandler.h"
#include "gopt.h"
#include "m.d"
#include "ps.d"
#include "rtype.d"
#include "uvector.d"
#include "OSdump.h"
#include "OSstring.h"
#include "RTptoken.d"
#include "RTundefuv.d"
#include "RTvector.d"
#include "RTvstore.d"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

#ifndef DBUPDATE
#define DBUPDATE
#endif

/****** Defines *****/

#define MAXBUFSIZE	524268
#define OUTADDSIZE	1024
#define MAXLINE		1024

#define DATAUSEG	1
#define UNDEFUSEG	0

#define     AddToInstallationSpec(filename)\
    fprintf (Ispec, "%s\n", filename)

#define TracingMemoryMovement 0

/****** Externals ******/
PublicFnDecl FILE *fopen();


/****** Globals ******/
PrivateVarDef FILE *		Logfile;
PrivateVarDef FILE *		Ispec;
PrivateVarDef LAYOUT *		Layout;

PrivateVarDef char *		OutBuffer;

PrivateVarDef int		OutBufSize,
				OutSegment = 0;

PrivateVarDef int		StringStorePToken;

union BUFPTR {
    int *		intptr;
    float *		fltptr;
    double *		dblptr;
    char *		chrptr;
    M_CPreamble*	preptr;
};

PrivateVarDef BUFPTR Outptr;


/********************************************
 **********	NA Table	*************
 ********************************************/

PrivateVarDef struct FloatNA_t {
    float value;
    int segment;
} FloatNATable[] = {
	LAY_FloatNA, 0,
	-0.002f, 2,
	-0.003f, 3,
	-0.004f, 4,
	-0.008f, 5,
};

PrivateVarDef struct DoubleNA_t {
    double value;
    int segment;
} DoubleNATable[] = {
	LAY_DoubleNA, 0,
	-0.002, 2,
	-0.003, 3,
	-0.004, 4,
	-0.008, 5,
};

PrivateVarDef int NACount;

#define IntegerNA_value		    LAY_IntegerNA

#define FloatNA_value(n)	    (FloatNATable[n].value)
#define FloatNA_seg(n)		    (FloatNATable[n].segment)


#define DoubleNA_value(n)	    (DoubleNATable[n].value)
#define DoubleNA_seg(n)		    (DoubleNATable[n].segment)

PrivateVarDef char const *StringNA_value =
	"                                                               ";


/*---------------------------------------------------------------------------
 *****  Internal utility to accumulate a segment checksum.
 *
 *  Arguments:
 *	checksum		- the address of the checksum to update.
 *	structure		- the address of the structure to process.
 *	structureSize		- the size of the structure in bytes.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void AccumulateChecksum (
    unsigned long *		checksum,
    void *			structure,
    unsigned long		structureSize
)
{
    unsigned long		*wp, *wl;

    wp = (unsigned long *)structure;
    wl = wp
       + (structureSize + sizeof (unsigned long) - 1) / sizeof (unsigned long);

    while (wp < wl) *checksum ^= *wp++;
}


/*---------------------------------------------------------------------------
 *****  Macro to manage the reallocation of OutBuffer
 *
 *  Arguments:
 *	None.
 *
 *  Syntactic Context:
 *	Compond Statement.
 *
 *****/
#define growOutBuffer(bytes)\
{\
    char *ptr;\
    int headerOffset = (int)((char *)headerPtr - OutBuffer);\
\
    OutBufSize += bytes;\
    ptr = (char*)realloc(OutBuffer, OutBufSize);\
    if (ptr == NULL)\
    {\
	display_error("Realloc error on OutBuffer");\
	exit (ErrorExitValue);\
    }\
    OutBuffer = ptr;\
    *(char **)&headerPtr = OutBuffer + headerOffset;\
    Outptr.chrptr = OutBuffer + outsize;\
}


/***********************************************************
 **********	Macro to write to OutBuffer	************
 ***********************************************************
 *
 *  Arguments:
 *	data	-  A pointer to the beginning of an array of data to be copied
 *		   to the OutBuffer.
 *	bytes	-  The number of bytes to be copied.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *  Notes:
 *	This macro will realloc OutBuffer if it is too small. It maintains
 *  the global variable OutBufSize in order to determine the necessity of a
 *  reallocation.  It adjusts the global ptr Outptr to reflect any possible
 *  movement of OutBuffer resulting from reallocation.  A local within this
 *  macro's scope, headerPtr, is similarly maintained.
 *
 ***************************************************/

#define writeToBuffer(data, bytes)\
{\
    int ibytes = (int)bytes;\
    int outsize = (int)(Outptr.chrptr - OutBuffer);\
\
    if (outsize + ibytes >= OutBufSize) growOutBuffer (ibytes)\
    memcpy(Outptr.chrptr, data, ibytes);\
    Outptr.chrptr += ibytes;\
}


/*******************************************************************
 **********	Macro to write pMapEntry to OutBuffer	************
 *******************************************************************
 *
 *  Arguments:
 *	vpos	-  The first field of the PMapEntry -- an integer representing
 *		   the starting position of this range in the vector.
 *	useg	-  The second field of the PMapEntry -- an integer representing
 *		   the usegment where the range of data referenced by this
 *		   entry resides.
 *	upos	-  The third field of the PMapEntry -- an integer representing
 *		   the starting position of this range in the usegment.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *  Notes:
 *	This macro will realloc OutBuffer if it is too small. It maintains
 *  the global variable OutBufSize in order to determine the necessity of a
 *  reallocation.  It adjusts the global ptr Outptr to reflect any possible
 *  movement of OutBuffer resulting from reallocation.  A local within this
 *  macro's scope, headerPtr, is similarly maintained.
 *
 ***************************************************/

#define writePMapEntry(vpos, useg, upos)\
{\
    int outsize = (int)(Outptr.chrptr - OutBuffer);\
    if ( outsize + (int)(3 * sizeof(int)) > OutBufSize) growOutBuffer (OUTADDSIZE);\
    *Outptr.intptr++ = vpos;\
    *Outptr.intptr++ = useg;\
    *Outptr.intptr++ = upos;\
}


/***************************************************************************
 **********	Macro to write shifDescriptor to OutBuffer	************
 ***************************************************************************
 *
 *  Arguments:
 *	origin	-  The first field of the shift descriptor
 *	shift	-  The second field of the shift descriptor
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *  Notes:
 *	This macro will realloc OutBuffer if it is too small. It maintains
 *  the global variable OutBufSize in order to determine the necessity of a
 *  reallocation.  It adjusts the global ptr Outptr to reflect any possible
 *  movement of OutBuffer resulting from reallocation.  A local within this
 *  macro's scope, headerPtr, is similarly maintained.
 *
 ***************************************************/

#define writeShiftDescriptor(origin, shift)\
{\
    int outsize = (int)(Outptr.chrptr - OutBuffer);\
    if ( outsize + (int)(2 * sizeof(int)) > OutBufSize)\
	growOutBuffer (OUTADDSIZE)\
    *Outptr.intptr++ = origin;\
    *Outptr.intptr++ = shift;\
}


/*******************************************************************
 **********	Routine to write uvectors to OutBuffer	************
 *******************************************************************
 *
 *  Arguments:
 *	vector		-  A pointer to a structure of type osDUMP_Vector
 *			   whose usegments are being 'written' to OutBuffer.
 *	datavalues	-  A pointer to the array of values to be placed in
 *			   this vector's data usegment.
 *	datasize	-  The number of bytes to be 'written'.
 *	uSegCounts	-  An array of integers providing the element count
 *			   for each usegment.
 *
 *  Returns:
 *	Nothing.
 *
 *  Notes:
 *	The Incorporator makes two assumptions about its vectors.
 *		(1)  There is only one data usegment per vector.  The rest
 *	     would be various flavors of undefined.
 *		(2)  The data usegment is usegment 1.
 *
 **********************************************************/
PrivateFnDef void WriteUvectors (
    osDUMP_VectorType *		vector,
    char *			datavalues,
    int				datasize,
    int *			uSegCounts
)
{
    for (int useg = 0; useg < osDUMP_UsegCount(vector); useg++) {
		/*** ptr to old vector or ptoken ***/
        M_CPreamble* inputPtr = osDUMP_GetUvectorPtr (vector, useg);
		/*** ptr to new container in OutBuffer ***/
	M_CPreamble* headerPtr = Outptr.preptr;
	PS_CT_SetSegmentAndOffset (
	    osDUMP_ContainerTable, osDUMP_CTIndex,
	    OutSegment, (int)((char *)headerPtr - OutBuffer)
	);
		/*** ptr to old vector or ptoken ***/
	M_CPreamble* ptokenPtr = osDUMP_GetUsegPtokenPtr(vector, useg);

	if (useg != DATAUSEG)
	{
	    if ((RTYPE_Type)M_CPreamble_RType (inputPtr) != RTYPE_C_UndefUV)
	    {
		writeToBuffer(inputPtr,UV_HeaderSize + M_SizeOfPreambleType);
		M_CPreamble_FixSize (headerPtr);

		if (!osDUMP_Replacing) writeToBuffer (
		    UV_DataPtr (inputPtr),
		    UV_DataSize (inputPtr, rtPTOKEN_ElementCount (ptokenPtr))
		);

		UV_SetElementCount(headerPtr, uSegCounts [useg]);
		UV_SetContainerSize(headerPtr);
		writeToBuffer(
		    headerPtr, UV_UV_Granularity (UV_Content (headerPtr))
		);
		writeToBuffer(headerPtr, M_SizeOfEndMarker);
	    }
	    else
	    {
		writeToBuffer(inputPtr, rtUNDEFUV_ContainerSize);
		M_CPreamble_FixSizes (headerPtr);
		UV_SetElementCount(headerPtr, uSegCounts [useg]);
		UV_SetContainerSize (headerPtr);
	    }
	}
	else
	{
	    writeToBuffer(inputPtr,UV_HeaderSize + M_SizeOfPreambleType);
	    M_CPreamble_FixSize (headerPtr);

	    if (!osDUMP_Replacing) writeToBuffer (
		UV_DataPtr (inputPtr),
		UV_DataSize (inputPtr, rtPTOKEN_ElementCount (ptokenPtr))
	    );

	    if (LAY_type (Layout) == LAY_string ||
	        LAY_type (Layout) == LAY_code
	    ) UV_SetRefPToken (headerPtr, StringStorePToken);

	    UV_SetElementCount(headerPtr, uSegCounts [useg]);
	    writeToBuffer(datavalues, datasize);
	    UV_SetContainerSize(headerPtr);
	    writeToBuffer(
		headerPtr, UV_UV_Granularity (UV_Content (headerPtr))
	    );
	    writeToBuffer(headerPtr, M_SizeOfEndMarker);
	}
/**** Circumvent Vision's property of leave usegment's unaligned  *****/
	M_POP currentPTokenPOP;
	M_POP_D_ContainerIndex (currentPTokenPOP) = osDUMP_CTIndex;
	M_POP_D_ObjectSpace (currentPTokenPOP)    = osDUMP_SpaceName;
	UV_SetPToken (headerPtr, currentPTokenPOP);

	headerPtr = Outptr.preptr;
	writeToBuffer (ptokenPtr, rtPTOKEN_HeaderSize);
	M_CPreamble_FixSize (headerPtr);

	rtPTOKEN_SetElementCount(headerPtr, uSegCounts [useg]);
	writeToBuffer (headerPtr, M_SizeOfEndMarker);
	PS_CT_SetSegmentAndOffset (
	    osDUMP_ContainerTable, osDUMP_CTIndex,
	    OutSegment, (int)((char *)headerPtr - OutBuffer)
	);
    }
}


/************************************************************
 ************		Macros to write pmaps 	*************
 ***********************************************************
 *
 *  Arguments:
 *	newData		-  A pointer to the incoming data for a vector.
 *	dataUSeg	-  A pointer to a buffer where the content for
 *			   usegment one (the data usegment) is to be stored.
 *	stringLength	-  The granularity of the data. Used only for string
 *			   items.
 *	uSegCounts	-  An array of integers where the element counts for
 *			   the various usegments are maintained.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *  Notes:
 *	These macros squeeze the 'not available' data out of the incoming data
 *	arrays and build the vector's positional map in OutBuffer.
 *
 ************************************************************/
#define WriteVectorContentString(newData, dataUSeg, stringLength, uSegCounts)\
	for (limit = vpos + LAY_keyCount (Layout); vpos < limit; vpos++)\
	{\
	    if (memcmp (newData, StringNA_value, stringLength))\
	    {\
		segment = DATAUSEG;\
		*dataUSeg.intptr++ =\
		    osSTRING_LocateOrAdd (newData, stringLength);\
	    }\
	    else segment = UNDEFUSEG;\
\
	    if (segment != currentSeg)\
	    {\
		currentSeg = segment;\
		writePMapEntry(vpos, segment, uSegCounts[segment]);\
		pMapEntryCount++;\
	    }\
\
	    newData += granularity;\
	    uSegCounts[segment]++;\
	}

#define WriteVectorContentCode(newData, dataUSeg, stringLength, uSegCounts)\
	for (limit = vpos + LAY_keyCount (Layout); vpos < limit; vpos++)\
	{\
	    if (memcmp (newData, StringNA_value, stringLength))\
	    {\
		segment = DATAUSEG;\
		*dataUSeg.intptr++ =\
		    osSTRING_LocateOrAddCode (newData, stringLength);\
	    }\
	    else segment = UNDEFUSEG;\
\
	    if (segment != currentSeg)\
	    {\
		currentSeg = segment;\
		writePMapEntry(vpos, segment, uSegCounts[segment]);\
		pMapEntryCount++;\
	    }\
\
	    newData += granularity;\
	    uSegCounts[segment]++;\
	}

#define WriteVectorContentInteger(newData, dataUSeg, stringLength, uSegCounts)\
	for (limit = vpos + LAY_keyCount (Layout); vpos < limit; vpos++)\
	{\
	    if (*(int *)newData == IntegerNA_value)\
	        segment = UNDEFUSEG;\
	    else\
	    {\
		segment = DATAUSEG;\
		*dataUSeg.intptr++ = *(int *) newData;\
	    }\
	    if (segment != currentSeg)\
	    {\
		currentSeg = segment;\
		writePMapEntry(vpos, segment, uSegCounts[segment]);\
		pMapEntryCount++;\
	    }\
\
	    newData += granularity;\
	    uSegCounts[segment]++;\
	}

#define WriteVectorContentDouble(newData, dataUSeg, vector, uSegCounts)\
	for (limit = vpos + LAY_keyCount (Layout); vpos < limit; vpos++)\
	{\
	    doubleValue = *(double *) newData;\
	    segment = -1;\
	    for (\
		ii = 0;\
		ii < NACount && DoubleNA_seg (ii) < osDUMP_UsegCount (vector);\
		ii++\
	    )\
	    {\
		if (doubleValue == DoubleNA_value(ii))\
		{\
		    if ((RTYPE_Type)osDUMP_RType (vector, DoubleNA_seg (ii)) ==\
			RTYPE_C_UndefUV)\
			segment = DoubleNA_seg(ii);\
		    else segment = UNDEFUSEG;\
		    break;\
		}\
	    }\
	    if (segment == -1)\
	    {\
		segment = DATAUSEG;\
		*dataUSeg.dblptr++ = doubleValue;\
	    }\
	    if (segment != currentSeg)\
	    {\
		currentSeg = segment;\
		writePMapEntry(vpos, segment, uSegCounts[segment]);\
		pMapEntryCount++;\
	    }\
\
	    newData += granularity;\
	    uSegCounts[segment]++;\
	}

#define WriteVectorContentFloat(newData, dataUSeg, vector, uSegCounts)\
	for (limit = vpos + LAY_keyCount (Layout); vpos < limit; vpos++)\
	{\
	    value = *(float *) newData;\
	    segment = -1;\
	    for (\
		ii = 0;\
		ii < NACount && FloatNA_seg (ii) < osDUMP_UsegCount (vector);\
		ii++\
	    )\
	    {\
		if (value == FloatNA_value(ii))\
		{\
		    if ((RTYPE_Type) osDUMP_RType (vector, FloatNA_seg (ii)) ==\
			RTYPE_C_UndefUV)\
			segment = FloatNA_seg(ii);\
		    else segment = UNDEFUSEG;\
		    break;\
		}\
	    }\
\
 	    if (segment == -1)\
	    {\
		segment = DATAUSEG;\
		*dataUSeg.fltptr++ = value;\
	    }\
\
	    if (segment != currentSeg)\
	    {\
		currentSeg = segment;\
		writePMapEntry(vpos, segment, uSegCounts[segment]);\
		pMapEntryCount++;\
	    }\
\
	    newData += granularity;\
	    uSegCounts[segment]++;\
	}


/*******************************
 *****  Utility Functions  *****
 *******************************/

PrivateFnDef void DetermineNACount (
    osDUMP_VectorType *		dbVector
)
{
    int i;

    for (i = 2; i < osDUMP_UsegCount (dbVector); i++)
        if (osDUMP_RType (dbVector, i) != (int) RTYPE_C_UndefUV)
	    break;
    NACount = i >= 5 ? 5 : 1;
}

PrivateVarDef int *ObtainUCountArray (
    osDUMP_VectorType *		dbVector,
    char *			Unused(itemName)
)
{
    int *ucount, i;

    ucount = (int *)malloc (osDUMP_UsegCount (dbVector) * sizeof (int));

    if (ucount == NULL)
    {
    	display_error ("Error allocating ucount array");
	exit (ErrorExitValue);
    }
    for (i = 0; i < osDUMP_UsegCount (dbVector); i++) ucount[i] = 0;

    return ucount;
}


/*---------------------------------------------------------------------------
 *****  Routine to update the positional map and the uniform segments of a
 *****  vector for a particular data item.
 *
 *  Arguments:
 *	itemName	-  A character string identifying the item in the
 *			   data base.
 *	dbVector	-  A pointer to be set to the vector in the system
 *			   dump associated with itemName.
 *	incomingData	-  A pointer to the array of data to be used to
 *			   update the vector.
 *	granularity	-  The granularity of the incoming data.
 *	pTokenPOP	-  An integer representation of the vector's new
 *			   terminal ptoken.
 *
 *  Returns:
 *	true if a fatal error is encountered.
 *	false if successfully updated.
 *---------------------------------------------------------------------------
 */
PrivateFnDef int UpdateVector (
    char *			itemName,
    osDUMP_VectorType **	dbVector,
    char *			incomingData,
    int				granularity,
    int				pTokenPOP
)
{
    int			ii, vpos,
			limit, lbytes,
			segment,
			currentSeg,	/*** current segment type	***/
			pMapEntryCount,	/*** pMapEntrys			***/
			*ucount;	/*** current pos in uvector	***/
    float		value;
    double		doubleValue;
    BUFPTR		actualData;
    char *		actualValues;   /*** array of data ***/

    actualValues = (char*)malloc (
	LAY_keyCount (Layout) * (
	    granularity > sizeof (int) ? granularity : sizeof (int)
	) + 1
    );
    if (actualValues == NULL)
    {
    	display_error ("Error allocating actualValues buffer");
	return true;
    }

    actualData.chrptr = actualValues;

/**** write vector header *****/
	/*** ptr to old vector or ptoken ***/
    M_CPreamble* inputPtr = osDUMP_GetVectorPtr (dbVector, itemName, NilOf (size_t *));
    if (IsNil (inputPtr)) {
	fprintf(Logfile, "%-15s: not found in system dump --", itemName);
	free (actualValues);
	return false;
    }

    ucount = ObtainUCountArray (*dbVector, itemName);
    DetermineNACount (*dbVector);

	/*** ptr to new container in OutBuffer ***/
    M_CPreamble* headerPtr = Outptr.preptr;
    writeToBuffer(inputPtr, (rtVECTOR_HeaderSize + M_SizeOfPreambleType));
    M_CPreamble_FixSize (headerPtr);

    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex,
	OutSegment, (int)((char *)headerPtr - OutBuffer)
    );

    if (!osDUMP_Replacing)
    {
        currentSeg = rtVECTOR_FinalSegment (inputPtr);
	vpos = rtVECTOR_ElementCount (inputPtr);
	pMapEntryCount = rtVECTOR_PMapCount (inputPtr);
	writeToBuffer (
	    rtVECTOR_PMapPtr (inputPtr), rtVECTOR_PMapSize (inputPtr)
	);
	for (ii = 0; ii < osDUMP_UsegCount (*dbVector); ii++) {
		/*** ptr to old vector or ptoken ***/
	    M_CPreamble* ptokenPtr  = osDUMP_GetUsegPtokenPtr (*dbVector, ii);
	    ucount [ii] = rtPTOKEN_ElementCount (ptokenPtr);
	}
    }
    else
    {
        vpos = pMapEntryCount = 0;
	currentSeg = -1;
    }

/**** write out pmaps, and save data values for uvectors ****/
    switch (LAY_type(Layout))
    {
    case LAY_string:
	WriteVectorContentString (
	    incomingData, actualData, granularity, ucount
	);
	break;
    case LAY_code:
	WriteVectorContentCode (
	    incomingData, actualData, granularity, ucount
	);
	break;
    case LAY_integer:
	WriteVectorContentInteger (
	    incomingData, actualData, granularity, ucount
	);
	break;
    case LAY_double:
	WriteVectorContentDouble (
	    incomingData, actualData, *dbVector, ucount
	);
	break;
    case LAY_float:
	WriteVectorContentFloat (
	    incomingData, actualData, *dbVector, ucount
	);
	break;
    default:
	fprintf (stderr, "Unknown Data Type\n");
	return true;
    }

    writePMapEntry(vpos, -1, 0);

    rtVECTOR_SetPMapSize(headerPtr, pMapEntryCount);
    rtVECTOR_SetContainerSize(headerPtr);
    rtVECTOR_SetPTokenPOP (headerPtr, pTokenPOP);

/**** write vector tail ***/
    writeToBuffer(rtVECTOR_Tail(inputPtr), rtVECTOR_TailSize(inputPtr));
    writeToBuffer(headerPtr, M_SizeOfEndMarker);

/**** do all uvectors for this vector ****/
    lbytes =  ((int)(actualData.chrptr - actualValues));
    WriteUvectors (*dbVector, actualValues, lbytes, ucount);

    fprintf (Logfile, "%-15s:%6d%8d%9d", itemName,
	     pMapEntryCount, ucount[0], ucount[1]);
    for (ii = 2; ii < osDUMP_UsegCount (*dbVector); ii++)
        fprintf (Logfile, "%6d", ucount [ii]);
    fprintf (Logfile, " -- ");

    free (actualValues);
    free (ucount);

    return false;
}


/*---------------------------------------------------------------------------
 *****  Routine to convert the current table's ptoken into a shift ptoken
 *****  whose next generation is a newly created base ptoken.
 *
 *  Argument:
 *	index	- the address of an integer where the new base ptoken's
 *		  container table index will be deposited.
 *
 *  Returns:
 *	The new base ptoken's POP as an integer.
 *
 *****/
PrivateFnDef int UpdateTablePToken (
    int *			index,
    int *			ptIndex
)
{
    M_CPreamble *tablePtr = osDUMP_GetTablePtr ();
    M_CPreamble *ptokenPtr = osDUMP_GetPtokenPtr (tablePtr);

    *ptIndex = (int)osDUMP_CTIndex;

    int oldElementCount = rtPTOKEN_ElementCount (ptokenPtr);
    int newElementCount = LAY_keyCount (Layout) +
		      (osDUMP_Replacing
		       ?    0
		       :    oldElementCount);

/*****  Get the container table entry for the new terminal ptoken *****/
    M_POP newPTokenPOP;
    M_POP_D_ContainerIndex (newPTokenPOP) = *index = osDUMP_GetNewCTEntry ();
    M_POP_D_ObjectSpace (newPTokenPOP) = osDUMP_SpaceName;

/*****  Build the new terminal PToken  *****/
    M_CPreamble* headerPtr = Outptr.preptr;
    writeToBuffer (ptokenPtr, rtPTOKEN_ContainerSize);
    M_CPreamble_FixSizes (headerPtr);

    rtPTOKEN_SetElementCount (headerPtr, newElementCount);
    M_POP_D_AsInt (
	M_CPreamble_POP ((M_CPreamble*)headerPtr)
    ) = M_POP_D_AsInt (newPTokenPOP);

    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, *index, OutSegment,
	(int)((char *)headerPtr - OutBuffer)
    );

/*****  Deposit it into the table  *****/
    headerPtr = Outptr.preptr;
    writeToBuffer (tablePtr, rtVSTORE_ContainerSize (tablePtr));
    M_CPreamble_FixSizes (headerPtr);

    rtVSTORE_ReplaceRowPToken (headerPtr, M_POP_D_AsInt (newPTokenPOP));

    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_TableIndex, OutSegment,
	(int)((char *)headerPtr - OutBuffer)
    );

/*****  Transform the current ptoken into a shift ptoken  *****/
    headerPtr = Outptr.preptr;
    writeToBuffer (ptokenPtr, rtPTOKEN_HeaderSize);
    M_CPreamble_FixSize (headerPtr);

    rtPTOKEN_ConvertToShift (headerPtr, newPTokenPOP);
    if (!osDUMP_Replacing || oldElementCount == 0)
    {
        writeShiftDescriptor (oldElementCount, LAY_keyCount (Layout));
	rtPTOKEN_SetAltCount (headerPtr, 1);
    }
    else
    {
	writeShiftDescriptor (oldElementCount, oldElementCount * (-1));
	writeShiftDescriptor (0, LAY_keyCount (Layout));
	rtPTOKEN_SetAltCount (headerPtr, 2);
    }
    rtPTOKEN_SetContainerSize (headerPtr);

    writeToBuffer (headerPtr, M_SizeOfEndMarker);
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, OutSegment,
	(int)((char *)headerPtr - OutBuffer)
    );

    return M_POP_D_AsInt (newPTokenPOP);
}


/*********************************************************************
 *************		ConvertDataFileToSegment	**************
 *********************************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to handle the mapping, buffering and modification of a
 *****  standardized data file, turning it, in place, into an Object Space
 *****  segment.
 *
 *  Arguments:
 *	fileprefix	- a character string identifying the prefix of the
 *			  file to be converted.
 *	fileNumber	- the number of the current fileprefix being processed
 *	totalFileCount	- the total number of files converted prior to the
 *			  processing of this fileprefix.
 *	updatingTablePToken  -  a flag indicating whether the current table's
 *			  ptoken needs processing.
 *	newPTokenPOP	- the address of an integer representation of the
 *			  current ptoken pop for the table being processed.
 *			  If updatingTablePToken, a new ptoken will be created
 *			  and deposited at this address.
 *
 *  Returns:
 *	true if a fatal error is encountered.
 *	false if successfully converted.
 *
 *****/
PrivateFnDef int ConvertDataFileToSegment (
    char const*			fileprefix,
    int				fileNumber,
    int				Unused(totalFileCount),
    int				updatingTablePToken,
    int *			newPTokenPOP
)
{
    VkMemory			fileMapping;
    char			filename [MAXPATHLENGTH],
				*input,
				*fileptr,
				*oldfileptr,
				*readPtr,
				*writePtr,
				*endPtr,
				*origEnd;
    int				seg,
				adjustment,
				itemNumber,
				bufferOffset,
				fileOffset,
				indx,
				uctr,
				newPTokenIndex,
				outsize,
				origSegmentSize = 0,
				filesize,
				granularity,
				count,
				needsTruncating;
    unsigned long		checksum;
    int				fd;
    osDUMP_VectorType		*vector;

    sprintf(filename, "%s%d%s", fileprefix, fileNumber, ".dat");
    if (NULL == (Layout = read_layout(filename)))
    {
	fprintf (stderr, "%s: ", filename);
	display_error("Error reading layout file");
	exit (ErrorExitValue);
    }
    if (OutSegment++ == 0) OutSegment += (int)osDUMP_ContSegment;

    checksum = 0;

    for (
	itemNumber = 0;
        itemNumber < LAY_itemCount(Layout) && !osDUMP_Replacing;
	itemNumber++
    ) if (
	osDUMP_GetVectorPtr (
	    &vector, LAY_item (Layout, itemNumber), (size_t *)&origSegmentSize
	)
    ) break;

    filesize = atoi (GOPT_GetValueOption ("XFactor")) * LAY_fileSize(Layout)
	+ MAXBUFSIZE
	+ origSegmentSize;
    if (filesize < LAY_fileSize(Layout))
	filesize = LAY_fileSize(Layout);
    if (filesize < sizeof (PS_SH))
	filesize = sizeof (PS_SH);

    if (!fileMapping.Map (filename, 0, filesize, VkMemory::Share_Private)) {
	fprintf (stderr, "%s: ", filename);
	display_error("Error mapping data file");
	return true;
    }
    fileptr	= (char*)fileMapping.RegionAddress ();
    filesize	= V_Max (LAY_fileSize(Layout), sizeof (PS_SH));

    input = (char*)malloc (LAY_vectorSize(Layout));

    granularity = LAY_granule (Layout);
    readPtr = fileptr + LAY_header(Layout);
    writePtr = fileptr + sizeof(PS_SH);
    origEnd = endPtr = (fileptr + filesize);

    for (itemNumber = 0; itemNumber < LAY_itemCount(Layout); itemNumber++)
    {
        memcpy(input, readPtr, LAY_vectorSize(Layout));

	readPtr += LAY_vectorSize(Layout);
	Outptr.chrptr = OutBuffer;
	if (updatingTablePToken)
	{
	    *newPTokenPOP = UpdateTablePToken (&newPTokenIndex, &indx);
	}

	if (UpdateVector (
		LAY_item (Layout, itemNumber),
		&vector,
		input,
		granularity,
		*newPTokenPOP
	    )
	) return true;
	outsize = (int) (Outptr.chrptr - OutBuffer);
	AccumulateChecksum (&checksum, OutBuffer, outsize);

	if (outsize <= (readPtr - writePtr)) {
	    fileOffset = (int)(writePtr - fileptr);
	    memcpy(writePtr, OutBuffer, outsize);
	    writePtr += outsize;
	}
	else {
	    filesize += outsize;

	    if (filesize > (int)fileMapping.RegionSize ()) {
		oldfileptr = fileptr;
		fileMapping.Destroy ();
		if (!fileMapping.Map (filename, 0, filesize + outsize, VkMemory::Share_Private)) {
		    fprintf (stderr, "%s: ", filename);
		    display_error("Error extending data file");
		    return true;
		}
		fileptr = (char*)fileMapping.RegionAddress ();
		adjustment = fileptr - oldfileptr;
		writePtr += adjustment;
		readPtr += adjustment;
		endPtr += adjustment;
		origEnd += adjustment;
#if TracingMemoryMovement
		fprintf (
		    Logfile, "\n\t remapped data file (%d) -- ", adjustment
		);
#endif
	    }
	    fileOffset = (int)(endPtr - fileptr);
	    memcpy(endPtr, OutBuffer, outsize);
	    endPtr += outsize;
	}

 /**** fix offsets for all CT entries generated ****/
	if (updatingTablePToken)
	{
/****   The original, updated ptoken  ****/
	    PS_CT_GetSegmentAndOffset (osDUMP_ContainerTable, indx,
				       seg, bufferOffset);
	    PS_CT_SetSegmentAndOffset (osDUMP_ContainerTable, indx,
				       seg, (bufferOffset + fileOffset));
/****	The new terminal ptoken	****/
	    PS_CT_GetSegmentAndOffset (osDUMP_ContainerTable, newPTokenIndex,
				       seg, bufferOffset);
	    PS_CT_SetSegmentAndOffset (osDUMP_ContainerTable, newPTokenIndex,
				       seg, (bufferOffset + fileOffset));

/****	The current table	****/
	    PS_CT_GetSegmentAndOffset (osDUMP_ContainerTable,osDUMP_TableIndex,
				       seg, bufferOffset);
	    PS_CT_SetSegmentAndOffset (osDUMP_ContainerTable,osDUMP_TableIndex,
				       seg, (bufferOffset + fileOffset));
	    updatingTablePToken = false;
	}
	if (vector)
	{
	    indx = osDUMP_VectorIndex(vector);
	    PS_CT_GetSegmentAndOffset (
		osDUMP_ContainerTable, indx, seg, bufferOffset
	    );
	    PS_CT_SetSegmentAndOffset (
		osDUMP_ContainerTable, indx, seg, (bufferOffset + fileOffset)
	    );
	    for (uctr = 0; uctr < osDUMP_UsegCount(vector); uctr++)
	    {
		indx = osDUMP_UvectorIndex(vector, uctr);
		PS_CT_GetSegmentAndOffset (
		    osDUMP_ContainerTable, indx, seg, bufferOffset
		);
		PS_CT_SetSegmentAndOffset (
		    osDUMP_ContainerTable,
		    indx,
		    seg,
		    (bufferOffset + fileOffset)
		);
		indx = osDUMP_UPtokenIndex(vector, uctr);
		PS_CT_GetSegmentAndOffset (
		    osDUMP_ContainerTable, indx, seg, bufferOffset
		);
		PS_CT_SetSegmentAndOffset (
		    osDUMP_ContainerTable, indx,
		    seg, (bufferOffset + fileOffset)
		);
	    }	/*** for each uvector ***/
	}   /*** for each vector ***/

	fprintf(Logfile, "Finished\n");
    }	/*** for each item in the data file ****/

    free(input);
#if TracingMemoryMovement
    fprintf(Logfile, "\n%s...oe:%p,wp:%p,ep:%p,fp:%p\n",
	    filename, origEnd, writePtr, endPtr, fileptr);
#endif
/**** fill in gap if there is one *****/
    if ((fileOffset = (int)(origEnd - writePtr)) > 0)
    {
	outsize = (int)(endPtr - origEnd);
	endPtr = (writePtr + outsize);
	filesize = (endPtr - fileptr);
#if TracingMemoryMovement
	fprintf(Logfile, "             os:%d, fs:%d\n", outsize, filesize);
#endif
	if (outsize > 0)
	{
#ifdef sun
#define BoundaryAddress ((char *)0x80000000)
// if the move is overlapping AND across this boundary, Solaris botches it ....
// ... sept 1998
	    if (outsize > fileOffset &&
		BoundaryAddress > writePtr &&
		BoundaryAddress <= origEnd)
	    {
		int firstPart = (int)(BoundaryAddress - writePtr);
		memmove (writePtr, origEnd, firstPart);
		memmove (BoundaryAddress,
			 firstPart + origEnd,
			 outsize - firstPart);
	    }
	    else
#endif
		memmove(writePtr, origEnd,  outsize);
 /**** fix offsets for all CT entries generated ****/
	    count = PS_CT_EntryCount (osDUMP_ContainerTable);
	    for (indx = 0; indx < count; indx++) {
		PS_CTE *ctep = &PS_CT_Entry (osDUMP_ContainerTable, indx);
		if (ctep->isInUse ()) {
		    ctep->getSegmentAndOffset (seg, bufferOffset);
		    if (seg == OutSegment && bufferOffset > ((int)(writePtr - fileptr)))
			ctep->setSegmentAndOffset (seg, (bufferOffset - fileOffset));
		}
	    }

	}
	else if (outsize < 0)
	{
	    fprintf (stderr,
		     "Error: Inconsistent state. origEnd:%p; endPtr:%p\n",
		     origEnd, endPtr);
	    return true;
	}
    }

/*****  Enter the segment in the list to be installed  *****/
    AddToInstallationSpec (filename);

    PS_SH_Reserved	  ((PS_SH*)fileptr) = 0;
    PS_SH_DataFormat	  ((PS_SH*)fileptr) = Vk_DataFormatNative;
    PS_SH_SoftwareVersion ((PS_SH*)fileptr) = PS_SoftwareVersion;
    PS_SH_Checksum	  ((PS_SH*)fileptr) = checksum;
    PS_SH_CTOffset	  ((PS_SH*)fileptr) =
    PS_SH_CTSize	  ((PS_SH*)fileptr) = 0;

    filesize = endPtr - fileptr;
    needsTruncating = (int)fileMapping.RegionSize () > filesize;
    fileMapping.Destroy ();

    if (needsTruncating)
    {
    	if (-1 == (int)(fd = open(filename, O_RDWR, 0)))
	{
	    fprintf (stderr, "%s: ", filename);
	    display_error("Error opening data file");
	    return true;
	}
    	if (-1 == ftruncate(fd, filesize))
	{
	    fprintf (stderr, "%s: ", filename);
	    display_error("Error truncating data file");
	    return true;
	}
    	if (-1 == close(fd))
	{
	    fprintf (stderr, "%s: ", filename);
	    display_error("Error closing data file");
	    return true;
	}
    }

    return false;
}


PrivateFnDef void OutputFinalSegment (
    char const*			fileprefix,
    int				currentFileCount
)
{
    VkMemory			fileMapping;
    char			*filePtr,
				*fileMark,
				*endPtr,
				filename [MAXPATHLENGTH];
    long			filesize = sizeof (PS_SH);
    int				temp;
    int				fd;
    unsigned long		checksum;

    sprintf (filename, "%s%d.dat", fileprefix, currentFileCount);
    if (-1 == (int)(fd = open (filename, O_RDWR | O_CREAT, 0640)))
    {
	fprintf (stderr, "%s: ", filename);
	display_error("Error creating final segment");
	exit (ErrorExitValue);
    }
    close (fd);

    if (!fileMapping.Map (
	    filename, 0, sizeof (PS_SH)
		+ osSTRING_SizeOfStringStore  (&temp)
		+ osDUMP_SizeOfContainerTable (),
	    VkMemory::Share_Private
	)
    )
    {
	fprintf (stderr, "%s: ", filename);
	display_error("Error mapping final segment");
	exit (ErrorExitValue);
    }
    filePtr = (char*)fileMapping.RegionAddress ();

    checksum = 0;
    endPtr = fileMark = filePtr + filesize;
    osSTRING_WriteStringStore (
	fileprefix, filePtr, &endPtr, &filesize, ++OutSegment
    );
    AccumulateChecksum (&checksum, fileMark, endPtr - fileMark);

    fileMark = endPtr;
    osDUMP_WriteContainerTable (
	filePtr, &endPtr, &filesize, OutSegment
    );
    AccumulateChecksum (&checksum, fileMark, endPtr - fileMark);

/*****  Enter the segment in the list to be installed  *****/
    AddToInstallationSpec (filename);

    PS_SH_Reserved	  ((PS_SH*)filePtr) = 0;
    PS_SH_DataFormat	  ((PS_SH*)filePtr) = Vk_DataFormatNative;
    PS_SH_SoftwareVersion ((PS_SH*)filePtr) = PS_SoftwareVersion;
    PS_SH_Checksum	  ((PS_SH*)filePtr) = checksum;

    fileMapping.Destroy ();
}


#define AscertainNumAndIdOfInputFiles(keyname, fileCount, fileNum) {\
    char buffer[MAXLINE], *ptr;\
    int i;\
    FILE *fd;\
\
    if (NULL == (fd = fopen(keyname, "r")))\
    {\
	fprintf (stderr, "%s: ", keyname);\
	display_error("Error opening key file");\
	exit (ErrorExitValue);\
    }\
\
    fgets(buffer, MAXLINE, fd);\
    fclose (fd);\
    sscanf(buffer, "%d %d %d", &i, &i, &fileCount);\
    ptr = buffer;\
\
    for (i = 0; i < 3;)\
        if (*ptr++ == ' ') i++;\
\
    fileNum = (int *) malloc (fileCount * sizeof (int));\
    if (fileNum == NULL)\
    {\
    	display_error ("Error allocating file number buffer");\
	exit (ErrorExitValue);\
    }\
    for (i = 0; i < fileCount; i++)\
    {\
        fileNum[i] = atoi(ptr++);\
	ptr = strchr(ptr, ' ');\
    }\
}


PrivateFnDef int DetermineNumOfTablesToProcess (
    char const*			fileprefix,
    int  *			numTables
)
{
    char filename [MAXPATHLENGTH];
    int x;
    FILE *layoutfp;

    sprintf(filename,  "%s%s", fileprefix, ".lay");
    if (NULL == (layoutfp = fopen(filename, "r")))
    {
	fprintf (stderr, "%s: ", filename);
        display_error("Error opening layoutfile");
	exit (ErrorExitValue);
    }
    fscanf (
	layoutfp, "%s %d %d %d %d %d",
	filename, &x, &x, &x, &x, numTables
    );

    fclose (layoutfp);
    return *numTables;
}


PrivateFnDef void SetUp (
    char const*			fileprefix,
    int *			numTables,
    int				initial
)
{
    char filename [MAXPATHLENGTH];

    if (initial)
    {
    /*****  Set up error reporting on the reception of signals.
	    also ignore quits,interrupts and hangups
     *****/
	if (1 != set_signals (handle_signal))
	  display_error("Error setting up signal handlers\n");

    /*****  Open up the output log  *****/
	if (NULL == (Logfile = fopen("inc.log", "w")))
	{
	    display_error("Error opening error log");
	    exit (ErrorExitValue);
	}
	setbuf (Logfile, NULL);

    /***** allocate the initial holding buffer  *****/
	OutBufSize = MAXBUFSIZE;
	if (NULL == (OutBuffer = (char*)malloc(OutBufSize)))
	{
	    display_error("Error allocating output buffer");
	    exit (ErrorExitValue);
	}

    /*****  open the data description file  *****/
	sprintf (filename, "%s.ispec", fileprefix);
	if (NULL == (Ispec = fopen (filename, "w")))
	{
	    fprintf (stderr, "%s: ", filename);
	    display_error ("Error creating ISpec file");
	    exit (ErrorExitValue);
	}
	fprintf (Ispec, "XUISpecFile:%04ld:%04ld:%04d\n",
					      osDUMP_SpaceName,
					      osDUMP_ContSegment + 1,
					      0);
    }

    DetermineNumOfTablesToProcess (fileprefix, numTables);
}


PrivateFnDef void IncorporateData (
    char const*			fileprefix,
    int *			currentFileCount,
    int				numTables,
    int				totalFileCount
)
{
    int file,
	fileCount,
	*fileNum,
	pTokenPOP,
	needToUpdatePtoken,
        table;

    char filename[MAXPATHLENGTH];

    if (totalFileCount == 0) osSTRING_ReadStringStore (
	fileprefix, (M_POP*)&StringStorePToken
    );

/*****  Loop through the tables, processing each one  *****/
    for (*currentFileCount = 0, table = 0;
         table < numTables;
	 table++, *currentFileCount += fileCount)
    {
/*****  Determine which files contain data for this table  *****/
	sprintf(filename, "%s%d%s", fileprefix, table, ".key");
	AscertainNumAndIdOfInputFiles (filename, fileCount, fileNum);

	sprintf(filename, "%s%d%s", fileprefix, table, ".tab");
	if (osDUMP_ReadDump(filename))
	    exit (ErrorExitValue);

	needToUpdatePtoken = true;

	for (file = 0; file < fileCount; file++)
	{
	    if (ConvertDataFileToSegment
	    		(fileprefix,
			 fileNum[file],
			 totalFileCount,
			 needToUpdatePtoken,
			 &pTokenPOP)
	    ) exit (ErrorExitValue);
	    needToUpdatePtoken = false;
	}	/*** for FileCount ***/
	free (fileNum);
	osDUMP_CleanupDump ();
    }	/*** for tables ***/
}


PrivateFnDef void CloseDataDescriptionFile (
    int				totalFileCount
)
{
    rewind (Ispec);
    fprintf (Ispec, "XUISpecFile:%04ld:%04ld:%04d",
			osDUMP_SpaceName,
			osDUMP_ContSegment + 1,
			totalFileCount
    );
    fclose (Ispec);
}


PrivateFnDef void DoInputConsistencyCheck (
    int				argc,
    char *			argv[]
)
{
    int argCount,
        i, j, k, ii,
	haventReadContTable = true,
	itemNumber,
	*fileNum,
	fileCount,
        numTables;
    osDUMP_VectorType
	*vector;
    char
	stringBuffer [MAXPATHLENGTH],
	type;
    RTYPE_Type
	rtype;

/****  Get the command line arguments  *****/
    GOPT_AcquireOptions (argc, argv);
    GOPT_SeekExtraArgument (0, 0);

    for (i = 1,
         argCount = GOPT_GetExtraArgCnt ();
	 i <= argCount;
	 i++)
    {
	char const* fileprefix = GOPT_GetExtraArgument();
	DetermineNumOfTablesToProcess (fileprefix, &numTables);
	for (j = 0; j < numTables; j++)
	{
	/*****  Determine which files contain data for this table  *****/
	    sprintf (stringBuffer, "%s%d%s", fileprefix, j, ".key");
	    AscertainNumAndIdOfInputFiles (stringBuffer, fileCount, fileNum);

	    sprintf (stringBuffer, "%s%d%s", fileprefix, j, ".tab");
	    if (osDUMP_ReadDump(stringBuffer))
		exit (ErrorExitValue);
	    if (haventReadContTable)
	    {
	        haventReadContTable = false;
		if (osDUMP_ReadContTable ())
		    exit (ErrorExitValue);
	    }

	    for (k = 0; k < fileCount; k++)
	    {
		sprintf (stringBuffer, "%s%d%s", fileprefix, fileNum [k], ".dat");
		if (NULL == (Layout = read_layout(stringBuffer)))
		{
		    fprintf (stderr, "%s%d: ", fileprefix, fileNum [k]);
		    display_error("Error reading layout file");
		    exit (ErrorExitValue);
		}

		for (itemNumber = 0, type = LAY_type (Layout);
		     itemNumber < LAY_itemCount(Layout);
		     itemNumber++)
		{
		    osDUMP_GetDumpEntry (&vector, LAY_item (Layout, itemNumber));

/****  If the item is not present in the data base, skip it  *****/
		    if (IsNil (vector)) continue;

/****
 *  Enforce requirement that enough usegments exist to store the data
 *  and at least one flavor of NA
 ****/
		    if (osDUMP_UsegCount (vector) < 2)
		    {
			fprintf (
			    stderr,
			    "There were less than two usegments in %s\n",
			    osDUMP_Item (vector)
			);
			exit (ErrorExitValue);
		    }
/****
 *  Enforce the requirement that the vector be aligned.
 ****/
		    osDUMP_GetVectorPtokenPtr (vector);
/****
 *  Enforce the requirement that the vector's usegments be aligned.
 ****/
		    for (ii = 0; ii < osDUMP_UsegCount (vector); ii++)
		        osDUMP_GetUsegPtokenPtr (vector, ii);

/****
 *  Enforce the requirement that the input data type matches the existing data
 *  type. Also that the data usegment resides where expected.
 ****/
		    switch (type)
		    {
		    case LAY_string:
		    case LAY_code:
		        rtype = RTYPE_C_RefUV;
		        break;
		    case LAY_integer:
		        rtype = RTYPE_C_IntUV;
		        break;
		    case LAY_float:
		        rtype = RTYPE_C_FloatUV;
		        break;
		    case LAY_double:
		        rtype = RTYPE_C_DoubleUV;
		        break;
		    default:
		        fprintf
			    (stderr,
			     "%s(%s): Unknown Data Type (%c)\n",
			     fileprefix,
			     LAY_item (Layout, itemNumber),
			     type);
			exit (ErrorExitValue);
		        break;
		    }
		    if (rtype != (RTYPE_Type) osDUMP_RType (vector, DATAUSEG))
		    {
		        fprintf
			    (stderr,
    ">>> ERROR <<< %s(%s): Data Type Inconsistency (Dump:%d,  Layout:%c)\n",
			     fileprefix,
			     LAY_item (Layout, itemNumber),
			     osDUMP_RType (vector, DATAUSEG),
			     type);
			 exit (ErrorExitValue);
		    }

/*****
 *  Determine that the general undefined usegment resides where expected.
 ****/
		    if (RTYPE_C_UndefUV !=
		        (RTYPE_Type) osDUMP_RType (vector, UNDEFUSEG))
		    {
			fprintf (
			    stderr,
			    ">>> ERROR <<< %s(%s): NA Storage Not Found\n",
			    fileprefix,
			    LAY_item (Layout, itemNumber)
			);
			exit (ErrorExitValue);
		    }
		}

	    }
	    free (fileNum);
	    osDUMP_CleanupDump ();
	}
    }
}


GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription	 ("XFactor", 'X', "IncXFactor", "2")
    GOPT_SwitchOptionDescription ("CkOnly" , 'c', NilOf (char *))
GOPT_EndOptionDescriptions;

#define firstIteration (i == 1)
#define lastIteration (i == argCount)

int main (
    int				argc,
    char *			argv[]
)
{
    int argCount,
        i,
	currentFileCount,
        totalFileCount,
        numTables;

/****
 *  First check the dump from Vision and the layout file for the input for
 *  mutual consistency.
 ****/
    DoInputConsistencyCheck (argc, argv);
    if (GOPT_GetSwitchOption ("CkOnly"))
        return NormalExitValue;

/****  Get the command line arguments  *****/
    GOPT_AcquireOptions (argc, argv);
    GOPT_SeekExtraArgument (0, 0);
    if (0 >= (argCount = GOPT_GetExtraArgCnt ()))
    {
	display_error("Must specify a file prefix");
	return ErrorExitValue;
    }

/*****  ... Global Reclamation Manager Initialization, ...  *****/
    VkMemory::StartGRM (argv[0]);

    char const *fileprefix;
    for (i = 1,
	 totalFileCount = 0;
	 i <= argCount;
	 i++,
	 totalFileCount += currentFileCount)
    {
	fileprefix = GOPT_GetExtraArgument();

	SetUp (fileprefix, &numTables, firstIteration);
	IncorporateData (fileprefix, &currentFileCount, numTables, totalFileCount);

    }

    OutputFinalSegment (fileprefix, currentFileCount);
    CloseDataDescriptionFile (++totalFileCount);

    fclose(Logfile);

    osDUMP_unmapSegments ();

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  inc.c 1 replace /users/jck/updates
  870129 13:27:19 jck First release of the incorporator

 ************************************************************************/
/************************************************************************
  inc.c 2 replace /users/jck/updates
  870202 13:33:47 jck Fixed miscellaneous bugs

 ************************************************************************/
/************************************************************************
  inc.c 3 replace /users/jck/updates
  870213 19:21:54 jck Modified output to logfile to include pmap and usegment counts

 ************************************************************************/
/************************************************************************
  inc.c 4 replace /users/jck/updates
  870219 07:24:04 jck Code Type Tree added

 ************************************************************************/
/************************************************************************
  inc.c 5 replace /users/jck/updates
  870219 14:28:13 jck converted M_OSSPreamble to PS_SegmentHeader

 ************************************************************************/
/************************************************************************
  inc.c 6 replace /users/mjc/dbupdate
  870601 19:46:22 mjc Added 'X' option / 'IncXFactor' environment variable to parameterize 'memvary' expansion factor

 ************************************************************************/
/************************************************************************
  inc.c 7 replace /users/jck/updates
  870618 10:28:22 jck Shortened names

 ************************************************************************/
/************************************************************************
  inc.c 8 replace /users/jck/updates
  870629 14:07:04 jck Implemented processing of multiple prefixes; reimplemented
allocation scheme for final segment; integrated avl search routines

 ************************************************************************/
/************************************************************************
  inc.c 9 replace /users/jck/updates
  870714 09:49:45 jck Corrected problem with processing data files smaller than 32 bytes

 ************************************************************************/
/************************************************************************
  inc.c 10 replace /users/jck/updates
  870730 08:51:30 jck Improved Heuristic used to determine maximum size for a new segment

 ************************************************************************/
/************************************************************************
  inc.c 11 replace /users/jck/updates
  870731 09:54:46 jck Make the occurence of a second non-undefined usegment generate a fatal error

 ************************************************************************/
/************************************************************************
  inc.c 12 replace /users/jck/updates
  870824 14:53:33 jck Fortified the incorporator

 ************************************************************************/
/************************************************************************
  inc.c 13 replace /users/jck/updates
  870904 09:36:45 jck Data file now remapped if memvary called with a size larger than the maxlength of the mapped file

 ************************************************************************/
/************************************************************************
  inc.c 14 replace /users/jck/updates
  871002 13:28:38 jck Moved check for '< 2 usegments' from readdump to UpdateVector

 ************************************************************************/
/************************************************************************
  inc.c 15 replace /users/jck/updates
  871019 10:03:15 jck Fixed problems occurring when an item was not present in the system's dump

 ************************************************************************/
/************************************************************************
  inc.c 16 replace /users/jck/updates
  871110 13:37:32 jck  (1) Changed MAXLINE from 200 to 1024
 (2) Changed calculation of output segment number (lines 818, 1046)
 (3) Added some error checking (line 990)
 (4) Decentralized the output of the ispec file
 (5) Made allocation

 ************************************************************************/
/************************************************************************
  inc.c 17 replace /users/jck/updates
  880112 12:56:24 jck Defined DBUPDATE

 ************************************************************************/
/************************************************************************
  inc.c 18 replace /users/m2/dbup
  880426 12:03:40 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  inc.c 19 replace /users/jck/updates
  880615 14:23:42 jck Hardened some soft spots in the incorporator

 ************************************************************************/
/************************************************************************
  inc.c 20 replace /users/jck/updates
  880628 08:51:31 jck Plugged another gap in the incorporator

 ************************************************************************/
/************************************************************************
  inc.c 21 replace /users/jck/updates
  880815 15:20:29 jck Altered interface to memvary and memfree

 ************************************************************************/

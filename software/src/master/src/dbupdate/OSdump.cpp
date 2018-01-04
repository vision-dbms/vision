/*****************************************
 **********	OSdump.c	**********
 *****************************************/

/*****  System  *****/
#include "Vk.h"
#include "VkMemory.h"

/*****  Vision  *****/
#include "sigHandler.h"

#include "m.d"
#include "ps.d"
#include "rtype.d"
#include "RTlstore.d"
#include "RTptoken.d"
#include "RTvstore.d"

/*****  Self  *****/
#include "OSdump.h"

/***** Constants  *****/
#define MAXLINE			2100
#define CTGrowthIncrement	512

/***** Public Variables *****/
					/**** Container Table ****/
PublicVarDef M_CPreamble*	osDUMP_ContainerTablePreamble;
PublicVarDef PS_CT*		osDUMP_ContainerTable;
PublicVarDef char		osDUMP_PathName[MAXPATHLENGTH],
				osDUMP_NDFPathName[MAXPATHLENGTH];
PublicVarDef long		osDUMP_ContSegment,
					/**** Container Table Index ****/
				osDUMP_CTIndex,
				osDUMP_TableIndex,
				osDUMP_SpaceName,
				osDUMP_Replacing;
PublicVarDef PS_Type_FO		osDUMP_NVDFO;

/***** Globals *****/
PrivateVarDef struct DumpTable_t {
    size_t		numEntries;
    osDUMP_VectorType	**entries;
} DumpTable;

#define Dump	    (DumpTable.entries)
#define NumEntries  (DumpTable.numEntries)
#define DumpGrowthIncrement  200

PrivateVarDef size_t		DumpCount;
PrivateVarDef size_t		ContainerTableSize;


/*---------------------------------------------------------------------------
 ***** sort OS dump by recordType, item to enable binary search
 *****/
PrivateFnDef int __cdecl vectorSort (
    osDUMP_VectorType **	v1,
    osDUMP_VectorType **	v2
)
{
    int test;

    return
	(test = osDUMP_RecordType (*v2) - osDUMP_RecordType (*v1)) != 0
	?  test
	:  strcmp(osDUMP_Item(*v1), osDUMP_Item(*v2));
}


#define ConditionallyGrowDump(entryNumber)\
{\
    if (NumEntries == 0)\
    {\
	NumEntries += DumpGrowthIncrement;\
        Dump = (osDUMP_VectorType **)\
	    malloc (NumEntries * sizeof (osDUMP_VectorType *));\
	if (IsNil (Dump))\
	{\
	    display_error ("Error Allocating Dump Table");\
	    return true;\
	}\
    }\
    while (entryNumber >= NumEntries) {\
	NumEntries += DumpGrowthIncrement;\
        Dump = (osDUMP_VectorType **)\
	    realloc (Dump, NumEntries * sizeof (osDUMP_VectorType *));\
	if (IsNil (Dump)) {\
	    display_error ("Error Reallocating Dump Table");\
	    return true;\
	}\
    }\
}


PublicFnDef int osDUMP_ReadDump (
    char *			dumpname
)
{
    char buffer[MAXLINE + 1];
    FILE *fptr;
    bool notDone = true;
    int i, recordType, usegCount;
    osDUMP_VectorType *vector;

    fptr = fopen(dumpname, "r");
    if (fptr == NULL)
    {
	fprintf (stderr, "%s: ", dumpname);
	display_error("Error opening system dump");
	return true;
    }

    fgets(buffer, MAXLINE, fptr);

    sscanf(
	buffer,
	"%d %s %ld %ld %ld %ld %s %u",
	&recordType,
	osDUMP_PathName,
	&osDUMP_SpaceName,
	&osDUMP_ContSegment,
	&osDUMP_TableIndex,
	&osDUMP_Replacing,
	osDUMP_NDFPathName,
	&osDUMP_NVDFO
    );

    if (recordType != HEADER)
    {
        fprintf (stderr, "Header Record Not Found\n");
	return true;
    }

    DumpCount = 0;
    fgets(buffer, MAXLINE, fptr);
    while (notDone) {
	sscanf(buffer, "%d %*s %*d %*d %d", &recordType, &usegCount);
	if (recordType != VECTOR && recordType != LSTORE)
	{
	    fprintf (
		stderr, "Error Parsing Dump . . .\n\tRecord: '%s'\n", buffer
	    );
	    return true;
	}

	ConditionallyGrowDump (DumpCount);
	if (IsNil (
		vector = Dump[DumpCount] = (osDUMP_VectorType *)malloc (
		    osDUMP_SizeofVectorType (usegCount)
		)
	    )
	)
	{
	    display_error("Error malloc'ing dump table entry");
	    return true;
	}

	sscanf(buffer, "%d %s %d %d %d", &osDUMP_RecordType(vector),
					 osDUMP_Item(vector),
					 &osDUMP_VectorIndex(vector),
					 &osDUMP_PtokenIndex(vector),
					 &osDUMP_UsegCount(vector));

	for (i = 0; i <= usegCount; i++) {
	    if (NULL == fgets(buffer, MAXLINE, fptr)) {
	        notDone = false;
		break;
	    }
	    sscanf(buffer, "%d", &recordType);
	    if (recordType != USEGMENT)
	        break;
	    sscanf(buffer, "%d %d %d %d", &recordType,
				  &osDUMP_RType (vector, i),
				  &osDUMP_UvectorIndex (vector, i),
				  &osDUMP_UPtokenIndex (vector, i));
	}
	if (i != usegCount)
	{
	    fprintf (stderr, "Number of usegments predicted for '%s' (%d)\n\
			     \tdoesn't agree with number dumped (%d)",
		     osDUMP_Item (vector), usegCount, i);
	    return true;
	}

	DumpCount++;
    }

    fclose(fptr);

/**** sort entries by item, period to allow binary ****/
    qsort ((char *)Dump, DumpCount, sizeof(osDUMP_VectorType *), (VkComparator)vectorSort);
    return false;
}


PublicFnDef void osDUMP_CleanupDump () {
    size_t i;

    for (i = 0; i < DumpCount; i++)
        free (Dump [i]);
}


/*---------------------------------------------------------------------------
 *****  Routine to read and store the contents of an object space's container
 *****  table.
 *
 *  Arguments:
 *	None.
 *	size		-  An address where the size in bytes of the container
 *		           table will be deposited.
 *
 *  Returns:
 *	true if fatal error. false otherwise.
 *
 *****/
PublicFnDef int osDUMP_ReadContTable () {
    char segmentName [MAXPATHLENGTH];
    int fd;
    PS_SH preamble;
    long containerTableOffset;

    sprintf(segmentName, "%s/%ld/%ld", osDUMP_PathName,
				     osDUMP_SpaceName,
				     osDUMP_ContSegment);

    if (-1 == (int)(fd = open(segmentName, O_RDONLY, 0))) {
	fprintf (stderr, "%s: ", segmentName);
	display_error("Error opening container table segment");
	return true;
    }
    if ( -1 == read(fd, &preamble, sizeof (PS_SH))) {
	fprintf (stderr, "%s: ", segmentName);
	display_error("Error reading OSpace preamble");
	close(fd);
	return true;
    }

    if (PS_SH_SoftwareVersion (&preamble) > PS_SoftwareVersion)
    {
	fprintf (
	    stderr,
	   "Version mismatch: Software version = %d, data base version = %d\n",
	    PS_SoftwareVersion,
	    PS_SH_SoftwareVersion (&preamble)
	);
	close (fd);
	return true;
    }

    ContainerTableSize	 = (size_t)PS_SH_CTSize	(&preamble);
    containerTableOffset = PS_SH_CTOffset	(&preamble);
    if (PS_SH_SoftwareVersion (&preamble) < PS_4GBContainers)
    {
	ContainerTableSize   += sizeof (unsigned long);
	containerTableOffset -= sizeof (unsigned long);
    }
    
    if (IsNil (osDUMP_ContainerTablePreamble = (M_CPreamble*)malloc (ContainerTableSize))) {
	display_error("Error malloc'ing container table");
	close(fd);
	return true;
    }
    osDUMP_ContainerTable = M_CPreamble_ContainerBaseAsType (
	osDUMP_ContainerTablePreamble, PS_CT
    );

    if ( -1 == lseek(fd, containerTableOffset, 0))
    {
	fprintf (stderr, "%s: ", segmentName);
	display_error("Error lseeking to container table");
	close(fd);
	return true;
    }
    if ( -1 == read(fd, osDUMP_ContainerTablePreamble, ContainerTableSize))
    {
	fprintf (stderr, "%s: ", segmentName);
	display_error("Error reading container table");
	close(fd);
	return true;
    }
    M_CPreamble_FixSize (osDUMP_ContainerTablePreamble);

    close(fd);
    return false;
}


/*---------------------------------------------------------------------------
 ******  Routine to retrieve the next free entry from containerTable,
 ******  growing the table if necessary.
 *
 *  Arguments:
 *	None
 *
 *  Returns:
 *	the index of the requested entry.
 *
 *****/
PublicFnDef long osDUMP_GetNewCTEntry () {
    long			index;
    PS_CTE *			entry;

    if (PS_CT_FreeListNil == PS_CT_FreeList (osDUMP_ContainerTable))
    {
        osDUMP_ContainerTablePreamble = (M_CPreamble*)realloc (
	    osDUMP_ContainerTablePreamble,
	    ContainerTableSize += CTGrowthIncrement * sizeof (PS_CTE)
	);
	if (IsNil (osDUMP_ContainerTablePreamble))
	{
	    display_error ("Realloc error on containerTable");
	    exit (ErrorExitValue);
	}
	M_CPreamble_NSize (osDUMP_ContainerTablePreamble) +=
	    CTGrowthIncrement * sizeof (PS_CTE);

	osDUMP_ContainerTable = M_CPreamble_ContainerBaseAsType (
	    osDUMP_ContainerTablePreamble, PS_CT
	);
	for (
	    index = PS_CT_EntryCount (osDUMP_ContainerTable),
	    PS_CT_EntryCount (osDUMP_ContainerTable) += CTGrowthIncrement;
	    index < PS_CT_EntryCount (osDUMP_ContainerTable);
	    index++
	)
	{
	    entry = &PS_CT_Entry (osDUMP_ContainerTable, index);
	    PS_CTE_IsCopy   (*entry) = false;
	    PS_CTE_IsFree   (*entry) = true;
	    PS_CTE_Segment  (*entry) = 0;
	    PS_CTE_NextFree (*entry) = PS_CT_FreeList (osDUMP_ContainerTable);
	    PS_CT_FreeList (osDUMP_ContainerTable) = (int)index;
	}
    }

    index = PS_CT_FreeList (osDUMP_ContainerTable);
    entry = &PS_CT_Entry (osDUMP_ContainerTable, index);

    PS_CT_FreeList (osDUMP_ContainerTable) = (int)PS_CTE_NextFree (*entry);
    PS_CTE_IsCopy (*entry) =
    PS_CTE_IsFree (*entry) = false;

    return index;
}


PublicFnDef int osDUMP_SizeOfContainerTable (
    void
)
{
    return ContainerTableSize + M_SizeOfEndMarker;
}

/*---------------------------------------------------------------------------
 *****  Macro to write the container table to the end of the Output File.
 *
 *  Arguments:
 *	fileptr		-  A pointer to the beginning of the mapped file
 *			   which will become an object space segment.
 *	endPtr		-  A pointer to the end of the same mapped file.
 *			   The container table will be written at this
 *			   location.
 *	filesize	-  The size in bytes of the mapped file. (Equal
 *			   to endPtr - fileptr).
 *	segment		-  The segment number that the container table is
 *			   being written into.
 *
 *  Returns
 *	Nothing.
 *
 *  Notes:
 *	This routine also inserts the offset for the container table and the
 *  table's size into the segment's header.
 *
 *****/
PublicFnDef void osDUMP_WriteContainerTable (
    char *			fileptr,
    char **			endPtr,
    long *			filesize,
    long			segment
)
{
    long			offset;

    offset = *filesize;
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, M_KnownCTI_ContainerTable, segment, offset
    );
    PS_CT_SegmentIndex (osDUMP_ContainerTable) = (int)segment;

    *filesize += osDUMP_SizeOfContainerTable ();
    memcpy (*endPtr, osDUMP_ContainerTablePreamble, ContainerTableSize);
    *endPtr += ContainerTableSize;
    memcpy (*endPtr, osDUMP_ContainerTablePreamble, M_SizeOfEndMarker);
    *endPtr += M_SizeOfEndMarker;

    PS_SH_CTOffset	  ((PS_SH*)fileptr) = offset;
    PS_SH_CTSize	  ((PS_SH*)fileptr) = ContainerTableSize;
}


PrivateVarDef VkMemory *SMV = NilOf (VkMemory*);

PublicFnDef void osDUMP_unmapSegments () {
    if (IsNil (SMV))
	return;

    for (int i = 0; i <= osDUMP_ContSegment; i++)
	SMV[i].Destroy ();
}


PrivateFnDef M_CPreamble* GetSegmentPtr (
    int				segment,
    int				offset,
    size_t *			size
)
{
    char filename[MAXPATHLENGTH];

    if (IsntNil (size))
	*size = 0;

    if (IsNil (SMV) && IsNil (SMV = new VkMemory [osDUMP_ContSegment + 1])) {
	display_error("Error allocating segment mapping vector");
	exit (ErrorExitValue);
    }

    if (segment > osDUMP_ContSegment) {
	fprintf (
	    stderr,
	    "*** Fatal Error: Non-existent segment referenced:%s/%ld/%d ***\n",
	    osDUMP_PathName, osDUMP_SpaceName, segment
	);
	exit (ErrorExitValue);
    }

    if (IsNil (SMV[segment].RegionAddress ())) {
	sprintf (
	    filename, "%s/%ld/%d", osDUMP_PathName, osDUMP_SpaceName, segment
	);

	if (!SMV[segment].Map (filename, true, VkMemory::Share_Private)) {
	    fprintf (stderr, "%s: ", filename);
	    display_error("Memallc error on segment");
	    return NilOf (M_CPreamble*);
	}
    }

    if (IsntNil (size))
	*size = SMV[segment].RegionSize ();

    return PS_SH_ContainerPreamble ((PS_SH*)SMV[segment].RegionAddress (), offset);
}


PublicFnDef M_CPreamble* osDUMP_GetColumnPtr (
    osDUMP_VectorType **	dbVector,
    char const *		item,
    size_t *			size,
    int				type,
    int				entryOnly
)
{
    osDUMP_VectorType		vector, *vectorPtr, **result;
    int				segment, offset;

    vector.recordType = type;
    strcpy(vector.item, item);
    vectorPtr = &vector;

    result = (osDUMP_VectorType **)bsearch(
	(char *)&vectorPtr, (char *)Dump, DumpCount, sizeof(osDUMP_VectorType *), (VkComparator)vectorSort
    );

    if (IsNil (result))
    {
        fprintf(stderr, "\nBsearch failed for %s", item);
	*dbVector = (osDUMP_VectorType *)NULL;
        return NULL;
    }
    else *dbVector = *result;

    if (!entryOnly)
    {
	osDUMP_CTIndex = osDUMP_VectorIndex(*dbVector);
	PS_CT_GetSegmentAndOffset (
	    osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
	);
	if (segment > osDUMP_ContSegment)
	{
	    fprintf (
	      stderr,
	      "Duplicate Item name (%s) found in layout file\n", item
	    );
	}
	return GetSegmentPtr (segment, offset, size);
    }
    else return NilOf (M_CPreamble*);
}


PublicFnDef M_CPreamble* osDUMP_GetTablePtr () {
    int				segment, offset;

    PS_CT_GetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_TableIndex, segment, offset
    );
    return GetSegmentPtr (segment, offset, NilOf (size_t *));
}


PublicFnDef M_CPreamble* osDUMP_GetPtokenPtr (
    M_CPreamble*		table
)
{
    int				segment, offset, index;
    M_CPreamble*		ptoken;
    RTYPE_Type			rtype;

    switch (rtype = (RTYPE_Type) M_CPreamble_RType (table))
    {
    case RTYPE_C_ValueStore:
        osDUMP_CTIndex = M_POP_D_ContainerIndex (rtVSTORE_RowPToken (table));
        break;
    case RTYPE_C_ListStore:
        osDUMP_CTIndex =  M_POP_D_ContainerIndex (rtLSTORE_RowPToken (table));
        break;
    default:
        fprintf (stderr, "Unknown table type %p:%d\n", table, rtype);
	exit (ErrorExitValue);
        break;
    }
    PS_CT_GetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    ptoken = GetSegmentPtr (segment, offset, NilOf (size_t *));

    if (rtPTOKEN_IsntTerminal (ptoken))
    {
        fprintf (stderr, "Table ptoken not current\n");
	exit (ErrorExitValue);
    }
    if (rtype == RTYPE_C_ListStore)
    {
        index = M_POP_D_ContainerIndex (rtLSTORE_ContentPToken (table));
	PS_CT_GetSegmentAndOffset (
	    osDUMP_ContainerTable, index, segment, offset
	);

	if (rtPTOKEN_IsntTerminal (
		GetSegmentPtr (segment, offset, NilOf (size_t *))
	    )
	)
	{
	    fprintf (stderr, "Content ptoken not current\n");
	    exit (ErrorExitValue);
	}

    }

    return ptoken;
}


PublicFnDef M_CPreamble* osDUMP_GetUvectorPtr (
    osDUMP_VectorType *		dbVector,
    int				i
)
{
    int				segment, offset;

    osDUMP_CTIndex = osDUMP_UvectorIndex(dbVector, i);
    PS_CT_GetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    return GetSegmentPtr (segment, offset, NilOf (size_t *));
}


PublicFnDef M_CPreamble* osDUMP_GetVectorPtokenPtr (
    osDUMP_VectorType *		dbVector
)
{
    int				segment, offset;
    M_CPreamble*		ptoken;

    osDUMP_CTIndex = osDUMP_PtokenIndex(dbVector);
    PS_CT_GetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    ptoken = GetSegmentPtr (segment, offset, NilOf (size_t *));
    if (!osDUMP_Replacing && rtPTOKEN_IsntTerminal (ptoken))
    {
	fprintf (stderr, "Vector ptoken (%ld) not current [%s]\n",
		 osDUMP_CTIndex, osDUMP_Item (dbVector));
	exit (ErrorExitValue);
    }
    return ptoken;
}


PublicFnDef M_CPreamble* osDUMP_GetUsegPtokenPtr (
    osDUMP_VectorType *		dbVector,
    int				i
)
{
    int				segment, offset;
    M_CPreamble*		ptoken;

    osDUMP_CTIndex = osDUMP_UPtokenIndex(dbVector, i);
    PS_CT_GetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    ptoken = GetSegmentPtr (segment, offset, NilOf (size_t *));
    if (!osDUMP_Replacing && rtPTOKEN_IsntTerminal (ptoken))
    {
	fprintf (stderr, "USegment ptoken (%ld) not current [%s]\n",
		 osDUMP_CTIndex, osDUMP_Item (dbVector));
	exit (ErrorExitValue);
    }
    return ptoken;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  OSdump.c 1 replace /users/ees/dbutil
  861030 12:01:36 ees read, access OSpace dump

 ************************************************************************/

/************************************************************************
  OSdump.c 2 replace /users/jck/updates
  870129 13:26:31 jck First release as a module of the incorporator

 ************************************************************************/
/************************************************************************
  OSdump.c 3 replace /users/jck/updates
  870202 13:33:36 jck Fixed miscellaneous bugs

 ************************************************************************/
/************************************************************************
  OSdump.c 4 replace /users/jck/updates
  870219 14:28:18 jck converted M_OSSPreamble to PS_SH

 ************************************************************************/
/************************************************************************
  OSdump.c 5 replace /users/jck/updates
  870305 11:11:24 mjc Fixed bug that neglected to map segments as shared

 ************************************************************************/
/************************************************************************
  OSdump.c 6 replace /users/jck/updates
  870618 10:28:27 jck Shortened names

 ************************************************************************/
/************************************************************************
  OSdump.c 7 replace /users/jck/updates
  870629 14:06:45 jck Implemented processing of multiple prefixes; reimplemented
allocation scheme for final segment; integrated avl search routines

 ************************************************************************/
/************************************************************************
  OSdump.c 8 replace /users/jck/updates
  870730 08:51:34 jck Improved Heuristic used to determine maximum size for a new segment

 ************************************************************************/
/************************************************************************
  OSdump.c 9 replace /users/jck/updates
  870805 16:27:03 jck Fixed a problem accessing table ptoken

 ************************************************************************/
/************************************************************************
  OSdump.c 10 replace /users/jck/updates
  870824 14:53:20 jck Fortified the incorporator

 ************************************************************************/
/************************************************************************
  OSdump.c 11 replace /users/jck/updates
  871002 13:28:43 jck Moved check for '< 2 usegments' from readdump to UpdateVector

 ************************************************************************/
/************************************************************************
  OSdump.c 12 replace /users/jck/updates
  871019 10:03:20 jck Fixed problems occurring when an item was not present in the system's dump

 ************************************************************************/
/************************************************************************
  OSdump.c 13 replace /users/m2/dbup
  880426 11:52:15 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  OSdump.c 14 replace /users/jck/updates
  880615 14:09:50 jck Hardened some soft spots in the incorporator

 ************************************************************************/
/************************************************************************
  OSdump.c 15 replace /users/jck/updates
  880628 08:54:11 jck Plugged another gap in the incorporator

 ************************************************************************/

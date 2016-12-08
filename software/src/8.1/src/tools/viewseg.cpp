/*** program to read a Segment File and display it in humanly
    readable form on the terminal.
    (Originally compatible with PS.[HD] of July, 1987)

CHANGES:
7/23/87 -- added '-d' option which causes a segment file to be displayed
	   as a header, followed by a
	   sequence of containers, rather than displaying details of a
	   container table found in a segment header.
7/30/87 -- added 'POP' to list of values displayed by '-d'.
3/10/88 -- Changed it to use the PS defines.  Added 'SoftwareVersion' to
	   the 'DisplaySegHdr' display.
***/

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

#include "gopt.h"

#include "m.d"
#include "ps.d"
#include "rtype.d"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to read a structure from a file.
 *
 *  Arguments:
 *	fd			- an open UNIX file descriptor.
 *	fo			- the file offset at which the structure
 *				  resides.
 *	structure		- the address of a buffer which will be
 *				  filled with the structure.
 *	structureSize		- the size of the structure in bytes.
 *
 *  Returns:
 *	NOTHING ...also, no error checking!!!
 *
 *****/
PrivateFnDef void ReadStructure (
    int				fd,
    PS_Type_FO			fo,
    void			*structure,
    unsigned int		structureSize
)
{
    lseek (fd, fo, SEEK_SET);
    read (fd, structure, structureSize);
}


/*---------------------------------------------------------------------------
 *****  Function to display the Segment Header.
 *
 *  Arguments:
 *	segHeadBuf	- pointer to the Segment Header buffer
 *
 *  Returns:
 *	NOTHING
 *****/
PrivateFnDef void DisplaySegHdr (PS_SH *segHeadBuf) {
    char const *segmentType;

    switch (PS_RID_Type (PS_SH_RID (segHeadBuf)))
    {
    case PS_ResourceType_UpdateSegment:
	segmentType = "U";
	break;
    case PS_ResourceType_IncorporatorSegment:
	segmentType = "I";
	break;
    default:
	segmentType = "?";
	break;
    }

    printf (
	"\nSEGMENT HEADER (Version %d Interpretation):\n",
	PS_SoftwareVersion
    );
    printf (
	"    Container Table Offset:     %d bytes\n",
	PS_SH_CTOffset (segHeadBuf)
    );
    printf (
	"    Container Table Size:       %d bytes\n",
	PS_SH_CTSize (segHeadBuf)
    );
    printf (
	"    Segment Software Version:   %d\n",
	PS_SH_SoftwareVersion (segHeadBuf)
    );
    printf (
	"    Segment Data Format:        %s(%d)\n",
	0 == PS_SH_DataFormat (segHeadBuf) ? "Sparc/HP" :
	1 == PS_SH_DataFormat (segHeadBuf) ? "Intel" : "",
	PS_SH_DataFormat (segHeadBuf)
    );
    printf (
	"    Segment Identifier:         %08x.%08x.%08x.%s.%u\n",
	PS_TID_High	(PS_SH_TID (segHeadBuf)),
	PS_TID_Low	(PS_SH_TID (segHeadBuf)),
	PS_TID_Sequence (PS_SH_TID (segHeadBuf)),
	segmentType,
	PS_RID_Sequence (PS_SH_RID (segHeadBuf))
    );
    printf (
	"    Segment Checksum:           %u\n",
	PS_SH_Checksum (segHeadBuf)
    );
}


/***************
 *  Type Name  *
 ***************/

/*---------------------------------------------------------------------------
 *****  Routine to return a representation type id as a string.
 *	(Stolen from rtype.c in the backend)
 *
 *  Argument:
 *	r			- the representation type whose string name is
 *				  desired.
 *
 *  Returns:
 *	The address of a read only string naming the representation type id.
 *
 *****/
PrivateFnDef char const* RTYPE_TypeIdAsString (
    RTYPE_Type			r
)
{
/*---------------------------------------------------------------------------
 * Redefine the 'RTYPE_TypeId' macro so that the representation type list
 * expands into an initializer for the representation type id string array.
 *---------------------------------------------------------------------------
 */
#undef RTYPE_TypeId
#define RTYPE_TypeId(rTypeId) #rTypeId

    static char const *rTypeIdStrings [] = {RTYPE_RTypeList};
    char	errorMsg [64];

    sprintf (errorMsg, "Unknown RTYPE#%d", (int)r);
    return
	RTYPE_IsAValidType (r)
	? rTypeIdStrings[(int)r]
	: errorMsg;

/*****  Undefine 'RTYPE_TypeId' to avoid its subsequent mis-use  *****/
#undef RTYPE_TypeId
}


/*---------------------------------------------------------------------------
 *****  Function to display a container's preamble and the offset of the
 *****  preamble in the segment file in a simple format.
 *
 *  Arguments:
 *	p 	- pointer to the container preamble
 *	offset 	- file offset of the preamble in the segment file
 *
 *****/
PrivateFnDef void DisplayCtrData (M_CPreamble *p, off_t offset, bool oldPreambleFormat) {
    printf (
	"[%4d:%7d] #%-20s%11d%11d\n",
	M_CPreamble_POPObjectSpace (p),
	M_CPreamble_POPContainerIndex (p),
	RTYPE_TypeIdAsString ((RTYPE_Type)M_CPreamble_RType (p)),
	oldPreambleFormat
	    ? M_CPreamble_OSize (p)
		+ M_SizeOfPreambleType
		+ M_SizeOfEndMarker
		- sizeof (unsigned int)
	    : M_CPreamble_NSize (p)
		+ M_SizeOfPreambleType
		+ M_SizeOfEndMarker,
	oldPreambleFormat
	    ? offset - M_SizeOfPreambleType + sizeof (unsigned int)
	    : offset - M_SizeOfPreambleType
    );
}


/*---------------------------------------------------------------------------
 *****  Function to display a container's preamble in a pretty format.
 *
 *  Arguments:
 *	p 	- pointer to the container preamble
 *
 *****/
PrivateFnDef void  DisplayCTPreamble (
    M_CPreamble*		p
)
{
    printf("\nCONTAINER TABLE PREAMBLE:\n");
    printf("    RTYPE:                      %s\n",
	RTYPE_TypeIdAsString ((RTYPE_Type)M_CPreamble_RType (p)));

    printf("    CT size:                    %d\n",
	M_CPreamble_Size (p) + M_SizeOfPreambleType + M_SizeOfEndMarker);

    printf("    Object Space:               %d\n",
	M_CPreamble_POPObjectSpace (p));

    printf("    Container Index:            %d\n",
	M_CPreamble_POPContainerIndex (p));
}


/*---------------------------------------------------------------------------
 *****  Function to display the contents of a given Container Table Entry
 *****  of a Container Table if it is in use; no display otherwise.
 *
 *  Arguments:
 *	fd	    - file descriptor
 *	offset	    - file offset of the CTE
 *	index	    - index of the CTE in the CT entries array
 *
 *  RETURNS:
 *	the file offset of the next CTE to display.
 *
 *  NOTE:
 *	assumes that the file pointer is positioned at a CTE.
 *
 *****/
PrivateFnDef off_t DisplayACTE (int fd, off_t offset, int index) {
    PS_CTE CTEBuf;
    ReadStructure (fd, (PS_Type_FO)offset, &CTEBuf, sizeof(PS_CTE));

    if (!PS_CTE_IsFree (CTEBuf)) printf (
	"\n%d\t%d\t%lu", index, PS_CTE_Segment (CTEBuf), PS_CTE_Offset (CTEBuf)
    );

    return offset + sizeof(PS_CTE);
}


/*---------------------------------------------------------------------------
 *****  Funtion to display the contents of the in use
 *****  Container Table Entries in a Container Table.
 *
 *  Arguments:
 *	fd 		- file descriptor
 *	CTBuf 		- pointer to the container table buffer
 *	offset 		- file offset of the CT
 *
 *****/
PrivateFnDef void DisplayCTEs (int fd, PS_CT *CTBuf, off_t offset) {
    /**** set "offset" at first CTE for DisplayACTE ****/
    offset = lseek (
	fd,
	offset + sizeof (*CTBuf) - sizeof (PS_CT_Entries (CTBuf)),
	SEEK_SET
    );

    for (int i = 0; i < PS_CT_EntryCount (CTBuf); i++)
	offset = DisplayACTE(fd, offset, i);
}


/*---------------------------------------------------------------------------
 *****  Funtion to display the contents of the container table -- first
 *****  the static fields, then the CTEs.
 *
 *  Arguments:
 *	fd 		- file descriptor
 *	CTBuf 		- pointer to the container table buffer
 *	offset 		- file offset of the CT
 *
 *****/
PrivateFnDef void DisplayCT (int fd, PS_CT *CTBuf, off_t offset) {
    printf("\n#CTEs: %d   1st free CTE FO: %d",
	PS_CT_EntryCount (CTBuf), PS_CT_FreeList (CTBuf));

    printf("\n\nACTIVE CTEs:");
    printf("\n\nCTE #\tSEG\tFO");
    DisplayCTEs (fd, CTBuf, offset); /*  display the active CTEs  */
    printf("\n");
}


/*--------------------------------------------------------------------------
*  internal routine to display containers in a segment file by reading
*  successive containers, starting right after the segment header, and
*  displaying type & size info for each.  It assumes that the next container
*  will be found right after the current one.
*****/
PrivateFnDef bool NextContainer (M_CPreamble &rPreamble, int fd) {
    bool bDone = false, bRead = false;
    while (!bDone && sizeof (rPreamble.nSize) == read (fd, &rPreamble.nSize, sizeof (rPreamble.nSize))) {
        if (rPreamble.nSize != 0xffffffff) {
            size_t const sRead = sizeof (rPreamble) - sizeof (rPreamble.nSize);
            bRead = sRead == read (fd, &rPreamble.nSize + 1, sRead);
            bDone = true;
        }
    }
    return bRead;
}

PrivateFnDef void DisplayContainers (
    int fd, bool oldPreambleFormat, bool displayingDetails
) {
    if (displayingDetails) printf (
	"\nCONTAINER DATA:\n\n[%-12.12s] %-20s %11.11s       %s\n",
	" POP", " RTYPE", "SIZE", "OFFSET\n"
    );

    unsigned int totalContainers = 0;
    unsigned int smallContainers = 0;
    unsigned int largeContainers = 0;

    if (oldPreambleFormat)
	lseek (fd, -(int)sizeof (unsigned int), SEEK_CUR);

    M_CPreamble ctrPreBuf;
    while (NextContainer (ctrPreBuf, fd)) {
	totalContainers++;
	if (M_CPreamble_OSize (&ctrPreBuf) ||
	    RTYPE_C_Undefined0 == (RTYPE_Type)M_CPreamble_RType (&ctrPreBuf)
	) smallContainers++;
	else largeContainers++;

	off_t offset = lseek (fd, 0, SEEK_CUR);  /* get current file offset */
	if (displayingDetails) DisplayCtrData (
	    &ctrPreBuf, offset, oldPreambleFormat
	);

    /**** get to next container preamble *****/
	lseek (
	    fd,
	    oldPreambleFormat
		? M_CPreamble_OSize (&ctrPreBuf)
		    + sizeof (M_CEndMarker)
		    - sizeof (unsigned int)
		: M_CPreamble_NSize (&ctrPreBuf) + sizeof (M_CEndMarker),
	    SEEK_CUR
	);
    }

    printf (
	"    Container Format:           %s\n",
	oldPreambleFormat ? "Small" : "Large"
    );
    printf (
	"    Total Containers:           %u\n", totalContainers);
    if (smallContainers > 0 && smallContainers != totalContainers) printf (
	"    Small Containers:           %u\n", smallContainers
    );
    if (largeContainers > 0 && largeContainers != totalContainers) printf (
	"    Large Containers:           %u\n", largeContainers
    );
}


/*-------------------------------------------------------------------------*/

GOPT_BeginOptionDescriptions

    GOPT_SwitchOptionDescription ("DisplayContainers"	    , 'd', NilOf (char*))
    GOPT_SwitchOptionDescription ("SummarizeContainers"	    , 's', NilOf (char*))

GOPT_EndOptionDescriptions;

int main (int argc, char *argv[]) {
    int			fd;
    int			ctOffset;
    char const*		filename;
    PS_SH		segHeadBuf;	/* buffer for Segment Header */
    PS_CT		CTBuf;		/* buffer for Container Table */
    M_CPreamble		pbuf;		/* buffer for CT preamble */

/****  Get the command line arguments  *****/
    GOPT_AcquireOptions (argc, argv);
    GOPT_SeekExtraArgument (0, 0);

    if (IsNil (filename = GOPT_GetExtraArgument ())) {
        fprintf (stderr, "Usage: %s [-d] segment\n", argv[0]);
	exit (ErrorExitValue);
    }

/****  open the file ****/
    fd = open (filename, O_RDONLY, 0);
    if (fd < 0) {
	perror ("File Open Error");
	exit (ErrorExitValue);
    }

    off_t filesize = lseek (fd, 0, SEEK_END);

    ReadStructure (fd, 0, &segHeadBuf, sizeof(PS_SH));
    DisplaySegHdr (&segHeadBuf);

    if (GOPT_GetSwitchOption ("SummarizeContainers")) DisplayContainers (
	fd, PS_SH_IsASmallContainerSegment (&segHeadBuf) ? true : false, false
    );
    else if (GOPT_GetSwitchOption ("DisplayContainers")) DisplayContainers (
	fd, PS_SH_IsASmallContainerSegment (&segHeadBuf) ? true : false, true
    );
    else if (
	PS_SH_CTOffset (&segHeadBuf) < PS_SH_Size ||
	PS_SH_CTOffset (&segHeadBuf) > filesize
    ) printf ("Segment '%s' contains no container table\n", filename);
    else if (
	Vk_DataFormatNative != PS_SH_DataFormat (&segHeadBuf)
    ) printf ("Segment '%s' binary format not recognized by this processor\n", filename);
    else {
	if (PS_SH_IsASmallContainerSegment (&segHeadBuf)) {
	    PS_SH_CTOffset (&segHeadBuf) -= sizeof (unsigned int);
	    PS_SH_CTSize   (&segHeadBuf) += sizeof (unsigned int);
	}

	/*****  Read the Container Table Preamble ... *****/
	ReadStructure (
	    fd, PS_SH_CTOffset (&segHeadBuf), &pbuf, sizeof(M_CPreamble)
	);
	/*****  Read the Container Table ... *****/
	ctOffset = PS_SH_CTOffset (&segHeadBuf) + sizeof(M_CPreamble);
	ReadStructure (fd, ctOffset, &CTBuf, sizeof (PS_CT));

	/*****  Display them ... *****/
	DisplayCTPreamble (&pbuf);
	DisplayCT (fd, &CTBuf, ctOffset);
    }
    close (fd);

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  viewseg.c 1 replace /vision/tools/source
  871017 22:09:46 insyte Install tool source in source control

 ************************************************************************/
/************************************************************************
  viewseg.c 2 replace /vision/tools/releases/original
  880219 17:04:10 jad remove specific paths from include stmts

 ************************************************************************/
/************************************************************************
  viewseg.c 3 replace /users/jad/system/TOOLS
  880314 13:05:32 jad NDF structure changes -> Version 2 NDF

 ************************************************************************/
/************************************************************************
  viewseg.c 4 replace /users/jck/tools
  880503 09:06:23 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  viewseg.c 5 replace /users/jck/tools
  890927 16:15:56 jck Added gopt

 ************************************************************************/

/*** program to read a Network Directory File and display it in humanly
     readable form on the terminal.  
     (Originally consistent with PS.C & PS.D as of April 8, 1987)
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

#include "V_VTime.h"

#include "ps.d"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

#define PATHBUFSIZE  1024


/*---------------------------------------------------------------------------
 *****  Internal utility to read a structure from a file.
 *
 *  Arguments:
 *	fd			- an open NDF descriptor.
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
    void *			structure,
    size_t			structureSize
)
{
    lseek (fd, fo, SEEK_SET);
    read (fd, structure, structureSize);
}


PrivateFnDef void ReadNVD (
    int				fd,
    PS_Type_FO			fo,
    PS_NVD			*nvd
)
{
    PS_NVD_Signature (*nvd) = 0;

    ReadStructure (fd, fo, nvd, sizeof (PS_NVD));

    if (PS_NetworkDirectorySignature == PS_NVD_Signature (*nvd)) return;

    PS_NVD_Signature		    (*nvd) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		    (*nvd) = false;
    PS_NVD_UnusedFlags		    (*nvd) = 0;
    PS_TID_High		(PS_NVD_TID (*nvd))=
    PS_TID_Low		(PS_NVD_TID (*nvd))= 0;
    PS_TID_Sequence	(PS_NVD_TID (*nvd))= fo;
    PS_NVD_TD			    (*nvd) =
    PS_NVD_MP			    (*nvd) =
    PS_NVD_Annotation		    (*nvd) =
    PS_NVD_Extensions		    (*nvd) = PS_FO_Nil;
}


PrivateFnDef void ReadSVD (
    int				fd,
    PS_Type_FO			fo,
    PS_SVD			*svd
)
{
    PS_SVD_Signature (*svd) = 0;

    ReadStructure (fd, fo, svd, sizeof (PS_SVD));

    if (PS_NetworkDirectorySignature == PS_SVD_Signature (*svd)) return;

    PS_SVD_Signature			(*svd) = PS_NetworkDirectorySignature;
    PS_SVD_SD				(*svd) =
    PS_SVD_MP				(*svd) =
    PS_SVD_Extensions			(*svd) = PS_FO_Nil;
}


PrivateFnDef void Read6ByteSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		index,
    PS_SRD *			srd
) {
    struct srd6_t {
        unsigned int		role : 2;
	unsigned short		data[2];
    } altSRD;

    ReadStructure (fd, fo + sizeof (int) + index * 6, &altSRD, 6);

    memcpy (srd, altSRD.data, sizeof (int));
    PS_SRD_Role (*srd) = altSRD.role;
}

PrivateFnDef void Read8ByteSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		index,
    PS_SRD *			srd
) {
    struct srd8_t {
        unsigned int		role : 32 - PS_FO_FieldWidth,
				     : PS_FO_FieldWidth;
	PS_Type_FO		svd;
    } altSRD;

    ReadStructure (fd, fo + sizeof (int) + index * 8, &altSRD, 8);

    PS_SRD_Role (*srd) = altSRD.role;
    PS_SRD_SVD	(*srd) = altSRD.svd;
}


PrivateFnDef void ReadSRD (
    int				fd,
    PS_Type_FO			fo,
    unsigned int		srdIndex,
    PS_SRD *			srd,
    int				softwareVersion
)
{
    int				srd0[2];

    if (softwareVersion >= 2) {
	ReadStructure (
	    fd, fo + sizeof (int) + srdIndex * sizeof (PS_SRD), srd, sizeof (PS_SRD)
	);
	return;
    }

    ReadStructure (fd, fo + sizeof (int), srd0, sizeof (srd0));

    if (PS_FO_IsNil (srd0[1]))
	Read8ByteSRD (fd, fo, srdIndex, srd);
    else
	Read6ByteSRD (fd, fo, srdIndex, srd);
}


/*---------------------------------------------------------------------------
 *****  Format a micro-second resolution time stamp.
 *
 *  Arguments:
 *	tsp			- the address of the time stamp to format.
 *
 *  Returns:
 *	The address of a reused buffer containing the formatted representation
 *	of the time stamp.
 *
 *****/
PrivateFnDef char const *ctimeval (PS_TS *tsp) {
    V::VTime iTime (*tsp);
    struct tm iTimeStruct;
    struct tm *tm = iTime.localtime (iTimeStruct);

/****  using 'mm/dd/yy:hh:mm:ss' format  ****/
    static char buffer[80];
    sprintf
        (buffer,
	 "%02d/%02d/%02d:%02d:%02d:%02d.%06d",
	 tm->tm_mon + 1,
	 tm->tm_mday,
	 tm->tm_year % 100,
	 tm->tm_hour,
	 tm->tm_min,
	 tm->tm_sec,
	 tsp->microseconds);

    return buffer;
}


/*---------------------------------------------------------------------------
 ***** displays the Network Directory Header in nice format on screen.
 *     
 *     ndh -- the NDH structure
 *     pathbuf -- the directory pathname as string
 *****/
PrivateFnDef void displayNDH (
    PS_NDH			ndh,
    char			pathbuf[]
)
{
    printf("\nNETWORK DIRECTORY HEADER -- ( %s )", pathbuf);

    printf("\n  signature        : %d",  PS_NDH_Signature (ndh));
    printf("\n  ndf version      : %d",  PS_NDH_NDFVersion (ndh));
    printf("\n  directory version: %d",	 PS_NDH_DirectoryVersion (ndh));
    printf("\n  current NVD FO   : %lu", PS_NDH_CurrentNetworkVersion (ndh));
    printf("\n  update timestamp : %s\n",
		ctimeval (&PS_NDH_ModificationTimeStamp (ndh))
    );
}


/*---------------------------------------------------------------------------
 *	collect a counted string from a file; store in the supplied
 *	buffer as a null-terminated string.  
 *
 *	fd -- NDF descriptor
 *	offset -- file offset of the counted string
 *	buffer -- pointer to a buffer for the output string
 *	maxbuf -- capacity of the buffer (including the '\0')
 *
 * NOTE:
 *	file pointer may not be left at a sensible place (if the
 *	string was very long...
 *****/
PrivateFnDef void getCountedString (
    int				fd,
    int				offset,
    char *			buffer,
    size_t			maxbuf
)
{
    size_t count;

    /**** get the count field ****/
    ReadStructure (fd, offset, &count, sizeof(int));
    if (count > maxbuf) 	/* adjust count to max buffer size */
	count = maxbuf;

    /**** and the pathname itself ****/
    ReadStructure (fd, (offset + sizeof(int)), buffer, count);
    if (count = maxbuf)		/* null-terminate stg */
	buffer[count-1] = '\0';
    else buffer[count] = '\0';
}


/*---------------------------------------------------------------------------
 ***** read the Network Directory Header and the directory pathname from
 *     a newly opened Network Directory File.
 *
 *	fd -- the file descriptor 
 *	ndh -- pointer to a buffer for the NDH\
 *	pathbuf -- a string buffer for the pathname
 *
 * NOTE:
 *	file pointer may not be left at a sensible place (if pathname
 *	string was very long...
 *
 * ERROR:
 *	if the version of the NDF is incompatible with this program, 
 *	print an error message and abort.
 *****/
PrivateFnDef void getNDHAndPathname (
    int				fd,
    PS_NDH *			ndh,
    char			pathbuf[]
)
{
/*****  Read the NDH ...  *****/
    ReadStructure (fd, 0, ndh, sizeof(PS_NDH));

/*****  Check the NDF version ...  *****/
    if (PS_NDH_NDFVersion (*ndh) != PS_NDFVersionNumber)
    {
	printf ("\nviewndf: Cannot read the NDF, the version is incompatible.\n");
	printf ("  Version in NDF:%d,  Version in viewndf:%d\n", 
	    PS_NDH_NDFVersion (*ndh), PS_NDFVersionNumber);
	exit (ErrorExitValue);
    }

/*****  Get the path ... *****/
    getCountedString (fd, sizeof(PS_NDH), pathbuf, PATHBUFSIZE);
}


/*---------------------------------------------------------------------------
 *	display the contents of a Space Version Descriptor
 *
 *	fd -- file descriptor
 *	offset -- offset in the Network Dir. file of the SVD
 *****/
PrivateFnDef void displaySVD (
    int				fd,
    int				offset
)
{
    PS_SVD svd;
    char segDirBuf[PATHBUFSIZE]; 
    
/****  read the SVD from file into buffer 'svd'  ****/
    ReadSVD (fd, offset, &svd);

/****  get the segment directory name ****/
    getCountedString (fd, svd.spacePathName, segDirBuf, PATHBUFSIZE);

    printf("\t%d\t%d\t%d\t%d\t%d\t%lu",
		PS_SVD_SpaceIndex (svd), 
		PS_SVD_MinSegment (svd),
		PS_SVD_MaxSegment (svd), 
		PS_SVD_RootSegment (svd), 
		PS_SVD_OrigRootSegment (svd), 
		PS_SVD_CTOffset (svd));
}


/*---------------------------------------------------------------------------
 *	display info in a Space Role Descriptor and its associated 
 *	Space Version Descriptor (if it exists).
 *
 *	fd -- file descriptor
 *	srd -- buffer containing the space role descriptor
 *	index -- the index of the SRD in the Space Role Vector of the NVD.
 *
 *****/
PrivateFnDef void displaySRDAndSVD (
    int				fd,
    PS_SRD			srd,
    int				index
)
{
    switch (srd.role)  {
      case PS_Role_Nonextant  : printf("\nN"); break;
      case PS_Role_Unaccessed : printf("\nU"); break;
      case PS_Role_Read       : printf("\nR"); break;
      case PS_Role_Modified   : printf("\nM"); break;
      default:
	printf("/n/ndisplaySRDAndSVD: error in switch");
	exit (ErrorExitValue);
	break;
    }

/**** display the Space Version Descriptor, if it exists ****/

    if (PS_SRD_Role (srd) != PS_Role_Nonextant)
	displaySVD (fd, PS_SRD_SVD (srd));
    else
	printf("\t%d", index);  /* just print the space index */
}


/*---------------------------------------------------------------------------
 *	Displays info contained in one version of a network
 *
 *	fd -- file descriptor
 *	fo -- offset to NVD of the version to display
 *
 *	RETURNS:  file offset of next net version in the chain --
 *		  (stored in previousVersion field of the NVD)
 *
 *****/
PrivateFnDef PS_Type_FO doThisVersion (
    int				fd,
    PS_Type_FO			fo
)
{
    PS_NVD nvd;
    PS_SRD srd;

    int srvCount, i;

/**** get the Network Version Descriptor and display info ****/

    ReadNVD (fd, fo, &nvd);

    printf("\n  timestamp -- %s\n", ctimeval (&PS_NVD_CommitTimeStamp (nvd)));
    printf("\n  previous version  : %lu", PS_NVD_PreviousVersion (nvd));
    printf("\n  previous nvd chain: %lu", PS_NVD_PreviousNVDChain (nvd));
    printf("\n  update thread     : %lu", PS_NVD_UpdateThread (nvd));
    printf("\n  accessed version  : %lu", PS_NVD_AccessedVersion (nvd));
    printf("\n  directory version : %d",  PS_NVD_DirectoryVersion (nvd));
    printf("\n  software version  : %d",  PS_NVD_SoftwareVersion (nvd));

/****  get count field of the Space Role Vector  ****/

    ReadStructure (fd, nvd.srv, &srvCount, sizeof(int));

/*****  print a header  *****/
    if (srvCount > 0) {
	printf ("\n\t----------------------- SVD ----------------");
	printf ("\nROLE	SPC INX	MIN	MAX	ROOT	ORIGRT	CTFO");
    }

/**** traverse the array of SRDs (file ptr pts to first SRD) ****/ 

    for (i = 0; i < srvCount; i++)  {
	ReadSRD (fd, nvd.srv, i, &srd, nvd.softwareVersion);
	displaySRDAndSVD (fd, srd, i);
    }
    return PS_NVD_BaseVersion (nvd) ? PS_FO_Nil : PS_NVD_PreviousVersion (nvd);
}


/*---------------------------------------------------------------------------
 *	Displays info for all versions of the network in the current NVD
 *	chain in the file, starting from the NVD at the given offset fo.
 *
 *	fd -- file descriptor
 *	fo -- offset of NVD of the 1st Network version in chain to
 *		traverse.
 *****/
PrivateFnDef void doNetVersions (
    int				fd,
    PS_Type_FO			fo
)
{
    int netcount = 1;

    while (fo != PS_FO_Nil)  {
	printf (
	    "\n\n\nNETWORK VERSION #%d%s NVD FO:%u",
	    netcount,
	    netcount == 1 ? " (most recent)" : "",
	    fo
	);
	netcount += 1;
	fo = doThisVersion(fd, fo);
    }
}


/*--------------------------------------------------------------------------
 *	Displays info for all versions of the network in ALL NVD
 *	chains in the file, starting from the NVD at the given offset fo.
 *
 *	fd -- file descriptor
 *	fo -- offset of NVD of the 1st Network version in an NVD chain
 *****/
PrivateFnDef void doAllNVDChains (
    int				fd,
    PS_Type_FO			fo
)
{
    int chaincount = 1;
    PS_NVD nvd;

    while (fo != PS_FO_Nil)
    {
	printf("\n\n\nNETWORK VERSION CHAIN #%d%s", chaincount,
			chaincount == 1 ? " (most recent)" : "");
	chaincount += 1;
	doNetVersions (fd, fo);		/* display the current NVD chain */
	ReadNVD (fd, fo, &nvd);
  	fo = nvd.previousNVDChain;
    }
}


/*-------------------------------------------------------------------------*/
int main (
    int				argc,
    char *			argv[]
)
{
/****  open the file ****/
    char const* ndfPathName = argc >= 2 ? argv[1] : getenv ("NDFPathName");
    ndfPathName = IsntNil (ndfPathName) ? ndfPathName : "/vision/network/NDF";

#ifdef __VMS
    int fd = open (ndfPathName, O_RDONLY, 0, "shr=upd,put,get,nql");
#else
    int fd = open (ndfPathName, O_RDONLY, 0);
#endif
    if (fd < 0)
    {
	perror ("File Open Error");
	exit (ErrorExitValue);
    }

    PS_NDH ndh;		/* NDH buffer */
    char pathbuf[PATHBUFSIZE];
    getNDHAndPathname (fd, &ndh, pathbuf);
    displayNDH (ndh, pathbuf);

    if (argc > 2 && strcmp(argv[2], "-a") == 0)		/* -a option */

	doAllNVDChains (fd, ndh.components.currentNetworkVersion);
    else
	doNetVersions (fd, ndh.components.currentNetworkVersion);

    close (fd);
    printf ("\n");

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  viewndf.c 1 replace /vision/tools/source
  871017 22:09:44 insyte Install tool source in source control

 ************************************************************************/
/************************************************************************
  viewndf.c 2 replace /vision/tools/releases/original
  880219 17:03:21 jad remove specific paths from include stmts

 ************************************************************************/
/************************************************************************
  viewndf.c 3 replace /users/jad/system/TOOLS
  880314 13:02:13 jad NDF structure changes -> Version 2 NDF

 ************************************************************************/
/************************************************************************
  viewndf.c 4 replace /users/jck/tools
  880503 09:06:06 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  viewndf.c 5 replace /users/mjc/Workbench/port/tools
  880519 22:56:31 mjc Convert tools to use NDF file server on the Apollo

 ************************************************************************/
/************************************************************************
  viewndf.c 6 replace /users/mjc/Workbench/port/tools
  880521 17:15:53 mjc Re-implemented u-sec timestamp display, added NDF path name defaults

 ************************************************************************/

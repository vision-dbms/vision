/*****  Object Network Profiler  *****/

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#ifdef _WIN32
#define FMT64 "I64"
#else
#define FMT64 "ll"
#endif

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "V_VCommandLine.h"

#include "VkMemory.h"

#include "rtype.d"
#include "ps.d"
#include "m.d"

#include "RTblock.d"
#include "RTcharuv.d"
#include "RTclosure.d"
#include "RTcontext.d"
#include "RTdictionary.d"
#include "RTdoubleuv.d"
#include "RTdsc.d"
#include "RTfloatuv.d"
#include "RTindex.d"
#include "RTintuv.d"
#include "RTlink.d"
#include "RTlstore.d"
#include "RTmethod.d"
#include "RTparray.d"
#include "RTpct.d"
#include "RTptoken.d"
#include "RTrefuv.d"
#include "RTseluv.d"
#include "RTstring.d"
#include "RTtct.d"
#include "RTundefuv.d"
#include "RTvector.d"
#include "RTvstore.d"

#include "VSimpleFile.h"
#include "VTransientServices.h"

#include "V_VString.h"


/*****************************************
 *****************************************
 *****  Transient Services Provider  *****
 *****************************************
 *****************************************/

class VTransientServicesForOnprof : public VTransientServices {
//  Resource Reclamation
public:
    //  Routines return true when resources are reclaimed, false otherwise.
    virtual bool mappingsReclaimed ();
};

PrivateVarDef VTransientServicesForOnprof g_iTransientServicesProvider;


/******************************
 ******************************
 *****  Utility Routines  *****
 ******************************
 ******************************/

/*****************************
 *****  Error Reporting  *****
 *****************************/

static void SignalUnixError (char const *message) {
    perror (message);
    exit (ErrorExitValue);
}

static void SignalApplicationError (char const *message) {
    fprintf (stderr, "%s\n", message);
    exit (ErrorExitValue);
}

static bool ApplicationErrorOccurred = false;
static void DisplayApplicationError (char const *message) {
    fprintf (stderr, "%s\n", message);
    fflush (stderr);
    ApplicationErrorOccurred = true;
}

static char const *MakeMessage (char const *fmt, ...) {
    static char buffer[1024];
    va_list ap;

    va_start (ap, fmt);
    vsprintf (buffer, fmt, ap);
    va_end   (ap);

    return buffer;
}


/*******************************
 *****  Memory Management  *****
 *******************************/

static void *Allocate (size_t bytes) {
    void *result;

    if (!(result = malloc (bytes))) SignalApplicationError (
	MakeMessage ("Error Allocating %d Bytes", bytes)
    );
    memset (result, 0, bytes);
    return result;
}


/*********************
 *****  NDF I/O  *****
 *********************/

static int Open (char const *file) {
#ifdef __VMS
    int fd = open(file, O_RDONLY, 0, "shr=upd,put,get,nql");
#else
    int fd = open(file, O_RDONLY, 0);
#endif

    if (fd < 0) SignalUnixError (
	MakeMessage ("Error Opening '%s'", file)
    );

    return fd;
}

static void Close (int fd) {
    if (close (fd) < 0) SignalUnixError (
	"Error Closing NDF"
    );
}

static off_t Seek (int fd, off_t offset, int whence) {
    off_t loc = lseek (fd, offset, whence);

    if (loc < 0)
	SignalUnixError ("Seek Error");

    return loc;
}

static void Read (
    int	fd, void *buffer, size_t bytes, bool sizeMustBeChecked
) {
    ssize_t bytesRead = read (fd, buffer, bytes);

    if (bytesRead < 0)
	SignalUnixError ("File Read Error");
    else if (sizeMustBeChecked && bytesRead != bytes)
	SignalApplicationError ("File Read Truncation");
}


static void *ReadCountedArray (int fd, size_t elementSize) {
    int elementCount;
    void *result;

    Read (fd, &elementCount, sizeof elementCount, true);
    result = Allocate (elementCount * elementSize);
    Read (fd, result, elementCount * elementSize, true);

    return result;
}


PrivateFnDef void Read6ByteSRD (int fd, PS_SRD *srd) {
    struct srd6_t {
	unsigned int		role : 2;
	unsigned short		data[2];
    } altSRD;

    Read (fd, &altSRD, 6, true);

    memcpy (srd, altSRD.data, sizeof (int));
    PS_SRD_Role (*srd) = altSRD.role;
}

PrivateFnDef void Read8ByteSRD (int fd, PS_SRD *srd) {
    struct srd8_t {
	unsigned int		role : 32 - PS_FO_FieldWidth,
				     : PS_FO_FieldWidth;
	PS_Type_FO		svd;
    } altSRD;

    Read (fd, &altSRD, 8, true);

    PS_SRD_Role (*srd) = altSRD.role;
    PS_SRD_SVD	(*srd) = altSRD.svd;
}


static PS_SRD *ReadSRV (
    int				fd,
    PS_Type_FO			fo,
    int				softwareVersion,
    unsigned int*		srdCount
) {
    Seek (fd, (int)fo, SEEK_SET);
    Read (fd, srdCount, sizeof (int), true);
    PS_SRD *srv = (PS_SRD *)Allocate (*srdCount * sizeof (PS_SRD));
    if (softwareVersion >= 2) {
	Read (fd, srv, *srdCount * sizeof (PS_SRD), true);
	return srv;
    }

    unsigned int srd0[2], srdIndex;
    Read (fd, srd0, sizeof (srd0), true);
    Seek (fd, (int)fo + sizeof (int), SEEK_SET);
    if (PS_FO_IsNil (srd0[1])) for (
	srdIndex = 0; srdIndex < *srdCount; srdIndex++
    ) Read8ByteSRD (fd, &srv[srdIndex]);
    else for (srdIndex = 0; srdIndex < *srdCount; srdIndex++
    ) Read6ByteSRD (fd, &srv[srdIndex]);

    return srv;
}


/**************************
 **************************
 *****  RType Naming  *****
 **************************
 **************************/

static char const *RTypeName (RTYPE_Type rtype) {
    switch (rtype) {
    case RTYPE_C_Undefined:	return "Undefined";
    case RTYPE_C_Undefined0:	return "Undefined0";
    case RTYPE_C_TCT:		return "TCT";
    case RTYPE_C_PCT:		return "PCT";
    case RTYPE_C_String:	return "String";
    case RTYPE_C_PArray:	return "PArray";
    case RTYPE_C_PToken:	return "PToken";
    case RTYPE_C_Link:		return "Link";
    case RTYPE_C_CharUV:	return "CharUV";
    case RTYPE_C_DoubleUV:	return "DoubleUV";
    case RTYPE_C_FloatUV:	return "FloatUV";
    case RTYPE_C_IntUV:		return "IntUV";
    case RTYPE_C_RefUV:		return "RefUV";
    case RTYPE_C_SelUV:		return "SelUV";
    case RTYPE_C_UndefUV:	return "UndefUV";
    case RTYPE_C_MethodD:	return "MethodD";
    case RTYPE_C_Dictionary:	return "Dictionary";
    case RTYPE_C_Block:		return "Block";
    case RTYPE_C_Method:	return "Method";
    case RTYPE_C_Context:	return "Context";
    case RTYPE_C_Closure:	return "Closure";
    case RTYPE_C_Vector:	return "Vector";
    case RTYPE_C_ListStore:	return "ListStore";
    case RTYPE_C_ValueStore:	return "ValueStore";
    case RTYPE_C_Descriptor:	return "Descriptor";
    case RTYPE_C_Descriptor0:	return "Descriptor0";
    case RTYPE_C_Descriptor1:	return "Descriptor1";
    case RTYPE_C_Index:		return "Index";
    case RTYPE_C_Unsigned64UV:	return "Unsigned64";
    case RTYPE_C_Unsigned96UV:	return "Unsigned96";
    case RTYPE_C_Unsigned128UV:	return "Unsigned128";
    default:			return MakeMessage ("RType # %d", rtype);
    }
}


/********************************
 ********************************
 *****  Network Management  *****
 ********************************
 ********************************/

/***********************************
 *****  Structure Definitions  *****
 ***********************************/

struct ConnectivityStatistics;

/********************************
 *----  Segment Descriptor  ----*
 ********************************/

struct SegmentDescriptor {
//  Construction
public:
    void Construct ();

//  Address/Header
public:
    void const *segmentAddress () const {
	return mapping.RegionAddress ();
    }
    PS_SH const *segmentHeader () const {
        return (PS_SH const *)segmentAddress ();
    }

//  Access/Query
public:
    bool hasIdentityProblems () const {
	PS_SH const *pHeader = segmentHeader ();
	return hasExpectedID && (
	    expectedTID != PS_SH_TID (pHeader) || expectedRID != PS_SH_RID (pHeader)
	);
    }

//  State
public:
    VkMemory			mapping;
    PS_TID			expectedTID;
    PS_RID			expectedRID;
    unsigned int		isLocked	:  1,
				isNeeded	:  1,
				hasExpectedID	:  1,
						: 29;
};

void SegmentDescriptor::Construct () {
    mapping.Initialize ();
    hasExpectedID = isLocked = isNeeded = false;
}


/******************************
 *----  Space Descriptor  ----*
 ******************************/

typedef unsigned char mark_t;

struct SpaceDescriptor {
//  Construction
public:
    void Construct (unsigned int xSpace) {
	m_xSpace = xSpace;
        m_cMisalignedContainers = m_cMisalignedValues = 0;
	minSegment = INT_MAX;
	maxSegment = -1;
    }
//  Access
public:
    unsigned int containerCount () const {
	return PS_CT_EntryCount (ct);
    }
    unsigned __int64 misalignedContainers () const {
        return m_cMisalignedContainers;
    }
    unsigned __int64 misalignedValues () const {
        return m_cMisalignedValues;
    }

//  Query
public:
    bool exists () const {
	return maxSegment != -1;
    }

//  Container Access
public:
    M_CPreamble *MapContainer (unsigned int xContainer);

    PS_CTE const &containerCTE (unsigned int xContainer) const {
	return PS_CT_Entry (ct, xContainer);
    }
    unsigned int containerOffset (unsigned int xContainer) const {
	return PS_CTE_Offset (containerCTE (xContainer));
    }
    unsigned int containerSegment (unsigned int xContainer) const {
	return PS_CTE_Segment (containerCTE (xContainer));
    }

//  Container Marking
public:
    template <class T> void checkAlignment (T const *pT, unsigned int cValues) {
        checkAlignment (pT, sizeof (T), cValues);
    }
    void checkAlignment (void const *pT, size_t sT, unsigned int cValues) {
        pointer_size_t iT = reinterpret_cast<pointer_size_t>(pT);
        if (iT != iT / sT * sT) {
            m_cMisalignedContainers++;
            m_cMisalignedValues += cValues;
        }
    }
    mark_t GetMark (unsigned int xContainer) const {
	return markingVector[xContainer];
    }
    void SetMark (unsigned int xContainer, mark_t xMark) const {
	markingVector[xContainer] = xMark;
    }

    void MarkContainer (unsigned int xContainer);

//  Segment Descriptor
public:
    SegmentDescriptor &segmentDescriptor (unsigned int xSegment) const {
	return mappingVector[xSegment - minSegment];
    }

//  Statistics
public:
    void CollectStatistics (ConnectivityStatistics *pStatistics);

//  State
public:
    unsigned int		m_xSpace;
    char const*			spaceDirectory;
    char			spaceName[16];
    int				minSegment,
				maxSegment;
    SegmentDescriptor		*mappingVector;
    PS_CT			*ct;
    mark_t			*markingVector;
    unsigned __int64		m_cMisalignedContainers;
    unsigned __int64		m_cMisalignedValues;
};


/********************************
 *----  Network Descriptor  ----*
 ********************************/

struct NetworkDescriptor {
//  Space Descriptor
public:
    SpaceDescriptor &spaceDescriptor (unsigned int xSpace) const {
	return sdv[xSpace];
    }

//  Segment Descriptor
public:
    SegmentDescriptor &segmentDescriptor (unsigned int xSpace, unsigned int xSegment) const {
	return spaceDescriptor (xSpace).segmentDescriptor (xSegment);
    }

//  Statistics
public:
    void DisplayConnectivityData (bool bProfiling, bool bShowingMisalignments) const;

//  Operation
public:
    void MarkContainer (unsigned int xSpace, unsigned int xContainer);
    void VerifyNetworkConnectivity ();
private:
    void ClearMarks ();
    void MarkInitialContainers ();
    void ProcessNextContainer ();

//  State
public:
    char const*			ndfPathName;
    char			osdPathName[1024];
    unsigned int		spaceCount;
    PS_SRD			*srv;
    SpaceDescriptor		*sdv;
};


/********************************************
 *****  Global State and Access Macros  *****
 ********************************************/

static struct NetworkDescriptor Network;
static VkMemory::Share SegmentMemoryShare = VkMemory::Share_Group;

#define NetworkNDFPathName		Network.ndfPathName
#define NetworkOSDPathName		Network.osdPathName
#define NetworkSpaceCount		Network.spaceCount
#define NetworkSRV			Network.srv
#define NetworkSDV			Network.sdv

#define SpaceSRD(space)			NetworkSRV[space]

#define SpaceDSC(space)			Network.spaceDescriptor (space)
#define SpaceName(space)		SpaceDSC(space).spaceName
#define SpaceMinSegment(space)		SpaceDSC(space).minSegment
#define SpaceMaxSegment(space)		SpaceDSC(space).maxSegment
#define SpaceMappingVector(space)	SpaceDSC(space).mappingVector
#define SpaceCT(space)			SpaceDSC(space).ct
#define SpaceContainerCount(space)	PS_CT_EntryCount (SpaceCT (space))
#define SpaceMarkingVector(space)	SpaceDSC(space).markingVector

#define SegmentMapping(space, segment)	(&Network.segmentDescriptor ((space), (segment)).mapping)
#define SegmentAddress(space,segment)	(Network.segmentDescriptor ((space), (segment)).segmentAddress ())
#define SegmentHeader(space,segment)	((PS_SH*)SegmentAddress (space, segment))

#define SegmentExpectedTID(space,segment)	Network.segmentDescriptor ((space), (segment)).expectedTID
#define SegmentExpectedRID(space,segment)	Network.segmentDescriptor ((space), (segment)).expectedRID
#define SegmentIsLocked(space,segment)		Network.segmentDescriptor ((space), (segment)).isLocked
#define SegmentIsNeeded(space,segment)		Network.segmentDescriptor ((space), (segment)).isNeeded
#define SegmentHasExpectedID(space,segment)	Network.segmentDescriptor ((space), (segment)).hasExpectedID


/*********************************************
 *****  Segment Mapping and Reclamation  *****
 *********************************************/

/*************
 *  Globals  *
 *************/

static unsigned int		MappedSegmentCount	= 0,
#ifdef __VMS
				MappedSegmentLimit	= 500;
#else
				MappedSegmentLimit	= INT_MAX;
#endif

static void const*
#if defined(sun) && !defined(_LP64)
				MappedAddressThreshold	= (void const*)0x20000000;
#else
				MappedAddressThreshold	= 0;
#endif
static unsigned int		ReclamationCount	= 0;
static unsigned int		SmallLargeThreshold	= 10000;


/*******************************
 *  Unmapping and Reclamation  *
 *******************************/

static void Unmap (VkMemory *pMapping) {
    MappedSegmentCount--;
    pMapping->Destroy ();
}

bool VTransientServicesForOnprof::mappingsReclaimed () {
    ReclamationCount++;

    bool mappingsWereReclaimed = false;
    for (unsigned int space = 1; space < NetworkSpaceCount; space++) {
	for (
	    int segment  = SpaceMinSegment (space); 
	    segment <= SpaceMaxSegment (space);
	    segment++
	) {
	    if (SegmentAddress (space, segment) && !SegmentIsLocked (space, segment)) {
		Unmap (SegmentMapping (space, segment));
		mappingsWereReclaimed = true;
	    }
	}
    }

    return mappingsWereReclaimed; 
}

inline void ReclaimSegments () {
    g_iTransientServicesProvider.mappingsReclaimed ();
}


/*************
 *  Mapping  *
 *************/

static void ResolveSpaceLinks (char *spacePathName) {
    char linkPathName[1024], *nlp;
    FILE *ls;

    while (snprintf (linkPathName, sizeof(linkPathName), "%s/LINK", spacePathName),
	   access (linkPathName, 0) == 0)
    {
	if (NULL == (ls = fopen (linkPathName, "r")))
	    SignalUnixError ("Space Link Open");
	if (spacePathName != fgets (spacePathName, 1024, ls))
	    SignalUnixError ("Space Link Read");
	fclose (ls);

	if (IsntNil (nlp = strchr (spacePathName, '\n')))
	    *nlp = '\0';
    }
}

static void Map (
    VkMemory *pMapping, char const *directory, char const *space, unsigned int segment
) {
    char spacedir[1025], file[1100];

    snprintf (spacedir, sizeof(spacedir), "%s/%s", directory, space);
    ResolveSpaceLinks (spacedir);
    snprintf (file, sizeof(file), "%s/%u", spacedir, segment);

    if (++MappedSegmentCount >= MappedSegmentLimit)
	ReclaimSegments ();

    if (pMapping->Map (file, true, SegmentMemoryShare)) {
	if (MappedAddressThreshold > (void const*) pMapping->RegionAddress ()) {
	    ReclaimSegments ();
	    ++MappedSegmentCount;
	    if (pMapping->Map (file, true, SegmentMemoryShare)) return;
	} else
	    return;
    }
    SignalUnixError (
	MakeMessage ("Error Mapping File '%s'", file)
    );
}


/****************************
 *****  Network Access  *****
 ****************************/

/*******************************
 *  Structure Initialization   *
 *******************************/

PrivateFnDef void ResolveNetworkLink (char *pOSDPathName, char const *pNDFPathName) {
    char  pLinkBuffer[1024], *pNewLine;
    FILE* pLinkStream;

    snprintf (pLinkBuffer, sizeof(pLinkBuffer), "%s.OSDPATH", pNDFPathName);

#if defined(_WIN32)
    pLinkStream = fopen (pLinkBuffer, "rt");
#else
    pLinkStream = fopen (pLinkBuffer, "r");
#endif

/*****  If NDF.OSDPATH doesn't exist, assume the directory in which the NDF lives, ... *****/
    if (IsNil (pLinkStream)) {
	char const* pLastSlash = strrchr (pNDFPathName, '/');
	if (!pLastSlash)
	    strcpy (pOSDPathName, ".");
	else {
	    size_t const sDirname = static_cast<size_t>(pLastSlash - pNDFPathName);
	    strncpy (pOSDPathName, pNDFPathName, sDirname);
	    pOSDPathName[sDirname] = '\0';
	}
	return;
    }

    if (IsntNil (fgets (pLinkBuffer, sizeof (pLinkBuffer), pLinkStream))) {
	if (IsntNil (pNewLine = strchr (pLinkBuffer, '\n')))
	    *pNewLine = '\0';
	strcpy (pOSDPathName, pLinkBuffer);
    }

    fclose (pLinkStream);
}

static void LoadNetworkHeaderData (int fd) {
/*****  Read the NDH and check the version ... *****/
    Seek (fd, 0, SEEK_SET);

    PS_NDH ndh;
    Read (fd, &ndh, sizeof ndh, true);
    if (PS_NDH_NDFVersion (ndh) != PS_NDFVersionNumber)
	SignalApplicationError (
	    MakeMessage (
		"Error - the NDF version(%d) is newer than this program(%d)",
		PS_NDH_NDFVersion (ndh),
		PS_NDFVersionNumber
	    )
	);

/*****  Read the object space directory, ...   *****/
    unsigned int sOSDPathName;
    Read (fd, &sOSDPathName, sizeof sOSDPathName, true);
    Read (fd, NetworkOSDPathName, sOSDPathName, true);
    ResolveNetworkLink (NetworkOSDPathName, NetworkNDFPathName);

/*****  Read the most recent NVD and check the software version ...  *****/
    Seek (fd, (int)PS_NDH_CurrentNetworkVersion (ndh), SEEK_SET);

    PS_NVD nvd;
    Read (fd, &nvd, sizeof nvd, false);
    if (PS_NVD_SoftwareVersion (nvd) > PS_SoftwareVersion)
	SignalApplicationError (
	    MakeMessage (
		"Error - the software version of this program(%d) is older\n\tthan that of the object network(%d)",
		PS_SoftwareVersion,
		PS_NVD_SoftwareVersion (nvd)
	    )
	);

/*****  Read the SRV ...  *****/
    NetworkSRV = ReadSRV (
	fd,
	PS_NVD_SRV (nvd),
	PS_NVD_SoftwareVersion (nvd),
	&NetworkSpaceCount
    );

/*****  Look for segment mapping hints...  *****/
    V::VString iText (NetworkOSDPathName);
    iText.append ("/.hints");

    VSimpleFile iFile;
    if (iFile.OpenForTextRead (iText) && iFile.GetContents (iText)) {
	char const * const pMappingHintTag = "MappingHint:";
	char const *pMappingHint = strstr (iText, pMappingHintTag);
	if (pMappingHint) {
	    pMappingHint += strlen (pMappingHintTag);
	    bool bNotDone = true; bool bS = false, bG = false, bP = true, bValue = true;
	    while (bNotDone) {
		switch (*pMappingHint++) {
		case ' ':
		case '\t':
		    break;
		case '+':
		    bValue = true;
		    break;
		case '-':
		    bValue = false;
		    break;
		case 's':
		case 'S':
		    bS = bValue;
		    break;
		case 'g':
		case 'G':
		    bG = bValue;
		    break;
		case 'p':
		case 'P':
		    bP = bValue;
		    break;
		default:
		    bNotDone = false;
		    break;
		}
	    }
	    VkMemory::Share xShare = VkMemory::Share_None;
	    if (bS)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_System);
	    if (bG)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_Group);
	    if (bP)
		xShare = static_cast<VkMemory::Share>(xShare + VkMemory::Share_Private);
	    if (xShare != VkMemory::Share_None)
		SegmentMemoryShare = xShare;
	}
    }

}


static void InitializeSpaceVector (int fd) {
    unsigned int space;
    int		segment;
    PS_SVD	svd;
    PS_SD	sd;
    PS_Type_FO	fo;

    NetworkSDV = (SpaceDescriptor *)Allocate (sizeof (SpaceDescriptor) * NetworkSpaceCount);
    for (space = 1; space < NetworkSpaceCount; space++) {
	/*****  Initialize with no segments in each space  *****/
	Network.spaceDescriptor (space).Construct (space);
    }
    printf ("\nSpace Statistics:\n");
    for (space = 1; space < NetworkSpaceCount; space++) {
	SpaceDescriptor &rSpace = Network.spaceDescriptor (space);
	/*****  Access SVD  *****/
	if (PS_FO_Nil == PS_SRD_SVD (SpaceSRD (space)))
	    continue;
	Seek (fd, PS_SRD_SVD (SpaceSRD (space)), SEEK_SET);
	Read (fd, &svd, sizeof svd, false);

	/*****  Initialize Space Directory Names  *****/
	strcpy (SpaceName (space), PS_SVD_SpaceBaseName (svd));

	/*****  Initialize Mapping Vector  *****/
	SpaceMinSegment		(space) = PS_SVD_MinSegment (svd);
	SpaceMaxSegment		(space) = PS_SVD_MaxSegment (svd);
	SpaceMappingVector	(space) = (SegmentDescriptor *)Allocate(
	    sizeof (SegmentDescriptor) *
		(1 + PS_SVD_MaxSegment (svd) - PS_SVD_MinSegment (svd))
	);
	/*****  Enable Segment Verification if NDF has ID info  *****/
	for (
	    segment = PS_SVD_MinSegment (svd);
	    segment <= PS_SVD_MaxSegment (svd);
	    segment++
	) {
	    rSpace.segmentDescriptor (segment).Construct ();
	}
	for (
	     fo = PS_SVD_SD (svd);
	     PS_FO_IsntNil (fo);
	     fo = PS_SD_Predecessor (sd)
	)
	{
	    Seek (fd, (int)fo, SEEK_SET);
	    Read (fd, &sd, sizeof (sd), true);
	    segment = (int)PS_SD_Segment (sd);
	    if (segment < SpaceMinSegment (space)) break;
	    SegmentHasExpectedID (space, segment) = true;
	    SegmentExpectedTID	 (space,segment) = PS_SD_TID (sd);
	    SegmentExpectedRID	 (space,segment) = PS_SD_RID (sd);
	}
	/*****  Map Container Table  *****/
	segment = PS_SVD_RootSegment (svd);
	Map (
	    SegmentMapping (space, segment),
	    NetworkOSDPathName,
	    SpaceName (space),
	    segment
	);
	/*****  Perform Segment Verification if it is enabled  *****/
	if (SegmentHasExpectedID (space, segment) && (
		SegmentExpectedTID (space,segment) != PS_SH_TID (
		    SegmentHeader (space, segment)
		) || SegmentExpectedRID (space,segment) != PS_SH_RID (
		    SegmentHeader (space, segment)
		)
	    )
	) {
	    DisplayApplicationError (
		MakeMessage ("%s/%s/%d does not match NDF description!!\n",
			     NetworkOSDPathName,
			     SpaceName (space),
			     segment)
	    );
	}
	SegmentIsLocked (space, segment) =
	SegmentIsNeeded (space, segment) = true;

	SpaceCT (space) = PS_SH_ContainerBaseAsType (
	    SegmentAddress (space, segment), PS_SVD_CTOffset (svd), PS_CT
	);

	/*****  Initialize Marking Vector  *****/
	SpaceMarkingVector (space) = (mark_t*)Allocate (SpaceContainerCount (space) * sizeof (mark_t));

	/*****  Report Space Parameters  *****/
	printf (
	    "\tSpace %3d: Segments: %4d...%-4d  CTE Count: %6d\n",
	    space,
	    SpaceMinSegment (space),
	    SpaceMaxSegment (space),
	    SpaceContainerCount (space)
	);
	fflush (stdout);
    }
}


/*********************
 *  'AttachNetwork'  *
 *********************/

static void AttachNetwork (char const *ndf) {
    printf ("Processing Network Described by '%s'\n", ndf);
    int fd = Open (ndf);

    NetworkNDFPathName = ndf;

    LoadNetworkHeaderData (fd);
    InitializeSpaceVector (fd);

    Close (fd);
}


/*******************************
 *****  Container Mapping  *****
 *******************************/

M_CPreamble *SpaceDescriptor::MapContainer (unsigned int xContainer) {
    PS_CTE const &rCTE = containerCTE (xContainer);
    if (PS_CTE_IsFree (rCTE))
	return NilOf (M_CPreamble*);

    int xSegment = PS_CTE_Segment (rCTE);
    SegmentDescriptor &rSegment = segmentDescriptor (xSegment);
    PS_SH const *pSegmentHeader = rSegment.segmentHeader ();
    if (IsNil (pSegmentHeader)) {
	Map (&rSegment.mapping, NetworkOSDPathName, spaceName, xSegment);

	/*****  Verify Segment belongs to this NDF  *****/
	if (rSegment.hasIdentityProblems ()) {
	    DisplayApplicationError (
		MakeMessage (
		    "%s/%s/%d does not match NDF description!!\n",
		    NetworkOSDPathName, spaceName, xSegment
		)
	    );
	}
	rSegment.isNeeded = true;
	pSegmentHeader = rSegment.segmentHeader ();
    }

    return PS_SH_ContainerPreamble (pSegmentHeader, PS_CTE_Offset (rCTE));
}


/*******************************************
 *******************************************
 *****  Network Connectivity Analysis  *****
 *******************************************
 *******************************************/

/*************************************
 *****  Container Queue Manager  *****
 *************************************/

struct QueueEntry {
    struct QueueEntry
	*next;
    int
	space, index;
};

struct Queue {
    struct QueueEntry
	*head, *tail, *free;
};

static struct Queue Queue;


static void ExtendFreeList () {
    unsigned int n = 1024;
    QueueEntry *qe = (struct QueueEntry *)Allocate (n * sizeof (QueueEntry));

    while (n-- > 0) {
	qe->next	= Queue.free;
	Queue.free	= qe++;
    }
}


static void EnqueueContainer (unsigned int xSpace, unsigned int xContainer) {
    if (IsNil (Queue.free))
	ExtendFreeList ();

    struct QueueEntry *qe = Queue.free;
    Queue.free		= qe->next;

    qe->next		= NilOf (struct QueueEntry *);
    qe->space		= xSpace;
    qe->index		= xContainer;

    if (IsntNil (Queue.tail))
	Queue.tail->next= qe;
    else
	Queue.head	= qe;

    Queue.tail		= qe;
}

static void DequeueContainer (unsigned int *pxSpace, unsigned int *pxContainer) {
    struct QueueEntry *qe = Queue.head;

    if (IsNil (Queue.head = qe->next))
	Queue.tail = NilOf (struct QueueEntry *);

    *pxSpace	= qe->space;
    *pxContainer= qe->index;

    qe->next	= Queue.free;
    Queue.free	= qe;
}

#define ContainerQueueIsntEmpty	IsntNil (Queue.head)

/*******************************
 *****  Container Marking  *****
 *******************************/

#define UNMARKED	0
#define REFERENCED	1
#define PROCESSED	2
#define UNREASONABLE	3

void NetworkDescriptor::ClearMarks () {
    for (unsigned int xSpace = 1; xSpace < spaceCount; xSpace++) {
	SpaceDescriptor &rSpace = spaceDescriptor (xSpace);
	for (unsigned int xContainer = 0; rSpace.exists () && xContainer < rSpace.containerCount (); xContainer++) {
	    rSpace.SetMark (xContainer, UNMARKED);
	}
    }
}

void SpaceDescriptor::MarkContainer (unsigned int xContainer) {
    if (UNMARKED == GetMark (xContainer)) {
	SetMark (xContainer, REFERENCED);
	EnqueueContainer (m_xSpace, xContainer);
    }
}

void NetworkDescriptor::MarkContainer (unsigned int xSpace, unsigned int xContainer) {
    spaceDescriptor (xSpace).MarkContainer (xContainer);
}

void NetworkDescriptor::MarkInitialContainers () {
    for (unsigned int space = 1; space < spaceCount; space++) {
	SpaceDescriptor &rSpace = spaceDescriptor (space);
	if (rSpace.exists ())
	    rSpace.MarkContainer (0);		 // Container table
	if (rSpace.maxSegment > 0) 
	    rSpace.MarkContainer (1);		 // Space root (won't exist if
						 // max segment is 0)
    }
}

static void MarkPOP (M_POP const &pop) {
    Network.MarkContainer (M_POP_D_ObjectSpace (pop), M_POP_D_ContainerIndex (pop));
}

static void MarkPOPArray (M_POP *array, size_t nelements) {
    while (nelements-- > 0) {
	MarkPOP (*array++);
    }
}


/**********************************
 *****  Container Processing  *****
 **********************************/

/****************************
 *  Type Specific Handlers  *
 ****************************/

static void ProcessPArray (M_CPreamble *container) {
    M_POP *cp = M_CPreamble_ContainerBaseAsType (container, M_POP);

    MarkPOPArray (cp, (size_t)M_CPreamble_Size (container) / sizeof (M_POP));
}

static void ProcessBlock (M_CPreamble *container) {
    rtBLOCK_BlockType* cp = M_CPreamble_ContainerBaseAsType (container, rtBLOCK_BlockType);

    MarkPOP (rtBLOCK_localEnvironmentPOP (cp));
    if (rtBLOCK_PLVector (cp)) {
	rtBLOCK_PLVectorType *plv = (rtBLOCK_PLVectorType *)((V::pointer_t)(cp) + rtBLOCK_PLVector (cp));
	MarkPOPArray(rtBLOCK_PLVector_POP (plv), rtBLOCK_PLVector_Count (plv));
    }
}

static void ProcessClosure (M_CPreamble *container) {
    rtCLOSURE_Closure* cp = M_CPreamble_ContainerBaseAsType (container, struct rtCLOSURE_Closure);

    MarkPOP (rtCLOSURE_Closure_Context (cp));
    MarkPOP (rtCLOSURE_Closure_Block (cp));
}

static void ProcessPToken (M_CPreamble *container) {
    rtPTOKEN_Type *cp = M_CPreamble_ContainerBaseAsType (container, rtPTOKEN_Type);

    MarkPOP (rtPTOKEN_PT_nextGeneration (cp));
    if (rtPTOKEN_PT_TokenType (cp) == rtPTOKEN_TT_Cartesian) {
	MarkPOPArray (&rtPTOKEN_PT_CartesianPTokens(cp)->rowPToken, 2);
    }
}

static void ProcessLink (M_CPreamble *container) {
    rtLINK_Type *cp = M_CPreamble_ContainerBaseAsType (container, rtLINK_Type);

    MarkPOP (rtLINK_L_PosPToken (cp));
    MarkPOP (rtLINK_L_RefPToken (cp));
}

static void ProcessVector (M_CPreamble *container) {
    rtVECTOR_Type *cp = M_CPreamble_ContainerBaseAsType (container, rtVECTOR_Type);

    MarkPOP (rtVECTOR_V_PToken (cp));
    MarkPOPArray (
	&rtVECTOR_V_USegArray(cp)->pToken,
	rtVECTOR_V_USegArraySize (cp) * rtVECTOR_USD_POPsPerUSD
    );
}

static void ProcessLStore (M_CPreamble *container) {
    rtLSTORE_LStore *cp = M_CPreamble_ContainerBaseAsType (container, rtLSTORE_LStore);

    MarkPOP (rtLSTORE_LStore_RowPToken		(cp));
    MarkPOP (rtLSTORE_LStore_Content		(cp));
    MarkPOP (rtLSTORE_LStore_ContentPToken	(cp));
}

#if defined(__GNUC__) && defined(VMS_LINUX_EXPLICIT_COMPAT)
#pragma GCC diagnostic warning "-Wattributes"
#endif
static void ProcessDescriptor0 (M_CPreamble *container) {
    struct Descriptor0 {
	DSC_PointerType		pointerType;
	M_POP			store,
				pointer;
    } *cp;

    cp = M_CPreamble_ContainerBaseAsType (container, struct Descriptor0);

    MarkPOP (cp->store);
    MarkPOP (cp->pointer);
}
#if defined(__GNUC__) && defined(VMS_LINUX_EXPLICIT_COMPAT)
#pragma GCC diagnostic error "-Wattributes"
#endif

static void ProcessDescriptor (M_CPreamble *container) {
    rtDSC_Descriptor* cp = M_CPreamble_ContainerBaseAsType (container, struct rtDSC_Descriptor);

    MarkPOP (rtDSC_Descriptor_Store	(cp));
    MarkPOP (rtDSC_Descriptor_Pointer	(cp));
}

static void ProcessUVector (M_CPreamble *container, SpaceDescriptor &rSpace) {
    UV_PreambleType *cp = M_CPreamble_ContainerBaseAsType (container, UV_PreambleType);

    MarkPOP (UV_Preamble_D_PToken	(*cp));
    MarkPOP (UV_Preamble_D_RefPT	(*cp));
    MarkPOP (UV_Preamble_D_AuxiliaryPOP	(*cp));

    switch (M_CPreamble_RType (container)) {
    case RTYPE_C_DoubleUV:
        rSpace.checkAlignment (reinterpret_cast<double const*>(cp + 1), UV_Preamble_D_ElementCount (*cp));
        break;
    }
}

static void ProcessPOPVector (M_CPreamble *container) {
    POPVECTOR_PVType *cp = M_CPreamble_ContainerBaseAsType (container, POPVECTOR_PVType);

    MarkPOPArray (POPVECTOR_PV_Array (cp), POPVECTOR_PV_ElementCount (cp));
}


/****************
 *  Dispatcher  *
 ****************/

static void MarkReferencedContainers (M_CPreamble *container, SpaceDescriptor &rSpace) {
    switch ((RTYPE_Type)M_CPreamble_RType (container)) {
    case RTYPE_C_Undefined:
    case RTYPE_C_Undefined0:
    case RTYPE_C_TCT:
    case RTYPE_C_PCT:
    case RTYPE_C_String:
	break;

    case RTYPE_C_PArray:
	ProcessPArray (container);
	break;

    case RTYPE_C_Block:
	ProcessBlock (container);
	break;

    case RTYPE_C_Closure:
	ProcessClosure (container);
	break;

    case RTYPE_C_PToken:
	ProcessPToken (container);
	break;

    case RTYPE_C_Link:
	ProcessLink (container);
	break;

    case RTYPE_C_Vector:
	ProcessVector (container);
	break;

    case RTYPE_C_ListStore:
	ProcessLStore (container);
	break;

    case RTYPE_C_Descriptor0:
	ProcessDescriptor0 (container);
	break;

    case RTYPE_C_Descriptor1:
    case RTYPE_C_Descriptor:
	ProcessDescriptor (container);
	break;

    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_SelUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
	ProcessUVector (container, rSpace);
	break;

    case RTYPE_C_Context:
    case RTYPE_C_Dictionary:
    case RTYPE_C_Index:
    case RTYPE_C_MethodD:
    case RTYPE_C_Method:
    case RTYPE_C_ValueStore:
	ProcessPOPVector (container);
	break;

    default:
	DisplayApplicationError (
	    MakeMessage (
		">>>  Unrecognized Container R-Type <%d>\n",
		M_CPreamble_RType (container)
	    )
	);
	break;
    }
}


void NetworkDescriptor::ProcessNextContainer () {
    static int counter = 0;

    if (0 == counter % 70000)
	printf ("\n");
    if (0 == counter++ % 1000) {
	printf ("-");
	fflush (stdout);
    }

    unsigned int xSpace, xContainer;
    DequeueContainer (&xSpace, &xContainer);
    SpaceDescriptor &rSpace = spaceDescriptor (xSpace);
    M_CPreamble *container = rSpace.MapContainer (xContainer);
    if (IsNil (container)) {
	rSpace.SetMark (xContainer, UNREASONABLE);

	DisplayApplicationError (
	    MakeMessage (
		"\n>>>  Container [%d:%d] Referenced But Not In Use  <<<\n",
		xSpace, xContainer
	    )
	);
	return;
    }

    rSpace.SetMark (xContainer, PROCESSED);

    MarkReferencedContainers (container, rSpace);
}


/***************************************
 *****  Connectivity Verification  *****
 ***************************************/

void NetworkDescriptor::VerifyNetworkConnectivity () {
    ClearMarks ();
    MarkInitialContainers ();
    while (ContainerQueueIsntEmpty)
	ProcessNextContainer ();
}


/**********************************
 *****  Connectivity Display  *****
 **********************************/

/*****************
 *  Definitions  *
 *****************/

struct ConnectivityStatistics {
    unsigned __int64
	reachableCount		, reachableBytes	,
	reachableSmall		, reachableSmallBytes	, 
	unreachableCount	, unreachableBytes	,
	unreachableSmall	, unreachableSmallBytes	, 
	reachableSegments	, freeCount		,
        unreasonableCount	,
        misalignedContainers	, misalignedValues	;
    struct counterSet_t {
	unsigned __int64
	    reachableCount	, reachableBytes	,
	    unreachableCount	, unreachableBytes	;
    } rtype[(unsigned int)RTYPE_C_MaxType];
};


/***************
 *  Utilities  *
 ***************/

static void InitializeStatistics (ConnectivityStatistics *stats) {
    stats->reachableCount	=
    stats->reachableBytes	=
    stats->reachableSmall	=
    stats->reachableSmallBytes	=
    stats->unreachableCount	=
    stats->unreachableBytes	=
    stats->unreachableSmall	=
    stats->unreachableSmallBytes=
    stats->reachableSegments	=
    stats->freeCount		=
    stats->unreasonableCount	=
    stats->misalignedContainers	=
    stats->misalignedValues	= 0;
    for (unsigned i = 0; i < (unsigned int)RTYPE_C_MaxType; i++) {
	stats->rtype[i].reachableCount	=
	stats->rtype[i].reachableBytes	=
	stats->rtype[i].unreachableCount=
	stats->rtype[i].unreachableBytes= 0;
    }
}

static void AccumulateStatistics (ConnectivityStatistics *dst, ConnectivityStatistics const *src) {
    dst->reachableCount		+= src->reachableCount;
    dst->reachableBytes		+= src->reachableBytes;
    dst->reachableSmall		+= src->reachableSmall;
    dst->reachableSmallBytes	+= src->reachableSmallBytes;
    dst->unreachableCount	+= src->unreachableCount;
    dst->unreachableBytes	+= src->unreachableBytes;
    dst->unreachableSmall	+= src->unreachableSmall;
    dst->unreachableSmallBytes	+= src->unreachableSmallBytes;
    dst->reachableSegments	+= src->reachableSegments;
    dst->freeCount		+= src->freeCount;
    dst->unreasonableCount	+= src->unreasonableCount;
    dst->misalignedContainers	+= src->misalignedContainers;
    dst->misalignedValues	+= src->misalignedValues;
    for (unsigned int i = 0; i < (unsigned int)RTYPE_C_MaxType; i++) {
	dst->rtype[i].reachableCount	+= src->rtype[i].reachableCount;
	dst->rtype[i].reachableBytes	+= src->rtype[i].reachableBytes;
	dst->rtype[i].unreachableCount	+= src->rtype[i].unreachableCount;
	dst->rtype[i].unreachableBytes	+= src->rtype[i].unreachableBytes;
    }
}

static void DisplayStatistics (ConnectivityStatistics const *stats, bool bProfiling, bool bShowingMisalignments) {
    printf ("\
\tReachable Segments  . . . %14" FMT64 "u\n\
\tReachable Containers  . . %14" FMT64 "u%14" FMT64 "u%14" FMT64 "u\n\
\t          Bytes . . . . . %14" FMT64 "u%14" FMT64 "u%14" FMT64 "u\n\
\tUnreachable Containers  . %14" FMT64 "u%14" FMT64 "u%14" FMT64 "u\n\
\t            Bytes . . . . %14" FMT64 "u%14" FMT64 "u%14" FMT64 "u\n\
\tFree Containers . . . . . %14" FMT64 "u\n",

	stats->reachableSegments,
	stats->reachableCount,
	stats->reachableSmall,
	stats->reachableCount - stats->reachableSmall,
	stats->reachableBytes,
	stats->reachableSmallBytes,
	stats->reachableBytes - stats->reachableSmallBytes,
	stats->unreachableCount,
	stats->unreachableSmall,
	stats->unreachableCount - stats->unreachableSmall,
	stats->unreachableBytes,
	stats->unreachableSmallBytes,
	stats->unreachableBytes - stats->unreachableSmallBytes,
	stats->freeCount
    );

    printf ("\t\t  (Small Containers contain less than %u bytes)\n", SmallLargeThreshold);
		 
    if (stats->unreasonableCount) {
	printf (
	    ">>>\tUnreasonable Containers . %14" FMT64 "u\n", stats->unreasonableCount
	);
    }
    if (bShowingMisalignments && stats->misalignedContainers) {
	printf (
	    ">>>\tMisaligned Containers . . %14" FMT64 "u\n", stats->misalignedContainers
	);
	printf (
	    ">>>\tMisaligned Values . . . . %14" FMT64 "u\n", stats->misalignedValues
	);
    }

    if (!bProfiling)
        return;

    printf (
	"\tBy RType:\n%-16.16s %10.10s %19.19s %10.10s %19.19s\n",
	"R-Type", "Reach. #", "Reach. Bytes", "Unreach. #", "Unreach. Bytes"
    );
    printf (
	"%-16.16s %10.10s %19.19s %10.10s %19.19s\n",
	"--------------------",
	"--------------------",
	"--------------------",
	"--------------------",
	"--------------------"
    );
    for (unsigned int i = 0; i < (unsigned int)RTYPE_C_MaxType; i++) {
	if (0 == stats->rtype[i].reachableCount &&
	    0 == stats->rtype[i].unreachableCount
	) continue;

	printf (
	    "%-16.16s %10" FMT64 "u %19" FMT64 "u %10" FMT64 "u %19" FMT64 "u\n",
	    RTypeName ((RTYPE_Type)i),
	    stats->rtype[i].reachableCount,
	    stats->rtype[i].reachableBytes,
	    stats->rtype[i].unreachableCount,
	    stats->rtype[i].unreachableBytes
	);
    }
}


/********************
 *  Space Analysis  *
 ********************/

void SpaceDescriptor::CollectStatistics (ConnectivityStatistics *stats) {
    InitializeStatistics (stats);
    stats->misalignedContainers = m_cMisalignedContainers;
    stats->misalignedValues = m_cMisalignedValues;

    for (unsigned int xContainer = 0; xContainer < containerCount (); xContainer++) {
	M_CPreamble *container = MapContainer (xContainer);
	size_t size = IsNil (container)
	    ? 0
	    : M_CPreamble_OSize (container) ||
	      RTYPE_C_Undefined0 == (RTYPE_Type)M_CPreamble_RType (container)
	    ? M_CPreamble_OSize (container)
	    + sizeof (M_CPreamble)
	    + sizeof (M_CEndMarker)
	    - sizeof (unsigned int)
	    : M_CPreamble_NSize (container)
	    + sizeof (M_CPreamble)
	    + sizeof (M_CEndMarker);

	if (IsntNil (container) && 0 == M_CPreamble_OSize (
		container
	    ) &&  M_CPreamble_NSize (
		container
	    ) != M_CEndMarker_Size (M_CPreamble_EndMarker (container))
	) {
	    DisplayApplicationError (
		MakeMessage (
		    "\n+++ [%u:%u] Framing Error: POP=[%u:%u], RType=%d, Segment=%u, Offset=%u, Preamble=%u[%08x], EndMarker=%u[%08x]",
		    m_xSpace,
		    xContainer,
		    M_CPreamble_POPObjectSpace (container),
		    M_CPreamble_POPContainerIndex (container),
		    M_CPreamble_RType (container),
		    containerSegment (xContainer),
		    containerOffset  (xContainer),
		    M_CPreamble_NSize (container),
		    M_CPreamble_NSize (container),
		    M_CEndMarker_Size (M_CPreamble_EndMarker (container)),
		    M_CEndMarker_Size (M_CPreamble_EndMarker (container))
		)
	    );
	}

	switch (GetMark (xContainer)) {
	case UNMARKED:
	    if (IsntNil (container)) {
		stats->unreachableCount++;
		stats->unreachableBytes += size;
		if (SmallLargeThreshold > size) {
		    stats->unreachableSmall++;
		    stats->unreachableSmallBytes += size;
		}

		stats->rtype[M_CPreamble_RType(container)].unreachableCount++;
		stats->rtype[M_CPreamble_RType(container)].unreachableBytes += size;
	    }
	    else {
		stats->freeCount++;
	    }
	    break;
	case REFERENCED:
	    DisplayApplicationError (
		MakeMessage (
		    ">>>  Referenced Container [%d:%d] Encountered by Statistics Generator  <<<\n",
		    m_xSpace, xContainer
		)
	    );
	    break;
	case PROCESSED:
	    stats->reachableCount++;
	    stats->reachableBytes += size;
	    if (SmallLargeThreshold > size) {
		stats->reachableSmall++;
		stats->reachableSmallBytes += size;
	    }
	    stats->rtype[M_CPreamble_RType(container)].reachableCount++;
	    stats->rtype[M_CPreamble_RType(container)].reachableBytes += size;
	    break;
	case UNREASONABLE:
	    stats->unreasonableCount++;
	    break;
	default:
	    DisplayApplicationError (
		MakeMessage (
		    ">>>  Unrecognized Mark <%d> on Container [%d:%d]  <<<\n",
		    m_xSpace, xContainer
		)
	    );
	    break;
	}
    }

    for (int xSegment = minSegment; xSegment <= maxSegment; xSegment++)
	stats->reachableSegments += segmentDescriptor (xSegment).isNeeded ? 1 : 0;
}


/************
 *  Driver  *
 ************/

void NetworkDescriptor::DisplayConnectivityData (bool bProfiling, bool bShowingMisalignments) const {
    ConnectivityStatistics networkStats, spaceStats;
    InitializeStatistics (&networkStats);

    for (unsigned int xSpace = 1; xSpace < spaceCount; xSpace++) {
	SpaceDescriptor &rSpace = spaceDescriptor (xSpace);
	if (rSpace.exists ()) {
	    rSpace.CollectStatistics (&spaceStats);
	    AccumulateStatistics (&networkStats, &spaceStats);

	    printf ("\nSpace %d Statistics:\t\t\t   Total\t Small\t       Large\n", xSpace);
	    DisplayStatistics (&spaceStats, bProfiling, bShowingMisalignments);
	}
    }
    printf ("\n\nNetwork Total Statistics:\t\t   Total\t Small\t       Large\n");
    DisplayStatistics (&networkStats, bProfiling, bShowingMisalignments);

    if (ReclamationCount > 0) printf (
	"\n\n%u Segment Reclamations Performed.\n", ReclamationCount
    );
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    V::VCommandLine iCommandLine (argc, argv);
    V::VCommandLine::Cursor iCommandCursor (&iCommandLine);
    char const *pNDF = iCommandCursor.firstToken ();
    if (!pNDF)
        pNDF = iCommandCursor.firstString ("n", "NDFPathName");

    if (!pNDF) SignalApplicationError (
	MakeMessage ("Usage: %s NDF\n", argv[0])
    );

    int prefix = strlen (argv[0]) - strlen ("onck");
    bool bProfiling = prefix < 0 || strcmp (argv[0] + prefix, "onck") != 0;

    char const *estring; unsigned long evalue;
    if (IsntNil (estring = getenv ("VisionMappingLimit")) &&
	(evalue = strtoul (estring, (char **)NULL, 0)) > 0
    ) MappedSegmentLimit = evalue;
    if (IsntNil (estring = getenv ("VisionAddressThreshold")) &&
	(evalue = strtoul (estring, (char **)NULL, 0)) > 0
    ) MappedAddressThreshold = (void const*)evalue;
    if (IsntNil (estring = getenv ("VisionLargeContainerSize")) &&
	(evalue = strtoul (estring, (char **)NULL, 0)) > 0
    ) SmallLargeThreshold = evalue;

    VkMemory::StartGRM (
	argv[0], NilOf (char const*), NilOf (VkMemoryTraceCallback), SignalUnixError
    );

    AttachNetwork (pNDF);

    Network.VerifyNetworkConnectivity ();
    Network.DisplayConnectivityData (bProfiling, iCommandLine.switchValue ("misalignments"));

    if (ApplicationErrorOccurred) {
	printf ("\n!!!!  Network consistency check FAILED  !!!!\n");
	return ErrorExitValue;
    }
    else {
	printf ("\n>>> Network consistency check passed <<<\n");
	return NormalExitValue;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  onck.c 1 replace /users/mjc/system
  871017 21:59:06 mjc Create Garbage Collection Checking Utility

 ************************************************************************/
/************************************************************************
  onck.c 2 replace /users/jad/system/TOOLS
  880314 12:57:47 jad NDF structure changes -> Version 2 NDF

 ************************************************************************/
/************************************************************************
  onck.c 3 replace /vision/source/revision/tools/ndfv2
  880322 13:59:23 jad changed the printing of dots to dashes
because mailx eats everything that occurs after a dot

 ************************************************************************/
/************************************************************************
  onck.c 4 replace /users/mjc/Workbench/tools
  880429 18:55:41 mjc Add cross disk linking capabilities

 ************************************************************************/
/************************************************************************
  onck.c 5 replace /users/jck/tools
  880503 09:05:39 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  onck.c 6 replace /users/mjc/Workbench/port/tools
  880519 22:55:14 mjc Convert tools to use NDF file server on the Apollo

 ************************************************************************/
/************************************************************************
  onck.c 7 replace /users/jck/tools
  890914 22:17:37 jck Removed References to RTtstore

 ************************************************************************/

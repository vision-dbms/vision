/*****  The Persistent Storage Manager Shared Declarations  *****/
#ifndef PS_D
#define PS_D

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

#include "m.d"


/*********************************
 *********************************
 *****  General Definitions  *****
 *********************************
 *********************************/

/*********************
 *****  User ID  *****
 *********************/
/*---------------------------------------------------------------------------
 * A 'User ID' records the current real and effective identification of a user.
 *
 *  User ID Field Descriptions:
 *	realUID			- the real user ID of the user.
 *	effectiveUID		- the effective user ID of the user.
 *	realGID			- the real group ID of the user.
 *	effectiveGID		- the effective group ID of the user.
 *
 *---------------------------------------------------------------------------
 */
struct PS_UID {
    unsigned short		realUID,
				effectiveUID,
				realGID,
				effectiveGID;
};

/*****  Access Macros  *****/
#define PS_UID_RealUID(uid)	((uid).realUID)
#define PS_UID_EffectiveUID(uid)((uid).effectiveUID)
#define PS_UID_RealGID(uid)	((uid).realGID)
#define PS_UID_EffectiveGID(uid)((uid).effectiveGID)


/****************************
 *****  Transaction Id  *****
 ****************************/
/*---------------------------------------------------------------------------
 * A 'Transaction ID' uniquely identifies a data base update transaction.
 *---------------------------------------------------------------------------
 */
struct PS_TID {
//  Query
public:
    int operator == (PS_TID const &rOther) const {
	return high == rOther.high && low == rOther.low && sequence == rOther.sequence;
    }
    int operator != (PS_TID const &rOther) const {
	return !operator == (rOther);
    }

//  State
public:
    unsigned int high, low, sequence;
};

/*****  Access Macros  *****/
#define PS_TID_High(tid)	((tid).high)
#define PS_TID_Low(tid)		((tid).low)
#define PS_TID_Sequence(tid)	((tid).sequence)


/*************************
 *****  Resource Id  *****
 *************************/
/*---------------------------------------------------------------------------
 * A 'Resource Id' uniquely identifies a resource created by a transaction.
 *---------------------------------------------------------------------------
 */

struct PS_RID {
//  Comparison
public:
    int operator == (PS_RID const &rOther) const {
	return type == rOther.type && sequence == rOther.sequence;
    }
    int operator != (PS_RID const &rOther) const {
	return !operator== (rOther);
    }

//  State
public:
    unsigned short type, sequence;
};

/*****  Access Macros  *****/
#define PS_RID_Type(rid)	((rid).type)
#define PS_RID_Sequence(rid)	((rid).sequence)

/*****  Resource Types  *****/
#define PS_ResourceType_UpdateSegment		1
#define PS_ResourceType_IncorporatorSegment	2


/************************
 *****  Time Stamp  *****
 ************************/
/*---------------------------------------------------------------------------
 * A 'TimeStamp' is synonomous with the notion of absolute time defined in
 * "stdoss.h".
 *---------------------------------------------------------------------------
 */

class PS_TS : public time {
//  Comparisons
public:
    int operator < (PS_TS const &rOther) const {
	return seconds < rOther.seconds || (
	    seconds == rOther.seconds && microseconds < rOther.microseconds
	);
    }
    int operator <= (PS_TS const &rOther) const {
	return seconds < rOther.seconds || (
	    seconds == rOther.seconds && microseconds <= rOther.microseconds
	);
    }
    int operator == (PS_TS const &rOther) const {
	return seconds == rOther.seconds && microseconds == rOther.microseconds;
    }
    int operator != (PS_TS const &rOther) const {
	return seconds != rOther.seconds || microseconds != rOther.microseconds;
    }
    int operator >= (PS_TS const &rOther) const {
	return seconds > rOther.seconds || (
	    seconds == rOther.seconds && microseconds >= rOther.microseconds
	);
    }
    int operator > (PS_TS const &rOther) const {
	return seconds > rOther.seconds || (
	    seconds == rOther.seconds && microseconds > rOther.microseconds
	);
    }
};

/*****  Access Macros  *****/
#define PS_TS_Seconds(ts) 	((ts).seconds)
#define PS_TS_MicroSeconds(ts)	((ts).microseconds)


/*************************
 *****  File Offset  *****
 *************************/
/*---------------------------------------------------------------------------
 * A 'FileOffset' is a 'SEEK_SET' offset into a file.  A 'FileOffset' is
 * normally treated as a unsigned integer; however, it may be declared as a
 * structure field of width 'PS_FileOffset_FieldWidth'.
 *---------------------------------------------------------------------------
 */

/*****  Type Definition  *****/
#define PS_FO_FieldWidth	30

typedef unsigned int PS_Type_FO;

/*****  Nil Definitions  *****/
#define PS_FO_Nil		(~((~0) << PS_FO_FieldWidth))
#define PS_FO_IsNil(fo)		(PS_FO_Nil == (fo))
#define PS_FO_IsntNil(fo)	(PS_FO_Nil != (fo))


/********************************
 ********************************
 *****  Network Structures  *****
 ********************************
 ********************************/

/********************************
 *****  Symbolic Constants  *****
 ********************************/

/*********************************
 *  Network Directory Signature  *
 *********************************/

#define PS_NetworkDirectorySignature	314159265

/*******************************************
 *  Network Directory File Version Number  *
 *******************************************/
/*---------------------------------------------------------------------------
 *  HISTORY:
 *
 *  VERSION	DATE		REASON
 *  -------	----		------
 *  1     	Feb 1987	initial release 
 *  2		March 1988	added a software version field
 *				to the NVD structure.  Renamed the
 *				Software version field in the NDH to
 *				NDFVersion.
 *---------------------------------------------------------------------------
 */
#define PS_NDFVersionNumber	2

/*****************************
 *  Software Version Number  *
 *****************************/
/*---------------------------------------------------------------------------
 *  HISTORY:
 *
 *  VERSION	DATE		REASON
 *  -------	----		------
 *  1	  	Feb 1987	software at time of initial release
 *  2		Jan 1993	type definitions of 'rtDSC_Type' and
 *				'PS_SRD' changed to have the same
 *				alignment on all platforms.
 *  3		Jan 1993	Maximum container size increased to 4GB.
 *  4		Oct 1996	High Frequency Time Series
 *---------------------------------------------------------------------------
 */
#define PS_OriginalSystem	1
#define PS_PlatformAlignment	2
#define PS_4GBContainers	3
#define PS_HighFreqTimeSeries	4

#define PS_SoftwareVersion	PS_HighFreqTimeSeries


/**************************************
 *****  Network Directory Header  *****
 **************************************/

/*---------------------------------------------------------------------------
 * A network directory contains a rooted, directed, acyclic graph.   The root
 * of that graph is the Network Directory Header found at the beginning of the
 * network directory file.
 *
 *  Network Directory Header Field Descriptions:
 *	signature		- a 'unique' bit pattern used heuristically by
 *				  the persistent storage manager to determine
 *				  the validity of a network directory file.
 *	ndfVersion		- the version number of this Network Directory
 *				  file.  
 *	directoryVersion	- the number of network compactions performed
 *				  using this NDF.
 *	modificationTimeStamp	- the timestamp of the last update to this
 *				  network directory file.
 *	currentNetworkVersion	- the network directory file offset of the
 *				  Network Version Descriptor associated with
 *				  the current version of the network.
 *	alternateVersionChainHead
 *	alternateVersionChainMark
 *				- fields reserved to support the '-x' option
 *				  of 'ndftool'.
 *	signature2		- a duplicate of 'signature' whose validity
 *				  designates a version 5.6 or greater NDH
 *				  format.
 *	layoutType		- the layout of this NDH.  Introduced in
 *				  version 5.6 to allow NDH's to be self
 *				  describing.  The following values apply:
 *					1 - version 5.6 layout.  Ends at
 *					    'generation'.
 *	generation		- the generation of the version chain.
 *				  Incremented by 'ndftool' whenever a
 *				  rollback is performed.
 *
 * The Network Directory Header is declared to occupy 512 bytes to provide
 * space for future expansion.
 *
 *****/

union PS_NDH {
    struct component_t {
	unsigned int		signature,
				ndfVersion,
				directoryVersion;
	PS_TS			modificationTimeStamp;
	PS_Type_FO		currentNetworkVersion,
				alternateVersionChainHead,
				alternateVersionChainMark;
	unsigned int		signature2,
				layoutType,
				generation;
    } components;
    char			space [512];
};


/*****  Access Macros  *****/
#define PS_NDH_Signature(ndh)		((ndh).components.signature)
#define PS_NDH_NDFVersion(ndh)		((ndh).components.ndfVersion)
#define PS_NDH_DirectoryVersion(ndh)	((ndh).components.directoryVersion)
#define PS_NDH_ModificationTimeStamp(ndh)\
				((ndh).components.modificationTimeStamp)
#define PS_NDH_CurrentNetworkVersion(ndh)\
				((ndh).components.currentNetworkVersion)
#define PS_NDH_AlternateVersionChainHead(ndh)\
				((ndh).components.alternateVersionChainHead)
#define PS_NDH_AlternateVersionChainMark(ndh)\
				((ndh).components.alternateVersionChainMark)
#define PS_NDH_Signature2(ndh)	((ndh).components.signature2)
#define PS_NDH_LayoutType(ndh)	((ndh).components.layoutType)
#define PS_NDH_Generation(ndh)	((ndh).components.generation)


/****************************************
 *****  Network Version Descriptor  *****
 ****************************************/

/*---------------------------------------------------------------------------
 * A 'Network Version Descriptor' records the state associated with a version
 * of the object network.  Network Version Descriptors are organized as a
 * chain with each NVD pointing to the NVD from which it was derived.  As
 * noted above, the NVD describing the current state of the object network is
 * pointed to by the Network Directory Header.
 *
 *  Network Version Descriptor Field Descriptions:
 *	creator			- a structure defining the creator of this
 *				  network version.
 *	commitTimeStamp		- the commitment timestamp of this network
 *				  version.
 *	softwareVersion		- the version of the software that this network
 *				  version was saved with.
 *	srv			- the network directory file offset of a
 *				  counted array of 'spaceCount' 'Space Role
 *				  Descriptors'.  Each SRD identifies for its
 *				  space the SVD included in this network
 *				  version and the role that space had in
 *				  creating this version.
 *	newSpacePathName	- the network directory file offset of a
 *				  counted string containing the path name of
 *				  the directory in which new object spaces are
 *				  to be created.
 *	previousVersion		- the network directory file offset of the
 *				  Network Version Descriptor associated with
 *				  the previous version of the object network.
 *	previousNVDchain	- the network directory file offset of the 
 *				  chain of Network Version Descriptors replaced
 *				  by the last network compaction.  If none, 
 *				  this field will be set to PS_FO_Nil.
 *	updateThread		- the network directory file offset of the
 *				  Network Version Descriptor defining the
 *				  network version from which this version was
 *				  derived.  The update thread created by a
 *				  process ends when 'updateThread' equals
 *				  'accessedVersion'.
 *	accessedVersion		- the network directory file offset of the
 *				  Network Version Descriptor defining the
 *				  network version accessed by the process
 *				  which created this network version.
 *	directoryVersion	- the number of times the network containing
 *				  this NVD has been compacted.
 *	signature		- a signature word whose validity indicates
 *				  a version 5.6 or greater NVD layout.
 *	tid			- the id of the transaction which created the
 *				  data base version described by this NVD.
 *	td			- the offset of the transaction data object
 *				  describing the transaction which created this
 *				  specific NVD.  As of this implementation,
 *				  this structure is always 'Nil'.
 *	mp			- the offset of the NVD from which this NVD was
 *				  derived by either the compactor or rollback
 *				  (ndftool) tool.  Initially 'PS_FO_Nil'.
 *	annotation		- the offset of the annotation text for this
 *				  version or 'PS_FO_Nil' if no annotation is
 *				  available.
 *	extensions		- the file offset of the list of extension
 *				  structures attached to this NVD.  Currently,
 *				  there are no NVD extension structures so this
 *				  slot is always set to 'PS_FO_Nil'.
 *****/

struct PS_NVD {
    PS_UID			creator;
    PS_TS			commitTimeStamp;
    int				softwareVersion;
    PS_Type_FO			srv,
				newSpacePathName,
				previousVersion,
				previousNVDChain,
				updateThread,
				accessedVersion;
    unsigned int		directoryVersion,
    				signature,
				baseVersion	:  1,
				unusedFlags	: 31;
    PS_TID			tid;
    PS_Type_FO			td,
				mp,
				annotation,
				extensions;
};

/*****  Access Macros  *****/
#define PS_NVD_Creator(nvd)		((nvd).creator)
#define PS_NVD_CommitTimeStamp(nvd)	((nvd).commitTimeStamp)
#define PS_NVD_SoftwareVersion(nvd)	((nvd).softwareVersion)
#define PS_NVD_SRV(nvd)			((nvd).srv)
#define PS_NVD_NewSpacePathName(nvd)	((nvd).newSpacePathName)
#define PS_NVD_PreviousVersion(nvd)	((nvd).previousVersion)
#define PS_NVD_PreviousNVDChain(nvd)	((nvd).previousNVDChain)
#define PS_NVD_UpdateThread(nvd)	((nvd).updateThread)
#define PS_NVD_AccessedVersion(nvd)	((nvd).accessedVersion)
#define PS_NVD_DirectoryVersion(nvd)	((nvd).directoryVersion)
#define PS_NVD_Signature(nvd)		((nvd).signature)
#define PS_NVD_BaseVersion(nvd)		((nvd).baseVersion)
#define PS_NVD_UnusedFlags(nvd)		((nvd).unusedFlags)
#define PS_NVD_TID(nvd)			((nvd).tid)
#define PS_NVD_TD(nvd)			((nvd).td)
#define PS_NVD_MP(nvd)			((nvd).mp)
#define PS_NVD_Annotation(nvd)		((nvd).annotation)
#define PS_NVD_Extensions(nvd)		((nvd).extensions)


/***********************************
 *****  Space Role Descriptor  *****
 ***********************************/

/*---------------------------------------------------------------------------
 * A 'Space Role Descriptor' identifies a version of an object space and the
 * role that space played in creating a network version.  The associated space
 * of an SRD is determined by its position in the SRV associated with a NVD.
 *
 *  Space Role Descriptor Role Types:
 *	Nonextant		- the space associated with this SRD does not
 *				  exist in this version of the network.
 *	Unaccessed		- the space associated with this SRD was
 *				  inherited with no access by the process that
 *				  created this network version.
 *	Read			- the space associated with this SRD was
 *				  read but not modified by the process that
 *				  created this network version.
 *	Modified		- the space associated with this SRD was
 *				  read and modified by this network version.
 *
 *  Space Role Descriptor Field Descriptions:
 *	role			- the role type (chosen from the preceeding
 *				  list) associated with this SRD.
 *	svd			- the file offset in the network directory
 *				  file of the SVD associated with this space's
 *				  role in this version the network.
 *
 *****/

/*****  Role Types  *****/
#define PS_Role_Nonextant	0
#define PS_Role_Unaccessed	1
#define PS_Role_Read		2
#define PS_Role_Modified	3

/*****  Type Definition  *****/
struct PS_SRD {
    unsigned int		role	: 32 - PS_FO_FieldWidth,
				svd	: PS_FO_FieldWidth;
};

/*****  Access Macros *****/
#define PS_SRD_Role(srd)	((srd).role)
#define PS_SRD_SVD(srd)		((srd).svd)


/********************************
 *****  Segment Descriptor  *****
 ********************************/

/*---------------------------------------------------------------------------
 * A 'Segment Descriptor' describes a specific data base segment.
 *
 *  Segment Descriptor Field Definitions:
 *	predecessor		- the offset of the predecessor segment
 *				  descriptor for this space.
 *	tid			- the transaction id of the segment.
 *	rid			- the resource id of the segment.
 *	segment			- the number of the segment described by this
 *				  descriptor.
 *	extensions		- the offset of the first extension in the list
 *				  of extensions attached to this descriptor.
 *				  Currently, there are no segment descriptor
 *				  extensions defined so this field is always
 *				  'PS_FO_Nil'.
 *****/

struct PS_SD {
    PS_Type_FO			predecessor;
    PS_TID			tid;
    PS_RID			rid;
    unsigned int		segment;
    PS_Type_FO			extensions;
};

/*****  Access Macros  *****/
#define PS_SD_Predecessor(sd)		((sd).predecessor)
#define PS_SD_TID(sd)			((sd).tid)
#define PS_SD_RID(sd)			((sd).rid)
#define PS_SD_Segment(sd)		((sd).segment)
#define PS_SD_Extensions(sd)		((sd).extensions)


/**************************************
 *****  Space Version Descriptor  *****
 **************************************/

/*---------------------------------------------------------------------------
 * A 'Space Version Descriptor' defines a version of an object space.  SVDs
 * are the leaf nodes of the graph contained in the network directory file.
 *
 *  Space Version Descriptor Field Descriptions:
 *	spaceIndex		- the index of the space with which this SVD
 *				  is associated.
 *	minSegment		- the index of the oldest segment referenced
 *				  by the 'rootSegment' container table.
 *	maxSegment		- the index of the largest segment associated
 *				  with this space.  'maxSegment' is used to
 *				  determine the index of the next segment to
 *				  create when updating a space.  Normally,
 *				  'maxSegment' is equal to 'rootSegment';
 *				  however, it can be greater if a network
 *				  rollback has been performed.
 *	rootSegment		- the index of the segment containing the
 *				  container table for this version of the
 *				  space.  'rootSegment' must be a value in
 *				  closed interval '[minSegment, maxSegment]'.
 *	origRootSegment		- the index of the segment which contained the
 *				  container table when the space version was
 *				  created.  Used by the Space Compactor.
 *	ctOffset		- the network directory file offset of the 
 *				  container table in the root segment file.
 *	previousSVD		- the network directory file offset of the
 *				  SVD for the previous version of this space;
 *				  set to PS_FO_Nil if prev. vn. doesn't exist.
 *	spacePathName		- the network directory file offset of a
 *				  counted string naming the file system
 *				  directory containing this space.
 *	spaceBaseName		- the null terminated character string base
 *				  name (see basename(1)) of this space's
 *				  segment directory.  The persistent storage
 *				  manager expects to find a directory named
 *				  "spacePathName/spaceBaseName" containing a
 *				  collection of segment number named files.
 *	signature		- a signature word whose validity indicates
 *				  a version 5.6 or greater SVD layout.
 *	sd			- the offset of the start of the SD chain
 *				  associated with this SVD.
 *	mp			- the offset of the SVD from which this SVD was
 *				  derived. 'PS_FO_Nil' by default, set to the
 *				  offset of the source SVD by the multi-version
 *				  compactor.
 *	extensions		- the file offset of the list of extension
 *				  structures attached to this SVD.  Currently,
 *				  there are no SVD extension structures so this
 *				  slot is always set to 'PS_FO_Nil'.
 *****/

struct PS_SVD {
    int				spaceIndex,
				minSegment,
				maxSegment,
				rootSegment,
				origRootSegment;
    PS_Type_FO			ctOffset,
				previousSVD,
				spacePathName;
    char			spaceBaseName[16];
    unsigned int		signature;
    PS_Type_FO			sd,
				mp,
				extensions;
};

/*****  Access Macros  *****/
#define PS_SVD_SpaceIndex(svd)		((svd).spaceIndex)
#define PS_SVD_MinSegment(svd)		((svd).minSegment)
#define PS_SVD_MaxSegment(svd)		((svd).maxSegment)
#define PS_SVD_RootSegment(svd)		((svd).rootSegment)
#define PS_SVD_OrigRootSegment(svd)	((svd).origRootSegment)
#define PS_SVD_CTOffset(svd)		((svd).ctOffset)
#define PS_SVD_PreviousSVD(svd)		((svd).previousSVD)
#define PS_SVD_SpacePathName(svd)	((svd).spacePathName)
#define PS_SVD_SpaceBaseName(svd)	((svd).spaceBaseName)
#define PS_SVD_Signature(svd)		((svd).signature)
#define PS_SVD_SD(svd)			((svd).sd)
#define PS_SVD_MP(svd)			((svd).mp)
#define PS_SVD_Extensions(svd)		((svd).extensions)


/********************************
 ********************************
 *****  Segment Structures  *****
 ********************************
 ********************************/

/********************************
 *****  The Segment Header  *****
 ********************************/

/*---------------------------------------------------------------------------
 * A Segment Header is written at the head of each segment in an object
 * space.
 *
 * Fields:
 *	ctOffset	-  the offset in the file where the container table
 * 			   for this space resides.  It actually points to the 
 *			   container preamble for the container table.
 *	softwareVersion -  the version of the software that this segment was
 *			   saved with.
 *	ctSize		-  the size in bytes of the container table, including
 *		           the container preamble.
 *---------------------------------------------------------------------------
 */

/*****  Type Definition  *****/
struct PS_SH {
    int				m_ctOffset;   /*** MUST be first ***/
#if defined(_AIX)
    unsigned int
#else
    unsigned short
#endif
				m_reserved	: 8,
				m_dataFormat	: 8;
    unsigned short		m_softwareVersion;
    int				m_ctSize;
    PS_TID			m_tid;
    PS_RID			m_rid;
    int				m_checksum;
};

/*****  Access Macros  *****/
#define PS_SH_CTOffset(sh)		((sh)->m_ctOffset)
#define PS_SH_Reserved(sh)		((sh)->m_reserved)
#define PS_SH_DataFormat(sh)		((sh)->m_dataFormat)
#define PS_SH_SoftwareVersion(sh)	((sh)->m_softwareVersion)
#define PS_SH_CTSize(sh)		((sh)->m_ctSize)
#define PS_SH_TID(sh)			((sh)->m_tid)
#define PS_SH_RID(sh)			((sh)->m_rid)
#define PS_SH_Checksum(sh)		((sh)->m_checksum)

/*****  Container Access Macros  *****/
#define PS_SH_IsASmallContainerSegment(sh) (\
   PS_SH_SoftwareVersion (sh) < PS_4GBContainers ||\
   PS_SH_SoftwareVersion (sh) > PS_SoftwareVersion\
)
#define PS_SH_ContainerPreamble(sh,offset) (\
    (M_CPreamble*)(\
	(pointer_t)(sh) + (offset) - (\
	    PS_SH_IsASmallContainerSegment ((PS_SH*)(sh)) ? sizeof (unsigned int) : 0\
	)\
    )\
)

#define PS_SH_ContainerBase(sh, offset) M_CPreamble_ContainerBase (\
    PS_SH_ContainerPreamble ((sh),(offset))\
)

#define PS_SH_ContainerBaseAsType(sh,offset,type)\
M_CPreamble_ContainerBaseAsType (PS_SH_ContainerPreamble ((sh),(offset)),type)

/*****  Sizing Macro  *****/
#define PS_SH_Size sizeof (PS_SH)


/****************************************
 ****************************************
 *****  Container Table Structures  *****
 ****************************************
 ****************************************/

/**************************************
 *****  Free List Nil Definition  *****
 **************************************/

#define PS_CT_FreeListNil	-1


/***********************************
 *****  Container Table Entry  *****
 ***********************************/
/*---------------------------------------------------------------------------
 * Field Descriptions:
 *	m_bCopy			- a boolean that, when true, indicates that
 *				  this entry is an unmodified copy of a
 *				  container written by an earlier update.  Set
 *				  during compactions.
 *	m_bFree			- a boolean which, when true, indicates that
 *				  this entry is unused.
 *	m_xSegment		- the index of the segment in which the
 *				  container associated with this entry lives.
 *	m_xOffset		- the byte offset within the segment at which
 *				  the container associated with this entry
 *				  is found.  Valid only when 'm_bFree' is false.
 *	m_xNextFree		- the index of the next free entry in a free
 *				  list of container table entries.  Valid only
 *				  when 'm_bFree' is true.
 *---------------------------------------------------------------------------
 */

struct PS_CTE {
//  Access
public:
    void getSegmentAndOffset (unsigned int &rxSegment, unsigned int &rxOffset) const {
	rxSegment = m_xSegment;
	rxOffset = reference.m_xOffset;
    }
    void getSegmentAndOffset (int &rxSegment, int &rxOffset) const {
	rxSegment = m_xSegment;
	rxOffset = reference.m_xOffset;
    }

//  Query
public:
    bool isCopy () const {
	return m_bCopy;
    }
    bool isFree () const {
	return m_bFree;
    }
    bool isInUse () const {
	return !m_bFree;
    }

//  Update
public:
    void setSegmentAndOffset (unsigned int xSegment, unsigned int xOffset, bool bCopy = false) {
	m_bFree = false;
	m_bCopy = bCopy;
	m_xSegment = xSegment;
	reference.m_xOffset = xOffset;
    }

//  State
public:
    unsigned int		m_bCopy		:  1,
				m_bFree		:  1,
				m_xSegment	: 30;
    union reference_t {
	unsigned int		m_xOffset;
	int			m_xNextFree;
    } reference;
};

/*****  Access Macros  *****/
#define PS_CTE_IsCopy(cte)	((cte).m_bCopy)
#define PS_CTE_IsFree(cte)	((cte).m_bFree)
#define PS_CTE_InUse(cte)	(!(cte).m_bFree)
#define PS_CTE_Segment(cte)	((cte).m_xSegment)
#define PS_CTE_Offset(cte)	((cte).reference.m_xOffset)
#define PS_CTE_NextFree(cte)	((cte).reference.m_xNextFree)


/*****************************
 *****  Container Table  *****
 *****************************/
/*---------------------------------------------------------------------------
 * Field Descriptions:
 *	spaceIndex		- the index of the object space for which this
 *				  container table serves as guide.
 *	segmentIndex		- the segment for which it is valid.
 *	entryCount		- the total number of entries (used and
 *				  unused) in the table.
 *	freeList		- the index of the first free entry in the
 * 				  table.
 *	entries			- the container table entry array.  Every
 *				  container table contains at least two
 *				  entries - one for the container table
 *				  and one for the space root.
 *****/

struct PS_CT {
    int				spaceIndex,
				segmentIndex,  
				entryCount, 
				freeList;
    PS_CTE			entries [2];
};

/*****  Access Macros  *****/
#define PS_CT_SpaceIndex(ct)	((ct)->spaceIndex)
#define PS_CT_SegmentIndex(ct)	((ct)->segmentIndex)
#define PS_CT_EntryCount(ct)	((ct)->entryCount)
#define PS_CT_FreeList(ct)	((ct)->freeList)
#define PS_CT_Entries(ct)	((ct)->entries)

#define PS_CT_Entry(ct,index)	PS_CT_Entries (ct)[index]

/*****  Sizing Macro  *****/
#define PS_CT_ByteCount(cteCount) (\
    sizeof (PS_CT) + sizeof (PS_CTE) * ((cteCount) - 2)\
)


/***************************************
 *  Container Table CTE Access Macros  *
 ***************************************/

#define PS_CT_GetSegmentAndOffset(ct, index, segment, offset)\
    (PS_CT_Entry (ct, index).getSegmentAndOffset ((segment), (offset)))

#define PS_CT_SetSegmentAndOffset(ct, index, segment, offset)\
    (PS_CT_Entry (ct, index).setSegmentAndOffset ((segment), (offset)))


/********************************
 ********************************
 *****  Process Structures  *****
 ********************************
 ********************************/


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  ps.d 1 replace /users/mjc/system
  870215 22:56:28 lcn New memory manager modules

 ************************************************************************/
/************************************************************************
  ps.d 2 replace /users/mjc/system
  870326 19:28:07 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  ps.d 3 replace /users/cmm/system
  870519 16:11:54 cmm Release compactor

 ************************************************************************/
/************************************************************************
  ps.d 4 replace /users/mjc/translation
  870524 09:40:55 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  ps.d 5 replace /users/mjc/system
  870614 20:14:48 mjc Replaced UNIX specific include files with 'stdoss.h' equivalents

 ************************************************************************/
/************************************************************************
  ps.d 6 replace /users/cmm/system
  870821 11:02:42 cmm compactor: added omitlist, minseg formula

 ************************************************************************/
/************************************************************************
  ps.d 7 replace /users/cmm/system
  871006 14:43:25 cmm added stats-only option to compactor, initial MSS back to orig rt. seg of previous SVD, mjc's MSS override

 ************************************************************************/
/************************************************************************
  ps.d 8 replace /users/jad/system/LINK
  880314 14:05:03 jad Include more version info -> NDF Version 2

 ************************************************************************/
/************************************************************************
  ps.d 9 replace /users/jck/system
  880315 14:39:30 jck New Lock Manager interface

 ************************************************************************/
/************************************************************************
  ps.d 12 replace /users/jck/system
  880422 06:34:28 jck Added Changes due to the Apollo port to the pre-sequence-link version

 ************************************************************************/

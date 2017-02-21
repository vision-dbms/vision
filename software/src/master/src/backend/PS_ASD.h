#ifndef PS_ASD_Interface
#define PS_ASD_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkMemory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "PS_AND.h"

class M_ASD;

class VContainerHandle;

class XUIS;


/*************************
 *****  Definitions  *****
 *************************/

/****************************************
 *****  Segment Mapping Descriptor  *****
 ****************************************/

/*---------------------------------------------------------------------------
 * A Segment Mapping Descriptor identifies an object space segment.  A SMD
 * contains the mapping address of the first byte of the segment and a scan
 * generation.  The scan generation is used by the segment flushing algorithms
 * to identify the segment flushing scan which last required access to this
 * segment.  A 'Nil' segment address denotes an unmapped segment.  These facts
 * are encoded in a variety of access macros.
 *****/

class PS_SMD {
//  State
public:
    VkMemory			fileMapping;
    unsigned int		segmentUtilization,
				scanGeneration;
    PS_TID			expectedTID;
    PS_RID			expectedRID;
};

/*****  Access Macros  *****/
#define PS_SMD_FileMapping(smd)		((smd)->fileMapping)
#define PS_SMD_SegmentUtilization(smd)	((smd)->segmentUtilization)
#define PS_SMD_ScanGeneration(smd)	((smd)->scanGeneration)
#define PS_SMD_ExpectedTID(smd)		((smd)->expectedTID)
#define PS_SMD_ExpectedRID(smd)		((smd)->expectedRID)

#define PS_SMD_SegmentAddress(smd) (\
    (PS_SH*)PS_SMD_FileMapping (smd).RegionAddress ()\
)
#define PS_SMD_SegmentSize(smd) (\
    PS_SMD_FileMapping (smd).RegionSize ()\
)

#define PS_SMD_SegmentIsMapped(smd)	IsntNil (PS_SMD_SegmentAddress (smd))
#define PS_SMD_SegmentIsntMapped(smd)	IsNil (PS_SMD_SegmentAddress (smd))

#define PS_SMD_ContainerAddress(smd, offset) PS_SH_ContainerPreamble (\
    PS_SMD_SegmentAddress(smd), offset\
)

/*****  SMV Sizing Macro  *****/
#define PS_SMV_ByteCount(rootSegment, minSegment) (\
    sizeof (PS_SMD) * (1 + (rootSegment) - (minSegment))\
)


/********************************************
 *****  Segment Descriptor Constructor  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *  Segment Descriptor Constructors register the set of segments created or
 *  incorporated into a space by an update.  They are used by 'CommitUpdate'
 *  to generate the list of segment descriptors stored in the NDF.
 *---------------------------------------------------------------------------
 */

class PS_SDC {
//  State
public:
    PS_SDC*			successor;
    PS_TID			tid;
    PS_RID			rid;
    unsigned int		segment;
};

#define PS_SDC_Successor(sdc)	((sdc)->successor)
#define PS_SDC_TID(sdc)		((sdc)->tid)
#define PS_SDC_RID(sdc)		((sdc)->rid)
#define PS_SDC_Segment(sdc)	((sdc)->segment)


/***************************************
 *****  Accessed Space Descriptor  *****
 ***************************************/

/*---------------------------------------------------------------------------
 * An Acessed Space Descriptor records the state of an accessed space.
 *
 *  Accessed Space Descriptor Field Descriptions:
 *	m_pNextASD		- the address of the next ASD in the ASD chain
 *				  associated with this space's database.
 *	m_pLogicalASD		- the address of the logical ASD associated
 *				  with this space.
 *	m_pAND			- the address of the network with which this ASD
 *				  is associated.
 *	m_xSpace		- the index of this space.
 *	m_xBaseSegment		- the index of the smallest segment represented
 *				  in the segment mapping vector.
 *	m_bCTIsMapped		- a boolean which, when true, designates that
 *				  the address contained in the 'm_pCT' slot refers
 *				  to a mapped segment and, when false, denotes
 *				  that the address refers to a process private
 *				  copy of the container table.
 *	m_bDoingCompaction	- a system controlled boolean which, when set,
 *				  indicates that this space is being compacted.
 *	m_bInCompaction		- a user controlled boolean which, when set to
 *				  false, disables compaction of this space.
 *				  Defaults to true.
 *	m_bCompactingAll	- a system controlled boolean which, when set,
 *				  indicates that all containers in this space
 *				  should be processed.
 *	m_bCompactingSmall	- a user controlled boolean which, when set,
 *				  indicates that small containers in this space
 *				  should be copied forward. This provides the
 *				  ability to set a policy which improves locality
 *				  of reference.
 *	m_bMinSMDSet		- a user controlled boolean which, when set to
 *				  true, indicates that 'm_xMinSMD' is
 *				  explicitly set.
 *	m_xMinSMD		- the index of the smallest segment to retain
 *				  in the space after the next update.
 *	m_iMaxCompactionSegs	- the maximum number of segments that will be
 *				  created during a compaction.
 *	m_iMaxSegmentSize	- the approximate maximum size of any single
 *				  segment created during an update.
 *	m_iLargeContainerSize	- The threshold value which determines
 *				  what segment a container gets stored in.
 *	m_iCopyCoefficient	- the copy weight applied to this space by the
 *				  incremental compactor.  Corresponds to the
 *				  value of the 'NCmpTuningParY' environment
 *				  variable of the standalone compactor.
 *	m_iReclaimCoefficient	- the save weight applied to this space by the
 *				  incremental compactor.  Corresponds to the
 *				  value of the 'NCmpTuningParX' environment
 *				  variable of the standalone compactor.
 *	m_pSMV			- the address of the segment mapping vector for
 *				  this space.  The segment mapping vector is an
 *				  array of 'rootSegment(svd) - minSegment(svd)'
 *				  SMDs.
 *	m_pCT			- the address of the mapped container table for
 *				  this space.  This address will ALWAYS be
 *				  read only and is used exclusively to compute
 *				  the mapped address of containers.
 *	m_pUpdateCT		- the address of a container table being
 *				  constructed as part of a space update.
 *				  Only ASDs associated with modified spaces
 *				  will own an 'm_pUpdateCT' - all other spaces
 *				  will have a 'Nil' in this field.
 *	m_xUpdateFD		- a writeable file descriptor for the new
 *				  segment being created by an update.
 *	m_pUpdateSDC		- the address of the list of segment descriptor
 *				  constructors created by this update.
 *	m_xUpdateSegment	- the index of the new segment being created
 *				  by an update.
 *	m_xUpdateCT		- the offset in 'm_xUpdateSegment' of the new
 *				  container table created by an update.
 *****/
class PS_ASD : public VTransient {
//  Friends
    friend class PS_AND;
    friend class M_ASD;
    friend class VContainerHandle;

//  Construction
private:
    void GarnerNDFSegmentInfo ();
    void GarnerSDCSegmentInfo ();

public:
    PS_ASD (PS_AND *pAND, unsigned int xSpace, PS_CT *pCT = 0);

//  Destruction
public:
    ~PS_ASD () {
    }

//  Access
public:
    unsigned int baseSegment () const {
	return m_xBaseSegment;
    }

    unsigned int CTEntryCount () const {
	return PS_CT_EntryCount (m_pCT);
    }

    PS_AND *Database () const {
	return m_pAND;
    }

    bool GetCTE (unsigned int xContainer, PS_CTE &rResult) const;
    bool GetCTE (int xContainer, PS_CTE &rResult) const {
	return xContainer >= 0 && GetCTE ((unsigned int)xContainer, rResult);
    }

    bool GetLiveCTE (unsigned int xContainer, PS_CTE &rResult) const {
	return GetCTE (xContainer, rResult) && PS_CTE_InUse (rResult);
    }
    bool GetLiveCTE (int xContainer, PS_CTE &rResult) const {
	return GetCTE (xContainer, rResult) && PS_CTE_InUse (rResult);
    }

    PS_ASD *NextASD () const {
	return m_pNextASD;
    }
    M_ASD* LogicalASD () const {
	return m_pLogicalASD;
    }

    VkMemory::Share segmentMemoryShare () const {
	return m_pAND->segmentMemoryShare ();
    }

    unsigned int spaceIndex () const {
	return m_xSpace;
    }

    char* SpacePathName () const;
    char const *SegmentPathName (unsigned int xSegment) const;

    PS_SMD* SMD (unsigned int xSegment) const {
	return m_pSMV + (xSegment - m_xBaseSegment);
    }

    PS_SRD& SRD () const {
	return m_pAND->SRD (m_xSpace);
    }

    PS_SVD& SVD () const {
	return m_pAND->SVD (m_xSpace);
    }
    unsigned int MaxSegment () const {
	return PS_SVD_MaxSegment (SVD ());
    }
    unsigned int MinSegment () const {
	return PS_SVD_MinSegment (SVD ());
    }
    unsigned int RootSegment () const {
	return PS_SVD_RootSegment (SVD ());
    }
    unsigned int OrigRootSegment () const {
	return PS_SVD_OrigRootSegment (SVD ());
    }
    unsigned int CTOffset () const {
	return PS_SVD_CTOffset (SVD ());
    }

    unsigned int MinSavedSegment () const {
	return m_xBaseSegment + m_xMinSMD;
    }
    unsigned int MinSMDIndex () const {
	return m_xMinSMD;
    }

    bool UpdateIsntWell () const {
	return m_pAND->UpdateIsntWell ();
    }
    bool UpdateIsWell () const {
	return m_pAND->UpdateIsWell ();
    }

//  Query
public:
    bool AccessedSpaceVersionIsNewest () const;

    bool ContainerHasBeenRetained (unsigned int xContainer) const;

    bool CTIsMapped () const {
	return m_bCTIsMapped;
    }

    bool DoingCompaction () const {
	return m_bDoingCompaction;
    }

    bool CompactingAll () const {
	return m_bCompactingAll;
    }

    bool CompactingSmall () const {
	return m_bCompactingSmall;
    }

    unsigned long MappedSizeOfSpace (unsigned int *segCount) const;

    unsigned int TotalSegments () const {
	return 1 + RootSegment () - m_xBaseSegment;
    }

    bool TracingCompaction () const {
	return m_pAND->TracingCompaction ();
    }
    bool TracingUpdate () const {
	return m_pAND->TracingUpdate ();
    }

//  Settings Control
public:
    void SetCopyCoefficientTo (double iValue) {
	m_iCopyCoefficient = iValue;
    }
    void SetCopyCoefficientTo (int iValue) {
	m_iCopyCoefficient = (double)iValue;
    }

    void SetInCompactionTo (bool iValue) {
	m_bInCompaction = iValue ? true : false;
    }

    void SetCompactingSmallTo (bool iValue) {
	m_bCompactingSmall = iValue ? true : false;
    }

    void SetLargeContainerSizeTo (double iValue);
    void SetLargeContainerSizeTo (int iValue);

    void SetMaxCompactionSegmentsTo (double iValue);
    void SetMaxCompactionSegmentsTo (int iValue);

    void SetMaxSegmentSizeTo (double iValue);
    void SetMaxSegmentSizeTo (int iValue);

    void SetMSSOverrideTo (double iValue);
    void SetMSSOverrideTo (int iValue);

    void SetReclaimCoefficientTo (double iValue) {
	m_iReclaimCoefficient = iValue;
    }
    void SetReclaimCoefficientTo (int iValue) {
	m_iReclaimCoefficient = (double)iValue;
    }

//  Segment Management
private:
    PS_SH* MapSegment (unsigned int xSegment);
    PS_SH* MapSegment (char const *pSegmentPathName, PS_SMD *pSMD) const;

//  Container Management
public:
    M_CPreamble *AccessContainer (unsigned int xContainer, bool hasBeenAccessed);

    void RetainContainer (unsigned int xContainer) const;

    void ReleaseContainer (unsigned int xContainer, unsigned int sContainer);

//  Space Update
private:
    bool AcquireSpaceLock (VdbNetworkLockOperation fWait) {
	return m_pAND->LockOp (m_xSpace, fWait);
    }

    void AwaitSpaceLockRelease ();

    bool LockSpace ();

    bool ValidateSpaceConsistency () const;

    void DetermineCompactionPlan ();

    bool PersistReferences ();
    bool PersistStructures ();

    bool BeginSpaceUpdate ();

    void OpenUpdateSegment  ();
    void CloseUpdateSegment (PS_Type_FO ctOffset, unsigned int ctSize);

    void ValidateChecksum (char const *pSegmentPathName) const;
    void ValidateChecksum () const {
	ValidateChecksum (SegmentPathName (m_xUpdateSegment));
    }

    void EndSpaceUpdate ();

    void LinkExternalSegmentsToSpace	(XUIS& rXUIS);
    void LinkExternalSegmentToSpace	(char const* pathName);

private:
    void SynchronizeAndCloseSegment();

public:
    void UpdateSpace () {
	m_pAND->UpdateSpace (this);
    }

//  Incorporator Support
public:
    void WriteDBUpdateInfo (unsigned int xContainer, bool replace);

//  Diagnostics
private:
    void dumpCTSegment ();

//  State
protected:
    PS_AND *const		m_pAND;
    PS_ASD *const		m_pNextASD;
    unsigned int const		m_xSpace;
    M_ASD*			m_pLogicalASD;
    unsigned int		m_xBaseSegment,
				m_bCTIsMapped		:  1,
				m_bDoingCompaction	:  1,
				m_bInCompaction		:  1,
				m_bCompactingAll	:  1,
				m_bCompactingSmall	:  1,
				m_bMinSMDSet		:  1,
				m_xMinSMD		: 26,
				m_iMaxCompactionSegs,
				m_iMaxSegmentSize,
				m_iLargeContainerSize;
    double			m_iCopyCoefficient,
				m_iReclaimCoefficient;
    PS_SMD*			m_pSMV;
    PS_CT			*m_pCT,
				*m_pUpdateCT;
    int				m_xUpdateFD;
    PS_SDC*			m_pUpdateSDC;
    int				m_xUpdateSegment,
				m_xUpdateCT;
    unsigned int		m_iUpdateChecksum;
};


/*****  Access Macros *****/
#define PS_ASD_CTIsMapped(asd)		((asd)->m_bCTIsMapped)
#define PS_ASD_MaxSegmentSize(asd)	((asd)->m_iMaxSegmentSize)
#define PS_ASD_SMV(asd)			((asd)->m_pSMV)
#define PS_ASD_CT(asd)			((asd)->m_pCT)
#define PS_ASD_UpdateCT(asd)		((asd)->m_pUpdateCT)
#define PS_ASD_Updated(asd)		IsntNil (PS_ASD_UpdateCT (asd))
#define PS_ASD_UpdateFD(asd)		((asd)->m_xUpdateFD)
#define PS_ASD_UpdateSDC(asd)		((asd)->m_pUpdateSDC)
#define PS_ASD_UpdateSegment(asd)	((asd)->m_xUpdateSegment)
#define PS_ASD_UpdateCTOffset(asd)	((asd)->m_xUpdateCT)
#define PS_ASD_UpdateChecksum(asd)	((asd)->m_iUpdateChecksum)

/*****  Network Component Access Macros  *****/
#define PS_ASD_RSequence(asd)		PS_AND_RSequence ((asd)->Database ())

/*****  SRD Access Macros  *****/
#define PS_ASD_SRD(asd)			((asd)->SRD ())

#define PS_ASD_Role(asd)		PS_SRD_Role	(PS_ASD_SRD (asd))
#define PS_ASD_SVDFO(asd)		PS_SRD_SVD	(PS_ASD_SRD (asd))

/*****  SVD Access Macros  *****/
#define PS_ASD_SVD(asd)			((asd)->SVD ())

#define PS_ASD_MinSegment(asd)		PS_SVD_MinSegment     (PS_ASD_SVD(asd))
#define PS_ASD_MaxSegment(asd)		PS_SVD_MaxSegment     (PS_ASD_SVD(asd))
#define PS_ASD_RootSegment(asd)		PS_SVD_RootSegment    (PS_ASD_SVD(asd))
#define PS_ASD_OrigRootSegment(asd)	PS_SVD_OrigRootSegment(PS_ASD_SVD(asd))
#define PS_ASD_CTOffset(asd)		PS_SVD_CTOffset       (PS_ASD_SVD(asd))

/*****  CT Component Access Macros  *****/
#define PS_ASD_CTEntry(asd,xContainer) PS_CT_Entry (\
    PS_ASD_CT (asd), xContainer\
)


#endif

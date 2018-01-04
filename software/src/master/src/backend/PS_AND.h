#ifndef PS_AND_Interface
#define PS_AND_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkLicense.h"
#include "VkMemory.h"
#include "VdbNetwork.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VArgList.h"

#include "VDatabaseNDF.h"

class M_AND;

class PS_ASD;


/*************************
 *****  Definitions  *****
 *************************/

/****************************************
 *****  Network Update Status Codes *****
 ****************************************/

/*---------------------------------------------------------------------------
 *  Network Update Status Code Descriptions:
 *	Successful		- the update succeeded.
 *	Incomplete		- the update did not complete.  This is an
 *				  internal code which should not be seen after
 *				  an update routine returns.
 *	Inconsistent		- the update was not serializable.
 *	NotPermitted		- the update could not create or modify a
 *				  required structure because of a lack of
 *				  permission.
 *	NotSeparable		- a space (i.e., local) update would produce
 *				  a dangling reference if allowed to commit.
 *	NoLicense		- the update license isn't valid.
 *	Disabled		- update capability has been disabled for session
 *
 *****/

enum PS_UpdateStatus {
    PS_UpdateStatus_Successful,		/*  Must Be First   */
    PS_UpdateStatus_Incomplete,		/*  Must Be Second  */
    PS_UpdateStatus_Inconsistent,
    PS_UpdateStatus_NotPermitted,
    PS_UpdateStatus_NotSeparable,
    PS_UpdateStatus_NoLicense,
    PS_UpdateStatus_Disabled
};

/*****  Query Macros  *****/
#define PS_UpdateStatus_WasSuccessful(status)\
    (PS_UpdateStatus_Successful == (status))

#define PS_UpdateStatus_WasntSuccessful(status)\
    (PS_UpdateStatus_Successful != (status))

#define PS_UpdateStatus_WasntComplete(status)\
    (PS_UpdateStatus_Incomplete == (status))

#define PS_UpdateStatus_WasntConsistent(status)\
    (PS_UpdateStatus_Inconsistent == (status))

#define PS_UpdateStatus_WasntPermitted(status)\
    (PS_UpdateStatus_NotPermitted == (status))

#define PS_UpdateStatus_WasntSeparable(status)\
    (PS_UpdateStatus_NotSeparable == (status))

#define PS_UpdateStatus_UpdateIsWell(status)\
    ((int)(status) <= (int)PS_UpdateStatus_Incomplete)

#define PS_UpdateStatus_UpdateIsntWell(status)\
    ((int)(status) > (int)PS_UpdateStatus_Incomplete)


/*****************************************
 *****  Accessed Network Descriptor  *****
 *****************************************/

/*---------------------------------------------------------------------------
 * An 'Accessed Network Descriptor' holds the state needed to describe a
 * process' access to a network.
 *
 *  Accessed Network Descriptor Field Descriptions:
 *	m_iNetworkHandle	- the instance of the network handle used by
 *				  this network.
 *	m_pASDList		- the address of the first ASD in a chain
 *				  of ASDs associated with this network.
 *	m_iSpaceCount		- the number of spaces in the accessed network
 *				  version.
 *	m_pSRDArray		- the SRD array of the accessed NVD.
 *	m_pSVDArray		- the SVD array of the accessed NVD.
 *	m_iAccessFO		- the network directory file offset of the
 *				  Network Version Descriptor accessed by this
 *				  process.
 *	m_iUpdateFO		- the network directory file offset of the
 *				  Network Version Descriptor defining the
 *				  update thread associated with this process.
 *				  'm_iUpdateFO' points to the NVD created
 *				  by the most recent update performed by this
 *				  process.  If this process has not performed
 *				  an update, 'm_iUpdateFO' will be identical
 *				  to 'm_iAccessFO'.
 *	m_iUpdateNVD		- the NVD associated with the most recent
 *				  update performed by this process.  If this
 *				  process has not performed an update, this
 *				  will be the NVD accessed by this process.
 *	m_xUpdateStatus		- the completion status of the most recent
 *				  update of this network attempted by this
 *				  process.
 *	m_pUngrantedASD		- the address of an ASD for a space which this
 *				  process is waiting to lock.  A 'Nil' value
 *				  indicates that no space locks are currently
 *				  ungranted.
 *	lockSet	    		- a set of locks for the network based on a
 *				  writeable UNIX file descriptor accessing
 *				  the network directory file.
 *	m_xScanGeneration	- the index of the last/current segment scan
 *				  performed on this network.  Segment scans
 *				  are used to reclaim and unmap segments not
 *				  required to support accessed containers.  A
 *				  segment can be reclaimed by a scan if no
 *				  CPCCs are detected which address a container
 *				  in the segment.
 *	m_bDoingCompaction	- a boolean which, when true, causes updates
 *				  do perform a compaction analysis and copy
 *				  for non-excluded, modified object spaces.
 *	m_bMakingBaseVersion	- a boolean which, when true, causes updates
 *				  to generate a base version.  Generating a
 *				  base version causes the base version mark
 *				  to be set in the NVD and MSS files to be
 *				  written for all spaces.
 *****/
class PS_AND {
    DECLARE_FAMILY_MEMBERS (PS_AND, void);

//  Friends
    friend class PS_ASD;
    friend class M_AND;

//  Aliases
public:
    typedef V::VArgList VArgList;

//  Construction
public:
    static PS_AND *AccessNetwork (
	VDatabaseNDF *pDatabaseNDF, char const *versionSpecification, bool makingNewNdf
    );

protected:
    PS_AND (
	VDatabaseNDF	*pDatabaseNDF,
	char		*osdPathName,
	bool		 makingNewNdf,
	int		 ndfd,
	PS_NDH		&ndh,
	PS_Type_FO	 nvdId,
	PS_NVD		&nvd,
	bool		 restart
    );

//  Destruction
public:
    ~PS_AND () {
    }

//  Access
public:
    VDatabaseNDF *databaseNDF () const {
	return m_pDatabaseNDF;
    }
    VDatabase *database () const {
	return m_pDatabaseNDF->database ();
    }

    PS_Type_FO AccessFO () const {
	return m_iAccessFO;
    }

    int DirectoryFD () const {
	return m_iNetworkHandle.NDFHandle ();
    }
    int JournalFD () const {
	return m_iNetworkHandle.NJFHandle ();
    }

    VkLicense const &License () const {
	return m_iLicense;
    }
    VkLicense &License () {
	return m_iLicense;
    }

    char const* NDFPathName () const {
	return m_iNetworkHandle.NDFPathName ();
    }
    char const* NJFPathName () const {
	return m_iNetworkHandle.NJFPathName ();
    }
    char const* NLFPathName () const {
	return m_iNetworkHandle.NLFPathName ();
    }
    char const* OSDPathName () const {
	return m_iNetworkHandle.OSDPathName ();
    }

    unsigned int ScanGeneration () const {
	return m_xScanGeneration;
    }

    VkMemory::Share segmentMemoryShare () const {
	return m_xSegmentMemoryShare;
    }

    unsigned int SpaceCount () const {
	return m_iSpaceCount;
    }

    PS_SRD& SRD (unsigned int xSpace) const {
	return m_pSRDArray[xSpace];
    }
    PS_SVD& SVD (unsigned int xSpace) const {
	return m_pSVDArray[xSpace];
    }

    char *SpacePathName (PS_SVD const &rSVD) const;

    PS_TID const& TID () const {
	return m_iTID;
    }

    PS_Type_FO UpdateFO () const {
	return m_iUpdateFO;
    }

    PS_NVD const& UpdateNVD () const {
	return m_iUpdateNVD;
    }

    PS_UpdateStatus UpdateStatus () const {
	return m_xUpdateStatus;
    }

//  Query
public:
    bool CurrentUserIsInRightsList ();

    bool DoingCompaction () const {
	return m_bDoingCompaction;
    }

    bool MakingBaseVersion () const {
	return m_bMakingBaseVersion;
    }

    bool TracingCompaction () const {
	return m_bTracingCompaction;
    }
    bool TracingUpdate () const {
	return m_bTracingUpdate;
    }

    bool UngrantedLocksExist () const {
	return IsntNil (m_pUngrantedASD);
    }

    bool UpdateWasSuccessful () const {
	return PS_UpdateStatus_Successful == m_xUpdateStatus;
    }

    bool UpdateWasntSuccessful () const {
	return PS_UpdateStatus_WasntSuccessful (m_xUpdateStatus);
    }

    bool UpdateWasntComplete () const {
	return PS_UpdateStatus_WasntComplete (m_xUpdateStatus);
    }

    bool UpdateWasntConsistent () const {
	return PS_UpdateStatus_WasntConsistent (m_xUpdateStatus);
    }

    bool UpdateWasntPermitted () const {
	return PS_UpdateStatus_WasntPermitted (m_xUpdateStatus);
    }

    bool UpdateIsWell () const {
	return PS_UpdateStatus_UpdateIsWell (m_xUpdateStatus);
    }

    bool UpdateIsntWell () const {
	return PS_UpdateStatus_UpdateIsntWell (m_xUpdateStatus);
    }

    bool ValidatingChecksums () const {
	return m_bValidatingChecksums;
    }

//  Settings Control
public:
    void SetDoingCompactionTo (bool iValue) {
	m_bDoingCompaction = iValue ? true : false;
    }
    void SetMakingBaseVersionTo (bool iValue) {
	m_bMakingBaseVersion = iValue ? true : false;
    }

    void SetTracingCompactionTo (bool iValue) {
	m_bTracingCompaction = iValue ? true : false;
    }
    void SetTracingUpdateTo (bool iValue) {
	m_bTracingUpdate = iValue ? true : false;
    }

    void SetUpdateAnnotationTo (char const* pValue);

    void SetValidatingChecksumsTo (bool iValue) {
	m_bValidatingChecksums = iValue ? true : false;
    }

//  Resource Management
private:
    unsigned int ReclaimSegments    ();
    unsigned int ReclaimAllSegments ();

//  Error Logging
public:
    void VLogError (char const* format, VArgList const &rArgList);
    void __cdecl LogError (char const* format, ...);

//  Space Access
public:
    bool AccessSpace (PS_ASD *&rpResult, unsigned int spaceIndex);

//  Database Update
protected:
    void CreateNewNDF (PS_NDH ndh);

    void SetDirectoryFDTo (int xFD) {
	m_iNetworkHandle.SetNDFHandleTo (xFD);
    }
    void ResetDirectoryFD () {
	m_iNetworkHandle.SetNDFHandleTo (-1);
    }

    void SetJournalFDTo (int xFD) {
	m_iNetworkHandle.SetNJFHandleTo (xFD);
    }
    void ResetJournalFD () {
	m_iNetworkHandle.SetNJFHandleTo (-1);
    }

    bool LockOp (int lockNumber, VdbNetworkLockOperation lockOp);

    void LockNetwork ();
    void LockSpaces (M_AND *pLogicalAND);

    void UnlockNetwork (bool reopen = false);

    void UnlockSpaces ();

    void ResetLockManager ();
    void WaitForUngrantedLocks ();

    void ValidateNetworkConsistency (M_AND *pLogicalAND);

    void AcquireUpdateResources	    ();

    void ResetUpdateStatus () {
	m_xUpdateStatus = PS_UpdateStatus_Incomplete;
    }

    void ReleaseUpdateResources (bool recovering);

    void WriteJournalEntry (char const* message);
    void WriteMinimumSegmentJournalEntry (
	unsigned int space, unsigned int segment
    );
    void WriteSegmentJournalEntry (
	unsigned int space, unsigned int segment, PS_SH* sh
    );
    void WriteCommitJournalEntry (PS_Type_FO nvdfo);

    void PersistReferences (M_AND *pLogicalAND);
    void PersistStructures (M_AND *pLogicalAND);

    void SaveMinSegIndexInFile (PS_SVD const& rSVD);

    void CommitUpdate ();

    void UpdateSpace (PS_ASD *pASD);

public:
    void SetUpdateStatusTo (PS_UpdateStatus xUpdateStatus) {
	m_xUpdateStatus = xUpdateStatus;
    }

    void UpdateNetwork (M_AND *pLogicalAND);

//  Space Creation
protected:
    PS_ASD *NewSpaceASD (M_AND *pLogicalAND);

public:
    PS_ASD *CreateSpace (M_AND *pLogicalAND);

//  Bulk Update
public:
    PS_ASD *IncorporateExternalUpdate (char const* xuSpecPathName);

//  State
protected:
    VReference<VDatabaseNDF>	m_pDatabaseNDF;
    VdbNetwork			m_iNetworkHandle;
    PS_ASD*			m_pASDList;
    unsigned int		m_iGeneration;
    unsigned int		m_iSpaceCount;
    PS_SRD *			m_pSRDArray;
    PS_SVD *			m_pSVDArray;
    PS_Type_FO			m_iAccessFO;
    PS_Type_FO			m_iUpdateFO;
    PS_NVD			m_iUpdateNVD;
    PS_UpdateStatus		m_xUpdateStatus;
    char const*			m_pUpdateAnnotation;
    PS_ASD*			m_pUngrantedASD;
    unsigned int		m_xScanGeneration;
    unsigned int		m_bDoingCompaction	:  1,
				m_bTracingCompaction	:  1,
				m_bTracingUpdate	:  1,
				m_bMakingBaseVersion	:  1,
				m_bValidatingChecksums	:  1;
    unsigned short		m_xNextRIDSequence;
    PS_TID			m_iTID;
    VkLicense			m_iLicense;
    VkMemory::Share		m_xSegmentMemoryShare;
};


/*****  Access Macros  *****/
#define PS_AND_RSequence(pAND)		((pAND)->m_xNextRIDSequence)

/*****  SRD Access Macros  *****/
#define PS_AND_Role(pAND,si)		PS_SRD_Role (pAND->SRD (si))
#define PS_AND_SVDFO(pAND,si)		PS_SRD_SVD  (pAND->SRD (si))


#endif

/*****  NDF Editing and Reporting Tool  *****/

/***************************************
 ***************************************
 *****  Interfaces and Signatures  *****
 ***************************************
 ***************************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/************************
 *****  Supporting  *****
 ************************/

#include "gopt.h"

#include "m.d"
#include "ps.d"

#include "VdbNetwork.h"

#include "VkStatus.h"

#include "VTransient.h"
#include "VTransientServices.h"

#include "V_VTime.h"


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

/*****************************
 *****  Error Reporting  *****
 *****************************/

PrivateFnDef void SignalUnixError (char const *message) {
    fflush  (stdout);
    fprintf (stderr, ">>> ");
    perror  (message);
    exit (ErrorExitValue);
}

PrivateFnDef void SignalApplicationError (char const *message) {
    fflush  (stdout);
    fprintf (stderr, ">>> %s\n", message);
    exit (ErrorExitValue);
}

PrivateFnDef void DisplayApplicationError (char const *message) {
    fflush  (stdout);
    fprintf (stderr, "+++ %s\n", message);
    fflush  (stderr);
}

PrivateFnDef char const *MakeMessage (char const *fmt, ...) {
    static char buffer[1024];
    va_list ap;

    va_start (ap, fmt);
    vsprintf (buffer, fmt, ap);
    va_end   (ap);

    return buffer;
}


/*********************
 *****  NDF I/O  *****
 *********************/

PrivateVarDef int			g_hNDF = -1;
PrivateVarDef VdbNetwork*		g_pNDF;
PrivateVarDef bool			NDFFDLockable = false;
PrivateVarDef char const *		NDFPathName;
PrivateVarDef char			OSDPathName[1024];
PrivateVarDef PS_NDH			NDH;

#define NDFFD g_hNDF

PrivateFnDef int Canonical (PS_Type_FO xFO) {
    return PS_FO_IsntNil (xFO) ? (int)xFO : -1;
}

PrivateFnDef PS_Type_FO SeekNDFLocation (PS_Type_FO offset) {
    off_t loc;

    if (PS_FO_IsNil (offset)) {
	if ((loc = lseek (g_hNDF, 0, SEEK_END)) < 0) SignalUnixError (
	    MakeMessage ("NDF '%s' Seek To End Error", NDFPathName)
	);
    }
    else if ((loc = lseek (g_hNDF, offset, SEEK_SET)) < 0) SignalUnixError (
	MakeMessage ("NDF '%s' Seek To '%d' Error", NDFPathName, offset)
    );

    return (PS_Type_FO)loc;
}

PrivateFnDef void ReadNDFStructure (
    void *buffer, unsigned int bytes, bool sizeMustBeChecked
) {
    int bytesRead = read (g_hNDF, buffer, bytes);

    if (-1 == bytesRead) SignalUnixError (
	MakeMessage ("NDF '%s' Read Error", NDFPathName)
    );
    else if (sizeMustBeChecked && (unsigned int)bytesRead != bytes) SignalApplicationError (
	MakeMessage (
	    "NDF '%s' Structure Size Mismatch: Requested %d Bytes, Read %d Bytes",
	    NDFPathName, bytes, bytesRead
	)
    );
}

PrivateFnDef unsigned int ReadNDFArray (void *buffer, unsigned int elementSize) {
    unsigned int elementCount;

    ReadNDFStructure (&elementCount, sizeof elementCount, true);
    ReadNDFStructure (buffer, elementCount * elementSize, true);

    return elementCount;
}

PrivateFnDef unsigned int ReadNDFArraySize () {
    unsigned int arraySize;

    ReadNDFStructure (&arraySize, sizeof arraySize, true);

    return arraySize;
}

PrivateFnDef char const *ReadNDFString (PS_Type_FO fo) {
    SeekNDFLocation (fo);
    char *result = (char *)VTransient::transientServicesProvider()->allocate (ReadNDFArraySize ());
    SeekNDFLocation (fo);
    ReadNDFArray (result, sizeof (char));

    return result;
}


PrivateFnDef void ReadNDH (PS_NDH *ndh) {
    PS_NDH_Signature2 (*ndh) = 0;

    ReadNDFStructure (ndh, sizeof (PS_NDH), true);

    if (PS_NetworkDirectorySignature == PS_NDH_Signature2 (*ndh))
	return;

    PS_NDH_Signature2 (*ndh) = PS_NetworkDirectorySignature;
    PS_NDH_LayoutType (*ndh) = 1;
    PS_NDH_Generation (*ndh) = 0;

    memset (
	(char *)ndh + sizeof (ndh->components),
	0,
	sizeof (PS_NDH) - sizeof (ndh)->components
    );
}

PrivateFnDef void ReadNVD (PS_Type_FO fo, PS_NVD *pNVD) {
    PS_NVD_Signature (*pNVD) = 0;

    SeekNDFLocation	(fo);
    ReadNDFStructure	(pNVD, sizeof (PS_NVD), false);

    if (PS_NetworkDirectorySignature == PS_NVD_Signature (*pNVD))
	return;

    PS_NVD_Signature		    (*pNVD) = PS_NetworkDirectorySignature;
    PS_NVD_BaseVersion		    (*pNVD) = false;
    PS_NVD_UnusedFlags		    (*pNVD) = 0;
    PS_TID_High		(PS_NVD_TID (*pNVD))=
    PS_TID_Low		(PS_NVD_TID (*pNVD))= 0;
    PS_TID_Sequence	(PS_NVD_TID (*pNVD))= fo;
    PS_NVD_TD			    (*pNVD) =
    PS_NVD_MP			    (*pNVD) =
    PS_NVD_Annotation		    (*pNVD) =
    PS_NVD_Extensions		    (*pNVD) = PS_FO_Nil;
}


PrivateFnDef void Read6ByteSRD (PS_SRD *pSRD) {
    struct srd6_t {
        unsigned int		m_iRole : 2;
	unsigned short		data[3];
    } altSRD;

    ReadNDFStructure (&altSRD, 6, true);

    memcpy (pSRD, altSRD.data, sizeof (int));
    PS_SRD_Role (*pSRD) = altSRD.m_iRole;
}

PrivateFnDef void Read8ByteSRD (PS_SRD *pSRD) {
    struct srd8_t {
        unsigned int		m_iRole : 32 - PS_FO_FieldWidth,
					: PS_FO_FieldWidth;
	PS_Type_FO		m_iSVD;
    } altSRD;

    ReadNDFStructure (&altSRD, 8, true);

    PS_SRD_Role (*pSRD) = altSRD.m_iRole;
    PS_SRD_SVD	(*pSRD) = altSRD.m_iSVD;
}


PrivateFnDef unsigned int ReadSRV (
    int softwareVersion, PS_Type_FO fo, unsigned int *srvGrain, PS_SRD *pSRV
) {
    SeekNDFLocation (fo);
    if (softwareVersion >= 2) {
	*srvGrain = 4;
	return ReadNDFArray (pSRV, sizeof (PS_SRD));
    }

    unsigned int srdCount = ReadNDFArraySize ();
    unsigned int srd0[2];
    ReadNDFStructure (srd0, sizeof (srd0), true);

    SeekNDFLocation (fo + sizeof (int));
    if (PS_FO_IsNil (srd0[1])) {
        *srvGrain = 8;
	for (unsigned int srdIndex = 0; srdIndex < srdCount; srdIndex++)
	    Read8ByteSRD (&pSRV[srdIndex]);
    }
    else {
        *srvGrain = 6;
	for (unsigned int srdIndex = 0; srdIndex < srdCount; srdIndex++)
	    Read6ByteSRD (&pSRV[srdIndex]);
    }

    return srdCount;
}


PrivateFnDef void ReadSVD (PS_Type_FO fo, PS_SVD *pSVD) {
    PS_SVD_Signature (*pSVD) = 0;

    SeekNDFLocation	(fo);
    ReadNDFStructure	(pSVD, sizeof (PS_SVD), false);

    if (PS_NetworkDirectorySignature == PS_SVD_Signature (*pSVD))
	return;

    PS_SVD_Signature	(*pSVD) = PS_NetworkDirectorySignature;
    PS_SVD_SD		(*pSVD) =
    PS_SVD_MP		(*pSVD) =
    PS_SVD_Extensions	(*pSVD) = PS_FO_Nil;
}


PrivateFnDef void WriteNDFStructure (void *buffer, unsigned int bytes) {
    int bytesWritten = write(g_hNDF, buffer, bytes);

    if (-1 == bytesWritten) SignalUnixError (
	MakeMessage ("NDF '%s' Write Error", NDFPathName)
    );
    else if (bytesWritten != bytes) SignalApplicationError (
	MakeMessage (
	    "NDF '%s' Write Truncated: Requested = %d, Actual = %d",
	    NDFPathName, bytes, bytesWritten
	)
    );
}

PrivateFnDef void WriteNDFArray (void *buffer, unsigned int elementSize, unsigned int elementCount) {
    WriteNDFStructure (&elementCount, sizeof elementCount);
    WriteNDFStructure (buffer, elementCount * elementSize);
}


PrivateFnDef void Write6ByteSRD (PS_SRD *pSRD) {
    struct srd6_t {
        unsigned int		m_iRole : 2;
	unsigned short		data[3];
    } altSRD;

    memcpy (altSRD.data, pSRD, sizeof (int));
    altSRD.m_iRole = PS_SRD_Role (*pSRD);

    WriteNDFStructure (&altSRD, 6);
}

PrivateFnDef void Write8ByteSRD (PS_SRD *pSRD) {
    struct srd8_t {
        unsigned int		m_iRole : 32 - PS_FO_FieldWidth,
					: PS_FO_FieldWidth;
	PS_Type_FO		m_iSVD;
    } altSRD;

    altSRD.m_iRole = PS_SRD_Role (*pSRD);
    altSRD.m_iSVD = PS_SRD_SVD  (*pSRD);

    WriteNDFStructure (&altSRD, 8);
}


PrivateFnDef void WriteSRV (unsigned int srdCount, unsigned int srvGrain, PS_SRD *pSRV) {
    unsigned int srdIndex;

    switch (srvGrain) {
    case 4:
	WriteNDFArray (pSRV, sizeof (PS_SRD), srdCount);
        break;
    case 6:
	WriteNDFStructure (&srdCount, sizeof (int));
	for (srdIndex = 0; srdIndex < srdCount; srdIndex++)
	    Write6ByteSRD (&pSRV[srdIndex]);
        break;
    case 8:
	WriteNDFStructure (&srdCount, sizeof (int));
	for (srdIndex = 0; srdIndex < srdCount; srdIndex++)
	    Write8ByteSRD (&pSRV[srdIndex]);
        break;
    default:
	SignalApplicationError (
	    MakeMessage ("Corrupted SRV Grain %d Found, Must be 4, 6, or 8", srvGrain)
	);
        break;
    }
}


PrivateFnDef void SynchronizeNDF (int retryCount) {
    while (fsync (g_hNDF) < 0) {
	if (retryCount-- <= 0) SignalUnixError (
	    MakeMessage ("NDF '%s' Synchronization Error", NDFPathName)
	);
	sleep (5);
    }
}


/*******************************
 *****  Low Level Locking  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to perform a lock operation.
 *
 *  Arguments:
 *	xLock			- the lock to select.  A negative 'xLock'
 *				  selects all possible locks.
 *	xOperation		- the lock operation to be performed.
 *
 *  Returns:
 *	true if the operation completed successfully, false if a 'TLock' was
 *	requested which did not procur the requested lock.
 *
 *****/
PrivateFnDef bool LockOp (int xLock, VdbNetworkLockOperation xOperation) {
    VkStatus iStatus;
    g_pNDF->Lock (xLock, xOperation, &iStatus);

    switch (iStatus.Type ()) {
    case VkStatusType_Completed:
	return true;
    case VkStatusType_Blocked:
	return false;
    }

    SignalApplicationError (
	MakeMessage (
	    "Lock Select [%d:%d]: %s", xLock, xOperation, iStatus.CodeDescription ()
	)
    );

    return false;
}


/********************************
 *****  High Level Locking  *****
 ********************************/

#define WaitForLock		VdbNetworkLockOperation_Lock
#define DontWaitForLock		VdbNetworkLockOperation_TLock

#define UnlockSpaces()		LockOp (-1, VdbNetworkLockOperation_Unlock)
#define LockSpace(xSpace,fWait)	LockOp (xSpace,fWait)

PrivateFnDef void LockSpaces (int numSpacesToLock) {
    int currSpace;

    for (currSpace = 0;  currSpace < numSpacesToLock;  currSpace++) {
	if (!LockSpace (currSpace, DontWaitForLock)) {
	    UnlockSpaces ();
	    LockSpace (currSpace, WaitForLock);
	    UnlockSpaces ();
	    currSpace = -1;
	}
    }
}


PrivateFnDef void CloseNDF () {
    if (NDFFDLockable)
	UnlockSpaces ();

    if (close (NDFFD) < 0) SignalUnixError (
	MakeMessage ("NDF '%s' Close Error", NDFPathName)
    );

    NDFFDLockable = false;
    NDFFD = -1;
}

PrivateFnDef void OpenNDF (char const *ndfPathName, bool bUpdate) {
    if (IsNil (NDFPathName = ndfPathName)) SignalApplicationError (
	"NDF Path Name Not Specified"
    );

#ifdef __VMS
    const char* const openflags = (bUpdate ? "shr=get,nql" : "shr=upd,put,get,nql");
    g_hNDF = open (NDFPathName, bUpdate ? O_RDWR : O_RDONLY, 0, openflags);
#else
    g_hNDF = open (NDFPathName, bUpdate ? O_RDWR : O_RDONLY, 0);
#endif
    if (g_hNDF < 0) SignalUnixError (
	MakeMessage ("NDF '%s' Open Error", NDFPathName)
    );

    ReadNDH (&NDH);
    if (PS_NDH_NDFVersion(NDH) != PS_NDFVersionNumber) SignalApplicationError (
	MakeMessage (
	    "NDF '%s' Version %d Incompatible With Software Version %d",
	    NDFPathName, PS_NDFVersionNumber, PS_NDH_NDFVersion (NDH)
	)
    );
    ReadNDFArray (OSDPathName, sizeof (char));

    g_pNDF = new VdbNetwork (NDFPathName, OSDPathName, g_hNDF);

    if (!bUpdate)
	return;

    NDFFDLockable = true;

    PS_NVD iNVD;
    ReadNVD		(PS_NDH_CurrentNetworkVersion (NDH), &iNVD);
    SeekNDFLocation	(PS_NVD_SRV (iNVD));

    unsigned int spaceCount;
    ReadNDFStructure	(&spaceCount, sizeof (int), true);

    LockSpaces (spaceCount);
}


/************************************
 ************************************
 *****  Version Model Creation  *****
 ************************************
 ************************************/

/*---------------------------------------------------------------------------
 *  Structure Notes
 *  ========= =====
 *
 *  The data structures declared below model the dependancy information
 *  contained in an NDF.  As currently implemented, NDF's are optimized to
 *  return database configurations efficiently.  As databases are updated,
 *  new version and configuration information is written to the NDF.  The
 *  top level structure containing that information is an NVD (Network
 *  Version Descriptor).  An NVD contains pointers to prior NVD's and an
 *  NVD captive SRV (Space Role Vector).  The prior NVD's are the previous
 *  NVD for this database (commit thread), the previous NVD, if any, created
 *  by the Vision session that wrote this NVD (update thread), and the NVD
 *  accessed at start-up by the Vision session that wrote this NVD.  The
 *  SRV contains, for each space, an indicator that records whether the
 *  space was updated, read, or un-accessed by this transaction, along with
 *  a pointer to newest SVD (Space Version Descriptor) for the space at the
 *  time the NVD/SRV pair was created.  Because the SRV names the newest
 *  space versions currently available at the time of the update, reading
 *  a single NVD/SRV pair is all that is required to open the current
 *  configuration of the database.  Because the SRV does NOT identify the
 *  space versions actually read by a transaction, the dependancy information
 *  required to support this program ('ndftool'), must be reconstructed from
 *  the access, update, and commit predecessor pointers linking the NVDs.
 *  Reconstructing that dependancy information requires knowledge of Vision's
 *  isolation rules.  If those rules are changed, the reconstructions used
 *  in this program can be invalidated.  As an example, consider Vision's
 *  current restriction that transactions only read versions of object spaces
 *  named in their accessed configuration.  A less restrictive condition
 *  allows Vision to dynamically merge newer compatible versions of object
 *  spaces with an accessed configuration; however, doing so requires that
 *  this tool derive the same dynamic configurations.
 *
 *  CVersion
 *	- an update to / configuration of the database.  Decomposes 
 *  CResource
 *	- a version of an object space.
 *  CAccess:
 *	- a read-use of an object space version.
 *---------------------------------------------------------------------------
 */
class CAccess;
class CResource;
class CVersion;


/***************************
 *****  class CAccess  *****
 ***************************/

class CAccess : public VTransient {
    friend class CVersion;

//  Construction
public:
    CAccess (CVersion *pVersion, int xSpace);

//  Access
public:
    unsigned int Space () const {
	return m_xSpace;
    }
    CVersion *Version () const {
	return m_pVersion;
    }
    CResource *Resource () const {
	return m_pResource;
    }
    CAccess *ReadsetSuccessor () const {
	return m_pReadsetLink;
    }
    CAccess *ReadersSuccessor () const {
	return m_pReadersLink;
    }

//  State
protected:
    unsigned int	  const	m_xSpace;
    CVersion		* const	m_pVersion;
    CResource		*	m_pResource;
    CAccess		* const	m_pReadsetLink;
    CAccess		*	m_pReadersLink;
};


/*****************************
 *****  class CResource  *****
 *****************************/

class CResource : public VTransient {
    friend class CVersion;

//  Construction
public:
    CResource (PS_Type_FO xResource, CResource *pLB, CResource *pUB);
    CResource *LocateResource (PS_Type_FO xResource);

    void SetCreatorTo (CVersion *pVersion) {
	if (IsNil (m_pCreator))
	    m_pCreator = pVersion;
    }

//  Access
public:
    CResource *CommitPredecessor () const {
	return m_pCommitPredecessor;
    }
    CResource *CommitSuccessor () const {
	return m_pCommitSuccessor;
    }

    CVersion *Creator () const {
	return m_pCreator;
    }

    CAccess *Readers () const {
	return m_pReaders;
    }

    PS_SVD const &SVD () const {
	return m_iSVD;
    }
    PS_SVD &SVD () {
	return m_iSVD;
    }
    PS_Type_FO FO () const {
	return m_xResource;
    }

//  Query
public:
    bool operator< (PS_Type_FO xResource) const {
	return m_xResource < xResource;
    }
    bool operator<= (PS_Type_FO xResource) const {
	return m_xResource <= xResource;
    }
    bool operator== (PS_Type_FO xResource) const {
	return m_xResource == xResource;
    }
    bool operator!= (PS_Type_FO xResource) const {
	return m_xResource != xResource;
    }
    bool operator>= (PS_Type_FO xResource) const {
	return m_xResource >= xResource;
    }
    bool operator> (PS_Type_FO xResource) const {
	return m_xResource > xResource;
    }

    bool CreatedBy (CVersion const *pVersion) const {
	return pVersion == m_pCreator;
    }

//  State
protected:
    PS_Type_FO		  const	m_xResource;
    PS_SVD			m_iSVD;
    CVersion		       *m_pCreator;
    CResource		       *m_pCommitPredecessor;
    CResource		       *m_pCommitSuccessor;
    CAccess		       *m_pReaders;
};


/****************************
 *****  class CVersion  *****
 ****************************/

class CVersion : public VTransient {
//  Configuration Class
public:
    class Configuration : public VTransient {
	friend class CVersion;

    //  Construction
    protected:
	Configuration (CVersion *pVersion, unsigned int xVersion, CVersion *pLB, CVersion *pUB);
	Configuration (Configuration const &rSource);

    //  Access
    public:
	CVersion *Ancestor () const {
	    return m_pAncestor;
	}
	CVersion *CommitPredecessor () const {
	    return m_pCommitPredecessor;
	}
	CVersion *CommitSuccessor () const {
	    return m_pCommitSuccessor;
	}
	CVersion *UpdatePredecessor () const {
	    return m_pUpdatePredecessor;
	}
	CVersion *UpdateSuccessor () const {
	    return m_pUpdateSuccessor;
	}
	PS_Type_FO FO () const {
	    return m_xVersion;
	}
	PS_Type_FO MP () const {
	    return PS_NVD_MP (m_iNVD);
	}
	PS_NVD const &NVD () const {
	    return m_iNVD;
	}
	PS_NVD &NVD () {
	    return m_iNVD;
	}
	unsigned int SpaceCount () const {
	    return m_iSpaceCount;
	}
	CResource *SpaceResource (unsigned int xSpace) const {
	    return m_pConfiguration[xSpace];
	}

    //  Query
    public:

    //  Editing
    private:
	void AdjustLinks (CVersion *pVersion, CVersion *pLastLiveVersion);

    //  Update
    private:
	void WriteToNDF (CVersion *pVersion);

    //  State
    protected:
	PS_Type_FO		m_xVersion;
	PS_NVD			m_iNVD;
	unsigned int		m_iSpaceCount;
	CVersion	       *m_pAncestor;
	CVersion	       *m_pCommitPredecessor;
	CVersion	       *m_pCommitSuccessor;
	CVersion	       *m_pUpdatePredecessor;
	CVersion	       *m_pUpdateSuccessor;
	CResource	      **m_pConfiguration;
    };
    friend class Configuration;

//  Construction
protected:
    CVersion (PS_Type_FO xVersion, CVersion *pLB, CVersion *pUB);

private:
    void AddResource (unsigned int xSpace, PS_Type_FO xResource, bool isOwner);
    void AddAccess   (unsigned int xSpace);

    void InitializeAsGround	();
    void InitializeAsDependent	();

    void PropagateResourcesToDirectDescendent  (CVersion *pDescendent);
    void PropagateResourcesToProcessDescendent (CVersion *pDescendent);
    void PropagateResourcesToDescendents ();

    void SetInitialAncestorTo (CVersion *pVersion) {
	m_iConfiguration.m_pAncestor = pVersion;
    }
    void SetInitialCommitPredecessorTo (CVersion *pVersion) {
	m_iConfiguration.m_pCommitPredecessor = pVersion;
    }
    void SetInitialCommitSuccessorTo (CVersion *pVersion) {
	m_iConfiguration.m_pCommitSuccessor = pVersion;
    }
    void SetInitialUpdatePredecessorTo (CVersion *pVersion) {
	m_iConfiguration.m_pUpdatePredecessor = pVersion;
    }
    void SetInitialUpdateSuccessorTo (CVersion *pVersion) {
	m_iConfiguration.m_pUpdateSuccessor = pVersion;
    }
    
    bool Initialize (int *baseVersionCount);

public:
    CVersion *LocateVersion (PS_Type_FO xVersion, bool create);

    static void CreateModel (int *baseVersionCount, bool bExchange);

//  Access
public:
    CVersion *CurrentAncestor () const {
	return m_pConfiguration->Ancestor ();
    }
    CVersion *CurrentCommitPredecessor () const {
	return m_pConfiguration->CommitPredecessor ();
    }
    CVersion *CurrentCommitSuccessor () const {
	return m_pConfiguration->CommitSuccessor ();
    }
    CVersion *CurrentUpdatePredecessor () const {
	return m_pConfiguration->UpdatePredecessor ();
    }
    CVersion *CurrentUpdateSuccessor () const {
	return m_pConfiguration->UpdateSuccessor ();
    }
    static PS_Type_FO CurrentFO (CVersion const *pVersion) {
        return pVersion ? pVersion->CurrentFO () : PS_FO_Nil;
    }
    PS_Type_FO CurrentFO () const {
	return m_pConfiguration->FO ();
    }
    PS_Type_FO CurrentMP () const {
	return m_pConfiguration->MP ();
    }
    PS_NVD &CurrentNVD () {
	return m_pConfiguration->NVD ();
    }
    unsigned int CurrentSpaceCount () const {
	return m_pConfiguration->SpaceCount ();
    }
    CResource *CurrentSpaceResource (unsigned int xSpace) const {
	return m_pConfiguration->SpaceResource (xSpace);
    }

    CVersion *InitialAncestor () const {
	return m_iConfiguration.Ancestor ();
    }
    CVersion *InitialCommitPredecessor () const {
	return m_iConfiguration.CommitPredecessor ();
    }
    CVersion *InitialCommitSuccessor () const {
	return m_iConfiguration.CommitSuccessor ();
    }
    CVersion *InitialUpdatePredecessor () const {
	return m_iConfiguration.UpdatePredecessor ();
    }
    CVersion *InitialUpdateSuccessor () const {
	return m_iConfiguration.UpdateSuccessor ();
    }
    static PS_Type_FO InitialFO (CVersion const *pVersion) {
	return pVersion ? pVersion->InitialFO () : PS_FO_Nil;
    }
    PS_Type_FO InitialFO () const {
	return m_iConfiguration.FO ();
    }
    PS_Type_FO InitialMP () const {
	return m_iConfiguration.MP ();
    }
    PS_NVD const &InitialNVD () const {
	return m_iConfiguration.NVD ();
    }
    unsigned int InitialSpaceCount () const {
	return m_iConfiguration.SpaceCount ();
    }
    CResource *InitialSpaceResource (unsigned int xSpace) const {
	return m_iConfiguration.SpaceResource (xSpace);
    }

    CAccess *Readset () const {
	return m_pReadset;
    }

//  Query
public:
    bool Live () const {
	return !m_fDeleted;
    }
    bool Dead () const {
	return m_fDeleted;
    }
    bool Updated () const {
	return m_pConfiguration != &m_iConfiguration;
    }

//  Deletion
private:
    static CVersion *g_pDeletionQueue;

    bool EnqueueDeletion (bool bUserDeleted); // ... returns true if deleted
    void     ProcessDeletion ();

public:
    static void MarkDeletions (bool bQuiet);

//  Editing
private:
    void CreateNewConfiguration () {
	if (m_pConfiguration == &m_iConfiguration)
	    m_pConfiguration  = new Configuration (m_iConfiguration);
    }

    void UpdateConfiguration (CVersion *pLastLiveVersion) {
	CreateNewConfiguration ();
	m_pConfiguration->AdjustLinks (this, pLastLiveVersion);
    }

public:
    static void EditModel ();

//  Persistence
private:
    void WriteToNDF () {
	m_pConfiguration->WriteToNDF (this);
    }

public:
    static void UpdateNDF (bool bUpdate, bool bCommit, bool bExchange);

//  Display
protected:
    void DisplayHeader		(int *originalOffset, int *adjustedOffset) const;
    void DisplayUpdateThread	(unsigned int initialIndentation = 0) const;

public:
    void DisplayConfiguration	() const;
    void DisplayDescendents	() const;
    void DisplayDependencies	() const;

    void DisplayDetails	 (bool bVerbose, int *originalOffset, int *adjustedOffset) const;
    void DisplaySegments (bool bVerbose) const;

//  State
protected:
    char const		       *m_pAnnotation;
    CAccess		       *m_pReadset;
    CVersion		       *m_pDescendents;
    CVersion		       *m_pDescendentsLink;
    CVersion		       *m_pDeletionQueue;
    unsigned int		m_fDeleted		: 1,
				m_fDeletedByUser	: 1,
				m_fIsntGround		: 1;
    unsigned int		m_iSRVGrain;
    Configuration		m_iConfiguration;
    Configuration	       *m_pConfiguration;
};


/*******************
 *****  State  *****
 *******************/

PrivateVarDef CVersion*		NewestVersion		= NilOf (CVersion*);
PrivateVarDef CVersion*		OldestVersion		= NilOf (CVersion*);
PrivateVarDef CVersion*		OldestUpdatedVersion	= NilOf (CVersion*);

PrivateVarDef unsigned int	ReachableVersionCount	= 0,
				VisitedVersionCount	= 0,
				UserDeletedVersionCount = 0,
				RuleDeletedVersionCount	= 0;


/*******************************
 *****  Resource Creation  *****
 *******************************/

CResource::CResource (PS_Type_FO xResource, CResource *pLB, CResource *pUB)
: m_xResource			(xResource)
, m_pCommitPredecessor	(pLB)
, m_pCommitSuccessor	(pUB)
, m_pCreator		(0)
, m_pReaders		(0)
{
    ReadSVD (xResource, &m_iSVD);

    if (pLB)
	pLB->m_pCommitSuccessor = this;

    if (pUB)
	pUB->m_pCommitPredecessor = this;
}


/*****************************
 *****  Resource Lookup  *****
 *****************************/

CResource *CResource::LocateResource (PS_Type_FO xResource) {
    if (*this == xResource)
	return this;

    CResource *pLB;
    CResource *pUB = this;
    while (IsntNil (pLB = pUB->m_pCommitPredecessor) && *pLB > xResource)
	pUB = pLB;

    return IsntNil (pLB) && *pLB == xResource ? pLB : new CResource (xResource, pLB, pUB);
}


/****************************************
 *****  Version Creation Utilities  *****
 ****************************************/

void CVersion::AddResource (
    unsigned int xSpace, PS_Type_FO xResource, bool isOwner
) {
    m_iConfiguration.m_pConfiguration[xSpace] = IsntNil (
	InitialCommitSuccessor ()
    ) ? InitialCommitSuccessor ()->InitialSpaceResource (xSpace)->LocateResource (
	xResource
    ) : new CResource (xResource, NilOf (CResource*), NilOf (CResource*));
    if (isOwner)
	InitialSpaceResource (xSpace)->SetCreatorTo (this);
}

void CVersion::AddAccess (unsigned int xSpace) {
    if (PS_FO_IsntNil (PS_SVD_PreviousSVD (InitialSpaceResource (xSpace)->m_iSVD)))
	m_pReadset = new CAccess (this, xSpace);
}

CAccess::CAccess (CVersion *pVersion, int xSpace)
: m_pVersion		(pVersion)
, m_xSpace		(xSpace)
, m_pReadsetLink	(pVersion->Readset ())
, m_pResource		(0)
, m_pReadersLink	(0)
{
}


/******************************
 *****  Version Creation  *****
 ******************************/

CVersion::Configuration::Configuration (
    CVersion *pVersion, unsigned int xVersion, CVersion *pLB, CVersion *pUB
)
: m_xVersion		(xVersion)
, m_pAncestor		(0)
, m_pCommitPredecessor	(pLB)
, m_pCommitSuccessor	(pUB)
, m_pUpdatePredecessor	(0)
, m_pUpdateSuccessor	(0)
{
    ReadNVD (xVersion, &m_iNVD);

    pVersion->m_pAnnotation = PS_FO_IsntNil (PS_NVD_Annotation (m_iNVD))
	? ReadNDFString (PS_NVD_Annotation (m_iNVD)) : NilOf (char const *);

    PS_SRD iSRV[M_POP_MaxObjectSpace + 1];
    m_iSpaceCount = ReadSRV (
	PS_NVD_SoftwareVersion (m_iNVD), PS_NVD_SRV (m_iNVD), &pVersion->m_iSRVGrain, iSRV
    );
    m_pConfiguration = m_iSpaceCount > 0
	? (CResource**)allocate (m_iSpaceCount * sizeof (CResource*))
	: NilOf (CResource**);

/*****
 *  Process spaces in reverse order so configuration reporting logic works as expected...
 *****/
    for (unsigned int xSpace = m_iSpaceCount; xSpace-- > 0;) switch (
	PS_SRD_Role (iSRV[xSpace])
    ) {
    case PS_Role_Nonextant:
	m_pConfiguration[xSpace] = NilOf (CResource*);
	break;
    case PS_Role_Unaccessed:
	pVersion->AddResource (xSpace, PS_SRD_SVD (iSRV[xSpace]), false);
	break;
    case PS_Role_Read:
	pVersion->AddResource (xSpace, PS_SRD_SVD (iSRV[xSpace]), false);
	pVersion->AddAccess (xSpace);
	break;
    case PS_Role_Modified:
	pVersion->AddResource (xSpace, PS_SRD_SVD (iSRV[xSpace]), true);
	pVersion->AddAccess (xSpace);
	break;
    default:
	SignalApplicationError (
	    MakeMessage (
		"Unrecognized Role %d Found: Space %d, Version '%d', NDF '%s'",
		PS_SRD_Role (iSRV[xSpace]), xSpace, m_xVersion, NDFPathName
	    )
	);
	break;
    }

    if (pLB)
	pLB->SetInitialCommitSuccessorTo (pVersion);

    if (pUB)
	pUB->SetInitialCommitPredecessorTo (pVersion);
}


CVersion::Configuration::Configuration (Configuration const &rSource)
: m_xVersion		(rSource.m_xVersion)
, m_iNVD		(rSource.m_iNVD)
, m_iSpaceCount		(rSource.m_iSpaceCount)
, m_pAncestor		(rSource.m_pAncestor)
, m_pCommitPredecessor	(rSource.m_pCommitPredecessor)
, m_pCommitSuccessor	(0)
, m_pUpdatePredecessor	(rSource.m_pUpdatePredecessor)
, m_pUpdateSuccessor	(0)
, m_pConfiguration	(rSource.m_pConfiguration)
{
}

CVersion::CVersion (PS_Type_FO xVersion, CVersion *pLB, CVersion *pUB)
: m_iConfiguration	(this, xVersion, pLB, pUB)
, m_pConfiguration	(&m_iConfiguration)
, m_fDeleted		(false)
, m_fDeletedByUser	(false)
, m_fIsntGround		(true)
, m_pDeletionQueue	(0)
, m_pDescendents	(0)
, m_pDescendentsLink	(0)
, m_pReadset		(0)
{
    VisitedVersionCount++;
}


/****************************
 *****  Version Lookup  *****
 ****************************/

CVersion *CVersion::LocateVersion (PS_Type_FO xVersion, bool create) {
    if (xVersion == InitialFO ())
	return this;

    CVersion *pLB;
    CVersion *pUB = this;
    while (IsntNil (pLB = pUB->InitialCommitPredecessor ()) && xVersion < pLB->InitialFO ())
	pUB = pLB;

    return IsntNil (pLB) && xVersion == pLB->InitialFO ()
	? pLB : create ? new CVersion (xVersion, pLB, pUB) : NilOf (CVersion*);
}


/************************************
 *****  Version Initialization  *****
 ************************************/

void CVersion::InitializeAsGround () {
    if (IsntNil (InitialCommitPredecessor ()) && IsNil (InitialUpdateSuccessor ()))
	DisplayApplicationError ("Uninitialized Versions Present In Commit Chain");

    SetInitialCommitPredecessorTo (NilOf (CVersion*));
    m_fIsntGround = false;

    for (unsigned int xSpace = 0; xSpace < InitialSpaceCount (); xSpace++)
	if (InitialSpaceResource (xSpace))
	    InitialSpaceResource (xSpace)->SetCreatorTo (this);
}


void CVersion::InitializeAsDependent () {
    SetInitialAncestorTo (
	LocateVersion (PS_NVD_AccessedVersion (InitialNVD ()), true)
    );
    if (PS_NVD_AccessedVersion (InitialNVD ()) != PS_NVD_UpdateThread (InitialNVD ())) {
	SetInitialUpdatePredecessorTo (
	    LocateVersion (PS_NVD_UpdateThread (InitialNVD ()), true)
	);
	InitialUpdatePredecessor ()->SetInitialUpdateSuccessorTo (this);
    }
    else {
	m_pDescendentsLink = InitialAncestor ()->m_pDescendents;
	InitialAncestor ()->m_pDescendents = this;
    }
}


void CVersion::PropagateResourcesToDirectDescendent (CVersion *pDescendent) {
    for (
	CAccess *pAccess = pDescendent->m_pReadset;
	IsntNil (pAccess);
	pAccess = pAccess->m_pReadsetLink
    ) if (IsNil (pAccess->m_pResource)) {
	pAccess->m_pResource = InitialSpaceResource (pAccess->m_xSpace);
	pAccess->m_pReadersLink = pAccess->m_pResource->Readers ();

	pAccess->m_pResource->m_pReaders = pAccess;
    }
}

void CVersion::PropagateResourcesToProcessDescendent (CVersion *pDescendent) {
    CResource *pResource;

    for (
	CAccess *pAccess = pDescendent->m_pReadset;
	IsntNil (pAccess);
	pAccess = pAccess->m_pReadsetLink
    ) if (
	IsNil (pAccess->m_pResource) && (
	    pResource = InitialSpaceResource (pAccess->m_xSpace)
	)->CreatedBy (this)
    )
    {
	pAccess->m_pResource	= pResource;
	pAccess->m_pReadersLink	= pResource->m_pReaders;

	pResource->m_pReaders = pAccess;
    }
}


void CVersion::PropagateResourcesToDescendents () {
    CVersion *processDescendent;
    CVersion *directDescendent;

    for (
	processDescendent = InitialUpdateSuccessor ();
	IsntNil (processDescendent);
	processDescendent = processDescendent->InitialUpdateSuccessor ()
    ) PropagateResourcesToProcessDescendent (processDescendent);

    for (
	directDescendent = m_pDescendents;
	IsntNil (directDescendent);
	directDescendent = directDescendent->m_pDescendentsLink
    )
    {
	PropagateResourcesToDirectDescendent (directDescendent);
	for (
	    processDescendent = directDescendent->InitialUpdateSuccessor ();
	    IsntNil (processDescendent);
	    processDescendent = processDescendent->InitialUpdateSuccessor ()
	) PropagateResourcesToDirectDescendent (processDescendent);
    }
}

bool CVersion::Initialize (int *baseVersionCount) {
    ReachableVersionCount++;

    if (PS_NVD_BaseVersion (InitialNVD ()) && (*baseVersionCount)-- <= 0
    || PS_FO_IsNil (PS_NVD_PreviousVersion (InitialNVD ()))
    || PS_FO_IsNil (PS_NVD_AccessedVersion (InitialNVD ()))
    || PS_FO_IsNil (PS_NVD_UpdateThread    (InitialNVD ()))	
    )	InitializeAsGround ();
    else
	InitializeAsDependent ();

    PropagateResourcesToDescendents ();

    return PS_FO_IsntNil (PS_NVD_PreviousVersion (InitialNVD ())) && m_fIsntGround;
}


/****************************
 *****  Model Creation  *****
 ****************************/

void CVersion::CreateModel (int *baseVersionCount, bool bExchange) {
    if (bExchange) {
	if (PS_NDH_AlternateVersionChainMark (NDH) != PS_NDH_CurrentNetworkVersion (NDH)
	) SignalApplicationError (
	    MakeMessage (
		"Exchange Invalid - Network Has Been Updated.\n\
     Alternate Version %d, Alternate Version Base %d, Current Version %d",
		PS_NDH_AlternateVersionChainHead	(NDH),
		PS_NDH_AlternateVersionChainMark	(NDH),
		PS_NDH_CurrentNetworkVersion	(NDH)
	    )
	);

	PS_NDH_AlternateVersionChainMark (NDH) = PS_NDH_AlternateVersionChainHead (NDH);
	PS_NDH_AlternateVersionChainHead (NDH) = PS_NDH_CurrentNetworkVersion     (NDH);
	PS_NDH_CurrentNetworkVersion     (NDH) = PS_NDH_AlternateVersionChainMark (NDH);
    }

    OldestVersion = NewestVersion = new CVersion (
	PS_NDH_CurrentNetworkVersion (NDH), NilOf (CVersion*), NilOf (CVersion*)
    );
    while (OldestVersion->Initialize (baseVersionCount)) {
	OldestVersion = OldestVersion->LocateVersion (
	    PS_NVD_PreviousVersion (OldestVersion->InitialNVD ()), true
	);
    }
}


/***********************************
 ***********************************
 *****  Version Model Editing  *****
 ***********************************
 ***********************************/

/********************
 *****  Lookup  *****
 ********************/

PrivateFnDef CVersion* LocateVersionBySpaceAndSegment (unsigned int xSpace, int xSegment) {
    CResource *locatedResource;
    CResource *pResource;

    if (xSpace < 1 || xSpace >= NewestVersion->InitialSpaceCount ()) {
	SignalApplicationError (
	    MakeMessage (
		"Space Index %d Out Of Range 1 ... %d",
		xSpace, NewestVersion->InitialSpaceCount () - 1
	    )
	);
	return NilOf (CVersion*);
    }

    for (
	locatedResource = NilOf (CResource*),
	pResource = NewestVersion->InitialSpaceResource (xSpace);

	IsntNil (pResource);

	locatedResource = pResource, pResource = pResource->CommitPredecessor ()
    ) if (xSegment > PS_SVD_MaxSegment (pResource->SVD ()))
	break;

    if (IsNil (locatedResource) || xSegment < PS_SVD_MinSegment (locatedResource->SVD ())) {
	SignalApplicationError (
	    MakeMessage ("Segment %d Does Not Exist In Space %d", xSegment, xSpace)
	);
	return NilOf (CVersion*);
    }

    return locatedResource->Creator ();
}

PrivateFnDef CVersion* LocateVersionByIdAndOffset (PS_Type_FO xVersion, int versionOffset) {
    CVersion *pVersion = 0 == xVersion ? NewestVersion : NewestVersion->LocateVersion (
	xVersion, false
    );
    while (IsntNil (pVersion) && versionOffset-- > 0)
	pVersion = pVersion->InitialCommitPredecessor ();

    return pVersion;
}


/**********************
 *****  Deletion  *****
 **********************/

CVersion *CVersion::g_pDeletionQueue = 0;

bool CVersion::EnqueueDeletion (bool bUserDeleted) {
    if (m_fDeleted)
	return true;
    
    if (IsNil (InitialCommitPredecessor ()))
	return false;

    m_fDeleted = true;
    m_fDeletedByUser = bUserDeleted;

    m_pDeletionQueue = g_pDeletionQueue;
    g_pDeletionQueue = this;

    return true;
}

void CVersion::ProcessDeletion () {
    g_pDeletionQueue = m_pDeletionQueue;

    if (m_fDeletedByUser)
	UserDeletedVersionCount++;
    else
	RuleDeletedVersionCount++;

    for (unsigned int xSpace = 0; xSpace < InitialSpaceCount (); xSpace++) {
	CResource *pResource = InitialSpaceResource (xSpace);
	if (IsntNil (pResource) && pResource->CreatedBy (this)) {
	    CAccess *pAccess = pResource->m_pReaders;
	    while (pAccess) {
		pAccess->m_pVersion->EnqueueDeletion (false);
		pAccess = pAccess->m_pReadersLink;
	    }
	}
    }
}


void CVersion::MarkDeletions (bool bQuiet) {
    GOPT_SeekExtraArgument (0, 0);

    char const *pVersionSpecification;
    while (IsntNil (pVersionSpecification = GOPT_GetExtraArgument ())) {
	int xSpace, xSegment, xVersion = 0, versionOffset = 0;
	CVersion *pVersion;
	if(sscanf(pVersionSpecification, " S %d / %d", &xSpace, &xSegment) == 2) {
	    if (!bQuiet) display (
		"--->  Segment %d/%d Deletion Requested.\n", xSpace, xSegment
	    );
	    pVersion = LocateVersionBySpaceAndSegment (xSpace, xSegment);
	}
	else if (
	    sscanf (pVersionSpecification, " V %d - %d", &xVersion, &versionOffset) > 0
	) {
	    if (xVersion < 0) {
		versionOffset -= xVersion;
		xVersion = 0;
	    }
	    if (versionOffset < 0)
		versionOffset = 0;
	    if (!bQuiet) display (
		"--->  Version %d-%d Deletion Requested.\n", xVersion, versionOffset
	    );
	    pVersion = LocateVersionByIdAndOffset (xVersion, versionOffset);
	}
	else {
	    SignalApplicationError (
		MakeMessage ("Invalid Version Specification: <%s>\n", pVersionSpecification)
	    );
	    pVersion = NilOf (CVersion*);
	}

	if (IsNil (pVersion)) SignalApplicationError (
	    MakeMessage (
		"Version Specification <%s> Did Not Select A Valid Version",
		pVersionSpecification
	    )
	);
	else if (!pVersion->EnqueueDeletion (true)) SignalApplicationError (
	    MakeMessage (
		"Version Specification <%s> Selected The Undeletable Base Version",
		pVersionSpecification
	    )
	);
    }

    while (g_pDeletionQueue)
	g_pDeletionQueue->ProcessDeletion ();
}


/*********************
 *****  Editing  *****
 *********************/

void CVersion::Configuration::AdjustLinks (CVersion *pVersion, CVersion *pLastLiveVersion) {
//  Adjust the commit chain, ...
    m_pCommitPredecessor = pLastLiveVersion;
    pLastLiveVersion->m_pConfiguration->m_pCommitSuccessor = pVersion;

//  ... adjust the ancestor, ...
    while (m_pAncestor->Dead ())
	m_pAncestor = m_pAncestor->InitialCommitPredecessor ();

    while (IsntNil (m_pUpdatePredecessor) && m_pUpdatePredecessor->Dead ())
	m_pUpdatePredecessor = m_pUpdatePredecessor->InitialUpdatePredecessor ();
    if (m_pUpdatePredecessor)
	m_pUpdatePredecessor->m_pConfiguration->m_pUpdateSuccessor = pVersion;

//  ... rebuild the space configuration, ...
    m_pConfiguration = (CResource**)allocate (m_iSpaceCount * sizeof (CResource*));

    for (unsigned int xSpace = 0; xSpace < m_iSpaceCount; xSpace++) {
	CResource *pResource = pVersion->InitialSpaceResource (xSpace);
	m_pConfiguration[xSpace] = IsNil (pResource) || pResource->CreatedBy (pVersion)
	? pResource
	: xSpace < m_pCommitPredecessor->CurrentSpaceCount ()
	? m_pCommitPredecessor->CurrentSpaceResource (xSpace)
	: NilOf (CResource*);
    }

//  ... and note the deletion of any object spaces:
    while (m_iSpaceCount > 0 && IsNil (m_pConfiguration [m_iSpaceCount - 1]))
	m_iSpaceCount--;
}


void CVersion::EditModel () {
    if (UserDeletedVersionCount > 0) {
    //  Locate the oldest deleted version, ...
	CVersion *pVersion = OldestVersion;
	do {
	    pVersion = pVersion->InitialCommitSuccessor ();
	} while (IsntNil (pVersion) && pVersion->Live ());

    //  ...construct a new configuration for its predecessor, ...
	CVersion *pLastLiveVersion = pVersion->InitialCommitPredecessor ();
	pLastLiveVersion->CreateNewConfiguration ();

	OldestUpdatedVersion = pLastLiveVersion;

    //  ... and edit all subsequent non-deleted versions:
	while (pVersion = pVersion->InitialCommitSuccessor ()) {
	    if (pVersion->Live ()) {
		pVersion->UpdateConfiguration (pLastLiveVersion);
		pLastLiveVersion = pVersion;
	    }
	}
    }
}


/**********************************
 **********************************
 *****  Version Model Update  *****
 **********************************
 **********************************/

void CVersion::Configuration::WriteToNDF (CVersion *pVersion) {
/*****  Rebuild the SRV...  *****/
    PS_SRD iSRV[M_POP_MaxObjectSpace + 1];
    for (unsigned int xSpace = 0; xSpace < m_iSpaceCount; xSpace++) {
	CResource *pResource = m_pConfiguration [xSpace];
	if (pResource) {
	    PS_SRD_Role (iSRV[xSpace]) = pResource->CreatedBy (pVersion)
		? PS_Role_Modified : PS_Role_Unaccessed;
	    PS_SRD_SVD  (iSRV[xSpace]) = pResource->FO ();
	}
	else {
	    PS_SRD_Role (iSRV[xSpace]) = PS_Role_Nonextant;
	    PS_SRD_SVD  (iSRV[xSpace]) = PS_FO_Nil;
	}
    }
    for (
	CAccess *pAccess = pVersion->Readset ();
	IsntNil (pAccess);
	pAccess = pAccess->ReadsetSuccessor ()
    ) if (
	PS_SRD_Role (iSRV[pAccess->Space ()]) != PS_Role_Modified
    )   PS_SRD_Role (iSRV[pAccess->Space ()])  = PS_Role_Read;

/*****  Rebuild the NVD...  *****/
    PS_NVD_SRV (m_iNVD) = SeekNDFLocation (PS_FO_Nil);
    WriteSRV (m_iSpaceCount, pVersion->m_iSRVGrain, iSRV);

    PS_NVD_PreviousVersion (m_iNVD) = CurrentFO (m_pCommitPredecessor);

    PS_NVD_UpdateThread (m_iNVD) = CurrentFO (
	m_pUpdatePredecessor ? m_pUpdatePredecessor : m_pAncestor
    );

    PS_NVD_AccessedVersion	(m_iNVD) = CurrentFO (m_pAncestor);
    PS_NVD_MP			(m_iNVD) = InitialFO (m_pAncestor);

    m_xVersion = SeekNDFLocation (PS_FO_Nil);
    WriteNDFStructure (&m_iNVD, sizeof (PS_NVD));
}


void CVersion::UpdateNDF (bool bUpdate, bool bCommit, bool bExchange) {
/*****  Rebuild and save the configuration of updated versions...  *****/
    if (!bUpdate)
	return;

    bool changesMade = UserDeletedVersionCount > 0;
    if (changesMade) {
	CVersion *pNewestVersion = OldestUpdatedVersion;
	CVersion *pVersion;
	while (IsntNil (pVersion = pNewestVersion->CurrentCommitSuccessor ())) {
	    pVersion->WriteToNDF ();
	    pNewestVersion = pVersion;
	}

	if (bCommit) {
	    PS_NDH_AlternateVersionChainHead (NDH) = PS_NDH_CurrentNetworkVersion (NDH);
	    PS_NDH_AlternateVersionChainMark (NDH) =
	    PS_NDH_CurrentNetworkVersion     (NDH) = pNewestVersion->CurrentFO ();
	}
	else {
	    PS_NDH_AlternateVersionChainHead (NDH) = pNewestVersion->CurrentFO ();
	    PS_NDH_AlternateVersionChainMark (NDH) = PS_NDH_CurrentNetworkVersion (NDH);
	}
    }

/*****  ...and force all NDF updates to disk.  *****/
    if (changesMade || bExchange) {
	PS_NDH_Generation (NDH)++;
	SeekNDFLocation		(0);
	WriteNDFStructure	(&NDH, sizeof (PS_NDH));
	SynchronizeNDF		(5);
    }
}


/***********************************
 ***********************************
 *****  Version Model Display  *****
 ***********************************
 ***********************************/

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
PrivateFnDef char const *TimeStampAsString (PS_TS const *tsp) {
    V::VTime iTime (*tsp);
    struct tm iTimeStruct;
    struct tm *tm = iTime.localtime (iTimeStruct);

/****  using 'mm/dd/yy:hh:mm:ss' format  ****/
    static char buffer[80]; 
    sprintf (
	buffer,
	"%02d/%02d/%02d:%02d:%02d:%02d.%06d",
	tm->tm_mon + 1,
	tm->tm_mday,
	tm->tm_year % 100,
	tm->tm_hour,
	tm->tm_min,
	tm->tm_sec,
	tsp->microseconds
    );

    return buffer;
}

void CVersion::DisplayHeader (int *originalOffset, int *adjustedOffset) const {
    display (
	"----------------------------------------------------------------\n"
    );
    if (m_fDeleted && m_fDeletedByUser) display (
	"!!!!!!!!!!!!!!!!!!!!    DELETED BY USER    !!!!!!!!!!!!!!!!!!!!!\n"
    );
    else if (m_fDeleted) display (
	"!!!!!!!!!!!!!!!!!!!!    DELETED BY RULE    !!!!!!!!!!!!!!!!!!!!!\n"
    );
    else if (Updated ()) display (
	"!!!!!!!!!!!!!!!!!!!!        UPDATED        !!!!!!!!!!!!!!!!!!!!!\n"
    );

    display ("\
*    Version. . . . . . . . . . . %d (%d/%d)\n\
*    TID. . . . . . . . . . . . . %08x.%08x.%08x(%d)\n\
*    Commit Time Stamp. . . . . . %s\n\
*    Annotation . . . . . . . . . <%s>\n\
*    Compaction Generation. . . . %d (%d)\n\
*    Software Generation. . . . . %d\n",
	InitialFO (),
	(*originalOffset)--,
	m_fDeleted ? *adjustedOffset : (*adjustedOffset)--,
	PS_TID_High	(PS_NVD_TID (InitialNVD ())),
	PS_TID_Low	(PS_NVD_TID (InitialNVD ())),
	PS_TID_Sequence	(PS_NVD_TID (InitialNVD ())),
	PS_TID_Sequence	(PS_NVD_TID (InitialNVD ())),
	TimeStampAsString (&PS_NVD_CommitTimeStamp (InitialNVD ())),
	IsntNil (m_pAnnotation) ? m_pAnnotation : "",
	PS_NVD_DirectoryVersion (InitialNVD ()),
	Canonical (PS_NVD_PreviousNVDChain (InitialNVD ())),
	PS_NVD_SoftwareVersion  (InitialNVD ())
    );

    if (InitialMP () != CurrentMP ()) display ("\
*    Maintenance Predecessor. . . %d (Changed To %d)\n",
	Canonical (InitialMP ()), CurrentMP ()
    );
    else display ("\
*    Maintenance Predecessor. . . %d\n",
	Canonical (InitialMP ())
    );

    if (InitialAncestor () != CurrentAncestor ()) display ("\
*    Accessed Version . . . . . . %d (Changed To %d)\n",
	Canonical (InitialFO (InitialAncestor ())),
	Canonical (InitialFO (CurrentAncestor ()))
    );
    else display ("\
*    Accessed Version . . . . . . %d\n",
	Canonical (InitialFO (InitialAncestor ()))
    );

    if (InitialCommitPredecessor () != CurrentCommitPredecessor ()) display ("\
*    Commit Predecessor . . . . . %d (Changed To %d)\n",
	Canonical (InitialFO (InitialCommitPredecessor ())),
	Canonical (InitialFO (CurrentCommitPredecessor ()))
    );
    else display ("\
*    Commit Predecessor . . . . . %d\n",
	Canonical (InitialFO (InitialCommitPredecessor ()))
    );

    if (InitialCommitSuccessor () != CurrentCommitSuccessor ()) display ("\
*    Commit Successor . . . . . . %d (Changed To %d)\n",
	Canonical (InitialFO (InitialCommitSuccessor ())),
	Canonical (InitialFO (CurrentCommitSuccessor ()))
    );
    else display ("\
*    Commit Successor . . . . . . %d\n",
	Canonical (InitialFO (InitialCommitSuccessor ()))
    );

    if (InitialUpdatePredecessor () != CurrentUpdatePredecessor ()) display ("\
*    Process Predecessor. . . . . %d (Changed To %d)\n",
	Canonical (InitialFO (InitialUpdatePredecessor ())),
	Canonical (InitialFO (CurrentUpdatePredecessor ()))
    );
    else display ("\
*    Process Predecessor. . . . . %d\n",
	Canonical (InitialFO (InitialUpdatePredecessor ()))
    );

    if (InitialUpdateSuccessor () != CurrentUpdateSuccessor ()) display ("\
*    Process Successor. . . . . . %d (Changed To %d)\n",
	Canonical (InitialFO (InitialUpdateSuccessor ())),
	Canonical (InitialFO (CurrentUpdateSuccessor ()))
    );
    else display ("\
*    Process Successor. . . . . . %d\n",
	Canonical (InitialFO (InitialUpdateSuccessor ()))
    );
}


void CVersion::DisplayConfiguration () const {
    display ("*    Configuration:\n\
Space ConfigId   ReadId  RootSeg   MaxSeg   MinSeg  CTOffset OrigRoot   PrevId\
\n"
    );

    CAccess *access = m_pReadset;
    for (unsigned int xSpace = 0; xSpace < InitialSpaceCount (); xSpace++) {
	CResource *pResource = InitialSpaceResource (xSpace);
	if (IsNil (pResource))
	    display (" %4d\n", xSpace);
	else {
	    CResource *altResource = Updated () ? m_pConfiguration->m_pConfiguration[xSpace] : NilOf (CResource*);

	    display (
		"%c%4d%9d",
		pResource->CreatedBy (this)
		? 'M'
		: Updated () && IsNil (altResource)
		? 'D'
		: IsntNil (altResource) && altResource != pResource
		? 'U'
		: ' ',
		xSpace,
		pResource->m_xResource
	    );
	    if (IsntNil (access) && xSpace == access->m_xSpace) {
		display (
		    "%9d", IsntNil (access->m_pResource) ? (int)access->m_pResource->m_xResource : -1
		);
		access = access->m_pReadsetLink;
	    }
	    else
		display ("%9c", ' ');
	    display (
		"%9d%9d%9d%10d%9d%9d\n",
		PS_SVD_RootSegment	(pResource->m_iSVD),
		PS_SVD_MaxSegment	(pResource->m_iSVD),
		PS_SVD_MinSegment	(pResource->m_iSVD),
		PS_SVD_CTOffset		(pResource->m_iSVD),
		PS_SVD_OrigRootSegment	(pResource->m_iSVD),
		Canonical (PS_SVD_PreviousSVD (pResource->m_iSVD))
	    );
	    if (IsntNil (altResource) && altResource != pResource) display (
		"U    %9d         %9d%9d%9d%10d%9d%9d\n",
		altResource->m_xResource,
		PS_SVD_RootSegment	(altResource->m_iSVD),
		PS_SVD_MaxSegment	(altResource->m_iSVD),
		PS_SVD_MinSegment	(altResource->m_iSVD),
		PS_SVD_CTOffset		(altResource->m_iSVD),
		PS_SVD_OrigRootSegment	(altResource->m_iSVD),
		Canonical (PS_SVD_PreviousSVD (altResource->m_iSVD))
	    );
	}
    }
}


void CVersion::DisplayUpdateThread (unsigned int initialIndentation) const {
    unsigned int indentation = initialIndentation;

    CVersion *pDescendent = InitialUpdateSuccessor ();
    while (pDescendent) {
	if (indentation <= 70)
	    indentation += display ("->%d", pDescendent->InitialFO ());
	else indentation = display (
	    "\n%*c->%d", initialIndentation, ' ', pDescendent->InitialFO ()
	) - 1;

	pDescendent = pDescendent->InitialUpdateSuccessor ();
    }
}

void CVersion::DisplayDescendents () const {
    display ("*    Children:\n");

    CVersion *pDescendent = m_pDescendents;
    while (pDescendent) {
	pDescendent->DisplayUpdateThread (display ("     %d", pDescendent->InitialFO ()));
	display ("\n");

	pDescendent = pDescendent->m_pDescendentsLink;
    }
}


void CVersion::DisplayDependencies () const {
    display ("*    Dependents:\n");
    for (unsigned int xSpace = 0; xSpace < InitialSpaceCount (); xSpace++) {
	CResource *pResource = InitialSpaceResource (xSpace);
	if (IsntNil (pResource) && pResource->CreatedBy (this)) {
	    int initialIndentation = display ("    %d:", xSpace);
	    int indentation = initialIndentation;
	    for (
		CAccess *pAccess = pResource->m_pReaders;
		IsntNil (pAccess);
		pAccess = pAccess->m_pReadersLink
	    ) if (indentation > 70) indentation = display (
		"\n%*c %d", initialIndentation, ' ', pAccess->m_pVersion->InitialFO ()
	    ) - 1;
	    else
		indentation += display (" %d", pAccess->m_pVersion->InitialFO ());
	    display ("\n");
	}
    }
}

void CVersion::DisplayDetails (bool bVerbose, int *originalOffset, int *adjustedOffset) const {
    if (bVerbose || m_fDeleted || Updated ()) {
	DisplayHeader		(originalOffset, adjustedOffset);
	DisplayConfiguration	();
	DisplayDescendents	();
	DisplayDependencies	();
    }
}

void CVersion::DisplaySegments (bool bVerbose) const {
    if (bVerbose || m_fDeleted) {
	for (unsigned int xSpace = 0; xSpace < InitialSpaceCount (); xSpace++) {
	    CResource *pResource = InitialSpaceResource (xSpace);
	    if (IsntNil (pResource) && pResource->CreatedBy (this)) display (
		"%-9d %-7s %4d/%-6d %17.17s %s\n",
		InitialFO (),
		!m_fDeleted ? "Valid" : m_fDeletedByUser ? "UserDel" : "RuleDel",
		xSpace,
		PS_SVD_RootSegment (pResource->m_iSVD),
		TimeStampAsString (&PS_NVD_CommitTimeStamp (InitialNVD ())),
		IsntNil (m_pAnnotation) ? m_pAnnotation : ""
	    );
	}
    }
}


PrivateFnDef void DisplayVersionModelSegmentReport (bool bVerbose) {
    if (bVerbose || UserDeletedVersionCount + RuleDeletedVersionCount > 0) {
	printf ("Version   Role     Segment    Commit Time       Annotation\n");

	CVersion *pVersion = NewestVersion;
	while (pVersion) {
	    pVersion->DisplaySegments (bVerbose);
	    pVersion = pVersion->InitialCommitPredecessor ();
	}
    }
}


PrivateFnDef void DisplayVersionModelSummaryReport (bool /*bVerbose*/) {
    printf ("\
----------------------------------------------------------------\n\
*    NDF Path Name. . . . . . . . %s\n\
*    Signature. . . . . . . . . . %d\n\
*    NDF Version. . . . . . . . . %d\n\
*    Compaction Generation. . . . %d\n\
*    Modification Time Stamp. . . %s\n\
*    Object Space Directory . . . %s\n\
*    Newest Version . . . . . . . %d\n\
*    Oldest Version . . . . . . . %d\n\
*    Reachable Version Count. . . %d\n\
*    Visited Version Count. . . . %d\n\
*    User Deleted Version Count . %d\n\
*    Rule Deleted Version Count . %d\n",
	NDFPathName,
	PS_NDH_Signature	(NDH),
	PS_NDH_NDFVersion	(NDH),
	PS_NDH_DirectoryVersion	(NDH),
	TimeStampAsString (&PS_NDH_ModificationTimeStamp (NDH)),
	OSDPathName,
	CVersion::InitialFO (NewestVersion),
	CVersion::InitialFO (OldestVersion),
	ReachableVersionCount,
	VisitedVersionCount,
	UserDeletedVersionCount,
	RuleDeletedVersionCount
    );
}


PrivateFnDef void DisplayVersionModelDetailReport (bool bVerbose) {
    int originalOffset = 0, adjustedOffset = 0;

    DisplayVersionModelSummaryReport (bVerbose);

    CVersion *pVersion = NewestVersion;
    while (pVersion) {
	pVersion->DisplayDetails (bVerbose, &originalOffset, &adjustedOffset);
	pVersion = pVersion->InitialCommitPredecessor ();
    }
}


PrivateFnDef void DisplayVersionModel (int xFormat, bool bQuiet, bool bVerbose) {
    if (bQuiet)
	return;

    switch (xFormat) {
    case 0:
	DisplayVersionModelSegmentReport (bVerbose);
	break;
    case 1:
	DisplayVersionModelDetailReport (bVerbose);
	break;
    case 2:
	DisplayVersionModelSummaryReport (bVerbose);
	break;
    default:
	break;
    }
}


/********************************
 ********************************
 *****  Option Definitions  *****
 ********************************
 ********************************/

GOPT_BeginOptionDescriptions

    GOPT_ValueOptionDescription (
	"NDFPathName"		, 'n', "NDFPathName"	, "/vision/network/NDF"
    )
    GOPT_ValueOptionDescription (
	"BaseVersionCount"	, 'b', NilOf (char *)	, "0"
    )
    GOPT_SwitchOptionDescription (
	"CommitMode"		, 'c', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"UpdateMode"		, 'u', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"ExchangeMode"		, 'x', NilOf (char *)
    )
    GOPT_ValueOptionDescription (
	"ReportFormat"		, 'r', NilOf (char *)	, "0"
    )
    GOPT_SwitchOptionDescription (
	"QuietMode"		, 'q', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"VerboseMode"		, 'v', NilOf (char *)
    )

GOPT_EndOptionDescriptions;


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    GOPT_AcquireOptions (argc, argv);

    bool bCommit	= GOPT_GetSwitchOption ("CommitMode") > 0;
    bool bUpdate	= GOPT_GetSwitchOption ("UpdateMode") > 0;
    bool bExchange	= GOPT_GetSwitchOption ("ExchangeMode") > 0;
    bool bQuiet		= GOPT_GetSwitchOption ("QuietMode") > 0;
    bool bVerbose	= GOPT_GetSwitchOption ("VerboseMode") > 0;

    int baseVersionCount= atoi (GOPT_GetValueOption ("BaseVersionCount"));
    int reportFormat	= atoi (GOPT_GetValueOption ("ReportFormat"));

    OpenNDF (GOPT_GetValueOption ("NDFPathName"), bUpdate);

    CVersion::CreateModel   (&baseVersionCount, bExchange);
    CVersion::MarkDeletions (bQuiet);
    CVersion::EditModel	    ();
    CVersion::UpdateNDF	    (bUpdate, bCommit, bExchange);

    CloseNDF ();

    DisplayVersionModel	(reportFormat, bQuiet, bVerbose);

    return NormalExitValue;
}

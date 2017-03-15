/*****  Machine Specific Memory Services  *****/

#include "MSsupport.h"
#include "stdoss.h"
#include "SImem.h"

#include <stdio.h>

#ifdef hp9000s500		/*****  HP9000 series 500  *****/
#include <fcntl.h>

PublicFnDecl ADDRESS	memallc();

PublicFnDef ADDRESS HPUX_memallc(fn, offset, len, maxlen, type, mode)
char	*fn;
int	offset, len, maxlen, type, mode;
{
	int	fd, amode, omode;
	ADDRESS	ptr;
	int	terrno, terrinfo;
	
	if( fn == NULL )
	{
		ptr = memallc(-1, offset, len, maxlen, type, mode);
		return ptr != (ADDRESS)(-1) ? ptr : Nil;
	}

	if( mode & MEM_W )
	{
		amode = 02 | 04;
		omode = O_RDWR;
	}
	else
	{
		amode = 04;
		omode = O_RDONLY;
	}
	
	if( access(fn,amode) < 0 )
		return Nil;

	if( -1 == (fd = open(fn,omode,0)) )
		return Nil;

	ptr = memallc(fd, offset, len, maxlen, type, mode);

	terrno = errno;
	terrinfo = errinfo;

	close(fd);

	errno = terrno;
	errinfo = terrinfo;

	return ptr != (ADDRESS)(-1) ? ptr : Nil;
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#include file
#include psldef
#include rms
#include rmsdef
#include secdef
#include ssdef
#include stsdef
#include stat
#endif

/******************************
 *  Mapped Memory Management  *
 ******************************/
#ifdef VMS				/*****  VAX/VMS  *****/
/*---------------------------------------------------------------------------
 * This section contains an emulation of the features of HP-UX's memallc,
 * group.  The emulation is not exact, but rather claims to duplicate only the
 * features relevant to the backend and to the update facilities.
 *
 * A pool of 'virtual extents' is maintained.  Each extent is created via a
 * call to memallc. The size of each extent is determined by the maxlen
 * parameter. The extents are described by data structures which are arranged
 * in a singly linked list. New extents are appended to the end of the chain. 
 * A pool keeps track of its head, its tail and the number of extents which
 * are 'free'.  When memfree is called, the virtual memory described by the
 * extent is not relinquished. Only the mapping of the file to the extent is
 * discarded.  Subsequent attempts to map a file will attempt to use the freed
 * extent. If the extent is large enough for the mapping it is used. No
 * attempt is currently made to create a new extent out of any left over space.
 *
 * The maintainance of these extents is charged against the process's
 * virtualpagecount, but not against the pagefilequota. Moreover, the mapped
 * files are not charged against the processes file count.
 *---------------------------------------------------------------------------
 */

/****************************************
 *****  Helper Macros and Mnemonics *****
 ****************************************/
#define ProgramRegion	  0
#define PageSize	512
#define PageCount(bytes)\
    (bytes/PageSize + 1)

#define max(x, y)  ((x) > (y) ? (x) : (y))
/*****  RMS Name Block Access Macros  *****/
#define FID0(nam)		((nam).nam$w_fid[0])
#define FID1(nam)		((nam).nam$w_fid[1])
#define FID2(nam)		((nam).nam$w_fid[2])
#define DVI_L(nam)		((nam).nam$t_dvi[0])
#define DVI(nam)		((nam).nam$t_dvi + 1)


/***** Utility to build a unique global name based on a file's device
 ***** residence and its identification number.
 *
 *  Arguments:
 *	buffer			- pointer to the character array where the
 *				  constructed global name should be deposited.
 *	nam			- an RMS name block structure which contains a
 *				  file's device name and unique id.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PrivateFnDef void
    GenerateGlobalName (buffer, nam)
char *buffer;
struct NAM nam;
{
    char format[32];

    sprintf (format, "%%%d.%ds%%d%%d%%d", DVI_L(nam), DVI_L (nam));
    sprintf (buffer, 
	     format, 
	     DVI (nam), 
	     FID0 (nam), 
	     FID1 (nam), 
	     FID2 (nam));
}

/****************************
 *****  Virtual Extent  *****
 ****************************/

/*---------------------------------------------------------------------------
 * A Virtual Extent Descriptor records the state of a file-mappable address 
 * space.
 *
 *  Virtual Extent Descriptor Field Descriptions:
 *	start			- the starting virtual address of the extent.
 *	end			- the virtual address marking the end of the
 *				  mapped file.
 *	maxend			- the final address of the extent. Files may
 *				  not be 'memvaried' beyond this point.
 *	next			- the address of the next VED in
 *				  the pool of extents.
 *	fab			- An RMS FileAccessBlock for the file to be
 *				  mapped.
 *	nam			- An RMS Name Block used in determining a
 *				  unique global section name for shared
 *				  extents.
 *	inputfd			- a 'C' file descriptor for the file to be
 *				  mapped.
 *	isFree			- a flag which when set means that no file is
 *				  mapped to this extent.
 *	mode			- Indicates whether this memory is read only
 *				  or read-write.
 *	type			- Indicates whether the file is mapped into
 * 				  private or shared memory.
 *	fname			- An ASCII character string for the mapped
 *				  file's name.
 *---------------------------------------------------------------------------
 */

typedef struct VirtualExtentDescriptor 
{
    ADDRESS		 start,
			 end,
			 maxend;
    struct VirtualExtentDescriptor
			 *next;
    struct FAB		 fab;
    struct NAM		 nam;
    int			 inputfd;
    unsigned short	 isFree 	:1,
			 mode		:7,
			 type		:8;
    char		 fname [256];
} *VED;

/*****  Access Macros  *****/
#define NextExtent(ve) 		((ve)->next)
#define StartAddress(ve)	((ve)->start)
#define EndAddress(ve)		((ve)->end)
#define MaxAddress(ve)		((ve)->maxend)
#define Fab(ve)			((ve)->fab)
#define Nam(ve)			((ve)->nam)
#define Channel(ve)		(Fab (ve).fab$l_stv)
#define InputFD(ve)		((ve)->inputfd)
#define Mode(ve)		((ve)->mode)
#define Type(ve)		((ve)->type)
#define FName(ve)		((ve)->fname)
#define IsFree(ve)		((ve)->isFree)

#define EntrySize(ve)\
    (MaxAddress (ve) - StartAddress (ve))

/******************************
 *****  Memory Pool Type  *****
 ******************************/

/*---------------------------------------------------------------------------
 * This structure describes a memory pool.
 *
 * Memory Pool Type field descriptions:
 *	firstExtent		- the first of a singly linked list of VED's.
 *	finalExtent		- the last of a singly linked list of VED's.
 *	freeCount		- the number of extents in the pool which have
 *				  no files mapped into them.
 *---------------------------------------------------------------------------
 */

typedef struct {
    VED		firstExtent, 
		finalExtent;
    int		freeCount;
} MemoryPoolType, *MemoryPoolPtr;

/*****  Access Macros  *****/
#define FirstExtent(pool)	((pool)->firstExtent)
#define FinalExtent(pool)	((pool)->finalExtent)
#define FreeCount(pool)		((pool)->freeCount)

/*****  The process's Memallc Pool  *****/
PrivateVarDef MemoryPoolType 
    MemallcPool;

/***************************************
 *****  Pool Management Utilities  *****
 ***************************************/

/*****  Utility to search for a free extent greater than some size.
 *
 *  Arguments:
 *	pool		- the pool to search.
 *	size		- the minimum acceptable size for the extent.
 *
 *  Returns:
 *	The extent that it found or Nil if none was suitable.
 *
 *  Note:
 *	This routine naively follows the chain from beginning to end searching
 * for a suitable extent.  It returns the first one it finds, making no
 * attempt to determine a 'best match'.  This naivete is acceptable under
 * current usage patterns but should be examined if the usage is made more
 * general.
 *****/
PrivateFnDef VED
    LocateFreeArea (pool, size)
MemoryPoolPtr pool;
int size;
{
    VED
    	extent = FirstExtent (pool);

/**** 
 * If the pool is empty, or there are no free extents, return Nil ... 
 ****/
    if (IsNil (extent) || 0 == FreeCount (pool)) 
        return (VED) Nil;

/**** Plow through the pool looking for a free extent of acceptable size  ****/
    do
    {
	if (IsFree (extent) && size <= EntrySize (extent))
	{
	/****  Indicate that one has been found and return it  ****/
		IsFree (extent) = FALSE;
		FreeCount (pool)--;
		return extent;
	}
    }
    while (IsntNil (extent = NextExtent (extent)));
    return extent;
}

/*****  Utility to find a particular extent in a memory pool.
 *
 *  Arguments:
 *	pool		- The pool to search.
 *	address		- The address identifying which extent is desired.
 *
 *  Returns:
 *	The VED for the desired extent or Nil if it isn't present.
 *
 *  Note:
 *	Current Usage patterns make the linear search in this function
 * acceptable. Only processes with small pools will use this utility. If usage
 * patterns change, this algorithm will need adjustment.
 *****/
PrivateFnDef VED
    FindVirtualAddressInPool (pool, address)
MemoryPoolPtr pool;
char *address;
{
    VED
    	extent = FirstExtent (pool);

    if (IsNil (extent)) return extent;
    do
    {
	if (StartAddress (extent) == address)
		return extent;
    }
    while (IsntNil (extent = NextExtent (extent)));
    return extent;
}

/*****  Utility to add a virtual extent to a memory pool
 *
 *  Arguments:
 *	pool		- the memory pool to be grown.
 *	size		- the size of the extent to be created.
 *
 *  Returns:
 *	A VED for the newly created extent.
 *
 *****/
PrivateFnDef VED
    AddToPool (pool, size)
MemoryPoolPtr pool;
int size;
{
    VED
	tail = FinalExtent (pool);
    ADDRESS retadr[2];
    STATUS status;

    if (IsNil (tail))
    /****  The pool is empty, create it  ****/
    {
	FirstExtent (pool) = 
	tail 		   = malloc (sizeof (struct VirtualExtentDescriptor));
    }
    else
    /****  Otherwise link a new VED to the tail  ****/
    {
	NextExtent (tail) = malloc (sizeof (struct VirtualExtentDescriptor));
	tail = NextExtent (tail);
    }
    if (IsNil (tail)) return tail;

/**** Initialize the new tail  ****/
    FinalExtent  (pool) 	= tail;
    NextExtent	 (tail)		= (VED)Nil;
    Channel 	 (tail)		= -1;

/**** Acquire the necessary virtual address space  ****/
    SpecialHandleVMSRoutine 
        (sys$expreg (size, retadr, 0, ProgramRegion),
	 TrueCond,
	 "sys$expreg",
    	 IsFree	 (tail)	= TRUE,
	 Nil);

    StartAddress (tail)	=
    EndAddress	 (tail)	= retadr [0];
    MaxAddress	 (tail)	= retadr [1];
    IsFree	 (tail)	= FALSE;
    	
    return tail;
}

/***** Utility to get address space of sufficient size.
 *
 *  Argument:
 *      pool			-  The pool from which to allocate the space.
 *	size			-  The size of address space needed.
 *
 *  Returns:
 *	A VED for the desired address space or Nil if unable to procure it.
 *
 *****/
PrivateFnDef VED
    GetVirtualAddressSpace (pool, size)
MemoryPoolPtr pool;
int size;
{
    VED
    	extent;

/****  If there is a suitable, unused extent in the pool, use it ...  ****/
    if (IsNil (extent = LocateFreeArea (pool, size)))
    {
    /****  Make a new one if no suitable old ones are available  ****/
	extent = AddToPool (pool, PageCount (size));
    }
    return extent;
}

/*****  Utility to get a channel to a file as a precursor to mapping the file.
 *
 *  Argument:
 *	extent			-  The extent into which the file will be
 * 				   mapped.
 *  Returns:
 *	Zero if successful, -1 if unsuccessful.
 *
 *****/
PrivateFnDef int
    GetChannel (extent)
VED extent;
{
    STATUS status;

/****  Initialize the RMS structures  ****/
    Fab (extent) = cc$rms_fab;
    Nam (extent) = cc$rms_nam;

    Fab (extent).fab$l_fna = FName (extent);
    Fab (extent).fab$b_fns = strlen (FName (extent));

/**** 
 * Indicate that RMS will only open and close the file.  No other
 * operations can be performed.  This is a requirement for mapping a file on
 * VMS.
 ****/
    Fab (extent).fab$l_fop = FAB$M_UFO;
/****  Allow shared reading and writing if access allows it  ****/
    Fab (extent).fab$b_shr = FAB$M_SHRGET | FAB$M_UPI;
/****  Allow read access  ****/
    Fab (extent).fab$b_fac = FAB$M_GET;
    if( Mode (extent) & MEM_W )
/****  Allow write access if requested  ****/
	Fab (extent).fab$b_fac |= FAB$M_PUT;
/****  Request that the entire file be mapped  ****/
    Fab (extent).fab$b_rtv = -1;
/****  Attach the name block (needed to generate global section names)  ****/
    Fab (extent).fab$l_nam = &Nam (extent);
/****  Open the file  ****/
    HandleVMSRoutine (sys$open (&Fab (extent)), "sys$open", ;);

    return 0;
}

/*****  Routine to map a file into and address space 
 *
 *  Arguments:
 *	extent			- a descriptor for the file and address space
 *				  to be mapped.
 *	offset			- The offset in the file where the mapping is
 *				  to start. (unimplemented -- set to 0)
 *	len			- The number of bytes to map.
 *
 *  Returns:
 *	If successful, the starting address of the mapped file.
 *	If not, Nil.
 *
 *****/
PrivateFnDef ADDRESS
    MapFile (extent, offset, len)
VED extent;
int offset, len;
{
    ADDRESS inadr[2], retadr[2];
    int acmode, flags, ident, relpag, pagcnt, vbn, prot, pfc;
    STATUS status;
    char gsdstring[50];
    $LDESCRIPTOR(gsdnam, 49, gsdstring);

    flags = 0;

/****
 * If the section is to be shared, set up the parameters to enable sharing
 ****/
    if( (Type (extent) & MEM_SHARED) && !(Mode (extent) & MEM_W) )
	    {
	GenerateGlobalName (gsdstring, Nam (extent));
	$LENGTH (gsdnam) = strlen (gsdstring);
	flags |= SEC$M_GBL;
    }
/****  Fill in the address range parameters  ****/
    inadr[0] = StartAddress (extent);
    inadr[1] = StartAddress (extent) + len;
/****  We want user access mode  ****/
    acmode = PSL$C_USER;
/****  Allow writing if requested  ****/
    if( Mode (extent) & MEM_W )
	flags |= SEC$M_WRT;
/****  Calculate the number of pages needed ****/
    pagcnt = PageCount (len);
/****  Unused parameters  ****/
    ident = 0;
    relpag = 0;
    vbn = 0;
    prot = 0;
    pfc = 0;

/****  Do the mapping  ****/
    HandleVMSRoutine 
	(sys$crmpsc (inadr, retadr, acmode, flags, &gsdnam, ident, 
		     relpag, Channel (extent), pagcnt, vbn, prot, pfc),
	 "sys$crmpsc", ;);

    if (inadr[0] != retadr[0]) 
    {
/****
 * VMS couldn't give us what we asked for. Our management of the memallc
 * pool isn't correct.
 ****/
	printf ("Unrequested return address: %X : %X\n", inadr[0], retadr [0]);
	return Nil;
    }
    EndAddress (extent) = retadr[1];
/****  Return the address of the mapped file  ****/
    return retadr[0];
}

/*****  Utility to reopen the file descriptor passed into the memallc routines
 *
 *  Argument:
 *	extent			_ A VED for the extent being processed.
 *
 *  Returns:
 *	If successful, the newly reopened file descriptor.
 *	If not, -1.
 *
 *****/
PrivateFnDef int
    ReopenInputFD (extent)
VED extent;
{
/**** 
 * Allow read only access via this descriptor, but allow others to read or
 * write. (e.g. The mapped file)
 ****/
    return (InputFD (extent) = 
	    open(FName (extent),O_RDONLY,0,"shr = get","shr = put"));
}

/*****  Routine to map a file into a process's address space
 *
 *  Arguments:
 *	fd		- The address of a file descriptor identifying the
 *			  file to be mapped.  This descriptor will be closed
 *			  and reopened with options which make it compatible
 *			  with a mapped file.
 *			  If fd is less than 0, then only virtual address space
 *			  is allocated. No file mapping occurs. (This option 
 *			  will result in charges against the process's 
 *			  pagefilequota as the memory is actual used).
 *	offset		- The offset(in bytes) into the file where the mapping
 *			  is to start. (Currently ignored and offset is assumed
 *			  to be  zero.
 *	len		- The number of bytes to map.
 *	maxlen		- The total number of bytes that this mapping may grow
 *			  via memvary.
 *	mode		- Indicates whether read-only or read-write memory is
 *			  desired.
 *	type		- Indicates whether shared or private memory is
 *			  desired.
 *
 *  Returns:
 *	If successful, the starting address of the mapped file.
 *	If not, Nil.
 *
 *****/
PublicFnDef ADDRESS VMS_memallc(fn, offset, len, maxlen, type, mode)
char	*fn;
int	offset, len, maxlen, type, mode;
{
    char shrmemname[257];
    STATUS status;
    VED extent;
    int fd;
    struct stat filestats;

/****  make sure len and maxlen are consistent ****/
    if (len > maxlen || len <= 0)
    {
    	errno = EINVAL;
	return Nil;
    }

/****  Acquire the memory ****/
    if (IsNil (extent = GetVirtualAddressSpace (&MemallcPool, maxlen)))
	return Nil;

    if( fn == NULL )
/****  No file mapping requested  ****/
	return StartAddress (extent);

/****  File name must be VMS-style pathname not UNIX-style  ****/
    if (-1 == (fd = open(fn, O_RDONLY, 0, "shr = get, put")))
    	return Nil;
    getname (fd, FName (extent), 1);
    close(fd);

/****  Extend the file if it is less than len bytes ****/
    if (-1 == stat(FName (extent), &filestats))
        return Nil;

    if (len > filestats.st_size)
    {
	if (-1 == (fd = open(FName (extent), O_RDWR, 0, "shr = get, put")))
	    return Nil;
	if (-1 == lseek (fd, max (512, len - 1), 0))
	    return Nil;
/****
 * lseek on VMS doesn't physically extend the file if the current filesize is a
 * multiple of 512 and len is less than 512 greater than the current filesize.
 * Thus one byte is written to force the extension. (1/88)
 ****/
	if (-1 == write (fd, &0, 1))
	    return Nil;
	close (fd);
    }

/****  Fill in fields of the extent  ****/
    Mode (extent) = mode;
    Type (extent) = type;

    if (-1 == GetChannel (extent)) 
	return Nil;
/**** return the address of the newly mapped file  ****/
    return MapFile (extent, offset, len);
}

/*****  Utility to remove the mapping of a file to a virtual extent
 *
 *  Argument:
 *	extent			-  The extent to be freed
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PrivateFnDef int
    UnmapFile (extent)
VED extent;
{
    ADDRESS 
	inadr [2], retadr[2];
    STATUS
	status;
    char gsdstring[50];
    $LDESCRIPTOR(gsdnam, 49, gsdstring);

    if (Channel (extent) != -1)
    {
/***** Enter the address range to be reclaimed  *****/
	inadr[0] = StartAddress (extent);
	inadr[1] = EndAddress (extent);
/****
 *  Unmap the file by creating new virtual memory in the same address space.
 ****/
	HandleVMSRoutine
	     (sys$cretva (inadr,retadr,0), "sys$cretva", ;)
	if (inadr[0] != retadr[0]) 
	{
/****
 * VMS couldn't give us what we asked for. Our management of the memallc
 * pool isn't correct.
 ****/
	    fprintf 
	        (stderr, 
		 "Unrequested return address: %X : %X\n", 
		 inadr[0], retadr [0]);
	    return -1;
	}
/****  Close the channel to the file  ****/
	HandleVMSRoutine (sys$dassgn (Channel (extent)), "sys$dassgn", ;);

    }
    return 0;
}

/*****  Routine to free an address space allocated via a call to memallc
 *
 *  Argument:
 *	addr			-  The address of the memory to be returned
 *				   to the memallc pool.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int memfree(addr)
char *addr;
{
    VED
    	extent;
 
/**** Locate the address to be freed  *****/
    if (IsNil (extent = FindVirtualAddressInPool (&MemallcPool, addr)))
	return -1;

/**** Mark it as free  ****/
    FreeCount (&MemallcPool)++;
    IsFree (extent) = TRUE;
/**** and unmap the associated file  ****/
    if (-1 == UnmapFile (extent))
	return -1;
    return 0;
}

/*****  Utility to extend a file  
 *
 *  Arguments:
 *	extent			-  A VED for the extent whose associated file
 *				   is to be extended.
 *	len			-  The size the file must be grown to.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PrivateFnDef int
    GrowFile (extent, len)
VED extent;
int len;
{
    int
	outstandingFD,
	currentLen, 
	fd;
    char
	buffer [256];
    struct stat
	filestats;

/***** If the file is already big enough, return  *****/
    if (-1 == stat(FName (extent), &filestats))
    	return -1;
    currentLen = filestats.st_size;
    if (len <= currentLen)
        return 0;
/****  Do we need this really ?????	
    buffer [0] = '\0';
    getname (InputFD (extent), buffer, 1);
    outstandingFD =
	strcmp (FName (extent), buffer)
	? FALSE
	: (close (InputFD (extent)), TRUE);
****/
/**** open the associated file for writing  ****/
    if (-1 == (fd = open (FName (extent), O_RDWR, 0, "shr = get, put")))
	return -1;
/****
 * lseek doesn't physically extend the file if the current filesize is a
 * multiple of 512 and len is less than 512 greater than the current filesize.
 * Thus one byte is written to force the extension.
 ****/
    if (-1 == lseek (fd, len - 1, 0))
	return -1;
    if (-1 == write (fd, &0, 1))
	return -1;

/****  Close the file descriptor used to extend the file  ****/
    close (fd);
/****  Do we need this ?????????
    if (outstandingFD)
	return ReopenInputFD (extent);
****/
    return 0;
}

/****  Routine to extend a region allocated via memallc
 *
 *  Arguments:
 *	addr			-  The address in the memallc pool to extend.
 *	len			-  The size in bytes the region should now
 *				   occupy.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    memvary (addr, len)
ADDRESS addr;
int len;
{
    VED
    	extent;
    STATUS
	status;
    ADDRESS
	retadr[2];

/**** First, locate the extent identified by addr  ****/
    if (IsNil (extent = FindVirtualAddressInPool (&MemallcPool, addr)))
	return -1;
    
/****
 *  If the requested length is greater than the maximum allocated size
 *  then the len argument is invalid
 ****/
    if (len > EntrySize (extent))
    {
	errno = EINVAL;
	return -1;
    }
/**** If there is no associated file, nothing else need be done  ****/
    if (Channel (extent) == -1)
	return 0;

/**** Unmap the file ****/
    if (-1 == UnmapFile (extent))
	return -1;

/**** extend it if necessary  ****/
    if (-1 == GrowFile (extent, len))
    {
	perror ("Error extending file\n");
	return -1;
    }

/**** remap it ****/
    return 
	-1 == GetChannel (extent) ||
	IsNil (MapFile (extent,0,len))
	? -1
	: 0;
}
#endif

#ifdef apollo

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <apollo/base.h>
#include <apollo/error.h>
#include <apollo/ms.h>
#include <apollo/name.h>

#define	ARRAYINCR	256

PrivateVarDef ADDRESS		*MappedSegments = NULL;
PrivateVarDef unsigned long	*MappedLengths = NULL,
				*MappedMaxLengths = NULL,
				*MappedLenMapped = NULL;
PrivateVarDef int		*MappedExtend = NULL;
PrivateVarDef int		SegmentIndex = 0, CurrentMaxIndex = 0;;

PrivateFnDef int
checkArrayBounds()
{
	int	i;
	
	if( CurrentMaxIndex == 0 )
	{
		if( (int)(MappedSegments = 
			(ADDRESS *)malloc(ARRAYINCR * sizeof(ADDRESS)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLengths = 
			(unsigned long *)malloc(ARRAYINCR * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedMaxLengths = 
			(unsigned long *)malloc(ARRAYINCR * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLenMapped = 
			(unsigned long *)malloc(ARRAYINCR * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedExtend = 
			(int *)malloc(ARRAYINCR * sizeof(int)))
				== (int)NULL )
			return -1;
	
		for( i=CurrentMaxIndex ; i<CurrentMaxIndex+ARRAYINCR ; i++ )
		{
			MappedSegments[i] = (ADDRESS)0;
			MappedLengths[i] = 0L;
			MappedMaxLengths[i] = 0L;
			MappedLenMapped[i] = 0L;
			MappedExtend[i] = FALSE;
		}
		
		CurrentMaxIndex = ARRAYINCR;
		return 0;
	}
	
	if( CurrentMaxIndex == SegmentIndex )
	{
		CurrentMaxIndex += ARRAYINCR;
		if( (int)(MappedSegments = 
			(ADDRESS *)realloc(MappedSegments,CurrentMaxIndex * sizeof(ADDRESS)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLengths = 
			(unsigned long *)realloc(MappedLengths,CurrentMaxIndex * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedMaxLengths = 
			(unsigned long *)realloc(MappedMaxLengths,CurrentMaxIndex * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLenMapped = 
			(unsigned long *)realloc(MappedLenMapped,CurrentMaxIndex * sizeof(unsigned long)))
				== (int)NULL )
			return -1;
		if( (int)(MappedExtend = 
			(int *)realloc(MappedExtend,CurrentMaxIndex * sizeof(int)))
				== (int)NULL )
			return -1;

		for( i=CurrentMaxIndex-ARRAYINCR ; i<CurrentMaxIndex ; i++ )
		{
			MappedSegments[i] = (ADDRESS)0;
			MappedLengths[i] = 0L;
			MappedMaxLengths[i] = 0L;
			MappedLenMapped[i] = 0L;
			MappedExtend[i] = FALSE;
		}
	}
	return 0;
}
		
PublicFnDef ADDRESS
APO_memallc(fn,offset,len,maxlen,type,mode)
char	*fn;
int	offset, len, maxlen, type, mode;
{
	ADDRESS			ptr;
	name_$pname_t		pathname;
	short			namelength;
	status_$t		status;
	unsigned long		len_mapped;
	ms_$conc_mode_t		concurrencymode;
	ms_$acc_mode_t		accessmode;
	unsigned char		extension;
	struct stat		filestats;

	if( type & MEM_SHARED )
		concurrencymode = ms_$cowriters;
	else
		concurrencymode = ms_$nr_xor_1w;

	if( fn == NULL )
	{
		strcpy(pathname, "");
		namelength = 0;

		ptr = (ADDRESS)ms_$crtemp(pathname,
					  namelength,
					  (unsigned long)offset,
					  (unsigned long)maxlen,
					  concurrencymode,
					  &status );
		
		if( status.all != status_$ok )
		{
			error_$print(status);
			return Nil;
		}

		if( checkArrayBounds() == -1 )
			return Nil;
		MappedMaxLengths[SegmentIndex] = (unsigned long)maxlen;
		MappedLengths[SegmentIndex] = (unsigned long)len;
		MappedLenMapped[SegmentIndex] = (unsigned long)maxlen;
		MappedExtend[SegmentIndex] = TRUE;
		MappedSegments[SegmentIndex++] = ptr;

		return ptr;
	}
	
	strcpy(pathname, fn);
	namelength = (short)strlen(pathname);
	
	if( mode & MEM_W )
	{
		if( mode & MEM_X )
			accessmode = ms_$wrx;
		else
			accessmode = ms_$wr;
	}
	else
	{
		if( mode & MEM_X )
			accessmode = ms_$rx;
		else
			accessmode = ms_$r;
	}

	if( -1 == stat(fn, &filestats) )
		return Nil;
			
	if( (mode & MEM_W) && ((maxlen > len) || (len > filestats.st_size)) )
		extension = true;
	else
		extension = false;

	ptr = (ADDRESS)ms_$mapl(pathname,
				namelength,
				(unsigned long)offset,
				(unsigned long)maxlen,
				concurrencymode,
				accessmode,
				extension,
				&len_mapped,
				&status );
	if (status_$ok == status.all)
	{
	}
	else if (0x03060005 == status.all & 0x7fffffff)
						/*  File too big to map  */
	{
	    errno = EFBIG;
	    return Nil;
	}
	else if (0x800f0009 == status.all)	/*  Lock table full  */
	{
	    errno = EMFILE;
	    return Nil;
	}
	else {
	    fprintf (stderr, "Error Mapping File: %08x: ", status.all);
	    error_$print(status);
	    return Nil;
	}
	
	if( checkArrayBounds() == -1 )
		return Nil;
	MappedMaxLengths[SegmentIndex] = (unsigned long)maxlen;
	MappedLengths[SegmentIndex] = (unsigned long)len;
	MappedLenMapped[SegmentIndex] = len_mapped;
	if( extension == true )
		MappedExtend[SegmentIndex] = TRUE;
	MappedSegments[SegmentIndex++] = ptr;

	return ptr;
}

int
FindMappedIndex(addr)
ADDRESS	addr;
{
	int	i;
	
	for( i=0 ; i<SegmentIndex ; i++ )
		if( (unsigned long)addr == (unsigned long)MappedSegments[i] )
			return i;
			
	return -1;
}

int
memfree(addr)
ADDRESS	addr;
{
	status_$t	status;
	int		idx;
	
	if( -1 == (idx = FindMappedIndex(addr)) )
		return -1;

	if( MappedExtend[idx] )
	{
		ms_$truncate( addr, MappedLengths[idx], &status );

		if( status.all != status_$ok )
		{
			error_$print(status);
			return -1;
		}
	}
	
	ms_$unmap( addr, MappedLengths[idx], &status );
	
	if( status.all != status_$ok )
	{
		error_$print(status);
		return -1;
	}
	
	MappedSegments[idx] = (ADDRESS)0;
	
	return 0;
}

int
memvary(addr, len)
ADDRESS	addr;
int	len;
{
	int	idx;

	if( -1 == (idx = FindMappedIndex(addr)) )
		return -1;
	
	if( (unsigned long)len > MappedLenMapped[idx] )
		return -2;
		
	if( (unsigned long)len <= MappedLengths[idx] )
		return 0;
		
	MappedLengths[idx] = (unsigned long)len;
	
	return 0;
}

#endif

#ifdef sun
#include <fcntl.h>

#ifdef lint
#define malloc(x)	0
#else
PublicFnDecl ADDRESS	malloc();
#endif

#define	ARRAYINCR	256

PrivateVarDef ADDRESS 	*MappedSegments = NULL;
PrivateVarDef int 	*MappedLengths = NULL;
PrivateVarDef int 	*MappedIsTmp = NULL;
PrivateVarDef int 	SegmentIndex = 0, CurrentMaxIndex = 0;
PublicVarDef int	errinfo;

PrivateFnDef int
checkArrayBounds()
{
	int	i;
	
	if( CurrentMaxIndex == 0 )
	{
		if( (int)(MappedSegments = 
			(ADDRESS *)malloc(ARRAYINCR * sizeof(ADDRESS)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLengths = 
			(int *)malloc(ARRAYINCR * sizeof(int)))
				== (int)NULL )
			return -1;
		if( (int)(MappedIsTmp = 
			(int *)malloc(ARRAYINCR * sizeof(int)))
				== (int)NULL )
			return -1;
	
		for( i=CurrentMaxIndex ; i<CurrentMaxIndex+ARRAYINCR ; i++ )
		{
			MappedSegments[i] = (ADDRESS)0;
			MappedLengths[i] = 0;
			MappedIsTmp[i] = FALSE;
		}
		
		CurrentMaxIndex = ARRAYINCR;
		return 0;
	}
	
	if( CurrentMaxIndex == SegmentIndex )
	{
		CurrentMaxIndex += ARRAYINCR;
		if( (int)(MappedSegments = 
			(ADDRESS *)realloc(MappedSegments,CurrentMaxIndex * sizeof(ADDRESS)))
				== (int)NULL )
			return -1;
		if( (int)(MappedLengths = 
			(int *)realloc(MappedLengths,CurrentMaxIndex * sizeof(int)))
				== (int)NULL )
			return -1;
		if( (int)(MappedIsTmp = 
			(int *)realloc(MappedIsTmp,CurrentMaxIndex * sizeof(int)))
				== (int)NULL )
			return -1;

		for( i=CurrentMaxIndex-ARRAYINCR ; i<CurrentMaxIndex ; i++ )
		{
			MappedSegments[i] = (ADDRESS)0;
			MappedLengths[i] = 0;
			MappedIsTmp[i] = FALSE;
		}
	}
	return 0;
}
		
PublicFnDef ADDRESS
SUN_memallc(fn, offset, len, maxlen, type, mode)
char	*fn;
int	offset, len, maxlen, type, mode;
{
	int	fd, amode, omode;
	ADDRESS	ptr;
	int	terrno, terrinfo;
	
	if( fn == NULL )
	{
		if( (ptr = malloc(maxlen)) == NULL )
			return Nil;
		if( checkArrayBounds() == -1 )
			return Nil;
		MappedIsTmp[SegmentIndex] = TRUE;
		MappedLengths[SegmentIndex] = maxlen;
		MappedSegments[SegmentIndex++] = ptr;
		return ptr;
	}

	if( mode & MEM_W )
	{
		amode = 02 | 04;
		omode = O_RDWR;
		mode |= MEM_R;
	}
	else
	{
		amode = 04;
		omode = O_RDONLY;
	}
	
	if( access(fn,amode) < 0 )
		return Nil;

	if( -1 == (fd = open(fn,omode,0)) )
	{
		perror("open");
		return Nil;
	}

	ptr = (ADDRESS)mmap(0, maxlen, mode, type, fd,  offset);

	terrno = errno;
	terrinfo = errinfo;

	close(fd);

	errno = terrno;
	errinfo = terrinfo;

	if( (int)ptr != -1 )
	{
		if( checkArrayBounds() == -1 )
			return Nil;
		MappedSegments[SegmentIndex] = ptr;
		MappedIsTmp[SegmentIndex] = FALSE;
		MappedLengths[SegmentIndex++] = maxlen;
	}

	return ptr;
}

int
memfree(addr)
ADDRESS	addr;
{
	int	i;
	for( i=0 ; i<SegmentIndex ; i++ )
		if( MappedSegments[i] == addr )
		{
			if( MappedIsTmp[i] )
				return(free(addr));
			else
				return(munmap(addr,MappedLengths[i]));
		}
	return -1;
}

int
memvary(addr, len)
ADDRESS	addr;
int	len;
{
	int	i;
	for( i=0 ; i<SegmentIndex ; i++ )
		if( MappedSegments[i] == addr )
		{
			if( MappedIsTmp[i] )
			{
				if( len <= MappedLengths[i] )
					return 0;
			}
			else
				return -1;
		}
	return -1;
}
#endif /* sun */

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSmem.c 1 replace /users/jck/system
  880330 21:53:39 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSmem.c 2 replace /users/jck/system
  880407 10:46:58 jck Cleaned Up VAX interface

 ************************************************************************/
/************************************************************************
  MSmem.c 3 replace /users/m2/dbup
  880421 17:19:55 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  MSmem.c 4 replace /users/m2/backend
  890503 14:23:38 m2 VAX-Apollo fixes

 ************************************************************************/
/************************************************************************
  MSmem.c 5 replace /users/jck/system
  890714 15:10:20 jck removed call to sys in VMS implementation of memfree

 ************************************************************************/
/************************************************************************
  MSmem.c 6 replace /users/m2/backend
  890927 14:32:34 m2 SUN port (preliminary)

 ************************************************************************/

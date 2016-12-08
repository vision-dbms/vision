/*****  Machine Specific File System Services  *****/

#include <stdio.h>

#include "stdoss.h"
#include "MSsupport.h"

#ifdef VMS			    /*****  VAX/VMS  *****/
#include "SIfile.h"
#include "SIutil.h"
#include <lnmdef.h>
#include <psldef.h>
#include <ssdef.h>
#endif

#if defined(hp9000s500) || defined(apollo) || defined(sun)

/* For Translating Path names (only valid on VMS) */
#define UNIXPathFormat	0
#define VMSPathFormat	1

PublicFnDef int
STD_getFullPathName(odir,ndir,formatType)
char	*odir, *ndir;
int	formatType;
{
    if (IsNil (ndir))
	return -1;

    if (IsNil (odir))
    {
	*ndir = '\0';
	return -1;
    }

    strcpy(ndir,odir);
    return 0;
}
#endif

/*******************
 *  Read and Write *
 *******************/

#ifdef VMS			    /*****  VAX/VMS  *****/

/***** In SIfile.h, write is defined as STD_write but here *****/
/***** we want the real system routine.                    *****/
#ifdef write
#undef write
#endif

PublicFnDef int
    STD_write (fd, buffer, bytesToWrite)
int fd, bytesToWrite;
char *buffer;
{
    int bytes, bytesWritten, result;
    for (bytes = bytesToWrite, 
         bytesWritten = 0; 

	 bytes > 65535; 

	 bytes -= 65535, 
	 bytesWritten += result, 
	 buffer += result)
    {
        if ((result = write (fd, buffer, 65535)) < 0) return result;
    }
    
    if ((result = write (fd, buffer, bytes)) < 0) return result;
    return bytesWritten + result;
}

/***** In SIfile.h, read is defined as STD_read but here *****/
/***** we want the real system routine.                  *****/
#ifdef read
#undef read
#endif

PublicFnDef int
    STD_read (fd, buffer, bytesToRead)
int fd, bytesToRead;
char *buffer;
{
    int bytes, bytesRead, result;
    for (bytes = bytesToRead, 
         bytesRead = 0; 

	 bytes > 65535; 

	 bytes -= 65535, 
	 bytesRead += result, 
	 buffer += result)
    {
        if ((result = read (fd, buffer, 65535)) < 0) return result;
    }
    
    if ((result = read (fd, buffer, bytes)) < 0) return result;
    return bytesRead + result;
}

/********************************
 *  File Manipulation Utilities *
 ********************************/

PublicFnDef int
VMS_ftruncate(fd,fsize)
int *fd, fsize;
{
    char vmsfilename[257], tbuf[512];
    STATUS status;
    struct FAB fab;
    struct RAB rab;
    int npages, skipTruncationStep = FALSE;

    if( *fd > 0 )
    {
	getname (*fd, vmsfilename, 1);
	close(*fd);
	
	fab = cc$rms_fab;
	fab.fab$l_fna = vmsfilename;
	fab.fab$b_fns = strlen (vmsfilename);
	fab.fab$l_fop = FAB$M_TEF;
	fab.fab$b_shr = FAB$M_NIL;
	fab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_BRO | FAB$M_TRN;
	HandleVMSRoutine
	    (sys$open (&fab), "sys$open", ;);
	rab = cc$rms_rab;
	rab.rab$l_fab = &fab;
	rab.rab$l_bkt = 0;
	
	HandleVMSRoutine
	     (sys$connect (&rab), "sys$connect",;);

	rab.rab$l_rop |= RAB$M_BIO;
	rab.rab$l_bkt = (fsize / 512) + 1;
	SpecialHandleVMSRoutine 
	    (status = sys$space(&rab), !EndOfFile, "sys$space",;, -1);
	if (Failure (status))
	    skipTruncationStep = TRUE;

	rab.rab$l_rop &= ~RAB$M_BIO;
	rab.rab$l_rop |= RAB$M_NXR;
	SpecialHandleVMSRoutine 
	     (status = skipTruncationStep || sys$find(&rab), 
	      !EndOfFile,
	      "sys$find",
	      ;,
	      -1);
	if (Failure (status))
	    skipTruncationStep = TRUE;
	rab.rab$l_rop &= ~RAB$M_NXR;
	HandleVMSRoutine 
	    (skipTruncationStep || sys$truncate(&rab), "sys$truncate", ;);
	HandleVMSRoutine 
	    (sys$close(&fab), "sys$close", ;);
	if( (*fd = open(vmsfilename,O_RDONLY,0)) == -1 )
	{
	    fprintf (stderr, "Couldn't reopen file descriptor, %s\n",vmsfilename);
	}
    }
    return( 0 );
}
#endif

/**************************
 *  NDF function support  *
 **************************/

/*****  
 *  The following definition is meant for use ONLY by the NDF... macros in 
 *  "SIfile.h".
 *****/
PublicVarDef char *_NetworkDirectory;

/**************************
 *  Directory Management  *
 **************************/
#ifdef VMS

PrivateFnDef int
    FilenameToVMSStandard (filename, vmsfilename)
char *filename, *vmsfilename;
{
    int fd;
    char *semicolon;

    fd = creat (filename, 0600);
    if (fd < 0)
	return -1;

    getname (fd, vmsfilename, 1);
    semicolon = strrchr (vmsfilename, ';');
    if (IsntNil (semicolon))
	*semicolon = '\0';

    close (fd);
    return delete (filename);
}

PublicFnDef int
    link (pathName1, pathName2)
char *pathName1, *pathName2;
{
    char buffer [256], vmsName1 [256], vmsName2 [256];
    STATUS status;
    
    if (FilenameToVMSStandard (pathName1, vmsName1) < 0 ||
	FilenameToVMSStandard (pathName2, vmsName2) < 0)
	    return -1;

    return rename (vmsName1, vmsName2);
}

PublicFnDef int
    unlink (filename)
char *filename;
{
    return 0;
}


PrivateFnDef int
add000000ToFullPathName(dir1,dir2)
char	*dir1, *dir2;
{
	char	*ptr1, *ptr2, *d2;
	int	len;
	
	ptr2 = dir1;
	d2 = dir2;
	if( (ptr1 = strchr(ptr2,'[')) == NULL )
	{
		strcpy(dir2,dir1);
		return(0);
	}
	ptr1++;
	len = ptr1 - ptr2;
	strncpy(d2,ptr2,len);
	d2 += len;
	*d2 = '\0';
	ptr2 = ptr1;
	if( strncmp(ptr1,"000000",6) != 0 )
	{
		strcat(d2,"000000.");
		d2 += 7;
	}
	while( (ptr1 = strchr(ptr2,']')) != NULL )
	{
		ptr1++;
		if( *ptr1 != '[' )
			break;
		len = (ptr1 - ptr2) - 1;
		strncpy(d2,ptr2,len);
		d2 += len;
		*d2 = '\0';
		ptr2 = ptr1 + 1;
	}	
	strcat(d2,ptr2);
	return(0);
}

	
struct _itemlist {
	unsigned long	namelen : 16, code : 16;
	char		*buf;
	unsigned short	*retlen;
};

PrivateVarDef struct _itemlist	itemlist[2];
PrivateVarDef unsigned short	retlen;
PrivateVarDef char		transLogName[257], logTableName[32];

PrivateFnDef int
transLogicalName(oname,nname)
char	*oname, *nname;
{
	char		lname[257], dir[257], *dp, *p, *op, tname[257];
	int		istatus, attr = LNM$M_CASE_BLIND, len;
	unsigned char	accmode = PSL$C_USER;
	$DESCRIPTOR(logname,lname);
	$DESCRIPTOR(tabname,logTableName);
	
	itemlist[0].namelen = 256;
	itemlist[0].code = LNM$_STRING;
	itemlist[0].buf = transLogName;
	itemlist[0].retlen = &retlen;
	itemlist[1].namelen = 0;
	itemlist[1].code = 0;
	itemlist[1].buf = NULL;
	itemlist[1].retlen = NULL;
	strcpy(logTableName,"LNM$FILE_DEV");

	transLogName[0] = '\0';
	retlen = 0;

	strcpy(lname,oname);
	if( (p = strrchr(lname,':')) != NULL )
		*p = '\0';
	$LENGTH(logname) = strlen(lname);
	$LENGTH(tabname) = strlen(logTableName);
	istatus = sys$trnlnm( &attr,
			      &tabname,
			      &logname,
			      &accmode,
			      itemlist );
	if( istatus != SS$_NORMAL )
	{
		if( istatus != SS$_NOLOGNAM )
		{
			vaxc$errno = istatus;
			errno = EVMSERR;
			perror("TRNLNM");
		}
		strcpy(nname,oname);
		return(0);
	}

	transLogName[retlen] = '\0';
	strcpy(nname,transLogName);
	return(0);
}

PrivateVarDef STD_DirectoryEntryType	dirEntry;

PublicFnDef int
STD_getFullPathName(oname,nname,formatType)
char	*oname, *nname;
int	formatType;
{
	char 		*vmsdirname, *ocwd, tmp[MAXNAMLEN+1], tmp2[MAXNAMLEN+1];
	char		ocwdbuf[257], vmsdirnamebuf[257];
	int		retval;
	STATUS		status;
	struct FAB	*fab;
	struct NAM	*nam;

	if( (oname == NULL) || (nname == NULL) )
	{
		if( nname != NULL )
			*nname = '\0';
		return(-1);
	}
	if( (ocwd = getcwd(ocwdbuf,257,VMSPathFormat)) == NULL )
	{
		*nname = '\0';
		return(-1);
	}
	if( chdir(oname) == -1 )
	{
		*nname = '\0';
		return(-1);
	}
	if( (vmsdirname = getcwd(vmsdirnamebuf,257,VMSPathFormat)) == NULL )
	{
		chdir(ocwd);
		*nname = '\0';
		return(-1);
	}
	
	if( (fab = (struct FAB *)malloc(sizeof(struct FAB))) == NULL )
	{
		*nname = '\0';
		chdir(ocwd);
		return(-1);
	}
	*fab = cc$rms_fab;
	fab->fab$l_fna = vmsdirname;
	fab->fab$b_fns = strlen (vmsdirname);

	if( (nam = (struct NAM *)malloc(sizeof(struct NAM))) == NULL )
	{
		free(fab);
		*nname = '\0';
		chdir(ocwd);
		return(-1);
	}
	*nam = cc$rms_nam;
	fab->fab$l_nam = nam;
	if( (nam->nam$l_esa = malloc(MAXNAMLEN+1)) == NULL )
	{
		free(nam);
		free(fab);
		*nname = '\0';
		chdir(ocwd);
		return(-1);
	}
	nam->nam$b_ess = MAXNAMLEN;	
	
	nam->nam$b_nop = NAM$V_NOCONCEAL | NAM$V_SYNCHK;

	if( (nam->nam$l_rsa = malloc(MAXNAMLEN+1)) == NULL )
	{
		free(nam->nam$l_esa);
		free(nam);
		free(fab);
		*nname = '\0';
		chdir(ocwd);
		return(-1);
	}
	nam->nam$b_rss = MAXNAMLEN;	

	status = sys$parse(fab);

	if( Failure(status) )
	{
		vaxc$errno = status;
		errno = EVMSERR;
		perror ("sys$parse");
		*nname = '\0';
		retval = -1;
	}
	else if( (nam->nam$b_rsl == 0) && (nam->nam$b_esl == 0) )
	{
		*nname = '\0';
		retval = -1;
	}
	else		
	{
		strncpy( nname,
			 nam->nam$l_node,
			 nam->nam$b_node );
		nname[nam->nam$b_node] = '\0';
		strncpy( tmp,
			 nam->nam$l_dev,
			 nam->nam$b_dev );
		tmp[nam->nam$b_dev] = '\0';
		tmp2[0] = '\0';
		transLogicalName(tmp,tmp2);
		strncpy( tmp,
			 nam->nam$l_dir,
			 nam->nam$b_dir );
		tmp[nam->nam$b_dir] = '\0';
		strcat(tmp2,tmp);
		tmp[0] = '\0';
		add000000ToFullPathName(tmp2,tmp);
		strcat(nname,tmp);
		retval = 0;
	}

	free(nam->nam$l_rsa);
	free(nam->nam$l_esa);
	free(nam);
	free(fab);

	if( formatType == UNIXPathFormat )
	{
		if( chdir(nname) == -1 )
		{
			*nname = '\0';
			retval = -1;
		}
		else if( (vmsdirname = getcwd(vmsdirnamebuf,257,formatType)) == NULL )
		{
			*nname = '\0';
			retval = -1;
		}
		else
			strcpy(nname,vmsdirname);
	}

	chdir(ocwd);
	return(retval);
}

PublicFnDef void
closedir(dir)
DIR	*dir;
{
	struct FAB	*fab;
	struct NAM	*nam;
	
	if( dir == NULL )
		return;
		
	fab = dir->fab;
	if( fab != NULL )
	{
		nam = dir->fab->fab$l_nam;
		if( nam != NULL )
		{
			if( nam->nam$l_esa != NULL )
				free(nam->nam$l_esa);
			if( nam->nam$l_rsa != NULL )
				free(nam->nam$l_rsa);
			free(nam);
		}
		
		if( fab->fab$l_fna != NULL )
			free(fab->fab$l_fna);
		free(fab);
	}

	free(dir);
}

PublicFnDef DIR *
opendir(dirname)
char	*dirname;
{
	char 		*vmsdirname;
	STATUS		status;
	struct FAB	*fab;
	struct NAM	*nam;
	DIR		*dir;

	if( (vmsdirname = malloc(MAXNAMLEN+1)) == NULL )
		return(NULL);

	if( STD_getFullPathName(dirname,vmsdirname,VMSPathFormat) == -1 )
	{
		free(vmsdirname);
		return(NULL);
	}
    
	/***** Add wildcard characters.  We assume we want to	*****/
	/***** see everything in the given directory.		*****/
	/***** (But only the most recent version)		*****/
	
	strcat(vmsdirname,"*.*;");
    
	if( (fab = (struct FAB *)malloc(sizeof(struct FAB))) == NULL )
	{
		free(vmsdirname);
		return(NULL);
	}
	*fab = cc$rms_fab;
	fab->fab$l_fna = vmsdirname;
	fab->fab$b_fns = strlen (vmsdirname);

	if( (nam = (struct NAM *)malloc(sizeof(struct NAM))) == NULL )
	{
		free(fab);
		free(vmsdirname);
		return(NULL);
	}
	*nam = cc$rms_nam;
	fab->fab$l_nam = nam;
	if( (nam->nam$l_esa = malloc(MAXNAMLEN+1)) == NULL )
	{
		free(nam);
		free(fab);
		free(vmsdirname);
		return(NULL);
	}
	nam->nam$b_ess = MAXNAMLEN;	
	if( (nam->nam$l_rsa = malloc(MAXNAMLEN+1)) == NULL )
	{
		free(nam->nam$l_esa);
		free(nam);
		free(fab);
		free(vmsdirname);
		return(NULL);
	}
	nam->nam$b_rss = MAXNAMLEN;	

	nam->nam$b_nop = NAM$V_NOCONCEAL;

	if( (dir = (DIR *)malloc(sizeof(DIR))) == NULL )
	{
		free(nam->nam$l_rsa);
		free(nam->nam$l_esa);
		free(nam);
		free(fab);
		free(vmsdirname);
		return(NULL);
	}
	dir->fab = fab;

	status = sys$parse(fab);

	if( Failure(status) )
	{
		vaxc$errno = status;
		errno = EVMSERR;
		perror ("sys$parse");
		/***** Free the malloc'ed stuff with closedir	*****/
		closedir(dir);
		return(NULL);
	}

	return(dir);
}

PublicFnDef STD_DirectoryEntryType *
readdir(dir)
DIR	*dir;
{
	STATUS	status;
	char	vmsfilename[MAXNAMLEN+1], ext[MAXNAMLEN+1];

	status = sys$search(dir->fab);
	
	/***** Don't print an error if No-More-Files or 	*****/
	/***** File-Not-Found.					*****/
	
	if( Failure(status) && ((status == RMS$_NMF) || (status == RMS$_FNF)) )
		return(NULL);

	if( Failure(status) && !((status == RMS$_NMF) || (status == RMS$_FNF)) )
	{
		vaxc$errno = status;
		errno = EVMSERR;
		perror ("sys$search");
		return(NULL);
	}

	strncpy( vmsfilename,
		 dir->fab->fab$l_nam->nam$l_name,
		 dir->fab->fab$l_nam->nam$b_name );
	vmsfilename[dir->fab->fab$l_nam->nam$b_name] = '\0';

	/***** Strip off the ".DIR" extension for directory files *****/
	strncpy( ext,
		 dir->fab->fab$l_nam->nam$l_type,
		 dir->fab->fab$l_nam->nam$b_type );
	ext[dir->fab->fab$l_nam->nam$b_type] = '\0';

	if( (strcmp(ext,".DIR") == 0) ||
	    (strcmp(ext,".dir") == 0) )
		STD_DirectoryEntryIsDir(&dirEntry) = TRUE;
	else if( strcmp(ext,".") == 0 )
		STD_DirectoryEntryIsDir(&dirEntry) = FALSE;
	else
	{
		strcat(vmsfilename,ext);
		STD_DirectoryEntryIsDir(&dirEntry) = FALSE;
	}
	
	strcpy(STD_DirectoryEntryName(&dirEntry), vmsfilename);
	STD_DirectoryEntryNameLen(&dirEntry) = strlen(vmsfilename);

	return(&dirEntry);
}
#endif /* vax  */

#ifdef apollo			/*****  DOMAIN\IX  *****/

#include "SIfile.h"

#include <apollo/base.h>
#include <apollo/error.h>
#include <apollo/type_uids.h>
#include <apollo/ios.h>

PublicFnDef void APO_ForceWriteFile (
    SEG_fdTypePtr		fd
)
{
    ios_$id_t	id;
    status_$t	status;

    id = fd;
    ios_$force_write_file (id, &status);
    if (status.all != status_$ok)
	error_$print (status);
}

#endif /* apollo */

#ifdef sun			/*****  SUN-OS  *****/

PublicFnDef void NDF_Synchronize (fd)
int fd;
{
}

PublicFnDef void SEG_Synchronize (fd)
SEG_fdTypePtr fd;
{
}

#endif /* sun */


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSfile.c 1 replace /users/jck/system
  880330 21:51:29 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSfile.c 3 replace /users/jck/system
  880407 09:28:44 jck Modified VAXimplementation of 'link()'

 ************************************************************************/
/************************************************************************
  MSfile.c 4 replace /users/m2/dbup
  880421 17:05:27 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  MSfile.c 5 replace /users/m2/backend
  890503 14:19:11 m2 VAX-Apollo fixes

 ************************************************************************/
/************************************************************************
  MSfile.c 6 replace /users/jck/system
  890714 15:04:00 jck Support for VMS handling of the NDF

 ************************************************************************/
/************************************************************************
  MSfile.c 7 replace /users/m2/backend
  890927 14:27:26 m2 SUN port (preliminary)

 ************************************************************************/

/*****  Standard Interface to File System Services  *****/
#ifndef siFILE_H
#define siFILE_H

/**************************
 *****   File Access  *****
 **************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/
PublicFnDecl int
    open	(),
    read	(),
    write	(),
    dup		(),
    ftruncate	(),
    link	(),	/***** These two are used in combination as   *****/
    unlink	();	/*****		a rename function	      *****/
PublicFnDecl ADDRESS
    getcwd	();

PublicFnDecl int
    STD_getFullPathName();
    
/* Symbolic constants for the "lseek" function: */
#define SEEK_SET	0	/* Set file pointer to "offset" */
#define	SEEK_CUR	1	/* Set file pointer to current plus "offset" */
#define	SEEK_END	2	/* Set file pointer to EOF plus "offset" */

/* For Translating Path names */
#define UNIXPathFormat	0
#define VMSPathFormat	1

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ndir.h>

typedef struct direct
    STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)\
	(dirent->d_name)

#define STD_DirectoryEntryNameLen(dirent)\
	(dirent->d_namlen)

#define delete			unlink
#define STD_write		write
#define STD_read		read

#define SEG_fdTypePtr		int
#define SEG_fdType		int
#define SEG_write		write
#define SEG_read		read
#define SEG_close		close
#define SEG_creat		creat
#define SEG_open		open
#define SEG_fstat		fstat
#define SEG_fsync(fd)		((fd), 0)
#define SEG_lseek		lseek
#define SEG_ftruncate		ftruncate

#define NDF_Refresh(fd)		(fd)
#define NDF_EnableUpdate(fd)	(fd)
#define NDF_creat		creat
#define NDF_open		open
#define NDF_close		close
#define NDF_fstat		fstat
#define NDF_fsync(fd)		((fd), 0)
#define NDF_lseek		lseek
#define NDF_read		read
#define NDF_write		write

#define STD_fputs		fputs
#endif /* hp */

#ifdef VMS			/*****  VAX/VMS  *****/
#include file
#include stat
#include rms
#include stsdef

#define DIRBLKSIZ	1024
#define	MAXNAMLEN	255

struct	direct {
	unsigned long	d_ino;		/* inode number of entry */
	unsigned short	d_reclen;	/* length of this record */
	unsigned short	d_namlen;	/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
	int		isdir;		/* Need flag to signify ".DIR" */
};


typedef struct direct
    STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)\
	((dirent)->d_name)

#define STD_DirectoryEntryNameLen(dirent)\
	((dirent)->d_namlen)

#define STD_DirectoryEntryIsDir(dirent)\
	((dirent)->isdir)

typedef struct _dirdesc {
	struct FAB	*fab;
} DIR;

PublicFnDecl void			closedir();
PublicFnDecl DIR			*opendir();
PublicFnDecl STD_DirectoryEntryType	*readdir();

PublicFnDecl int 
    STD_write	    (), 
    STD_read	    (), 
    VMS_ftruncate   ();

#define write			STD_write
#define read			STD_read
#define	ftruncate(fd, fsize)	VMS_ftruncate (&fd, fsize)

#define SEG_fdTypePtr		int
#define SEG_fdType		int
#define SEG_write		STD_write
#define SEG_read		STD_read
#define SEG_close		close
#define SEG_creat		creat
#define SEG_open		open
#define SEG_fstat		fstat
#define SEG_fsync(fd)		((fd), 0)
#define SEG_lseek		lseek
#define SEG_ftruncate		ftruncate

/*******************
 *  NDF interface  *
 *******************/

PublicVarDecl char *_NetworkDirectory;

#define NDF_Refresh(fd)	(\
    close (fd), (fd) = open (_NetworkDirectory, O_RDONLY, 0, "shr=put,upd")\
)
#define NDF_EnableUpdate(fd) (\
    close (fd), (fd) = open (_NetworkDirectory, O_RDWR, 0, "shr=put,upd")\
)
#define NDF_creat		creat
#define NDF_open(path,flags)	open (\
    _NetworkDirectory = (path), O_RDONLY, 0, "shr=put,upd"\
)
#define NDF_close		close
#define NDF_fstat		fstat
#define NDF_fsync(fd)		((fd), 0)
#define NDF_lseek		lseek
#define NDF_read		STD_read
#define NDF_write		STD_write

#define FPUTSBLOCKSIZE	512
#define FPUTSNUMBLOCKS	127
#define FPUTSMAXWRITE	(FPUTSBLOCKSIZE * FPUTSNUMBLOCKS)

#define STD_fputs(s,f)\
{\
    char *ptr = s;\
    while( fputs(ptr,f) == EOF )\
    {\
    	if( errno != EINVAL )\
    		break;\
    	fwrite(ptr,FPUTSBLOCKSIZE,FPUTSNUMBLOCKS,f);\
    	ptr += FPUTSMAXWRITE;\
    }\
}
#endif /* VMS */

#ifdef apollo			/*****  DOMAIN\IX  *****/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fclient.h"

#ifdef FRONTEND
#include <sys/dir.h>

typedef struct direct
    STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)\
	(dirent->d_name)

#define STD_DirectoryEntryNameLen(dirent)\
	(dirent->d_namlen)

#else /* FRONTEND */
#include <dirent.h>

typedef struct dirent
    STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)\
	(dirent->d_name)

#define STD_DirectoryEntryNameLen(dirent)\
	(dirent->d_namlen)

#endif /* FRONTEND */

#define delete			unlink
#define STD_write		write
#define STD_read		read

#define SEG_fdTypePtr		int
#define SEG_fdType		int
#define SEG_write		write
#define SEG_read		read
#define SEG_close		close
#define SEG_creat		creat
#define SEG_open		open
#define SEG_fstat		fstat
#define SEG_fsync		APO_ForceWriteFile
#define SEG_lseek		lseek
#define SEG_ftruncate		ftruncate

#define	prealloc(fd,len)	lseek(fd,(long)len,0)

#define NDF_Refresh(fd)		(fd)
#define NDF_EnableUpdate(fd)	(fd)
#define NDF_creat		FCLIENT_creat
#define NDF_open(path,flags)	FCLIENT_open (path, flags, 0)
#define NDF_close		FCLIENT_close
#define NDF_fstat		FCLIENT_fstat
#define NDF_fsync		FCLIENT_fsync
#define NDF_lseek		FCLIENT_lseek
#define NDF_read		FCLIENT_read
#define NDF_write		FCLIENT_write

#define STD_fputs		fputs
#endif /* apollo */

#ifdef sun		/*****  Sun  *****/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct dirent
    STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)\
	((dirent)->d_name)

#define STD_DirectoryEntryNameLen(dirent)\
	((dirent)->d_namlen)

#define delete			unlink
#define STD_write		write
#define STD_read		read

#define SEG_fdTypePtr		int
#define SEG_fdType		int
#define SEG_write		write
#define SEG_read		read
#define SEG_close		close
#define SEG_creat		creat
#define SEG_open		open
#define SEG_fstat		fstat
#define SEG_fsync(fd)		((fd), 0)
#define SEG_lseek		lseek
#define SEG_ftruncate		ftruncate

#define NDF_Refresh(fd)		(fd)
#define NDF_EnableUpdate(fd)	(fd)
#define NDF_creat		creat
#define NDF_open		open
#define NDF_close		close
#define NDF_fstat		fstat
#define NDF_fsync(fd)		((fd), 0)
#define NDF_lseek		lseek
#define NDF_read		read
#define NDF_write		write

#define STD_fputs		fputs
#endif /* sun */

#endif /*****  siFILE_H  *****/


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SIfile.h 1 replace /users/jck/system
  880330 21:55:06 jck SIutil.h

 ************************************************************************/
/************************************************************************
  SIfile.h 2 replace /users/jck/system
  880407 10:48:09 jck Cleaned Up VAX interface

 ************************************************************************/
/************************************************************************
  SIfile.h 3 replace /users/m2/dbup
  880421 17:16:40 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  SIfile.h 4 replace /users/mjc/Workbench/port/backend
  880519 23:04:01 mjc Convert NDF access to use file server calls

 ************************************************************************/
/************************************************************************
  SIfile.h 5 replace /users/m2/backend
  890503 14:53:45 m2 VAX-Apollo fixes

 ************************************************************************/
/************************************************************************
  SIfile.h 6 replace /users/m2/backend
  890828 16:58:34 m2 STD_fputs, NDF_EnableUpdate, NDF_Refresh

 ************************************************************************/
/************************************************************************
  SIfile.h 7 replace /users/m2/backend
  890927 14:28:41 m2 SUN port (preliminary)

 ************************************************************************/

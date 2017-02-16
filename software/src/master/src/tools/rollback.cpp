/*** program to roll back a net to a previous version.
*
*  default:  roll back one version
*       -n:  supply arg stating number of the version to roll back to 
*
*  A copy of the NDF argument is created and modified to represent the rolled
*  back network.  It is automatically placed in the current working directory
*  under the name "RollBkNDF" (to change the name, change the constant
*  "ROLLBACKNDF" below).
*  
*  USAGE:  rollback <NDF-name> [-n <# of vns to skip>]
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

#include "ps.d"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

#define BUFSIZE  	127
#define ROLLBACKNDF	"RollBkNDF"  /* name of modified (rolled-back) NDF */
#define PERMISSIONS	0666	     /* owner, group, others: RW  */

/*****  Error Handler (assumes sprintf won't fail) *****/
#define error(s1, s2)\
{\
    char buf [BUFSIZE];\
\
    sprintf (buf, "%s %s", s1, s2);\
    perror (buf);\
    exit (ErrorExitValue);\
}
/**** Global Function Declarations ****/

static void DoTheRollback (
    int				fdIn,
    int				rbCount
);
static void ReadStructure (
    int				fd,
    PS_Type_FO			fo,
    void *			structure,
    int				structureSize
);
static void WriteStructure (
    int				fd,
    PS_Type_FO			fo,
    void *			structure,
    int				structureSize
);


/*-------------------------------------------------------------------------*/
int main (
    int			argc,
    char *		argv[]
)
{
    int fdIn, rollbackCount; 
    PS_NDH ndh;

/****  open the original NDF  ****/
    if ((fdIn = open (argv[1], O_RDONLY, 0)) < 0)
	error ("rollback: can't open", argv[1]);

/*****  check the NDF version  *****/
    ReadStructure (fdIn, 0, &ndh, sizeof(PS_NDH));
    if (PS_NDH_NDFVersion (ndh) != PS_NDFVersionNumber)
    {
	printf ("\nrollback: Cannot read the NDF, the version is incompatible.\n");
	printf ("  Version in NDF:%d,  Version in rollback:%d\n", 
	    PS_NDH_NDFVersion (ndh), PS_NDFVersionNumber);
	exit (ErrorExitValue);
    }

/*****  get # of versions to roll back from command line  *****/
    rollbackCount = 1;					/* init to default */
    if (argc >= 3) 					/* look for	   */
	if (strcmp(argv[2], "-n") == 0)			/* ...-n option    */
	{
	    if (argc == 3 || sscanf (argv[3], "%d", &rollbackCount) == 0)
	        error ("usage: rollback <NDF-name> [-n <#-of-vns-to-skip>]","");
	}
	else 			/* argv[2] wasn't "-n", avoid "-n3" */
	    error ("usage: rollback <NDF-name> [-n <#-of-vns-to-skip>]","");

    if (rollbackCount <= 0)
    {
	printf ("\nrollback: can't roll back %d versions\n", rollbackCount);
	exit (ErrorExitValue);
    }

    DoTheRollback (fdIn, rollbackCount);

    return NormalExitValue;
} /* main */


/*--------------------------------------------------------------------------
*  Internal function resets the "current network version" pointer in the 
*  Network Directory File to the Network Version specified by "rbCount".
*  If there aren't enough NVDs to satisfy "rbCount", a fatal error occurs.
*
*  Arguments:
*	fdIn 	-- an open NDF descriptor for the original NDF
*	rbCount -- the number of NVDs to be skipped over in search of the new
*		   "most recent" NVD.
*
*  Returns: NOTHING
*****/
static void DoTheRollback (
    int				fdIn,
    int				rbCount
)
{
    PS_NDH ndh;
    PS_NVD nvd;
    PS_Type_FO  currentNvd;
    char buf [BUFSIZE];
    int n, i, fdOut;

/*****  get head of NVD chain *****/
    ReadStructure (fdIn, 0, &ndh, sizeof(PS_NDH));
    currentNvd = PS_NDH_CurrentNetworkVersion (ndh);

/*****  follow chain back past rbCount NVDs, (error if they run out)  *****/
    for (i = 0; i < rbCount && PS_FO_IsntNil (currentNvd); i++)
    {
	ReadStructure (fdIn, currentNvd, &nvd, sizeof(PS_NVD));
	currentNvd = PS_NVD_PreviousVersion (nvd);
	if (PS_FO_IsNil (currentNvd))
	{
	    printf ("\nnot enough NVDs to roll back %d versions\n", rbCount);
	    exit (ErrorExitValue);
	}
    }
/*****  make the copy to modify  *****/
    unlink (ROLLBACKNDF);
    if ((fdOut = creat (ROLLBACKNDF, PERMISSIONS)) < 0)
	error ("rollback: can't create", ROLLBACKNDF);
    if (lseek (fdIn, 0, SEEK_SET) < 0)		/* ...back to beginning */
	error ("DoTheRollback:  seek error", "");
    while ((n = read(fdIn, buf, BUFSIZE)) > 0)
	if (write (fdOut, buf, n) != n)
	    error ("rollback: error copying", ROLLBACKNDF);

/*****  update NDH copy, write NDH  *****/
    PS_NDH_CurrentNetworkVersion (ndh) = currentNvd;
    WriteStructure (fdOut, 0, &ndh, sizeof(PS_NDH));
    close (fdIn);
    close (fdOut);
}


/*---------------------------------------------------------------------------
 *****  Internal utility to read a structure from an NDF
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
 *	NOTHING 
 *****/
static void ReadStructure (
    int				fd,
    PS_Type_FO			fo,
    void *			structure,
    int				structureSize
)
{
    if (lseek (fd, fo, SEEK_SET) < 0)
	error ("ReadStructure:  seek error", "");
    if (read (fd, structure, structureSize) < 0)
	error ("ReadStructure:  read error", "");
}


/*---------------------------------------------------------------------------
 *****  Internal utility to write a structure to a file.
 *
 *  Arguments:
 *	fd			- an open UNIX file descriptor.
 *	fo			- the file offset at which the structure
 *				  resides.
 *	structure		- the address of a buffer supplying the 
 *				  structure to be written
 *	structureSize		- the size of the structure in bytes.
 *
 *  Returns:
 *	NOTHING 
 *****/
static void WriteStructure (
    int				fd,
    PS_Type_FO			fo,
    void *			structure,
    int				structureSize
)
{
    if (lseek (fd, fo, SEEK_SET) < 0)
	error ("WriteStructure:  seek error", "");
    if (write (fd, structure, structureSize) < 0)
	error ("WriteStructure:  write error", "");
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rollback.c 1 replace /vision/tools/source
  871017 22:09:41 insyte Install tool source in source control

 ************************************************************************/
/************************************************************************
  rollback.c 2 replace /vision/tools/releases/original
  880219 17:00:12 jad remove specific paths from include stmts

 ************************************************************************/
/************************************************************************
  rollback.c 3 replace /users/jad/system/TOOLS
  880314 13:00:59 jad NDF structure changes -> Version 2 NDF

 ************************************************************************/
/************************************************************************
  rollback.c 4 replace /users/jck/tools
  880503 09:08:06 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  rollback.c 5 replace /users/mjc/Workbench/port/tools
  880519 22:56:07 mjc Convert tools to use NDF file server on the Apollo

 ************************************************************************/

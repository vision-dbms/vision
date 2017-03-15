/*---------------------------------------------------------------------------
 * Program to display the object space directories associated with an NDF.
 *---------------------------------------------------------------------------
 */

/**************************
 *****  Header Files  *****
 **************************/

#include "Vk.h"

#include "ps.d"

#define PATHBUFSIZE  1024


/***********************************
 *****  File Access Utilities  *****
 ***********************************/

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


/*---------------------------------------------------------------------------
 ***** Routine to collect a counted string from a file and store it in the
 ***** supplied buffer as a null-terminated string.
 *
 *  Arguments:
 *	fd			- an NDF descriptor
 *	offset			- file offset of the counted string
 *	buffer			- pointer to a buffer for the output string
 *	maxbuf			- capacity of the buffer (including the '\0')
 *
 *  Returns:
 *	NOTHING
 *****/
PrivateFnDef void GetCountedString (
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
    if (count == maxbuf)		/* null-terminate stg */
	buffer[count-1] = '\0';
    else buffer[count] = '\0';
}


/***********************************
 *****  NDH & OSD Path Access  *****
 ***********************************/

/*---------------------------------------------------------------------------
 ***** Routine to read the Network Directory Header and Object Space
 ***** Directory path name of a newly opened NDF.
 *
 *  Arguments:
 *	fd			- the file descriptor for the NDF
 *	ndh			- pointer to a buffer for the NDH
 *	pathbuf			- a string buffer for the pathname
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	The file pointer is assumed to point to the beginning of the NDF
 *	file.  It will not be left at a sensible place.
 *
 *  Error:
 *	If the version of the NDF is incompatible with this program, 
 *	print an error message and abort.
 *
 *****/
PrivateFnDef void GetNDHAndPathname (
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
	printf ("\nxndfosds: Cannot read the NDF, the version is incompatible.\n");
	printf ("  Version in NDF:%d,  Version in xndfosds:%d\n", 
	    PS_NDH_NDFVersion (*ndh), PS_NDFVersionNumber);
	exit (ErrorExitValue);
    }

/*****  Get the path ... *****/
    GetCountedString (fd, sizeof(PS_NDH), pathbuf, PATHBUFSIZE);
}


/**************************
 *****  Main Program  *****
 **************************/

int main (
    int				Unused(argc),
    char *			argv[]
)
{
    int fd; 
    char pathbuf[PATHBUFSIZE];

    PS_NDH ndhbuf;		/* NDH buffer */

/****  open the file ****/

    fd = open (argv[1], O_RDONLY, 0);
    if (fd < 0)
    {
	perror ("File Open Error");
	exit (ErrorExitValue);
    }

    GetNDHAndPathname (fd, &ndhbuf, pathbuf);
    printf ("%s\n", pathbuf);

    close (fd);

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  xndfosds.c 1 replace /vision/tools/source
  871017 22:09:49 insyte Install tool source in source control

 ************************************************************************/
/************************************************************************
  xndfosds.c 2 replace /vision/tools/releases/original
  880219 17:04:42 jad remove specific paths from include stmts

 ************************************************************************/
/************************************************************************
  xndfosds.c 3 replace /users/jad/system/TOOLS
  880314 13:07:13 jad NDF structure changes -> Version 2 NDF

 ************************************************************************/
/************************************************************************
  xndfosds.c 4 replace /users/jck/tools
  880503 09:08:26 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  xndfosds.c 5 replace /users/mjc/Workbench/port/tools
  880519 22:56:52 mjc Convert tools to use NDF file server on the Apollo

 ************************************************************************/

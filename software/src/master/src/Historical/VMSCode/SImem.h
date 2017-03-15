/*****  Machine Specific Memory Services Header File  *****/
#ifndef siMEM_H
#define siMEM_H
/***********************************
 *****  File Mapping Services  *****
 ***********************************/
/*---------------------------------------------------------------------------
 *  NOTE:
 *	The routines in this section will be subject to major revision as part
 *	of a redesign of the memory/persistent storage manager.
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

PublicFnDecl int
    memfree (),
    memvary ();
    
/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#include <sys/ems.h>

PublicFnDecl ADDRESS	HPUX_memallc ();
#define STD_memallc	HPUX_memallc
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#define MEM_SHARED               0x01   /* SHARED MEMORY OBJECT */
#define MEM_PRIVATE              0x02   /* PRIVATE MEMORY OBJECT */
#define MEM_CODE		 0x04   /* CODE MEMORY OBJECT   */
#define MEM_DATA                 0x08   /* DATA MEMORY OBJECT   */
#define MEM_PAGED                0x10   /* PAGED MEMORY OBJECT  */

/* File protection mode definitions          */
#define MEM_X                    0x01   /* EXECUTABLE OBJECT    */
#define MEM_W                    0x02   /* WRITABLE OBJECT      */
#define MEM_R			 0x04   /* READABLE OBJECT      */

PublicFnDecl ADDRESS	VMS_memallc ();
#define STD_memallc	VMS_memallc
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#define MEM_SHARED               0x01   /* SHARED MEMORY OBJECT */
#define MEM_PRIVATE              0x02   /* PRIVATE MEMORY OBJECT */
#define MEM_CODE		 0x04   /* CODE MEMORY OBJECT   */
#define MEM_DATA                 0x08   /* DATA MEMORY OBJECT   */
#define MEM_PAGED                0x10   /* PAGED MEMORY OBJECT  */

/* File protection mode definitions          */
#define MEM_X                    0x01   /* EXECUTABLE OBJECT    */
#define MEM_W                    0x02   /* WRITABLE OBJECT      */
#define MEM_R			 0x04   /* READABLE OBJECT      */

PublicFnDecl ADDRESS	APO_memallc ();
#define STD_memallc	APO_memallc
#endif

#ifdef sun		/*****  Sun  *****/
#include <sys/mman.h>

#define MEM_SHARED               MAP_SHARED	/* SHARED MEMORY OBJECT */
#define MEM_PRIVATE              MAP_PRIVATE	/* PRIVATE MEMORY OBJECT */
#define MEM_CODE		 0x00		/* CODE MEMORY OBJECT   */
#define MEM_DATA                 0x00		/* DATA MEMORY OBJECT   */
#define MEM_PAGED                0x00		/* PAGED MEMORY OBJECT  */

/* File protection mode definitions          */
#define MEM_X                    PROT_EXECUTE	/* EXECUTABLE OBJECT    */
#define MEM_W                    PROT_WRITE	/* WRITABLE OBJECT      */
#define MEM_R			 PROT_READ	/* READABLE OBJECT      */

PublicFnDecl ADDRESS	SUN_memallc ();
#define STD_memallc	SUN_memallc
#endif

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SImem.h 1 replace /users/jck/system
  880330 21:56:31 jck SIutil.h

 ************************************************************************/
/************************************************************************
  SImem.h 2 replace /users/jck/system
  880407 10:52:04 jck Cleaned Up VAX interface

 ************************************************************************/
/************************************************************************
  SImem.h 3 replace /users/m2/dbup
  880421 17:17:12 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  SImem.h 4 replace /users/m2/backend
  890927 14:33:43 m2 SUN port (preliminary)

 ************************************************************************/

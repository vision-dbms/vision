/*****  Machine Specific Terminal IO Services Header File  *****/
#ifndef siTERMIO_H
#define siTERMIO_H


/****************
 *  Invariants  *
 ****************/


#ifdef FRONTEND
PublicVarDecl int	STD_delwinDoesNotRefresh,
			STD_doNotUseNodelay,
			STD_hasInsertDeleteLine;
			
PublicFnDecl int	STD_checkTerminalCapabilities();
PublicFnDecl int	STD_checkInputStream();
#endif /* FRONTEND */

/********************
 *  Customizations  *
 ********************/

#ifdef FRONTEND

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#define	STD_fgets		fgets
#define	STD_fcntl		fcntl
#endif /* hp */

#ifdef VMS			/*****  VAX/VMS  *****/

#define	F_GETFL			0
#define	F_SETFL			1
#define	STD_fgets		VMS_fgets
#define	STD_fcntl		VMS_fcntl

PublicFnDecl char	*VMS_fgets();
PublicFnDecl int	VMS_fcntl();

/*****  Originally the following was in the file ttyqio.h *****/
/***** TTYQIO character reader *****/

PublicFnDecl char ttyqio_$qio_c ();
PublicFnDecl char ttyqio_$get_vtxxxc ();
PublicFnDecl int ttyqio_$enable_ast();
PublicFnDecl int tty_$assign_term_chan();
PublicFnDecl int ttyqio_$get_term_chan();
PublicFnDecl int ttyqio_$disable_ast();

/****  VT2xx Key Codes Definitions  *****/

#define VT2XX_F6	-6
#define VT2XX_F7	-7
#define VT2XX_F8	-8
#define VT2XX_F9	-9
#define VT2XX_F10	-10

#define VT2XX_F11	-11
#define VT2XX_F12	-12
#define VT2XX_F13	-13
#define VT2XX_F14	-14
#define VT2XX_F15	-15		/*  HELP	*/

#define VT2XX_F16	-16		/*  DO		*/
#define VT2XX_F17	-17
#define VT2XX_F18	-18
#define VT2XX_F19	-19
#define VT2XX_F20	-20

#define VT2XX_FIND	-21
#define VT2XX_INSERT	-22
#define VT2XX_REMOVE	-23
#define VT2XX_SELECT	-24
#define VT2XX_PREV	-25

#define VT2XX_NEXT	-26
#define VT2XX_UP	-27
#define VT2XX_DOWN	-28
#define VT2XX_RIGHT	-29
#define VT2XX_LEFT	-30

#define VT2XX_0		-31
#define VT2XX_1		-32
#define VT2XX_2		-33
#define VT2XX_3		-34
#define VT2XX_4		-35

#define VT2XX_5		-36
#define VT2XX_6		-37
#define VT2XX_7		-38
#define VT2XX_8		-39
#define VT2XX_9		-40

#define VT2XX_COMMA	-41
#define VT2XX_MINUS	-42
#define VT2XX_PERIOD	-43
#define VT2XX_PF1	-44
#define VT2XX_PF2	-45

#define VT2XX_PF3	-46
#define VT2XX_PF4	-47
#define VT2XX_ENTER	-48

#define ECHO		1

#define NOECHO		0

#define get_char(echo) ttyqio_$qio_c(echo & 1)

#define get_allchar(echo) ttyqio_$qio_c(echo & 1 | 4)

#define get_vtchar(echo) ttyqio_$get_vtxxxc(echo & 1)

#define get_allvtchar(echo) ttyqio_$get_vtxxxc(echo & 1 | 4)

#endif /* VMS */

#ifdef apollo			/*****  DOMAIN/IX  *****/
#define	STD_fgets		fgets
#define	STD_fcntl		fcntl
#endif /* apollo */

#ifdef sun			/*****  Sun  *****/
#define	STD_fgets		fgets
#define	STD_fcntl		fcntl
#endif /* sun */

#endif /* FRONTEND */

#endif /* siTERMIO_H */


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SItermio.h 1 replace /users/jck/interface
  880330 22:07:03 jck Installing New Standard Interface

 ************************************************************************/
/************************************************************************
  SItermio.h 2 replace /users/lis/frontend/M2/sys
  880505 11:36:58 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  SItermio.h 3 replace /users/m2/frontend/sys
  891011 14:14:08 m2 VAX signals and pipes fixes, APOLLO DN10000 fixes

 ************************************************************************/

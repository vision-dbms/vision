/*****  Machine Specific Math Functions Header File  *****/
#ifndef siMATH_H
#define siMATH_H

#include "MSmath.d"
/*****************************
 *****  Math Facilities  *****
 *****************************/

/*************************
 *  sqrt, pow, exp, log  *
 *************************/
/*---------------------------------------------------------------------------
 *  Cover routines to approximate HP-UX's "matherr" facility in the VAX 
 *  implementation of selected math functions (see MATHERR(3M) ).  The 'C'
 *  runtime routine identifiers SQRT, POW, EXP, LOG are redefined (in STDOSS.C)
 *  to be the cover routines listed below.
 *
 *  These routines assume the existence of a user-defined "matherr".  Different 
 *  "matherr"s might be defined in different contexts;  the macros equating 
 *  these routines with their 'C' runtime library counterparts might be defined 
 *  & undefined locally to allow direct access to the 'C' routines when
 *  necessary, as well as access to this "matherr" emulation.
 *
 *  Required Definitions:
 *	squarert (x)		- returns square root of x (sic)
 *	exponentiate (x)	- returns e to the x
 *	power (x, y)		- returns x to the y
 *	log (x)			- returns the natural log of x
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

PublicFnDecl int
    matherr ();

PublicFnDecl double
    sqrt (),
    log (),
    exp (),
    pow (), 
    atof (),
    strtod (),
    dtb ();

#define STD_RobustDivide(quotient, dividend, divisor, handler)\
    (quotient = (dividend) / (divisor))


/********************
 *  Customizations  *
 ********************/
#ifdef hp9000s500		/*****  HP 9000 Series 500  *****/
#endif

#ifdef VMS			/*****  VAX/VMS  *****/

#undef STD_RobustDivide
#define STD_RobustDivide(quotient, dividend, divisor, handler)\
    (quotient = (divisor) ? (dividend) / (divisor) : (handler))

PublicFnDecl double 
    squarert (), 
    exponentiate (),
    power (),
    natlLog ();


/*****  define C run-time routines as their "covers"  *****/

#define sqrt 	squarert
#define log	natlLog
#define exp	exponentiate
#define pow	power
#endif

#ifdef apollo				/*****  DOMAIN/IX  *****/

#undef STD_RobustDivide
#define STD_RobustDivide(quotient, dividend, divisor, handler)\
    (quotient = (divisor) ? (dividend) / (divisor) : (handler))

#endif /* apollo */

#ifdef sun		/*****  Sun  *****/
#endif

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SImath.h 1 replace /users/jck/system
  880330 21:56:17 jck SIutil.h

 ************************************************************************/
/************************************************************************
  SImath.h 2 replace /users/m2/dbup
  880421 17:26:47 m2 Apollo port

 ************************************************************************/
/************************************************************************
  SImath.h 3 replace /users/jck/system
  881104 14:37:36 jck Added STD_RobustDivide macro

 ************************************************************************/
/************************************************************************
  SImath.h 4 replace /users/m2/backend
  890503 14:57:05 m2 Prprocessor fix

 ************************************************************************/
/************************************************************************
  SImath.h 5 replace /users/m2/backend
  890927 14:29:52 m2 SUN port (preliminary)

 ************************************************************************/

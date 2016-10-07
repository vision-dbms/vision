/*****  Machine Specific Math Routines  *****/

/*****************************
 *****  Math Facilities  *****
 *****************************/

#include <math.h>

#include "stdoss.h"
#include "MSmath.d"

/**************************
 *  'sqrt, pow, exp, log  *
 **************************/

#ifdef VMS			/*****  VAX/VMS  *****/
#ifdef DBUPDATE
/*---------------------------------------------------------------------------
 * Provide a no-op matherr routine for the data base update subsystem. This is
 * to keep the VAX linker quiet.
 *---------------------------------------------------------------------------
 */

PublicFnDef int
    matherr (x)
struct exception *x;
{
}
#endif

/*-------------------------------------------------------------------------- 
*  Cover routines to approximate HP-UX's "matherr" facility in the VAX 
*  implementation of selected math functions (see MATHERR(3M) ).  The 'C'
*  runtime routine identifiers SQRT, POW, EXP, LOG are redefined in this file 
*  to be the cover routines given below.
*
*  These routines assume the existence of a user-defined "matherr".  Different 
*  "matherr"s might be defined in different contexts;  the macros equating 
*  these routines with their 'C' runtime library counterparts might be defined 
*  & undefined locally to allow direct access to the 'C' routines when
*  necessary, as well as access to this "matherr" emulation.
*****/


/*--------------------------------------------------------------------------
*  SQUARERT -- cover for SQRT -- vax and HP-UX error reporting is the same:
*
*	if argument < 0, DOMAIN error reported to "matherr"; function returns 0
*	if matherr returns 0, "errno" is set to EDOM.
*****/
PublicFnDef double 
    squarert(y)
double y;
{
    struct exception ex;
    int errnoSave = errno;

    errno = 0;				/* clear errno */
    ex.retval = sqrt (y);
    if (errno == EDOM)
    {
        ex.type   = DOMAIN;
	ex.name   = "sqrt";
	ex.arg1   = y;
	ex.retval = 0;

	if (matherr (&ex) != 0)		/* if matherr returns non-zero */
	    errno = errnoSave;		/* ...unset errno */
	return ex.retval;
    }
    errno = errnoSave;
    return ex.retval;
}

/*--------------------------------------------------------------------------
*  NATLLOG -- cover for LOG -- vax and HP-UX error reporting is the same:
*
*	DOMAIN error reported to "matherr" if arg < 0, SING error reported 
*	to "matherr" if arg = 0.  If matherr returns 0, "errno" is set to EDOM.
*	Function will return -HUGE. 
*****/
PublicFnDef double 
    natlLog (y)
double y;
{
    struct exception ex;
    int errnoSave = errno;

    errno = 0;				/* clear errno */
    ex.retval = log (y);
    if (errno == EDOM)
    {
	if (y == 0)
	    ex.type = SING;
	else
	    ex.type = DOMAIN;
	ex.name   = "log";
	ex.arg1   = y;
	ex.retval = -HUGE;

	if (matherr (&ex) != 0)		/* if matherr returns non-zero */
	    errno = errnoSave;		/* ...unset errno */
	return ex.retval;
    }
    errno = errnoSave;
    return ex.retval;
}

/*--------------------------------------------------------------------------
*  EXPONENTIATE -- cover for EXP -- vax and HP-UX error reporting differ...
*
*	if result would overflow,  errno is set to ERANGE unless matherr 
*	suppresses this;  in all error cases, function returns HUGE.
*
*  HP-UX exp would return 0 if the result would underflow.
*****/
PublicFnDef double 
    exponentiate (y)
double y;
{
    struct exception ex;
    int errnoSave = errno;

    errno = 0;				/* clear errno */
    ex.retval = exp (y);
    if (errno == ERANGE)
    { 					/*  ex.type not set in HP-UX */
	ex.name = "exp";
	ex.arg1 = y;
	ex.retval = HUGE;

	if (matherr (&ex) != 0)		/* if matherr returns non-zero */
	    errno = errnoSave;		/* ...unset errno */
	return ex.retval;
    }
    errno = errnoSave;
    return ex.retval;		
}

/*--------------------------------------------------------------------------
*  POWER -- cover for POW -- vax and HP-UX error reporting differ...
*
*	if x < 0   or   (x = 0 and y <= 0), errno is set to EDOM unless 
*		matherr suppresses this; function returns 0
*		(...DOMAIN error reported to "matherr" if arg < 0)
*
*	if result would overflow, errno is set to ERANGE unless matherr 
*		suppresses this; function returns HUGE.
*****/
PublicFnDef double 
    power (x, y)
double x, y;
{
    struct exception ex;
    int errnoSave = errno;

    errno = 0;				/* clear errno */
    ex.retval = pow (x, y);

    if (errno == EDOM)
    {	
        ex.type = DOMAIN;
	ex.name = "pow";
	ex.arg1 = x;
	ex.arg2 = y;
	ex.retval = 0;

	if (matherr (&ex) != 0)		/* if matherr returns non-zero */
	    errno = errnoSave;		/* ...unset errno */
	return ex.retval;
    }
    if (errno == ERANGE)
    {
	ex.type = OVERFLOW;
	ex.name = "pow";
	ex.arg1 = x;
	ex.arg2 = y;
	ex.retval = HUGE;

	if (matherr (&ex) != 0)		/* if matherr returns non-zero */
	    errno = errnoSave;		/* ...unset errno */
	return ex.retval;
    }
    errno = errnoSave;
    return ex.retval;
}
#endif /* VMS */



/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSmath.c 1 replace /users/jck/system
  880330 21:52:49 jck SIutil.h

 ************************************************************************/

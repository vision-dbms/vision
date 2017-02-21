#ifndef msMATH_D
#define msMATH_D

#ifdef VMS				/*****  VAX/VMS  *****/
/***** FROM HP-UX <math.h>  *****/

struct exception {
	int type;
	char *name;
	double arg1, arg2, retval;
};

#define DOMAIN		1
#define SING		2
#define OVERFLOW	3
#define UNDERFLOW	4
#define TLOSS		5
#define PLOSS		6

/*****  ...end of HP-UX <math.h> stuff  *****/

#endif
#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSmath.d 1 replace /users/jck/system
  880330 21:53:07 jck SIutil.h

 ************************************************************************/

/*****  Physical Object Pointer Array Shared Definitions  *****/
#ifndef rtPARRAY_D
#define rtPARRAY_D

/*---------------------------------------------------------------------------
 * P-Arrays are the simplest form of container which references other
 * physical objects.  A P-Array is a contiguous array of physical object
 * pointers.  A P-Array contains no explicit count field - the number of
 * physical object pointers in a P-Array is the size of the P-Array
 * container (obtained from its preamble) divided by 'sizeof (M_POP)'.
 * The first POP in a P-Array is at the first data byte of the container.
 *---------------------------------------------------------------------------
 */


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTparray.d 1 replace /users/mjc/system
  860323 17:20:18 mjc Physical object pointer array R-Type

 ************************************************************************/
/************************************************************************
  RTparray.d 2 replace /users/mjc/system
  860412 18:05:14 mjc Changed name from 'RTpvector'

 ************************************************************************/
/************************************************************************
  RTparray.d 3 replace /users/mjc/system
  860627 13:57:28 mjc Release new CPD definitions

 ************************************************************************/
/************************************************************************
  RTparray.d 4 replace /users/mjc/translation
  870524 09:34:19 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

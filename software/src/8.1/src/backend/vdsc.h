/*****  Descriptor Facility Header File  *****/
#ifndef DSC_H
#define DSC_H

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

#include "DSC_Descriptor.h"

/****************************************
 ****************************************
 *****  'Pointer' Support Routines  *****
 ****************************************
 ****************************************/

PublicFnDecl void DSC__ComplainAboutEmptyPtrType (char const* pText);

/*********************************
 *********************************
 *****  Descriptor Routines  *****
 *********************************
 *********************************/

/*****************************
 *****  Traversal Setup  *****
 *****************************/

PublicFnDecl void DSC__SetupJointTraversal(
    DSC_Descriptor		*dsc1,
    DSC_Descriptor		*dsc2,
    M_CPD*			*cpd1,
    M_CPD*			*cpd2,
    rtLINK_CType*		*linkc1,
    rtLINK_CType*		*linkc2,
    int				*converted
);

/*****************************
 *****************************
 *****  Counter Display  *****
 *****************************
 *****************************/

PublicFnDecl void DSC_DisplayCounts ();


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dsc.h 1 replace /users/mjc/system
  861002 17:59:47 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  dsc.h 2 replace /users/mjc/translation
  870524 09:37:33 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

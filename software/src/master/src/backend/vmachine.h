/*****  Virtual Machine Header File  *****/
#ifndef VMachine_Interface
#define VMachine_Interface

/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void V_SetLargeTaskSize (unsigned int size);

PublicFnDecl void V_SetDefragmentAttempt (unsigned int flag);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  vmachine.d 1 replace /users/mjc/system
  860325 11:32:33 mjc Virtual Machine

 ************************************************************************/
/************************************************************************
  vmachine.d 2 replace /users/mjc/system
  860402 18:02:58 mjc Defined byte codes

 ************************************************************************/
/************************************************************************
  vmachine.d 3 replace /users/mjc/system
  860403 08:21:37 mjc Added specific byte codes for binary-ops, added valence and mwi to message byte codes

 ************************************************************************/
/************************************************************************
  vmachine.d 4 replace /users/mjc/system
  860404 20:16:37 mjc Corrected definition of wrapup unary byte codes

 ************************************************************************/
/************************************************************************
  vmachine.d 5 replace /users/mjc/system
  860406 09:51:09 mjc Release repositioned pad byte codes and the 'store selector' byte codes

 ************************************************************************/
/************************************************************************
  vmachine.d 6 replace /users/mjc/system
  860407 08:26:24 mjc Add parameter acquisition instructions

 ************************************************************************/
/************************************************************************
  vmachine.d 7 replace /users/mjc/system
  860410 19:20:54 mjc Restructured dispatch/added 'FetchInstruction'

 ************************************************************************/
/************************************************************************
  vmachine.d 8 replace /users/mjc/system
  860412 19:18:33 mjc Release 'exec' byte codes

 ************************************************************************/
/************************************************************************
  vmachine.d 9 replace /users/jck/system
  860414 12:02:09 jck Corrected documentation for acqkp bytecode (19)

 ************************************************************************/
/************************************************************************
  vmachine.d 10 replace /users/jck/source
  860416 13:28:39 jck Fixed bug in ..BCPS_Pad case in ..FetchInstruction macro

 ************************************************************************/
/************************************************************************
  vmachine.d 11 replace /users/jck/system
  860429 10:36:08 jck Modified interpretation of tag param

 ************************************************************************/
/************************************************************************
  vmachine.d 12 replace /users/mjc/system
  860526 20:32:26 mjc Preliminary release to free up other modules

 ************************************************************************/
/************************************************************************
  vmachine.d 13 replace /users/mjc/system
  860527 18:00:33 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.d 14 replace /users/mjc/system
  860531 10:53:27 mjc Eliminated 'VMACHINE_TopLevel...' variable

 ************************************************************************/
/************************************************************************
  vmachine.d 15 replace /users/mjc/system
  860601 18:26:46 mjc Release virtual machine through byte code 4

 ************************************************************************/
/************************************************************************
  vmachine.d 16 replace /users/mjc/system
  860602 15:00:08 mjc Release changes to use new value descriptor format

 ************************************************************************/
/************************************************************************
  vmachine.d 17 replace /users/mjc/system
  860604 19:24:56 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.d 18 replace /users/mjc/system
  860606 13:37:34 mjc Release initial working version of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.d 19 replace /users/mjc/system
  860607 12:14:01 mjc Fix a few bugs

 ************************************************************************/
/************************************************************************
  vmachine.d 20 replace /users/mjc/system
  860610 11:40:13 mjc Added 'StoreNoDescriptor' byte code

 ************************************************************************/
/************************************************************************
  vmachine.d 21 replace /users/mjc/system
  860623 10:21:50 mjc Converted to use new 'valuedsc' macros

 ************************************************************************/
/************************************************************************
  vmachine.d 22 replace /users/mjc/system
  860703 11:52:28 mjc Changed declaration of 'acqp' byte codes, added support for '^current'

 ************************************************************************/
/************************************************************************
  vmachine.d 23 replace /users/mjc/system
  860709 10:16:36 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  vmachine.d 24 replace /users/mjc/system
  860728 13:57:04 mjc Prerelease for db update

 ************************************************************************/
/************************************************************************
  vmachine.d 25 replace /users/mjc/system
  860803 00:19:02 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  vmachine.d 26 replace /users/mjc/system
  860807 18:05:43 mjc Release basic time operations

 ************************************************************************/
/************************************************************************
  vmachine.d 27 replace /users/mjc/system
  860811 13:50:21 mjc Completed initial implementation of date offsets

 ************************************************************************/
/************************************************************************
  vmachine.d 28 replace /users/mjc/system
  861002 19:23:02 mjc Release of new list architecture

 ************************************************************************/
/************************************************************************
  vmachine.d 29 replace /users/jad/system
  861106 17:57:51 jad added several routines for fragmentations

 ************************************************************************/
/************************************************************************
  vmachine.d 30 replace /users/jad/system
  861116 16:00:36 jad in order to split up primfns, needed to make the
primitive function array owned by vmachine

 ************************************************************************/
/************************************************************************
  vmachine.d 31 replace /users/jad/system
  870204 15:29:07 jad changed comment to give PTutil more primitive numbers

 ************************************************************************/
/************************************************************************
  vmachine.d 32 replace /users/jad/system
  870205 14:58:15 jad changed comment containing primitive numbers

 ************************************************************************/
/************************************************************************
  vmachine.d 33 replace /users/jck/system
  870420 13:34:18 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  vmachine.d 34 replace /users/mjc/translation
  870524 09:42:57 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  vmachine.d 35 replace /users/jck/system
  870611 10:19:41 jck updated comment describing primitve index allocation

 ************************************************************************/
/************************************************************************
  vmachine.d 36 replace /users/jck/system
  870706 08:16:14 jck Added a couple of 'LoadA...' macros

 ************************************************************************/
/************************************************************************
  vmachine.d 37 replace /users/jad/system
  880711 16:39:56 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  vmachine.h 1 replace /users/mjc/system
  860325 11:32:35 mjc Virtual Machine

 ************************************************************************/
/************************************************************************
  vmachine.h 2 replace /users/mjc/system
  860410 19:20:57 mjc Restructured dispatch/added 'FetchInstruction'

 ************************************************************************/
/************************************************************************
  vmachine.h 3 replace /users/jck/source
  860416 13:29:31 jck Added declaration of ..ByteCodeDispatchVector

 ************************************************************************/
/************************************************************************
  vmachine.h 4 replace /users/jck/system
  860429 10:37:44 jck Export Internal Known Selector translation 
capability for the benefit of rslang

 ************************************************************************/
/************************************************************************
  vmachine.h 5 replace /users/mjc/system
  860526 20:32:29 mjc Preliminary release to free up other modules

 ************************************************************************/
/************************************************************************
  vmachine.h 6 replace /users/mjc/system
  860527 18:00:36 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.h 7 replace /users/mjc/system
  860531 10:53:30 mjc Eliminated 'VMACHINE_TopLevel...' variable

 ************************************************************************/
/************************************************************************
  vmachine.h 8 replace /users/mjc/system
  860601 18:26:50 mjc Release virtual machine through byte code 4

 ************************************************************************/
/************************************************************************
  vmachine.h 9 replace /users/mjc/system
  860604 19:25:00 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.h 10 replace /users/mjc/system
  860607 12:14:06 mjc Fix a few bugs

 ************************************************************************/
/************************************************************************
  vmachine.h 11 replace /users/mjc/system
  860709 10:16:39 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  vmachine.h 12 replace /users/mjc/system
  860728 13:57:14 mjc Prerelease for db update

 ************************************************************************/
/************************************************************************
  vmachine.h 13 replace /users/mjc/system
  860803 00:19:05 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  vmachine.h 14 replace /users/mjc/system
  861002 19:23:05 mjc Release of new list architecture

 ************************************************************************/
/************************************************************************
  vmachine.h 15 replace /users/mjc/system
  861015 23:53:40 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  vmachine.h 16 replace /users/jad/system
  861106 17:57:54 jad added several routines for fragmentations

 ************************************************************************/
/************************************************************************
  vmachine.h 17 replace /users/jad/system
  861116 16:00:39 jad in order to split up primfns, needed to make the 
primitive function array owned by vmachine

 ************************************************************************/
/************************************************************************
  vmachine.h 18 replace /users/jad/system
  861119 12:42:20 jad added ln, exp, and sqrt primitives

 ************************************************************************/
/************************************************************************
  vmachine.h 19 replace /users/mjc/system
  861226 12:25:54 mjc Implemented 'super'

 ************************************************************************/
/************************************************************************
  vmachine.h 20 replace /users/jad/system
  870205 14:59:05 jad moved DecodeClosureInAccumulator from PFutil to vmachine

 ************************************************************************/
/************************************************************************
  vmachine.h 21 replace /users/jad/system
  870210 14:20:36 jad implement LoadAccumulatorWithStringStore routine

 ************************************************************************/
/************************************************************************
  vmachine.h 22 replace /users/jad/system
  870220 18:58:04 jad implement LoadAccumulatorWithDomainLimitedDateFnResult routine for date na's

 ************************************************************************/
/************************************************************************
  vmachine.h 23 replace /users/mjc/translation
  870524 09:43:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  vmachine.h 24 replace /users/mjc/system
  870627 10:54:31 mjc Implemented 'SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  vmachine.h 25 replace /users/jad/system
  871215 14:10:12 jad release a public V_NormalizeDescriptor function

 ************************************************************************/
/************************************************************************
  vmachine.h 26 replace /users/jck/system
  890413 14:22:33 jck Added the ability to split homomorphic tasks into subtasks

 ************************************************************************/

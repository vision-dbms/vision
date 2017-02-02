/*****  Shared Representation Type Definitions  *****/
#ifndef RTYPE_D
#define RTYPE_D

/**************************************
 *****  The Representation Types  *****
 **************************************/
/*---------------------------------------------------------------------------
 * The following list and the enumerated type built from it defines the set of
 * representation types in the system.  New types MUST ALWAYS be added to the
 * end of the list and existing types MUST NOT be deleted from the list.  The
 * type names are surrounded with the 'RTYPE_TypeId' to allow the text of the
 * name to be expanded into different syntactic forms by changing the
 * definition of the 'RTYPE_TypeId' macro.
 *---------------------------------------------------------------------------
 */

#define RTYPE_TypeId(typeName) typeName

#define RTYPE_RTypeList\
    RTYPE_TypeId (RTYPE_C_Undefined0),\
\
    RTYPE_TypeId (RTYPE_C_TCT),\
    RTYPE_TypeId (RTYPE_C_PCT),\
\
    RTYPE_TypeId (RTYPE_C_Descriptor1),\
\
    RTYPE_TypeId (RTYPE_C_String),\
    RTYPE_TypeId (RTYPE_C_PArray),\
\
    RTYPE_TypeId (RTYPE_C_PToken),\
\
    RTYPE_TypeId (RTYPE_C_Link),\
\
    RTYPE_TypeId (RTYPE_C_CharUV),\
    RTYPE_TypeId (RTYPE_C_DoubleUV),\
    RTYPE_TypeId (RTYPE_C_FloatUV),\
    RTYPE_TypeId (RTYPE_C_IntUV),\
    RTYPE_TypeId (RTYPE_C_RefUV),\
    RTYPE_TypeId (RTYPE_C_SelUV),\
    RTYPE_TypeId (RTYPE_C_UndefUV),\
\
    RTYPE_TypeId (RTYPE_C_MethodD),\
\
    RTYPE_TypeId (RTYPE_C_Block),\
    RTYPE_TypeId (RTYPE_C_Method),\
    RTYPE_TypeId (RTYPE_C_Context),\
    RTYPE_TypeId (RTYPE_C_Closure),\
\
    RTYPE_TypeId (RTYPE_C_Vector),\
    RTYPE_TypeId (RTYPE_C_ListStore),\
    RTYPE_TypeId (RTYPE_C_Undefined),\
    RTYPE_TypeId (RTYPE_C_ValueStore),\
\
    RTYPE_TypeId (RTYPE_C_Descriptor0),\
\
    RTYPE_TypeId (RTYPE_C_Index),\
\
    RTYPE_TypeId (RTYPE_C_Descriptor),\
    RTYPE_TypeId (RTYPE_C_Unused1),\
    RTYPE_TypeId (RTYPE_C_Unused2),\
    RTYPE_TypeId (RTYPE_C_Unused3),\
    RTYPE_TypeId (RTYPE_C_Unused4),\
    RTYPE_TypeId (RTYPE_C_Unused5),\
    RTYPE_TypeId (RTYPE_C_Unused6),\
    RTYPE_TypeId (RTYPE_C_Unused7),\
    RTYPE_TypeId (RTYPE_C_Unused8),\
    RTYPE_TypeId (RTYPE_C_Unused9),\
\
    RTYPE_TypeId (RTYPE_C_Dictionary),\
    RTYPE_TypeId (RTYPE_C_Unused11),\
    RTYPE_TypeId (RTYPE_C_Unused12),\
    RTYPE_TypeId (RTYPE_C_Unused13),\
    RTYPE_TypeId (RTYPE_C_Unused14),\
    RTYPE_TypeId (RTYPE_C_Unused15),\
    RTYPE_TypeId (RTYPE_C_Unused16),\
    RTYPE_TypeId (RTYPE_C_Unused17),\
    RTYPE_TypeId (RTYPE_C_Unused18),\
    RTYPE_TypeId (RTYPE_C_Unused19),\
\
    RTYPE_TypeId (RTYPE_C_Unsigned32UV),\
    RTYPE_TypeId (RTYPE_C_Unsigned64UV),\
    RTYPE_TypeId (RTYPE_C_Unsigned96UV),\
    RTYPE_TypeId (RTYPE_C_Unsigned128UV)

enum RTYPE_Type {RTYPE_RTypeList, RTYPE_C_Any, RTYPE_C_MaxType = RTYPE_C_Any};


/***************************************************
 *****  Representation Type Validation Macros  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 * The following macros determine if a representation type code is among the
 * valid representation types.  The macro 'RTYPE_IsAValidType' returns true
 * if the representation type is valid, false otherwise.  The macro
 * 'RTYPE_ValidateType' signals an error if the code is invalid.
 *---------------------------------------------------------------------------
 */

#define RTYPE_IsAValidType(rtype) (\
    (int)(rtype) >= 0 && (int)(rtype) < (int)RTYPE_C_MaxType\
)

#define RTYPE_ValidateType(rtype, errorMessage)\
if (!RTYPE_IsAValidType (rtype)) ERR_SignalFault (\
    EC__UnknownRType, errorMessage\
)

/*
 *---------------------------------------------------------------------------
 * The following macros complain about incorrect object types.
 *---------------------------------------------------------------------------
 */

#define RTYPE_ComplainAboutType(fromWhom, actualType, expectedType)\
ERR_SignalFault (\
    EC__InternalInconsistency,\
    UTIL_FormatMessage (\
	"%s: R-Type Error: %s Found, %s Expected.",\
	fromWhom,\
	RTYPE_TypeIdAsString ((RTYPE_Type)(actualType)),\
	RTYPE_TypeIdAsString ((RTYPE_Type)(expectedType))\
    )\
)

#define RTYPE_MustBeA(fromWhom, actualType, expectedType)\
if ((RTYPE_Type)(actualType) != (RTYPE_Type)(expectedType))\
    RTYPE_ComplainAboutType (fromWhom, actualType, expectedType);\
else


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rtype.d 1 replace /users/mjc/system
  860226 16:24:51 mjc create

 ************************************************************************/
/************************************************************************
  rtype.d 2 replace /users/mjc/system
  860321 17:59:11 mjc Release 1 of new R-Type architecture

 ************************************************************************/
/************************************************************************
  rtype.d 3 replace /users/mjc/system
  860323 17:22:04 mjc Release of additional work on new memory architecture

 ************************************************************************/
/************************************************************************
  rtype.d 4 replace /users/jad
  860326 19:53:18 jad added string rtype

 ************************************************************************/
/************************************************************************
  rtype.d 5 replace /users/mjc/system
  860404 20:17:34 mjc Added shells for new representation types

 ************************************************************************/
/************************************************************************
  rtype.d 6 replace /users/hjb/system
  860408 02:51:11 hjb Done adding rtSELECTOR and rtSELB Representation Types

 ************************************************************************/
/************************************************************************
  rtype.d 7 replace /users/mjc/system
  860409 08:01:27 mjc Added 'RTptoken'

 ************************************************************************/
/************************************************************************
  rtype.d 8 replace /users/mjc/system
  860410 19:18:31 mjc Added standard print services and 'RPrint'

 ************************************************************************/
/************************************************************************
  rtype.d 9 replace /users/mjc/system
  860411 22:02:37 mjc Added 'RTseluv'

 ************************************************************************/
/************************************************************************
  rtype.d 10 replace /users/mjc/system
  860412 16:35:07 mjc Added 'RTposition', changed 'RTselectr' to 'RTselector'

 ************************************************************************/
/************************************************************************
  rtype.d 11 replace /users/mjc/system
  860412 18:12:43 mjc Change 'P-Vector' to 'P-Array', release new 'P-Vector'

 ************************************************************************/
/************************************************************************
  rtype.d 12 replace /users/mjc/system
  860421 12:30:18 mjc Add 'RTlink'

 ************************************************************************/
/************************************************************************
  rtype.d 13 replace /users/hjb/system
  860428 19:24:03 hjb added declarations for RTpobject

 ************************************************************************/
/************************************************************************
  rtype.d 14 replace /users/mjc/system
  860430 20:01:00 mjc Added 'ComplainAboutType' and 'MustBeA' macros

 ************************************************************************/
/************************************************************************
  rtype.d 15 replace /users/mjc/system
  860508 18:13:00 mjc Added 'character' u-vector

 ************************************************************************/
/************************************************************************
  rtype.d 16 replace /users/mjc/system
  860514 10:07:59 mjc Added standard u-vector handler request definitions

 ************************************************************************/
/************************************************************************
  rtype.d 17 replace /users/mjc/system
  860516 18:11:13 mjc Added 'undefined value' u-vectors

 ************************************************************************/
/************************************************************************
  rtype.d 18 replace /users/mjc/system
  860517 19:38:24 mjc Added 'RTYPE_\VectorProtocolNew' handler request

 ************************************************************************/
/************************************************************************
  rtype.d 19 replace /users/jck/system
  860519 07:36:41 jck Added entries for RTmethodd

 ************************************************************************/
/************************************************************************
  rtype.d 20 replace /users/mjc/system
  860525 16:21:27 mjc Add 'RTprimfnuv'

 ************************************************************************/
/************************************************************************
  rtype.d 21 replace /users/mjc/system
  860526 20:31:24 mjc Moved terminating '#define' and '#endif' to right place

 ************************************************************************/
/************************************************************************
  rtype.d 22 replace /users/mjc/system
  860530 14:31:48 mjc Added 'DoRF{Assign,Extract}', deleted 'RTYPE_C_{Position,
PVector,PrimFn}'

 ************************************************************************/
/************************************************************************
  rtype.d 23 replace /users/mjc/system
  860615 19:02:56 mjc Deleted 'RT{intensn,pobject}' added 'RT{context,tstore'

 ************************************************************************/
/************************************************************************
  rtype.d 24 replace /users/mjc/system
  860616 10:04:21 mjc Change 'Partition' to 'ListStore'

 ************************************************************************/
/************************************************************************
  rtype.d 25 replace /users/mjc/system
  860623 10:19:56 mjc Added 'RTvaluedsc'

 ************************************************************************/
/************************************************************************
  rtype.d 26 replace /users/mjc/maketest
  860708 12:45:13 mjc Deleted 'RTselb' and 'RTselector'

 ************************************************************************/
/************************************************************************
  rtype.d 27 replace /users/mjc/system
  860711 10:39:08 mjc Deleted 'RTlexb' and 'RTcontext', added 'RTbcontext' and 'RTmcontext'

 ************************************************************************/
/************************************************************************
  rtype.d 28 replace /users/mjc/system
  861002 18:03:24 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  rtype.d 29 replace /users/mjc/system
  870109 10:27:48 mjc Release declaration of r-type 'Index'

 ************************************************************************/
/************************************************************************
  rtype.d 30 replace /users/mjc/system
  870215 22:57:46 lcn Change memory manager modules and rtypes

 ************************************************************************/
/************************************************************************
  rtype.d 31 replace /users/mjc/translation
  870524 09:41:16 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rtype.d 32 replace /users/jck/system
  890912 16:06:08 jck Removing RTtstore from the system

 ************************************************************************/

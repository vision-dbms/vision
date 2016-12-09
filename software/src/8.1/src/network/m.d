/*****  The Memory Manager Shared Declarations  *****/
#ifndef M_D
#define M_D

/*---------------------------------------------------------------------------
 * To facilitate the use of macro definitions wherever possible, all memory
 * manager type definitions are made publicly available here and in the
 * declaration files associated with the various container table
 * types.  Many memory manager internal state variables are declared in "m.h"
 * for this same reason.  The presence of these declarations should not be a
 * temptation to misuse the information available through these declarations
 * and variables - this will only complicate changing the memory manager and
 * may introduce many strange and exotic bugs into the system.  To aid in
 * determining the information the memory manager considers private, names
 * which should not be used outside the memory manager or its defined macros
 * have been prefixed with an underscore (_).
 *---------------------------------------------------------------------------
 */

/***********************
 ***********************
 *****  Constants  *****
 ***********************
 ***********************/

/**************************
 *****  Known Spaces  *****
 **************************/

#define M_KnownSpace_ScratchPad		0
#define M_KnownSpace_Nil		1
#define M_KnownSpace_SystemRoot		2
#define M_KnownSpace_UserRoot		3

/*******************************************
 *****  Known Container Table Indices  *****
 *******************************************/

#define M_KnownCTI_ContainerTable	0
#define M_KnownCTI_SpaceRoot		1


/*********************************************
 *********************************************
 *****  Physical Object Pointers (POPs)  *****
 *********************************************
 *********************************************/

/*****************************
 *****  Type Definition  *****
 *****************************/

/*****  Field Sizes and Limits  *****/
#define M_POP_ObjectSpaceFieldSize	10
#define M_POP_ContainerIndexFieldSize	(32 - M_POP_ObjectSpaceFieldSize)

#define M_POP_MaxObjectSpace	(~((~0) << M_POP_ObjectSpaceFieldSize))
#define M_POP_MaxContainerIndex	(~((~0) << M_POP_ContainerIndexFieldSize))

/*****  Type Definition  *****/
union M_POP {
    int				asInt;
    struct component_t {
#   if Vk_DataFormatNative == Vk_DataFormatLEndian
	unsigned int		m_xContainer	: M_POP_ContainerIndexFieldSize,
				m_xObjectSpace	: M_POP_ObjectSpaceFieldSize;
# elif Vk_DataFormatNative == Vk_DataFormatBEndian
	unsigned int		m_xObjectSpace	: M_POP_ObjectSpaceFieldSize,
				m_xContainer	: M_POP_ContainerIndexFieldSize;
#endif
    } asComponents;
};


/***************************
 *****  Access Macros  *****
 ***************************/

/*****  Direct  *****/
#define M_POP_D_AsInt(pop)		((pop).asInt)
#define M_POP_D_AsComponents(pop)	((pop).asComponents)
#define M_POP_D_ObjectSpace(pop)	(M_POP_D_AsComponents (pop).m_xObjectSpace)
#define M_POP_D_ContainerIndex(pop)	(M_POP_D_AsComponents (pop).m_xContainer)

/*****  Indirect  *****/
#define M_POP_AsInt(popPtr)		M_POP_D_AsInt		(*(popPtr))
#define M_POP_AsComponents(popPtr)	M_POP_D_AsComponents	(*(popPtr))
#define M_POP_ObjectSpace(popPtr)	M_POP_D_ObjectSpace	(*(popPtr))
#define M_POP_ContainerIndex(popPtr)	M_POP_D_ContainerIndex	(*(popPtr))


/**********************************
 **********************************
 *****  Container Structures  *****
 **********************************
 **********************************/

/********************************
 *****  Container Preamble  *****
 ********************************/

/*---------------------------------------------------------------------------
 * The container preamble pointer of a CPD always points to a structure of
 * type M_CPreamble.  The first data byte of a container is always located at
 * (pointer_t)(cPreamblePtr + 1).  The preamble holds two useful pieces
 * of container information - the container's allocated size in bytes and the
 * index of its representation type.  Because the address of the preamble is
 * placed in a CPD (see below), these pieces of information are always just a
 * pointer away!  Look but DO NOT TOUCH !!!
 *
 * CPreamble Field Description:
 *	nSize			- the number of bytes in this container if
 *				  'oSize' is zero and a meaningless value
 *				  otherwise.  If this field is valid, its
 *				  value does not include the size of this
 *				  preamble.
 *	rType			- the representation type of this container.
 *	oSize			- the original size field of the container.
 *				  For containers written by pre-5.7 versions
 *				  of the system, this field holds the size
 *				  of the container exclusive of its preamble
 *				  and end markers.  For containers written
 *				  by version 5.7 and beyond, this field is
 *				  zero and 'nSize' hold the container size.
 *	pop			- the container table entry which references
 *				  this container.
 *---------------------------------------------------------------------------
 */

/*****  Type Definition  *****/
struct M_CPreamble {
    unsigned int	nSize;
    unsigned int	rType	: 8,
			oSize	: 24;
    M_POP		pop;
};

/*****  Access Macros  *****/
#define M_CPreamble_NSize(p)	((p)->nSize)
#define M_CPreamble_RType(p)	((p)->rType)
#define M_CPreamble_OSize(p)	((p)->oSize)
#define M_CPreamble_POP(p)	((p)->pop)

#define M_CPreamble_Size(p) ((p)->oSize ? (p)->oSize : (p)->nSize)

#define M_CPreamble_POPObjectSpace(p) M_POP_D_ObjectSpace (\
    M_CPreamble_POP (p)\
)
#define M_CPreamble_POPContainerIndex(p) M_POP_D_ContainerIndex (\
    M_CPreamble_POP (p)\
)

#define M_CPreamble_ContainerBase(p)			((pointer_t)((p) + 1))
#define M_CPreamble_ContainerBaseAsType(p, type)	((type *)((p) + 1))

#define M_CPreamble_ValidateFraming(p) if (\
    0 == M_CPreamble_OSize (p) &&\
    M_CPreamble_NSize (p) != M_CEndMarker_Size (M_CPreamble_EndMarker (p))\
) ERR_SignalFault (\
    EC__ContainerFramingError,\
    UTIL_FormatMessage (\
	"Specifics: MA:%08X, POP:[%d:%d], RT:[%d], S:[%d/%d]\n",\
	p,\
	M_CPreamble_POPObjectSpace (p),\
	M_CPreamble_POPContainerIndex (p),\
	M_CPreamble_RType (p),\
	M_CPreamble_NSize (p),\
	M_CEndMarker_Size (M_CPreamble_EndMarker (p)))\
); else

#define M_CPreamble_FixSize(p) if (M_CPreamble_OSize (p))\
{\
    M_CPreamble_NSize (p) = M_CPreamble_OSize (p);\
    M_CPreamble_OSize (p) = 0;\
}\
else

#define M_CPreamble_FixSizes(p) if (M_CPreamble_OSize (p))\
{\
    M_CPreamble_NSize (p) = M_CPreamble_OSize (p);\
    M_CPreamble_OSize (p) = 0;\
    M_CEndMarker_Size (M_CPreamble_EndMarker (p)) = M_CPreamble_NSize (p);\
}\
else

#define M_CPreamble_EndMarker(p) (\
    (M_CEndMarker*)(M_CPreamble_ContainerBase (p) + M_CPreamble_Size (p))\
)


/**********************************
 *****  Container End Marker  *****
 **********************************/
/*---------------------------------------------------------------------------
 * Every container is followed by an end marker.  The end marker format
 * depends on the structure of the container preamble.  Old containers with
 * 24 size fields (i.e., containers with a non-zero 'oSize' preamble field)
 * duplicate the second long word (i.e., the word containing the 'rType' and
 * 'oSize' fields of the preamble.  New containers with 32 size fields
 * duplicate the 'nSize' field of the preamble.
 *---------------------------------------------------------------------------
 */
struct M_CEndMarker {
    unsigned int size;
};

/*****  Access Macros  *****/
#define M_CEndMarker_Size(p)	((p)->size)


/*******************************************
 *******************************************
 *****  Data Base Update Declarations  *****
 *******************************************
 *******************************************/

#define M_SizeOfPreambleType (sizeof (M_CPreamble))
#define M_SizeOfEndMarker (sizeof (M_CEndMarker))

#define M_SetContainerSize(container, size) (\
    M_CPreamble_OSize (container) = 0,\
    M_CPreamble_NSize (container) = (size)\
)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  m.d 2 replace /users/mjc/system
  860321 17:53:31 mjc Release 1 of new memory architecture

 ************************************************************************/
/************************************************************************
  m.d 3 replace /users/mjc/system
  860323 17:22:16 mjc Release of additional work on new memory architecture

 ************************************************************************/
/************************************************************************
  m.d 4 replace /users/mjc/system
  860327 16:03:33 mjc Added container end markers and some framing tests

 ************************************************************************/
/************************************************************************
  m.d 5 replace /users/mjc/system
  860408 23:09:46 mjc Added standard CPD allocation capability

 ************************************************************************/
/************************************************************************
  m.d 6 replace /users/mjc/system
  860410 19:17:27 mjc Added standard CPDs, POP testing, and default printer

 ************************************************************************/
/************************************************************************
  m.d 7 replace /users/mjc/system
  860414 12:44:04 mjc Release 'M_ShiftContainerTail', delete 'M_Browser' calls

 ************************************************************************/
/************************************************************************
  m.d 8 replace /users/mjc/system
  860501 01:39:05 mjc Added 'CPD_POPReferenceTo...' and POP reference testing macros

 ************************************************************************/
/************************************************************************
  m.d 9 replace /users/hjb/system
  860517 19:00:59 hjb added declarations for User Spaces

 ************************************************************************/
/************************************************************************
  m.d 10 replace /users/jck/system
  860530 10:42:17 jck Added M_ComparePOPs macro

 ************************************************************************/
/************************************************************************
  m.d 11 replace /users/mjc/system
  860605 16:30:34 mjc Release 'M_CPDContainersIdentical' macros

 ************************************************************************/
/************************************************************************
  m.d 12 replace /users/mjc/system
  860607 12:14:40 mjc Added 'M_DuplicateCPDPtr' and CPD reference counts

 ************************************************************************/
/************************************************************************
  m.d 13 replace /users/mjc/system
  860615 18:57:52 mjc Deleted 'M_RTD_Browser...' declaration, added standard CPD free list slot to RTD, changed built-in os to nil object os

 ************************************************************************/
/************************************************************************
  m.d 14 replace /users/jck/system
  860619 16:19:21 jck Object Space Segment Preamble Type Added

 ************************************************************************/
/************************************************************************
  m.d 15 replace /users/jck/system
  860711 16:55:28 jck Release Object Saver

 ************************************************************************/
/************************************************************************
  m.d 16 replace /users/jck/system
  860723 12:33:38 jck Added DataBase Update Utility Macros

 ************************************************************************/
/************************************************************************
  m.d 17 replace /users/jad/system
  860731 17:47:10 jad added DBUPDATE utilities

 ************************************************************************/
/************************************************************************
  m.d 18 replace /users/mjc/rsystem
  860813 18:23:03 mjc Implemented statistics query/display, simple optimization of CPD allocator

 ************************************************************************/
/************************************************************************
  m.d 19 replace /users/mjc/system
  860819 12:31:56 mjc Added experimental fast preamble pointer access routine

 ************************************************************************/
/************************************************************************
  m.d 20 replace /users/jad/system
  860822 14:22:08 jad added Free Instance Counters

 ************************************************************************/
/************************************************************************
  m.d 21 replace /users/jad/system
  860826 14:42:31 jad changed the CPD and their pointer array to be in one
piece of memory

 ************************************************************************/
/************************************************************************
  m.d 22 replace /users/jck/system
  860828 09:45:13 jck Changed POP size from 64 bits to 32

 ************************************************************************/
/************************************************************************
  m.d 23 replace /users/jad/system
  860914 11:15:15 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  m.d 24 replace /users/mjc/system
  861002 18:03:08 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  m.d 25 replace /users/jad/system
  861010 14:06:38 jad added a field to the RTD table.  The RTD Table now
contains total cpd allocation and deallocation counts.

 ************************************************************************/
/************************************************************************
  m.d 26 replace /users/mjc/system
  861112 09:34:41 mjc Implement garbage collector lock for CPDs

 ************************************************************************/
/************************************************************************
  m.d 27 replace /users/mjc/system
  870215 23:05:05 lcn Release of new memory manager

 ************************************************************************/
/************************************************************************
  m.d 28 replace /users/mjc/system
  870326 19:27:53 mjc Implemented restricted update capability

 ************************************************************************/
/************************************************************************
  m.d 29 replace /users/jck/system
  870415 09:54:45 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  m.d 30 replace /users/mjc/MERGE/vax
  870519 17:16:28 mjc Added omitted ';' in 'Byte array[6]' declaration

 ************************************************************************/
/************************************************************************
  m.d 31 replace /users/mjc/translation
  870524 09:39:41 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  m.d 32 replace /users/jck/system
  871007 13:39:46 jck Release implementation of Global Persistent Garbage Collector

 ************************************************************************/
/************************************************************************
  m.d 33 replace /users/jck/system
  880422 06:39:37 jck Added IsVolatile declaration to CPD's typedef

 ************************************************************************/
/************************************************************************
  m.d 34 replace /users/jck/system
  890222 14:33:04 jck Adding a consistency checker function to M's rtype descriptors

 ************************************************************************/
/************************************************************************
  m.d 35 replace /users/m2/backend
  890503 15:24:15 m2 Changed volatile attr definition

 ************************************************************************/
/************************************************************************
  m.d 36 replace /users/m2/backend
  890828 17:08:41 m2 Moved ValidateContainerFraming macro from ts.c

 ************************************************************************/

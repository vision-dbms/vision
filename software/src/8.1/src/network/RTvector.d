/*****  Vector Representation Type Shared Declarations  *****/
#ifndef rtVECTOR_D
#define rtVECTOR_D

/*****************************************
 *****  U-Segment Descriptor Format  *****
 *****************************************/
/*---------------------------------------------------------------------------
 * A U-Segment is a homogeneous subset of a vector.  Homogeneity is defined as
 * common type for the primitive types (integer, float, double, date, etc.),
 * and a common value store for the non-primitive types.
 *
 * U-Segment Descriptor Field Descriptions:
 *	pToken			- a POP for the P-Token describing the
 *				  positional state associated with this
 *				  U-Segment.  Because vectors expect to have
 *				  complete control of their U-Segments, this
 *				  must always be a terminal P-Token.  Because
 *				  of a number of structural optimizations in
 *				  some of the vector algorithms, this MUST be
 *				  the first field in the 'struct'.
 *	vStore			- a POP for the value store referenced by
 *				  this vector segment.
 *	values			- a POP for a U-Vector containing/referencing
 *				  the values held in this segment.
 *---------------------------------------------------------------------------
 */
struct rtVECTOR_USDType {
    M_POP
	pToken,
	vStore,
	values;
};


/*****  Access Macros  *****/
#define rtVECTOR_USD_PToken(p)\
    (p)->pToken

#define rtVECTOR_USD_VStore(p)\
    (p)->vStore

#define rtVECTOR_USD_Values(p)\
    (p)->values

/*****  USD POP Count Macro  *****/
#define rtVECTOR_USD_POPsPerUSD\
    (sizeof (rtVECTOR_USDType) / sizeof (M_POP))

/*****  USD 'IsFree' Determination  *****/
#define rtVECTOR_USD_IsFree(cpd, p) (\
    (cpd)->ReferenceIsNil (&rtVECTOR_USD_PToken (p))\
)


/*******************************************
 *****  P-Map Range Descriptor Format  *****
 *******************************************/
/*---------------------------------------------------------------------------
 * A vector's P-Map is a specialized form of link which associates ranges of
 * positions in the vector with ranges of values in a U-Segment.  Like a link,
 * a P-Map is an array of transition descriptors sorted by vector position.
 * The last range descriptor in a P-Map always contains the number of elements
 * in the vector.  'PMRD's do not explicitly store the number of elements
 * they describe; that information is determined from the vector origins of a
 * 'pmrd' and its successor.
 *
 *  P-Map Range Descriptor Field Descriptions:
 *	vectorOrigin		- the origin of the range described by this
 *				  descriptor relative to the positional state
 *				  of the vector.
 *	segmentIndex		- the index of the U-Segment Descriptor Array
 *				  entry referenced by this 'pmrd'.  U-Segment
 *				  zero always holds the undefined values in a
 *				  vector. 
 *	segmentOrigin		- the origin of the range described by this
 *				  descriptor relative to the positional state
 *				  of the U-Segment.
 *---------------------------------------------------------------------------
 */
struct rtVECTOR_PMRDType {
    int
	vectorOrigin,
	segmentIndex,
	segmentOrigin;
};

/*****  Access Macros  *****/
#define rtVECTOR_PMRD_VectorOrigin(p)\
    (p)->vectorOrigin

#define rtVECTOR_PMRD_SegmentIndex(p)\
    (p)->segmentIndex

#define rtVECTOR_PMRD_SegmentOrigin(p)\
    (p)->segmentOrigin

#define rtVECTOR_PMRD_SegmentLength(p) (\
    rtVECTOR_PMRD_VectorOrigin ((p) + 1) - rtVECTOR_PMRD_VectorOrigin (p)\
)


/***************************
 *****  Vector Format  *****
 ***************************/
/*---------------------------------------------------------------------------
 * A vector consists of four components - a Header, a P-Map, a U-Segment Array,
 * and a U-Segment Index.  The header is a simple structure which specifies
 * the positional state of the vector and the sizes and relative locations of
 * the P-Map, U-Segment List, and U-Segment Index.  The P-Map is an array of
 * PMRD's sorted by vector origin identifying transitions from objects of one
 * 'type' to another.  The U-Segment Array is an array of U-Segment Descriptors
 * for the various types present in the vector.  Finally, the U-Segment Index
 * is a vector of integer subscripts into the U-Segment Array ordering that
 * array in a manner suitable for binary search by the internal homogeneity
 * defining routines.
 *
 * Vector Field Descriptions:
 *	pToken			- the P-Token associated with the positional
 *				  state of the vector.
 *	isASet			- true if the vector is a set vector, false
 *		                  otherwise.
 *	pMapSize		- the number of entries in the P-Map.  The
 *				  value in this field does NOT include the
 *				  P-Map entry which holds the vector's element
 *				  count.
 *	pMapTransition		- the P-Map position at which the transition
 *				  from 'short' to 'long' form PMRD's occurs.
 *				  Since 'short' form PMRD's are not currently
 *				  implemented, this value is always zero.
 *	uSegArraySize		- the number of entries in the U-Segment Array.
 *	uSegFreeBound		- the lowest possible position at which a free
 *				  entry can be found in the U-Segment Array.
 *				  It is not necessarily true that this entry is
 *				  free; just that no entry below this bound is
 *				  free.
 *	uSegIndexSize		- the size of the U-Segment Index.  This is the
 *				  number of unique homogeneous segments defined
 *				  for the vector.
 *	pMap			- the P-Map of the vector.  The actual size of
 *				  the P-Map is dynamic and determined at run
 *				  time; however, the P-Map always has at least
 *				  one entry at its end which contains the
 *				  number of elements in the vector.  
 *                                The VectorOrigin of the last P-Map entry
 *                                contains the number of elements in the vector.
 *                                The SegmentIndex of the last P-Map entry
 *                                must contain -1.
 *	uSegArray		- the U-Segment Array for the vector.  The
 *				  U-Segment Array always immediately follows
 *				  the P-Map in the vector, but, for obvious
 *				  reasons, cannot be explicitly included in
 *				  the type definition in a meaningful way.
 *	uSegIndex		- the U-Segment Index for the vector.  The
 *				  U-Segment Index is an array of integer
 *				  subscripts into 'uSegArray'.  It cannot be
 *				  included in the type definition for the same
 *				  reasons that 'uSegArray' cannot.
 *---------------------------------------------------------------------------
 */


struct rtVECTOR_Type {
    M_POP
	pToken;
    unsigned int
	isASet		: 1, 
	isInconsistent  :  1, 
	pMapSize	: 30;
    int
	pMapTransition,
	uSegArraySize,
	uSegFreeBound,
	uSegIndexSize;
    rtVECTOR_PMRDType
	pMap[1];
};


/*****  Access Macros  *****/
#define rtVECTOR_V_PToken(p)\
    (p)->pToken

#define rtVECTOR_V_IsASet(p)\
    (p)->isASet

#define rtVECTOR_V_PMapSize(p)\
    (p)->pMapSize

#define rtVECTOR_V_IsInconsistent(p)\
    (p)->isInconsistent

#define rtVECTOR_V_PMapTransition(p)\
    (p)->pMapTransition

#define rtVECTOR_V_USegArraySize(p)\
    (p)->uSegArraySize

#define rtVECTOR_V_USegFreeBound(p)\
    (p)->uSegFreeBound

#define rtVECTOR_V_USegIndexSize(p)\
    (p)->uSegIndexSize

#define rtVECTOR_V_PMap(p)\
    (p)->pMap

#define rtVECTOR_V_USegArray(p)\
    ((rtVECTOR_USDType *)(rtVECTOR_V_PMap (p) + rtVECTOR_V_PMapSize (p) + 1))

#define rtVECTOR_V_USegIndex(p)\
    ((int *)(rtVECTOR_V_USegArray (p) + rtVECTOR_V_USegArraySize (p)))

/*****  Vector Element Count Access Macro  *****/
#define rtVECTOR_V_ElementCount(p)\
    rtVECTOR_PMRD_VectorOrigin (rtVECTOR_V_PMap (p) + rtVECTOR_V_PMapSize (p))

/*****  Vector Sizing Macro  *****/
/*---------------------------------------------------------------------------
 * 'pMapSize' should not include the PMRD used to hold the vector's element
 * count.
 *---------------------------------------------------------------------------
 */
#define rtVECTOR_V_SizeofVector(pMapSize, uSegArraySize, uSegIndexSize)\
    (sizeof (rtVECTOR_Type) +\
        (pMapSize)	* sizeof (rtVECTOR_PMRDType) +\
	(uSegArraySize) * sizeof (rtVECTOR_USDType)  +\
	(uSegIndexSize) * sizeof (int))

/*****  U-Segment Array Initial Allocation and Increment  *****/
#define rtVECTOR_V_USegArrayInitialSize		4

#define rtVECTOR_V_USegArrayGrowthInc	8


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define rtVECTOR_Content(preamblePtr)\
    ((rtVECTOR_Type *)((M_CPreamble*)(preamblePtr) + 1))

#define rtVECTOR_HeaderSize\
    (sizeof (rtVECTOR_Type) - sizeof (rtVECTOR_PMRDType))

#define rtVECTOR_TailSize(preamblePtr)\
    (M_CPreamble_Size ((M_CPreamble*)(preamblePtr)) -\
        sizeof (rtVECTOR_Type) -\
	rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr))\
	    * sizeof (rtVECTOR_PMRDType))

#define rtVECTOR_Tail(preamblePtr)\
    ((pointer_t)\
        ((pointer_t)preamblePtr +\
	    M_SizeOfPreambleType +\
	    sizeof (rtVECTOR_Type) +\
	    rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr))\
		* sizeof (rtVECTOR_PMRDType)))

#define rtVECTOR_SetPMapSize(preamblePtr, size)\
    (rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr)) = (size))

#define rtVECTOR_PMapCount(preamblePtr)\
    (rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr)))

#define rtVECTOR_PMapSize(preamblePtr)\
    (rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr)) *\
		    sizeof (rtVECTOR_PMRDType))

#define rtVECTOR_PMapPtr(preamblePtr)\
    (rtVECTOR_V_PMap (rtVECTOR_Content (preamblePtr)))

#define rtVECTOR_ElementCount(preamblePtr)\
    (rtVECTOR_V_ElementCount (rtVECTOR_Content (preamblePtr)))

#define rtVECTOR_FinalSegment(preamblePtr)\
    (rtVECTOR_PMRD_SegmentIndex (rtVECTOR_PMapPtr (preamblePtr) + \
				 rtVECTOR_PMapCount (preamblePtr) - 1))

#define rtVECTOR_SetPTokenPOP(preamblePtr, ptokenPOP)\
    (M_POP_D_AsInt \
        (rtVECTOR_V_PToken \
	    (rtVECTOR_Content (preamblePtr))) = ptokenPOP)

#define rtVECTOR_SetContainerSize(preamblePtr)\
    M_SetContainerSize\
	(preamblePtr,\
	 rtVECTOR_V_SizeofVector\
	    (rtVECTOR_V_PMapSize (rtVECTOR_Content (preamblePtr)),\
	     rtVECTOR_V_USegArraySize (rtVECTOR_Content (preamblePtr)),\
	     rtVECTOR_V_USegIndexSize (rtVECTOR_Content (preamblePtr))))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvector.d 1 replace /users/mjc/system
  860404 20:14:49 mjc Create Vector Representation Type

 ************************************************************************/
/************************************************************************
  RTvector.d 2 replace /users/mjc/system
  860422 22:56:51 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTvector.d 3 replace /users/mjc/system
  860504 18:46:00 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTvector.d 4 replace /users/mjc/system
  860515 13:29:28 mjc Intermediate release of value descriptors for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.d 5 replace /users/mjc/system
  860521 11:12:01 mjc Release for coordination with 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.d 6 replace /users/mjc/system
  860523 00:59:58 mjc Implemented vector align

 ************************************************************************/
/************************************************************************
  RTvector.d 7 replace /users/mjc/system
  860527 15:38:21 mjc Return vector to library for 'jck'

 ************************************************************************/
/************************************************************************
  RTvector.d 8 replace /users/jck/system
  860605 16:16:47 jck Implemented rtVECTOR_RefExtract

 ************************************************************************/
/************************************************************************
  RTvector.d 9 replace /users/mjc/system
  860606 13:26:02 mjc Added 'primitive function' USVT, eliminated 'RTypeVStorePArray...'

 ************************************************************************/
/************************************************************************
  RTvector.d 10 replace /users/jck/system
  860617 11:59:45 jck Conversion of Constructor Data Types begun. Return
to library for jad.

 ************************************************************************/
/************************************************************************
  RTvector.d 11 replace /users/mjc/system
  860623 10:24:22 mjc Converted to use new value descriptor macros, restructured assign operations

 ************************************************************************/
/************************************************************************
  RTvector.d 12 replace /users/jad/system
  860627 19:33:46 jad release assign, still working on extract

 ************************************************************************/
/************************************************************************
  RTvector.d 13 replace /users/jck/system
  860723 12:33:23 jck Added DataBase Update Utility Macros

 ************************************************************************/
/************************************************************************
  RTvector.d 14 replace /users/jck/system
  860725 16:44:44 jck Fixed type errors in macros

 ************************************************************************/
/************************************************************************
  RTvector.d 15 replace /users/mjc/system
  860728 13:54:39 mjc Release of changed preamble

 ************************************************************************/
/************************************************************************
  RTvector.d 16 replace /users/jad/system
  860731 17:45:24 jad added DBUPDATE utilties

 ************************************************************************/
/************************************************************************
  RTvector.d 17 replace /users/jad/system
  860803 09:48:12 mjc Changed data base update utility macros

 ************************************************************************/
/************************************************************************
  RTvector.d 18 replace /users/mjc/system
  861002 18:37:20 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  RTvector.d 19 replace /users/jad/system
  861112 10:50:58 jad add routines to convert a vector into a simple descriptor

 ************************************************************************/
/************************************************************************
  RTvector.d 20 replace /users/jck/updates
  870129 13:58:34 jck Macros added for the incorporator

 ************************************************************************/
/************************************************************************
  RTvector.d 21 replace /users/jad/system
  870317 15:16:22 jad implemented product primitive

 ************************************************************************/
/************************************************************************
  RTvector.d 22 replace /users/jad/system
  870519 19:29:04 jad partial implementation of set vectors

 ************************************************************************/
/************************************************************************
  RTvector.d 23 replace /users/mjc/translation
  870524 09:36:25 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvector.d 24 replace /users/jck/updates
  880112 12:58:29 jck RTlink.h is now conditionally included

 ************************************************************************/
/************************************************************************
  RTvector.d 25 replace /users/jck/system
  890223 09:19:00 jck Fortified against saving corrupted vectors. Reimplemented Vector reconstruction

 ************************************************************************/

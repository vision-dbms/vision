/*****  List Store Representation Type Shared Declarations  *****/
#ifndef rtLSTORE_D
#define rtLSTORE_D

/****************************
 *****  L-Store Format  *****
 ****************************/
/*---------------------------------------------------------------------------
 * An L-Store is used to store a collection of partitioned lists.  An L-Store
 * consists of two components - a storage vector and a breakpoint array.  The
 * storage vector contains the elements of the concatenated lists; the break-
 * point array contains the boundaries between the lists.  Currently, L-Stores
 * are used to model lists (an 'RTvector' storage vector) and strings (an
 * 'RTcharuv' storage vector).
 *
 *  List Store Field Descriptions
 *	rowPToken		- a P-Token for the set of lists (the rows) in
 *				  this L-Store.
 *	content			- a vector or u-vector containing the elements
 *				  of the concatenated lists.
 *	contentPToken		- a P-Token for the positional state of the
 *				  storage vector to which the elements of the
 *				  breakpoint array refer.  Currently, it is an
 *				  error if this P-Token is not a base P-Token.
 *	stringStore		- a boolean which, when true, designates that
 *				  this L-Store is being used to hold strings.
 *	breakpointCount		- the size of the breakpoint array AND the
 *				  number of lists in this L-Store.  This value
 *				  should match the 'rowPToken' base element
 *				  count.
 *	breakpointArray		- a list of the boundaries between the lists
 *				  in 'content'.  There is always one more
 *				  element in the breakpoint array than the
 *				  number of lists in the L-Store.  The elements
 *				  of 'list[i]' begin at 'breakpointArray[i]';
 *				  the number of elements in 'list[i]' is
 *				  'breakpointArray[i+1] - breakpointArray[i]'.
 *				  The breakpoint array corresponding to the
 *				  lists: {a,b,c}, {p,q}, {}, {v,w,x,y,z} is:
 *				  {0, 3, 5, 5, 10}.  Although declared to
 *				  contain one element, the actual size of this
 *				  this array is dynamic and determined at run
 *				  time.
 *---------------------------------------------------------------------------
 */
typedef struct rtLSTORE_LStore {
    M_POP
	rowPToken,
	content,
	contentPToken;
    unsigned int
    	stringStore	:  1,
	isInconsistent	:  1, 
	breakpointCount	: 30,
	breakpointArray[1];
} rtLSTORE_Type_LStore;


/*****  Access Macros  *****/
#define rtLSTORE_LStore_RowPToken(p)		((p)->rowPToken)
#define rtLSTORE_LStore_Content(p)		((p)->content)
#define rtLSTORE_LStore_ContentPToken(p)	((p)->contentPToken)
#define rtLSTORE_LStore_StringStore(p)		((p)->stringStore)
#define rtLSTORE_LStore_BreakpointCount(p)	((p)->breakpointCount)
#define rtLSTORE_LStore_IsInconsistent(p)	((p)->isInconsistent)
#define rtLSTORE_LStore_BreakpointArray(p)	((p)->breakpointArray)

/*****  L-Store Sizing Macro  *****/
#define rtLSTORE_SizeofLStore(nbreakpoints) (\
    sizeof (rtLSTORE_LStore) + sizeof (int) * (nbreakpoints)\
)


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define rtLSTORE_StringStoreItem    "stringstore"

#define rtLSTORE_Content(preamblePtr)\
    ((rtLSTORE_LStore *)((M_CPreamble*)(preamblePtr) + 1))

#define rtLSTORE_Size(count)\
    rtLSTORE_SizeofLStore(count) + M_SizeOfPreambleType + M_SizeOfEndMarker

#define rtLSTORE_RowPToken(preamblePtr)\
        (rtLSTORE_LStore_RowPToken \
	    (rtLSTORE_Content (preamblePtr)))

#define rtLSTORE_ContentPToken(preamblePtr)\
        (rtLSTORE_LStore_ContentPToken \
	    (rtLSTORE_Content (preamblePtr)))

#define rtLSTORE_SetPTokenPOP(preamblePtr, ptokenPOP)\
    (M_POP_D_AsInt \
        (rtLSTORE_RowPToken (preamblePtr)) = ptokenPOP)

#define rtLSTORE_HeaderSize\
    (sizeof (rtLSTORE_LStore) - sizeof (int))

#define rtLSTORE_SetBreakpointCount(preamblePtr, count)\
    (rtLSTORE_LStore_BreakpointCount\
         (rtLSTORE_Content (preamblePtr)) = (count))

#define rtLSTORE_BreakpointCount(preamblePtr)\
    (rtLSTORE_LStore_BreakpointCount\
         (rtLSTORE_Content (preamblePtr)))

#define rtLSTORE_BreakpointArray(preamblePtr)\
    (rtLSTORE_LStore_BreakpointArray \
	    (rtLSTORE_Content (preamblePtr)))

#define rtLSTORE_StringContentSize(preamblePtr)\
    ((3 + rtLSTORE_BreakpointArray (preamblePtr)\
	        [rtLSTORE_BreakpointCount (preamblePtr)])/4 * 4)

#define rtLSTORE_SetContainerSize(preamblePtr)\
    M_SetContainerSize\
	(preamblePtr,\
	 (sizeof (rtLSTORE_LStore) + sizeof (int) *\
	        (rtLSTORE_LStore_BreakpointCount\
		     (rtLSTORE_Content (preamblePtr)))))

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlstore.d 1 replace /users/mjc/system
  860404 20:13:19 mjc Create List Store Representation Type

 ************************************************************************/
/************************************************************************
  RTlstore.d 2 replace /users/mjc/system
  860530 14:38:19 mjc No further work for now

 ************************************************************************/
/************************************************************************
  RTlstore.d 3 replace /users/mjc/system
  860616 10:03:24 mjc Changed name from 'RTpartitn' to 'RTlstore'

 ************************************************************************/
/************************************************************************
  RTlstore.d 4 replace /users/mjc/system
  860709 10:18:34 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTlstore.d 5 replace /users/mjc/system
  860714 10:52:57 mjc Return L-Store for access by 'spe'

 ************************************************************************/
/************************************************************************
  RTlstore.d 6 replace /users/mjc/system
  860728 13:50:41 mjc Prerelease for db update work

 ************************************************************************/
/************************************************************************
  RTlstore.d 7 replace /users/jck/system
  860910 14:15:33 jck Added data base update utility macros

 ************************************************************************/
/************************************************************************
  RTlstore.d 8 replace /users/jad/system
  870123 16:14:17 jad modified the DataBase Update Dump lstore output

 ************************************************************************/
/************************************************************************
  RTlstore.d 9 replace /users/jck/updates
  870129 13:58:31 jck Macros added for the incorporator

 ************************************************************************/
/************************************************************************
  RTlstore.d 10 replace /users/mjc/translation
  870524 09:33:41 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlstore.d 11 replace /users/jck/updates
  870824 14:56:26 jck Added Macros for the incorporator's use

 ************************************************************************/
/************************************************************************
  RTlstore.d 12 replace /users/jck/system
  890223 12:36:14 jck Fortified against saving corrupted lstores. Implemented Comprehensive Align

 ************************************************************************/

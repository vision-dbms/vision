/*****  Link Representation Type Shared Declarations  *****/
#ifndef rtLINK_D
#define rtLINK_D

/***********************************************
 *****  Range Reference Descriptor Types   *****
 ***********************************************/

#define rtLINK_RRDType_Range	    0
#define rtLINK_RRDType_Repeat	    2
/*---------------------------------------------------------------------------
 * Reserved for future use
 *  #define rtLINK_RRDType_Sequence	    1
 *  #define rtLINK_RRDType_SeqToo	    3
 *---------------------------------------------------------------------------
 */
/***********************************************
 *****  Range Reference Descriptor Format  *****
 ***********************************************/
/*---------------------------------------------------------------------------
 * A range reference descriptor (rrd) defines a positional range of a link.
 * An 'rrd' can reference either a contiguous range of 'n' elements or a
 * single element 'n' times.  An 'rrd' is always part of a vector of 'rrd's
 * which completely describes every position in the link.  The number of
 * positions contributed to the link by the 'rrd' is the difference between
 * the 'linkCumulative' values of its successor and it.  The last 'rrd' in a
 * link always contains the number of positions the link appears to contain
 * when accessed positionally.
 *
 * Range Reference Descriptor Field Descriptions:
 *	linkCumulative		- the positional origin in the link of the
 *				  elements referenced by this 'rrd'.
 *	repeatedRef		- a boolean which when true specifies that
 *				  this link references a single element 'n'
 *				  times.
 *	referenceOrigin		- the origin of the range (!repeatedRef) or
 *				  element (repeatedRef) referenced by this
 *				  'rrd'.
 *---------------------------------------------------------------------------
 */
struct rtLINK_RRDType {
    unsigned int
	linkCumulative,
	repeatedRef	: 1,
	referenceOrigin	: 31;
};

/*****  Access Macros  *****/
#define rtLINK_RRD_D_LinkCumulative(rrd)((rrd).linkCumulative)
#define rtLINK_RRD_D_RepeatedRef(rrd)	((rrd).repeatedRef)
#define rtLINK_RRD_D_RefOrigin(rrd)	((rrd).referenceOrigin)

#define rtLINK_RRD_LinkCumulative(p)	((p)->linkCumulative)
#define rtLINK_RRD_RepeatedRef(p)	((p)->repeatedRef)
#define rtLINK_RRD_ReferenceOrigin(p)	((p)->referenceOrigin)

#define rtLINK_RRD_N(p, successorCount) (\
    (successorCount) - rtLINK_RRD_LinkCumulative (p)\
)


/*************************
 *****  Link Format  *****
 *************************/
/*---------------------------------------------------------------------------
 * A link represents an SORTED collection of positional references.
 * Essentially, a link is a special case of 'reference u-vector' which
 * references the elements of a positionally accessible object in
 * monotonically increasing order.  Because a link is a special case of
 * reference u-vector, it is a positionally accessible object.
 *
 * Link Field Descriptions:
 *	posPToken		- a POP for the P-Token describing the
 *				  positional state of this link.
 *	refPToken		- a POP describing the positional state
 *				  associated with the references made by
 *				  this link.
 *	hasRepeats		- true if any of the 'rrd's are repetitions.
 *	hasRanges		- true if any of the 'rrd's are ranges.
 *	tracking		- used by align to determine whether to split
 *                                or enlarge ranges.
 *	rrdCount		- the number of 'rrd's in the 'rrdArray'.
 *				  The value in this field does NOT include
 *				  the 'rrdArray' element which holds the
 *				  final cumulative.
 *	rrdArray		- the array of 'rrd's for the elements
 *				  referenced by this link.  The last element
 *				  of this array contains the size of the
 *				  link when viewed as a positionally
 *				  accessible object.
 *---------------------------------------------------------------------------
 */
struct rtLINK_Type {
    M_POP
	posPToken,
	refPToken;
    unsigned int
	tracking	:  1, 
	hasRepeats	:  1, 
	hasRanges	:  1, 
	isInconsistent	:  1, 
	rrdCount	: 28;
    rtLINK_RRDType
	rrdArray[1];
};


/*****  Access Macros  *****/
#define rtLINK_L_PosPToken(p)		((p)->posPToken)
#define rtLINK_L_RefPToken(p)		((p)->refPToken)
#define rtLINK_L_Tracking(p)		((p)->tracking)
#define rtLINK_L_HasRepeats(p)		((p)->hasRepeats)
#define rtLINK_L_HasRanges(p)		((p)->hasRanges)
#define rtLINK_L_IsInconsistent(p)	((p)->isInconsistent)
#define rtLINK_L_RRDCount(p)		((p)->rrdCount)
#define rtLINK_L_RRDArray(p)		((p)->rrdArray)

/*****  Range Reference Descriptor Element Count  *****/
/*---------------------------------------------------------------------------
 * The following macro determines the number of elements in an RRD contained
 * in the RRD array of a link.  Note that the parameter to this macro is a
 * pointer to an RRD and NOT a pointer to a link!
 *---------------------------------------------------------------------------
 */
#define rtLINK_L_N(rrdp) rtLINK_RRD_N (rrdp, rtLINK_RRD_LinkCumulative ((rrdp) + 1))


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlink.d 1 replace /users/mjc/system
  860421 12:29:50 mjc Create link representation type

 ************************************************************************/
/************************************************************************
  RTlink.d 2 replace /users/mjc/system
  860422 22:55:54 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  RTlink.d 3 replace /users/mjc/system
  860504 18:45:50 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  RTlink.d 4 replace /users/mjc/system
  860513 09:28:51 mjc Adjusted 'TraverseRRDCList' to deal with empty lists

 ************************************************************************/
/************************************************************************
  RTlink.d 5 replace /users/jad/system
  860523 15:33:48 jad release link to link extractions

 ************************************************************************/
/************************************************************************
  RTlink.d 6 replace /users/jad/system
  860605 18:18:41 jad implement 1st version of Align, add ref cnt 
to linkc's, and remove references to valuedsc refuv

 ************************************************************************/
/************************************************************************
  RTlink.d 7 replace /users/jad/system
  860612 15:02:56 jad fully implemented align, added ToLinkNoDiscard,
discard constructor now zeros out chain head and tail

 ************************************************************************/
/************************************************************************
  RTlink.d 8 replace /users/mjc/system
  860623 10:21:57 mjc Converted to use new 'valuedsc' macros

 ************************************************************************/
/************************************************************************
  RTlink.d 9 replace /users/mjc/system
  860709 15:30:22 jad changed traverse macro to split a range
if it contains a reference nil 

 ************************************************************************/
/************************************************************************
  RTlink.d 10 replace /users/jad/system
  860710 20:51:44 jad value descriptor changes complete

 ************************************************************************/
/************************************************************************
  RTlink.d 11 replace /users/jad/system
  860723 16:42:05 jad added ToLink routine with a new pos ptoken

 ************************************************************************/
/************************************************************************
  RTlink.d 12 replace /users/jad/system
  860725 18:51:15 jad made the traverse macro safer

 ************************************************************************/
/************************************************************************
  RTlink.d 13 replace /users/jad/system
  860903 12:56:03 jad 1) added hasRepeats and hasRanges fields to both links and linkcs 2) added 2 simple link creation routines

 ************************************************************************/
/************************************************************************
  RTlink.d 14 replace /users/jad/system
  861205 17:32:05 jad added LocateOrAdd, and Lookup routines

 ************************************************************************/
/************************************************************************
  RTlink.d 15 replace /users/jad/system
  870119 12:55:04 jad added a traversal macro with an extra arg.

 ************************************************************************/
/************************************************************************
  RTlink.d 16 replace /users/jad/system
  870128 19:03:04 jad added a rtLINK_LC_IsOpen define

 ************************************************************************/
/************************************************************************
  RTlink.d 17 replace /users/jck/system
  870513 11:36:59 jck Added rtLINK_LookupCase_GT

 ************************************************************************/
/************************************************************************
  RTlink.d 18 replace /users/mjc/translation
  870524 09:33:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlink.d 19 replace /users/jad/system/real
  871103 14:58:43 jad fixed bug caused by
the simple case optimization of rtVECTOR_LCExtract when selecting a
 'referenceNil' value.

 ************************************************************************/
/************************************************************************
  RTlink.d 21 replace /users/jck/system
  880826 09:41:26 jck Removing sequence links from the main line of descent

 ************************************************************************/
/************************************************************************
  RTlink.d 22 replace /users/jck/system
  890223 08:27:42 jck Fortified against saving links which have been corrupted

 ************************************************************************/

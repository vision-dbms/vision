/*****  Miscellaneous Utilities  *****/

/**************************
 *****  Facility Name *****
 **************************/

#define FacilityName UTIL

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "vutil.h"

/*****  Supporting *****/
#include "verr.h"


/*********************
 *********************
 *****  Sorters  *****
 *********************
 *********************/

/**************************
 *  Quick Sort Utilities  *
 **************************/

/*---------------------------------------------------------------------------
 *****  Macro to determine if one (value, index) pair is < (>) than another.
 *
 *  Arguments:
 *	array		-  the array of elements being sorted.
 *	indices		-  an array of integer indices into 'array'. The qsort
 *			   algorithm swaps these indices instead of the array 
 *			   elements themselves. At the completion of the qsort
 *			   operation, these indices can be used to extract the
 *			   elements of 'array' in sorted order.
 *	i1		-  the position in 'indices' of the first element.
 *	i2		-  the position in 'indices' of the second element.
 *	value2		-  array[indices[i2]], where used.
 *	index2		-  indices[i2], where used.
 *	descending	-  a boolean, which when true, computes descending
 *			   sort indices.  Used here to implement a stable
 *			   tie breaking algorithm.
 *
 *  'QSortLT' Returns:
 *	true if array[indices[i1]] < array[indices[i2]], false otherwise
 *  'QSortGT' Returns:
 *	true if array[indices[i1]] > array[indices[i2]], false otherwise
 *
 *****/

#define QSortLT(array, indices, i1, i2, descending)\
QSortLTE (array, indices, i1, array[indices[i2]], indices[i2], descending)

#define QSortGT(array, indices, i1, i2, descending)\
QSortGTE (array, indices, i1, array[indices[i2]], indices[i2], descending)

#define QSortLTE(array, indices, i1, value2, index2, descending) (\
    array[indices[i1]] <  (value2) ||\
    array[indices[i1]] == (value2) && (\
	(descending) ? indices[i1] > (index2) : indices[i1] < (index2)\
    )\
)

#define QSortGTE(array, indices, i1, value2, index2, descending) (\
    array[indices[i1]] >  (value2) ||\
    array[indices[i1]] == (value2) && (\
	(descending) ? indices[i1] < (index2) : indices[i1] > (index2)\
    )\
)


/*---------------------------------------------------------------------------
 *****  Macro to define the input partitioning function for a quick sort.
 *
 *  Arguments:
 *	array		-  the array of elements being sorted.
 *	indices		-  an array of integer indices into 'array'. The qsort
 *			   algorithm swaps these indices instead of the array 
 *			   elements themselves. At the completion of the qsort
 *			   operation, these indices can be used to extract the
 *			   elements of 'array' in sorted order.
 *	left		-  an integer index into 'indices' defining the left
 *			   element of this partition.
 *	right		-  an integer index into 'indices' defining the right
 *			   element of this partition.
 *	descending	-  a boolean, which when true, computes descending
 *			   sort indices.  Used here to implement a stable
 *			   tie breaking algorithm.
 *	elementType	-  the type of element being sorted by this
 *			   quick sort.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *****/
#define Partition(array, indices, left, right, descending, elementType) {\
    unsigned int	_right = right,	j = right;\
    unsigned int	temp, index = indices[_right];\
    elementType	value = array[index];\
\
    i = left;\
    while (j > i) {\
        while (\
            ++i, QSortLTE (array, indices, i, value, index, descending) && i < j\
        );\
	while (\
	    --j, QSortGTE (array, indices, j, value, index, descending) && i < j\
	);\
	temp = indices [i];\
	indices [i] = indices [j];\
	indices [j] = temp;\
    }\
\
    indices [j] = indices [i];\
    indices [i] = indices [_right];\
    indices [_right] = temp;\
}


/*---------------------------------------------------------------------------
 *****  Macro to define the median of 3 function for a quick sort.
 *
 *  Arguments:
 *	array		-  the array of elements being sorted.
 *	indices		-  an array of integer indices into 'array'. The qsort
 *			   algorithm swaps these indices instead of the array 
 *			   elements themselves. At the completion of the qsort
 *			   operation, these indices can be used to extract the
 *			   elements of 'array' in sorted order.
 *	left		-  an integer index into 'indices' defining the left
 *			   element of this partition.
 *	right		-  an integer index into 'indices' defining the right
 *			   element of this partition.
 *	descending	-  a boolean, which when true, computes descending
 *			   sort indices.  Used here to implement a stable
 *			   tie breaking algorithm.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *****/
#define MedianOf3(array, indices, left, right, descending) {\
    unsigned int	middle = left + (right - left)/2;\
    unsigned int	temp;\
\
    if (middle == left) {\
	if (QSortGT (array, indices, left, right, descending)) {\
	    temp = indices[left];\
	    indices [left] = indices [right];\
	    indices [right] = temp;\
	}\
    }\
    else {\
	if (QSortGT (array, indices, left, middle, descending)) {\
	    temp = indices[left];\
	    indices [left] = indices [middle];\
	    indices [middle] = temp;\
	}\
	if (QSortGT (array, indices, left, right, descending)) {\
	    temp = indices[left];\
	    indices [left] = indices [right];\
	    indices [right] = temp;\
	}\
	if (QSortGT (array, indices, middle, right, descending)) {\
	    temp = indices[middle];\
	    indices [middle] = indices [right];\
	    indices [right] = temp;\
	}\
	temp = indices [middle];\
	indices [middle] = indices [right - 1];\
	indices [right - 1] = temp;\
    }\
}


/*---------------------------------------------------------------------------
 *****  Macro to define a Q-Sorter and its partitioning function.
 *
 *  Arguments:
 *	sortFnName		- the name to be given to the main Q-Sort
 *				  routine.
 *	elementType		- the type of element to be sorted by this
 *				  Q-Sorter.
 *
 *  Syntactic Context:
 *	Function Definition(s)
 *
 *  Notes:
 *	This macro defines a public Q-Sort routine of four arguments -
 *		array		- the address of the array to be sorted.
 *		indices		- the address of a parallel array of integers
 *				  which will be filled with ASCENDING sort
 *				  indices for 'array'.
 *		n		- the number of elements in 'array' and
 *				  'indices'.
 *		descending	- a boolean, which when true, computes
 *				  descending sort indices.
 *
 *****/
#define DefineQSorter(sortFnName, elementType)\
PublicFnDef void sortFnName (\
    elementType const *array, unsigned int *indices, size_t n, bool descending\
) {\
    unsigned int i, left, right, stack[100], *sp;\
\
    for (i = 0; i < n; i++) indices [i] = i;\
    if (n < 2) return;\
\
    sp    = stack + 2;\
    left  = 0;\
    right = n - 1;\
    while (sp > stack) {\
	if (right > left) MedianOf3 (\
	    array, indices, left, right, descending\
	);\
	if (right > left + 2) {\
	    Partition (\
		array, indices, left, right - 1, descending, elementType\
	    );\
	    if (2 * i > left + right) {\
	        *sp++ = left;\
		*sp++ = i - 1;\
		left = i + 1;\
	    }\
	    else {\
		*sp++ = i + 1;\
		*sp++ = right;\
		right = i - 1;\
	    }\
	}\
	else {\
	    right = *--sp;\
	    left = *--sp;\
	}\
    }\
    if (descending) for (\
	left = 0, right = n;\
	left < right;\
	i = indices[--right],\
	indices[right] = indices[left],\
	indices[left++] = i\
    );\
}


/*************************
 *  Quick Sort Routines  *
 *************************/

DefineQSorter (UTIL_CharQSort		, char)
DefineQSorter (UTIL_DoubleQSort		, double)
DefineQSorter (UTIL_FloatQSort		, float)
DefineQSorter (UTIL_IntQSort		, int)
DefineQSorter (UTIL_Unsigned64QSort	, VkUnsigned64)
DefineQSorter (UTIL_Unsigned96QSort	, VkUnsigned96)
DefineQSorter (UTIL_Unsigned128QSort	, VkUnsigned128)


/***************************************
 *****  Number Formating Services  *****
 ***************************************/

/*---------------------------------------------------------------------------
 ***** Private routine to determine if a string representing a number has
 ***** a decimal point.
 *
 *  Arguments:
 *	string   - a pointer to the beginning of the string to search.  It will
 *		   be set to the position where the decimal point is.
 *
 *  Returns:
 *	true if a decimal point was found, false otherwise.
 *
 *****/
PrivateFnDef bool FindPeriod (char **string, int *pos) {
    bool notFound = true;
    char const *eptr = *string + strlen (*string);
    *pos = 0;

    if (**string == '-' || **string == '+') {
	(*string)++;
	(*pos)++;
    }

    while (eptr > *string && (notFound = (**string != '.'))) {
	(*string)++;
	(*pos)++;
    }

    return !notFound;
}


/*****  Routine to format a number with the specified format and commas.
 *
 *  Arguments:
 *
 *	buffer			- the buffer to output the formatted number to.
 *				  The caller must guarantee that this buffer
 *				  is at least 'abs(fieldSpecification) + 1' 
 *                                characters long.
 *	maxSize			- an integer specifying the maximum number of 
 *                                characters that could be output using a
 *                                format made up of fieldSpecification and
 *                                precision to format number. This number is
 *                                used to allocate a temporary internal buffer.
 *				  This number may be a guess, if so it must be 
 *                                larger than neccessary.
 *	overflowChar		- the character to replace the output with 
 *                                on overflow.
 *	fieldSpecification 	- an integer specifing the format's field 
 *				  width and justification.  The 
 *                                abs (fieldSpecification) denotes the field
 *				  width.  The sign specifies the justification.
 *			          If the number to be output 
 *				  takes up less space than the field width it
 *                                will be padded with blanks.  If the number
 *                                takes up more space than the field width it
 *                                'overflowed' and field width 'overflowChar's
 *                                will be output instead of the number.  If
 *                                the field size is negative, the padding 
 *                                occurs on the right, otherwise it occurs
 *                                on the left.  
 *	precision		- an integer specifing how many decimal
 *                                places to output.  
 *	number			- the number to format.
 *
 *  Returns:
 *	false if the number overflowed, true otherwise.
 *
 *  Examples:
 *	A fieldSpecification of 10 with a precision of 2 means 
 *	to format the number with 2 decimal places and output 
 *	10 characters with padding on the left if neccessary.
 *	123.456 formatted with this format produces:
 *		'    123.45'
 *
 *	Formatting the number 123.456 with a fieldSpecification of -10 and
 *	a precision of 2 pads on the right producing:
 *		'123.45    '
 *
 *	Formatting the number 1234.0 with a fieldSpecification of 3 and a 
 *	precision of 2 causes an overflow because 1234 cannot fit into 3
 *	characters.  Three 'overflowChar's will be output instead of the 
 *      number.
 *
 *  Notes:
 *	The number of characters output is always the abs (fieldSpecification).
 *	(Plus the '\0')
 *****/
PublicFnDef int UTIL_FormatNumberWithCommas (
    char *			buffer,
    size_t			maxSize,
    int				overflowChar,
    int				fieldSpecification,
    int				precision,
    double			number
)
{
    int				leftSpace = 0, rightSpace = 0,
				i, more;
    char			format[25], 
				*tptr, *tlptr;
	
#define HandleOverflow {\
/*****  Too big - replace with the overflow character  *****/\
    memset (buffer, overflowChar, fieldWidth);\
    buffer[fieldWidth] = '\0';\
\
    UTIL_Free (tmpBuffer);\
    return false;\
}


    size_t fieldWidth = abs (fieldSpecification);

    /*****  If field requires more space than allocated, punt...  *****/
    if (fieldWidth > maxSize) ERR_SignalFault (
	EC__InternalInconsistency,
	"UTIL_FormatNumberWithCommas: internal usage error, fieldWidth > maxSize"
    );

    /*****  Create a temporary buffer *****/
    char *tmpBuffer = (char *)UTIL_Malloc (maxSize+1);
    
/*****   
 *  Step 1 is to format the number with the specified number
 *  of decimals and let 'sprintf' determine the neccessary number 
 *  of whole digits.
 *****/
    if (precision == 0)
	/***** 
	 *  need to special case 'x.0' formats because '%g' truncates them
	 *  to 'x'
	 *****/
	strcpy (format,"%0.0f");
    else STD_sprintf (format, "%%0.%df", precision);
    
    int sz = STD_sprintf (tmpBuffer, format, number);

    /*****  If it doesn't fit - stop now  *****/
    if (sz < 0 || (size_t)sz > fieldWidth) HandleOverflow;
	
/*****
 *  Step 2 is to calculate the final size of the number.
 *****/
    char *spptr = tmpBuffer;
    int pos;
    bool isAPeriod = FindPeriod (&spptr, &pos);
    int numOfWholeDigits = isAPeriod ? pos : sz;

    if (tmpBuffer[0] == '-')
	numOfWholeDigits--;

    int numOfCommas = (numOfWholeDigits - 1) / 3;
    int totalSize = sz + numOfCommas;
    
    /*****  if the final size is too big - stop now *****/
    if ((size_t)totalSize > fieldWidth) HandleOverflow;

    /*****  ... else determine if we need to add white space... *****/
    if ((size_t)totalSize < fieldWidth)   
    {
	if (fieldSpecification > 0)
	    leftSpace = fieldWidth - totalSize;
	else
	    rightSpace = fieldWidth - totalSize;

	totalSize = fieldWidth;
    }

/*****  Check that 'maxSize' is <= 'totalSize'  *****/
    if ((size_t)totalSize > maxSize) ERR_SignalFault (
	EC__InternalInconsistency, 
	"UTIL_FormatNumberWithCommas: internal usage error, totalSize > maxSize"
    );

/*****
 *  Step 3 is to construct the number in the buffer ...
 *****/
    tptr = buffer;
    tlptr = tptr + (totalSize);
    *tlptr-- = '\0';
    
    char *sptr = tmpBuffer;
    char *slptr = sptr + (sz - 1);

    /*****  Output backwards the right spaces ...  *****/
    while (rightSpace > 0) {
	*tlptr-- = ' ';
	rightSpace--;
    }

    /*****  Output backwards up to and including the period ...  *****/
    if (isAPeriod)
	while (slptr >= spptr)
	    *tlptr-- = *slptr--;
	    
    /*****  Output backwards three digits and a comma ...  *****/
    while (slptr >= sptr) {
	for (i=0, more = slptr >= sptr;
	     more && i < 3;
	     more = slptr >= sptr, i++)

	    *tlptr-- = *slptr--;
	    
	if (more && *slptr != '-')  /* don't put a ',' before a leading '-' */
	    *tlptr-- = ',';
    }

    /*****  Output backwards the left spaces ...  *****/
    while (leftSpace > 0) {
	*tlptr-- = ' ';
	leftSpace--;
    }

    /***** 
     *  Testing: check that 'size - tptr' is at the beginning of the string
     *****/
    if (++tlptr != tptr) ERR_SignalFault (
	EC__InternalInconsistency, 
	"UTIL_FormatNumberWithCommas: computed size is wrong"
    );

    UTIL_Free (tmpBuffer);

    return true;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (UTIL);
    FAC_FDFCase_FacilityDescription ("Utilities");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  util.c 1 replace /users/mjc/system
  860226 16:26:13 mjc create

 ************************************************************************/
/************************************************************************
  util.c 2 replace /users/mjc/system
  860407 11:04:21 mjc Added 'Display{Expression, SizeofType} macros

 ************************************************************************/
/************************************************************************
  util.c 3 replace /users/jad/system
  860422 15:16:26 jad use new io module

 ************************************************************************/
/************************************************************************
  util.c 4 replace /users/mjc/system
  860430 20:00:28 mjc Added free list manager and radix list sorter

 ************************************************************************/
/************************************************************************
  util.c 5 replace /users/mjc/system
  861014 22:54:45 mjc Implemented Q-Sorters and 'ExpandFreeList'

 ************************************************************************/
/************************************************************************
  util.c 6 replace /users/mjc/system
  861109 00:40:16 mjc Fixed bug in 3-pass radix list sort

 ************************************************************************/
/************************************************************************
  util.c 7 replace /users/jad/system
  870429 16:49:31 jad defined a character qsort routine

 ************************************************************************/
/************************************************************************
  util.c 8 replace /users/mjc/translation
  870524 09:42:28 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  util.c 9 replace /users/mjc/system
  870607 03:12:14 mjc Add 'array-of-free-list' allocation utilities

 ************************************************************************/
/************************************************************************
  util.c 10 replace /users/mjc/system
  870614 20:12:16 mjc Made 'RadixListSort' machine independent

 ************************************************************************/
/************************************************************************
  util.c 11 replace /users/jck/system
  870810 14:15:01 jck Added counter to keep track of amount of space used by free lists

 ************************************************************************/
/************************************************************************
  util.c 12 replace /users/jad/system
  880805 13:43:32 jad Improve the performance on the PrintOf and SPrint primitives

 ************************************************************************/
/************************************************************************
  util.c 13 replace /users/jck/system
  881207 09:38:52 jck Fixed pointer dereferencing bug in 'FindPeriod' function

 ************************************************************************/
/************************************************************************
  util.c 14 replace /users/m2/backend
  890503 15:34:33 m2 STD_printf() and other fixes

 ************************************************************************/
/************************************************************************
  util.c 15 replace /users/jck/system
  890719 12:30:30 jck changed qsort algorithm to avoid degenerating to O(N2) when sorting sorted lists

 ************************************************************************/
/************************************************************************
  util.c 16 replace /users/jck/system
  890719 13:50:34 jck Corrected some comments on the qsort algorithm

 ************************************************************************/

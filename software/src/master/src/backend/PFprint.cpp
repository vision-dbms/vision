/*****  Print Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfPRINT

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"
#include "VConfig.h"

/*****  Facility  *****/
#include "m.h"
#include "rslang.h"
#include "selector.h"

#include "vdbupdate.h"
#include "vdates.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vprimfns.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTblock.h"
#include "RTclosure.h"
#include "RTcontext.h"
#include "RTlstore.h"
#include "RTmethod.h"
#include "RTptoken.h"
#include "RTstring.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"

#include "RTu64uv.h"
#include "RTu96uv.h"

#include "VOutputGenerator.h"

#include "V_VString.h"

/*****  Self  *****/
#include "PFprint.h"


/*******************************
 *******************************
 *****  Primitive Indices  *****
 *******************************
 *******************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that assign indices to the primitives.
 *
 * The numeric indices associated in this section with each of the primitives
 * are the system wide 'indices' by which the primitives are known.  These
 * indices are stored in permanent object memory;  consequently, once assigned,
 * they must not be changed or deleted.  To avoid an fatal error at system
 * startup, the largest index assigned in this section must be less than
 * 'V_PF_MaxPrimitiveIndex' (see "vprimfns.h").
 * If necessary this maximum can be increased.
 *---------------------------------------------------------------------------
 */
/*======================================*
 *  This module currently has indices:  *
 *	 120 - 139  and                 *
 *       276 - 280  and                 *
 *       400 - 410                      *
 *======================================*/


/******************************
 *****  Print Primitives  *****
 ******************************/

#define XPrintSelfAsPointer			120
#define XPrintCurrentAsPointer			121
#define XPrintSelf				122
#define XPrintCurrent				123
#define XPrintSelector				124
#define XPrintString				125

#define XPrintPrimitive				126
#define XPrintMethod				127
#define XPrintClosure				128
#define XSPrintPrimitive			277
#define XSPrintMethod				278
#define XSPrintClosure				279

#define XPrintDate				129
#define XPrintCurrentAs12Pointer		130
#define XPrint12String				131
#define XPrint24String				132

#define XIntegerPrintOf				133
#define XIntegerNicePrintOf			400
#define XDoublePrintOf				134
#define XDoubleNicePrintOf			280
#define XIntegerPrintWithCommasOf		136
#define XDoublePrintWithCommasOf		137
#define XIntegerSPrintOf			135
#define XDoubleSPrintOf				138
#define XIntegerSPrintWithCommasOf		139
#define XDoubleSPrintWithCommasOf		276


/******************************
 *****  Print Primitives  *****
 ******************************/

/*****
 *  The maximum allowable format field width.  The format field width
 *  is the whole part of the specified format.  Since the format can be
 *  a double number, the requested field width could be very large.  Eventually
 *  space for the formatted output must be allocated and 1e308 characters
 *  is too large.  In addition it needs to fit into an integer since it
 *  is passed around as an integer variable.  The number 'MaxFormatFieldWidth'
 *  was chosen arbitrarily, it can be set to anything as long as it fits
 *  into an integer.
 *****/
#define MaxFormatFieldWidth	10000	/* this must be less than INT_MAX */
					/* it has to fit into an integer */

/*****
 *  Since the 'x' in the '%xd' format only specifies the minimum size,
 *  the actual possible size is either the number of digits in the largest
 *  possible integer or the specified format field width - whichever is larger.
 *****/
#define MaxSizeOfIntegerNumber(fieldWidth)\
    (V_Max (MAXINTDIGITS, fieldWidth))

/* Include commas */
#define MaxSizeOfIntegerNumberWC(fieldWidth)\
    (MaxSizeOfIntegerNumber (fieldWidth) + ((MAXINTDIGITS - 1) / 3))

/*****
 *  The 'x' in the '%x.yf' format only specifies the minimum size,
 *  the actual possible size is either the number of digits in the
 *  largest whole double number or the format field width - whichever
 *  is larger.  (The '.y' size is calculated elsewhere.)
 *  Also, make allowance for a possible '-' sign .... ( ... + 1)
 *****/
#define MaxSizeOfWholeDoubleNumber(fieldWidth) V_Max (MAXDOUBLEDIGITS + 1, fieldWidth)

/* Include commas */
#define MaxSizeOfWholeDoubleNumberWC(fieldWidth) (\
    MaxSizeOfWholeDoubleNumber (fieldWidth) + (MAXDOUBLEDIGITS - 1) / 3\
)


/********************************
 *  Pointer Printing Utilities  *
 ********************************/

/*---------------------------------------------------------------------------
 *****  Macro to print a descriptor pointer as a pointer.
 *
 *  Arguments:
 *	fmtString		- a C 'printf' format string which will print
 *				  an element of the pointer.
 *	fmtSize			- an integer containing the maximum number of
 *                                characters that 'fmtString' will print.
 *	elementType		- the type of a pointer element.
 *	scalarAccessMacro	- the name of an access macro or routine which
 *				  returns the pointer value from a scalar
 *				  pointer.
 *	uvAccessMacro		- the name of an access macro or routine which
 *				  returns a standard CPD for a u-vector for the
 *				  elements to be printed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define PrintAValue(fmtString,fmtSize,elementType,scalarAccessMacro,uvAccessMacro) {\
    if (DucIsAScalar) pTask->outputBufferPrintScalar (\
	fmtSize, fmtString, scalarAccessMacro(ADescriptor)\
    );\
    else {\
	M_CPD *uv = uvAccessMacro (ADescriptor);\
	elementType\
	    *uvp = UV_CPD_Array (uv, elementType),\
	    *uvl = uvp + UV_CPD_ElementCount (uv);\
\
	VOutputGenerator iOutputGenerator (pTask);\
	while (uvp < uvl) {\
	    iOutputGenerator.printf (fmtSize, fmtString, *uvp++);\
	    iOutputGenerator.advance ();\
	}\
    }\
}


/*---------------------------------------------------------------------------
 *****  Routine to print the contents of 'ADescriptor' as a pointer.
 *
 *  Arguments:
 *	fieldWidth		- the field width of the printed result.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *  Notes:
 *	This routine silently ignores pointer types with which it cannot deal.
 *
 *****/
PrivateFnDef void PrintAAsPointer (VPrimitiveTask* pTask, int fieldWidth) {
    char fmtString[16];

/*****  Obtain and decode the R-Type of the accumulator pointer...  *****/
    switch (ADescriptor.pointerRType ()) {
    case RTYPE_C_CharUV:
	STD_sprintf (fmtString, "%%%dc", fieldWidth);
	PrintAValue (
	    fmtString,
	    V_Max (fieldWidth, 1),
	    rtCHARUV_ElementType,
	    DSC_Descriptor_Scalar_Char,
	    DSC_Descriptor_Value
	);
        break;
    case RTYPE_C_DoubleUV:
	STD_sprintf (fmtString, "%%%d.2f", fieldWidth);
	PrintAValue (
	    fmtString,
	    MaxSizeOfWholeDoubleNumber (fieldWidth) + 3,
	    rtDOUBLEUV_ElementType,
	    DSC_Descriptor_Scalar_Double,
	    DSC_Descriptor_Value
	);
        break;
    case RTYPE_C_FloatUV:
	STD_sprintf (fmtString, "%%%d.2f", fieldWidth);
	PrintAValue (
	    fmtString,
	    MaxSizeOfWholeDoubleNumber (fieldWidth) + 3,
	    rtFLOATUV_ElementType,
	    DSC_Descriptor_Scalar_Float,
	    DSC_Descriptor_Value
	);
        break;
    case RTYPE_C_IntUV:
	STD_sprintf (fmtString, "%%%dd", fieldWidth);
	PrintAValue (
	    fmtString,
	    MaxSizeOfIntegerNumber (fieldWidth),
	    rtINTUV_ElementType,
	    DSC_Descriptor_Scalar_Int,
	    DSC_Descriptor_Value
	);
        break;
    default:
        break;
    }
}

/******************************
 *  Pointer Print Primitives  *
 ******************************/

V_DefinePrimitive (PrintSelfAsPointer) {
    int fieldWidth = 9;

/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XPrintSelfAsPointer:
        break;
    default:
        pTask->raiseUnimplementedAliasException ("PrintSelfAsPointer");
        break;
    }

/*****  Obtain the objects to be printed...  *****/
    pTask->loadDucWithSelf ();

/*****  Print them as a pointer...  *****/
    PrintAAsPointer (pTask, fieldWidth);

/*****  And return the recipient of this message...  *****/
}


V_DefinePrimitive (PrintCurrentAsPointer) {
    int fieldWidth;

/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XPrintCurrentAsPointer:
        fieldWidth = 9;
        break;
    case XPrintCurrentAs12Pointer:
        fieldWidth = 12;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("PrintSelfAsPointer");
        break;
    }

/*****  Obtain the objects to be printed...  *****/
    pTask->loadDucWithCurrent ();

/*****  Print them as a pointer...  *****/
    PrintAAsPointer (pTask, fieldWidth);

/*****  And return the recipient of this message...  *****/
    pTask->loadDucWithSelf ();
}

/*******************************
 *****  PRINTOF Utilities  *****
 *******************************/

/*---------------------------------------------------------------------------
 *  The FMTType structure is used by the PrintOf primitives to compile
 *  the various bits of information that are needed by the LIO print
 *  routines.
 *
 *  fieldSpecification	- the whole part of the format.  For example
 *			  -10 is the fieldSpecification for the -10.3 format.
 *  fieldWidth		- the absolute value of the formatSpecification.
 *  truncated		- true if the user supplied fieldSpecification was
 *			  too large and was shortened.
 *                        (See MaxFormatFieldWidth)
 *  precision		- the decimal part of the user supplied format.  For
 *			  example,  .3 is the precision of a -10.3 format.
 *  maxOutputSize	- the maximum possible number of characters that could
 *			  be output by this print.  This is not obvious because
 *			  the field width specifies the minumum number of
 *			  characters not the maximum.  The LIO print routines
 *			  need to know this in order to allocate enough buffer
 *			  space.  This number is usually a guess - the idea is
 *			  to guess high.
 *  printWithCommas	- specifies which print routine to use.
 *  string		- the format string to use with a '...printf' function.
 *****/
struct FMTType {
    int
	fieldSpecification,
	fieldWidth,
	truncated,
	precision,
	maxOutputSize,
	printWithCommas;
    char
	/*****
 	 *  The largest format I can think of is:
	 *  "%-x.yf\0"
	 *	where x = the maximum double number and
	 *	      y = the maximum double number
 	 *****/
	string[(MAXDOUBLEDIGITS * 2) + 5];
};


/*****  Access Macros  *****/
#define FMT_FieldSpecification(fmt)\
    (fmt).fieldSpecification

#define FMT_FieldWidth(fmt)\
    (fmt).fieldWidth

#define FMT_Truncated(fmt)\
    (fmt).truncated

#define FMT_Precision(fmt)\
    (fmt).precision

#define FMT_MaxOutputSize(fmt)\
    (fmt).maxOutputSize

#define FMT_PrintWithCommas(fmt)\
    (fmt).printWithCommas

#define FMT_String(fmt)\
    (fmt).string

#define FMT_Dump(fmtStruct) {\
    IO_printf ("\nFormat Structure Dump\n");\
    IO_printf ("   fieldspec=%d, fieldWidth=%d, trunc=%d, maxOutputSize=%d\n",\
	FMT_FieldSpecification (fmtStruct),\
	FMT_FieldWidth (fmtStruct),\
	FMT_Truncated (fmtStruct),\
	FMT_MaxOutputSize (fmtStruct));\
    if (FMT_PrintWithCommas (fmtStruct))\
	IO_printf ("   precision=%d\n", FMT_Precision (fmtStruct));\
    else\
	IO_printf ("   format=%s\n", FMT_String (fmtStruct));\
}


#define FMT_RealizeFields(fmtStruct, format) {\
/*****\
 *  Macro to set FMT_FieldSpecification, FMT_FieldWidth, and FMT_Truncated\
 *  from a numeric format (IE. -10.3).\
 *\
 *  This macro is complicated because 'format' can be either int or double so\
 *  1) format may be greater than INT_MAX.  If so, (int)format is invalid.\
 *  2) format may be negative.  The sign must be preserved.\
 *****/\
    FMT_Truncated (fmtStruct) = false;\
\
/*****  Number is Positive  *****/\
    if ((format) >= 0) {\
	if ((format) > MaxFormatFieldWidth) {\
	    FMT_FieldSpecification (fmtStruct) = FMT_FieldWidth (fmtStruct) =\
		MaxFormatFieldWidth;\
	    FMT_Truncated (fmtStruct) = true;\
	}\
	else\
	    FMT_FieldSpecification (fmtStruct) = FMT_FieldWidth (fmtStruct) =\
		(int)(format);\
    }\
    else {\
	if  ((format) < -(MaxFormatFieldWidth)) {\
	    FMT_FieldSpecification (fmtStruct) = -(MaxFormatFieldWidth);\
	    FMT_FieldWidth (fmtStruct) = MaxFormatFieldWidth;\
	    FMT_Truncated (fmtStruct) = true;\
	}\
	else {\
	    FMT_FieldSpecification (fmtStruct) = (int)(format);\
	    FMT_FieldWidth (fmtStruct) = abs (FMT_FieldSpecification (fmtStruct));\
	}\
    }\
}


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
PrivateFnDef bool FindPeriod (char const **string) {
    bool notFound = true;
    char const *eptr;

    eptr = *string + strlen (*string);
    while (eptr > *string && (notFound = (**string != '.')))
	(*string)++;

    return !notFound;
}


/*---------------------------------------------------------------------------
 * Macros which fill in the format structure.
 *---------------------------------------------------------------------------
 */
#define ProduceFormatForInteger(fmtStruct, format, withCommas) {\
    int fmt = (int)format;\
\
    FMT_RealizeFields (fmtStruct, fmt);\
    FMT_PrintWithCommas (fmtStruct) = withCommas;\
\
    if (withCommas) {\
	FMT_MaxOutputSize (fmtStruct) =\
	    MaxSizeOfIntegerNumberWC (FMT_FieldWidth (fmtStruct));\
	FMT_Precision (fmtStruct) = 0;\
    }\
    else {\
	FMT_MaxOutputSize (fmtStruct) =\
	    MaxSizeOfIntegerNumber (FMT_FieldWidth (fmtStruct));\
	STD_sprintf\
	    (FMT_String (fmtStruct),\
	     "%%%dd",\
	     FMT_FieldSpecification (fmtStruct));\
    }\
}


#define ProduceDoubleFormatForDouble(fmtStruct, format, withCommas) {\
    double fmt = (double)format;\
\
    FMT_RealizeFields (fmtStruct, fmt);\
    FMT_PrintWithCommas (fmtStruct) = withCommas;\
\
    if (withCommas) {\
	if (FMT_Truncated (fmtStruct))\
	    FMT_Precision (fmtStruct) = 3;\
	else if ((fmt - FMT_FieldSpecification (fmtStruct)) == 0)\
	    FMT_Precision (fmtStruct) = 0;\
	else {\
	    STD_sprintf (FMT_String (fmtStruct), "%%%gf", fmt);\
\
	    /*****\
	     *  Need to calculate the number of decimals requested\
	     *****/\
	    char const *sptr = FMT_String (fmtStruct);\
	    if (FindPeriod (&sptr)) {\
		FMT_Precision (fmtStruct) = atoi (++sptr);\
\
		/*****  Limit the number of decimal digits  *****/\
		FMT_Precision (fmtStruct) =\
		    V_Min (FMT_Precision (fmtStruct), MAXDOUBLESIGDIGITS);\
	    }\
	    else FMT_Precision (fmtStruct) = 0;\
	}\
\
	FMT_MaxOutputSize (fmtStruct) =\
	    MaxSizeOfWholeDoubleNumberWC (FMT_FieldWidth (fmtStruct)) +\
		FMT_Precision (fmtStruct) + 1; /* num of decimals + '.' */\
    }\
/***** ... else no commas ...  *****/\
    else {\
	if (FMT_Truncated (fmtStruct)) {\
	    STD_sprintf (\
		FMT_String (fmtStruct), "%%%d.3f", FMT_FieldSpecification (fmtStruct)\
	    );\
	    FMT_MaxOutputSize (fmtStruct) = 4;  /* 3 decimals and a decimal point */\
	}\
	/****\
	 *  Need to special case 'x.0' formats because '%g' truncates\
	 *  them to 'x'\
	 *****/\
	else if ((fmt - FMT_FieldSpecification (fmtStruct)) == 0) {\
	    STD_sprintf\
		(FMT_String (fmtStruct), "%%%d.0f", FMT_FieldSpecification (fmtStruct));\
	    FMT_MaxOutputSize (fmtStruct) = 0; /* no decimal point or decimals */\
	}\
	else {\
	    STD_sprintf (FMT_String (fmtStruct), "%%%gf", fmt);\
\
	    /*****\
	     *  Need to calculate the number of decimals requested\
	     *****/\
	    char const *sptr = FMT_String (fmtStruct);\
	    if (FindPeriod (&sptr)) {\
		if ((FMT_MaxOutputSize (fmtStruct) = atoi (++sptr)) > MAXDOUBLESIGDIGITS) {\
		    /*****  Limit the number of decimal digits *****/\
		    STD_sprintf ((char*)sptr, "%df", MAXDOUBLESIGDIGITS);\
		    FMT_MaxOutputSize (fmtStruct) = MAXDOUBLESIGDIGITS;\
		}\
		FMT_MaxOutputSize (fmtStruct)++; /* count the decimal point */\
	    }\
	    else FMT_MaxOutputSize (fmtStruct) = 0;\
	}\
\
	/*****  Add the size of the double's whole part  *****/\
	FMT_MaxOutputSize (fmtStruct) +=\
	    MaxSizeOfWholeDoubleNumber (FMT_FieldWidth (fmtStruct));\
    }\
}


#define ProduceIntegerFormatForDouble(fmtStruct, format, withCommas) {\
    int fieldWidth;\
    int precision = (int) (format / 65536);\
    if (0 == precision) {\
	FMT_Precision (fmtStruct) = 2;\
	fieldWidth = (int) format;\
    }\
    else {\
	if (0 > precision)  precision = 0 - precision;\
	if (16 < precision) FMT_Precision (fmtStruct) = 16;\
	else FMT_Precision (fmtStruct) = precision;\
	fieldWidth = (int) (format % 65536);\
    }\
    FMT_RealizeFields (fmtStruct, fieldWidth);\
    FMT_PrintWithCommas (fmtStruct) = withCommas;\
\
    if (withCommas) {\
	FMT_MaxOutputSize (fmtStruct) =\
	    MaxSizeOfWholeDoubleNumberWC (FMT_FieldWidth (fmtStruct)) + FMT_Precision (fmtStruct) + 1;\
    }\
    else {\
	STD_sprintf\
	    (FMT_String (fmtStruct),\
	     "%%%d.%df",\
	     FMT_FieldSpecification (fmtStruct),\
	     FMT_Precision (fmtStruct));\
	FMT_MaxOutputSize (fmtStruct) =\
	    MaxSizeOfWholeDoubleNumber (FMT_FieldWidth (fmtStruct)) + FMT_Precision (fmtStruct) + 1;\
    }\
}

#define ProduceFormatForString(fmtStruct, format) {\
/*****\
 *  Flip the sign on the format because we want strings to be\
 *  justified opposite from the way the '...printf' functions do it.\
 *****/\
    int fmt = -((int)format);\
\
    FMT_RealizeFields (fmtStruct, fmt);\
    FMT_PrintWithCommas (fmtStruct) = false;\
\
    STD_sprintf\
	(FMT_String (fmtStruct),\
	 "%%%d.%ds",\
	 FMT_FieldSpecification (fmtStruct),\
	 FMT_FieldWidth (fmtStruct));\
    FMT_MaxOutputSize (fmtStruct) = FMT_FieldWidth (fmtStruct);\
}

#define ProduceFormatForChar(fmtStruct, format) {\
    int fmt = (int)format;\
\
    FMT_RealizeFields (fmtStruct, fmt);\
    FMT_PrintWithCommas (fmtStruct) = false;\
\
    STD_sprintf (FMT_String (fmtStruct), "%%%dc", FMT_FieldSpecification (fmtStruct));\
    FMT_MaxOutputSize (fmtStruct) = V_Max (FMT_FieldWidth (fmtStruct), 1);\
}


#define PrintRealNumber(fmtStruct, value) {\
    if (FMT_PrintWithCommas (fmtStruct))\
	PrintWithCommas (fmtStruct, value)\
    else\
	PrintWithoutCommas (fmtStruct, value)\
}

#define PrintInteger(fmtStruct, value) {\
    if (FMT_PrintWithCommas (fmtStruct))\
	PrintWithCommas (fmtStruct, value)\
    else\
	PrintWithoutCommas (fmtStruct, value)\
}

#define PrintWithoutCommas(fmtStruct, value) {\
    iOutputGenerator.printf (\
	 FMT_MaxOutputSize (fmtStruct),\
	 FMT_FieldWidth (fmtStruct),\
	 '*',\
	 FMT_String (fmtStruct),\
	 value);\
    iOutputGenerator.advance ();\
}

#define PrintWithCommas(fmtStruct, value) {\
    iOutputGenerator.printWithCommas (\
	 FMT_MaxOutputSize (fmtStruct),\
	 '*',\
	 FMT_FieldSpecification (fmtStruct),\
	 FMT_Precision (fmtStruct),\
	 (double)(value));\
    iOutputGenerator.advance ();\
}


/*
 *---------------------------------------------------------------------------
 * This macro is passed as an argument to 'DSC_Traverse' by the PrintOf
 * primitives.
 *
 *  Function:
 *	The 'DSC_Traverse' macro does its traverse calling this macro with
 *	each 'value' that it finds.  This macro formats the string specified
 *	by 'value' and stores the result.
 *
 *---------------------------------------------------------------------------
 */
#define PrintOfHandleStrings(value) {\
    if (lastFormat != *fmtp) {\
	ProduceFormatForString (resultFormat, *fmtp);\
    }\
    PrintWithoutCommas (resultFormat, pStrings [value]);\
    lastFormat = *fmtp++;\
}


/*
 *---------------------------------------------------------------------------
 * This macro is passed as an argument to 'DSC_Traverse' by the PrintOf
 * primitives.
 *
 *  Function:
 *	The 'DSC_Traverse' macro does its traverse calling this macro with
 *	each 'value' that it finds.  This macro formats the selector specified
 *	by 'value' and stores the result.
 *
 *---------------------------------------------------------------------------
 */
#define PrintOfHandleSelectors(value) {\
    if (lastFormat != *fmtp) {\
	ProduceFormatForString (resultFormat, *fmtp);\
    }\
    PrintWithoutCommas (resultFormat, KS__ToString (value));\
    lastFormat = *fmtp++;\
}


void getIntegerFormatInfoForDouble (rtINTUV_ElementType const iFmt, int &iFieldWidth, int &iDecDig, int &iSigDig, bool &bRightAlign, bool const bDouble) {
    int iAdjFmt;
    iDecDig = (int) (iFmt / 65536);
    if (0 == iDecDig) {
	iDecDig = 2;
	iAdjFmt = iFmt;
    }
    else {
	if (0 > iDecDig)  iDecDig = 0 - iDecDig;
	if (16 < iDecDig) iDecDig = 16;
	iAdjFmt = iFmt % 65536;
    }
    
    if ((iAdjFmt) >= 0) {
	if ((iAdjFmt) > MaxFormatFieldWidth) 
	    iFieldWidth = MaxFormatFieldWidth;
	else 
	    iFieldWidth = iAdjFmt;
    }
    else {
	if ((iAdjFmt) < -(MaxFormatFieldWidth)) 
	    iFieldWidth = MaxFormatFieldWidth;
	else 
	    iFieldWidth = abs (iAdjFmt);
    }
    if (iFmt < 0) 
	bRightAlign = false;    
    else
	bRightAlign = true;    

    // double type has more precision
    if (bDouble)
	iSigDig = 15;
    else
	iSigDig = 6;
}

void getDoubleFormatInfoForDouble (rtDOUBLEUV_ElementType const iFmt, int &iFieldWidth, int &iDecDig, int &iSigDig, bool &bRightAlign, bool const bDouble) {
    char iFormat[(MAXDOUBLEDIGITS * 2) + 5];
    bool bTruncated = false;
    int iFieldSpec;
    
    if ((iFmt) >= 0) {
	if ((iFmt) > MaxFormatFieldWidth) {
	    iFieldSpec = iFieldWidth = MaxFormatFieldWidth;
	    bTruncated = true;
	}
	else 
	    iFieldSpec = iFieldWidth = (int)(iFmt);
    }
    else {
	if ((iFmt) < -(MaxFormatFieldWidth)) {
	    iFieldSpec = -(MaxFormatFieldWidth);
	    iFieldWidth = MaxFormatFieldWidth;
	    bTruncated = true;
	}
	else {
	    iFieldSpec = (int)(iFmt);
	    iFieldWidth = abs (iFieldSpec);
	}
    }
    if (bTruncated)
	iDecDig = 3;
    else if ((iFmt - iFieldSpec) == 0)
	iDecDig = 0;
    else {
	STD_sprintf (iFormat, "%%%gf", iFmt);
	/*****
	 *  Need to calculate the number of decimals requested
	 *****/
	char const *pBuffer = iFormat;
	char const *pPos = strchr (pBuffer, '.');
	if (pPos) {
	    iDecDig = atoi (++pPos);
	    /*****  Limit the number of decimal digits  *****/
	    iDecDig = V_Min (iDecDig, MAXDOUBLESIGDIGITS);
	}
	else 
	    iDecDig = 0;
    }
    if (iFmt < 0) 
	bRightAlign = false;    
    else
	bRightAlign = true;    

    // double type has more precision
    if (bDouble)
	iSigDig = 15;
    else
	iSigDig = 6;
}

void nicePrintRealNumber (VOutputGenerator &rOutputGenerator, int iFieldWidth, int iDecDig, int iSigDig, bool bRightAlign, bool bCompact, double iValue) {
    if (iFieldWidth < 0) iFieldWidth = (MAXDOUBLEDIGITS * 2) + 5;
    char iResult[MaxFormatFieldWidth + 1];    

    int xPos = 0, iDecPos, iSign;
    bool bHasDecimalPoint = false;
    char *pBuffer = ecvt (iValue, iSigDig, &iDecPos, &iSign);
    if (!strncmp ("inf", pBuffer, 3)) {
	strcpy (iResult, "inf");
	xPos = 3; // pointing to the null terminator of the string "inf\0"
    }
    else {
	// Rounding the number to the right digit before converting to a string 
	// if the specified decimal precision does not require max significant digits.
	if (iSigDig - iDecPos > iDecDig) {
	    double iPower = pow (10.0, iDecDig);
	    iValue = floor (iValue * iPower + 0.5) / iPower;
	    pBuffer = ecvt (iValue, iSigDig, &iDecPos, &iSign);
	}

	bool bPrefixZero = iDecPos <= 0;
	// The whole digit is only one charater '0' when '0' has to be prefixed.
	int iWholeDig = (bPrefixZero) ? 1 : iDecPos; 
	// Extra characters are decimal point and possibly '-' sign.
	int cExtraChar = (iSign) ? 2 : 1; 

	if (pBuffer == NULL || iFieldWidth < 0) { 
	    // Printing * when unexpected case encountered.
	    for (int i = 0; i < 6; i++)
		iResult[i] = '*';
	    iResult[6] = '\0';
	} else if (iWholeDig + iDecDig + cExtraChar > iFieldWidth) { 
	    // Printing * when not enough space is specified 
	    iResult[iFieldWidth] = '\0';
	    for( ; iFieldWidth > 0; iFieldWidth--)
		iResult[iFieldWidth-1] = '*';
	} else {
	    if(iSign)
		iResult[xPos++] = '-'; 

	    for ( ; *pBuffer && iDecPos > 0; pBuffer++, iDecPos--)
		// Copying the whole digits
		iResult[xPos++] = *pBuffer; 

	    if (bPrefixZero) 
		// Adding '0' before the decimal point if necessary
		iResult[xPos++] = '0'; 

	    for ( ; iDecPos > 0; iDecPos--)
		// Padding '0' until to the decimal point
		iResult[xPos++] = '0';  
	    
	    if (iDecDig > 0) {
		// Adding the decimal point
		iResult[xPos++] = '.';
		bHasDecimalPoint = true;
	    } 
	    
	    for( ; iDecPos < 0 && iDecDig > 0; iDecPos++, iDecDig--) 
		// Padding '0' after the decimal point
		iResult[xPos++] = '0';  

	    for( ; iDecDig > 0; iDecDig--, pBuffer++) {
		// Copying it if there is still a digit
		if (*pBuffer)
		    iResult[xPos++] = *pBuffer; 
		// Otherwise it's the end of pBuffer, NEVER peek beyond the terminator...It's UGLY!
		// just exit this loop and use a separate loop to pad the remaining spaces with '0'
		else 
		    break;
	    } 

	    // if not compact printing, padding the remaining spaces with '0'
	    if (!bCompact)
		for( ; iDecDig > 0; iDecDig--) 
		    iResult[xPos++] = '0';
	    iResult[xPos] = '\0';
	}
    }

    // even though we do not explicitly pad the remaining spaces with '0' as immediately above for 
    // compact printing, there is still chance the pBuffer itself has trailing zeroes. 
    // so the following process of stripping zeroes is still necessary.
    if (bCompact && bHasDecimalPoint) {
	// deleting the trailing zeroes
	char *pPos = iResult + xPos;
	while ('0' == *--pPos) {
	    --xPos;
	    *pPos = '\0';
	}
	// delete the decimal point if all decimal digits are zeroes
	if(*pPos == '.') 
	    *pPos = '\0';
    }

    // Right aligning the number if requested.
    // iResult[iFieldWidth] is the iFieldWidth+1 element. It is copying '\0' for the first loop.
    if (!bCompact && iFieldWidth > xPos) {
	if (bRightAlign) {
	    for ( ; xPos >= 0; xPos--, iFieldWidth--)
		iResult[iFieldWidth] = iResult[xPos];
	    for ( ; iFieldWidth >= 0; iFieldWidth--) 
		iResult[iFieldWidth] = ' ';
	} else { // Padding the remaining field width with space.
	    for ( ; xPos < iFieldWidth; xPos++)
		iResult[xPos] = ' ';
	    iResult[xPos] = '\0';
	}
    }

    rOutputGenerator.putString (iResult);
    rOutputGenerator.advance ();
}

/***** This routine prints the accumulator with the integer format specified.
 ***** If the format is negative, it left justifies numbers and right justifies
 ***** strings.  If the format is positive, it right justifies numbers and
 ***** left justifies strings.
 *****/
V_DefinePrimitive (IntegerPrintOf) {
    bool	 withCommas = false, bNice = false, bCompact = false;
    unsigned int converseMessageSelector;
    FMTType	 resultFormat;


/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XIntegerPrintOf:
	converseMessageSelector = KS__PrintColon;
        break;
    case XIntegerPrintWithCommasOf:
	withCommas = true;
	converseMessageSelector = KS__PrintWithCommasColon;
        break;
    case XIntegerNicePrintOf:
	bNice = true;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("IntegerPrintOf");
        break;
    }
    switch (V_TOTSC_PrimitiveFlags) {
    case 1:
	bCompact = true;
	break;
    default:
	break;
    }

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	if (bNice)
	    pTask->sendBinaryConverseWithCurrent ("nicePrint:");
	else 
	    pTask->sendBinaryConverseWithCurrent (converseMessageSelector);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();
    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

/*****  Do the scalar case ... *****/
    if (DucIsAScalar) {
	rtINTUV_ElementType size = DSC_Descriptor_Scalar_Int (rCurrent);

	VOutputGenerator iOutputGenerator (pTask);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV:
	    ProduceFormatForChar (resultFormat, size);
	    PrintWithoutCommas (resultFormat, DSC_Descriptor_Scalar_Char (ADescriptor));
	    break;

	case RTYPE_C_DoubleUV:
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getIntegerFormatInfoForDouble (size, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   
	        nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, DSC_Descriptor_Scalar_Double (ADescriptor));    
	    }
	    else {
		ProduceIntegerFormatForDouble (resultFormat, size, withCommas);
		PrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Double (ADescriptor));
	    }
	    break;

	case RTYPE_C_FloatUV:
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getIntegerFormatInfoForDouble (size, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);   
	        nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, DSC_Descriptor_Scalar_Float (ADescriptor));    
	    }
	    else {
		ProduceIntegerFormatForDouble (resultFormat, size, withCommas);
		PrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Float (ADescriptor));
	    }
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, pLStoreStrings [DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    break;

	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /***** String Case *****/
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, pBlockStrings [DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, KS__ToString (DSC_Descriptor_Scalar_Int (ADescriptor))
		);
	    }
	    else {
	    /***** Integer Case *****/
		ProduceFormatForInteger (resultFormat, size, withCommas);
		PrintInteger (resultFormat, DSC_Descriptor_Scalar_Int (ADescriptor));
	    }
	    break;

	default:
	    break;
	}
    }

/*****  Non Scalar case ...  *****/
    else {
	M_CPD *fmtuv = DSC_Descriptor_Value (rCurrent);
	rtINTUV_ElementType
	    lastFormat,
	    *fmtp = rtINTUV_CPD_Array (fmtuv),
	    *fmtlp = fmtp + UV_CPD_ElementCount (fmtuv);

	VOutputGenerator iOutputGenerator (pTask);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtCHARUV_ElementType
		    *uvp = rtCHARUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForChar (resultFormat, *fmtp);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForChar (resultFormat, *fmtp);
		    }
		    PrintWithoutCommas (resultFormat, *uvp++);
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_DoubleUV: {
	    M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
	    rtDOUBLEUV_ElementType
		*uvp = rtDOUBLEUV_CPD_Array (uv),
		*uvl = uvp + UV_CPD_ElementCount (uv);

	    lastFormat = *fmtp;

	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getIntegerFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) 
			getIntegerFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   
		    nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, *uvp);    
		    uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    else {
		ProduceIntegerFormatForDouble (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceIntegerFormatForDouble (resultFormat, *fmtp, withCommas);
		    }
		    PrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	}
	    break;
	case RTYPE_C_FloatUV: {
	    M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
	    rtFLOATUV_ElementType
		*uvp = rtFLOATUV_CPD_Array (uv),
		*uvl = uvp + UV_CPD_ElementCount (uv);

	    lastFormat = *fmtp;
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getIntegerFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);   

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) 
			getIntegerFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);   
		    nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, *uvp);    
		    uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    else {
		ProduceIntegerFormatForDouble (resultFormat, *fmtp, withCommas);
		
		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceIntegerFormatForDouble (resultFormat, *fmtp, withCommas);
		    }
		    PrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	}
	    break;
	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pLStoreStrings
		DSC_Traverse (ADescriptor, PrintOfHandleStrings);
#undef pStrings
	    }
	    break;
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /*****  String Case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pBlockStrings
		DSC_Traverse (ADescriptor, PrintOfHandleStrings);
#undef pStrings
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

		DSC_Traverse (ADescriptor, PrintOfHandleSelectors);
    	    }
	    else {
	    /***** Integer U-Vector Case *****/
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtINTUV_ElementType
		    *uvp = rtINTUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForInteger (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForInteger (resultFormat, *fmtp, withCommas);
		    }
		    PrintInteger (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    } /* end integer case */
	    break;
	default:
	    break;
	}
    } /* end non scalar case */
}


V_DefinePrimitive (DoublePrintOf) {
/***** This routine prints the accumulator with the double format specified.
 ***** If the format is negative, it left justifies numbers and right justifies
 ***** strings.  If the format is positive, it right justifies numbers and
 ***** left justifies strings.
 *****/
    FMTType resultFormat;


/*****  Decode the alias by which this primitive was invoked...  *****/
    bool withCommas = false, bNice = false, bCompact = false;
    int converseMessageSelector;
    switch (V_TOTSC_Primitive) {
    case XDoublePrintOf:
	converseMessageSelector = KS__PrintColon;
        break;
    case XDoublePrintWithCommasOf:
	withCommas = true;
	converseMessageSelector = KS__PrintWithCommasColon;
        break;
    case XDoubleNicePrintOf:
	bNice = true;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("DoublePrintOf");
        break;
    }
    switch (V_TOTSC_PrimitiveFlags) {
    case 1:
	bCompact = true;
	break;
    default:
	break;
    }


/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	if (bNice)
	    pTask->sendBinaryConverseWithCurrent ("nicePrint:");
	else
	    pTask->sendBinaryConverseWithCurrent (converseMessageSelector);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

/*****  Do the scalar case ... *****/
    if (DucIsAScalar) {
	rtDOUBLEUV_ElementType size = DSC_Descriptor_Scalar_Double (rCurrent);

	VOutputGenerator iOutputGenerator (pTask);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV:
	    ProduceFormatForChar (resultFormat, size);
	    PrintWithoutCommas (resultFormat, DSC_Descriptor_Scalar_Char (ADescriptor));
	    break;

	case RTYPE_C_DoubleUV:
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getDoubleFormatInfoForDouble (size, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   
	        nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, DSC_Descriptor_Scalar_Double (ADescriptor));    
	    }
	    else {
		ProduceDoubleFormatForDouble (resultFormat, size, withCommas);
		PrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Double (ADescriptor));
	    }
	    break;

	case RTYPE_C_FloatUV:
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getDoubleFormatInfoForDouble (size, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);
	        nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, DSC_Descriptor_Scalar_Float (ADescriptor));    
	    }
	    else {
		ProduceDoubleFormatForDouble (resultFormat, size, withCommas);
	        PrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Float (ADescriptor));
	    }
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, pLStoreStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    break;

	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /***** String Case *****/
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, pBlockStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		ProduceFormatForString (resultFormat, size);
		PrintWithoutCommas (
		    resultFormat, KS__ToString (DSC_Descriptor_Scalar_Int (ADescriptor))
		);
	    }
	    else {
	    /***** Integer Case *****/
		ProduceFormatForInteger (resultFormat, size, withCommas);
		PrintInteger (resultFormat, DSC_Descriptor_Scalar_Int (ADescriptor));
	    }
	    break;

	default:
	    break;
	}
    }

/*****  Non Scalar case ...  *****/
    else {
	M_CPD* fmtuv = DSC_Descriptor_Value (rCurrent);
	rtDOUBLEUV_ElementType
	    lastFormat,
	    *fmtp = rtDOUBLEUV_CPD_Array (fmtuv),
	    *fmtlp = fmtp + UV_CPD_ElementCount (fmtuv);

	VOutputGenerator iOutputGenerator (pTask);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV: {
		M_CPD* uv = DSC_Descriptor_Value (ADescriptor);
		rtCHARUV_ElementType
		    *uvp = rtCHARUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForChar (resultFormat, *fmtp);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForChar (resultFormat, *fmtp);
		    }
		    PrintWithoutCommas (resultFormat, *uvp++);
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_DoubleUV: {
	    M_CPD* uv = DSC_Descriptor_Value (ADescriptor);
	    rtDOUBLEUV_ElementType
		*uvp = rtDOUBLEUV_CPD_Array (uv),
		*uvl = uvp + UV_CPD_ElementCount (uv);

	    lastFormat = *fmtp;
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getDoubleFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) 
			getDoubleFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, true);   
		    nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, *uvp);    
		    uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    else {
		ProduceDoubleFormatForDouble (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceDoubleFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    PrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	}
	    break;
	case RTYPE_C_FloatUV: {
	    M_CPD* uv = DSC_Descriptor_Value (ADescriptor);
	    rtFLOATUV_ElementType
		*uvp = rtFLOATUV_CPD_Array (uv),
		*uvl = uvp + UV_CPD_ElementCount (uv);

	    lastFormat = *fmtp;
	    if (bNice) {
		int iFieldWidth, iDecDig, iSigDig;
		bool bRightAlign;
		getDoubleFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);   

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) 
			getDoubleFormatInfoForDouble (*fmtp, iFieldWidth, iDecDig, iSigDig, bRightAlign, false);   
		    nicePrintRealNumber (iOutputGenerator, iFieldWidth, iDecDig, iSigDig, bRightAlign, bCompact, *uvp);    
		    uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    else {
		ProduceDoubleFormatForDouble (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceDoubleFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    PrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	}
	    break;
	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);
#define pStrings pLStoreStrings
		DSC_Traverse (ADescriptor, PrintOfHandleStrings);
#undef pStrings
	    }
	    break;
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /*****  String case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pBlockStrings
		DSC_Traverse (ADescriptor, PrintOfHandleStrings);
#undef pStrings
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);
		DSC_Traverse (ADescriptor, PrintOfHandleSelectors);
    	    }
	    else {
		/***** Integer U-Vector Case *****/
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtINTUV_ElementType
		    *uvp = rtINTUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForInteger (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForInteger (resultFormat, *fmtp, withCommas);
		    }
		    PrintInteger (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    } /* end integer case */
	    break;
	default:
	    break;
	}
    } /* end non scalar case */
}


/******************************
 *****  SPRINT Utilities  *****
 ******************************/

PrivateVarDef char SprintTmpBuffer[MaxFormatFieldWidth + 1];

#define SPrintRealNumber(fmtStruct, value) {\
    if (FMT_PrintWithCommas (fmtStruct))\
	SPrintWithCommas (fmtStruct, value)\
    else\
	SPrintWithoutCommas (fmtStruct, value)\
}

#define SPrintInteger(fmtStruct, value) {\
    if (FMT_PrintWithCommas (fmtStruct))\
	SPrintWithCommas (fmtStruct, value)\
    else\
	SPrintWithoutCommas (fmtStruct, value)\
}

#define SPrintWithoutCommas(fmtStruct, value) {\
    /*****  SPrint into SprintTmpBuffer because it may overflow  *****/\
    if ((STD_sprintf (SprintTmpBuffer, FMT_String (fmtStruct), value)) > FMT_FieldWidth (fmtStruct))\
	/*****  ... if overflow, return '*'s ...  *****/\
	memset (sprintResultp, '*', FMT_FieldWidth (fmtStruct));\
    else\
	/*****  ... else return the sprint result ...  *****/\
	memcpy (sprintResultp, SprintTmpBuffer, FMT_FieldWidth (fmtStruct));\
\
    /*****  Add the terminating character and update the pointer ...  *****/\
    sprintResultp += FMT_FieldWidth (fmtStruct);\
    *sprintResultp++ = '\0';\
}

#define SPrintWithCommas(fmtStruct, value) {\
/*****\
 *  The 'UTIL_FormatNumberWithCommas' function guarantees that only\
 *  'FieldWidth' characters will be written to 'sprintResultp'.\
 *****/\
    UTIL_FormatNumberWithCommas (\
	sprintResultp,\
	FMT_MaxOutputSize (fmtStruct),\
	'*',\
	FMT_FieldSpecification (fmtStruct),\
	FMT_Precision (fmtStruct),\
	(double)(value));\
    sprintResultp += FMT_FieldWidth (fmtStruct) + 1;\
}


/*
 *---------------------------------------------------------------------------
 * This macro is passed as an argument to 'DSC_Traverse' by the SPrint
 * primitives.
 *
 *  Function:
 *	The 'DSC_Traverse' macro does its traverse calling this macro with
 *	each 'value' that it finds.  This macro formats the string specified
 *	by 'value' and stores the result.
 *
 *---------------------------------------------------------------------------
 */
#define SPrintHandleStrings(value) {\
    if (lastFormat != *fmtp) {\
	ProduceFormatForString (resultFormat, *fmtp);\
    }\
    SPrintWithoutCommas (resultFormat, pStrings[value]);\
    lastFormat = *fmtp++;\
}

/*
 *---------------------------------------------------------------------------
 * This macro is passed as an argument to 'DSC_Traverse' by the SPrint
 * primitives.
 *
 *  Function:
 *	The 'DSC_Traverse' macro does its traverse calling this macro with
 *	each 'value' that it finds.  This macro formats the selector specified
 *	by 'value' and stores the result.
 *
 *---------------------------------------------------------------------------
 */
#define SPrintHandleSelectors(value) {\
    if (lastFormat != *fmtp) {\
	ProduceFormatForString (resultFormat, *fmtp);\
    }\
    SPrintWithoutCommas (resultFormat, KS__ToString (value));\
    lastFormat = *fmtp++;\
}

/*---------------------------------------------------------------------------
 *  Macros to manage the resulting SPrint structures.
 *
 *  These macros manage the following variables and depend on them being
 *  declared within the calling program:
 *
 *      M_CPD *
 *	    sprintResultCharUV;
 *	char
 *	    *sprintScalarBuffer;
 *	rtCHARUV_ElementType
 *	    *sprintResultp;
 *
 *---------------------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 * Macro to setup for SPrint scalar output.
 *---------------------------------------------------------------------------
 */
#define SPrintInitializeForScalar(format) {\
    FMTType fmtStruct;\
    FMT_RealizeFields (fmtStruct, format);\
    sprintScalarBuffer = (char *)UTIL_Malloc (FMT_FieldWidth (fmtStruct) + 1);\
    sprintResultp = sprintScalarBuffer;\
}

/*
 *---------------------------------------------------------------------------
 * Macro to setup for SPrint list output.
 *---------------------------------------------------------------------------
 */
#define SPrintInitializeForList() {\
    /***  Make a character U-Vector to hold the results ... ***/\
    int numberOfCharacters = 0;\
    FMTType fmtStruct;\
\
    /*** Count the number of characters needed from the formats ***/\
    while (fmtp < fmtlp) {\
        FMT_RealizeFields (fmtStruct, *fmtp);\
        numberOfCharacters += (FMT_FieldWidth (fmtStruct) + 1);\
        fmtp++;\
    }\
\
    /***  Create the character uvector ***/\
    sprintResultCharUV = pTask->NewCharUV (numberOfCharacters);\
\
    /*** Initialize 'sprintResultp' for later ***/\
    sprintResultp = rtCHARUV_CPD_Array (sprintResultCharUV);\
}

/*---------------------------------------------------------------------------
 * Macro to return the SPrint scalar result.
 *---------------------------------------------------------------------------
 */
#define SPrintOutputScalarResult {\
    pTask->loadDucWithListOrStringStore (\
        rtLSTORE_NewStringStore (pTask->codScratchPad (), sprintScalarBuffer)\
    );\
    UTIL_Free (sprintScalarBuffer);\
}

/*---------------------------------------------------------------------------
 * Macro to return the SPrint list results.
 *---------------------------------------------------------------------------
 */
#define SPrintOutputListResult {\
    pTask->loadDucWithListOrStringStore (\
	rtLSTORE_NewStringStoreFromUV (sprintResultCharUV)\
    );\
    sprintResultCharUV->release ();\
}


/*---------------------------------------------------------------------------
 ***** This routine formats the accumulator with the integer format specified.
 ***** If the format is negative, it left justifies numbers and right justifies
 ***** strings.  If the format is positive, it right justifies numbers and
 ***** left justifies strings.
 *****/
V_DefinePrimitive (IntegerSPrint) {
    M_CPD *sprintResultCharUV;
    int
	withCommas,
	converseMessageSelector;
    FMTType
	resultFormat;
    char
	*sprintScalarBuffer;
    rtCHARUV_ElementType
	*sprintResultp;


/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XIntegerSPrintOf:
        withCommas = false;
	converseMessageSelector = KS__SPrintColon;
        break;
    case XIntegerSPrintWithCommasOf:
	withCommas = true;
	converseMessageSelector = KS__SPrintWithCommasColon;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("IntegerSPrint");
        break;
    }

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (converseMessageSelector);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

/*****  Do the scalar case ... *****/
    if (DucIsAScalar) {
	rtINTUV_ElementType size = DSC_Descriptor_Scalar_Int (rCurrent);

	SPrintInitializeForScalar (size);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV:
	    ProduceFormatForChar (resultFormat, size);
	    SPrintWithoutCommas (resultFormat, DSC_Descriptor_Scalar_Char (ADescriptor));
	    break;

	case RTYPE_C_DoubleUV:
	    ProduceIntegerFormatForDouble (resultFormat, size, withCommas);
	    SPrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Double (ADescriptor));
	    break;

	case RTYPE_C_FloatUV:
	    ProduceIntegerFormatForDouble (resultFormat, size, withCommas);
	    SPrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Float (ADescriptor));
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, pLStoreStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    break;

	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /***** String Case *****/
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, pBlockStrings [DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, KS__ToString (DSC_Descriptor_Scalar_Int (ADescriptor))
		);
	    }
	    else {
	    /***** Integer Case *****/
		ProduceFormatForInteger (resultFormat, size, withCommas);
		SPrintInteger (resultFormat, DSC_Descriptor_Scalar_Int (ADescriptor));
	    }
	    break;

	default:
	    break;
	}

	SPrintOutputScalarResult;
    }

/*****  Non Scalar case ...  *****/
    else {
	M_CPD* fmtuv = DSC_Descriptor_Value (rCurrent);
	rtINTUV_ElementType
	    lastFormat,
	    *fmtp = rtINTUV_CPD_Array (fmtuv),
	    *fmtlp = fmtp + UV_CPD_ElementCount (fmtuv);

	SPrintInitializeForList ();
	fmtp = rtINTUV_CPD_Array (fmtuv); /* reset fmtp */

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtCHARUV_ElementType
		    *uvp = rtCHARUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForChar (resultFormat, *fmtp);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForChar (resultFormat, *fmtp);
		    }
		    SPrintWithoutCommas (resultFormat, *uvp++);
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_DoubleUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtDOUBLEUV_ElementType
		    *uvp = rtDOUBLEUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceIntegerFormatForDouble
		    (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceIntegerFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    SPrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_FloatUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtFLOATUV_ElementType
		    *uvp = rtFLOATUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceIntegerFormatForDouble
		    (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceIntegerFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    SPrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pLStoreStrings
		DSC_Traverse (ADescriptor, SPrintHandleStrings);
#undef pStrings
	    }
	    break;
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /*****  String case  *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pBlockStrings
		DSC_Traverse (ADescriptor, SPrintHandleStrings);
#undef pStrings
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
		/***** Selector Case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

		DSC_Traverse (ADescriptor, SPrintHandleSelectors);
    	    }
	    else {
		/***** Integer U-Vector Case *****/
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtINTUV_ElementType
		    *uvp = rtINTUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForInteger (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForInteger (resultFormat, *fmtp, withCommas);
		    }
		    SPrintInteger (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    } /* end integer case */
	    break;
	default:
	    break;
	}
	SPrintOutputListResult;
    } /* end non scalar case */
}


/***** This routine formats the accumulator with the double format specified.
 ***** If the format is negative, it left justifies numbers and right justifies
 ***** strings.  If the format is positive, it right justifies numbers and
 ***** left justifies strings.
 *****/
V_DefinePrimitive (DoubleSPrint) {
    M_CPD *sprintResultCharUV;
    bool 
	withCommas;
    int	
	converseMessageSelector;
    FMTType
	resultFormat;
    char
	*sprintScalarBuffer;
    rtCHARUV_ElementType
	*sprintResultp;


/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XDoubleSPrintOf:
        withCommas = false;
	converseMessageSelector = KS__SPrintColon;
        break;
    case XDoubleSPrintWithCommasOf:
	withCommas = true;
	converseMessageSelector = KS__SPrintWithCommasColon;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("DoubleSPrint");
        break;
    }

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (converseMessageSelector);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

/*****  Do the scalar case ... *****/
    if (DucIsAScalar) {
	rtDOUBLEUV_ElementType size = DSC_Descriptor_Scalar_Double (rCurrent);

	SPrintInitializeForScalar (size);

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV:
	    ProduceFormatForChar (resultFormat, size);
	    SPrintWithoutCommas (resultFormat, DSC_Descriptor_Scalar_Char (ADescriptor));
	    break;

	case RTYPE_C_DoubleUV:
	    ProduceDoubleFormatForDouble (resultFormat, size, withCommas);
	    SPrintRealNumber (resultFormat, DSC_Descriptor_Scalar_Double (ADescriptor));
	    break;

	case RTYPE_C_FloatUV:
	    ProduceDoubleFormatForDouble (resultFormat, size, withCommas);
	    SPrintRealNumber (
		resultFormat, DSC_Descriptor_Scalar_Float (ADescriptor)
	    );
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, pLStoreStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    break;

	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /***** String Case *****/
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, pBlockStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]
		);
	    }
	    else if (pTask->ducStore ()->NamesTheSelectorClass ()) {
	    /***** Selector Case *****/
		ProduceFormatForString (resultFormat, size);
		SPrintWithoutCommas (
		    resultFormat, KS__ToString (DSC_Descriptor_Scalar_Int (ADescriptor))
		);
	    }
	    else {
	    /***** Integer Case *****/
		ProduceFormatForInteger (resultFormat, size, withCommas);
		SPrintInteger (
		    resultFormat, DSC_Descriptor_Scalar_Int (ADescriptor)
		);
	    }
	    break;

	default:
	    break;
	}

	SPrintOutputScalarResult;
    }

/*****  Non Scalar case ...  *****/
    else {
	M_CPD* fmtuv = DSC_Descriptor_Value (rCurrent);
	rtDOUBLEUV_ElementType
	    lastFormat,
	    *fmtp = rtDOUBLEUV_CPD_Array (fmtuv),
	    *fmtlp = fmtp + UV_CPD_ElementCount (fmtuv);

	SPrintInitializeForList ();
	fmtp = rtDOUBLEUV_CPD_Array (fmtuv); /* reset fmtp */

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_CharUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtCHARUV_ElementType
		    *uvp = rtCHARUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForChar (resultFormat, *fmtp);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForChar (resultFormat, *fmtp);
		    }
		    SPrintWithoutCommas (resultFormat, *uvp++);
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_DoubleUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtDOUBLEUV_ElementType
		    *uvp = rtDOUBLEUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceDoubleFormatForDouble (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceDoubleFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    SPrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    break;
	case RTYPE_C_FloatUV: {
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtFLOATUV_ElementType
		    *uvp = rtFLOATUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceDoubleFormatForDouble (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceDoubleFormatForDouble
			    (resultFormat, *fmtp, withCommas);
		    }
		    SPrintRealNumber (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    }
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (pTask->ducStore ())) {
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pLStoreStrings
		DSC_Traverse (ADescriptor, SPrintHandleStrings);
#undef pStrings
	    }
	    break;
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (pTask->ducStore ())) {
	    /*****  String case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

#define pStrings pBlockStrings
		DSC_Traverse (ADescriptor, SPrintHandleStrings);
#undef pStrings
	    }
	    else if (pTask->ducStore()->NamesTheSelectorClass ()) {
		/***** Selector Case *****/
		lastFormat = *fmtp;
		ProduceFormatForString (resultFormat, *fmtp);

		DSC_Traverse (ADescriptor, SPrintHandleSelectors);
    	    }
	    else {
		/***** Integer U-Vector Case *****/
		M_CPD *uv = DSC_Descriptor_Value (ADescriptor);
		rtINTUV_ElementType
		    *uvp = rtINTUV_CPD_Array (uv),
		    *uvl = uvp + UV_CPD_ElementCount (uv);

		lastFormat = *fmtp;
		ProduceFormatForInteger (resultFormat, *fmtp, withCommas);

		while (uvp < uvl) {
		    if (lastFormat != *fmtp) {
			ProduceFormatForInteger (resultFormat, *fmtp, withCommas);
		    }
		    SPrintInteger (resultFormat, *uvp); uvp++;
		    lastFormat = *fmtp++;
		}
	    } /* end integer case */
	    break;
	default:
	    break;
	}

	SPrintOutputListResult;
    } /* end non scalar case */
}


/****************************************
 *  String/Selector Printing Utilities  *
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  block relative strings.
 *
 *  Arguments:
 *	size			- the combined minimum AND maximum field width
 *				  of the printed string.  Special significance
 *				  is attributed to a 'size' of 0 - the string
 *				  is printed in exactly the number of places
 *				  required to represent it.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintAAsStrings (VPrimitiveTask* pTask, int size) {

#   define handleString(xString) {\
	char const *pString = pStrings[xString];\
	iOutputGenerator.printf (\
	    size == 0 ? strlen (pString) : size, format, pString\
	);\
	iOutputGenerator.advance ();\
    }

    char format[32];
    STD_sprintf (format, size == 0 ? "%%s" : "%%-%d.%ds", size, size);

    switch (ADescriptor.pointerRType ()) {
    case RTYPE_C_IntUV: {
	    rtBLOCK_Handle::Strings pStrings;
	    if (!pStrings.setTo (pTask->ducStore ())) {
	    }
	    else if (DucIsAScalar) {
		char const *pString = pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)];
		pTask->outputBufferPrintScalar (
		    size == 0 ? strlen (pString) : size, format, pString
		);
	    }
	    else {
		VOutputGenerator iOutputGenerator (pTask);
		DSC_Traverse (ADescriptor, handleString);
	    }
	}
	break;

    case RTYPE_C_Link: {
	    rtLSTORE_Handle::Strings pStrings;
	    if (!pStrings.setTo (pTask->ducStore ())) {
	    }
	    else if (DucIsAScalar) {
		char const *pString = pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)];
		pTask->outputBufferPrintScalar (
		    size == 0 ? strlen (pString) : size, format, pString
		);
	    }
	    else  {
		VOutputGenerator iOutputGenerator (pTask);
		DSC_Traverse (ADescriptor, handleString);
	    }
	}
	break;
    default:
	break;
    }

#undef handleString
}


/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  known selector indices.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintAAsKSIs (VPrimitiveTask* pTask) {
/*****  Print the selected string(s)...  *****/
    if (DucIsAScalar) {
	char const *ksiAsString = KS__ToString (DSC_Descriptor_Scalar_Int (ADescriptor));
	pTask->outputBufferPrintScalar (strlen (ksiAsString), "%s", ksiAsString);
    }
    else {
        int
	    *ksip = rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor)),
	    *ksil = ksip + UV_CPD_ElementCount (DSC_Descriptor_Value (ADescriptor));

	VOutputGenerator iOutputGenerator (pTask);
	while (ksip < ksil) {
	    iOutputGenerator.putString (KS__ToString (*ksip++));
	    iOutputGenerator.advance ();
	}
    }
}


/*****************************************
 *  String/Selector Printing Primitives  *
 *****************************************/

V_DefinePrimitive (PrintSelector) {
/*****  Obtain the strings to print...  *****/
    pTask->loadDucWithCurrent ();

/*****  Print them...  *****/
    RTYPE_Type selectorStoreRType = pTask->ducStoreRType ();
    switch (selectorStoreRType) {
    case RTYPE_C_Block:
        PrintAAsStrings (pTask, 0);
        break;
    case RTYPE_C_ValueStore:
        PrintAAsKSIs (pTask);
        break;
    default:
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "PrintSelector: A '%s' Cannot Hold Selectors",
	    RTYPE_TypeIdAsString (selectorStoreRType)
	);
        break;
    }

/*****  And return the recipient of this message...  *****/
    pTask->loadDucWithSelf ();
}


V_DefinePrimitive (PrintString) {
    int fieldWidth;

/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XPrintString:
        fieldWidth = 0;
        break;
    case XPrint12String:
        fieldWidth = 12;
        break;
    case XPrint24String:
        fieldWidth = 24;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("PrintString");
        break;
    }

/*****  Obtain the strings to print...  *****/
    pTask->loadDucWithCurrent ();

/*****  Print them...  *****/
    PrintAAsStrings (pTask, fieldWidth);

/*****  And return the recipient of this message...  *****/
    pTask->loadDucWithSelf ();
}


/*************************************************
 *  Primitive/Method/Closure Printing Utilities  *
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  primitive method indices.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintAAsPrimitiveFunctions (VPrimitiveTask* pTask) {
    if (DucIsAScalar) {
	char const* pname = PRIMFNS_PrimitiveName (DSC_Descriptor_Scalar_Int (ADescriptor));
        pTask->outputBufferPrintScalar (strlen (pname) + 2, "<%s>", pname);
    }
    else {
        int
	    *pfip = rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor)),
	    *pfil = pfip + UV_CPD_ElementCount (DSC_Descriptor_Value (ADescriptor));

	VOutputGenerator iOutputGenerator (pTask);
	while (pfip < pfil) {
	    char const* pname = PRIMFNS_PrimitiveName (*pfip++);
	    iOutputGenerator.printf (strlen (pname) + 2, "<%s>", pname);
	    iOutputGenerator.advance ();
	}
    }
}


/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  block closure indices.
 *
 *  Arguments:
 *	pBlock			- a handle for the block of the block closure.
 *	lbracket		- a character containing the left bracket to
 *				  be used in printing the block.
 *	rbracket		- a character containing the right bracket to
 *				  be used in printing the block.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintAAsBlockClosureIndices (
    VPrimitiveTask *pTask, rtBLOCK_Handle *pBlock, int lbracket, int rbracket
) {
    VCPDReference pBlockCPD (0, pBlock->GetCPD ());
    VString iSource;
    RSLANG_Decompile (iSource, pBlockCPD);
    pTask->printf (
	iSource.length () + 2, "%c%s%c", lbracket, (char const*)iSource, rbracket
    );
}


PrivateFnDef char const* MethodTraverseStrings (bool reset, va_list ap) {
    V::VArgList iArgList (ap);
    int* const beginPtr = iArgList.arg<int *>();
    int* const endPtr	= iArgList.arg<int *>();
    int**const workPtr	= iArgList.arg<int **>();

    if (reset) {
	*workPtr = beginPtr;
	return NilOf (char *);
    }

    if (*workPtr < endPtr)
	return PRIMFNS_PrimitiveName (*(*workPtr)++);

    return NilOf (char const *);
}

/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  primitive method indices and return the result strings in the
 *****  accumulator.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void SPrintAAsPrimitiveFunctions (VPrimitiveTask* pTask) {
    int *pp;

    pTask->loadDucWithListOrStringStore (
	DucIsAScalar ? rtLSTORE_NewStringStoreWithDelm (
	    pTask->codScratchPad (), "<", ">",
	    PRIMFNS_PrimitiveName (DSC_Descriptor_Scalar_Int (ADescriptor))
	) : rtLSTORE_NewStringStoreWithDelm (
	    pTask->codScratchPad (), "<", ">",
	    MethodTraverseStrings,
	    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor)),
	    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor))
	    + UV_CPD_ElementCount (DSC_Descriptor_Value (ADescriptor)),
	    &pp
	)
    );
}


/*---------------------------------------------------------------------------
 *****  Utility to print the contents of the accumulator as a collection of
 *****  block closure indices and return the result strings in the accumulator.
 *
 *  Arguments:
 *	pBlock			- a handle for the block of the block closure.
 *	lbracket		- a string containing the left bracket to
 *				  be used in printing the block.
 *	rbracket		- a string containing the right bracket to
 *				  be used in printing the block.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void SPrintAAsBlockClosureIndices (
    VPrimitiveTask *pTask, rtBLOCK_Handle *pBlock, char const *lbracket, char const *rbracket
) {
    VCPDReference pBlockCPD (0, pBlock->GetCPD ());
    VString iSource;
    RSLANG_Decompile (iSource, pBlockCPD);
    pTask->loadDucWithListOrStringStore (
	rtLSTORE_NewStringStoreWithDelm (
	    pTask->codScratchPad (), lbracket, rbracket, iSource
	)
    );
}


/**************************************************
 *  Primitive/Method/Closure Printing Primitives  *
 **************************************************/

V_DefinePrimitive (PrintPrimitive) {
    pTask->loadDucWithCurrent ();

/*****  SPrint Primitive  *****/
    if (V_TOTSC_Primitive == XSPrintPrimitive) {
        SPrintAAsPrimitiveFunctions (pTask);
    }
/*****  Print Primitive  *****/
    else {
        PrintAAsPrimitiveFunctions (pTask);
	pTask->loadDucWithSelf ();
    }
}

V_DefinePrimitive (PrintMethod) {
    pTask->loadDucWithCurrent ();

    rtBLOCK_Handle::Reference pBlock;
    static_cast<rtMETHOD_Handle*>(pTask->ducStore ())->getBlock (pBlock);

/*****  SPrint Block  *****/
    if (V_TOTSC_Primitive == XSPrintMethod) {
        SPrintAAsBlockClosureIndices (pTask, pBlock, "{", "}");
    }
/*****  Print Block  *****/
    else {
        PrintAAsBlockClosureIndices (pTask, pBlock, '{', '}');
        pTask->loadDucWithSelf ();
    }
}


V_DefinePrimitive (PrintClosure) {
    pTask->loadDucWithCurrent ();

    rtBLOCK_Handle::Reference pBlock; unsigned int primitive;
    pTask->decodeClosureInDuc (pBlock, primitive);

    if (pBlock) {
	/*****  SPrint Block  *****/
	if (V_TOTSC_Primitive == XSPrintClosure) {
	    SPrintAAsBlockClosureIndices (pTask, pBlock, "[", "]");
	}
	/*****  Print Block  *****/
	else {
	    PrintAAsBlockClosureIndices (pTask, pBlock, '[', ']');
	    pTask->loadDucWithSelf ();
	}
    }
    else {
	pTask->loadDucWithPrimitive (primitive);
	/*****  SPrint Primitive  *****/
	if (V_TOTSC_Primitive == XSPrintClosure) {
	    SPrintAAsPrimitiveFunctions (pTask);
	}
	/*****  Print Primitive  *****/
	else {
	    PrintAAsPrimitiveFunctions (pTask);
	    pTask->loadDucWithSelf ();
	}
    }
}


/**************************************
 *****  Time Stamp Print Helpers  *****
 **************************************/

void DecodeLRTTicks (
    unsigned int hTicks,
    unsigned int uTicks,
    unsigned int &rYear,
    unsigned int &rMonth,
    unsigned int &rDay,
    unsigned int &rHour,
    unsigned int &rMinute,
    unsigned int &rSecond,
    unsigned int &rUSecnd
) {
    DATES_ConvertIDate7ToYmd (hTicks / 24, (int*)&rYear, (int*)&rMonth, (int*)&rDay);

    rHour = hTicks % 24;

    rMinute = uTicks / 60000000 % 60;
    rSecond = uTicks /  1000000 % 60;
    rUSecnd = uTicks %  1000000; 
}

template <class T> void PrintTimeStamp (T* pT, VkUnsigned64 const& rTime) {
    unsigned int iYear = 2000, iMonth = 11, iDay = 13, iHour = 14, iMinute = 12, iSecond = 32, iUSecnd = 7;
    DecodeLRTTicks (rTime[0], rTime[1], iYear, iMonth, iDay, iHour, iMinute, iSecond, iUSecnd);
    pT->printf (
	7 * MAXINTDIGITS + 6,
	"%4u/%02u/%02u:%02u:%02u:%02u.%06u",
	iYear, iMonth, iDay, iHour, iMinute, iSecond, iUSecnd
    );
}

template <class T> void PrintTimeStamp (T* pT, VkUnsigned96 const& rTime) {
    unsigned int iYear, iMonth, iDay, iHour, iMinute, iSecond, iUSecnd;
    DecodeLRTTicks (rTime[0], rTime[1], iYear, iMonth, iDay, iHour, iMinute, iSecond, iUSecnd);
    pT->printf (
	7 * MAXINTDIGITS + 6,
	"%4u/%02u/%02u:%02u:%02u:%02u.%06u%09u",
	iYear, iMonth, iDay, iHour, iMinute, iSecond, iUSecnd, rTime[2]
    );
}

#ifdef VMS_LINUX_EXPLICIT_COMPAT
template void PrintTimeStamp<VPrimitiveTask> (VPrimitiveTask*, VkUnsigned64 const&);
template void PrintTimeStamp<VPrimitiveTask> (VPrimitiveTask*, VkUnsigned96 const&);

template void PrintTimeStamp<VOutputGenerator> (VOutputGenerator*, VkUnsigned64 const&);
template void PrintTimeStamp<VOutputGenerator> (VOutputGenerator*, VkUnsigned96 const&);
#endif


/**************************
 *****  Date Printer  *****
 **************************/

V_DefinePrimitive (PrintDate) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    switch (rCurrent.pointerRType ()) {
    case RTYPE_C_IntUV:		//  Date
	if (rCurrent.isScalar ()) {
	    int year, month, day;
	    DATES_ConvertIDate7ToYmd (
		DSC_Descriptor_Scalar_Int (rCurrent), &year, &month, &day
	    );
	    pTask->outputBufferPrintScalar (
		MAXINTDIGITS * 3 + 2, "%d/%d/%d", month, day, year
	    );
	}
	else {
	    int const *datep = rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent));
	    int const *datel = datep + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent));

	    VOutputGenerator iOutputGenerator (pTask);
	    while (datep < datel) {
		int year, month, day;
		DATES_ConvertIDate7ToYmd (*datep++, &year, &month, &day);
		iOutputGenerator.printf (
		    MAXINTDIGITS * 3 + 2, "%d/%d/%d", month, day, year
		);
		iOutputGenerator.advance ();
	    }
	}
	break;

    case RTYPE_C_Unsigned64UV:	//  LowResolutionTime
	if (rCurrent.isScalar ()) {
	    PrintTimeStamp (
		pTask, DSC_Descriptor_Scalar_Unsigned64 (rCurrent)
	    );
	}
	else {
	    rtU64UV_ElementType* pSourceCursor = rtU64UV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );
	    rtU64UV_ElementType* pSourceLimit = pSourceCursor + UV_CPD_ElementCount (
		DSC_Descriptor_Value (rCurrent)
	    );

	    VOutputGenerator iOutputGenerator (pTask);
	    while (pSourceCursor < pSourceLimit) {
		PrintTimeStamp (&iOutputGenerator, *pSourceCursor++);
		iOutputGenerator.advance ();
	    }
	}
	break;

    case RTYPE_C_Unsigned96UV:	// HighResolutionTime
	if (rCurrent.isScalar ()) {
	    PrintTimeStamp (
		pTask, DSC_Descriptor_Scalar_Unsigned96 (rCurrent)
	    );
	}
	else {
	    rtU96UV_ElementType* pSourceCursor = rtU96UV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );
	    rtU96UV_ElementType* pSourceLimit = pSourceCursor + UV_CPD_ElementCount (
		DSC_Descriptor_Value (rCurrent)
	    );

	    VOutputGenerator iOutputGenerator (pTask);
	    while (pSourceCursor < pSourceLimit) {
		PrintTimeStamp (&iOutputGenerator, *pSourceCursor++);
		iOutputGenerator.advance ();
	    }
	}
	break;

    default:
	pTask->raiseException (
	    EC__UsageError,
	    "PrintDate: Unsupported Recipient Value Type: %s",
	    RTYPE_TypeIdAsString (rCurrent.pointerRType ())
	);
	break;
    }

    pTask->loadDucWithSelf ();
}


/************************************
 ************************************
 *****  Primitive Descriptions  *****
 ************************************
 ************************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that describe the primitives to the virtual machine and
 * debugger.
 *---------------------------------------------------------------------------
 */

BeginDescriptions


/******************************
 *****  Print Primitives  *****
 ******************************/

    PD (XPrintSelfAsPointer,
	"PrintSelfAsPointer",
	PrintSelfAsPointer)
    PD (XPrintCurrentAsPointer,
	"PrintCurrentAsPointer",
	PrintCurrentAsPointer)
    PD (XPrintSelector,
	"PrintSelector",
	PrintSelector)
    PD (XPrintString,
	"PrintString",
	PrintString)

    PD (XPrintPrimitive,
	"PrintPrimitive",
	PrintPrimitive)
    PD (XPrintMethod,
	"PrintMethod",
	PrintMethod)
    PD (XPrintClosure,
	"PrintClosure",
	PrintClosure)
    PD (XSPrintPrimitive,
	"SPrintPrimitive",
	PrintPrimitive)
    PD (XSPrintMethod,
	"SPrintMethod",
	PrintMethod)
    PD (XSPrintClosure,
	"SPrintClosure",
	PrintClosure)

    PD (XPrintDate,
	"PrintDate",
	PrintDate)
    PD (XPrintCurrentAs12Pointer,
	"PrintCurrentAs12Pointer",
	PrintCurrentAsPointer)
    PD (XPrint12String,
	"Print12String",
	PrintString)
    PD (XPrint24String,
	"Print24String",
	PrintString)

    PD (XIntegerPrintOf,
	"IntegerPrintOf",
	IntegerPrintOf)
    PD (XIntegerNicePrintOf,
	"IntegerNicePrintOf",
	IntegerPrintOf)
    PD (XDoublePrintOf,
	"DoublePrintOf",
	DoublePrintOf)
    PD (XDoubleNicePrintOf,
	"DoubleNicePrintOf",
	DoublePrintOf)
    PD (XIntegerPrintWithCommasOf,
	"IntegerPrintWithCommasOf",
	IntegerPrintOf)
    PD (XDoublePrintWithCommasOf,
	"DoublePrintWithCommasOf",
	DoublePrintOf)
    PD (XIntegerSPrintOf,
	"IntegerSPrintOf",
	IntegerSPrint)
    PD (XDoubleSPrintOf,
	"DoubleSPrintOf",
	DoubleSPrint)
    PD (XIntegerSPrintWithCommasOf,
	"IntegerSPrintWithCommasOf",
	IntegerSPrint)
    PD (XDoubleSPrintWithCommasOf,
	"DoubleSPrintWithCommasOf",
	DoubleSPrint)

EndDescriptions


/*******************************************************
 *****  The Primitive Dispatch Vector Initializer  *****
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a portion of the primitive function
 *****	dispatch vector at system startup.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void pfPRINT_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfPRINT);
    FAC_FDFCase_FacilityDescription ("Print Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFprint.c 1 replace /users/jad/system
  870205 15:00:03 jad new module, split from PFutil

 ************************************************************************/
/************************************************************************
  PFprint.c 2 replace /users/jad/system
  870210 14:22:27 jad simplify printof primitives to use V_LoadAWithListOrStringStore routine.

 ************************************************************************/
/************************************************************************
  PFprint.c 3 replace /users/jad/system
  870211 16:30:15 jad fixed possible seg. fault in printof primitive with
unreasonably large format size

 ************************************************************************/
/************************************************************************
  PFprint.c 4 replace /users/jad/system
  870217 13:42:34 jad fixed bug in printof primitives

 ************************************************************************/
/************************************************************************
  PFprint.c 5 replace /users/jad/system
  870429 16:56:22 jad fixed print with commas bug: -,123,456

 ************************************************************************/
/************************************************************************
  PFprint.c 6 replace /users/mjc/translation
  870524 09:30:54 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFprint.c 7 replace /users/mjc/system
  870701 22:37:18 mjc Changed 'V_SendBinaryConverse' to 'V_SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  PFprint.c 8 replace /users/m2
  871218 12:47:26 m2 Fixed bug in FormatStringWithFormat which formatted with a negative width

 ************************************************************************/
/************************************************************************
  PFprint.c 9 replace /users/jad/system
  880107 16:17:20 jad fixed bug in justification of strings
in print: induced by the previous vax bug fix.

 ************************************************************************/
/************************************************************************
  PFprint.c 10 replace /users/jad/system
  880509 12:09:04 jad Implement 'sprint' for Blocks, Methods, and Closures

 ************************************************************************/
/************************************************************************
  PFprint.c 12 replace /users/jad/system
  880706 11:29:21 jad Fix Lio Print - Current Version 12

 ************************************************************************/
/************************************************************************
  PFprint.c 14 replace /users/jad/system
  880805 13:51:40 jad Improved the performance of the PrintOf and SPrint primitives - REAL CURRENT VERSION

 ************************************************************************/
/************************************************************************
  PFprint.c 15 replace /users/jck/system
  881207 09:48:46 jck Circumvent Apollo printf() limit

 ************************************************************************/
/************************************************************************
  PFprint.c 16 replace /users/m2/backend
  890503 14:47:24 m2 STD_printf(), and IO_puts() changes

 ************************************************************************/
/************************************************************************
  PFprint.c 17 replace /users/jck/system
  890809 11:30:51 jck Added hidden dependency on vfault.h

 ************************************************************************/
/************************************************************************
  PFprint.c 19 replace /users/jck/system
  890809 14:30:31 jck replace version 17 as most current

 ************************************************************************/

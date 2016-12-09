/*****  Date Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfDATE

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"

#include "vdates.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vprimfns.h"
#include "vstdio.h"

#include "RTptoken.h"
#include "RTlink.h"

#include "RTintuv.h"

/*****  Self  *****/
#include "PFdate.h"


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
 *	300 - 349                       *
 *======================================*/

/****************************************
 *****  Date Conversion Primitives  *****
 ****************************************/

#define XIntegerAsDate				300
#define XDateAsInteger				301
#define XDateYearAsInteger			302
#define XDateMonthAsInteger			303
#define XDateDayAsInteger			304


/****************************************
 *****  Date Arithmetic Primitives  *****
 ****************************************/

#define XByDaysIncrementDate			310
#define XByBDaysIncrementDate			311
#define XByMonthsIncrementDate			312
#define XByMonthBeginningsIncrementDate		313
#define XByMonthEndsIncrementDate		314
#define XByQtrsIncrementDate		315
#define XByQtrBeginningsIncDate	316
#define XByQtrEndsIncrementDate		317
#define XByYearsIncrementDate			318
#define XByYearBeginningsIncrementDate		319
#define XByYearEndsIncrementDate		320

#define XByDaysDecrementDate			330
#define XByBDaysDecrementDate			331
#define XByMonthsDecrementDate			332
#define XByMonthBeginningsDecrementDate		333
#define XByMonthEndsDecrementDate		334
#define XByQtrsDecrementDate		335
#define XByQtrBeginningsDecDate	336
#define XByQtrEndsDecrementDate		337
#define XByYearsDecrementDate			338
#define XByYearBeginningsDecrementDate		339
#define XByYearEndsDecrementDate		340


/********************************
 *  NA Error Handler Utilities  *
 ********************************/

/*****  Macro to find invalid IDate7's  *****/
#define InvalidDate(date)\
    (date <= 0)

/*****  Global Variables  *****/
PrivateVarDef rtLINK_CType *datePrimsGoodLinkc;
PrivateVarDef int datePrimsFoundNa;
PrivateVarDef int datePrimsScalarCase;
PrivateVarDef int datePrimsCount;
PrivateVarDef int datePrimsLastNa;

/*---------------------------------------------------------------------------
 * Macro to Initialize the variables used by the Na handlers.
 *
 * Notes:
 * This macro takes care of everything except for the following variable:
 *
 *	datePrimsCount 		- must be incremented after each calulation
 *            			  for the non scalar case, by the routine
 *                                that is doing the actual calculation
 ******/
#define datePrimsInitialize() {\
    datePrimsFoundNa	= false;\
    datePrimsCount	=\
    datePrimsLastNa	= 0;\
    datePrimsGoodLinkc	= NilOf (rtLINK_CType *);\
    datePrimsScalarCase = pTask->isScalar ();\
}


#define datePrimsNaHandler {\
/***** If scalar case return ... *****/\
    if (datePrimsScalarCase) {\
	datePrimsFoundNa = true;\
    }\
\
/***** If the first NA found ... *****/\
    if (!datePrimsFoundNa) {\
	datePrimsGoodLinkc = pTask->NewSubset ();\
	rtLINK_AppendRange (datePrimsGoodLinkc, 0, datePrimsCount);\
	datePrimsLastNa = datePrimsCount;\
	datePrimsFoundNa = true;\
    }\
    else {\
	rtLINK_AppendRange (\
	    datePrimsGoodLinkc,\
	    datePrimsLastNa + 1,\
	    datePrimsCount - datePrimsLastNa - 1\
	);\
	datePrimsLastNa = datePrimsCount;\
    }\
}


/*---------------------------------------------------------------------------
 * Macro to append the last range on to the datePrimsGoodLinkc, and close it.
 *****/
#define datePrimsFinishAndCloseLinkc() {\
/***** if this had any NA's ... *****/\
    if (datePrimsFoundNa && !datePrimsScalarCase) {\
/***** ...  append the last piece to the linkc and close it ... *****/\
        rtLINK_AppendRange (\
	    datePrimsGoodLinkc,\
	    datePrimsLastNa + 1,\
	    datePrimsCount - datePrimsLastNa - 1\
	);\
        datePrimsGoodLinkc->Close (pTask->NewDomPToken (datePrimsGoodLinkc->ElementCount ()));\
    }\
}

#define datePrimsCleanup() {\
    if (datePrimsGoodLinkc)\
	datePrimsGoodLinkc->release ();\
}


/****************************************
 *****  Date Conversion Primitives  *****
 ****************************************/

/******************************************************
 *  Date <-> Standard Format Integer Date Primitives  *
 ******************************************************/

V_DefinePrimitive (IntegerAsDate) {
/*****  Obtain the integers...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Initialize the NA handler ...  *****/
    datePrimsInitialize ();

/*****  And convert them...  *****/
/*****  Scalar Case  *****/
    if (rCurrent.isScalar ()) {
	int result = DATES_ConvertStandardToIDate7 (DSC_Descriptor_Scalar_Int (rCurrent));

	if (InvalidDate (result))
	    pTask->loadDucWithNA ();
	else 
	    pTask->loadDucWithDate (result);
    }

/*****  Non-scalar case  *****/
    else {
	M_CPD *intuv  = DSC_Descriptor_Value (rCurrent);
	M_CPD *dateuv = pTask->NewDateUV ();
        int
	    *datep = rtINTUV_CPD_Array (dateuv),
	    *datel = datep + UV_CPD_ElementCount (dateuv),
	    *intp  = rtINTUV_CPD_Array (intuv);

	/*****  Calculate the dates ...  *****/
	while (datep < datel) {
	    int result = *datep++ = DATES_ConvertStandardToIDate7 (*intp++);
	    if (InvalidDate (result)) {
		datePrimsNaHandler;
	    }
	    datePrimsCount++;
	}

	/*****  Load the result into the accumulator ... *****/
	if (datePrimsFoundNa) {
	    datePrimsFinishAndCloseLinkc ();
	}
	pTask->loadDucWithPartialFunction (datePrimsGoodLinkc, dateuv);
	datePrimsCleanup ();
	dateuv->release ();
    }
}


V_DefinePrimitive (DateAsInteger) {
/*****  Obtain the integers...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  And convert them...  *****/
    if (rCurrent.isScalar ()) {
	pTask->loadDucWithInteger (
	    DATES_ConvertIDate7ToStandard (DSC_Descriptor_Scalar_Int (rCurrent))
	);
    }
    else {
	M_CPD *dateuv = DSC_Descriptor_Value (rCurrent);
	M_CPD *intuv  = pTask->NewIntUV ();
        int
	    *datep = rtINTUV_CPD_Array (dateuv),
	    *datel = datep + UV_CPD_ElementCount (dateuv),
	    *intp  = rtINTUV_CPD_Array (intuv);

	while (datep < datel) {
	    *intp++ = DATES_ConvertIDate7ToStandard (*datep++);
	}

	pTask->loadDucWithMonotype (intuv);

	intuv->release ();
    }
}

/*****************************************
 *  Date Component Extraction Primitive  *
 *****************************************/

V_DefinePrimitive (DateComponentAsInteger) {
/*****  Decode the alias by which this primitive was invoked...  *****/

    int year, month, day, *dateComponent;
    switch (V_TOTSC_Primitive) {
    case XDateYearAsInteger:
        dateComponent = &year;
        break;
    case XDateMonthAsInteger:
        dateComponent = &month;
        break;
    case XDateDayAsInteger:
        dateComponent = &day;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("DateComponentAsInteger");
        break;
    }

/*****  Obtain the dates...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  And extract the appropriate component...  *****/
    if (rCurrent.isScalar ()) {
	DATES_ConvertIDate7ToYmd (
	    DSC_Descriptor_Scalar_Int (rCurrent), &year, &month, &day
	);

	pTask->loadDucWithInteger (*dateComponent);
    }
    else {
	M_CPD *dateuv = DSC_Descriptor_Value (rCurrent);
	M_CPD *intuv  = pTask->NewIntUV ();
        int
	    *datep = rtINTUV_CPD_Array (dateuv),
	    *datel = datep + UV_CPD_ElementCount (dateuv),
	    *intp  = rtINTUV_CPD_Array (intuv);

	while (datep < datel) {
	    DATES_ConvertIDate7ToYmd (*datep++, &year, &month, &day);
	    *intp++ = *dateComponent;
	}

	pTask->loadDucWithMonotype (intuv);

	intuv->release ();
    }
}

/****************************************
 *****  Date Arithmetic Primitives  *****
 ****************************************/

V_DefinePrimitive (IncrementOrDecrementDate) {
    int
	dateIncrementDirection,
	dateIncrementType;

/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    /*****  Increment Cases  *****/
    case XByDaysIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_DAY;
	break;
    case XByBDaysIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_BDAY;
	break;
    case XByMonthsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_MONTH;
	break;
    case XByMonthBeginningsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_MONTHBEGIN;
	break;
    case XByMonthEndsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_MONTHEND;
	break;
    case XByQtrsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_QUARTER;
	break;
    case XByQtrBeginningsIncDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_QUARTERBEGIN;
	break;
    case XByQtrEndsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_QUARTEREND;
	break;
    case XByYearsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_YEAR;
	break;
    case XByYearBeginningsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_YEARBEGIN;
	break;
    case XByYearEndsIncrementDate:
	dateIncrementDirection = 1;
	dateIncrementType = DATES_DateIncr_YEAREND;
	break;

    /*****  Decrement Cases  *****/
    case XByDaysDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_DAY;
	break;
    case XByBDaysDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_BDAY;
	break;
    case XByMonthsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_MONTH;
	break;
    case XByMonthBeginningsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_MONTHBEGIN;
	break;
    case XByMonthEndsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_MONTHEND;
	break;
    case XByQtrsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_QUARTER;
	break;
    case XByQtrBeginningsDecDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_QUARTERBEGIN;
	break;
    case XByQtrEndsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_QUARTEREND;
	break;
    case XByYearsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_YEAR;
	break;
    case XByYearBeginningsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_YEARBEGIN;
	break;
    case XByYearEndsDecrementDate:
	dateIncrementDirection = (-1);
	dateIncrementType = DATES_DateIncr_YEAREND;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("IncrementOrDecrementDate");
        break;
    }

/*****
 *  Send this primitive's converse message to the parameter if that
 *  parameter is polymorphic or non-date...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (
	    dateIncrementDirection < 0 ? KS__Minus : KS__Plus
	);
	return;
    }

    if (pTask->ducStore ()->DoesntNameTheDateClass ()) {
        pTask->sendBinaryConverseWithCurrent (
	    dateIncrementDirection < 0 ? KS__Minus : KS__Plus
	);
	return;
    }

/*****  Otherwise, normalize the parameter, ...  *****/
    pTask->normalizeDuc ();

/*****  ... access the increment values, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... and perform the operation...  *****/

    if (rCurrent.isScalar ()) {
	pTask->loadDucWithDate (
	    DATES_IncrementDate (
		DSC_Descriptor_Scalar_Int (ADescriptor),
		dateIncrementDirection * DSC_Descriptor_Scalar_Int (rCurrent),
		dateIncrementType
	    )
	);
    }
    else {
	M_CPD *incruv  = DSC_Descriptor_Value (rCurrent);
	M_CPD *sdateuv = DSC_Descriptor_Value (ADescriptor);
	M_CPD *rdateuv = pTask->NewDateUV ();
        int
	    *incrp  = rtINTUV_CPD_Array (incruv),
	    *incrl  = incrp + UV_CPD_ElementCount (incruv),
	    *sdatep = rtINTUV_CPD_Array (sdateuv),
	    *rdatep = rtINTUV_CPD_Array (rdateuv);

	while (incrp < incrl) {
	    *rdatep++ = DATES_IncrementDate (
		*sdatep++, dateIncrementDirection * *incrp++, dateIncrementType
	    );
	}

	pTask->loadDucWithMonotype (rdateuv);

	rdateuv->release ();
    }
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

/****************************************
 *****  Date Conversion Primitives  *****
 ****************************************/

    PD (XIntegerAsDate,
	"IntegerAsDate",
	IntegerAsDate)
    PD (XDateAsInteger,
	"DateAsInteger",
	DateAsInteger)
    PD (XDateYearAsInteger,
	"DateYearAsInteger",
	DateComponentAsInteger)
    PD (XDateMonthAsInteger,
	"DateMonthAsInteger",
	DateComponentAsInteger)
    PD (XDateDayAsInteger,
	"DateDayAsInteger",
	DateComponentAsInteger)

/****************************************
 *****  Date Arithmetic Primitives  *****
 ****************************************/

    PD (XByDaysIncrementDate,
	"ByDaysIncrementDate",
	IncrementOrDecrementDate)
    PD (XByBDaysIncrementDate,
	"ByBDaysIncrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthsIncrementDate,
	"ByMonthsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthBeginningsIncrementDate,
	"ByMonthBeginningsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthEndsIncrementDate,
	"ByMonthEndsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrsIncrementDate,
	"ByQuartersIncrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrBeginningsIncDate,
	"ByQuarterBeginningsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrEndsIncrementDate,
	"ByQuarterEndsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByYearsIncrementDate,
	"ByYearsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByYearBeginningsIncrementDate,
	"ByYearBeginningsIncrementDate",
	IncrementOrDecrementDate)
    PD (XByYearEndsIncrementDate,
	"ByYearEndsIncrementDate",
	IncrementOrDecrementDate)

    PD (XByDaysDecrementDate,
	"ByDaysDecrementDate",
	IncrementOrDecrementDate)
    PD (XByBDaysDecrementDate,
	"ByBDaysDecrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthsDecrementDate,
	"ByMonthsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthBeginningsDecrementDate,
	"ByMonthBeginningsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByMonthEndsDecrementDate,
	"ByMonthEndsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrsDecrementDate,
	"ByQuartersDecrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrBeginningsDecDate,
	"ByQuarterBeginningsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByQtrEndsDecrementDate,
	"ByQuarterEndsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByYearsDecrementDate,
	"ByYearsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByYearBeginningsDecrementDate,
	"ByYearBeginningsDecrementDate",
	IncrementOrDecrementDate)
    PD (XByYearEndsDecrementDate,
	"ByYearEndsDecrementDate",
	IncrementOrDecrementDate)

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
PublicFnDef void pfDATE_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfDATE);
    FAC_FDFCase_FacilityDescription ("Date Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFdate.c 1 replace /users/jad/system
  861116 16:06:24 jad new date primitive functions

 ************************************************************************/
/************************************************************************
  PFdate.c 2 replace /users/jad/system
  870220 18:58:52 jad fix IntegerAsDate primitive to return NA's for invalid dates

 ************************************************************************/
/************************************************************************
  PFdate.c 3 replace /users/mjc/translation
  870524 09:30:29 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFdate.c 4 replace /users/mjc/system
  870701 22:37:28 mjc Changed 'V_SendBinaryConverse' to 'V_SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  PFdate.c 5 replace /users/mjc/Software/system
  880108 15:58:23 mjc Added decoding binary converse to 'IncrementOrDecrementDate'

 ************************************************************************/

/*****  Date Utilities Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName DATES

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "vdebug.h"
#include "verr.h"
#include "vfac.h"
#include "viobj.h"
#include "vstdio.h"

#include "V_VTime.h"

/*****  Shared  *****/
#include "vdates.h"


/***********************************************
 *****  Internal Defines and Declarations  *****
 ***********************************************/

#define Min(a, b) ( (a) < (b) ? (a) : (b) )

#define ErrorExit(message) ERR_SignalFault (EC__UsageError, message)

#define errorBadDate		ErrorExit("DATES: Error Bad Date.")
#define errorBadDateForm	ErrorExit("DATES: Error Bad DateForm.")
#define errorBadStandardDate	ErrorExit("DATES: Error Bad Standard Date.")
#define errorBad7DayDate	ErrorExit("DATES: Error Bad 7 Day Date.")
#define errorBadJulianDate	ErrorExit("DATES: Error Bad Julian Date.")
#define errorBadDateIncrement	ErrorExit("DATES: Error Bad Date Increment.")
#define errorBadDateIncrementMultiple ErrorExit("DATES: Error Bad Date Incrment Multiple.")

#define _DATES_AdjWeekEndBACK -1
#define _DATES_AdjWeekEndFORWARD 1

#define OutputSlash 1
#define OutputSlashShort 2
#define OutputFull 3
#define OutputFullShort 4
#define OutputDayFirst 5
#define OutputDayFirstShort 6
#define OutputMonth 7
#define OutputMonthShort 8

PublicVarDef int _DATES_DayVector[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

PublicVarDef int _DATES_CumDays[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

PublicVarDef int _DATES_CumDays_LeapYear[] = {
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366
};

PrivateVarDef char const* const MonthNames [] = {
    "January"  , "February", "March"   , "April",
    "May"      , "June"    , "July"    , "August",
    "September", "October" , "November", "December"
};

PrivateVarDef char const* const MonthNamesShort [] = {
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug", 
    "Sep", "Oct", "Nov", "Dec"
};

PrivateVarDef int const YearBreaks[4] = {400, 100, 4, 1};
PrivateVarDef int const DayChunks [4] = {146097, 36524, 1461, 365};


/*********************************************************************
 *************************   DATE ROUTINES   *************************
 *********************************************************************/

/*---------------------------------------------------------------------------
 * GENERAL DEFINITIONS:
 *   
 *   ymd               Year-month-day form - the three components
 *                     are supplied or returned separately.  Also
 *                     known as gregorian date form.
 *   standard          An integer date form which expresses a ymd
 *                     date as YYYYMMDD.  
 *   idate7            An integer date form which expresses a date
 *                     as the number of days since 1/1/0000 (which
 *                     equals 1).  This
 *                     form is the normal unit of date interaction.
 *   idate5            An integer date form which expresses a date
 *                     as the number of business (monday - friday)
 *                     days since 1/1/0000 (which equals 1).
 *   julian            The number of days since January 1 of the
 *                     year specified by a date.
 *   year              integer year > 0.
 *   month             integer month (1 ... 12)
 *   day               integer day of month (1 ... 31) depending on month
 *   jday              integer day of year (1  ... 366) depeding on year
 *
 *   jdate             integer date form (YYYYDDD) for representing a
 *                     julian date
 *   bday              business day
 *
 * NOTES:
 *   -  The integer date forms (idate5, idate7, standard, and jdate) are
        collectively known as internal dates (idates).
 *
 *---------------------------------------------------------------------------
 */


/***************************
 *** CONVERSION ROUTINES ***
 ***************************/

/*---------------------------------------------------------------------------
 * CONVERSION ROUTINES:   routines to convert from one date form
 *                        to another.
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:  to convert a julian date (year/jday) into ymd form
 *            (year/month/day)
 * ARGUMENTS
 *   year     integer year
 *   jday     integer jday
 *   *month   integer month to be computed 
 *   *day     integer day to be computed 
 *
 * RETURNS
 *   *month      computed month
 *   *day        computed day
 *   function    boolean based on DATES_ValidYmd(year,month,day)
 *               calculation
 *
 * ASSUMPTIONS:  jday is valid for year provided
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertJulianToYmd (
    int				year,
    int				jday,
    int *			month,
    int *			day
)
{
    *month = DATES_FindJulianMonth (year, jday);
    *day = (jday - DATES_ComputeCumDaysToMonth (year, *month - 1));

    return (DATES_ValidYmd (year, *month, *day) );
}     /*  convert julian to ymd  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert a standard date to its year/month/day
 *           components
 * ARGUMENTS
 *   st_date   integer date in standard date form
 *   *year     integer year computed from st_date
 *   *month    integer month computed from st_date
 *   *day      integer day computed from st_date
 *
 * RETURNS
 *   *year     
 *   *month
 *   *day
 *   function returns DATES_ValidYmd (*year,*month,*day)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertStandardToYmd (
    int				st_date,
    int *			year,
    int *			month,
    int *			day
)
{
    *year = st_date /10000;
    *month = (st_date - *year * 10000)/100;
    *day = st_date % 100;

    return (DATES_ValidYmd (*year, *month, *day) );
}     /*  convert standard to ymd  */


/*---------------------------------------------------------------------------
 * FUNCTION:   to convert a julian date (year,jday) to integer standard
 *             julian date form (jdate).
 * ARGUMENTS
 *   year      integer year
 *   jday      valid integer jday 
 *
 * RETURNS:    jdate (YYYYDDD)
 *
 * ASSUMES
 *   -   year < 100 assumes 1900
 *   -   DATES_ValidJulianDay (year,jday)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertJulianToJDate (
    int				year,
    int				jday
)
{
    int				tempyear = year;

    if (year < 100) tempyear = year + 1900;

    return (tempyear * 1000 + jday);
}     /*  convert julian to jdate  */


/*---------------------------------------------------------------------------
 * FUNCTION:   to convert the standard julian date code jdate to its
 *             year and jday components
 *
 * ARGUMENTS
 *   jdate     integer standard julian date code
 *   *year     computed year
 *   *jday     computed jday
 *
 * RETURNS
 *   *year
 *   *jday
 *   function returns DATES_ValidJulianDay (year,jday)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertJDateToJulian (
    int				jdate,
    int *			year,
    int *			jday
)
{
    *year = jdate/1000;
    *jday = (jdate - *year *1000) % 1000;

    return DATES_ValidJulianDay (*year, *jday);
}     /*  convert jdate to julian  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert a ymd to a 7day date code
 *
 * ARGUMENTS
 *   year   integer year
 *   month  integer month
 *   day    integer day
 *
 * RETURNS:  idate7
 *
 * APPROACH: Using the full years (ie through year -1) compute
 *           the number of 400, 100, 4, and 1 year chunks.  We
 *           know that every 400 years there will be:
 *             (365*400) + (100/4-1)*4 + 1 =              146097 days
 *           every additional 100 years there will be:
 *             (365*100) + (100/4-1)       =               36524 days
 *           every additional 4 years there will be:
 *             (365*4) + 1                 =                1461 days
 *           every additional year there will be:            365 days
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertYmdToIDate7 (
    int				year,
    int				month,
    int				day
)
{
    int				ndays, i, chunks, yearsleft;

    ndays = 0;
    yearsleft = year - 1;

    for (i=0; i<4 && yearsleft >0; i++)
    {
	chunks = (yearsleft/YearBreaks[i]);     /* compute whole chunks  */
	ndays += chunks * DayChunks[i];         /* add appropriate ndays */
	yearsleft = yearsleft % YearBreaks[i];  /* compute years left   */
    }

/*  add for current year  */
    ndays += DATES_ComputeCumDaysToMonth (year, month-1) + day;

    return ndays;
}     /* convert ymd to idate7  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert 7day date code to its component parts
 *
 * ARGUMENTS
 *   idate7       7day date code to be converted
 *   *year        computed year
 *   *month       computed month
 *   *day         computed day
 *
 * RETURNS
 *   *year
 *   *month
 *   *day
 *   function returns DATES_ValidYmd (*year, *month, *day)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertIDate7ToYmd (
    int				idate7,
    int *			year,
    int *			month,
    int *			day
)
{
    int				i, chunks, daysleft;

    *year = 0;
    *month = 0;
    *day = 0;

    daysleft = idate7;

    for (i=0; i<4 && daysleft >0; i++)
    {
	chunks = (daysleft/DayChunks[i]);         /* compute whole chunks  */
	*year += chunks * YearBreaks[i];          /* add appropriate ndays */
	daysleft = daysleft % DayChunks[i];     /* compute days left   */
    }

/*  days left represent julian day in year  */
    if (daysleft == 0)
    {
	daysleft = DATES_ComputeDaysInYear (*year);
	if (DATES_IsLeapYear (*year) )  /* adjust for over correct on some leapyrs */
	    daysleft = ( (i % 2) == 0 ) ? daysleft - 1 : daysleft;  
    }
    else *year += 1;

    if (!DATES_ConvertJulianToYmd (*year, daysleft, month, day) )
	return DATES_BadJulianDate;
   
    return DATES_ValidYmd (*year, *month, *day);
}     /*  convert idate7 to ymd  */


/*---------------------------------------------------------------------------
 ************ CONVERT BETWEEN INTERNAL FORMS
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION: to convert an internal date in fromDateform to an
 *           internal date in toDateform.
 * ARGUMENTS
 *   date               an internal date code
 *   fromDateform       one of the internal DateformFORMs
 *   toDateform         one of the internal DateformFORMs
 *
 * RETURNS: an internal date in toDateform.
 *
 * ASSUMPTION: date is a valid fromDateform type of date
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertFromDateToDate (
    int				date,
    DATES_Dateform		fromDateform,
    DATES_Dateform		toDateform
)
{
   switch (toDateform)
   {
   case DATES_DateformSTANDARD:
      return DATES_ConvertDateToStandard (date, fromDateform);
   case DATES_Dateform7DAY:
      return DATES_ConvertDateToIDate7 (date, fromDateform);
   case DATES_Dateform5DAY:
      return DATES_ConvertDateToIDate5 (date, fromDateform);
   case DATES_DateformJDATE:
      return DATES_ConvertDateToJDate (date, fromDateform);
   default:
      return DATES_BadDateForm;
   }

}     /*  convert fromdate todate  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert an internal date code in fromDateform
 *           to a 7day date code.
 *
 * ARGUMENTS
 *   date            internal date code in fromDateform
 *   fromDateform    one of the internal DateformFORMs
 *
 * RETURNS:  internal 7day date code.  
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertDateToIDate7 (
    int				date,
    DATES_Dateform		fromDateform
)
{
   switch (fromDateform)
   {
   case DATES_DateformSTANDARD:
      return DATES_ConvertStandardToIDate7 (date);
   case DATES_Dateform7DAY:
      return date;
   case DATES_Dateform5DAY:
      return DATES_ConvertIDate5ToIDate7 (date);
   case DATES_DateformJDATE:
      return DATES_ConvertJDateToIDate7 (date);
   default:
      return DATES_BadDateForm;
   }

}     /*  convert date to idate7  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert internal date in fromDateform to internal 5day
 *            date code.
 * ARGUMENTS
 *   date         internal date in fromDateform
 *   fromDateform one of the internal DateformFORMs
 *
 * RETURNS: internal 5day date code.
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertDateToIDate5 (
    int				date,
    DATES_Dateform		fromDateform
)
{
   switch (fromDateform)
   {
   case DATES_DateformSTANDARD:
      return DATES_ConvertStandardToIDate5 (date);
   case DATES_Dateform7DAY:
      return DATES_ConvertIDate7ToIDate5 (date, _DATES_AdjWeekEndBACK);
   case DATES_Dateform5DAY:
      return date;
   case DATES_DateformJDATE:
      return DATES_ConvertJDateToIDate5 (date);
   default:
      return DATES_BadDateForm;
   }

}     /*  convert date to idate5  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert internal date in fromDateform to standard date
 *           code.
 * ARGUMENTS
 *   date         internal date in fromDateform
 *   fromDateform one of the internal DateformFORMs
 *
 * RETURNS:  internal standard date code.
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertDateToStandard (
    int				date,
    DATES_Dateform		fromDateform
)
{
   switch (fromDateform)
   {
   case DATES_DateformSTANDARD:
      return date;
   case DATES_Dateform7DAY:
      return DATES_ConvertIDate7ToStandard (date);
   case DATES_Dateform5DAY:
      return DATES_ConvertIDate5ToStandard (date);
   case DATES_DateformJDATE:
      return DATES_ConvertJDateToStandard (date);
   default:
      return DATES_BadDateForm;
   }

}     /*  convert date to standard  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert internal date in fromDateform to standard juliandate
 *           code.
 * ARGUMENTS
 *   date         internal date in fromDateform
 *   fromDateform one of the internal DateformFORMs
 *
 * RETURNS:  internal standard julian date code.
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertDateToJDate (
    int				date,
    DATES_Dateform		fromDateform
)
{

switch (fromDateform)
   {
   case DATES_DateformSTANDARD:
      return DATES_ConvertStandardToJDate (date);
   case DATES_Dateform7DAY:
      return DATES_ConvertIDate7ToJDate (date);
   case DATES_Dateform5DAY:
      return DATES_ConvertIDate5ToJDate (date);
   case DATES_DateformJDATE:
      return date;
   default:
      return DATES_BadDateForm;
   }

}     /*  convert date to jdate  */


/*---------------------------------------------------------------------------
 * FUNCTION: to convert standard date code to 7day date code
 *
 * ARGUMENTS
 *   st_date   a standard date code
 *
 * RETURNS:   idate7.  
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertStandardToIDate7 (
    int				st_date
)
{
    int				year, month, day;

    if (!DATES_ConvertStandardToYmd (st_date, &year, &month, &day)
    ) return DATES_BadStandardDate;

    return DATES_ConvertYmdToIDate7 (year, month, day);
}     /*  convert standard to idate7  */

/*---------------------------------------------------------------------------
 * FUNCTION: to convert a 7day date code to standard date code form
 *
 * ARGUMENTS
 *   idate7    7day date code
 *
 * RETURNS:    standard date code. 
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertIDate7ToStandard (
    int				idate7
)
{
    int				year, month, day;

    if (!DATES_ConvertIDate7ToYmd (idate7, &year, &month, &day)
    ) return DATES_Bad7DayDate;

    return DATES_ConvertYmdToStandard (year, month, day);
}     /* convert idate7 to standard  */

/*---------------------------------------------------------------------------
 * FUNCTION: to convert a 7day date code to a 5day date code.
 *           Sat and Sun converted to previous Friday unless
 *           adjustweekend is > 0 and then uses following Monday
 * ARGUMENTS
 *   idate7            valid 7day date code
 *   adjustweekend     <= 0 use Friday; otherwise use Monday if
 *                     day is a weekend
 *
 * RETURNS:  internal 5day date code
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertIDate7ToIDate5 (
    int				idate7,
    int				adjustweekend
)
{
    int				dayofweek;

    dayofweek = DATES_FindDayOfWeek (idate7, DATES_Dateform7DAY);
    dayofweek = dayofweek > 5
    	? (adjustweekend == _DATES_AdjWeekEndFORWARD ? 6 : 5)
	: dayofweek;

    return (idate7-1)/7 * 5 + dayofweek;
}     /*  convert idate7 to idate5  */


/*---------------------------------------------------------------------------
 * FUNCTION:      to convert a standard julian date to a 7day date code
 *
 * ARGUMENTS
 *   jdate        an internal julian standard date code
 *
 * RETURNS:       internal 7day date code. 
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertJDateToIDate7 (
    int				jdate
)
{
    int				year, jday, month, day;

    if (!DATES_ConvertJDateToJulian (jdate, &year, &jday) ||
	!DATES_ConvertJulianToYmd (year, jday, &month, &day)
    ) return DATES_BadJulianDate;

    return DATES_ConvertYmdToIDate7 (year, month, day);
}     /*  convert jdate to idate7  */

/*---------------------------------------------------------------------------
 * FUNCTION:     to convert an internal 7day date code to a standard
 *               internal julian date form
 * ARGUMENT
 *   idate7      valid 7day date code
 *
 * RETURNS       internal standard julian date code.  
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_ConvertIDate7ToJDate (
    int				idate7
)
{
    int year, month, day;

    if (!DATES_ConvertIDate7ToYmd (idate7, &year, &month, &day)
    ) return DATES_Bad7DayDate;

    return DATES_ConvertJulianToJDate (
	year, DATES_ConvertYmdToJulian (year, month, day)
    );
}     /*  convert idate7 to jdate  */


/*---------------------------------------------------------------------------
 * *********************   Convert Between specialized forms
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION: To convert a 7-day date code that corresponds to a weekend
 *           date to the previous Friday or following Monday as specified
 *
 * ARGUMENTS:
 *    idate7           a 7day date code
 *    AdjWeekEnd    FORWARD or BACK (Monday or Friday)
 *
 * RETURNS:  a 7day date code representing the adjusted date - returns
 *           original date if not a weekend
 *======================================================================*/
PublicFnDef int DATES_ConvertIDate7ToWeekday (
    int				idate7,
    int				AdjWeekEnd
)
{
    int				dayofweek;

    dayofweek = DATES_FindDayOfWeek (idate7, DATES_Dateform7DAY);
    if (dayofweek <= 5) return idate7;
	
    return AdjWeekEnd == _DATES_AdjWeekEndFORWARD
	? ( dayofweek == 6 ? idate7 + 2 : idate7 + 1)
        : ( dayofweek == 6 ? idate7 - 1 : idate7 - 2);
}     /*  convert idate7 to weekday  */


/***********************
 *** FIND ROUTINES ***
 ***********************/

/*---------------------------------------------------------------------------
 * FIND ROUTINES:   basic date routines to find date specific properties
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:  to return todays date as a 7day date code and as its
 *            ymd components
 *
 * ARGUMENTS:
 *   *year    pointer to integer year
 *   *month   pointer to integer month
 *   *day     pointer to integer day
 *
 * RETURNS:   7day date code representing todays date
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_FindTodaysDate (
    int *			year,
    int *			month,
    int *			day
) {
    V::VTime iNow;
    struct tm iTimeStruct;
    struct tm *timeparts = iNow.localtime (iTimeStruct);

    *year = timeparts -> tm_year + 1900;
    *month = timeparts -> tm_mon + 1;
    *day = timeparts -> tm_mday;

    return DATES_ConvertYmdToIDate7 (*year, *month, *day);
}     /*  find todays date  */


/*---------------------------------------------------------------------------
 * FUNCTION:   to find the month in which the jday falls
 *
 * ARGUMENTS
 *   year      valid year
 *   jday      valid jday for year
 *
 * RETURNS:    integer month where 1-jan ... 12-dec
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_FindJulianMonth (
    int				year,
    int				jday
)
{
    int				leapyear, m;

    leapyear = DATES_IsLeapYear (year);

    for (m=0; m<12; m++) if (
    	( leapyear && jday <= _DATES_CumDays_LeapYear[m]) ||
        (!leapyear && jday <= _DATES_CumDays[m])
    ) return m;

    return 12;
}     /*  find julian month  */


/*---------------------------------------------------------------------------
 * FUNCTION: to return the day of the week 1-mon ... 7-sun for the date
 *
 * ARGUMENTS
 *   date         a date in an internal date form
 *   dateform     a valid internal Dateform
 *
 * RETURNS:   integer day of week (1 ... 7)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_FindDayOfWeek (
    int				date,
    DATES_Dateform		dateform
)
{
    int				tempdate = date;

    switch (dateform)
    {
    default:
    case DATES_DateformSTANDARD:
    case DATES_DateformJDATE:
	tempdate = DATES_ConvertDateToIDate7 (date, dateform);
    case DATES_Dateform7DAY:
	return ( (tempdate-1) % 7) + 1;
    case DATES_Dateform5DAY:
	return ( (tempdate-1) % 5) + 1;
    }   /* of switch  */
}     /*  find day of week  */

/* ----------------------------------------------------------------------*/

/**************************************
 *****  Routines to format dates  *****
 **************************************/

/*---------------------------------------------------------------------------
 *  FUNCTION: To format the day of the week given a 7day date code
 */
PublicFnDef void DATES_FormatDayOfWeek (
    int				idate7,
    char *			string
)
{
   switch (DATES_FindDayOfWeek (idate7, DATES_Dateform7DAY) )
   {
   case 1:
      strcpy (string,  "Monday");
      return;
   case 2:
      strcpy (string,  "Tuesday");
      return;
   case 3:
      strcpy (string, "Wednesday");
      return;
   case 4:
      strcpy (string,  "Thursday");
      return;
   case 5:
      strcpy (string, "Friday");
      return;
   case 6:
      strcpy (string,  "Saturday");
      return;
   case 7:
      strcpy (string,  "Sunday");
      return;
   }

}     /* format day of week */


/*---------------------------------------------------------------------------
 *  FUNCTION: to format the day of week for a 5day date as a string
 */
PublicFnDef void DATES_FormatBDayOfWeek (
    int				idate5,
    char *			string
)
{
   switch (DATES_FindDayOfWeek (idate5, DATES_Dateform5DAY) )
   {
   case 1:
      strcpy (string,  "Monday");
      return;
   case 2:
      strcpy (string,  "Tuesday");
      return;
   case 3:
      strcpy (string, "Wednesday");
      return;
   case 4:
      strcpy (string,  "Thursday");
      return;
   case 5:
      strcpy (string, "Friday");
      return;
   }

}     /*  format bday of week  */


/*---------------------------------------------------------------------------
 *  FUNCTION: to format the date in 1 of output formats
 */
PublicFnDef void DATES_FormatIDate7 (
    int				idate7,
    int				format,
    char *			string
)
{
    int				year, month, day;

    DATES_ConvertIDate7ToYmd (idate7, &year, &month, &day);

    switch (format)
    {
    case OutputSlash:
	sprintf (string, "%d/%d/%d", month, day, year % 100);
	return;
    case OutputSlashShort:
	sprintf (string,  "%d/%d", month, day);
	return;
    case OutputFull:
	sprintf (string,  "%s %d, %d", MonthNames[month-1], day, year);
	return;
    case OutputFullShort:
	sprintf (
	    string,
	    "%s %d, %d", MonthNamesShort[month-1], day, year
	);
	return;
    case OutputDayFirst:
	sprintf (string, "%d %s %d", day, MonthNames[month-1], year);
	return;
    case OutputDayFirstShort:
	sprintf (string,  "%d %s %d", day, MonthNamesShort[month-1], year %100);
	return;
    case OutputMonth:
	strcpy (string, MonthNames[month-1]);
	return;
    case OutputMonthShort:
	strcpy (string,  MonthNamesShort[month-1]);
	return;
    }
}     /*  format idate7  */


/*********************************************************************
 ************************   DATE RANGE ROUTINES   ********************
 *********************************************************************/

/*---------------------------------------------------------------------------
 * GENERAL DEFINITIONS:
 *   
 *
 * NOTES:
 *
 *---------------------------------------------------------------------------
 */


/*******************
 *** COUNT IN RANGE
 *******************/

/*---------------------------------------------------------------------------
 * ***** COUNT IN RANGE: routines that compute the number of observations
 *                       of a given dateIncrement in a range
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION: to compute the number of observations of type dateIncrement
 *           exist in date range
 * ARGUMENTS:
 *   startdate        7day date code
 *   enddate          7day date code
 *   dateIncrement    one of valid DATES_DateIncr_DATE forms
 *
 * RETURNS:  number of observations.
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_CountDatesInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
)
{
   switch (dateIncrement)
   {
   case DATES_DateIncr_DAY:
      return DATES_CountDaysInRange (startdate, enddate);
   case DATES_DateIncr_BDAY:
      return DATES_CountBDaysInRange (startdate, enddate);
   case DATES_DateIncr_MONTH:
   case DATES_DateIncr_MONTHBEGIN:
   case DATES_DateIncr_MONTHEND:
      return DATES_CountMonthsInRange (startdate, enddate, dateIncrement);
   case DATES_DateIncr_QUARTER:
   case DATES_DateIncr_QUARTERBEGIN:
   case DATES_DateIncr_QUARTEREND:
      return DATES_CountQtrsInRange (startdate, enddate, dateIncrement);
   case DATES_DateIncr_YEAR:
   case DATES_DateIncr_YEARBEGIN:
   case DATES_DateIncr_YEAREND:
      return DATES_CountYearsInRange (startdate, enddate, dateIncrement);
      
   default:
      return DATES_BadDateIncrement;

   }  /* of switch  */
}      /* count dates in range  */


/*---------------------------------------------------------------------------
 * FUNCTION: to count the number of days in a date range where the
 *           start and end dates are specified in 7 day date codes.
 *           The end points are included in the count.            
 * ARGUMENTS
 *   idate7_start    7day date code representing start date
 *   idate7_end      7day date code representing end date
 *
 * RETURNS:  integer representing number of days in range
 *
 * NOTE:     routine is indifferent to date order
*---------------------------------------------------------------------------
*/
PublicFnDef int DATES_CountDaysInRange (
    int				idate7_start,
    int				idate7_end
)
{
    return (abs (idate7_end - idate7_start) + 1);
}     /*  count days in range  */

/*---------------------------------------------------------------------------
 * FUNCTION: to count the number of business days in a date range
 *           where the start end end dates are specified in 7 day
 *           date codes.  The end points are included in the count
 *           if they are business days.
 * ARGUMENTS
 *   idate7_start    7day date code representing start date
 *   idate7_end      7day date code representing end date
 *
 * RETURNS:  integer representing number of business days in range
 *
 * NOTE:   
 *   -  routine is indifferent to date order
 *   -  if two dates are consecutive and both weekends, routine
 *      returns 0 rather than doing incorrect adjustment
*---------------------------------------------------------------------------
*/
PublicFnDef int DATES_CountBDaysInRange (
    int				idate7_start,
    int				idate7_end
)
{
    int idate5_start,  idate5_end, adjuststart, adjustend;

    if (
	abs (idate7_start - idate7_end) == 1  &&
	DATES_IsWeekend(idate7_start)       &&
	DATES_IsWeekend(idate7_end  )
    ) return 0;

    if (idate7_start > idate7_end)
    {
	adjuststart = _DATES_AdjWeekEndBACK;
	adjustend   = _DATES_AdjWeekEndFORWARD;
    }
    else if (idate7_start < idate7_end)
    {
	adjuststart = _DATES_AdjWeekEndFORWARD;
	adjustend = _DATES_AdjWeekEndBACK;
    }
    else return !DATES_IsWeekend (idate7_start);     /*  same date  */

    idate5_start = DATES_ConvertIDate7ToIDate5 (idate7_start, adjuststart);
    idate5_end   = DATES_ConvertIDate7ToIDate5 (idate7_end  , adjustend);
    return abs (idate5_end - idate5_start) + 1;
}     /*  count bdays in range  */



/*---------------------------------------------------------------------------
 * FUNCTION: to count the number of months, month ends, or month beg dates
 *           in the range
 * ARGUMENTS:
 *   startdate        initial date as a 7day date code
 *   enddate          final date as a 7day date code
 *   dateIncrement    one of the DATES_DateIncr_DATE form
 *
 * RETURNS:  number of months, mes, or mbs in range
 *
 * ASSUMPTIONS:
 *   1) start and enddate are the same
 *      -  if DATES_DateIncr_MONTH returns 1
 *      -  if DATES_DateIncr_MONTHBEGIN returns 1 only if date is a month begin
 *         date
 *      -  if DATES_DateIncr_MONTHEND returns 1 only if date is a month end
 *	   date
 *   2) startdate before enddate
 *      - compute number of full years (12 months per full year)
 *      - compute number of months left in start year (13 - start month)
 *      - compute number of months into end year (end month)
 *      - sum total months and return if DATES_DateIncr_MONTH
 *      - if DATES_DateIncr_MONTHBEGIN subtract 1 if startdate is not a month
 *        begin date
 *      - if DATES_DateIncr_MONTHEND subtract 1 if enddate is not a month end
 *        date
 *   3) startdate after enddate
 *      - reverse procedure from 2)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_CountMonthsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
)
{
    int year1, month1, day1, year2, month2, day2;
    int tempmonths, fullyears ;

    if (!DATES_ConvertIDate7ToYmd (startdate, &year1, &month1, &day1) ||
	!DATES_ConvertIDate7ToYmd (enddate,   &year2, &month2, &day2)
    ) return DATES_Bad7DayDate;

    if (enddate == startdate)
	return (dateIncrement == DATES_DateIncr_MONTH) 
          ?  1
          :  (dateIncrement == DATES_DateIncr_MONTHBEGIN)    
             ? startdate == DATES_FindMonthBeginDay (year1, month1)
             : enddate   == DATES_FindMonthEndDay   (year2, month2);

    if (enddate > startdate)
    {
	fullyears = year2 - year1 - 1;
	tempmonths = (13 - month1) + fullyears * 12 + month2;

	switch (dateIncrement)
	{
	case DATES_DateIncr_MONTH:
	    return tempmonths;
	case DATES_DateIncr_MONTHBEGIN:
	    return tempmonths - 
		   (startdate != DATES_FindMonthBeginDay (year1, month1));
	case DATES_DateIncr_MONTHEND:
	    return tempmonths - 
                   (enddate != DATES_FindMonthEndDay (year2, month2));
        }
    }   /*  enddate > startdate  */

/*  enddate < startdate  */
    fullyears = year1 - year2 - 1;
    tempmonths = (13 - month2) + fullyears * 12 + month1;
   
    switch (dateIncrement)
    {
    case DATES_DateIncr_MONTH:
	return tempmonths;
    case DATES_DateIncr_MONTHBEGIN:
	return tempmonths - (enddate != DATES_FindMonthBeginDay (year2, month2));
    case DATES_DateIncr_MONTHEND:
	return tempmonths - (startdate != DATES_FindMonthEndDay (year1, month1));
    }

    return DATES_BadDateIncrement;
}     /*  count months in range  */


/*---------------------------------------------------------------------------
 * FUNCTION: to count the number of quarters, quart ends, or quart beg dates
 *           in the range
 * ARGUMENTS:
 *   startdate        initial date as a 7day date code
 *   enddate          final date as a 7day date code
 *   dateIncrement    one of the DATES_DateIncr_DATE form
 *
 * RETURNS:  number of quarters, qes, or qbs in range
 *
 * ASSUMPTIONS:
 *   1) start and enddate are the same
 *      -  if DATES_DateIncr_QUARTER returns 1
 *      -  if DATES_DateIncr_QUARTERBEGIN returns 1 only if date is a quarter
 *	   begin date
 *      -  if DATES_DateIncr_QUARTEREND returns 1 only if date is a quarter end
 *	   date
 *   2) startdate before enddate
 *      - compute number of full years (4 quarters per full year)
 *      - compute number of quarters left in start year (5 - start quarter)
 *      - compute number of quarters into end year (end quarter)
 *      - sum total quarters and return if DATES_DateIncr_QUARTER
 *      - if DATES_DateIncr_QUARTERBEGIN subtract 1 if startdate is not a
 *        quarter begin date
 *      - if DATES_DateIncr_QUARTEREND subtract 1 if enddate is not a quarter
 *        end date
 *   3) startdate after enddate
 *      - reverse procedure from 2)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_CountQtrsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
)
{
    int year1, month1, day1, year2, month2, day2;
    int tempquarts, fullyears ;

    if (!DATES_ConvertIDate7ToYmd (
	    startdate, &year1, &month1, &day1
	) || !DATES_ConvertIDate7ToYmd (
	    enddate,   &year2, &month2, &day2
	)
    ) return DATES_Bad7DayDate;

    if (enddate == startdate)
	return dateIncrement == DATES_DateIncr_QUARTER
	    ? 1
	    : dateIncrement == DATES_DateIncr_QUARTERBEGIN
	    ? startdate == DATES_FindMonthBeginDay (
		year1, DATES_FindQtrBeginMonth (month1)
	    )
	    : enddate == DATES_FindMonthEndDay (
		year2, DATES_FindQtrEndMonth (month2) 
	    );

    if (enddate > startdate)
    {
	fullyears = year2 - year1 - 1;
	tempquarts = (5 - DATES_FindQuarterForMonth (month1)) +
		     fullyears * 4 +
		     DATES_FindQuarterForMonth (month2);

	switch (dateIncrement)
	{
	case DATES_DateIncr_QUARTER:
	    return tempquarts;
	case DATES_DateIncr_QUARTERBEGIN:
	    return tempquarts - (
		startdate != DATES_FindMonthBeginDay (year1, DATES_FindQtrBeginMonth(month1))
	    );
	case DATES_DateIncr_QUARTEREND:
	    return tempquarts - (
		enddate != DATES_FindMonthEndDay (year2, DATES_FindQtrEndMonth (month2))
	    );
	}

    }   /*  enddate > startdate  */

    /*  enddate < startdate  */
    fullyears = year1 - year2 - 1;
    tempquarts = (5 - DATES_FindQuarterForMonth (month2)) +
		     fullyears * 4 +
		     DATES_FindQuarterForMonth (month1);

    switch (dateIncrement)
    {
    case DATES_DateIncr_QUARTER:
	return tempquarts;
    case DATES_DateIncr_QUARTERBEGIN:
	return tempquarts - (
	    enddate != DATES_FindMonthBeginDay (year2, DATES_FindQtrBeginMonth(month2))
	);
    case DATES_DateIncr_QUARTEREND:
	return tempquarts - (
	    startdate != DATES_FindMonthEndDay (year1, DATES_FindQtrEndMonth (month1))
	);
    }
	 
    return DATES_BadDateIncrement;
}     /*  count quarters in range  */


/*---------------------------------------------------------------------------
 * FUNCTION: to count the number of years, year ends, or year beg dates
 *           in the range
 * ARGUMENTS:
 *   startdate        initial date as a 7day date code
 *   enddate          final date as a 7day date code
 *   dateIncrement    one of the DATES_DateIncr_DATE form
 *
 * RETURNS:  number of years, qes, or qbs in range
 *
 * ASSUMPTIONS:
 *   1) start and enddate are the same
 *      -  if DATES_DateIncr_YEAR returns 1
 *      -  if DATES_DateIncr_YEARBEGIN returns 1 only if date is a year begin
 *         date
 *      -  if DATES_DateIncr_YEAREND returns 1 only if date is a year end date
 *   2) startdate before enddate
 *      - years = year2 - year1 + 1
 *      - if DATES_DateIncr_YEARBEGIN subtract 1 if startdate is not a year
 *        begin date
 *      - if DATES_DateIncr_YEAREND subtract 1 if enddate is not a year end
 *        date
 *   3) startdate after enddate
 *      - reverse procedure from 2)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_CountYearsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
)
{
    int year1, month1, day1, year2, month2, day2;
    int years;

    if (!DATES_ConvertIDate7ToYmd (
	    startdate, &year1, &month1, &day1
	) || !DATES_ConvertIDate7ToYmd (
	    enddate,   &year2, &month2, &day2
	)
    ) return DATES_Bad7DayDate;

    if (enddate == startdate)
	return dateIncrement == DATES_DateIncr_YEAR
	    ? 1
	    : dateIncrement == DATES_DateIncr_YEARBEGIN
	    ? month1 == 1 && day1 == 1
	    : month2 == 12 && day2 == 31;

    years = abs(year2 - year1) + 1;

    if (enddate > startdate)
    {
	switch (dateIncrement)
	{
	case DATES_DateIncr_YEAR:
	    return years;
	case DATES_DateIncr_YEARBEGIN:
	    return years - (month1 != 1 || day1 != 1);
	case DATES_DateIncr_YEAREND:
	    return years - (month2 != 12 || day2 != 31);
	}
    }   /*  enddate > startdate  */

/*  enddate < startdate  */
   
    switch (dateIncrement)
    {
    case DATES_DateIncr_YEAR:
	return years;
    case DATES_DateIncr_YEARBEGIN:
	return years - (month2 != 1 || day2 != 1);
    case DATES_DateIncr_YEAREND:
	return years - (month1 != 12 || day1 != 31);
    }

    return DATES_BadDateIncrement;
}     /*  count years in range  */


/**********************
 * INCREMENT ROUTINES *
 **********************/

/*---------------------------------------------------------------------------
 * INCREMENT ROUTINES:  routines to find the next(last) date given
 *                      a date and an increment
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:   to increment idate7 by n increments at dateIncrement frequency
 *
 * ARGUMENTS
 *   idate7          7day date code
 *   n               number of times to increment by dateIncrement.  If n is
 *                   negative, the result is a decrement by n increments.
 *   dateIncrement   one of the valid DATES_DateIncr_INCR forms.
 *
 * RETURNS:    7day date code.
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_IncrementDate (
    int				idate7,
    int				n,
    int				dateIncrement
)
{
switch (dateIncrement)
   {
   case DATES_DateIncr_DAY:
      return DATES_IncrementDateByDay (idate7, n, dateIncrement);
   case DATES_DateIncr_BDAY:
      return DATES_IncrementDateByBDay (idate7, n, dateIncrement);
#if 0
   case DATES_DateIncr_WEEK:
   case DATES_DateIncr_WEEK1:
   case DATES_DateIncr_WEEK2:
   case DATES_DateIncr_WEEK3:
   case DATES_DateIncr_WEEK4:
   case DATES_DateIncr_WEEK5:
   case DATES_DateIncr_WEEK6:
   case DATES_DateIncr_WEEK7:
      return DATES_IncrementDateByWeek (idate7, n, dateIncrement);
#endif
   case DATES_DateIncr_MONTH:
   case DATES_DateIncr_MONTHBEGIN:
   case DATES_DateIncr_MONTHEND:
      return DATES_IncrementDateByMonth (idate7, n, dateIncrement);
   case DATES_DateIncr_QUARTER:
   case DATES_DateIncr_QUARTERBEGIN:
   case DATES_DateIncr_QUARTEREND:
      return DATES_IncrementDateByQtr (idate7, n, dateIncrement);
   case DATES_DateIncr_YEAR:
   case DATES_DateIncr_YEARBEGIN:
   case DATES_DateIncr_YEAREND:
      return DATES_IncrementDateByYear (idate7, n, dateIncrement);
      
   default:
      return DATES_BadDateIncrement;

   } /* of switch */

}     /*  increment date  */


/*======================================================================*/

/*---------------------------------------------------------------------------
 * FUNCTION: to increment the date supplied by n business days
 *
 * ARGUMENTS
 *   idate7	    7day date code
 *   n		    number of business days to increment by - if negative
 *		    decrement by n bdays
 *   dateIncrement  'DATES_DateIncr_BDay'
 * 
 * RETURNS:       7day date code
 *
 * ASSUMPTIONS:   if idate7 is a weekend date, then assumes Friday if n
 *                is positive and assumes Monday if n is negative
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_IncrementDateByBDay (
    int				idate7,
    int				n,
    int				Unused(dateIncrement)
)
{
    int tempdate = idate7;

if (DATES_IsWeekend (idate7))
   tempdate = (n < 0) ? DATES_ConvertIDate7ToWeekday
                                  (idate7, _DATES_AdjWeekEndFORWARD)
                      : DATES_ConvertIDate7ToWeekday (idate7, _DATES_AdjWeekEndBACK);

tempdate = DATES_ConvertIDate7ToIDate5 (tempdate, 0); 
return DATES_ConvertIDate5ToIDate7 (tempdate + n);

}     /*  increment date by bday  */


/*---------------------------------------------------------------------------
 * FUNCTION: to increment date by n months where months can be determined
 *           based on current day in month, month begin date,  or month end
 *           date.
 * ARGUMENTS
 *   idate7           7 day date code representing base date
 *   n                number of month increment to use,  if negative
 *                    decrements date by n months
 *   dateIncrement    one of the valid DATES_DateIncr_MONTH forms
 *
 * RETURNS:  7day date representing the incremented date code.  
 *
 * ASSUMPTIONS:
 *   -  if n = 0 returns the day, monthend or monthbeg date for current
 *      month based on dateIncrement value
 *   -  if n = 1 returns the day,  monthend or monthbeg date for the
 *      next month based on dateIncrement value
 *   -  if n = -1 returns the day,  monthend,  or monthbeg date for
 *      previous month based on dateIncrement value
 *
 * NOTE:  could add dateIncrements for MONTHBDAY,  MONTHENDBDAY,  MONTHBEGBDAY
 *        and day of week version (e.g.,  MONTHENDWEDNESDAY)
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_IncrementDateByMonth (
    int				idate7,
    int				n,
    int				dateIncrement
)
{
    int tempyear, tempmonth, year, month, day;

if (!DATES_ConvertIDate7ToYmd (idate7, &tempyear, &tempmonth, &day))
   return DATES_Bad7DayDate;

DATES_IncrementYmByN (tempyear, tempmonth, n, year, month);  

switch (dateIncrement)
   {
   case DATES_DateIncr_MONTH:
	 return DATES_ConvertYmdToIDate7
               (year, month, Min (day, DATES_ComputeDaysInMonth (year, month)) );
   case DATES_DateIncr_MONTHBEGIN:
         return DATES_FindMonthBeginDay (year, month);

   case DATES_DateIncr_MONTHEND:
         return DATES_FindMonthEndDay (year, month);

   default:
      return DATES_BadDateIncrement;
   
   }  /* of switch */

}     /*  increment date by month  */


/*---------------------------------------------------------------------------
 * FUNCTION: to increment date by n quarters where quarters can be one of the
 *           valid quarter increment forms.
 * ARGUMENTS
 *   idate7             7day date code representing base date
 *   n                  number of quarter incre/decrements to use
 *   dateIncrement      one of the valid DATES_DateIncr_QUARTER forms
 *
 * RETURNS:   7day date representing the incremented/decremented date code.
 *
 * ASSUMPTIONS:
 *    -  if n = 0 returns the day,  quarterend, or quarterbegin date for
 *       idate7
 *    -  if n = -1 returns the day, quarterend, or quarterbegin for the 
 *       previous quarter
 *    -  if n = 1 returns the day, quarterend, or quarterbegin for the
 *       next quarter
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_IncrementDateByQtr (
    int				idate7,
    int				n,
    int				dateIncrement
)
{
    int tempyear, tempmonth, year, month, day;

if (!DATES_ConvertIDate7ToYmd (idate7, &tempyear, &tempmonth, &day))
   return DATES_Bad7DayDate;

DATES_IncrementYmByN                   /*  each quarter = 3 months */
     (tempyear, tempmonth, 3*n, year, month);  

switch (dateIncrement)
   {
   case DATES_DateIncr_QUARTER:
	 return DATES_ConvertYmdToIDate7
               (year, month, Min (day, DATES_ComputeDaysInMonth (year, month)) );
   case DATES_DateIncr_QUARTERBEGIN:
         month = DATES_FindQtrBeginMonth (month);
         return DATES_FindMonthBeginDay (year, month);

   case DATES_DateIncr_QUARTEREND:
         month = DATES_FindQtrEndMonth (month);
         return DATES_FindMonthEndDay (year, month);

   default:
      return DATES_BadDateIncrement;
   
   }  /* of switch */

}    /*  increment date by quarter  */


/*---------------------------------------------------------------------------
 * FUNCTION: to increment date by n years where years can be determined using
 *           a valid DATES_DateIncr_YEAR form
 * 
 * ARGUMENTS:
 *   idate7             7day date code representing base date
 *   n                  number of year incre/decrements to use
 *   dateIncrement      one of the valid DATES_DateIncr_YEAR forms
 *
 * RETURNS:   7day date representing the incremented/decremented date code.
 *
 * ASSUMPTIONS:
 *    -  if n = 0 returns the day,  yearend, or yearbegin date for
 *       idate7
 *    -  if n = -1 returns the day, yearend, or yearbegin for the 
 *       previous year
 *    -  if n = 1 returns the day, yearend, or yearbegin for the
 *       next year
 *---------------------------------------------------------------------------
 */
PublicFnDef int DATES_IncrementDateByYear (
    int				idate7,
    int				n,
    int				dateIncrement
)
{
    int year, month, day;

if (!DATES_ConvertIDate7ToYmd (idate7, &year, &month, &day))
   return DATES_Bad7DayDate;

year += n;

switch (dateIncrement)
   {
   case DATES_DateIncr_YEAR:
	 return DATES_ConvertYmdToIDate7
               (year, month, Min (day, DATES_ComputeDaysInMonth (year, month)) );
   case DATES_DateIncr_YEARBEGIN:
         return DATES_ConvertYmdToIDate7 (year, 1, 1);

   case DATES_DateIncr_YEAREND:
         return DATES_ConvertYmdToIDate7 (year, 12, 31);

   default:
      return DATES_BadDateIncrement;
   
   }  /* of switch */

}     /*  increment date by year  */


/*******************************
 ***** ENUMERATE vectors    ****
 *******************************/

/*---------------------------------------------------------------------------
 * ENUMERATE:
 *
 *---------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------
 * FUNCTION:  to produce in dvector the list of 7day dates in the range
 *            specified using the increment specified by numberOfIncs and 
 *	      dateIncrementType
 *
 * ARGUMENTS:
 *   startdate		initial date in 7day date code form
 *   enddate		last date in 7day date code form
 *   numberOfIncs       number of increments at a time, if negative
 *			assumes decrements
 *   dateIncrementType	one of the valid DATES_DateIncr_ forms
 *   dvector		pointer to an integer array of 7day date codes
 *			which are identified in range
 *
 * RETURNS: The number of dates stored in dvector.  
 *	    If numberOfIncs=0 (infinite dates)
 *          or invalid dateIncrementType, an error exit occurs.
 *
 * ASSUMPTIONS:
 *   -  dvector is large enough to hold results
 *   -  numberOfIncs < 0 assumes decrements
 *   -  startdate will always be first element of dvector when done
 *      and is adjusted to be of the right DATES_DateIncr_ form
 *---------------------------------------------------------------------------
 */
PublicFnDef size_t DATES_EnumerateDatesInRange (
    int				startdate,
    int				enddate,
    int				numberOfIncs,
    int				dateIncrementType,
    int *			dvector,
    int				countOnly
) {
#ifdef __VMS
#pragma __message __save
#pragma __message __disable(BOOLEXPRCONST)
#endif

    size_t		count = 0;

/*****  
 *  Static variables to remember the last date range enumerated
 *  in order to save time and work.
 *****/
    static bool			lastdvectorValid = false;
    static size_t		lastcount = 0;
    static int			laststartdate,
				lastenddate,
				lastn,
				lastDateIncrementType, 
				lastdvector[256];
		/* Don't store the 'lastdvector' values if its 
		 * 'countOnly' and a 'nonDaily' date range because it 
		 * would cause an additional conversion to idate7 for 
		 * every element.
		 * The only case where saving the dvector for 'countOnly'
		 * calls is useful would be when the next call has the 
  		 * same range and is '!countOnly'.
		 * Based on how 'EnumerateDateRange' works, not causing 
		 * the extra conversion is the best approach.  Generally 
		 * it will call this function 2n times with the
		 * same range.  The first n times will be 'countOnly'
		 * the second n times will be '!countOnly'.  Out of 2n 
		 * times saving the devector will only help once and
		 * hinder us 2n-1 times.
		 */
  
				   
#define enumerateTheDailyDates(incrementFn) {\
    int date;\
\
    /*****  Always include adjusted start date as first element ... *****/\
    lastdvectorValid = true;\
    lastdvector[0] = date = incrementFn (startdate, 0, dateIncrementType);\
    if (!countOnly) dvector[0] = date;\
\
    /*****  Enumerate the rest ... *****/\
    date = incrementFn (date, numberOfIncs, dateIncrementType);\
    count = 1;\
\
    while ((numberOfIncs > 0 && date <= enddate) || (numberOfIncs < 0 && date >= enddate)) {\
	if (!countOnly) dvector[count] = date;\
	if (count < sizeof (lastdvector) / sizeof(int)) \
	    lastdvector[count] = date;\
        date = incrementFn (date, numberOfIncs, dateIncrementType);\
	count++;\
    }\
}

#define enumerateTheNonDailyDates(incrementFn) {\
/*****\
 *  Note:  The idea here is to ommit as many conversions between\
 *         IDate7 and Ymd as possible.\
 *****/\
\
    int year, month, day, standardEnddate, idate7;\
\
    /*****  Convert the enddate to standard form ... *****/\
    DATES_ConvertIDate7ToYmd (enddate, &year, &month, &day);\
    standardEnddate = DATES_ConvertYmdToStandard (year, month, day);\
\
    /*****  Convert the startdate to Ymd form ... *****/\
    DATES_ConvertIDate7ToYmd (startdate, &year, &month, &day);\
\
    /*****  Always include adjusted start date as first element ... *****/\
    incrementFn (year, month, day, 0, dateIncrementType);\
    if (!countOnly) {\
	dvector[0] = lastdvector[0] = DATES_ConvertYmdToIDate7 (year, month, day);\
	lastdvectorValid = true;\
    }\
\
    /*****  Enumerate the rest ... *****/\
    incrementFn (year, month, day, numberOfIncs, dateIncrementType);\
    count = 1;\
\
    while (\
	numberOfIncs > 0 && DATES_ConvertYmdToStandard (year,month,day) <= standardEnddate ||\
	numberOfIncs < 0 && DATES_ConvertYmdToStandard (year,month,day) >= standardEnddate\
    ) {\
	if (!countOnly)  {\
	    idate7 = DATES_ConvertYmdToIDate7 (year, month, day);\
	    dvector[count] = idate7;\
	    if (count < sizeof (lastdvector) / sizeof(int)) \
		lastdvector[count] = idate7;\
	}\
        incrementFn (year, month, day, numberOfIncs, dateIncrementType);\
	count++;\
    }\
}


/*---------------------------------------------------------------------------
 * Code Body of 'DATES_EnumerateDatesInRange'
 *---------------------------------------------------------------------------
 */


/*****  Check for illegal increment multiple ... *****/
    if (numberOfIncs == 0)
    {
	DEBUG_TraceTestdeck ("dateRange (1B1)");
	/*return DATES_BadDateIncrementMultiple; */
    }
   
/*****  If this range is identical to the last one, simply copy it *****/
    if (startdate == laststartdate				&&
	enddate == lastenddate					&&
	numberOfIncs == lastn					&&
	dateIncrementType == lastDateIncrementType		&&
	(lastdvectorValid || countOnly)				&&  /* vector must exist if !countonly */
	lastcount <= sizeof (lastdvector) / sizeof(int)
    ) {
	DEBUG_TraceTestdeck ("dateRange (1B2)");
	if (!countOnly)
	    memcpy (dvector, lastdvector, lastcount * sizeof(int));
	return lastcount;
    }

/*****  Remember this range ... *****/
    lastcount			= sizeof (lastdvector) / sizeof(int);
    laststartdate		= startdate;
    lastenddate			= enddate;
    lastn			= numberOfIncs;
    lastDateIncrementType	= dateIncrementType;
    lastdvectorValid		= false;

/*****  Check for backwards ranges ... *****/
    numberOfIncs = abs (numberOfIncs);
    if (startdate > enddate) {
	DEBUG_TraceTestdeck ("dateRange (1B3)");
	numberOfIncs = -numberOfIncs;
    }

/*****  Enumerate the dates ... *****/
    DEBUG_TraceTestdeck ("dateRange (1B4)");
    switch (dateIncrementType) {
    case DATES_DateIncr_DAY:
	enumerateTheDailyDates (DATES_IncrementDateByDay);
	break;
    case DATES_DateIncr_BDAY:
	enumerateTheDailyDates (DATES_IncrementDateByBDay);
	break;
    case DATES_DateIncr_WEEK:
    case DATES_DateIncr_WEEK1:
    case DATES_DateIncr_WEEK2:
    case DATES_DateIncr_WEEK3:
    case DATES_DateIncr_WEEK4:
    case DATES_DateIncr_WEEK5:
    case DATES_DateIncr_WEEK6:
    case DATES_DateIncr_WEEK7:
	enumerateTheNonDailyDates (DATES_IncrementYmdByWeek);
	break;
    case DATES_DateIncr_MONTH:
    case DATES_DateIncr_MONTHBEGIN:
    case DATES_DateIncr_MONTHEND:
	enumerateTheNonDailyDates (DATES_IncrementYmdByMonth);
	break;
    case DATES_DateIncr_QUARTER:
    case DATES_DateIncr_QUARTERBEGIN:
    case DATES_DateIncr_QUARTEREND:
	enumerateTheNonDailyDates (DATES_IncrementYmdByQtr);
	break;
    case DATES_DateIncr_YEAR:
    case DATES_DateIncr_YEARBEGIN:
    case DATES_DateIncr_YEAREND:
	enumerateTheNonDailyDates (DATES_IncrementYmdByYear);
	break;
    default:
	return 0; /* DATES_BadDateIncrement */
    } /* of switch */

    lastcount = count;
    return count;

#undef enumerateTheDailyDates
#undef enumerateTheNonDailyDates

#ifdef __VMS
#pragma __message __restore
#endif
}   /*  enumerate dates in range   */


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"			, FAC_DisplayFacilityIObject)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (DATES);
    FAC_FDFCase_FacilityDescription ("Date Utilities");
    FAC_FDFCase_FacilityMD (methodDictionary);
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dates.c 1 replace /users/jad/system
  860804 18:23:36 jad new dates module

 ************************************************************************/
/************************************************************************
  dates.c 2 replace /users/mjc/system
  860807 18:05:11 mjc Release basic time operations

 ************************************************************************/
/************************************************************************
  dates.c 3 replace /users/mjc/system
  860811 13:49:37 mjc Completed initial implementation of date offsets

 ************************************************************************/
/************************************************************************
  dates.c 4 replace /users/mjc/system
  870519 15:27:15 mjc Removed asymmetric conversion of <y m d> to standard dates

 ************************************************************************/
/************************************************************************
  dates.c 5 replace /users/mjc/translation
  870524 09:36:46 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  dates.c 6 replace /users/jad/system
  871215 13:58:10 jad modify the DateRangeEvaluate routine

 ************************************************************************/
/************************************************************************
  dates.c 7 replace /users/jad/system
  880107 16:25:10 jad turned many of the date fuctions 
into macros.  Modified 'EnumerateDatesInRange' to make it faster.

 ************************************************************************/
/************************************************************************
  dates.c 8 replace /users/jad/system/real
  880111 17:34:40 jad modified 'EnumerateDatesInRange' to remember the last range evaluated

 ************************************************************************/
/************************************************************************
  dates.c 9 replace /users/jad/system/real
  880112 18:36:13 jad fixed bug in EnumerateDatesInRange

 ************************************************************************/
/************************************************************************
  dates.c 10 replace /users/jad/system
  880321 11:28:57 jad added testdeck comments (no source changes)

 ************************************************************************/
/************************************************************************
  dates.c 11 replace /users/jad/system
  880321 16:24:03 jad implement testdeck tracing

 ************************************************************************/
/************************************************************************
  dates.c 12 replace /users/jck/system
  880324 15:34:52 jck Corrected declaration of _DATES_DayVector

 ************************************************************************/
/************************************************************************
  dates.c 13 replace /users/m2/backend
  890503 15:01:49 m2 nested comments

 ************************************************************************/
/************************************************************************
  dates.c 14 replace /users/m2/backend
  890828 17:00:46 m2 Added two PublicVarDefs

 ************************************************************************/

/*****  Date Utilities Facility Header File  *****/
#ifndef DATES_H
#define DATES_H

/*************************************
 *****  Supporting Declarations  *****
 *************************************/

/*******************************
 *****  Date Format Types  *****
 *******************************/

typedef enum {
    DATES_DateformSTANDARD, 
    DATES_Dateform7DAY, 
    DATES_Dateform5DAY, 
    DATES_DateformJDATE, 
    DATES_DateformJULIAN, 
    DATES_DateformYMD
} DATES_Dateform;


/**********************************
 *****  Date Increment Types  *****
 **********************************/

#define DATES_DateIncr_DAY 1
#define DATES_DateIncr_BDAY 2
#define DATES_DateIncr_WEEK 10
#define DATES_DateIncr_WEEK1 11
#define DATES_DateIncr_WEEK2 12
#define DATES_DateIncr_WEEK3 13
#define DATES_DateIncr_WEEK4 14
#define DATES_DateIncr_WEEK5 15
#define DATES_DateIncr_WEEK6 16
#define DATES_DateIncr_WEEK7 17
#define DATES_DateIncr_MONTH 20
#define DATES_DateIncr_MONTHBEGIN 21
#define DATES_DateIncr_MONTHEND 22
#define DATES_DateIncr_QUARTER 30
#define DATES_DateIncr_QUARTERBEGIN 31
#define DATES_DateIncr_QUARTEREND 32
#define DATES_DateIncr_YEAR 40
#define DATES_DateIncr_YEARBEGIN 41
#define DATES_DateIncr_YEAREND 42


/******************************
 *****  Date Error Codes  *****
 ******************************/

#define DATES_BadDate			-1
#define DATES_BadDateForm		-2
#define DATES_BadStandardDate		-3
#define DATES_Bad7DayDate		-4
#define DATES_BadJulianDate		-5
#define DATES_BadDateIncrement		-6
#define DATES_BadDateIncrementMultiple	-7

   
/*************************
 *****  Date Macros  *****
 *************************/

/*******************
  *** IS MACROS ***
 *******************/

/*---------------------------------------------------------------------------
 * IS MACROS:         basic date query routines that return
 *                      boolean values
 *
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:    to determine if year is a leapyear.
 *
 * ARGUMENTS:
 *   year	- an integer containing a valid year
 *
 * RETURNS:     boolean value
 *---------------------------------------------------------------------------
 */
#define DATES_IsLeapYear(year)\
    ( (year % 4 == 0)  &&\
         (year % 400 == 0 || year % 100 != 0) )

/*---------------------------------------------------------------------------
 * FUNCTION:    to determine if 7day date represents a Saturday or Sunday.
 *
 * ARGUMENTS:
 *   idate7     a 7day date code (integer)
 *
 * RETURNS:     boolean value
 *---------------------------------------------------------------------------
 */
#define DATES_IsWeekend(idate7)\
    (DATES_FindDayOfWeek (idate7, DATES_Dateform7DAY) > 5)

/*---------------------------------------------------------------------------
 * FUNCTION:  to determine if date is in the date range start .. end
 *
 * ARGUMENTS: (all integers)
 *   date           7day date code
 *   idate7_start   7day date code
 *   idate7_end     7day date code >= idate7_start
 *
 * RETURNS:  a boolean value
 *---------------------------------------------------------------------------
 */
#define	DATES_IsDateInRange(date, idate7_start, idate7_end)\
    ((date >= idate7_start) && (date <= idate7_end))


/***************************
  *** VALIDATION MACROS ***
 ***************************/

/*---------------------------------------------------------------------------
 * VALIDATION MACROS: basic date validation routines that return
 *                    boolean values
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:     Determines if year, month, date combination form a
 *               valid gregorian date.
 * ARGUMENTS: (all integers)
 *   year        valid integer year (> 0)
 *   month       valid integer month (1 ... 12)
 *   day         valid integer day (1 ... 31 depending on month)
 *
 * RETURNS:      boolean value
 *
 *---------------------------------------------------------------------------
 */
#define	DATES_ValidYmd(year, month, day)\
    (year > 0 && month > 0 && month <= 12 &&\
        day > 0 && day <= DATES_ComputeDaysInMonth (year, month))

/*---------------------------------------------------------------------------
 *  FUNCTION: Returns true if jday is a valid julian day for year
 *
 *  ARGUMENTS: (all integers)
 *     year    integer year
 *     jday    integer julian day (1 .. 366)
 *
 *  RETURNS:   boolean value
 *
 *---------------------------------------------------------------------------
 */
#define DATES_ValidJulianDay(year, jday)\
    (year > 0 && jday > 0 && jday <= DATES_ComputeDaysInYear (year))


/*********************
 ***  COMPUTE      ***
 *********************/

/*---------------------------------------------------------------------------
 * FUNCTION: to compute the number of days in month
 *
 * ARGUMENTS: (all integers)
 *   year     a valid year
 *   month    a valid month
 *
 * RETURNS:   an integer representing the number of days in the month
 *            for the year specified
 *
 * ASSUMES:   valid month provided
 *
 *---------------------------------------------------------------------------
 */
#define DATES_ComputeDaysInMonth(year, month)\
    (_DATES_DayVector[month-1] + (month == 2 ? DATES_IsLeapYear(year) : 0))

/*---------------------------------------------------------------------------
 * FUNCTION: computes the number of days in year 
 *
 * ARGUMENTS
 *   year     integer year
 *
 * RETURNS: integer number of days
 *
 *---------------------------------------------------------------------------
 */
#define DATES_ComputeDaysInYear(year)\
    (DATES_IsLeapYear (year) ? 366 : 365)

/*---------------------------------------------------------------------------
 * FUNCTION: to compute the cumulative days in the year through the 
 *           month specified (i.e., month = 1 implies 31 days to date
 *
 * ARGUMENTS:
 *   year    integer year
 *   month   integer month (0 - 12 where 0 represents begining of year 
 *           with 0 days to day; 1 represents January with 31 days to
 *           date and 12 represents December with 365 or 366 days to
 *           date.
 *
 * RETURNS:  integer value of cumulative days to date
 *
 * ASSUMPTIONS: valid month (0 ... 12) provided
 *
 *---------------------------------------------------------------------------
 */
#define DATES_ComputeCumDaysToMonth(year, month)\
    (DATES_IsLeapYear (year) ? _DATES_CumDays_LeapYear[month]\
                             : _DATES_CumDays[month])


/***************************
  *** CONVERSION MACROS ***
 ***************************/

/*---------------------------------------------------------------------------
 * CONVERSION MACROS:   routines to convert from one date form
 *                      to another.
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION: to convert a ymd date to a julian day for month, day in year
 *
 * ARGUMENTS:
 *   year    integer year
 *   month   integer month
 *   day     integer day
 *
 * RETURNS:  integer jday
 *
 * ASSUMES:  DATES_ValidYmd(year, month, day)
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertYmdToJulian(year, month, day)\
    (day + _DATES_CumDays[month-1] + (month > 2 ? DATES_IsLeapYear(year) : 0))

/*---------------------------------------------------------------------------
 * FUNCTION:   to convert a standard date code given the ymd parts
 *
 * ARGUMENTS:
 *    year    integer year
 *    month   integer month
 *    day     integer day
 *
 * RETURNS:   an integer representing a standard date (YYYYMMDD)
 *
 * ASSUMPTIONS: 
 *   -  DATES_ValidYmd (year, month, day)
 *   -  if year < 100 assumes 1900 + year      (removed 4/16/87)
 *
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertYmdToStandard(year, month, day)\
    (year * 10000 + month * 100 + day)

/*---------------------------------------------------------------------------
 * FUNCTION: to return a 7 day date code from a 5 day date code
 *
 * ARGUMENT
 *   idate5   integer containing a valid internal 5day date code
 *
 * RETURNS:  internal 7day date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertIDate5ToIDate7(idate5)\
    (((idate5 - 1)/5 * 7 + 1)	/* week component + offset */\
     + ((idate5 - 1) % 5))

/*---------------------------------------------------------------------------
 * FUNCTION:  to convert an internal 5day date code to an internal 
 *            standard date code.
 * ARGUMENT
 *   idate5   integer containing a valid internal 5day date code
 *
 * RETURNS:   valid standard date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertIDate5ToStandard(idate5)\
    DATES_ConvertIDate7ToStandard (DATES_ConvertIDate5ToIDate7 (idate5))

/*---------------------------------------------------------------------------
 * FUNCTION:  to convert an internal standard date code to an internal
 *            5day date code
 * ARGUMENT:
 *   st_date  integer containing a valid internal standard date code
 *
 * RETURNS:   internal 5day date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertStandardToIDate5(st_date)\
    DATES_ConvertIDate7ToIDate5 \
         (DATES_ConvertStandardToIDate7 (st_date), _DATES_AdjWeekEndBACK)

/*---------------------------------------------------------------------------
 * FUNCTION:     to convert a standard julian date to 5day date code
 * 
 * ARGUMENTS:
 *   jdate       integer containing a valid internal julian date code
 *
 * RETURNS:      internal 5 day date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertJDateToIDate5(jdate)\
    DATES_ConvertIDate7ToIDate5\
	(DATES_ConvertJDateToIDate7 (jdate), _DATES_AdjWeekEndBACK)

/*---------------------------------------------------------------------------
 * FUNCTION:    to convert an inernal 5day date code to a standard
 *              julian date code
 * ARGUMENTS:
 *   idate5     integer containing a valid internal 5day date code
 *
 * RETURNS:     standard julian internal date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertIDate5ToJDate(idate5)\
    DATES_ConvertIDate7ToJDate (DATES_ConvertIDate5ToIDate7 (idate5))

/*---------------------------------------------------------------------------
 * FUNCTION:   to convert a standard julian internal date code to a 
 *             standard internal date code
 * ARGUMENT:
 *   jdate     integer containing a valid standard julian date code
 *
 * RETURNS:    standard internal date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertJDateToStandard(jdate)\
    DATES_ConvertIDate7ToStandard (DATES_ConvertJDateToIDate7 (jdate))

/*---------------------------------------------------------------------------
 * FUNCTION:     to convert a standard internal date code to a standard
 *               julian inernal date code
 * ARGUMENT:
 *   st_date     integer containing a valid internal standard date code
 *
 * RETURNS:      standard date code
 *---------------------------------------------------------------------------
 */
#define DATES_ConvertStandardToJDate(st_date)\
    DATES_ConvertIDate7ToJDate (DATES_ConvertStandardToIDate7 (st_date))


/*******************
 *** FIND MACROS ***
 *******************/

/*---------------------------------------------------------------------------
 * FIND MACROS:   basic date routines to find date specific properties
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION: to find the quarter that the month is in
 *
 * ARGUMENTS: 
 *   month    integer containing a valid month
 *
 * RETURNS:  an integer containing the quarter (1-4)
 *---------------------------------------------------------------------------
 */
#define DATES_FindQuarterForMonth(month) ((month)/4 + 1)

/*---------------------------------------------------------------------------
 * FUNCTION: to find which month in the quarter that the month is in
 *
 * ARGUMENTS: 
 *   month    integer containing a valid month
 *
 * RETURNS:  an integer containing the month (1-3)
 *---------------------------------------------------------------------------
 */
#define DATES_FindMonthInQuarter(month)  ( ( (month)-1) % 3 + 1)

/*---------------------------------------------------------------------------
 * FUNCTION: to find the month end date for the given year/month inputs
 *
 * ARGUMENTS: (all integers)
 *   year       valid year
 *   month      valid month
 *
 * RETURNS:  the month end date as a 7day date code
 *---------------------------------------------------------------------------
 */
#define DATES_FindMonthEndDay(year, month)\
    DATES_ConvertYmdToIDate7 \
	(year, month, DATES_ComputeDaysInMonth(year, month))

/*---------------------------------------------------------------------------
 * FUNCTION: to find the month begin date for the given year/month inputs
 *
 * ARGUMENTS: (all integers)
 *   year     valid year
 *   month    valid month
 *
 * RETURNS:  the month begin date as a 7day date code 
 *---------------------------------------------------------------------------
 */
#define DATES_FindMonthBeginDay(year, month)\
    DATES_ConvertYmdToIDate7 (year, month, 1)

/*---------------------------------------------------------------------------
 * FUNCTION: to find the last weekday in the month for the given inputs
 * 
 * ARGUMENTS: (all integers)
 *   year    valid year
 *   month   valid month
 *
 * RETURNS:  the 7 day date code of the last weekday in month    
 *---------------------------------------------------------------------------
 */
#define DATES_FindMonthEndWeekday(year, month)\
    (DATES_ConvertIDate7ToWeekday\
	(DATES_FindMonthEndDay (year, month), _DATES_AdjWeekEndBACK))

/*---------------------------------------------------------------------------
 * FUNCTION: to find the first weekday in the month for the given inputs
 *
 * ARGUMENTS: (all integers)
 *   year     valid year
 *   month    valid month
 *
 * RETURNS:  the  7 day date code of the first business day in the month 
*---------------------------------------------------------------------------
*/
#define DATES_FindMonthBeginWeekday(year, month)\
    (DATES_ConvertIDate7ToWeekday\
	(DATES_FindMonthBeginDay (year, month), _DATES_AdjWeekEndFORWARD))

/*---------------------------------------------------------------------------
 * FUNCTION:   to  return the month representing the quarter end
 *             for the supplied month
 * ARGUMENTS:
 *   month     integer month
 *
 * RETURNS:    integer month
 *---------------------------------------------------------------------------
 */
#define DATES_FindQtrEndMonth(month)\
    (month + 3 - DATES_FindMonthInQuarter (month))

/*---------------------------------------------------------------------------
 * FUNCTION:   to  return the month representing the quarter beginning
 *             for the supplied month
 * ARGUMENTS:
 *   month     integer month
 *
 * RETURNS:    integer month
 *---------------------------------------------------------------------------
 */
#define DATES_FindQtrBeginMonth(month)\
    (month + 1 - DATES_FindMonthInQuarter (month))


/********************
 * INCREMENT MACROS *
 ********************/

/*---------------------------------------------------------------------------
 * INCREMENT MACROS:  routines to find the next(last) date given
 *                    a date and an increment
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 * FUNCTION:  to increment a 7day date by n days
 *
 * ARGUMENTS: (all integers)
 *   idate7	    7day date to be incremented
 *   n		    number of days to increment day by.  if n i negative
 *		    the result is a decrement of n days
 *   dateIncrement  'DATES_DateIncr_Day'
 *
 * RETURNS:       incremented(decremented) 7day date code
 *---------------------------------------------------------------------------
 */
#define DATES_IncrementDateByDay(idate7, n, dateIncrement)\
    ((idate7) + (n))


/*---------------------------------------------------------------------------
 * FUNCTION:     to determine the month n months after (before) the
 *               current month.  Year is adjusted if needed.
 * ARGUMENTS: (all integers)
 *   year1       initial year
 *   month1      initial month
 *   n           number of months to add (subtract)
 *   year2       modified year
 *   month2      modified month
 *
 * RETURNS:      modified year and month in 'year2' and 'month2'
 *
 * APPROACH:     determine the number of 12 month increments in n.
 *               Use this number to determine how many years and
 *               months can be added directly.
 *---------------------------------------------------------------------------
 */
#define DATES_IncrementYmByN(year1, month1, n, year2, month2)\
{\
    int yearchunks, monthsleft, increments;\
\
    increments = abs (n);\
    yearchunks = increments/12;\
    monthsleft = increments - yearchunks * 12;\
\
    if ((n) < 0)       /*  subtract months  */\
    {\
	year2 = (year1) - yearchunks;    \
	month2 = (month1) - monthsleft;\
   \
	if (month2 < 1)     /* adjust for year end boundary  */\
	{\
	    year2 -= 1;\
	    month2 += 12;\
	}\
    }\
    else    /*  add months */\
    { \
	year2 = (year1) + yearchunks;      \
	month2 = (month1) + monthsleft;\
\
	if (month2 > 12)    /*  adjust for year end boundary  */\
	{\
	    year2 += 1;\
	    month2 -= 12;\
	}\
    }\
}


/*---------------------------------------------------------------------------
 * 
 *---------------------------------------------------------------------------
 */
#define DATES_IncrementYmdByWeek(year, month, day, n, dateIncrement)\
{\
    /******  TO BE INSERTED   *****/\
}


/*---------------------------------------------------------------------------
 * FUNCTION: to increment the date by n months where months can be determined
 *           based on current day in month, month begin date, or month end
 *           date.
 * ARGUMENTS: (all integers)
 *   year	      the date's year - will be modified
 *   month	      the date's month - will be modified
 *   day	      the date's day - will be modified
 *   n                number of month increment to use, if negative
 *                    decrements date by n months
 *   dateIncrement    one of the valid DATES_DateIncr_MONTH forms
 *
 * RETURNS:  incremented date in 'year', 'month', and 'day'.
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
#define DATES_IncrementYmdByMonth(year, month, day, n, dateIncrement)\
{\
    int tempyear = (year), tempmonth = (month), tempday;\
\
    DATES_IncrementYmByN (tempyear, tempmonth, n, year, month);  \
\
    switch (dateIncrement)\
    {\
	case DATES_DateIncr_MONTH:\
	    tempday = DATES_ComputeDaysInMonth (year, month);\
	    day = (day < tempday) ? day : tempday;  /* min (day, tempday) */\
	    break;\
	case DATES_DateIncr_MONTHBEGIN:\
	    day = 1;\
	    break;\
	case DATES_DateIncr_MONTHEND:\
	    day = DATES_ComputeDaysInMonth (year, month);\
	    break;\
    }\
}


/*---------------------------------------------------------------------------
 * FUNCTION: to increment date by n quarters where quarters can be one of the
 *           valid quarter increment forms.
 * ARGUMENTS: (all integers)
 *   year	      the date's year - will be modified
 *   month	      the date's month - will be modified
 *   day	      the date's day - will be modified
 *   n                number of quarter incre/decrements to use
 *   dateIncrement    one of the valid DATES_DateIncr_QUARTER forms
 *
 * RETURNS:   the incremented/decremented date in 'year', 'month', 'day'.
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
#define DATES_IncrementYmdByQtr(year, month, day, n, dateIncrement)\
{\
    int tempyear = (year), tempmonth = (month), tempday;\
\
    DATES_IncrementYmByN    /*  each quarter = 3 months */\
	(tempyear, tempmonth, 3*n, year, month);  \
\
    switch (dateIncrement)\
    {\
	case DATES_DateIncr_QUARTER:\
	    tempday = DATES_ComputeDaysInMonth (year, month);\
	    day = (day < tempday) ? day : tempday;  /* min (day,tempday) */\
	    break;\
	case DATES_DateIncr_QUARTERBEGIN:\
	    month = DATES_FindQtrBeginMonth (month);\
	    day = 1;\
	    break;\
	case DATES_DateIncr_QUARTEREND:\
	    month = DATES_FindQtrEndMonth (month);\
	    day = DATES_ComputeDaysInMonth (year, month);\
	    break;\
    }\
}


/*---------------------------------------------------------------------------
 * FUNCTION: to increment date by n years where years can be determined using
 *           a valid DATES_DateIncr_YEAR form
 * 
 * ARGUMENTS: (all integers)
 *   year	        the date's year - will be modified
 *   month		the date's month - will be modified
 *   day		the date's day - will be modified
 *   n                  number of year incre/decrements to use
 *   dateIncrement      one of the valid DATES_DateIncr_YEAR forms
 *
 * RETURNS:   the incremented/decremented date in 'year', 'month', 'day'.
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
#define DATES_IncrementYmdByYear(year, month, day, n, dateIncrement) {\
    int tempday;\
\
    year += n;\
\
    switch (dateIncrement) {\
	case DATES_DateIncr_YEAR:\
	    tempday = DATES_ComputeDaysInMonth (year, month);\
	    day = (day < tempday) ? day : tempday;  /* min (day,tempday) */\
	    break;\
	case DATES_DateIncr_YEARBEGIN:\
	    month = 1;\
	    day = 1;\
	    break;\
	case DATES_DateIncr_YEAREND:\
	    month = 12;\
	    day = 31;\
	    break;\
    }\
}


/*****  Global 'Private' Variables  *****/
PublicVarDecl int		_DATES_DayVector[],
				_DATES_CumDays[], 
				_DATES_CumDays_LeapYear[];

/*****  Date Functions  *****/
PublicFnDecl int DATES_ConvertJulianToYmd (
    int				year,
    int				jday,
    int *			month,
    int *			day
);

PublicFnDecl int DATES_ConvertStandardToYmd (
    int				st_date,
    int *			year,
    int *			month,
    int *			day
);

PublicFnDecl int DATES_ConvertJulianToJDate (
    int				year,
    int				jday
);

PublicFnDecl int DATES_ConvertJDateToJulian (
    int				jdate,
    int *			year,
    int *			jday
);

PublicFnDecl int DATES_ConvertYmdToIDate7 (
    int				year,
    int				month,
    int				day
);

PublicFnDecl int DATES_ConvertIDate7ToYmd (
    int				idate7,
    int *			year,
    int *			month,
    int *			day
);

PublicFnDecl int DATES_ConvertFromDateToDate (
    int				date,
    DATES_Dateform		fromDateform,
    DATES_Dateform		toDateform
);

PublicFnDecl int DATES_ConvertDateToIDate7 (
    int				date,
    DATES_Dateform		fromDateform
);

PublicFnDecl int DATES_ConvertDateToIDate5 (
    int				date,
    DATES_Dateform		fromDateform
);

PublicFnDecl int DATES_ConvertDateToStandard (
    int				date,
    DATES_Dateform		fromDateform
);

PublicFnDecl int DATES_ConvertDateToJDate (
    int				date,
    DATES_Dateform		fromDateform
);

PublicFnDecl int DATES_ConvertStandardToIDate7 (
    int				st_date
);

PublicFnDecl int DATES_ConvertIDate7ToStandard (
    int				idate7
);

PublicFnDecl int DATES_ConvertIDate7ToIDate5 (
    int				idate7,
    int				adjustweekend
);

PublicFnDecl int DATES_ConvertJDateToIDate7 (
    int				jdate
);

PublicFnDecl int DATES_ConvertIDate7ToJDate (
    int				idate7
);

PublicFnDecl int DATES_ConvertIDate7ToWeekday (
    int				idate7,
    int				AdjWeekEnd
);

PublicFnDecl int DATES_FindTodaysDate (
    int *			year,
    int *			month,
    int *			day
);

PublicFnDecl int DATES_FindJulianMonth (
    int				year,
    int				jday
);

PublicFnDecl int DATES_FindDayOfWeek (
    int				date,
    DATES_Dateform		dateform
);

PublicFnDecl void DATES_FormatDayOfWeek (
    int				idate7,
    char *			string
);

PublicFnDecl void DATES_FormatBDayOfWeek (
    int				idate5,
    char *			string
);

PublicFnDecl void DATES_FormatIDate7 (
    int				idate7,
    int				format,
    char *			string
);

PublicFnDecl int DATES_CountDatesInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
);

PublicFnDecl int DATES_CountDaysInRange (
    int				idate7_start,
    int				idate7_end
);

PublicFnDecl int DATES_CountBDaysInRange (
    int				idate7_start,
    int				idate7_end
);

PublicFnDecl int DATES_CountMonthsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
);

PublicFnDecl int DATES_CountQtrsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
);

PublicFnDecl int DATES_CountYearsInRange (
    int				startdate,
    int				enddate,
    int				dateIncrement
);

PublicFnDecl int DATES_IncrementDate (
    int				idate7,
    int				n,
    int				dateIncrement
);

PublicFnDecl int DATES_IncrementDateByBDay (
    int				idate7,
    int				n,
    int				dateIncrement
);

PublicFnDecl int DATES_IncrementDateByMonth (
    int				idate7,
    int				n,
    int				dateIncrement
);

PublicFnDecl int DATES_IncrementDateByQtr (
    int				idate7,
    int				n,
    int				dateIncrement
);

PublicFnDecl int DATES_IncrementDateByYear (
    int				idate7,
    int				n,
    int				dateIncrement
);

PublicFnDecl size_t DATES_EnumerateDatesInRange (
    int				startdate,
    int				enddate,
    int				numberOfIncs,
    int				dateIncrementType,
    int *			dvector,
    int				countOnly
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dates.h 1 replace /users/jad/system
  860804 18:28:03 jad new dates module

 ************************************************************************/
/************************************************************************
  dates.h 2 replace /users/mjc/translation
  870524 09:36:55 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  dates.h 3 replace /users/jad/system
  871215 13:58:15 jad modify the DateRangeEvaluate routine

 ************************************************************************/
/************************************************************************
  dates.h 4 replace /users/jad/system
  880107 16:26:25 jad turned many of the date fuctions 
into macros.  Modified 'EnumerateDatesInRange' to make it faster.

 ************************************************************************/

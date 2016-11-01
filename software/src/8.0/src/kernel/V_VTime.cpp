/*****  V_VTime Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "V_VTime.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"


/**********************
 **********************
 *****            *****
 *****  V::VTime  *****
 *****            *****
 **********************
 **********************/

/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

V::U64 V::VTime::encode (struct time const &rTime) {
    U64 iResult = static_cast<U64>(rTime.seconds);
    iResult *= OneMM;
    iResult += rTime.microseconds;
    return iResult;
}

V::U64 V::VTime::encodeNow () {
    struct time iTime;
    gettime (&iTime);
    return encode (iTime);
}

bool V::VTime::asString (VString &rString) const {
    struct tm iTimeParts;
    if (!localtime (iTimeParts))
	return false;

/****  using 'mm/dd/yyyy:hh:mm:ss.mmm' format  ****/
    rString.printf (
	"%02u/%02u/%04u:%02u:%02u:%02u.%03u",
	iTimeParts.tm_mon + 1,
	iTimeParts.tm_mday,
	iTimeParts.tm_year + 1900,
	iTimeParts.tm_hour,
	iTimeParts.tm_min,
	iTimeParts.tm_sec,
	static_cast<unsigned int>((m_iValue%OneMM)/OneM)
    );

    return true;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void V::VTime::asValue (struct time& rResult) const {
    rResult.seconds = static_cast<unsigned int>(m_iValue / OneMM);
    rResult.microseconds = static_cast<unsigned int>(m_iValue % OneMM);
}

void V::VTime::asValue (struct timespec& rResult) const {
#ifndef _WIN32
    rResult.tv_sec = static_cast<unsigned long>(m_iValue / OneMM);
    rResult.tv_nsec = static_cast<long>((m_iValue % OneMM) * OneM);
#endif
}

char *V::VTime::ctime (char *pResult) const {
    time_t iTime = operator time_t ();
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
    char iResult[32];
    if (!ctime_r (&iTime, static_cast<char*>(iResult)))
#pragma __pointer_size __restore
	return 0;
    strcpy (pResult, iResult);
    return pResult;
#else
    return ctime_r (&iTime, pResult);
#endif
}

struct tm *V::VTime::localtime (struct tm &rResult) const {
    time_t iTime = operator time_t ();
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
    struct tm iResult;
    if (!localtime_r (&iTime, static_cast<struct tm*>(&iResult)))
#pragma __pointer_size __restore
	return 0;
    rResult = iResult;
    return &rResult;
#else
    return localtime_r (&iTime, &rResult);
#endif
}

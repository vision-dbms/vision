#ifndef RSINT_D
#define RSINT_D

#define RS_MaxLine	    1023
#define RS_NameFile	    "CompanyName"
#define RS_NameLen	    40

#define YEAR		    (RS_Date->tm_year + 1900)
#define YY		    (RS_Date->tm_year)
#define MM		    (RS_Date->tm_mon + 1)
#define DD		    (RS_Date->tm_mday)

#endif

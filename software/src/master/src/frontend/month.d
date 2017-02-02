#ifndef MONTH_D
#define MONTH_D

typedef struct {
    char const *text;
    int day;
} MONTHTABLE;

PrivateVarDef MONTHTABLE Months[] = {
" January ",	31, 
" February ",	28, 
" March ",	31, 
" April ",	30, 
" May ",	31, 
" June ",	30, 
" July ",	31, 
" August ",	31, 
" September ",	30, 
" October ",	31, 
" November ",	30, 
" December ",	31, 
 NULL, 
};

#define MONTH_lastDay(n)    (Months[n -1].day)
#define MONTH_text(n)	    (Months[n -1].text)

#endif

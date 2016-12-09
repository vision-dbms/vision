/****************************************************************************
*****									*****
*****			report.d	    				*****
*****									*****
****************************************************************************/
#ifndef REPORT_D
#define REPORT_D

#define REP_ItemWidth	    20

typedef struct {
    char item[81];
    char heading[REP_ItemWidth];
    char width[REP_ItemWidth];
    char decimals[REP_ItemWidth];
    char total[REP_ItemWidth];
} REP_Field;

typedef struct {
    int fieldCount;
    int fieldsSize;
    REP_Field **fields;
} REP_Format;

#define REP_fieldCount(rep)	    (rep->fieldCount)
#define REP_fieldsSize(rep)	    (rep->fieldsSize)
#define REP_field(rep, n)	    (rep->fields[n])

#define REP_item(field)		    (field->item)
#define REP_heading(field)	    (field->heading)
#define REP_width(field)	    (field->width)
#define REP_decimals(field)	    (field->decimals)
#define REP_totalType(field)	    (field->total)

#define REP_createReport(rep)\
{\
    if (NULL == (rep = (REP_Format *) calloc(1, sizeof(REP_Format))))\
	ERR_fatal(" Error allocating space");\
    if (NULL ==\
    (rep->fields = (REP_Field **) calloc(10, sizeof(REP_Field *))))\
	ERR_fatal(" Error allocating space");\
    REP_fieldsSize(rep) = 10;\
}

#define REP_deleteReport(rep, i)\
{\
    for (i = 0; i < REP_fieldCount(rep); i++)\
        free(REP_field(rep, i));\
    free(rep->fields);\
    free(rep);\
}

#define REP_addField(rep, ptr)\
{\
    if (REP_fieldCount(rep) >= REP_fieldsSize(rep))\
    {\
        if (NULL == (ptr = realloc(rep->fields, REP_fieldsSize(rep) + 10)))\
	    ERR_fatal(" Error allocating space");\
	rep->fields = (REP_Field **)ptr;\
	REP_fieldsSize(rep) += 10;\
    }\
    if (NULL == (REP_field(rep, REP_fieldCount(rep)) =\
			    (REP_Field *)calloc(1, sizeof(REP_Field))))\
        ERR_fatal(" Error allocating space");\
    REP_fieldCount(rep)++;\
}


#endif

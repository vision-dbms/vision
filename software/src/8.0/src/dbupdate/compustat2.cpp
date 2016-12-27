/*******************************************
 ************	compustat.c	************
 *******************************************/
/* Takes as input the compustat tapes and  */
/* the indann350.lay or indann175.lay file.*/
/* Extracts the needed information (spec-  */
/* ified in the .lay file) from the tapes, */
/* and creates key, layout, and data files.*/
/* There is an output file generated for   */
/* each item of a different type and size. */
/*******************************************/

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "gopt.h"
#include "reader.h"
#include "readlayout.h"
#include "filelayout.h"
#include "sigHandler.h"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

/*****  Constants  *****/
#define SAME		0

#define FULL_COV	"13"	/* The FILE_CODE */
#define DUP_PRIM	"91"
#define DUP_TERT	"93"
#define DUP_OTC		"96"
#define DUP_SUP		"94"

#define MAX_REC_W	2

#define MIN_BUF_SIZ     (FILE_recLen(Layout) * RECORDS_PER_COMPANY)

/***************
    Globals
 ***************/

PrivateVarDef int INDANN, INDQTR,
		  SHORT_KEY, 
		  PDS_PER_REC,
		  RECORDS_PER_COMPANY,
		  CUSIP_0,
		  YEAR_0,
		  FILE_0,
		  DUP_0,
		  DATA_REP_SIZE,
		  FOOTNOTE_REP_SIZE,
		  UCODE_0, 
		  MAXRECS, 
		  NUM_TAPES, 
		  START_YEAR,		/* command line switch */
		  END_YEAR,		/* beginning and ending year */
		  START_QTR,		/* command line switch */
		  END_QTR,		/* beginning and ending quarter */
		  QTR_0,
		  PDA_REP_SIZE,
		  PDF_REP_SIZE,
		  REC_0,
		  DATA_0,
		  ENDDATA_0,
		  DATA2_0,
		  ENDDATA2_0,
		  FNOTE_0,     
		  ENDFNOTE_0,
		  FNOTE2_0,     
		  ENDFNOTE2_0,
		  PDA_0,
		  ENDPDA_0,
		  PDF_0,
		  ENDPDF_0,
		  REC_W,
		  QTR_W,
		  YEAR_W,
		  FILE_W,
		  CUSIP_W;


PrivateVarDef char PrevCusip[8];
PrivateVarDef char const *TAPE_NAME;
PrivateVarDef char const *LAYOUT_FILE;
PrivateVarDef char const *PARAMS_FILE;
PrivateVarDef char const *NULL_REC;
PrivateVarDef char UCODE_SCREEN;
      
PrivateVarDef int FirstTSItem = 0,       
		  RecordCount = 0,          
		  BkPtrCount = -1,     /* Back pointer count */
		  Start_Period,		/* user-specified period  */
		  End_Period,		/* range.		  */
		  Start_Record,
		  End_Record;

PrivateVarDef FILE_LAYOUT *Layout;


PrivateFnDef void ConvertToFloat(
    char *rptr, 
    OUTFILE *file, 
    ITEM_LAYOUT *item, 
    int pctr)
{
    double ftmp = dtb (
	&rptr[(ITEM_pos(item) + (ITEM_width(item) * pctr))], ITEM_len(item)
    ) / ITEM_decs(item);

    if (-.001 == ftmp)
    {
	 *OUT_floatPtr(file)++ =  LAY_FloatNA;
    }
    else
    {
	 *OUT_floatPtr(file)++ =  (float)ftmp;
    }
}

PrivateFnDef void ConvertToDouble(
    char *rptr, 
    OUTFILE *file, 
    ITEM_LAYOUT *item, 
    int pctr)
{
    double ftmp = dtb (
	&rptr[(ITEM_pos(item) + (ITEM_width(item) * pctr))], ITEM_len(item)
    ) / ITEM_decs(item);

    if (-.001 == ftmp)
    {
	 *OUT_doublePtr(file)++ =  LAY_DoubleNA;
    }
    else
    {
	 *OUT_doublePtr(file)++ =  ftmp;
    }
}



/*************************************************
 **********	Record Processing	**********
 *************************************************/

#define static_key()\
{\
    key = FILE_key(Layout, 0);\
    memcpy(KEY_bufPtr(key), &rptr[CUSIP_0], CUSIP_W);\
    KEY_bufPtr(key) += CUSIP_W;\
    *KEY_bufPtr(key)++ = '\n';\
    KEY_count(key)++;\
    bytes = KEY_bufPtr(key) - KEY_buffer(key);\
    if ((bytes + CUSIP_W + 1)\
		>= WRITE_BUF_SIZE)\
    {\
	if ( -1 == write(KEY_fd(key), KEY_buffer(key), bytes))\
	    READER_FatalError("Key File Write");\
	    KEY_bufPtr(key) = KEY_buffer(key);\
    }\
}

#define ts_key()\
{\
    key = FILE_key(Layout, 1);\
    KEY_count(key)++;\
    if (!SHORT_KEY)\
    {\
    memcpy(KEY_bufPtr(key), &rptr[CUSIP_0], CUSIP_W);\
    KEY_bufPtr(key) += CUSIP_W;\
    if (INDANN)\
    {\
	memcpy(KEY_bufPtr(key), &rptr[YEAR_0 + (pctr * YEAR_W)], YEAR_W);\
	KEY_bufPtr(key) += YEAR_W;\
	memcpy(KEY_bufPtr(key), "1231", 4);\
	KEY_bufPtr(key) += 4;\
	*KEY_bufPtr(key)++ = '\n';\
    }\
    else\
    {\
	memcpy(KEY_bufPtr(key), &rptr[YEAR_0 + (pctr * PDA_REP_SIZE)], YEAR_W);\
	KEY_bufPtr(key) += YEAR_W;\
	*KEY_bufPtr(key)++ = rptr[QTR_0 + (pctr * PDA_REP_SIZE)];\
	*KEY_bufPtr(key)++ = '\n';\
    }\
    if ((KEY_bufPtr(key) + CUSIP_W + YEAR_W + 5)\
			     >= (KEY_buffer(key) + WRITE_BUF_SIZE))\
    {\
	if ( -1 == write(KEY_fd(key), KEY_buffer(key), \
					KEY_bufPtr(key) - KEY_buffer(key)))\
	    READER_FatalError("Key File Write");\
	KEY_bufPtr(key) = KEY_buffer(key);\
    }\
    }\
}

#define static_items()\
{\
    for (i = 0; i < FirstTSItem; i++)\
    {\
	item = FILE_item(Layout, i);\
	file = ITEM_file(item);\
	switch(ITEM_type(item)) {\
	    case DECIMAL:\
		ConvertToFloat (rptr, file, item, 0);\
        	break;\
	    case DOUBLE:\
		ConvertToDouble (rptr, file, item, 0);\
        	break;\
	    case INTEGER:\
	        memcpy(string, &rptr[ITEM_pos(item)], ITEM_len(item));\
	        string[ITEM_len(item)] = '\0';\
	        *OUT_intPtr(file)++ = atoi(string);\
		break;\
	    default:\
		printf ("\n!!! Unknown Item Type (%x) for Item: %s ... treating as alpha\n",\
			ITEM_type(item), ITEM_code(item));\
		/* no break */\
	    case ALPHA:\
	    case CODE:\
	        memcpy(OUT_charPtr(file), \
		    &rptr[ITEM_pos(item)], ITEM_len(item));\
	        *(OUT_charPtr(file) + ITEM_len(item)) = '\0';\
	        OUT_charPtr(file) += ITEM_out(item);\
        	break;\
	} /* switch */\
    } /* for */\
}

#define ts_items()\
{\
    for (i = FirstTSItem; i < FILE_itemCount(Layout); i++)\
    {\
	item = FILE_item(Layout, i);\
	file = ITEM_file(item);\
	switch(ITEM_type(item)) {\
	    case DECIMAL:\
		ConvertToFloat (rptr, file, item, pctr);\
	    	break; \
	    case DOUBLE:\
		ConvertToDouble (rptr, file, item, pctr);\
	    	break; \
	    case INTEGER:\
	        memcpy(string, &rptr[(ITEM_pos(item)+(ITEM_width(item)*pctr))],\
		   	     ITEM_len(item));\
	        string[ITEM_len(item)] = '\0';\
	        *OUT_intPtr(file)++ = atoi(string);\
		break;\
	    case BACKPTR:\
	        *OUT_intPtr(file)++ = BkPtrCount;\
		break;\
	    default:\
		printf ("\n!!! Unknown Item Type (%x) for Item: %s ... treating as alpha\n",\
			ITEM_type(item), ITEM_code(item));\
		/* no break */\
	    case ALPHA:\
	    case CODE:\
	        memcpy(OUT_charPtr(file), \
		    &rptr[(ITEM_pos(item) + (ITEM_width(item) * pctr))],\
					         ITEM_len(item));	    \
	        *(OUT_charPtr(file) + ITEM_len(item)) = '\0';\
	        OUT_charPtr(file) += ITEM_out(item);\
		break;\
	} /* switch */\
    }  /* for */\
}


#define calc_totqtr(Year,Qtr) (Year-firstyear-1)*4 + (4-firstqtr+1) + (Qtr-1)

PrivateFnDef void CalcStartEndPeriods (
    char *			record
)
{
    int	
 	firstyear,
	firstqtr,
	totqtr;
    char 
        *fyear,
	*fqtr;

   fyear = (char*)malloc(YEAR_W+1);
   fqtr = (char*)malloc(QTR_W+1);
   memcpy(fyear, &record[YEAR_0], YEAR_W);
   fyear[YEAR_W] = '\0';
   firstyear = atoi(fyear);

   if (!INDANN)
   {
	memcpy(fqtr, &record[QTR_0], QTR_W);
        fqtr[QTR_W] = '\0';
	firstqtr = atoi(fqtr);
   }
   if (INDANN)
   {
	if (START_YEAR <= firstyear) 
	{
	   Start_Period = 0;
	   Start_Record = 1;
	}
	else
	{
	   Start_Period = (START_YEAR - firstyear) % PDS_PER_REC;
	   Start_Record = (START_YEAR - firstyear) / PDS_PER_REC + 1;
	} 
	if (END_YEAR <= -1) 
	{
	    End_Period = PDS_PER_REC;
	    End_Record = RECORDS_PER_COMPANY + 1;
	}
	else
	{
	   End_Period = (END_YEAR - firstyear) % PDS_PER_REC;
	   End_Record = (END_YEAR - firstyear) / PDS_PER_REC + 1;
	} 
    }  /* end annual calculations */
    else	/*quarterly*/
    {
	if (START_QTR > 4) START_QTR = 4;

	if ((START_YEAR <= -1 && START_QTR <= 0) || 
	    (START_YEAR != -1 && START_YEAR < firstyear) ||
	    (START_YEAR == firstyear && START_QTR < firstqtr))
	{
	    Start_Period = 0;
	    Start_Record = 1;
	}
	else if (START_QTR > 0 &&  START_YEAR <= -1)
	{
	    START_YEAR = firstyear;
	    if (START_QTR > firstqtr)
	    {
		totqtr = calc_totqtr (START_YEAR, START_QTR);
		Start_Period = totqtr % PDS_PER_REC;
	    }
	    else
		Start_Period = 0;
	    Start_Record = 1;
	}
	else if (START_QTR <= 0 &&  START_YEAR > -1)
	{
	    START_QTR = 1;
	    totqtr = calc_totqtr(START_YEAR, START_QTR);
	    Start_Period = totqtr % PDS_PER_REC;
	    Start_Record = totqtr / PDS_PER_REC + 1;
	}
	else if (START_QTR > 0 && START_YEAR > -1)  /* yr and qtr entered */
	{
	   totqtr = calc_totqtr(START_YEAR, START_QTR);
	   Start_Period = totqtr % PDS_PER_REC;
	   Start_Record = totqtr / PDS_PER_REC + 1;
	}

	if (END_QTR > 4) END_QTR = 4;

	if ((END_QTR <= 0 && END_YEAR <= -1) ||
	     END_YEAR > firstyear + 10 ||
	    (END_YEAR == firstyear + 10 && (firstqtr == 1 || 
					    END_QTR >= firstqtr)))
	{
	    End_Period = PDS_PER_REC;
	    End_Record = RECORDS_PER_COMPANY + 1;
	}
	else if (END_QTR > 0 && END_YEAR <= -1)
	{
	   END_YEAR = firstyear + 10;       	/* not entered - last */
	   if (firstqtr == 1)			/* yr used            */
	   	END_YEAR--;
	    totqtr = calc_totqtr(END_YEAR, END_QTR);
	    End_Period = totqtr % PDS_PER_REC;
	    End_Record = totqtr / PDS_PER_REC + 1;
	}
	if (END_QTR < 0 && END_YEAR > -1)
	{
	    END_QTR = 4;
	    totqtr = calc_totqtr(END_YEAR, END_QTR);
	    End_Period = totqtr % PDS_PER_REC;
	    End_Record = totqtr / PDS_PER_REC + 1;
	}
	if (END_QTR > -1 && END_YEAR > -1)
	{
	    totqtr = calc_totqtr(END_YEAR, END_QTR);
	    End_Period = totqtr % PDS_PER_REC;
	    End_Record = totqtr / PDS_PER_REC + 1;
	}
   } /* end quarterly calculations */
#ifdef DEBUG_CalcStartEnd
    if (!INDANN)
	printf("fqtr         = %s\n", fqtr);
    printf("fyear        = %s\n", fyear);
    if (!INDANN)
	printf("firstqtr     = %d\n", firstqtr);
    printf("firstyear    = %d\n", firstyear);
    printf("Start_Period = %d\n", Start_Period);
    printf("Start_Record = %d\n", Start_Record);
    printf("End_Period   = %d\n", End_Period);
    printf("End_Record   = %d\n\n", End_Record);
    READER_CleanupAndExit ();
#endif
    free(fqtr);
    free(fyear);
} /* end procedure */


/******************************************
 *  Consumer Process  (Convert and Save)  *
 ******************************************/

PrivateFnDef void repeatWidths(
    void
)
{
    int i;
    ITEM_LAYOUT *item;
    
    if (INDANN)
    {
	for (i = 0; i < FILE_itemCount(Layout); i++)
	{
	    item = FILE_item(Layout, i);

	    if (!FirstTSItem && ITEM_key(item) > 0)
		FirstTSItem = i;
	    
	    if ((ITEM_pos(item) >= FNOTE_0 && ITEM_pos(item) < ENDFNOTE_0) ||
	        (ITEM_pos(item) >= FNOTE2_0 && ITEM_pos(item) < ENDFNOTE2_0))
		ITEM_width(item) = FOOTNOTE_REP_SIZE;
	    else
	    if ((ITEM_pos(item) >= DATA_0 && ITEM_pos(item) < ENDDATA_0) ||
	        (ITEM_pos(item) >= DATA2_0 && ITEM_pos(item) < ENDDATA2_0))
		 ITEM_width(item) = DATA_REP_SIZE;
	    else
		ITEM_width(item) = ITEM_len(item);
	}
    }
    else
    {
	for (i = 0; i < FILE_itemCount(Layout); i++)
	{
	    item = FILE_item(Layout, i);

	    if (!FirstTSItem && ITEM_key(item) > 0)
		FirstTSItem = i;
	    
	    if (ITEM_pos(item) >= FNOTE_0 && ITEM_pos(item) < ENDFNOTE_0)
		ITEM_width(item) = FOOTNOTE_REP_SIZE;
	    else 
	    if (ITEM_pos(item) >= DATA_0 && ITEM_pos(item) < ENDDATA_0)
		ITEM_width(item) = DATA_REP_SIZE;
	    else 
	    if (ITEM_pos(item) >= PDA_0 && ITEM_pos(item) < ENDPDA_0)
		ITEM_width(item) = PDA_REP_SIZE;
	    else 
	    if (ITEM_pos(item) >= PDF_0 && ITEM_pos(item) < ENDPDF_0)
		ITEM_width(item) = PDF_REP_SIZE;
	    else
		ITEM_width(item) = ITEM_len(item);
	}
    }
}


PrivateFnDef void process_buffer (
    char *			buffer,
    int				count
)
{
    int	pctr,	       /* the period counter */
	i, 
	bytes,
	pd_start,	/* start value for period counter */
	pd_stop,	/* stop value for period counter  */
	curr_rec;
    char *rptr, *rlim, string[50], recbuf[MAX_REC_W+1];
    OUTFILE *file;    
    KEY_LAYOUT *key;
    ITEM_LAYOUT *item;
    
    for (rptr = buffer, rlim = buffer + count; rptr < rlim;
					rptr += FILE_recLen(Layout)) {
	RecordCount++;

	if (RecordCount == 1)
	    CalcStartEndPeriods(rptr);

	memcpy(recbuf, &rptr[REC_0], REC_W);
	recbuf[REC_W] = '\0';
	curr_rec = atoi(recbuf);
/*	printf("process_buffer: recbuf = |%s|, curr_rec = %d\n",recbuf,curr_rec);*/

	if (INDANN && curr_rec >= 5 && curr_rec <= 8)
	    continue;	/*** skip records 5 thru 8 ***/
         
	if (memcmp(&rptr[FILE_0], FULL_COV, FILE_W) == 0) {
	    if( memcmp(&rptr[DUP_0], DUP_PRIM, FILE_W) == 0
	    || memcmp(&rptr[DUP_0], DUP_TERT, FILE_W) == 0
	    || memcmp(&rptr[DUP_0], DUP_OTC, FILE_W) == 0
	    || memcmp(&rptr[DUP_0], DUP_SUP, FILE_W) == 0)
		continue;   /*** skip duplicate records ***/
	} 		    /* offsets are already in layout */
			   
	if (SAME == memcmp(rptr, NULL_REC, strlen (NULL_REC)))
	    READER_FatalError ("Unexpected NULL Record");
	    
	if (SAME != memcmp(PrevCusip, &rptr[CUSIP_0], CUSIP_W)){
		/*** Change of Company ***/
	    memcpy(PrevCusip, &rptr[CUSIP_0], CUSIP_W);
	    BkPtrCount++;
	    static_key();
	    flush_files(Layout);	/* in filelayout module */
	    static_items();
	}			

	if (curr_rec < Start_Record)	  	/* skip records not */
	    continue;				/* part of user     */
	if (curr_rec > End_Record)		/* specified range  */
	    continue;	
            
	pd_start = 0;
	pd_stop = PDS_PER_REC;
	if ( curr_rec == Start_Record )
	    pd_start = Start_Period;
	if ( curr_rec == End_Record )
	    pd_stop = End_Period + 1;

	for (pctr = pd_start; pctr < pd_stop; pctr++) {
      	    if ((INDANN && rptr[UCODE_0 + pctr] < UCODE_SCREEN) ||
      	        (!INDANN && rptr[UCODE_0 + (pctr * PDA_REP_SIZE)] < UCODE_SCREEN))
		continue;   /*** skip periods with update code < UCODE_SCREEN ***/

	    ts_key();			/* ts = time series */
	    flush_files(Layout);
	    ts_items();
	 } /* for pctr loop */

    } /* for rptr loop */

 } /* end procedure process_buffer */


PrivateFnDef void convert_and_save (
    void
)
{

    DATABUFFER *buffer;
    
    open_keys(Layout);				/* in filelayout module */

    while (buffer = READER_GetBufferToSave ())	/* in reader module */
    {
        if (FILE_ebcdic(Layout))
	    READER_ConvertToAscii (buffer->content, buffer->count);

	process_buffer((char *)buffer->content, buffer->count);
	
	READER_ReturnBufferToReader ();		/* in reader module */
    }

    printf("\nRecords Processed = %d\n", RecordCount);

    close_files(Layout);			/* in filelayout module */

    close_keys(Layout);				/* in filelayout module */

}


struct load_data {
    char const *name;
    int		pos;
    int		width;
};

PrivateVarDef struct load_data loaddata[] = {
	{"CUSIP", 4, 8},
	{"YEAR", 64, 2},
	{"QTR", 178, 1},
	{"FILE", 14, 2},
	{"DUP", 14, 2},
	{"UCODE", 87, 1},
	{"REC", 13, 1},
	{"FNOTE_ARRAY", 92, 350},
	{"DATA_ARRAY", 442, 7190},
	{"FNOTE2_ARRAY", 30620, 350},
	{"DATA2_ARRAY", 30970, 7190},
	{"PDA_ARRAY", 176, 256},
	{"PDF_ARRAY", 432, 64},
	{"PDS_PER_REC", 5, -1},
	{"RECS_PER_COMP", 8, -1},
	{"", -1, -1}
};

PrivateFnDef int open_params_file(char const* loadname) {
	FILE	*fp;
	char	buf[128], name[20];
	int	pos, width, i;

	if( (fp = fopen(loadname, "r")) == NULL )
		return(-1);
	while( fgets(buf,128,fp) != NULL )
	{
		if( buf[0] == '#' )
			continue;
		if( sscanf(buf,"%s %d %d",name,&pos,&width) == -1 )
			continue;
		i=0;
		while( *loaddata[i].name != '\0' )
		{
			if( strcmp(loaddata[i].name,name) == 0 )
			{
				loaddata[i].pos = pos;
				loaddata[i].width = width;
				break;
			}
			i++;
		}
		if( *loaddata[i].name == '\0' )
			printf("no match: |%s|\n",name);
	}
	fclose(fp);
	return(0);
}

PrivateFnDef int get_item_pos_width (
    char const *		item_name,
    int	 *			item_width
)
{
	int i;

	for (i = 0; *loaddata[i].name != '\0'; i++ )
	{
		if( strcmp(loaddata[i].name,item_name) == 0 )
		{
			*item_width = loaddata[i].width;
			return(loaddata[i].pos);
		}
	}
	*item_width = -1;
	return(-1);
}
      

/****************************
 *  Main Program Utilities  *
 ****************************/

GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription
        ("OPTION_L", 'l', NilOf (char *), "indann350.lay")
    GOPT_ValueOptionDescription
        ("OPTION_P", 'p', NilOf (char *), "params.load")
    GOPT_ValueOptionDescription
        ("OPTION_R", 'r', NilOf (char *), "999999999")
    GOPT_ValueOptionDescription
        ("OPTION_T", 't', "DBSourceName", "/dev/rmt")
    GOPT_ValueOptionDescription
        ("OPTION_N", 'n', NilOf (char *), "1")
    GOPT_ValueOptionDescription
        ("OPTION_NR", 'N', NilOf (char *), "000000000000")
    GOPT_ValueOptionDescription	    
        ("OPTION_S", 's', NilOf (char *), "-1")
    GOPT_ValueOptionDescription	    
        ("OPTION_E", 'e', NilOf (char *), "-1")
    GOPT_ValueOptionDescription	    
        ("OPTION_B", 'b', NilOf (char *), "-1")
    GOPT_ValueOptionDescription	    
        ("OPTION_F", 'f', NilOf (char *), "-1")
    GOPT_ValueOptionDescription	    
        ("OPTION_U", 'u', NilOf (char *), "x")
    GOPT_SwitchOptionDescription    
        ("OPTION_K", 'k', NilOf (char *))
    GOPT_SwitchOptionDescription    
        ("OPTION_A", 'a', NilOf (char *))
    GOPT_SwitchOptionDescription    
        ("OPTION_Q", 'q', NilOf (char *))
GOPT_EndOptionDescriptions;


/******************
 *  Main Program  *
 ******************/

int main (
    int				argc,
    char *			argv[]
)
{
    int dummywidth;
    char *nullRecPattern;

    GOPT_AcquireOptions (argc, argv);
    MAXRECS =   atoi( GOPT_GetValueOption ("OPTION_R"));
    NUM_TAPES = atoi( GOPT_GetValueOption ("OPTION_N"));
    TAPE_NAME =   GOPT_GetValueOption ("OPTION_T");
    LAYOUT_FILE = GOPT_GetValueOption ("OPTION_L");
    PARAMS_FILE = GOPT_GetValueOption ("OPTION_P");
    NULL_REC    = GOPT_GetValueOption ("OPTION_NR");
    START_QTR = atoi( GOPT_GetValueOption ("OPTION_B"));
    END_QTR =   atoi( GOPT_GetValueOption ("OPTION_F"));
    START_YEAR =atoi( GOPT_GetValueOption ("OPTION_S"));
    END_YEAR =  atoi( GOPT_GetValueOption ("OPTION_E"));
    UCODE_SCREEN =  *GOPT_GetValueOption ("OPTION_U");
    SHORT_KEY = GOPT_GetSwitchOption ("OPTION_K");
    INDANN = GOPT_GetSwitchOption ("OPTION_A");
    INDQTR = GOPT_GetSwitchOption ("OPTION_Q");
    GOPT_SeekExtraArgument (0, 0);

    if( (!INDANN && !INDQTR) || (INDANN && INDQTR) )
    {
       printf("Usage: %s -a [-l] [-n] [-t] [-r] [-s] [-e] [-b] [-f] [-u]\n",
	      argv [0]);
       printf("       %s -q [-l] [-n] [-t] [-r] [-s] [-e] [-b] [-f] [-u]\n",
	      argv [0]);
       printf("                (-a  or -q required )\n");
       exit(ErrorExitValue);
    }

    if( open_params_file(PARAMS_FILE) == -1 )
    {
	READER_FatalError ("Error Reading Parameters File");
    }
    
    PDS_PER_REC = get_item_pos_width("PDS_PER_REC",&dummywidth);
    RECORDS_PER_COMPANY = get_item_pos_width("RECS_PER_COMP",&dummywidth);

    CUSIP_0 = get_item_pos_width("CUSIP",&CUSIP_W);
    YEAR_0 = get_item_pos_width("YEAR",&YEAR_W);
    FILE_0 = get_item_pos_width("FILE",&FILE_W);
    DUP_0 = get_item_pos_width("DUP",&dummywidth);
    UCODE_0 = get_item_pos_width("UCODE",&dummywidth);
    REC_0 = get_item_pos_width("REC",&REC_W);
    FNOTE_0 = get_item_pos_width("FNOTE_ARRAY",&dummywidth);
    ENDFNOTE_0 = FNOTE_0 + dummywidth;
    DATA_0 = get_item_pos_width("DATA_ARRAY",&dummywidth);
    ENDDATA_0 = DATA_0 + dummywidth;

    DATA_REP_SIZE = (ENDDATA_0 - DATA_0) / PDS_PER_REC;
    FOOTNOTE_REP_SIZE = (ENDFNOTE_0 - FNOTE_0) / PDS_PER_REC;

    if (INDANN)
    {  	
	FNOTE2_0 = get_item_pos_width("FNOTE2_ARRAY",&dummywidth);
	ENDFNOTE2_0 = FNOTE2_0 + dummywidth;
	DATA2_0 = get_item_pos_width("DATA2_ARRAY",&dummywidth);
	ENDDATA2_0 = DATA2_0 + dummywidth;

	if (UCODE_SCREEN == 'x')
	    UCODE_SCREEN = '2';

  	printf("\nCompustat Industrial Annual...\n");
    }
    else if (INDQTR)
    {
    	INDANN = false;
	QTR_0 = get_item_pos_width("QTR",&QTR_W);
	PDA_0 = get_item_pos_width("PDA_ARRAY",&dummywidth);
	ENDPDA_0 = PDA_0 + dummywidth;
	PDF_0 = get_item_pos_width("PDF_ARRAY",&dummywidth);
	ENDPDF_0 = PDF_0 + dummywidth;

	PDA_REP_SIZE = (ENDPDA_0 - PDA_0) / PDS_PER_REC;
	PDF_REP_SIZE = (ENDPDF_0 - PDF_0) / PDS_PER_REC;

	if (UCODE_SCREEN == 'x')
	    UCODE_SCREEN = '0';

  	printf("\nCompustat Industrial Quarterly...\n");
    }

    set_signals(READER_SignalResponse);

    if (NULL == (Layout = file_layout(LAYOUT_FILE)))
    {
	READER_FatalError ("Error Reading Layout");
    }

    repeatWidths();

    READER_InitSemaphoreStructures ();

    READER_CreateBuffers ();

    READER_CreateSemaphores ();

    if ( IsntNil (nullRecPattern = (char *) malloc (strlen (NULL_REC) + 1)))
    {
	strcpy (nullRecPattern, NULL_REC);
	if (FILE_ebcdic(Layout)) READER_ConvertToEbcdic (
	    (unsigned char *)nullRecPattern, 
	    strlen (nullRecPattern)
	);
    }
    else READER_FatalError ("Can't Allocate Null Record Pattern");

    READER_SpawnReader (
	FILE_blkLen(Layout), 
	NUM_TAPES, 
	READER_BufferSize (MIN_BUF_SIZ), 
	MAXRECS, 
	FILE_prefix(Layout), 
	TAPE_NAME, 
	true,
	nullRecPattern // Don't include records beginning with this string ...
    );
    convert_and_save ();

    write_layouts(Layout);
          
    READER_CleanupAndExit();

    return NormalExitValue;
}
 
         

/************************************************************************
  compustat.c 2 replace /usr/dbs/utils
  860619 10:49:13 ees newer version

 ************************************************************************/
/************************************************************************
  compustat.c 3 replace /users/ees/dbutil
  860627 15:28:27 ees added key header record

 ************************************************************************/
/************************************************************************
  compustat.c 4 replace /users/ees/dbutil
  860813 13:05:51 ees changed UCODE seletion

 ************************************************************************/
/************************************************************************
  compustat.c 5 replace /users/ees/dbutil
  861031 13:16:48 ees removed tape reader functions, and layout functions

 ************************************************************************/
/************************************************************************
  compustat.c 6 replace /users/ees/dbutil
  861031 13:21:07 ees added SignalResponse

 ************************************************************************/
/************************************************************************
  compustat2.c 2 replace /users/jck/updates
  870219 12:12:51 jck Added options to filter periods

 ************************************************************************/
/************************************************************************
  compustat2.c 3 replace /users/m2/dbup
  880426 11:59:46 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  compustat2.c 4 replace /users/jck/updates
  880512 16:05:40 jck The env variable 'DBSourceName' is now checked in determining the input source.
	'/dev/rmt' is still the default

 ************************************************************************/
/************************************************************************
  compustat2.c 5 replace /users/jck/updates
  880526 14:08:21 jck Fix uninitialized variable bugs exposed on the Apollo

 ************************************************************************/
/************************************************************************
  compustat2.c 6 replace /users/jck/updates
  880902 08:12:50 jck Fortify the period range calculations

 ************************************************************************/
/************************************************************************
  compustat2.c 7 replace /users/jck/updates
  881101 09:26:21 jck Commented out inappropriate debugging statements in 'CalcStartEndPeriods'

 ************************************************************************/
/************************************************************************
  compustat2.c 8 replace /users/jck/dbupdate
  891009 12:53:15 jck Standardizer updates

 ************************************************************************/

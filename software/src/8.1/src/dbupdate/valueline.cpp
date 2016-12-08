/*******************************************
 ************	valueline.c	************
 *******************************************/
/* Takes as input the valueline historical */
/* tapes and the vlineb.lay file.          */
/* Extracts the needed information (spec-  */
/* ified in the .lay file) from the tapes, */
/* and creates key, layout, and data files.*/
/* There is an output file generated for   */
/* each item of a different type and size. */
/*	GOPT:				   */
/*		r	# Records	   */
/*		l	layout file	   */
/* 		t 	tape device	   */
/*		n	# of tapes	   */	
/*		s	start year	   */
/*		e	end year	   */
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
#define ASCII_0		'0'     
#define STRING_NA   ' '			/* incorporator changes this to NA */
#define VL_NA		"99999999999999999999"   /* Valueline's NA */
#define YEAR_0		39
#define YEAR_W		2

#define BUFFERSIZE\
     ((MAX_BUFFERSIZE /  * FILE_blkLen(Layout))

/***************
    Globals
 ***************/

PrivateVarDef int MAXRECS, 
		  NUM_TAPES, 
		  START_YEAR,		/* command line switch */
		  END_YEAR;		/* beginning and ending year */

PrivateVarDef char Period[MAX_PERIOD_W];
PrivateVarDef char const *TAPE_NAME;
PrivateVarDef char const *LAYOUT_FILE;
      
PrivateVarDef int RecordCount = 0,         
		  BkPtrCount = -1,     /* Back pointer count */
		  Period_W = 0;

PrivateVarDef FILE_LAYOUT *Layout;


/*************************************************
 **********	Record Processing	**********
 *************************************************/

#define process_record()\
{\
    flush_files(Layout);\
    for (i = 0; i < FILE_itemCount(Layout); i++)\
    {\
	item = FILE_item(Layout, i);\
	file = ITEM_file(item);\
	if (ITEM_type(item) == BACKPTR)\
	    *OUT_intPtr(file)++ = BkPtrCount;\
	else {\
	       if (ITEM_type(item) == DECIMAL)\
	       {\
		   if (SAME == memcmp(&rptr[ITEM_pos (item)], VL_NA, ITEM_len(item)))\
	               *OUT_floatPtr(file)++ = LAY_FloatNA;\
	           else \
			*OUT_floatPtr(file)++ = (float)\
	                   ( dtb(&rptr[ITEM_pos(item)], ITEM_len(item)) /\
					 ITEM_decs(item));\
	       } \
	       else if (ITEM_type(item) == DOUBLE)\
	       {\
		    memcpy(string, &rptr[ITEM_pos(item)], ITEM_len(item));\
		    string[ITEM_len(item)] = '\0';\
		    if (SAME == memcmp(string, VL_NA, ITEM_len(item)))\
			*OUT_doublePtr(file)++ = LAY_DoubleNA;\
		    else \
			*OUT_doublePtr(file)++ =(atof(string)/ITEM_decs(item));\
	       } \
	       else if (ITEM_type(item) == INTEGER)\
	       {\
		    memcpy(string, &rptr[ITEM_pos(item)], ITEM_len(item));\
		    string[ITEM_len(item)] = '\0';\
		   if (SAME == memcmp(string, VL_NA, ITEM_len(item)))\
	               *OUT_intPtr(file)++ = LAY_IntegerNA; \
	           else  \
	               *OUT_intPtr(file)++ = atoi(string);\
	       }\
	       else \
	       {\
		    if (SAME == memcmp(&rptr[ITEM_pos(item)], VL_NA, ITEM_len(item))) \
		    {\
			memset (OUT_charPtr(file), STRING_NA, ITEM_out(item)); \
			OUT_charPtr(file) += ITEM_out(item);\
		    }\
	           else { \
	           	memcpy(OUT_charPtr(file), \
		           &rptr[ITEM_pos(item)], ITEM_len(item));\
	           	for (j = ITEM_len(item); j < ITEM_out(item); j++)\
	               	   *(OUT_charPtr(file) + j) = '\0';\
	           	OUT_charPtr(file) += ITEM_out(item);\
 			} \
	       }\
           }  /* else */ \
   } /* for */ \
} 


/*************************************************
 **********	Special Key Functions	**********
 *************************************************/

PrivateFnDef void keyFunc1(
    KEY_LAYOUT *		key,
    char *			record
)
{
    memcpy(KEY_bufPtr(key), &record[KEY_idPos(key)], KEY_idLen(key));
    KEY_bufPtr(key) += KEY_idLen(key);
    memcpy(KEY_bufPtr(key), Period, Period_W);
    KEY_bufPtr(key) += Period_W;
    *(KEY_bufPtr(key)++) = '\n';
}

PrivateVarDef void (*KeyFunc[])(KEY_LAYOUT *, char *) = {
    NULL, /**** leave NULL ****/
    keyFunc1
};


/*************************************************
 **********	Record Processing	**********
 *************************************************/

PrivateFnDef void process_buffer (
    char *			buffer,
    int				count
)
{
    int	i,j, year;
    char *rptr, *rlim, string[50], Year[YEAR_W + 1];
    OUTFILE *file;    
    ITEM_LAYOUT *item;
    
    for (rptr = buffer, rlim = buffer + count; rptr < rlim;
					rptr += FILE_recLen(Layout)) {
	memcpy(Year, &rptr[YEAR_0], YEAR_W);
	Year [YEAR_W] = '\0';
	year = atoi(Year);
	if (year < START_YEAR ) 	/* skip records not */
	    continue;			/* part of user     */
	if (year > END_YEAR )		/* specified range  */
	    continue;	
            
        BkPtrCount++;			/* BkPtrCount gets incremented with */
					/* every record because for each    */
					/* recored both the timeseries and  */
					/* static data are written.         */
	RecordCount++;
        write_keys(rptr, Layout, KeyFunc);
	process_record();

    } /* for rptr loop */

} /* end procedure process_buffer */


/******************************************
 *  Consumer Process  (Convert and Save)  *
 ******************************************/

PrivateFnDef void convert_and_save (
    void
)
{

    DATABUFFER *buffer;
    
    open_keys(Layout);				/* in filelayout module */

    while (buffer = READER_GetBufferToSave ())	/* in reader module */
    {
        if (FILE_ebcdic(Layout)) /*in reader module*/
	    READER_ConvertToAscii (buffer->content, buffer->count);

	process_buffer((char *)buffer->content, buffer->count);
	
	READER_ReturnBufferToReader ();		/* in reader module */
    }

    printf("\nRecords Written = %d\n", RecordCount);

    close_files(Layout);			/* in filelayout module */

    close_keys(Layout);				/* in filelayout module */

}
      

/****************************
 *  Main Program Utilities  *
 ****************************/

GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription     
        ("OPTION_R", 'r', NilOf (char *), "999999999")
    GOPT_ValueOptionDescription	    
        ("OPTION_L", 'l', NilOf (char *), "vlineb.lay")
    GOPT_ValueOptionDescription	    
        ("OPTION_T", 't', "DBSourceName", "/dev/rmt")
    GOPT_ValueOptionDescription	    
        ("OPTION_N", 'n', NilOf (char *), "1")
    GOPT_ValueOptionDescription	    
        ("OPTION_S", 's', NilOf (char *), "-1")
    GOPT_ValueOptionDescription	    
        ("OPTION_E", 'e', NilOf (char *), "98")
GOPT_EndOptionDescriptions;
				/*  The default end year is 98 because */
				/*  99 signifies NA.                   */


/******************
 *  Main Program  *
 ******************/

int main (
    int				argc,
    char *			argv[]
)
{
    GOPT_AcquireOptions (argc, argv);
    NUM_TAPES = atoi( GOPT_GetValueOption ("OPTION_N"));
    TAPE_NAME =   GOPT_GetValueOption ("OPTION_T");
    LAYOUT_FILE = GOPT_GetValueOption ("OPTION_L");
    START_YEAR =atoi( GOPT_GetValueOption ("OPTION_S"));
    END_YEAR =atoi( GOPT_GetValueOption ("OPTION_E"));
    MAXRECS =atoi( GOPT_GetValueOption ("OPTION_R"));
    GOPT_SeekExtraArgument (0, 0);

    printf("\nValueline Historical ...\n");

    set_signals(READER_SignalResponse);

    if (NULL == (Layout = file_layout(LAYOUT_FILE)))
    {
	READER_FatalError ("Error Reading Layout");
    }
    
    READER_InitSemaphoreStructures ();

    READER_CreateBuffers ();

    READER_CreateSemaphores ();

    READER_SpawnReader (FILE_blkLen(Layout), 
		  NUM_TAPES, 
		  READER_BufferSize (FILE_blkLen(Layout)), 
		  MAXRECS, 
		  FILE_prefix(Layout), 
		  TAPE_NAME, 
		  true);

    convert_and_save ();

    write_layouts(Layout);
          
    READER_CleanupAndExit();

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  valueline.c 1 replace /users/lis/updates
  880317 08:42:18 jck Valueline Standardizer

 ************************************************************************/
/************************************************************************
  valueline.c 2 replace /users/m2/dbup
  880426 12:25:25 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  valueline.c 3 replace /users/jck/updates
  880512 16:06:17 jck The env variable 'DBSourceName' is now checked in determining the input source.
	'/dev/rmt' is still the default

 ************************************************************************/
/************************************************************************
  valueline.c 4 replace /users/jck/updates
  880526 14:11:07 jck Fix uninitialized variable bugs exposed on the Apollo

 ************************************************************************/
/************************************************************************
  valueline.c 5 replace /users/jck/updates
  880902 15:27:00 jck Made valueline pay attention to the EBCDIC->ASCII conversion flag in the layout file

 ************************************************************************/
/************************************************************************
  valueline.c 6 replace /users/jck/updates
  881219 08:35:29 jck Fixed pointer reference problem in the handling of string NA's

 ************************************************************************/
/************************************************************************
  valueline.c 7 replace /users/jck/dbupdate
  891009 12:57:36 jck Standardizer updates

 ************************************************************************/

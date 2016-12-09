/*****************************************************
 ************	preproc2.c	**********************
 ****************************************************/

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
#define YEAR_0	    20
#define YY_0	    22

/*****
    Command Line Parameters
 *****/
PrivateVarDef int MaxRecs, NumTapes;
PrivateVarDef char const*TapeName, *LayoutFile;
PrivateVarDef double DoubleNACode;
PrivateVarDef float FloatNACode;
PrivateVarDef int DecimalNACodeDefined = 0,
		  IntegerNACodeDefined = 0,
		  IntegerNACode;

/****** 
    Globals 
******/
PrivateVarDef FILE_LAYOUT *FileLayout;
PrivateVarDef char Period[MAX_PERIOD_W];
PrivateVarDef char Date[26];
PrivateVarDef int Period_W = 0, 
		  RecordCount = 0, 
		  HeadersDone = false, 
	          HeaderCount = 0,
		  BkPtrCount = -1;


/*************************************************************
**************	Special Date Routines   **********************
*************************************************************/

PrivateFnDef void getDate(
    void
)
{
    time_t longtime;

    time(&longtime);
    ctime_r (&longtime, Date);
}

PrivateFnDef int abelDate(
    char *			buffer
)
{
    memcpy(Period, &Date[YY_0], 2);
    memcpy(&Period[2], &buffer[7], 4);
    Period[6] = '\0';
    Period_W = 6;
    KEY_periodLen(FILE_key(FileLayout, 1)) = 6;
    FILE_dateFn(FileLayout) = 0;
    return false;
}

PrivateFnDef int trinityDate(
    char *			buffer
)
{
    int i;
    
    memcpy(Period, &buffer[186], 2);
    memcpy(&Period[2], &buffer[180], 2);
    memcpy(&Period[4], &buffer[183], 2);
    for (i = 0; i < 6; i++)
        if (Period[i] == ' ') Period[i] = '0';
    Period[6] = '\0';
    Period_W = 6;
    KEY_periodLen(FILE_key(FileLayout, 1)) = 6;
    FILE_dateFn(FileLayout) = 0;
    return false;
}

PrivateFnDef int muller180Date(
    char *			Unused(b)
)
{
    FILE *fd;
    char buffer[101];
    
    if (NULL == (fd = fopen("m180date.dat", "r")))
        READER_FatalError("Error Opening Muller Date File");
    if (NULL == fgets(buffer, 100, fd))
        READER_FatalError("Error Reading Muller Date File");
    fclose(fd);
    memcpy(Period, &buffer[13], 2);
    memcpy(&Period[2], &buffer[9], 4);
    Period[6] = '\0';
    Period_W = 6;
    FILE_dateFn(FileLayout) = 0;
    return false;
}

PrivateVarDef int (*DateFunc[])(char *) = {
    NULL, /*** leave NULL ***/
    abelDate,
    trinityDate,
    muller180Date
};


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
    0,
    keyFunc1, 
};


/*************************************************************
**************	Calculated Items    **************************
*************************************************************/

PrivateFnDef void mullerPrice(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{ 
    double cents, mode;

    cents   = dtb(&rec[ITEM_pos(item) + 4], 2);
    mode    = dtb(&rec[ITEM_pos(item) + 6], 1);

    if (9 == mode)
        cents /= 100;
    else
	cents /= (8 * (mode + 1));  /*** eights, sixtenths, etc. ***/
	
    *OUT_floatPtr(file)++ = (float)(dtb(&rec[ITEM_pos(item)], 4) + cents);
}

PrivateFnDef void mullerVolume(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{
    *OUT_floatPtr(file)++ = (rec[ITEM_pos(item) - 1] == '1') 
			? (float)(dtb(&rec[ITEM_pos(item)], ITEM_len(item)) / 100)
		        : (float) dtb(&rec[ITEM_pos(item)], ITEM_len(item));
}

PrivateVarDef int LastDayMonth[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

PrivateFnDef void FourDigitDate(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{
/**** date is in MMYY format ****/
    char buffer[10];
    int month;

    memcpy(buffer, &Date[YEAR_0], 2);
    memcpy(&buffer[2], &rec[ITEM_pos(item) + 2], 2);
    memcpy(&buffer[4], &rec[ITEM_pos(item)], 2);
    buffer[6] = '\0';
    month = atoi(&buffer[4]);
    memcpy(&buffer[6], "00", 2);
    
    *OUT_intPtr(file)++ = (int)(dtb(buffer, 8) + LastDayMonth[month - 1]);
}

PrivateFnDef void FiveDigitDate(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{
/**** date is in MMDDY format ****/
    char buffer[10];

    memcpy(buffer, &Date[YEAR_0], 3);
    buffer[3] = rec[ITEM_pos(item) + 4];
    memcpy(&buffer[4], &rec[ITEM_pos(item)], 4);
    
    *OUT_intPtr(file)++ = (int) dtb(buffer, 8);
}

PrivateFnDef void SixDigitDate(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{
/**** date is in MMDDYY format ****/
    char buffer[10];

    memcpy(buffer, &Date[YEAR_0], 2);
    memcpy(&buffer[2], &rec[ITEM_pos(item) + 4], 2);
    memcpy(&buffer[4], &rec[ITEM_pos(item)], 4);
    
    *OUT_intPtr(file)++ = (int)dtb(buffer, 8);
}

PrivateFnDef void EightDigitDate(
    char *			rec,
    ITEM_LAYOUT *		item,
    OUTFILE *			file
)
{
/**** date is in MMDDYYYY format ****/
    char buffer[10];

    memcpy(buffer, &rec[ITEM_pos(item) + 4], 4);
    memcpy(&buffer[4], &rec[ITEM_pos(item)], 4);
    
    *OUT_intPtr(file)++ = (int)dtb(buffer, 8);
}

PrivateVarDef void (*CalcFunc[])(char *, ITEM_LAYOUT *, OUTFILE *) = {
    NULL, /*** leave NULL ***/
    mullerPrice,
    mullerVolume,
    FourDigitDate,
    FiveDigitDate,
    SixDigitDate,
    EightDigitDate
};


/******************************************
 *  Consumer Process  (Convert and Save)  *
 ******************************************/

#define MAX_ITEM_LEN 256

PrivateFnDef void process_decimal (
    char *		pRecord,
    ITEM_LAYOUT *	item
)
{
    OUTFILE *	file		= ITEM_file (item);
    char *	itemStart	= pRecord + ITEM_pos (item);
    char	itemString [MAX_ITEM_LEN];
    char *	itemEnd		= itemString + ITEM_len (item);
    char *	parseStopper;

    if (ITEM_len (item) >= MAX_ITEM_LEN)
    {
	printf (">>>> Item:'%s' length:%d maximum:%d\n",
		ITEM_code (item), ITEM_len (item), MAX_ITEM_LEN);
	READER_FatalError (">>>>  This item exceeds maximum length  <<<<");
    }

    memcpy (itemString, itemStart, ITEM_len (item));
    *itemEnd = '\0';
    *OUT_floatPtr (file) =
	(float) (strtod (itemString, &parseStopper) / ITEM_decs (item));
    if (parseStopper <= itemString ||
	parseStopper > itemEnd) *OUT_floatPtr (file) = LAY_FloatNA;
    while (isspace ((int)*parseStopper))parseStopper++;
    if (*parseStopper != '\0') *OUT_floatPtr (file) = LAY_FloatNA;
    if (DecimalNACodeDefined && *OUT_floatPtr (file) == FloatNACode)
	*OUT_floatPtr (file) = LAY_FloatNA;
    OUT_floatPtr (file)++;
}

PrivateFnDef void process_double (
    char *		pRecord,
    ITEM_LAYOUT *	item
)
{
    OUTFILE *	file		= ITEM_file (item);
    char *	itemStart	= pRecord + ITEM_pos (item);
    char	itemString [MAX_ITEM_LEN];
    char *	itemEnd		= itemString + ITEM_len (item);
    char *	parseStopper;

    if (ITEM_len (item) >= MAX_ITEM_LEN)
    {
	printf ("Item:'%s' length:%d\n", ITEM_code (item), ITEM_len (item));
	READER_FatalError (">>>>  exceeds Maximum length of 255 characters");
    }

    memcpy (itemString, itemStart, ITEM_len (item));
    *itemEnd = '\0';
    *OUT_doublePtr (file) =
	strtod (itemString, &parseStopper) / ITEM_decs (item);
    if (parseStopper <= itemString ||
	parseStopper > itemEnd) *OUT_doublePtr (file) = LAY_DoubleNA;
    while (isspace ((int)*parseStopper))parseStopper++;
    if (*parseStopper != '\0') *OUT_doublePtr (file) = LAY_DoubleNA;
    if (DecimalNACodeDefined && *OUT_doublePtr (file) == DoubleNACode)
	*OUT_doublePtr (file) = LAY_DoubleNA;
    OUT_doublePtr (file)++;
}
PrivateFnDef void process_integer (
    char *		pRecord,
    ITEM_LAYOUT *	item
)
{
    OUTFILE *	file		= ITEM_file (item);
    char *	itemStart	= pRecord + ITEM_pos (item);
    char	itemString [MAX_ITEM_LEN];
    char *	itemEnd		= itemString + ITEM_len (item);
    char *	parseStopper;

    if (ITEM_len (item) >= MAX_ITEM_LEN)
    {
	printf ("Item:'%s' length:%d\n", ITEM_code (item), ITEM_len (item));
	READER_FatalError (">>>>  exceeds Maximum length of 255 characters");
    }

    memcpy (itemString, itemStart, ITEM_len (item));
    *itemEnd = '\0';
    *OUT_intPtr (file) =
	(int) strtol (itemString, &parseStopper, 10);
    if (parseStopper <= itemString ||
	parseStopper > itemEnd) *OUT_intPtr (file) = LAY_IntegerNA;
    while (isspace ((int)*parseStopper))parseStopper++;
    if (*parseStopper != '\0') *OUT_intPtr (file) = LAY_IntegerNA;
    if (IntegerNACodeDefined && *OUT_intPtr (file) == IntegerNACode)
	*OUT_intPtr (file) = LAY_IntegerNA;
    OUT_intPtr (file)++;
}


#define process_record()\
{\
    flush_files(FileLayout);\
    for (i = 0; i < FILE_itemCount(FileLayout); i++)\
    {\
	item = FILE_item(FileLayout, i);\
	file = ITEM_file(item);\
	if (ITEM_calc(item))\
	    (*CalcFunc[ITEM_calc(item)])(rptr, item, file);\
	else if (ITEM_type(item) == DECIMAL) process_decimal (rptr, item);\
	else if (ITEM_type(item) == DOUBLE) process_double (rptr, item);\
	else if (ITEM_type(item) == INTEGER) process_integer (rptr, item);\
	else if (ITEM_type(item) == BACKPTR)\
	    *OUT_intPtr(file)++ = BkPtrCount;\
	else\
	{\
	    memset (OUT_charPtr(file), 0, ITEM_out(item));\
	    memcpy (OUT_charPtr(file), &rptr[ITEM_pos(item)], ITEM_len(item));\
	    OUT_charPtr(file) += ITEM_out(item);\
	}\
    }\
}


PrivateFnDef void process_buffer (
    char *	    		buffer,
    int				count
)
{
    char *rptr, *rlim;
    int i;
    OUTFILE *file;
    ITEM_LAYOUT *item;
    
/***** Call Date Function ******/
    if (FILE_dateFn(FileLayout))
        (*DateFunc[FILE_dateFn(FileLayout)])(buffer);

/***** Process Buffer ******/         
    for (rptr = buffer, rlim = buffer + count; rptr < rlim;
					rptr += FILE_recLen(FileLayout)) 
    {
	RecordCount++;
	BkPtrCount++;

	if (!HeadersDone)
	{
	    HeaderCount++;
	    if (HeaderCount == FILE_headers(FileLayout))
		HeadersDone = true;
	    continue;
	}
    
	if (!select_record(rptr, FileLayout))
	    continue;
	    
	write_keys(rptr, FileLayout, KeyFunc);
	
	process_record();
   
    } /* for rptr loop */
    
 } /* end process_buffer */


PrivateFnDef void convert_and_save (
    void
)
{
    DATABUFFER *buffer;
    
    open_keys(FileLayout);

    if (FILE_headers(FileLayout) == 0)
	HeadersDone = true;

    while (buffer = READER_GetBufferToSave ())
    {
        if (FILE_ebcdic(FileLayout))
	    READER_ConvertToAscii (buffer->content, buffer->count);

	process_buffer((char *)buffer->content, buffer->count);
	
	READER_ReturnBufferToReader ();
    }
    
    printf("\nRecords Processed = %d\n", RecordCount);

    close_files(FileLayout);

    close_keys(FileLayout);
    
}


/****************************
 *  Main Program Utilities  *
 ****************************/

GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription	    
        ("OPTION_R", 'r', NilOf (char *), "999999999")
    GOPT_ValueOptionDescription	    
        ("OPTION_T", 't', "DBSourceName", "/dev/rmt")
    GOPT_ValueOptionDescription	    
        ("OPTION_L", 'l', NilOf (char *), "")
    GOPT_ValueOptionDescription	    
        ("OPTION_N", 'n', NilOf (char *), "0")
    GOPT_ValueOptionDescription	    
        ("OPTION_D", 'd', NilOf (char *), "")
    GOPT_ValueOptionDescription	    
        ("OPTION_I", 'i', NilOf (char *), "")
GOPT_EndOptionDescriptions;


/******************
 *  Main Program  *
 ******************/

int main (
    int				argc,
    char *			argv[]
)
{
    GOPT_AcquireOptions (argc, argv);
    MaxRecs	= atoi( GOPT_GetValueOption ("OPTION_R"));
    TapeName   = GOPT_GetValueOption ("OPTION_T");
    LayoutFile = GOPT_GetValueOption("OPTION_L");
    NumTapes   = atoi( GOPT_GetValueOption("OPTION_N"));
    DecimalNACodeDefined = strlen (GOPT_GetValueOption ("OPTION_D"));
    DoubleNACode = atof (GOPT_GetValueOption ("OPTION_D"));
    FloatNACode = (float)atof (GOPT_GetValueOption ("OPTION_D"));
    IntegerNACodeDefined = strlen (GOPT_GetValueOption ("OPTION_I"));
    IntegerNACode = atoi (GOPT_GetValueOption ("OPTION_I"));
    GOPT_SeekExtraArgument (0, 0);

    set_signals(READER_SignalResponse);
    getDate();
    
    if (0 == strlen (LayoutFile) ||
	NULL == (FileLayout = file_layout(LayoutFile)))
    {
	READER_FatalError ("Error Reading Layout");
    }

    READER_InitSemaphoreStructures ();
    READER_CreateBuffers ();
    READER_CreateSemaphores ();

    READER_SpawnReader (FILE_blkLen(FileLayout), 
		  NumTapes, 
		  READER_BufferSize (FILE_blkLen(FileLayout)), 
		  MaxRecs, 
		  FILE_prefix(FileLayout), 
		  TapeName, 
		  true);

    convert_and_save ();

    write_layouts(FileLayout);

    READER_CleanupAndExit();

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  preproc2.c 1 replace /users/ees/dbutil
  861216 14:07:40 jck Preprocessor to output type 'i' as integer

 ************************************************************************/
/************************************************************************
  preproc2.c 2 replace /users/jck/updates
  861219 10:58:59 jck Capacity to deal with double precision numbers added

 ************************************************************************/
/************************************************************************
  preproc2.c 3 replace /users/lis/updates
  870121 13:44:43 lis Back pointer added

 ************************************************************************/
/************************************************************************
  preproc2.c 4 replace /users/m2/dbup
  880426 12:15:53 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  preproc2.c 5 replace /users/jck/updates
  880512 16:05:49 jck The env variable 'DBSourceName' is now checked in determining the input source.
	'/dev/rmt' is still the default

 ************************************************************************/
/************************************************************************
  preproc2.c 6 replace /users/jck/updates
  880513 09:40:39 jck Changed the default for the -n option from 1 to 0

 ************************************************************************/
/************************************************************************
  preproc2.c 7 replace /users/jck/updates
  880902 08:02:31 jck Add handling for integer NA's

 ************************************************************************/
/************************************************************************
  preproc2.c 8 replace /users/jck/dbupdate
  891009 12:55:50 jck Standardizer updates

 ************************************************************************/

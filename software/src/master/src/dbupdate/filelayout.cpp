/*****************************************
 **********	filelayout.c	**********
 *****************************************/

#include "Vk.h"

#include "reader.h"
#include "filelayout.h"
#include "sigHandler.h"


/*************************************************
 **********	Initial File Layout	**********
 *************************************************/

PrivateFnDef OUTFILE *find_right_file(
    FILE_LAYOUT *		layout,
    int				inum
);

PrivateFnDef double power_10 (
    int				x
)
{
    int i;
    double p;

    p = 1.0;
    for( i = 1; i <= x; i++)
	p = p * 10;

    return (p);
}

#if defined (_WIN32)
PrivateFnDef char const* FinalPathComponent (char const* filename) {
    char const *baseptr;
    if (NULL == (baseptr = strrchr (filename, '\\')) &&
	NULL == (baseptr = strrchr (filename, '/')) &&
	NULL == (baseptr = strrchr (filename, ':')))
    {
	return filename;
    }
    else
	return baseptr + 1;
}
#else 

PrivateFnDef char const *FinalPathComponent (char const *filename) {
    char const *baseptr = strrchr (filename, '/');
    return baseptr ? baseptr + 1 : filename;
}


#endif

PublicFnDef FILE_LAYOUT *file_layout (
    char const*			filename
)
{
    FILE *fd;
    int i;
    char buffer[256];
    char *lastdot;
    FILE_LAYOUT *layout;
    KEY_LAYOUT *key;
    SELECT_LAYOUT *sel;
    ITEM_LAYOUT *item;

    fd = fopen(filename, "r");
    if (fd == NULL)
    	return  NULL;

    layout = (FILE_LAYOUT *)calloc(1, sizeof(FILE_LAYOUT));
    if (layout == NULL)
        return(NULL);


    if (fgets( buffer, 255, fd) == NULL)
	return NULL;

    while ( sscanf( buffer, "%s %d %d %d %d %d %d %d",
				FILE_prefix(layout), /* ignored, and taken
							from 'filename'
							instead (see below)*/
				&FILE_recLen(layout),
				&FILE_blkLen(layout),
				&FILE_ebcdic(layout),
				&FILE_headers(layout),
				&FILE_keyCount(layout),
				&FILE_selectCount(layout),
			        &FILE_dateFn(layout)) == -1 )
	fgets( buffer, 255, fd);   	/* sscanf = -1 signifies a blank */
					/* line                          */

/***
 * Determine the FILE_prefix to work with ...
 ***/
    filename = FinalPathComponent (filename);
    strncpy (FILE_prefix(layout), filename, PATH_MAX - 1);
    if (NULL != (lastdot = strrchr (FILE_prefix(layout), '.')))
    {
	*lastdot = '\0';
    }

    for (i = 0; i < FILE_keyCount(layout); i++)
    {
	FILE_key(layout, i) = (KEY_LAYOUT *)calloc(1, sizeof(KEY_LAYOUT));
        key = FILE_key(layout, i);
        do {				/* skip blank lines */
	    fgets(buffer, 255, fd);
	} while ( -1 == sscanf(buffer, "%d %d %d %d %d %d",
					 &KEY_idPos(key),
					 &KEY_idLen(key),
					 &KEY_periodPos(key),
					 &KEY_periodLen(key),
					 &KEY_specFn(key),
					 &KEY_estRows(key)) );
    }

    for (i = 0; i < FILE_selectCount(layout); i++)
    {
	FILE_select(layout, i) =
	    (SELECT_LAYOUT *)calloc(1, sizeof(SELECT_LAYOUT));
	sel = FILE_select(layout, i);
	do {				/* skip blank lines */
           fgets(buffer, 255, fd);
	} while ( -1 == sscanf(buffer, "%d %d %s", &SEL_pos(sel),
				                   &SEL_len(sel),
				                    SEL_value(sel)) );
    }

    i = 0;
    while (fgets( buffer, 255, fd))
    {
	char type[128], desc[512];

	FILE_item(layout, i) =
	    (ITEM_LAYOUT *)calloc(1, sizeof(ITEM_LAYOUT));
	item = FILE_item(layout, i);
        if( -1 == sscanf( buffer, "%s %d %d %d %d %s %d %[^\n]",
    				 ITEM_code(item),
    			    	 &ITEM_key(item),
    			    	 &ITEM_pos(item),
    			    	 &ITEM_len(item),
    			     	 &ITEM_inputdecs(item),
    			    	 type,
    			    	 &ITEM_calc(item),
    			    	 desc)) continue;
						/* skip blank lines */

	strncpy (ITEM_desc (item), desc, ITEM_DESC_LEN);
	ITEM_desc (item) [ITEM_DESC_LEN] = '\0';

	if (ITEM_key (item) >= FILE_keyCount (layout) ||
	    ITEM_key (item) < 0)
	{
	    fprintf (stderr, ">>>>>>>>>\tFATAL ERROR\t<<<<<<<<<<\n");
	    fprintf (stderr,
		     "\tItem[%s]: Unknown Table Index (%d)\n",
		     ITEM_code (item),  ITEM_key (item));
	    exit (ErrorExitValue);
	}

	switch (type[0])
	{
	case 'a':
	    ITEM_type (item) = ALPHA + type[1];

	    break;
	case 'b':
	    ITEM_type (item) = BACKPTR;
	    break;
	case 'c':
	    ITEM_type (item) = CODE + type[1];
	    break;
	case 'd':
	    ITEM_type (item) = DECIMAL + type[1];
	    break;
	case 'D':
	    ITEM_type (item) = DOUBLE + type[1];
	    break;
	case 'i':
	    ITEM_type (item) = INTEGER + type[1];
	    break;
	default:
	    fprintf (stderr, "Unknown Item Type: %s\n", type);
	    exit (ErrorExitValue);
	    break;
	}

	ITEM_out(item) = ITEM_len(item);

	if (strlen (ITEM_code(item)) > (size_t)20)
	{
	    printf ("Item Code %s truncated to %20s\n", ITEM_code(item),
							ITEM_code(item));
	    ITEM_code(item)[20] = '\0';
	}

	ITEM_decs (item) = power_10(ITEM_inputdecs(item));

#ifdef TRACING
        printf ("Item: %s %d %d %d %d %f %x %s \n",
    				 ITEM_code(item),
    			    	 ITEM_key(item),
				 ITEM_pos(item),
    			    	 ITEM_len(item),
    			     	 ITEM_inputdecs(item),
    			     	 ITEM_decs(item),
    			    	 ITEM_type(item),
    			    	 ITEM_desc(item));
#endif

        ITEM_pos(item) -= 1;

	if (NULL == (ITEM_file(item) = find_right_file(layout, i)))
	    return(NULL);

	i++;
    }

    FILE_itemCount(layout) = i;

    if (fclose (fd))
    	return 0;

    return (layout);

}


/*************************************************
 **********	Data File Functions	**********
 *************************************************/

PrivateFnDef OUTFILE *find_right_file(
    FILE_LAYOUT *		layout,
    int				inum
)
{
    int i, bytes;
    char filename[PATH_MAX+128];
    ITEM_LAYOUT *item;
    OUTFILE *file;

    item = FILE_item(layout, inum);

    for (i = 0; i < FILE_fileCount(layout); i++)
    {
	file = FILE_outfile(layout, i);

	if ((ITEM_type(item)==OUT_type(file) && ITEM_key(item)==OUT_key(file))
	 || (ITEM_type(item)==BACKPTR
		&& UnpartitionedType(OUT_type(file))==INTEGER
		&& ITEM_key(item)==OUT_key(file)
	    )
	)
	{
/*---------------------------------------------------------------------------
 * The different subtypes give the user a backdoor way to partition items of
 * like type into different physical files. Now that this goal has been
 * achieved, those subtypes can be collapsed back into their unpartitioned
 * types so that the remaining processing remains simple.
 *---------------------------------------------------------------------------
 */
	    ITEM_type(item) = UnpartitionedType (ITEM_type(item));

	    switch (ITEM_type(item))
	    {
	    case DECIMAL:
	    case DOUBLE:
	    case INTEGER:
	    case BACKPTR:
		OUT_item(file, OUT_itemCount(file)++) = inum;
		return(file);

	    default:
		if (ITEM_out(item) == OUT_granule(file))
		{
		    OUT_item(file, OUT_itemCount(file)++) = inum;
		    return(file);
		}

	        break;
	    }

	}
    }

/**** create new OUTFILE entry ****/
    if ( IsNil (
	    FILE_outfile(layout, FILE_fileCount(layout)) = (OUTFILE *) malloc (
		sizeof(OUTFILE)
	    )
	)
    )
    {
        display_error("Memallc Error");
	return(NULL);
    }
    file = FILE_outfile(layout, FILE_fileCount(layout));
    OUT_type(file) = ITEM_type(item) == BACKPTR ? INTEGER : ITEM_type(item);
    OUT_key(file) = ITEM_key(item);
    OUT_item(file, 0) = inum;
    OUT_itemCount(file) = 1;
/*---------------------------------------------------------------------------
 * The different subtypes give the user a backdoor way to partition items of
 * like type into different physical files. Now that this goal has been
 * achieved, those subtypes can be collapsed back into their unpartitioned
 * types so that the remaining processing remains simple.
 *---------------------------------------------------------------------------
 */
    ITEM_type (item) = UnpartitionedType (ITEM_type (item));
    switch (UnpartitionedType(OUT_type(file)))
    {
    case INTEGER:
	OUT_granule(file) = sizeof (int);
        break;
    case DECIMAL:
	OUT_granule(file) = sizeof (float);
        break;
    case DOUBLE:
        OUT_granule(file) = sizeof (double);
        break;
    default:
        OUT_granule(file) = ITEM_out(item);
        break;
    }

    OUT_header(file) = HEADERSIZE;
    OUT_charPtr(file) = OUT_buffer(file) + OUT_header(file);

/*** open new file ****/
    snprintf(filename, sizeof(filename),
	     "%s%d%s", FILE_prefix(layout),
	     FILE_fileCount(layout),
	     ".dat");
    bytes = numPreallocBytes(layout,file);
    if (-1 ==
        (int)(OUT_fd(file) = READER_SegmentCreate(filename, 0660, bytes)))
	return(NULL);

    FILE_fileCount(layout)++;
    return(file);
}


PublicFnDef int numPreallocBytes (
    FILE_LAYOUT *		layout,
    OUTFILE *			file
)
{
    unsigned bytes;

/**** pre-allocate file if KEY_estRows > 0 ****/
    bytes = (OUT_itemCount (file) *
	     OUT_granule   (file) *
	     KEY_estRows   (FILE_key (layout, OUT_key (file))));
    if (bytes > 0)
	bytes += OUT_header(file);

    return bytes;
}


PublicFnDef void flush_files (
    FILE_LAYOUT *		layout
)
{
    int i, bytes;
    OUTFILE *file;

    for (i = 0; i < FILE_fileCount(layout); i++)
    {
	file = FILE_outfile(layout, i);

 	bytes = (int)(OUT_charPtr(file) - OUT_buffer(file));

	if (bytes + (OUT_itemCount(file) * OUT_granule(file))
						     >= WRITE_BUF_SIZE)
	{
	    if (-1 == write(OUT_fd(file), OUT_buffer(file), bytes))
		READER_FatalError("Data File Write");
	    OUT_charPtr(file) = OUT_buffer(file);
	}
    }
}


PublicFnDef int select_record (
    char *			record,
    FILE_LAYOUT *		layout
)
{
    int i;
    SELECT_LAYOUT *sel;

    for (i = 0; i < FILE_selectCount(layout); i++)
    {
        sel = FILE_select(layout, i);
	if (0 != memcmp(&record[SEL_pos(sel)], SEL_value(sel), SEL_len(sel)))
		return false;
    }
    return true;
}


PublicFnDef void close_files (
    FILE_LAYOUT *		layout
)
{
    int i, bytes;
    OUTFILE *file;

    for (i = 0; i < FILE_fileCount(layout); i++)
    {
	file = FILE_outfile(layout, i);

	bytes = (int)(OUT_charPtr(file) - OUT_buffer(file));

	if (bytes > 0)
	{
	    if (-1 == write(OUT_fd(file), OUT_buffer(file), bytes))
		READER_FatalError("Data File Write");
	}

        if ( -1 == close(OUT_fd(file)))
	    READER_FatalError("Data File Close");
    }
}


/*****************************************
 **********	Key Functions	**********
 *****************************************/

PublicFnDef void open_keys (
    FILE_LAYOUT *		layout
)
{
    int i, j, fileCount, fileNum[MAX_FILES];
    char file_name[PATH_MAX+128];
    KEY_LAYOUT *key;
    OUTFILE *file;

    for (i = 0; i < FILE_keyCount(layout); i++)
    {
	key = FILE_key(layout, i);
	snprintf(file_name, sizeof(file_name),
		 "%s%d%s", FILE_prefix(layout),
		 i,
		 ".key");
	if ((KEY_fd(key) = READER_FileCreate (file_name, 0660, 0)) < 0)
	    READER_FatalError ("Key File Open");
	KEY_bufPtr(key) = KEY_buffer(key);
	KEY_count(key) = 0;
	fileCount = 0;
	for (j = 0; j < FILE_fileCount(layout); j++)
	{
	    file = FILE_outfile(layout, j);
	    if (OUT_key(file) == i)
		fileNum[fileCount++] = j;
	}

	sprintf(KEY_bufPtr(key), "%d %d %d",  KEY_idLen(key),
					      KEY_periodLen(key),
					      fileCount);
	KEY_bufPtr(key) += (strlen(KEY_buffer(key)));
	for(j = 0; j < fileCount; j++)
	{
	    sprintf(KEY_bufPtr(key), " %d",  fileNum[j]);
	    KEY_bufPtr(key) += strlen(KEY_bufPtr(key));
	}
	*KEY_bufPtr(key)++ = '\n';
    }

}


PublicFnDef void write_keys (
    char *			record,
    FILE_LAYOUT *		layout,
    void			(*keyFunc[])(KEY_LAYOUT *, char *)
)
{
    int i, bytes;
    KEY_LAYOUT *key;

    for (i = 0; i < FILE_keyCount(layout); i++)
    {
	key = FILE_key(layout, i);

	if (KEY_specFn(key))
	    (*keyFunc[KEY_specFn(key)])(key, record);
	else
	{
	    memcpy(KEY_bufPtr(key), &record[KEY_idPos(key)], KEY_idLen(key));
	    KEY_bufPtr(key) += KEY_idLen(key);
	    memcpy(KEY_bufPtr(key), &record[KEY_periodPos(key)],
						     KEY_periodLen(key));
	    KEY_bufPtr(key) += KEY_periodLen(key);
	    *(KEY_bufPtr(key)++) = '\n';
	}

	bytes = KEY_bufPtr(key) - KEY_buffer(key);
	if ((bytes + KEY_idLen(key) + KEY_periodLen(key) + MAX_PERIOD_W)
		>= WRITE_BUF_SIZE)
	{
	    if ( -1 == write(KEY_fd(key), KEY_buffer(key), bytes))
		READER_FatalError("Key File Write");
	    KEY_bufPtr(key) = KEY_buffer(key);
	}
	KEY_count(key)++;
    }
}


PublicFnDef void close_keys (
    FILE_LAYOUT *		layout
)
{
    int i, bytes;
    KEY_LAYOUT *key;

    for (i = 0; i < FILE_keyCount(layout); i++)
    {
	key = FILE_key(layout, i);

	if (KEY_bufPtr(key) > KEY_buffer(key))
	{
	    bytes = (int)(KEY_bufPtr(key) - KEY_buffer(key));
	    if (-1 == write(KEY_fd(key), KEY_buffer(key), bytes))
		READER_FatalError("Key File Write");
	}
	if ( -1 == close (KEY_fd(key)))
	    READER_FatalError("Key File Close");
    }

}


/*****************************************
 **********	Final Layouts	**********
 *****************************************/

PublicFnDef void write_layouts (
    FILE_LAYOUT *		layout
)
{
    int i, col, keyNum;
    char type, buffer[256], filename[PATH_MAX+128];
    FILE *file_ptr;
    OUTFILE *file;
    ITEM_LAYOUT *item;
    KEY_LAYOUT *key;

    for (i = 0; i < FILE_fileCount(layout); i++)
    {
	file = FILE_outfile(layout, i);
	snprintf(filename, sizeof(filename),
		 "%s%d%s",
		 FILE_prefix(layout),
		 i,
		 ".lay");

	if ( NULL == (file_ptr = fopen(filename, "w")))
	    READER_FatalError("Layout File Open");

	keyNum = OUT_key (file) >= FILE_keyCount (layout)
		 ? FILE_keyCount (layout) - 1
		 : OUT_key (file);

	key = FILE_key(layout, keyNum);

/****  Collapse the different subtypes back to their unpartitioned types  ****/
	switch (UnpartitionedType (OUT_type (file)))
	{
	case ALPHA:
	    type = 'a';
	    break;
	case CODE:
	    type = 'c';
	    break;
	case INTEGER:
	    type = 'i';
	    break;
	case DECIMAL:
	    type = 'd';
	    break;
	case DOUBLE:
	    type = 'D';
	    break;
	default:
	    fprintf (stderr, "Unknown Item Type: %d", OUT_type (file));
	    exit (ErrorExitValue);
	    break;
	}
	sprintf(buffer, "%c %d %d %d %d %d\n", type,
					    OUT_key(file),
					    OUT_granule(file),
					    KEY_count(key),
					    OUT_itemCount(file),
					    OUT_header(file));
	if ( EOF == fputs(buffer, file_ptr))
	    READER_FatalError("Layout File Write");

/***** print the list of item numbers by column and/or any other info
	from the ITEM_LAYOUT structure  *****/

	for (col = 0; col < OUT_itemCount(file); col++)
	{
	    item = FILE_item(layout, OUT_item(file, col));
	    sprintf(buffer, "%4.4d %s\n", OUT_item(file, col),
					  ITEM_code(item));
	    if ( EOF == fputs(buffer, file_ptr))
		READER_FatalError("Layout File Write");
	}

	if ( -1 == fclose(file_ptr))
	    READER_FatalError("Layout File Close");

    } /* for file */

}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  filelayout.c 1 replace /users/ees/dbutil
  861030 12:04:18 ees preprocessor routines

 ************************************************************************/
/************************************************************************
  filelayout.c 2 replace /users/ees/dbutil
  861031 13:13:58 ees added READER_FatalError()

 ************************************************************************/
/************************************************************************
  filelayout.c 3 replace /users/ees/dbutil
  861113 10:11:44 ees modified read layout function to scan for item calc index

 ************************************************************************/
/************************************************************************
  filelayout.c 4 replace /users/jck/updates
  861219 10:58:53 jck Capacity to deal with double precision numbers added

 ************************************************************************/
/************************************************************************
  filelayout.c 5 replace /users/lis/updates
  870121 13:43:58 lis Back pointer added

 ************************************************************************/
/************************************************************************
  filelayout.c 6 replace /users/lis/updates
  870204 14:10:03 lis Blank lines in .lay files permitted

 ************************************************************************/
/************************************************************************
  filelayout.c 7 replace /users/jck/updates
  871110 13:54:58 jck Added support for multiple decimal types

 ************************************************************************/
/************************************************************************
  filelayout.c 8 replace /users/jck/updates
  880107 08:45:00 jck Fixed lurking bug in the output of keyfile's header

 ************************************************************************/
/************************************************************************
  filelayout.c 9 replace /users/m2/dbup
  880426 12:01:05 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  filelayout.c 10 replace /users/jck/updates
  880505 23:01:15 jck Layout reader now checks that an item's table number is in bounds

 ************************************************************************/
/************************************************************************
  filelayout.c 11 replace /users/jck/dbupdate
  891009 12:55:09 jck Standardizer updates

 ************************************************************************/

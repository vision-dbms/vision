#ifndef FILELAYOUT_H
#define FILELAYOUT_H

/**************************************
 *****  Implementation Constants  *****
 **************************************/

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#define MAX_ITEMS	16000
#define MAX_FILES	1024
#define MAX_KEYS	10
#define MAX_SELECTS	10
#define MAX_VAL		31	
#define WRITE_BUF_SIZE 10000          
#define HEADERSIZE	0
#define MAX_PERIOD_W	10


/************************
 *****  FILE_TABLE  *****
 ************************/

union BUFFER_POINTER {
    char  *char_ptr;
    int	  *int_ptr;
    float *flt_ptr;
    double *dbl_ptr;
};
    
struct OUTFILE {
    unsigned int type;
    char key;
    int granule;
    int columns;
    int header;
    int fildes;
    int item_list[MAX_ITEMS];
    BUFFER_POINTER bp;
    char buffer[WRITE_BUF_SIZE];
};

#define OUT_type(f)	    ((f)->type)
#define OUT_key(f)	    ((f)->key)
#define OUT_granule(f)	    ((f)->granule)
#define OUT_itemCount(f)    ((f)->columns)
#define OUT_header(f)	    ((f)->header)
#define OUT_fd(f)	    ((f)->fildes)
#define OUT_item(f, n)	    ((f)->item_list[n])
#define OUT_floatPtr(f)	    ((f)->bp.flt_ptr)
#define OUT_doublePtr(f)    ((f)->bp.dbl_ptr)
#define OUT_charPtr(f)	    ((f)->bp.char_ptr)
#define OUT_intPtr(f)	    ((f)->bp.int_ptr)
#define OUT_buffer(f)	    ((f)->buffer)


/*************************
 *****  ITEM_LAYOUT  *****
 *************************/
#define ITEM_DESC_LEN 64
struct ITEM_LAYOUT {
    char item_code[MAX_VAL];         /* = v+item num */
    int start_char_pos;              /* = start pos'n 1 relative */  
    int num_chars;		     /* = num of chars in item */
    int out_chars;		     /* = output chars for strings */
    int input_decs;		     /* = num of decimals */
    double dec_divisor;
    unsigned int item_type;	     /* a=alpha, i=integer, d=decimal */
    char item_desc[ITEM_DESC_LEN + 1]; /* = item text descr */
    OUTFILE *file;		     /* pointer to ftable for item */
    int key;                         /* number of correct key file */
    int width;			     /* repitition width for compustat, or
					calc function index for preproc1 */
};

#define ALPHA	    0x0000
#define INTEGER	    0x0100
#define DOUBLE	    0x0200
#define BACKPTR	    0x0300
#define CODE	    0x0400
#define DECIMAL	    0x0500

/*---------------------------------------------------------------------------
 *  The following macro removes the extension optionally appended to all
 *  type codes to allow them to be partitioned into multiple files.  It
 *  replaces the original partitioning that only worked for the single
 *  precision float (DECIMAL) data types.
 *---------------------------------------------------------------------------
 */
#define UnpartitionedType(type) ((type) & 0xffffff00)

#define ITEM_code(item)		    (item->item_code)
#define ITEM_desc(item)		    (item->item_desc)
#define ITEM_type(item)		    (item->item_type)
#define ITEM_pos(item)		    (item->start_char_pos)
#define ITEM_len(item)		    (item->num_chars)
#define ITEM_out(item)		    (item->out_chars)
#define ITEM_inputdecs(item)	    (item->input_decs)
#define ITEM_decs(item)		    (item->dec_divisor)
#define ITEM_file(item)		    (item->file)
#define ITEM_key(item)		    (item->key)
#define ITEM_width(item)	    (item->width)
#define ITEM_calc(item)		    (item->width)


/*****************************************
 **********	KEY_LAYOUT	**********
 *****************************************/

struct KEY_LAYOUT {
    int id_pos;
    int id_len;
    int period_pos;
    int period_len;
    int est_rows;
    int count;
    int spec_fn;
    char *bufptr;
    char buffer[WRITE_BUF_SIZE];
    int fd;
};

#define KEY_idPos(k)	    ((k)->id_pos)
#define KEY_idLen(k)	    ((k)->id_len)
#define KEY_periodPos(k)    ((k)->period_pos)
#define KEY_periodLen(k)    ((k)->period_len)
#define KEY_specFn(k)	    ((k)->spec_fn)
#define KEY_bufPtr(k)	    ((k)->bufptr)
#define KEY_buffer(k)	    ((k)->buffer)
#define KEY_fd(k)	    ((k)->fd)
#define KEY_estRows(k)	    ((k)->est_rows)
#define KEY_count(k)	    ((k)->count)


/***************************
 *****  SELECT_LAYOUT  *****
 ***************************/

struct SELECT_LAYOUT {
    int pos;
    int len;
    char value[MAX_VAL * sizeof (char *)];	/* originally misdeclared as */
						/* a 'char *value[MAX_VAL]', */
						/* this bizarre declaration  */
						/* preserves the original    */
						/* size in case it's been    */
						/* needed all along.         */
};

#define SEL_pos(s)	    ((s)->pos)
#define SEL_len(s)	    ((s)->len)
#define SEL_value(s)	    ((s)->value)


/*************************
 *****  FILE_LAYOUT  *****
 *************************/

struct FILE_LAYOUT {
    char name[PATH_MAX];
    int rec_len;
    int block_len;
    int ebcdic;
    int headers;
    int date_fn;
    int keys;
    KEY_LAYOUT	    *key[MAX_KEYS];
    int selects;
    SELECT_LAYOUT   *select[MAX_SELECTS];
    int items;
    ITEM_LAYOUT	    *item[MAX_ITEMS];
    int files;
    OUTFILE	    *file[MAX_FILES];
};

#define FILE_prefix(f)	    ((f)->name)
#define FILE_recLen(f)	    ((f)->rec_len)
#define FILE_blkLen(f)	    ((f)->block_len)
#define FILE_ebcdic(f)	    ((f)->ebcdic)
#define FILE_headers(f)	    ((f)->headers)
#define FILE_keyCount(f)    ((f)->keys)
#define FILE_key(f, n)	    ((f)->key[n])
#define FILE_selectCount(f) ((f)->selects)
#define FILE_select(f, n)   ((f)->select[n])
#define FILE_dateFn(f)	    ((f)->date_fn)
#define FILE_itemCount(f)   ((f)->items)
#define FILE_item(f, n)	    ((f)->item[n])
#define FILE_fileCount(f)   ((f)->files)
#define FILE_outfile(f, n)  ((f)->file[n])


/************************
 *****  Operations  *****
 ************************/

PublicFnDecl FILE_LAYOUT *file_layout (
    char const*			filename
);

PublicFnDecl int numPreallocBytes (
    FILE_LAYOUT *		layout,
    OUTFILE *			file
);

PublicFnDecl void flush_files (
    FILE_LAYOUT *		layout
);

PublicFnDecl int select_record (
    char *			record,
    FILE_LAYOUT *		layout
);

PublicFnDecl void close_files (
    FILE_LAYOUT *		layout
);

PublicFnDecl void open_keys (
    FILE_LAYOUT *		layout
);

PublicFnDecl void write_keys (
    char *			record,
    FILE_LAYOUT *		layout,
    void			(*keyFunc[])(KEY_LAYOUT *, char *)
);

PublicFnDecl void close_keys (
    FILE_LAYOUT *		layout
);

PublicFnDecl void write_layouts (
    FILE_LAYOUT *		layout
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  filelayout.h 1 replace /users/ees/dbutil
  861030 12:03:19 ees preprocessor rountine decl's

 ************************************************************************/
/************************************************************************
  filelayout.h 2 replace /users/ees/dbutil
  861031 13:14:09 ees added fatal_error()

 ************************************************************************/
/************************************************************************
  filelayout.h 3 replace /users/ees/dbutil
  861113 10:12:17 ees  removed process_record() decl

 ************************************************************************/
/************************************************************************
  filelayout.h 4 replace /users/m2/dbup
  880426 12:03:20 m2 Apollo and VAX port

 ************************************************************************/

/*********************************************************************
********    shuffle.c					**************
********    shuffle pieces  of transposed square        **************
********    matrices produced by transpose.c            **************
*********************************************************************/

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

#include "VkMemory.h"

#include "gopt.h"
#include "readlayout.h"
#include "sigHandler.h"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

#define MAX_BUF_SIZE 524268

PrivateVarDef LAYOUT		*layout;

PrivateVarDef int		rows,
				row_size,
				buffercount,
				rows_in_buf,
				buffersize;

/***** Each buffer contains multiple rows, buf_ptr is an array of pointers
	to rows that spans all buffers, free_buf is a list of free buffers,
	free_indx is the index of the last free buffer in the list *****/

PrivateVarDef char		*buffer[10],
				*temp_buf,
				**buf_ptr;
PrivateVarDef int		*free_buf,
				free_indx = -1;


/********************************************************************/
PrivateFnDef int create_buffers (
    size_t			filesize
)
{
    int i, j, k, size;

/**** each matrix has LAY_itemCount(layout) number of transposable elements (rows) ****/

    rows = LAY_matrixCount(layout) * LAY_itemCount(layout);
    row_size = LAY_itemCount(layout) * LAY_granule(layout);

/***** set up buffer and buffer pointers *****/
    rows_in_buf = MAX_BUF_SIZE / row_size > rows
    ? rows
    : MAX_BUF_SIZE / row_size;

    buffersize = rows_in_buf * row_size;

    if (buffercount == 0)
    {
        buffercount = (filesize / MAX_BUF_SIZE) + 1;
	if (buffercount > 10)
	    buffercount = 10;
    }

    temp_buf = (char*)malloc(row_size);

    buf_ptr = (char **) malloc(rows_in_buf * buffercount * sizeof(char *));

    size = (rows_in_buf * buffercount * sizeof(int));
    if (IsNil (free_buf = (int *) malloc (size)))
    {
        display_error("Memallc Error");
	return(1);
    }

    for (i = 0; i < buffercount; i++)
    {
	buffer[i] = (char*)malloc (buffersize);
	if (IsNil (buffer[i]))
	{
	    display_error("Buffer Creation");
	    return(1);
	}

	k = 0;

	for ( j = (i * rows_in_buf); j < ((i + 1) * rows_in_buf); j++)
	{
	    buf_ptr[j] = buffer[i] + (k * row_size);
	    free_buf[j] = j;
	    free_indx++;
	    k++;
	}

    }

    return(0);
}


/********************************************************************/
PrivateFnDef int shuffle_file (char const* filename) {
/***** For each row in the file there is a status indicator
       and an index pointing to a buffer row (if the correct row is
       present in the buffer) *****/

    char *status, *t_1;    /**** n = not processed, v = vacated, d = done *****/

    int *right_buf, *t_2,  /**** -1 = not in buffer, n = number of buf_ptr *****/
	*offset, *t_3,
	*right_n, *t_4;

    int passes, donecnt, n,
	complete_cols = 0,
	not_done = 1,
	ok_so_far = 1,
	first_bad_guy = 0;

    VkMemory mapping;
    char *file_ptr;

    if (!mapping.Map (filename, false, VkMemory::Share_Private)) {
        display_error("File Mapping Error");
	return(1);
    }
    file_ptr = (char*)mapping.RegionAddress ();

    if (create_buffers (mapping.RegionSize ())) {
	mapping.Destroy ();
	return 1;
    }

    if (IsNil (status = (char*)malloc (rows * sizeof(char)))) {
	display_error("Memallc Error 1");
	return(1);
    }
    t_1 = status;

    if (IsNil (right_buf = (int *) malloc (rows * sizeof(int)))) {
	display_error("Memallc Error 2");
	return(1);
    }
    t_2 = right_buf;

    if (IsNil (offset = (int *) malloc (rows * sizeof(int))))
    {
	display_error("Memallc Error 3");
	return(1);
    }
    t_3 = offset;

    if (IsNil (right_n = (int *) malloc (rows * sizeof(int))))
    {
	display_error("Memallc Error 4");
	return(1);
    }
    t_4 = right_n;

    for (n = 0; n < rows; n++)
    {
	right_buf[n] = -1;
	offset[n] = (row_size * n) + LAY_header(layout);
	right_n[n] = complete_cols *
		    (rows/LAY_itemCount(layout)) + (n/LAY_itemCount(layout));
	if (right_n[n] == n)
	    status[n] = 'd';
	else
	    status[n] = 'n';
	complete_cols = (complete_cols + 1) % LAY_itemCount(layout);
    }

    n = passes = 0;

    donecnt = 0;

    while (not_done) {

/****** not processed *****/
	if (status[n] == 'n')
	{
	    if (right_buf[n] != -1)
	    {
	        memcpy(temp_buf, (file_ptr + offset[n]), row_size);
		memcpy((file_ptr + offset[n]),
				 buf_ptr[right_buf[n]], row_size);
	        memcpy(buf_ptr[right_buf[n]], temp_buf, row_size);
		right_buf[right_n[n]] = right_buf[n];
		status[n] = 'd';
		donecnt++;

	    }

	    else
	    {
		if (ok_so_far)
		{
		    ok_so_far = false;
		    first_bad_guy = n;
		}

		if (free_indx >= 0)
		{
		    ;
		    memcpy(buf_ptr[free_buf[free_indx]],
			     (file_ptr + offset[n]), row_size);
		    status[n] = 'v';
		    right_buf[right_n[n]] = free_buf[free_indx--];
		}
	    }
	}
/***** vacated  and correct contents in buffer *****/
	else
	if ((status[n] == 'v') && (right_buf[n] != -1))
	{
	    memcpy((file_ptr + offset[n]), buf_ptr[right_buf[n]], row_size);
	    free_buf[++free_indx] = right_buf[n];
	    status[n] = 'd';
	    donecnt++;

	}
	else
/***** vacated  and correct contents not in buffer *****/
	if ((status[n] == 'v') && ok_so_far)
	{
	    ok_so_far = false;
	    first_bad_guy = n;
	}


        n++;

	if (n == rows)
	{
	    if (ok_so_far)
	        not_done = false;
	    else
	    {
		n = first_bad_guy;
		ok_so_far = true;
	    }
	}

    }

    free(t_1);
    free(t_2);
    free(t_3);
    free(t_4);

    mapping.Destroy ();

    return(0);

}

GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription	    ("OPTION_A", 'b', NilOf (char *), "0")
GOPT_EndOptionDescriptions;


/********************************************************************/
int main (
    int				argc,
    char *			argv[]
)
{
    set_signals (handle_signal);

    GOPT_AcquireOptions(argc, argv);
    buffercount = atoi(GOPT_GetValueOption("OPTION_A"));
    GOPT_SeekExtraArgument(0, 0);
    char const *filename = GOPT_GetExtraArgument();

/*****  ... Global Reclamation Manager Initialization, ...  *****/
    VkMemory::StartGRM (argv[0]);

    if (IsNil (layout = read_layout(filename))) {
        display_error("Error reading layout file");
	return ErrorExitValue;
    }

    if (1 == LAY_itemCount (layout))
	return NormalExitValue;

    return shuffle_file (filename) ? ErrorExitValue : NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  shuffle.c 1 replace /usr/dbs/utils
  860527 17:25:40 ees adding file shuffle utility: ees 5-27-86

 ************************************************************************/
/************************************************************************
  shuffle.c 2 replace /users/ees/dbutil
  860620 16:45:40 ees changed signals caught

 ************************************************************************/
/************************************************************************
  shuffle.c 3 replace /users/ees/dbutil
  861009 15:21:59 ees removed interrupt signal

 ************************************************************************/
/************************************************************************
  shuffle.c 4 replace /users/ees/dbutil
  861017 12:20:51 ees removed signal handler

 ************************************************************************/
/************************************************************************
  shuffle.c 5 replace /users/jck/updates
  861103 09:04:21 jck Modified signal handling organization

 ************************************************************************/
/************************************************************************
  shuffle.c 6 replace /users/jck/updates
  861219 11:15:17 jck Paging Added

 ************************************************************************/
/************************************************************************
  shuffle.c 7 replace /users/jck/updates
  870206 15:13:22 jck Fixed a bug in free_buf allocator

 ************************************************************************/
/************************************************************************
  shuffle.c 8 replace /users/jck/updates
  871117 10:49:59 jck Enabled single column files to be true no-ops

 ************************************************************************/
/************************************************************************
  shuffle.c 9 replace /users/m2/dbup
  880426 12:23:32 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  shuffle.c 10 replace /users/jck/dbupdate
  891006 14:48:59 jck Added STD exit codes. Adjusted memallc interface

 ************************************************************************/

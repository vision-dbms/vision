/**************************************************************
************		transpose.c           *****************
****	transposes square matrixes of a file in place    ******
**************************************************************/

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

#include "readlayout.h"
#include "sigHandler.h"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

PrivateVarDef LAYOUT *layout;

PrivateFnDef void transpose_string (
    char *			matrix
)
{
    char *temp;

    long int   row_size, m, i1, i2, i1min, i1max;

    row_size = (LAY_itemCount(layout) * LAY_granule(layout));

    m = (row_size * (LAY_itemCount(layout) - 1));

    i1min = LAY_granule(layout);

    temp = (char*)malloc(LAY_granule(layout));

    for (i1max = row_size; i1max <= m; i1max += row_size)
     {

        i2 = i1min + row_size - LAY_granule(layout);

	for (i1 = i1min; i1 < i1max; i1 += LAY_granule(layout))
	{
	    memcpy(temp, &matrix[i1], LAY_granule(layout));
	    memcpy(&matrix[i1], &matrix[i2], LAY_granule(layout));
	    memcpy(&matrix[i2], temp, LAY_granule(layout));
	    i2 += row_size;
	}
	i1min = i1min + row_size + LAY_granule(layout);
     }

}


PrivateFnDef void transpose_int (
    int *			matrix
)
{
    int temp;
    long int  m, i1, i2, i1min, i1max;

    m = (LAY_itemCount(layout) * LAY_itemCount(layout) - 1);

    i1min = 1;

    for (i1max = LAY_itemCount(layout); i1max <= m; i1max += LAY_itemCount(layout))
     {

        i2 = i1min + LAY_itemCount(layout) - 1;
	for (i1 = i1min; i1 < i1max; i1++)
	{
	    temp = matrix[i1];
	    matrix[i1] = matrix[i2];
	    matrix[i2] = temp;
	    i2 += LAY_itemCount(layout);
	}
	i1min = i1min + LAY_itemCount(layout) + 1;
     }

}


PrivateFnDef void transpose_double (
    double *			matrix
)
{
    double temp;
    long int  m, i1, i2, i1min, i1max;

    m = (LAY_itemCount(layout) * LAY_itemCount(layout) - 1);

    i1min = 1;

    for (i1max = LAY_itemCount(layout); i1max <= m; i1max += LAY_itemCount(layout))
     {

        i2 = i1min + LAY_itemCount(layout) - 1;
	for (i1 = i1min; i1 < i1max; i1++)
	{
	    temp = matrix[i1];
	    matrix[i1] = matrix[i2];
	    matrix[i2] = temp;
	    i2 += LAY_itemCount(layout);
	}
	i1min = i1min + LAY_itemCount(layout) + 1;
     }

}


int main (
    int				argc,
    char *			argv[]
)
{
    char *			filename;
    VkMemory			mapping;
    char *			matrix_ptr;
    int				i;

    set_signals (handle_signal);

    if (argc < 2)
    {
	display_error ("No Argument");
	return ErrorExitValue;
    }

    filename = argv[1];

    if (IsNil (layout = read_layout(filename))) {
	display_error("Read Layout");
	return ErrorExitValue;
    }

    if (1 == LAY_itemCount (layout))
	return NormalExitValue;

/*****  ... Global Reclamation Manager Initialization, ...  *****/
    VkMemory::StartGRM (argv[0]);

    if (!mapping.Map (filename, 0, LAY_fileSize (layout), VkMemory::Share_Private)) {
	display_error("File Mapping Error");
	return ErrorExitValue;
    }

    matrix_ptr = (char*)mapping.RegionAddress ();
    matrix_ptr += LAY_header(layout);

    switch (LAY_granule (layout)) {
    case 4:
	for (i = 1; i <= LAY_matrixCount(layout); i++) {
	    transpose_int((int *)matrix_ptr);
	    matrix_ptr += LAY_matrixSize(layout);
	}
        break;
    case 8:
	for (i = 1; i <= LAY_matrixCount(layout); i++) {
	    transpose_double((double *)matrix_ptr);
	    matrix_ptr += LAY_matrixSize(layout);
	}
        break;
    default:
	for (i = 1; i <= LAY_matrixCount(layout); i++) {
	    transpose_string(matrix_ptr);
	    matrix_ptr += LAY_matrixSize(layout);
	}
        break;
    }

    mapping.Destroy ();

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  transpose.c 1 replace /usr/dbs/utils
  860527 17:24:54 ees adding file transpose utility: ees 5-27-86

 ************************************************************************/
/************************************************************************
  transpose.c 2 replace /users/ees/dbutil
  860620 16:45:25 ees changed signals caught

 ************************************************************************/
/************************************************************************
  transpose.c 3 replace /users/ees/dbutil
  861009 15:22:24 ees removed interrupt signal

 ************************************************************************/
/************************************************************************
  transpose.c 4 replace /users/ees/dbutil
  861017 12:20:32 ees removed signal handler

 ************************************************************************/
/************************************************************************
  transpose.c 5 replace /users/jck/updates
  861103 09:04:27 jck Modified signal handling organization

 ************************************************************************/
/************************************************************************
  transpose.c 6 replace /users/jck/updates
  861219 10:59:07 jck Capacity to deal with double precision numbers added

 ************************************************************************/
/************************************************************************
  transpose.c 7 replace /users/lis/updates
  870121 14:11:06 lis Replace float types with integer types

 ************************************************************************/
/************************************************************************
  transpose.c 8 replace /users/jck/updates
  870824 14:51:13 jck Circumvented a memallc bug

 ************************************************************************/
/************************************************************************
  transpose.c 9 replace /users/jck/updates
  871117 10:49:52 jck Enabled single column files to be true no-ops

 ************************************************************************/
/************************************************************************
  transpose.c 10 replace /users/m2/dbup
  880426 12:24:56 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  transpose.c 11 replace /users/jck/dbupdate
  891006 14:45:12 jck Added STD exit codes. Adjusted memallc interface

 ************************************************************************/

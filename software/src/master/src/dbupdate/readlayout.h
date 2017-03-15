#ifndef READLAYOUT_H
#define READLAYOUT_H

/*********************************************************
 ***************     readlayout.h    *********************
 *********************************************************/

#define LAY_IntegerNA	INT_MAX
#define LAY_FloatNA	FLT_MAX
#define LAY_DoubleNA	DBL_MAX

#define LAY_MaxItems	16000
#define LAY_string	'a'
#define LAY_code	'c'
#define LAY_float	'd'
#define LAY_double	'D'
#define LAY_integer	'i'

struct LAYOUT {
    char type;			    /**** item type (a, d or i) *****/
    int key;			    /**** number of key file	*****/
    int granule;		    /**** item size             *****/
    int keys;			    /**** number of observations ****/
    int items;			    /**** number of items	 ****/
    char *itemList[LAY_MaxItems];
    int header;			    /**** size of file header    ****/
    int matrices;		    /**** matrices to transpose  ****/
    int matrix_size;		    /**** matrix size		 ****/
    int total_size;		    /**** total size of file     ****/
    int vector_size;
};

#define LAY_type(s)		    (s->type)
#define LAY_key(s)		    (s->key)
#define LAY_granule(s)		    (s->granule)
#define LAY_keyCount(s)		    (s->keys)
#define LAY_itemCount(s)	    (s->items)
#define LAY_header(s)		    (s->header)
#define LAY_matrixCount(s)	    (s->matrices)
#define LAY_matrixSize(s)	    (s->matrix_size)
#define LAY_fileSize(s)		    (s->total_size)
#define LAY_item(s, n)		    (s->itemList[n])
#define LAY_vectorSize(s)	    (s->vector_size)

PublicFnDecl LAYOUT *read_layout(
    char const*			fname
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  readlayout.h 1 replace /users/ees/dbutil
  860527 18:47:50 ees adding readlayout.h: ees 5-27-86

 ************************************************************************/
/************************************************************************
  readlayout.h 2 replace /users/ees/dbutil
  860620 16:15:04 ees added items

 ************************************************************************/
/************************************************************************
  readlayout.h 3 replace /users/jck/system/updates/compQprices
  861010 08:02:03 jck added declaration of read_layout function

 ************************************************************************/
/************************************************************************
  readlayout.h 4 replace /users/jck/updates
  861219 10:59:04 jck Capacity to deal with double precision numbers added

 ************************************************************************/
/************************************************************************
  readlayout.h 5 replace /users/jck/updates
  870219 07:24:16 jck Code Type Tree added

 ************************************************************************/
/************************************************************************
  readlayout.h 6 replace /users/lis/updates
  870626 15:51:47 lis max items increased for valueline

 ************************************************************************/

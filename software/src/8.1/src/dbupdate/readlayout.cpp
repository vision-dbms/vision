#include "Vk.h"

#include "readlayout.h"

#define MAXLINE 100

/**************************
*
*	Arguments:	data file name 
*				(used to construct layout filename)
*
*	Returns:	LAYOUT pointer or NULL
*
**************************/
PublicFnDef LAYOUT *read_layout (
    char const*			fname
)
{
    LAYOUT *lay;
    char layout_file[80], 
	 *ptr,
	 layout_entry[MAXLINE];
    int i, k;      
    FILE *file_ptr;

    lay = (LAYOUT *)malloc(sizeof(LAYOUT));
    if (lay == NULL)
        return NULL;
	
    strcpy(layout_file, fname);
    ptr = strchr(layout_file, '.');
    strcpy(ptr, ".lay");
    file_ptr = fopen(layout_file, "r");
    if (file_ptr == NULL)
	return(NULL);
    fgets(layout_entry, MAXLINE, file_ptr);
    sscanf(layout_entry, "%c %d %d %d %d %d",  &LAY_type(lay), 
					    &LAY_key(lay), 
					    &LAY_granule(lay), 
					    &LAY_keyCount(lay), 
					    &LAY_itemCount(lay), 
					    &LAY_header(lay));

    LAY_matrixSize(lay) =
	 LAY_itemCount(lay) * LAY_itemCount(lay) * LAY_granule(lay);
    LAY_matrixCount(lay) = (LAY_keyCount(lay) / LAY_itemCount(lay)) + 1;
    LAY_fileSize(lay) =
	 (LAY_matrixCount(lay) * LAY_matrixSize(lay)) + LAY_header(lay);
    LAY_vectorSize(lay) = 
	 LAY_itemCount(lay) * LAY_granule(lay) * LAY_matrixCount(lay);

    i = 0;
    while (fgets(layout_entry, MAXLINE, file_ptr))    
    {
	LAY_item(lay, i) = (char*)malloc(strlen(layout_entry));
	sscanf(layout_entry, "%d %[^\n]", &k, LAY_item(lay, i));
	i++;
    }

    fclose(file_ptr);
    return(lay);
    
}	   


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  readlayout.c 1 replace /users/ees/dbutil
  860527 18:47:08 ees adding readlayout.c: ees 5-27-86

 ************************************************************************/
/************************************************************************
  readlayout.c 2 replace /users/ees/dbutil
  860620 16:14:47 ees added items

 ************************************************************************/
/************************************************************************
  readlayout.c 3 replace /users/ees/dbutil
  861030 13:35:42 ees added stdefs.h

 ************************************************************************/

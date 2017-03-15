/*****  List Index Representation Type Shared Declarations  *****/
#ifndef rtINDEX_D
#define rtINDEX_D

/******************************
 *****  Index Definition  *****
 ******************************/
/*---------------------------------------------------------------------------
 * List Index Field Descriptions:
 *	lists			- the POP of an L-Store containing the indexed
 *				  lists.
 *	keyStore		- the POP of the store associated with the key
 *				  values held in this index.
 *	keyValues		- the POP of an ORDERED SET containing the key
 *				  values used to index the elements of 'lists'.
 *	map			- the POP of a link containing a mapping from
 *				  the elements in 'lists' to the cartesian
 *				  product of 'lists' and 'keyValues'.
 *---------------------------------------------------------------------------
 */
struct rtINDEX_Index {
    M_POP
    	lists,
	keyStore,
	keyValues,
	map;
};

/*****  Access Macros  *****/
#define rtINDEX_Index_Lists(index)\
    (index).lists

#define rtINDEX_Index_KeyStore(index)\
    (index).keyStore

#define rtINDEX_Index_KeyValues(index)\
    (index).keyValues

#define rtINDEX_Index_Map(index)\
    (index).map

#define rtINDEX_SizeofIndex()\
    (sizeof rtINDEX_Index)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTindex.d 1 replace /users/mjc/system
  870109 10:26:42 mjc Release 'index' representation type shell

 ************************************************************************/
/************************************************************************
  RTindex.d 2 replace /users/mjc/system
  870330 19:07:10 mjc Release for ongoing work by 'jad'

 ************************************************************************/
/************************************************************************
  RTindex.d 3 replace /users/jad/system
  870414 16:01:26 jad implemented basics, key routines, & Define and
Lookup routines.

 ************************************************************************/
/************************************************************************
  RTindex.d 4 replace /users/mjc/translation
  870524 09:33:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTindex.d 5 replace /users/jck/system
  870605 16:10:32 jck Generalized handling to include vectors as one choice for an indexed list's index

 ************************************************************************/

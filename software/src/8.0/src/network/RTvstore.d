/*****  Value Store Representation Type Shared Declarations  *****/
#ifndef rtVSTORE_D
#define rtVSTORE_D

/********************************
 *****  Value Store Format  *****
 ********************************/
/*---------------------------------------------------------------------------
 * Value Store Field Descriptions:
 *	rowPToken		- a P-Token defining the set of rows contained
 *				  in this value store.
 *	inheritanceStore	- an optional store which is the next store to
 *				  search for names not defined in this value
 *				  store.
 *	inheritancePointer	- an optional link or u-vector identifying the
 *				  rows in 'inheritanceStore' corresponding to
 *				  the rows of this value store.  This field
 *				  must be specified if 'inheritanceStore' is
 *				  present.  If 'inheritanceStore' is absent,
 *				  this slot is ignored.  The link or u-vector
 *				  in this slot must positionally correspond to
 *				  'rowPToken'.
 *	columnPToken		- a P-Token defining the set of columns
 *				  contained in this value store.
 *	methodDictionary	- a method dictionary defining the columns of
 *				  this value store.
 *	auxillaryColumn		- an optional object used by certain classes to
 *				  contain additional information associated
 *				  with the rows of this value store.  This
 *				  object is accessible to primitive methods
 *				  only.
 *	columnArray		- an array of physical objects representing the
 *				  information stored in the columns of this
 *				  value store.  The number of elements in the
 *				  column array is determined at run time.
 *---------------------------------------------------------------------------
 */
struct rtVSTORE_Type {
    M_POP
	rowPToken,
	inheritanceStore,
	inheritancePointer,
	columnPToken,
	methodDictionary,
	auxillaryColumn,
	columnArray[1];
};


/*****  Access Macros  *****/
#define rtVSTORE_VS_RowPToken(vsp)\
    (vsp)->rowPToken

#define rtVSTORE_VS_InheritanceStore(vsp)\
    (vsp)->inheritanceStore

#define rtVSTORE_VS_InheritancePointer(vsp)\
    (vsp)->inheritancePointer

#define rtVSTORE_VS_ColumnPToken(vsp)\
    (vsp)->columnPToken

#define rtVSTORE_VS_MD(vsp)\
    (vsp)->methodDictionary

#define rtVSTORE_VS_AuxillaryColumn(vsp)\
    (vsp)->auxillaryColumn

#define rtVSTORE_VS_ColumnArray(vsp)\
    (vsp)->columnArray

/*****  Value Store Sizing Macro  *****/
#define rtVSTORE_SizeofVStore(columnCount) (\
    (sizeof rtVSTORE_Type) + sizeof (M_POP) * ((columnCount) - 1)\
)


/********************************************
 *****  DataBase Update Utility Macros  *****
 ********************************************/

#define rtVSTORE_Content(preamblePtr)\
    ((rtVSTORE_Type *)((char *)(preamblePtr) + \
			M_SizeOfPreambleType + \
			sizeof (int)))	/**** popvector element count ****/

#define rtVSTORE_RowPToken(preamblePtr)\
    rtVSTORE_VS_RowPToken \
	    (rtVSTORE_Content (preamblePtr))

#define rtVSTORE_ReplaceRowPToken(preamblePtr, ptokenPOP)\
    (M_POP_D_AsInt \
        (rtVSTORE_RowPToken (preamblePtr)) = ptokenPOP)

#define rtVSTORE_ContainerSize(preamblePtr)\
    (M_SizeOfPreambleType + \
     M_CPreamble_Size ((M_CPreamble*)(preamblePtr)) + \
     M_SizeOfEndMarker)

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTvstore.d 1 replace /users/mjc/system
  860404 20:15:18 mjc Create Value Store Representation Type

 ************************************************************************/
/************************************************************************
  RTvstore.d 2 replace /users/mjc/system
  860523 01:32:09 mjc Return to library for completion by 'jck'

 ************************************************************************/
/************************************************************************
  RTvstore.d 3 replace /users/jck/system
  860530 10:44:45 jck Release partial implementation for mjc

 ************************************************************************/
/************************************************************************
  RTvstore.d 4 replace /users/jck/system
  860605 16:22:25 jck Creation and Search operations functional

 ************************************************************************/
/************************************************************************
  RTvstore.d 5 replace /users/jck/system
  860611 11:12:06 jck AppendRows and Align routines implemented

 ************************************************************************/
/************************************************************************
  RTvstore.d 6 replace /users/mjc/system
  860623 10:23:42 mjc Converted to use new value descriptor macros/R-Type

 ************************************************************************/
/************************************************************************
  RTvstore.d 7 replace /users/mjc/system
  860709 10:18:55 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTvstore.d 8 replace /users/mjc/system
  860713 17:14:02 mjc Release new version of 'rtVSTORE_AddRows'

 ************************************************************************/
/************************************************************************
  RTvstore.d 9 replace /users/mjc/system
  861002 18:02:40 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  RTvstore.d 10 replace /users/jck/updates
  870129 13:58:44 jck Macros added for the incorporator

 ************************************************************************/
/************************************************************************
  RTvstore.d 11 replace /users/mjc/translation
  870524 09:36:38 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTvstore.d 12 replace /users/jck/updates
  870824 14:56:35 jck Added Macros for the incorporator's use

 ************************************************************************/
/************************************************************************
  RTvstore.d 13 replace /users/mjc/Software/system
  871208 15:35:19 mjc Fixed erroneous definition of 'rtVSTORE_CPD_ColumnCount'

 ************************************************************************/

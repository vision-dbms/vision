/******************************************************
 ************	    OSstring.c	    *******************
 ******************************************************/

/*****  System  *****/
#include "Vk.h"

/****** Local includes *****/
#include "sigHandler.h"
#include "m.d"
#include "ps.d"
#include "OSdump.h"
#include "uvector.d"
#include "RTlstore.d"
#include "RTptoken.d"

#include "OSstring.h"

PrivateVarDef Type_StringStore StringStore;


/****************************
 *****	TWALK routines	*****
 ****************************/

PrivateVarDef int ptrAdjustment;

/*****  Function passed to the HP_Unix provided twalk routine.
 *****  See documentation for tsearch(3C) in HP_Unix manual.
 *****/
PrivateFnDef void AdjustNodePtrs (
    struct treenode *		node,
    STD_VISIT			order,
    int				Unused(level)
)
{
    if (order == STD_preorder || order == STD_leaf)
	*((char **)&STD_tdata (node)) += ptrAdjustment;
}

/*****  Function passed to the HP_Unix provided twalk routine.
 *****  See documentation for tsearch(3C) in HP_Unix manual.
 *****/
PrivateFnDef void AdjustNodeStrings (
    struct treenode *		node,
    STD_VISIT			order,
    int				Unused(level)
)
{

    if (order == STD_preorder || order == STD_leaf)
        NodeString ((Type_NodePTR)STD_tdata (node)) += ptrAdjustment;
}


/***********************************************
 ****	String Store Management Routines    ****
 ***********************************************/

PrivateVarDef void AllocateStringStoreComponents (
    M_CPreamble*		lstore
)
{
/*****  First, allocate the breakpoint array ....  *****/
    BreakpointArraySize (StringStore) = V_Max (
	BreakpointInitialSize, 1 + (int) rtLSTORE_BreakpointCount (lstore)
    );

    if (NULL == (BreakpointArray (StringStore) =
		 BreakpointPtr (StringStore) =
		     (int *)malloc (BreakpointArraySize (StringStore)
							     * sizeof (int))))
    {
        display_error ("Error allocating Breakpoint Array");
	exit (ErrorExitValue);
    }

/*****  .... then, the binary tree ....  *****/
    if (NULL == (Tree (StringStore) =
		  (Type_NodePTR)malloc (BreakpointArraySize (StringStore)
						     * sizeof (Type_Node))))
    {
        display_error ("Error allocating Node Array");
	exit (ErrorExitValue);
    }
    Root (StringStore) = CodeRoot (StringStore) = NULL;

/*****  .... Finally,  allocate the content array.  *****/
    ContentArraySize (StringStore) = V_Max (ContentInitialSize,
					  rtLSTORE_StringContentSize (lstore));
    if (NULL == (ContentArray (StringStore) =
		ContentPtr (StringStore) =
		     (char *)malloc (ContentArraySize (StringStore))))
    {
        display_error ("Error allocating Content Array");
	exit (ErrorExitValue);
    }

}


/***************************************
 *****   Node Comparison Routine   *****
 ***************************************/

PrivateFnDef int __cdecl nodeCompare (
    Type_NodePTR		node1,
    Type_NodePTR		node2
)
{
    return  strcmp (NodeString (node1), NodeString (node2));
}


/*****************************
 *****  Public Routines  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to read an lstore from an object space and build a
 *****  binary tree to ease lookup of strings.
 *
 *  Arguments:
 *	fileprefix	-  A character string from which the filename
 *			   for the dump of the string store can be derived.
 *	lstorePToken	-  A pointer to a POP that this routine will initialize
 *			   to point to the lstore's updated ptoken.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef int osSTRING_ReadStringStore (
    char const*			fileprefix,
    M_POP*			lstorePToken
)
{
    char			filename [MAXPATHLENGTH],
				*contentPtr;
    M_CPreamble*		lstorePtr;
    osDUMP_VectorType		*lstore;
    Type_NodePTR		nodePtr;
    int				i;

    Valid (StringStore) = false;

    sprintf (filename, "%s.ss", fileprefix);
    if (osDUMP_ReadDump(filename))
	return true;

    lstorePtr = osDUMP_GetLStorePtr (&lstore, rtLSTORE_StringStoreItem);
    AllocateStringStoreComponents (lstorePtr);

/*****  Store Breakpoint Array  *****/
    memcpy (BreakpointArray (StringStore),
	    rtLSTORE_BreakpointArray (lstorePtr),
	    ((StringCount (StringStore) =
	        rtLSTORE_BreakpointCount (lstorePtr)) + 1) * sizeof (int));

/*****  Store Content Array  *****/
    contentPtr = (char *)osDUMP_GetUvectorPtr (lstore, 0);
    memcpy (ContentArray (StringStore),
	    UV_DataPtr (contentPtr),
	    UV_ElementCount (contentPtr));

/*****  Insert the strings in the binary tree  *****/
    for (i = 0,
         nodePtr = Tree (StringStore);

         i < StringCount (StringStore);

	 i++,
	 nodePtr++,
	 BreakpointPtr (StringStore)++,
	 ContentPtr (StringStore) = ContentArray (StringStore) +
				    *BreakpointPtr (StringStore))
    {
        NodeString (nodePtr) = ContentPtr (StringStore);
	NodeIndex (nodePtr) = i;
	STD_tsearch ((void *)nodePtr, (void **)&Root (StringStore), (VkComparator)nodeCompare);
    }

/*****
 *  Make new ptoken for LStore.  This new ptoken's POP will be used to
 *  'align' the alpha items that refer to the lstore.
 *****/
    M_POP_ContainerIndex (lstorePToken) =
    NewPTokenIndex	 (StringStore)  = (int)osDUMP_GetNewCTEntry ();
    M_POP_ObjectSpace (lstorePToken) = osDUMP_SpaceName;
    memcpy (&NewPTokenPOP (StringStore), lstorePToken, sizeof (M_POP));

    Valid (StringStore) = true;

    return false;
}


#define TrimTrailingSpaces(buffer, length)\
{\
    char *sptr = buffer, \
         *eptr;\
    \
    for (eptr = sptr + length;\
	 eptr > sptr && isspace (*(eptr - 1));\
	 eptr--);\
	 \
    length = eptr - sptr;\
    \
    sptr [length]  = '\0';\
}


/*---------------------------------------------------------------------------
 *****  Routine to find a string in the string store and add it if not found.
 *
 *  Arguments:
 *	string		-  The string to be added. (NOT NULL terminated)
 *	length		-  The number of characters in the string.
 *
 *  Returns:
 *	The index into the string store of this string.
 *
 *  Note: This routine makes use of a second root accessing an abbreviated set
 *    of nodes. It is intended for 'code' type strings, meaning strings that
 *    are repeated many times and are relatively few in number.  Compustat
 *    footnotes provide the classic example. This will result in a small tree
 *    that will provide fast access to these codes.
 *
 *****/
PublicFnDef int osSTRING_LocateOrAddCode (
    char const*			string,
    int				length
)
{
    char
        buffer[256];
    struct treenode
	*resultNode;
    Type_Node
	node;
    int
	index;

    memcpy (buffer, string, length);
    TrimTrailingSpaces (buffer, length);
    NodeString (&node) = buffer;
    if (resultNode = (struct treenode *)STD_tfind (
	    (void *)&node, (void **)&CodeRoot (StringStore), (VkComparator)nodeCompare
	)
    ) return NodeIndex ((Type_NodePTR)STD_tdata(resultNode));


/*****
 *  Not Found from CodeRoot, so locate or add it to main root and then
 *  add it to CodeRoot.
 *****/
    index = osSTRING_LocateOrAdd (string, length);
    STD_tsearch (
	(void *)&Leaf (StringStore, index), (void **)&CodeRoot (StringStore), (VkComparator)nodeCompare
    );
    return index;
}




/*---------------------------------------------------------------------------
 *****  Routine to find a string in the string store and add it if not found.
 *
 *  Arguments:
 *	string		-  The string to be added. (NOT NULL terminated)
 *	length		-  The number of characters in the string.
 *
 *  Returns:
 *	The index into the string store of this string.
 *
 *****/
PublicFnDef int osSTRING_LocateOrAdd (
    char const*			string,
    int				length
)
{
    struct treenode
	*resultNode;
    Type_NodePTR
        nodePtr;
    int
	*oldBPArray,
        count = StringCount(StringStore);
    char
	*oldTree,
	*oldContentArray;

    if (!Valid (StringStore))
    {
        display_error ("String Store not initialized");
	exit (ErrorExitValue);
    }

/*****
 *  Reallocate if necessary. Note that breakpoint array is one larger than
 *  the number of strings in the lstore.
 *****/
    if (count + 1 >= BreakpointArraySize (StringStore))
    {
        BreakpointArraySize (StringStore) += StandardIncrement;
	oldBPArray = BreakpointArray (StringStore);
	if (NULL == (BreakpointArray (StringStore) =
		   (int *)realloc (oldBPArray,
				   BreakpointArraySize (StringStore) *
					sizeof (int))))
	{
	    display_error ("Breakpoint Array Reallocation");
	    exit (ErrorExitValue);
	}
        BreakpointPtr (StringStore) +=
	    BreakpointArray (StringStore) - oldBPArray;

	oldTree = (char *)Tree (StringStore);
	if (NULL == (Tree (StringStore) =
		   (Type_NodePTR)realloc (Tree (StringStore),
					  BreakpointArraySize (StringStore) *
							sizeof (Type_Node))))
	{
	    display_error ("Node Reallocation");
	    exit (ErrorExitValue);
	}
	if (ptrAdjustment = (char *)Tree (StringStore) - oldTree)
	{
	    STD_twalk ((char *)Root (StringStore), AdjustNodePtrs);
	    STD_twalk ((char *)CodeRoot (StringStore), AdjustNodePtrs);
	}
    }


    if (ContentPtr (StringStore) - ContentArray (StringStore) + length + 1 >
        ContentArraySize (StringStore))
    {
        ContentArraySize (StringStore) += ContentIncrement;
	oldContentArray = ContentArray (StringStore);
	if (NULL == (ContentArray (StringStore) =
			  (char*)realloc (oldContentArray,
				   ContentArraySize (StringStore) )))
	{
	    display_error ("Content Array Reallocation");
	    exit (ErrorExitValue);
	}

	if (ptrAdjustment =ContentArray (StringStore) - oldContentArray)
	{
	    STD_twalk ((char *)Root (StringStore), AdjustNodeStrings);
	    ContentPtr (StringStore) += ptrAdjustment;
	}


    }

    nodePtr = Tree (StringStore) + count;
    NodeIndex (nodePtr) = StringCount (StringStore);
    NodeString (nodePtr) = ContentPtr (StringStore);
    memcpy (ContentPtr (StringStore), string, length);
    TrimTrailingSpaces (ContentPtr (StringStore), length);
    resultNode = (struct treenode *)STD_tsearch (
	(void *)nodePtr, (void **)&Root (StringStore), (VkComparator)nodeCompare
    );
    if (NodeIndex(nodePtr) == NodeIndex((Type_NodePTR)STD_tdata(resultNode)))
    {
        StringCount (StringStore)++;
	ContentPtr (StringStore) += length + 1;
	*++BreakpointPtr (StringStore) = ContentPtr (StringStore) -
					 ContentArray (StringStore);

    }
    return NodeIndex ((Type_NodePTR)STD_tdata (resultNode));

}


PublicFnDef int osSTRING_SizeOfStringStore (
    int *			contentSize
)
{
    if (!Valid (StringStore))
        return 0;

/***** First, make character array conform to 4 byte boundary constraint *****/
    *contentSize =
        ((ContentPtr (StringStore) -
		    ContentArray (StringStore) + 4) /4) * 4;

    return  rtLSTORE_Size (StringCount (StringStore)) +
		UV_Size (*contentSize) +
		rtPTOKEN_ContainerSize +
		rtPTOKEN_ContainerSize +
		rtPTOKEN_Size (1);
}


/*---------------------------------------------------------------------------
 *****  Routine to write out the updated string store to a space segment.
 *
 *  Arguments:
 *	fileptr		-  A pointer to the beginning of the mapped file
 *			   which will become an object space segment.
 *	endPtr		-  A pointer to the end of the same mapped file.
 *			   The string store will be written at this
 *			   location.
 *	filesize	-  The size in bytes of the mapped file. (Equal
 *			   to endPtr - fileptr).
 *	segment		-  The segment number that the string store is
 *			   being written into.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void osSTRING_WriteStringStore (
    char const*			fileprefix,
    char *			fileptr,
    char **			endPtr,
    long *			filesize,
    long			segment
)
{
    int				stringCount = StringCount (StringStore),
				elementCount,
				characterCount,
				contentSize,
				offset = (int)*filesize;
    char			filename [MAXPATHLENGTH];
    M_CPreamble*		inputPtr;
    osDUMP_VectorType		*lstore;
    rtPTOKEN_SDType		shiftDescriptor;

    if (!Valid (StringStore))
        return;

    sprintf (filename, "%s.ss", fileprefix);
    if (osDUMP_ReadDump(filename))
	exit (ErrorExitValue);

    inputPtr = osDUMP_GetLStorePtr (&lstore, rtLSTORE_StringStoreItem);
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, *filesize
    );

/***** Determine how much room will be needed  *****/
    *filesize += osSTRING_SizeOfStringStore (&contentSize);
    characterCount = ContentPtr (StringStore) - ContentArray (StringStore);

/*****  Write out the lstore  *****/
    memcpy(*endPtr, inputPtr, rtLSTORE_HeaderSize + M_SizeOfPreambleType);
    M_CPreamble_FixSize ((M_CPreamble*)(*endPtr));

    rtLSTORE_SetBreakpointCount (*endPtr, stringCount);
    rtLSTORE_SetContainerSize ((M_CPreamble*)(*endPtr));
    rtLSTORE_SetPTokenPOP (*endPtr,
			   M_POP_D_AsInt (NewPTokenPOP (StringStore)));
    *endPtr += rtLSTORE_HeaderSize + M_SizeOfPreambleType;
    memcpy (*endPtr, BreakpointArray (StringStore), (stringCount + 1) *
								sizeof (int));
    *endPtr += (stringCount + 1) * sizeof (int);
    memcpy (*endPtr, fileptr + offset, M_SizeOfEndMarker);
    *endPtr += M_SizeOfEndMarker;
    offset = *endPtr - fileptr;

/*****  Write out the LStore PTokens  *****/
    inputPtr = osDUMP_GetPtokenPtr (inputPtr);

/*****  First the old one  *****/
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    memcpy (*endPtr, inputPtr, rtPTOKEN_HeaderSize);
    M_CPreamble_FixSize ((M_CPreamble*)(*endPtr));

    rtPTOKEN_ConvertToShift (*endPtr, NewPTokenPOP (StringStore));
    rtPTOKEN_SetAltCount (*endPtr, 1);
    rtPTOKEN_SetContainerSize ((M_CPreamble*)(*endPtr));
    *endPtr += rtPTOKEN_HeaderSize;
    rtPTOKEN_SD_D_Origin (shiftDescriptor) = elementCount =
				rtPTOKEN_ElementCount (inputPtr);
    rtPTOKEN_SD_D_Shift (shiftDescriptor) = stringCount - elementCount;
    memcpy (*endPtr, &shiftDescriptor, sizeof (rtPTOKEN_SDType));
    *endPtr += sizeof (rtPTOKEN_SDType);
    memcpy (*endPtr, fileptr + offset, M_SizeOfEndMarker);
    *endPtr += M_SizeOfEndMarker;
    offset = *endPtr - fileptr;

/*****  Then the new one  *****/
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, NewPTokenIndex (StringStore), segment, offset
    );
    memcpy (*endPtr, inputPtr, rtPTOKEN_ContainerSize);
    M_CPreamble_FixSizes ((M_CPreamble*)(*endPtr));

    rtPTOKEN_SetElementCount (*endPtr, stringCount);
    M_POP_D_AsInt (
	M_CPreamble_POP ((M_CPreamble*)*endPtr)
    ) = M_POP_D_AsInt (NewPTokenPOP (StringStore));

    *endPtr += rtPTOKEN_ContainerSize;
    offset = *endPtr - fileptr;

/*****   Now, the content array  *****/
    inputPtr = osDUMP_GetUvectorPtr (lstore, 0);
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    memcpy (*endPtr, inputPtr, UV_HeaderSize + M_SizeOfPreambleType);
    M_CPreamble_FixSize ((M_CPreamble*)(*endPtr));

    UV_SetElementCount (*endPtr, characterCount);
    M_SetContainerSize (
	(M_CPreamble*)(*endPtr), UV_HeaderSize + contentSize
    );
    *endPtr += UV_HeaderSize +M_SizeOfPreambleType;
    memcpy (*endPtr, ContentArray (StringStore), characterCount);
    *endPtr += characterCount;
    memset (*endPtr, 0, contentSize - characterCount);
    *endPtr += (contentSize - characterCount);
    memcpy (*endPtr, fileptr + offset, M_SizeOfEndMarker);
    *endPtr += M_SizeOfEndMarker;
    offset = *endPtr - fileptr;

/*****  Finally the ContentArray's ptoken  *****/
    inputPtr = osDUMP_GetUsegPtokenPtr (lstore, 0);
    PS_CT_SetSegmentAndOffset (
	osDUMP_ContainerTable, osDUMP_CTIndex, segment, offset
    );
    memcpy (*endPtr, inputPtr, rtPTOKEN_ContainerSize);
    M_CPreamble_FixSizes ((M_CPreamble*)(*endPtr));

    rtPTOKEN_SetElementCount (*endPtr, characterCount);
    *endPtr += rtPTOKEN_ContainerSize;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  OSstring.c 1 replace /users/jck/updates
  870129 13:27:21 jck First release of the incorporator

 ************************************************************************/
/************************************************************************
  OSstring.c 2 replace /users/jck/updates
  870202 13:33:40 jck Fixed miscellaneous bugs

 ************************************************************************/
/************************************************************************
  OSstring.c 3 replace /users/jck/updates
  870219 07:24:07 jck Code Type Tree added

 ************************************************************************/
/************************************************************************
  OSstring.c 4 replace /users/jck/updates
  870513 14:49:53 jck Fixed the final writing of the string store's character uvector

 ************************************************************************/
/************************************************************************
  OSstring.c 5 replace /users/jck/updates
  870618 10:28:32 jck Shortened names

 ************************************************************************/
/************************************************************************
  OSstring.c 6 replace /users/jck/updates
  870629 14:06:55 jck Implemented processing of multiple prefixes; reimplemented
allocation scheme for final segment; integrated avl search routines

 ************************************************************************/
/************************************************************************
  OSstring.c 7 replace /users/jck/updates
  870824 14:53:39 jck Fortified the incorporator

 ************************************************************************/
/************************************************************************
  OSstring.c 8 replace /users/m2/dbup
  880426 11:55:00 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  OSstring.c 9 replace /users/jck/updates
  880628 08:58:32 jck Plugged another gap in the incorporator

 ************************************************************************/
/************************************************************************
  OSstring.c 10 replace /users/jck/dbupdate
  891031 14:34:30 jck Utilize standard exit values

 ************************************************************************/

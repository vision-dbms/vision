#ifndef osSTRING_H
#define osSTRING_H

/*************************
 *****  Node Format  *****
 *************************/
/*---------------------------------------------------------------------------
 * A node is defined as the building block of a binary tree that makes up the
 * lookup data structures of the string store.  Its two fields are defined as
 * follows:
 *
 *	string			- A pointer to a particular string in the
 * 				  content array of the string store.
 *	index			- An integer which identifies the string as
 * 				  the "n'th" one in the store.
 *---------------------------------------------------------------------------
 */

typedef struct Type_Node {
    char *string;
    int  index;
} *Type_NodePTR;

/*****  Access Macros  *****/
#define NodeString(n)	((n)->string)
#define NodeIndex(n)	((n)->index)


/*********************************
 *****  String Store Format  *****
 *********************************/
/*---------------------------------------------------------------------------
 * The string store is a structure designed to manage the updates of an Object
 * Space's string lstore.  Its fields are defined as follows:
 *
 *	breakpointArray		-  A pointer to the beginning of the lstore's
 * 				   breakpoint array.
 *	breakpointPtr		-  A pointer to the next free spot in the
 * 				   breakpoint array.  Subtracting
 * 				   breakpointArray from breakpointPtr gives
 * 				   the breakpoint count.
 *	breakpointArraySize	-  The allocated size of the breakpoint array.
 * 				   If the breakpoint count exceeds this, a
 * 				   reallocation becomes necessary.
 *	newPTokenIndex		-  The index into the object space's container
 * 				   table where the entry for the lstore's 
 * 				   updated ptoken can be found.
 *	stringCount		-  The number of strings in the store.
 *	contentArraySize	-  The allocated size for the content. If
 * 				   contentPtr - contentArray exceed this, a
 * 				   reallocation becomes necessary.
 *	contentArray		-  A pointer to the beginning of the lstore's
 * 				   content array.
 *	contentPtr		-  A pointer to the first free byte at the end
 * 				   of the content array.
 *	tree			-  A pointer to an array of nodes. There will
 * 				   be the same number of nodes as there are
 * 				   breakpoints and reallocations of the tree
 * 				   will be coincident with reallocations of
 * 				   the breakpoint array.
 *	root			-  A pointer to the root of the tree.
 *	codeRoot		-  A second root, provide for fast access to
 *	    			   strings that will be often repeated in a
 *				   data base (e.g. Compustat footnotes).
 *	valid			-  A flag indicating that the store has been
 *				   successfully initialized.
 *---------------------------------------------------------------------------
 */
struct Type_StringStore {
    int			*breakpointArray,
			*breakpointPtr,
			breakpointArraySize,
			newPTokenIndex, 
			stringCount,
			contentArraySize;
    char		*contentArray,
			*contentPtr;
    M_POP		newPTokenPOP;
    Type_NodePTR	tree,
			root,
			codeRoot;
    int			valid;
};


/*****  Access Macros for String Store  *****/
#define BreakpointArray(ss)	((ss).breakpointArray)
#define BreakpointPtr(ss)	((ss).breakpointPtr)
#define BreakpointArraySize(ss)	((ss).breakpointArraySize)
#define NewPTokenIndex(ss)	((ss).newPTokenIndex)
#define NewPTokenPOP(ss)	((ss).newPTokenPOP)
#define StringCount(ss)		((ss).stringCount)
#define ContentArraySize(ss)	((ss).contentArraySize)
#define ContentArray(ss)	((ss).contentArray)
#define ContentPtr(ss)		((ss).contentPtr)
#define Tree(ss)		((ss).tree)
#define Leaf(ss, i)		((ss).tree[i])
#define Root(ss)		((ss).root)
#define CodeRoot(ss)		((ss).codeRoot)
#define Valid(ss)		((ss).valid)


/*****  Allocation Defaults  *****/
/*****
 *  Content Sizes must be multiples of 4 .
 *  This is because the character uvector which will hold the strings in the
 *  system's lstore must be fill to a four byte boundary. Defining these
 *  constants as four byte multiples eliminates the need to check for
 *  reallocation when the uvector is squared off.
 *****/

#define BreakpointInitialSize	16384
#define TreeInitialSize		BreakpointInitialSize
#define ContentInitialSize	262134

#define StandardIncrement	BreakpointInitialSize
#define ContentIncrement	ContentInitialSize


/************************
 *****  Operations  *****
 ************************/

PublicFnDecl int osSTRING_ReadStringStore (
    char const*			fileprefix,
    M_POP*			lstorePToken
);

PublicFnDecl int osSTRING_LocateOrAddCode (
    char const*			string,
    int				length
);

PublicFnDecl int osSTRING_LocateOrAdd (
    char const*			string,
    int				length
);

PublicFnDef int osSTRING_SizeOfStringStore (
    int *			contentSize
);

PublicFnDecl void osSTRING_WriteStringStore (
    char const*			fileprefix,
    char *			fileptr,
    char **			endPtr,
    long *			filesize,
    long			segment
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  OSstring.h 1 replace /users/jck/updates
  870129 13:27:25 jck First release of the incorporator

 ************************************************************************/
/************************************************************************
  OSstring.h 2 replace /users/jck/updates
  870219 07:24:13 jck Code Type Tree added

 ************************************************************************/
/************************************************************************
  OSstring.h 3 replace /users/jck/updates
  870629 14:07:00 jck Implemented processing of multiple prefixes; reimplemented
allocation scheme for final segment; integrated avl search routines

 ************************************************************************/

/*****  Machine Specific Utilities  *****/

#include "stdoss.h"
#include "MSsupport.h"
#include <stdio.h>
#include "SIutil.h"

#ifdef hp9000s500		/*****  HP9000 series 500  *****/
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
#include <malloc.h>
#include <time.h>
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#include file
#include ssdef
#include <time.h>
#include varargs
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
#include </bsd4.3/usr/include/sys/time.h>

#if !defined(OLDMALLOC)
#include <malloc.h>
#endif

PublicVarDef int errinfo;
#endif

#ifdef sun		/*****  Sun  *****/
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>
#include <malloc.h>
#include <sys/time.h>

PublicVarDef int errinfo;
#endif

/************************************
 *****  Timers and Time Stamps  *****
 ************************************/

/****************
 *  'gettimer'  *
 ****************/

#if defined hp9000s500	|| defined apollo || defined(sun)

#ifndef HZ
#define HZ	60
#endif /* HZ */

PublicFnDef int
    gettimer (timerp)
struct timer *timerp;
{
    struct tms tms;
    long elapsed;
    static unsigned long elapsedBase = 0;

    if ((elapsed = times (&tms)) < 0)
        return -1;
    if (elapsedBase == 0)
        elapsedBase = elapsed;

    timerp->elapsed	= (int) ((elapsed - elapsedBase) / (double)HZ * 1000);
    timerp->user	= (int) (tms.tms_utime / (double)HZ * 1000);
    timerp->system	= (int) (tms.tms_stime / (double)HZ * 1000);
    timerp->cuser	= (int) (tms.tms_cutime / (double)HZ * 1000);
    timerp->csystem	= (int) (tms.tms_cstime / (double)HZ * 1000);

    return 0;
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
PublicFnDef int
    gettimer (timerp)
struct timer *timerp;
{
    struct {
	int tms_utime, tms_stime, tms_cutime, tms_cstime;
    } tms;
    long elapsed;
    static unsigned long elapsedBase = 0;

    times (&tms);
    elapsed = time (0);

    if (elapsedBase == 0)
        elapsedBase = elapsed;

    timerp->elapsed	= (elapsed - elapsedBase) * 1000;
    timerp->user	= tms.tms_utime * 10;
    timerp->system	= tms.tms_stime * 10;
    timerp->cuser	= tms.tms_cutime * 10;
    timerp->csystem	= tms.tms_cstime * 10;

    return 0;
}
#endif

/***************
 *  'gettime'  *
 ***************/

#if defined hp9000s500	|| defined apollo || defined(sun)
PublicFnDef int
    gettime (timep)
struct time *timep;
{
    struct timeval	timeval;
    struct timezone	timezone;

    if (gettimeofday (&timeval, &timezone) < 0)
        return -1;

    timep->seconds	= timeval.tv_sec;
    timep->microseconds	= timeval.tv_usec;

    return 0;
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
PublicFnDef int
    gettime (timep)
struct time *timep;
{
    struct timeb	timeval;

    ftime (&timeval);

    timep->seconds	= timeval.time;
    timep->microseconds	= timeval.millitm * 1000L;

    return 0;
}
#endif

/*****************************************
 *****  Character String Operations  *****
 *****************************************/

/**********************************
 *****  Formatting Utilities  *****
 **********************************/

#ifdef VMS			/*****  VAX/VMS  *****/
/**************************
 *  Utility Declarations  *
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to count the number of VAX longwords expected on the parameter
 *****  stack by a 'printf' format string.
 *
 *  Argument:
 *	fmt			- the format string to be analysed.
 *
 *  Returns:
 *	The number of longwords (4 byte) required to satisfy the conversion
 *	parameters of 'fmt'.
 *
 *****/
static int
    LongwordsExpectedByFormat (fmt)
char *fmt;
{
    enum {ordinary, conversion, done} state;
    char *p;
    int longwords;

    longwords = 0;
    state = ordinary;

    do {
        switch (state)
	{
	case ordinary:
	    if (IsntNil (p = strchr (fmt, '%')))
	    {
	        fmt = p + 1;
		state = conversion;
	    }
	    else
	    {
		state = done;
	    }
	    break;
	case conversion:
	    switch (*fmt++)
	    {
	    case 'c':		/* %c */
	    case 'd':		/* %d */
	    case 'o':		/* %o */
	    case 's':		/* %s */
	    case 'u':		/* %u */
	    case 'x':		/* %x */
	    case 'X':		/* %X */
	        longwords++;
		state = ordinary;
	        break;
	    case 'e':		/* %e */
	    case 'E':		/* %E */
	    case 'f':		/* %f */
	    case 'g':		/* %g */
		longwords += 2;
		state = ordinary;
		break;
	    case '*':
	        longwords++;
	        break;
	    case '%':
	        state = ordinary;
	        break;
	    case '\0':
	        state = done;
	        break;
	    default:
	        break;
	    }
	    break;
	default:
	    break;
	}
    } while (state != done);

    return longwords;
}

/*---------------------------------------------------------------------------
 *****  'lib$callg' argument buffer.
 *---------------------------------------------------------------------------
 */

PrivateVarDef int
    ArgumentBuffer [256];

/***************
 *  'vprintf'  *
 ***************/

PublicFnDef int
    VMS_vprintf (fmt, ap)
char *fmt;
va_list ap;
{
    int
	argumentLongwords, printf ();

    argumentLongwords = LongwordsExpectedByFormat (fmt);

    ArgumentBuffer[0] = argumentLongwords + 1;
    ArgumentBuffer[1] = (int)fmt;
    memcpy (&ArgumentBuffer[2], ap, argumentLongwords * sizeof (int));

    return lib$callg (ArgumentBuffer, printf);
}

/****************
 *  'vsprintf'  *
 ****************/

PublicFnDef int
    VMS_vsprintf (buffer, fmt, ap)
char *buffer, *fmt;
va_list ap;
{
    int
	argumentLongwords, sprintf ();

    argumentLongwords = LongwordsExpectedByFormat (fmt);

    ArgumentBuffer[0] = argumentLongwords + 2;
    ArgumentBuffer[1] = (int)buffer;
    ArgumentBuffer[2] = (int)fmt;
    memcpy (&ArgumentBuffer[3], ap, argumentLongwords * sizeof (int));

    return lib$callg (ArgumentBuffer, sprintf);
}

/****************
 *  'vfprintf'  *
 ****************/

PublicFnDef int
    VMS_vfprintf (stream, fmt, ap)
FILE *stream;
char *fmt;
va_list ap;
{
    int
	argumentLongwords, fprintf ();

    argumentLongwords = LongwordsExpectedByFormat (fmt);

    ArgumentBuffer[0] = argumentLongwords + 2;
    ArgumentBuffer[1] = (int)stream;
    ArgumentBuffer[2] = (int)fmt;
    memcpy (&ArgumentBuffer[3], ap, argumentLongwords * sizeof (int));

    return lib$callg (ArgumentBuffer, fprintf);
}
#endif

/***********************************
 *****  Searching and Sorting  *****
 ***********************************/

/***************
 *  'bsearch'  *
 ***************/

#ifdef VMS			/*****  VAX/VMS  *****/

/*---------------------------------------------------------------------------
 *  Implementation of UNIX "bsearch" to be used on the VAX in the same context.
 *****/
PublicFnDef ADDRESS
    VMS_bsearch (key, base, nel, elsize, compar)
ADDRESS key, base;
unsigned nel, elsize;
int (*compar)();
{
    int
	lb, ub, loc, comparResult;

    for (lb = (-1), ub = nel; ub - lb > 1; )
    {
	loc = (ub + lb) / 2;
	comparResult = (*compar)(base + elsize * loc, key);
	if (comparResult == 0)
	{
/*****  Found...  *****/
	    return base + elsize * loc;
	}
	else if (comparResult < 0)
	    lb = loc;
	else
	    ub = loc;
    }

/*****  Not found...  *****/
    return NULL;
}

#endif

/*************
 *  'qsort'  *
 *************/

#ifdef VMS			/*****  VAX/VMS  *****/

/*--------------------------------------------------------------------------
 *  Implementation of UNIX "qsort" to be used on the VAX in the same context.
 *  Origin:  Wirth, "A + D = P"
 *****/
PublicFnDef void
    VMS_qsort (base, nel, elsize, compar)
char *base;
unsigned nel;
int elsize, (*compar)();
{
    int sp, left, right, L, R, pivotIndex, stack[100];
    char *pivot, *temp;

    if (nel <= 1 || elsize <= 0)
	return;

    temp  = malloc (elsize);
    pivot = malloc (elsize);
    left = 0;  right = nel - 1;  sp = 0;
    stack[sp++] = left;  stack[sp++] = right;     /* push initial r/l pair */
    do
    {
	right = stack[--sp];  left = stack[--sp];	/*  pop r/l pair  */
	do
	{
	    L = left; R = right; pivotIndex = (L + R) / 2;
	    memcpy (pivot, base + pivotIndex * elsize, elsize);
	    do
	    {
		while ((*compar) (base + L * elsize,  pivot) < 0)
		    L++;
		while ((*compar) (base + R * elsize,  pivot) > 0)
		    R--;
		if (L <= R)
		{
		    memcpy (temp, 	       base + L * elsize, elsize);
		    memcpy (base + L * elsize, base + R * elsize, elsize);
		    memcpy (base + R * elsize, temp, 		  elsize);
		    L++; R--;
		}
	    } while (L <= R);

	    if (L < right)
	    {
		stack[sp++] = L;
		stack[sp++] = right;
	    }
	    right = R;
	} while (left < right);		/* split a[1] .. a[r] */

    } while (sp > 0);	/* ...until s = 0 */
    free (temp);
    free (pivot);
}
#endif

#ifdef sun			/*****  Sun  *****/

/*--------------------------------------------------------------------------
 *  Implementation of UNIX "qsort" to be used on the Sun in the same context.
 *  Origin:  Wirth, "A + D = P"
 *****/
#if 0
PublicFnDef void
    SUN_qsort (base, nel, elsize, compar)
char *base;
unsigned nel;
int elsize, (*compar)();
{
    int sp, left, right, L, R, pivotIndex, stack[512];
    char *pivot, *temp;

    if (nel <= 1 || elsize <= 0)
	return;

    temp  = malloc (elsize);
    pivot = malloc (elsize);
    left = 0;  right = nel - 1;  sp = 0;
    stack[sp++] = left;  stack[sp++] = right;     /* push initial r/l pair */
    do
    {
	right = stack[--sp];  left = stack[--sp];	/*  pop r/l pair  */
	do
	{
	    L = left; R = right; pivotIndex = (L + R) / 2;
	    SUN_memcpy (pivot, base + pivotIndex * elsize, elsize);
	    do
	    {
		while ((*compar) (base + L * elsize,  pivot) < 0)
		    L++;
		while ((*compar) (base + R * elsize,  pivot) > 0)
		    R--;
		if (L <= R)
		{
		    SUN_memcpy (temp,		   base + L * elsize, elsize);
		    SUN_memcpy (base + L * elsize, base + R * elsize, elsize);
		    SUN_memcpy (base + R * elsize, temp, 	      elsize);
		    L++; R--;
		}
	    } while (L <= R);

	    if (L < right)
	    {
		stack[sp++] = L;
		stack[sp++] = right;
	    }
	    right = R;
	} while (left < right);		/* split a[1] .. a[r] */

    } while (sp > 0);	/* ...until s = 0 */
    free (temp);
    free (pivot);
}
#endif
#endif

/*********************
 *  'tsearch', etc.  *
 *********************/

#if defined(hp9000s500) || defined(apollo) || defined(sun)

/****************************************************************************

	Implementation of UNIX "tsearch" using an AVL tree algorithm
	lifted from Wirth, "A + D = P" (1976), pgs. 220 - 1.  6/87, cmm

 ****************************************************************************/
/*--------------------------------------------------------------------------
*  macro balances left subtree of an AVL tree after its height has increased
*  due to an insertion.
*
*  Arguments:
*	p -- address of the root of the subtree to balance,
*	h -- address of boolean variable which records subtree height change
*	     status;  it is always set TRUE on entry to this macro...
*
*****/
#define balanceLeft(p, h)\
{\
    switch ((p)->bal)\
    {\
      case  1:\
        (p)->bal =  0; *(h) = FALSE;  		break;\
\
      case  0:\
	(p)->bal = -1;		  		break;\
\
      case -1:\
	p1 = (p)->left;\
	if (p1->bal == -1)\
	{				/* single LL rotation */\
	    (p)->left = p1->right;  p1->right = (p);\
	    (p)->bal  = 0;	    (p) = p1;\
	}\
	else\
	{				/* double LR rotation */\
	    p2 = p1->right;\
	    p1->right = p2->left;  p2->left = p1;\
	    (p)->left = p2->right; p2->right = (p);\
	    (p)->bal = (p2->bal == -1) ?   1 : 0;\
	    p1->bal  = (p2->bal ==  1) ?  -1 : 0;\
	    (p) = p2;\
	}\
	(p)->bal = 0;  *(h) = FALSE;\
	break;\
    }\
}

/*--------------------------------------------------------------------------
*  macro balances right subtree of an AVL tree after its height has increased
*  due to an insertion.
*
*  Arguments:
*	p -- address of the root of the subtree to balance,
*	h -- address of boolean variable which records subtree height change
*	     status;  it is always set TRUE on entry to this macro...
*
*****/
#define balanceRight(p, h)\
{\
    switch ((p)->bal)\
    {\
      case -1:\
        (p)->bal =  0;  *(h) = FALSE;  		break;\
\
      case  0:\
	(p)->bal =  1;		  		break;\
\
      case  1:\
	p1 = (p)->right;\
	if (p1->bal ==  1)\
	{				/* single RR rotation */\
	    (p)->right = p1->left;  p1->left = (p);\
	    (p)->bal  = 0;	    (p) = p1;\
	}\
	else\
	{				/* double RL rotation */\
	    p2 = p1->left;\
	    p1->left = p2->right;  p2->right = p1;\
	    (p)->right = p2->left; p2->left = (p);\
	    (p)->bal = (p2->bal ==  1) ? -1 : 0;\
	    p1->bal  = (p2->bal == -1) ?  1 : 0;\
	    (p) = p2;\
	}\
	(p)->bal = 0;  *(h) = FALSE;\
	break;\
    }\
}

/*--------------------------------------------------------------------------
*  AVLSEARCH helper routine implements AVL tree algorithm given in
*  Wirth, "A + D = P", pgs 220 - 1
*
*  Arguments:
*	key	     -- pointer to datum to find,
*	rootp	     -- address of pointer to root node of tree to be searched,
*	compar	     -- address of function to use in search.
*	heightChange -- address of variable flags the need to check (& possibly
*			restore) tree balance.
*	foundNodePtr -- address of variable to be set to the address of the
*			found or inserted node.
*
*  RETURNS:  error (-1) if unable to alloc mem'y for node; 0 otherwise.
*  SIDE EFFECTS:  "foundNodePtr" is set to a pointer to the found or inserted
*		   node; "heightChange" will be set to reflect the balance
*		   status of the tree in question.
*****/
PrivateFnDef int search (key, rootp, compar, heightChange, foundNodePtr)
ADDRESS key;
tnode **rootp, **foundNodePtr;
int (*compar)(), *heightChange;
{
    tnode *p1, *p2;

/*****  key not in tree;  insert it  *****/
    if (*rootp == NULL)
    {
    	if ((*rootp = (tnode *)malloc (sizeof (tnode))) == NULL)
	    return -1;

	(*rootp)->data  = key;
	(*rootp)->bal   = 0;
	(*rootp)->left  = (*rootp)->right = NULL;
	*heightChange   = TRUE;
	*foundNodePtr   = *rootp;
	return 0;
    }

/*****  search left subtree?  *****/

    if (((*compar)(key, (*rootp)->data)) < 0)
    {
	if ((search (key, &(*rootp)->left, compar, heightChange, foundNodePtr))
	    < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange)
	    balanceLeft (*rootp, heightChange);
	return 0;
    }

/***** search right subtree? *****/

    if (((*compar)(key, (*rootp)->data)) > 0)
    {
    if ((search (key, &(*rootp)->right, compar, heightChange, foundNodePtr))
	    < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange)
	    balanceRight (*rootp, heightChange);
	return 0;
    }

/*****  key found!  *****/
    *heightChange = FALSE;
    *foundNodePtr = *rootp;
    return 0;
}

/*--------------------------------------------------------------------------
*  		UNIX TSEARCH emulation using an AVL tree.
*  	     Algorithm from Wirth, "A + D = P", pgs. 220 - 1.
*
*  Arguments:
*	key	-- pointer to datum to find,
*	rootp	-- address of pointer to root node of tree to be searched,
*	compar	-- function to use in search.
*
*  Returns:  address of tree node containing the datum, whether found or
*	     inserted;  returns NULL if unable to allocate space for new node
*****/
PublicFnDef tnode *avlSearch (key, rootp, compar)
ADDRESS key;
tnode **rootp;
int (*compar)();
{
    int heightChange = FALSE;
    tnode *foundNodep;

    if (search (key, rootp, compar, &heightChange, &foundNodep) < 0)
	return NULL;

    return foundNodep;
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
/****************************************************************************
 ****************************************************************************
 ****************************************************************************

		Emulation of UNIX "tsearch", "twalk", "tfind"
		  	to run under VAX/VMS.
			     6/4/87, cmm

 NOTE:  the implementations of tsearch() and tfind() are not reentrant due to
 	the strategy used to mesh the VAX runtime routines called with the
	UNIX user interface being emulated.  See routines comparCover(),
	tsearch(), tfind();  global variable compareFn.
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************/

globalvalue int
    LIB$_INSVIRMEM, LIB$_KEYALRINS, LIB$_KEYNOTFOU, LIB$_NORMAL;

/***** global variable used to put "compar" in scope accessible from
       LIB$xx(comparCover()) ...see tsearch, tfind.  THIS STRATEGY RENDERS
       THIS IMPLEMENTATION OF TSEARCH, TFIND NON-REENTRANT
*****/
static int (*compareFn)();

/*****  typedef describes tree node to be built by LIB$INSERT_TREE *****/
typedef struct treenode node;

/*--------------------------------------------------------------------------
*  Recursive helper routine for TWALK emulation
*
*  Arguments:
*	root 	-- pointer to root node of tree...
*	action  -- pointer to function to apply to each node
*	level	-- level in the tree of node being processed;  level of the
*		   root of the tree passed to "twalk" is 0.
*
*  Returns:  NOTHING
*****/
PrivateFnDef void visitNode (root, action, level)
node *root;
void (*action)();
int level;
{
    level++;

    if (root == NULL)			/* empty tree? */
	return;

    if (root->left == NULL && root->right == NULL)
    {
	(*action) (root, leaf, level);
	return;
    }

    (*action) (root, preorder, level);

    visitNode (root->left, action, level);

    (*action) (root, postorder, level);

    visitNode (root->right, action, level);

    (*action) (root, endorder, level);
}

/*--------------------------------------------------------------------------
*  UNIX TWALK emulation
*
*  Arguments:
*	root 	-- pointer to root node of tree...
*	action  -- pointer to function to apply to each node
*
*  Returns:  NOTHING
*****/
PublicFnDef void twalk (root, action)
node *root;
void (*action)();
{
    visitNode (root, action, -1);
}

/*--------------------------------------------------------------------------
*  Helper function that strips the descriptor off arguments to the "compar"
*  function specified in the UNIX interface being emulated.
*
*  Arguments:
*	symstr   -- address of a descriptor pointing to a pointer to
*		    the symbol key... (!)
*	treehead -- pointer to the current node -- this is what LIB$LOOKUP_TREE
*		    and LIB$INSERT_TREE actually pass to their "compare-rtn"s;
*		    the documentation for these library routines is wrong.
*	dummy	 -- optional argument offered by the VAX protocol for
*		    user-defined info; not used here.
*
*  Returns:  NOTHING
*  Note:   "compar" is the comparison function to be used, accessed globally.
*	    It takes pointers to two data as arguments.  See TSEARCH(3C).
*****/
PrivateFnDef int comparCover (symstr, treehead, dummy)
struct dsc$descriptor_s *symstr;
node *treehead;
ADDRESS dummy;
{
    return (*compareFn) ( *((char **)($ADDRESS (*symstr))), treehead->data);
}

/*--------------------------------------------------------------------------
*  UNIX TFIND emulation
*
*  Arguments:
*	key	-- pointer to datum to find,
*	rootp	-- handle to root node of tree to search,
*	compar	-- function to use in search.  This is actually accessed
*		   globally by "comparCover" within LIB$LOOKUP_TREE;  it is
*		   given here to satisfy the UNIX interface being emulated.
*
*  Returns:
*	A pointer to the located tree node or NULL if a matching node was
*	NOT found.  Empirically, this is the correct value to return even
*	though the UNIX documentation suggests that a pointer to the 'datum'
*	is returned.
*****/
PublicFnDef node
    *tfind (key, rootp, compar)
ADDRESS key;
node **rootp;
int (*compar)();
{
    node *newnode;
    int  dummy;

/***** descriptor for pointer to key; known size  *****/
    $LDESCRIPTOR (keyDescriptor, sizeof (key), &key);

/***** put "compar" in scope accessible from LIB$..(comparCover())  *****/
    compareFn = compar;

    if ( lib$lookup_tree
	   (rootp, &keyDescriptor, comparCover, &newnode) == LIB$_KEYNOTFOU )
	return NULL;

    return newnode;
}

/*--------------------------------------------------------------------------
*  Routine allocates memory for a new tree node - required by LIB$INSERT_TREE
*
*  Arguments:
*	keyDsc 	- pointer to descriptor holding address of datum
*	nodeHandle - pointer to variable in which to store address of
*		  newly inserted node,
*	dummy	- optional arg, not used.
*
*  Returns:  result code from memory allocation attempt
*****/
PrivateFnDef int allocNode (keyDsc, nodeHandle, dummy)
struct dsc$descriptor_s *keyDsc;
ADDRESS dummy;
node **nodeHandle;
{
    *nodeHandle = (node *)malloc (sizeof (node));
    if (*nodeHandle == NULL)
	return LIB$_INSVIRMEM;

/*****  allocation successful; install key  *****/
    (*nodeHandle)->data = *((char **)($ADDRESS (*keyDsc)));
    return SS$_NORMAL;
}

/*--------------------------------------------------------------------------
*  UNIX TSEARCH emulation
*
*  Arguments:
*	key	-- pointer to datum to find,
*	rootp	-- handle to root node of tree to search,
*	compar	-- function to use in search.  This is actually accessed
*		   globally by "comparCover" within LIB$LOOKUP_TREE;  it is
*		   given here to satisfy the UNIX interface being emulated.
*
*  Returns:
*	A pointer to the inserted/located tree node.  Empirically, this is
*	the correct value to return even though the UNIX documentation
*	suggests that a pointer to the 'datum' is returned.  Returns NULL
*	if the node cannot be allocated.
*****/
PublicFnDef node
    *tsearch (key, rootp, compar)
ADDRESS key;
node **rootp;
int (*compar)();
{
    node *newnode;

/***** descriptor for pointer to key; known size  *****/
    $LDESCRIPTOR (keyDescriptor, sizeof (key), &key);
        int  dummy;

/***** put "compar" in scope accessible from LIB$..(comparCover())  *****/
    compareFn = compar;

    if ( lib$lookup_tree (rootp, &keyDescriptor, comparCover, &newnode)
         == LIB$_KEYNOTFOU )

	 if (lib$insert_tree (rootp, &keyDescriptor, &0, comparCover,
			      allocNode, &newnode, &dummy)
             == LIB$_INSVIRMEM )

 	             return NULL;

    return newnode;
}


/************************
 *  Option Acquisition  *
 ************************/

PublicVarDef char
    *optarg;

PublicVarDef int
    optind = 1,
    opterr;


/*--------------------------------------------------------------------------
*  Emulation of HP-UX getopt().  see GETOPT(3C).
*****/
PublicFnDef int getopt (argc, argv, optstring)
int argc;
char **argv, *optstring;
{
    static int
	currArg = 1, 	/* duplicates "optind" */
	currCh  = 1;	/* skip '-', assuming it exists */
    int ch;
    char *optChPtr;

    optarg = "";

/*****  get next char from argv option arg (if any exist)  *****/
    if (currArg != argc && *argv [currArg] == '-'
			&&  argv [currArg] [currCh] != '\0')
    {
	ch = argv [currArg] [currCh++];

/*****  check special case option '--' (by itself) as opt arg terminator  *****/
	if (currCh == 2 && ch == '-' && argv [currArg] [currCh] == '\0')
	    return EOF;

	if (argv [currArg] [currCh] == '\0')
	{
	    optind++;  currArg++;  currCh = 1;
	}
    } else return EOF;

/*****  check against option string  *****/
    if ((optChPtr = strchr (optstring, ch)) == NULL)
    {						/* illegal option...  	      */
	if (opterr != 0)
	    fprintf (stderr, "%s: illegal option -- %c\n", argv[0], ch);
	return '?';
    }
    else					/* option is legal	      */
    {
	if (*(optChPtr + 1) == ':') 		/* option requires argument?  */
	{
	    if (currCh == 1)  			/* white space btwn opt & arg */
	    {
		if (currArg == argc)		/* ...or missing argument?    */
		{
		    if (opterr != 0)
			fprintf
			  (stderr,
			   "%s: option requires an argument -- %c\n",
			    argv[0], ch);
		    return '?';
		}
		optarg = argv[currArg];
	    }
	    else	/* no such white space;  option arg is present        */
		optarg = argv[currArg] + currCh;
	    optind = currArg += 1;
	    currCh = 1;
	}
	return ch;
    }
}
#endif

/*****************************
 *****  Memory Movement  *****
 *****************************/

/**************
 *  'memcpy'  *
 **************/

/***************
 *  'memcpyb'  *
 ***************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
PublicFnDef char
    *memcpyb (dst, src, n)
char *dst, *src;
int n;
{
    char *nsrc, *ndst;

    nsrc = src + n - 1;
    ndst = dst + n - 1;

    asm ("
    #  Shift tail...
	    lds	~nsrc
	    lds	~ndst
	    lds	~n
	    mvb	3,b
    ");

    return dst;
}
#endif

#ifdef sun		/*****  Sun  *****/
PublicFnDef char
    *memcpyb (dst, src, n)
char *dst, *src;
int n;
{
    char *nsrc, *ndst;

    nsrc = src + n - 1;
    ndst = dst + n - 1;

    for( ; n ; n-- )
    	*ndst-- = *nsrc--;

    return dst;
}

PublicFnDef char
    *SUN_memcpy (dst, src, n)
char *dst, *src;
int n;
{
    char	*ndst = dst;
    if( (dst == src) || (n <= 0) )
    	return(dst);
    if( ((unsigned int)dst > (unsigned int)src) &&
    	((unsigned int)dst < ((unsigned int)src + n)) )
    	return(memcpyb(dst,src,n));
    for( ; n ; n-- )
    	*ndst++ = *src++;

    return dst;
}
#endif

/**************
 *  'memset'  *
 **************/

/*******************************
 *****  Memory Allocation  *****
 *******************************/

/**************
 *  'malloc'  *
 **************/

/***************
 *  'realloc'  *
 ***************/

/************
 *  'free'  *
 ************/

/******************
 *  'initmalloc'  *
 ******************/

#ifndef FRONTEND
/* The frontend can't be compiled with '-lmalloc' because of conflicts */
#ifdef hp9000s500		/*****  HP9000 Series 500  *****/

PublicFnDef void
    initmalloc ()
{
/***** Turn off small block allocation to avoid 'malloc(3X)' bug... *****/
    mallopt (M_MXFAST, 0);
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
PublicFnDef void
    initmalloc ()
{
}
#endif

#ifdef apollo
PublicFnDef void
    initmalloc ()
{
#if !defined(OLDMALLOC)
/***** Turn off small block allocation to avoid 'malloc(3X)' bug... *****/
    mallopt (M_MXFAST, 0);
#endif /* !OLDMALLOC */
}
#endif

#ifdef sun		/*****  Sun  *****/

PublicFnDef void
    initmalloc ()
{
#ifndef OLDMALLOC
/***** Turn off small block allocation to avoid 'malloc(3X)' bug... *****/
    mallopt (M_MXFAST, 0);
#endif /* !OLDMALLOC */
}
#endif

/*******************
 *  'querymalloc'  *
 *******************/
#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
PublicFnDef void
    querymalloc (xstats)
struct MallocStats *xstats;
{
    struct mallinfo
	istats;

    istats = mallinfo (0);

    xstats->total	= istats.arena;
    xstats->used	= istats.usmblks + istats.uordblks + istats.hblkhd;
    xstats->free	= istats.fsmblks + istats.fordblks;
}
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
PublicFnDef void
    querymalloc (xstats)
struct MallocStats *xstats;
{
    static readonly int
	inUseCode = 3;

    lib$stat_vm (&inUseCode, &xstats->used);

    xstats->total	= xstats->used;
    xstats->free	= 0;
}
#endif

#ifdef apollo
PublicFnDef void
    querymalloc (xstats)
struct MallocStats *xstats;
{
#if !defined(OLDMALLOC)
    struct mallinfo istats;

    istats = mallinfo ();

    xstats->total	= istats.arena;
    xstats->used	= istats.usmblks + istats.uordblks + istats.hblkhd;
    xstats->free	= istats.fsmblks + istats.fordblks;
#else
    xstats->total	=
    xstats->used	=
    xstats->free	= 0;
#endif /* !OLDMALLOC */
}
#endif

#ifdef sun		/*****  Sun  *****/
PublicFnDef void
    querymalloc (xstats)
struct MallocStats *xstats;
{
#ifndef OLDMALLOC
    struct mallinfo
	istats;

    istats = mallinfo (0);

    xstats->total	= istats.arena;
    xstats->used	= istats.usmblks + istats.uordblks + istats.hblkhd;
    xstats->free	= istats.fsmblks + istats.fordblks;
#else
    xstats->total	=
    xstats->used	=
    xstats->free	= 0;
#endif /* !OLDMALLOC */
}
#endif
#endif /* FRONTEND */

/*************************************************
********** 	Error Routines		**********
**************************************************/
#ifdef FRONTEND
PublicFnDecl void	RS_callSignalHandler();
#endif /* FRONTEND */

PublicFnDef void STD_syserr (msg)
char *msg;
{
    fprintf (stderr, "ERROR: %s (%d", msg, errno);
    if (errno > 0 && errno < sys_nerr)
	fprintf (stderr, "; %s)\n", sys_errlist[errno]);
#ifdef VMS
    else if (errno == EVMSERR)
    {
    	fprintf(stderr,", %d)\n",vaxc$errno);
    	perror(msg);
    }
#endif
    else
        fprintf (stderr, ")\n");
#ifdef FRONTEND
    RS_callSignalHandler(0);
#else /* FRONTEND */
    exit (STD_ErrorExitValue);
#endif /* FRONTEND */
}

PublicFnDef void STD_syswarn (msg)
char *msg;
{
    fprintf (stderr, "ERROR: %s (%d", msg, errno);
    if (errno > 0 && errno < sys_nerr)
	fprintf (stderr, "; %s)\n", sys_errlist[errno]);
#ifdef VMS
    else if (errno == EVMSERR)
    {
    	fprintf(stderr,", %d)\n",vaxc$errno);
    	perror(msg);
    }
#endif
    else
        fprintf (stderr, ")\n");

}


/************************************************
 *****  Vfork, Exec, Pipe                   *****
 ************************************************/

/*** NOTE: The stderr and stdout are mapped to the same streams ***/

PublicFnDef int STD_execute (name, args, fdin, fdout, preChildProcessing)
char	*name, *args[];
int	*fdin, *fdout;
int	(*preChildProcessing)();
{
    int	Pfdin[2], Pfdout[2], tmpin, tmpout, tmperr, pid;

    if (STD_pipe (Pfdin,0) == -1 ||
#if 1
	STD_pipe (Pfdout,0) == -1)
#else
	STD_pipe (Pfdout,O_NDELAY) == -1)
#endif
	STD_syswarn ("opening 2 pipes");

    tmpin  = dup(0);
    tmpout = dup(1);
    tmperr = dup(2);

    switch (pid = vfork ())
    {
    case -1:
	STD_syswarn ("vfork error");

    case 0:
	if (close (0) == -1)
	    STD_syswarn ("close std in");
	if ( dup (Pfdin[0]) != 0)
	    STD_syserr ("dup std in");
	if (close (1) == -1)
	    STD_syswarn ("close std out");
	if (dup (Pfdout[1]) != 1)
	    STD_syserr ("dup std out");
	if (close (2) == -1)
	    STD_syswarn ("close std err");
	if (dup (Pfdout[1]) != 2)
	    STD_syserr ("dup std err");

	if( preChildProcessing != NULL )
		(*preChildProcessing)();

	execvp (name, args);

	/* should never get here */
	STD_syswarn ("should never be here");
    }

    *fdin  = Pfdin[1];
    *fdout = Pfdout[0];

    if (close (0) == -1)
	    STD_syswarn ("close std in");
    if (dup (tmpin) != 0)
	    STD_syserr ("dup std in");
    if (close (1) == -1)
	    STD_syswarn ("close std out");
    if (dup (tmpout) != 1)
	    STD_syserr ("dup std out");
    if (close (2) == -1)
	    STD_syswarn ("close std err");
    if (dup (tmperr) != 2)
	    STD_syserr ("dup std err");
    close(tmpin);
    close(tmpout);
    close(tmperr);

    return( pid );
}

#ifdef VMS
PublicFnDef int
    VMS_system (command)
char *command;
{
    $DESCRIPTOR (dsc, "");

    $ADDRESS (dsc) = command;
    $LENGTH (dsc) = strlen (command);

    return lib$spawn (&dsc);
}
#endif

/********************
 * String to Double *
 ********************/

#ifdef hp9000s500
/*****  Assembly Language routine to convert an ascii string to a floating
 *****  point number.
 *
 *  Arguments:
 *	s			-  the string to be converted.
 *	n			-  the number of charcters to be considered.
 *
 *  Returns:
 *	The resulting double precision floating point number.
 *
 *  Note:
 *	This routine converts 10000 10 character strings to floating point
 *  representation in less than 2 seconds.  By comparison, the C routine,
 *  atof(), takes aproximately 7 seconds.
 *
 *****/
PublicFnDef double dtb (s, n)
char *s;
int n;
{
    int sign = 0, b[5];

/*  Compute sign  */
    if (*s == '-')
    {
        sign = 1;
	s++;
	n--;
    }
    else if (*s == '+')
    {
    	s++;
	n--;
    }

    asm ("
	# Zero the translation buffer
	zero
	lras	~b
	ldis	17
	filb	1	# Leave 0 for 'pack pdn[0]', '&b' for addr. calc.
	# Compute destination offset in translation buffer
	addi	17
	lds	~n
	sub
	# Copy the number into the translation buffer
    	lds	~s
	xch
	lds	~n
	mvb	3
	# Pack pdn[0]
	# Initial 'zero' left by 'filb'
	lds	~sign
	dpf	(12:4)
	# Pack first word ...
	lds	~b
	lds	~L.mask
	and
	beq	~L.le13Digits
	dup
	exf	(0:28)
	or
	dup
	exf	(8:8)
	dpf	(16:8)
	dpf	(16:16)
	# Pack pdn[1]
	zero
~L.le13Digits:
	lds	~b+4
	lds	~L.mask
	and
	beq	~L.le9Digits
	dup
	exf	(0:28)
	or
	dup
	exf	(8:8)
	dpf	(16:8)
~L.le9Digits:
	dpf	(0:16)
	lds	~b+8
	lds	~L.mask
	and
	beq	~L.le5Digits
	dup
	exf	(0:28)
	or
	dup
	exf	(8:8)
	dpf	(16:8)
	dpf	(16:16)
	# Pack pdn[2]
	zero
~L.le5Digits:
	lds	~b+12
	lds	~L.mask
	and
	beq	~L.le1Digit
	dup
	exf	(0:28)
	or
	dup
	exf	(8:8)
	dpf	(16:8)
~L.le1Digit:
	dpf	(0:16)
	ldb	~b+16
	dpf	(16:4)
	# Copy 'pdn' to look at it
	#lras	S-11
	#lras	~pdn
	#ldis	12
	#mvb	3
	# Convert and return the packed decimal number on the stack...
	dtb
	std	Q-20,i
	exit	0
~L.mask:
	word	0x0f0f0f0f
        ");
}
#endif

#ifdef VMS
/***** This library routine is about 23% faster than atof() *****/
PublicFnDef double dtb (s, n)
char	*s;
int	n;
{
	double	retval;
	$LDESCRIPTOR(desc,n,s);

#if CC$gfloat
	ots$cvt_t_g(&desc,&retval,0,0,0,0);
#else
	ots$cvt_t_d(&desc,&retval,0,0,0,0);
#endif
	return(retval);
}
#endif

#ifdef apollo
PublicFnDef double dtb (s, n)
char *s;
int n;
{
	double	atof();
	char	buf[512];

	memcpy(buf,s,n);
	buf[n] = '\0';
	return( atof(buf) );
}
#endif

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSutil.c 1 replace /users/jck/system
  880330 21:54:48 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSutil.c 2 replace /users/jck/system
  880407 10:29:08 jck VAX modifications

 ************************************************************************/
/************************************************************************
  MSutil.c 3 replace /users/m2/dbup
  880421 17:11:55 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  MSutil.c 4 replace /users/lis/frontend/M2/sys
  880505 11:37:53 m2 Apollo fix for errinfo

 ************************************************************************/
/************************************************************************
  MSutil.c 5 replace /users/lis/frontend/M2/sys
  880505 12:51:26 m2 OOPS, I added errinfo twice

 ************************************************************************/
/************************************************************************
  MSutil.c 6 replace /users/m2/backend
  890503 14:29:09 m2 VAX-Apollo fixes

 ************************************************************************/
/************************************************************************
  MSutil.c 7 replace /users/m2/backend
  890927 14:36:21 m2 SUN port (preliminary)

 ************************************************************************/

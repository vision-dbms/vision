/*****  Vision Kernel Implementation  *****/

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

/*****  Kernel  *****/
#include "Vk.h"


/*******************************
 *******************************
 *****  Memory Allocation  *****
 *******************************
 *******************************/

PublicVarDef VkAllocator VkAllocator_DefaultInstance = {
    (void*(*)(size_t))	    malloc,
    (void*(*)(void*,size_t))realloc,
    (void (*)(void*))	    free
};

PublicFnDef void VkAllocator_Initialize (
    VkAllocator*		pThis,
    void			*(*pAllocateImplementation	)(size_t),
    void			*(*pReallocateImplementation	)(void *, size_t),
    void			 (*pFreeImplementation		)(void *)
)
{
    if (IsntNil (pAllocateImplementation))
    	pThis->m_pAllocateImplementation = pAllocateImplementation;
    if (IsntNil (pReallocateImplementation))
    	pThis->m_pReallocateImplementation = pReallocateImplementation;
    if (IsntNil (pFreeImplementation))
    	pThis->m_pFreeImplementation = pFreeImplementation;
}


/********************
 ********************
 *****  Timers  *****
 ********************
 ********************/

#if defined (_WIN32)

/************************
 *****  Windows/NT  *****
 ************************/

#define FileTimeToMilliseconds(ft) (\
    ((double)(ft).dwHighDateTime * (double)UINT_MAX + (double)(ft).dwLowDateTime) / 1e4\
)

PublicFnDef int gettimer (
    struct timer *timerp
)
{
    SYSTEMTIME		currentSystemTime;
    FILETIME		currentTime;
    FILETIME		processCreationTime;
    FILETIME		processExitTime;
    FILETIME		processKernelTime;
    FILETIME		processUserTime;

    GetSystemTime	(&currentSystemTime);
    SystemTimeToFileTime(&currentSystemTime, &currentTime);

    GetProcessTimes (
	GetCurrentProcess (),
	&processCreationTime,
	&processExitTime,
	&processKernelTime,
	&processUserTime
    );

    timerp->elapsed	= (unsigned long)(
	FileTimeToMilliseconds (currentTime) - FileTimeToMilliseconds (processCreationTime)
    );

    timerp->user	= (unsigned long)FileTimeToMilliseconds (processUserTime);
    timerp->system	= (unsigned long)FileTimeToMilliseconds (processKernelTime);
    timerp->cuser	= 0;
    timerp->csystem	= 0;

    return 0;
}


#elif defined (__hp9000s700) || defined(solaris_2) || defined(_AIX) || defined(__linux__)

/***************************
 *****  HP-UX/Solaris  *****
 ***************************/

PublicFnDef int gettimer (
    struct timer *ptimer
)
{
    struct tms tms;
    unsigned long elapsed;
    static unsigned long elapsedBase = 0;
    static double clockTick;

    
    if (-1 == (int)(elapsed = times (&tms)))
        return -1;
    if (0 == elapsedBase)
    {
	elapsedBase = elapsed;
	clockTick = CLOCKS_PER_SEC;
    }

    ptimer->elapsed	= (int) ((elapsed - elapsedBase) / clockTick * 1000);
    ptimer->user	= (int) (tms.tms_utime / clockTick * 1000);
    ptimer->system	= (int) (tms.tms_stime / clockTick * 1000);
    ptimer->cuser	= (int) (tms.tms_cutime / clockTick * 1000);
    ptimer->csystem	= (int) (tms.tms_cstime / clockTick * 1000);

    return 0;
}


#elif defined(solaris_1)

/*******************
 *****  SunOS  *****
 *******************/

#ifndef HZ
#define HZ	60
#endif /* HZ */

PublicFnDef int gettimer (
    struct timer *ptimer
)
{
    static double	elapsedBase = 0.0;

    struct timeval	timeval;
    struct rusage	rusage;

    gettimeofday (&timeval, NilOf (struct timezone *));
    getrusage	 (RUSAGE_SELF, &rusage);

    if (elapsedBase <= 0.0)
    {
	elapsedBase	= (double)timeval.tv_sec  * 1e3
			+ (double)timeval.tv_usec / 1e3;
	ptimer->elapsed	= 0;
    }
    else ptimer->elapsed = (int)(
	(double)timeval.tv_sec  * 1e3 +
	(double)timeval.tv_usec / 1e3 -
	elapsedBase
    );
    ptimer->user	= (int) (
	(double)rusage.ru_utime.tv_sec  * 1e3 +
	(double)rusage.ru_utime.tv_usec / 1e3
    );
    ptimer->system	= (int) (
	(double)rusage.ru_stime.tv_sec  * 1e3 +
	(double)rusage.ru_stime.tv_usec / 1e3
    );
    ptimer->cuser	=
    ptimer->csystem	= 0;

    return 0;
}
#endif


/*************************
 *************************
 *****  Time Stamps  *****
 *************************
 *************************/

#if defined (_WIN32)

/************************
 *****  Windows/NT  *****
 ************************/

PublicFnDef int gettime (
    struct time			*timep
)
{
    struct _timeb		timedata;

    _ftime (&timedata);
    timep->seconds	= timedata.time;
    timep->microseconds	= timedata.millitm * 1000;

    return 0;
}

#else

/******************
 *****  Unix  *****
 ******************/

PublicFnDef int gettime (
    struct time *ptime
)
{
    struct timeval	timeval;
    struct timezone	timezone;

    if (gettimeofday (&timeval, &timezone) < 0)
        return -1;

    ptime->seconds	= timeval.tv_sec;
    ptime->microseconds	= timeval.tv_usec;

    return 0;
}
#endif


/**********************************
 **********************************
 *****  Formatting Utilities  *****
 **********************************
 **********************************/

#if defined(solaris_1)

PublicFnDef int STD_sprintf (
    char			*resultbuf,
    const char *		format,
    ...
)
{
    va_list	ap;
    int		result;

    va_start (ap, format);
    result = STD_vsprintf (resultbuf, format, ap);
    va_end (ap);

    return result;
}

PublicFnDef int STD_vsprintf (
    char			*resultbuf,
    const char *		format,
    va_list			ap
)
{
    return strlen(vsprintf (resultbuf, format, ap));
}

#endif


#if defined(__hp9000s700)

PublicFnDef int STD_sscanf (
    const char *		source,
    const char *		format,
    ...
)
{
    va_list			ap;
    char			sbuffer[1024];
    char *			srccpy;
    int				result;

    va_start (ap, format);

    if (source < (char *)0x80000000) return vsscanf ((char *)source, format, ap);

    if (strlen (source) < sizeof (sbuffer))
    {
	strcpy (sbuffer, source);
	return vsscanf (sbuffer, format, ap);
    }

    srccpy = strdup (source);
    result = vsscanf (srccpy, format, ap);
    free (srccpy);

    va_end (ap)

    return result;
}

#endif


/************************************
 ************************************
 *****  Byte Array Re-Ordering  *****
 ************************************
 ************************************/

PublicFnDef void Vk_ReverseArray (
    void *			pArray,
    size_t			sArray
)
{
    unsigned char *p1 = (unsigned char*)pArray;
    unsigned char *p2 = p1 + sArray - 1;
    unsigned char tchr;

    while (p1 < p2)
    {
	tchr  = *p1;
	*p1++ = *p2;
	*p2-- = tchr;
    }
}


/********************************************
 ********************************************
 *****  Balanced Binary Tree Utilities  *****
 ********************************************
 ********************************************/

/***********************
 *****  'bsearch'  *****
 ***********************/

#if 0
/*---------------------------------------------------------------------------
 *****  This routine is preserved from the VAX implementation to supply an
 *****  implementation if we ever need one.
 *---------------------------------------------------------------------------
 */
PublicFnDef void *STD_bsearch (
    const void			*key,
    const void			*base,
    size_t			nel,
    size_t			elsize,
    int				(*compar)(
				    const void *, const void *
				)
)
{
    void			*result;
    int				lb, ub, loc, comparResult;

    for (lb = (-1), ub = nel; ub - lb > 1; )
    {
	loc = (ub + lb) / 2;
	result = (void *)((char *)base + elsize * loc);
	comparResult = (*compar)(result, key);
	if (comparResult == 0) return result;
	if (comparResult < 0)
	    lb = loc;
	else ub = loc;
    }

/*****  Not found...  *****/
    return NilOf (void *);
}
#endif


/*********************
 *****  'qsort'  *****
 *********************/

#if 0
/*---------------------------------------------------------------------------
 *****  This routine is preserved from the VAX implementation to supply an
 *****  implementation if we ever need one.
 *---------------------------------------------------------------------------
 */
PublicFnDef void STD_qsort (
    void			*base,
    size_t			nel,
    size_t			elsize,
    int				(*compar)(
				    const void *, const void *
				)
)
{
    int sp, left, right, L, R, pivotIndex, stack[100];
    void *pivot, *temp;

    if (nel <= 1 || 0 == elsize) return;

    temp  = Vk_Allocate (elsize);
    pivot = Vk_Allocate (elsize);
    left = 0;  right = nel - 1;  sp = 0;
    stack[sp++] = left;  stack[sp++] = right;     /* push initial r/l pair */
    do
    {
	right = stack[--sp];  left = stack[--sp];	/*  pop r/l pair  */
	do
	{
	    L = left; R = right; pivotIndex = (L + R) / 2;
	    memcpy (pivot, (char *)base + pivotIndex * elsize, elsize);
	    do
	    {
		while ((*compar) ((char *)base + L * elsize,  pivot) < 0) L++;
		while ((*compar) ((char *)base + R * elsize,  pivot) > 0) R--;
		if (L <= R)
		{
		    memcpy (temp			, (const char *)base + L * elsize, elsize);
		    memcpy ((char *)base + L * elsize	, (const char *)base + R * elsize, elsize);
		    memcpy ((char *)base + R * elsize	, temp				 , elsize);
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
    Vk_Deallocate (temp);
    Vk_Deallocate (pivot);
}
#endif


/*****************************
 *****  'tsearch', etc.  *****
 *****************************/

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
PrivateFnDef int search (
    void const			*key,
    tnode			**rootp,
    int				(*compar)(void const *, void const *),
    int				*heightChange,
    tnode			**foundNodePtr
)
{
    int				comparisonResult;
    tnode			*p1, *p2;

/*****  key not in tree;  insert it  *****/
    if (IsNil (*rootp))
    {
	if (IsNil (*rootp = (tnode *)Vk_Allocate (sizeof (tnode))))
	    return -1;

	(*rootp)->data  = key;
	(*rootp)->bal   = 0;
	(*rootp)->left  = (*rootp)->right = NULL;
	*heightChange   = TRUE;
	*foundNodePtr   = *rootp;
	return 0;
    }

    /*****  search left subtree?  *****/
    comparisonResult = (*compar)(key, (*rootp)->data);
    if (comparisonResult < 0)
    {
	if (search (key, &(*rootp)->left, compar, heightChange, foundNodePtr) < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange)
	{
	/*****  balanceLeft (*rootp, heightChange);  *****/
	    switch ((*rootp)->bal)
	    {
	      case  1:
	        (*rootp)->bal =  0; *(heightChange) = FALSE;
	        break;
	      case  0:
		(*rootp)->bal = -1;
		break;
	      case -1:
		p1 = (*rootp)->left;
		if (p1->bal == -1)
		{				/* single LL rotation */
		    (*rootp)->left = p1->right;  p1->right = (*rootp);
		    (*rootp)->bal  = 0;	    (*rootp) = p1;
		}
		else
		{				/* double LR rotation */
		    p2 = p1->right;
		    p1->right = p2->left;  p2->left = p1;
		    (*rootp)->left = p2->right; p2->right = (*rootp);
		    (*rootp)->bal = (p2->bal == -1) ?   1 : 0;
		    p1->bal  = (p2->bal ==  1) ?  -1 : 0;
		    (*rootp) = p2;
		}
		(*rootp)->bal = 0;  *(heightChange) = FALSE;
		break;
	    }
	}
	return 0;
    }

/***** search right subtree? *****/

    if (comparisonResult > 0)
    {
	if (search (key, &(*rootp)->right, compar, heightChange, foundNodePtr) < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange)
	{
	/*****	    balanceRight (*rootp, heightChange);  *****/
	    switch ((*rootp)->bal)
	    {
	      case -1:
	        (*rootp)->bal =  0;  *(heightChange) = FALSE;
	        break;
	      case  0:
		(*rootp)->bal =  1;
		break;
	      case  1:
		p1 = (*rootp)->right;
		if (p1->bal ==  1)
		{				/* single RR rotation */
		    (*rootp)->right = p1->left;  p1->left = (*rootp);
		    (*rootp)->bal  = 0;	    (*rootp) = p1;
		}
		else
		{				/* double RL rotation */
		    p2 = p1->left;
		    p1->left = p2->right;  p2->right = p1;
		    (*rootp)->right = p2->left; p2->left = (*rootp);
		    (*rootp)->bal = (p2->bal ==  1) ? -1 : 0;
		    p1->bal  = (p2->bal == -1) ?  1 : 0;
		    (*rootp) = p2;
		}
		(*rootp)->bal = 0;  *(heightChange) = FALSE;
		break;
	    }
	}
	return 0;
    }

/*****  key found!  *****/
    *heightChange = FALSE;
    *foundNodePtr = *rootp;
    return 0;
}


#if defined(_WIN32)

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
PrivateFnDef void visitNode (
    tnode			*root,
    void			(*action)(struct treenode *, STD_VISIT, int),
    int				level
)
{
    level++;

    if (IsNil (root)) return;

    if (IsNil (root->left) && IsNil(root->right))
    {
	(*action) (root, STD_leaf, level);
	return;
    }

    (*action) (root, STD_preorder, level);

    visitNode (root->left, action, level);

    (*action) (root, STD_postorder, level);

    visitNode (root->right, action, level);

    (*action) (root, STD_endorder, level);
}
#endif


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
PublicFnDef struct treenode *STD_tsearch (
    void const *		key,
    void **			rootp,
    int				(*compar)(void const *, void const *)
)
{
    int heightChange = FALSE;
    tnode *foundNodep;

    if (search (key, (tnode **)rootp, compar, &heightChange, &foundNodep) < 0)
	return NilOf (struct treenode *);

    return foundNodep;
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
PublicFnDef struct treenode *STD_tfind (
    void const *		key,
    void **			rootp,
    int				(*compar)(void const *, void const *)
)
{
#if defined(_WIN32)
    tnode const			*node;
    int				comparisonResult;

    if (IsNil (rootp)) return NilOf (struct treenode *);

    node = (const tnode*)*rootp;
    while (IsntNil (node))
    {
	comparisonResult = (*compar)(key, node->data);
	if (0 == comparisonResult) return (struct treenode *)node;
	if (comparisonResult < 0) node = node->left;
	else if (comparisonResult > 0) node = node->right;
    }
    return NilOf (struct treenode *);
#else
    return (struct treenode *)tfind (key, rootp, compar);
#endif
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
PublicFnDef void STD_twalk (
    void const *		rootp,
    void			(*action)(struct treenode *, STD_VISIT, int)
)
{
#if defined (_WIN32)
    visitNode ((tnode *)rootp, action, -1);
#else
    twalk ((void *)rootp, (void (*)(void const *, VISIT, int))action);
#endif
}


/********************************
 ********************************
 *****  Option Acquisition  *****
 ********************************
 ********************************/

#if defined(_WIN32)

/************************
 *****  Windows/NT  *****
 ************************/

PublicVarDef char	*optarg;

PublicVarDef int	optind = 1,
			opterr;


/*--------------------------------------------------------------------------
 *  Emulation of HP-UX getopt().  see GETOPT(3C).
 *****/
PublicFnDef int getopt (
    int				argc,
    char			**argv,
    char			*optstring
)
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
 *****************************
 *****  Memory Movement  *****
 *****************************
 *****************************/

/*******************
 *****  SunOS  *****
 *******************/

#if defined (solaris_1)
PublicFnDef void *STD_memmove (
    void *			dst,
    const void *		src,
    unsigned int		n
)
{
    bcopy (src, dst, n);
    return dst;
}
#endif


/******************************
 ******************************
 *****  Directory Access  *****
 ******************************
 ******************************/

#if defined(_WIN32)

/************************
 *****  Windows/NT  *****
 ************************/

#define MaxConcurrentEnumerations	10

PrivateVarDef unsigned short MinUnusedEnumerator = 0;

PrivateVarDef DIR {
    HANDLE			searchHandle;
    int				searchDataValid;
    int				searchCompleted;
    WIN32_FIND_DATA		searchData;
} DirectoryEnumerator[MaxConcurrentEnumerations];

PublicFnDef DIR *opendir (
    char 			*directoryName
)
{
    unsigned short		enumeratorIndex;
    DIR				*enumerator;
    char			searchPattern[MAX_PATH];

    for (enumeratorIndex = 0; enumeratorIndex < MinUnusedEnumerator; enumeratorIndex++) if (
	INVALID_HANDLE_VALUE == DirectoryEnumerator[enumeratorIndex].searchHandle
    ) break;

    if (enumeratorIndex > MaxConcurrentEnumerations) return NilOf (DIR *);
    enumerator = &DirectoryEnumerator[enumeratorIndex];

    strcpy (searchPattern, directoryName);
    strcat (searchPattern, "\\*");
    enumerator->searchHandle = FindFirstFile (searchPattern, &enumerator->searchData);
    if (INVALID_HANDLE_VALUE == enumerator->searchHandle) return NilOf (DIR *);

    enumerator->searchDataValid = TRUE;
    enumerator->searchCompleted = FALSE;
    MinUnusedEnumerator = enumeratorIndex + 1;

    return enumerator;
}

PublicFnDef STD_DirectoryEntryType *readdir (
    DIR				*enumerator
)
{
    if (enumerator <  &DirectoryEnumerator[0]			||
	enumerator >= &DirectoryEnumerator[MinUnusedEnumerator]	||
	INVALID_HANDLE_VALUE == enumerator->searchHandle	||
	enumerator->searchCompleted
    ) return NilOf (STD_DirectoryEntryType *);

    if (!enumerator->searchDataValid) enumerator->searchDataValid = FindNextFile (
	enumerator->searchHandle, &enumerator->searchData
    );

    enumerator->searchCompleted = !enumerator->searchDataValid;
    enumerator->searchDataValid = FALSE;
    return enumerator->searchCompleted
	? NilOf (STD_DirectoryEntryType *)
	: (STD_DirectoryEntryType *)(enumerator->searchData.cFileName);
}

PublicFnDef void closedir (
    DIR				*enumerator
)
{
    if (enumerator <  &DirectoryEnumerator[0]			||
	enumerator >= &DirectoryEnumerator[MinUnusedEnumerator]	||
	INVALID_HANDLE_VALUE == enumerator->searchHandle
    ) return;
    FindClose (enumerator->searchHandle);
    enumerator->searchHandle = INVALID_HANDLE_VALUE;
}
#endif


/*****************************
 *****************************
 *****  Signal Handling  *****
 *****************************
 *****************************/

/************************
 *****  Windows/NT  *****
 ************************/

#if defined(_WIN32)

PrivateVarDef BOOL SignalVectorInvalid = TRUE;

PrivateVarDef struct sigvec SignalVector[NSIG];

PublicFnDef STD_maskType STD_sigblock (
    STD_maskType		bitmap
)
{
    return bitmap;
}

PublicFnDef STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
)
{
    return bitmap;
}

PublicFnDef int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
)
{
    if (SignalVectorInvalid)
    {
    	int i;
	for (i = 0; i < NSIG; i++)
	{
	    SignalVector[i].sv_handler	= (STD_SignalHandlerType)SIG_DFL;
	    SignalVector[i].sv_mask	=
	    SignalVector[i].sv_flags	=
	    SignalVector[i].sv_onstack	= 0;
	}
	SignalVectorInvalid = FALSE;
    }

    if (sig < 0 || sig >= NSIG)
    {
	errno = EINVAL;
	return -1;
    }

    if (IsntNil (ovec))
	*ovec = SignalVector[sig];

    if (IsntNil (ivec))
    {
	SignalVector[sig] = *ivec;
//	signal (sig, (void (__cdecl *)(int))(sig, ivec->sv_handler));
    }

    return 0;
}
#endif


/*************************
 *****  AIX/Solaris  *****
 *************************/

#if defined(_AIX) || defined(sun) || defined(__linux__)
PrivateFnDef void FillSetFromBitMap (
    sigset_t			*set,
    STD_maskType		bitmap
)
{
    int				i;

    sigemptyset (set);
    for (i = 1; i < 32; i++) if (bitmap & (1 << i - 1)) sigaddset (set, i);
}

PrivateFnDef void FillBitMapFromSet (
    sigset_t			*set,
    STD_maskType 		*bitmap
)
{
    int				i;

    *bitmap = 0;
    for (i = 1; i < 32; i++) if (sigismember (set, i)) *bitmap |= 1 << i - 1;
}

PublicFnDef STD_maskType STD_sigblock (
    STD_maskType		bitmap
)
{
    sigset_t			iset, oset;

    FillSetFromBitMap (&iset, bitmap);

    if (sigprocmask (SIG_BLOCK, &iset, &oset) < 0) return 0;

    FillBitMapFromSet (&oset, &bitmap);

    return bitmap;
}

PublicFnDef STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
)
{
    sigset_t			iset, oset;

    FillSetFromBitMap (&iset, bitmap);

    if (sigprocmask (SIG_SETMASK, &iset, &oset) < 0) return 0;

    FillBitMapFromSet (&oset, &bitmap);

    return bitmap;
}

PublicFnDef int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
)
{
    struct sigaction		iact, oact, *iactp;

    if (IsntNil (ivec))
    {
	iact.sa_handler	= (void(*)(int))ivec->sv_handler;
#if defined(_AIX) || defined(solaris_2)
	iact.sa_flags	= SA_RESTART | SA_NOCLDSTOP;
#else
	iact.sa_flags	= SA_NOCLDSTOP;
#endif

	FillSetFromBitMap (&iact.sa_mask, ivec->sv_mask);

	iactp = &iact;
    }
    else iactp = NilOf (struct sigaction *);

    if (sigaction (sig, iactp, &oact) < 0) return -1;

    if (IsntNil (ovec))
    {
	ovec->sv_handler	= oact.sa_handler;
	FillBitMapFromSet (&oact.sa_mask, &ovec->sv_mask);
    }

    return 0;
}
#endif


/*******************
 *******************
 *****  Other  *****
 *******************
 *******************/

#if defined(_WIN32)

PublicFnDef int Vk_alarm (
    size_t			sInterval
)
{
    return 0;
}

PublicFnDef char* Vk_cuserid (
    char*			pBuffer
)
{
    static char iBuffer[256];
    DWORD	sBuffer = sizeof (iBuffer);
    return GetUserName (iBuffer, &sBuffer) ? iBuffer : "UNKNOWN";
}

PublicFnDef int Vk_link (
    const char*			pSrcPath,
    const char*			pDstPath
)
{
    return rename (pSrcPath, pDstPath);
}

PublicFnDef void Vk_remove (
    const char*			pFileName
)
{
    SetFileAttributes (pFileName, FILE_ATTRIBUTE_NORMAL);
    DeleteFile (pFileName);
}

PublicFnDef size_t Vk_sleep (
    size_t			sInterval
)
{
    static HANDLE hSleepEvent = INVALID_HANDLE_VALUE;
    if (SystemHandleIsInvalid (hSleepEvent))
	hSleepEvent = CreateEvent (NULL, TRUE, FALSE, NULL);

    if (SystemHandleIsInvalid (hSleepEvent))
	return sInterval;

    return WAIT_TIMEOUT != WaitForSingleObject (hSleepEvent, sInterval * 1000)
    ? sInterval
    : 0;
}

#endif

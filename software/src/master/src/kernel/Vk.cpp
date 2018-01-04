/*****  Vision Kernel Implementation  *****/

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"
#include "VTransient.h"

#if defined(_WIN32)
#pragma comment (lib, "advapi32.lib")
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

V_API int gettime (struct time *pTime) {
    struct _timeb timedata;

    _ftime (&timedata);
    pTime->seconds	= timedata.time;
    pTime->microseconds	= timedata.millitm * 1000;

    return 0;
}

class CriticalSection : public CRITICAL_SECTION {
//  Construction
public:
    CriticalSection () {
	InitializeCriticalSection (this);
    }

//  Destruction
public:
    ~CriticalSection () {
	DeleteCriticalSection (this);
    }

//  Use
public:
    void Enter () {
	EnterCriticalSection (this);
    }
    void Leave () {
	LeaveCriticalSection (this);
    }
};

static CriticalSection asctimeCS, ctimeCS, gmtimeCS, localtimeCS;

V_API char *asctime_r (struct tm const *pTime, char *pReturn) {	// ASSERT(sizeof (*pReturn) >= 26);
    CriticalSection &rCS = asctimeCS;
    char *pResult = 0;
    __try {
	rCS.Enter ();
	pResult = asctime (pTime);
	if (pResult) {
	    strncpy (pReturn, pResult, 26);
	    pReturn[25] = '\0';
	}
    } __finally {
	rCS.Leave ();
    }
    return pResult ? pReturn : 0;
}

V_API char *ctime_r (time_t const *pTime, char *pReturn) {	// ASSERT(sizeof (*pReturn) >= 26);
    CriticalSection &rCS = ctimeCS;
    char *pResult = 0;
    __try {
	rCS.Enter ();
	pResult = ctime (pTime);
	if (pResult) {
	    strncpy (pReturn, pResult, 26);
	    pReturn[25] = '\0';
	}
    } __finally {
	rCS.Leave ();
    }
    return pResult ? pReturn : 0;
}

V_API struct tm *gmtime_r (time_t const *pTime, struct tm *pReturn) {
    CriticalSection &rCS = gmtimeCS;
    struct tm *pResult = 0;
    __try {
	rCS.Enter ();
	pResult = gmtime (pTime);
	if (pResult)
	    *pReturn = *pResult;
    } __finally {
	rCS.Leave ();
    }
    return pResult ? pReturn : 0;
}

V_API struct tm *localtime_r (time_t const *pTime, struct tm *pReturn) {
    CriticalSection &rCS = localtimeCS;
    struct tm *pResult = 0;
    __try {
	rCS.Enter ();
	pResult = localtime (pTime);
	if (pResult)
	    *pReturn = *pResult;
    } __finally {
	rCS.Leave ();
    }
    return pResult;
}

#elif defined(__VMS)

/*********************
 *****  OpenVMS  *****
 *********************/

PublicFnDef V_API int gettime (struct time *pTime) {
    struct timeval	timeval;

    if (gettimeofday (&timeval, 0) < 0)
        return -1;

    pTime->seconds	= timeval.tv_sec;
    pTime->microseconds	= timeval.tv_usec;

    return 0;
}

#else

/******************
 *****  Unix  *****
 ******************/

PublicFnDef V_API int gettime (struct time *pTime) {
    struct timeval	timeval;
    struct timezone	timezone;

    if (gettimeofday (&timeval, &timezone) < 0)
        return -1;

    pTime->seconds	= timeval.tv_sec;
    pTime->microseconds	= timeval.tv_usec;

    return 0;
}
#endif


/**********************************
 **********************************
 *****  Formatting Utilities  *****
 **********************************
 **********************************/

#if defined(__hp9000s700)

PublicFnDef int STD_sscanf (char const* source, char const* format, ...) {
    V_VARGLIST (ap, format);

    if (source < (char *)0x80000000)
	return vsscanf ((char *)source, format, ap);

    char sbuffer[1024];
    if (strlen (source) < sizeof (sbuffer)) {
	strcpy (sbuffer, source);
	return vsscanf (sbuffer, format, ap);
    }

    char *srccpy = strdup (source);
    int const result = vsscanf (srccpy, format, ap);
    free (srccpy);

    return result;
}

#endif


/************************************
 ************************************
 *****  Byte Array Re-Ordering  *****
 ************************************
 ************************************/

PublicFnDef V_API void Vk_ReverseArray (void *pElements, size_t sElement, size_t cElements) {
    switch (sElement) {
    case 0:
    case 1:
	break;
    case 2: {
	    typedef unsigned short element_t;
	    element_t *pElement = reinterpret_cast<element_t*> (pElements);
	    while (cElements-- > 0) {
		element_t iElement = *pElement;
		*pElement++ =
		    ((iElement & static_cast<element_t>(0x00ff)) << 8) +
		    ((iElement & static_cast<element_t>(0xff00)) >> 8);
	    }
	} break;
    case 4: {
	    typedef unsigned int element_t;
	    element_t *pElement = reinterpret_cast<element_t*> (pElements);
	    while (cElements-- > 0) {
		element_t iElement = *pElement;
	    //  First, swap adjacent bytes, ...
		iElement =
		    ((iElement & static_cast<element_t>(0x00ff00ff)) << 8) +
		    ((iElement & static_cast<element_t>(0xff00ff00)) >> 8) ;
	    //  ... and then adjacent shorts to produce the result:
		*pElement++ =
		    ((iElement & static_cast<element_t>(0x0000ffff)) << 16) +
		    ((iElement & static_cast<element_t>(0xffff0000)) >> 16) ;
	    }
	} break;
    case 8: {
	    typedef unsigned __int64 element_t;
	    element_t *pElement = reinterpret_cast<element_t*> (pElements);
	    while (cElements-- > 0) {
		element_t iElement = *pElement;
	    //  First, swap adjacent bytes, ...
		iElement =
		    ((iElement & static_cast<element_t>(0x00ff00ff00ff00ffLL)) << 8) +
		    ((iElement & static_cast<element_t>(0xff00ff00ff00ff00LL)) >> 8) ;
	    //  ... then adjacent shorts, ...
		iElement =
		    ((iElement & static_cast<element_t>(0x0000ffff0000ffffLL)) << 16) +
		    ((iElement & static_cast<element_t>(0xffff0000ffff0000LL)) >> 16) ;
	    //  .. and finally adjacent 'ints' to produce the result:
		*pElement++ =
		    ((iElement & static_cast<element_t>(0x00000000ffffffffLL)) << 32) +
		    ((iElement & static_cast<element_t>(0xffffffff00000000LL)) >> 32) ;
	    }
	} break;
    default: {
	    unsigned char *pElement = reinterpret_cast<unsigned char*>(pElements);
	    while (cElements-- > 0) {
		unsigned char *p1 = pElement;
		unsigned char *p2 = p1 + sElement - 1;
		while (p1 < p2) {
		    unsigned char const tchr  = *p1;
		    *p1++ = *p2;
		    *p2-- = tchr;
		}
		pElement += sElement;
	    }
	} break;
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
    void const *key, void const *base, size_t nel, size_t elsize, VkComparator compar
) {
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
PublicFnDef void STD_qsort (void *base, size_t nel, size_t elsize, VkComparator compar) {
    int sp, left, right, L, R, pivotIndex, stack[100];
    void *pivot, *temp;

    if (nel <= 1 || 0 == elsize) return;

    temp  = VTransient::allocate (elsize);
    pivot = VTransient::allocate (elsize);
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
		    memcpy (temp			, (char const*)base + L * elsize, elsize);
		    memcpy ((char *)base + L * elsize	, (char const*)base + R * elsize, elsize);
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
    VTransient::deallocate (temp);
    VTransient::deallocate (pivot);
}
#endif


/*****************************
 *****  'tsearch', etc.  *****
 *****************************/

#if defined(__VMS) || defined(_WIN32)
#define USING_OWN_TREE_ROUTINES
#endif

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
    void const *key, treenode **rootp, VkComparator compar, bool *heightChange, treenode **foundNodePtr
) {
    int				comparisonResult;
    treenode			*p1, *p2;

/*****  key not in tree;  insert it  *****/
    if (IsNil (*rootp)) {
	if (IsNil (*rootp = (treenode*)VTransient::allocate (sizeof (treenode))))
	    return -1;

	(*rootp)->data  = key;
	(*rootp)->bal   = 0;
	(*rootp)->left  = (*rootp)->right = NULL;
	*heightChange   = true;
	*foundNodePtr   = *rootp;
	return 0;
    }

    /*****  search left subtree?  *****/
    comparisonResult = (*compar)(key, (*rootp)->data);
    if (comparisonResult < 0) {
	if (search (key, &(*rootp)->left, compar, heightChange, foundNodePtr) < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange) {
	/*****  balanceLeft (*rootp, heightChange);  *****/
	    switch ((*rootp)->bal) {
	      case  1:
	        (*rootp)->bal =  0; *heightChange = false;
	        break;
	      case  0:
		(*rootp)->bal = -1;
		break;
	      case -1:
		p1 = (*rootp)->left;
		if (p1->bal == -1) {		/* single LL rotation */
		    (*rootp)->left = p1->right;  p1->right = (*rootp);
		    (*rootp)->bal  = 0;	    (*rootp) = p1;
		}
		else {				/* double LR rotation */
		    p2 = p1->right;
		    p1->right = p2->left;  p2->left = p1;
		    (*rootp)->left = p2->right; p2->right = (*rootp);
		    (*rootp)->bal = (treenode::bal_t)(p2->bal == -1 ? 1 : 0);
		    p1->bal  = (treenode::bal_t)(p2->bal ==  1 ?  -1 : 0);
		    (*rootp) = p2;
		}
		(*rootp)->bal = 0;  *heightChange = false;
		break;
	    }
	}
	return 0;
    }

/***** search right subtree? *****/

    if (comparisonResult > 0) {
	if (search (key, &(*rootp)->right, compar, heightChange, foundNodePtr) < 0)
	    return -1;  			/* error in malloc? */

	if (*heightChange) {
	/*****	    balanceRight (*rootp, heightChange);  *****/
	    switch ((*rootp)->bal) {
	      case -1:
	        (*rootp)->bal =  0;  *heightChange = false;
	        break;
	      case  0:
		(*rootp)->bal =  1;
		break;
	      case  1:
		p1 = (*rootp)->right;
		if (p1->bal ==  1) {		/* single RR rotation */
		    (*rootp)->right = p1->left;  p1->left = (*rootp);
		    (*rootp)->bal  = 0;	    (*rootp) = p1;
		}
		else {				/* double RL rotation */
		    p2 = p1->left;
		    p1->left = p2->right;  p2->right = p1;
		    (*rootp)->right = p2->left; p2->left = (*rootp);
		    (*rootp)->bal = (treenode::bal_t)(p2->bal ==  1 ? -1 : 0);
		    p1->bal  = (treenode::bal_t)(p2->bal == -1 ?  1 : 0);
		    (*rootp) = p2;
		}
		(*rootp)->bal = 0;  *heightChange = false;
		break;
	    }
	}
	return 0;
    }

/*****  key found!  *****/
    *heightChange = false;
    *foundNodePtr = *rootp;
    return 0;
}


#if defined(USING_OWN_TREE_ROUTINES)

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
    treenode *root, void (*action)(treenode *, STD_VISIT, int), int level
) {
    level++;

    if (IsNil (root))
	return;

    if (IsNil (root->left) && IsNil(root->right)) {
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
PublicFnDef V_API treenode *STD_tsearch (void const *key, void **rootp, VkComparator compar) {
    bool heightChange = false;
    treenode *foundNodep;

    if (search (key, (treenode**)rootp, compar, &heightChange, &foundNodep) < 0)
	return NilOf (treenode *);

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
PublicFnDef V_API treenode *STD_tfind (void const *key, void **rootp, VkComparator compar) {
#if defined(USING_OWN_TREE_ROUTINES)
    if (IsNil (rootp))
	return NilOf (treenode *);

    treenode const *node = (treenode const*)*rootp;
    while (IsntNil (node)) {
	int comparisonResult = (*compar)(key, node->data);
	if (0 == comparisonResult)
	    return (treenode *)node;
	if (comparisonResult < 0)
	    node = node->left;
	else if (comparisonResult > 0)
	    node = node->right;
    }
    return NilOf (treenode *);
#else
    return (treenode *)tfind (key, rootp, compar);
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
PublicFnDef V_API void STD_twalk (
    void const *rootp, void (*action)(treenode *, STD_VISIT, int)
)
{
#if defined (USING_OWN_TREE_ROUTINES)
    visitNode ((treenode*)rootp, action, -1);
#else
    twalk ((void *)rootp, (void (*)(void const *, VISIT, int))action);
#endif
}


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

PrivateVarDef struct DIR {
    HANDLE			searchHandle;
    int				searchDataValid;
    int				searchCompleted;
    WIN32_FIND_DATA		searchData;
} DirectoryEnumerator[MaxConcurrentEnumerations];

PublicFnDef V_API DIR *opendir (char const* directoryName) {
    unsigned short		enumeratorIndex;
    DIR				*enumerator;
    char			searchPattern[MAX_PATH];

    for (enumeratorIndex = 0; enumeratorIndex < MinUnusedEnumerator; enumeratorIndex++)
	if (INVALID_HANDLE_VALUE == DirectoryEnumerator[enumeratorIndex].searchHandle)
	    break;

    if (enumeratorIndex > MaxConcurrentEnumerations)
	return NilOf (DIR *);
    enumerator = &DirectoryEnumerator[enumeratorIndex];

    strcpy (searchPattern, directoryName);
    strcat (searchPattern, "\\*");
    enumerator->searchHandle = FindFirstFile (searchPattern, &enumerator->searchData);
    if (INVALID_HANDLE_VALUE == enumerator->searchHandle)
	return NilOf (DIR *);

    enumerator->searchDataValid = true;
    enumerator->searchCompleted = false;
    MinUnusedEnumerator = enumeratorIndex + 1;

    return enumerator;
}

PublicFnDef V_API STD_DirectoryEntryType *readdir (DIR *enumerator) {
    if (enumerator <  &DirectoryEnumerator[0]			||
	enumerator >= &DirectoryEnumerator[MinUnusedEnumerator]	||
	INVALID_HANDLE_VALUE == enumerator->searchHandle	||
	enumerator->searchCompleted
    ) return NilOf (STD_DirectoryEntryType *);

    if (!enumerator->searchDataValid) enumerator->searchDataValid = FindNextFile (
	enumerator->searchHandle, &enumerator->searchData
    );

    enumerator->searchCompleted = !enumerator->searchDataValid;
    enumerator->searchDataValid = false;
    return enumerator->searchCompleted
	? NilOf (STD_DirectoryEntryType *)
	: (STD_DirectoryEntryType *)(enumerator->searchData.cFileName);
}

PublicFnDef V_API void closedir (DIR *enumerator) {
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

PrivateVarDef BOOL SignalVectorInvalid = true;

PrivateVarDef struct sigvec SignalVector[NSIG];

PublicFnDef V_API STD_maskType STD_sigblock (STD_maskType bitmap) {
    return bitmap;
}

PublicFnDef V_API STD_maskType STD_sigsetmask (STD_maskType bitmap) {
    return bitmap;
}

PublicFnDef V_API int STD_sigvector (int sig, struct sigvec *ivec, struct sigvec *ovec) {
    if (SignalVectorInvalid) {
    	int i;
	for (i = 0; i < NSIG; i++) {
	    SignalVector[i].sv_handler	= (STD_SignalHandlerType)SIG_DFL;
	    SignalVector[i].sv_mask	=
	    SignalVector[i].sv_flags	=
	    SignalVector[i].sv_onstack	= 0;
	}
	SignalVectorInvalid = false;
    }

    if (sig < 0 || sig >= NSIG) {
	errno = EINVAL;
	return 0; // if this function actually did anything, would return -1
    }

    if (IsntNil (ovec))
	*ovec = SignalVector[sig];

    if (IsntNil (ivec)) {
	SignalVector[sig] = *ivec;
//	signal (sig, (void (__cdecl *)(int))(sig, ivec->sv_handler));
    }

    return 0;
}
#endif


/*******************************
 *****  The 'nix and 'nux  *****
 *******************************/

#if defined(_AIX) || defined(sun) || defined(__linux__) || defined(__APPLE__)
PrivateFnDef void FillSetFromBitMap (sigset_t *set, STD_maskType bitmap) {
    sigemptyset (set);
    for (int i = 1; i < 32; i++)
	if (bitmap & (1 << i - 1))
	    sigaddset (set, i);
}

PrivateFnDef void FillBitMapFromSet (sigset_t *set, STD_maskType *bitmap) {
    *bitmap = 0;
    for (int i = 1; i < 32; i++)
	if (sigismember (set, i))
	    *bitmap |= 1 << i - 1;
}

PublicFnDef STD_maskType STD_sigblock (STD_maskType bitmap) {
    sigset_t iset, oset;

    FillSetFromBitMap (&iset, bitmap);

    if (sigprocmask (SIG_BLOCK, &iset, &oset) < 0)
	return 0;

    FillBitMapFromSet (&oset, &bitmap);

    return bitmap;
}

PublicFnDef STD_maskType STD_sigsetmask (STD_maskType bitmap) {
    sigset_t			iset, oset;

    FillSetFromBitMap (&iset, bitmap);

    if (sigprocmask (SIG_SETMASK, &iset, &oset) < 0)
	return 0;

    FillBitMapFromSet (&oset, &bitmap);

    return bitmap;
}

PublicFnDef int STD_sigvector (int sig, struct sigvec *ivec, struct sigvec *ovec) {
    struct sigaction iact, oact, *iactp;

    if (IsntNil (ivec)) {
	iact.sa_handler	= (void(*)(int))ivec->sv_handler;
#if defined(_AIX) || defined(sun) || defined(__linux__) || defined(__APPLE__)
	iact.sa_flags	= SA_RESTART | SA_NOCLDSTOP;
#else
	iact.sa_flags	= SA_NOCLDSTOP;
#endif

	FillSetFromBitMap (&iact.sa_mask, ivec->sv_mask);

	iactp = &iact;
    }
    else iactp = NilOf (struct sigaction *);

    if (sigaction (sig, iactp, &oact) < 0)
	return -1;

    if (IsntNil (ovec)) {
	ovec->sv_handler	= (STD_SignalHandlerType)oact.sa_handler;
	FillBitMapFromSet (&oact.sa_mask, &ovec->sv_mask);
    }

    return 0;
}
#endif


/*************************
 *************************
 *****  'gethostid'  *****
 *************************
 *************************/

PublicFnDef V_API long Vk_gethostid () {
#if defined(_WIN32)
    DWORD iVolumeSerialNumber;
    return GetVolumeInformation (
	"c:/", 0, 0, &iVolumeSerialNumber, 0, 0, 0, 0
    ) ? iVolumeSerialNumber : UINT_MAX;

#elif defined(__hp9000s800) || defined (__hp9000s700)
    struct utsname name;
    long hostid = -1;

    uname (&name);
    sscanf (name.idnumber, "%lu", &hostid);
    return hostid;

#elif defined (sun)
    char serialNumber[16];
    sysinfo (SI_HW_SERIAL, serialNumber, sizeof(serialNumber));
    return atoi (serialNumber);

#elif defined(__APPLE__)
    /*****************
     *
     *  'gethostid/hostid' is deprecated in BSD and its derivatives.  Unless someone has
     *  taken the time to set it explicitly (unlikely), the value returned on MacOS will
     *  be 0.  If that's the case, return a wired non-zero value to keep scripts that
     *  interpret 0 as meaning this is VMS (totally bogus, ARRRRRRGH).
     *
     ****************/
    int32_t iHostID = 0;
    size_t  sHostID = sizeof (iHostID);
    return sysctlbyname ("kern.hostid", &iHostID, &sHostID, NULL, 0) == 0 && iHostID ? iHostID : 987654321;

#else
    return 0;
#endif
}


/**********************
 **********************
 *****  'finite'  *****
 **********************
 **********************/

#if defined(__hp9000s800) || defined(__hp9000s700)
PublicFnDef int Vk_finite (double x) {
#undef finite
    return (-HUGE_VAL != x && finite (x));
}
#endif


/*******************
 *******************
 *****  Other  *****
 *******************
 *******************/

#if defined(_WIN32)
PublicFnDef V_API char const *Vk_username (char *pBuffer, size_t sBuffer) {
    DWORD dwsBuffer = sBuffer;
    return GetUserName (pBuffer, &dwsBuffer) ? pBuffer : "UNKNOWN";
}

PublicFnDef V_API int Vk_alarm (size_t sInterval) {
    return 0;
}

PublicFnDef V_API void Vk_remove (char const *pFileName) {
    SetFileAttributes (pFileName, FILE_ATTRIBUTE_NORMAL);
    DeleteFile (pFileName);
}

class EventHandle {
public:
    EventHandle () : m_hEvent (CreateEvent (NULL, true, false, NULL)) {
    }
    ~EventHandle () {
	CloseHandle (m_hEvent);
    }
    bool isValid () const {
	return SystemHandleIsValid (m_hEvent);
    }
    bool isInvalid () const {
	return SystemHandleIsInvalid (m_hEvent);
    }
    operator HANDLE () const {
	return m_hEvent;
    }
private:
    HANDLE m_hEvent;
};

PublicFnDef V_API size_t Vk_sleep (size_t sInterval) {
    static EventHandle iEventHandle;

    return iEventHandle.isInvalid () || WAIT_TIMEOUT != WaitForSingleObject (
	iEventHandle, sInterval * 1000
    ) ? sInterval : 0;
}
#else
#include <pwd.h>

PublicFnDef V_API char const *Vk_username (char *pBuffer, size_t sBuffer) {
#ifdef __VMS
    typedef struct __passwd64 passwd_t;
#else
    typedef struct passwd passwd_t;
#endif
    passwd_t iPasswd, *pPasswd = 0;
    return getpwuid_r (getuid (), &iPasswd, pBuffer, sBuffer, &pPasswd) == 0 && pPasswd ? pPasswd->pw_name : "UNKNOWN";
}

#endif

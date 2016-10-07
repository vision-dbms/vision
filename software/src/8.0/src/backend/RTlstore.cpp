/*****  List Store Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtLSTORE

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "vdbupdate.h"
#include "vdsc.h"
#include "verr.h"
#include "venvir.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"

/*****  Self  *****/
#include "RTlstore.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtLSTORE_Handle);


/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool
    TracingCalls		= false;


/****************************
 *****  Usage Counters  *****
 ****************************/

PrivateVarDef unsigned int
    NewCount			= 0,
    NewUsingLCInitCount		= 0,
    NewStringStoreCount		= 0,
    NewStringStoreFromUVCount	= 0,

    AddListsCount		= 0,
    AlignCount			= 0,
    AlignFromContentCount	= 0,
    AlignUsingLCListsCount	= 0,
    AlignUsingRefListCount	= 0,

    RefListElementCount		= 0,
    LCListElementCount		= 0,

    LCExtractCount		= 0,
    RefExtractCount		= 0,
    LCExtractReallocCount 	= 0,

    LCSubscriptCount		= 0,
    RefSubscriptCount		= 0,

    AppendToLCListCount		= 0,
    AppendToRefListCount	= 0;


/**********************************
 *****  Consistency Checkers  *****
 **********************************/

/*---------------------------------------------------------------------------
 * A corrupted lstore discovered at EMR inspired the creation of this bit of
 * paranoia. The corruption could never be duplicated, so now the Public
 * functions of RTlstore check for corruption (some via calls to other
 * functions like rtLSTORE_Align
 *---------------------------------------------------------------------------
 */

#define FinalBreakpoint(cpd) rtLSTORE_CPD_BreakpointArray (cpd)[\
    rtLSTORE_CPD_BreakpointCount (cpd)\
]

PrivateFnDef void RaiseContentBreakpointDiscrepancyException () {
    ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE: Content/Breakpoint size disagreement."
    );
}

#define ValidateBreakpointConsistency(lstore) {\
    if (FinalBreakpoint (lstore) != rtPTOKEN_BaseElementCount(\
	    lstore, rtLSTORE_CPx_ContentPToken\
	)\
    ) RaiseContentBreakpointDiscrepancyException ();\
}

void rtLSTORE_Handle::CheckConsistency () {
    if (rtLSTORE_LStore_IsInconsistent ((rtLSTORE_LStore*)ContainerContent ())
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "Corrupted lstore[%u:%u] detected", SpaceIndex (), ContainerIndex ()
	)
    );
}


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a standard L-Store CPD.
 *
 *  Argument:
 *	cpd			- the address of the CPD to be initialized.
 *
 *  Returns:
 *	'cpd' - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD *cpd) {
    rtLSTORE_LStore* base = rtLSTORE_CPD_Base (cpd);

    rtLSTORE_CPD_RowPToken	(cpd) = &rtLSTORE_LStore_RowPToken	(base);
    rtLSTORE_CPD_Content	(cpd) = &rtLSTORE_LStore_Content	(base);
    rtLSTORE_CPD_ContentPToken	(cpd) = &rtLSTORE_LStore_ContentPToken	(base);
    rtLSTORE_CPD_Breakpoint	(cpd) =  rtLSTORE_LStore_BreakpointArray(base);
}


/***************************
 *****  Instantiators  *****
 ***************************/

/**************************************
 *  Internal Initialization Routines  *
 **************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize the contents of a new L-Store.
 *
 *  Arguments:
 *	cpd			- the address of an uninitialized standard
 *				  CPD for the L-Store being created.
 *	ap			- a <varargs.h> argument pointer referencing
 *				  a standard CPD for the positional P-Token
 *				  for the new L-Store (the number of elements
 *				  in the breakpoint array is one more than the
 *				  base element count of this P-Token), an
 *				  optional link constructor for link driven
 *				  initialization, and an optional string space
 *				  pointer for char-uv driven initialization.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitNewLStore (
    M_CPD *cpd, M_CPD *ptoken, rtLINK_CType *linkc, char const* pStringStorage
) {
    unsigned int breakpointCount = rtPTOKEN_CPD_BaseElementCount (ptoken);

/*****  Initialize the CPD, ...  *****/
    rtLSTORE_CPD_IsInconsistent (cpd) = true;
    InitStdCPD (cpd);

/*****  ...the POPs, ...  *****/
    cpd->constructReference (rtLSTORE_CPx_RowPToken);
    cpd->StoreReference (rtLSTORE_CPx_RowPToken, ptoken);

    cpd->constructReference (rtLSTORE_CPx_Content);
    cpd->constructReference (rtLSTORE_CPx_ContentPToken);

/*****  ...the string store flag, ...  *****/
    rtLSTORE_CPD_StringStore (cpd) = false;

/*****  ...the breakpoint count, ...  *****/
    rtLSTORE_CPD_BreakpointCount (cpd) = breakpointCount;

/*****  ...and the breakpoint array...  *****/
    if (linkc) {
	/*****  ... using the link content map: *****/
	unsigned int totalElementsViewed = 0;
	unsigned int *breakpointArray = rtLSTORE_CPD_BreakpointArray (cpd);
	unsigned int *breakpoint = breakpointArray;

#	define fillSkippedElements(through, with) {\
	    unsigned int *p;\
	    for (p = breakpointArray + (through);\
		 breakpoint < p;\
		 *breakpoint++ = (with));\
	}

#	define nilReferenceHandler(c, n, r)\
	fillSkippedElements (r + 1, totalElementsViewed)


#	define repeatedReferenceHandler(c, n, r)\
	fillSkippedElements (r, totalElementsViewed)\
	*breakpoint++ = totalElementsViewed;\
	totalElementsViewed += n

#	define rangeReferenceHandler(c, n, r)\
	fillSkippedElements (r, totalElementsViewed)\
	while (n-- > 0) {\
	    *breakpoint++ = totalElementsViewed++;\
	}

	rtLINK_TraverseRRDCList (
	    linkc,
	    nilReferenceHandler,
	    repeatedReferenceHandler,
	    rangeReferenceHandler
	);
	fillSkippedElements (breakpointCount + 1, totalElementsViewed);

#	undef fillSkippedElements
#	undef nilReferenceHandler
#	undef repeatedReferenceHandler
#	undef rangeReferenceHandler
    }
    else if (pStringStorage) {
	/*****  ... using the string content: *****/
	unsigned int *pBreakpointArray = rtLSTORE_CPD_BreakpointArray (cpd);
	unsigned int xBreakpoint = 0;
	unsigned int iBreakpoint = 0;

	while (xBreakpoint < breakpointCount) {
	    pBreakpointArray[xBreakpoint++] = iBreakpoint;
	    iBreakpoint += strlen (pStringStorage + iBreakpoint) + 1;
	}

	pBreakpointArray[xBreakpoint] = iBreakpoint;
    }
    else {
	/*****  ... using the p-token's cardinality:  *****/
	memset (
	    (void *)rtLSTORE_CPD_BreakpointArray (cpd),
	    0,
	    (breakpointCount + 1) * sizeof (unsigned int)
	);
    }
}


/**********************************
 *  Instantiator Implementations  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  General L-Store Cluster Creation Routine
 *
 *  Arguments:
 *	pInstancePPT	- the address of a CPD for the new cluster's
 *				  instance set p-token.
 *	pContentPrototypeCPD	- the address of a CPD for the new cluster's
 *				  content prototype.
 *	--			- a dummy argument used to disambiguate the
 *				  overloading of rtLSTORE_NewCluster.
 *
 *  Returns:
 *	A standard CPD for the L-Store created.
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_NewCluster (
    M_CPD *pInstancePPT, M_CPD *pContentPrototypeCPD, int
)
{
    NewCount++;

/*****  Verify that 'pInstancePPT' is a p-token...  *****/
    RTYPE_MustBeA (
	"rtLSTORE_NewCluster", M_CPD_RType (pInstancePPT), RTYPE_C_PToken
    );

/*****  Create and initialize the L-Store's content...  *****/
    M_ASD *pContainerSpace = pInstancePPT->Space ();
    M_CPD *pContentPTokenCPD = rtPTOKEN_New (pContainerSpace, 0);
    M_CPD *pContentCluster = IsntNil (pContentPrototypeCPD) ? rtVSTORE_NewCluster (
	pContentPTokenCPD, pContentPrototypeCPD
    ) : rtVECTOR_New (pContentPTokenCPD);

/*****  Create and initialize the L-Store...  *****/
    M_CPD *pNewCluster = pContainerSpace->CreateContainer (
	RTYPE_C_ListStore,
	rtLSTORE_SizeofLStore (rtPTOKEN_CPD_BaseElementCount (pInstancePPT))
    );

    InitNewLStore (
	pNewCluster, pInstancePPT, NilOf (rtLINK_CType*), NilOf (char const*)
    );

    pNewCluster->StoreReference (
	rtLSTORE_CPx_ContentPToken, pContentPTokenCPD
    )->StoreReference (
	rtLSTORE_CPx_Content, pContentCluster
    );
    ValidateBreakpointConsistency (pNewCluster);
    rtLSTORE_CPD_StringStore (pNewCluster) = RTYPE_C_CharUV == (RTYPE_Type)M_CPD_RType (
	pContentCluster
    );
    rtLSTORE_CPD_IsInconsistent (pNewCluster) = false;

/*****  Cleanup...  *****/
    pContentCluster->release ();
    pContentPTokenCPD->release ();

/*****  Return...  *****/
    return pNewCluster;
}


/*---------------------------------------------------------------------------
 *****  Cloned L-Store Cluster Creation Routine.
 *
 *  Arguments:
 *	pInstancePPT	- the address of a CPD for the new cluster's
 *				  instance set p-token.
 *	pPrototypeIndexCPD	- the address of a CPD for the cluster's
 *				  new prototype.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_NewCluster (
    M_CPD *pInstancePPT, M_CPD *pPrototypeStoreCPD
) {
    M_CPD *pContentPrototypeCPD = rtLSTORE_CPD_ContentCPD (pPrototypeStoreCPD);
    M_CPD *pNewCluster = rtLSTORE_NewCluster (
	pInstancePPT, pContentPrototypeCPD, -1
    );
    pContentPrototypeCPD->release ();

    return pNewCluster;
}


/*---------------------------------------------------------------------------
 *****  Link Initialized L-Store Instantiator
 *
 *  Arguments:
 *	pContentMap		- a link constructor whose reference ptoken defines the
 *				  number of lists to be held in this L-Store,  whose
 *				  positional ptoken defines the total number of elements
 *				  in the L-Store, and whose transitional state defines
 *				  the breakpoint array.
 *	pContentCluster		- the address of a CPD for the l-store's content.
 *
 *  Returns:
 *	The address of a CPD for the L-Store created.
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_NewCluster (
    rtLINK_CType *pContentMap, M_CPD *pContentCluster, bool bDecoupled
) {
    NewUsingLCInitCount++;

    M_CPD *pInstancePPT = pContentMap->RPT ();
    if (bDecoupled) pInstancePPT = rtPTOKEN_New (
	pContentCluster->Space (), rtPTOKEN_CPD_BaseElementCount (pInstancePPT)
    );

    M_CPD *pNewCluster = pInstancePPT->CreateContainer (
	RTYPE_C_ListStore,
	rtLSTORE_SizeofLStore (rtPTOKEN_CPD_BaseElementCount (pInstancePPT))
    );

    InitNewLStore (pNewCluster, pInstancePPT, pContentMap, NilOf (char const*));

    pNewCluster->StoreReference (
	rtLSTORE_CPx_ContentPToken, pContentMap->PPT ()
    )->StoreReference (
	rtLSTORE_CPx_Content, pContentCluster
    );
    ValidateBreakpointConsistency (pNewCluster);
    rtLSTORE_CPD_StringStore (pNewCluster) =  pContentCluster->RTypeIs (RTYPE_C_CharUV);
    rtLSTORE_CPD_IsInconsistent (pNewCluster) = false;

    if (bDecoupled)
	pInstancePPT->release ();

    return pNewCluster;
}


/*---------------------------------------------------------------------------
 *****  String Initialized L-Store Instantiator
 *
 *  Arguments:
 *	cStrings		- the number of strings present in the content cluster.
 *	pContentCluster	- the address of a CPD for the string store's content.
 *
 *  Returns:
 *	The address of a CPD for the L-Store created.
 *
 *****/
PrivateFnDef M_CPD *rtLSTORE_NewCluster (
    unsigned int cContentClusterStrings, M_CPD *pContentCluster
)
{
//  Create the required instance p-token, ...
    M_ASD *pContainerSpace = pContentCluster->Space ();
    M_CPD *pInstancePPT = rtPTOKEN_New (pContainerSpace, cContentClusterStrings);

/*****  Create and initialize the L-Store...  *****/
    M_CPD *pNewCluster = pContainerSpace->CreateContainer (
	RTYPE_C_ListStore,
	rtLSTORE_SizeofLStore (rtPTOKEN_CPD_BaseElementCount (pInstancePPT))
    );

    InitNewLStore (
	pNewCluster,
	pInstancePPT,
	NilOf (rtLINK_CType*),
	rtCHARUV_CPD_Array (pContentCluster)
    );

    pNewCluster->StoreReference (
	rtLSTORE_CPx_ContentPToken, pContentCluster, UV_CPx_PToken
    )->StoreReference (
	rtLSTORE_CPx_Content, pContentCluster
    );
    ValidateBreakpointConsistency (pNewCluster);
    rtLSTORE_CPD_StringStore (pNewCluster) = true;
    rtLSTORE_CPD_IsInconsistent (pNewCluster) = false;

/*****  Cleanup...  *****/
    pInstancePPT->release ();

/*****  Return...  *****/
    return pNewCluster;
}


/*---------------------------------------------------------------------------
 *****  Routine to make a positionally related copy of an existing L-Store.
 *
 *  Arguments:
 *	pSource			- the L-Store to copy.
 *	pResultSpace		- the space in which to create the copy.
 *	pResult			- the address of a monotype that will be
 *				  initialized to identify, for each of the
 *				  original lists, the location of the copy.
 *				  (i.e., Dom (pResult) = Dom (pSource))
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtLSTORE_Copy (
    M_CPD *pSource, M_ASD *pResultSpace, DSC_Descriptor *pResult
) {
/*****  Align the source, ...  *****/
    rtLSTORE_Align (pSource);

/*****  ...  copy the content, ...  *****/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!  The Following Code Does Not Support Content Generalized L-Stores  !!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    M_CPD *pResultContentPPT; M_CPD *pResultContent; {
	M_CPD *pSourceContent = rtLSTORE_CPD_ContentCPD (pSource);
	switch (pSourceContent->RType ()) {
	case RTYPE_C_Vector:
	    rtVECTOR_Align (pSourceContent); {
		M_CPD *pSourceContentPPT = rtVECTOR_CPD_RowPTokenCPD (pSourceContent);
		unsigned int nElements = rtPTOKEN_CPD_BaseElementCount (pSourceContentPPT);

		pResultContentPPT = rtPTOKEN_New (pResultSpace, nElements);
		rtLINK_CType *pElementSelection = rtLINK_RefConstructor (
		    pSourceContentPPT, -1
		)->AppendRange (0, nElements)->Close (pResultContentPPT);

		rtVECTOR_Extract (pSourceContent, pElementSelection, &pResultContent);

		pElementSelection->release ();

		pSourceContentPPT->release ();
	    }
	    break;

	case RTYPE_C_CharUV:
	    rtCHARUV_Align (pSourceContent);
	    pResultContentPPT = rtPTOKEN_New (
		pResultSpace, UV_CPD_ElementCount (pSourceContent)
	    );
	    pResultContent = UV_CopyWithNewPToken (pSourceContent, pResultContentPPT);
	    break;

	default:
	    ERR_SignalFault (
		EC__InternalInconsistency, "rtLSTORE_Copy: Unknown Content Type"
	    );
	    break;
	}
	pSourceContent->release ();
    }

/*****  ... access and create PPTs for the two l-stores, ...  *****/
    M_CPD *pSourcePPT = rtLSTORE_CPD_RowPTokenCPD (pSource);
    unsigned int nLists = rtPTOKEN_CPD_BaseElementCount (pSourcePPT);

    M_CPD *pResultPPT = rtPTOKEN_New (pResultSpace, nLists);

/*****  ... bulk copy the l-store, ...  *****/
    size_t cbResult = rtLSTORE_SizeofLStore (rtLSTORE_CPD_BreakpointCount (pSource));
    M_CPD *pResultLStore = pResultSpace->CreateContainer (RTYPE_C_ListStore, cbResult);
    memcpy (rtLSTORE_CPD_Base (pResultLStore), rtLSTORE_CPD_Base (pSource), cbResult);

/*****  ... fill in the new details, ...  *****/
    InitStdCPD (pResultLStore);

    rtLSTORE_CPD_IsInconsistent (pResultLStore) = true;

    pResultLStore->constructReference (rtLSTORE_CPx_RowPToken);
    pResultLStore->constructReference (rtLSTORE_CPx_Content);
    pResultLStore->constructReference (rtLSTORE_CPx_ContentPToken);

    pResultLStore->StoreReference (
	rtLSTORE_CPx_RowPToken, pResultPPT
    )->StoreReference (
	rtLSTORE_CPx_ContentPToken, pResultContentPPT
    )->StoreReference (
	rtLSTORE_CPx_Content, pResultContent
    );
    ValidateBreakpointConsistency (pResultLStore);

    rtLSTORE_CPD_IsInconsistent (pResultLStore) = false;

/*****  ... manufacture the source to copy relationship, and pack up the result:  *****/
    rtLINK_CType *pResultMap = rtLINK_PosConstructor (pSourcePPT, -1)->AppendRange (
	0, nLists
    )->Close (pResultPPT);

    pResult->constructMonotype (pResultLStore, pResultMap);

    pResultMap->release ();
    pResultContent->release ();
    pResultContentPPT->release ();
    pResultPPT->release ();
    pSourcePPT->release ();
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new lstore string store.
 *
 *  Arguments:
 *	ldelim			- a read only character string containing the left
 *				  delimiter to be added to every string in the lstore.
 *	rdelim			- a read only character string containing the right
 *				  delimiter to be added to every string in the lstore.
 *	pEnumerator		- a function which when called will return a pointer
 *				  to the next string to be added to the string store.
 *				  When all the strings have been processed a Nil
 *				  pointer will be returned.
 *				  The arguments passed to this function are
 *				  'reset' and a <varargs.h> argument pointer pointing
 *				  to 'stringEnumeratorArg1'.  If 'reset' is true the function
 *				  will reset itself to point to the first string, so
 *				  that the next time it is called the first string will
 *				  be returned.  This is to enable the strings to be
 *				traversed more than once.
 *	pEnumeratorArgs		- a collection of optional arguments passed by
 *				  <varargs.h> pointer to 'pEnumerator'.
 *
 *  Returns:
 *	a standard CPD for the new lstore string store.
 *
 *****/
PrivateFnDef M_CPD *CreateStringStore (
    M_ASD*			pContainerSpace,
    char const*			ldelim,
    char const*			rdelim,
    rtLSTORE_StringEnumerator	pEnumerator,
    V::VArgList&		pEnumeratorArgs
) {
    NewStringStoreCount++;

/*****  Count the number of strings and characters ...  *****/
    size_t ldelimSize = strlen(ldelim);
    size_t rdelimSize = strlen(rdelim);
    size_t delimSize = ldelimSize + rdelimSize;

    size_t size = 0;
    unsigned int numberOfStrings = 0;

    char const *strPtr;

    pEnumerator (true, pEnumeratorArgs);
    while (IsntNil (strPtr = pEnumerator (false, pEnumeratorArgs))) {
	size += strlen (strPtr) + 1 + delimSize;  /* include null delimiters */
	numberOfStrings++;
    }

/*****  Make the character uvector ... *****/
    M_CPD *sizePToken = rtPTOKEN_New (pContainerSpace, size);
    M_CPD *charCPD = rtCHARUV_New (
	sizePToken, pContainerSpace->TheCharacterClass ().PTokenCPD ()
    );

/*****  Second pass to copy the strings, ... *****/
    char *pOutputLocation = rtCHARUV_CPD_Array (charCPD);

    pEnumerator (true, pEnumeratorArgs);
    while (IsntNil (strPtr = pEnumerator (false, pEnumeratorArgs))) {
	strcpy (pOutputLocation, ldelim);
	pOutputLocation += ldelimSize;
	strcpy (pOutputLocation, strPtr);
	pOutputLocation += strlen (strPtr);
	strcpy (pOutputLocation, rdelim);
	pOutputLocation += rdelimSize + 1;  /* don't forget null string delimiter */
    }

/*****  Make the lstore ... *****/
    M_CPD *lstore = rtLSTORE_NewCluster (numberOfStrings, charCPD);
    charCPD->release ();
    sizePToken->release ();

    return lstore;
}


/*****  This routine is passed as an argument to the 'rtLSTORE_NewStringStore'
 *****  routine.  It is used to manufacture a string store for the scalar case.
 *
 *  Function:
 *	To either return a pointer to the string to be put into the lstore
 *	or Nil.  The lstore routine expects this routine to return a string
 *	pointer for every string to put into the lstore.  When all the strings
 *	have been processed, this routine returns Nil.  The special case
 *	is when 'reset' is true, then this routine resets itself to the first
 *	string and returns Nil.
 *
 *****/
PrivateFnDef char const *ReturnSingleString (bool reset, va_list ap) {
    V::VArgList iArgList (ap);

    bool *const validString = iArgList.arg<bool*> ();

    if (reset) {
	*validString = true;
	return NilOf (char const *);
    }

    if (*validString) {
	*validString = false;
	return iArgList.arg<char const*> ();
    }

    return NilOf (char const*);
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new lstore string store.
 *
 *  Arguments:
 *	pEnumerator		- a function which when called will return a pointer
 *				  to the next string to be added to the string store.
 *				  When all the strings have been processed a Nil
 *				  pointer will be returned.
 *				  The arguments passed to this function are
 *				  'reset' and a <varargs.h> argument pointer pointing
 *				  to 'stringEnumeratorArg1'.  If 'reset' is true the function
 *				  will reset itself to point to the first string, so
 *				  that the next time it is called the first string will
 *				  be returned.  This is to enable the strings to be
 *				  traversed more than once.
 *	stringEnumeratorArg1	- a collection of optional arguments passed by
 *				  <varargs.h> pointer to 'pEnumerator'.
 *
 *  Returns:
 *	a standard CPD for the new lstore string store.
 *
 *****/
PublicFnDef M_CPD *__cdecl rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, rtLSTORE_StringEnumerator pEnumerator, ...
) {
    V_VARGLIST (pEnumeratorArgs, pEnumerator);

    M_CPD *result = CreateStringStore (
	pContainerSpace, "", "", pEnumerator, pEnumeratorArgs
    );

    return result;
}

PublicFnDef M_CPD *rtLSTORE_NewStringStore (M_ASD *pContainerSpace, char const *pString) {
    bool validString;
    return rtLSTORE_NewStringStore (
	pContainerSpace, ReturnSingleString, &validString, pString
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new lstore string store.
 *
 *  Arguments:
 *	ldelim			- a read only character string containing the left
 *				  delimiter to be added to every string in the lstore.
 *	rdelim			- a read only character string containing the right
 *				  delimiter to be added to every string in the lstore.
 *	pEnumerator		- a function which when called will return a pointer
 *				  to the next string to be added to the string store.
 *				  When all the strings have been processed a Nil
 *				  pointer will be returned.
 *				  The arguments passed to this function are
 *				  'reset' and a <varargs.h> argument pointer pointing
 *				  to 'stringEnumeratorArg1'.  If 'reset' is true the function
 *				  will reset itself to point to the first string, so
 *				  that the next time it is called the first string will
 *				  be returned.  This is to enable the strings to be
 *				  traversed more than once.
 *	stringEnumeratorArg1	- a collection of optional arguments passed by
 *				  <varargs.h> pointer to 'pEnumerator'.
 *
 *  Returns:
 *	a standard CPD for the new lstore string store.
 *
 *****/
PublicFnDef M_CPD *__cdecl rtLSTORE_NewStringStoreWithDelm (
    M_ASD *pContainerSpace, char const *ldelim, char const *rdelim,
    rtLSTORE_StringEnumerator pEnumerator, ...
) {
    V_VARGLIST (pEnumeratorArgs, pEnumerator);

    M_CPD *result = CreateStringStore (
	pContainerSpace, ldelim, rdelim, pEnumerator, pEnumeratorArgs
    );

    return result;
}

PublicFnDef M_CPD *rtLSTORE_NewStringStoreWithDelm (
    M_ASD *pContainerSpace, char const *ldelim, char const *rdelim, char const *pString
) {
    bool validString;
    return rtLSTORE_NewStringStoreWithDelm (
	pContainerSpace, ldelim, rdelim, ReturnSingleString, &validString, pString
    );
}


/*---------------------------------------------------------------------------
 ***** Routine to make a new string store from a character uvector.
 *
 *  Arguments:
 *	charUV		- a standard CPD for the character uvector which
 *			  contains the content for the lstore string store.
 *
 *  Returns:
 *	A standard CPD for the new lstore string store created.
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_NewStringStoreFromUV (M_CPD *charUV) {
    NewStringStoreFromUVCount++;

/*****  Determine the number of characters in the uvector ... *****/
    unsigned int size = UV_CPD_ElementCount (charUV);

/*****  Count the number of strings, ... *****/
    rtCHARUV_ElementType const *cptr = rtCHARUV_CPD_Array (charUV);
    rtCHARUV_ElementType const *cptrl = cptr + size;
    unsigned int numberOfStrings = 0;

    while (cptr < cptrl) {
	cptr += strlen (cptr) + 1;
	numberOfStrings++;
    }

/*****  Make and return the lstore ... *****/
    return rtLSTORE_NewCluster (numberOfStrings, charUV);
} 


/*******************************
 *****  Alignment Routine  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to align an L-Store when the content ptoken is not current.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to be aligned.
 *
 *  Returns:
 *	'lstore'
 *
 *****/
PrivateFnDef M_CPD *AlignFromContent (M_CPD *lstore) {
    M_CPD *pTokenCPD;
    rtPTOKEN_CType *ptokenc;
    int adjustment = 0;

/***** Traversal Macros ... *****/

#define processTo(where)\
{\
    unsigned int limit = (unsigned int)(where);\
    while (*rtLSTORE_CPD_Breakpoint (lstore) <= limit)\
	(*rtLSTORE_CPD_Breakpoint (lstore)++) += adjustment;\
}

#define processToEnd()\
{\
    while (\
	rtLSTORE_CPD_Breakpoint (lstore) <= rtLSTORE_CPD_BreakpointArray (\
	    lstore\
	) + rtLSTORE_CPD_BreakpointCount (lstore)\
    ) (*rtLSTORE_CPD_Breakpoint (lstore)++) += adjustment;\
}

#define handleDelete(ptOrigin, ptShift)\
{\
    processTo (ptOrigin + ptShift);\
    while (*rtLSTORE_CPD_Breakpoint (lstore) < (unsigned int)ptOrigin)\
    {\
	int size = ptOrigin + ptShift - *rtLSTORE_CPD_Breakpoint (lstore);\
	adjustment += size;\
	ptShift -= size;\
	*rtLSTORE_CPD_Breakpoint (lstore)++ += adjustment;\
    }\
    adjustment += ptShift;\
}

#define handleInsert(ptOrigin, ptShift)\
{\
    ERR_SignalFault (\
	EC__InternalInconsistency,\
	"AlignFromContent:  Illegal insertion in content ptoken"\
    );\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'AlignFromContent'
 *---------------------------------------------------------------------------
 */
/*****  Return if the L-Store is already aligned...  *****/
    rtPTOKEN_IsntCurrent (lstore, rtLSTORE_CPx_ContentPToken, pTokenCPD);
    if (IsNil (pTokenCPD))
	return lstore;

/***** If the row ptoken is also not current, error ... *****/
    if (!rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_RowPToken)) {
	pTokenCPD->release ();
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "AlignFromContent:  Both LStore PTokens are not current"
	);
    }

    AlignFromContentCount++;
    if (TracingCalls)
	IO_printf ("AlignFromContent: cpd=%08X\n", lstore);

/***** Get a ptoken constructor for the content ptoken ... *****/
    ptokenc = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
    pTokenCPD->release ();

/***** Don't forget to enable modifications ... *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;

/***** Traverse the referential ptoken *****/
    rtLSTORE_CPD_Breakpoint (lstore) = rtLSTORE_CPD_BreakpointArray (lstore);
    rtPTOKEN_FTraverseInstructions (ptokenc, handleInsert, handleDelete);

/*** process the remaining values ***/
    processToEnd ();

/***** Now that the row state is adjusted, update the content ptoken  *****/
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, ptokenc->NextGeneration ());

    ptokenc->discard ();
    ValidateBreakpointConsistency (lstore);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;

/***** All done *****/
    return lstore;

#undef handleDelete
#undef handleInsert
#undef processTo
#undef processToEnd
}


/*---------------------------------------------------------------------------
 *****  Routine to align an L-Store when the row or content ptoken (not both)
 *****  is not current.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to be aligned.
 *
 *  Returns:
 *	'lstore'
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_Align (M_CPD *lstore) {
    M_CPD			*pTokenCPD;
    int				size, newSize, contentAdj, origPos, i;
    unsigned int		*newbp, *newbpstart;

/***** Traversal Macros ... *****/
#define output(value)\
    *newbp++ = (value)

#define processTo(newPos) {\
    while (origPos < (newPos))\
	output((rtLSTORE_CPD_BreakpointArray(lstore)[origPos++])+contentAdj);\
}

#define originalValue(newPos)\
    (rtLSTORE_CPD_BreakpointArray (lstore)[(newPos)])

#define handleInsert(ptOrigin, ptShift) {\
    int i, value;\
\
    processTo (ptOrigin);\
    value = originalValue (ptOrigin) + contentAdj;\
    for (i=0; i < ptShift; i++) output (value);\
}

#define handleDelete(ptOrigin, ptShift) {\
    int ptorig, at, x, amt;\
\
    ptorig = ptOrigin + ptShift;\
    processTo (ptorig);\
    at = originalValue (ptorig);\
    x  = originalValue (ptOrigin);\
    amt = at - x;\
    contentPTokenC->AppendAdjustment (x + contentAdj, amt);\
    contentAdj += amt;\
    origPos -= ptShift; /* skip past deleted values */\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLSTORE_Align'
 *---------------------------------------------------------------------------
 */
    RTYPE_MustBeA (
	"rtLSTORE_Align", M_CPD_RType (lstore), RTYPE_C_ListStore
    );

    ValidateBreakpointConsistency (lstore);

/*****  Return if the L-Store is already aligned...  *****/
    rtPTOKEN_IsntCurrent (lstore, rtLSTORE_CPx_RowPToken, pTokenCPD);
    if (IsNil (pTokenCPD))
	return AlignFromContent (lstore);

/***** If the content ptoken is also not current, error ... *****/
    if (!rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_ContentPToken)) {
	pTokenCPD->release ();
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLSTORE_Align:  Both LStore PTokens are not current"
	);
    }

    AlignCount++;
    if (TracingCalls)
	IO_printf ("rtLSTORE_Align: cpd=%08X\n", lstore);

/***** Get a ptoken constructor for the row ptoken ... *****/
    rtPTOKEN_CType *ptokenc = rtPTOKEN_CPDCumAdjustments (pTokenCPD);
    pTokenCPD->release ();

/***** Make a ptoken constructor for the content ptoken ... *****/
    rtPTOKEN_CType *contentPTokenC = rtPTOKEN_NewShiftPTConstructor (
	lstore, rtLSTORE_CPx_ContentPToken
    );

/***** Determine the sizes ... *****/
    size = rtLSTORE_CPD_BreakpointCount (lstore);
    newSize = rtPTOKEN_CPD_BaseElementCount(ptokenc->NextGeneration ());

/***** Allocate some temporary space for the breakpoint table values ... *****/
/*** (remember, the breakpoint table is 1 larger than the size.) ***/
    newbpstart = newbp = (unsigned int *) UTIL_Malloc (
	(newSize + 1) * sizeof (int)
    );

/***** Setup for the traversal *****/
    contentAdj = 0;
    origPos = 0;

/***** Traverse the referential ptoken *****/
    rtPTOKEN_FTraverseInstructions (ptokenc, handleInsert, handleDelete);
    /*** process the remaining values ***/
    processTo (size + 1);

/***** Reconstruct the breakpoint array ... *****/
    rtLSTORE_CPD_Breakpoint (lstore) = rtLSTORE_CPD_BreakpointArray (lstore) + size + 1;

/*****  Don't forget to enable modifications ... *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;

    lstore->ShiftContainerTail (
	rtLSTORE_CPx_Breakpoint, 0, ((ptrdiff_t)newSize - size) * sizeof (int), true
    );

    rtLSTORE_CPD_BreakpointCount (lstore) = newSize;

    /*** copy in the new values ... ***/
    newbp = newbpstart;
    for (i=0; i<=newSize; i++)
	rtLSTORE_CPD_BreakpointArray (lstore)[i] =
	    *newbp++;

    UTIL_Free (newbpstart);


/***** Fix the row ptoken to be the new one ... *****/
    lstore->StoreReference (rtLSTORE_CPx_RowPToken, ptokenc->NextGeneration ());

    ptokenc->discard ();

/***** Close the content ptoken and copy it into the lstore ... *****/
    pTokenCPD = contentPTokenC->ToPToken ();
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, pTokenCPD);
    pTokenCPD->release ();

    ValidateBreakpointConsistency (lstore);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;

    if (rtLSTORE_CPD_StringStore (lstore)) {
	M_CPD *content = lstore->GetCPD (rtLSTORE_CPx_Content, RTYPE_C_CharUV);
	if (!rtPTOKEN_IsCurrent (content, UV_CPx_PToken))
	    rtCHARUV_Align (content);
	content->release ();
    }

/***** All done *****/
    return lstore;
}


/*---------------------------------------------------------------------------
 *****  Routine to align the lstore and all structures which make up the
 *****  lstore.
 *
 *  Arguments:
 *	pLStoreCPD			- the lstore to align.
 *	deletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	true if any alignments were necessary. false otherwise.
 *
 *****/
PublicFnDef bool rtLSTORE_AlignAll (M_CPD *pLStoreCPD, bool deletingEmptyUSegments) {
    RTYPE_MustBeA (
	"rtLSTORE_AlignAll", M_CPD_RType (pLStoreCPD), RTYPE_C_ListStore
    );

    bool result = rtPTOKEN_IsCurrent (
	pLStoreCPD, rtLSTORE_CPx_RowPToken
    ) && rtPTOKEN_IsCurrent (
	pLStoreCPD, rtLSTORE_CPx_ContentPToken
    ) ? false : (rtLSTORE_Align (pLStoreCPD), true);

    M_CPD *pContentCPD = rtLSTORE_CPD_ContentCPD (pLStoreCPD);
    if (RTYPE_C_CharUV != (RTYPE_Type)M_CPD_RType (pContentCPD))
	result |= rtVSTORE_AlignAll (pContentCPD, deletingEmptyUSegments);
    else if (!rtPTOKEN_IsCurrent (pContentCPD, UV_CPx_PToken)) {
	rtCHARUV_Align (pContentCPD);
	result = true;
    }
    pContentCPD->release ();

    return result;
}


/*---------------------------------------------------------------------------
 ***** Routine to align an lstore when the content ptoken is not current.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the lstore to align.
 *	listSelectorLC		- a pointer to a link constructor containing
 *				  the lists which need to be updated.
 *	adjustmentUV		- optional (Nil if ommitted).
 *				a standard CPD for an integer uvector
 *				  containing the amount that each list in
 *				  'listSelectorLC' needs to be modified by.
 *				If ommitted, the adjustment amount will a
 *				always be +1.
 *
 *  Returns:
 *	'lstore'
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_AlignUsingLCSelctLists (
    M_CPD *lstore, rtLINK_CType *listSelectorLC, M_CPD *adjustmentUV
) {
    M_CPD *newPToken;
    unsigned int
	breakpointPos;
    int noAdjustments,
	cummulativeAdjustment;
    rtINTUV_ElementType
	*adjustmentp;

/***** The list selector link constructor traversal macros: *****/
#define currentAdjustment ((noAdjustments) ? (1) : (*adjustmentp++))

#define outputThru(position) {\
    while (breakpointPos <= position)\
	rtLSTORE_CPD_BreakpointArray (lstore)[breakpointPos++]\
	    += cummulativeAdjustment;\
}

#define handleRange(position, count, value) {\
    outputThru ((unsigned int)value);\
    while (count > 0) {\
	cummulativeAdjustment += (currentAdjustment);\
	rtLSTORE_CPD_BreakpointArray (lstore)[breakpointPos++]\
	    += cummulativeAdjustment;\
	count--;\
	value++;\
    }\
}

#define handleRepeat(position, count, value) {\
    outputThru ((unsigned int)value);\
    /*** add up all of the adjustments and apply once ***/\
    while (count > 0) {\
	cummulativeAdjustment += (currentAdjustment);\
	count--;\
    }\
    rtLSTORE_CPD_BreakpointArray (lstore)[breakpointPos++] += cummulativeAdjustment;\
}

#define handleNil(position, count, value)\
    /* nada */


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLSTORE_AlignUsingLCSelctLists'
 *---------------------------------------------------------------------------
 */
    ValidateBreakpointConsistency (lstore);

/*****  Return if the content ptoken is already aligned ...  *****/
    if (rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_ContentPToken))
	return lstore;

/***** If the row ptoken is also not current, error ... *****/
    if (!rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_RowPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_AlignUsingLCSelectedLists: One P-Token Must Be Current"
    );

    AlignUsingLCListsCount++;

/***** Determine if 'adjustmentUV' was specified ... *****/
    noAdjustments = IsNil (adjustmentUV) ? true : false;

/***** Align and validate the linkc ... *****/
    if (noAdjustments)
	listSelectorLC->AlignForExtract (lstore, rtLSTORE_CPx_RowPToken);
    else {
	listSelectorLC->AlignForAssign (
	    lstore, rtLSTORE_CPx_RowPToken, adjustmentUV, UV_CPx_PToken
	);

	adjustmentp = rtINTUV_CPD_Array (adjustmentUV);
    }

    if (TracingCalls) IO_printf (
      "rtLSTORE_AlignUseLCLists: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	listSelectorLC->ElementCount (),
	rtLINK_LC_RRDCount (listSelectorLC)
    );

/*****  Don't forget to enable modifications ... *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;

/*****  Modify the break point array ... *****/
/*****  Traverse the selector linkc ... *****/
    breakpointPos = 0;
    cummulativeAdjustment = 0;

    rtLINK_TraverseRRDCList (
	listSelectorLC, handleNil, handleRepeat, handleRange
    );

/*****  Output the rest ... *****/
    outputThru (listSelectorLC->ReferenceNil ());

/***** Set the content PToken to the new one ... *****/
    newPToken = rtPTOKEN_BasePToken (lstore, rtLSTORE_CPx_ContentPToken);
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, newPToken);
    newPToken->release ();

/***** and return the aligned lstore ... *****/
    ValidateBreakpointConsistency (lstore);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;
    return lstore;

/***** undef the macros ... *****/
#undef handleNil
#undef handleRepeat
#undef handleRange
#undef outputThru
#undef currentAdjustment
}


/*---------------------------------------------------------------------------
 ***** Routine to align an lstore when the content ptoken is not current.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the lstore to align.
 *	listSelectorRef		- a pointer to a reference specifying which
 *				  list needs to be updated.
 *	adjustment		- an integer specifying the amount that the
 *				list needs to be modified by.
 *
 *  Returns:
 *	'lstore'
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_AlignUsingRefSelctList (
    M_CPD *lstore, rtREFUV_TypePTR_Reference listSelectorRef, int adjustment
) {
    int i;
    M_CPD *newPToken;

    ValidateBreakpointConsistency (lstore);

/*****  Return if the content ptoken is already aligned ...  *****/
    if (rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_ContentPToken))
	return lstore;

/***** If the row ptoken is also not current, error ... *****/
    if (!rtPTOKEN_IsCurrent (lstore, rtLSTORE_CPx_RowPToken)
    ) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_AlignUsingRefSelectedList: One PToken Must Be Current"
    );

    AlignUsingRefListCount++;
    if (TracingCalls) IO_printf (
	"rtLSTORE_AlignUseRefList: cpd=%08X, lsBpaSz=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore)
    );

/***** Align and validate the reference ... *****/
    rtREFUV_AlignAndValidateRef (
	listSelectorRef, lstore, rtLSTORE_CPx_RowPToken
    );

/*****  Don't forget to enable modifications ... *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;

/*****  Modify the break point array ... *****/
    for (i= rtREFUV_Ref_Element (listSelectorRef) + 1;
	 i <= (int) rtLSTORE_CPD_BreakpointCount (lstore);
	 i++
    ) rtLSTORE_CPD_BreakpointArray (lstore)[i] += (adjustment);

/***** Set the content PToken to the new one ... *****/
    newPToken =	rtPTOKEN_BasePToken (lstore, rtLSTORE_CPx_ContentPToken);
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, newPToken);
    newPToken->release ();

/***** and return the aligned lstore ... *****/
    ValidateBreakpointConsistency (lstore);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;
    return lstore;
}


/**********************************
 *****  List Addition Routine  *****
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to add a specified number of rows to the end of a list store.
 *
 *  Arguments:
 *	lstore			- a standard CPD for a list store.
 *	newListsPToken		- a standard CPD for a P-Token which
 *				  specifies the number of lists to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the list store.  The
 *	positional P-Token of this link constructor will be 'newListsPToken'.
 *
 *****/
PublicFnDef rtLINK_CType *rtLSTORE_AddLists (M_CPD *lstore, M_CPD *newListsPToken) {
    AddListsCount++;
    if (TracingCalls)
	IO_printf ("rtLSTORE_AddLists: cpd=%08X\n", lstore);

/*****  Verify that 'newListsPToken' is a p-token...  *****/
    RTYPE_MustBeA (
	"rtLSTORE_AddLists", M_CPD_RType (newListsPToken), RTYPE_C_PToken
    );

/*****  First make sure that the row ptoken is current ...  *****/
    rtLSTORE_Align (lstore);

/*****  ...  and then modify it ...  *****/
    rtPTOKEN_CType *ptc = rtPTOKEN_NewShiftPTConstructor (lstore, rtLSTORE_CPx_RowPToken);
    int origin = rtPTOKEN_PTC_BaseCount (ptc);
    int numLists = rtPTOKEN_CPD_BaseElementCount (newListsPToken);

    M_CPD *lstoreRowPToken = ptc->AppendAdjustment (origin, numLists)->ToPToken ();

/*****  ... cause the rows to be added  ...  *****/
    rtLSTORE_Align (lstore);

/*****  ...create a link constructor for the added rows...  *****/
    rtLINK_CType *addedLists = rtLINK_AppendRange (
	rtLINK_RefConstructor (lstoreRowPToken, -1), origin, numLists
    )->Close (newListsPToken);

/*****  ...cleanup, ...  *****/
    lstoreRowPToken->release ();

/*****  ...and return.  *****/
    return addedLists;
}


/************************************
 *****  Content Query Routines  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to get the number of elements in a list.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to be queried.
 *	selectionRef		- a reference specifying which list to be
 *				  counted.
 *
 *  Returns:
 *	The element count for the specified list.
 *
 *****/
PublicFnDef int rtLSTORE_RefSelListElementCount (
    M_CPD *lstore, rtREFUV_TypePTR_Reference selectionRef
) {
    unsigned int
	*breakpointPtr,
	referenceValue;

    RefListElementCount++;
    ValidateBreakpointConsistency (lstore);
    if (TracingCalls) IO_printf (
	"rtLSTORE_RefSelListElementCount: cpd=%08X", lstore
    );

/*****  Align the arguments and validate the reference as an extraction *****/
    rtREFUV_AlignAndValidateRef (
	selectionRef, rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken
    );

/*****  Get the value out of the reference ... *****/
    referenceValue =(int) rtREFUV_Ref_Element (selectionRef);

    breakpointPtr = rtLSTORE_CPD_BreakpointArray (lstore);

/*---------------------------------------------------------------------------
 * If the referenceValue is the Nil value (i.e. equal to the breakpoint count)
 * return 0; otherwise return the number of elements as calculated via the
 * breakpoint array
 *---------------------------------------------------------------------------
 */
    return
	referenceValue == rtLSTORE_CPD_BreakpointCount (lstore)
	? 0
	: breakpointPtr[referenceValue + 1] - breakpointPtr [referenceValue];
}


/*---------------------------------------------------------------------------
 *****  Routine to get the number of elements in each of a set of lists.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to be queried.
 *	selectionLC		- a reference specifying which list to be
 *				  counted.
 *
 *  Returns:
 *	An integer uvector containing the element counts for the specified
 *  lists.
 *
 *****/
PublicFnDef M_CPD *rtLSTORE_LCSelListElementCount (
    M_CPD *lstore, rtLINK_CType *selectionLC
) {
#define nilReferenceHandler(c, n, r)\
    while (n-- > 0)\
	*resultPtr++ = 0

#define repeatedReferenceHandler(c, n, r)\
    while (n-- > 0)\
	*resultPtr++ = breakpointArray [r + 1] - breakpointArray [r]

#define rangeReferenceHandler(c, n, r)\
    while (n-- > 0) {\
	*resultPtr++ = breakpointArray [r + 1] - breakpointArray [r];\
	r++;\
    }

    LCListElementCount++;

/*****
 * Align the arguments and validate the link constructor as an extraction
 * index for the L-Store.
 *****/
    selectionLC->AlignForExtract (rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken);

    if (TracingCalls) IO_printf (
	"rtLSTORE_LCSelListElementCount: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	selectionLC->ElementCount (),
	rtLINK_LC_RRDCount (selectionLC)
    );

/*****  Create the result uvector  *****/
    M_CPD *resultUV = rtINTUV_New (
	selectionLC->PPT (), lstore->KOT ()->TheIntegerPTokenCPD ()
    );

    rtINTUV_ElementType *resultPtr = rtINTUV_CPD_Array (resultUV);

/***** Access the breakpoint array *****/
    unsigned int *breakpointArray = rtLSTORE_CPD_BreakpointArray (lstore);

/*****  Do the counting  *****/
    rtLINK_TraverseRRDCList (
	selectionLC, nilReferenceHandler, repeatedReferenceHandler, rangeReferenceHandler
    );

    return resultUV;

#undef nilReferenceHandler
#undef repeatedReferenceHandler
#undef rangeReferenceHandler
}


/*********************************
 *****  Extraction Routines  *****
 *********************************/

/*---------------------------------------------------------------------------
 ***** L-Store Link Constructor Extraction Routine.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to extract
 *				from.
 *	linkc			- the address of a link constructor specifying
 *				which lists to extract.
 *
 *  Returns:
 *	The element expansion.
 *
 *****/
PrivateFnDef rtLINK_CType *DetermineLCExpansion (M_CPD *lstore, rtLINK_CType *linkc) {
    unsigned int
	elementCnt = 0,
	srcValue,
	srcCnt;
    int i;

/*****  Define the Traversal Macros  *****/
#define locateSource(subscriptValue) {\
    srcValue = rtLSTORE_CPD_BreakpointArray (lstore)[(subscriptValue)];\
    srcCnt = rtLSTORE_CPD_BreakpointArray (lstore)[(subscriptValue) + 1] - srcValue;\
}

#define outputExpansionLinkc(value, cnt) {\
    elementCnt += (cnt);\
    rtLINK_AppendRepeat (pElementExpansion, value, cnt);\
}

#define handleNil(subscriptOffset, subscriptCnt, subscriptValue)

#define handleRange(subscriptOffset, subscriptCnt, subscriptValue) {\
    for (i=0; i<(subscriptCnt); i++) {\
	locateSource (subscriptValue);\
	outputExpansionLinkc  (subscriptOffset, srcCnt);\
	subscriptValue++;\
	subscriptOffset++;\
    }\
}

#define handleRepeat(subscriptOffset, repeatCnt, subscriptValue) {\
    locateSource (subscriptValue);\
    for (i=0; i<(repeatCnt); i++)\
	outputExpansionLinkc (subscriptOffset++, srcCnt);\
}


/*---------------------------------------------------------------------------
 *  Code Body: 'DetermineLCExpansion'
 *---------------------------------------------------------------------------
 */

/*****
 *  Align the arguments and validate the link constructor as an extraction
 *  index for the L-Store ...
 *****/
    linkc->AlignForExtract (rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken);

    if (TracingCalls) IO_printf (
	"DetermineLCExpansion: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	linkc->ElementCount (),
	rtLINK_LC_RRDCount (linkc)
    );

/***** Create the element expansion ... *****/
    rtLINK_CType *pElementExpansion = rtLINK_RefConstructor (linkc->PPT (), -1);

/***** Do the traversal ... *****/
    rtLINK_TraverseRRDCList (linkc, handleNil, handleRepeat, handleRange);

/***** Close the element expansion... *****/
    M_CPD *newPosPToken = rtPTOKEN_New (lstore->ScratchPad (), elementCnt);
    pElementExpansion->Close (newPosPToken);
    newPosPToken->release ();

/*****  And return:  *****/
    return pElementExpansion;

/***** Delete the DetermineLCExpansion macros ... *****/
#undef locateSource
#undef outputExpansionLinkc
#undef handleNil
#undef handleRange
#undef handleRepeat
}


/*---------------------------------------------------------------------------
 ***** L-Store Extraction Routine which produces an empty result.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store of interest.
 *	ptoken			- the address of a cpd for the referential ptoken
 *				  of the element expansion.
 *	pElementSelection	- the address of a link constructor pointer
 *				  which will be set to the result of the
 *				  extraction.  Indexes the content p-token
 *				  of the lstore.
 * 	pElementExpansion	- the address of a link constructor pointer
 *				  which will be set to the position of 'linkc'
 *				  associated with each extracted element.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void CreateEmptyResult (
    M_CPD	 *lstore,
    M_CPD	 *ptoken,
    rtLINK_CType *&pElementSelection,
    rtLINK_CType *&pElementExpansion
) {
/*****
 *  Align the arguments
 *****/
    rtLSTORE_Align (lstore);

/*****  Create the element expansion domain p-token, ...  *****/
    M_CPD *newPosPToken = rtPTOKEN_New (lstore->ScratchPad (), 0);

/***** Create the element expansion ... *****/
    pElementExpansion = rtLINK_RefConstructor (ptoken, -1);
    pElementExpansion->Close (newPosPToken);

/*****  Create the element selection ... *****/
    pElementSelection = rtLINK_RefConstructor (lstore, rtLSTORE_CPx_ContentPToken);
    pElementSelection->Close (newPosPToken);

/***** Cleanup and return ... *****/
    newPosPToken->release ();
}


/*---------------------------------------------------------------------------
 ***** L-Store Link Constructor Extraction Routine.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store of interest.
 *	linkc			- the address of a link constructor specifying
 *				  which lists to extract.
 *	pElementSelection	- the address of a link constructor pointer
 *				  which will be set to the result of the
 *				  extraction.  Indexes the content p-token
 *				  of the lstore.
 * 	pElementExpansion	- the address of a link constructor pointer
 *				  which will be set to the position of 'linkc'
 *				  associated with each extracted element.
 *	pElementReordering	- a pointer to a standard CPD for a reference
 *				  uvector which will be set by the extraction
 *				  process if 'linkc' contains any RRDC's with
 *				  repeats.  Used to reorder the element selection
 *				  link constructor.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	DOM (lstore-Content)	= COD (pElementSelection)
 *	DOM (pElementSelection)	= DOM (pElementReordering)
 *	COD (pElementReordering)= DOM (pElementExpansion)
 *	COD (pElementExpansion)	= DOM (linkc)
 *
 *	or, in the APIWATW department:
 *
 *	LS-Lists	TOTSC	    Expansion	    Selection	    LS-Content
 *
 *	    *<------------*<------------*<--------------*-------------->*
 *	          linkc       expansion     reordering       selector
 *
 *****/
PrivateFnDef void LCExtract (
    M_CPD		*lstore,
    rtLINK_CType	*linkc,
    rtLINK_CType	*&pElementSelection,
    rtLINK_CType	*&pElementExpansion,
    M_CPD		*&pElementReordering
) {
    int
	repeat = rtLINK_RRDType_Repeat,
	range  = rtLINK_RRDType_Range,
	elementCnt = 0,
	tempValue,
	origElementCnt,
	srcValue,
	srcCnt,
	i;
    rtREFUV_ElementType
	*reOrderUVectorPtr;


/*****  Define the Traversal Macros  *****/
#define locateSource(subscriptValue) {\
    srcValue= rtLSTORE_CPD_BreakpointArray (lstore)[(subscriptValue)];\
    srcCnt  = rtLSTORE_CPD_BreakpointArray (lstore)[(subscriptValue) + 1] - srcValue;\
}

#define outputElementSelector(value, cnt, type) {\
    if (type == rtLINK_RRDType_Range)\
	rtLINK_AppendRange (pElementSelection, value, cnt);\
    else\
	rtLINK_AppendRepeat (pElementSelection, value, cnt);\
    elementCnt += (cnt);\
}

#define outputReOrderUVector(value, cnt) {\
    if (hasRepeats) {\
	tempValue = (value);\
	for (int j=0; j<(cnt); j++)\
	    *reOrderUVectorPtr++ = (rtREFUV_ElementType) tempValue++;\
    }\
}

#define handleNil(subscriptOffset, subscriptCnt, subscriptValue)

#define handleRange(subscriptOffset, subscriptCnt, subscriptValue) {\
    for (i=0; i<(subscriptCnt); i++) {\
	locateSource (subscriptValue);\
	outputElementSelector (srcValue, srcCnt, range);\
	outputReOrderUVector  (elementCnt - srcCnt, srcCnt);\
	subscriptValue++;\
	subscriptOffset++;\
    }\
}

#define handleRepeat(subscriptOffset, repeatCnt, subscriptValue) {\
    int repetitionNum;\
\
    origElementCnt = elementCnt;\
    locateSource (subscriptValue);\
\
    tempValue = srcValue;\
    for (i=0; i<srcCnt; i++)\
	outputElementSelector (tempValue++, repeatCnt, repeat);\
\
    for (i=0; i<srcCnt; i++, origElementCnt++) {\
	for (repetitionNum = 0; repetitionNum < repeatCnt; repetitionNum++) {\
	    outputReOrderUVector (origElementCnt + repetitionNum * srcCnt, 1);\
	}\
    }\
}


/*---------------------------------------------------------------------------
 *  Code Body: 'LCExtract'
 *---------------------------------------------------------------------------
 */
    LCExtractCount++;

/*****
 *  Align the arguments and validate the link constructor as an extraction
 *  index for the L-Store ...
 *****/
    linkc->AlignForExtract (rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken);

    if (TracingCalls) IO_printf (
	"rtLSTORE_LCExtract: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	linkc->ElementCount (),
	rtLINK_LC_RRDCount (linkc)
    );

/***** Determine if the subscript link constructor has any repeats ... *****/
    bool hasRepeats = rtLINK_LC_HasRepeats (linkc);

/***** Create the element expansion ... *****/
    pElementExpansion = DetermineLCExpansion (lstore, linkc);

/*****  Create the element selection ... *****/
    pElementSelection = rtLINK_RefConstructor (lstore, rtLSTORE_CPx_ContentPToken);

/***** Create the temp space for the pElementReordering if needed ... *****/
    M_CPD *pElementSelectorPPT;
    if (hasRepeats) {
	pElementSelectorPPT = rtPTOKEN_New (
	    lstore->ScratchPad (), pElementExpansion->ElementCount ()
	);
	pElementReordering = rtREFUV_New (
	    pElementSelectorPPT, pElementExpansion->PPT ()
	);
	reOrderUVectorPtr = rtREFUV_CPD_Array (pElementReordering);
    }
    else {
	pElementSelectorPPT = pElementExpansion->PPT ();
	pElementReordering = NilOf (M_CPD *);
    }

/***** Do the traversal ... *****/
    rtLINK_TraverseRRDCList (linkc, handleNil, handleRepeat, handleRange);

/***** Close the element selection ... *****/
    pElementSelection->Close (pElementSelectorPPT);
    if (hasRepeats)
	pElementSelectorPPT->release ();

/***** Delete the LCExtract macros ... *****/
#undef locateSource
#undef outputElementSelector
#undef outputReOrderUVector
#undef handleNil
#undef handleRange
#undef handleRepeat
}


/*---------------------------------------------------------------------------
 ***** L-Store Reference Extraction Routine.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store of interest.
 *	reference 		- the address of a reference specifying
 *				  which list to extract.
 *	pElementSelection	- the address of a link constructor which will
 *				  be set to contain the result of the
 *				  extraction: subscripts into the content of
 *				  the lstore.
 * 	pElementExpansion	- the address of a link constructor which will
 *				  be set to contain a 0 for each extracted
 *				  element.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void RefExtract (
    M_CPD			*lstore,
    rtREFUV_Type_Reference	&reference,
    rtLINK_CType		*&pElementSelection,
    rtLINK_CType		*&pElementExpansion
) {
    RefExtractCount++;

/*****
 *  Align the arguments and validate the reference as an extraction
 *****/
    rtREFUV_AlignAndValidateRef (
	&reference, rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken
    );

    if (TracingCalls) IO_printf (
	"rtLSTORE_RefExtract: cpd=%08X, lsBpaSz=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore)
    );

/*****  Create the element selection ... *****/
    pElementSelection = rtLINK_RefConstructor (lstore, rtLSTORE_CPx_ContentPToken);

/***** Create the element expansion ... *****/
    pElementExpansion = rtLINK_RefConstructor (M_AttachedNetwork->TheScalarPToken (), -1);

/***** Do the extraction ... *****/
    unsigned int cElements;
    unsigned int referenceValue = rtREFUV_Ref_D_Element (reference);
    if (referenceValue >= rtLSTORE_CPD_BreakpointCount (lstore))
	cElements = 0;
    else {
	unsigned int xElements = rtLSTORE_CPD_BreakpointArray (lstore)[(referenceValue)];
	cElements = rtLSTORE_CPD_BreakpointArray (lstore)[(referenceValue) + 1] - xElements;

	rtLINK_AppendRange (pElementSelection, xElements, cElements);
	rtLINK_AppendRepeat (pElementExpansion, 0, cElements);
    }

/***** Close the links and return: *****/
    M_CPD *newPosPToken = rtPTOKEN_New (lstore->ScratchPad (), cElements);
    pElementSelection->Close (newPosPToken);
    pElementExpansion->Close (newPosPToken);
    newPosPToken->release ();
}


/*---------------------------------------------------------------------------
 ***** L-Store Extraction Routine.
 *
 *  Arguments:
 *	rSourceDescriptor	- the address of the descriptor that names the
 *				  lists of interest.
 *	pElementCluster		- the return address for a CPD identifying the
 *				  element cluster.
 *	pElementSelection	- the return address for a link constructor
 *				  identifying the list elements of interest.
 * 	pElementExpansion	- the address of a link constructor pointer
 *				  which will be set to the position of 'linkc'
 *				  associated with each extracted element.
 *	pElementReordering	- a pointer to a standard CPD for a reference
 *				  uvector which will be set by the extraction
 *				  process if any list is mentioned more than
 *				  once.  Used to reorder the element selection
 *				  link constructor.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtLSTORE_Extract (
    DSC_Descriptor	 &rSourceDescriptor,
    M_CPD		*&pElementCluster,
    rtLINK_CType	*&pElementSelection,
    rtLINK_CType	*&pElementExpansion,
    M_CPD		*&pElementReordering
) {
/*****  Get the lstore out of 'rSourceDescriptor' ... *****/
    M_CPD *pLStoreCPD = rSourceDescriptor.storeCPD ();
    ValidateBreakpointConsistency (pLStoreCPD);

/*****  Decode the descriptor pointer ... *****/
    if (rSourceDescriptor.holdsALink ()) {
	LCExtract (
	    pLStoreCPD,
	    DSC_Descriptor_Link (rSourceDescriptor),
	    pElementSelection,
	    pElementExpansion,
	    pElementReordering
	);
    }
    else if (rSourceDescriptor.holdsAScalarReference ()) {
	RefExtract (
	    pLStoreCPD,
	    DSC_Descriptor_Scalar (rSourceDescriptor),
	    pElementSelection,
	    pElementExpansion
	);
	pElementReordering = NilOf (M_CPD *);
    }
    else if (rSourceDescriptor.holdsValuesInAnyForm ()) {
	M_CPD *pPosPToken = rSourceDescriptor.PPT ();

	CreateEmptyResult (pLStoreCPD, pPosPToken, pElementSelection, pElementExpansion);
	pElementReordering = NilOf (M_CPD*);

	pPosPToken->release ();
    }
    else ERR_SignalFault (
	EC__InternalInconsistency, "rtLSTORE_Extract: Invalid Descriptor Type"
    );

/*****  Finally, return a CPD for the element cluster, ... *****/
    pElementCluster = rtLSTORE_CPD_ContentCPD (pLStoreCPD);
}


/*---------------------------------------------------------------------------
 ***** L-Store Link Constructor Subscript Routine.
 *
 *  Arguments:
 *	lstore		- a standard CPD for the L-Store to subscript in.
 *	linkc		- the address of a link constructor specifying
 *			which lists to subscript in.
 *	keyCPD		- a standard CPD for an integer uvector
 *			specifying the list positions to subscript in
 *			  the selected lists.  'keyCPD' must be positionally
 *			  related to 'linkc'.
 *	keyModifier	- an integer to be added to each value in 'keyCPD'.
 *			  This usually either 0 or -1.  It is used to
 *			  simulate 1 based subscripts.
 *	theResultDsc	- a pointer to a descriptor which this routine
 *			  will set to the result of the subscript.
 *	theLocatedLinkc	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will not be created
 *			  either if all of the values were found or if all
 *			of the values were not found. (It will be created
 *			if some values were found and some not found).  It
 *			will contain the positions in 'keyCPD' for which a
 *			  value was found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void LCSubscript (
    M_CPD		 *lstore,
    rtLINK_CType	 *linkc,
    M_CPD		 *keyCPD,
    int			 keyModifier,
    DSC_Descriptor	 *theResultDsc,
    rtLINK_CType	**theLocatedLinkc
) {
#define locateList(list) {\
    listOffset = rtLSTORE_CPD_BreakpointArray (lstore)[list];\
    listCnt =\
	rtLSTORE_CPD_BreakpointArray (lstore)[(list) + 1] - listOffset;\
}

#define outputPosition(list) {\
    position = keyModifier + *positionPtr++;\
    if (list >= (int) rtLSTORE_CPD_BreakpointCount (lstore) ||\
	position < 0 || position >= listCnt\
    ) {\
	/*****  Not Found  *****/\
	/* Do Nothing */\
    }\
    else {\
	/*****  Found  *****/\
	rtLINK_AppendRange (locatedLinkc, elementCount, 1);\
	*resultPtr++ = listOffset + position;\
    }\
\
    elementCount++;\
}

#define handleNil(subscriptOffset, subscriptCnt, subscriptValue)

#define handleRange(subscriptOffset, subscriptCnt, subscriptValue) {\
    for (int _i_=0; _i_<(subscriptCnt); _i_++) {\
	locateList (subscriptValue);\
	outputPosition (subscriptValue);\
	subscriptValue++;\
	subscriptOffset++;\
    }\
}

#define handleRepeat(subscriptOffset, subscriptCnt, subscriptValue) {\
    locateList (subscriptValue);\
\
    for (int _i_=0; _i_<(subscriptCnt); _i_++) {\
	outputPosition (subscriptValue);\
    }\
}


/*---------------------------------------------------------------------------
 *  Code Body: 'LCSubscript'
 *---------------------------------------------------------------------------
 */
    LCSubscriptCount++;

/*****
 *  Align the arguments and validate the link constructor as an extraction
 *  index for the L-Store ...
 *****/
    linkc->AlignForExtract (rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken);

/*****  Align and validate the key integer uvector ...  *****/
    RTYPE_MustBeA("rtLSTORE LCSubscript", M_CPD_RType (keyCPD), RTYPE_C_IntUV);
    rtINTUV_Align (keyCPD);

/*****  Validate that 'keyCPD' and 'linkc' are positionally related ...  *****/
    if (keyCPD->ReferenceDoesntName (UV_CPx_PToken, linkc->PPT ())) ERR_SignalFault (
	EC__InternalInconsistency, "rtLSTORE LCSubscript: Positional Inconsistency"
    );

/*****  If tracing ...  *****/
    if (TracingCalls) IO_printf (
	"rtLSTORE_LCSubscript: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	linkc->ElementCount (),
	rtLINK_LC_RRDCount (linkc)
    );

/*****  Create the space for the result positions ... *****/
    unsigned int elementCount = linkc->BaseElementCount ();
    rtREFUV_ElementType *result = (rtREFUV_ElementType *) UTIL_Malloc (
	sizeof (rtREFUV_ElementType) * elementCount
    );
    rtREFUV_ElementType *resultPtr = result;

/*****  Create 'locatedLinkc' ...  *****/
    rtLINK_CType *locatedLinkc = *theLocatedLinkc = rtLINK_RefConstructor (
	linkc->PPT (), -1
    );

/***** Do the traversal ... *****/
    rtINTUV_ElementType *positionPtr = rtINTUV_CPD_Array (keyCPD);
    elementCount = 0;

    int listOffset, listCnt, position;
    rtLINK_TraverseRRDCList (linkc, handleNil, handleRepeat, handleRange);

/*****  Determine how many positions were found ...  *****/
    unsigned int numFound = locatedLinkc->ElementCount ();

    if (numFound == 0) {
	/***** NONE FOUND *****/

	theResultDsc->constructNA (linkc->PPT (), lstore->KOT ());

   	locatedLinkc->release ();
	*theLocatedLinkc = NilOf (rtLINK_CType*);
	UTIL_Free (result);

	return;
    }

/*****  Create the locatedLinkc if neccessary (if not all found) ...  *****/
    bool allFound = elementCount == numFound;

    M_CPD *foundPosPToken;
    if (allFound) {
	/*****  All Found  *****/
	locatedLinkc->release ();
	*theLocatedLinkc = NilOf (rtLINK_CType*);
    }
    else {
	/*****  Only Some Found  *****/
	foundPosPToken = rtPTOKEN_New (keyCPD->ScratchPad (), numFound);
	locatedLinkc->Close (foundPosPToken);
    }

/*****  Create the result reference uvector ...  *****/
    M_CPD *resultUV;
    if (allFound) resultUV = rtREFUV_New (
	linkc->PPT (), lstore, rtLSTORE_CPx_ContentPToken
    );
    else {
	resultUV = rtREFUV_New (
	    foundPosPToken, lstore, rtLSTORE_CPx_ContentPToken
	);
	foundPosPToken->release ();
    }

    memcpy (
	rtREFUV_CPD_Array (resultUV), result, numFound * sizeof (rtREFUV_ElementType)
    );
    UTIL_Free (result);

/*****  Create the result descriptor ...  *****/
    theResultDsc->constructMonotype (rtLSTORE_CPD_ContentCPD (lstore), resultUV);
    resultUV->release ();

    return;

/***** Delete the LCSubscript macros ... *****/
#undef locateList
#undef outputPosition
#undef handleNil
#undef handleRange
#undef handleRepeat
}


/*---------------------------------------------------------------------------
 ***** L-Store Reference Subscript Routine.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to extract
 *				from.
 *	reference 		- the address of a reference specifying
 *				which list to extract.
 *	key			- an integer specifying which position of the
 *				selected list to extract.
 *	keyModifier		- an integer to be added to 'key'.
 *				  This usually either 0 or -1.  It is used to
 *				  simulate 1 based subscripts.
 *	resultDsc		- a pointer to a descriptor which this routine
 *				  will set to the result of the subscript.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void RefSubscript (
    M_CPD			*lstore,
    rtREFUV_TypePTR_Reference	reference,
    int				key,
    int				keyModifier,
    DSC_Descriptor		*resultDsc
) {
/*****  Define the Subscript Macros  *****/
#define NotFound -1
#define WasntFound(x) (x == NotFound)

#define locate(list, position, result) {\
    int	listOffset, listCnt;\
\
    if ((list) >= (int) rtLSTORE_CPD_BreakpointCount (lstore))\
	result = NotFound;\
    else {\
	listOffset = rtLSTORE_CPD_BreakpointArray (lstore)[(list)];\
	listCnt   =\
	    rtLSTORE_CPD_BreakpointArray (lstore)[(list) + 1] - listOffset;\
\
	if ((position) < 0 || (position) >= listCnt)\
	    result = NotFound;\
	else\
	    result = listOffset + (position);\
    }\
}


/*---------------------------------------------------------------------------
 *  Code Body: 'RefSubscript'
 *---------------------------------------------------------------------------
 */
    RefSubscriptCount++;

/*****
 *  Align the arguments and validate the reference as an extraction
 *****/
    rtREFUV_AlignAndValidateRef (
	reference, rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken
    );

    if (TracingCalls) IO_printf (
	"rtLSTORE_RefSubscript: cpd=%08X, lsBpaSz=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore)
    );

/*****  Get the value out of the reference ... *****/
    int list = (int) rtREFUV_Ref_Element (reference);

/*****  Do the extraction ... *****/
    int result;
    locate (list, key + keyModifier, result);

/*****  Create the result descriptor ...  *****/
    if (WasntFound (result))
	resultDsc->constructNA (lstore->KOT ());
    else {
	resultDsc->constructReferenceScalar (
	    rtLSTORE_CPD_ContentCPD (lstore),
	    rtLSTORE_CPD_ContentPTokenCPD (lstore),
	    result
	);
    }

    return;

/***** Delete the RefSubscript macros ... *****/
#undef NotFound
#undef WasntFound
#undef locate
}


/*---------------------------------------------------------------------------
 ***** L-Store Subscript Routine.
 *
 *  Arguments:
 *	sourceDsc	- a pointer to a descriptor whose store
 *			  contains the L-Store to extract from.
 *			  The descriptor must be either a Link
 *			Descriptor or a Reference Scalar Descriptor
 *			  whose pointer specifies which lists to
 *			extract.
 *	keyDsc		- a pointer to a descriptor whose Pointer contains
 *			  either an integer scalar pointer or an integer
 *			  value pointer.  This set of integers specifies
 *			  the list positions to subscript in the
 *			  selected lists.  The Pointer in 'keyDsc' must
 *			be positionally related to the Pointer in
 *			'sourceDsc'.
 *	keyModifier	- an integer to be added to each value in 'keyDsc'.
 *			  This usually either 0 or -1.  It is used to
 *			simulate 1 based subscripts.
 *	resultDsc	- a pointer to a descriptor which this routine will
 *			set to the result of the subscript.
 *	locatedLinkc	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found or not found
 *			(if some were found and some not found).  It will
 *			contain the positions in 'keyDsc' for which a value
 *			  was found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtLSTORE_Subscript (
    DSC_Descriptor	 *sourceDsc,
    DSC_Descriptor	 *keyDsc,
    int			  keyModifier,
    DSC_Descriptor	 *resultDsc,
    rtLINK_CType	**locatedLinkc
) {
/*****  Get the lstore out of 'sourceDsc' ... *****/
    M_CPD *pLStoreCPD = sourceDsc->storeCPD ();
    ValidateBreakpointConsistency (pLStoreCPD);

/*****  Decode the descriptor pointer ... *****/
    if (sourceDsc->holdsALink ()) {
	if (!keyDsc->holdsNonScalarValues ()) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLSTORE_Subscript: the key descriptor must be a value dsc"
	);

	LCSubscript (
	    pLStoreCPD, 
	    DSC_Descriptor_Link (*sourceDsc),
	    DSC_Descriptor_Value (*keyDsc),
	    keyModifier,
	    resultDsc,
	    locatedLinkc
	);
    }
    else if (sourceDsc->holdsAScalarReference ()) {
	if (keyDsc->isntScalar () || DSC_Scalar_RType (
		DSC_Descriptor_Scalar (*keyDsc)
	    ) != RTYPE_C_IntUV
	) ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLSTORE_Subscript: Key Descriptor Must Be An Integer Scalar"
	);

	RefSubscript (
	    pLStoreCPD,
	    &DSC_Descriptor_Scalar (*sourceDsc),
	    DSC_Descriptor_Scalar_Int (*keyDsc),
	    keyModifier,
	    resultDsc
	);
	*locatedLinkc = NilOf (rtLINK_CType*);
    }
    else if (sourceDsc->holdsValuesInAnyForm ()) {
	M_CPD *posPToken = sourceDsc->PPT ();

	resultDsc->constructNA (posPToken, pLStoreCPD->KOT ());
	*locatedLinkc = NilOf (rtLINK_CType*);

	posPToken->release ();
    }
    else ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_Subscript: Invalid Descriptor Type"
    );
}


/***************************************************
 *****  Element Append And Insertion Routines  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to append a position at the end of each list selected by
 *****  selectionLC
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store containing the
 *				  lists being modified.
 *	selectionLC		- a link constructor selecting the lists to be
 *				  modified.
 *	elementLC		- a link constructor which will be made to
 *				  reference the actual elements that have been
 *				  appended.
 *
 *  Returns:
 *	Nothing. Executed for side effects.
 *
 *****/
PublicFnDef void rtLSTORE_AppendToLCSelectedList (
    M_CPD *lstore, rtLINK_CType *selectionLC, rtLINK_CType **elementLC
) {


/*---------------------------------------------------------------------------
 * AppendToLCSelectedList: Local Definitions
 *---------------------------------------------------------------------------
 */
    M_CPD*
	newContentPToken;
    unsigned int
	breakpointCount,
	*breakpointArray,
	*breakpointPtr,
	totalElementsInserted;

#define adjustEntries(through, by) for (\
    unsigned int *p = breakpointArray + (through);\
    breakpointPtr < p;\
    *breakpointPtr++ += (by)\
)

#define nilReferenceHandler(c, n, r)\
    adjustEntries (r, totalElementsInserted)

#define repeatedReferenceHandler(c, n, r) {\
    adjustEntries (r, totalElementsInserted);\
    unsigned int oldListSize = breakpointPtr[1] - breakpointPtr[0];\
    unsigned int oldListEnd = oldListSize + (\
	*breakpointPtr++ += totalElementsInserted\
    );\
    rtLINK_AppendRange (*elementLC, oldListEnd, n);\
    newContentPTokenC->AppendAdjustment (oldListEnd, n);\
    totalElementsInserted += n;\
}

#define rangeReferenceHandler(c, n, ir) {\
    unsigned int ur = ir;\
    adjustEntries (ur, totalElementsInserted);\
    while (n-- > 0 && ur++ < breakpointCount) {\
	unsigned int oldListSize = breakpointPtr[1] - breakpointPtr[0];\
	unsigned int oldListEnd  = oldListSize + (\
	    *breakpointPtr++ += totalElementsInserted++\
	);\
	rtLINK_AppendRange (*elementLC, oldListEnd, 1);\
	newContentPTokenC->AppendAdjustment (oldListEnd, 1);\
    }\
}


/*---------------------------------------------------------------------------
 * AppendToLCSelectedList: Algorithm
 *---------------------------------------------------------------------------
 */

    AppendToLCListCount++;

/*****
 * Align the arguments and validate the link constructor as an assignment
 * index for the L-Store,  (note: the use of 'AlignForExtract' is correct)...
 *****/
    selectionLC->AlignForExtract (rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken);

    if (TracingCalls) IO_printf (
	"rtLSTORE_AppendToLCSelList: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore),
	selectionLC->ElementCount (),
	rtLINK_LC_RRDCount (selectionLC)
    );

/*****  ...create the content assignment link constructor, ...  *****/
    *elementLC = rtLINK_PosConstructor (selectionLC->PPT (), -1);

/*****  ...create a constructor for the new content P-Token, ...  *****/
    rtPTOKEN_CType *newContentPTokenC =	rtPTOKEN_NewShiftPTConstructor (
	lstore, rtLSTORE_CPx_ContentPToken
    );

/*****  ...enable modification of the L-Store, ...  *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;

/*****  ...initialize the L-Store traversal variables, ...  *****/
    breakpointPtr   =
    breakpointArray = rtLSTORE_CPD_BreakpointArray (lstore);
    breakpointCount = rtLSTORE_CPD_BreakpointCount (lstore);
    totalElementsInserted = 0;

/*****  ...traverse the link constructor, ...  *****/
    rtLINK_TraverseRRDCList (
	selectionLC, nilReferenceHandler, repeatedReferenceHandler, rangeReferenceHandler
    );

/*****  ...adjust remaining breakpoints for the insertions made, ...  *****/
    adjustEntries (breakpointCount + 1, totalElementsInserted);

/*****  ...close and install the new content P-Token, ...  *****/
    (*elementLC)->Close (newContentPToken = newContentPTokenC->ToPToken ());
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, newContentPToken);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;
    newContentPToken->release ();


/*---------------------------------------------------------------------------
 * AppendToLCSelectedList: Definition Cleanup
 *---------------------------------------------------------------------------
 */

#undef adjustEntries
#undef nilReferenceHandler
#undef repeatedReferenceHandler
#undef rangeReferenceHandler
}


/*---------------------------------------------------------------------------
 *****  Routine to append a position at the end of each list selected by
 *****  selectionRef
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store containing the
 *				  lists being modified.
 *	selectionRef		- a reference selecting the lists to be
 *				  modified.
 *	elementRef		- a reference which will be made to
 *				  the particular element that has been
 *				  appended.
 *
 *  Returns:
 *	Nothing. Executed for side effects.
 *
 *****/
PublicFnDef void rtLSTORE_AppendToRefSelectdList (
    M_CPD*			lstore,
    rtREFUV_TypePTR_Reference	selectionRef,
    rtREFUV_TypePTR_Reference	elementRef
)
{


/*---------------------------------------------------------------------------
 * AppendToRefSelectedList: Local Definitions
 *---------------------------------------------------------------------------
 */
    M_CPD*
	newContentPToken;
    unsigned int
	breakpointCount,
	*breakpointPtr,
	*breakpointLimit,
	referenceValue,
	insertionPosition;


    AppendToRefListCount++;
    if (TracingCalls) IO_printf (
	"rtLSTORE_AppendToRefSelList: cpd=%08X, lsBpaSz=%u\n",
	lstore, rtLSTORE_CPD_BreakpointCount (lstore)
    );

/*****  Align the arguments and validate the reference as an extraction *****/
    rtREFUV_AlignAndValidateRef (
	selectionRef, rtLSTORE_Align (lstore), rtLSTORE_CPx_RowPToken
    );

/*****  Access the reference value and breakpoint count, ...  *****/
    referenceValue  = rtREFUV_Ref_Element (selectionRef);
    breakpointCount = rtLSTORE_CPD_BreakpointCount (lstore);

/*****  If selector is reference Nil, do nothing but set elementRef to
 *****  reference Nil for the content vector.
 *****/
    if (referenceValue >= breakpointCount)
    {
	DSC_InitReferenceScalar (
	    *elementRef,
	    rtLSTORE_CPD_ContentPTokenCPD (lstore),
	    rtLSTORE_CPD_BreakpointArray (lstore) [breakpointCount]
	);

	return;
    }

/*****  ...obtain the element insertion position, ...  *****/
    insertionPosition = rtLSTORE_CPD_BreakpointArray(lstore)[++referenceValue];

/*****  ...create and install a new content P-Token, ...  *****/
    rtPTOKEN_CType *newContentPTokenC = rtPTOKEN_NewShiftPTConstructor (
	lstore, rtLSTORE_CPx_ContentPToken
    );
    newContentPTokenC->AppendAdjustment (insertionPosition, 1);
    newContentPToken = newContentPTokenC->ToPToken ();

/*****  ...enable modification of the L-Store, ...  *****/
    lstore->EnableModifications ();
    lstore->CheckConsistency ();
    rtLSTORE_CPD_IsInconsistent (lstore) = true;
    lstore->StoreReference (rtLSTORE_CPx_ContentPToken, newContentPToken);
    newContentPToken->release ();

/*****  ...set elementRef to point to the appended element, ...  *****/
    DSC_InitReferenceScalar (
	*elementRef,
	rtLSTORE_CPD_ContentPTokenCPD (lstore),
	insertionPosition
    );

/*****  ...and adjust the remaining breakpoints for the insertion made.  *****/
    for (breakpointPtr = rtLSTORE_CPD_BreakpointArray (lstore),
	 breakpointLimit = breakpointPtr + breakpointCount,
	 breakpointPtr += referenceValue;
	 breakpointPtr <= breakpointLimit;
	 ++*breakpointPtr++);
    rtLSTORE_CPD_IsInconsistent (lstore) = false;
}


/***********************************************************
 *****  Standard Representation Type Service Routines  *****
 ***********************************************************/

/***************************
 *  Container Reclamation  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PrivateFnDef void ReclaimContainer (
    M_ASD *ownerASD, M_CPreamble const *preambleAddress
) {
    rtLSTORE_LStore const *lstore = (rtLSTORE_LStore const*)(preambleAddress + 1);

    ownerASD->Release (&rtLSTORE_LStore_RowPToken	(lstore));
    ownerASD->Release (&rtLSTORE_LStore_Content		(lstore));
    ownerASD->Release (&rtLSTORE_LStore_ContentPToken	(lstore));
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Routine to save an L-Store.
 *****/
bool rtLSTORE_Handle::PersistReferences () {
    rtLSTORE_LStore *lstore = (rtLSTORE_LStore*)ContainerContent ();

    return Persist (&rtLSTORE_LStore_RowPToken		(lstore))
	&& Persist (&rtLSTORE_LStore_Content		(lstore))
	&& Persist (&rtLSTORE_LStore_ContentPToken	(lstore));
}


/*********************************************
 *****  Standard Lstore Marking Routine  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'mark' containers referenced by an L-Store.
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void MarkContainers (M_ASD *pSpace, M_CPreamble const *pContainer) {
    pSpace->Mark ((M_POP const*)(pContainer + 1), 3);
}


/****************************************
 *****  Data Base Update Utilities  *****
 ****************************************/

PublicFnDef void rtLSTORE_WriteDBUpdateInfo (M_CPD *lstoreCPD) {
/***** Do NOT Align LStore ... *****/
    ValidateBreakpointConsistency (lstoreCPD);
/***** Output the lstore DB File Record ... *****/
    if (rtLSTORE_CPD_StringStore (lstoreCPD)) {
	int lstoreIndex = lstoreCPD->ContainerIndex ();

	DBUPDATE_OutputLStoreRecord (
	    lstoreIndex,
	    M_POP_ContainerIndex (rtLSTORE_CPD_RowPToken (lstoreCPD))
	);

	DBUPDATE_OutputLStoreContentRec (
	    M_POP_ContainerIndex (rtLSTORE_CPD_Content (lstoreCPD)),
	    M_POP_ContainerIndex (rtLSTORE_CPD_ContentPToken (lstoreCPD))
	);
    }
}


/*************
 *  Printer  *
 *************/

/*---------------------------------------------------------------------------
 *****  Internal routine to display an L-Store.
 *
 *  Arguments:
 *	lstore			- a standard CPD for the L-Store to be
 *				  displayed.
 *	full			- a boolean which, when true, requests a
 *				  detailed display of the L-Store.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void PrintLStore (M_CPD *lstore, bool full) {
    rtLSTORE_Align (lstore);

    IO_printf ("%s{", RTYPE_TypeIdAsString (RTYPE_C_ListStore));
    if (full) {
	IO_printf ("\nRow P-Token----------->");
	RTYPE_Print (lstore, rtLSTORE_CPx_RowPToken);
	IO_printf ("\nStorage P-Token--->");
	RTYPE_Print (lstore, rtLSTORE_CPx_ContentPToken);
    }

    unsigned int n = rtLSTORE_CPD_BreakpointCount (lstore);
    IO_printf ("\nBreak Points (%u)...", n);

    unsigned int *bpp = rtLSTORE_CPD_BreakpointArray (lstore);
    for (unsigned int i = 0; i <= n; i++)
	IO_printf (i % 12 ? " %5d" : "\n    %5d", *bpp++);
    IO_printf ("\nStorage Vector...\n");
    RTYPE_Print (lstore, rtLSTORE_CPx_Content);
    IO_printf ("}");
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (
	rtLSTORE_NewCluster (RTYPE_QRegisterCPD (parameterArray[0]), NilOf (M_CPD*), -1)
    );
}

IOBJ_DefineNewaryMethod (NewUsingContentPrototypeDM) {
    return RTYPE_QRegister (
	rtLSTORE_NewCluster (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1]),
	    -1
	)
    );
}

IOBJ_DefineNewaryMethod (NewUsingLinkDM) {
    rtLINK_CType *pContentMap = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));
    M_CPD *pContentCluster = rtVECTOR_New (pContentMap->PPT ());

    M_CPD *pNewCluster = rtLSTORE_NewCluster (pContentMap, pContentCluster);

    pContentCluster->release ();
    pContentMap->release ();

    return RTYPE_QRegister (pNewCluster);
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nLStore Routines Usage Counts\n");
    IO_printf (  "----------------------------\n");

    IO_printf ("NewCount			= %u\n", NewCount);
    IO_printf ("NewUsingLCInitCount		= %u\n", NewUsingLCInitCount);
    IO_printf ("NewStringStoreCount		= %u\n", NewStringStoreCount);
    IO_printf ("NewStringStoreFromCharUVCount	= %u\n", NewStringStoreFromUVCount);
    IO_printf ("\n");

    IO_printf ("AlignCount			= %u\n", AlignCount);
    IO_printf ("AlignFromContentCount		= %u\n", AlignFromContentCount);
    IO_printf ("AlignUsingLCListsCount		= %u\n", AlignUsingLCListsCount);
    IO_printf ("AlignUsingRefListCount		= %u\n", AlignUsingRefListCount);
    IO_printf ("\n");

    IO_printf ("RefListElementCount		= %u\n", RefListElementCount);
    IO_printf ("LCListElementCount		= %u\n", LCListElementCount);
    IO_printf ("AddListsCount			= %u\n", AddListsCount);
    IO_printf ("\n");

    IO_printf ("LCExtractCount			= %u\n", LCExtractCount);
    IO_printf ("RefExtractCount			= %u\n", RefExtractCount);
    IO_printf ("LCExtractReallocCount		= %u\n", LCExtractReallocCount);
    IO_printf ("\n");

    IO_printf ("LCSubscriptCount		= %u\n", LCSubscriptCount);
    IO_printf ("RefSubscriptCount		= %u\n", RefSubscriptCount);
    IO_printf ("\n");

    IO_printf ("AppendToLCListCount		= %u\n", AppendToLCListCount);
    IO_printf ("AppendToRefListCount		= %u\n", AppendToRefListCount);
    IO_printf ("\n");

    return self;
}

IOBJ_DefineUnaryMethod (InitCountsDM) {
    NewCount			= 0;
    NewUsingLCInitCount		= 0;
    NewStringStoreCount		= 0;
    NewStringStoreFromUVCount	= 0;
    AlignCount			= 0;
    AlignFromContentCount	= 0;
    AlignUsingLCListsCount	= 0;
    AlignUsingRefListCount	= 0;
    RefListElementCount		= 0;
    LCListElementCount		= 0;
    AddListsCount		= 0;
    LCExtractCount		= 0;
    RefExtractCount		= 0;
    LCExtractReallocCount	= 0;
    LCSubscriptCount		= 0;
    RefSubscriptCount		= 0;
    AppendToLCListCount		= 0;
    AppendToRefListCount	= 0;

    return self;
}

IOBJ_DefineNilaryMethod (TracingCallsDM) {
    return IOBJ_SwitchIObject (&TracingCalls);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (BreakpointCountDM) {
    return IOBJ_IntIObject (rtLSTORE_CPD_BreakpointCount (RTYPE_QRegisterCPD (self)));
}

IOBJ_DefineUnaryMethod (RowPTokenDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtLSTORE_CPx_RowPToken);
}

IOBJ_DefineUnaryMethod (ContentDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtLSTORE_CPx_Content);
}

IOBJ_DefineUnaryMethod (ContentPTokenDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtLSTORE_CPx_ContentPToken);
}


IOBJ_DefineUnaryMethod (AlignDM) {
    rtLSTORE_Align (RTYPE_QRegisterCPD (self));
    return self;
}

IOBJ_DefineMethod (AlignListAdjustmentDM) {
    rtREFUV_Type_Reference listRef;
    DSC_InitReferenceScalar (
	listRef,
	rtLSTORE_CPD_RowPTokenCPD (RTYPE_QRegisterCPD (self)),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );
    int adjustment = IOBJ_IObjectValueAsInt (parameterArray[1]);

    rtLSTORE_AlignUsingRefSelctList (RTYPE_QRegisterCPD (self), &listRef, adjustment);

    DSC_ClearScalar (listRef);
    return self;
}

IOBJ_DefineMethod (AlignListsAdjustmentsDM) {
    M_CPD *listLink     = RTYPE_QRegisterCPD (parameterArray[0]);
    M_CPD *adjustmentUV = RTYPE_QRegisterCPD (parameterArray[1]);
    rtLINK_CType *listLC = rtLINK_ToConstructor (listLink);

    rtLSTORE_AlignUsingLCSelctLists (RTYPE_QRegisterCPD (self), listLC, adjustmentUV);

    listLC->release ();
    return self;
}

IOBJ_DefineMethod (AlignListsDM) {
    M_CPD *listLink      = RTYPE_QRegisterCPD (parameterArray[0]);
    rtLINK_CType *listLC = rtLINK_ToConstructor (listLink);

    rtLSTORE_AlignUsingLCSelctLists (
	RTYPE_QRegisterCPD (self), listLC, NilOf (M_CPD*)
    );

    listLC->release ();
    return self;
}

IOBJ_DefineMethod (LCExtractDM) {
    M_CPD *lstore = RTYPE_QRegisterCPD (self);
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));

    rtLINK_CType *pElementSelection, *pElementExpansion; M_CPD *pElementReordering;
    LCExtract (lstore, linkc, pElementSelection, pElementExpansion, pElementReordering);

    IO_printf ("elementSelector:\n");
    pElementSelection->DebugPrint ();

    IO_printf ("pElementExpansion:\n");
    pElementExpansion->DebugPrint ();

    pElementSelection->release ();
    pElementExpansion->release ();
    linkc->release ();

    if (IsntNil (pElementReordering))
	return RTYPE_QRegister (pElementReordering);
    else
	return (self);
}

IOBJ_DefineMethod (RefExtractDM) {
    M_CPD *lstore = RTYPE_QRegisterCPD (self);

    rtREFUV_Type_Reference reference;
    DSC_InitReferenceScalar (
	reference,
	rtLSTORE_CPD_RowPTokenCPD (lstore),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    rtLINK_CType *pElementExpansion, *pElementSelection;
    RefExtract (lstore, reference, pElementSelection, pElementExpansion);

    IO_printf ("elementSelector:\n");
    pElementSelection->DebugPrint ();

    IO_printf ("pElementExpansion:\n");
    pElementExpansion->DebugPrint ();

    pElementSelection->release ();
    pElementExpansion->release ();
    DSC_ClearScalar (reference);

    return (self);
}


IOBJ_DefineMethod (AppendToUsingLCDM) {
    rtLINK_CType *selectionLinkC = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray[0])
    );

    rtLINK_CType *elementLinkC;
    rtLSTORE_AppendToLCSelectedList (
	RTYPE_QRegisterCPD (self), selectionLinkC, &elementLinkC
    );

    selectionLinkC->release ();
    return RTYPE_QRegister (elementLinkC);
}

IOBJ_DefineMethod (AppendToUsingRefDM) {
    M_CPD *lstore = RTYPE_QRegisterCPD (self);

    rtREFUV_Type_Reference selectionRef;
    DSC_InitReferenceScalar (
	selectionRef,
	rtLSTORE_CPD_RowPTokenCPD (lstore),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    rtREFUV_Type_Reference elementRef;
    rtLSTORE_AppendToRefSelectdList (
	RTYPE_QRegisterCPD (self), &selectionRef, &elementRef
    );

    int position = rtREFUV_Ref_Element (&elementRef);

    DSC_ClearScalar (selectionRef);
    DSC_ClearScalar (elementRef);

    return IOBJ_IntIObject (position);
}


IOBJ_DefineMethod (ElementCountUsingRefDM) {
    M_CPD *lstore = RTYPE_QRegisterCPD (self);

    rtREFUV_Type_Reference selectionRef;
    DSC_InitReferenceScalar (
	selectionRef,
	rtLSTORE_CPD_RowPTokenCPD (lstore),
	IOBJ_IObjectValueAsInt (parameterArray[0])
    );

    int count = rtLSTORE_RefSelListElementCount (lstore, &selectionRef);

    DSC_ClearScalar (selectionRef);
    return IOBJ_IntIObject (count);
}

IOBJ_DefineMethod (ElementCountUsingLCDM) {
    rtLINK_CType *selectionLC = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray [0])
    );

    M_CPD *result = rtLSTORE_LCSelListElementCount (RTYPE_QRegisterCPD (self), selectionLC);

    selectionLC->release ();
    return RTYPE_QRegister (result);
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtLSTORE_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"		, NewDM)
	IOBJ_MDE (
	    "newUsingContentPrototype:"	, NewUsingContentPrototypeDM
	)
	IOBJ_MDE ("newUsingLink:"	, NewUsingLinkDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
	IOBJ_MDE ("initCounts"		, InitCountsDM)
	IOBJ_MDE ("tracingCalls"	, TracingCallsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("breakpointCount"	, BreakpointCountDM)
	IOBJ_MDE ("rowPToken"		, RowPTokenDM)
	IOBJ_MDE ("content"		, ContentDM)
	IOBJ_MDE ("contentPToken"	, ContentPTokenDM)
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("alignList:adjustment:"
					, AlignListAdjustmentDM)
	IOBJ_MDE ("alignLists:adjustments:"
					, AlignListsAdjustmentsDM)
	IOBJ_MDE ("alignLists:"		, AlignListsDM)
	IOBJ_MDE ("lcExtract:"		, LCExtractDM)
	IOBJ_MDE ("refExtract:"		, RefExtractDM)
	IOBJ_MDE ("appendUsingRef:"	, AppendToUsingRefDM)
	IOBJ_MDE ("appendUsingLink:"	, AppendToUsingLCDM)
	IOBJ_MDE ("elementCountRef:"	, ElementCountUsingRefDM)
	IOBJ_MDE ("elementCountLink:"	, ElementCountUsingLCDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *const rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtLSTORE_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtLSTORE_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
	break;
    case RTYPE_TypeMD:
	*iArgList.arg<IOBJ_MD*>() = typeMD;
	break;
    case RTYPE_InstanceMD:
	*iArgList.arg<IOBJ_MD*>() = instanceMD;
	break;
    case RTYPE_PrintObject:
	PrintLStore (iArgList.arg<M_CPD*>(), false);
	break;
    case RTYPE_RPrintObject:
	PrintLStore (iArgList.arg<M_CPD*>(), true);
	break;
    default:
	return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtLSTORE);
    FAC_FDFCase_FacilityDescription ("List Store Representation Type Handler");
    default:
	break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlstore.c 1 replace /users/mjc/system
  860404 20:13:17 mjc Create List Store Representation Type

 ************************************************************************/
/************************************************************************
  RTlstore.c 2 replace /users/jad/system
  860422 15:15:55 jad use new io module

 ************************************************************************/
/************************************************************************
  RTlstore.c 3 replace /users/mjc/system
  860530 14:38:16 mjc No further work for now

 ************************************************************************/
/************************************************************************
  RTlstore.c 4 replace /users/mjc/system
  860616 10:03:21 mjc Changed name from 'RTpartitn' to 'RTlstore'

 ************************************************************************/
/************************************************************************
  RTlstore.c 5 replace /users/mjc/system
  860709 10:18:30 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTlstore.c 6 replace /users/mjc/system
  860709 12:11:09 mjc Corrected omission of 'MethodD' and 'VStore' slots from CPD initialization

 ************************************************************************/
/************************************************************************
  RTlstore.c 7 replace /users/mjc/system
  860714 10:52:52 mjc Return L-Store for access by 'spe'

 ************************************************************************/
/************************************************************************
  RTlstore.c 8 replace /users/spe/system
  860718 13:23:18 spe Added optional extractedLStore and reverseLink to rtLSTORE_BasicExtract.

 ************************************************************************/
/************************************************************************
  RTlstore.c 9 replace /users/mjc/system
  860728 13:50:34 mjc Prerelease for db update work

 ************************************************************************/
/************************************************************************
  RTlstore.c 10 replace /users/m2/system
  860804 11:35:40 m2 Added 'rtLSTORE_NewFromRefUVandCharUV()' and debug methods

 ************************************************************************/
/************************************************************************
  RTlstore.c 11 replace /users/jck/system
  860910 14:14:48 jck Added update dump utility

 ************************************************************************/
/************************************************************************
  RTlstore.c 12 replace /users/jad/system
  860914 11:14:33 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  RTlstore.c 13 replace /users/mjc/system
  861002 18:12:31 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  RTlstore.c 13 replace /users/mjc/system
  861002 18:33:36 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  RTlstore.c 14 replace /users/mjc/system
  861028 22:13:02 mjc Add omitted 'EnableModifications' call to 'AppendToLCSelectedLists'

 ************************************************************************/
/************************************************************************
  RTlstore.c 15 replace /users/jad/system
  861222 17:48:32 jad implemented align for the content ptoken

 ************************************************************************/
/************************************************************************
  RTlstore.c 16 replace /users/jad/system
  861224 15:08:08 jad implemented the align routines

 ************************************************************************/
/************************************************************************
  RTlstore.c 17 replace /users/jad/system
  870120 14:59:17 jad modified DB update procedure:

 ************************************************************************/
/************************************************************************
  RTlstore.c 18 replace /users/jad/system
  870123 16:13:45 jad 1) implemented NewStringStore routine.
2) modified the DataBase Update Dump lstore output

 ************************************************************************/
/************************************************************************
  RTlstore.c 19 replace /users/jad/system
  870204 15:27:27 jad implemented another routine to make new string stores

 ************************************************************************/
/************************************************************************
  RTlstore.c 20 replace /users/jad/system
  870205 14:56:55 jad fixed a reference nil limit bug in RefExtract

 ************************************************************************/
/************************************************************************
  RTlstore.c 21 replace /users/jck/system
  870415 09:53:28 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTlstore.c 22 replace /users/jad/system
  870421 12:40:13 jad change interface to the Extract routines

 ************************************************************************/
/************************************************************************
  RTlstore.c 23 replace /users/jck/system
  870511 15:31:05 jck Added a copy routine and an initialized new routine

 ************************************************************************/
/************************************************************************
  RTlstore.c 24 replace /users/jck/system
  870515 08:31:27 jck Fixed a POP problem in InitCopiedLStore

 ************************************************************************/
/************************************************************************
  RTlstore.c 25 replace /users/mjc/translation
  870524 09:33:37 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlstore.c 26 replace /users/jck/system
  870612 13:31:07 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  RTlstore.c 27 replace /users/jck/system
  870804 17:26:44 jck Fixed problems with rtLSTORE_Align

 ************************************************************************/
/************************************************************************
  RTlstore.c 28 replace /users/jck/system
  871007 13:37:45 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTlstore.c 29 replace /users/jck/system
  871016 14:13:54 jck The element counters now align the lstore BEFORE accessing the breakpoint array

 ************************************************************************/
/************************************************************************
  RTlstore.c 30 replace /users/jck/system
  871111 11:17:47 jck Fixed scalar element count for reference Nil list

 ************************************************************************/
/************************************************************************
  RTlstore.c 31 replace /users/jck/system
  880128 16:17:41 jck Added a breakpoint consistency checker

 ************************************************************************/
/************************************************************************
  RTlstore.c 32 replace /users/jck/system
  880226 09:51:15 jck Added Value cases to the extract routine

 ************************************************************************/
/************************************************************************
  RTlstore.c 33 replace /users/jad/system
  880413 10:05:40 jad Release the SequenceLinks Project

 ************************************************************************/
/************************************************************************
  RTlstore.c 34 replace /users/jad/system
  880509 12:12:23 jad Implement 'sprint' for Blocks, Methods, and Closures

 ************************************************************************/
/************************************************************************
  RTlstore.c 35 replace /users/jad/system
  880711 17:02:21 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTlstore.c 36 replace /users/jck/system
  890117 13:31:11 jck Implemented Indexed List entire column deletion

 ************************************************************************/
/************************************************************************
  RTlstore.c 37 replace /users/jck/system
  890223 12:19:59 jck Fortified against saving corrupted lstores. Implemented Comprehensive Align

 ************************************************************************/
/************************************************************************
  RTlstore.c 38 replace /users/jck/system
  890413 13:15:32 jck Added rtLSTORE_DetermineExpansion

 ************************************************************************/
/************************************************************************
  RTlstore.c 39 replace /users/jck/system
  890419 09:24:32 jck Fixed bug in comprehensiveAlign

 ************************************************************************/

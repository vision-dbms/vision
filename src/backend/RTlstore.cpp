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

#include "VOrdered.h"

/*****  Self  *****/
#include "RTlstore.h"


/**************************************
 **************************************
 *****                            *****
 *****  rtLSTORE_Handle::Strings  *****
 *****                            *****
 **************************************
 **************************************/

rtLSTORE_Handle::Strings::Strings () : m_sBreakpoints (0), m_pBreakpoints (0), m_pCharacters () {
}

rtLSTORE_Handle::Strings::~Strings () {
}

void rtLSTORE_Handle::Strings::refresh () {
    if (m_pListStoreHandle) {
	m_sBreakpoints = m_pListStoreHandle->breakpointCount ();
	m_pBreakpoints = m_pListStoreHandle->breakpointArray ();
	m_pCharacters = m_pCharStoreHandle->array ();
    }
}

void rtLSTORE_Handle::Strings::align () {
    if (m_pListStoreHandle) {
	m_pListStoreHandle->align ();
	refresh ();
    }
}

bool rtLSTORE_Handle::Strings::setTo (rtLSTORE_Handle *pStoreHandle) {
    bool bSet = pStoreHandle && pStoreHandle->isAStringStore ();
    if (bSet) {
	m_pListStoreHandle.setTo (pStoreHandle);
	m_pCharStoreHandle.setTo (pStoreHandle->stringContentHandle ());
	align ();
    }
    else {
	m_pListStoreHandle.clear ();
	m_pCharStoreHandle.clear ();
	m_sBreakpoints = 0;
	m_pBreakpoints = 0;
	m_pCharacters = 0;
    }
    return bSet;
}


/*****************************
 *****************************
 *****                   *****
 *****  rtLSTORE_Handle  *****
 *****                   *****
 *****************************
 *****************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtLSTORE_Handle);

/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool rtLSTORE_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.setTo (
	static_cast<rtVSTORE_Handle*>(
	    (isAStringStore () ? TheStringClass () : TheListClass ()).ObjectHandle ()
	)
    );
    return true;
}


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
 * functions like align).
 *---------------------------------------------------------------------------
 */

#define ValidateBreakpointConsistency(lstore) {\
    static_cast<rtLSTORE_Handle*>(lstore->containerHandle ())->CheckBreakpointConsistency ();\
}

void rtLSTORE_Handle::CheckBreakpointConsistency () const {
    if (finalBreakpoint () != rtPTOKEN_BaseElementCount (this, contentPTokenPOP ())) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtLSTORE[%u:%u]: Content/Breakpoint size disagreement.", spaceIndex (), containerIndex ()
	)
    );
}

void rtLSTORE_Handle::CheckConsistency () {
    if (isInconsistent ()) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtLSTORE[%u:%u]: Corruption detected", spaceIndex (), containerIndex ()
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
 *	ptoken			- a standard CPD for the positional P-Token
 *				  for the new L-Store (the number of elements
 *				  in the breakpoint array is one more than the
 *				  base element count of this P-Token).
 *	linkc			- an optional link constructor for link driven
 *				  initialization.
 *	pStringStorage		- an optional string space pointer for string
 *				  driven initialization.
 *
 *
 *****/
void rtLSTORE_Handle::InitNewLStore (
    rtPTOKEN_Handle *ptoken, rtLINK_CType *linkc, char const* pStringStorage
) {
    unsigned int cBreakpoints = ptoken->cardinality ();

/*****  Initialize the CPD, ...  *****/
    setIsInconsistent ();

/*****  ...the POPs, ...  *****/
    constructReference (rowPTokenPOP ());
    StoreReference (rowPTokenPOP (), ptoken);

    constructReference (contentPOP ());
    constructReference (contentPTokenPOP ());

/*****  ...the string store flag, ...  *****/
    clearIsAStringStore ();

/*****  ...the breakpoint count, ...  *****/
    setBreakpointCountTo (cBreakpoints);

/*****  ...and the breakpoint array...  *****/
    unsigned int *pBreakpoints = breakpointArray ();
    if (linkc) {
	/*****  ... using the link content map: *****/
	unsigned int totalElementsViewed = 0;
	unsigned int *pBreakpoint = pBreakpoints;

#	define fillSkippedElements(through, with) {\
	    unsigned int *p = pBreakpoints + (through);\
	    while (pBreakpoint < p)\
		*pBreakpoint++ = (with);\
	}

#	define nilReferenceHandler(c, n, r)\
	fillSkippedElements (r + 1, totalElementsViewed)


#	define repeatedReferenceHandler(c, n, r)\
	fillSkippedElements (r, totalElementsViewed)\
	*pBreakpoint++ = totalElementsViewed;\
	totalElementsViewed += n

#	define rangeReferenceHandler(c, n, r)\
	fillSkippedElements (r, totalElementsViewed)\
	while (n-- > 0) {\
	    *pBreakpoint++ = totalElementsViewed++;\
	}

	rtLINK_TraverseRRDCList (
	    linkc, nilReferenceHandler, repeatedReferenceHandler, rangeReferenceHandler
	);
	fillSkippedElements (cBreakpoints + 1, totalElementsViewed);

#	undef fillSkippedElements
#	undef nilReferenceHandler
#	undef repeatedReferenceHandler
#	undef rangeReferenceHandler
    }
    else if (pStringStorage) {
	/*****  ... using the string content: *****/
	unsigned int xBreakpoint = 0;
	unsigned int iBreakpoint = 0;

	while (xBreakpoint < cBreakpoints) {
	    pBreakpoints[xBreakpoint++] = iBreakpoint;
	    iBreakpoint += strlen (pStringStorage + iBreakpoint) + 1;
	}

	pBreakpoints[xBreakpoint] = iBreakpoint;
    }
    else {
	/*****  ... using the p-token's cardinality:  *****/
	memset (pBreakpoints, 0, (cBreakpoints + 1) * sizeof (unsigned int));
    }
}


/**********************************
 *  Instantiator Implementations  *
 **********************************/

/*---------------------------------------------------------------------------
 *****  General L-Store Cluster Creation Routine
 *
 *  Arguments:
 *	pInstancePPT		- the address of a handle for the new cluster's
 *				  instance set p-token.
 *	pContentPrototype	- the address of a CPD for the new cluster's
 *				  content prototype.
 *
 *  Returns:
 *	A standard CPD for the L-Store created.
 *
 *****/
rtLSTORE_Handle::rtLSTORE_Handle (
    rtPTOKEN_Handle *pInstancePPT, Vdd::Store *pContentPrototype, bool bAStringStore
) : BaseClass (
    pInstancePPT->Space (), RTYPE_C_ListStore, rtLSTORE_SizeofLStore (pInstancePPT->cardinality ())
) {
    NewCount++;

/*****  Create and initialize the L-Store's content...  *****/
    rtPTOKEN_Handle::Reference pContentPPT (new rtPTOKEN_Handle (pInstancePPT->Space (), 0));
    Vdd::Store::Reference pContentStore;
    if (pContentPrototype)
	pContentPrototype->clone (pContentStore, pContentPPT);
    else if (!bAStringStore) {
	pContentStore.setTo (new rtVECTOR_Handle (pContentPPT));
    }

/*****  Create and initialize the L-Store...  *****/
    InitNewLStore (pInstancePPT);

    rtLSTORE_Type_LStore *pContainerContent = typecastContent ();

    StoreReference (&rtLSTORE_LStore_ContentPToken (pContainerContent), pContentPPT);
    if (pContentStore)
	StoreReference (&rtLSTORE_LStore_Content (pContainerContent), pContentStore);
    else {
	M_CPD *pCharUV = rtCHARUV_New (
	    pContentPPT, pInstancePPT->TheCharacterClass ().PTokenHandle ()
	);
	StoreReference (&rtLSTORE_LStore_Content (pContainerContent), pCharUV->containerHandle ());
	pCharUV->release ();
    }

    CheckBreakpointConsistency ();

    pContainerContent->stringStore = pContentStore.isNil () && bAStringStore;
    pContainerContent->isInconsistent = false;
}


/*---------------------------------------------------------------------------
 *****  Cloned L-Store Cluster Creation Routine.
 *
 *  Arguments:
 *	pInstancePPT		- the address of a CPD for the new cluster's
 *				  instance set p-token.
 *	pPrototypeIndexCPD	- the address of a CPD for the cluster's
 *				  new prototype.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
bool rtLSTORE_Handle::isACloneOfListStore (rtLSTORE_Handle const *pOther) const {
    return isAStringStore () == pOther->isAStringStore ();
}

void rtLSTORE_Handle::clone (Reference &rpResult, rtPTOKEN_Handle *pInstancePPT) const {
    Vdd::Store::Reference pContentPrototype;
    getContent (pContentPrototype);
    rpResult.setTo (new rtLSTORE_Handle (pInstancePPT, pContentPrototype, isAStringStore ()));
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
 *	pContentStore		- the address of a handle for the l-store's content.
 *
 *  Returns:
 *	The address of a CPD for the L-Store created.
 *
 *****/
rtLSTORE_Handle::rtLSTORE_Handle (
    rtLINK_CType *pContentMap, Vdd::Store *pContentStore, bool bDecoupled
) : BaseClass (
    bDecoupled ? pContentStore->objectSpace () : pContentMap->RPT()->Space (), RTYPE_C_ListStore, rtLSTORE_SizeofLStore (
	pContentMap->RPT()->cardinality ()
    )
) {
    NewUsingLCInitCount++;

    rtPTOKEN_Handle::Reference pInstancePPT (pContentMap->RPT ());
    if (bDecoupled)
	pInstancePPT.setTo (new rtPTOKEN_Handle (Space (), pInstancePPT->cardinality ()));

    InitNewLStore (pInstancePPT, pContentMap);

    rtLSTORE_Type_LStore *pContainerContent = typecastContent ();

    StoreReference (&rtLSTORE_LStore_ContentPToken (pContainerContent), pContentMap->PPT ());
    StoreReference (&rtLSTORE_LStore_Content (pContainerContent), pContentStore);

    CheckBreakpointConsistency ();

    pContainerContent->stringStore = false;
    pContainerContent->isInconsistent = false;
}


/*---------------------------------------------------------------------------
 *****  String Initialized L-Store Instantiator
 *
 *  Arguments:
 *	cStrings		- the number of strings present in the content cluster.
 *	pContentCluster		- the address of a CPD for the string store's content.
 *
 *  Returns:
 *	The address of a CPD for the L-Store created.
 *
 *****/
rtLSTORE_Handle::rtLSTORE_Handle (
    unsigned int cContentClusterStrings, M_CPD *pContentStore
) : BaseClass (pContentStore->Space (), RTYPE_C_ListStore, rtLSTORE_SizeofLStore (cContentClusterStrings)) {
//  Create the required instance p-token, ...
    rtPTOKEN_Handle::Reference pInstancePPT (new rtPTOKEN_Handle (Space (), cContentClusterStrings));

/*****  Create and initialize the L-Store...  *****/
    InitNewLStore (pInstancePPT, 0, rtCHARUV_CPD_Array (pContentStore));

    rtLSTORE_Type_LStore *pContainerContent = typecastContent ();

    rtPTOKEN_Handle::Reference pContentStorePPT (
	static_cast<rtUVECTOR_Handle*>(pContentStore->containerHandle ())->pptHandle ()
    );
    StoreReference (&rtLSTORE_LStore_ContentPToken (pContainerContent), pContentStorePPT.referent ());
    StoreReference (&rtLSTORE_LStore_Content (pContainerContent), pContentStore->containerHandle  ());

    CheckBreakpointConsistency ();

    pContainerContent->stringStore = true;
    pContainerContent->isInconsistent = false;
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
void rtLSTORE_Handle::copy (DSC_Descriptor &rResult, M_ASD *pResultSpace) {
/*****  Align the source, ...  *****/
    align ();

/*****  ...  copy the content, ...  *****/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!  The Following Code Does Not Support Content Generalized L-Stores  !!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    rtPTOKEN_Handle::Reference pResultContentPPT; M_CPD *pResultContent; {
	VContainerHandle::Reference pSourceContent;
	getContent (pSourceContent);
	switch (pSourceContent->RType ()) {
	case RTYPE_C_Vector: {
		rtVECTOR_Handle::Reference pSourceVector (
		    static_cast<rtVECTOR_Handle*>(pSourceContent.referent ())
		);
		pSourceVector->align ();

		unsigned int nElements = pSourceVector->elementCount ();

		pResultContentPPT.setTo (new rtPTOKEN_Handle (pResultSpace, nElements));

		rtLINK_CType *pElementSelection = rtLINK_RefConstructor (
		    pSourceVector->getPToken ()
		)->AppendRange (0, nElements)->Close (pResultContentPPT);

		pSourceVector->getElements (pResultContent, pElementSelection);

		pElementSelection->release ();
	    }
	    break;

	case RTYPE_C_CharUV: {
		M_CPD *pSourceUVector = pSourceContent->GetCPD ();
		pResultContentPPT.setTo (
		    new rtPTOKEN_Handle (pResultSpace, UV_CPD_ElementCount (pSourceUVector))
		);
		pResultContent = UV_CopyWithNewPToken (pSourceUVector, pResultContentPPT);
		pSourceUVector->release ();
	    }
	    break;

	default:
	    ERR_SignalFault (
		EC__InternalInconsistency, "rtLSTORE_Copy: Unknown Content Type"
	    );
	    break;
	}
    }

/*****  ... access and create PPTs for the two l-stores, ...  *****/
    rtPTOKEN_Handle::Reference pSourcePPT (getPToken ());

    unsigned int nLists = pSourcePPT->cardinality ();

    rtPTOKEN_Handle::Reference pResultPPT (new rtPTOKEN_Handle (pResultSpace, nLists));

/*****  ... bulk copy the l-store, ...  *****/
    size_t cbResult = rtLSTORE_SizeofLStore (breakpointCount ());
    rtLSTORE_Handle *pResultLStore = static_cast<rtLSTORE_Handle*>(
	pResultSpace->CreateContainer (RTYPE_C_ListStore, cbResult)
    );
    memcpy (pResultLStore->typecastContent (), typecastContent (), cbResult);

/*****  ... fill in the new details, ...  *****/
    pResultLStore->setIsInconsistent ();

    pResultLStore->constructReference (pResultLStore->rowPTokenPOP ());
    pResultLStore->constructReference (pResultLStore->contentPOP ());
    pResultLStore->constructReference (pResultLStore->contentPTokenPOP ());

    pResultLStore->StoreReference (
	pResultLStore->rowPTokenPOP (), pResultPPT
    );
    pResultLStore->StoreReference (
	pResultLStore->contentPTokenPOP (), pResultContentPPT
    );
    pResultLStore->StoreReference (
	pResultLStore->contentPOP (), pResultContent->containerHandle ()
    );
    pResultLStore->CheckBreakpointConsistency ();

    pResultLStore->clearIsInconsistent ();

/*****  ... manufacture the source to copy relationship, and pack up the result:  *****/
    rtLINK_CType *pResultMap = rtLINK_PosConstructor (pSourcePPT)->AppendRange (
	0, nLists
    )->Close (pResultPPT);

    rResult.constructMonotype (pResultLStore, pResultMap);

    pResultMap->release ();
    pResultContent->release ();
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
PrivateFnDef rtLSTORE_Handle *CreateStringStore (
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
    rtPTOKEN_Handle::Reference sizePToken (new rtPTOKEN_Handle (pContainerSpace, size));
    M_CPD *charCPD = rtCHARUV_New (
	sizePToken, pContainerSpace->TheCharacterClass ().PTokenHandle ()
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
    rtLSTORE_Handle *lstore = new rtLSTORE_Handle (numberOfStrings, charCPD);
    charCPD->release ();

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
PublicFnDef rtLSTORE_Handle *__cdecl rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, rtLSTORE_StringEnumerator pEnumerator, ...
) {
    V_VARGLIST (pEnumeratorArgs, pEnumerator);

    rtLSTORE_Handle *result = CreateStringStore (
	pContainerSpace, "", "", pEnumerator, pEnumeratorArgs
    );

    return result;
}

PublicFnDef rtLSTORE_Handle *rtLSTORE_NewStringStore (M_ASD *pContainerSpace, char const *pString) {
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
PublicFnDef rtLSTORE_Handle *__cdecl rtLSTORE_NewStringStoreWithDelm (
    M_ASD *pContainerSpace, char const *ldelim, char const *rdelim,
    rtLSTORE_StringEnumerator pEnumerator, ...
) {
    V_VARGLIST (pEnumeratorArgs, pEnumerator);

    rtLSTORE_Handle *result = CreateStringStore (
	pContainerSpace, ldelim, rdelim, pEnumerator, pEnumeratorArgs
    );

    return result;
}

PublicFnDef rtLSTORE_Handle *rtLSTORE_NewStringStoreWithDelm (
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
PublicFnDef rtLSTORE_Handle *rtLSTORE_NewStringStoreFromUV (M_CPD *charUV) {
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
    return new rtLSTORE_Handle (numberOfStrings, charUV);
} 


/*******************************
 *****  Alignment Routine  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to align an L-Store when the content ptoken is not current.
 *
 *  Returns:
 *	'true' if alignments performed, false otherwise.
 *
 *****/
bool rtLSTORE_Handle::AlignFromContent () {
/***** Traversal Macros ... *****/

#define processTo(where) {\
    unsigned int limit = (unsigned int)(where);\
    while (*pBreakpoint <= limit)\
	(*pBreakpoint++) += adjustment;\
}

#define processToEnd() {\
    while (pBreakpoint < pBreakpointLimit)\
	(*pBreakpoint++) += adjustment;\
}

#define handleDelete(ptOrigin, ptShift) {\
    processTo (ptOrigin + ptShift);\
    while (*pBreakpoint < (unsigned int)ptOrigin) {\
	int size = ptOrigin + ptShift - *pBreakpoint;\
	adjustment += size;\
	ptShift -= size;\
	*pBreakpoint++ += adjustment;\
    }\
    adjustment += ptShift;\
}

#define handleInsert(ptOrigin, ptShift) {\
    ERR_SignalFault (\
	EC__InternalInconsistency, "AlignFromContent:  Illegal insertion in content ptoken"\
    );\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'AlignFromContent'
 *---------------------------------------------------------------------------
 */
/*****  Return if the L-Store is already aligned...  *****/
    rtPTOKEN_Handle::Reference pPToken;
    if (isTerminal (contentPTokenPOP (), pPToken))
	return false;

/***** If the row ptoken is also not current, error ... *****/
    if (isntTerminal (rowPTokenPOP ())) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "AlignFromContent:  Both LStore PTokens are not current"
	);
    }

    AlignFromContentCount++;
    if (TracingCalls)
	IO_printf ("AlignFromContent: cpd=%08X\n", this);

/***** Get a ptoken constructor for the content ptoken ... *****/
    rtPTOKEN_CType *ptokenc = pPToken->getAdjustments ();

/***** Don't forget to enable modifications ... *****/
    EnableModifications ();
    CheckConsistency ();
    setIsInconsistent ();

/***** Traverse the referential ptoken *****/
    int adjustment = 0;
    unsigned int *pBreakpoint = breakpointArray ();
    unsigned int const *const pBreakpointLimit = pBreakpoint + breakpointCount () + 1;
    rtPTOKEN_FTraverseInstructions (ptokenc, handleInsert, handleDelete);

/*** process the remaining values ***/
    processToEnd ();

/***** Now that the row state is adjusted, update the content ptoken  *****/
    StoreReference (contentPTokenPOP (), ptokenc->NextGeneration ());
    ptokenc->discard ();

    CheckBreakpointConsistency ();
    clearIsInconsistent ();

/***** All done *****/
    return true;

#undef handleDelete
#undef handleInsert
#undef processTo
#undef processToEnd
}


/*---------------------------------------------------------------------------
 *****  Routine to align an L-Store when the row or content ptoken (not both)
 *****  is not current.
 *
 *  Returns:
 *	true if alignments done, false otherwise.
 *
 *****/
bool rtLSTORE_Handle::align () {
/***** Traversal Macros ... *****/
#define output(value)\
    *newbp++ = (value)

#define processTo(newPos) {\
    while (origPos < (newPos))\
	output(pBreakpoints[origPos++]+contentAdj);\
}

#define originalValue(newPos)\
    (pBreakpoints[(newPos)])

#define handleInsert(ptOrigin, ptShift) {\
    processTo (ptOrigin);\
    int value = originalValue (ptOrigin) + contentAdj;\
    for (int i=0; i < ptShift; i++) output (value);\
}

#define handleDelete(ptOrigin, ptShift) {\
    int ptorig = ptOrigin + ptShift;\
    processTo (ptorig);\
    int at = originalValue (ptorig);\
    int x  = originalValue (ptOrigin);\
    int amt = at - x;\
    contentPTokenC->AppendAdjustment (x + contentAdj, amt);\
    contentAdj += amt;\
    origPos -= ptShift; /* skip past deleted values */\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLSTORE_Handle::align ()'
 *---------------------------------------------------------------------------
 */
    CheckBreakpointConsistency ();

/*****  Return if the L-Store is already aligned...  *****/
    rtPTOKEN_Handle::Reference pPToken;
    if (isTerminal (rowPTokenPOP (), pPToken)) {
	if (isAStringStore ()) {
	    VContainerHandle::Reference pContent;
	    getContent (pContent);
	    pContent->align ();
	}
	return AlignFromContent ();
    }

/***** If the content ptoken is also not current, error ... *****/
    if (isntTerminal (contentPTokenPOP ())) {
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtLSTORE_Handle::align:  Both LStore PTokens are not current"
	);
    }

    AlignCount++;
    if (TracingCalls)
	IO_printf ("rtLSTORE_Handle::align: handle=%08X\n", this);

/***** Get a ptoken constructor for the row ptoken ... *****/
    rtPTOKEN_CType *ptokenc = pPToken->getAdjustments ();

/***** Make a ptoken constructor for the content ptoken ... *****/
    getContentPToken (pPToken);
    rtPTOKEN_CType *contentPTokenC = pPToken->makeAdjustments ();

/***** Determine the sizes ... *****/
    unsigned int oldSize = breakpointCount ();
    unsigned int newSize = ptokenc->NextGeneration ()->cardinality ();

/***** Allocate some temporary space for the breakpoint table values ... *****/
/*** (remember, the breakpoint table is 1 larger than the size.) ***/
    unsigned int *const newbpstart = (unsigned int*)UTIL_Malloc ((newSize + 1) * sizeof (unsigned int));
    unsigned int *newbp = newbpstart;

/***** Setup for the traversal *****/
    int contentAdj = 0;
    int origPos = 0;

/***** Traverse the referential ptoken *****/
    unsigned int *pBreakpoints = breakpointArray ();
    rtPTOKEN_FTraverseInstructions (ptokenc, handleInsert, handleDelete);
    /*** process the remaining values ***/
    processTo (oldSize + 1);

/***** Reconstruct the breakpoint array ... *****/
//    rtLSTORE_CPD_Breakpoint (lstore) = rtLSTORE_CPD_BreakpointArray (lstore) + oldSize + 1;

/*****  Don't forget to enable modifications ... *****/
    EnableModifications ();
    CheckConsistency ();

    setIsInconsistent ();

    ShiftContainerTail (
	breakpointLimit (), 0, ((ptrdiff_t)newSize - oldSize) * sizeof (unsigned int), true
    );

    setBreakpointCountTo (newSize);

    /*** copy in the new values ... ***/
    memcpy (breakpointArray (), newbpstart, sizeof (unsigned int) * (newSize + 1));
    UTIL_Free (newbpstart);


/***** Fix the row ptoken to be the new one ... *****/
    StoreReference (rowPTokenPOP (), ptokenc->NextGeneration ());
    ptokenc->discard ();

/***** Close the content ptoken and copy it into the lstore ... *****/
    pPToken.setTo (contentPTokenC->ToPToken ());
    StoreReference (contentPTokenPOP (), pPToken);

    CheckBreakpointConsistency ();
    clearIsInconsistent ();

    if (isAStringStore ()) {
	VContainerHandle::Reference pContent;
	getContent (pContent);
	pContent->align ();
    }

/***** All done *****/
    return true;
}


/*---------------------------------------------------------------------------
 *****  Routine to align the lstore and all structures which make up the
 *****  lstore.
 *
 *  Arguments:
 *	bCleaning		- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	true if any alignments were necessary. false otherwise.
 *
 *****/
bool rtLSTORE_Handle::alignAll (bool bCleaning) {
    bool result = isTerminal (rowPTokenPOP ()) && isTerminal (contentPTokenPOP ()) ? false : align ();

    VContainerHandle::Reference pContent;
    getContent (pContent);
    return pContent->alignAll (bCleaning) || result;
}


/*---------------------------------------------------------------------------
 ***** Routine to align an lstore when the content ptoken is not current.
 *
 *  Arguments:
 *	listSelectorLC		- a pointer to a link constructor containing
 *				  the lists which need to be updated.
 *	adjustmentUV		- optional (Nil if omitted).
 *				  a standard CPD for an integer uvector
 *				  containing the amount that each list in
 *				  'listSelectorLC' needs to be modified by.
 *				  If omitted, the adjustment amount defaults
 *				  to +1.
 *
 *  Returns:
 *	Nothing
 *
 *****/
void rtLSTORE_Handle::alignUsingSelectedLists (rtLINK_CType *listSelectorLC, M_CPD *adjustmentUV) {
/***** The list selector link constructor traversal macros: *****/
#define currentAdjustment (bNoAdjustments ? 1 : (*adjustmentp++))

#define outputThru(position) {\
    while (xBreakpoint <= position)\
	pBreakpoints[xBreakpoint++] += cumulativeAdjustment;\
}

#define handleRange(position, count, value) {\
    outputThru ((unsigned int)value);\
    while (count > 0) {\
	cumulativeAdjustment += (currentAdjustment);\
	pBreakpoints[xBreakpoint++] += cumulativeAdjustment;\
	count--;\
	value++;\
    }\
}

#define handleRepeat(position, count, value) {\
    outputThru ((unsigned int)value);\
    /*** add up all of the adjustments and apply once ***/\
    while (count > 0) {\
	cumulativeAdjustment += (currentAdjustment);\
	count--;\
    }\
    pBreakpoints[xBreakpoint++] += cumulativeAdjustment;\
}

#define handleNil(position, count, value)\
    /* nada */


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'rtLSTORE_AlignUsingLCSelctLists'
 *---------------------------------------------------------------------------
 */
    CheckBreakpointConsistency ();

/*****  Return if the content ptoken is already aligned ...  *****/
    if (isTerminal (contentPTokenPOP ()))
	return;

/***** If the row ptoken is also not current, error ... *****/
    if (isntTerminal (rowPTokenPOP ())) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_AlignUsingLCSelectedLists: One P-Token Must Be Current"
    );

    AlignUsingLCListsCount++;

/***** Determine if 'adjustmentUV' was specified ... *****/
    bool bNoAdjustments = IsNil (adjustmentUV) ? true : false;

/***** Align and validate the linkc ... *****/
    rtINTUV_ElementType *adjustmentp = 0;
    if (bNoAdjustments)
	listSelectorLC->AlignForExtract (this, rowPTokenPOP ());
    else {
	listSelectorLC->AlignForAssign (
	    this, rowPTokenPOP (), adjustmentUV->containerHandle (), UV_CPD_PToken (adjustmentUV)
	);
	adjustmentp = rtINTUV_CPD_Array (adjustmentUV);
    }

    unsigned int cBreakpoints = breakpointCount ();
    if (TracingCalls) IO_printf (
      "rtLSTORE_AlignUseLCLists: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, cBreakpoints, listSelectorLC->ElementCount (), rtLINK_LC_RRDCount (listSelectorLC)
    );

/*****  Don't forget to enable modifications ... *****/
    EnableModifications ();
    CheckConsistency ();
    setIsInconsistent ();

/*****  Modify the break point array ... *****/
/*****  Traverse the selector linkc ... *****/
    unsigned int *pBreakpoints = breakpointArray ();
    unsigned int xBreakpoint = 0;
    int cumulativeAdjustment = 0;

    rtLINK_TraverseRRDCList (
	listSelectorLC, handleNil, handleRepeat, handleRange
    );

/*****  Output the rest ... *****/
    outputThru (listSelectorLC->ReferenceNil ());

/***** Set the content PToken to the new one ... *****/
    rtPTOKEN_Handle::Reference pOldContentPToken;
    getContentPToken (pOldContentPToken);

    rtPTOKEN_Handle::Reference pNewContentPToken (pOldContentPToken->basePToken ());
    StoreReference (contentPTokenPOP (), pNewContentPToken);

/***** and return the aligned lstore ... *****/
    CheckBreakpointConsistency ();
    clearIsInconsistent ();

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
 *	rInstance		- a pointer to a reference specifying which
 *				  list needs to be updated.
 *	adjustment		- an integer specifying the amount that the
 *				list needs to be modified by.
 *
 *****/
void rtLSTORE_Handle::alignUsingSelectedLists (
    DSC_Scalar &rInstance, int adjustment
) {
    CheckBreakpointConsistency ();

/*****  Return if the content ptoken is already aligned ...  *****/
    if (isTerminal (contentPTokenPOP ()))
	return;

/***** If the row ptoken is also not current, error ... *****/
    if (isntTerminal (rowPTokenPOP ())) ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_AlignUsingRefSelectedList: One PToken Must Be Current"
    );

    AlignUsingRefListCount++;

    unsigned int cBreakpoints = breakpointCount ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_AlignUseRefList: cpd=%08X, lsBpaSz=%u\n", this, cBreakpoints
    );

/***** Align and validate the reference ... *****/
    rtREFUV_AlignAndValidateRef (&rInstance, this, rowPTokenPOP ());

/*****  Don't forget to enable modifications ... *****/
    EnableModifications ();
    CheckConsistency ();
    setIsInconsistent ();

/*****  Modify the break point array ... *****/
    unsigned int *pBreakpoints = breakpointArray ();
    for (unsigned int i= DSC_Scalar_Int (rInstance) + 1; i <= cBreakpoints; i++)
	pBreakpoints[i] += (adjustment);

/***** Set the content PToken to the new one ... *****/
    rtPTOKEN_Handle::Reference pOldContentPToken;
    getContentPToken (pOldContentPToken);

    rtPTOKEN_Handle::Reference pNewContentPToken (pOldContentPToken->basePToken ());
    StoreReference (contentPTokenPOP (), pNewContentPToken);

/***** and return the aligned lstore ... *****/
    CheckBreakpointConsistency ();
    clearIsInconsistent ();
}


/**********************************
 *****  List Addition Routine  *****
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to add a specified number of rows to the end of a list store.
 *
 *  Arguments:
 *	pAdditionsPPT		- a handle for a P-Token specifying the lists
 *				  to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the list store.  The
 *	positional P-Token of this link constructor will be 'pAdditionsPPT'.
 *
 *****/
rtLINK_CType *rtLSTORE_Handle::addInstances_(rtPTOKEN_Handle *pAdditionsPPT) {
    AddListsCount++;
    if (TracingCalls)
	IO_printf ("rtLSTORE_AddLists: cpd=%08X\n", this);

/*****  First make sure that the row ptoken is current ...  *****/
    align ();

/*****  ...  and then modify it ...  *****/
    rtPTOKEN_Handle::Reference pStorePPT (getPToken ());
    rtPTOKEN_CType *ptc = pStorePPT->makeAdjustments ();

    unsigned int origin = rtPTOKEN_PTC_BaseCount (ptc);
    unsigned int numLists = pAdditionsPPT->cardinality ();

    pStorePPT.setTo (ptc->AppendAdjustment (origin, numLists)->ToPToken ());

/*****  ... cause the rows to be added  ...  *****/
    align ();

/*****  ...create a link constructor for the added rows...  *****/
    rtLINK_CType *pResult = rtLINK_AppendRange (
	rtLINK_RefConstructor (pStorePPT), origin, numLists
    )->Close (pAdditionsPPT);

/*****  ...and return.  *****/
    return pResult;
}


/************************************
 *****  Content Query Routines  *****
 ************************************/

/*---------------------------------------------------------------------------
 *****  Routine to get the number of elements in a list.
 *
 *  Arguments:
 *	rResult			- the address at which the element count
 *				  will be returned.
 *	rSubscript		- a reference specifying which list to be
 *				  counted.
 *
 *  Returns:
 *	true
 *
 *****/
bool rtLSTORE_Handle::getCardinality (unsigned int &rResult, DSC_Scalar &rSubscript) {
    RefListElementCount++;

    CheckBreakpointConsistency ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_RefSelListElementCount: cpd=%08X", this
    );

/*****  Align the arguments and validate the reference as an extraction *****/
    align ();
    rtREFUV_AlignAndValidateRef (&rSubscript, this, rowPTokenPOP ());

/*****  Get the value out of the reference ... *****/
    unsigned int xElement = DSC_Scalar_Int (rSubscript);
    unsigned int const *pBreakpoints = breakpointArray ();

/*---------------------------------------------------------------------------
 * If the xElement is the Nil value (i.e. equal to the breakpoint count)
 * return 0; otherwise return the number of elements as calculated via the
 * breakpoint array
 *---------------------------------------------------------------------------
 */
    rResult = xElement >= breakpointCount () ? 0 : pBreakpoints[xElement + 1] - pBreakpoints [xElement];
    return true;
}


/*---------------------------------------------------------------------------
 *****  Routine to get the number of elements in each of a set of lists.
 *
 *  Arguments:
 *	rpResult		- the address that will receive an integer
 *				  u-vector containing the element counts of
 *				  the specified lists.
 *	pSubscript		- a reference specifying which list to be
 *				  counted.
 *
 *  Returns:
 *	true
 *
 *****/
bool rtLSTORE_Handle::getCardinality (M_CPD *&rpResult, rtLINK_CType *pSubscript) {
#define nilReferenceHandler(c, n, r)\
    while (n-- > 0)\
	*resultPtr++ = 0

#define repeatedReferenceHandler(c, n, r)\
    while (n-- > 0)\
	*resultPtr++ = pBreakpoints [r + 1] - pBreakpoints [r]

#define rangeReferenceHandler(c, n, r)\
    while (n-- > 0) {\
	*resultPtr++ = pBreakpoints [r + 1] - pBreakpoints [r];\
	r++;\
    }

    LCListElementCount++;

/*****
 * Align the arguments and validate the link constructor as an extraction
 * index for the L-Store.
 *****/
    align ();
    pSubscript->AlignForExtract (this, rowPTokenPOP ());

    if (TracingCalls) IO_printf (
	"rtLSTORE_LCSelListElementCount: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, breakpointCount (),
	pSubscript->ElementCount (),
	rtLINK_LC_RRDCount (pSubscript)
    );

/*****  Create the result uvector  *****/
    rpResult = rtINTUV_New (pSubscript->PPT (), KOT ()->TheIntegerPTokenHandle ());
    rtINTUV_ElementType *resultPtr = rtINTUV_CPD_Array (rpResult);

/***** Access the breakpoint array *****/
    unsigned int *pBreakpoints = breakpointArray ();

/*****  Do the counting  *****/
    rtLINK_TraverseRRDCList (
	pSubscript, nilReferenceHandler, repeatedReferenceHandler, rangeReferenceHandler
    );

    return true;

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
 *	pInstances		- the address of a link constructor specifying
 *				  which lists to extract.
 *
 *  Returns:
 *	The element expansion.
 *
 *****/
void rtLSTORE_Handle::getExpansion (rtLINK_CType *&rpResult, rtLINK_CType *pInstances) {
    unsigned int
	elementCnt = 0,
	srcValue,
	srcCnt;
    int i;

/*****  Define the Traversal Macros  *****/
#define locateSource(subscriptValue) {\
    srcValue = pBreakpoints[(subscriptValue)];\
    srcCnt = pBreakpoints[(subscriptValue) + 1] - srcValue;\
}

#define outputExpansionLinkc(value, cnt) {\
    elementCnt += (cnt);\
    rtLINK_AppendRepeat (rpResult, value, cnt);\
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
 *  Code Body: 'rtLSTORE_Handle::getExpansion'
 *---------------------------------------------------------------------------
 */

/*****
 *  Align the arguments and validate the link constructor as an extraction
 *  index for the L-Store ...
 *****/
    align ();
    pInstances->AlignForExtract (this, rowPTokenPOP ());

    unsigned int cBreakpoints = breakpointCount ();
    unsigned int*pBreakpoints = breakpointArray ();
    if (TracingCalls) IO_printf (
	"DetermineLCExpansion: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, cBreakpoints, pInstances->ElementCount (), rtLINK_LC_RRDCount (pInstances)
    );

/***** Create the element expansion ... *****/
    rpResult = rtLINK_RefConstructor (pInstances->PPT ());

/***** Do the traversal ... *****/
    rtLINK_TraverseRRDCList (pInstances, handleNil, handleRepeat, handleRange);

/***** Close the element expansion and return... *****/
    rpResult->Close (new rtPTOKEN_Handle (ScratchPad (), elementCnt));

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
 *	ptoken			- the address of a cpd for the referential ptoken
 *				  of the element expansion.
 *	rpElementStore		- a reference that will be set to the element store.
 *	rpElementPointer	- the address of a link constructor pointer
 *				  which will be set to the result of the
 *				  extraction.  Indexes the content p-token
 *				  of the lstore.
 * 	rpExpansion		- the address of a link constructor pointer
 *				  which will be set to the position of 'linkc'
 *				  associated with each extracted element.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    rtPTOKEN_Handle *ptoken, Vdd::Store::Reference &rpElementStore, rtLINK_CType *&rpElementPointer, rtLINK_CType *&rpExpansion
) {
/*****  Align the arguments  *****/
    align ();

/*****  Create the element expansion domain p-token, ...  *****/
    rtPTOKEN_Handle::Reference pNewPPT (new rtPTOKEN_Handle (ScratchPad (), 0));

/***** Create the element expansion ... *****/
    rpExpansion = rtLINK_RefConstructor (ptoken);
    rpExpansion->Close (pNewPPT);

/*****  Create the element selection ... *****/
    rtPTOKEN_Handle::Reference pContentPToken;
    getContentPToken (pContentPToken);
    rpElementPointer = rtLINK_RefConstructor (pContentPToken);

    rpElementPointer->Close (pNewPPT);

/*****  Get the content store ...  *****/
    getContent (rpElementStore);

    return true;
}


/*---------------------------------------------------------------------------
 ***** L-Store Link Constructor Extraction Routine.
 *
 *  Arguments:
 *	linkc			- the address of a link constructor specifying
 *				  which lists to extract.
 *	rpElementStore		- a reference set to the store containing the
 *				  elements.
 *	rpElementPointer	- the address of a link constructor pointer
 *				  which will be set to the result of the
 *				  extraction.  Indexes the content p-token
 *				  of the lstore.
 * 	rpExpansion		- the address of a link constructor pointer
 *				  which will be set to the position of a 'linkc'
 *				  associated with each extracted element.
 *	rpDistribution	- a pointer to a standard CPD for a reference
 *				  uvector which will be set by the extraction
 *				  process if 'linkc' contains any RRDC's with
 *				  repeats.  Used to reorder the element selection
 *				  link constructor.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	DOM (lstore-Content)	= COD (rpElementPointer)
 *	DOM (rpElementPointer)	= DOM (rpDistribution)
 *	COD (rpDistribution)= DOM (rpExpansion)
 *	COD (rpExpansion)	= DOM (linkc)
 *
 *	or, in the APIWATW department:
 *
 *	LS-Lists	TOTSC	    Expansion	    Selection	    LS-Content
 *
 *	    *<------------*<------------*<--------------*-------------->*
 *	          linkc       expansion     reordering       selector
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    rtLINK_CType	*pInstances,
    Vdd::Store::Reference&rpElementStore,
    rtLINK_CType	*&rpElementPointer,
    rtLINK_CType	*&rpExpansion,
    M_CPD		*&rpDistribution
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
    srcValue= pBreakpoints[(subscriptValue)];\
    srcCnt  = pBreakpoints[(subscriptValue) + 1] - srcValue;\
}

#define outputElementSelector(value, cnt, type) {\
    if (type == rtLINK_RRDType_Range)\
	rtLINK_AppendRange (rpElementPointer, value, cnt);\
    else\
	rtLINK_AppendRepeat (rpElementPointer, value, cnt);\
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
    origElementCnt = elementCnt;\
    locateSource (subscriptValue);\
    tempValue = srcValue;\
    for (i=0; i<srcCnt; i++)\
	outputElementSelector (tempValue++, repeatCnt, repeat);\
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
    align ();
    pInstances->AlignForExtract (this, rowPTokenPOP ());

    unsigned int cBreakpoints = breakpointCount ();
    unsigned int*pBreakpoints = breakpointArray ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_LCExtract: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, cBreakpoints, pInstances->ElementCount (), rtLINK_LC_RRDCount (pInstances)
    );

/***** Determine if the subscript link constructor has any repeats ... *****/
    bool hasRepeats = rtLINK_LC_HasRepeats (pInstances);

/***** Create the element expansion ... *****/
    getExpansion (rpExpansion, pInstances);

/*****  Create the element selection ... *****/
    rtPTOKEN_Handle::Reference pContentPToken;
    getContentPToken (pContentPToken);
    rpElementPointer = rtLINK_RefConstructor (pContentPToken);

/***** Create the temp space for the distribution if needed ... *****/
    rtPTOKEN_Handle::Reference rpElementPointerPPT;
    if (hasRepeats) {
	rpElementPointerPPT.setTo (
	    new rtPTOKEN_Handle (ScratchPad (), rpExpansion->ElementCount ())
	);
	rpDistribution = rtREFUV_New (rpElementPointerPPT, rpExpansion->PPT ());
	reOrderUVectorPtr = rtREFUV_CPD_Array (rpDistribution);
    }
    else {
	rpElementPointerPPT.setTo (rpExpansion->PPT ());
	rpDistribution = NilOf (M_CPD *);
    }

/***** Do the traversal ... *****/
    rtLINK_TraverseRRDCList (pInstances, handleNil, handleRepeat, handleRange);

/***** Close the element selection ... *****/
    rpElementPointer->Close (rpElementPointerPPT);

/*****  Get the content store ...  *****/
    getContent (rpElementStore);

/*****  And return:  *****/
    return true;

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
 *	rInstances 		- the address of a reference specifying
 *				  which list to extract.
 *	pElementSelection	- the address of a link constructor which will
 *				  be set to contain the result of the
 *				  extraction: subscripts into the content of
 *				  the lstore.
 * 	rpExpansion	- the address of a link constructor which will
 *				  be set to contain a 0 for each extracted
 *				  element.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    DSC_Scalar		 &rInstances,
    Vdd::Store::Reference&rpElementStore,
    rtLINK_CType	*&pElementSelection,
    rtLINK_CType	*&rpExpansion
) {
    RefExtractCount++;

/*****
 *  Align the arguments and validate the reference as an extraction
 *****/
    align ();
    rtREFUV_AlignAndValidateRef (&rInstances, this, rowPTokenPOP ());

    unsigned int cBreakpoints = breakpointCount ();
    unsigned int*pBreakpoints = breakpointArray ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_RefExtract: cpd=%08X, lsBpaSz=%u\n", this, cBreakpoints
    );

/*****  Create the element selection ... *****/
    rtPTOKEN_Handle::Reference pContentPToken;
    getContentPToken (pContentPToken);
    pElementSelection = rtLINK_RefConstructor (pContentPToken);

/***** Create the element expansion ... *****/
    rpExpansion = rtLINK_RefConstructor (M_AttachedNetwork->TheScalarPTokenHandle ());

/***** Do the extraction ... *****/
    unsigned int cElements;
    unsigned int xList = DSC_Scalar_Int (rInstances);
    if (xList >= cBreakpoints)
	cElements = 0;
    else {
	unsigned int xElements = pBreakpoints[(xList)];
	cElements = pBreakpoints[(xList) + 1] - xElements;

	rtLINK_AppendRange (pElementSelection, xElements, cElements);
	rtLINK_AppendRepeat (rpExpansion, 0, cElements);
    }

/***** Close the links... *****/
    rtPTOKEN_Handle::Reference pNewPPT (new rtPTOKEN_Handle (ScratchPad (), cElements));
    pElementSelection->Close (pNewPPT);
    rpExpansion->Close (pNewPPT);

/*****  Get the content store ...  *****/
    getContent (rpElementStore);

/*****  And return:  *****/
    return true;
}


/*---------------------------------------------------------------------------
 ***** L-Store Extraction Routine.
 *
 *  Arguments:
 *	rInstances		- a pointer to the lists of interest.
 *	rpElementStore		- the return address for a CPD identifying the
 *				  element cluster.
 *	pElementSelection	- the return address for a link constructor
 *				  identifying the list elements of interest.
 * 	rpExpansion	- the address of a link constructor pointer
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
bool rtLSTORE_Handle::getListElements (
    DSC_Pointer		 &rInstances,
    Vdd::Store::Reference&rpElementStore,
    rtLINK_CType	*&rpElementPointer,
    rtLINK_CType	*&rpExpansion,
    M_CPD		*&pElementReordering
) {
/*****  Get the lstore out of 'rInstances' ... *****/
    CheckBreakpointConsistency ();

/*****  Decode the descriptor pointer ... *****/
    if (rInstances.holdsALink ()) {
	getListElements (
	    DSC_Pointer_Link (rInstances), rpElementStore, rpElementPointer, rpExpansion, pElementReordering
	);
    }
    else if (rInstances.holdsAScalarReference ()) {
	pElementReordering = NilOf (M_CPD *);
	getListElements (
	    DSC_Pointer_Scalar (rInstances), rpElementStore, rpElementPointer, rpExpansion
	);
    }
    else if (rInstances.holdsValuesInAnyForm ()) {
	pElementReordering = NilOf (M_CPD*);
	rtPTOKEN_Handle::Reference pPPT (rInstances.PPT ());
	getListElements (pPPT, rpElementStore, rpElementPointer, rpExpansion);
    }
    else ERR_SignalFault (
	EC__InternalInconsistency, "rtLSTORE_Extract: Invalid Descriptor Type"
    );

/*****  Finally, return a CPD for the element cluster, ... *****/

    return true;
}


/*---------------------------------------------------------------------------
 ***** L-Store Link Constructor Subscript Routine.
 *
 *  Arguments:
 *	rResult		- the location at which the result will be stored.
 *	pInstances	- the address of a link constructor specifying the
 *			  the lists to access.
 *	pSubscript	- an integer uvector specifying the list elements to
 *			  access.  Must be positionally related to 'pInstances'.
 *	iModifier	- an integer to be added to each value in 'pSubscript'.
 *			  Usually either 0 or -1, this argument is used to
 *			  choose between 0 and 1 origin subscripting.
 *	ppGuard		- a pointer to a link constructor which this routine
 *			  will create if some elements were not found.
 *
 *  Returns:
 *	true
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int iModifier, rtLINK_CType **ppGuard
) {
#define locateList(list) {\
    listOffset = pBreakpoints[list];\
    listCnt = pBreakpoints[(list) + 1] - listOffset;\
}

#define outputPosition(list) {\
    int position = iModifier + *positionPtr++;\
    if (list >= (int)cBreakpoints || position < 0 || (unsigned int)position >= listCnt) {\
	/*****  Not Found  *****//* Do Nothing */\
    }\
    else {\
	/*****  Found  *****/\
	rtLINK_AppendRange (pLocatedSubset, elementCount, 1);\
	*resultPtr++ = listOffset + position;\
    }\
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
    align ();
    pInstances->AlignForExtract (this, rowPTokenPOP ());

/*****  Align and validate the key integer uvector ...  *****/
    RTYPE_MustBeA("rtLSTORE LCSubscript", pSubscript->RType (), RTYPE_C_IntUV);
    pSubscript->align ();

/*****  Validate that 'pSubscript' and 'pInstances' are positionally related ...  *****/
    if (pSubscript->ReferenceDoesntName (UV_CPx_PToken, pInstances->PPT ())) ERR_SignalFault (
	EC__InternalInconsistency, "rtLSTORE LCSubscript: Positional Inconsistency"
    );

/*****  If tracing ...  *****/
    if (TracingCalls) IO_printf (
	"rtLSTORE_LCSubscript: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, breakpointCount (),
	pInstances->ElementCount (),
	rtLINK_LC_RRDCount (pInstances)
    );

/*****  Create the space for the result positions ... *****/
    unsigned int elementCount = pInstances->BaseElementCount ();
    rtREFUV_ElementType *result = (rtREFUV_ElementType *) UTIL_Malloc (
	sizeof (rtREFUV_ElementType) * elementCount
    );
    rtREFUV_ElementType *resultPtr = result;

/*****  Create 'pLocatedSubset' ...  *****/
    rtLINK_CType *pLocatedSubset = *ppGuard = rtLINK_RefConstructor (pInstances->PPT ());

/***** Do the traversal ... *****/
    rtINTUV_ElementType *positionPtr = rtINTUV_CPD_Array (pSubscript);
    elementCount = 0;

    unsigned int listOffset, listCnt;
    unsigned int cBreakpoints = breakpointCount ();
    unsigned int*pBreakpoints = breakpointArray ();
    rtLINK_TraverseRRDCList (pInstances, handleNil, handleRepeat, handleRange);

/*****  Determine how many positions were found ...  *****/
    unsigned int numFound = pLocatedSubset->ElementCount ();
    if (numFound == 0) {
	/***** NONE FOUND *****/
	rResult.constructNA (pInstances->PPT (), KOT ());
    }
    else {
	/*****  Create the pLocatedSubset if neccessary (if not all found) ...  *****/
	bool allFound = elementCount == numFound;

	rtPTOKEN_Handle::Reference foundPosPToken;
	if (allFound) {
	    /*****  All Found  *****/
	    pLocatedSubset->release ();
	    *ppGuard = NilOf (rtLINK_CType*);
	}
	else {
	    /*****  Only Some Found  *****/
	    foundPosPToken.setTo (new rtPTOKEN_Handle (pSubscript->ScratchPad (), numFound));
	    pLocatedSubset->Close (foundPosPToken);
	}

	/*****  Create the result reference uvector ...  *****/
	M_CPD *resultUV; {
	    rtPTOKEN_Handle::Reference pContentPToken;
	    getContentPToken (pContentPToken);

	    resultUV = rtREFUV_New (allFound ? pInstances->PPT () : foundPosPToken, pContentPToken);
	}

	memcpy (rtREFUV_CPD_Array (resultUV), result, numFound * sizeof (rtREFUV_ElementType));

	/*****  Create the result descriptor ...  *****/
	Vdd::Store::Reference pContent;
	getContent (pContent);
	rResult.constructMonotype (pContent, resultUV);
	resultUV->release ();
    }

    UTIL_Free (result);

    return true;

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
 *	rResult		- the location at which the result will be stored.
 *	rInstance 	- a reference specifying the list to access.
 *	xSubscript	- an integer specifying which list element to access.
 *	iModifier	- an integer to be added to 'xElement'.
 *			  Usually either 0 or -1, this argument is used to
 *			  choose between 0 and 1 origin subscripting.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
) {
/*---------------------------------------------------------------------------
 *  Code Body: 'RefSubscript'
 *---------------------------------------------------------------------------
 */
    RefSubscriptCount++;

/*****
 *  Align the arguments and validate the reference as an extraction
 *****/
    align ();
    rtREFUV_AlignAndValidateRef (&rInstance, this, rowPTokenPOP ());

    unsigned int cBreakpoints = breakpointCount ();
    unsigned int*pBreakpoints = breakpointArray ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_RefSubscript: cpd=%08X, lsBpaSz=%u\n", this, cBreakpoints
    );

/*****  ... and determine the result: *****/
    unsigned int xList = DSC_Scalar_Int (rInstance);
    xSubscript += iModifier;
    if (xList > cBreakpoints || xSubscript < 0 || (unsigned int)xSubscript >= pBreakpoints[xList + 1] - pBreakpoints[xList])
	rResult.constructNA (KOT ());
    else {
	Vdd::Store::Reference pContent;
	getContent (pContent);

	rtPTOKEN_Handle::Reference pContentPToken;
	getContentPToken (pContentPToken);

	rResult.constructReferenceScalar (
	    pContent, pContentPToken, xSubscript + pBreakpoints[xList]
	);
    }

    return true;
}


/*---------------------------------------------------------------------------
 ***** L-Store Subscript Routine.
 *
 *  Arguments:
 *	rResult		- the location at which the result will be stored.
 *	rInstances	- a pointer identifying the instances to access.
 *	rSubscript	- a pointer to a descriptor whose Pointer contains
 *			  either an integer scalar pointer or an integer
 *			  value pointer.  This set of integers specifies
 *			  the list positions to subscript in the
 *			  selected lists.  The Pointer in 'rSubscript' must
 *			  be positionally related to the Pointer in
 *			  'rInstances'.
 *	iModifier	- an integer to be added to each value in 'rSubscript'.
 *			  This usually either 0 or -1.  It is used to
 *			  simulate 1 based subscripts.
 *	ppGuard		- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found or not found
 *			  (if some were found and some not found).  It will
 *			  contain the positions in 'rSubscript' for which a value
 *			  was found.
 *
 *  Returns:
 *	true
 *
 *****/
bool rtLSTORE_Handle::getListElements (
    DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
) {
    CheckBreakpointConsistency ();

/*****  Decode the descriptor pointer ... *****/
    if (rInstances.holdsALink ()) {
	if (!rSubscript.holdsNonScalarValues ())
	    ERR_SignalFault (EC__InternalInconsistency, "rtLSTORE_Subscript: the key descriptor must be a value dsc");
	getListElements (
	    rResult, DSC_Pointer_Link (rInstances), DSC_Descriptor_Value (rSubscript), iModifier, ppGuard
	);
    }
    else if (rInstances.holdsAScalarReference ()) {
	if (rSubscript.isntScalar () || DSC_Descriptor_Scalar (rSubscript).RType () != RTYPE_C_IntUV)
	    ERR_SignalFault (EC__InternalInconsistency, "rtLSTORE_Subscript: Key Descriptor Must Be An Integer Scalar");
	getListElements (
	    rResult, DSC_Pointer_Scalar (rInstances), DSC_Descriptor_Scalar_Int (rSubscript), iModifier
	);
	*ppGuard = NilOf (rtLINK_CType*);
    }
    else if (rInstances.holdsValuesInAnyForm ()) {
	rtPTOKEN_Handle::Reference pPPT (rInstances.PPT ());
	rResult.constructNA (pPPT, KOT ());
	*ppGuard = NilOf (rtLINK_CType*);
    }
    else ERR_SignalFault (
	EC__InternalInconsistency,
	"rtLSTORE_Subscript: Invalid Descriptor Type"
    );

    return true;
}


/***************************************************
 *****  Element Append And Insertion Routines  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to append a position at the end of each list selected by
 *****  pSubscript
 *
 *  Arguments:
 *	pInstances		- a link constructor selecting the lists to be
 *				  modified.
 *	rpResult		- a link constructor which will be made to
 *				  reference the actual elements that have been
 *				  appended.
 *
 *  Returns:
 *	Nothing. Executed for side effects.
 *
 *****/
void rtLSTORE_Handle::appendToList (rtLINK_CType *&rpResult, rtLINK_CType *pInstances) {

/*---------------------------------------------------------------------------
 * AppendToLCSelectedList: Local Definitions
 *---------------------------------------------------------------------------
 */

#define adjustEntries(through, by) for (\
    unsigned int *p = pBreakpoints + (through);\
    pBreakpoint < p;\
    *pBreakpoint++ += (by)\
)

#define nilReferenceHandler(c, n, r)\
    adjustEntries (r, totalElementsInserted)

#define repeatedReferenceHandler(c, n, r) {\
    adjustEntries (r, totalElementsInserted);\
    unsigned int oldListSize = pBreakpoint[1] - pBreakpoint[0];\
    unsigned int oldListEnd = oldListSize + (\
	*pBreakpoint++ += totalElementsInserted\
    );\
    rtLINK_AppendRange (rpResult, oldListEnd, n);\
    pContentPTokenC->AppendAdjustment (oldListEnd, n);\
    totalElementsInserted += n;\
}

#define rangeReferenceHandler(c, n, ir) {\
    unsigned int ur = ir;\
    adjustEntries (ur, totalElementsInserted);\
    while (n-- > 0 && ur++ < cBreakpoints) {\
	unsigned int oldListSize = pBreakpoint[1] - pBreakpoint[0];\
	unsigned int oldListEnd  = oldListSize + (\
	    *pBreakpoint++ += totalElementsInserted++\
	);\
	rtLINK_AppendRange (rpResult, oldListEnd, 1);\
	pContentPTokenC->AppendAdjustment (oldListEnd, 1);\
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
    align ();
    pInstances->AlignForExtract (this, rowPTokenPOP ());

    unsigned int cBreakpoints = breakpointCount ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_AppendToLCSelList: cpd=%08X, lsBpaSz=%u, lcSz=%u, lcRRDCnt=%u\n",
	this, cBreakpoints, pInstances->ElementCount (), rtLINK_LC_RRDCount (pInstances)
    );

/*****  ...create the content assignment link constructor, ...  *****/
    rpResult = rtLINK_PosConstructor (pInstances->PPT ());

/*****  ...create a constructor for the new content P-Token, ...  *****/
    rtPTOKEN_Handle::Reference pContentPToken;
    getContentPToken (pContentPToken);
    rtPTOKEN_CType *pContentPTokenC = pContentPToken->makeAdjustments ();

/*****  ...enable modification of the L-Store, ...  *****/
    EnableModifications ();
    CheckConsistency ();
    setIsInconsistent ();

/*****  ...initialize the L-Store traversal variables, ...  *****/
    unsigned int *pBreakpoints = breakpointArray ();
    unsigned int *pBreakpoint   = pBreakpoints;
    unsigned int totalElementsInserted = 0;

/*****  ...traverse the link constructor, ...  *****/
    rtLINK_TraverseRRDCList (
	pInstances, nilReferenceHandler, repeatedReferenceHandler, rangeReferenceHandler
    );

/*****  ...adjust remaining breakpoints for the insertions made, ...  *****/
    adjustEntries (cBreakpoints + 1, totalElementsInserted);

/*****  ...close and install the new content P-Token, ...  *****/
    pContentPToken.setTo (pContentPTokenC->ToPToken ());
    rpResult->Close (pContentPToken);

    StoreReference (contentPTokenPOP (), pContentPToken);

    clearIsInconsistent ();


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
 *	rResult			- a reference which will be made to
 *				  the particular element that has been
 *				  appended.
 *	rInstance		- a reference selecting the lists to be
 *				  modified.
 *
 *  Returns:
 *	Nothing. Executed for side effects.
 *
 *****/
void rtLSTORE_Handle::appendToList (DSC_Scalar &rResult, DSC_Scalar &rInstance) {

/*---------------------------------------------------------------------------
 * AppendToRefSelectedList: Local Definitions
 *---------------------------------------------------------------------------
 */
    AppendToRefListCount++;

    unsigned int cBreakpoints = breakpointCount ();
    if (TracingCalls) IO_printf (
	"rtLSTORE_AppendToRefSelList: cpd=%08X, lsBpaSz=%u\n", this, cBreakpoints
    );

/*****  Align the arguments and validate the reference as an extraction *****/
    align ();
    rtREFUV_AlignAndValidateRef (&rInstance, this, rowPTokenPOP ());

/*****  Access the reference value and breakpoint count, ...  *****/
    unsigned int xList = DSC_Scalar_Int (rInstance);

/*****  If selector is reference Nil, do nothing but set rResult to
 *****  reference Nil for the content vector.
 *****/
    rtPTOKEN_Handle::Reference pContentPToken;
    getContentPToken (pContentPToken);

    if (xList >= cBreakpoints) {
	rResult.constructReference (pContentPToken, breakpointArray ()[cBreakpoints]);
	return;
    }

/*****  Otherwise, enable modification of the L-Store, ...  *****/
    EnableModifications ();
    CheckConsistency ();
    setIsInconsistent ();

/*****  ...obtain the element insertion position, ...  *****/
    unsigned int *pBreakpoints = breakpointArray ();
    unsigned int insertionPosition = pBreakpoints[++xList];

/*****  ...create and install a new content P-Token, ...  *****/
    rtPTOKEN_CType *pContentPTokenC = pContentPToken->makeAdjustments ();
    pContentPTokenC->AppendAdjustment (insertionPosition, 1);
    pContentPToken.setTo (pContentPTokenC->ToPToken ());

    StoreReference (contentPTokenPOP (), pContentPToken);

/*****  ...set rResult to point to the appended element, ...  *****/
    rResult.constructReference (pContentPToken, insertionPosition);

/*****  ...and adjust the remaining breakpoints for the insertion made.  *****/
    unsigned int const * const pBreakpointLimit = pBreakpoints + cBreakpoints;
    unsigned int *pBreakpoint = pBreakpoints + xList;
    while (pBreakpoint <= pBreakpointLimit)
	 ++*pBreakpoint++;

    clearIsInconsistent ();
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
    rtLSTORE_LStore *lstore = typecastContent ();

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
PrivateFnDef void MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD *pSpace, M_CPreamble const *pContainer) {
    pGCV->Mark (pSpace, (M_POP const*)(pContainer + 1), 3);
}


/****************************************
 *****  Data Base Update Utilities  *****
 ****************************************/

PublicFnDef void rtLSTORE_WriteDBUpdateInfo (M_CPD *lstoreCPD) {
/***** Do NOT Align LStore ... *****/
    ValidateBreakpointConsistency (lstoreCPD);
/***** Output the lstore DB File Record ... *****/
    if (rtLSTORE_CPD_StringStore (lstoreCPD)) {
	int lstoreIndex = lstoreCPD->containerIndex ();

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
    lstore->align ();

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
    return RTYPE_QRegister (new rtLSTORE_Handle (RTYPE_QRegisterPToken (parameterArray[0])));
}

IOBJ_DefineNewaryMethod (NewUsingContentPrototypeDM) {
    Vdd::Store::Reference pStore (RTYPE_QRegisterHandle (parameterArray[1])->getStore ());
    return RTYPE_QRegister (new rtLSTORE_Handle (RTYPE_QRegisterPToken (parameterArray[0]), pStore));
}

IOBJ_DefineNewaryMethod (NewUsingLinkDM) {
    rtLINK_CType *pContentMap = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));
    rtVECTOR_Handle::Reference pContentCluster (new rtVECTOR_Handle (pContentMap->PPT ()));
    rtLSTORE_Handle::Reference pNewCluster (
	new rtLSTORE_Handle (pContentMap, pContentCluster)
    );
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
    RTYPE_QRegisterCPD (self)->align ();
    return self;
}

IOBJ_DefineMethod (AlignListAdjustmentDM) {
    rtLSTORE_Handle *pStore = static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self));

    rtREFUV_Type_Reference listRef;
    listRef.constructReference (pStore->getPToken (), IOBJ_IObjectValueAsInt (parameterArray[0]));
    int adjustment = IOBJ_IObjectValueAsInt (parameterArray[1]);

    pStore->alignUsingSelectedLists (listRef, adjustment);

    listRef.destroy ();
    return self;
}

IOBJ_DefineMethod (AlignListsAdjustmentsDM) {
    rtLINK_CType *listLC = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));
    static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self))->alignUsingSelectedLists (
	listLC, RTYPE_QRegisterCPD (parameterArray[1])
    );
    listLC->release ();

    return self;
}

IOBJ_DefineMethod (AlignListsDM) {
    rtLINK_CType *listLC = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));
    static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self))->alignUsingSelectedLists (
	listLC, NilOf (M_CPD*)
    );
    listLC->release ();

    return self;
}

IOBJ_DefineMethod (LCExtractDM) {
    rtLSTORE_Handle *lstore = static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self));
    rtLINK_CType *linkc = rtLINK_ToConstructor (RTYPE_QRegisterCPD (parameterArray[0]));

    Vdd::Store::Reference pElementStore;
    rtLINK_CType *pElementPointer, *pExpansion; M_CPD *pElementReordering;
    lstore->getListElements (linkc, pElementStore, pElementPointer, pExpansion, pElementReordering);

    IO_printf ("elementSelector:\n");
    pElementPointer->DebugPrint ();

    IO_printf ("pExpansion:\n");
    pExpansion->DebugPrint ();

    pElementPointer->release ();
    pExpansion->release ();
    linkc->release ();

    if (IsntNil (pElementReordering))
	return RTYPE_QRegister (pElementReordering);
    else
	return (self);
}

IOBJ_DefineMethod (RefExtractDM) {
    rtLSTORE_Handle *lstore = static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self));

    rtREFUV_Type_Reference reference;
    reference.constructReference (lstore->getPToken (), IOBJ_IObjectValueAsInt (parameterArray[0]));

    Vdd::Store::Reference pElementStore;
    rtLINK_CType *pExpansion, *pElementPointer;
    lstore->getListElements (reference, pElementStore, pElementPointer, pExpansion);

    IO_printf ("elementSelector:\n");
    pElementPointer->DebugPrint ();

    IO_printf ("pExpansion:\n");
    pExpansion->DebugPrint ();

    pElementPointer->release ();
    pExpansion->release ();
    reference.destroy ();

    return (self);
}


IOBJ_DefineMethod (AppendToUsingLCDM) {
    rtLINK_CType *selectionLinkC = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray[0])
    );

    rtLINK_CType *elementLinkC;
    static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self))->appendToList (
	elementLinkC, selectionLinkC
    );

    selectionLinkC->release ();
    return RTYPE_QRegister (elementLinkC);
}

IOBJ_DefineMethod (AppendToUsingRefDM) {
    rtLSTORE_Handle *pStore = static_cast<rtLSTORE_Handle*>(RTYPE_QRegisterHandle (self));

    rtREFUV_Type_Reference pInstance;
    pInstance.constructReference (pStore->getPToken (), IOBJ_IObjectValueAsInt (parameterArray[0]));

    rtREFUV_Type_Reference iResult;
    pStore->appendToList (iResult, pInstance);

    int position = rtREFUV_Ref_Element (&iResult);

    pInstance.destroy ();
    iResult.destroy ();

    return IOBJ_IntIObject (position);
}


IOBJ_DefineMethod (ElementCountUsingRefDM) {
    Vdd::Store::Reference pStore (RTYPE_QRegisterHandle (self)->getStore ());

    DSC_Scalar iSubscript;
    iSubscript.constructReference (pStore->getPToken (), IOBJ_IObjectValueAsInt (parameterArray[0]));

    unsigned int iResult = 0;
    pStore->getCardinality (iResult, iSubscript);

    iSubscript.destroy ();

    return IOBJ_IntIObject (iResult);
}

IOBJ_DefineMethod (ElementCountUsingLCDM) {
    rtLINK_CType *pSubscript = rtLINK_ToConstructor (
	RTYPE_QRegisterCPD (parameterArray [0])
    );

    Vdd::Store::Reference pStore (RTYPE_QRegisterHandle (self)->getStore ());

    M_CPD *pResult;
    pStore->getCardinality (pResult, pSubscript);

    pSubscript->release ();

    return RTYPE_QRegister (pResult);
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

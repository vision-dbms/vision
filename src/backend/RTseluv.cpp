/*****  Selector U-Vector Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtSELUV

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTstring.h"
#include "RTvector.h"

#include "RTcharuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"

#include "VfDeleteGenerator.h"
#include "VfLocateGenerator.h"
#include "VfLocateOrAddGenerator.h"

#include "VCollectionOf.h"
#include "VCollectionOfStrings.h"
#include "VOrdered.h"
#include "VSet.h"

/*****  Self  *****/
#include "RTseluv.h"


/***************************
 ***************************
 *****  Globals, Etc.  *****
 ***************************
 ***************************/

/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool
    TracingLocateOrAdd	    	= false;

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*>;
template class VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int>;

template class VOrdered<VCollectionOfStrings, const char*>;
template class VOrdered<VCollectionOfUnsigned32, unsigned int>;


/***********************
 ***********************
 *****             *****
 *****  Set Class  *****
 *****             *****
 ***********************
 ***********************/

/**************************
 *****  Construction  *****
 **************************/

rtSELUV_Set::rtSELUV_Set (M_CPD* pSetRef, int xSetRef)
: m_pSetCPD		(pSetRef, xSetRef, RTYPE_C_SelUV)
, m_pStringStorage	(0)
{
    align ();
}

rtSELUV_Set::rtSELUV_Set (M_CPD* pSelectorSetCPD)
: m_pSetCPD		(pSelectorSetCPD)
, m_pStringStorage	(0)
{
    align ();
}

/*************************
 *****  Destruction  *****
 *************************/

rtSELUV_Set::~rtSELUV_Set ()
{
}

/*************************************
 *****  Deferred Initialization  *****
 *************************************/

void rtSELUV_Set::initializeDPT ()
{
    m_pDPT.claim (UV_CPD_PosPTokenCPD (m_pSetCPD));
}


/*************************************
 *****  String Space Management  *****
 *************************************/

void rtSELUV_Set::accessStringSpace ()
{
    m_pStringSpaceCPD.claim (rtSELUV_CPD_StringSpaceCPD (m_pSetCPD));
    m_pStringStorage = rtCHARUV_CPD_Array (m_pStringSpaceCPD);
}

void rtSELUV_Set::resyncStringSpace ()
{
    m_pStringStorage = rtCHARUV_CPD_Array (m_pStringSpaceCPD);
}

unsigned int rtSELUV_Set::growStringSpace (char const* pString)
{
    unsigned int xStringOrigin = growStringSpace (strlen (pString) + 1);
    strcpy ((char*)m_pStringStorage + xStringOrigin, pString);
    return xStringOrigin;
}

PrivateVarDef int GrowStringSpaceErrorHandler (ERR_ErrorDescription *errorDescription) {
    ERR_ReportErrorOnStderr ("", errorDescription);

    return 0;
}

unsigned int rtSELUV_Set::growStringSpace (unsigned int sGrowth, unsigned int xOrigin)
{
    accessStringSpaceIfNecessary ();
    if (0 == xOrigin) xOrigin = M_CPD_Size (m_pStringSpaceCPD);
    if (xOrigin > rtSELUV_Element_MaxIndex)
	ERR_SignalFault (
	     EC__InternalInconsistency,
	     string ("Dictionary String Space[%d:%d] Size Exceeds Maximum (%d)",
				  m_pStringSpaceCPD->SpaceIndex (),
				  m_pStringSpaceCPD->ContainerIndex (),
				  rtSELUV_Element_MaxIndex)
	);
    else if (xOrigin > rtSELUV_Element_MaxIndex - 100000) {
	ERR_PushErrorHandler (EC__InternalInconsistency, GrowStringSpaceErrorHandler);
	ERR_SignalWarning (
	     EC__InternalInconsistency,
	     string ("Dictionary String Space[%d:%d] Size (%d) Nearing Maximum (%d)",
				  m_pStringSpaceCPD->SpaceIndex (),
				  m_pStringSpaceCPD->ContainerIndex (),
				  xOrigin,
				  rtSELUV_Element_MaxIndex)
	);
	ERR_PopErrorHandler (EC__InternalInconsistency);
    }
    m_pStringSpaceCPD->GrowContainer (sGrowth);

    unsigned int xGrowthOrigin = UV_CPD_ElementCount (m_pStringSpaceCPD);
    UV_CPD_ElementCount (m_pStringSpaceCPD) += sGrowth;
    resyncStringSpace ();

    return xGrowthOrigin;
}


/********************
 *****  Access  *****
 ********************/

void rtSELUV_Set::Locate (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Locate (pKeys, rpReordering, rResult);
}

void rtSELUV_Set::Locate (
    VCollectionOfUnsigned32* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    iSetCursor.Locate (pKeys, rpReordering, rResult);
}


/********************
 *****  Update  *****
 ********************/

void rtSELUV_Set::Insert (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Insert (pKeys, rpReordering, rResult);
}

void rtSELUV_Set::Insert (
    VCollectionOfUnsigned32* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    iSetCursor.Insert (pKeys, rpReordering, rResult);
}

void rtSELUV_Set::Delete (
    VCollectionOfStrings* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    iSetCursor.Delete (pKeys, rpReordering, rResult);
}

void rtSELUV_Set::Delete (
    VCollectionOfUnsigned32* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    iSetCursor.Delete (pKeys, rpReordering, rResult);
}


/*---------------------------------------------------------------------
 *  The scalar 'Insert' and 'Delete' routines return true if the set was
 *  updated and false if no changes were made.
 *---------------------------------------------------------------------
 */
bool rtSELUV_Set::Insert (char const* pKey, unsigned int& rxElement)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Insert (pKey, rxElement);
}

bool rtSELUV_Set::Insert (unsigned int xKey, unsigned int& rxElement)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    return iSetCursor.Insert (xKey, rxElement);
}

bool rtSELUV_Set::Delete (char const* pKey)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfStrings,char const*> iSetCursor (this);
    return iSetCursor.Delete (pKey);
}

bool rtSELUV_Set::Delete (unsigned int xKey)
{
    VAssociativeCursor<rtSELUV_Set,VCollectionOfUnsigned32,unsigned int> iSetCursor (this);
    return iSetCursor.Delete (xKey);
}


/******************************
 *****  Update Utilities  *****
 ******************************/

void rtSELUV_Set::install (
    rtLINK_CType* pElements, VOrdered<VCollectionOfStrings,char const*>& rAdditions
)
{
//  Determine the required string space growth, ...
    accessStringSpaceIfNecessary ();

    unsigned int sStringSpaceGrowth = 0;
    unsigned int xOrigin = UV_CPD_ElementCount (m_pStringSpaceCPD);
    unsigned int xLastOrigin;

    rAdditions.resetCursor ();
    while (rAdditions.elementIsValid ())
    {
	unsigned int sElement = strlen (rAdditions.element ()) + 1;
	xLastOrigin = xOrigin;
	xOrigin += sElement;
	sStringSpaceGrowth += sElement;
	rAdditions.goToNextElement ();
    }

//  ... grow the string space, ...
    unsigned int xNextString = growStringSpace (sStringSpaceGrowth, xLastOrigin);
    char *pNextString = (char*)m_pStringStorage + xNextString;

//  ... update the set, ...
#   define copyThisString() {\
	strcpy (pNextString, rAdditions.element ());\
	size_t sThisString = strlen (pNextString) + 1;\
	pNextString += sThisString;\
	xNextString += sThisString;\
    }
#   define handleNil(c, n, r)
#   define handleRepeat(c, n, r) {\
	install (r, SELECTOR_C_DefinedSelector, xNextString);\
	copyThisString ();\
	rAdditions.skip (n);\
    }
#   define handleRange(c, n, r) {\
	while (n-- > 0) {\
	    install (r++, SELECTOR_C_DefinedSelector, xNextString);\
	    copyThisString ();\
	    rAdditions.goToNextElement ();\
	}\
    }

    rAdditions.resetCursor ();
    rtLINK_TraverseRRDCList (
	pElements,
	handleNil,
	handleRepeat,
	handleRange
    );

#   undef handleRange
#   undef handleRepeat
#   undef handleNil
#   undef copyThisString

//  ...and return.
}


void rtSELUV_Set::install (
    rtLINK_CType* pElements, VOrdered<VCollectionOfUnsigned32,unsigned int>& rAdditions
)
{
//  Update the set, ...
#   define handleNil(c, n, r)
#   define handleRepeat(c, n, r) {\
	install (r, SELECTOR_C_KnownSelector, rAdditions.element ());\
	rAdditions.skip (n);\
    }
#   define handleRange(c, n, r) {\
	while (n-- > 0) {\
	    install (r++, SELECTOR_C_KnownSelector, rAdditions.element ());\
	    rAdditions.goToNextElement ();\
	}\
    }

    rAdditions.resetCursor ();
    rtLINK_TraverseRRDCList (
	pElements, handleNil, handleRepeat, handleRange
    );

#   undef handleNil
#   undef handleRepeat
#   undef handleRange

/*****  ...and return.  *****/
}


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

DEFINE_CONCRETE_RTT (rtSELUV_Handle);


/***************************
 ***************************
 *****  Instantiators  *****
 ***************************
 ***************************/

/*********************************
 *****  Basic Instantiator   *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Basic SelUV instantiation routine.
 *
 *  Arguments:
 *	pPPT			- the P-Token which defines the initial
 *				  positional state of this U-Vector.
 *	pRPT			- the P-Token which defines the initial
 *				  referential state of this U-Vector.
 *	initFn			- the address of an initialization function for
 *				  the new U-Vector.  This function will be
 *				  passed an initialized standard CPD for the
 *				  new U-Vector, the number of elements in the
 *				  U-Vector, and a <varargs.h> argument pointer
 *				  positioned at 'initFnArg1'.  If this function
 *				  is not specified (i.e., 'IsNil (initFn)'),
 *				  the U-Vector array will be initialized to
 *				  zero.
 *	initFnArg1, ...		- a collection of optional arguments to the
 *				  'initFn'.
 *
 *  Returns:
 *	A standard CPD for the SelUV created.
 *
 *****/
PrivateFnDef M_CPD *__cdecl rtSELUV_New (
    M_CPD *pPPT, M_CPD *pRPT, Ref_UV_Initializer initFn, ...
) {
/*****  Acquire the arguments passed to this function  *****/
    V_VARGLIST (initFnAP, initFn);

/*****  Create and initialize the SelUV  *****/
    return UV_New (
	RTYPE_C_SelUV, pPPT, pRPT, sizeof (rtSELUV_Type_Element), initFn, initFnAP
    );
}


/********************************
 ***** String Space Creator *****
 ********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine invoked by rtSELUV_New to create the selector
 *****  uvector's string space.
 *
 *  Arguments:
 *	cpd		- The cpd for the selector uvector.
 *	nelements	- unused.
 *	ap		- unused.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PrivateFnDef void StringSpaceInit (M_CPD *cpd, size_t Unused(nelements), va_list Unused(ap)) {
    M_CPD *ptokenCPD = rtPTOKEN_New (cpd->Space (), 0);
/*---------------------------------------------------------------------------
 * Passing the same P-Token as both the positional and referential P-Token is
 * a temporary patch which will need to be corrected.
 *---------------------------------------------------------------------------
 */
    M_CPD *stringSpace = rtCHARUV_New (ptokenCPD, ptokenCPD);
    cpd->StoreReference (rtSELUV_CPx_StringSpace, stringSpace);

    ptokenCPD->release ();
    stringSpace->release ();
}


/***********************************
 *****  Standard Instantiator  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Standard SelUV instantiation routine.
 *
 *  Arguments:
 *	posPToken		- the positional P-Token of the new uvector.
 *	refPToken		- the referential P-Token of the new uvector.
 *
 *  Returns:
 *	A standard CPD for the SelUV created.
 *
 *****/
PublicFnDef M_CPD *rtSELUV_New (M_CPD *pPPT, M_CPD *pRPT) {
    return rtSELUV_New (pPPT, pRPT, StringSpaceInit);
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

/*---------------------------------------------------------------------------
 *****  Routine to positionally align a SelUV.
 *
 *  Argument:
 *	cpd			- a CPD for the SelUV to be aligned.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef M_CPD *rtSELUV_Align (M_CPD *cpd) {
/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtSELUV_Align", M_CPD_RType (cpd), RTYPE_C_SelUV);

/*****  Align Positionally  *****/
    UV_Align (cpd, NilOf (M_CPD::UVShiftProcessor));

/*****  Return the Argument  *****/
    return cpd;
}


/****************************
 ****************************
 *****  Query Routines  *****
 ****************************
 ****************************/

/*************************************
 *****  Contents Query Routines  *****
 *************************************/

PrivateFnDef char const *TraverseUserNames (bool reset, va_list ap) {
    V::VArgList iArgList (ap);
    M_CPD* stringStore	= iArgList.arg<M_CPD*>();
    M_CPD* selUV	= iArgList.arg<M_CPD*>();
    int beginOff	= iArgList.arg<int>();
    int endOff		= iArgList.arg<int>();
    int*workOff		= iArgList.arg<int *>();

    if (reset) {
	*workOff = beginOff;
	return NilOf (char const*);
    }

    return *workOff < endOff
	? rtCHARUV_CPD_Array (stringStore)
	    + rtSELUV_Element_Index (rtSELUV_CPD_Array (selUV) + (*workOff)++)
	: NilOf (char const*);
}


/*---------------------------------------------------------------------------
 *****  Routine to produce a vector containing all of the names in the
 *****	selector uvector.
 *
 *  Arguments:
 *	pSelectorSetCPD	- a standard CPD for the selector u-vector to be
 *			  queried.
 *
 *  Returns:
 *	A standard CPD for a vector containing all of the names in
 *      'pSelectorSetCPD'.
 *
 *****/
PublicFnDef M_CPD *rtSELUV_Contents (M_ASD *pContainerSpace, M_CPD *pSelectorSetCPD) {
/*****  Make the new vector ... *****/
    M_CPD *vectorPToken = rtPTOKEN_New (
	pContainerSpace, rtPTOKEN_BaseElementCount (pSelectorSetCPD, UV_CPx_PToken)
    );
    M_CPD *vector = rtVECTOR_New (vectorPToken);

/*****  If the selector uvector is empty ... *****/
    unsigned int size = UV_CPD_ElementCount (pSelectorSetCPD);

    if (size == 0) {
	vectorPToken->release ();
	return vector;
    }

/*****   Determine where the user defined names start ... *****/
    unsigned int i;
    for (i=0; i < size; i++) {
	if (rtSELUV_Element_Type (rtSELUV_CPD_Array (pSelectorSetCPD) + i) ==
	    SELECTOR_C_DefinedSelector
	) break;
    }

    /*** There are some user defined names ***/
    unsigned int userNameStart = i < size ? i : size;
    unsigned int knownNameCount = userNameStart;
    unsigned int userNameCount = size - userNameStart;

/*****
 *       Install the selector names into the vector ...
 *****/

    if (knownNameCount > 0) {
/*****  Obtain the KOTE for the result, ...  *****/
	M_KOTE const &rSelectorKOTE = pSelectorSetCPD->TheSelectorClass ();

/*****  Make a KSI U-Vector for the KSI names... *****/
	M_CPD *ptoken = rtPTOKEN_New (pContainerSpace, knownNameCount);
	M_CPD *ksiUV = rtINTUV_New (ptoken, rSelectorKOTE.PTokenCPD ());
	ptoken->release ();

/*****  Fill the KSI uvector ... *****/
	for (i=0; i < knownNameCount; i++) {
	    rtINTUV_CPD_Array (ksiUV)[i] = rtSELUV_Element_Index (
		rtSELUV_CPD_Array (pSelectorSetCPD) + i
	    );
	}

/*****  Put ksiUV into the vector ... *****/
	/***  Make a descriptor with ksiUV ***/
	ksiUV->retain ();
	DSC_Descriptor ksiDsc;
	ksiDsc.constructValue (rSelectorKOTE.RetainedObjectCPD (), ksiUV);

	/***  Make a link constructor for 'rtVECTOR_Assign' ***/
	rtLINK_CType *linkc = rtLINK_RefConstructor (vectorPToken, -1);
	rtLINK_AppendRange (linkc, 0, knownNameCount);
	linkc->Close (ksiUV, UV_CPx_PToken);

	/***  Install the ksiUV into the vector ***/
	rtVECTOR_Assign (vector, linkc, &ksiDsc);
	ksiUV->release ();
	linkc->release ();
	ksiDsc.clear ();
    }

/*****
 *      Install the known names into the vector ...
 *****/
    if (userNameCount > 0) {
/*****  Make a string store with the remaining names ... *****/
	M_CPD *nameStorage = rtSELUV_CPD_StringSpaceCPD (pSelectorSetCPD);
	int temp;
	M_CPD *stringStore = rtLSTORE_NewStringStore (
	    pContainerSpace,
	    TraverseUserNames,
	    nameStorage,
	    pSelectorSetCPD,
	    knownNameCount,
	    size,
	    &temp
	);
	nameStorage->release ();

/***** ... put the known selectors into the vector ... *****/

	/***  Make a descriptor containing the known string store ***/
	M_CPD *ptoken = rtLSTORE_CPD_RowPTokenCPD (stringStore);
	M_DuplicateCPDPtr (stringStore);
	DSC_Descriptor stringDsc;
	stringDsc.constructIdentity (stringStore, ptoken);

	/***  Make a link constructor for 'rtVECTOR_Assign' ***/
	rtLINK_CType *linkc = rtLINK_RefConstructor (vectorPToken, -1);
	rtLINK_AppendRange (linkc, knownNameCount, userNameCount);
	linkc->Close (stringStore, rtLSTORE_CPx_RowPToken);

	/***  Install the stringStore into the vector ***/
	rtVECTOR_Assign (vector, linkc, &stringDsc);
	linkc->release ();
	stringStore->release ();
	stringDsc.clear ();
    }

    vectorPToken->release ();

    return vector;
}


/*******************************************************************
 *******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************
 *******************************************************************/

/********************
 *  Element Printer  *
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to print a representation of an 'RTseluv' element.
 *
 *  Arguments:
 *	cpd			- a standard CPD positioned at the element
 *				  to be displayed.
 *
 *  Returns:
 *	The number of characters displayed.
 *
 *****/
PrivateFnDef int PrintElement (M_CPD *cpd) {
    int		result,
		t = rtSELUV_Element_Type (rtSELUV_CPD_Element (cpd)),
		i = rtSELUV_Element_Index (rtSELUV_CPD_Element (cpd));

    if (t == SELECTOR_C_DefinedSelector) {
        M_CPD *stringSpace = rtSELUV_CPD_StringSpaceCPD (cpd);
	result = IO_printf (
	    "%s(SSI%d)", rtCHARUV_CPD_Array (stringSpace) + i, i
	);
	stringSpace->release ();
    }
    else if (t == SELECTOR_C_KnownSelector)
        result = IO_printf ("%s(KSI%d)", KS__ToString (i), i);
    else result = IO_printf ("%d:%d", t, i);

    return result;
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineUnaryMethod (TracesOnDM)
{
    TracingLocateOrAdd	    	= true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM)
{
    TracingLocateOrAdd	    	= false;

    return self;
}

IOBJ_DefineNilaryMethod (LocateOrAddTraceDM)
{
    return IOBJ_SwitchIObject (&TracingLocateOrAdd);
}

IOBJ_DefineNewaryMethod (NewDM)
{
    return RTYPE_QRegister (
	rtSELUV_New (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1])
	)
    );
}


/************************
 *  'Instance' Methods  *
 ************************/

UV_DefineEPrintDM (PrintElementDM, PrintElement)

UV_DefineAlignDM (AlignDM, rtSELUV_Align)


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler(rtSELUV_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("tracesOn"		, TracesOnDM)
	IOBJ_MDE ("tracesOff"		, TracesOffDM)
	IOBJ_MDE ("locateOrAddTrace"	, LocateOrAddTraceDM)
	IOBJ_MDE ("new:referencing:"	, NewDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	UV_StandardDMDEPackage
	IOBJ_MDE ("eprint"		, PrintElementDM)
	IOBJ_MDE ("align"		, AlignDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(UV_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = UV_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtSELUV_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = UV_ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = UV_MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	UV_Print (iArgList.arg<M_CPD*>(), false, PrintElement);
        break;
    case RTYPE_RPrintObject:
	UV_Print (iArgList.arg<M_CPD*>(), true, PrintElement);
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
    FAC_FDFCase_FacilityIdAsString (rtSELUV);
    FAC_FDFCase_FacilityDescription ("Selector U-Vector Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTseluv.c 1 replace /users/mjc/system
  860411 22:02:06 mjc Create 'selector' u-vectors

 ************************************************************************/
/************************************************************************
  RTseluv.c 2 replace /users/mjc/system
  860420 18:49:42 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  RTseluv.c 3 replace /users/hjb/system
  860423 00:37:29 hjb done updating calls

 ************************************************************************/
/************************************************************************
  RTseluv.c 4 replace /users/jck/system
  860521 12:37:14 jck First Cut Implementation with enough functionallity for Method Dictionary

 ************************************************************************/
/************************************************************************
  RTseluv.c 5 replace /users/jck/system
  860522 12:04:53 jck Added New Scalar Assignment routines

 ************************************************************************/
/************************************************************************
  RTseluv.c 6 replace /users/mjc/system
  860530 14:36:57 mjc Added a referential p-token to the creation protocol for all u-vectors

 ************************************************************************/
/************************************************************************
  RTseluv.c 7 replace /users/jck/system
  860605 16:14:55 jck Changed interfaces to rtSELUV_FindPositionName to take char ** rather than char*

 ************************************************************************/
/************************************************************************
  RTseluv.c 8 replace /users/mjc/system
  860617 15:39:01 mjc Declared save routine

 ************************************************************************/
/************************************************************************
  RTseluv.c 9 replace /users/mjc/maketest
  860708 12:45:39 mjc Deleted references to 'RTselector' and unneeded routines

 ************************************************************************/
/************************************************************************
  RTseluv.c 10 replace /users/jck/system
  860718 09:40:18 jck Fixed Bug in Assign to Name routine

 ************************************************************************/
/************************************************************************
  RTseluv.c 11 replace /users/jck/system
  860725 16:46:42 jck Decoupled the referential and positional ptokens

 ************************************************************************/
/************************************************************************
  RTseluv.c 12 replace /users/mjc/system
  870104 22:52:06 lcn Added parallel lookup and addition routines

 ************************************************************************/
/************************************************************************
  RTseluv.c 13 replace /users/jad/system
  870128 19:05:16 jad implemented delete and contents routines

 ************************************************************************/
/************************************************************************
  RTseluv.c 14 replace /users/jad/system
  870203 15:20:25 jad fixed bug in locateOrAdd and added some tracing

 ************************************************************************/
/************************************************************************
  RTseluv.c 15 replace /users/mjc/translation
  870524 09:35:06 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTseluv.c 16 replace /users/jck/system
  871007 13:34:44 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTseluv.c 17 replace /users/jad/system
  880418 13:28:29 jad Fixed unitialized variable for the VAX

 ************************************************************************/
/************************************************************************
  RTseluv.c 18 replace /users/jck/system
  880915 11:58:28 jck Initialized setDeletionCount in the '...DeleteNonSet' routines (Apollo/Vax bug)

 ************************************************************************/
/************************************************************************
  RTseluv.c 19 replace /users/jck/system
  890222 15:21:55 jck Implemented corruption detection mechanism

 ************************************************************************/

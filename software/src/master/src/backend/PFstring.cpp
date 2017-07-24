/*****  String Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfSTRING

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

#if defined(__VMS) || defined(__linux__) || defined(_WIN32) || defined(__APPLE__)
#define UsingVkRegularExpressions
#endif

#if defined(UsingVkRegularExpressions)
#include "VkRegExp.h"

#else
#include <libgen.h>
typedef char VkRegExp;

#endif

/*****  Facility  *****/
#include "m.h"
#include "selector.h"
#include "uvector.h"

#include "vdebug.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"

#include "RTptoken.h"
#include "RTlink.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTintuv.h"

#include "RTvector.h"

#include "RTblock.h"
#include "RTlstore.h"
#include "RTvstore.h"

#include "VFragment.h"

/*****  Self  *****/
#include "PFstring.h"


/*******************************
 *******************************
 *****  Primitive Indices  *****
 *******************************
 *******************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that assign indices to the primitives.
 *
 * The numeric indices associated in this section with each of the primitives
 * are the system wide 'indices' by which the primitives are known.  These
 * indices are stored in permanent object memory;  consequently, once assigned,
 * they must not be changed or deleted.  To avoid an fatal error at system
 * startup, the largest index assigned in this section must be less than
 * 'V_PF_MaxPrimitiveIndex' (see "vprimfns.h").
 * If necessary this maximum can be increased.
 *---------------------------------------------------------------------------
 */
/*======================================*
 *  This module currently has indices:  *
 *	350 - 370, 380 - 390            *
 *======================================*/


/*******************************
 *****  String Primitives  *****
 *******************************/

#define XStringSize				350
#define XEncodeStringAsDouble			351
#define XToLStoreString			        352
#define XStringFill				353
#define XStringTake				354
#define XStringDrop				355
#define XStringConcat				356
#define XToDouble				357
#define XStringReverse				358
#define XStringProcessor			359


#define XStringStrip                            386 // Flags == 0 -> stripLead:
						    // Flags == 1 -> stripTrail:
						    // Flags == 2 -> stripBound:
						    // Flags == 3 -> stripLeadAlwaysCopy:
						    // Flags == 4 -> stripTrailAlwaysCopy:
						    // Flags == 5 -> stripBoundAlwaysCopy:
#define XGetEnv					387

/******************************************
 *****  String Relational Primitives  *****
 ******************************************/

#define XStringLessThan				360
#define XStringLessThanOrEqual			361
#define XStringEqual				362
#define XStringNotEqual				363
#define XStringGreaterThanOrEqual		364
#define XStringGreaterThan			365

#define XStringMatch				366

#define XStringSpan				367
#define XStringCSpan				368

#define XStringBreak				369 // Flags == 1 -> Tokenize
#define XStringCBreak				370 // Flags == 1 -> Tokenize

#define XFindSubstring				380
#define XFindPatternOrigin			381
#define XFindPatternExtent			382

#define XStringBreakOnString			389 // Flags == 1 -> Tokenize
#define XStringBreakOnPattern			390 // Flags == 1 -> Tokenize


/***********************************************
 ***********************************************
 *****  Constants, Globals, and Utilities  *****
 ***********************************************
 ***********************************************/

/****************************
 *----  Duc Validation  ----*
 ****************************/

PrivateFnDef bool ParameterReferencesAStringStore (VPrimitiveTask* pTask) {
    DSC_Descriptor &rParameter = ADescriptor;

    Vdd::Store *pStore = rParameter.store ();
    RTYPE_Type xPointerRType = rParameter.pointerRType ();

    bool bTrue = false;
    switch (pStore->rtype ()) {
    case RTYPE_C_Block:
	bTrue = xPointerRType == RTYPE_C_IntUV;
	break;
    case RTYPE_C_ListStore:
	bTrue = (
	    xPointerRType == RTYPE_C_Link || xPointerRType == RTYPE_C_RefUV
	) && static_cast<rtLSTORE_Handle*>(pStore)->isAStringStore ();
	break;
    }
    return bTrue;
}


PrivateFnDef bool ParameterReferencesAStringStoreExtension (VPrimitiveTask* pTask) {
    DSC_Descriptor &rParameter = ADescriptor;

    Vdd::Store::Reference pCandidateStore (rParameter.store ());
    RTYPE_Type xCandidateStoreRType = pCandidateStore->rtype ();

    RTYPE_Type rtype = rParameter.pointerRType ();

    while (RTYPE_C_ValueStore == xCandidateStoreRType) {
	rtVSTORE_Handle::Reference pCandidateVStore (
	    static_cast<rtVSTORE_Handle*>(pCandidateStore.referent ())
	);
	Vdd::Store::Reference pSuperStore;
	if (pCandidateVStore->getInheritanceStore (pSuperStore)) {
	    xCandidateStoreRType = pSuperStore->rtype ();
	switch (xCandidateStoreRType) {
	case RTYPE_C_Undefined:
	case RTYPE_C_ValueStore:
	    break;

	default: {
		    VContainerHandle::Reference pSuperPointer;
		    pCandidateVStore->getInheritancePointer (pSuperPointer);
		    rtype = pSuperPointer->RType ();
	    }
	    break;
	}
	pCandidateStore.claim (pSuperStore);
	} else {
	    xCandidateStoreRType = RTYPE_C_Undefined;
	    pCandidateStore.clear ();
	}
    }

    return (
	xCandidateStoreRType == RTYPE_C_Block && rtype == RTYPE_C_IntUV
    ) || (
	xCandidateStoreRType == RTYPE_C_ListStore && (
	    rtype == RTYPE_C_Link || rtype == RTYPE_C_RefUV
	) && static_cast<rtLSTORE_Handle*>(pCandidateStore.referent ())->isAStringStore ()
	);
}


/*************************************
 *----  String Store Descriptor  ----*
 *************************************/

class StringStoreDescriptor : public VTransient {
//  Construction
public:
    StringStoreDescriptor (DSC_Descriptor &rMonotype);

//  Destruction
public:
    ~StringStoreDescriptor ();

//  Access
public:
    DSC_Descriptor &monotype () const {
	return m_rMonotype;
    }

    char const *string (unsigned int xString) const {
	return m_pLStoreStrings.isSet () ? m_pLStoreStrings[xString] : m_pBlockStrings[xString];
    }

//  State
protected:
    DSC_Descriptor&	m_rMonotype;
    rtLSTORE_Handle::Strings	m_pLStoreStrings;
    rtBLOCK_Handle::Strings	m_pBlockStrings;
};


/******************
 *  Construction  *
 ******************/

StringStoreDescriptor::StringStoreDescriptor (DSC_Descriptor &rMonotype) : m_rMonotype (rMonotype) {
    if (!m_pLStoreStrings.setTo (rMonotype.store ()))
	m_pBlockStrings.setTo (rMonotype.store ());
}


/*****************
 *  Destruction  *
 *****************/

StringStoreDescriptor::~StringStoreDescriptor () {
}


/**********************************
 *----  String Search Cursor  ----*
 **********************************/

/*---------------------------------------------------------------------*
 *  Notes:
 *
 *  This class employs lazy initialization for its member variables.
 *  To use this class correctly, 'setStringTo' must be called before
 *  attempting a search, 'reset' must be called before searching for
 *  the first occurrence of a string or pattern, 'succeed' or 'fail'
 *  must be called before examining any pattern location properties,
 *  and 'step' must be called after each 'succeed' to search for the
 *  next occurrence of a string or pattern.
 *
 *  
 *---------------------------------------------------------------------*/
class StringSearchCursor : public VTransient {
//  Construction
public:
    StringSearchCursor (char const *pStringOrigin = 0) : m_pStringOrigin (pStringOrigin) {
    }

//  Destruction
public:
    ~StringSearchCursor () {
    }

//  Access
public:
    char const *stringOrigin  () const {
	return m_pStringOrigin;
    }
    char const *searchOrigin  () const {
	return m_pSearchOrigin;
    }
    unsigned int searchExtent () const {
	return m_pPatternOrigin - m_pSearchOrigin;
    }

    bool patternFound () const {
	return *m_pPatternOrigin ? true : false;
    }
    unsigned int patternExtent () const {
	return m_pPatternExtent - m_pPatternOrigin;
    }
    char const *patternOrigin () const {
	return m_pPatternOrigin;
    }

//  Use
public:
    void setStringTo (char const *pStringOrigin) {
	m_pStringOrigin = pStringOrigin;
    }

    void reset () {
	m_pSearchOrigin = m_pStringOrigin;
    }

    void succeed (char const *pPatternOrigin, char const *pPatternExtent) {
	m_pPatternOrigin = pPatternOrigin;
	m_pPatternExtent = pPatternExtent;
    }

    void succeed (unsigned int iPatternOffset, unsigned int iPatternExtent) {
	m_pPatternOrigin = m_pSearchOrigin + iPatternOffset;
	m_pPatternExtent = m_pPatternOrigin + iPatternExtent;
    }

    void fail (size_t iFailOffset) {
	m_pPatternExtent = m_pPatternOrigin = m_pSearchOrigin + iFailOffset;
    }

    void fail () {
	fail (strlen (m_pSearchOrigin));
    }

    bool step () {
	if (m_pPatternExtent > m_pPatternOrigin)
	    m_pSearchOrigin = m_pPatternOrigin = m_pPatternExtent;
	else
	    fail ();
	return patternFound ();
    }

//  State
protected:
    char const *m_pStringOrigin;
    char const*	m_pSearchOrigin;
    char const*	m_pPatternOrigin;
    char const*	m_pPatternExtent;
};


/**************************************
 *----  String Search Controller  ----*
 **************************************/

class StringSearchController : public VTransient {
//  Construction
protected:
    StringSearchController () {
    }

//  Destruction
protected:
    ~StringSearchController () {
    }

//  Access
public:

//  Use
public:
    void findFirst (StringSearchCursor& rSearchCursor, char const *pSearchPattern) {
	rSearchCursor.reset ();
	doSearch (rSearchCursor, pSearchPattern);
    }

    void findNext (StringSearchCursor& rSearchCursor) {
	if (rSearchCursor.step ())
	    doSearch (rSearchCursor);
    }

protected:
    virtual void doSearch (StringSearchCursor& rSearchCursor, char const *pSearchPattern = 0) = 0;

//  State
protected:
};


/******************************************************
 *----  String Search Break Character Controller  ----*
 ******************************************************/

extern "C" {
    typedef size_t (__cdecl *SpanFunction) (char const*, char const*);
}

class StringSearchControllerBreakSet : public StringSearchController {
//  Construction
public:
    StringSearchControllerBreakSet (SpanFunction pSpanFunction) : m_pSpanFunction (pSpanFunction) {
    }

//  Destruction
public:
    ~StringSearchControllerBreakSet () {
    }

//  Use
protected:
    void doSearch (StringSearchCursor& rSearchCursor, char const *pSearchPattern);

//  State
protected:
    SpanFunction const	m_pSpanFunction;
    char const*		m_pBreakSet;
};


/*********
 *  Use  *
 *********/

void StringSearchControllerBreakSet::doSearch (
    StringSearchCursor& rSearchCursor, char const *pSearchPattern
)
{
    if (pSearchPattern)
	m_pBreakSet = pSearchPattern;

    char const *pSearchOrigin = rSearchCursor.searchOrigin ();
    size_t iSearchExtent = m_pSpanFunction (pSearchOrigin, m_pBreakSet);

    if (pSearchOrigin[iSearchExtent])
	rSearchCursor.succeed (iSearchExtent, 1);
    else
	rSearchCursor.fail (iSearchExtent);
}


/********************************************
 *----  String Search Regex Controller  ----*
 ********************************************/

class StringSearchControllerRegex : public StringSearchController {
//  Construction
public:
    StringSearchControllerRegex ();

//  Destruction
public:
    ~StringSearchControllerRegex ();

//  Use
protected:
    void doSearch (StringSearchCursor& rSearchCursor, char const *pSearchPattern);
private:
    void deleteCompiledPattern ();

//  State
protected:
    VkRegExp*	m_pCompiledPattern;
    char const*	m_pCompiledPatternSource;
};


/******************
 *  Construction  *
 ******************/

StringSearchControllerRegex::StringSearchControllerRegex ()
: m_pCompiledPattern (0), m_pCompiledPatternSource (0)
{
}

/*****************
 *  Destruction  *
 *****************/

StringSearchControllerRegex::~StringSearchControllerRegex () {
}


/*********
 *  Use  *
 *********/

void StringSearchControllerRegex::deleteCompiledPattern () {
    if (m_pCompiledPattern) {
#if defined(UsingVkRegularExpressions)
	delete m_pCompiledPattern;
#else
	::free (m_pCompiledPattern);
#endif
	m_pCompiledPattern = 0;
    }
}

void StringSearchControllerRegex::doSearch (
    StringSearchCursor& rSearchCursor, char const *pSearchPattern
)
{
    if (IsntNil (pSearchPattern) && (
	    IsNil  (m_pCompiledPatternSource) ||
	    strcmp (m_pCompiledPatternSource, pSearchPattern) != 0
	)
    ) {
	deleteCompiledPattern ();

#if defined(UsingVkRegularExpressions)
	m_pCompiledPattern = new VkRegExp ((char *)pSearchPattern);
#else
	m_pCompiledPattern = regcmp (pSearchPattern, NilOf (char *));
#endif
	m_pCompiledPatternSource = pSearchPattern;
    }

#if defined(UsingVkRegularExpressions)
    VkRegExp::Query iQuery (rSearchCursor.searchOrigin ());
    if (m_pCompiledPattern->Execute (iQuery)) {
	unsigned int xStart = 0;
	unsigned int xEnd = 0;
	iQuery.getMatch (xStart, xEnd, 0);
	rSearchCursor.succeed (xStart, xEnd);
    }
#else
    char const *pPatternExtent;
    if (IsntNil (m_pCompiledPattern) &&
	IsntNil (pPatternExtent = regex (m_pCompiledPattern, rSearchCursor.searchOrigin ()))
    )
    {
	rSearchCursor.succeed (__loc1, pPatternExtent);
    }
#endif
    else rSearchCursor.fail ();
}


/*********************************************
 *----  String Search String Controller  ----*
 *********************************************/

class StringSearchControllerString : public StringSearchController {
//  Construction
public:
    StringSearchControllerString () {
    }

//  Destruction
public:
    ~StringSearchControllerString () {
    }

//  Use
protected:
    void doSearch (StringSearchCursor& rSearchCursor, char const *pSearchPattern);

//  State
protected:
    char const *m_pSearchString;
    size_t	m_sSearchString;
};


/*********
 *  Use  *
 *********/

void StringSearchControllerString::doSearch (
    StringSearchCursor& rSearchCursor, char const *pSearchPattern
) {
    if (pSearchPattern) {
	m_pSearchString = pSearchPattern;
	m_sSearchString = strlen (pSearchPattern);
    }

    char const *pPatternOrigin = strstr (rSearchCursor.searchOrigin (), m_pSearchString);

    if (pPatternOrigin)
	rSearchCursor.succeed (pPatternOrigin, pPatternOrigin + m_sSearchString);
    else
	rSearchCursor.fail ();
}


/****************************************
 *----  String String Binary Class  ----*
 ****************************************/

class StringPrimitive : public VTransient {
//  Construction
protected:
    StringPrimitive (VPrimitiveTask* pTask);

//  Destruction
protected:
    ~StringPrimitive ();

//  Access
public:
    bool isScalar () const {
	return m_pTask->isScalar ();
    }

    unsigned int flags () const { 
	return m_pTask->flags ();
    }
    unsigned int primitive () const {
	return m_pTask->primitive ();
    }

    rtPTOKEN_Handle *ptoken () const {
	return m_pTask->ptoken ();
    }

    DSC_Descriptor &recipientMonotype () const {
	return m_iRecipientStrings.monotype ();
    }
    char const *recipientString (unsigned int xString) const {
	return m_iRecipientStrings.string (xString);
    }

    DSC_Descriptor &parameterMonotype () const {
	return m_iParameterStrings.monotype ();
    }
    char const *parameterString (unsigned int xString) const {
	return m_iParameterStrings.string (xString);
    }

    M_KOT *codKOT () const {
	return m_pTask->codKOT ();
    }

//  State
protected:
    VPrimitiveTask const*	m_pTask;
    StringStoreDescriptor	m_iRecipientStrings;
    StringStoreDescriptor	m_iParameterStrings;
};


/******************
 *  Construction  *
 ******************/

StringPrimitive::StringPrimitive (VPrimitiveTask* pTask)
: m_pTask		(pTask)
, m_iRecipientStrings	(pTask->getCurrent ())
, m_iParameterStrings	(ADescriptor)
{
}

/*****************
 *  Destruction  *
 *****************/

StringPrimitive::~StringPrimitive () {
}


/************************
 ************************
 *****  Primitives  *****
 ************************
 ************************/

/****************************
 *****  Size Primitive  *****
 ****************************/

V_DefinePrimitive (StringSize) {
/***** Traversal Macros ... *****/
#define handleString(value)\
    *rptr++ = strlen (pStrings[value])


/*****  Obtain the strings ...  *****/
    pTask->loadDucWithCurrent ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

/*****  Determine the sizes ...  *****/
    if (DucIsAScalar) {
	int size = 0;

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (ADescriptor.store ()))
		size = strlen (pBlockStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]);
	    break;
	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (ADescriptor.store ()))
		size = strlen (pLStoreStrings[DSC_Descriptor_Scalar_Int (ADescriptor)]);
	    break;
	}

	pTask->loadDucWithInteger (size);
    }
    else {
/******  Make the result Integer UVector  ... *****/
	M_CPD *resultuv = pTask->NewIntUV ();
        int *rptr = rtINTUV_CPD_Array (resultuv);

/*****  Determine the kind of string ... *****/
	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV:
	    if (pBlockStrings.setTo (ADescriptor.store ())) {
#define pStrings pBlockStrings
	    DSC_Traverse (ADescriptor, handleString);
#undef pStrings
	    }
	    break;

	case RTYPE_C_Link:
	    if (pLStoreStrings.setTo (ADescriptor.store ())) {
#define pStrings pLStoreStrings
		DSC_Traverse (ADescriptor, handleString);
	}
#undef pStrings
	}


/*****  Put the result into the accumulator ... *****/
	pTask->loadDucWithMonotype (resultuv);
	resultuv->release ();
    }

/***** Undef the traversal macros ... *****/
#undef handleString
}


/***************************************************
 *****  Numeric Encoding Primitive (Obsolete)  *****
 ***************************************************/

#define EncodeString(string, encodedString) {\
    char const *p; char c; unsigned int i;\
    for (i=0, encodedString = 0.0, p = (string);\
	 i<12;\
	 i++,\
	 encodedString = encodedString * 38 + (\
	    (!*p) ? 0 :\
	    islower(c= *p++) ? c-'a'+1 : isupper (c) ? c-'A'+1 : isdigit(c) ?\
	    c-'0'+27 : isspace(c) ? 0 : 37));\
}


V_DefinePrimitive (EncodeStringAsDouble) {
/*****  Traversal macros ... *****/
#define handleString(value) {\
    EncodeString (pStrings[value], encodedString);\
    *rptr++ = encodedString;\
}

/*****  Obtain the strings ...  *****/
    pTask->loadDucWithCurrent ();

    if (DucIsAScalar) {
	double encodedString = 0.0;

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV: {
		rtBLOCK_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ()))
		    EncodeString (pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)], encodedString);
	    }
	    break;
	case RTYPE_C_Link: {
		rtLSTORE_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ()))
		    EncodeString (pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)], encodedString);
	}
	}
	pTask->loadDucWithDouble (encodedString);
    }
    else {
	double encodedString;

/******  Make the result Double UVector  ... *****/
	M_CPD *resultuv = pTask->NewDoubleUV ();

	double *rptr = rtDOUBLEUV_CPD_Array (resultuv);

/*****  Determine the type of string ... *****/
	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV: {
		rtBLOCK_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
		    DSC_Traverse (ADescriptor, handleString);
		}
	    }
	    break;
	case RTYPE_C_Link: {
		rtLSTORE_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
		    DSC_Traverse (ADescriptor, handleString);
		}
	}
	    break;
	}

/*****  Put the result into the accumulator ... *****/
	pTask->loadDucWithMonotype (resultuv);
	resultuv->release ();
    }

/*****  Undef the traversal macros ... *****/
#undef handleString
}


/******************************************
 *****  Numeric Conversion Primitive  *****
 ******************************************/

/***************************************
 *  String NA Error Handler Utilities  *
 ***************************************/

/*****  Global Variables  *****/
PrivateVarDef rtLINK_CType *StringPrimsGoodLinkc;
PrivateVarDef int StringPrimsFoundNa;
PrivateVarDef int StringPrimsScalarCase;
PrivateVarDef int StringPrimsCount;
PrivateVarDef int StringPrimsLastNa;

/*---------------------------------------------------------------------------
 * Macro to Initialize the variables used by the Na handlers.
 *
 * Notes:
 * This macro takes care of everything except for the following 2 variables:
 *
 *	StringPrimsScalarCase 	- must be set somewhere, before any
 *                                calculation(s) occur(s).
 *	StringPrimsCount	- must be incremented after each calculation
 *            			  for the non scalar case, by the routine
 *                                that is doing the actual calculation
 ******/
#define StringPrimsInitialize() {\
    StringPrimsFoundNa = false;\
    StringPrimsCount  = StringPrimsLastNa = 0;\
    StringPrimsGoodLinkc = NilOf (rtLINK_CType *);\
}


#define StringPrimsNaHandler(result) {\
/***** If scalar case return ... *****/\
    if (StringPrimsScalarCase) {\
	StringPrimsFoundNa = true;\
	result = 1;\
    }\
\
    else {\
/***** If the first NA found ... *****/\
	if (!StringPrimsFoundNa)\
	{\
	    StringPrimsGoodLinkc = pTask->NewSubset ();\
	    rtLINK_AppendRange (StringPrimsGoodLinkc, 0, StringPrimsCount);\
	    StringPrimsLastNa = StringPrimsCount;\
	    StringPrimsFoundNa = true;\
	}\
	else\
	{\
	    rtLINK_AppendRange (\
		StringPrimsGoodLinkc,\
		StringPrimsLastNa + 1,\
		StringPrimsCount - StringPrimsLastNa - 1\
	    );\
	    StringPrimsLastNa = StringPrimsCount;\
	}\
	result = 1;\
    }\
}


/*---------------------------------------------------------------------------
 * Macro to append the last range on to the StringPrimsGoodLinkc, and close it.
 *****/
#define StringPrimsFinishAndCloseLinkc() {\
/***** if this had any NA's ... *****/\
    if (StringPrimsFoundNa && (!StringPrimsScalarCase)) {\
/***** ...  append the last piece to the linkc and close it ... *****/\
        rtLINK_AppendRange (\
	    StringPrimsGoodLinkc,\
	    StringPrimsLastNa + 1,\
	    StringPrimsCount - StringPrimsLastNa - 1\
	);\
        StringPrimsGoodLinkc->Close (pTask->NewDomPToken (StringPrimsGoodLinkc->ElementCount ()));\
    }\
}


#define ConvertStringToDouble(string, result) {\
    char *ptr;\
\
    DEBUG_TraceTestdeck ("stringPrims (1A)");\
    result = strtod (string, &ptr);\
    if (ptr == string || strspn (ptr, " ") != strlen (ptr)) {\
	StringPrimsNaHandler (result);\
    }\
}


V_DefinePrimitive (ToDouble) {
/*****  Traversal macros ... *****/
#define handleString(value) {\
    ConvertStringToDouble (pStrings[value], result);\
    *rptr++ = result;\
    StringPrimsCount++;\
}


/***** Setup ... *****/
    StringPrimsInitialize ();
    pTask->loadDucWithCurrent ();

/***** Single Case ... *****/
    if (StringPrimsScalarCase = (DucIsAScalar)) {
	double result;

	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV: {
		rtBLOCK_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
	    DEBUG_TraceTestdeck ("stringPrims (1B)");
		    ConvertStringToDouble (pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)], result);
		}
	}
	    break;
	case RTYPE_C_Link: {
		rtLSTORE_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
	    DEBUG_TraceTestdeck ("stringPrims (1C)");
		    ConvertStringToDouble (pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)], result);
		}
	    }
	    break;
	}

	if (StringPrimsFoundNa)
	    pTask->loadDucWithNA ();
	else
	    pTask->loadDucWithDouble (result);
    }

/***** ... else non scalar case ... *****/
    else {
	M_CPD* resultUV = pTask->NewDoubleUV ();
	double *rptr = rtDOUBLEUV_CPD_Array (resultUV);
	double result;

/*****  Determine the type of string ... *****/
	switch (ADescriptor.pointerRType ()) {
	case RTYPE_C_IntUV: {
		rtBLOCK_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
	    DEBUG_TraceTestdeck ("stringPrims (1D)");
		    DSC_Traverse (ADescriptor, handleString);
		}
	}
	    break;
	case RTYPE_C_Link: {
		rtLSTORE_Handle::Strings pStrings;
		if (pStrings.setTo (ADescriptor.store ())) {
	    DEBUG_TraceTestdeck ("stringPrims (1E)");
		    DSC_Traverse (ADescriptor, handleString);
		}
	    }
	    break;
	}

/*****  Close the linkc if necessary ... *****/
	StringPrimsFinishAndCloseLinkc ();

/*****  Load the result into the accumulator ... *****/
	pTask->loadDucWithPartialFunction (StringPrimsGoodLinkc, resultUV);

/*****  Cleanup ... *****/
	if (StringPrimsGoodLinkc)
	    StringPrimsGoodLinkc->release ();
	resultUV->release ();
    } /* end of non-scalar case */

/*****  Undef the traversal macros ... *****/
#undef handleString
}


/*********************************************
 *****  Relational Comparison Primitive  *****
 *********************************************/

PrivateFnDef void StringRelationalComparison (VPrimitiveTask* pTask) {
    int result;

#define calculateResult(sValue, aValue) {\
    char const *pString1 = iRecipientStrings.string (sValue);\
    char const *pString2 = iParameterStrings.string (aValue);\
    result = strcmp (pString1, pString2);\
}

#define handleComparisons(sValue, aValue) {\
    calculateResult (sValue, aValue);\
\
    thisLink = result < 0\
	? lessThanLink\
	: result == 0\
	? equalToLink\
	: greaterThanLink;\
\
    if (thisLink == lastLink)\
	count++;\
    else {\
	if (count > 0) {\
	    rtLINK_AppendRange (lastLink, origin, count);\
	    origin	+= count;\
	}\
	count		= 1;\
	lastLink	= thisLink;\
    }\
}

/*****  Get store descriptors for the two sets of strings, ... *****/
    StringStoreDescriptor iRecipientStrings (pTask->getCurrent ());
    StringStoreDescriptor iParameterStrings (ADescriptor);

/***** Handle the single string comparison first ... *****/
    if (DucIsAScalar) {

/*****  Calculate result ... *****/
	calculateResult (
	    DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()),
	    DSC_Descriptor_Scalar_Int (iParameterStrings.monotype ())
	);

	if (result < 0) {
	    switch (V_TOTSC_Primitive)
	    {
	    case XStringLessThan:
	    case XStringLessThanOrEqual:
	    case XStringNotEqual:
		pTask->loadDucWithTrue ();
		break;
	    case XStringEqual:
	    case XStringGreaterThanOrEqual:
	    case XStringGreaterThan:
		pTask->loadDucWithFalse ();
		break;
	    default:
		pTask->raiseUnimplementedAliasException("StringRelationalComparison");
		break;
	    }
	}
	else if (result == 0) {
	    switch (V_TOTSC_Primitive) {
	    case XStringLessThanOrEqual:
	    case XStringEqual:
	    case XStringGreaterThanOrEqual:
		pTask->loadDucWithTrue ();
		break;
	    case XStringLessThan:
	    case XStringNotEqual:
	    case XStringGreaterThan:
		pTask->loadDucWithFalse ();
		break;
	    default:
		pTask->raiseUnimplementedAliasException("StringRelationalComparison");
		break;
	    }
	}
	else /* result is greater than */ {
	    switch (V_TOTSC_Primitive) {
	    case XStringLessThan:
	    case XStringLessThanOrEqual:
	    case XStringEqual:
		pTask->loadDucWithFalse ();
		break;
	    case XStringNotEqual:
	    case XStringGreaterThanOrEqual:
	    case XStringGreaterThan:
		pTask->loadDucWithTrue ();
		break;
	    default:
		pTask->raiseUnimplementedAliasException("StringRelationalComparison");
		break;
	    }
	}
    }


/***** Non single string case ... *****/
    else {
	rtLINK_CType *trueLink  = pTask->NewSubset ();
	rtLINK_CType *falseLink = pTask->NewSubset ();
	rtLINK_CType *lessThanLink, *equalToLink, *greaterThanLink, *thisLink, *lastLink;

        int origin, count;

	switch (V_TOTSC_Primitive) {
	case XStringLessThan:
	    lessThanLink	= trueLink;
	    equalToLink		=
	    greaterThanLink	= falseLink;
	    break;
	case XStringLessThanOrEqual:
	    lessThanLink	=
	    equalToLink		= trueLink;
	    greaterThanLink	= falseLink;
	    break;
	case XStringEqual:
	    equalToLink		= trueLink;
	    lessThanLink	=
	    greaterThanLink	= falseLink;
	    break;
	case XStringNotEqual:
	    equalToLink		= falseLink;
	    lessThanLink	=
	    greaterThanLink	= trueLink;
	    break;
	case XStringGreaterThanOrEqual:
	    lessThanLink	= falseLink;
	    equalToLink		=
	    greaterThanLink	= trueLink;
	    break;
	case XStringGreaterThan:
	    lessThanLink	=
	    equalToLink		= falseLink;
	    greaterThanLink	= trueLink;
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("StringRelationalComparison");
	    break;
	}

	lastLink	= trueLink;
	origin		=
	count		= 0;

	DSC_JointTraversal (
	    iRecipientStrings.monotype (), iParameterStrings.monotype (), handleComparisons
	);

	if (count > 0)
	    rtLINK_AppendRange (lastLink, origin, count);

	pTask->loadDucWithPredicateResult (trueLink, falseLink);
    }


/*****  Undef the macros ... *****/
#undef calculateResult
#undef handleComparisons
}


V_DefinePrimitive (CompareStrings) {
/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	int converse;

	switch (V_TOTSC_Primitive) {
	case XStringLessThan:
	    converse = KS__GreaterThan;
	    break;
	case XStringLessThanOrEqual:
	    converse = KS__GreaterThanOrEqual;
	    break;
	case XStringEqual:
	    converse = KS__Equal;
	    break;
	case XStringNotEqual:
	    converse = KS__NotEqual;
	    break;
	case XStringGreaterThanOrEqual:
	    converse = KS__LessThanOrEqual;
	    break;
	case XStringGreaterThan:
	    converse = KS__LessThan;
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("StringRelationalComparison");
	    break;
	}

        pTask->sendBinaryConverseWithCurrent (converse);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();
 
/***** If the argument is a (collection of) string(s) ... *****/
    if (ParameterReferencesAStringStore (pTask))
	StringRelationalComparison (pTask);
    else if (ParameterReferencesAStringStoreExtension (pTask)) {
	int converse;

	switch (V_TOTSC_Primitive) {
	case XStringLessThan:
	    converse = KS__GreaterThan;
	    break;
	case XStringLessThanOrEqual:
	    converse = KS__GreaterThanOrEqual;
	    break;
	case XStringEqual:
	    converse = KS__Equal;
	    break;
	case XStringNotEqual:
	    converse = KS__NotEqual;
	    break;
	case XStringGreaterThanOrEqual:
	    converse = KS__LessThanOrEqual;
	    break;
	case XStringGreaterThan:
	    converse = KS__LessThan;
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("StringRelationalComparison");
	    break;
	}

        pTask->sendBinaryConverseWithCurrent (converse);
    }
    else
	pTask->loadDucWithFalse ();
}


/**************************************
 *****  String Search Primitives  *****
 **************************************/

/*****************************************
 *----  String Search Handler Class  ----*
 *****************************************/

class StringPrimitiveFindStringOrPattern : public StringPrimitive {
//  Construction
public:
    StringPrimitiveFindStringOrPattern (
	VPrimitiveTask* pTask, StringSearchController& rSearchController
    ) : StringPrimitive (pTask), m_rSearchController (rSearchController)
    {
    }

//  Destruction
public:
    ~StringPrimitiveFindStringOrPattern () {
    }

//  Access
public:
    bool patternFound () const {
	return m_iSearchCursor.patternFound ();
    }

    char const *  searchOrigin () const { return m_iSearchCursor.searchOrigin (); }
    size_t const& searchExtent () const { return m_iPatternOffset; }

    char const *  patternOrigin () const { return m_iSearchCursor.patternOrigin (); }
    size_t const& patternExtent () const { return m_iPatternExtent; }

//  Execution
public:
    void execute (size_t const& rPatternLocatorResult);

protected:
    void find (unsigned int xString, unsigned int xPattern) {
	m_iSearchCursor.setStringTo (recipientString (xString));

	m_rSearchController.findFirst (m_iSearchCursor, parameterString (xPattern));

	m_iPatternOffset = m_iSearchCursor.searchExtent ();
	m_iPatternExtent = m_iSearchCursor.patternExtent ();
    }

//  State
protected:
    StringSearchCursor		m_iSearchCursor;
    StringSearchController&	m_rSearchController;
    size_t			m_iPatternOffset;
    size_t			m_iPatternExtent;
};


/***********************
 *----  Execution  ----*
 ***********************/

void StringPrimitiveFindStringOrPattern::execute (size_t const& rPatternLocatorResult) {
    if (isScalar ()) {
	find (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
	if (XStringMatch != primitive ())
	    m_pTask->loadDucWithInteger (rPatternLocatorResult);
	else
	    m_pTask->loadDucWithBoolean (m_iSearchCursor.patternFound ());
    }
    else if (XStringMatch != primitive ()) {
#	define elementHandler(sValue, aValue) {\
	    find (sValue, aValue);\
	    *pResult++ = rPatternLocatorResult;\
	}

	M_CPD* resultUV = m_pTask->NewIntUV ();
	int *pResult = rtINTUV_CPD_Array (resultUV);

	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), elementHandler);

	m_pTask->loadDucWithMonotype (resultUV);
	resultUV->release ();
#	undef elementHandler
    }
    else {
#	define elementHandler(sValue, aValue) {\
	    find (sValue, aValue);\
	    thisLink = m_iSearchCursor.patternFound () ? trueLink : falseLink;\
	    if (thisLink == lastLink)\
		count++;\
	    else {\
		if (count > 0) {\
		    rtLINK_AppendRange (lastLink, origin, count);\
		    origin	+= count;\
		}\
		count	 = 1;\
		lastLink = thisLink;\
	    }\
	}

	rtLINK_CType *trueLink  = m_pTask->NewSubset ();
	rtLINK_CType *falseLink = m_pTask->NewSubset ();
	rtLINK_CType *lastLink  = trueLink;
	rtLINK_CType *thisLink;
	int origin	= 0;
	int count	= 0;

	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), elementHandler);

	if (count > 0)
	    rtLINK_AppendRange (lastLink, origin, count);

	m_pTask->loadDucWithPredicateResult (trueLink, falseLink);
#	undef elementHandler
    }
}


/*************************************
 *----  String Search Primitive  ----*
 *************************************/

V_DefinePrimitive (StringSearch) {
/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ())
	switch (V_TOTSC_Primitive) {
	case XStringMatch:
	    pTask->sendBinaryConverseWithCurrent (KS__MatchesString);
	    return;
	case XFindSubstring:
	    pTask->sendBinaryConverseWithCurrent ("substringOriginIn:");
	    return;
	case XFindPatternOrigin:
	    pTask->sendBinaryConverseWithCurrent ("patternOriginIn:");
	    return;
	case XFindPatternExtent:
	    pTask->sendBinaryConverseWithCurrent ("patternExtentIn:");
	    return;
	default:
	    pTask->raiseUnimplementedAliasException ("StringSearch");
	    return;
	}

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();

/***** If the argument is a (collection of) string(s) ... *****/
    if (!ParameterReferencesAStringStore (pTask))
	pTask->loadDucWithNA ();
    else switch (V_TOTSC_Primitive) {
    /*****  ... perform the search, ...  *****/
    case XStringMatch:
    case XFindPatternOrigin: {
//	    predicate = &StringPrimitiveFindStringOrPattern::findPattern;
//	    pResultLocation = &locator.m_iPatternOffset;
	    StringSearchControllerRegex iSearchController;
	    StringPrimitiveFindStringOrPattern iPrimitive (pTask, iSearchController);
	    iPrimitive.execute (iPrimitive.searchExtent ());
	}
	break;
    case XFindPatternExtent: {
//	    predicate = &StringPrimitiveFindStringOrPattern::findPattern;
//	    pResultLocation = &locator.m_iPatternExtent;
	    StringSearchControllerRegex iSearchController;
	    StringPrimitiveFindStringOrPattern iPrimitive (pTask, iSearchController);
	    iPrimitive.execute (iPrimitive.patternExtent ());
	}
	break;
    case XFindSubstring: {
//	    predicate = &StringPrimitiveFindStringOrPattern::findSubstring;
//	    pResultLocation = &locator.m_iPatternOffset;
	    StringSearchControllerString iSearchController;
	    StringPrimitiveFindStringOrPattern iPrimitive (pTask, iSearchController);
	    iPrimitive.execute (iPrimitive.searchExtent ());
	}
	break;
    default:
	pTask->raiseUnimplementedAliasException ("StringSearch");
	break;
    }
}


/********************************
 *****  Spanning Primitive  *****
 ********************************/

PrivateFnDef void CalculateStringSpans (VPrimitiveTask* pTask, SpanFunction spanfn) {
#define computeSpan(sValue, aValue) {\
    char const *pString1 = iRecipientStrings.string (sValue);\
    char const *pString2 = iParameterStrings.string (aValue);\
    *resultp++ = spanfn (pString1, pString2);\
}

/*****  Get store descriptors for the two sets of strings, ... *****/
    StringStoreDescriptor iRecipientStrings (pTask->getCurrent ());
    StringStoreDescriptor iParameterStrings (ADescriptor);

/*****  ... do the scan, ...  *****/
    if (DucIsAScalar) {
	int  iResult;
	int* resultp = &iResult;
	computeSpan (
	    DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()),
	    DSC_Descriptor_Scalar_Int (iParameterStrings.monotype ())
	);
	pTask->loadDucWithInteger (iResult);
    }
    else {
	M_CPD* resultuv = pTask->NewIntUV ();
	int *resultp = rtINTUV_CPD_Array (resultuv);

	DSC_JointTraversal (
	    iRecipientStrings.monotype (), iParameterStrings.monotype (), computeSpan
	);

	pTask->loadDucWithMonotype (resultuv);
	resultuv->release ();
    }

/*****  ... and #undef the macros.  *****/
#undef computeSpan
}


V_DefinePrimitive (StringSpans)
{
    SpanFunction	spanfn;
    int			converse;

/***** Determine the desired span... *****/
    switch (V_TOTSC_Primitive) {
    case XStringSpan:
	converse    = KS__PrefixSpannedIn;
        spanfn	    = strspn;
        break;
    case XStringCSpan:
	converse    = KS__PrefixNotSpannedIn;
        spanfn	    = strcspn;
        break;
    default:
        pTask->raiseUnimplementedAliasException ("String Span");
        break;
    }

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (converse);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();

/*****  ... and do it if the argument is a (collection of) string(s):  *****/
    if (ParameterReferencesAStringStore (pTask))
	CalculateStringSpans (pTask, spanfn);
    else
	pTask->loadDucWithNA ();
}


/********************************
 *****  Breaking Primitive  *****
 ********************************/

/**********************************
 *----  StringPrimitiveBreak  ----*
 **********************************/

class StringPrimitiveBreak : public StringPrimitive {
//  Construction
protected:
    StringPrimitiveBreak (VPrimitiveTask* pTask) : StringPrimitive (pTask) {
    }

//  Destruction
protected:
    ~StringPrimitiveBreak () {
    }

//  Execution
public:
    void execute ();

protected:
    virtual void countCharacters () = 0;
    virtual void breakStrings    () = 0;

    void output (char const *pString, unsigned int sString) {
	strncpy (m_pResultEndpoint, pString, sString);
	m_pResultEndpoint += sString;
	*m_pResultEndpoint++ = '\0';
    }

    void outputSearchExtent (StringSearchCursor& rSearchCursor) {
	output (rSearchCursor.searchOrigin (), rSearchCursor.searchExtent ());
    }

    void outputPatternExtent (StringSearchCursor& rSearchCursor) {
	output (rSearchCursor.patternOrigin (), rSearchCursor.patternExtent ());
    }

//  State
protected:
    unsigned int		m_iResultCharacterCount;
    char*			m_pResultEndpoint;
    unsigned int		m_xResultList;
    rtLINK_CType *		m_pResultListPartition;
};


/***************
 *  Execution  *
 ***************/

void StringPrimitiveBreak::execute () {
//  Count the characters in the result, ...
    countCharacters ();

//  ... create the strings, ...
    M_CPD* pCharUV = m_pTask->NewCharUV (m_iResultCharacterCount);

    m_pResultEndpoint = rtCHARUV_CPD_Array (pCharUV);
    m_xResultList = 0;

    m_pResultListPartition = rtLINK_RefConstructor (m_pTask->NewCodPToken ());

    breakStrings ();

/*****  ... create the string store ...  *****/
    rtLSTORE_Handle *pStringStore = rtLSTORE_NewStringStoreFromUV (pCharUV);
    pCharUV->release ();

/*****  ... create the result list store ...  *****/
    rtPTOKEN_Handle::Reference pStringStorePPT (pStringStore->getPToken ());

    rtPTOKEN_Handle::Reference pContentPPT (m_pTask->NewCodPToken (pStringStorePPT->cardinality ()));
    rtVECTOR_Handle::Reference pContent (new rtVECTOR_Handle (pContentPPT));
    rtLINK_CType *pContentLC = rtLINK_AppendRange (
	rtLINK_RefConstructor (pContentPPT), 0, pStringStorePPT->cardinality ()
    )->Close (pStringStorePPT);

    DSC_Descriptor iStringStoreDSC;
    iStringStoreDSC.constructIdentity (pStringStore, pStringStorePPT);
    pContent->setElements (pContentLC, iStringStoreDSC);
    pContentLC->release ();
    iStringStoreDSC.clear ();

    m_pResultListPartition->Close (pContentPPT);

    m_pTask->loadDucWithListOrStringStore (
	new rtLSTORE_Handle (m_pResultListPartition, pContent)
    );

    m_pResultListPartition->release ();
}


/********************************************
 *----  StringPrimitiveBreakIntoTokens  ----*
 ********************************************/

class StringPrimitiveBreakIntoTokens : public StringPrimitiveBreak {
//  Construction
public:
    StringPrimitiveBreakIntoTokens (
	VPrimitiveTask* pTask, StringSearchController& rSearchController
    );

//  Destruction
public:
    ~StringPrimitiveBreakIntoTokens () {
    }

//  Use
protected:
    void countCharacters ();
    void breakStrings    ();

    void countCharactersIn (unsigned int xRecipientString, unsigned int xSearchString) {
	char const *pRecipientString = recipientString (xRecipientString);
	m_iResultCharacterCount += strlen (pRecipientString) + 1;

	StringSearchCursor iSearchCursor (pRecipientString);
	m_rSearchController.findFirst (iSearchCursor, parameterString (xSearchString));
	while (iSearchCursor.patternFound ())
	{
	    m_iResultCharacterCount += 2;
	    m_rSearchController.findNext (iSearchCursor);
	}
    }

    void breakString (unsigned int xRecipientString, unsigned int xSearchString) {
	unsigned int iStringCount = 1;
	StringSearchCursor iSearchCursor (recipientString (xRecipientString));
	m_rSearchController.findFirst (iSearchCursor, parameterString (xSearchString));
	while (iSearchCursor.patternFound ())
	{
	    iStringCount += 2;

	    outputSearchExtent	(iSearchCursor);
	    outputPatternExtent	(iSearchCursor);

	    m_rSearchController.findNext (iSearchCursor);
	}
	outputSearchExtent (iSearchCursor);
	rtLINK_AppendRepeat (m_pResultListPartition, m_xResultList++, iStringCount);
    }

//  State
protected:
    StringSearchController& m_rSearchController;
};


/******************
 *  Construction  *
 ******************/

StringPrimitiveBreakIntoTokens::StringPrimitiveBreakIntoTokens (
    VPrimitiveTask* pTask, StringSearchController& rSearchController
) : StringPrimitiveBreak (pTask), m_rSearchController (rSearchController)
{
}


/***************
 *  Execution  *
 ***************/

void StringPrimitiveBreakIntoTokens::countCharacters () {
    m_iResultCharacterCount = 0;
    if (isScalar ()) {
	countCharactersIn (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
    }
    else {
	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), countCharactersIn);
    }
}

void StringPrimitiveBreakIntoTokens::breakStrings () {
    if (isScalar ()) {
	breakString (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
    }
    else {
	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), breakString);
    }
}


/********************************************
 *----  StringPrimitiveBreakOnBreakSet  ----*
 ********************************************/

class StringPrimitiveBreakOnBreakSet : public StringPrimitiveBreak {
//  Construction
public:
    StringPrimitiveBreakOnBreakSet (VPrimitiveTask* pTask, SpanFunction pSpanFunction);

//  Destruction
public:
    ~StringPrimitiveBreakOnBreakSet () {
    }

//  Execution
protected:
    void countCharacters ();
    void breakStrings    ();

    void countCharactersIn (unsigned int xRecipientString) {
	m_iResultCharacterCount += strlen (recipientString (xRecipientString)) + 1;
    }

    void breakString (unsigned int xRecipientString, unsigned int xBreakString) {
	strcpy (m_pResultEndpoint, recipientString (xRecipientString));
	char *rlimit = m_pResultEndpoint + strlen (m_pResultEndpoint);
	char const *pBreakSet = parameterString (xBreakString);
	size_t stringCount = 0;
	while (m_pResultEndpoint <= rlimit)
	{
	    m_pResultEndpoint += m_pSpanFunction (m_pResultEndpoint, pBreakSet);
	    *m_pResultEndpoint++ = '\0';
	    stringCount++;
	}
	rtLINK_AppendRepeat (m_pResultListPartition, m_xResultList++, stringCount);
    }

//  State
protected:
    SpanFunction const m_pSpanFunction;
};


/******************
 *  Construction  *
 ******************/

StringPrimitiveBreakOnBreakSet::StringPrimitiveBreakOnBreakSet (
    VPrimitiveTask* pTask, SpanFunction pSpanFunction
) : StringPrimitiveBreak (pTask), m_pSpanFunction (pSpanFunction)
{
}


/***************
 *  Execution  *
 ***************/

void StringPrimitiveBreakOnBreakSet::countCharacters ()
{
    m_iResultCharacterCount = 0;
    if (isScalar ())
    {
	countCharactersIn (DSC_Descriptor_Scalar_Int (recipientMonotype ()));
    }
    else
    {
	DSC_Traverse (recipientMonotype (), countCharactersIn);
    }
}

void StringPrimitiveBreakOnBreakSet::breakStrings ()
{
    if (isScalar ())
    {
	breakString (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
    }
    else
    {
	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), breakString);
    }
}


/***************************************************
 *----  StringPrimitiveBreakOnStringOrPattern  ----*
 ***************************************************/

class StringPrimitiveBreakOnStringOrPattern : public StringPrimitiveBreak {
//  Construction
public:
    StringPrimitiveBreakOnStringOrPattern (
	VPrimitiveTask* pTask, StringSearchController& rSearchController
    );

//  Destruction
public:
    ~StringPrimitiveBreakOnStringOrPattern () {
    }

//  Use
protected:
    void countCharacters ();
    void breakStrings    ();

    void countCharactersIn (unsigned int xRecipientString, unsigned int xSearchString) {
	StringSearchCursor iSearchCursor (recipientString (xRecipientString));
	m_rSearchController.findFirst (iSearchCursor, parameterString (xSearchString));
	while (iSearchCursor.patternFound ())
	{
	    m_iResultCharacterCount += iSearchCursor.searchExtent () + 1;
	    m_rSearchController.findNext (iSearchCursor);
	}
	m_iResultCharacterCount += iSearchCursor.searchExtent () + 1;
    }

    void breakString (unsigned int xRecipientString, unsigned int xSearchString) {
	unsigned int iStringCount = 1;
	StringSearchCursor iSearchCursor (recipientString (xRecipientString));
	m_rSearchController.findFirst (iSearchCursor, parameterString (xSearchString));
	while (iSearchCursor.patternFound ())
	{
	    iStringCount++;
	    outputSearchExtent (iSearchCursor);
	    m_rSearchController.findNext (iSearchCursor);
	}
	outputSearchExtent (iSearchCursor);
	rtLINK_AppendRepeat (m_pResultListPartition, m_xResultList++, iStringCount);
    }

//  State
protected:
    StringSearchController& m_rSearchController;
};


/******************
 *  Construction  *
 ******************/

StringPrimitiveBreakOnStringOrPattern::StringPrimitiveBreakOnStringOrPattern (
    VPrimitiveTask* pTask, StringSearchController& rSearchController
) : StringPrimitiveBreak (pTask), m_rSearchController (rSearchController)
{
}


/***************
 *  Execution  *
 ***************/

void StringPrimitiveBreakOnStringOrPattern::countCharacters () {
    m_iResultCharacterCount = 0;
    if (isScalar ()) {
	countCharactersIn (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
    }
    else {
	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), countCharactersIn);
    }
}

void StringPrimitiveBreakOnStringOrPattern::breakStrings () {
    if (isScalar ()) {
	breakString (
	    DSC_Descriptor_Scalar_Int (recipientMonotype ()),
	    DSC_Descriptor_Scalar_Int (parameterMonotype ())
	);
    }
    else {
	DSC_JointTraversal (recipientMonotype (), parameterMonotype (), breakString);
    }
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (StringBreak) {
    static char const *const pCaller = "String Break";

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	switch (V_TOTSC_Primitive) {
	case XStringBreak:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0:
		pTask->sendBinaryConverseWithCurrent (KS__BreakString);
		return;
	    case 1:
		pTask->sendBinaryConverseWithCurrent ("breakTokenize:");
		return;
	    default:
		break;
	    }
	    break;
	case XStringCBreak:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0:
		pTask->sendBinaryConverseWithCurrent (KS__CBreakString);
		return;
	    case 1:
		pTask->sendBinaryConverseWithCurrent ("cbreakTokenize:");
		return;
	    default:
		break;
	    }
	    break;
	case XStringBreakOnString:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0:
		pTask->sendBinaryConverseWithCurrent ("stringBreak:");
		return;
	    case 1:
		pTask->sendBinaryConverseWithCurrent ("stringTokenize:");
		return;
	    default:
		break;
	    }
	    break;
	case XStringBreakOnPattern:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0:
		pTask->sendBinaryConverseWithCurrent ("patternBreak:");
		return;
	    case 1:
		pTask->sendBinaryConverseWithCurrent ("patternTokenize:");
		return;
	    default:
		break;
	    }
	    break;
	default:
	    break;
	}
	pTask->raiseUnimplementedAliasException (pCaller);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();

/***** ... and do it if the argument is a (collection of) string(s):  *****/
    if (!ParameterReferencesAStringStore (pTask))
	pTask->loadDucWithNA ();
    else {
	bool notHandled = false;
	switch (V_TOTSC_Primitive) {
	case XStringBreak:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0: {
		    StringPrimitiveBreakOnBreakSet iPrimitive (pTask, strcspn);
		    iPrimitive.execute ();
		}
		break;
	    case 1: {
		    StringSearchControllerBreakSet iSearchController (strcspn);
		    StringPrimitiveBreakIntoTokens iPrimitive (pTask, iSearchController);
		    iPrimitive.execute ();
		}
		break;
	    default:
		notHandled = true;
		break;
	    }
	    break;
	case XStringCBreak:
	    switch (V_TOTSC_PrimitiveFlags) {
	    case 0: {
		    StringPrimitiveBreakOnBreakSet iPrimitive (pTask, strspn);
		    iPrimitive.execute ();
		}
		break;
	    case 1: {
		    StringSearchControllerBreakSet iSearchController (strspn);
		    StringPrimitiveBreakIntoTokens iPrimitive (pTask, iSearchController);
		    iPrimitive.execute ();
		}
		break;
	    default:
		notHandled = true;
		break;
	    }
	    break;
	case XStringBreakOnString: {
		StringSearchControllerString iSearchController;
		switch (V_TOTSC_PrimitiveFlags) {
		case 0: {
			StringPrimitiveBreakOnStringOrPattern iPrimitive (
			    pTask, iSearchController
			);
			iPrimitive.execute ();
		    }
		    break;
		case 1: {
			StringPrimitiveBreakIntoTokens iPrimitive (pTask, iSearchController);
			iPrimitive.execute ();
		    }
		    break;
		default:
		    notHandled = true;
		    break;
		}
	    }
	    break;
	case XStringBreakOnPattern: {
		StringSearchControllerRegex iSearchController;
		switch (V_TOTSC_PrimitiveFlags) {
		case 0: {
			StringPrimitiveBreakOnStringOrPattern iPrimitive (
			    pTask, iSearchController
			);
			iPrimitive.execute ();
		    }
		    break;
		case 1: {
			StringPrimitiveBreakIntoTokens iPrimitive (pTask, iSearchController);
			iPrimitive.execute ();
		    }
		    break;
		default:
		    notHandled = true;
		    break;
		}
	    }
	    break;
	default:
	    notHandled = true;
	    break;
	}
	if (notHandled)
	    pTask->raiseUnimplementedAliasException (pCaller);
    }
}


/***********************************
 *****  String Reversal, Etc.  *****
 ***********************************/

PrivateFnDef void Reverse (char* dst, char const *src) {
    dst += strlen (src);
    *dst = '\0';
    while (*src)
	*--dst = *src++;
}

PrivateFnDef void ToUpper (char *dst, char const *src) {
    strcpy (dst, src);
    while (*dst) {
	if (islower (*dst))
	    *dst = (char)(toupper (*dst));
	dst++;
    }
}

PrivateFnDef void ToLower (char *dst, char const *src) {
    strcpy (dst, src);
    while (*dst) {
	if (isupper (*dst))
	    *dst = (char)(tolower (*dst));
	dst++;
    }
}

PrivateFnDef void ToggleCase (char* dst, char const *src) {
    strcpy (dst, src);
    while (*dst) {
	if (isupper (*dst))
	    *dst = (char)(tolower (*dst));
	else if (islower (*dst))
	    *dst = (char)(toupper (*dst));
	dst++;
    }
}

PrivateFnDef void Capitalize (char *dst, char const *src) {
    strcpy (dst, src);
    *dst = (char)(toupper (*dst));
}

PrivateFnDef void Uncapitalize (char *dst, char const *src) {
    strcpy (dst, src);
    *dst = (char)(tolower (*dst));
}


/*---------------------------------------------------------------------------
 *****  Utility to copy and reverse a set of strings in a string store.
 *
 *  Arguments:
 *	dsc	- a descriptor referencing a collection of Block Strings or
 *		  LStore Strings.
 *
 *  Returns:
 *	A standard CPD for the LStore string store containing the copied and
 *	reversed strings.
 *
 *****/
PrivateFnDef rtLSTORE_Handle *CopyAndProcessStrings (
    VPrimitiveTask *pTask, void (*pStringProcessor)(char* dst, char const *src)
) {
    M_CPD *uvector = 0;
    rtCHARUV_ElementType *uvp;
    size_t size = 0;

#define CalcTotalSize(value)\
    size += strlen (pStrings[value]) + 1

#define Fill(value) {\
    pStringProcessor (uvp, pStrings[value]);\
    uvp += strlen (pStrings[value]) + 1;\
}

    DSC_Descriptor &rCurrent = pTask->getCurrent ();

/*****  LStore Strings  *****/
    rtLSTORE_Handle::Strings pLStoreStrings;
    rtBLOCK_Handle::Strings pBlockStrings;
    if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings

	/*****  Calculate the total number of characters ... *****/
	if (rCurrent.isScalar ()) {
	    CalcTotalSize (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, CalcTotalSize);
	}

	/*****  Create the new character uvector ... *****/
	uvector = pTask->NewCharUV (size);

	/*****  Fill the character uvector ... *****/
	uvp = rtCHARUV_CPD_Array (uvector);
	if (rCurrent.isScalar ()) {
	    Fill (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, Fill);
	}

#undef pStrings
    }

/*****  Block Strings  *****/
    else if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings

	/*****  Calculate the total number of characters ... *****/
	if (rCurrent.isScalar ()) {
	    CalcTotalSize (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, CalcTotalSize);
	}


	/*****  Create the new character uvector ... *****/
	uvector = pTask->NewCharUV (size);

	/*****  Fill the character uvector ... *****/
	uvp = rtCHARUV_CPD_Array (uvector);
	if (rCurrent.isScalar ()) {
	    Fill (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, Fill);
	}

#undef pStrings
    }

    /*****  Make the lstore ... *****/
    rtLSTORE_Handle *lstore = rtLSTORE_NewStringStoreFromUV (uvector);
    uvector->release ();

    return lstore;

#undef CalcTotalSize
#undef Fill
}


V_DefinePrimitive (StringReverse) {
    void (*pStringProcessor)(char*, char const*) = 0;

    switch (V_TOTSC_Primitive) {
    case XStringReverse:
	pStringProcessor = Reverse;
	break;

    case XStringProcessor:
	switch (V_TOTSC_PrimitiveFlags) {
	case 1:
	    pStringProcessor = ToUpper;
	    break;
	case 2:
	    pStringProcessor = ToLower;
	    break;
	case 3:
	    pStringProcessor = ToggleCase;
	    break;
	case 4:
	    pStringProcessor = Capitalize;
	    break;
	case 5:
	    pStringProcessor = Uncapitalize;
	    break;

	default:
	    break;
	}
	break;

    default:
	break;
    }

    if (pStringProcessor)
	pTask->loadDucWithListOrStringStore (
	    CopyAndProcessStrings (pTask, pStringProcessor)
	);
    else pTask->raiseUnimplementedAliasException ("StringProcessor");
}


/******************************************
 *****  Cluster Conversion Primitive  *****
 ******************************************/

PrivateFnDef char const *TraverseStrings (bool reset, va_list ap) {
    V::VArgList iArgList (ap);
    rtBLOCK_Handle::Strings *pStrings =  iArgList.arg<rtBLOCK_Handle::Strings*>();

    int* beginPtr	= iArgList.arg<int *>();
    int* endPtr		= iArgList.arg<int *>();
    int**workPtr	= iArgList.arg<int **>();

    if (reset) {
	*workPtr = beginPtr;
	return NilOf (char const*);
    }
    else if (*workPtr < endPtr)
	return (*pStrings)[*(*workPtr)++];
    else
	return NilOf (char const*);
}


V_DefinePrimitive (ToLStoreString) {
    pTask->loadDucWithCurrent ();

    DSC_Descriptor &rDucMonotype = pTask->ducMonotype ();

    rtBLOCK_Handle::Strings pStrings;
    if (rDucMonotype.pointerRType () != RTYPE_C_IntUV || !pStrings.setTo (rDucMonotype.store ())) {
    }
    else if (DucIsAScalar) {
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), pStrings[DSC_Descriptor_Scalar_Int (rDucMonotype)]
	    )
	);
    }
    else {
	int *ssip;

	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (),
		TraverseStrings,
		&pStrings,
		rtINTUV_CPD_Array (DSC_Descriptor_Value (rDucMonotype)),
		rtINTUV_CPD_Array (DSC_Descriptor_Value (rDucMonotype))
		+ UV_CPD_ElementCount (DSC_Descriptor_Value (rDucMonotype)),
		&ssip
	    )
	);
    }
}



/****************************************
 *****  String Strip Primitive      *****
 ****************************************/

PrivateFnDef size_t strspnFromEnd (char const* pString1, char const* pString2)
{
    size_t count = 0;
    int sLen1 = strlen (pString1);
    int sLen2 = strlen (pString2);
    bool f;  // f is True means a char in string1 is not in string2

    for (int sp1 = sLen1; sp1; sp1--) {
	f = true;
	for (int sp2 = sLen2; sp2; sp2--) {
	    if (*(pString1+sp1-1) == *(pString2+sp2-1)) {
		count++;
		f = false;
		break;
	    }
	}
	if (f) break;
    }
    return count;
}


V_DefinePrimitive (StringStrip)
{
    M_CPD *newCharUV;
    char const *converseString;
    bool fStripLead, fStripTrail, fAlwaysCopy;
    int strLength; // used in the macro as string length

/************************************************
 *  Macros for Strip Blanks Primitives          *
 ************************************************/

#define ComputeSpanAndTotalSize(sValue, aValue) {\
    char const* pString1 = iRecipientStrings.string (sValue);\
    char const* pString2 = iParameterStrings.string (aValue);\
    strLength = strlen (pString1);\
    *prefixSizesp = 0;\
    *suffixSizesp = 0;\
    if(fStripLead) *prefixSizesp = strspn(pString1, pString2);\
    if(fStripTrail && *prefixSizesp != strLength) *suffixSizesp = strspnFromEnd(pString1, pString2);\
\
    if (fAlwaysCopy || ((*prefixSizesp + (*suffixSizesp)) != 0))  {\
	totalSize += (strLength - (*prefixSizesp + (*suffixSizesp)) + 1);\
    }\
\
    prefixSizesp++;\
    suffixSizesp++;\
}


#define HandleSpan(sValue, aValue) {\
    ComputeSpanAndTotalSize (sValue, aValue); \
\
    thisLink =\
	(((*(prefixSizesp-1)) + (*(suffixSizesp-1))) == 0)\
	? zeroSpanLink\
        : nonZeroSpanLink;\
\
    if (thisLink == lastLink)\
     count++;\
    else\
    {\
	if (count > 0)\
	{\
	    rtLINK_AppendRange (lastLink, origin, count);\
	    origin    += count;\
	}\
	count          = 1;\
	lastLink       = thisLink;\
    }\
}


#define DropString(orig, new, pre, suf)\
{\
    char *origp;\
\
    origp = orig + pre;\
    strLength = strlen (origp) - suf;\
    memcpy(new,origp,strLength);\
\
    new[strLength] = '\0';\
}



/*****  Macros which drop the character uvector  *****/

#define HandleDrop(value)\
{\
    char const* pString = iRecipientStrings.string (value);\
    if (fAlwaysCopy|| ((*prefixSizesp + (*suffixSizesp)) != 0))  {\
	DropString (\
	    const_cast<char*>(pString), newCharp, *prefixSizesp, *suffixSizesp);\
	strLength = strlen (pString);\
	newCharp += (strLength - (*prefixSizesp + (*suffixSizesp)) + 1);\
    }\
    prefixSizesp++;\
    suffixSizesp++;\
}


/*
 *---------------------------------------------------------------------------
 * Code Body for 'StringStrip' primitive
 *---------------------------------------------------------------------------
 */

    switch (V_TOTSC_PrimitiveFlags)
    {
	case 0:
	    fStripLead = true;
	    fStripTrail = false;
	    fAlwaysCopy = false;
	    converseString = "stripFromFrontOf:";
	    break;
	case 1:
	    fStripLead = false;
	    fStripTrail = true;
	    fAlwaysCopy = false;
	    converseString = "stripFromEndOf:";
	    break;
	case 2:
	    fStripLead = true;
	    fStripTrail = true;
	    fAlwaysCopy = false;
	    converseString = "stripFromBothEndsOf:";
	    break;
	case 3:
	    fStripLead = true;
	    fStripTrail = false;
	    fAlwaysCopy = true;
	    converseString = "stripFromFrontOfAlwaysCopy:";
	    break;
	case 4:
	    fStripLead = false;
	    fStripTrail = true;
	    fAlwaysCopy = true;
	    converseString = "stripFromEndOfAlwaysCopy:";
	    break;
	case 5:
	    fStripLead = true;
	    fStripTrail = true;
	    fAlwaysCopy = true;
	    converseString = "stripFromBothEndsOfAlwaysCopy:";
	    break;
	default:
	    break;
    }



/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (converseString);
	return;
    }

    pTask->normalizeDuc ();

    if (ParameterReferencesAStringStore (pTask)) {
      // We can proceed with the task at hand ....
    } else if (ParameterReferencesAStringStoreExtension (pTask)) {
      // rely on the binary converse to strip the extensions ...
	pTask->sendBinaryConverseWithCurrent (converseString);
	return;
    } else {
      // invalid parameter ... do Nothing ...
	pTask->loadDucWithSelf ();
	return;
    }

/*****  Get store descriptors for the two sets of strings, ... *****/
    StringStoreDescriptor iRecipientStrings (pTask->getCurrent ());
    StringStoreDescriptor iParameterStrings (ADescriptor);
    
/*****  Scalar Case *****/
    if (DucIsAScalar)
    {
	int
	    prefixSize, suffixSize, totalSize = 0;
	char const
	    *pString;

/*****  Determine the size argument's value ...  *****/
	int* prefixSizesp = &prefixSize;
	int* suffixSizesp = &suffixSize;

	ComputeSpanAndTotalSize (
	    DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()),
	    DSC_Descriptor_Scalar_Int (iParameterStrings.monotype ())
	);

	if (fAlwaysCopy || ((prefixSize + suffixSize) != 0))  {

	    pString = iRecipientStrings.string (DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()));

	    newCharUV = pTask->NewCharUV ((unsigned int)totalSize);

	    DropString (const_cast<char*>(pString), rtCHARUV_CPD_Array (newCharUV), prefixSize, suffixSize);

	    pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (newCharUV));
	    newCharUV->release ();
	} else pTask->loadDucWithSelf();
    }

/***** Non-Scalar case *****/
    else
    {
	M_CPD
	    *prefixSizeUV,
	    *suffixSizeUV;
	int
	    totalSize = 0;
	rtCHARUV_ElementType
	    *newCharp;

// Declared here no matter whether they are used or not.
	rtLINK_CType
	    *zeroSpanLink,
	    *nonZeroSpanLink,
	    *thisLink, *lastLink;
	int
	    origin, count;

	prefixSizeUV = pTask->NewIntUV ();
	suffixSizeUV = pTask->NewIntUV ();
	int *prefixSizesp = rtINTUV_CPD_Array (prefixSizeUV);
	int *suffixSizesp = rtINTUV_CPD_Array (suffixSizeUV);


	if (fAlwaysCopy )  {
	    DSC_JointTraversal (
		iRecipientStrings.monotype (), iParameterStrings.monotype (), ComputeSpanAndTotalSize
	    );
	} else {
	    zeroSpanLink    = rtLINK_RefConstructor (V_TOTSC_PToken);
	    nonZeroSpanLink = rtLINK_RefConstructor (V_TOTSC_PToken);

	    lastLink = nonZeroSpanLink;
	    origin   =
	    count    = 0;

	    DSC_JointTraversal (
		iRecipientStrings.monotype (), iParameterStrings.monotype (), HandleSpan
	    );

	    if (count > 0)
		rtLINK_AppendRange (lastLink, origin, count);
	}

	if(totalSize != 0) {
	    /*****  Create the new character uvector ... *****/
	    newCharUV = pTask->NewCharUV ((unsigned int)totalSize);
	    newCharp = rtCHARUV_CPD_Array (newCharUV);

	    /*****  fill in the new character uvector ... *****/
	    prefixSizesp = rtINTUV_CPD_Array (prefixSizeUV);
	    suffixSizesp = rtINTUV_CPD_Array (suffixSizeUV);

	    DSC_Traverse (iRecipientStrings.monotype (), HandleDrop)
	}

	prefixSizeUV->release ();
	suffixSizeUV->release ();

	/*****  Make the lstore ... *****/
	if (fAlwaysCopy )  {
		pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (newCharUV)); 
		newCharUV->release ();
	} else {
	    if (0 == nonZeroSpanLink->ElementCount ()) {
		zeroSpanLink->release ();
		nonZeroSpanLink->release ();
		pTask->loadDucWithSelf ();
//		newCharUV->release ();   This can be deleted because newCharUV was created only when totalSize !=0
	    } else if (0 == zeroSpanLink->ElementCount ()) {
		zeroSpanLink->release ();
		nonZeroSpanLink->release ();
		pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (newCharUV)); 
		newCharUV->release ();
	    } else {
		VFragmentation& rFragmentation = pTask->loadDucWithFragmentation ();
		rtPTOKEN_Handle::Reference pFragmentPPT (
		    pTask->NewDomPToken (nonZeroSpanLink->ElementCount ())
		);
		nonZeroSpanLink->Close (pFragmentPPT);
		rFragmentation.createFragment (nonZeroSpanLink)->datum ().setToCorrespondence (
		    pFragmentPPT, rtLSTORE_NewStringStoreFromUV (newCharUV)
		);
		newCharUV->release ();

		zeroSpanLink->Close (pTask->NewDomPToken (zeroSpanLink->ElementCount ()));
		rFragmentation.createFragment (zeroSpanLink)->datum ().setToSubset (
		    zeroSpanLink, iRecipientStrings.monotype ()
		);	    
	    }
	}
    }
	
/*****  Undef the macros ... *****/
#undef ComputeSpanAndTotalSize
#undef HandleSpan
#undef DropString
#undef HandleDrop
}


/****************************************
 *****  Fill, Take, Drop Primitive  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to take a descriptor containing a string store and copy it.
 *
 *  Arguments:
 *	dsc	- a descriptor containing either a list of Block Strings or
 *		  a list of LStore Strings.
 *
 *  Returns:
 *	A standard CPD for the LStore string store containing the copied
 *	strings.
 *
 *****/
PrivateFnDef rtLSTORE_Handle *CopyStrings (VPrimitiveTask *pTask) {
    size_t size = 0;

#define CalcTotalSize(value)\
    size += strlen (pStrings[value]) + 1

#define Fill(value) {\
    strcpy (uvp, pStrings[value]);\
    uvp += strlen (pStrings[value]) + 1;\
}


    DSC_Descriptor &rCurrent = pTask->getCurrent ();

/*****  LStore Strings  *****/
    M_CPD *uvector = 0;

    rtLSTORE_Handle::Strings pLStoreStrings;
    rtBLOCK_Handle::Strings pBlockStrings;

    if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings

	/*****  Calculate the total number of characters ... *****/
	DSC_Traverse (rCurrent, CalcTotalSize);

	/*****  Create the new character uvector ... *****/
	uvector = pTask->NewCharUV (size);

	/*****  Fill the character uvector ... *****/
	rtCHARUV_ElementType *uvp = rtCHARUV_CPD_Array (uvector);
	DSC_Traverse (rCurrent, Fill);

#undef pStrings
    }

/*****  Block Strings  *****/
    else if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings

	/*****  Calculate the total number of characters ... *****/
	DSC_Traverse (rCurrent, CalcTotalSize);

	/*****  Create the new character uvector ... *****/
	uvector = pTask->NewCharUV (size);

	/*****  Fill the character uvector ... *****/
	rtCHARUV_ElementType *uvp = rtCHARUV_CPD_Array (uvector);
	DSC_Traverse (rCurrent, Fill);

#undef pStrings
    }

    /*****  Make the lstore ... *****/
    rtLSTORE_Handle *lstore = rtLSTORE_NewStringStoreFromUV (uvector);
    uvector->release ();

    return lstore;

#undef CalcTotalSize
#undef Fill
}


V_DefinePrimitive (StringFillTakeDrop) {
    rtLSTORE_Handle *lstore;
    M_CPD *newCharUV;


/************************************************
 *  Macros for Fill, Take, and Drop Primitives  *
 ************************************************/

#define min(a, b) (((a) < (b)) ? (a) : (b))

/*---------------------------------------------------------------------------
 * Macro to fill a new string with the contents of a source string.
 *
 *  Arguments:
 *	orig		- a 'char *' variable pointing to the source string.
 *	new		- a 'char *' variable pointing to where the new string
 *		  	  is to be placed.
 *	fillSize	- a integer whose absolute value describes the size
 *                        of the new string and whose sign describes which
 *			  end of the source string to fill from.
 *
 *  Examples:
 *	"ABC" Fill +5	-> "ABCAB"
 *	"ABC" Fill -5	-> "BCABC"
 *	"ABC" Fill  0	-> ""
 *
 *  Notes:
 *	None of the arguments will be modified by this macro.
 *
 *****/
#define FillString(orig, new, fillSize) {\
    char const *origp, *origlp;\
    char *newp;\
    int cnt;\
\
/***** If empty string, fill with blanks ... *****/\
    char const *origsp = 0 == strlen (orig) ? " " : orig;\
\
/*****  Forward Fill ... *****/\
    if (fillSize > 0) {\
	origp = origsp;\
	origlp = origsp + strlen (origsp) - 1;\
	newp = new;\
	cnt = fillSize;\
\
	while (cnt-- > 0) {\
	    *newp++ = *origp++;\
	    if (origp > origlp)\
		origp = origsp;\
	}\
	*newp = '\0';\
    }\
/***** Backwards Fill *****/\
    else {\
	newp = new + abs(fillSize);\
	origlp = origp = origsp + strlen (origsp) - 1;\
	cnt = abs(fillSize);\
\
	*newp-- = '\0';\
	while (cnt-- > 0) {\
	    *newp-- = *origp--;\
	    if (origp < origsp)\
		origp = origlp;\
	}\
    }\
}


/*---------------------------------------------------------------------------
 * Macro to fill a new string with the part of the contents of a source string.
 *
 *  Arguments:
 *	orig		- a 'char *' variable pointing to the source string.
 *	new		- a 'char *' variable pointing to where the new string
 *		  	  is to be placed.
 *	fillSize	- a integer whose absolute value describes the size
 *                        of the new string and whose sign describes which
 *			  end of the source string to 'take' from.
 *
 *  Examples:
 *	"ABC" Take +2	-> "AB"
 *	"ABC" Take -2	-> "BC"
 *	"ABC" Take  0	-> ""
 *
 *	"ABC" Take +5	-> "ABC  "
 *	"ABC" Take -5	-> "  ABC"
 *
 *  Notes:
 *	None of the arguments will be modified by this macro.
 *
 *****/
#define TakeString(orig, new, takeSize) {\
/*****  If takeSize > the size of the source string, pad with blanks ... *****/\
    size_t maxTake = min (strlen (orig), (size_t)abs (takeSize));\
\
/*****  Forward Take ... *****/\
    if (takeSize > 0) {\
	char const *origp = orig;\
	char *newp = new;\
	int remaining = takeSize - maxTake;\
\
	while (maxTake-- > 0)\
	    *newp++ = *origp++;\
\
	while (remaining-- > 0)\
	    *newp++ = ' ';\
\
	*newp = '\0';\
    }\
\
/*****  Backward Take ... *****/\
    else {\
	char const *origp = orig + strlen (orig) - 1;\
	char *newp = new + abs(takeSize);\
	int remaining = abs(takeSize) - maxTake;\
\
	*newp-- = '\0';\
	while (maxTake-- > 0)\
	    *newp-- = *origp--;\
\
	while (remaining-- > 0)\
	    *newp-- = ' ';\
    }\
}


/*---------------------------------------------------------------------------
 * Macro to fill a new string with the part of the contents of a source string.
 *
 *  Arguments:
 *	orig		- a 'char *' variable pointing to the source string.
 *	new		- a 'char *' variable pointing to where the new string
 *		  	  is to be placed.
 *	fillSize	- a integer whose absolute value describes the number
 *                        of characters to 'drop' from the source string and
 *                        whose sign describes which end of the source string
 *                        to 'drop' from.
 *
 *  Examples:
 *	"ABC" Drop +2	-> "C"
 *	"ABC" Drop -2	-> "A"
 *	"ABC" Drop  0	-> "ABC"
 *
 *	"ABC" Drop +5	-> ""
 *	"ABC" Drop -5   -> ""
 *
 *  Notes:
 *	None of the arguments will be modified by this macro.
 *
 *****/
#define DropString(orig, new, dropSize) {\
/***** dropSize cannot be > than the size of the source string ... *****/\
    size_t maxDrop = min (strlen (orig), (size_t)abs (dropSize));\
\
/*****  Forward Drop ... *****/\
    if (dropSize > 0) {\
	char const *origp = orig + maxDrop;\
	char const *origlp = orig + strlen (orig);\
	char *newp = new;\
\
	while (origp < origlp)\
	    *newp++ = *origp++;\
	*newp = '\0';\
    }\
\
/*****  Backward Drop ... *****/\
    else {\
	size_t origSize = strlen (orig);\
	char const *origp = orig + origSize - maxDrop - 1;\
	char *newp = new + origSize - min ((size_t)abs (dropSize), origSize);\
\
	*newp-- = '\0';\
	while (origp >= orig)\
	    *newp-- = *origp--;\
    }\
}


/*****  Macros which fill in the character uvector  *****/

#define HandleStringFill(value) {\
    FillString (pStrings[value], newCharp, *sizesp);\
    newCharp += (abs (*sizesp++) + 1);\
}

#define HandleStringTake(value) {\
    TakeString (pStrings[value], newCharp, *sizesp);\
    newCharp += (abs (*sizesp++) + 1);\
}

#define HandleStringDrop(value) {\
    DropString (pStrings[value], newCharp, *sizesp);\
    itemp = strlen (pStrings[value]);\
    newCharp += (itemp - min (abs (*sizesp), itemp) + 1);\
    sizesp++;\
}


/*****  Macros to determine the total size needed for the result ... *****/

#define CalcStringDropTotalSize(value) {\
    size_t origSize = strlen (pStrings[value]);\
    totalSize += (origSize - min ((size_t)abs (*sizesp), origSize) + 1);\
    sizesp++;\
}

#define CalcNewTotalSize {\
    unsigned int count;\
    switch (V_TOTSC_Primitive) {\
    case XStringFill:\
    case XStringTake:\
        totalSize = 0;\
        count = UV_CPD_ElementCount (sizesUV);\
        while (count-- > 0)\
	    totalSize += (size_t)abs(*sizesp++) + 1;\
	break;\
    case XStringDrop:\
        totalSize = 0;\
	DSC_Traverse (rCurrent, CalcStringDropTotalSize);\
	break;\
    default:\
	pTask->raiseUnimplementedAliasException ("StringFillTakeDrop");\
	break;\
    }\
}


/*
 *---------------------------------------------------------------------------
 * Code Body for 'StringFillTakeDrop' primitive
 *---------------------------------------------------------------------------
 */

/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	unsigned int converse;
	switch (V_TOTSC_Primitive) {
	case XStringFill:
	    converse = KS__FillFromString;
	    break;
	case XStringTake:
	    converse = KS__TakeFromString;
	    break;
	case XStringDrop:
	    converse = KS__DropFromString;
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("StringFillTakeDrop");
	    break;
	}
        pTask->sendBinaryConverseWithCurrent (converse);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();
    DSC_Descriptor &rDucMonotype = pTask->ducMonotype ();

/*****  Scalar Case *****/
    if (DucIsAScalar) {
	size_t origSize, totalSize;

/*****  Determine the size argument's value ...  *****/
	int sizeArg = rDucMonotype.holdsAScalarValue () && rDucMonotype.store ()->NamesTheIntegerClass ()
	    ? DSC_Descriptor_Scalar_Int (rDucMonotype) : 0;

/*****  Set a pointer to the actual string ... *****/
	char const *pString = "";
	switch (rCurrent.pointerRType ()) {
	case RTYPE_C_IntUV: {
		rtBLOCK_Handle::Strings pStrings;
		if (pStrings.setTo (rCurrent.store ()))
		    pString = pStrings[DSC_Descriptor_Scalar_Int (rCurrent)];
	    }
	    break;
	case RTYPE_C_Link: {
		rtLSTORE_Handle::Strings pStrings;
		if (pStrings.setTo (rCurrent.store ()))
		    pString = pStrings[DSC_Descriptor_Scalar_Int (rCurrent)];
	}
	    break;
	}

/*****  Create the new character uvector and fill it ... *****/
	switch (V_TOTSC_Primitive) {
	case XStringFill:
	    newCharUV = pTask->NewCharUV ((unsigned int)abs (sizeArg) + 1);
	    FillString (pString, rtCHARUV_CPD_Array (newCharUV), sizeArg);
	    break;
	case XStringTake:
	    newCharUV = pTask->NewCharUV ((unsigned int)abs (sizeArg) + 1);
	    TakeString (pString, rtCHARUV_CPD_Array (newCharUV), sizeArg);
	    break;
	case XStringDrop:
	    origSize = strlen (pString);
	    totalSize = origSize - min ((unsigned int)abs (sizeArg), origSize) + 1;
	    newCharUV = pTask->NewCharUV (totalSize);
	    DropString (pString, rtCHARUV_CPD_Array (newCharUV), sizeArg);
	    break;
	}

        lstore = rtLSTORE_NewStringStoreFromUV (newCharUV);
	newCharUV->release ();
    }

/***** Non-Scalar case *****/
    else {
	M_CPD *sizesUV;
	size_t totalSize;
	int *sizesp;
	rtCHARUV_ElementType *newCharp;


/*****  If the size argument is valid ... *****/
	if (rDucMonotype.holdsNonScalarValues () && rDucMonotype.store ()->NamesTheIntegerClass ()) {
	    int itemp;

	    /*****  Set up the size argument uvector ... *****/
	    sizesUV = DSC_Descriptor_Value (rDucMonotype);
	    sizesp  = rtINTUV_CPD_Array (sizesUV);

	    /*****  Determine the type of source strings ... *****/
	    rtBLOCK_Handle::Strings pBlockStrings;
	    rtLSTORE_Handle::Strings pLStoreStrings;
	    if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
		CalcNewTotalSize;
#undef pStrings
	    }
	    else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    CalcNewTotalSize;
#undef pStrings
	    }

	    /*****  Create the new character uvector ... *****/
	    newCharUV = pTask->NewCharUV (totalSize);
	    newCharp = rtCHARUV_CPD_Array (newCharUV);

	    /*****  fill in the new character uvector ... *****/
	    sizesp = rtINTUV_CPD_Array (sizesUV);

	    if (pBlockStrings.isSet ()) {
#define pStrings pBlockStrings
		switch (V_TOTSC_Primitive) {
		case XStringFill:
		    DSC_Traverse (rCurrent, HandleStringFill);
		    break;
		case XStringTake:
		    DSC_Traverse (rCurrent, HandleStringTake);
		    break;
		case XStringDrop:
		    DSC_Traverse (rCurrent, HandleStringDrop);
		    break;
		}
#undef pStrings
	    }
	    else {
#define pStrings pLStoreStrings
		switch (V_TOTSC_Primitive) {
		case XStringFill:
		    DSC_Traverse (rCurrent, HandleStringFill);
		    break;
		case XStringTake:
		    DSC_Traverse (rCurrent, HandleStringTake);
		    break;
		case XStringDrop:
		    DSC_Traverse (rCurrent, HandleStringDrop);
		    break;
		}
#undef pStrings
	    }

	    /*****  Make the lstore ... *****/
	    lstore = rtLSTORE_NewStringStoreFromUV (newCharUV);
	    newCharUV->release ();
	}

/***** ... the size argument is not valid ... *****/
	else switch (V_TOTSC_Primitive) {
	case XStringDrop:
	    /*****  Return the original strings ... *****/
	    lstore = CopyStrings (pTask);
	    break;

	    /* Fill & Take */
	default:
	    /*****  Return all empty strings ... *****/
	    sizesUV = DSC_Descriptor_Value (rDucMonotype);
	    totalSize = UV_CPD_ElementCount (sizesUV);

	    newCharUV = pTask->NewCharUV (totalSize);

	    memset (rtCHARUV_CPD_Array (newCharUV), '\0', totalSize);

	    lstore = rtLSTORE_NewStringStoreFromUV (newCharUV);
	    newCharUV->release ();

	    break;
	}
    }

/*****  Return the lstore in the accumulator ... *****/
    pTask->loadDucWithListOrStringStore (lstore);

/*****  Undef the macros ... *****/
#undef min
#undef FillString
#undef TakeString
#undef DropString
#undef HandleStringFill
#undef HandleStringTake
#undef HandleStringDrop
#undef FillInTheCharacterUVector
#undef CalcStringDropTotalSize
#undef CalcNewTotalSize
}


/*************************************
 *****  Concatenation Primitive  *****
 *************************************/

PrivateFnDef void PerformStringConcat (VPrimitiveTask* pTask) {

/*****  'PerformStringConcat' macros  *****/
#define CalcTotalSize(sValue, aValue) {\
    char const *pString1 = iRecipientStrings.string (sValue);\
    char const *pString2 = iParameterStrings.string (aValue);\
    totalSize += strlen (pString1) + strlen (pString2) + 1;\
}

#define FillInCharacterUV(sValue, aValue) {\
    *charp = '\0';\
\
    strcat (charp, iRecipientStrings.string (sValue));\
    strcat (charp, iParameterStrings.string (aValue));\
\
    charp += strlen (charp) + 1;\
}


/*
 *---------------------------------------------------------------------------
 *  Code Body of 'PerformStringConcat' function.
 *---------------------------------------------------------------------------
 */


/*****  Get store descriptors for the two sets of strings... *****/
    StringStoreDescriptor iRecipientStrings (pTask->getCurrent ());
    StringStoreDescriptor iParameterStrings (ADescriptor);

/*****  Scalar Case  *****/
    M_CPD *newCharUV;
    if (DucIsAScalar) {
/*****  Determine the total size of the result string ... *****/
	size_t totalSize = 0;
	CalcTotalSize (
	    DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()),
	    DSC_Descriptor_Scalar_Int (iParameterStrings.monotype ())
	);

/*****  Make a character uvector for the result ... *****/
	newCharUV = pTask->NewCharUV (totalSize);

/*****  Fill in the character uvector ... *****/
	rtCHARUV_ElementType *charp = rtCHARUV_CPD_Array (newCharUV);

	FillInCharacterUV (
	    DSC_Descriptor_Scalar_Int (iRecipientStrings.monotype ()),
	    DSC_Descriptor_Scalar_Int (iParameterStrings.monotype ())
	);

/*****  Make a LStore ... *****/
    }

/*****  Non-scalar case  *****/
    else {
/*****  Determine the total size of the result strings ... *****/
	size_t totalSize = 0;
	DSC_JointTraversal (
	    iRecipientStrings.monotype (), iParameterStrings.monotype (), CalcTotalSize
	);

/*****  Make a character uvector for the result ... *****/
	newCharUV = pTask->NewCharUV (totalSize);

/*****  Fill in the character uvector ... *****/
	rtCHARUV_ElementType *charp = rtCHARUV_CPD_Array (newCharUV);

	DSC_JointTraversal (
	    iRecipientStrings.monotype (), iParameterStrings.monotype (), FillInCharacterUV
	);
    }

/*****  Return the LStore in the accumulator ... *****/
    pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (newCharUV));
    newCharUV->release ();

/*****  Cleanup ... *****/
#undef CalcTotalSize
#undef FillInCharacterUV
}


V_DefinePrimitive (StringConcat) {
/*****  Acquire the operand...  *****/
/*****
 *  Send the binary converse message of this primitive if the operand is
 *  polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (KS__ReverseConcat);
	return;
    }

/*****  Otherwise, acquire both operands of the binary...  *****/
    pTask->normalizeDuc ();

/***** If the argument is a (collection of) string(s) ... *****/
    if (ParameterReferencesAStringStore (pTask))
	PerformStringConcat (pTask);
    else
	pTask->loadDucWithCurrent ();
}


V_DefinePrimitive (GetEnv) {
/*****  'GetEnv' macros  *****/
#define CalcTotalSize(sValue) {\
    char const *pVarValue = getenv (iEnvironmentVariables.string (sValue));\
    sTotalLengthOfStrings += pVarValue ? strlen (pVarValue) + 1 : 0;\
}

#define FillInCharacterUV(sValue) {\
    *pResult = '\0';\
    char const *pVarValue = getenv (iEnvironmentVariables.string (sValue));\
    if (pVarValue) {\
	strcpy (pResult, pVarValue);\
	pResult += strlen (pResult) + 1;\
    } else\
        StringPrimsNaHandler (sTotalLengthOfStrings);\
    StringPrimsCount++;\
}


/*
 *---------------------------------------------------------------
 *  Code Body of 'GetEnv' function.
 *---------------------------------------------------------------
 */


/*****  Get store descriptor for the environment variable(s)... *****/
    StringStoreDescriptor iEnvironmentVariables (pTask->getCurrent ());
    StringPrimsInitialize();

/*****  Scalar Case  *****/
    if (StringPrimsScalarCase = (DucIsAScalar)) {
/*****  Determine the total size of the result string ... *****/
	size_t sTotalLengthOfStrings = 0;
	CalcTotalSize (
	    DSC_Descriptor_Scalar_Int (iEnvironmentVariables.monotype ())
	);
	if (0 < sTotalLengthOfStrings) {
/*****  Make a character uvector for the result ... *****/
	    M_CPD *pCharUV = pTask->NewCharUV (sTotalLengthOfStrings);

/*****  Fill in the character uvector ... *****/
	    rtCHARUV_ElementType *pResult = rtCHARUV_CPD_Array (pCharUV);

	    FillInCharacterUV (
		DSC_Descriptor_Scalar_Int (iEnvironmentVariables.monotype ())
	    );
	    pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (pCharUV));
	    pCharUV->release ();
	} else
	    pTask->loadDucWithNA ();
    }

/*****  Non-scalar case  *****/
    else {
/*****  Determine the total size of the result strings ... *****/
	size_t sTotalLengthOfStrings = 0;
	DSC_Traverse (
	    iEnvironmentVariables.monotype (), CalcTotalSize
	);

	if (0 < sTotalLengthOfStrings) {
/*****  Make a character uvector for the result ... *****/
	    M_CPD *pCharUV = pTask->NewCharUV (sTotalLengthOfStrings);

/*****  Fill in the character uvector ... *****/
	    rtCHARUV_ElementType *pResult = rtCHARUV_CPD_Array (pCharUV);

	    DSC_Traverse (
		iEnvironmentVariables.monotype (), FillInCharacterUV
	    );
/*****  Close the linkc if necessary ... *****/
	    StringPrimsFinishAndCloseLinkc ();

/*****  Return the LStore in the accumulator ... *****/
	    pTask->loadDucWithPartialCorrespondence (rtLSTORE_NewStringStoreFromUV (pCharUV), StringPrimsGoodLinkc);
	    pCharUV->release ();
	    if (StringPrimsGoodLinkc) StringPrimsGoodLinkc->release();
	} else
	    pTask->loadDucWithNA ();
    }


/*****  Cleanup ... *****/
#undef CalcTotalSize
#undef FillInCharacterUV
}


/************************************
 ************************************
 *****  Primitive Descriptions  *****
 ************************************
 ************************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that describe the primitives to the virtual machine and
 * debugger.
 *---------------------------------------------------------------------------
 */

BeginDescriptions


/*******************************
 *****  String Primitives  *****
 *******************************/

 
    PD (XStringStrip,
	"StringStrip",
	StringStrip)


    PD (XStringSize,
	"StringSize",
	StringSize)
    PD (XEncodeStringAsDouble,
	"EncodeStringAsDouble",
	EncodeStringAsDouble)
    PD (XToLStoreString,
	"ToLStoreString",
	ToLStoreString)
    PD (XStringFill,
	"StringFill",
	StringFillTakeDrop)
    PD (XStringTake,
	"StringTake",
	StringFillTakeDrop)
    PD (XStringDrop,
	"StringDrop",
	StringFillTakeDrop)
    PD (XStringConcat,
	"StringConcat",
	StringConcat)
    PD (XToDouble,
	"ToDouble",
	ToDouble)
    PD (XStringReverse,
	"StringReverse",
	StringReverse)
    PD (XStringProcessor,
	"StringProcessor",
	StringReverse)

    PD (XStringLessThan,
	"StringLessThan",
	CompareStrings)
    PD (XStringLessThanOrEqual,
	"StringLessThanOrEqual",
	CompareStrings)
    PD (XStringEqual,
	"StringEqual",
	CompareStrings)
    PD (XStringNotEqual,
	"StringNotEqual",
	CompareStrings)
    PD (XStringGreaterThanOrEqual,
	"StringGreaterThanOrEqual",
	CompareStrings)
    PD (XStringGreaterThan,
	"StringGreaterThan",
	CompareStrings)

    PD (XStringMatch,
	"StringMatch",
	StringSearch)

    PD (XStringSpan,
	"StringSpan",
	StringSpans)
    PD (XStringCSpan,
	"StringCSpan",
	StringSpans)

    PD (XStringBreak,
	"StringBreak",
	StringBreak)
    PD (XStringCBreak,
	"StringCBreak",
	StringBreak)

    PD (XFindSubstring,
	"SubstringOrigin",
	StringSearch)
    PD (XFindPatternOrigin,
	"PatternOrigin",
	StringSearch)
    PD (XFindPatternExtent,
	"PatternExtent",
	StringSearch)

    PD (XStringBreakOnString,
	"StringBreakOnString",
	StringBreak)
    PD (XStringBreakOnPattern,
	"StringBreakOnPattern",
	StringBreak)
    PD (XGetEnv,
	"GetEnv",
	GetEnv)

EndDescriptions


/*******************************************************
 *****  The Primitive Dispatch Vector Initializer  *****
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a portion of the primitive function
 *****	dispatch vector at system startup.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void pfSTRING_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfSTRING);
    FAC_FDFCase_FacilityDescription ("String Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFstring.c 1 replace /users/jad/system
  861116 16:06:29 jad new string primitive functions

 ************************************************************************/
/************************************************************************
  PFstring.c 2 replace /users/jad/system
  870119 12:54:35 jad implemented lstore strings

 ************************************************************************/
/************************************************************************
  PFstring.c 3 replace /users/jad/system
  870123 16:11:50 jad changed the newString primitive to use the new
newStringStore function in RTlstore.c

 ************************************************************************/
/************************************************************************
  PFstring.c 4 replace /users/jad/system
  870210 14:21:44 jad added fill, take, & drop primitives

 ************************************************************************/
/************************************************************************
  PFstring.c 5 replace /users/jad/system
  870210 17:48:41 jad simplified printof primitives to use some new vamchine routines

 ************************************************************************/
/************************************************************************
  PFstring.c 6 replace /users/jad/system
  870211 16:30:42 jad implemented concat primitive

 ************************************************************************/
/************************************************************************
  PFstring.c 7 replace /users/jad/system
  870218 13:55:41 jad fixed bugs in StringFillTakeDrop primitive

 ************************************************************************/
/************************************************************************
  PFstring.c 8 replace /users/mjc/translation
  870524 09:31:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFstring.c 9 replace /users/mjc/system
  870701 22:37:13 mjc Changed 'V_SendBinaryConverse' to 'V_SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  PFstring.c 10 replace /users/jad/system
  880321 10:47:28 jad implemented 'toDouble' primitive

 ************************************************************************/
/************************************************************************
  PFstring.c 11 replace /users/jad/system
  880321 16:22:14 jad implement testdeck tracing

 ************************************************************************/

/*****  Descriptor Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName DSC

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Foundation  *****/
#include "m.h"

#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

/*****  Self  *****/
#include "vdsc.h"

/*****  Supporting  *****/
#include "RTclosure.h"
#include "RTcontext.h"

#include "RTlink.h"
#include "RTptoken.h"
#include "RTvstore.h"

#include "uvector.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

#include "RTlstore.h"


/******************************************
 ******************************************
 *****  Macro Identification Strings  *****
 ******************************************
 ******************************************/
/*---------------------------------------------------------------------------
 * The strings declared in this section provide strings which can be passed to
 * the descriptor pointer type complaint routines that follow.  Use of these
 * strings eliminates the duplication of string storage for each expansion of
 * the macro.
 *---------------------------------------------------------------------------
 */
PrivateVarDef char const
    *DSC_N_PointerCPD		= "DSC_Pointer::pointerCPD ()",
    *DSC_N_PointerRType		= "DSC_Pointer::pointerRType ()",
    *DSC_N_DistributePointer	= "DSC_Pointer::distribute(M_CPD*)",
    *DSC_N_LCExtractFromPointer	= "DSC_LCExtractFromPointer",
    *DSC_N_UVExtractFromPointer	= "DSC_UVExtractFromPointer",
    *DSC_N_RefExtractFromPointer= "DSC_RefExtractFromPointer",
    *DSC_N_PositionalPTOfPtr	= "DSC_PositionalPTokenOfPointer",
    *DSC_N_ReferentialPTOfPtr	= "DSC_ReferentialPTokenOfPointer",
    *DSC_N_InitSetPointer	= "DSC_InitSetPointer",
    *DSC_N_LCAssignIntoUVFromPtr= "DSC_LCAssignIntoUVFromPointer";


/**************************************
 **************************************
 *****  'Store' Support Routines  *****
 **************************************
 **************************************/

/*******************
 *****  Clear  *****
 *******************/

void DSC_Store::clear () {
/*** Deallocate the contents... ***/
    if (m_pPO)
        m_pPO->release ();
    if (m_pCPD)
        m_pCPD->release ();

/*** ... and set the fields to Nil ... ***/
    construct ();
}


/******************************
 *****  Closure Decoding  *****
 ******************************/

void DSC_Store::decodeClosure (
    rtBLOCK_Handle::Reference &rpBlock, unsigned int &rxPrimitive, rtCONTEXT_Constructor **ppContext
) const {
    rtCLOSURE_Constructor* pConstructor = (rtCLOSURE_Constructor*)m_pPO;
    if (pConstructor) {
	rpBlock.setTo (pConstructor->block ());
	rxPrimitive = pConstructor->primitive ();
	if (ppContext) {
	    *ppContext = pConstructor->context ();
	    (*ppContext)->retain ();
	}
    }
    else {
	rpBlock.setTo (
	    rtCLOSURE_CPD_IsABlockClosure (m_pCPD) ? static_cast <rtBLOCK_Handle*> (
		m_pCPD->GetContainerHandle (rtCLOSURE_CPx_Block, RTYPE_C_Block)
	    ) : 0
	);
	rxPrimitive = rtCLOSURE_CPD_Primitive (m_pCPD);
	if (ppContext) {
	    M_CPD *pContextContainer = rtCLOSURE_CPD_ContextCPD (m_pCPD);
#if 0
	    *ppContext = (rtCONTEXT_Constructor*)pContextContainer->TransientExtensionIfA (
		rtCONTEXT_Constructor::RTT
	    );
	    if (*ppContext)
		(*ppContext)->retain ();
	    else
		*ppContext = new rtCONTEXT_Constructor (pContextContainer);
#else
	    *ppContext = new rtCONTEXT_Constructor (pContextContainer);
#endif
	    pContextContainer->release ();
	}
    }
}


/************************************
 *****  Canonical Store Access  *****
 ************************************/

void DSC_Store::getCanonicalStore_(
    M_CPD *&rpCanonicalStore, bool &rbConvertPointer, DSC_Pointer const &rPointer
) const {
    RTYPE_Type xRType = RType ();
    switch (xRType) {
    case RTYPE_C_ValueStore:
	rpCanonicalStore = m_pCPD;
	rbConvertPointer = false;
	break;
    case RTYPE_C_ListStore:
	rpCanonicalStore = rtLSTORE_CPD_StringStore (m_pCPD)
	    ? m_pCPD->TheStringClass	()
	    : m_pCPD->TheListClass	();
	rbConvertPointer = true;
	break;
    case RTYPE_C_Index:
	rpCanonicalStore = rtINDEX_CPD_IsATimeSeries (m_pCPD)
	    ? m_pCPD->TheTimeSeriesClass ()
	    : m_pCPD->TheIndexedListClass();
	rbConvertPointer = true;
	break;
    case RTYPE_C_Dictionary:
    case RTYPE_C_Vector:
	rpCanonicalStore = KOT ()->TheFixedPropertyClass;
	rbConvertPointer = true;
	break;
    case RTYPE_C_Closure:
	rpCanonicalStore = KOT ()->TheClosureClass;
	rbConvertPointer = true;
	break;
    case RTYPE_C_Method:
	rpCanonicalStore = KOT ()->TheMethodClass;
	rbConvertPointer = true;
	break;
    case RTYPE_C_Block: {
	    M_CPD* pRPTRef; int xRPTRef;
	    rPointer.getRPTReference (pRPTRef, xRPTRef);
	    rpCanonicalStore = pRPTRef->GetBlockEquivalentClassFromPToken (xRPTRef);
	    rbConvertPointer = true;
	    pRPTRef->release ();
	}
	break;
    default:
	rbConvertPointer = false;
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"DSC_Store::getCanonicalStore: Cannot Canonicalize Stores of Type %s",
		RTYPE_TypeIdAsString (xRType)
	    )
	);
	break;
    }
}


/*******************************
 *****  Dictionary Access  *****
 *******************************/

M_CPD *DSC_Store::dictionaryCPD (DSC_Pointer const &rPointer) const {
    M_CPD *pDictionary;

    RTYPE_Type xRType = RType ();
    switch (xRType) {
    case RTYPE_C_ValueStore:
        return rtVSTORE_CPD_DictionaryCPD (m_pCPD);

    case RTYPE_C_Index:
	pDictionary = rtINDEX_CPD_IsATimeSeries (m_pCPD)
	    ? m_pCPD->TheTimeSeriesClassDictionary ()
	    : m_pCPD->TheIndexedListClassDictionary();
        break;

    case RTYPE_C_ListStore:
	pDictionary = rtLSTORE_CPD_StringStore (m_pCPD)
	    ? m_pCPD->TheStringClassDictionary	()
	    : m_pCPD->TheListClassDictionary	();
        break;

    case RTYPE_C_Dictionary:
    case RTYPE_C_Vector:
	pDictionary = KOT()->TheFixedPropertyClassDictionary;
	break;

    case RTYPE_C_Closure:
        pDictionary = KOT()->TheClosureClassDictionary;
        break;

    case RTYPE_C_Method:
        pDictionary = KOT()->TheMethodClassDictionary;
        break;

    case RTYPE_C_Block: {
	    M_CPD *pRPTRef; int xRPTRef;
	    rPointer.getRPTReference (pRPTRef, xRPTRef);

	    M_KOT *pKOT = pRPTRef->ReferencedKOT (xRPTRef);
	    pDictionary = pRPTRef->ReferenceNames (
		xRPTRef, pKOT->TheStringPTokenCPD ()
	    ) ? pKOT->TheStringClassDictionary : pKOT->TheSelectorClassDictionary;

	    pRPTRef->release ();
	}
        break;

    default:
        ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"DSC_Store::dictionaryCPD: '%s's do not have dictionaries",
		RTYPE_TypeIdAsString (xRType)
	    )
	);
        return NilOf (M_CPD*);
    }

    pDictionary->retain ();

    return pDictionary;
}


/****************************************
 ****************************************
 *****  'Pointer' Support Routines  *****
 ****************************************
 ****************************************/

void DSC_Value::raiseTypeException (DSC_Value::ExceptionSource xSource) const {
    char const *pSourceName;
    switch (xSource) {
    case ExceptionSource_ConstructComposition:
	pSourceName = "DSC_Value::constructComposition";
	break;
    default:
	pSourceName = "DSC_Value::<Unknown>";
	break;
    }

    ERR_SignalFault (
	EC__UnimplementedCase, UTIL_FormatMessage (
	    "%s: Unsupported R-Type %s", pSourceName, m_pValues->RTypeName ()
	)
    );
}

void DSC_Reference::raiseMissingComponentException (
    DSC_Reference::ExceptionSource xSource
) const {
    char const *pMessage;
    switch (xSource) {
    case ExceptionSource_Distribute:
	pMessage = "DSC_Reference::distribute: Missing UVector";
	break;
    case ExceptionSource_Factor:
	pMessage = "DSC_Reference::factor: Missing UVector";
	break;
    case ExceptionSource_GetPPTReference:
	pMessage = "DSC_Reference::getPPTReference: Missing UVector";
	break;
    case ExceptionSource_GetRPTReference:
	pMessage = "DSC_Reference::getRPTReference: Missing UVector";
	break;
    case ExceptionSource_RealizeValues:
	pMessage = "DSC_Reference::RealizeValues: Missing UVector";
	break;
    default:
	pMessage = "DSC_Reference::Missing UVector";
	break;
    }

    ERR_SignalFault (EC__InternalInconsistency, pMessage);
}

/****************************************************************
 *****  Unexpected / Empty Pointer Type Complaint Routines  *****
 ****************************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to complain about an unknown pointer type.
 *
 *  Argument:
 *	pointer			- the address of the pointer whose type is
 *				  unrecognizable.
 *	text			- a text string specifying who's complaining.
 *
 *  Returns:
 *	This routine never returns.
 *
 *****/
void DSC_Pointer::complainAboutBadPointerType (char const *pText) const {
    ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "Unexpected Descriptor Pointer Type '%d' Found By '%s'", m_xType, pText
	)
    );
}

/*---------------------------------------------------------------------------
 *****  Routine to complain about an empty pointer type.
 *
 *  Argument:
 *	text			- a text string specifying who's complaining.
 *
 *  Returns:
 *	This routine never returns.
 *
 *****/
PublicFnDef void DSC__ComplainAboutEmptyPtrType (char const* pText) {
    ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage ("Empty Descriptor Pointer Found By '%s'", pText)
    );
}


/**********************
 **********************
 *****  Pointers  *****
 **********************
 **********************/

/*********************
 *****  SCALARS  *****
 *********************/

/***************************
 *  Scalar Initialization  *
 ***************************/

void DSC_Scalar::constructComposition (unsigned int xSubscript, M_CPD *pSource) {
/*****  Fill out the fields of the descriptor...  *****/
    DSC_InitScalar (*this, NilOf (M_CPD*), pSource->RType (), DSC_Scalar_Int, 0);

/*****  ... and fill in the value ... *****/
    switch (m_xRType) {
    case RTYPE_C_CharUV:
	rtCHARUV_Align (pSource);
	DSC_Scalar_Char (*this) = rtCHARUV_CPD_Array (pSource)[xSubscript];
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Align (pSource);
	DSC_Scalar_Float (*this) = rtFLOATUV_CPD_Array (pSource)[xSubscript];
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Align (pSource);
	DSC_Scalar_Double (*this) = reinterpret_cast<double UNALIGNED*>(rtDOUBLEUV_CPD_Array(pSource))[xSubscript];
	break;
    case RTYPE_C_IntUV:
	rtINTUV_Align (pSource);
	DSC_Scalar_Int (*this) = rtINTUV_CPD_Array (pSource)[xSubscript];
	break;
    case RTYPE_C_RefUV:
	rtREFUV_Align (pSource);
	DSC_Scalar_Int (*this) = xSubscript >= UV_CPD_ElementCount (pSource)
	    ? rtPTOKEN_BaseElementCount (pSource, UV_CPx_RefPToken)
	    : UV_CPD_Array (pSource, unsigned int)[xSubscript];
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_Align (pSource);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Align (pSource);
	DSC_Scalar_Unsigned64 (*this) = rtU64UV_CPD_Array (pSource)[xSubscript];
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Align (pSource);
	DSC_Scalar_Unsigned96 (*this) = rtU96UV_CPD_Array (pSource)[xSubscript];
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Align (pSource);
	DSC_Scalar_Unsigned128 (*this) = rtU128UV_CPD_Array (pSource)[xSubscript];
	break;
    default:
        ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_InitScalarWithUVElement: Unsupported R-Type %s", pSource->RTypeName ()
	    )
	);
	break;
    }
    m_pRPT = UV_CPD_RefPTokenCPD (pSource);
}


/*******************************
 *  Scalar Container Creation  *
 *******************************/

M_CPD *DSC_Scalar::asContainer (M_CPD *pPPT) {
    M_CPD *cpd;

    if (!pPPT)
	pPPT = M_AttachedNetwork->TheScalarPToken ();
    else if (rtPTOKEN_CPD_BaseElementCount (pPPT) != 1) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "DSC_Scalar::asContainer: Cardinality disagreement: Current:1, Proposed:%u.",
	    rtPTOKEN_CPD_BaseElementCount (pPPT)
	)
    );

    switch (m_xRType) {
    case RTYPE_C_CharUV:
	cpd = rtCHARUV_New (pPPT, m_pRPT);
	*rtCHARUV_CPD_Array (cpd) = DSC_Scalar_Char (*this);
	break;
    case RTYPE_C_FloatUV:
	cpd = rtFLOATUV_New (pPPT, m_pRPT);
	*rtFLOATUV_CPD_Array (cpd) = DSC_Scalar_Float (*this);
	break;
    case RTYPE_C_DoubleUV:
	cpd = rtDOUBLEUV_New (pPPT, m_pRPT);
	*rtDOUBLEUV_CPD_Array (cpd) = DSC_Scalar_Double (*this);
	break;
    case RTYPE_C_IntUV:
	cpd = rtINTUV_New (pPPT, m_pRPT);
	*rtINTUV_CPD_Array (cpd) = DSC_Scalar_Int (*this);
	break;
    case RTYPE_C_RefUV: {
	    rtREFUV_AlignReference (this);
	    rtLINK_CType *linkc = rtLINK_RefConstructor (m_pRPT, -1);
	    rtLINK_AppendRange (linkc, DSC_Scalar_Int (*this), 1);
	    linkc->Close (pPPT);
	    cpd = linkc->ToLink ();
	}
	break;
    case RTYPE_C_UndefUV:
	cpd = rtUNDEFUV_New (pPPT, m_pRPT);
	break;
    case RTYPE_C_Unsigned64UV:
	cpd = rtU64UV_New (pPPT, m_pRPT);
	*rtU64UV_CPD_Array (cpd) = DSC_Scalar_Unsigned64 (*this);
	break;
    case RTYPE_C_Unsigned96UV:
	cpd = rtU96UV_New (pPPT, m_pRPT);
	*rtU96UV_CPD_Array (cpd) = DSC_Scalar_Unsigned96 (*this);
	break;
    case RTYPE_C_Unsigned128UV:
	cpd = rtU128UV_New (pPPT, m_pRPT);
	*rtU128UV_CPD_Array (cpd) = DSC_Scalar_Unsigned128 (*this);
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_Scalar::asContainer: Unsupported R-Type %s",
		RTYPE_TypeIdAsString (m_xRType)
	    )
	);
    }

    return cpd;
}


M_CPD *DSC_Scalar::asCoercedContainer (M_CPD *pPPT) {
    M_CPD*		resultCPD;
    rtLINK_CType*	linkc;
    char		*cptr;
    float		*fptr;
    double		*dptr;
    int			*iptr;
    VkUnsigned64	*u64ptr;
    VkUnsigned96	*u96ptr;
    VkUnsigned128	*u128ptr;

    unsigned int i;
    unsigned int size = rtPTOKEN_CPD_BaseElementCount (pPPT);

    switch (m_xRType) {
    case RTYPE_C_CharUV:
	resultCPD = rtCHARUV_New (pPPT, m_pRPT);
	cptr = rtCHARUV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *cptr++ = DSC_Scalar_Char (*this);
	break;
    case RTYPE_C_FloatUV:
	resultCPD = rtFLOATUV_New (pPPT, m_pRPT);
	fptr = rtFLOATUV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *fptr++ = DSC_Scalar_Float (*this);
	break;
    case RTYPE_C_DoubleUV:
	resultCPD = rtDOUBLEUV_New (pPPT, m_pRPT);
	dptr = rtDOUBLEUV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *dptr++ = DSC_Scalar_Double (*this);
	break;
    case RTYPE_C_IntUV:
	resultCPD = rtINTUV_New (pPPT, m_pRPT);
	iptr = rtINTUV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *iptr++ = DSC_Scalar_Int (*this);
	break;
    case RTYPE_C_RefUV:
	rtREFUV_AlignReference (this);
	linkc = rtLINK_PosConstructor (pPPT, -1);
	rtLINK_AppendRepeat (linkc, DSC_Scalar_Int (*this), size);
        linkc->Close (m_pRPT);
	resultCPD = linkc->ToLink ();
	break;
    case RTYPE_C_UndefUV:
	resultCPD = rtUNDEFUV_New (pPPT, m_pRPT);
	break;
    case RTYPE_C_Unsigned64UV:
	resultCPD = rtU64UV_New (pPPT, m_pRPT);
	u64ptr = rtU64UV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *u64ptr++ = DSC_Scalar_Unsigned64 (*this);
	break;
    case RTYPE_C_Unsigned96UV:
	resultCPD = rtU96UV_New (pPPT, m_pRPT);
	u96ptr = rtU96UV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *u96ptr++ = DSC_Scalar_Unsigned96 (*this);
	break;
    case RTYPE_C_Unsigned128UV:
	resultCPD = rtU128UV_New (pPPT, m_pRPT);
	u128ptr = rtU128UV_CPD_Array (resultCPD);
	for (i=0; i<size; i++)
	    *u128ptr++ = DSC_Scalar_Unsigned128 (*this);
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_Scalar::asCoercedContainer:: Unsupported R-Type %s",
		RTYPE_TypeIdAsString (m_xRType)
	    )
	);
    }

    return resultCPD;
}


M_CPD *DSC_Scalar::asUVector () {
    if (holdsAValue ())
	return asContainer (M_AttachedNetwork->TheScalarPToken ());

    rtREFUV_AlignReference (this);
    M_CPD *pResult = rtREFUV_New (M_AttachedNetwork->TheScalarPToken (), m_pRPT);
    *rtREFUV_CPD_Array (pResult) = DSC_Scalar_Int (*this);
    return pResult;
}


/******************
 *  Scalar Query  *
 ******************/

bool DSC_Scalar::holdsANil () const {
    return (unsigned int)DSC_Scalar_Int (*this) == rtPTOKEN_CPD_BaseElementCount (m_pRPT);
}


/********************
 *****  VALUES  *****
 ********************/

/*********************************
 *  Is-Coerced-Scalar Predicate  *
 *********************************/

bool DSC_Value::isACoercedScalar () const {
    bool result;

    switch (m_pValues->RType ()) {
    case RTYPE_C_CharUV:
    	rtCHARUV_Align (m_pValues); {
	    rtCHARUV_ElementType const
	    	*sp = rtCHARUV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_FloatUV:
    	rtFLOATUV_Align (m_pValues); {
	    rtFLOATUV_ElementType const
	    	*sp = rtFLOATUV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_DoubleUV:
    	rtDOUBLEUV_Align (m_pValues); {
	    rtDOUBLEUV_ElementType const
	    	*sp = rtDOUBLEUV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_IntUV:
    	rtINTUV_Align (m_pValues); {
	    rtINTUV_ElementType const
	    	*sp = rtINTUV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_UndefUV:
	result = true;
	break;

    case RTYPE_C_Unsigned64UV:
    	rtU64UV_Align (m_pValues); {
	    rtU64UV_ElementType const
	    	*sp = rtU64UV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_Unsigned96UV:
    	rtU96UV_Align (m_pValues); {
	    rtU96UV_ElementType const
	    	*sp = rtU96UV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    case RTYPE_C_Unsigned128UV:
    	rtU128UV_Align (m_pValues); {
	    rtU128UV_ElementType const
	    	*sp = rtU128UV_CPD_Array (m_pValues),
	    	*sl = sp + UV_CPD_ElementCount (m_pValues),
	    	num = *sp;

	/*** if the uvector is empty return false ... *****/
	    result = (sp != sl);

	/*** return true unless all of the values are not the same ... ***/
	    while (sp < sl) {
	    	if (*sp++ != num) {
		    result = false;
		    break;
	    	}
	    }
    	}
	break;

    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_ValueIsCoercedScalar: Unsupported R-Type %s", m_pValues->RTypeName ()
	    )
	);
	break;
    }

    return result;
}

bool DSC_Pointer::isACoercedScalar () const {
    switch (m_xType) {
    case DSC_PointerType_Empty:
    case DSC_PointerType_Scalar:
    case DSC_PointerType_Identity:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference:
	break;

    case DSC_PointerType_Value:
	return m_iContent.as_iValue.isACoercedScalar ();

    default:
	complainAboutBadPointerType ("DSC_IsCoercedScalarPointer");
	break;
    }

    return false;
}


/**********************
 **********************
 *****  POINTERS  *****
 **********************
 **********************/

/**************************
 *****  Array Access  *****
 **************************/

void const *DSC_Pointer::valueArrayOfType (RTYPE_Type const xExpectedType) const {
    static char const* const g_pErrorSourceIdentification = "DSC_Pointer::valueArrayOfType";

    RTYPE_Type xActualType;
    void const* pValueArray;

    switch (m_xType) {
    case DSC_PointerType_Scalar: {
	    DSC_Scalar const& rScalar = m_iContent.as_iScalar;
	    xActualType = DSC_ScalarRType (rScalar);
	    pValueArray = (void const*)&DSC_Scalar_Value (rScalar);
	}
	break;

    case DSC_PointerType_Value: {
	    M_CPD *pUVector = m_iContent.as_iValue;
	    xActualType = pUVector->RType ();
	    pValueArray = UV_CPD_Array (pUVector, void const);
	}
	break;

    default:
	complainAboutBadPointerType (g_pErrorSourceIdentification);
	xActualType = RTYPE_C_Undefined;
	pValueArray = 0;
	break;
    }

    if (xExpectedType != xActualType) {
	RTYPE_ComplainAboutType (g_pErrorSourceIdentification , xActualType, xExpectedType);
    }

    return pValueArray;
}


/********************************
 *****  Container Creation  *****
 ********************************/

M_CPD *DSC_Pointer::pointerCPD (M_CPD *pPPT) {
    switch (m_xType) {
    default:
        complainAboutBadPointerType (DSC_N_PointerCPD);
        break;

    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_PointerCPD);
        break;

    case DSC_PointerType_Scalar:
        return m_iContent.as_iScalar.asContainer (pPPT);

    case DSC_PointerType_Value:
        return m_iContent.as_iValue.asContainer (pPPT);

    case DSC_PointerType_Identity:
        return m_iContent.as_iIdentity.asContainer (pPPT);

    case DSC_PointerType_Link:
        return m_iContent.as_iLink.asContainer (pPPT);

    case DSC_PointerType_Reference:
	return m_iContent.as_iReference.asContainer (pPPT);
    }

    return NilOf (M_CPD *);
}

bool DSC_Pointer::getUVector (M_CPD *&rpUVector) {
    static char const *RoutineName = "DSC_Pointer::getUVector";

    switch (m_xType) {
    default:
	complainAboutBadPointerType (RoutineName);
	break;

    case DSC_PointerType_Empty:
	DSC__ComplainAboutEmptyPtrType (RoutineName);
	break;

    case DSC_PointerType_Scalar:
	rpUVector = m_iContent.as_iScalar.asUVector ();
	return true;

    case DSC_PointerType_Value:
	rpUVector = m_iContent.as_iValue;
	return false;

    case DSC_PointerType_Identity:
	rpUVector = m_iContent.as_iIdentity.asUVector ();
	return true;

    case DSC_PointerType_Link:
	rpUVector = m_iContent.as_iLink.asUVector ();
	return true;

    case DSC_PointerType_Reference:
	rpUVector = m_iContent.as_iReference;
	return false;
    }

    rpUVector = NilOf (M_CPD *);
    return false;
}


/*************************
 ***** RType Access  *****
 *************************/

RTYPE_Type DSC_Pointer::pointerRType () const {
    switch (m_xType) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_PointerRType);
        break;
    case DSC_PointerType_Scalar:
        return DSC_ScalarRType (m_iContent.as_iScalar);
    case DSC_PointerType_Value:
        return m_iContent.as_iValue.RType ();
    case DSC_PointerType_Identity:
	return m_iContent.as_iIdentity.RType ();
    case DSC_PointerType_Link:
	return m_iContent.as_iLink.RType ();
    case DSC_PointerType_Reference:
	return m_iContent.as_iReference.RType ();
    default:
        complainAboutBadPointerType (DSC_N_PointerRType);
        break;
    }

    return RTYPE_C_Undefined;
}


/****************************
 *****  P-Token Access  *****
 ****************************/

void DSC_Pointer::getPPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
    switch (m_xType) {
    case DSC_PointerType_Empty:
	DSC__ComplainAboutEmptyPtrType (DSC_N_PositionalPTOfPtr);
	break;
    case DSC_PointerType_Scalar:
	DSC_PositionalPTokenOfScalar (
	    m_iContent.as_iScalar, pPTokenRef, xPTokenRef
	);
	break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.getPPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Identity:
	m_iContent.as_iIdentity.getPPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Link:
	m_iContent.as_iLink.getPPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.getPPTReference (pPTokenRef, xPTokenRef);
	break;
    default:
	complainAboutBadPointerType (DSC_N_PositionalPTOfPtr);
	break;
    }
}


void DSC_Pointer::getRPTReference (M_CPD *&pPTokenRef, int &xPTokenRef) const {
    switch (m_xType) {
    case DSC_PointerType_Empty:
	DSC__ComplainAboutEmptyPtrType (DSC_N_ReferentialPTOfPtr);
	break;
    case DSC_PointerType_Scalar:
	DSC_ReferentialPTokenOfScalar (
	    m_iContent.as_iScalar, pPTokenRef, xPTokenRef
	);
	break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.getRPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Identity:
	m_iContent.as_iIdentity.getRPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Link:
	m_iContent.as_iLink.getRPTReference (pPTokenRef, xPTokenRef);
	break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.getRPTReference (pPTokenRef, xPTokenRef);
	break;
    default:
	complainAboutBadPointerType (DSC_N_ReferentialPTOfPtr);
	break;
    }
}


/**************************
 *****  Construction  *****
 **************************/

void DSC_Pointer::construct (M_CPD *pMonotype) {
    RTYPE_Type rtype = pMonotype->RType ();
    switch (rtype) {
    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
/*****  If the positional ptoken is the scalar ptoken... *****/
	if (pMonotype->ReferenceNames (UV_CPx_PToken, &M_KnownObjectTable::TheScalarPToken)) {
/*****  ... then create a scalar pointer  *****/
	    constructScalarComposition (0, pMonotype);
	}
	else if (rtype == RTYPE_C_RefUV) {
/*****  ... else create a reference pointer  *****/
	    pMonotype->retain ();
	    constructReference (pMonotype);
	}
	else {
/*****  ... else create a value pointer  *****/
	    pMonotype->retain ();
	    constructValue (pMonotype);
	}
	break;

    case RTYPE_C_Link:
	rtLINK_AlignLink (pMonotype);

/*****  ... If its positional ptoken is the scalar ptoken ... *****/
	if (pMonotype->ReferenceNames (rtLINK_CPx_PosPToken, &M_KnownObjectTable::TheScalarPToken)) {
/*****  ... then create a scalar reference pointer  *****/
	    constructReferenceScalar (
		rtLINK_CPD_RefPTokenCPD (pMonotype),
		rtLINK_RRD_ReferenceOrigin (rtLINK_CPD_RRDArray (pMonotype))
	    );
	}
	else {
/*****  ... else create a link pointer  *****/
	    pMonotype->retain ();
	    constructLink (pMonotype);
	}
	break;

    default:
	complainAboutBadPointerType ("DSC_InitPointerFromPointerCPD");
        break;
    }
}


void DSC_Pointer::construct (rtLINK_CType *pMonotype) {
    pMonotype->Align ();

/*****  ... If its positional ptoken is the scalar ptoken ... *****/
    if (pMonotype->PPT ()->NamesTheScalarPToken ()) {
/*****  ... then create a scalar reference pointer  *****/
	M_CPD *ptoken = pMonotype->RPT ();
	int value = rtLINK_RRDC_ReferenceOrigin (rtLINK_LC_ChainHead (pMonotype));

	ptoken->retain ();
	constructReferenceScalar (ptoken, value);
    }
    else {
/*****  ... else create a link pointer  *****/
	pMonotype->retain ();
	constructLink (pMonotype);
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to initialize a correspondence pointer.
 *
 *  Arguments:
 *	this			- the pointer to be initialized.
 *	posPTokenCPD		- a standard CPD for the positional P-Token
 *				  of the pointer being initialized.
 *	p/xRPTReference		- a CPD/Index pair for the referential state
 *				  of this pointer.
 *				  The positional and referenctial P-Tokens
 *				  MUST have the same number of elements.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *      None of the arguments will be assigned directly into the pointer.
 *      If they are needed, they will be duplicated.
 *
 *	A 'correspondence' pointer is the equivalent of an identity pointer
 *	but with differing positional and referential P-Tokens.  If these
 *	p-tokens have the same cardinality, a one-to-one mapping by position
 *	is created from the elements of the set denoted by 'posPToken' to the
 *	elements of the set denoted by 'refPToken'.  If the cardinality of the
 *	reference p-token is 1, a many-to-one mapping is created from the
 *	elements of the set denoted by 'posPToken' to the single element of
 *	the set denoted by 'refPToken' (i.e., scalar coercion).  If neither of
 *	these conditions is true, an error is generated.
 *
 *****/
void DSC_Pointer::constructCorrespondence (
    M_CPD *posPTokenCPD, M_CPD *pRPTReference, unsigned int xRPTReference
) {
    M_CPD *refPTokenCPD = pRPTReference->GetCPD (xRPTReference, RTYPE_C_PToken);
    unsigned int rptCardinality = rtPTOKEN_CPD_BaseElementCount (refPTokenCPD);

    if (1 == rptCardinality)
	constructReferenceConstant (posPTokenCPD, refPTokenCPD, 0);
    else if (rptCardinality == rtPTOKEN_CPD_BaseElementCount (posPTokenCPD)) {
	rtLINK_CType *linkc = rtLINK_RefConstructor (refPTokenCPD, -1);
	rtLINK_AppendRange (linkc, 0, rptCardinality);
	linkc->Close (posPTokenCPD);
	constructLink (linkc);
	refPTokenCPD->release ();
    }
    else {
	refPTokenCPD->release ();
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"%s: |PPT| == %u, |RPT| == %u",
		"DSC_InitCorrespondenceDescriptor: P-Token Size Difference",
		rtPTOKEN_CPD_BaseElementCount (posPTokenCPD),
		rptCardinality
	    )
	);
    }
}


/*********************************
 *****  Extraction Routines  *****
 *********************************/

/***********************************************
 *----  DSC_Pointer::constructComposition  ----*
 ***********************************************/

void DSC_Pointer::constructComposition (rtLINK_CType *pSubscript, DSC_Pointer &rSource) {
    switch (rSource.Type ()) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_LCExtractFromPointer);
        break;
    case DSC_PointerType_Scalar:
	pSubscript->AlignForCoerce (); {
	    DSC_Scalar &rScalar = DSC_Pointer_Scalar (rSource);

    /***  If source holds a reference, create a Link, otherwise create a Value Pointer.  ***/
	    if (rScalar.holdsAReference ()) {
		rtREFUV_AlignReference (&rScalar);

		rtLINK_CType *pResult = rtLINK_PosConstructor (pSubscript->PPT (), -1);

#		define handleNil(c,n,r)
#		define handleRef(c,n,r) pResult->AppendRepeat (DSC_Scalar_Int (rScalar), n)

		rtLINK_TraverseRRDCList (pSubscript, handleNil, handleRef, handleRef);

#		undef handleRef
#		undef handleNil

		pResult->Close (rScalar.m_pRPT);
		constructLink (pResult);
	    }
	    else {
	/********************************************************************************
	 ***********************************  TO DO:  ***********************************
	 *  Convert the composition with a coerced scalar into a direct construction.   *
	 ********************************************************************************
	 ********************************************************************************/
		M_CPD *pSource = rScalar.asContainer (pSubscript->RPT ());
		m_iContent.as_iValue.constructComposition (pSubscript, pSource);
		m_xType = DSC_PointerType_Value;
		pSource->release ();
	    }
	}
	break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.constructComposition (pSubscript, DSC_Pointer_Value (rSource));
	m_xType = DSC_PointerType_Value;
	break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.constructComposition (
	    pSubscript, DSC_Pointer_Reference (rSource)
	);
	m_xType = DSC_PointerType_Reference;
	break;
    case DSC_PointerType_Link:
	m_iContent.as_iLink.constructComposition (pSubscript, DSC_Pointer_Link (rSource));
	m_xType = DSC_PointerType_Link;
	break;
    case DSC_PointerType_Identity:
	pSubscript->retain ();
	constructLink (pSubscript);
	break;
    default:
	rSource.complainAboutBadPointerType (DSC_N_LCExtractFromPointer);
        break;
    }
}


void DSC_Pointer::constructComposition (M_CPD *pSubscript, DSC_Pointer &rSource) {
    switch (rSource.Type ()) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_UVExtractFromPointer);
        break;
    case DSC_PointerType_Scalar: {
    /********************************************************************************
     ***********************************  TO DO:  ***********************************
     *  Convert the composition with a coerced scalar into a direct construction.   *
     ********************************************************************************
     ********************************************************************************/
    //  Coerce using the subscript RPT to avoid cross database scalar ptoken problems...
	    M_CPD *pSourcePPT = UV_CPD_RefPTokenCPD (pSubscript);
	    if (pSourcePPT->DoesntNameTheScalarPToken ()) {
		pSourcePPT->release ();
		ERR_SignalFault (
		    EC__InternalInconsistency,
		    "DSC_Pointer::constructComposition (U,P): Non-scalar co-domain."
		);
	    }
	    M_CPD *pSource = DSC_Pointer_Scalar (rSource).asContainer (pSourcePPT);

    //	If source holds a reference, create a Reference, otherwise create a Value...
	    if (rSource.m_iContent.as_iScalar.holdsAReference ())
		constructReference (rtLINK_UVExtract (pSource, pSubscript));
	    else {
		m_iContent.as_iValue.constructComposition (pSubscript, pSource);
		m_xType = DSC_PointerType_Value;
	    }
	    pSource->release ();
	    pSourcePPT->release ();
	}
	break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.constructComposition (pSubscript, DSC_Pointer_Value (rSource));
	m_xType = DSC_PointerType_Value;
	break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.constructComposition (pSubscript, DSC_Pointer_Reference (rSource));
	m_xType = DSC_PointerType_Reference;
	break;
    case DSC_PointerType_Link:
	m_iContent.as_iReference.constructComposition (pSubscript, DSC_Pointer_Link (rSource));
	m_xType = DSC_PointerType_Reference;
	break;
    case DSC_PointerType_Identity:
	pSubscript->retain ();
        constructReference (pSubscript);
	break;
    default:
	rSource.complainAboutBadPointerType (DSC_N_UVExtractFromPointer);
        break;
    }
}


void DSC_Pointer::constructComposition (DSC_Scalar &rSubscript, DSC_Pointer &rSource) {
    rtREFUV_AlignReference (&rSubscript);

    switch (rSource.Type ()) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_RefExtractFromPointer);
        break;
    case DSC_PointerType_Scalar:
	DSC_DuplicateScalar (
	    m_iContent.as_iScalar, DSC_Pointer_Scalar (rSource)
	);
	if (rSource.holdsAScalarReference () && DSC_Scalar_Int (rSubscript) != 0) {
	    DSC_Pointer_Scalar_Int (*this) = rtPTOKEN_CPD_BaseElementCount (
		DSC_Scalar_RefPToken (m_iContent.as_iScalar)
	    );
	}
	break;

    case DSC_PointerType_Value:
	m_iContent.as_iScalar.constructComposition (
	    DSC_Scalar_Int (rSubscript), DSC_Pointer_Value (rSource)
	);
	break;
    case DSC_PointerType_Reference:
        m_iContent.as_iScalar.constructComposition (
	    DSC_Scalar_Int (rSubscript), DSC_Pointer_Reference (rSource)
	);
	break;
    case DSC_PointerType_Link:
	rtLINK_RefExtract (
	    &m_iContent.as_iScalar,
	    DSC_Pointer_Link (rSource).realizedContainer (),
	    &rSubscript
	);
	break;

    case DSC_PointerType_Identity:
	DSC_DuplicateScalar (m_iContent.as_iScalar, rSubscript);
	break;

    default:
	rSource.complainAboutBadPointerType (DSC_N_RefExtractFromPointer);
        break;
    }

    m_xType = DSC_PointerType_Scalar;
}


void DSC_Pointer::constructComposition (DSC_Pointer &rSubscript, DSC_Pointer &rSource) {
    switch (DSC_Pointer_Type (rSubscript)) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType ("DSC_PointerExtractFromPointer");
	break;
    case DSC_PointerType_Reference:
	constructComposition (DSC_Pointer_Reference (rSubscript), rSource);
	break;
    case DSC_PointerType_Link:
	constructComposition (DSC_Pointer_Link (rSubscript), rSource);
	break;
    case DSC_PointerType_Value: {
	    rtLINK_CType *linkc = rtLINK_PosConstructor (
		DSC_Pointer_Value (rSubscript), UV_CPx_PToken
	    );
	    rtLINK_AppendRepeat (
		linkc, rtPTOKEN_BaseElementCount (
		    DSC_Pointer_Value (rSubscript), UV_CPx_RefPToken
		), linkc->BaseElementCount ()
	    );
	    linkc->Close (DSC_Pointer_Value (rSubscript), UV_CPx_RefPToken);
	    constructComposition (linkc, rSource);
	    linkc->release ();
	}
	break;
    case DSC_PointerType_Identity:
	construct (rSource);
	break;
    case DSC_PointerType_Scalar:
	if (rSubscript.holdsAScalarReference ())
	    constructComposition (DSC_Pointer_Scalar (rSubscript), rSource);
	else {
	    DSC_Scalar newScalar;
	    DSC_DuplicateScalar (newScalar, DSC_Pointer_Scalar (rSubscript));
	    DSC_Scalar_Int (newScalar) = rtPTOKEN_CPD_BaseElementCount (
		DSC_Scalar_RefPToken (DSC_Pointer_Scalar (rSubscript))
	    );
	    constructComposition (newScalar, rSource);
	    DSC_ClearScalar (newScalar);
	}
	break;
    }
}


/*********************************************
 *----  DSC_Value::constructComposition  ----*
 *********************************************/

void DSC_Value::constructComposition (rtLINK_CType *pSubscript, M_CPD *pSource) {
    if (RTYPE_PerformHandlerOperation (
	    RTYPE_DoLCExtract, pSource->RType (), &m_pValues, pSource, pSubscript
	)
    ) raiseTypeException (ExceptionSource_ConstructComposition);
}

void DSC_Value::constructComposition (M_CPD *pSubscript, M_CPD *pSource) {
    switch (pSource->RType ()) {
    case RTYPE_C_CharUV:
	m_pValues = rtCHARUV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_DoubleUV:
	m_pValues = rtDOUBLEUV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_FloatUV:
	m_pValues = rtFLOATUV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_IntUV:
	m_pValues = rtINTUV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_Unsigned128UV:
	m_pValues = rtU128UV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_Unsigned64UV:
	m_pValues = rtU64UV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_Unsigned96UV:
	m_pValues = rtU96UV_UVExtract (pSource, pSubscript);
	break;
    case RTYPE_C_UndefUV:
	m_pValues = rtUNDEFUV_UVExtract (pSource, pSubscript);
	break;
    default:
	raiseTypeException (ExceptionSource_ConstructComposition);
	break;
    }
}


/*****************************
 *----  *::constructSet  ----*
 *****************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize a set Value and an enumeration from a source
 *****  Value.
 *
 *  Arguments:
 *	target	    		- the set Value to be initialized.
 *	enumeration		- an 'lval' of type 'M_CPD*' which will
 *				  be set to a standard CPD for a reference
 *				  u-vector specifying the ordering factored
 *				  from 'source'.
 *	source			- the Value to use as the source of the
 *				  set.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 ****/
void DSC_Value::constructSet (
    M_ASD *pContainerSpace, M_CPD *&rpEnumeration, M_CPD *source
) {
    switch (source->RType ()) {
    case RTYPE_C_CharUV:
	rtCHARUV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_ToSetUV (pContainerSpace, source, &m_pValues, &rpEnumeration);
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_InitSetValue: Unsupported R-Type %s", source->RTypeName ()
	    )
	);
	break;
    }
}


void DSC_Pointer::constructSet (
    M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Pointer &rSource
) {
    switch (rSource.Type ()) {
    case DSC_PointerType_Empty:
        DSC__ComplainAboutEmptyPtrType (DSC_N_InitSetPointer);
        break;
    case DSC_PointerType_Scalar:
	ERR_SignalFault (
	    EC__UnimplementedCase, "DSC_InitSetPointer: Scalar Case Not Handled"
	);
	break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.constructSet (
	    pContainerSpace, rpEnumeration, DSC_Pointer_Reference (rSource)
	);
	m_xType = DSC_PointerType_Reference;
	break;
    case DSC_PointerType_Link:
	m_iContent.as_iReference.constructSet (
	    pContainerSpace, rpEnumeration, DSC_Pointer_Link (rSource)
	);
	m_xType = DSC_PointerType_Reference;
	break;
    case DSC_PointerType_Identity:
	m_iContent.as_iReference.constructSet (
	    pContainerSpace, rpEnumeration, DSC_Pointer_Identity (rSource)
	);
	m_xType = DSC_PointerType_Reference;
	break;

    case DSC_PointerType_Value:
	m_iContent.as_iValue.constructSet (
	    pContainerSpace, rpEnumeration, DSC_Pointer_Value (rSource)
	);
	m_xType = DSC_PointerType_Value;
	break;

    default:
	rSource.complainAboutBadPointerType (DSC_N_InitSetPointer);
        break;
    }
}


/**************************************
 *  Pointer Duplication and Clearing  *
 **************************************/

void DSC_Pointer::construct (DSC_Pointer const& rSource) {
    switch (rSource.m_xType) {
    case DSC_PointerType_Empty:
        break;
    case DSC_PointerType_Scalar:
	DSC_DuplicateScalar (m_iContent.as_iScalar, DSC_Pointer_Scalar (rSource));
        break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.construct (DSC_Pointer_Value (rSource));
        break;
    case DSC_PointerType_Identity:
	m_iContent.as_iIdentity.construct (DSC_Pointer_Identity (rSource));
        break;
    case DSC_PointerType_Link:
	m_iContent.as_iLink.construct (DSC_Pointer_Link (rSource));
        break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.construct (DSC_Pointer_Reference (rSource));
        break;
    default:
	rSource.complainAboutBadPointerType ("DSC_Pointer::construct(DSC_Pointer const&)");
        break;
    }

    m_xType = rSource.m_xType;
}


void DSC_Pointer::clear () {
    switch (m_xType) {
    case DSC_PointerType_Empty:
        break;
    case DSC_PointerType_Scalar:
	DSC_ClearScalar (m_iContent.as_iScalar);
        break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.clear ();
        break;
    case DSC_PointerType_Identity:
	m_iContent.as_iIdentity.clear ();
        break;
    case DSC_PointerType_Link:
	m_iContent.as_iLink.clear ();
        break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.clear ();
        break;
    default:
	complainAboutBadPointerType ("DSC_Pointer::clear");
        break;
    }
    m_xType = DSC_PointerType_Empty;
}


/**********************
 *****  Coercion  *****
 **********************/

void DSC_Pointer::coerce (M_CPD *pPPT) {
    if (isntScalar () || pPPT->NamesTheScalarPToken ())
	return;

    DSC_Scalar &rScalar = m_iContent.as_iScalar;

    if (rScalar.holdsAValue ()) {
	M_CPD *pCoercedScalar = rScalar.asCoercedContainer (pPPT);
	DSC_ClearScalar (rScalar);
	constructValue (pCoercedScalar);
    }
    else {
	rtREFUV_AlignReference (&rScalar);
	rtLINK_CType *pCoercedScalar = rtLINK_PosConstructor (pPPT, -1);
	rtLINK_AppendRepeat (
	    pCoercedScalar, DSC_Scalar_Int (rScalar), rtPTOKEN_CPD_BaseElementCount (pPPT)
	);
	pCoercedScalar->Close (rScalar.m_pRPT);
	DSC_ClearScalar (rScalar);
	constructLink (pCoercedScalar);
    }
}


/*****************************************************
 *  Pointer Factoring, Distribution, and Reordering  *
 *****************************************************/

M_CPD *DSC_Pointer::factor () {
    M_CPD *pDistribution = NilOf (M_CPD *);

    switch (m_xType) {
    case DSC_PointerType_Empty:
    case DSC_PointerType_Scalar:
    case DSC_PointerType_Identity:
    case DSC_PointerType_Link:
    case DSC_PointerType_Value:
        break;
    case DSC_PointerType_Reference: {
	    /** get the linkc and redistribution uvector ... **/
	    rtLINK_CType *linkc;
	    pDistribution = m_iContent.as_iReference.factor (&linkc);

	    /** clear the reference ... **/
	    m_iContent.as_iReference.clear ();

	    /** now convert to either an identity or link pointer ... **/
	    if (linkc)
		constructLink (linkc);
	    else
		constructIdentity (UV_CPD_PosPTokenCPD (pDistribution));
	}
        break;
    default:
	complainAboutBadPointerType ("DSC_Pointer::factor");
        break;
    }

    return pDistribution;
}


/******************************
 *  Value Distribution Macro  *
 ******************************/

/*---------------------------------------------------------------------------
 *****  Macro to distribute a 'Value'
 *
 *  Arguments:
 *	value			- the Value to be 'distributed'.
 *	cpd			- a standard CPD for a reference u-vector
 *				  specifying the 'distribution' to be
 *				  applied to the Value.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
void DSC_Value::distribute (M_CPD *cpd) {
    M_CPD *newuv;

    switch (m_pValues->RType ()) {
    case RTYPE_C_CharUV:
	newuv = rtCHARUV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_FloatUV:
	newuv = rtFLOATUV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_DoubleUV:
	newuv = rtDOUBLEUV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_IntUV:
	newuv = rtINTUV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_UndefUV:
	newuv = rtUNDEFUV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_Unsigned64UV:
	newuv = rtU64UV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_Unsigned96UV:
	newuv = rtU96UV_Distribute (cpd, m_pValues);
	break;
    case RTYPE_C_Unsigned128UV:
	newuv = rtU128UV_Distribute (cpd, m_pValues);
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"DSC_DistributeValue: Unsupported R-Type %s", m_pValues->RTypeName ()
	    )
	);
	break;
    }
    m_pValues->release ();
    m_pValues = newuv;
}


void DSC_Pointer::distribute (M_CPD *distribution) {
    M_CPD *pptrefCPD;
    int	   pptrefIndex;
    getPPTReference (pptrefCPD, pptrefIndex);
    if (pptrefCPD->ReferenceDoesntName (pptrefIndex, distribution, UV_CPx_PToken)) {
	pptrefCPD->release ();
        ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"%s: Inconsistent Descriptor Distribution",
		DSC_N_DistributePointer
	    )
	);
    }
    pptrefCPD->release ();

    switch (m_xType) {
    case DSC_PointerType_Empty:
    case DSC_PointerType_Scalar:
	break;

    case DSC_PointerType_Identity:
	/** turn it into a reference with cpd as the redistribution ... **/
	m_iContent.as_iIdentity.clear ();
	distribution->retain ();
	constructReference (distribution, NilOf (rtLINK_CType*));
	break;
    case DSC_PointerType_Link: {
	/** turn it into a reference with cpd as the redistribution and
         ** the link as the subset ...
         **/
	    rtLINK_CType *linkc = m_iContent.as_iLink;
	    linkc->retain ();
	    m_iContent.as_iLink.clear ();
	    distribution->retain ();
	    constructReference (distribution, linkc);
	}
	break;
    case DSC_PointerType_Value:
	m_iContent.as_iValue.distribute (distribution);
        break;
    case DSC_PointerType_Reference:
	m_iContent.as_iReference.distribute (distribution);
        break;
    default:
	complainAboutBadPointerType (DSC_N_DistributePointer);
        break;
   }
}


void DSC_Pointer::reorder (M_CPD *reordering) {
    M_CPD *distribution = rtREFUV_Flip (reordering);
    distribute (distribution);
    distribution->release ();
}


/*************************
 *  U-Vector Assignment  *
 *************************/

void DSC_Pointer::assignToUV (rtLINK_CType *pSubscript, M_CPD *pTarget) {
    switch (m_xType) {
/***** Empty Value Descriptor *****/
    case DSC_PointerType_Empty:
	DSC__ComplainAboutEmptyPtrType (DSC_N_LCAssignIntoUVFromPtr);
	break;

/***** Scalar Value Descriptor *****/
    case DSC_PointerType_Scalar:
	switch (pTarget->RType ()) {
	case RTYPE_C_CharUV:
	    rtCHARUV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Char (*this)
	    );
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Double (*this)
	    );
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Float (*this)
	    );
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Int (*this)
	    );
	    break;
	case RTYPE_C_RefUV:
	    /* Pass the whole scalar piece as source to REFUV */
	    rtREFUV_LCAssignScalar (
		pTarget, pSubscript, &m_iContent.as_iScalar
	    );
	    break;
	case RTYPE_C_UndefUV:
	    rtUNDEFUV_Align (pTarget);
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Unsigned64 (*this)
	    );
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Unsigned96 (*this)
	    );
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_LCAssignScalar (
		pTarget, pSubscript, &DSC_Pointer_Scalar_Unsigned128 (*this)
	    );
	    break;
	default:
	    ERR_SignalFault (
		EC__UnknownUVectorRType, UTIL_FormatMessage (
		    "DSC_LCAssignIntoUVFromPointer: Unsupported R-Type %s",
		    pTarget->RTypeName ()
		)
	    );
	    break;
	}
	break;

/***** Reference Pointer Value Descriptor ******/
    case DSC_PointerType_Reference:
	rtREFUV_LCAssign (pTarget, pSubscript, m_iContent.as_iReference);
	break;

/***** UVector Value Pointer Value Descriptor *****/
    case DSC_PointerType_Value:
	/*****  ...perform a case specific assignment...  *****/
	switch (pTarget->RType ()) {
	case RTYPE_C_CharUV:
	    rtCHARUV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_RefUV:
	    rtREFUV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_UndefUV:
	    rtUNDEFUV_Align (pTarget);
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_LCAssign (pTarget, pSubscript, m_iContent.as_iValue);
	    break;
	default:
	    ERR_SignalFault (
		EC__UnknownUVectorRType, UTIL_FormatMessage (
		    "DSC_LCAssignIntoUVFromPointer: Unsupported R-Type %s",
		    pTarget->RTypeName ()
		)
	    );
	    break;
	}
	break;

/***** Identity Case *****/
    case DSC_PointerType_Identity: {
	    /*** For the source:
	     *   Convert the Identity into a reference uvector initialized to
	     *   0 to the size of the Identity ptoken.
	     ***/
	    M_CPD *ptoken = m_iContent.as_iIdentity.PToken ();
	    M_CPD *refuv = rtREFUV_New (ptoken, ptoken);
	    rtREFUV_ElementType *ptr = rtREFUV_CPD_Array (refuv);
	    for (unsigned int i=0; i<rtPTOKEN_CPD_BaseElementCount (ptoken); i++)
		*ptr++ = (rtREFUV_ElementType)i;
	    /*** Do the Assign ***/
	    rtREFUV_LCAssign (pTarget, pSubscript, refuv);
	    refuv->release ();
	}
	break;

/***** Link Case *****/
    case DSC_PointerType_Link: {
	    /*** Convert the link into a reference uvector for the source ***/
	    M_CPD *refuv = m_iContent.as_iLink->ToRefUV ();
	    rtREFUV_LCAssign (pTarget, pSubscript, refuv);
	    refuv->release ();
	}
	break;

/***** Error case *****/
    default:
	DSC__ComplainAboutEmptyPtrType (DSC_N_LCAssignIntoUVFromPtr);
	break;
    }
}


/*************************
 *************************
 *****  DESCRIPTORS  *****
 *************************
 *************************/

/**************************
 *****  Construction  *****
 **************************/

void DSC_Descriptor::constructMonotype (M_CPD *pMonotype) {
    RTYPE_Type xMonotypeType = pMonotype->RType ();

    M_KOT *pKOT;
    switch (xMonotypeType) {
    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_UndefUV:
	pKOT = pMonotype->ReferencedKOT (UV_CPx_RefPToken);
	break;
    default:
	pKOT = NilOf (M_KOT*);
	break;
    }

    M_CPD *pStore = NilOf (M_CPD*);
    switch (xMonotypeType) {
    case RTYPE_C_CharUV:
	if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheCharacterPTokenCPD ()))
	    pStore = pKOT->TheCharacterClass;
	break;
    case RTYPE_C_DoubleUV:
	if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheDoublePTokenCPD ()))
	    pStore = pKOT->TheDoubleClass;
	break;
    case RTYPE_C_FloatUV:
	if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheFloatPTokenCPD ()))
	    pStore = pKOT->TheFloatClass;
	break;
    case RTYPE_C_IntUV:
	if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheIntegerPTokenCPD ()))
	    pStore = pKOT->TheIntegerClass;
	else if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheDatePTokenCPD ()))
	    pStore = pKOT->TheDateClass;
	break;
    case RTYPE_C_UndefUV:
	if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheNAPTokenCPD ()))
	    pStore = pKOT->TheNAClass;
	else if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheTruePTokenCPD ()))
	    pStore = pKOT->TheTrueClass;
	else if (pMonotype->ReferenceNames (UV_CPx_RefPToken, pKOT->TheFalsePTokenCPD ()))
	    pStore = pKOT->TheFalseClass;
    default:
	break;
    }

    if (pStore) {
	pStore->retain ();
	constructMonotype (pStore, pMonotype);
    }
    else ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "DSC_Descriptor::constructMonotype: Unrecognized Monotype Container: %s.",
	    pMonotype->RTypeName ()
	)
    );
}


/*******************************************
 *****  Correspondence Initialization  *****
 *******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to initialize a correspondence descriptor.
 *
 *  Arguments:
 *	rResultDsc		- the descriptor to be initialized.
 *	pPPT			- a standard CPD for the positional P-Token
 *				  of the descriptor being initialized.
 *	pStore			- a standard CPD for the store associated
 *				  with this descriptor.  This CPD will be
 *				  ASSIGNED into this descriptor.
 *	xStorePTokenRef		- the index of the 'store' CPD pointer which
 *				  points to the POP of the store's positional
 *				  P-Token.  The P-Token selected by this field
 *				  and 'store' MUST have the same number of
 *				  elements as 'pPPT'.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *  Notes:
 *	A 'correspondence' descriptor is the equivalent of a
 *	identity descriptor but with differing positional and referential
 *	P-Tokens.  If 'pPPT' is the scalar p-token, this descriptor's
 *	pointer is represented as a Scalar with reference element of 0 (zero).
 *	If 'pPPT' is not the scalar p-token, this descriptor's
 *	pointer is represented by a 'Link' containing a single range selecting
 *	all positions in the positional state of 'store/xStorePTokenRef'.
 *
 *****/


/*********************************
 *****  Zero Initialization  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Zero Descriptor Initialization
 *
 *  Arguments:
 *	this			- a reference to the descriptor that will be
 *				  initialized to the zero pointer.
 *	pStore			- the address of a CPD for the store of the
 *				  zero descriptor (assigned).
 *	xRType			- the index of the zero pointer's r-type.
 *	pPPT			- the address of a CPD for the positional
 *				  p-token of the zero (must be freed).
 *	pRPT			- the address of a CPD for the reference
 *				  p-token of the zero (assigned).
 */
void DSC_Descriptor::constructZero (
    M_CPD *pStore, RTYPE_Type xRType, M_CPD *pPPT, M_CPD *pRPT
) {
    switch (xRType) {
    case RTYPE_C_CharUV:
	constructConstant (pPPT, pStore, pRPT, '\0');
	break;
    case RTYPE_C_DoubleUV:
	constructConstant (pPPT, pStore, pRPT, 0.0);
	break;
    case RTYPE_C_FloatUV:
	constructConstant (pPPT, pStore, pRPT, (float)0.0);
	break;
    case RTYPE_C_IntUV:
	constructConstant (pPPT, pStore, pRPT, 0);
	break;
    case RTYPE_C_RefUV:
	constructReferenceConstant (pPPT, pStore, pRPT, 0);
	break;
    case RTYPE_C_UndefUV:
	constructConstant (pPPT, pStore, pRPT);
	break;
    case RTYPE_C_Unsigned64UV:
	constructConstant (pPPT, pStore, pRPT, VkUnsigned64::Zero ());
	break;
    case RTYPE_C_Unsigned96UV:
	constructConstant (pPPT, pStore, pRPT, VkUnsigned96::Zero ());
	break;
    case RTYPE_C_Unsigned128UV:
	constructConstant (pPPT, pStore, pRPT, VkUnsigned128::Zero ());
	break;
    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"DSC_InitZeroDescriptor: Invalid Pointer R-Type: %s",
		RTYPE_TypeIdAsString (xRType)
	    )
	);
    }
}


/**************************
 *  Descriptor Traversal  *
 **************************/

PublicFnDef void DSC__SetupTraversal (
    DSC_Descriptor* dsc, M_CPD **cpd, rtLINK_CType **linkc
) {
    if (dsc->holdsNonScalarValues ()) {
	*cpd = DSC_Descriptor_Value (*dsc);
	if ((*cpd)->RType () != RTYPE_C_IntUV)
	    ERR_SignalFault (
		EC__UnimplementedCase,
		"DSC_Traversal: Not Implemented for this descriptor type"
	    );
    }
    else if (dsc->holdsNonScalarReferences ())
	*cpd = DSC_Descriptor_Reference (*dsc);
    else if (dsc->holdsALink ())
	*linkc = DSC_Descriptor_Link (*dsc);
    else ERR_SignalFault (
	EC__UnimplementedCase,
	"DSC_Traversal: Not Implemented for this descriptor type"
    );
}


PublicFnDef void DSC__SetupJointTraversal(
    DSC_Descriptor	 *dsc1,
    DSC_Descriptor	 *dsc2,
    M_CPD		**cpd1,
    M_CPD		**cpd2,
    rtLINK_CType	**linkc1,
    rtLINK_CType	**linkc2,
    int			 *converted
) {
    *converted = false;

/*****  Determine the type of the first descriptor ... *****/
    if (dsc1->holdsNonScalarValues ()) {
	*cpd1 = DSC_Descriptor_Value (*dsc1);
	if ((*cpd1)->RType () != RTYPE_C_IntUV) ERR_SignalFault (
	    EC__UnimplementedCase,
	    "DSC_JointTraversal: Un-Implemented Descriptor Type."
	);
    }
    else if (dsc1->holdsNonScalarReferences ())
	*cpd1 = DSC_Descriptor_Reference (*dsc1);
    else if (dsc1->holdsALink ())
	*linkc1 = DSC_Descriptor_Link (*dsc1);
    else ERR_SignalFault (
	EC__UnimplementedCase,
	"DSC_JointTraversal: Not Implemented for this descriptor type"
    );

/*****  Determine the type of the second descriptor ... *****/
    if (dsc2->holdsNonScalarValues ()) {
	*cpd2 = DSC_Descriptor_Value (*dsc2);

	if ((*cpd2)->RType () != RTYPE_C_IntUV) ERR_SignalFault (
	    EC__UnimplementedCase,
	    "DSC_JointTraversal: Un-Implemented Descriptor Type."
	);
    }
    else if (dsc2->holdsNonScalarReferences ())
	*cpd2 = DSC_Descriptor_Reference (*dsc2);
    else if (dsc2->holdsALink ())
	*linkc2 = DSC_Descriptor_Link (*dsc2);
    else ERR_SignalFault (
	EC__UnimplementedCase,
	"DSC_JointTraversal: Un-Implemented Descriptor Type."
    );

/*****  Can't have two linkcs, turn *linkc1 into a uvector ... *****/
    if (IsntNil (*linkc1) && IsntNil (*linkc2)) {
	*cpd1 = (*linkc1)->ToRefUV ();
	*linkc1 = NilOf (rtLINK_CType*);
	*converted = true;
    }
}


/*****************************************
 *****  Usage Count Display Routine  *****
 *****************************************/

PublicFnDef void DSC_DisplayCounts () {
    IO_printf ("\nDescriptor Routines Usage Counts\n");
    IO_printf (  "--------------------------------\n");
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (DSC);
    FAC_FDFCase_FacilityDescription ("Descriptor");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dsc.c 1 replace /users/mjc/system
  861002 17:59:42 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  dsc.c 2 replace /users/jad/system
  861024 15:35:07 jad made link descriptors
contain a linkc and/or a linkcpd to eliminate some conversions

 ************************************************************************/
/************************************************************************
  dsc.c 3 replace /users/jad/system
  870414 16:02:30 jad implemented various new macros for the new time series operations

 ************************************************************************/
/************************************************************************
  dsc.c 4 replace /users/jad/system
  870421 12:41:41 jad implemented DSC_UVExtractFromPointer

 ************************************************************************/
/************************************************************************
  dsc.c 5 replace /users/mjc/translation
  870524 09:37:24 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  dsc.c 6 replace /users/mjc/system
  870607 03:09:18 mjc Convert long macros to routines to deal with VMS C Compiler limits

 ************************************************************************/
/************************************************************************
  dsc.c 7 replace /users/m2/backend
  890503 15:05:04 m2 PublicFnDecl -> PublicFnDef

 ************************************************************************/

#ifndef DSC_Scalar_Interface
#define DSC_Scalar_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkScalar.h"

#include "rtype.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"

#include "RTptoken.h"

PublicFnDecl void rtREFUV_ComplainAboutElementVal (int element, int upperBound);


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * Scalars provide an efficient representation for values not requiring the
 * more general, parallel representation of U-Vectors.  Conceptually, scalars
 * possess no positional state of their own.  When combined with other objects
 * in a context in which positional state matters, scalars are coerced to the
 * positional state required in that context.
 *
 *  Scalar Field Descriptions:
 *	m_xRType		- the R-Type of the u-vector used to hold a
 *				  value of this type.
 *	m_pRPT			- a standard CPD for the reference P-Token
 *				  associated with this scalar.  Currently
 *				  required for 'reference' scalars only.
 *	m_iValue			- a union of representational alternatives for
 *				  the scalar.  Definition found in 'VkScalar.h'.
 *---------------------------------------------------------------------------
 */


/***************************
 *----  Access Macros  ----*
 ***************************/

#define DSC_Scalar_RType(scalar)	((scalar).m_xRType)
#define DSC_Scalar_RefPToken(scalar)	((scalar).m_pRPT)
#define DSC_Scalar_Value(scalar)	((scalar).m_iValue)

#define DSC_Scalar_Char(scalar)		(DSC_Scalar_Value (scalar).asChar)
#define DSC_Scalar_Double(scalar)	(DSC_Scalar_Value (scalar).asDouble)
#define DSC_Scalar_Float(scalar)	(DSC_Scalar_Value (scalar).asFloat)
#define DSC_Scalar_Int(scalar)		(DSC_Scalar_Value (scalar).asInt)
#define DSC_Scalar_Unsigned64(scalar)	(DSC_Scalar_Value (scalar).asUnsigned64)
#define DSC_Scalar_Unsigned96(scalar)	(DSC_Scalar_Value (scalar).asUnsigned96)
#define DSC_Scalar_Unsigned128(scalar)	(DSC_Scalar_Value (scalar).asUnsigned128)


/********************
 *----  Access  ----*
 ********************/

/*---------------------------------------------------------------------------
 *****  Macro to return the R-Type of the 'Scalar' component of a descriptor.
 *
 *  Arguments:
 *	scalar			- the scalar whose R-Type is desired.
 *
 *  Syntactic Context:
 *	RType_Type Expression
 *
 *****/
#define DSC_ScalarRType(scalar) (\
    DSC_Scalar_RType (scalar) != RTYPE_C_RefUV ? DSC_Scalar_RType (scalar) : RTYPE_C_Link\
)

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the positional
 *****  P-Token of a scalar.
 *
 *  Arguments:
 *	scalar			- the scalar whose positional P-Token is
 *				  desired.
 *	pTokenRefCPD/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the positional
 *				  P-Token of 'scalar'.  'pTokenRefCPD' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_PositionalPTokenOfScalar(scalar, pTokenRefCPD, pTokenRefIndex) {\
    M_CPD *pPPT = M_AttachedNetwork->TheScalarPToken ();\
    pPPT->retain ();\
    pTokenRefCPD	= pPPT;\
    pTokenRefIndex	= (-1);\
}

/*---------------------------------------------------------------------------
 *****  Macro to obtain a reference (i.e., CPD/Index pair) to the referential
 *****  P-Token of a scalar.
 *
 *  Arguments:
 *	scalar			- the scalar whose referential P-Token is
 *				  desired.
 *	pTokenRefCPD/Index	- 'lval's which will be set to a CPD and index
 *				  pair referencing the POP of the referential
 *				  P-Token of 'scalar'.  'pTokenRefCPD' must be
 *				  freed when no longer needed.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_ReferentialPTokenOfScalar(scalar, pTokenRefCPD, pTokenRefIndex) {\
    DSC_Scalar_RefPToken (scalar)->retain ();\
    pTokenRefCPD	= DSC_Scalar_RefPToken (scalar);\
    pTokenRefIndex	= (-1);\
}


/****************************
 *----  Initialization  ----*
 ****************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize the contents of a 'Scalar'.
 *
 *  Arguments:
 *	scalar			- a variable of type 'DSC_Scalar'
 *				  to be initialized.
 *	pToken			- a CPD for the reference P-Token associated
 *				  with this scalar.  This CPD will be ASSIGNED
 *				  into the scalar.
 *	rType			- the R-Type of the U-Vector used to hold a
 *				  value of this type.
 *	field			- the name of the direct access macro for the
 *				  scalar field being set.
 *	value			- the value of the scalar.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_InitScalar(scalar,pToken,rType,field,value) {\
    DSC_Scalar_RType		(scalar) = (RTYPE_Type)(rType);\
    DSC_Scalar_RefPToken	(scalar) = (pToken);\
    field			(scalar) = (value);\
}

/*---------------------------------------------------------------------------
 *****  Macro to initialize a Reference 'Scalar'.
 *
 *  Arguments:
 *	scalar			- the variable of type 'DSC_Scalar'
 *				  to be initialized.
 *	pToken			- a CPD for the reference P-Token associated
 *				  with this scalar.  This CPD will be ASSIGNED
 *				  into the scalar.
 *	element			- the position being referenced.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_InitReferenceScalar(scalar, pToken, element) {\
    M_CPD*		iptoken	 = (pToken);\
    unsigned int	ielement = (element);\
\
    if (ielement > rtPTOKEN_CPD_BaseElementCount (iptoken)) {\
	int upperBound = rtPTOKEN_CPD_BaseElementCount (iptoken);\
	rtREFUV_ComplainAboutElementVal (ielement, upperBound);\
    }\
    DSC_InitScalar (scalar, iptoken, RTYPE_C_RefUV, DSC_Scalar_Int, ielement)\
}


/*---------------------------------------------------------------------------
 *****  Macros to initialize various flavors of value scalar.
 *
 *  Arguments:
 *	scalar			- the Pointer to initialize.
 *	ptoken			- a standard CPD for the reference P-Token of
 *				  the scalar.  This is ASSIGNED into the
 *                                scalar.
 *	value			- the value of the scalar.  This parameter is
 *				  omitted for undefined scalars.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_InitUndefinedScalar(scalar, ptoken)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_UndefUV, DSC_Scalar_Int, 0)

#define DSC_InitCharScalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_CharUV, DSC_Scalar_Char, value)

#define DSC_InitDoubleScalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_DoubleUV, DSC_Scalar_Double, value)

#define DSC_InitFloatScalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_FloatUV, DSC_Scalar_Float, value)

#define DSC_InitIntegerScalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_IntUV, DSC_Scalar_Int, value)

#define DSC_InitUnsigned64Scalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_Unsigned64UV, DSC_Scalar_Unsigned64, value)

#define DSC_InitUnsigned96Scalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_Unsigned96UV, DSC_Scalar_Unsigned96, value)

#define DSC_InitUnsigned128Scalar(scalar, ptoken, value)\
    DSC_InitScalar (scalar, ptoken, RTYPE_C_Unsigned128UV, DSC_Scalar_Unsigned128, value)


/*************************************
 *---  Duplication and Clearing  ----*
 *************************************/

/*---------------------------------------------------------------------------
 *****  Macro to duplicate a Scalar.
 *
 *  Arguments:
 *	target			- a location whose contents will be replaced
 *				  by the duplicated Scalar.  This location
 *				  is assumed to NOT contain a currently valid
 *				  Scalar.
 *	source			- a location containing the Scalar to be
 *				  duplicated.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_DuplicateScalar(target, source) {\
    DSC_Scalar_RefPToken (source)->retain ();\
    target = source;\
}

/*---------------------------------------------------------------------------
 *****  Macro to clear a Scalar.
 *
 *  Argument:
 *	scalar			- a variable of type 'DSC_Scalar'
 *				  whose contents are to be cleared.
 *
 *  Syntactic Context:
 *	Compound Statement
 *
 *****/
#define DSC_ClearScalar(scalar) {\
    DSC_Scalar_RefPToken (scalar)->release ();\
}


/**********************
 *  Class Definition  *
 **********************/

class DSC_Scalar {
//  Construction/Initialization
public:
    void constructComposition (unsigned int xSubscript, M_CPD *pSource);

//  Query
public:
    bool holdsANil () const;

    bool holdsAReference () const {
	return RTYPE_C_RefUV == m_xRType;
    }
    bool holdsAValue () const {
	return RTYPE_C_RefUV != m_xRType;
    }

//  Container Construction
public:
    M_CPD *asContainer (M_CPD *pPPT);
    M_CPD *asCoercedContainer (M_CPD *pPPT);

    M_CPD *asUVector ();

//  State
public:
    RTYPE_Type		 m_xRType;
    M_CPD		*m_pRPT;
    DSC_ScalarValue	 m_iValue;
};


#endif

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
 *	m_pRPT			- a handle for the reference P-Token
 *				  associated with this scalar.  Currently
 *				  required for 'reference' scalars only.
 *	m_iValue			- a union of representational alternatives for
 *				  the scalar.  Definition found in 'VkScalar.h'.
 *---------------------------------------------------------------------------
 */


/***************************
 *----  Access Macros  ----*
 ***************************/

#define DSC_Scalar_Value(scalar)	((scalar).m_iValue)
#define DSC_Scalar_Char(scalar)		(DSC_Scalar_Value (scalar).asChar)
#define DSC_Scalar_Double(scalar)	(DSC_Scalar_Value (scalar).asDouble)
#define DSC_Scalar_Float(scalar)	(DSC_Scalar_Value (scalar).asFloat)
#define DSC_Scalar_Int(scalar)		(DSC_Scalar_Value (scalar).asInt)
#define DSC_Scalar_Unsigned64(scalar)	(DSC_Scalar_Value (scalar).asUnsigned64)
#define DSC_Scalar_Unsigned96(scalar)	(DSC_Scalar_Value (scalar).asUnsigned96)
#define DSC_Scalar_Unsigned128(scalar)	(DSC_Scalar_Value (scalar).asUnsigned128)


/****************************
 *----  Initialization  ----*
 ****************************/

/*---------------------------------------------------------------------------
 *****  Macro to initialize the contents of a 'Scalar'.
 *
 *  Arguments:
 *	pToken			- a handle for the reference P-Token associated
 *				  with this scalar
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
#define DSC_InitScalar(pToken,rType,field,value) {\
    pToken->retain ();\
    m_xRType	= rType;\
    m_pRPT	= (pToken);\
    field(*this)= (value);\
}


/**********************
 *  Class Definition  *
 **********************/

class DSC_Scalar {
//  Construction/Initialization
public:
    void constructFrom (DSC_Scalar const &rOther) {
	*this = rOther;
	m_pRPT->retain ();
    }
    void constructFrom (rtPTOKEN_Handle *pRPT, RTYPE_Type xRType, DSC_ScalarValue const &rValue) {
	pRPT->retain ();
	m_pRPT	 = pRPT;
	m_xRType = xRType;
	m_iValue = rValue;
    }
    void constructComposition (unsigned int xSubscript, M_CPD *pSource) {
	constructComposition (xSubscript, pSource->containerHandle ());
    }
    void constructComposition (unsigned int xSubscript, VContainerHandle *pSource);

    void constructReference (rtPTOKEN_Handle *pRPT, unsigned int xReference) {
        if (xReference > pRPT->cardinality ()) {
	    rtREFUV_ComplainAboutElementVal (xReference, pRPT->cardinality ());
	}
	pRPT->retain ();
	m_pRPT		= pRPT;
        m_xRType	= RTYPE_C_RefUV;
	m_iValue.asInt	= xReference;
    }

    void constructValue (rtPTOKEN_Handle *pRPT) {
	DSC_InitScalar (pRPT, RTYPE_C_UndefUV, DSC_Scalar_Int, 0)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, char iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_CharUV, DSC_Scalar_Char, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, double iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_DoubleUV, DSC_Scalar_Double, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, float iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_FloatUV, DSC_Scalar_Float, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, int iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_IntUV, DSC_Scalar_Int, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, VkUnsigned64 const &iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_Unsigned64UV, DSC_Scalar_Unsigned64, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, VkUnsigned96 const &iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_Unsigned96UV, DSC_Scalar_Unsigned96, iValue)
    }
    void constructValue (rtPTOKEN_Handle *pRPT, VkUnsigned128 const &iValue) {
	DSC_InitScalar (pRPT, RTYPE_C_Unsigned128UV, DSC_Scalar_Unsigned128, iValue)
    }

//  Destruction
public:
    void destroy () {
	m_pRPT->release ();
    }

//  Access
public:
    static rtPTOKEN_Handle *PPT () {
	return M_AttachedNetwork->TheScalarPTokenHandle ();
    }
    rtPTOKEN_Handle *RPT () const {
	return m_pRPT;
    }
    unsigned int RPTCardinality () const {
	return m_pRPT->cardinality ();
    }
    RTYPE_Type RType () const {
	return m_xRType;
    }
    RTYPE_Type RTypeOfContainer () const {
	return m_xRType != RTYPE_C_RefUV ? m_xRType : RTYPE_C_Link;
    }

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
    M_CPD *asContainer (rtPTOKEN_Handle *pPPT);
    M_CPD *asCoercedContainer (rtPTOKEN_Handle *pPPT);

    M_CPD *asUVector ();

//  Update
public:
    void setRPT (rtPTOKEN_Handle *pRPT) {
	pRPT->retain  ();
	m_pRPT->release ();
	m_pRPT = pRPT;
    }

//  State
private:
    RTYPE_Type		m_xRType;
    rtPTOKEN_Handle*	m_pRPT;
public:
    DSC_ScalarValue	m_iValue;
};


#endif

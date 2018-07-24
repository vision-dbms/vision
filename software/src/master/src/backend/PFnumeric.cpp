/*****  Numeric Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfNUMERIC

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

extern "C" {
    typedef double (__cdecl *VkMathUnary) (double);
}

/*****  Facility  *****/
#include "m.h"
#include "selector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"

#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTptoken.h"
#include "RTlink.h"
#include "RTvstore.h"

#include "VNumericBinary.h"

/*****  Self  *****/
#include "PFnumeric.h"


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
 *	210 - 275                       *
 *======================================*/


/***********************************
 *****  Conversion Primitives  *****
 ***********************************/

#define XDoubleAsFloat				210
#define XDoubleAsInteger			211
#define XFloatAsDouble				212
#define XFloatAsInteger				213
#define XIntegerAsDouble			214
#define XIntegerAsFloat				215


/**************************************************
 *****  Arithmetic And Relational Primitives  *****
 **************************************************/

#define XIntegerAdd				220
#define XIntegerSubtract			221
#define XIntegerSubtractFrom			222
#define XIntegerMultiply			223
#define XIntegerDivide				224
#define XIntegerDivideInto			225
#define XIntegerLessThan			226
#define XIntegerLessThanOrEqual			227
#define XIntegerEqual				228

#define XCompareValues				229

/*****
 *  230 - 239 are assigned below to transcendental primitives.
 *****/

#define XFloatAdd				240
#define XFloatSubtract				241
#define XFloatSubtractFrom			242
#define XFloatMultiply				243
#define XFloatDivide				244
#define XFloatDivideInto			245
#define XFloatLessThan				246
#define XFloatLessThanOrEqual			247
#define XFloatEqual				248

#define XIntegerNotEqual			250
#define XIntegerGreaterThanOrEqual		251
#define XIntegerGreaterThan			252

#define XFloatNotEqual				253
#define XFloatGreaterThanOrEqual		254
#define XFloatGreaterThan			255

#define XDoubleNotEqual				256
#define XDoubleGreaterThanOrEqual		257
#define XDoubleGreaterThan			258

#define XDoubleAdd				260
#define XDoubleSubtract				261
#define XDoubleSubtractFrom			262
#define XDoubleMultiply				263
#define XDoubleDivide				264
#define XDoubleDivideInto			265
#define XDoubleLessThan				266
#define XDoubleLessThanOrEqual			267
#define XDoubleEqual				268


/*************************************************
 *****  Transcendental and Power Primitives  *****
 *************************************************/

#define XSine					230
#define XCosine					231
#define XTangent				232
#define XArcSine				233
#define XArcCosine				234
#define XArcTangent				235
#define XHyperbolicSine				236
#define XHyperbolicCosine			237
#define XHyperbolicTangent			238

/*****
 *  240 - 268 are assigned above to arithmetic and relational primitives.
 *****/

#define XDoubleExp				269
#define XDoubleLog				270
#define XDoubleSqrt				272
#define XDoublePow				271
#define XDoubleWop				273


/******************************************
 ******************************************
 *****  Error Handlers and Utilities  *****
 ******************************************
 ******************************************/

#if defined(__linux__) || defined(__APPLE__) || (defined (__HP_aCC) && __cplusplus >= 199707L)
#define USING_FAKE_MATHERR
struct exception {
    int type;
    char *name;
    double arg1;
    double arg2;
    double retval;
};
#endif

/*---------------------------------------------------------------------------
 *****  Run-time library matherr override.
 *
 *  Arguments:
 *	x		- a structure containing information about the type
 *                        of error that occurred.
 *
 *  Returns:
 *	true to instruct the run-time library that the error has been handled.
 *
 *****/
#if !defined(__VMS)
int __cdecl matherr (
#if defined(_WIN32)
    struct  _exception *pException
#elif defined(USING_FAKE_MATHERR)
    struct   exception *pException
#else
    struct __exception *pException
#endif
) {
    static const double iNaN (NaNQ);
    if (isfinite (pException->retval))
	pException->retval = iNaN;
    return true;
}
#endif	// !defined(__VMS)


/***********************************
 ***********************************
 *****  Conversion Primitives  *****
 ***********************************
 ***********************************/

/**************************************
 *  Convert From 'double' Primitives  *
 **************************************/

V_DefinePrimitive (DoubleAsFloat) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ())
    	pTask->loadDucWithFloat ((float)DSC_Descriptor_Scalar_Double (rCurrent));
    else {
	M_CPD* uv = rtDOUBLEUV_AsFloatUV (pTask->codKOT(), DSC_Descriptor_Value (rCurrent));
	pTask->loadDucWithMonotype (uv);
	uv->release ();
    }
}

V_DefinePrimitive (DoubleAsInteger) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isntScalar ()) {
	rtLINK_CType* lc;
	M_CPD* uv = rtDOUBLEUV_AsIntUV (
	    pTask->codKOT (), DSC_Descriptor_Value (rCurrent), &lc
	);
	pTask->loadDucWithPartialFunction (lc, uv);
	if (lc)
	    lc->release ();
	uv->release ();
    }
    else if (rtINTUV_IsAValidInteger (DSC_Descriptor_Scalar_Double (rCurrent)))
    	pTask->loadDucWithInteger ((int)DSC_Descriptor_Scalar_Double (rCurrent));
    else
	pTask->loadDucWithNA ();
}


/*************************************
 *  Convert From 'float' Primitives  *
 *************************************/

V_DefinePrimitive (FloatAsDouble) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ())
    	pTask->loadDucWithDouble ((double)DSC_Descriptor_Scalar_Float (rCurrent));
    else {
	M_CPD* uv = rtFLOATUV_AsDoubleUV (pTask->codKOT(), DSC_Descriptor_Value (rCurrent));
	pTask->loadDucWithMonotype (uv);
	uv->release ();
    }
}

V_DefinePrimitive (FloatAsInteger) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isntScalar ()) {
	rtLINK_CType* lc;
	M_CPD* uv = rtFLOATUV_AsIntUV (
	    pTask->codKOT (), DSC_Descriptor_Value (rCurrent), &lc
	);
	pTask->loadDucWithPartialFunction (lc, uv);
	if (lc)
	    lc->release ();
	uv->release ();
    }
    else if (rtINTUV_IsAValidInteger (DSC_Descriptor_Scalar_Float (rCurrent)))
    	pTask->loadDucWithInteger ((int)DSC_Descriptor_Scalar_Float (rCurrent));
    else
	pTask->loadDucWithNA ();
}


/***********************************
 *  Convert From 'int' Primitives  *
 ***********************************/

V_DefinePrimitive (IntegerAsDouble) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ())
    	pTask->loadDucWithDouble ((double)DSC_Descriptor_Scalar_Int (rCurrent));
    else {
	M_CPD* uv = rtINTUV_AsDoubleUV (pTask->codKOT(), DSC_Descriptor_Value (rCurrent));
	pTask->loadDucWithMonotype (uv);
	uv->release ();
    }
}

V_DefinePrimitive (IntegerAsFloat) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ())
    	pTask->loadDucWithFloat ((float)DSC_Descriptor_Scalar_Int (rCurrent));
    else {
	M_CPD* uv = rtINTUV_AsFloatUV (pTask->codKOT(), DSC_Descriptor_Value (rCurrent));
	pTask->loadDucWithMonotype (uv);
	uv->release ();
    }
}


/***********************************
 ***********************************
 *****  Arithmetic Primitives  *****
 ***********************************
 ***********************************/

/*****************
 *****  Add  *****
 *****************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(a) + (double)(b))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryAdd : public VNumericBinary {
//  Construction
public:
    VNumericBinaryAdd () : VNumericBinary (KS__Plus) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iAddOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (Add)
{
    pTask->process (g_iAddOperator);
}


/**********************
 *****  Subtract  *****
 **********************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(a) - (double)(b))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinarySubtract : public VNumericBinary {
//  Construction
public:
    VNumericBinarySubtract () : VNumericBinary (KS__SubtractedFrom) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }


    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iSubtractOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (Subtract)
{
    pTask->process (g_iSubtractOperator);
}


/**************************
 *****  SubtractFrom  *****
 **************************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(b) - (double)(a))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinarySubtractedFrom : public VNumericBinary {
//  Construction
public:
    VNumericBinarySubtractedFrom () : VNumericBinary (KS__Minus) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iSubtractFromOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (SubtractFrom)
{
    pTask->process (g_iSubtractFromOperator);
}


/**********************
 *****  Multiply  *****
 **********************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(a) * (double)(b))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryMultiply : public VNumericBinary {
//  Construction
public:
    VNumericBinaryMultiply () : VNumericBinary (KS__Times) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iMultiplyOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (Multiply)
{
    pTask->process (g_iMultiplyOperator);
}


/********************
 *****  Divide  *****
 ********************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(a) / (double)(b))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryDivide : public VNumericBinary {
//  Construction
public:
    VNumericBinaryDivide () : VNumericBinary (KS__DividedInto) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iDivideOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (Divide)
{
    pTask->process (g_iDivideOperator);
}


/************************
 *****  DivideInto  *****
 ************************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) ((double)(b) / (double)(a))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryDividedInto : public VNumericBinary {
//  Construction
public:
    VNumericBinaryDividedInto () : VNumericBinary (KS__Divide) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFD (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFF (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeFI (
	ResultGenerator&	rResultGenerator,
	float const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeID (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeIF (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeII (
	ResultGenerator&	rResultGenerator,
	int const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iDividedIntoOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (DivideInto)
{
    pTask->process (g_iDividedIntoOperator);
}


/***********************************
 ***********************************
 *****  Relational Primitives  *****
 ***********************************
 ***********************************/

/******************************
 *****  Supporting Types  *****
 ******************************/

enum ComparisonResultCase {
    LessThan,
    LessThanOrEqual,
    Equal,
    NotEqual,
    GreaterThanOrEqual,
    GreaterThan
};

enum ComparisonExecutionCase {
    NotHandled,
    IntegerInteger,
    IntegerFloat,
    IntegerDouble,
    FloatInteger,
    FloatFloat,
    FloatDouble,
    DoubleInteger,
    DoubleFloat,
    DoubleDouble,
    Unsigned64Unsigned64,
    Unsigned96Unsigned96,
    Unsigned128Unsigned128
};


/***********************
 *****  Utilities  *****
 ***********************/

PrivateFnDef void ComplainAboutRelationalAlias (VPrimitiveTask* pTask)
{
    pTask->raiseUnimplementedAliasException ("CompareValues");
}

PrivateFnDef void ComplainAboutResultCase (
    VPrimitiveTask* pTask, ComparisonResultCase xResultCase
)
{
    pTask->raiseException (
	EC__InternalInconsistency,
	"CompareValues: Invalid Result Case %u",
	xResultCase
    );
}

PrivateFnDef void ComplainAboutExecutionCase (
    VPrimitiveTask* pTask, ComparisonExecutionCase xExecutionCase
)
{
    pTask->raiseException (
	EC__InternalInconsistency,
	"CompareValues: Invalid Execution Case %u",
	xExecutionCase
    );
}


PrivateFnDef bool CompareValuesOperandsAreConformant (DSC_Descriptor &rCurrent, DSC_Descriptor &rArgument) {
    Vdd::Store *pReceiverStore = rCurrent.store ();
    Vdd::Store *pArgumentStore = rArgument.store ();

    return pReceiverStore->isACloneOf (pArgumentStore) || (
	pReceiverStore->NamesABuiltInNumericClass () && pArgumentStore->NamesABuiltInNumericClass ()
    );
}


PrivateFnDef void SendComparisonConverse (
    VPrimitiveTask* pTask, ComparisonResultCase xResultCase
) {
    int converse;

    switch (xResultCase) {
    case LessThan:
	converse = KS__GreaterThan;
	break;
    case LessThanOrEqual:
	converse = KS__GreaterThanOrEqual;
	break;
    case Equal:
	converse = KS__Equal;
	break;
    case NotEqual:
	converse = KS__NotEqual;
	break;
    case GreaterThanOrEqual:
	converse = KS__LessThanOrEqual;
	break;
    case GreaterThan:
	converse = KS__LessThan;
	break;
    default:
	ComplainAboutResultCase (pTask, xResultCase);
	break;
    }

    pTask->sendBinaryConverseWithCurrent (converse);
}


/****************************
 *****  Primitive Body  *****
 ****************************/

PrivateFnDef void DoComparison (
    VPrimitiveTask*		pTask,
    ComparisonResultCase	xResultCase,
    ComparisonExecutionCase	xExecutionCase
) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Compute the result:  *****/
    if (DucIsAScalar) {
	bool predicateResult;

	switch (xExecutionCase) {


/****************************
 *  Scalar Integer-Integer  *
 ****************************/

	case IntegerInteger:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/**************************
 *  Scalar Integer-Float  *
 **************************/

	case IntegerFloat:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/***************************
 *  Scalar Integer-Double  *
 ***************************/

	case IntegerDouble:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Int		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/**************************
 *  Scalar Float-Integer  *
 **************************/

	case FloatInteger:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/************************
 *  Scalar Float-Float  *
 ************************/

	case FloatFloat:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/*************************
 *  Scalar Float-Double  *
 *************************/

	case FloatDouble:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		==
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Float		(rCurrent)
		>
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/***************************
 *  Scalar Double-Integer  *
 ***************************/

	case DoubleInteger:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Int		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/*************************
 *  Scalar Double-Float  *
 *************************/

	case DoubleFloat:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Float		(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/**************************
 *  Scalar Double-Double  *
 **************************/

	case DoubleDouble:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Double	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Double	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/**********************************
 *  Scalar Unsigned64-Unsigned64  *
 **********************************/

	case Unsigned64Unsigned64:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned64	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Unsigned64	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/**********************************
 *  Scalar Unsigned96-Unsigned96  *
 **********************************/

	case Unsigned96Unsigned96:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned96	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Unsigned96	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/************************************
 *  Scalar Unsigned128-Unsigned128  *
 ************************************/

	case Unsigned128Unsigned128:
	    switch (xResultCase) {
	    case LessThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		<
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    case LessThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		<=
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    case Equal:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		==
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    case NotEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		!=
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    case GreaterThanOrEqual:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		>=
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    case GreaterThan:
		predicateResult =
		    DSC_Descriptor_Scalar_Unsigned128	(rCurrent)
		>
		    DSC_Descriptor_Scalar_Unsigned128	(ADescriptor);
		break;
	    default:
		ComplainAboutResultCase (pTask, xResultCase);
		break;
	    }
	    break;


/********************
 *  Scalar Wrap Up  *
 ********************/

	default:
	    ComplainAboutExecutionCase (pTask, xExecutionCase);
	    break;
	}

	pTask->loadDucWithBoolean (predicateResult);
    }
    else {


/**********************
 *  Non-Scalar Setup  *
 **********************/

	rtLINK_CType *trueLink  = pTask->NewSubset ();
	rtLINK_CType *falseLink = pTask->NewSubset ();
	rtLINK_CType *lessThanLink, *equalToLink, *greaterThanLink, *thisLink, *lastLink;
        unsigned int origin, count, i, n;
	union cursor_t {
	    void		*vArray;
	    int			*iArray;
	    float		*fArray;
	    double		*dArray;
	    VkUnsigned64	*u64Array;
	    VkUnsigned96	*u96Array;
	    VkUnsigned128	*u128Array;
	} a, b;

	switch (xResultCase) {
	case LessThan:
	    lessThanLink	= trueLink;
	    equalToLink		=
	    greaterThanLink	= falseLink;
	    break;
	case LessThanOrEqual:
	    lessThanLink	=
	    equalToLink		= trueLink;
	    greaterThanLink	= falseLink;
	    break;
	case Equal:
	    equalToLink		= trueLink;
	    lessThanLink	=
	    greaterThanLink	= falseLink;
	    break;
	case NotEqual:
	    equalToLink		= falseLink;
	    lessThanLink	=
	    greaterThanLink	= trueLink;
	    break;
	case GreaterThanOrEqual:
	    lessThanLink	= falseLink;
	    equalToLink		=
	    greaterThanLink	= trueLink;
	    break;
	case GreaterThan:
	    lessThanLink	=
	    equalToLink		= falseLink;
	    greaterThanLink	= trueLink;
	    break;
	default:
	    ComplainAboutResultCase (pTask, xResultCase);
	    break;
	}

	lastLink	= trueLink;
	origin		=
	count		= 0;

	if (!rCurrent.holdsNonScalarValues () || !ADescriptor.holdsNonScalarValues ()) {
	    pTask->raiseException (
		EC__UsageError,
		"CompareValues: Unexpected Descriptor Type (%d:%d)",
		rCurrent.pointerType (), ADescriptor.pointerType ()
	    );
	}
	a.vArray = UV_CPD_Array (DSC_Descriptor_Value (rCurrent   ), void);
	b.vArray = UV_CPD_Array (DSC_Descriptor_Value (ADescriptor), void);

	for (i = 0, n = pTask->cardinality (); i < n; i++) {
	    switch (xExecutionCase) {
	    case IntegerInteger:
		thisLink =
		    a.iArray[i] < b.iArray[i]
		    ? lessThanLink
		    : a.iArray[i] == b.iArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case IntegerFloat:
		thisLink =
		    a.iArray[i] < b.fArray[i]
		    ? lessThanLink
		    : a.iArray[i] == b.fArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case IntegerDouble:
		thisLink =
		    a.iArray[i] < b.dArray[i]
		    ? lessThanLink
		    : a.iArray[i] == b.dArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case FloatInteger:
		thisLink =
		    a.fArray[i] < b.iArray[i]
		    ? lessThanLink
		    : a.fArray[i] == b.iArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case FloatFloat:
		thisLink =
		    a.fArray[i] < b.fArray[i]
		    ? lessThanLink
		    : a.fArray[i] == b.fArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case FloatDouble:
		thisLink =
		    a.fArray[i] < b.dArray[i]
		    ? lessThanLink
		    : a.fArray[i] == b.dArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case DoubleInteger:
		thisLink =
		    a.dArray[i] < b.iArray[i]
		    ? lessThanLink
		    : a.dArray[i] == b.iArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case DoubleFloat:
		thisLink =
		    a.dArray[i] < b.fArray[i]
		    ? lessThanLink
		    : a.dArray[i] == b.fArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case DoubleDouble:
		thisLink =
		    a.dArray[i] < b.dArray[i]
		    ? lessThanLink
		    : a.dArray[i] == b.dArray[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case Unsigned64Unsigned64:
		thisLink =
		    a.u64Array[i] < b.u64Array[i]
		    ? lessThanLink
		    : a.u64Array[i] == b.u64Array[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case Unsigned96Unsigned96:
		thisLink =
		    a.u96Array[i] < b.u96Array[i]
		    ? lessThanLink
		    : a.u96Array[i] == b.u96Array[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    case Unsigned128Unsigned128:
		thisLink =
		    a.u128Array[i] < b.u128Array[i]
		    ? lessThanLink
		    : a.u128Array[i] == b.u128Array[i]
		    ? equalToLink
		    : greaterThanLink;
		break;
	    default:
		ComplainAboutExecutionCase (pTask, xExecutionCase);
		break;
	    }

	    if (thisLink == lastLink)
		count++;
	    else {
		if (count > 0) {
		    rtLINK_AppendRange (lastLink, origin, count);
		    origin	+= count;
		}
		count		= 1;
		lastLink	= thisLink;
	    }
	}


/************************
 *  Non-Scalar Wrap Up  *
 ************************/

	if (count > 0)
	    rtLINK_AppendRange (lastLink, origin, count);

	pTask->loadDucWithPredicateResult (trueLink, falseLink);
    }
}


/**************************************
 *****  CompareNumbers Primitive  *****
 **************************************/

V_DefinePrimitive (CompareNumbers) {
/*****  Determine the result case, ...  *****/
    ComparisonResultCase xResultCase;
    switch (V_TOTSC_Primitive) {
    case XIntegerLessThan:
    case XFloatLessThan:
    case XDoubleLessThan:
	xResultCase = LessThan;
	break;

    case XIntegerLessThanOrEqual:
    case XFloatLessThanOrEqual:
    case XDoubleLessThanOrEqual:
	xResultCase = LessThanOrEqual;
	break;

    case XIntegerEqual:
    case XFloatEqual:
    case XDoubleEqual:
	xResultCase = Equal;
	break;

    case XIntegerNotEqual:
    case XFloatNotEqual:
    case XDoubleNotEqual:
	xResultCase = NotEqual;
	break;

    case XIntegerGreaterThanOrEqual:
    case XFloatGreaterThanOrEqual:
    case XDoubleGreaterThanOrEqual:
	xResultCase = GreaterThanOrEqual;
	break;

    case XIntegerGreaterThan:
    case XFloatGreaterThan:
    case XDoubleGreaterThan:
	xResultCase = GreaterThan;
	break;

    default:
	ComplainAboutRelationalAlias (pTask);
	break;
    }


/*****  Get the Duc, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	SendComparisonConverse (pTask, xResultCase);
	return;
    }
    pTask->normalizeDuc ();


/*****  Determine the execution case, ...  *****/
    ComparisonExecutionCase xExecutionCase;

    Vdd::Store *pOperandStore = pTask->ducStore ();
    if (pOperandStore->NamesTheDoubleClass ()) {
	switch (V_TOTSC_Primitive) {
	case XIntegerLessThan:
	case XIntegerLessThanOrEqual:
	case XIntegerEqual:
	case XIntegerNotEqual:
	case XIntegerGreaterThanOrEqual:
	case XIntegerGreaterThan:
	    xExecutionCase = IntegerDouble;
	    break;

	case XFloatLessThan:
	case XFloatLessThanOrEqual:
	case XFloatEqual:
	case XFloatNotEqual:
	case XFloatGreaterThanOrEqual:
	case XFloatGreaterThan:
	    xExecutionCase = FloatDouble;
	    break;

	case XDoubleLessThan:
	case XDoubleLessThanOrEqual:
	case XDoubleEqual:
	case XDoubleNotEqual:
	case XDoubleGreaterThanOrEqual:
	case XDoubleGreaterThan:
	    xExecutionCase = DoubleDouble;
	    break;

	default:
	    ComplainAboutRelationalAlias (pTask);
	    break;
	}
    }


    else if (pOperandStore->NamesTheIntegerClass ()) {
	switch (V_TOTSC_Primitive) {
	case XIntegerLessThan:
	case XIntegerLessThanOrEqual:
	case XIntegerEqual:
	case XIntegerNotEqual:
	case XIntegerGreaterThanOrEqual:
	case XIntegerGreaterThan:
 	    xExecutionCase = IntegerInteger;
	    break;

	case XFloatLessThan:
	case XFloatLessThanOrEqual:
	case XFloatEqual:
	case XFloatNotEqual:
	case XFloatGreaterThanOrEqual:
	case XFloatGreaterThan:
	    xExecutionCase = FloatInteger;
	    break;

	case XDoubleLessThan:
	case XDoubleLessThanOrEqual:
	case XDoubleEqual:
	case XDoubleNotEqual:
	case XDoubleGreaterThanOrEqual:
	case XDoubleGreaterThan:
	    xExecutionCase = DoubleInteger;
	    break;

	default:
	    ComplainAboutRelationalAlias (pTask);
	    break;
	}
    }


    else if (pOperandStore->NamesTheFloatClass ()) {
	switch (V_TOTSC_Primitive) {
	case XIntegerLessThan:
	case XIntegerLessThanOrEqual:
	case XIntegerEqual:
	case XIntegerNotEqual:
	case XIntegerGreaterThanOrEqual:
	case XIntegerGreaterThan:
 	    xExecutionCase = IntegerFloat;
	    break;

	case XFloatLessThan:
	case XFloatLessThanOrEqual:
	case XFloatEqual:
	case XFloatNotEqual:
	case XFloatGreaterThanOrEqual:
	case XFloatGreaterThan:
	    xExecutionCase = FloatFloat;
	    break;

	case XDoubleLessThan:
	case XDoubleLessThanOrEqual:
	case XDoubleEqual:
	case XDoubleNotEqual:
	case XDoubleGreaterThanOrEqual:
	case XDoubleGreaterThan:
	    xExecutionCase = DoubleFloat;
	    break;

	default:
	    ComplainAboutRelationalAlias (pTask);
	    break;
	}
    }
    else {
	SendComparisonConverse (pTask, xResultCase);
	return;
    }


/*****  ... and compute the result:  *****/
    DoComparison (pTask, xResultCase, xExecutionCase);
}


/*************************************
 *****  CompareValues Primitive  *****
 *************************************/

V_DefinePrimitive (CompareValues) {
/*****  Determine the result case, ...  *****/
    ComparisonResultCase xResultCase = (ComparisonResultCase)V_TOTSC_PrimitiveFlags;

/*****  Acquire the operands, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	SendComparisonConverse (pTask, xResultCase);
	return;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    pTask->normalizeDuc ();

/*****  Determine the execution case, ...  *****/
    ComparisonExecutionCase xExecutionCase = NotHandled;
    if (CompareValuesOperandsAreConformant (rCurrent, ADescriptor)) {
	RTYPE_Type xReceiverValueType = rCurrent.pointerRType ();

	RTYPE_Type xArgumentValueType = ADescriptor.pointerRType ();

	switch (xReceiverValueType) {
	case RTYPE_C_DoubleUV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_DoubleUV:
		xExecutionCase = DoubleDouble;
		break;
	    case RTYPE_C_FloatUV:
		xExecutionCase = DoubleFloat;
		break;
	    case RTYPE_C_IntUV:
		xExecutionCase = DoubleInteger;
		break;
	    default:
		break;
	    }
	    break;
	case RTYPE_C_FloatUV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_DoubleUV:
		xExecutionCase = FloatDouble;
		break;
	    case RTYPE_C_FloatUV:
		xExecutionCase = FloatFloat;
		break;
	    case RTYPE_C_IntUV:
		xExecutionCase = FloatInteger;
		break;
	    default:
		break;
	    }
	    break;
	case RTYPE_C_IntUV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_DoubleUV:
		xExecutionCase = IntegerDouble;
		break;
	    case RTYPE_C_FloatUV:
		xExecutionCase = IntegerFloat;
		break;
	    case RTYPE_C_IntUV:
		xExecutionCase = IntegerInteger;
		break;
	    default:
		break;
	    }
	    break;
	case RTYPE_C_Unsigned64UV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_Unsigned64UV:
		xExecutionCase = Unsigned64Unsigned64;
		break;
	    default:
		break;
	    }
	    break;
	case RTYPE_C_Unsigned96UV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_Unsigned96UV:
		xExecutionCase = Unsigned96Unsigned96;
		break;
	    default:
		break;
	    }
	    break;
	case RTYPE_C_Unsigned128UV:
	    switch (xArgumentValueType) {
	    case RTYPE_C_Unsigned128UV:
		xExecutionCase = Unsigned128Unsigned128;
		break;
	    default:
		break;
	    }
	    break;
	default:
	    pTask->raiseException (
		EC__UsageError,
		"CompareValues: Unsupported Recipient Value Type: %s",
		RTYPE_TypeIdAsString (xReceiverValueType)
	    );
	    break;
	}
    }

/*****  ... and compute the result:  *****/
    if (NotHandled == xExecutionCase) {
	char const *pMessageName;

	switch (xResultCase) {
	case LessThan:
	    pMessageName = "lessThanNonConformantValue:";
	    break;
	case LessThanOrEqual:
	    pMessageName = "lessThanOrEqualToNonConformantValue:";
	    break;
	case Equal:
	    pMessageName = "equalToNonConformantValue:";
	    break;
	case NotEqual:
	    pMessageName = "notEqualToNonConformantValue:";
	    break;
	case GreaterThanOrEqual:
	    pMessageName = "greaterThanOrEqualToNonConformantValue:";
	    break;
	case GreaterThan:
	    pMessageName = "greaterThanNonConformantValue:";
	    break;
	default:
	    ComplainAboutResultCase (pTask, xResultCase);
	    break;
	}

	/*****  Retain the content of the current duc...  *****/
	VDescriptor iDelegationParameterDatum;
	iDelegationParameterDatum.setToMoved (pTask->duc ());

	/*****  ... and use that duc as the delegation message's parameter:  *****/
	pTask->beginMessageCall (pMessageName, 1);
	// Deliberately '^self', not '^current':
	pTask->commitRecipient (VMagicWord_Self);

	pTask->loadDucWithMoved (iDelegationParameterDatum);
	pTask->commitParameter ();

	pTask->commitCall ();
    }
    else DoComparison (pTask, xResultCase, xExecutionCase);
}


/************************************************
 ************************************************
 *****  Trancendental and Power Primitives  *****
 ************************************************
 ************************************************/

/***************************************
 *****  Transcendental Primitives  *****
 ***************************************/

V_DefinePrimitive (DoubleTranscendentals) {
    VkMathUnary fn;

    switch (V_TOTSC_Primitive) {
    case XDoubleExp:
	fn = exp;
	break;
    case XDoubleLog:
	fn = log;
        break;
    case XDoubleSqrt:
	fn = sqrt;
        break;
    case XSine:
	fn = sin;
	break;
    case XCosine:
	fn = cos;
	break;
    case XTangent:
	fn = tan;
	break;
    case XArcSine:
	fn = asin;
	break;
    case XArcCosine:
	fn = acos;
	break;
    case XArcTangent:
	fn = atan;
	break;
    case XHyperbolicSine:
	fn = sinh;
	break;
    case XHyperbolicCosine:
	fn = cosh;
	break;
    case XHyperbolicTangent:
	fn = tanh;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DoubleTranscendentals");
	break;
    }

/***** Setup ... *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (pTask->isScalar ())
	pTask->loadDucWithGuardedDouble (fn (DSC_Descriptor_Scalar_Double (rCurrent)));
    else {
	VGuardedDoubleGenerator iResultGenerator (pTask);
	unsigned int iCardinality = iResultGenerator.cardinality ();
	double const* pOperandCursor = rtDOUBLEUV_CPD_Array (DSC_Descriptor_Value (rCurrent));

	while (iCardinality-- > 0)
	    iResultGenerator.append (fn (*pOperandCursor++));

	iResultGenerator.commit ();
    }
}


/******************************
 ******************************
 *****  Power Primitives  *****
 ******************************
 ******************************/

/*****************
 *****  Pow  *****
 *****************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) pow ((double)(a), (double)(b))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryPow : public VNumericBinary {
//  Construction
public:
    VNumericBinaryPow () : VNumericBinary (KS__Wop) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iPowOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (DoublePow)
{
    pTask->process (g_iPowOperator);
}


/*****************
 *****  Wop  *****
 *****************/

/********************
 *  Operator Macro  *
 ********************/

#define op(a,b) pow ((double)(b), (double)(a))


/**********************
 *  Class Definition  *
 **********************/

class VNumericBinaryWop : public VNumericBinary {
//  Construction
public:
    VNumericBinaryWop () : VNumericBinary (KS__Pow) {
    }

//  Computation
protected:
    void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int const*		pOperand2Vector
    )
    {
	unsigned int iCardinality = rResultGenerator.cardinality ();
	for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	    rResultGenerator.append (
		op (pOperand1Vector[xElement], pOperand2Vector[xElement])
	    );
    }

    double value (double iOperand1, double iOperand2) {
	return op (iOperand1, iOperand2);
    }
} g_iWopOperator;


/***************
 *  Primitive  *
 ***************/

#undef op

V_DefinePrimitive (DoubleWop)
{
    pTask->process (g_iWopOperator);
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


/***********************************
 *****  Conversion Primitives  *****
 ***********************************/

    PD (XDoubleAsFloat,
	"DoubleAsFloat",
	DoubleAsFloat)
    PD (XDoubleAsInteger,
	"DoubleAsInteger",
	DoubleAsInteger)
    PD (XFloatAsDouble,
	"FloatAsDouble",
	FloatAsDouble)
    PD (XFloatAsInteger,
	"FloatAsInteger",
	FloatAsInteger)
    PD (XIntegerAsDouble,
	"IntegerAsDouble",
	IntegerAsDouble)
    PD (XIntegerAsFloat,
	"IntegerAsFloat",
	IntegerAsFloat)


/**********************************************************
 *****  Numeric Arithmetic And Relational Primitives  *****
 **********************************************************/

    PD (XIntegerAdd,
	"IntegerAdd",
	Add)
    PD (XIntegerSubtract,
	"IntegerSubtract",
	Subtract)
    PD (XIntegerSubtractFrom,
	"IntegerSubtractFrom",
	SubtractFrom)
    PD (XIntegerMultiply,
	"IntegerMultiply",
	Multiply)
    PD (XIntegerDivide,
	"IntegerDivide",
	Divide)
    PD (XIntegerDivideInto,
	"IntegerDivideInto",
	DivideInto)

    PD (XCompareValues,
	"CompareValues",
	CompareValues)

    PD (XIntegerLessThan,
	"IntegerLessThan",
	CompareNumbers)
    PD (XIntegerLessThanOrEqual,
	"IntegerLessThanOrEqual",
	CompareNumbers)
    PD (XIntegerEqual,
	"IntegerEqual",
	CompareNumbers)
    PD (XIntegerNotEqual,
	"IntegerNotEqual",
	CompareNumbers)
    PD (XIntegerGreaterThanOrEqual,
	"IntegerGreaterThanOrEqual",
	CompareNumbers)
    PD (XIntegerGreaterThan,
	"IntegerGreaterThan",
	CompareNumbers)

    PD (XFloatAdd,
	"FloatAdd",
	Add)
    PD (XFloatSubtract,
	"FloatSubtract",
	Subtract)
    PD (XFloatSubtractFrom,
	"FloatSubtractFrom",
	SubtractFrom)
    PD (XFloatMultiply,
	"FloatMultiply",
	Multiply)
    PD (XFloatDivide,
	"FloatDivide",
	Divide)
    PD (XFloatDivideInto,
	"FloatDivideInto",
	DivideInto)

    PD (XFloatLessThan,
	"FloatLessThan",
	CompareNumbers)
    PD (XFloatLessThanOrEqual,
	"FloatLessThanOrEqual",
	CompareNumbers)
    PD (XFloatEqual,
	"FloatEqual",
	CompareNumbers)
    PD (XFloatNotEqual,
	"FloatNotEqual",
	CompareNumbers)
    PD (XFloatGreaterThanOrEqual,
	"FloatGreaterThanOrEqual",
	CompareNumbers)
    PD (XFloatGreaterThan,
	"FloatGreaterThan",
	CompareNumbers)


    PD (XDoubleAdd,
	"DoubleAdd",
	Add)
    PD (XDoubleSubtract,
	"DoubleSubtract",
	Subtract)
    PD (XDoubleSubtractFrom,
	"DoubleSubtractFrom",
	SubtractFrom)
    PD (XDoubleMultiply,
	"DoubleMultiply",
	Multiply)
    PD (XDoubleDivide,
	"DoubleDivide",
	Divide)
    PD (XDoubleDivideInto,
	"DoubleDivideInto",
	DivideInto)

    PD (XDoubleLessThan,
	"DoubleLessThan",
	CompareNumbers)
    PD (XDoubleLessThanOrEqual,
	"DoubleLessThanOrEqual",
	CompareNumbers)
    PD (XDoubleEqual,
	"DoubleEqual",
	CompareNumbers)
    PD (XDoubleNotEqual,
	"DoubleNotEqual",
	CompareNumbers)
    PD (XDoubleGreaterThanOrEqual,
	"DoubleGreaterThanOrEqual",
	CompareNumbers)
    PD (XDoubleGreaterThan,
	"DoubleGreaterThan",
	CompareNumbers)


/*************************************************
 *****  Transcendental and Power Primitives  *****
 *************************************************/

    PD (XDoubleExp,
	"DoubleExp",
	DoubleTranscendentals)
    PD (XDoubleLog,
	"DoubleLog",
	DoubleTranscendentals)
    PD (XDoubleSqrt,
	"DoubleSqrt",
	DoubleTranscendentals)
    PD (XSine,
	"Sine",
	DoubleTranscendentals)
    PD (XCosine,
	"Cosine",
	DoubleTranscendentals)
    PD (XTangent,
	"Tangent",
	DoubleTranscendentals)
    PD (XArcSine,
	"ArcSine",
	DoubleTranscendentals)
    PD (XArcCosine,
	"ArcCosine",
	DoubleTranscendentals)
    PD (XArcTangent,
	"ArcTangent",
	DoubleTranscendentals)
    PD (XHyperbolicSine,
	"HyperbolicSine",
	DoubleTranscendentals)
    PD (XHyperbolicCosine,
	"HyperbolicCosine",
	DoubleTranscendentals)
    PD (XHyperbolicTangent,
	"HyperbolicTangent",
	DoubleTranscendentals)

    PD (XDoublePow,
	"DoublePow",
	DoublePow)
    PD (XDoubleWop,
	"DoubleWop",
	DoubleWop)

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
PublicFnDef void pfNUMERIC_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfNUMERIC);
    FAC_FDFCase_FacilityDescription ("Numeric Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFnumeric.c 1 replace /users/jad/system
  861116 16:06:19 jad new numeric primitive functions

 ************************************************************************/
/************************************************************************
  PFnumeric.c 2 replace /users/jad/system
  861119 14:06:18 jad added log, exp, and sqrt primitives

 ************************************************************************/
/************************************************************************
  PFnumeric.c 3 replace /users/jad/system
  861120 15:51:11 jad add toThe:, asAPowerOf:, and NA handling for
divide and dividedInto

 ************************************************************************/
/************************************************************************
  PFnumeric.c 4 replace /users/mjc/translation
  870524 09:30:46 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFnumeric.c 5 replace /users/mjc/system
  870614 19:59:33 mjc Fix VAX compilation errors

 ************************************************************************/
/************************************************************************
  PFnumeric.c 6 replace /users/mjc/system
  870701 22:37:08 mjc Changed 'V_SendBinaryConverse' to 'V_SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  PFnumeric.c 7 replace /users/jck/system
  871210 12:31:01 jck Reorganized the PerformBinary* and PerformRelational* macros
so that they would not exceed VAX-C macro expansion limits

 ************************************************************************/
/************************************************************************
  PFnumeric.c 8 replace /users/mjc/Software/system
  871230 13:06:53 mjc Fixed uninitialized 'origin' variable in 'PerformRelationalCalculation

 ************************************************************************/
/************************************************************************
  PFnumeric.c 9 replace /users/jck/system
  881104 14:47:37 jck Added PerformRobustDivide macro

 ************************************************************************/
/************************************************************************
  PFnumeric.c 10 replace /users/jck/system
  890410 14:51:29 jck Discarded unneeded link constructors

 ************************************************************************/

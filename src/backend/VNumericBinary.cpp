/*****  VNumericBinary Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VNumericBinary.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VPrimitiveTask.h"

#include "RTvstore.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VNumericBinary::VNumericBinary (unsigned int xSelector) : m_xSelector (xSelector) {
}


/******************************
 ******************************
 *****  Default Handlers  *****
 ******************************
 ******************************/

void VNumericBinary::computeDD (
    ResultGenerator&	rResultGenerator,
    double const*	pOperand1Vector,
    double const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeDF (
    ResultGenerator&	rResultGenerator,
    double const*	pOperand1Vector,
    float const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeDI (
    ResultGenerator&	rResultGenerator,
    double const*	pOperand1Vector,
    int const*		pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeFD (
    ResultGenerator&	rResultGenerator,
    float const*	pOperand1Vector,
    double const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeFF (
    ResultGenerator&	rResultGenerator,
    float const*	pOperand1Vector,
    float const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeFI (
    ResultGenerator&	rResultGenerator,
    float const*	pOperand1Vector,
    int const*		pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeID (
    ResultGenerator&	rResultGenerator,
    int const*		pOperand1Vector,
    double const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeIF (
    ResultGenerator&	rResultGenerator,
    int const*		pOperand1Vector,
    float const*	pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

void VNumericBinary::computeII (
    ResultGenerator&	rResultGenerator,
    int const*		pOperand1Vector,
    int const*		pOperand2Vector
)
{
    unsigned int iCardinality = rResultGenerator.cardinality ();
    for (unsigned int xElement = 0; xElement < iCardinality; xElement++)
	rResultGenerator.append (
	    value (pOperand1Vector[xElement], pOperand2Vector[xElement])
	);
}

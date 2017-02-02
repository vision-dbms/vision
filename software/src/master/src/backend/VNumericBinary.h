#ifndef VNumericBinary_Interface
#define VNumericBinary_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VGuardedDoubleGenerator.h"


/************************
 *****  Definition  *****
 ************************/

class VNumericBinary : public VTransient {
//  Types
public:
    typedef VGuardedDoubleGenerator ResultGenerator;

//  Construction
public:
    VNumericBinary (unsigned int xSelector);

//  Access
public:
    unsigned int selector () const { return m_xSelector; }

//  Computation
public:
    virtual void computeDD (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	double const*		pOperand2Vector
    );
    virtual void computeDF (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	float  const*		pOperand2Vector
    );
    virtual void computeDI (
	ResultGenerator&	rResultGenerator,
	double const*		pOperand1Vector,
	int   const*		pOperand2Vector
    );

    virtual void computeFD (
	ResultGenerator&	rResultGenerator,
	float  const*		pOperand1Vector,
	double const*		pOperand2Vector
    );
    virtual void computeFF (
	ResultGenerator&	rResultGenerator,
	float  const*		pOperand1Vector,
	float  const*		pOperand2Vector
    );
    virtual void computeFI (
	ResultGenerator&	rResultGenerator,
	float  const*		pOperand1Vector,
	int    const*		pOperand2Vector
    );

    virtual void computeID (
	ResultGenerator&	rResultGenerator,
	int    const*		pOperand1Vector,
	double const*		pOperand2Vector
    );
    virtual void computeIF (
	ResultGenerator&	rResultGenerator,
	int    const*		pOperand1Vector,
	float  const*		pOperand2Vector
    );
    virtual void computeII (
	ResultGenerator&	rResultGenerator,
	int    const*		pOperand1Vector,
	int    const*		pOperand2Vector
    );

    virtual double value (double iOperand1Value, double iOperand2Value) = 0;

//  State
protected:
    unsigned int const m_xSelector;
};


#endif

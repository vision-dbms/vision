/*****  VArray Definitions  *****/
#ifndef VArray_Definition
#define VArray_Definition

/******************************************
 *****  Declarations and Definitions  *****
 ******************************************/

#include "VStdLib.h"


/**************************************************
 *****  VArrayDimensionData Class Definition  *****
 **************************************************/

class VArrayDimensionData {
/*****  Construction  *****/
public:
    VArrayDimensionData () {
	m_sVector = 0;
	m_sMatrix = 1;
    }

/*****  Counts and Offsets  *****/
public:
    size_t arraySize () const {
	return m_sVector * m_sMatrix;
    }
    size_t elementSize () const {
	return m_sMatrix;
    }
    size_t elementCount () const {
	return m_sVector;
    }
    unsigned int elementOffset (unsigned int xSubscript) const {
	return xSubscript * m_sMatrix;
    }

/*****  State  *****/
public:
    size_t m_sVector;
    size_t m_sMatrix;
};


/*****************************************
 *****  VArrayBase Class Definition  *****
 *****************************************/

class VArrayBase {
/*****  Construction/Destruction/Initialization  *****/
protected:
    VArrayBase  (
	size_t sDimensionVector, VArrayDimensionData const* pDimensionVector = 0
    );
    ~VArrayBase ();

/*****  Predicates  *****/
    bool conformsTo (VArrayBase const* pOther) const {
	return conformsTo (*pOther);
    }
    bool conformsTo (VArrayBase const& iOther) const;

/*****  Counts and Offsets  *****/
public:
    size_t dimensionCount () const {
	return m_sDimensionVector;
    }
    size_t arraySize (unsigned int xDimension = 0) const {
	return m_sDimensionVector > xDimension ? m_pDimensionVector[
	    xDimension
	].arraySize () : 0;
    }
    size_t elementSize (unsigned int xDimension) const {
	return m_sDimensionVector > xDimension ? m_pDimensionVector[
	    xDimension
	].elementSize () : 0;
    }
    size_t elementCount (unsigned int xDimension) const {
	return m_sDimensionVector > xDimension ? m_pDimensionVector[
	    xDimension
	].elementCount () : 0;
    }

public:
    unsigned int elementOffset (unsigned int xSubscript0, ...) const {
	va_list pSubscriptVector;
	va_start (pSubscriptVector, xSubscript0);
	return elementOffset (pSubscriptVector, xSubscript0);
    }
    unsigned int elementOffset (
	va_list pSubscriptVector, unsigned int xSubscript0
    ) const;
    unsigned int elementOffset (unsigned int const* pSubscriptVector) const;

/*****  Re-shaping and Re-sizing  *****/
public:
    void reshape (VArrayBase const& iOtherArray);

    void reshape (size_t sDimensionVector, ...) {
	va_list pDimensionVector;
	va_start (pDimensionVector, sDimensionVector);
	reshape (sDimensionVector, pDimensionVector);
    }
    void reshape (size_t sDimensionVector, va_list pDimensionVector);

    void reshape (size_t sDimensionVector, size_t const *pDimensionVector);

    void clear ();

protected:
    void resizeDimensionVector (size_t sNewDimensionVector);

protected:
    void guarantee (unsigned int xElement);

    virtual void fixVector (size_t sOldElementVector) = 0;

private:
    void fixCounts ();

//  COM Support
#if defined(WIN32_COM_SERVER)
protected:
    bool NewSafeArray (VARTYPE xElementType, SAFEARRAY**ppArray, void**ppElements) const;

public:
    virtual bool getElements (VARIANT* pElementArray, bool bReturningUnicode) const;

    bool getArrayOfVariants (SAFEARRAY**ppArray, bool bReturningUnicode) const;

    bool getArray (VARIANT* pVariant, bool bReturningUnicode) const;
#endif

/*****  State  *****/
protected:
    size_t		 m_sDimensionVector;
    VArrayDimensionData* m_pDimensionVector;
};


/*************************************
 *****  VArray Class Definition  *****
 *************************************/

template <class E> class VArray : public VArrayBase {
/*****  Construction/Destruction/Initialization  *****/
public:
    VArray (VArray<E> const& iOther);
    VArray (size_t sDimensionVector = 1);

    ~VArray ();

private:
    void initialize ();

/*****  Comparison  *****/
public:
    bool operator== (VArray<E> const &iOther) const;
    bool operator!= (VArray<E> const &iOther) const;

/*****  Access  *****/
public:
    E& element (unsigned int xSubscript0, ...) const;

    E& element (unsigned int const* pSubscriptVector) const;

    E& operator[] (unsigned int xElement) const;

/*****  Update  *****/
public:
    VArray<E>& operator= (VArray<E> const& iOther);

    VArray<E>& operator<< (E const& iElement);

    E& newElement ();

/*****  Resizing  *****/
protected:
    virtual void fixVector (size_t sOldElementVector);

/*****  Formatting  *****/
public:
    void formatForVision (
	V::VString&	iExpression,
	char const*	pKeyword,
	bool		fQuote = true,
	bool		fClose = true,
	char const*	pSeparator = ", "
    ) const;

/*****  State  *****/
protected:
    E* m_pElementVector;
};


/*****************************************
 *****  VArray Template Definitions  *****
 *****************************************/

#if defined(_AIX)
#pragma implementation("VArray.d")
#else
#include "VArray.d"
#endif


#endif

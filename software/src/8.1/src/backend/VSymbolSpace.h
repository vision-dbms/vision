#ifndef VSymbolSpace_Interface
#define VSymbolSpace_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkSetOfFactoryReferences.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VSymbol.h"


/*************************
 *****  Definitions  *****
 *************************/

class VSymbolSpace : public VTransient {
//  Construction
public:
    VSymbolSpace ();

//  Destruction
public:
    ~VSymbolSpace ();

//  Access/Query/Display
public:
    unsigned int cardinality () const {
	return m_iSymbolSet.cardinality ();
    }
    VSymbol *operator[] (unsigned int xSymbol) const {
	return xSymbol < cardinality () ? m_iSymbolSet[xSymbol].referent () : 0;
    }
    void DisplayBindings () const;

//  Symbol Lookup
public:
    VSymbol *locate (char const *pString) {
	unsigned int xSymbol;
	return m_iSymbolSet.Locate (pString, xSymbol) ? (VSymbol*)m_iSymbolSet[xSymbol] : 0;
    }

//  Symbol Insertion
public:
    VSymbol *locateOrAdd (char const* pString) {
	unsigned int xSymbol;
	m_iSymbolSet.Insert (pString, xSymbol);
	return m_iSymbolSet[xSymbol];
    }
    VSymbol *operator[] (char const* pString) {
	return locateOrAdd (pString);
    }

//  Symbol Deletion
public:
    unsigned int Delete (char const* pString) {
	return m_iSymbolSet.Delete (pString);
    }

//  State
protected:
    VkSetOfFactoryReferences<VSymbol> m_iSymbolSet;
};


#endif

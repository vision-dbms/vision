#ifndef VSymbol_Interface
#define VSymbol_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

class VSymbolBinding;
class VSymbolImplementation;

template<class ReferentType> class VFactoryReference;


/*************************
 *****  Definitions  *****
 *************************/

class VSymbolBindingHolder : public VReference<VSymbolBinding> {
    DECLARE_FAMILY_MEMBERS (VSymbolBindingHolder, VReference<VSymbolBinding>);

//  Construction
public:
    VSymbolBindingHolder (ThisClass const &rOther);
    VSymbolBindingHolder (VSymbolBinding *pOther);
    VSymbolBindingHolder ();

//  Destruction
public:
    ~VSymbolBindingHolder ();

//  Update
public:
    ThisClass &operator= (ThisClass const &rOther);
    ThisClass &operator= (VSymbolBinding *pOther);
};

class VSymbol : public VReferenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VSymbol, VReferenceable);

//  Factory Reference Type Parameters
public:
    typedef char const *Initializer;
    typedef char const *Key;

//  Friends
    friend class VFactoryReference<ThisClass>;

//  Construction
protected:
    VSymbol (char const* pName);

//  Destruction
private:
    ~VSymbol ();

//  Access
public:
    operator char const* () const {
	return m_pName;
    }
    char const* name () const {
	return m_pName;
    }

    VSymbolBindingHolder& rootBindingHolder () {
	return m_pRootBinding;
    }
    VSymbolBinding* rootBinding () const {
	return m_pRootBinding;
    }

//  Query
public:
    bool operator<  (char const* pString) const {
	return strcmp (m_pName, pString) < 0;
    }
    bool operator<= (char const* pString) const {
	return strcmp (m_pName, pString) <= 0;
    }
    bool operator== (char const* pString) const {
	return strcmp (m_pName, pString) == 0;
    }
    bool operator!= (char const* pString) const {
	return strcmp (m_pName, pString) != 0;
    }
    bool operator>= (char const* pString) const {
	return strcmp (m_pName, pString) >= 0;
    }
    bool operator>  (char const* pString) const {
	return strcmp (m_pName, pString) > 0;
    }

//  Binding Management
protected:
    void createBinding (
	VRunTimeType *pType, VSymbolBindingHolder *&rpCandidateBindingHolder
    );
    VSymbolBinding *locateBinding (
	VRunTimeType const *pType, VSymbolBinding *&rpCandidateBinding
    ) const;

public:
    void set (VRunTimeType *pType, VSymbolImplementation const *pImplementation);
    void set (VRunTimeType &rType, VSymbolImplementation const *pImplementation) {
	set (&rType, pImplementation);
    }

    VSymbolImplementation const *implementationAt (VRunTimeType const *pType) const;

//  Display
public:
    void DisplayBindings () const;

//  State
protected:
    char const* const		m_pName;
    VSymbolBindingHolder	m_pRootBinding;
};


#endif

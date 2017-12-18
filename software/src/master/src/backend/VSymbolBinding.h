#ifndef VSymbolBinding_Interface
#define VSymbolBinding_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VkDynamicArrayOf.h"
#include "VSymbol.h"

class VSymbolImplementation;


/*************************
 *****  Definitions  *****
 *************************/

class VSymbolBinding : public VBenderenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VSymbolBinding, VBenderenceable);

//  Friends
    friend class VSymbol;

//  Aliases
public:
    typedef VRunTimeType::index_t index_t;
    typedef VRunTimeType::level_t level_t;

//  Construction
protected:
    VSymbolBinding (VRunTimeType *pRTT);

//  Destruction
private:
    ~VSymbolBinding () {
    }

//  Access
public:
    VRunTimeType *type () const {
	return m_pType;
    }
    VSymbolImplementation const *implementation () const {
	return m_pImplementation;
    }
    static level_t levelOf (VSymbolBinding const *pBinding) {
	return pBinding ? pBinding->leveL () : (level_t)USHRT_MAX;
    }
    level_t leveL () const {
	return m_pType->level ();
    }

    index_t subtypeBindingLB () const {
	return m_xSubtypeBindingLB;
    }
    index_t subtypeBindingUB () const {
	return m_xSubtypeBindingUB;
    }

protected:
    VSymbolBindingHolder &subtypeBindingHolder (index_t xSubtype);
    VSymbolBindingHolder &subtypeBindingHolder (VRunTimeType const *pSubtype) {
	return subtypeBindingHolder (pSubtype->index ());
    }

    VSymbolBinding *subtypeBinding (index_t xSubtype) const {
	return m_xSubtypeBindingLB <= xSubtype && xSubtype < m_xSubtypeBindingUB
	    ? m_iSubtypeBindings[xSubtype - m_xSubtypeBindingLB].operator->()
	    : 0;
    }
    VSymbolBinding *subtypeBinding (VRunTimeType const *pSubtype) const {
	return subtypeBinding (pSubtype->index ());
    }

//  Query
public:
    bool isBound () const {
	return IsntNil (m_pImplementation);
    }
    bool isntBound () const {
	return IsNil (m_pImplementation);
    }

//  Update
protected:
    void setImplementationTo (VSymbolImplementation const *pValue) {
	m_pImplementation = pValue;
    }

//  Display
public:
    static void Display (
        ThisClass const *pBinding, bool fDisplaySubtypeBindings = true, unsigned int xLevel = 0
    );
private:
    void Display (bool fDisplaySubtypeBindings, unsigned int xLevel) const;

//  State
protected:
    VRunTimeType *const				m_pType;
    VSymbolImplementation const*		m_pImplementation;
    index_t					m_xSubtypeBindingLB;
    index_t					m_xSubtypeBindingUB;
    VkDynamicArrayOf<VSymbolBindingHolder>	m_iSubtypeBindings;
};


#endif

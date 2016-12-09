#ifndef VSelector_Interface
#define VSelector_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

class rtSELUV_Set;

class VByteCodeScanner;

#include "m.h"
#include "selector.h"

#include "RTblock.h"
#include "RTvstore.h"

/*************************
 *****  Definitions  *****
 *************************/

class VSelector : public VTransient {
    DECLARE_FAMILY_MEMBERS (VSelector, VTransient);

//  Type Enumeration
protected:
    enum Type {
	Value, Known, Block, String
    };

//  Construction
protected:
    VSelector (int xMessageName, char const *pMessageName);
public:
    VSelector (VByteCodeScanner const &rMessageSource);
    VSelector (char const * pMessageName);
    VSelector (unsigned int xMessageName);
    VSelector ();

//  Destruction
public:
    ~VSelector ();

//  Query
public:
    bool operator== (char const* pMessageName) const {
	return 0 == strcmp (pMessageName, messageName ());
    }
    bool operator== (unsigned int xMessageName) const {
	return Known == m_xType && xMessageName == m_iValue.as_xMessageName;
    }

    bool operator!= (char const* pMessageName) const {
	return !(*this == pMessageName);
    }
    bool operator!= (unsigned int xMessageName) const {
	return !(*this == xMessageName);
    }

    bool isABlockSelector () const {
	return m_xType == Block;
    }
    bool isAKnownSelector () const {
	return m_xType == Known;
    }
    bool isAStringSelector () const {
	return m_xType == String;
    }
    bool isTheValueCallSelector	() const {
	return m_xType == Value;
    }

//  Access
public:
    bool getKSData (unsigned int& xSelector) const;
    bool getBSData (rtBLOCK_Handle::Reference &rpBlock, unsigned int& rxSelector) const;

    char const *messageName () const;

    operator char const* () const {
	return messageName ();
    }

//  Use
public:
    bool insertIn (rtSELUV_Set& rSet, unsigned int& rxElement) const; 
    bool locateIn (rtSELUV_Set& rSet, unsigned int& rxElement) const;

//  Exceptions
protected:
    void raiseSelectorTypeException () const;

//  State
protected:
    Type m_xType;
    union message_id_t {
	struct block_selector_t {
	    void construct (rtBLOCK_Handle *pBlock, unsigned int xSelector) {
		pBlock->retain ();
		m_pBlock = pBlock;
		m_xSelector = xSelector;
	    }
	    void destroy () {
		m_pBlock->release ();
	    }
	    rtBLOCK_Handle *block () const {
		return m_pBlock;
	    }
	    char const *messageName () const {
		return m_pBlock->addressOfString (m_xSelector);
	    }
	    rtBLOCK_Handle*	m_pBlock;
	    unsigned int	m_xSelector;
	}		as_iBlockMessage;
	unsigned int	as_xMessageName;
	char const*	as_pMessageName;
    } m_iValue;
};


class VSelectorGeneral : public VSelector {
//  Construction
public:
    VSelectorGeneral (M_CPD* pMessageName);
    VSelectorGeneral (char const* pMessageName);
    VSelectorGeneral (unsigned int xMessageName) : VSelector (xMessageName) {
    }
};

typedef class VSelectorGeneral VSelectorGenerale;


#endif

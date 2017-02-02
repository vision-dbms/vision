#ifndef DSC_Store_Interface
#define DSC_Store_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "Vdd_Store.h"

#include "M_CPD.h"

class DSC_Pointer;

class rtBLOCK_Handle;
class rtCONTEXT_Handle;
class rtDICTIONARY_Handle;
class rtVSTORE_Handle;


/*************************
 *****  Definitions  *****
 *************************/

class DSC_Store {
//  Aliases
public:
    typedef Vdd::Store Store;

//  Construction
public:
    void construct () {
	m_pStore = NilOf (Store*);
    }
    void construct (Store *pStore) {
	if (m_pStore = pStore)
	    m_pStore->retain ();
    }
    void construct (DSC_Store const &rSource) {
	if (m_pStore = rSource.m_pStore)
	    m_pStore->retain ();
    }

    void construct (M_KOTE const &rKOTE) {
	m_pStore = rKOTE;
	m_pStore->retain ();
    }

//  Destruction
public:
    void clear ();

//  Access
public:
    operator Store* () const {
	return m_pStore;
    }
    unsigned int attentionMask () const {
	return m_pStore ? m_pStore->attentionMask () : 0;
    }

    M_AND *Database () const {
	return m_pStore->database ();
    }

    /*---------------------------------------------------------------------------
     *****  Internal utility to obtain either a standard CPD for the block or the
     *****	index of the primitive closed by a closure.
     *
     *  Arguments:
     *	    rpBlock		- an address which will be set to a handle
     *				  of a block for block closures and 'Nil'
     *				  for primitive closure.
     *	    rxPrimitive		- an address which will be set to the index of
     *				  a primitive for primitive closures.
     *
     *****/
    bool decodeClosure (
	VReference<rtBLOCK_Handle> &rpBlock, unsigned int &rxPrimitive, VReference<rtCONTEXT_Handle> *ppContext = 0
    ) const {
	return m_pStore->decodeClosure (rpBlock, rxPrimitive, ppContext);
    }

    void getCanonicalStore (
	VReference<rtVSTORE_Handle> &rpStore, bool &rbConvertPointer, DSC_Pointer const &rPointer
    ) const {
	rbConvertPointer = m_pStore->getCanonicalization (rpStore, rPointer);
    }

    void getDictionary (VReference<rtDICTIONARY_Handle>&rpResult, DSC_Pointer const &rPointer) const;
    void getDictionary (Vdd::Store::Reference &rpResult, DSC_Pointer const &rPointer) const;

    bool getInheritance (DSC_Pointer &rPointer) {
	Vdd::Store::Reference pInheritance;
	if (m_pStore->getInheritance (pInheritance, rPointer)) {
	    setTo (pInheritance);
	    return true;
	}
	return false;
    }
    bool getProperty (DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype) {
	Vdd::Store::Reference pProperty;
	if (m_pStore->getProperty (pProperty, rPointer, xPropertySlot, pPropertyPrototype)) {
	    setTo (pProperty);
	    return true;
	}
	return false;
    }

    Vdd::Store::DictionaryLookup lookup (
	VSelector const &rSelector, DSC_Pointer const &rPointer, unsigned int &rxPropertySlot, DSC_Descriptor *pValueReturn = 0
    ) {
	return m_pStore->lookup (rSelector, rPointer, rxPropertySlot, pValueReturn);
    }

    M_KOT *KOT () const {
	return m_pStore->kot ();
    }

    RTYPE_Type RType () const {
	return m_pStore->rtype ();
    }

    M_ASD *ScratchPad () const {
	return m_pStore->scratchPad ();
    }
    M_ASD *Space () const {
	return m_pStore->objectSpace ();
    }

    transientx_t *transientExtension () const {
	return m_pStore ? m_pStore->transientExtension () : 0;
    }
    transientx_t *transientExtensionIfA (VRunTimeType const &rRTT) const {
	return m_pStore ? m_pStore->transientExtensionIfA (rRTT) : 0;
    }
    bool transientExtensionIsA (VRunTimeType const &rRTT) const {
	return m_pStore && m_pStore->transientExtensionIsA (rRTT);
    }

    void traverseHandleReferences(void (VContainerHandle::*visitFunction)(void)) { 
	if (m_pStore) m_pStore->traverseHandleReferences(visitFunction);
    }
//  Update
public:
    void setAttentionMaskTo (unsigned int iNewAttentionMask) {
	m_pStore->setAttentionMaskTo (iNewAttentionMask);
    }

    void setTo (Store *pStore) {
	if (m_pStore != pStore) {
	    clear ();
	    construct (pStore);
	}
    }

//  State
private:
    Store *m_pStore;
};


#endif

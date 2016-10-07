#ifndef DSC_Store_Interface
#define DSC_Store_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"

#include "VConstructor.h"

class DSC_Pointer;

class rtBLOCK_Handle;
class rtCONTEXT_Constructor;


/*************************
 *****  Definitions  *****
 *************************/

class DSC_Store {
//  Construction
public:
    void construct () {
	m_pPO = NilOf (VConstructor*);
	m_pCPD = NilOf (M_CPD*);
    }
    void construct (M_CPD *pCPD) {
	m_pPO = NilOf (VConstructor*);
	m_pCPD = pCPD;
    }
    void construct (VConstructor *pPO) {
	m_pPO = pPO;
	m_pCPD = NilOf (M_CPD*);
    }
    void construct (DSC_Store const &rSource) {
	if (m_pPO = rSource.m_pPO)
	    m_pPO->retain ();
	if (m_pCPD = rSource.m_pCPD)
	    m_pCPD->retain ();
    }

    void construct (M_KOTE const &rKOTE) {
	m_pPO = NilOf (VConstructor*);
	m_pCPD = rKOTE;
	m_pCPD->retain ();
    }

//  Destruction
public:
    void clear ();

//  Access
public:
    unsigned int attentionMask () const {
	return m_pCPD ? m_pCPD->attentionMask ()
	    :  m_pPO  ? m_pPO ->attentionMask ()
	    :  0;
    }

    /*---------------------------------------------------------------------------*
     *	This method will cause the instantiation of uninstantiated psuedo-objects.
     *---------------------------------------------------------------------------*/
    M_CPD *cpd () {
	if (IsNil (m_pCPD)) {
	    m_pCPD = m_pPO->realization ();
	    m_pCPD->retain ();
	}
	return m_pCPD;
    }
    M_CPD *cpdIfAvailable () const {
	return m_pCPD;
    }
    VContainerHandle *containerHandle () {
	return cpd ()->containerHandle ();
    }

    M_AND *Database () const {
	return m_pCPD ? m_pCPD->Database () : m_pPO->Database_();
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
    void decodeClosure (
	VReference<rtBLOCK_Handle> &rpBlock, unsigned int &rxPrimitive, rtCONTEXT_Constructor **ppContext = 0
    ) const;

    M_CPD *dictionaryCPD (DSC_Pointer const &rPointer) const;

    void getCanonicalStore_(
	M_CPD *&rpCanonicalStore, bool &rbConvertPointer, DSC_Pointer const &rPointer
    ) const;

    void getCanonicalStore (
	M_CPD *&rpCanonicalStore, bool &rbConvertPointer, DSC_Pointer const &rPointer
    ) const {
	if (m_pCPD && RTYPE_C_ValueStore == (RTYPE_Type)M_CPD_RType (m_pCPD)) {
	    rpCanonicalStore = m_pCPD;
	    rbConvertPointer = false;
	}
	else getCanonicalStore_(rpCanonicalStore, rbConvertPointer, rPointer);
    }

    M_KOT *KOT () const {
	return m_pCPD ? m_pCPD->KOT () : m_pPO->KOT_();
    }

    VConstructor *poIfAvailable () const {
	return m_pPO;
    }

    RTYPE_Type RType () const {
	return m_pCPD ? m_pCPD->RType () : m_pPO->RType_();
    }

    M_ASD *ScratchPad () const {
	return m_pCPD ? m_pCPD->ScratchPad () : m_pPO->ScratchPad_();
    }
    M_ASD *Space () const {
	return m_pCPD ? m_pCPD->Space () : m_pPO->Space_();
    }

    transientx_t *TransientExtension () const {
	return m_pCPD ? m_pCPD->TransientExtension () : 0;
    }
    transientx_t *TransientExtensionIfA (VRunTimeType const &rRTT) const {
	return m_pCPD ? m_pCPD->TransientExtensionIfA (rRTT) : 0;
    }

    bool HasATransientExtension () const {
	return m_pCPD ? m_pCPD->HasATransientExtension () : false;
    }
    bool TransientExtensionIsA (VRunTimeType const &rRTT) const {
	return m_pCPD ? m_pCPD->TransientExtensionIsA (rRTT) : false;
    }

//  Update
public:
    void setAttentionMaskTo (unsigned int iNewAttentionMask) {
	if (m_pCPD)
	    m_pCPD->setAttentionMaskTo (iNewAttentionMask);
	else if (m_pPO)
	    m_pPO ->setAttentionMaskTo (iNewAttentionMask);
    }

    void setTo (M_CPD *pStore) {
	clear ();
	construct (pStore);
    }

    void setTo (VConstructor *pStore) {
	clear ();
	construct (pStore);
    }

//  State
public:
    VConstructor*	m_pPO;
    M_CPD*		m_pCPD;
};


#endif

#ifndef VConstructor_Interface
#define VConstructor_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"
#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * Constructors are transient instances of certain R-Types.  Psuedo-objects
 * are not managed by the memory manager as container table based objects.
 * By employing psuedo-objects, frequently instantiated R-Types with mostly
 * transient instances (such as 'Closure' and 'Context') can bypass the
 * overhead of the container table based memory manager.
 *---------------------------------------------------------------------------
 */

class ABSTRACT VConstructor : public VBenderenceable {
    DECLARE_ABSTRACT_RTT (VConstructor, VBenderenceable);

//  Construction
protected:
    VConstructor (M_CPD* pRealization) : BaseClass (1), m_pRealization (pRealization), m_iAttentionMask (0) {
    }
    VConstructor (unsigned int iAttentionMask = 0) : BaseClass (1), m_iAttentionMask (iAttentionMask) {
    }

//  Access
public:
    unsigned int attentionMask () const {
	return m_pRealization ? m_pRealization->attentionMask () : m_iAttentionMask;
    }

    virtual RTYPE_Type RType_() const = 0;

    virtual M_ASD *Space_() const = 0;

    M_AND *Database_() const {
	return Space_()->Database ();
    }
    M_KOT *KOT_() const {
	return Space_()->KOT ();
    }
    M_ASD *ScratchPad_() const {
	return Space_()->ScratchPad ();
    }

//  Realization
protected:
    virtual M_CPD* newRealization () = 0;

public:
    M_CPD* realization () {
	if (m_pRealization.isNil ()) {
	    m_pRealization.claim (newRealization ());
	    m_pRealization->setAttentionMaskTo (m_iAttentionMask);
	}
	return m_pRealization;
    }

//  Update
public:
    void setAttentionMaskTo (unsigned int iNewAttentionMask) {
	if (m_pRealization)
	    m_pRealization->setAttentionMaskTo (iNewAttentionMask);
	else
	    m_iAttentionMask = iNewAttentionMask;
    }

//  State
protected:
    VCPDReference m_pRealization;
    unsigned int  m_iAttentionMask;
};


#endif

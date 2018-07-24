/*****  Context Representation Type Header File  *****/
#ifndef rtCONTEXT_Interface
#define rtCONTEXT_Interface

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "popvector.h"

#include "DSC_Descriptor.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTcontext.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdsc.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtCONTEXT_CPD_StdPtrCount (rtCONTEXT_Context_POPCount + POPVECTOR_CPD_XtraPtrCount)

#define rtCONTEXT_CPx_Current	(unsigned int)0
#define rtCONTEXT_CPx_Self	(unsigned int)1
#define rtCONTEXT_CPx_Origin	(unsigned int)2


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

/*---------------------------------------------------------------------------
 * Contexts define the values of '^current', '^self', and '^my' for a task.
 * The following type defines the structure of a context psuedo-object.
 *
 *  Context Constructor Field Descriptions:
 *	header			- a psuedo-object header for this context.
 *				  THIS FIELD MUST ALWAYS BE THE FIRST FIELD
 *				  IN THE CONTEXT.
 *	parent			- an optimal descriptor for the parent of this
 *				  context.  This descriptor is either empty or
 *				  references another context.
 *	current			- an optimal descriptor for this context's
 *				  value of '^current'.
 *	self			- an optimal descriptor for this context's
 *				  value of '^self'.  If this descriptor is
 *				  empty, its value can be derived from the
 *				  value of 'self' in the 'parent' context.
 *	origin			- a general descriptor for this context's
 *				  value of '^my'.  If this descriptor is
 *				  empty, its value can be derived from the
 *				  value of 'origin' in the 'parent' context.
 *---------------------------------------------------------------------------
 */

class rtCONTEXT_Handle : public rtPOPVECTOR_Handle {
//    DECLARE_CONCRETE_RTT (rtCONTEXT_Handle, rtPOPVECTOR_Handle);
    DECLARE_VIRTUAL_RTT (rtCONTEXT_Handle, rtPOPVECTOR_Handle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtCONTEXT_Handle (rCTE);
    }
    rtCONTEXT_Handle (ThisClass *pParentContext, DSC_Pointer &rParentPointer);
    rtCONTEXT_Handle (DSC_Descriptor &rSelf, DSC_Descriptor &rCurrent, DSC_Descriptor &rMy);
    rtCONTEXT_Handle (DSC_Descriptor const& rSelfCurrentAndMy);
private:
    rtCONTEXT_Handle (M_CTE &rCTE);
private:
    void createContainer ();

//  Destruction
private:
    ~rtCONTEXT_Handle ();
    virtual void deleteThis () OVERRIDE;

//  Lifetime Management
public:
    void protectFromGC (bool bProtect) {
	setPreciousTo (bProtect);
    }

//  Access
private:
    DSC_Descriptor &Get (DSC_Descriptor rtCONTEXT_Handle::*pMemberComponent);
public:
    void getSelf (rtDSC_Handle::Reference &rpResult) {
	rpResult.setTo (static_cast<rtDSC_Handle*>(elementHandle (rtCONTEXT_CPx_Self)));
    }
    void getCurrent (rtDSC_Handle::Reference &rpResult) {
	rpResult.setTo (static_cast<rtDSC_Handle*>(elementHandle (rtCONTEXT_CPx_Current)));
    }
    void getMy (rtDSC_Handle::Reference &rpResult) {
	rpResult.setTo (static_cast<rtDSC_Handle*>(elementHandle (rtCONTEXT_CPx_Origin)));
    }

    DSC_Descriptor &getSelf () {
	return Get (&rtCONTEXT_Handle::m_iSelf);
    }
    DSC_Descriptor &getCurrent () {
	return Get (&rtCONTEXT_Handle::m_iCurrent);
    }
    DSC_Descriptor &getMy () {
	return Get (&rtCONTEXT_Handle::m_iMy);
    }

    DSC_Descriptor &self () {
	return m_iSelf;
    }
    DSC_Descriptor &current () {
	return m_iCurrent;
    }
    DSC_Descriptor &my () {
	return m_iMy;
    }

//  Alignment
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }

// Garbage collection marking
public:
    virtual void visitReferencesUsing (Visitor *visitor) OVERRIDE;
    virtual void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const OVERRIDE;

public:
    bool align ();
    using BaseClass::alignAll;

//  Callbacks
protected:

//  Store Access
public:
    rtPTOKEN_Handle *getPToken () const;

//  Display and Inspection
public:
    virtual unsigned __int64 getClusterSize () OVERRIDE;

//  Maintenance
protected:
    virtual bool PersistReferences () OVERRIDE;

//  State
private:
    DSC_Descriptor	m_iSelf;
    DSC_Descriptor	m_iCurrent;
    DSC_Descriptor	m_iMy;
    Reference		m_pParentContext;
    DSC_Pointer		m_iParentPointer;
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTcontext.h 1 replace /users/mjc/system
  861002 17:55:38 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTcontext.h 2 replace /users/mjc/translation
  870524 09:32:16 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

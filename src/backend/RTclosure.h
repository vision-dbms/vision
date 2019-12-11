/*****  Closure Representation Type Header File  *****/
#ifndef rtCLOSURE_H
#define rtCLOSURE_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/************************
 *****  Components  *****
 ************************/

#include "VContainerHandle.h"
#include "Vdd_Store.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTclosure.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTblock.h"
#include "RTcontext.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtCLOSURE_CPD_StdPtrCount	2

#define rtCLOSURE_CPx_Context		(unsigned int)0
#define rtCLOSURE_CPx_Block		(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtCLOSURE_CPD_Base(cpd)\
    ((rtCLOSURE_Closure*) M_CPD_ContainerBase (cpd))

#define rtCLOSURE_CPD_Context(cpd)\
    M_CPD_PointerToPOP (cpd, rtCLOSURE_CPx_Context)

#define rtCLOSURE_CPD_Block(cpd)\
    M_CPD_PointerToPOP (cpd, rtCLOSURE_CPx_Block)

#define rtCLOSURE_CPD_Primitive(cpd)\
    rtCLOSURE_Closure_Primitive (rtCLOSURE_CPD_Base (cpd))

/*****  Content Query Predicates  *****/
#define rtCLOSURE_CPD_IsABlockClosure(cpd) (\
    (cpd)->ReferenceIsntNil (rtCLOSURE_CPx_Block)\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtCLOSURE_Handle : public VStoreContainerHandle {
    DECLARE_CONCRETE_RTT (rtCLOSURE_Handle, VStoreContainerHandle);

//  CPD Initialization
public:
    static void InitStdCPD (M_CPD* pCPD);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtCLOSURE_Handle (rCTE);
    }
    rtCLOSURE_Handle (
	rtCONTEXT_Handle *context, rtBLOCK_Handle *pBlockHandle, unsigned int iAttentionMask = 0
    );
    rtCLOSURE_Handle (
	rtCONTEXT_Handle *context, unsigned int xPrimitive, unsigned int iAttentionMask = 0
    );
private:
    rtCLOSURE_Handle (M_CTE &rCTE);
private:
    void createContainer ();

//  Destruction
private:
    ~rtCLOSURE_Handle () {
    }

//  Alignment
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
public:
    bool align ();
    using BaseClass::alignAll;

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;

//  Access
private:
    rtCLOSURE_Closure *typecastContent () const {
	return reinterpret_cast<rtCLOSURE_Closure*> (containerContent ());
    }
public:
    void getBlock (rtBLOCK_Handle::Reference &rpResult) const {
	rpResult.setTo (m_pBlock);
    }
    void getContext (rtCONTEXT_Handle::Reference &rpResult) const {
	rpResult.setTo (m_pContext);
    }
    DSC_Descriptor &getCurrent () const {
	return m_pContext->getCurrent ();
    }
    DSC_Descriptor &getSelf () const {
	return m_pContext->getSelf ();
    }
    DSC_Descriptor &getMy () const {
	return m_pContext->getMy ();
    }

    unsigned int primitiveIndex () const {
	return m_xPrimitive;
    }

//  Query
public:
    bool isABlockClosure () const {
	return m_pBlock.isntNil ();
    }
    bool isAPrimitiveClosure () const {
	return ReferenceIsNil (&rtCLOSURE_Closure_Block (typecastContent ()));
    }

//  Dictionary
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return static_cast<rtDICTIONARY_Handle*>(TheClosureClassDictionary ().ObjectHandle ());
    }

// Garbage collection marking
public:
    virtual void visitReferencesUsing (Visitor *visitor) OVERRIDE;
    virtual void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const OVERRIDE;

//  Store Access
private:
    virtual bool decodeClosure_(
	rtBLOCK_Handle::Reference &rpBlock, unsigned int &rxPrimitive, rtCONTEXT_Handle::Reference *ppContext
    ) const OVERRIDE;
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE;

//  Maintenance
protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual unsigned __int64 getClusterSize () OVERRIDE;

    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 2;
    }

//  State
private:
    rtCONTEXT_Handle::Reference	const m_pContext;
    rtBLOCK_Handle::Reference	const m_pBlock;
    unsigned int 		const m_xPrimitive;
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTclosure.h 1 replace /users/mjc/system
  861002 17:55:45 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTclosure.h 2 replace /users/mjc/translation
  870524 09:32:03 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

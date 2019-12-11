/*****  Descriptor Representation Type Header File  *****/
#ifndef rtDSC_H
#define rtDSC_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "VContainerHandle.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTdsc.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Scalar.h"

#include "Vdd_Store.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignements  *****/
#define rtDSC_CPD_StdPtrCount	2

#define rtDSC_CPx_Store		(unsigned int)0
#define rtDSC_CPx_Pointer	(unsigned int)1

/*****  Standard CPD Access Macros  *****/
#define rtDSC_CPD_Base(cpd) reinterpret_cast<rtDSC_Descriptor*>(M_CPD_ContainerBase (cpd))

#define rtDSC_CPD_PointerType(cpd) rtDSC_Descriptor_PointerType (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_RType(cpd) rtDSC_Descriptor_RType (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_ScalarValue(cpd) rtDSC_Descriptor_ScalarValue (\
    rtDSC_CPD_Base (cpd)\
)

#define rtDSC_CPD_Store(cpd) M_CPD_PointerToPOP (cpd, rtDSC_CPx_Store)

#define rtDSC_CPD_Pointer(cpd) M_CPD_PointerToPOP (cpd, rtDSC_CPx_Pointer)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtDSC_Handle : public VContainerHandle {
    DECLARE_CONCRETE_RTT (rtDSC_Handle, VContainerHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtDSC_Handle (rCTE);
    }
    rtDSC_Handle (M_ASD *pSpace, DSC_Descriptor &rValue);
protected:
    rtDSC_Handle (M_CTE &rCTE) : VContainerHandle (rCTE) {
    }

//  Destruction
private:
    ~rtDSC_Handle ();    

//  Access
private:
    rtDSC_Descriptor *typecastContent () const {
	return reinterpret_cast<rtDSC_Descriptor*>(containerContent ());
    }
    M_POP *pointerPOP () const {
	return &rtDSC_Descriptor_Pointer (typecastContent ());
    }
    M_POP *storePOP () const {
	return &rtDSC_Descriptor_Store (typecastContent ());
    }

    VContainerHandle *pointerHandle (RTYPE_Type xType = RTYPE_C_Any) const {
	return GetContainerHandle (pointerPOP (), xType);
    }
    M_CPD *pointerCPD (RTYPE_Type xType = RTYPE_C_Any) const {
	return pointerHandle (xType)->GetCPD ();
    }
    DSC_PointerType pointerType () const {
	return (DSC_PointerType)rtDSC_Descriptor_PointerType (typecastContent ());
    }

    VContainerHandle *storeHandle () const {
	return GetContainerHandle (storePOP ());
    }
    void getPointer (M_CPD *&rpResult, RTYPE_Type xType = RTYPE_C_Any) const {
	rpResult = pointerCPD (xType);
    }
    void getPointer (VContainerHandle::Reference &rpResult, RTYPE_Type xType = RTYPE_C_Any) const {
	rpResult.setTo (pointerHandle (xType));
    }
    void getScalar (DSC_Scalar &rValue) const {
	rtDSC_Descriptor *pContent = typecastContent ();
	rValue.constructFrom (
	    static_cast<rtPTOKEN_Handle*>(pointerHandle (RTYPE_C_PToken)),
	    rtDSC_Descriptor_RType (pContent),
	    rtDSC_Descriptor_ScalarValue (pContent)
	);
    }

    bool getStore (Vdd::Store::Reference &rpResult) const {
	return storeHandle ()->getStore (rpResult);
    }
    void getStore (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (storeHandle ());
    }

public:
    void getValue (DSC_Descriptor &rValue);

//  Update
private:
    void setPointerTo (M_CPD *pPointer) {
	setPointerTo (pPointer->containerHandle ());
    }
    void setPointerTo (VContainerHandle *pPointer) {
	StoreReference (pointerPOP (), pPointer);
    }
    void setPointerTypeTo (DSC_PointerType xType) {
	rtDSC_Descriptor_PointerType (typecastContent ()) = xType;
    }
    void setScalarTo (DSC_Scalar const &rValue) {
	rtDSC_Descriptor *pContent = typecastContent ();
	rtDSC_Descriptor_RType (pContent) = rValue.RType ();
	rtDSC_Descriptor_ScalarValue (pContent) = DSC_Scalar_Value (rValue);
        setPointerTo (rValue.RPT ());
    }
    void setStoreTo (Vdd::Store *pStore) {
	VContainerHandle::Reference pStoreHandle;
	pStore->getContainerHandle (pStoreHandle);
	StoreReference (storePOP (), pStoreHandle);
    }

//  Maintenance
protected:
    virtual bool align_() OVERRIDE {
	return align ();
    }
    virtual bool alignAll_(bool bCleaning) OVERRIDE {
	return align ();
    }
public:
    bool align ();
    using BaseClass::alignAll;

protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual unsigned __int64 getClusterSize () OVERRIDE;

    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 2;
    }
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl bool rtDSC_UnpackIObject (
    M_ASD *pContainerSpace, IOBJ_IObject iObject, DSC_Descriptor &rDsc
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTdsc.h 1 replace /users/mjc/system
  861002 17:59:39 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  RTdsc.h 2 replace /users/mjc/translation
  870524 09:32:41 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

/*****  Method Representation Type Header File  *****/
#ifndef rtMETHOD_H
#define rtMETHOD_H

/*****************************************
 *****************************************
 *****  Definitions And Signatures  ******
 *****************************************
 *****************************************/

/***********************
 *****  Component  *****
 ***********************/

#include "popvector.h"
#include "Vdd_Store.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTmethod.d"

/************************
 *****  Supporting  *****
 ************************/

class rtBLOCK_Handle;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/
/**************************************
 *****  Standard CPD Definitions  *****
 **************************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtMETHOD_CPD_StdPtrCount (rtMETHOD_Method_POPCount + POPVECTOR_CPD_XtraPtrCount)

#define rtMETHOD_CPx_Block	(unsigned int)0
#define rtMETHOD_CPx_Origin	(unsigned int)1


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtMETHOD_Handle : public rtPOPVECTOR_StoreHandle {
    DECLARE_CONCRETE_RTT (rtMETHOD_Handle, rtPOPVECTOR_StoreHandle);

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtMETHOD_Handle (rCTE);
    }
    rtMETHOD_Handle (M_ASD *pContainerSpace, rtBLOCK_Handle *pBlock, Vdd::Store *pMy);
protected:
    rtMETHOD_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    }

//  Destruction
private:
    ~rtMETHOD_Handle () {
    }

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;

//  Access
private:
    rtBLOCK_Handle *blockHandle () const {
	return static_cast<rtBLOCK_Handle*> (elementHandle (rtMETHOD_CPx_Block));
    }
    VContainerHandle *myStoreHandle () const {
	return elementHandle (rtMETHOD_CPx_Origin);
    }
public:
    void getBlock (rtBLOCK_Handle::Reference &rpResult) const {
	rpResult.setTo (blockHandle ());
    }
    void getMyStore (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (myStoreHandle ());
    }
    bool getMyStore (Vdd::Store::Reference &rpResult) const {
	return myStoreHandle ()->getStore (rpResult);
    }

//  Dictionary
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return static_cast<rtDICTIONARY_Handle*>(TheMethodClassDictionary().ObjectHandle ());
    }
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE {
	Vdd::Store::Reference pMyStore;
	getMyStore (pMyStore);
	return pMyStore->getPToken ();
    }

//  Maintenance
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
public:
    bool align ();
    using BaseClass::alignAll;

//  Callbacks
protected:

//  Display and Inspection
public:
    virtual unsigned __int64 getClusterSize () OVERRIDE;
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTmethod.h 1 replace /users/mjc/system
  861002 17:55:31 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTmethod.h 2 replace /users/mjc/translation
  870524 09:33:58 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

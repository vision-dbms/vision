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
    virtual /*override*/ bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer);

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
    virtual /*override*/ rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const {
	return static_cast<rtDICTIONARY_Handle*>(TheMethodClassDictionary().ObjectHandle ());
    }
    virtual /*override*/ rtPTOKEN_Handle *getPToken_() const {
	Vdd::Store::Reference pMyStore;
	getMyStore (pMyStore);
	return pMyStore->getPToken ();
    }

//  Maintenance
private:
    virtual /*override*/ bool align_() {
	return align ();
    }
public:
    bool align ();
    using BaseClass::alignAll;

//  Callbacks
protected:

//  Display and Inspection
public:
    virtual /*override*/ unsigned __int64 getClusterSize ();
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

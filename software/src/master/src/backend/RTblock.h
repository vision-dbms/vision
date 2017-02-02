/*****  Block Representation Type Header File  *****/
#ifndef rtBLOCK_H
#define rtBLOCK_H

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

#include "RTblock.d"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdictionary.h"


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Known CPD Pointers  *****/
#define rtBLOCK_CPx_End			(unsigned int)0
#define rtBLOCK_CPx_LocalEnv		(unsigned int)1
#define rtBLOCK_CPx_ByteCodeVector	(unsigned int)2
#define rtBLOCK_CPx_PC			(unsigned int)3
#define rtBLOCK_CPx_StringSpace		(unsigned int)4
#define rtBLOCK_CPx_PLVector		(unsigned int)5
#define rtBLOCK_CPx_PLVElement		(unsigned int)6
#define rtBLOCK_CPx_SelectorName	(unsigned int)7

#define rtBLOCK_CPD_StdPtrCount		8

#define rtBLOCK_CPDBase(cpd) ((rtBLOCK_BlockType *) M_CPD_ContainerBase (cpd))

#define rtBLOCK_CPDEnd(cpd) M_CPD_PointerToCharacter (\
    cpd, rtBLOCK_CPx_End\
)
#define rtBLOCK_CPD_LocalEnvironment(cpd) M_CPD_PointerToPOP (\
    cpd, rtBLOCK_CPx_LocalEnv\
)
#define rtBLOCK_CPD_ByteCodeVector(cpd) M_CPD_PointerToByte (\
    cpd, rtBLOCK_CPx_ByteCodeVector\
)
#define rtBLOCK_CPD_PC(cpd) M_CPD_PointerToByte (\
    cpd, rtBLOCK_CPx_PC\
)
#define rtBLOCK_CPD_StringSpace(cpd) M_CPD_PointerToCharacter (\
    cpd, rtBLOCK_CPx_StringSpace\
)
#define rtBLOCK_CPD_PLiteralVector(cpd) M_CPD_PointerToType (\
    cpd, rtBLOCK_CPx_PLVector, rtBLOCK_PLVectorType *\
)
#define rtBLOCK_CPD_PLVectorElement(cpd) M_CPD_PointerToPOP (\
    cpd, rtBLOCK_CPx_PLVElement\
)

#define rtBLOCK_CPD_DictionaryCPD(cpd) (\
    (cpd)->GetCPD (rtBLOCK_CPx_LocalEnv, RTYPE_C_Dictionary)\
)

/*****	Selector type access macros	*****/
#define rtBLOCK_CPD_SelectorName(cpd) M_CPD_PointerToCharacter (\
    cpd, rtBLOCK_CPx_SelectorName\
)
#define rtBLOCK_CPD_SelectorType(cpd)\
    (rtBLOCK_SelectorType (rtBLOCK_CPDBase (cpd)))

#define rtBLOCK_CPD_SelectorIndex(cpd)\
    (rtBLOCK_SelectorIndex (rtBLOCK_CPDBase (cpd)))

#define rtBLOCK_CPD_SelectorIsKnown(cpd)\
    (rtBLOCK_CPD_SelectorType (cpd) == rtBLOCK_C_KnownSelector)

#define rtBLOCK_CPD_SelectorIsDefined(cpd)\
    (rtBLOCK_CPD_SelectorType (cpd) == rtBLOCK_C_DefinedSelector)

#define rtBLOCK_CPD_NoSelector(cpd)\
    (rtBLOCK_CPD_SelectorType (cpd) == rtBLOCK_C_NoSelector)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void rtBLOCK_InitStdCPD (M_CPD *cpd);

PublicFnDecl M_CPD *rtBLOCK_New (M_ASD *pContainerSpace);

PublicFnDecl void rtBLOCK_AppendStringSpace (
    M_CPD *cpd, char const *stringSpace, int stringSpaceSize
);

PublicFnDecl void rtBLOCK_AppendPLVector (
    M_CPD *cpd, M_CPD* physicalLiteralVector[], int size
);

PublicFnDecl void rtBLOCK_AppendELEVector (
    M_CPD *cpd, int* evaledLEVector, int size
);

PublicFnDecl bool rtBLOCK_Align (M_CPD *cpd);


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtBLOCK_Handle : public VContainerHandle {
    DECLARE_CONCRETE_RTT (rtBLOCK_Handle, VContainerHandle);

//  Helper Types
public:
    typedef unsigned int offset_t;

//  Construction
protected:
    rtBLOCK_Handle (M_CTE &rCTE) : BaseClass (rCTE), m_pBlockHeader (typecastContent ()) {
    }
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtBLOCK_Handle (rCTE);
    }

//  Destruction
private:
    ~rtBLOCK_Handle () {
    }

//  Access
private:
    rtBLOCK_BlockType *typecastContent () const {
	return reinterpret_cast<rtBLOCK_BlockType*> (ContainerContent ());
    }

    pointer_t addressOf (offset_t xOffset) const {
	return reinterpret_cast<pointer_t>(m_pBlockHeader) + xOffset;
    }
    unsigned char const *addressOfByteCodes () const {
	return reinterpret_cast<unsigned char const*>(addressOf (offsetOfBCVector ()));
    }
    rtBLOCK_PLVectorType const *addressOfPLVector() const {
	return reinterpret_cast<rtBLOCK_PLVectorType*>(addressOf (offsetOfPLVector ()));
    }

    offset_t offsetOfBCVector () const {
	return rtBLOCK_byteCodeVector (m_pBlockHeader);
    }
    offset_t offsetOfPLVector () const {
	return rtBLOCK_PLVector (m_pBlockHeader);
    }
    offset_t offsetOfStringSpace () const {
	return rtBLOCK_stringSpace (m_pBlockHeader);
    }

public:
    unsigned char const*addressOfByteCode (offset_t xOffset) const {
	return addressOfByteCodes () + xOffset;
    }
    M_POP const *addressOfPOP (offset_t xOffset) const {
	return addressOfPLVector ()->physicalLiterals + xOffset;
    }
    char const *addressOfString (offset_t xOffset) const {
	return reinterpret_cast<char const*>(addressOf (offsetOfStringSpace ())) + xOffset;
    }

    M_CPD *getDictionaryCPD () {
	return GetCPD (&rtBLOCK_localEnvironmentPOP (m_pBlockHeader), RTYPE_C_Dictionary);
    }
    rtDICTIONARY_Handle *getDictionaryHandle () const {
	return static_cast<rtDICTIONARY_Handle*>(
	    GetContainerHandle (&rtBLOCK_localEnvironmentPOP (m_pBlockHeader), RTYPE_C_Dictionary)
	);
    }
	
    M_CPD *getNestedBlockCPD (offset_t xOffset) {
	return GetCPD (addressOfPOP (xOffset), RTYPE_C_Block);
    }
    rtBLOCK_Handle *getNestedBlockHandle (offset_t xOffset) const {
	return static_cast<rtBLOCK_Handle*>(GetContainerHandle (addressOfPOP (xOffset), RTYPE_C_Block));
    }

//  Maintenance
protected:
    virtual /*override*/ void AdjustContainerPointers (M_CPreamble *pNewAddress, bool bWritable);
    virtual /*override*/ bool PersistReferences ();

//  State
protected:
    rtBLOCK_BlockType *m_pBlockHeader;
};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTblock.h 1 replace /users/mjc/system
  860404 20:10:21 mjc Create Block Representation Type

 ************************************************************************/
/************************************************************************
  RTblock.h 2 replace /users/hjb/system
  860409 04:45:17 hjb Preliminary Release (jck)

 ************************************************************************/
/************************************************************************
  RTblock.h 3 replace /users/spe/system
  860624 16:56:48 spe Declare rtBLOCK_InitStdCPD as a public function.

 ************************************************************************/
/************************************************************************
  RTblock.h 4 replace /users/mjc/translation
  870524 09:31:37 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/

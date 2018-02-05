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
#include "Vdd_Store.h"

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


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtBLOCK_Handle : public VStoreContainerHandle {
    DECLARE_CONCRETE_RTT (rtBLOCK_Handle, VStoreContainerHandle);

//  Aliases
public:
    typedef unsigned int offset_t;

//  Strings
public:
    class Strings {
    //  Construction
    public:
	Strings ();

    //  Destruction
    public:
	~Strings ();

    //  Access
    public:
	char const *operator[] (unsigned int xString) const {
	    return string (xString);
	}
	char const *string (unsigned int xString) const {
	    return m_pCharacters + xString;
	}

    //  Query
    public:
	bool isSet () const {
	    return m_pStoreHandle.isntNil ();
	}
	bool isntSet () const {
	    return m_pStoreHandle.isNil ();
	}

    //  Update
    public:
	bool setTo (Vdd::Store *pStore) {
	    return setTo (dynamic_cast<rtBLOCK_Handle*>(pStore));
	}
	bool setTo (rtBLOCK_Handle *pStore);

    //  State
    private:
	Reference	m_pStoreHandle;
	char const*	m_pCharacters;
    };
    friend class Strings;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtBLOCK_Handle (rCTE);
    }
private:
    rtBLOCK_Handle (M_CTE &rCTE) : BaseClass (rCTE), m_pBlockHeader (typecastContent ()) {
    }

//  Destruction
private:
    ~rtBLOCK_Handle () {
    }

//  Lifetime Management
public:
    void protectFromGC (bool bProtect) {
	setPreciousTo (bProtect);
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
    rtBLOCK_BlockType *typecastContent () const {
	return reinterpret_cast<rtBLOCK_BlockType*> (containerContent ());
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
    char const *addressOfStringSpace () const {
	return const_cast<char const*>(addressOf (offsetOfStringSpace ()));
    }

    M_POP *dictionaryPOP () const {
	return &rtBLOCK_localEnvironmentPOP (m_pBlockHeader);
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
	return addressOfStringSpace () + xOffset;
    }

    rtDICTIONARY_Handle *getDictionary () const {
	return static_cast<rtDICTIONARY_Handle*>(GetContainerHandle (dictionaryPOP (), RTYPE_C_Dictionary));
    }

    unsigned int nestedBlockCount () const {
	return rtBLOCK_PLVector_Count (addressOfPLVector ());
    }
    M_CPD *nestedBlockCPD (offset_t xOffset) {
	return GetCPD (addressOfPOP (xOffset), RTYPE_C_Block);
    }
    rtBLOCK_Handle *nestedBlockHandle (offset_t xOffset) const {
	return static_cast<rtBLOCK_Handle*>(GetContainerHandle (addressOfPOP (xOffset), RTYPE_C_Block));
    }

    unsigned int selectorIndex () const {
	return rtBLOCK_SelectorIndex (typecastContent ());
    }
    char const *selectorName () const {
	return selectorIsUserDefined () ? addressOfString (selectorIndex ()) : 0;
    }
    unsigned int selectorType () const {
	return rtBLOCK_SelectorType (typecastContent ());
    }

    bool selectorIsKnown () const {
	return selectorType () == rtBLOCK_C_KnownSelector;
    }
    bool selectorIsUserDefined () const {
	return selectorType () == rtBLOCK_C_DefinedSelector;
    }
    bool selectorUndefined () const {
	return selectorType () == rtBLOCK_C_NoSelector;
    }

//  Attributes
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE;
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE;

//  Container Maintenance
protected:
    virtual void AdjustContainerPointers (M_CPreamble *pNewAddress, bool bWritable) OVERRIDE;
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 1/*dictionary*/ + nestedBlockCount ();
    }
    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;

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

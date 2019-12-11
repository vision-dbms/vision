/*****  List Store Representation Type Header File  *****/
#ifndef rtLSTORE_H
#define rtLSTORE_H

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

#include "VCPDReference.h"

#include "VSet.h"

/***********************
 *****  Container  *****
 ***********************/

#include "RTlstore.d"

/************************
 *****  Supporting  *****
 ************************/

#include "DSC_Descriptor.h"
#include "RTcharuv.h"
#include "RTlink.d"
#include "RTrefuv.d"

class rtINDEX_Key;

class VAssociativeResult;
class VCollectionOfStrings;
class VCollectionOfUnsigned32;

template <class SetClass, class KeyClass, class KeyValueType> class VAssociativeCursor;
template <class CollectionClass, class ElementType> class VOrdered;


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Assignments  *****/
#define rtLSTORE_CPD_StdPtrCount	4

#define rtLSTORE_CPx_RowPToken		(unsigned int)0
#define rtLSTORE_CPx_Content		(unsigned int)1
#define rtLSTORE_CPx_ContentPToken	(unsigned int)2
#define rtLSTORE_CPx_Breakpoint		(unsigned int)3

/*****  Standard CPD Access Macros  *****/
#define rtLSTORE_CPD_Base(cpd) M_CPD_ContainerBaseAsType (\
    cpd, rtLSTORE_LStore\
)
#define rtLSTORE_CPD_StringStore(cpd) rtLSTORE_LStore_StringStore (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_BreakpointCount(cpd) rtLSTORE_LStore_BreakpointCount (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_BreakpointArray(cpd) rtLSTORE_LStore_BreakpointArray (\
    rtLSTORE_CPD_Base (cpd)\
)
#define rtLSTORE_CPD_RowPToken(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_RowPToken\
)
#define rtLSTORE_CPD_Content(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_Content\
)
#define rtLSTORE_CPD_ContentPToken(cpd) M_CPD_PointerToPOP (\
    cpd, rtLSTORE_CPx_ContentPToken\
)
#define rtLSTORE_CPD_Breakpoint(cpd) M_CPD_PointerToCardinal (\
    cpd, rtLSTORE_CPx_Breakpoint\
)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class rtLSTORE_Handle : public VStoreContainerHandle {
    DECLARE_CONCRETE_RTT (rtLSTORE_Handle, VStoreContainerHandle);

//  Aliases
public:
    typedef unsigned int breakpoint_t;

//  Strings
public:
    class Strings {
    //  Aliases
    public:
	typedef Reference			ListStoreReference;
	typedef rtCHARUV_Handle::Reference	CharStoreReference;

    //  Construction
    public:
	Strings ();

    //  Destruction
    public:
	~Strings ();

    //  Access
    public:
	unsigned int cardinality () const {
	    return m_sBreakpoints;
	}

	rtLSTORE_Handle *store () const {
	    return m_pListStoreHandle;
	}

	char const *operator[] (unsigned int xString) const {
	    return string (xString);
	}
	char const *string (unsigned int xString) const {
	    return xString <= m_sBreakpoints ? m_pCharacters + m_pBreakpoints[xString] : 0;
	}
	unsigned int stringOrigin (unsigned int xString) const {
	    return xString <= m_sBreakpoints ? m_pBreakpoints[xString] : UINT_MAX;
	}

    //  Query
    public:
	bool isSet () const {
	    return m_pListStoreHandle.isntNil ();
	}
	bool isntSet () const {
	    return m_pListStoreHandle.isNil ();
	}

    //  Update
    public:
	void align ();
	void refresh ();

	bool setTo (Vdd::Store *pStore) {
	    return setTo (dynamic_cast<rtLSTORE_Handle*>(pStore));
	}
	bool setTo (rtLSTORE_Handle *pStore);

    //  State
    private:
	ListStoreReference	m_pListStoreHandle;
	CharStoreReference	m_pCharStoreHandle;
	unsigned int		m_sBreakpoints;
	breakpoint_t const*	m_pBreakpoints;
	char const*		m_pCharacters;
    };
    friend class Strings;

//  Construction
public:
    static VContainerHandle *Maker (M_CTE &rCTE) {
	return new rtLSTORE_Handle (rCTE);
    }
    rtLSTORE_Handle (rtLINK_CType *pPartition, Vdd::Store *pContent, bool bDecoupling = false);
    rtLSTORE_Handle (rtPTOKEN_Handle *pPPT, Vdd::Store *pContentPrototype = 0, bool bAStringStore = false);
    rtLSTORE_Handle (unsigned int cStrings, M_CPD *pStringStorage);
private:
    rtLSTORE_Handle (M_CTE &rCTE) : BaseClass (rCTE) {
    }
    void InitNewLStore (rtPTOKEN_Handle *ptoken, rtLINK_CType *linkc = 0, char const* pStringStorage = 0);

//  Destruction
private:
    ~rtLSTORE_Handle () {
    }

//  Canonicalization
private:
    virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) OVERRIDE;

//  Cloning
private:
    virtual void clone_(Vdd::Store::Reference &rpResult, rtPTOKEN_Handle *pPPT) const OVERRIDE {
	Reference pNewHandle;
	clone (pNewHandle, pPPT);
	rpResult.setTo (pNewHandle);
    }
    virtual bool isACloneOfListStore_(rtLSTORE_Handle const *pOther) const OVERRIDE {
	return isACloneOfListStore (pOther);
    }
public:
    void clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const;
    bool isACloneOfListStore (rtLSTORE_Handle const *pOther) const;

//  Copying
public:
    void copy (DSC_Descriptor &rResult, M_ASD *pResultSpace);

//  Access
private:
    rtLSTORE_LStore *typecastContent () const {
	return reinterpret_cast<rtLSTORE_LStore*>(containerContent ());
    }

    M_POP *contentPOP () const {
	return &rtLSTORE_LStore_Content (typecastContent ());
    }
    VContainerHandle *contentHandle () const {
	return GetContainerHandle (contentPOP ());
    }

    M_POP *contentPTokenPOP () const {
	return &rtLSTORE_LStore_ContentPToken (typecastContent ());
    }
    rtPTOKEN_Handle *contentPTokenHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (contentPTokenPOP (), RTYPE_C_PToken));
    }

    M_POP *rowPTokenPOP () const {
	return &rtLSTORE_LStore_RowPToken (typecastContent ());
    }
    rtPTOKEN_Handle *rowPTokenHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (rowPTokenPOP (), RTYPE_C_PToken));
    }

    rtCHARUV_Handle *stringContentHandle () const {
	return static_cast<rtCHARUV_Handle*> (GetContainerHandle (contentPOP (), RTYPE_C_CharUV));
    }
public:
    unsigned int *breakpointArray () const {
	return rtLSTORE_LStore_BreakpointArray (typecastContent ());
    }
    unsigned int breakpointCount () const {
	return rtLSTORE_LStore_BreakpointCount (typecastContent ());
    }
    unsigned int finalBreakpoint () const {
	rtLSTORE_LStore *pContent = typecastContent ();
	return rtLSTORE_LStore_BreakpointArray (pContent)[rtLSTORE_LStore_BreakpointCount (pContent)];
    }
private:
    pointer_t breakpointLimit () const {
	rtLSTORE_LStore *pContent = typecastContent ();
	return reinterpret_cast<pointer_t>(
	    rtLSTORE_LStore_BreakpointArray (pContent) + rtLSTORE_LStore_BreakpointCount (pContent) + 1
	);
    }
public:
    void getContent (VContainerHandle::Reference &rpResult) const {
	rpResult.setTo (contentHandle ());
    }
    bool getContent (Vdd::Store::Reference &rpResult) const {
	return contentHandle ()->getStore (rpResult);
    }

    void getContentPToken (rtPTOKEN_Handle::Reference &rpResult) const {
	rpResult.setTo (contentPTokenHandle ());
    }

//  Access Casts
public:
    static ThisClass *ifStringStore (Vdd::Store *pHandle) {
	ThisClass *pThisClass = dynamic_cast<ThisClass*>(pHandle);
	return pThisClass && pThisClass->isAStringStore () ? pThisClass : 0;
    }

//  Alignment
private:
    bool AlignFromContent ();
private:
    virtual bool align_() OVERRIDE {
	return align ();
    }
    virtual bool alignAll_(bool bCleaning) OVERRIDE {
	return alignAll (bCleaning);
    }
public:
    bool align ();
    bool alignAll (bool bCleaning = true);
    void alignUsingSelectedLists (DSC_Scalar &rInstance, int adjustment);
    void alignUsingSelectedLists (rtLINK_CType *pInstances, M_CPD *adjustmentUV);

//  Attribute Access
public:
    bool isAStringSet () const;
    bool isAStringStore () const {
	return rtLSTORE_LStore_StringStore (typecastContent ());
    }
    bool isInconsistent () const {
	return rtLSTORE_LStore_IsInconsistent (typecastContent ());
    }

//  Attribute Update
private:
    void clearIsAStringStore () {
	setIsAStringStoreTo (false);
    }
    void setIsAStringStore () {
	setIsAStringStoreTo (true);
    }
    void setIsAStringStoreTo (bool bValue) {
	rtLSTORE_LStore_StringStore (typecastContent ()) = bValue;
    }

    void clearIsInconsistent () {
	setIsInconsistentTo (false);
    }
    void setIsInconsistent () {
	setIsInconsistentTo (true);
    }
    void setIsInconsistentTo (bool bValue) {
	rtLSTORE_LStore_IsInconsistent (typecastContent ()) = bValue;
    }

//  Dictionary
private:
    virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const OVERRIDE {
	return static_cast<rtDICTIONARY_Handle*>(
	    (isAStringStore () ? TheStringClassDictionary () : TheListClassDictionary ()).ObjectHandle ()
	);
    }

//  Cluster Access
private:
    virtual rtPTOKEN_Handle *getPToken_() const OVERRIDE {
	return getPToken ();
    }
public:
    rtPTOKEN_Handle *getPToken () const {
	return rowPTokenHandle ();
    }

//  Cluster Forwarding
private:
    virtual bool forwardToSpace_(M_ASD *pSpace) OVERRIDE;

//  Cluster Update
private:
    virtual rtLINK_CType *addInstances_(rtPTOKEN_Handle *pAdditionPPT) OVERRIDE;

private:
    void setBreakpointCountTo (unsigned int iValue) {
	rtLSTORE_LStore_BreakpointCount (typecastContent ()) = iValue;
    }

//  Cluster Verification
public:
    void CheckBreakpointConsistency () const;
    virtual void CheckConsistency () OVERRIDE;

//  Associative Operations
private:
    virtual void associativeInsert_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeInsert_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE {
    }
    /*****/
    virtual void associativeLocate_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeLocate_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE {
    }
    /*****/
    virtual void associativeDelete_(
	VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE;
    virtual void associativeDelete_(
	VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
    ) OVERRIDE {
    }

//  Instance Deletion
private:
    virtual bool deleteInstances_(DSC_Scalar &pInstances) OVERRIDE {
	return doInstanceDeletion (pInstances);
    }
    virtual bool deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) OVERRIDE {
	return doInstanceDeletion (pInstances, rpTrues, rpFalses);
    }

//  List Cardinality Access
private:
    virtual bool getCardinality_(M_CPD *&rpResult, rtLINK_CType *pSubscript) OVERRIDE {
	return getCardinality (rpResult, pSubscript);
    }
    virtual bool getCardinality_(unsigned int &rpResult, DSC_Scalar &rSubscript) OVERRIDE {
	return getCardinality (rpResult, rSubscript);
    }
public:
    bool getCardinality (M_CPD *&rpResult, rtLINK_CType *pSubscript);
    bool getCardinality (unsigned int &rpResult, DSC_Scalar &rSubscript);

//  List Element Access
private:
    virtual bool getListElements_(
	DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int keyModifier, rtLINK_CType **ppGuard
    ) OVERRIDE {
	return getListElements (rResult, pInstances, pSubscript, keyModifier, ppGuard);
    }
    virtual bool getListElements_(
	DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
    ) OVERRIDE {
	return getListElements (rResult, rInstance, xSubscript, iModifier);
    }
    virtual bool getListElements_(
	DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
    ) OVERRIDE {
	return getListElements (rResult, rInstances, rSubscript, iModifier, ppGuard);
    }
    virtual bool getListElements_(
	DSC_Pointer&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution,
	rtINDEX_Key*&		rpKey
    ) OVERRIDE {
	rpKey = 0;
	return getListElements (
	    rInstances, rpElementStore, rpElementPointer, rpExpansion, rpDistribution
	);
    }
public:
    void getExpansion (rtLINK_CType *&rpResult, rtLINK_CType *pInstances);

    bool getListElements (
	DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int keyModifier, rtLINK_CType **ppGuard
    );
    bool getListElements (
	DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
    );
    bool getListElements (
	DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
    );

    bool getListElements (
	rtPTOKEN_Handle *ptoken, Vdd::Store::Reference &rpElementStore, rtLINK_CType *&pElementSelection, rtLINK_CType *&pElementExpansion
    );
    bool getListElements (
	DSC_Scalar&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion
    );
    bool getListElements (
	rtLINK_CType*		pInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution
    );
    bool getListElements (
	DSC_Pointer&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution
    );
    bool getListElements (
	DSC_Pointer&		rInstances,
	Vdd::Store::Reference&	rpElementStore,
	rtLINK_CType*&		rpElementPointer,
	rtLINK_CType*&		rpExpansion,
	M_CPD*&			rpDistribution,
	rtINDEX_Key*&		rpKey
    ) {
	rpKey = 0;
	return getListElements (
	    rInstances, rpElementStore, rpElementPointer, rpExpansion, rpDistribution
	);
    }

//  List Update
public:
    void appendToList (DSC_Scalar &rResult, DSC_Scalar &rInstance);
    void appendToList (rtLINK_CType *&rpResult, rtLINK_CType *pInstances);

//  Callbacks
protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual void getClusterReferenceMapData (MapEntryData &rData, unsigned int xReference) OVERRIDE;
    virtual unsigned int getClusterReferenceMapSize () OVERRIDE;

    virtual unsigned __int64 getClusterSize () OVERRIDE;

    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 3;
    }
};


/********************************
 ********************************
 *****  rtLSTORE_StringSet  *****
 ********************************
 ********************************/

class rtLSTORE_StringSet : public VSet {
//  Friends
    friend class VAssociativeCursor<rtLSTORE_StringSet, VCollectionOfStrings, char const*>;
    friend class VAssociativeCursor<rtLSTORE_StringSet, VCollectionOfUnsigned32, unsigned int>;

//  Alignment
private:
    void refreshCachedPointers ();
public:
    void align ();

//  Construction
public:
    rtLSTORE_StringSet (rtLSTORE_Handle *pSet);

//  Destruction
public:
    ~rtLSTORE_StringSet ();

//  Deferred Initialization
protected:
    virtual void initializeDPT ();

//  Access
protected:
    char *storage (unsigned int xElement) const {
	return const_cast<char*>(m_pStrings[xElement]);
    }
public:
    unsigned int cardinality () const {
	return m_pStrings.cardinality ();
    }

    char const *element (unsigned int xElement) const {
	return m_pStrings[xElement];
    }
    unsigned int origin (unsigned int xElement) const {
	return m_pStrings.stringOrigin (xElement);
    }

    int compare (unsigned int xElement, char const* pKey) const {
	return strcmp (element (xElement), pKey);
    }

    void Locate (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    bool Locate (char const* pKey, unsigned int& rxElement);

//  Update
public:
    void Insert (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Delete (
	VCollectionOfStrings *pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    unsigned int Insert (char const*  pKey, unsigned int& rxElement);
    unsigned int Delete (char const*  pKey);

protected:
    void install (unsigned int xElement, char const* pKey);

    void install (
	rtLINK_CType* pAdditions, VOrdered<VCollectionOfStrings,char const*> &rAdditions
    );

//  State
protected:
    rtLSTORE_Handle::Strings m_pStrings;
};


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

typedef char const *(*rtLSTORE_StringEnumerator) (
    bool restartEnumeration, va_list ap
);

PublicFnDecl rtLSTORE_Handle *__cdecl rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, rtLSTORE_StringEnumerator stringEnumerator, ...
);
PublicFnDecl rtLSTORE_Handle *rtLSTORE_NewStringStore (
    M_ASD *pContainerSpace, char const *pString
);

PublicFnDecl rtLSTORE_Handle *__cdecl rtLSTORE_NewStringStoreWithDelm (
    M_ASD			*pContainerSpace,
    char const			*ldelim,
    char const			*rdelim,
    rtLSTORE_StringEnumerator	 stringEnumerator,
    ...
);
PublicFnDecl rtLSTORE_Handle *rtLSTORE_NewStringStoreWithDelm (
    M_ASD *pContainerSpace, char const *ldelim, char const *rdelim, char const *pString
);

PublicFnDecl rtLSTORE_Handle *rtLSTORE_NewStringStoreFromUV (
    M_CPD*			charUV
);

PublicFnDecl void rtLSTORE_WriteDBUpdateInfo (
    M_CPD*			lstoreCPD
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTlstore.h 1 replace /users/mjc/system
  860404 20:13:20 mjc Create List Store Representation Type

 ************************************************************************/
/************************************************************************
  RTlstore.h 2 replace /users/mjc/system
  860530 14:38:22 mjc No further work for now

 ************************************************************************/
/************************************************************************
  RTlstore.h 3 replace /users/mjc/system
  860616 10:03:28 mjc Changed name from 'RTpartitn' to 'RTlstore'

 ************************************************************************/
/************************************************************************
  RTlstore.h 4 replace /users/mjc/system
  860709 10:18:38 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  RTlstore.h 5 replace /users/jck/system
  860910 14:14:52 jck Added update dump utility

 ************************************************************************/
/************************************************************************
  RTlstore.h 6 replace /users/mjc/system
  861002 18:37:15 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  RTlstore.h 7 replace /users/jad/system
  861222 17:48:36 jad implemented align for the content ptoken

 ************************************************************************/
/************************************************************************
  RTlstore.h 8 replace /users/jad/system
  870120 14:59:20 jad modified DB update procedure:

 ************************************************************************/
/************************************************************************
  RTlstore.h 9 replace /users/jad/system
  870123 16:14:38 jad implemented NewStringStore routine

 ************************************************************************/
/************************************************************************
  RTlstore.h 10 replace /users/jad/system
  870204 15:27:30 jad implemented another routine to make new string stores

 ************************************************************************/
/************************************************************************
  RTlstore.h 11 replace /users/jad/system
  870421 12:40:33 jad change interface to the Extract routines

 ************************************************************************/
/************************************************************************
  RTlstore.h 12 replace /users/jck/system
  870511 15:31:10 jck Added a copy routine and an initialized new routine

 ************************************************************************/
/************************************************************************
  RTlstore.h 13 replace /users/mjc/translation
  870524 09:33:45 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTlstore.h 14 replace /users/jck/system
  870612 13:31:20 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  RTlstore.h 15 replace /users/jck/system
  870615 06:15:00 jck Added conditional inclusion of RTlink.d

 ************************************************************************/
/************************************************************************
  RTlstore.h 16 replace /users/mjc/Software/system
  871214 10:52:57 mjc Added dummy 'rtLSTORE_AlignAll'

 ************************************************************************/
/************************************************************************
  RTlstore.h 17 replace /users/mjc/Software/system
  871214 16:25:55 mjc Added comprehensive alignment routines for all property types

 ************************************************************************/
/************************************************************************
  RTlstore.h 18 replace /users/jad/system
  880509 12:16:04 jad Implement 'sprint' for Blocks, Methods, and Closures

 ************************************************************************/
/************************************************************************
  RTlstore.h 19 replace /users/jad/system
  880711 17:03:54 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTlstore.h 20 replace /users/jck/system
  890223 12:37:52 jck Fortified against saving corrupted lstores. Implemented Comprehensive Align

 ************************************************************************/
/************************************************************************
  RTlstore.h 21 replace /users/jck/system
  890413 13:22:33 jck Added rtLSTORE_DetermineExpansion

 ************************************************************************/

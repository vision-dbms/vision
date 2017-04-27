#ifndef Vdd_Store_Interface
#define Vdd_Store_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vdd_Object.h"

#include "VMutex.h"

/**************************
 *****  Declarations  *****
 **************************/

class DSC_Descriptor;
class DSC_Pointer;
class DSC_Scalar;

class rtBLOCK_Handle;
class rtCONTEXT_Handle;
class rtDICTIONARY_Handle;
class rtINDEX_Handle;
class rtINDEX_Key;
class rtLINK_CType;
class rtLSTORE_Handle;
class rtPTOKEN_CType;
class rtVECTOR_CType;
class rtVECTOR_Handle;
class rtVSTORE_Handle;

class VAssociativeResult;
class VCollectionOfStrings;
class VCollectionOfUnsigned32;
class VContainerHandle;
class VDescriptor;
class VSelector;

namespace V {
    class VSimpleFile;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace Vdd {
    /*******************
     *----  Store  ----*
     *******************/

    class Store : public Object {
	DECLARE_FAMILY_MEMBERS (Store, Object);

    //  Aliases
    public:
	typedef VReference<ThisClass> Reference;

    //  Enumerations
    public:
	enum DictionaryLookup {
	    DictionaryLookup_FoundNothing,
	    DictionaryLookup_FoundProperty,
	    DictionaryLookup_FoundOther
	};

	enum ExtensionType {
	    ExtensionType_Add, ExtensionType_LocateOrAdd, ExtensionType_Locate
	};

    //  Construction
    protected:
	Store (Referenceable *pReferenceable) : BaseClass (pReferenceable) {
	}

    //  Destruction
    protected:
	~Store () {
	}

    //  Canonicalization
    private:
	virtual bool decodeClosure_(
	    VReference<rtBLOCK_Handle> &rpBlock, unsigned int &rxPrimitive, VReference<rtCONTEXT_Handle> *ppContext
	) const;
	virtual bool getCanonicalization_(VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer);
    public:
	bool decodeClosure (
	    VReference<rtBLOCK_Handle> &rpBlock, unsigned int &rxPrimitive, VReference<rtCONTEXT_Handle> *ppContext = 0
	) const {
	    return decodeClosure_(rpBlock, rxPrimitive, ppContext);
	}
	bool getCanonicalization (VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer const &rPointer) {
	    return getCanonicalization_(rpStore, rPointer);
	}
	void getCanonicalization (VReference<rtVSTORE_Handle> &rpStore, DSC_Pointer &rPointer);

    //  Cloning
    private:
	virtual void clone_(Reference &rpResult, rtPTOKEN_Handle *pPPT) const;
	virtual bool isACloneOf_(Vdd::Store const *pOther) const;
	virtual bool isACloneOfIndex_(rtINDEX_Handle const *pOther) const;
	virtual bool isACloneOfListStore_(rtLSTORE_Handle const *pOther) const;
	virtual bool isACloneOfValueStore_(rtVSTORE_Handle const *pOther) const;
    public:
	bool clone (Reference &rpResult, rtPTOKEN_Handle *pPPT) const {
	    clone_(rpResult, pPPT);
	    return rpResult.isntNil ();
	}
    public:
	bool isACloneOf (Vdd::Store const *pOther) const {
	    return isACloneOf_(pOther);
	}
	bool isACloneOfIndex (rtINDEX_Handle const *pOther) const {
	    return isACloneOfIndex_(pOther);
	}
	bool isACloneOfListStore (rtLSTORE_Handle const *pOther) const {
	    return isACloneOfListStore_(pOther);
	}
	bool isACloneOfValueStore (rtVSTORE_Handle const *pOther) const {
	    return isACloneOfValueStore_(pOther);
	}

	bool isntACloneOf (Vdd::Store const *pOther) const {
	    return !isACloneOf (pOther);
	}
	bool isntACloneOfIndex (rtINDEX_Handle const *pOther) const {
	    return !isACloneOfIndex (pOther);
	}
	bool isntACloneOfListStore (rtLSTORE_Handle const *pOther) const {
	    return !isACloneOfListStore (pOther);
	}
	bool isntACloneOfValueStore (rtVSTORE_Handle const *pOther) const {
	    return !isACloneOfValueStore (pOther);
	}

    //  Classification
    public:
	using BaseClass::Names;

	virtual bool Names (M_KOTM pKOTM) const {
	    return false;
	}
	virtual bool NamesTheDateClass () const {
	    return false;
	}
	virtual bool NamesTheDoubleClass () const {
	    return false;
	}
	virtual bool NamesTheFloatClass () const {
	    return false;
	}
	virtual bool NamesTheIntegerClass () const {
	    return false;
	}
	virtual bool NamesThePrimitiveClass () const {
	    return false;
	}
	virtual bool NamesTheSelectorClass () const {
	    return false;
	}
	virtual bool NamesTheNAClass () const {
	    return false;
	}
	virtual bool NamesTheTrueClass () const {
	    return false;
	}
	virtual bool NamesTheFalseClass () const {
	    return false;
	}
	virtual bool NamesABuiltInNumericClass () const {
	    return false;
	}

	using BaseClass::DoesntName;

	bool DoesntName (M_KOTM pKOTM) const {
	    return !Names (pKOTM);
	}
	bool DoesntNameTheDateClass () const {
	    return !NamesTheDateClass ();
	}
	bool DoesntNameTheDoubleClass () const {
	    return !NamesTheDoubleClass ();
	}
	bool DoesntNameTheFloatClass () const {
	    return !NamesTheFloatClass ();
	}
	bool DoesntNameTheIntegerClass () const {
	    return !NamesTheIntegerClass ();
	}
	bool DoesntNameThePrimitiveClass () const {
	    return !NamesThePrimitiveClass ();
	}
	bool DoesntNameTheSelectorClass () const {
	    return !NamesTheSelectorClass ();
	}
	bool DoesntNameTheNAClass () const {
	    return !NamesTheNAClass ();
	}
	bool DoesntNameTheTrueClass () const {
	    return !NamesTheTrueClass ();
	}
	bool DoesntNameTheFalseClass () const {
	    return !NamesTheFalseClass ();
	}
	bool DoesntNameABuiltInNumericClass () const {
	    return !NamesABuiltInNumericClass ();
	}

    //  Dictionary Access
    private:
	virtual rtDICTIONARY_Handle *getDictionary_(DSC_Pointer const &rPointer) const = 0;
    public:
	rtDICTIONARY_Handle *getDictionary (DSC_Pointer const &rPointer) const {
	    return getDictionary_(rPointer);
	}
	DictionaryLookup lookup (
	    VSelector const &rSelector, DSC_Pointer const &rPointer, unsigned int &rxPropertySlot, DSC_Descriptor *pResult = 0
	);

    //  Inheritance Access
    public:
	bool getInheritance (Reference &rpStore, DSC_Pointer &rPointer);

    //  Property Access
    private:
	virtual bool getProperty_(
	    Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
	);
    public:
	bool getProperty (
	    Reference &rpResult, DSC_Pointer &rPointer, unsigned int xPropertySlot, Vdd::Store *pPropertyPrototype
	) {
	    return getProperty_(rpResult, rPointer, xPropertySlot, pPropertyPrototype);
	}

    //  Instance Creation
    private:
	virtual rtLINK_CType *addInstances_(rtPTOKEN_Handle *pAdditionPPT) {
	    return 0;
	}
	rtLINK_CType *addExtensions_(
	    ExtensionType	xExtensionType,
	    Vdd::Store*		pInheritanceStore,
	    M_CPD*		pInheritancePointer,
	    rtLINK_CType*&	rpExtensionGuard,
	    rtLINK_CType**	ppLocateOrAddAdditions
	) {
	    return 0;
	}
	virtual bool forwardToSpace_(M_ASD *pSpace) {
	    return false;
	}
    public:
	rtLINK_CType *addInstances (rtPTOKEN_Handle *pAdditionPPT) {
	    return addInstances_(pAdditionPPT);
	}
	rtLINK_CType *addExtensions (
	    ExtensionType	xExtensionType,
	    Vdd::Store*		pInheritanceStore,
	    M_CPD*		pInheritancePointer,
	    rtLINK_CType*&	rpExtensionGuard,
	    rtLINK_CType**	ppLocateOrAddAdditions = 0 // optional locate or add result, nil if not wanted
	) {
	    return addExtensions_(
		xExtensionType, pInheritanceStore, pInheritancePointer, rpExtensionGuard, ppLocateOrAddAdditions
	    );
	}
	bool forwardToSpace (M_ASD *pSpace) {
	    return forwardToSpace_(pSpace);
	}

    //  Instance Deletion
    private:
	virtual bool deleteInstances_(DSC_Scalar &rInstances) {
	    return false;
	}
	virtual bool deleteInstances_(rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) {
	    return false;
	}
    protected:
	bool doInstanceDeletionSetup (VReference<rtPTOKEN_CType> &rpPTC, bool bIndependenceRequired = true);
	bool doInstanceDeletion (DSC_Scalar &pInstances, rtPTOKEN_CType *pPTC, VReference<rtPTOKEN_Handle> &rpNewPPT);
	bool doInstanceDeletion (
	    rtLINK_CType*		pInstances,
	    rtPTOKEN_CType*		pPTC,
	    VReference<rtPTOKEN_Handle>&rpNewPPT,
	    rtLINK_CType*&		rpTrues,
	    rtLINK_CType*&		rpFalses
	);
	bool doInstanceDeletion (DSC_Scalar &pInstances);
	bool doInstanceDeletion (rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses);
    public:
	bool deleteInstances (DSC_Scalar &rInstances) {
	    return deleteInstances_(rInstances);
	}
	bool deleteInstances (rtLINK_CType *pInstances, rtLINK_CType *&rpTrues, rtLINK_CType *&rpFalses) {
	    return deleteInstances_(pInstances, rpTrues, rpFalses);
	}

    //  Associative Operations
    private:
	virtual void associativeInsert_(
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	virtual void associativeInsert_(
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	/*****/
	virtual void associativeLocate_(
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	virtual void associativeLocate_(
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	/*****/
	virtual void associativeDelete_(
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	virtual void associativeDelete_(
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	}
	/*****/
    public:
	/*****/
	void associativeInsert (
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeInsert_(pElements, rpReordering, rAssociation);
	}
	void associativeInsert (
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeInsert_(pElements, rpReordering, rAssociation);
	}
	/*****/
	void associativeLocate (
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeLocate_(pElements, rpReordering, rAssociation);
	}
	void associativeLocate (
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeLocate_(pElements, rpReordering, rAssociation);
	}
	/*****/
	void associativeDelete (
	    VCollectionOfStrings *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeDelete_(pElements, rpReordering, rAssociation);
	}
	void associativeDelete (
	    VCollectionOfUnsigned32 *pElements, M_CPD *&rpReordering, VAssociativeResult &rAssociation
	) {
	    associativeDelete_(pElements, rpReordering, rAssociation);
	}
	/*****/

    //  Element Access
    private:
	virtual bool getElements_(VReference<rtVECTOR_Handle> &rpResult, rtLINK_CType *pSubscript) {
	    return false;
	}
	virtual bool getElements_(VDescriptor &rResult, rtLINK_CType *pSubscript) {
	    return false;
	}
	virtual bool getElements_(DSC_Descriptor &rResult, DSC_Scalar &rSubscript) {
	    return false;
	}
    public:
	bool getElements (VReference<rtVECTOR_Handle> &rpResult, rtLINK_CType *pSubscript) {
	    return getElements_(rpResult, pSubscript);
	}
	bool getElements (VDescriptor &rResult, rtLINK_CType *pSubscript) {
	    return getElements_(rResult, pSubscript);
	}
	bool getElements (DSC_Descriptor &rResult, DSC_Scalar &rSubscript) {
	    return getElements_(rResult, rSubscript);
	}

    //  Element Update
    private:
	virtual bool setElements_(rtLINK_CType *pSubscript, DSC_Descriptor &rValues) {
	    return false;
	}
	virtual bool setElements_(rtLINK_CType *pSubscript, rtVECTOR_CType *pValues) {
	    return false;
	}
	virtual bool setElements_(DSC_Scalar &rSubscript, DSC_Descriptor &rValues) {
	    return false;
	}
	virtual bool setElements_(DSC_Scalar &rSubscript, rtVECTOR_CType *pValues) {
	    return false;
	}
    public:
	bool setElements (rtLINK_CType *pSubscript, DSC_Descriptor &rValues) {
	    return setElements_(pSubscript, rValues);
	}
	bool setElements (rtLINK_CType *pSubscript, rtVECTOR_CType *pValues) {
	    return setElements_(pSubscript, pValues);
	}
	bool setElements (DSC_Scalar &rSubscript, DSC_Descriptor &rValues) {
	    return setElements_(rSubscript, rValues);
	}
	bool setElements (DSC_Scalar &rSubscript, rtVECTOR_CType *pValues) {
	    return setElements_(rSubscript, pValues);
	}

    //  List Cardinality Access
    private:
	virtual bool getCardinality_(M_CPD *&rpResult, rtLINK_CType *pSubscript) {
	    return false;
	}
	virtual bool getCardinality_(unsigned int &rpResult, DSC_Scalar &rSubscript) {
	    return false;
	}
    public:
	bool getCardinality (M_CPD *&rpResult, rtLINK_CType *pSubscript) {
	    return getCardinality_(rpResult, pSubscript);
	}
	bool getCardinality (unsigned int &rpResult, DSC_Scalar &rSubscript) {
	    return getCardinality_(rpResult, rSubscript);
	}

    //  List Element Access
    private:
	virtual bool getListElements_(
	    DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int iModifier, rtLINK_CType **ppGuard
	) {
	    return false;
	}
	virtual bool getListElements_(
	    DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
	) {
	    return false;
	}
	virtual bool getListElements_(
	    DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
	) {
	    return false;
	}
	virtual bool getListElements_(
	    DSC_Pointer	 &rInstances,
	    Reference	 &rpElementStore,
	    rtLINK_CType*&rpElementPointer,
	    rtLINK_CType*&rpExpansion,
	    M_CPD	*&rpDistribution,
	    rtINDEX_Key	*&rpKey
	) {
	    return false;
	}
    public:
	bool getListElements (
	    DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int iModifier, rtLINK_CType **ppGuard
	) {
	    return getListElements_(rResult, pInstances, pSubscript, iModifier, ppGuard);
	}
	bool getListElements (
	    DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
	) {
	    return getListElements_(rResult, rInstance, xSubscript, iModifier);
	}
	bool getListElements (
	    DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
	) {
	    return getListElements_(rResult, rInstances, rSubscript, iModifier, ppGuard);
	}
	bool getListElements (
	    DSC_Pointer	 &rInstances,
	    Reference	 &rpElementStore,
	    rtLINK_CType*&rpElementPointer,
	    rtLINK_CType*&rpExpansion,
	    M_CPD	*&rpDistribution,
	    rtINDEX_Key	*&rpKey
	) {
	    return getListElements_(
		rInstances, rpElementStore, rpElementPointer, rpExpansion, rpDistribution, rpKey
	    );
	}

    //  Mutex Management
    public:
	bool AcquireMutex (VMutexClaim &rClaim, VComputationUnit *pSupplicant);

    //  Description
    private:
	virtual void describe_(bool bVerbose);
    public:
	void describe (bool bVerbose = false) {
	    describe_(bVerbose);
	}
    public:
	virtual void visitUsing (void (VContainerHandle::*visitor)()) = 0;
	virtual void visitReferencesUsing (void (VContainerHandle::*visitor)()) = 0;
	virtual void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const = 0;

    //  State
    private:
	VMutex m_iMutex;
    };
}


#endif

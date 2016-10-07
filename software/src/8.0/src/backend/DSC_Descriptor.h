#ifndef DSC_Descriptor_Interface
#define DSC_Descriptor_Interface

/************************
 *****  Components  *****
 ************************/

#include "DSC_Store.h"
#include "DSC_Pointer.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 * Descriptors marry a store with a pointer that selects rows from that store.
 * Reflecting their pointer component, descriptors whose row pointers can be
 * replaced by equivalent links are termed link equivalent.  Although common
 * macros preclude different declarations, different descriptor type names are
 * provided to notationally preserve this distinction.
 *---------------------------------------------------------------------------
 */

/**************************************
 *****  Descriptor Access Macros  *****
 **************************************/

/************************************
 *  Descriptor Direct State Access  *
 ************************************/

#define DSC_Descriptor_Pointer(dsc) ((dsc).m_iPointer)

#define DSC_Descriptor_Type(dsc)\
    DSC_Pointer_Type (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar(dsc)\
    DSC_Pointer_Scalar (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Char(dsc)\
    DSC_Pointer_Scalar_Char (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Double(dsc)\
    DSC_Pointer_Scalar_Double (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Float(dsc)\
    DSC_Pointer_Scalar_Float (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Int(dsc)\
    DSC_Pointer_Scalar_Int (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Unsigned64(dsc)\
    DSC_Pointer_Scalar_Unsigned64 (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Unsigned96(dsc)\
    DSC_Pointer_Scalar_Unsigned96 (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Scalar_Unsigned128(dsc)\
    DSC_Pointer_Scalar_Unsigned128 (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Value(dsc)\
    DSC_Pointer_Value (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Identity(dsc)\
    DSC_Pointer_Identity (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Link(dsc)\
    DSC_Pointer_Link (DSC_Descriptor_Pointer (dsc))

#define DSC_Descriptor_Reference(dsc)\
    DSC_Pointer_Reference (DSC_Descriptor_Pointer (dsc))


/*********************************
 *  Descriptor Traversal Macros  *
 *********************************/

/*****  Macros for single traversals ... *****/

#define DSC__TravHandleNil(origin, count, value, workMacro)

#define DSC__TravHandleRepeat(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (value);\
    }\
}

#define DSC__TravHandleRange(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (value);\
	value++;\
    }\
}


#define DSC_Traverse(dsc, workMacro) {\
    M_CPD*		cpd = NilOf (M_CPD*);\
    rtLINK_CType*	linkc = NilOf (rtLINK_CType*);\
\
    DSC__SetupTraversal (&dsc, &cpd, &linkc);\
\
    if (IsntNil (cpd)) {\
	for (unsigned int ii=0; ii < UV_CPD_ElementCount (cpd); ii++) {\
	    workMacro (rtINTUV_CPD_Array (cpd)[ii]);\
	}\
    }\
    else  /* linkc */ {\
	rtLINK_TraverseRRDCListExtraArg (\
	    linkc, DSC__TravHandleNil, DSC__TravHandleRepeat, DSC__TravHandleRange, workMacro\
	);\
    }\
}


/*****  Macros for joint traversals ... *****/

#define DSC__TravHandleNil12(origin, count, value, workMacro)\
    element++

#define DSC__TravHandleRepeat1(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (rtINTUV_CPD_Array (cpd1)[element], value);\
	element++;\
    }\
}

#define DSC__TravHandleRepeat2(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (value, rtINTUV_CPD_Array (cpd2)[element]);\
	element++;\
    }\
}

#define DSC__TravHandleRange1(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (rtINTUV_CPD_Array (cpd1)[element], value);\
	value++;\
	element++;\
    }\
}

#define DSC__TravHandleRange2(origin, count, value, workMacro) {\
    while (count-- > 0) {\
	workMacro (value, rtINTUV_CPD_Array (cpd2)[element]);\
	value++;\
	element++;\
    }\
}


#define DSC_JointTraversal(dsc1, dsc2, workMacro) {\
    M_CPD *cpd1 = NilOf (M_CPD*);\
    M_CPD *cpd2 = NilOf (M_CPD*);\
    rtLINK_CType *linkc1 = NilOf (rtLINK_CType*);\
    rtLINK_CType *linkc2 = NilOf (rtLINK_CType*);\
    int			element = 0, converted;\
\
    DSC__SetupJointTraversal (\
	&dsc1, &dsc2, &cpd1, &cpd2, &linkc1, &linkc2, &converted\
    );\
\
/*****  Do the traversal now  *****/\
/*****  If both are uvectors ... *****/\
    if (IsntNil (cpd1) && IsntNil (cpd2))  {\
	for (unsigned int ii=0; ii< UV_CPD_ElementCount (cpd1); ii++) {\
	    workMacro (rtINTUV_CPD_Array (cpd1)[ii], rtINTUV_CPD_Array (cpd2)[ii]);\
	}\
    }\
\
/*****  If the first is a uvector and the second is a linkc ... *****/\
    else if (IsntNil (cpd1) && IsntNil (linkc2)) {\
	rtLINK_TraverseRRDCListExtraArg (\
	    linkc2,\
	    DSC__TravHandleRepeat1,\
	    DSC__TravHandleRepeat1,\
	    DSC__TravHandleRange1,\
	    workMacro\
	);\
    }\
\
/*****  If the first is a linkc and the second is a uvector ... *****/\
    else if (IsntNil (linkc1) && IsntNil (cpd2)) {\
	rtLINK_TraverseRRDCListExtraArg (\
	    linkc1,\
	    DSC__TravHandleRepeat2,\
	    DSC__TravHandleRepeat2,\
	    DSC__TravHandleRange2,\
	    workMacro\
	);\
    }\
\
/*****  Free cpd1 if neccessary ... *****/\
    if (converted)\
	cpd1->release ();\
}


/**********************
 *  Class Definition  *
 **********************/

class DSC_Descriptor {
//  Construction
public:
    void construct () {
	m_iStore.construct ();
	m_iPointer.construct ();
    }

    void construct (DSC_Descriptor const& rSource) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.construct (rSource.m_iPointer);
    }
    void construct (M_CPD *pStore, DSC_Pointer const &rPointer) {
	m_iStore.construct (pStore);
	m_iPointer.construct (rPointer);
    }

    void constructComposition (rtLINK_CType *pSubscript, DSC_Descriptor &rSource) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.constructComposition (pSubscript, rSource.m_iPointer);
    }
    void constructComposition (M_CPD *pSubscript, DSC_Descriptor &rSource) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.constructComposition (pSubscript, rSource.m_iPointer);
    }
    void constructComposition (DSC_Scalar &rSubscript, DSC_Descriptor &rSource) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.constructComposition (rSubscript, rSource.m_iPointer);
    }
    void constructComposition (DSC_Pointer &rSubscript, DSC_Descriptor &rSource) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.constructComposition (rSubscript, rSource.m_iPointer);
    }

    void constructScalarComposition (
	M_CPD *pStore, unsigned int xSubscript, M_CPD *pSourceUV
    ) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalarComposition (xSubscript, pSourceUV);
    }

    void constructSet (
	M_ASD *pContainerSpace, M_CPD *&rpEnumeration, DSC_Descriptor &rSource
    ) {
	m_iStore.construct (rSource.m_iStore);
	m_iPointer.constructSet (pContainerSpace, rpEnumeration, rSource.m_iPointer);
    }

    void constructCorrespondence (M_CPD *pPPT, M_CPD *pStore, unsigned int xRPTReference) {
	m_iStore.construct (pStore);
	m_iPointer.constructCorrespondence (pPPT, pStore, xRPTReference);
    }

    void constructIdentity (VConstructor *pStore, M_CPD *pPToken) {
	m_iStore.construct (pStore);
	m_iPointer.constructIdentity (pPToken);
    }
    void constructIdentity (M_CPD *pStore, M_CPD *pPToken) {
	m_iStore.construct (pStore);
	m_iPointer.constructIdentity (pPToken);
    }

    void constructLink (VConstructor *pStore, rtLINK_CType *pLink) {
	m_iStore.construct (pStore);
	m_iPointer.constructLink (pLink);
    }
    void constructLink (M_CPD *pStore, M_CPD *pLink) {
	m_iStore.construct (pStore);
	m_iPointer.constructLink (pLink);
    }
    void constructLink (M_CPD *pStore, rtLINK_CType *pLink) {
	m_iStore.construct (pStore);
	m_iPointer.constructLink (pLink);
    }

    void constructReference (M_CPD *pStore, M_CPD *pValues) {
	m_iStore.construct (pStore);
	m_iPointer.constructReference (pValues);
    }
    void constructReference (M_CPD *pStore, M_CPD *pDistribution, rtLINK_CType *pSubset) {
	m_iStore.construct (pStore);
	m_iPointer.constructReference (pDistribution, pSubset);
    }

    void constructValue (M_CPD *pStore, M_CPD *pValueMonotype) {
	m_iStore.construct (pStore);
	m_iPointer.constructValue (pValueMonotype);
    }

    void constructMonotype (M_CPD *pMonotype);

    void constructMonotype (M_CPD *pStore, M_CPD *pMonotype) {
	m_iStore.construct (pStore);
	m_iPointer.construct (pMonotype);
    }

    void constructMonotype (M_CPD *pStore, rtLINK_CType *pMonotype) {
	m_iStore.construct (pStore);
	m_iPointer.construct (pMonotype);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, DSC_Scalar const &rValue) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, rValue);
    }
    void constructScalar (M_CPD *pStore, DSC_Scalar const &rValue) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (rValue);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT);
    }
    void constructScalar (M_KOTE const &rKOTE) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructScalar (rKOTE);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructConstant (pPPT, rKOTE);
    }

    void constructNA (M_CPD *pPPT, M_KOT *pKOT) {
	constructConstant (pPPT, pKOT->TheNAClass);
    }
    void constructNA (M_KOT *pKOT) {
	constructScalar (pKOT->TheNAClass);
    }

    void constructTrue (M_CPD *pPPT, M_KOT *pKOT) {
	constructConstant (pPPT, pKOT->TheTrueClass);
    }
    void constructTrue (M_KOT *pKOT) {
	constructScalar (pKOT->TheTrueClass);
    }

    void constructFalse (M_CPD *pPPT, M_KOT *pKOT) {
	constructConstant (pPPT, pKOT->TheFalseClass);
    }
    void constructFalse (M_KOT *pKOT) {
	constructScalar (pKOT->TheFalseClass);
    }

    void constructBoolean (M_CPD *pPPT, M_KOT *pKOT, bool iBoolean) {
	constructConstant (pPPT, iBoolean ? pKOT->TheTrueClass : pKOT->TheFalseClass);
    }
    void constructBoolean (M_KOT *pKOT, bool iBoolean) {
	constructScalar (iBoolean ? pKOT->TheTrueClass : pKOT->TheFalseClass);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, char value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructScalar (M_KOTE const &rKOTE, char value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructScalar (rKOTE, value);
    }
    void constructScalar (M_KOT *pKOT, char value) {
	constructScalar (pKOT->TheCharacterClass, value);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, char value) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, char value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructConstant (pPPT, rKOTE, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, char value) {
	constructConstant (pPPT, pKOT->TheCharacterClass, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, double value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructScalar (M_KOTE const &rKOTE, double value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructScalar (rKOTE, value);
    }
    void constructScalar (M_KOT *pKOT, double value) {
	constructScalar (pKOT->TheDoubleClass, value);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, double value) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, double value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructConstant (pPPT, rKOTE, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, double value) {
	constructConstant (pPPT, pKOT->TheDoubleClass, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, float value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructScalar (M_KOTE const &rKOTE, float value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructScalar (rKOTE, value);
    }
    void constructScalar (M_KOT *pKOT, float value) {
	constructScalar (pKOT->TheFloatClass, value);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, float value) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, float value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructConstant (pPPT, rKOTE, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, float value) {
	constructConstant (pPPT, pKOT->TheFloatClass, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, int value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructScalar (M_KOTE const &rKOTE, int value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructScalar (rKOTE, value);
    }
    void constructScalar (M_KOT *pKOT, int value) {
	constructScalar (pKOT->TheIntegerClass, value);
    }

    void constructConstant (M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, int value) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOTE const &rKOTE, int value) {
	m_iStore.construct (rKOTE);
	m_iPointer.constructConstant (pPPT, rKOTE, value);
    }
    void constructConstant (M_CPD *pPPT, M_KOT *pKOT, int value) {
	constructConstant (pPPT, pKOT->TheIntegerClass, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, VkUnsigned64 const &value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructConstant (
	M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, VkUnsigned64 const &value
    ) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, VkUnsigned96 const &value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructConstant (
	M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, VkUnsigned96 const &value
    ) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }

    void constructScalar (M_CPD *pStore, M_CPD *pRPT, VkUnsigned128 const &value) {
	m_iStore.construct (pStore);
	m_iPointer.constructScalar (pRPT, value);
    }
    void constructConstant (
	M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, VkUnsigned128 const &value
    ) {
	m_iStore.construct (pStore);
	m_iPointer.constructConstant (pPPT, pRPT, value);
    }

    void constructReferenceScalar (M_CPD *pStore, M_CPD *pRPT, unsigned int value) {
	m_iStore.construct (pStore);
	m_iPointer.constructReferenceScalar (pRPT, value);
    }
    void constructReferenceConstant (
	M_CPD *pPPT, M_CPD *pStore, M_CPD *pRPT, unsigned int value
    ) {
	m_iStore.construct (pStore);
	m_iPointer.constructReferenceConstant (pPPT, pRPT, value);
    }

    void constructZero (M_CPD *pStore, RTYPE_Type xRType, M_CPD *pPPT, M_CPD *pRPT);

//  Store Access and Query
public:
    M_AND *codDatabase () const {
	return m_iStore.Database ();
    }
    M_KOT *codKOT () const {
	return m_iStore.KOT ();
    }
    M_ASD *codScratchPad () const {
	return m_iStore.ScratchPad ();
    }
    M_ASD *codSpace () const {
	return m_iStore.Space ();
    }

    void decodeClosure (
	VReference<rtBLOCK_Handle> &rpBlock, unsigned int& rxPrimitive, rtCONTEXT_Constructor** ppContext = 0
    ) const {
	m_iStore.decodeClosure (rpBlock, rxPrimitive, ppContext);
    }

    M_CPD *dictionaryCPD () const {
	return m_iStore.dictionaryCPD (m_iPointer);
    }

    void getCanonicalStore (M_CPD *&rpCanonicalStore, bool &rbConvertPointer) const {
	m_iStore.getCanonicalStore (rpCanonicalStore, rbConvertPointer, m_iPointer);
    }

    DSC_Store const &store () const {
	return m_iStore;
    }
    DSC_Store &store () {
	return m_iStore;
    }

    M_CPD *storeCPD () {
	return m_iStore.cpd ();
    }
    M_CPD *storeCPDIfAvailable () const {
	return m_iStore.cpdIfAvailable ();
    }
    VContainerHandle *storeHandle () {
	return m_iStore.containerHandle ();
    }
    unsigned int storeMask () const {
	return m_iStore.attentionMask ();
    }
    VConstructor *storePOIfAvailable () const {
	return m_iStore.poIfAvailable ();
    }
    RTYPE_Type storeRType () const {
	return m_iStore.RType ();
    }

    transientx_t *storeTransientExtension () const {
	return m_iStore.TransientExtension ();
    }
    transientx_t *storeTransientExtensionIfA (VRunTimeType const& rRTT) const {
	return m_iStore.TransientExtensionIfA (rRTT);
    }

    bool storeHasATransientExtension () const {
	return m_iStore.HasATransientExtension ();
    }
    bool storeTransientExtensionIsA (VRunTimeType const& rRTT) const {
	return m_iStore.TransientExtensionIsA (rRTT);
    }

//  Pointer Access and Query
public:
    void assignToUV (rtLINK_CType *pSubscript, M_CPD *pTarget) {
	m_iPointer.assignToUV (pSubscript, pTarget);
    }

    M_CPD *asUVector () {
	return m_iPointer.asUVector ();
    }

    unsigned int cardinality () const {
	return m_iPointer.cardinality ();
    }

    void getPPTReference (M_CPD *&pTokenRefCPD, int &pTokenRefIndex) const {
	m_iPointer.getPPTReference (pTokenRefCPD, pTokenRefIndex);
    }
    void getRPTReference (M_CPD *&pTokenRefCPD, int &pTokenRefIndex) const {
	m_iPointer.getRPTReference (pTokenRefCPD, pTokenRefIndex);
    }

    // Returns true if the u-vector must be freed by the caller
    bool getUVector (M_CPD *&rpUVector) {
	return m_iPointer.getUVector (rpUVector);
    }

    bool isEmpty () const {
	return m_iPointer.isEmpty ();
    }
    bool isntEmpty () const {
	return m_iPointer.isntEmpty ();
    }

    bool isScalar () const {
	return m_iPointer.isScalar ();
    }
    bool isntScalar () const {
	return m_iPointer.isntScalar ();
    }

    bool isACoercedScalar () const {
	return m_iPointer.isACoercedScalar ();
    }

    bool holdsAScalarValue () const {
	return m_iPointer.holdsAScalarValue ();
    }

    bool holdsAScalarReference () const {
	return m_iPointer.holdsAScalarReference ();
    }

    bool holdsNonScalarValues () const {
	return m_iPointer.holdsNonScalarValues ();
    }

    bool holdsValuesInAnyForm () const {
	return m_iPointer.holdsValuesInAnyForm ();
    }

    bool holdsAnIdentity () const {
	return m_iPointer.holdsAnIdentity ();
    }

    bool holdsAScalarIdentity () const {
	return m_iPointer.holdsAScalarIdentity ();
    }

    bool holdsALink () const {
	return m_iPointer.holdsALink ();
    }

    bool holdsNonScalarReferences () const {
	return m_iPointer.holdsNonScalarReferences ();
    }

    DSC_Pointer const &Pointer () const {
	return m_iPointer;
    }
    DSC_Pointer &Pointer () {
	return m_iPointer;
    }

    M_CPD *pointerCPD (M_CPD *pPPT) {
	return m_iPointer.pointerCPD (pPPT);
    }

    RTYPE_Type pointerRType () const {
	return m_iPointer.pointerRType ();
    }

    DSC_PointerType pointerType () const {
	return m_iPointer.Type ();
    }

    M_CPD *PPT () const {
	return m_iPointer.PPT ();
    }

    M_CPD *RPT () const {
	return m_iPointer.RPT ();
    }

    operator char const* () const {
	return m_iPointer;
    }
    operator float const* () const {
	return m_iPointer;
    }
    operator double const* () const {
	return m_iPointer;
    }
    operator int const* () const {
	return m_iPointer;
    }
    operator unsigned int const* () const {
	return m_iPointer;
    }
    operator VkUnsigned64 const* () const {
	return m_iPointer;
    }
    operator VkUnsigned96 const* () const {
	return m_iPointer;
    }
    operator VkUnsigned128 const* () const {
	return m_iPointer;
    }

//  Update
public:
    void clear () {
	m_iStore.clear ();
	m_iPointer.clear ();
    }

    void coerce (M_CPD *pPPT) {
	m_iPointer.coerce (pPPT);
    }

    void convertPointer (M_CPD *pEffectiveStoreCPD, M_CPD *pPPT) {
	m_iPointer.convert (pEffectiveStoreCPD, pPPT);
    }

    void distribute (M_CPD *distribution) {
	m_iPointer.distribute (distribution);
    }
    M_CPD *factor () {
	return m_iPointer.factor ();
    }
    void reorder (M_CPD *reordering) {
	m_iPointer.reorder (reordering);
    }

    void setStoreAttentionMaskTo (unsigned int iNewAttentionMask) {
	m_iStore.setAttentionMaskTo (iNewAttentionMask);
    }

    void setStoreTo (M_CPD *pStore) {
	m_iStore.setTo (pStore);
    }
    void setStoreTo (VConstructor *pStore) {
	m_iStore.setTo (pStore);
    }

    void setTo (M_CPD *pStore, M_CPD *pMonotype) {
	clear ();
	constructMonotype (pStore, pMonotype);
    }

    void setTo (M_CPD *pStore, rtLINK_CType *pMonotype) {
	clear ();
	constructMonotype (pStore, pMonotype);
    }

    void setToCopied (DSC_Descriptor const& rSource) {
	clear ();
	construct (rSource);
    }
    void setToMoved (DSC_Descriptor& rSource) {
	clear ();
	*this = rSource;
	rSource.construct ();
    }

    void Step (M_CPD *pEffectiveStore, bool fRequiresConversion, M_CPD *pPPT);

//  Exception Generation
public:
    void complainAboutBadPointerType (char const* pText) const {
	m_iPointer.complainAboutBadPointerType (pText);
    }

//  State
private:
    DSC_Store	m_iStore;
public:
    DSC_Pointer	m_iPointer;
};


#endif

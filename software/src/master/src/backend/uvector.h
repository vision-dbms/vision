/*****  U-Vector Facility Header File  *****/
#ifndef uvector_Interface
#define uvector_Interface

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

#include "uvector.d"

/************************
 *****  Supporting  *****
 ************************/

#include "M_CPD.h"
#include "RTptoken.h"

class rtLINK_CType;


/*******************************
 *******************************
 *****  Type Declarations  *****
 *******************************
 *******************************/

/**********************************
 *****  U-Vector Initializer  *****
 **********************************/

typedef void (*Ref_UV_Initializer) (M_CPD* pThis, size_t nElements, va_list ap);

/**************************
 *****  Lookup Types  *****
 **************************/

enum rtLINK_LookupCase {
    rtLINK_LookupCase_LT, 
    rtLINK_LookupCase_LE, 
    rtLINK_LookupCase_EQ, 
    rtLINK_LookupCase_GE, 
    rtLINK_LookupCase_GT
};


/*****************************
 *****************************
 *****  CPD Definitions  *****
 *****************************
 *****************************/

/*****  Standard CPD Pointer Count  *****/
#define UV_CPD_StdPtrCount	4

/*****  Standard CPD Pointer Interpretation  *****/
#define UV_CPx_PToken		(unsigned int)0
#define UV_CPx_RefPToken	(unsigned int)1
#define UV_CPx_AuxiliaryPOP	(unsigned int)2
#define UV_CPx_Element		(unsigned int)3

/*****  Standard CPD Access Macros  *****/
#define UV_CPD_Base(cpd)\
    ((UV_UVType *)M_CPD_ContainerBase (cpd))

#define UV_CPD_PToken(cpd)\
    M_CPD_PointerToPOP (cpd, UV_CPx_PToken)

#define UV_CPD_RefPToken(cpd)\
    M_CPD_PointerToPOP (cpd, UV_CPx_RefPToken)

#define UV_CPD_AuxiliaryPOP(cpd)\
    M_CPD_PointerToPOP (cpd, UV_CPx_AuxiliaryPOP)

#define UV_CPD_ElementCount(cpd)\
    UV_UV_ElementCount (UV_CPD_Base (cpd))

#define UV_CPD_Granularity(cpd)\
    UV_UV_Granularity (UV_CPD_Base (cpd))

#define UV_CPD_IsInconsistent(cpd)\
    UV_UV_IsInconsistent (UV_CPD_Base (cpd))

#define UV_CPD_IsASetUV(cpd)\
    UV_UV_IsASetUV (UV_CPD_Base (cpd))

#define UV_CPD_Array(cpd, elementType)\
    UV_UV_ArrayAsType (UV_CPD_Base (cpd), elementType)

#define UV_CPD_Element(cpd, elementType)\
    M_CPD_PointerToType (cpd, UV_CPx_Element, elementType *)


/******************************
 ******************************
 *****  Container Handle  *****
 ******************************
 ******************************/

class ABSTRACT rtUVECTOR_Handle : public VContainerHandle {
    DECLARE_ABSTRACT_RTT (rtUVECTOR_Handle, VContainerHandle);

//  Construction
protected:
    rtUVECTOR_Handle (M_CTE& rCTE) : VContainerHandle (rCTE) {
    }

//  Destruction
protected:
    ~rtUVECTOR_Handle () {
    }

//  Alignment
protected:
    class AlignmentFillProcessor {
	friend class rtUVECTOR_Handle;
    protected:
	AlignmentFillProcessor () {
	}
	~AlignmentFillProcessor () {
	}
    private:
	virtual void fill (void *pRegion, unsigned int sRegion) {
	    memset (pRegion, 0, sRegion);
	}
    };

private:
    virtual bool align_() OVERRIDE;
protected:
    bool align ();
    bool alignUsing (AlignmentFillProcessor &rFillProcessor);

//  Access
protected:
    UV_UVType *typecastContent () const {
	return reinterpret_cast<UV_UVType*>(containerContent ());
    }
    pointer_t arrayBase() const {
	return UV_UV_ArrayAsType (typecastContent (), char);
    }
    M_POP *pptPOP () const {
	return &UV_UV_PToken (typecastContent ());
    }
    M_POP *rptPOP () const {
	return &UV_UV_RefPToken (typecastContent ());
    }
    M_POP *auxPOP () const {
	return &UV_UV_AuxiliaryPOP (typecastContent ());
    }
public:
    rtPTOKEN_Handle *pptHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (pptPOP (), RTYPE_C_PToken));
    }
    rtPTOKEN_Handle *rptHandle () const {
	return static_cast<rtPTOKEN_Handle*>(GetContainerHandle (rptPOP (), RTYPE_C_PToken));
    }
    unsigned int cardinality () const {
	return UV_UV_ElementCount (typecastContent ());
    }
    unsigned int elementCount () const {
	return UV_UV_ElementCount (typecastContent ());
    }

    rtPTOKEN_Handle *getPPT () const {
	return pptHandle ();
    }
    rtPTOKEN_Handle *getRPT () const {
	return rptHandle ();
    }

    unsigned int granularity () const {
	return UV_UV_Granularity (typecastContent ());
    }

//  Attribute Access
public:
    bool isASet () const {
	return UV_UV_IsASetUV (typecastContent ());
    }
    bool isInconsistent () const {
	return UV_UV_IsInconsistent (typecastContent ());
    }

//  Attribute Update
protected:
    void clearIsASet () {
	setIsASetTo (false);
    }
    void setIsASet () {
	setIsASetTo (true);
    }
    void setIsASetTo (bool bValue) {
	UV_UV_IsASetUV (typecastContent ()) = bValue;
    }

    void clearIsInconsistent () {
	setIsInconsistentTo (false);
    }
    void setIsInconsistent () {
	setIsInconsistentTo (true);
    }
    void setIsInconsistentTo (bool bValue) {
	UV_UV_IsInconsistent (typecastContent ()) = bValue;
    }

//  Forwarding
private:
    virtual bool forwardToSpace_(M_ASD *pSpace) OVERRIDE;

//  Update
protected:
    void decrementElementCountBy (unsigned int iValue) {
	UV_UV_ElementCount (typecastContent ()) -= iValue;
    }
    void incrementElementCountBy (unsigned int iValue) {
	UV_UV_ElementCount (typecastContent ()) += iValue;
    }
    void setElementCountTo (unsigned int iValue) {
	UV_UV_ElementCount (typecastContent ()) = iValue;
    }

//  Callbacks
public:
    virtual void CheckConsistency () OVERRIDE;

protected:
    virtual bool PersistReferences () OVERRIDE;

//  Display and Inspection
public:
    virtual unsigned int getPOPCount () const OVERRIDE {
	return 3;
    }
    virtual bool getPOP (M_POP *pResult, unsigned int xPOP) const OVERRIDE;
};


/*********************************
 *****  UVector Copy Macros  *****
 *********************************/

#define UV_Copy(uvectorCPD) UV_BasicCopy (uvectorCPD, 0)

#define UV_CopyWithNewPToken(uvectorCPD,pToken) UV_BasicCopy (uvectorCPD, pToken)


/******************************************************
 *****  Debugger Method Definition Helper Macros  *****
 ******************************************************/

/*---------------------------------------------------------------------------
 * Element Print Debug Method Definition Macro:
 *
 *  Argument:
 *	methodName		- the name to be given to the debug method.
 *	elementPrinter		- the name of a function which will print the
 *				  element referenced by a standard CPD for the
 *				  type.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineEPrintDM(methodName, elementPrinter)\
IOBJ_DefineUnaryMethod (methodName) {\
    elementPrinter (RTYPE_QRegisterCPD (self));\
    return self;\
}


/*---------------------------------------------------------------------------
 * 'at:' Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	lcExtractFn		- the name of the type's link constructor
 *				  element extraction function.
 *	uvExtractFn		- the name of the type's reference u-vector
 *				  element extraction function.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineAtDM(methodName, lcExtractFn, uvExtractFn)\
IOBJ_DefineMethod (methodName) {\
    M_CPD* sourceCPD	= RTYPE_QRegisterCPD (self);\
    M_CPD* subscriptCPD	= RTYPE_QRegisterCPD (parameterArray[0]);\
    M_CPD* resultCPD;\
\
    if (RTYPE_C_RefUV == subscriptCPD->RType ())\
/*****  Handle the 'RefUV' case:  *****/\
	resultCPD = uvExtractFn (sourceCPD, subscriptCPD);\
    else {\
/*****  Handle the 'Link' case:  *****/\
	rtLINK_CType* linkc = rtLINK_ToConstructor (subscriptCPD);\
	resultCPD = lcExtractFn (sourceCPD, linkc);\
	linkc->release ();\
    }\
\
    return RTYPE_QRegister (resultCPD);\
}


/*---------------------------------------------------------------------------
 * 'atref:' Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	rfExtractFn		- the name of the type's reference element
 *				  extraction function.
 *	valueType		- the type of a scalar element.
 *	valueSaver		- the name of a function convert the
 *				  extracted value into an I-Object.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineAtRefDM(methodName,rfExtractFn,valueType,valueSaver)\
IOBJ_DefineMethod (methodName) {\
    M_CPD *uv = RTYPE_QRegisterCPD (self);\
    rtREFUV_Type_Reference ref;\
    ref.constructReference (\
	static_cast<rtUVECTOR_Handle*>(uv->containerHandle ())->pptHandle (),\
	IOBJ_NumericIObjectValue (parameterArray[0], int)\
    );\
    valueType result;\
    rfExtractFn (&result, uv, &ref);\
    ref.destroy ();\
\
    return valueSaver (result);\
}


/*---------------------------------------------------------------------------
 * 'at:put:' Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	lcAssignFn		- the name of the type's link constructor
 *				  element assignment function.
 *	uvAssignFn		- the name of the type's reference u-vector
 *				  element assignment function.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineAtPutDM(methodName, lcAssignFn, uvAssignFn)\
IOBJ_DefineMethod (methodName) {\
    M_CPD* targetCPD	= RTYPE_QRegisterCPD (self);\
    M_CPD* subscriptCPD	= RTYPE_QRegisterCPD (parameterArray[0]);\
    M_CPD* sourceCPD	= RTYPE_QRegisterCPD (parameterArray[1]);\
\
    if (RTYPE_C_RefUV == subscriptCPD->RType ())\
/*****  Handle the 'RefUV' case:  *****/\
	uvAssignFn (targetCPD, subscriptCPD, sourceCPD);\
    else {\
/*****  Handle the 'Link' case:  *****/\
	rtLINK_CType* linkc = rtLINK_ToConstructor (subscriptCPD);\
	lcAssignFn (targetCPD, linkc, sourceCPD);\
	linkc->release ();\
    }\
\
    return self;\
}


/*---------------------------------------------------------------------------
 * 'atref:put' Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	rfAssignFn		- the name of the type's reference element
 *				  assignment function.
 *	valueType		- the type of the scalar value expected by
 *				  the assignment function.
 *	valueInitializer	- an expression which will produce a value
 *				  suitable for assignment into a variable
 *				  of type 'valueType'.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineAtRefPutDM(methodName,rfAssignFn,valueType,valueInitializer)\
IOBJ_DefineMethod (methodName) {\
    M_CPD *uv = RTYPE_QRegisterCPD (self);\
    rtREFUV_Type_Reference ref;\
    ref.constructReference (\
	static_cast<rtUVECTOR_Handle*>(uv->containerHandle ())->pptHandle (),\
	IOBJ_NumericIObjectValue (parameterArray[0], int)\
    );\
    valueType value = (valueInitializer);\
    rfAssignFn (uv, &ref, &value);\
    ref.destroy ();\
\
    return self;\
}


/*---------------------------------------------------------------------------
 * 'at:putScalar' Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	lcAssignFn		- the name of the type's link constructor
 *				  scalar assignment function.
 *	uvAssignFn		- the name of the type's reference u-vector
 *				  scalar assignment function.
 *	valueType		- the type of the scalar value expected by
 *				  the assignment functions.
 *	valueInitializer	- an expression which will produce a value
 *				  suitable for assignment into a variable
 *				  of type 'valueType'.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineAtPutScalarDM(methodName, lcAssignFn, uvAssignFn, valueType, valueInitializer)\
IOBJ_DefineMethod (methodName) {\
    M_CPD* targetCPD	= RTYPE_QRegisterCPD (self);\
    M_CPD* subscriptCPD	= RTYPE_QRegisterCPD (parameterArray[0]);\
    valueType sourceValue = valueInitializer;\
\
    if (RTYPE_C_RefUV == subscriptCPD->RType ())\
/*****  Handle the 'RefUV' case:  *****/\
	uvAssignFn (targetCPD, subscriptCPD, &sourceValue);\
    else {\
/*****  Handle the 'Link' case:  *****/\
	rtLINK_CType* linkc = rtLINK_ToConstructor (subscriptCPD);\
	lcAssignFn (targetCPD, linkc, &sourceValue);\
	linkc->release ();\
    }\
\
    return self;\
}


/*---------------------------------------------------------------------------
 * Type Conversion Debug Method Definition Macro:
 *
 *  Arguments:
 *	methodName		- the name to be given to the debug method.
 *	conversionFn		- the name of the type conversion function.
 *
 *  Syntactic Context:
 *	Function Definition
 *
 *---------------------------------------------------------------------------
 */
#define UV_DefineTypeConversionDM(methodName, conversionFn)\
IOBJ_DefineMethod (methodName) {\
    M_CPD* pRPT = RTYPE_QRegisterCPD (parameterArray[0]);\
    M_CPD* result = conversionFn (pRPT->KOT (), RTYPE_QRegisterCPD (self));\
    return RTYPE_QRegister (result);\
}


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl void UV_InitStdCPD (M_CPD *cpd);

PublicFnDecl M_CPD* UV_New (
    RTYPE_Type			rType,
    rtPTOKEN_Handle*		pPPT,
    M_CPD*			refPTokenRefCPD,
    int				refPTokenRefIndex,
    size_t			granularity,
    Ref_UV_Initializer		initFn,
    va_list			initFnAP
);

PublicFnDecl M_CPD* UV_New (
    RTYPE_Type			rType,
    rtPTOKEN_Handle*		posPToken,
    rtPTOKEN_Handle*		refPToken,
    size_t			granularity,
    Ref_UV_Initializer		initFn,
    va_list			initFnAP
);

PublicFnDecl M_CPD* UV_BasicCopy (
    M_CPD*			uvectorCPD,
    rtPTOKEN_Handle*		pTokenCPD
);

PublicFnDecl void UV_ReclaimContainer (
    M_ASD			*ownerASD,
    M_CPreamble const		*preambleAddress
);

PublicFnDecl void UV_MarkContainers (
    M_ASD::GCVisitBase	*pGCV,
    M_ASD* pSpace,
    M_CPreamble const		*pContainer
);

PublicFnDecl void UV_InitLCExtractedUV (
    M_CPD*			resultCPD,
    size_t			nelements,
    va_list			ap
);

PublicFnDecl M_CPD* UV_LCAssign (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkc,
    M_CPD*			sourceCPD
);

PublicFnDecl M_CPD* UV_LCAssignScalar (
    M_CPD*			targetCPD,
    rtLINK_CType*		linkc,
    V::pointer_t		sourceValueAddress
);

PublicFnDecl void UV_Print (
    M_CPD*			uvectorCPD,
    int				recursive,
    int				(*elementPrinter)(M_CPD*)
);

PublicFnDecl void UV_NextElement (
    M_CPD*			uvectorCPD
);

PublicFnDecl void UV_GoToElement (
    M_CPD*			uvectorCPD,
    unsigned int		i
);


/**********************************
 *****  Debugger Definitions  *****
 **********************************/

PublicFnDecl IOBJ_IObject __cdecl UV_DM_Align (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_PToken (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_RefPToken (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_AuxiliaryPOP (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_Size (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_Granularity (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_IsInconsistent (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_IsASetUV (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_Position (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_NextElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_PreviousElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

PublicFnDecl IOBJ_IObject __cdecl UV_DM_GoToElement (
    IOBJ_IObject		self,
    IOBJ_IObject		paramterArray[]
);

#define UV_StandardDMDEPackage\
    RTYPE_StandardDMDEPackage\
    IOBJ_MDE ("align"		, UV_DM_Align)\
    IOBJ_MDE ("ptoken"		, UV_DM_PToken)\
    IOBJ_MDE ("refPToken"	, UV_DM_RefPToken)\
    IOBJ_MDE ("auxiliaryPOP"	, UV_DM_AuxiliaryPOP)\
    IOBJ_MDE ("size"		, UV_DM_Size)\
    IOBJ_MDE ("granularity"	, UV_DM_Granularity)\
    IOBJ_MDE ("isInconsistent"	, UV_DM_IsInconsistent)\
    IOBJ_MDE ("isASetUV"	, UV_DM_IsASetUV)\
    IOBJ_MDE ("position"	, UV_DM_Position)\
    IOBJ_MDE ("next"		, UV_DM_NextElement)\
    IOBJ_MDE ("previous"	, UV_DM_PreviousElement)\
    IOBJ_MDE ("goTo:"		, UV_DM_GoToElement)


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  uvector.h 1 replace /users/mjc/system
  860323 17:21:00 mjc New U-Vector facility

 ************************************************************************/
/************************************************************************
  uvector.h 2 replace /users/mjc/system
  860410 19:20:12 mjc Added P-Token support

 ************************************************************************/
/************************************************************************
  uvector.h 3 replace /users/mjc/system
  860414 12:43:17 mjc Release version of 'uvector' consistent with RT{int,float,double}uv.c

 ************************************************************************/
/************************************************************************
  uvector.h 4 replace /users/mjc/system
  860420 18:49:11 mjc Replace for hjb to change 'M_{StandardCPD,NewSPadContainer}' calls

 ************************************************************************/
/************************************************************************
  uvector.h 5 replace /users/mjc/system
  860422 22:57:10 mjc Return to library for 'hjb'

 ************************************************************************/
/************************************************************************
  uvector.h 6 replace /users/mjc/system
  860504 18:46:13 mjc Release uvectors

 ************************************************************************/
/************************************************************************
  uvector.h 7 replace /users/jad/system
  860514 16:15:58 jad added routines to implement the 
stepValues method

 ************************************************************************/
/************************************************************************
  uvector.h 8 replace /users/jck/system
  860519 01:16:44 jck Fixed spelling of auxiliaryPOP message

 ************************************************************************/
/************************************************************************
  uvector.h 9 replace /users/mjc/system
  860523 00:57:46 mjc Added 'reference P-Token' field, fixed print CPD initialization bug

 ************************************************************************/
/************************************************************************
  uvector.h 10 replace /users/mjc/system
  860617 15:10:01 mjc Added and declared container save function

 ************************************************************************/
/************************************************************************
  uvector.h 11 replace /users/jad/system
  860723 16:13:27 jad added Copy routines

 ************************************************************************/
/************************************************************************
  uvector.h 12 replace /users/jad/system
  861216 17:39:00 jad added set uvectors

 ************************************************************************/
/************************************************************************
  uvector.h 13 replace /users/jck/system
  870415 09:54:07 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  uvector.h 14 replace /users/mjc/translation
  870524 09:42:49 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  uvector.h 15 replace /users/jck/system
  871007 13:33:21 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  uvector.h 16 replace /users/jck/system
  890222 15:10:47 jck Implemented corruption detection mechanism

 ************************************************************************/

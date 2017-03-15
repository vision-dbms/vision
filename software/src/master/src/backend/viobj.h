/*****  Internal Object Interface  *****/
#ifndef IOBJ_Interface
#define IOBJ_Interface

#ifndef Vk_Interface
#include "Vk.h"
#endif

/*********************
 *****  Globals  *****
 *********************/

class M_ASD;

PublicVarDecl M_ASD *IOBJ_ScratchPad;

/**************************************************************************
 *****  I-Object Method and Method Dictionary Definitions and Macros  *****
 **************************************************************************/
/*---------------------------------------------------------------------------
 * The protocol associated with an I-Object is defined in terms of a method
 * dictionary.  A method dictionary is a 'Null' terminated array of method
 * method dictionary entries.  Each method dictionary entry associates a
 * character string message name with the address of a function called a
 * method that implements the operation described by the message.  Methods are
 * functions with the following declaration:
 *
 *	IOBJ_IObject <method> (receipient, parameterArray)
 *
 * where:
 *	receipient		- is the I-Object to which the message was
 *				  sent.
 *	parameterArray		- is an array of I-Objects which serve as the
 *				  parameters of the message.
 *
 * The method is expected to return an I-Object which is the result of its
 * execution.
 *
 *****/

/**********************
 *  Type Definitions  *
 **********************/

/*****  I-Object Reference  *****/
typedef struct IOBJ_IObject *Ref_IOBJ_IObject;

/*****  Messages  *****/
typedef char const* IOBJ_MessageSelector;

/*****  Methods  *****/
typedef IOBJ_IObject (__cdecl *IOBJ_Method)(IOBJ_IObject, IOBJ_IObject[]);

/*****  Method Dictionary Entry and Method Dictionary  *****/
typedef struct IOBJ_MDEntry {
    IOBJ_MessageSelector	messageSelector;
    IOBJ_Method			method;
} *IOBJ_MD;


/******************************
 *  Method Definition Macros  *
 ******************************/

#define IOBJ_DefineMethod(methodName)\
PrivateFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		self,\
    IOBJ_IObject		parameterArray[]\
)

#define IOBJ_DefineNewaryMethod(methodName)\
PrivateFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		Unused(self),\
    IOBJ_IObject		parameterArray[]\
)

#define IOBJ_DefineNilaryMethod(methodName)\
PrivateFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		Unused(self),\
    IOBJ_IObject		Unused(parameterArray)[]\
)

#define IOBJ_DefineUnaryMethod(methodName)\
PrivateFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		self,\
    IOBJ_IObject		Unused(parameterArray)[]\
)

#define IOBJ_DefinePublicMethod(methodName)\
PublicFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		self,\
    IOBJ_IObject		parameterArray[]\
)

#define IOBJ_DefinePublicNewaryMethod(methodName)\
PublicFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		Unused(self),\
    IOBJ_IObject		parameterArray[]\
)

#define IOBJ_DefinePublicNilaryMethod(methodName)\
PublicFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		Unused(self),\
    IOBJ_IObject		Unused(parameterArray)[]\
)

#define IOBJ_DefinePublicUnaryMethod(methodName)\
PublicFnDef IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		self,\
    IOBJ_IObject		Unused(parameterArray)[]\
)

#define IOBJ_DeclarePublicMethod(methodName)\
PublicFnDecl IOBJ_IObject __cdecl methodName (\
    IOBJ_IObject		self,\
    IOBJ_IObject		parameterArray[]\
)


/*****************************************
 *  Method Dictionary Definition Macros  *
 *****************************************/

/*****  Method Dictionary Entry Definition Macro  *****/
/*---------------------------------------------------------------------------
 * The following macro is used to declare a method dictionary entry.  Uses of
 * this macro are always bracketed by 'IOBJ_BeginMD' and 'IOBJ_EndMD' or
 * 'IOBJ_BeginIType' and 'IOBJ_EndIType'.
 *---------------------------------------------------------------------------
 */

#define IOBJ_MDE(methodName, methodFunction) {methodName, (IOBJ_Method)methodFunction},

/*****  Method Dictionary Definition Macros  *****/
/*---------------------------------------------------------------------------
 * The following macros are used to bracket a collection of method dictionary
 * entries.  These macros are automatically inserted by the '...DefineIType'
 * and '...DefinePublicIType' I-Type definition macros; they must be explicit
 * as part of the definition of a custom I-Type or method dictionary.
 *---------------------------------------------------------------------------
 */

#define IOBJ_BeginMD(methodDictionaryName)\
PrivateVarDef IOBJ_MDEntry methodDictionaryName[] = {

#define IOBJ_EndMD\
    IOBJ_MDE (NilOf(IOBJ_MessageSelector), (IOBJ_Method)IOBJ_BasicMethod)\
}


/********************************************************
 *****  I-Object and I-Type Definitions and Macros  *****
 ********************************************************/
/*---------------------------------------------------------------------------
 * Structurally, an I-Object consists of two parts - an I-Type and an
 * I-Content.  The I-Type of an I-Object is a function with the following
 * declaration:
 *
 *	IOBJ_MD <itypeFn> (containingIObject)
 *
 * where
 *	containingIObject	- is the I-Object containing the I-Type.  While
 *				  most I-Types can ignore this argument, is
 *				  provided to allow non-standard I-Types to
 *				  examine the content of the I-Object they are
 *				  servicing.  The 'RegisterReferent' and
 *				  'RegisterContent' I-Types defined by the
 *				  object memory manager are two such
 *				  non-standard I-Types.
 *
 * The I-Type function is expected to return a method dictionary (i.e., the
 * address of the first entry in a method dictionary) appropriate for the
 * I-Object.  The I-Content of an I-Object is a union capable of being
 * interpreted as an 'int', 'double', or 'pointer_t' as suits the needs of the
 * I-Type of the object.
 *---------------------------------------------------------------------------
 */

/**********************
 *  Type Definitions  *
 **********************/


/*---------------------------------------------------------------------------
 *****			   Facility Type Definition			*****
 * 'typedef' for a facility.  This type definition is appropriate for
 * declaring a variable which refers to a facility.
 *---------------------------------------------------------------------------
 */
struct FAC_Request;  // forward declaration

typedef void (*FAC_Facility)(
    FAC_Request *requestAddress
);


/*****  I-Type  *****/
typedef IOBJ_MD (__cdecl *IOBJ_IType)(IOBJ_IObject);

/*****  I-Content  *****/
union IOBJ_IContent {
    unsigned int		asCardinal;
    int				asInt;
    double			asDouble;
    void *			asAddress;
    FAC_Facility                asFacility;
};

/*****  I-Object  *****/
struct IOBJ_IObject {
    IOBJ_IType			iType;
    IOBJ_IContent		iContent;
};


/******************************
 *  I-Type Definition Macros  *
 ******************************/

#define IOBJ_BeginIType(iTypeName)\
PublicFnDef IOBJ_ITypeFunctionHeader(iTypeName)\
{\
    IOBJ_BeginMD (methodDictionary)

#define IOBJ_EndIType\
    IOBJ_EndMD;\
    return methodDictionary;\
}

#define IOBJ_DeclareIType(iTypeName)\
PublicFnDecl IOBJ_ITypeFunctionHeader(iTypeName)

#define IOBJ_ITypeFunctionHeader(iTypeName)\
IOBJ_MD __cdecl iTypeName (IOBJ_IObject containingIObject)


/************************************
 *  I-Object Content Access Macros  *
 ************************************/

#define IOBJ_IObjectIType(iObject)		((iObject).iType)
#define IOBJ_IObjectValueAsInt(iObject)		((iObject).iContent.asInt)
#define IOBJ_IObjectValueAsDouble(iObject)	((iObject).iContent.asDouble)
#define IOBJ_IObjectValueAsAddress(iObject)	((iObject).iContent.asAddress)
#define IOBJ_IObjectValueAsFacility(iObject)	((iObject).iContent.asFacility)

#define IOBJ_IsAnInstanceOf(iObject, iType)(IOBJ_IObjectIType (iObject) == iType)

#define IOBJ_IObjectMustBe(iObject, iType, errorMessage)\
{\
    if (!IOBJ_IsAnInstanceOf (iObject, iType)) ERR_SignalFault (\
	EC__UsageError, errorMessage\
    );\
}


/*****************************************************
 *****  Numeric I-Object Value Extraction Macro  *****
 *****************************************************/

#define IOBJ_NumericIObjectValue(iObject, resultType) (\
    IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Char)\
    ? (resultType)IOBJ_IObjectValueAsInt (iObject)\
    : IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Int)\
    ? (resultType)IOBJ_IObjectValueAsInt (iObject)\
    : IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Double)\
    ? (resultType)IOBJ_IObjectValueAsDouble (iObject)\
    : (resultType)ERR_SignalFault (\
	EC__UsageError, "Not a numeric I-Object"\
    )\
)


/*******************
 *******************
 *****  State  *****
 *******************
 *******************/

PublicVarDecl IOBJ_IObject IOBJ_TheNilIObject; /* added by m2 */


/**********************
 **********************
 *****  Behavior  *****
 **********************
 **********************/

PublicFnDecl IOBJ_IObject IOBJ_SendMessage (
    IOBJ_IObject		self,
    IOBJ_MessageSelector	messageSelector,
    IOBJ_IObject		parameterIObjectArray[]
);

PublicFnDecl IOBJ_IObject IOBJ_PackIntIObject (
    IOBJ_IType			iType,
    int				value
);

PublicFnDecl IOBJ_IObject IOBJ_PackDoubleIObject (
    IOBJ_IType			iType,
    double			value
);

PublicFnDecl IOBJ_IObject IOBJ_PackAddressIObject (
    IOBJ_IType			iType,
    void *			value
);

PublicFnDecl IOBJ_IObject IOBJ_PackFacilityIObject (
    IOBJ_IType			iType,
    FAC_Facility		value
);


/*********************
 *********************
 *****  I-Types  *****
 *********************
 *********************/

/*****  'Nil'  *****/
IOBJ_DeclareIType (IOBJ_IType_Nil);

/*****  'Char'   *****/
IOBJ_DeclareIType (IOBJ_IType_Char);

#define IOBJ_CharIObject(value)\
    IOBJ_PackIntIObject (IOBJ_IType_Char, value)

/*****  'Int'   *****/
IOBJ_DeclareIType (IOBJ_IType_Int);

#define IOBJ_IntIObject(value)\
    IOBJ_PackIntIObject (IOBJ_IType_Int, value)

/*****  'Double'  *****/
IOBJ_DeclareIType (IOBJ_IType_Double);

#define IOBJ_DoubleIObject(value)\
    IOBJ_PackDoubleIObject (IOBJ_IType_Double, value)

/*****  'Switch'  *****/
IOBJ_DeclareIType (IOBJ_IType_Switch);

#define IOBJ_SwitchIObject(value)\
    IOBJ_PackAddressIObject (IOBJ_IType_Switch, value)

/*****  'Basic' Method Dictionary  *****/
IOBJ_DeclareIType (IOBJ_BasicMethod);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  iobj.h 1 replace /users/mjc/system
  860226 16:22:59 mjc create

 ************************************************************************/
/************************************************************************
  iobj.h 2 replace /users/hjb/system
  860428 19:10:37 hjb added inheritance of method Dictionaries

 ************************************************************************/
/************************************************************************
  iobj.h 3 replace /users/mjc/system
  860508 18:14:37 mjc Added 'character' I-Object

 ************************************************************************/
/************************************************************************
  iobj.h 4 replace /users/mjc/translation
  870524 09:39:19 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  iobj.h 5 replace /users/m2/backend
  890927 15:30:32 m2 Made IOBJ_TheNilIObject a global var instead of a procedure call

 ************************************************************************/

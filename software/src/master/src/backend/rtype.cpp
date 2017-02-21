/*****  Representation Type Definition Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName RTYPE

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTlink.h"

/*****  Shared  *****/
#include "rtype.h"


/*********************************************
 *****  Representation Type Definitions  *****
 *********************************************/

/*---------------------------------------------------------------------------
 * Helper declarations, macros, and utilities
 *---------------------------------------------------------------------------
 */

/*****  Type Description Structure Type Definition  *****/
struct TypeDescriptionType {
    RTYPE_Type	type;
    int		(*handlerFn)(RTYPE_HandlerOperation, V::VArgList);
};

/*****  Handler Function Declaration Macros  *****/
#define DeclareHandler(rtype)\
PublicFnDecl int rtype(\
    RTYPE_HandlerOperation handlerOperation, V::VArgList ap\
)

/*****  Type Description Definition Macros  *****/
#define BeginTypeDescriptions \
PrivateVarDef TypeDescriptionType TypeDescriptionArray [] = {

#define EndTypeDescriptions	};

#define TypeDescription(typeCode, handlerFn)\
{typeCode, handlerFn}


/***************************************************************
 *****  Representation Type Handler Function Declarations  *****
 ***************************************************************/

DeclareHandler (rtUNDEF_ConversionHandler);
DeclareHandler (rtUNDEF_Handler);

DeclareHandler (rtTCT_Handler);
DeclareHandler (rtPCT_Handler);

DeclareHandler (rtDSC_ConversionHandler);
DeclareHandler (rtDSC_Handler);

DeclareHandler (rtSTRING_Handler);
DeclareHandler (rtPARRAY_Handler);

DeclareHandler (rtPTOKEN_Handler);
DeclareHandler (rtLINK_Handler);

DeclareHandler (rtCHARUV_Handler);
DeclareHandler (rtDOUBLEUV_Handler);
DeclareHandler (rtFLOATUV_Handler);
DeclareHandler (rtINTUV_Handler);
DeclareHandler (rtREFUV_Handler);
DeclareHandler (rtSELUV_Handler);
DeclareHandler (rtUNDEFUV_Handler);
DeclareHandler (rtU64UV_Handler);
DeclareHandler (rtU96UV_Handler);
DeclareHandler (rtU128UV_Handler);

DeclareHandler (rtBLOCK_Handler);
DeclareHandler (rtCLOSURE_Handler);
DeclareHandler (rtCONTEXT_Handler);
DeclareHandler (rtDICTIONARY_ConversionHandler);
DeclareHandler (rtDICTIONARY_Handler);
DeclareHandler (rtMETHOD_Handler);

DeclareHandler (rtINDEX_Handler);
DeclareHandler (rtLSTORE_Handler);
DeclareHandler (rtVECTOR_Handler);
DeclareHandler (rtVSTORE_Handler);


/**********************************************
 *****  Representation Type Descriptions  *****
 **********************************************/

BeginTypeDescriptions

    TypeDescription (RTYPE_C_Undefined0		, rtUNDEF_ConversionHandler),

    TypeDescription (RTYPE_C_TCT		, rtTCT_Handler),
    TypeDescription (RTYPE_C_PCT		, rtPCT_Handler),
    TypeDescription (RTYPE_C_Descriptor1	, rtDSC_ConversionHandler),

    TypeDescription (RTYPE_C_String		, rtSTRING_Handler), 
    TypeDescription (RTYPE_C_PArray		, rtPARRAY_Handler),

    TypeDescription (RTYPE_C_PToken		, rtPTOKEN_Handler),

    TypeDescription (RTYPE_C_Link		, rtLINK_Handler),

    TypeDescription (RTYPE_C_CharUV		, rtCHARUV_Handler),
    TypeDescription (RTYPE_C_DoubleUV		, rtDOUBLEUV_Handler),
    TypeDescription (RTYPE_C_FloatUV		, rtFLOATUV_Handler),
    TypeDescription (RTYPE_C_IntUV		, rtINTUV_Handler),
    TypeDescription (RTYPE_C_RefUV		, rtREFUV_Handler),
    TypeDescription (RTYPE_C_SelUV		, rtSELUV_Handler),
    TypeDescription (RTYPE_C_UndefUV		, rtUNDEFUV_Handler),

    TypeDescription (RTYPE_C_MethodD		, rtDICTIONARY_ConversionHandler),

    TypeDescription (RTYPE_C_Block		, rtBLOCK_Handler),
    TypeDescription (RTYPE_C_Method		, rtMETHOD_Handler),
    TypeDescription (RTYPE_C_Context		, rtCONTEXT_Handler),
    TypeDescription (RTYPE_C_Closure		, rtCLOSURE_Handler),

    TypeDescription (RTYPE_C_Vector		, rtVECTOR_Handler),
    TypeDescription (RTYPE_C_ListStore		, rtLSTORE_Handler),
    TypeDescription (RTYPE_C_Undefined		, rtUNDEF_Handler),
    TypeDescription (RTYPE_C_ValueStore		, rtVSTORE_Handler),

    TypeDescription (RTYPE_C_Descriptor0	, rtDSC_ConversionHandler),

    TypeDescription (RTYPE_C_Index		, rtINDEX_Handler),

    TypeDescription (RTYPE_C_Descriptor		, rtDSC_Handler),
    TypeDescription (RTYPE_C_Dictionary		, rtDICTIONARY_Handler),

    TypeDescription (RTYPE_C_Unsigned64UV	, rtU64UV_Handler),
    TypeDescription (RTYPE_C_Unsigned96UV	, rtU96UV_Handler),
    TypeDescription (RTYPE_C_Unsigned128UV	, rtU128UV_Handler),

/***  'RTYPE_C_MaxType' Dummy Entry  ***/
    TypeDescription (RTYPE_C_MaxType		, NilOf (Ref_RTYPE_Handler))

EndTypeDescriptions

/*****  Type Code Index Array  *****/
/*---------------------------------------------------------------------------
 * The representation type description array defined in above is not
 * guaranteed to be in any particular order.  The following array, indexed by
 * (int)typeCode is initialized during system startup.
 *---------------------------------------------------------------------------
 */

PrivateVarDef TypeDescriptionType* TypeDescriptionPtrArray [(int)RTYPE_C_MaxType + 1];


/***********************************************************
 *****  Standard Representation Type Dispatch Routine  *****
 ***********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to perform a representation type handler operation.
 *
 *  Arguments:
 *	operation		- the operation to be performed.
 *	type			- the type for which the operation is to be
 *				  performed.
 *	args			- any arguments which must be passed to the
 *				  handler.  These arguments are passed as a
 *				  "stdarg" pointer initialized to point to
 *				  the first argument.  The arguments expected
 *				  for each operation are defined in "rtype.d"
 *				  along with the "RTYPE_HandlerOperation"
 *				  type definition.
 *  Returns:
 *	The result code returned by the representation type handler -
 *	0 if operation succeeded, -1 otherwise.
 *
 *****/
PublicFnDef int __cdecl RTYPE_PerformHandlerOperation (
    RTYPE_HandlerOperation handlerOp, RTYPE_Type type, ...
) {
    V_VARGLIST (ap, type);

    TypeDescriptionType* pTypeDescription = TypeDescriptionPtrArray[(int)type];
    int result = IsntNil (pTypeDescription) ? (*pTypeDescription->handlerFn) (handlerOp, ap) : -1;

    return result;
}


/***************************************************************
 *****  Specialized Representation Type Dispatch Routines  *****
 ***************************************************************/

/***************
 *  Type Name  *
 ***************/

/*---------------------------------------------------------------------------
 *****  Routine to return a representation type id as a string.
 *
 *  Argument:
 *	r			- the representation type whose string name is
 *				  desired.
 *
 *  Returns:
 *	The address of a read only string naming the representation type id.
 *
 *****/
PublicFnDef char const* RTYPE_TypeIdAsString (
    RTYPE_Type			r
)
{
/*---------------------------------------------------------------------------
 * Redefine the 'RTYPE_TypeId' macro so that the representation type list
 * expands into an initializer for the representation type id string array.
 *---------------------------------------------------------------------------
 */
#undef RTYPE_TypeId
#define RTYPE_TypeId(rTypeId) #rTypeId

    static char const*rTypeIdStrings [] = {RTYPE_RTypeList};

    return
	RTYPE_IsAValidType (r)
	? rTypeIdStrings[(int)r]
	: UTIL_FormatMessage ("RTYPE#%d", (int)r);

/*****  Undefine 'RTYPE_TypeId' to avoid its subsequent mis-use  *****/
#undef RTYPE_TypeId
}


/***************************
 *  Object Print Routines  *
 ***************************/

/***** The C Pre-Processor on the VAX complains if the fallback arguments **/
/***** are not specified so the following DummyFallback can be used.      **/
/***** It is possible the optimizer may complain but that's OK.           **/
#define DummyFallback	&& (1)

/*****  Print Routine Generation Macros  *****/
#define TryPrintOperation(operation) RTYPE_PerformHandlerOperation(\
    operation, cpd->RType (), cpd\
) < 0

#define FallBackToPrintOperation(operation, fallBack)\
    && TryPrintOperation (operation) fallBack

#define DefinePrintRoutine(routineName, operation, fallBack)\
PublicFnDef void routineName (M_CPD *existingCPD, int whichPointer) {\
    M_CPD *cpd = existingCPD->GetCPD (whichPointer);\
    if (TryPrintOperation (operation) fallBack) {\
	if (whichPointer >= 0 && existingCPD->Database () != cpd->Database ()) {\
	    M_POP const *pPOP = M_CPD_PointerToPOP (existingCPD, whichPointer);\
	    IO_printf ("[%u:%u]->", M_POP_ObjectSpace (pPOP), M_POP_ContainerIndex (pPOP));\
	}\
	cpd->print ();\
    };\
    cpd->release ();\
}


/*****  Print Routines  *****/
/*---------------------------------------------------------------------------
 * The following macro expansions produce routines which 'QPrint', 'Print',
 * 'RPrint' an object respectively.  All three routines have the following
 * synopsis:
 *
 *  Arguments:
 *	existingCPD		- the address of a CPD which points to the POP
 *				  for the object to be printed.
 *	whichPointer		- the index of the CPD pointer which points to
 *				  the POP defined above.
 *
 *  Returns:
 *	NOTHING
 *
 *---------------------------------------------------------------------------
 */
DefinePrintRoutine
(
    RTYPE_QPrint,
    RTYPE_QPrintObject,
    DummyFallback
)

DefinePrintRoutine
(
    RTYPE_Print,
    RTYPE_PrintObject,
    FallBackToPrintOperation
    (
        RTYPE_QPrintObject,
        DummyFallback
    )
)

DefinePrintRoutine
(
    RTYPE_RPrint,
    RTYPE_RPrintObject,
    FallBackToPrintOperation
    (
        RTYPE_PrintObject,
	FallBackToPrintOperation
	(
	    RTYPE_QPrintObject,
	    DummyFallback
	)
    )
)


/*********************************
 *****  Q-Register Debugger  *****
 *********************************/
/*---------------------------------------------------------------------------
 * As part of its support for representation types, this facility defines a
 * collection of 'registers' for holding container browsing CPD's.  There are
 * two flavors of these 'Q-Registers' - named and temporary.  The named
 * Q-Registers are accessible directly as literals in the debugger and hold
 * their current value until it is explicitly replaced.  The temporary
 * Q-Registers exist as an applicative convenience for routines that are
 * expected to return an I-Object for a container browser CPD.  Temporary
 * Q-Registers hold their current values until the system decides it is time
 * to reuse the temporary.  Representation type container browser methods that
 * wish to return a container browser do so calling RTYPE_Browser which sets
 * up a CPD for the container browser and deposits it in a temporary
 * Q-Register.
 *---------------------------------------------------------------------------
 */

/*****************************
 *  Q-Register Declarations  *
 *****************************/

/*****  Q-Register Content Array  *****/
/*---------------------------------------------------------------------------
 * The Q-Registers are implemented as an array of CPD's.  A Q-Register is
 * considered empty when its CPD entry is Nil.
 *---------------------------------------------------------------------------
 */

/*****  Q-Register Array  *****/
PrivateVarDef M_CPD *QRegisters [RTYPE_TotalQRegisterCount];

/*****
 *  The Last Temporary Q-Register Used
 *  (This is 0 based you must add RTYPE_PermanentQRegisterCount)
 *****/
PrivateVarDef int TemporaryQRegisterLastUsed = -1;


/***********************************************
 *  Q-Register Startup Initialization Routine  *
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize the Q-Registers at system startup.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NONE
 *
 *****/
PrivateFnDef void InitializeQRegisters () {
    for (unsigned int i = 0; i < RTYPE_TotalQRegisterCount; i++)
	QRegisters [i] = NilOf (M_CPD*);
    TemporaryQRegisterLastUsed = -1;
}


/***********************************
 *  Q-Register Assignment Routine  *
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to change the contents of a Q-Register.
 *
 *  Arguments:
 *	xQRegister		- the index of the Q-Register to be changed.
 *	newCPD			- the index of the new CPD to be placed in
 *				  the specified Q-Register.
 *
 *  Returns:
 *	'xQRegister'
 *
 *****/
PrivateFnDef unsigned int AssignCPDToQRegister (unsigned int xQRegister, M_CPD *newCPD) {
    M_CPD *currentCPD = QRegisters [xQRegister];

    QRegisters [xQRegister] = IsNil (newCPD) ? newCPD : newCPD->GetCPD ();

    if (currentCPD)
	currentCPD->release ();

    return xQRegister;
}


/*******************************************
 *  Q-Register Referent I-Type Definition  *
 *******************************************/

/*****  Utilities  *****/
#define PackQRegisterReferentIObject(qr)\
    IOBJ_PackIntIObject (QRegisterReferentIType, qr)

/*****  I-Type Definition  *****/
PrivateFnDef IOBJ_ITypeFunctionHeader (QRegisterReferentIType)
{
    IOBJ_BeginMD (emptyQRegisterMD)
    IOBJ_EndMD;

    IOBJ_BeginMD (defaultObjectMD)
	IOBJ_MDE ("qprint"	, RTYPE_QPrintIObject)
	IOBJ_MDE ("print"	, RTYPE_PrintIObject)
	IOBJ_MDE ("rprint"	, RTYPE_RPrintIObject)
    IOBJ_EndMD;

    M_CPD *cpd = RTYPE_QRegisterCPD (containingIObject);
    IOBJ_MD resultMD;

    return
	IsNil (cpd)
	? emptyQRegisterMD
	: RTYPE_PerformHandlerOperation (RTYPE_InstanceMD, cpd->RType (), &resultMD)
	? defaultObjectMD
	: resultMD;
}


/**********************************
 *  Q-Register I-Type Definition  *
 **********************************/

/*****  Utilities  *****/
#define PackQRegisterIObject(qr)\
    IOBJ_PackIntIObject (QRegisterIType, qr)

#define UnpackQRegisterIObject(iObject)\
    IOBJ_IObjectValueAsInt (iObject)


/*****  Methods  *****/
IOBJ_DefineUnaryMethod (DisplayQRegister) {
    int qr = UnpackQRegisterIObject (self);

    if (qr >= 0 && qr < RTYPE_PermanentQRegisterCount)
	IO_printf ("#%s", RTYPE_QRegisterName ((RTYPE_QRegisterId)qr));
    else
	IO_printf ("#q[%d]", qr);

    return self;
}

IOBJ_DefineUnaryMethod (QRegisterReferent) {
    return PackQRegisterReferentIObject (UnpackQRegisterIObject (self));
}


IOBJ_DefineMethod (SetQRegister) {
    AssignCPDToQRegister (UnpackQRegisterIObject (self), RTYPE_QRegisterCPD (parameterArray[0]));
    return self;
}

IOBJ_DefineMethod (SetRQRegister) {
    AssignCPDToQRegister (UnpackQRegisterIObject (self), RTYPE_QRegisterCPD (parameterArray[0]));

    return PackQRegisterReferentIObject (UnpackQRegisterIObject (self));
}

IOBJ_DefineUnaryMethod (SetQRegisterFromLastTemp) {
    M_CPD *cpd = (TemporaryQRegisterLastUsed == -1)
	? NilOf (M_CPD*)
        : QRegisters [TemporaryQRegisterLastUsed + RTYPE_PermanentQRegisterCount];
    AssignCPDToQRegister (UnpackQRegisterIObject (self), cpd);

    return self;
}

IOBJ_DefineUnaryMethod (ClearQRegister) {
    AssignCPDToQRegister (UnpackQRegisterIObject (self), NilOf (M_CPD*));

    return self;
}


IOBJ_DefineUnaryMethod (DumpContainerDM) {
    M_CPD*			cpd = RTYPE_QRegisterCPD (self);
    M_CPreamble*		preamble;
    pointer_t			p, pl;
    unsigned int i;

    for (
	i = 1,
	preamble = M_CPD_PreamblePtr (cpd),
	p = (pointer_t)preamble,
	pl = p
	   + sizeof (M_CPreamble)
	   + sizeof (M_CEndMarker) 
	   + M_CPreamble_Size (preamble);

	p < pl;

	p += sizeof (int)
    ) IO_printf ("%08X%c", *(int *)p, 0 == i++ % 8 ? '\n' : ' ');
    IO_printf ("\n");

    return self;
}


/*****  I-Type Definition  *****/
IOBJ_BeginIType (QRegisterIType)
    IOBJ_MDE ("qprint"			, DisplayQRegister)
    IOBJ_MDE ("print"			, DisplayQRegister)
    IOBJ_MDE ("referent"		, QRegisterReferent)
    IOBJ_MDE ("ref"			, QRegisterReferent)
    IOBJ_MDE ("set:"			, SetQRegister)
    IOBJ_MDE ("setr:"			, SetRQRegister)
    IOBJ_MDE ("clear"			, ClearQRegister)
    IOBJ_MDE ("setFromLast"		, SetQRegisterFromLastTemp)
    IOBJ_MDE ("dumpContainer"		, DumpContainerDM)
IOBJ_EndIType


/**************************
 *  Q-Register Allocator  *
 **************************/

/*---------------------------------------------------------------------------
 *****  Basic routine for allocating a temporary Q-Register.
 *
 *  Argument:
 *	cpd			- the address of a CPD for a new container
 *				  browser.
 *
 *  Returns:
 *	A Q-Register Referent I-Object for a temporary Q-Register. The CPD
 *	currently contained in the temporary Q-Register will be freed.
 *
 *  Notes:
 *	This routine will copy the CPD it is passed and discard the original;
 *	consequently, the CPD passed to this routine MUST NOT BE USED after
 *	this routine returns !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *	This routine returns a referent to simplify the applicative use of
 *	browsers.
 *
 *****/
PublicFnDef IOBJ_IObject RTYPE_QRegister (M_CPD *cpd) {
    TemporaryQRegisterLastUsed = (TemporaryQRegisterLastUsed + 1) % RTYPE_TemporaryQRegisterCount;

    IOBJ_IObject result = PackQRegisterReferentIObject (
	AssignCPDToQRegister (TemporaryQRegisterLastUsed + RTYPE_PermanentQRegisterCount, cpd)
    );
    cpd->release ();

    return result;
}

PublicFnDef IOBJ_IObject RTYPE_QRegister (rtLINK_CType *lc) {
    return RTYPE_QRegister (lc->ToLink ());
}


/*****************************************
 *****  Container Browser Allocator  *****
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Standard container browser interface.
 *
 *  Arguments:
 *	existingCPD		- the address of a CPD which will serve as the
 *				  source of the physical object pointer to be
 *				  used in constructing the new browser CPD.
 *	whichPointer		- the index of the pointer to the POP described
 *				  above.  If whichPointer < 0, the new CPD will
 *				  refer to the same container as the existing
 *				  CPD; if whichPointer >= 0, whichPointer is
 *				  the index of a pointer in the existing CPD's
 *				  pointer array.
 *
 *  Returns:
 *	An I-Object of I-Type 'Q-Register' identifying the temporary Q-Register
 *	in which the new container browser was placed.
 *
 *****/
PublicFnDef IOBJ_IObject RTYPE_Browser (M_CPD *existingCPD, int whichPointer) {
    return RTYPE_QRegister (existingCPD->GetCPD (whichPointer));
}


/*********************************************************
 *  Q-Register / Q-Register Referent CPD Access Routine  *
 *********************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the CPD associated with a Q-Register or Q-Register
 *****  Referent I-Object.
 *
 *  Argument:
 *	iObject			- an I-Object of I-Type 'QRegisterIType' or
 *				  'QRegisterReferentIType'.
 *
 *  Returns:
 *	The address of the CPD currently associated with the Q-Register or
 *	'NilOf (M_CPD*)' if no CPD is currently associated with the
 *	register.
 *
 *****/
PublicFnDef M_CPD *RTYPE_QRegisterCPD (IOBJ_IObject iObject) {
    int qr;

    if (IOBJ_IsAnInstanceOf (iObject, QRegisterIType) ||
        IOBJ_IsAnInstanceOf (iObject, QRegisterReferentIType)
    ) qr = IOBJ_IObjectValueAsInt (iObject);
    else ERR_SignalFault (
	EC__UsageError,
	"RTYPE_QRegisterCPD: Not a Q-Register I-Object"
    );

    return QRegisters [qr];
}


/*************************************************
 *   Q-Register <-> Debugger Interface Routines  *
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the I-Object representing a Q-Register.
 *
 *  Arguments:
 *	xQRegister		- the index of the Q-Register.
 *
 *  Returns:
 *	An I-Object representing the Q-Register or 'IOBJ_TheNilIObject'
 *	if 'xQRegister' does not select a valid Q-Register.
 *
 *****/
PublicFnDef IOBJ_IObject RTYPE_QRegisterIObject (RTYPE_QRegisterId xQRegister) {
    return static_cast<unsigned int>(xQRegister) < RTYPE_TotalQRegisterCount
	? PackQRegisterIObject ((int)xQRegister)
	: IOBJ_TheNilIObject;
}


/*---------------------------------------------------------------------------
 *****  Routine to return a READ ONLY pointer to a character string containing
 *****  the name of the specified Q-Register.
 *
 *  Argument:
 *	xQRegister		- the index of the Q-Register whose name is
 *				  desired.
 *
 *  Returns:
 *	The address of a string naming the specified Q-Register.
 *
 *****/
PublicFnDef char const *RTYPE_QRegisterName (RTYPE_QRegisterId xQRegister) {
    static char const *QName[] = {
	"qa", "qb", "qc", "qd", "qe", "qf", "qg", "qh", "qi", "qj", "qk", "ql", "qm",
	"qn", "qo", "qp", "qq", "qr", "qs", "qt", "qu", "qv", "qw", "qx", "qy", "qz",
	"tq0", "tq1", "tq2", "tq3", "tq4", "tq5", "tq6", "tq7", "tq8", "tq9"
    };
    return static_cast<unsigned int>(xQRegister) < sizeof (QName) / sizeof (QName[0]) ? QName[xQRegister] : "q?";
}


/**************************************************
 *****  Representation Type I-Object Support  *****
 **************************************************/

/***************
 *  Utilities  *
 ***************/

#define UnpackRTypeIObject(iObject)\
    (RTYPE_Type)IOBJ_IObjectValueAsInt (iObject)


/*****************************
 *  Type Method Definitions  *
 *****************************/

IOBJ_DefinePublicUnaryMethod (RTYPE_DisplayTypeIObject) {
    IO_printf ("#%s", RTYPE_TypeIdAsString (UnpackRTypeIObject (self)));

    return self;
}

/***********************
 *  I-Type Definition  *
 ***********************/

PublicFnDef IOBJ_ITypeFunctionHeader (RTYPE_IType) {
    IOBJ_BeginMD (defaultMD)
	IOBJ_MDE ("qprint"	, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	, RTYPE_DisplayTypeIObject)
    IOBJ_EndMD;

    IOBJ_MD resultMD;

    return
	RTYPE_PerformHandlerOperation (
	    RTYPE_TypeMD,
	    UnpackRTypeIObject (containingIObject),
	    &resultMD
	)
	? defaultMD
	: resultMD;
}


/*************************************************
 *  Representation Type I-Object Access Routine  *
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to return the I-Object corresponding to a representation type.
 *
 *  Argument:
 *	r			- the index of the representation type whose
 *				  I-Object is desired.
 *
 *  Returns:
 *	An I-Object representing the representation type if the representation
 *	type index is valid, otherwise IOBJ_TheNilIObject.
 *
 *****/
PublicFnDef IOBJ_IObject RTYPE_TypeIObject (RTYPE_Type r) {
    return
	RTYPE_IsAValidType (r)
	? IOBJ_PackIntIObject (RTYPE_IType, (int)r)
	: IOBJ_TheNilIObject;
}


/***********************************************************
 *****  Standard Representation Type Instance Methods  *****
 ***********************************************************/

IOBJ_DefinePublicUnaryMethod (RTYPE_QPrintIObject) {
    RTYPE_QPrint (RTYPE_QRegisterCPD (self), -1);
    return self;
}

IOBJ_DefinePublicUnaryMethod (RTYPE_PrintIObject) {
    RTYPE_Print (RTYPE_QRegisterCPD (self), -1);
    return self;
}

IOBJ_DefinePublicUnaryMethod (RTYPE_RPrintIObject) {
    RTYPE_RPrint (RTYPE_QRegisterCPD (self), -1);
    return self;
}


/******************************
 *****  Facility Methods  *****
 ******************************/

/*****  Flush Temporary Q-Registers  *****/
IOBJ_DefineUnaryMethod (FlushTQRegisters) {
    for (unsigned int i = RTYPE_PermanentQRegisterCount; i < RTYPE_TotalQRegisterCount; i++)
        AssignCPDToQRegister (i, NilOf (M_CPD*));
    TemporaryQRegisterLastUsed = -1;

    return self;
}

IOBJ_DefineUnaryMethod (DisplayLastTempQRegister) {
    if (TemporaryQRegisterLastUsed == -1)
	IO_printf ("NONE USED");
    else
	IO_printf ("#tq%d", TemporaryQRegisterLastUsed);
    return self;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    int i, n;
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("flushTQRegisters"	, FlushTQRegisters)
	IOBJ_MDE ("flush"		, FlushTQRegisters)
	IOBJ_MDE ("printLast"		, DisplayLastTempQRegister)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (RTYPE);
    FAC_FDFCase_FacilityDescription
        ("Representation Type Definitions");
    case FAC_ReturnFacilityMD:
        FAC_MDResultField = methodDictionary;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    case FAC_DoStartupInitialization:
/*****  Initialize the representation type description pointer array  *****/
        for (i = 0; i <= (int)RTYPE_C_MaxType; i++)
	    TypeDescriptionPtrArray[i] = NilOf (TypeDescriptionType *);

	n = sizeof TypeDescriptionArray / sizeof (TypeDescriptionType);
	for (i = 0; i < n; i++)
	    TypeDescriptionPtrArray[(int)TypeDescriptionArray[i].type] = TypeDescriptionArray + i;

/*****  Initialize the Q-Registers  *****/
	InitializeQRegisters ();

	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rtype.c 1 replace /users/mjc/system
  860226 16:24:48 mjc create

 ************************************************************************/
/************************************************************************
  rtype.c 2 replace /users/mjc/system
  860321 17:59:07 mjc Release 1 of new R-Type architecture

 ************************************************************************/
/************************************************************************
  rtype.c 3 replace /users/mjc/system
  860323 17:22:01 mjc Release of additional work on new memory architecture

 ************************************************************************/
/************************************************************************
  rtype.c 4 replace /users/mjc/system
  860327 22:34:08 mjc Added methods to flush the temporary Q-Registers and POP cache

 ************************************************************************/
/************************************************************************
  rtype.c 5 replace /users/mjc/system
  860408 09:41:02 mjc Fixed 'flushPOPCache' method to check for POP Cache presence

 ************************************************************************/
/************************************************************************
  rtype.c 6 replace /users/mjc/system
  860408 22:44:47 mjc Added 'browser' message to POP Cache I-Type

 ************************************************************************/
/************************************************************************
  rtype.c 7 replace /users/mjc/system
  860410 19:18:28 mjc Added standard print services and 'RPrint'

 ************************************************************************/
/************************************************************************
  rtype.c 8 replace /users/mjc/system
  860412 18:14:09 mjc Changed 'RTpvector' references to 'RTparray'

 ************************************************************************/
/************************************************************************
  rtype.c 9 replace /users/mjc/system
  860414 12:42:23 mjc Eliminate POP Cache

 ************************************************************************/
/************************************************************************
  rtype.c 10 replace /users/hjb/system
  860423 00:38:37 hjb done updating calls

 ************************************************************************/
/************************************************************************
  rtype.c 11 replace /users/hjb/system
  860428 19:31:26 hjb added RTYPE_C_PObject declarations

 ************************************************************************/
/************************************************************************
  rtype.c 12 replace /users/hjb/system
  860505 00:51:06 hjb added ways to handle logical types

 ************************************************************************/
/************************************************************************
  rtype.c 13 replace /users/jad/system
  860515 14:26:56 jad added 2 methods to access the last temp
Q Register used

 ************************************************************************/
/************************************************************************
  rtype.c 14 replace /users/mjc/system
  860615 19:02:06 mjc Eliminated use of 'ltype'

 ************************************************************************/
/************************************************************************
  rtype.c 15 replace /users/mjc/system
  860624 10:52:52 mjc Moved CPD allocation timing tests from 'mainbody' to messages to Q-Registers

 ************************************************************************/
/************************************************************************
  rtype.c 16 replace /users/jad/system
  860630 12:07:08 jad added debug methods: ref and setr

 ************************************************************************/
/************************************************************************
  rtype.c 17 replace /users/mjc/system
  860817 20:17:26 mjc Added indirect CPD access timing routines

 ************************************************************************/
/************************************************************************
  rtype.c 18 replace /users/mjc/system
  860819 12:32:36 mjc Added timers for 'M_PreambleAddress'

 ************************************************************************/
/************************************************************************
  rtype.c 19 replace /users/jad/system
  860821 16:43:24 jad added flush as a synonym for flushTQRegisters

 ************************************************************************/
/************************************************************************
  rtype.c 20 replace /users/cmm/system
  870519 16:12:22 cmm Implement container dumper debug message

 ************************************************************************/
/************************************************************************
  rtype.c 21 replace /users/mjc/MERGE/vax
  870519 17:15:30 mjc Added omitted '*' in function address dereference

 ************************************************************************/
/************************************************************************
  rtype.c 22 replace /users/mjc/translation
  870524 09:41:12 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rtype.c 23 replace /users/mjc/system
  870614 20:13:57 mjc Replaced UNIX specific include files with 'stdoss.h'

 ************************************************************************/
/************************************************************************
  rtype.c 24 replace /users/mjc/Software/system
  871123 20:10:48 mjc Added experimental CPCC allocation routine and '#M displayMappingInfo' debug method

 ************************************************************************/
/************************************************************************
  rtype.c 25 replace /users/jck/system
  880824 09:06:39 jck The rtype printers now flush stdout. (Makes them more useful inside 'cdb'

 ************************************************************************/
/************************************************************************
  rtype.c 26 replace /users/m2/backend
  890503 15:30:08 m2 Fixed poorly defined macro

 ************************************************************************/

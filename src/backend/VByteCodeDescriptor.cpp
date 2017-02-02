/*****  VByteCodeDescriptor Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VByteCodeDescriptor.h"

/************************
 *****  Supporting  *****
 ************************/

#include "selector.h"

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VCodeTable<VByteCodeDescriptor::ByteCode, VByteCodeDescriptor, (size_t)256>;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VByteCodeDescriptor::VByteCodeDescriptor (
    ByteCode		xByteCode,
    ParameterStructure	xParameterStructure,
    DispatchCase	xDispatchCase,
    unsigned int	xAssociatedKSI,
    char const*		pByteCodeName,
    bool		fSuspendSafe
)
: VCodeDescriptor	(xByteCode, pByteCodeName)
, m_xParameterStructure	(xParameterStructure)
, m_xDispatchCase	(xDispatchCase)
, m_xAssociatedKSI	(xAssociatedKSI)
, m_fSuspendSafe	(fSuspendSafe)
{
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

/*******************************
 *****  The Undefined KSI  *****
 *******************************/

static unsigned int const KS__NONE = (unsigned int)-1;

/***********************************************
 *****  The Default Byte Code Description  *****
 ***********************************************/

VByteCodeDescriptor const VByteCodeDescriptor::DefaultDescription (
    VByteCodeDescriptor::ByteCode_Error,
    VByteCodeDescriptor::ParameterStructure_Unknown,
    VByteCodeDescriptor::DispatchCase_Unimplemented,
    KS__NONE,
    "undef"
);


/****************************************
 *****  The Byte Code Descriptions  *****
 ****************************************/

static VByteCodeDescriptor const Descriptions[] = {
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Pad0,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"pad0",
	true
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Pad1,
	VByteCodeDescriptor::ParameterStructure_Pad,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"pad1",
	true
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Pad2,
	VByteCodeDescriptor::ParameterStructure_PadS,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"pad2",
	true
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Tag,
	VByteCodeDescriptor::ParameterStructure_Tag,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"tag",
	true
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Comment,
	VByteCodeDescriptor::ParameterStructure_Tag,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"comment",
	true
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_CommentS,
	VByteCodeDescriptor::ParameterStructure_TagS,
	VByteCodeDescriptor::DispatchCase_NoOp,
	KS__NONE,
	"comments",
	true
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_AcquireParameter,
	VByteCodeDescriptor::ParameterStructure_SSI,
	VByteCodeDescriptor::DispatchCase_AcquireParameter,
	KS__NONE,
	"acqp"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_AcquireParameterS,
	VByteCodeDescriptor::ParameterStructure_SSIS,
	VByteCodeDescriptor::DispatchCase_AcquireParameter,
	KS__NONE,
	"acqps"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_AcquireKnownParameter,
	VByteCodeDescriptor::ParameterStructure_KSI,
	VByteCodeDescriptor::DispatchCase_AcquireKnownParameter,
	KS__NONE,
	"acqkp"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryMessage,
	VByteCodeDescriptor::ParameterStructure_SSI,
	VByteCodeDescriptor::DispatchCase_UnaryMessage,
	KS__NONE,
	"unarymsg"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryMessageS,
	VByteCodeDescriptor::ParameterStructure_SSIS,
	VByteCodeDescriptor::DispatchCase_UnaryMessage,
	KS__NONE,
	"unarymsgs"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_KeywordMessage,
	VByteCodeDescriptor::ParameterStructure_SSI_Valence,
	VByteCodeDescriptor::DispatchCase_KeywordMessage,
	KS__NONE,
	"keywordmsg"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_KeywordMessageS,
	VByteCodeDescriptor::ParameterStructure_SSIS_Valence,
	VByteCodeDescriptor::DispatchCase_KeywordMessage,
	KS__NONE,
	"keywordmsgs"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_KnownMessage,
	VByteCodeDescriptor::ParameterStructure_KSI,
	VByteCodeDescriptor::DispatchCase_KnownMessage,
	KS__NONE,
	"knownmsg"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_LAssign,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__LAssign,
	"lassign"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_RAssign,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__RAssign,
	"rassign"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Concat,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Comma,
	"concat"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_LT,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__LessThan,
	"lt"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_LE,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__LessThanOrEqual,
	"le"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_GE,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__GreaterThanOrEqual,
	"ge"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_GT,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__GreaterThan,
	"gt"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Equal,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Equal,
	"eq"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Equivalent,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Equivalent,
	"equiv"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_NEqual,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__NotEqual,
	"neq"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_NEquivalent,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__NotEquivalent,
	"nequiv"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Or,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Or,
	"or"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_And,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__And,
	"and"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Add,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Plus,
	"add"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Subtract,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Minus,
	"sub"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Multiply,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Times,
	"mul"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Divide,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_OptimizedMessage,
	KS__Divide,
	"div"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Dispatch,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_Dispatch,
	KS__NONE,
	"dispatch"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_DispatchMagic,
	VByteCodeDescriptor::ParameterStructure_MWI,
	VByteCodeDescriptor::DispatchCase_DispatchMagic,
	KS__NONE,
	"dispatchMagic"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_NextMessageParameter,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_NextMessageParameter,
	KS__NONE,
	"nextp"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Value,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_Value,
	KS__NONE,
	"value"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Intension,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_Intension,
	KS__NONE,
	"intension"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_KnownUnaryValue,
	VByteCodeDescriptor::ParameterStructure_KSI_MWI,
	VByteCodeDescriptor::DispatchCase_KnownUnaryValue,
	KS__NONE,
	"knownuv"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_KnownUnaryIntension,
	VByteCodeDescriptor::ParameterStructure_KSI_MWI,
	VByteCodeDescriptor::DispatchCase_KnownUnaryIntension,
	KS__NONE,
	"knownui"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryValue,
	VByteCodeDescriptor::ParameterStructure_SSI_MWI,
	VByteCodeDescriptor::DispatchCase_UnaryValue,
	KS__NONE,
	"unaryv"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryValueS,
	VByteCodeDescriptor::ParameterStructure_SSIS_MWI,
	VByteCodeDescriptor::DispatchCase_UnaryValue,
	KS__NONE,
	"unaryvs"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryIntension,
	VByteCodeDescriptor::ParameterStructure_SSI_MWI,
	VByteCodeDescriptor::DispatchCase_UnaryIntension,
	KS__NONE,
	"unaryi"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_UnaryIntensionS,
	VByteCodeDescriptor::ParameterStructure_SSIS_MWI,
	VByteCodeDescriptor::DispatchCase_UnaryIntension,
	KS__NONE,
	"unaryis"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_WrapupUnaryValue,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_WrapupUnaryValue,
	KS__NONE,
	"wrapuv"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_WrapupUnaryIntension,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_WrapupUnaryIntension,
	KS__NONE,
	"wrapui"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreMagic,
	VByteCodeDescriptor::ParameterStructure_MWI,
	VByteCodeDescriptor::DispatchCase_StoreMagic,
	KS__NONE,
	"stmagic"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreLexBinding,
	VByteCodeDescriptor::ParameterStructure_PLI,
	VByteCodeDescriptor::DispatchCase_StoreContext,
	KS__NONE,
	"stlexb"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreLexBindingS,
	VByteCodeDescriptor::ParameterStructure_PLIS,
	VByteCodeDescriptor::DispatchCase_StoreContext,
	KS__NONE,
	"stlexbs"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreInteger,
	VByteCodeDescriptor::ParameterStructure_Int,
	VByteCodeDescriptor::DispatchCase_StoreInteger,
	KS__NONE,
	"stint"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreDouble,
	VByteCodeDescriptor::ParameterStructure_Double,
	VByteCodeDescriptor::DispatchCase_StoreDouble,
	KS__NONE,
	"stdbl"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreString,
	VByteCodeDescriptor::ParameterStructure_SLI,
	VByteCodeDescriptor::DispatchCase_StoreString,
	KS__NONE,
	"ststr"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreStringS,
	VByteCodeDescriptor::ParameterStructure_SLIS,
	VByteCodeDescriptor::DispatchCase_StoreString,
	KS__NONE,
	"ststrs"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreSelector,
	VByteCodeDescriptor::ParameterStructure_SSI_Valence,
	VByteCodeDescriptor::DispatchCase_StoreSelector,
	KS__NONE,
	"stsel"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreSelectorS,
	VByteCodeDescriptor::ParameterStructure_SSIS_Valence,
	VByteCodeDescriptor::DispatchCase_StoreSelector,
	KS__NONE,
	"stsels"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreKnownSelector,
	VByteCodeDescriptor::ParameterStructure_KSI,
	VByteCodeDescriptor::DispatchCase_StoreKnownSelector,
	KS__NONE,
	"stksel"
    ),
    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_StoreNoValue,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_StoreNoValue,
	KS__NONE,
	"stnoval"
    ),


    VByteCodeDescriptor (
	VByteCodeDescriptor::ByteCode_Exit,
	VByteCodeDescriptor::ParameterStructure_None,
	VByteCodeDescriptor::DispatchCase_Exit,
	KS__NONE,
	"exit"
    )
};


/********************************************************
 *  Byte Code Description Table Initialization Routine  *
 ********************************************************/

VByteCodeDescriptorCodeTable const VByteCodeDescriptor__CodeTable (
    "Byte Code",
    sizeof (Descriptions) / sizeof (VByteCodeDescriptor),
    Descriptions,
    &VByteCodeDescriptor::DefaultDescription
);

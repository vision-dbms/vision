#ifndef VByteCodeDescriptor_Interface
#define VByteCodeDescriptor_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCodeDescriptor.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VByteCodeDescriptor : public VCodeDescriptor {


/****************************
 ****************************
 *****  The Byte Codes  *****
 ****************************
 ****************************/

/*---------------------------------------------------------------------------
 * The following #define's attach numeric values to the byte codes.  Each byte
 * code is accompanied by a synopsis of the form:
 *
 *	operator parameterName[parameterType] parameterName[parameterType] ...
 *
 * where 'parameterName' is one of:
 *
 *	ksi			- known selector index
 *	mwi			- magic word index
 *	pli			- physical literal index
 *	ssi			- string selector index
 *	sli			- string literal index
 *	tag			- tag value
 *	value			- numeric value
 *	valence			- selector valence
 *	void			- ignored numeric value
 *
 * and 'parameterType' is one of:
 *
 *	byte			- signed byte
 *	ubyte			- unsigned byte
 *	short			- aligned, signed short
 *	ushort			- aligned, unsigned short
 *	xshort			- un-aligned, signed short
 *	xushort			- un-aligned, unsigned short
 *	int			- aligned, signed int
 *	xint			- un-aligned, signed int
 *	double			- double
 *
 *---------------------------------------------------------------------------
 */
public:
    enum ByteCode {
	ByteCode_Pad0			= 1,	// pad0
	ByteCode_Pad1			= 2,	// pad1		void	[byte]
	ByteCode_Pad2			= 3,	// pad2		void	[xshort]

	ByteCode_Tag			= 4,	// tag		tag	[byte]

	ByteCode_Comment		= 5,	// comment	ssi	[ubyte]
	ByteCode_CommentS		= 6,	// comments	ssi	[xushort]

	ByteCode_AcquireParameter	= 16,	// acqp		ssi	[ubyte]
	ByteCode_AcquireParameterS	= 17,	// acqps	ssi	[ushort]
	ByteCode_AcquireKnownParameter  = 19,	// acqkp	ksi	[ubyte]

	ByteCode_UnaryMessage		= 32,	// unarymsg	ssi	[ubyte]
	ByteCode_UnaryMessageS		= 33,	// unarymsgs	ssi	[ushort]
	ByteCode_KeywordMessage		= 35,	// keywordmsg	ssi	[ubyte]
						//		valence	[ubyte]
	ByteCode_KeywordMessageS	= 36,	// keywordmsgs	ssi	[ushort]
						//		valence	[ubyte]

	ByteCode_KnownMessage		= 38,	// knownmsg	ksi	[ubyte]

	ByteCode_LAssign		= 39,	// lassign
	ByteCode_RAssign		= 40,	// rassign
	ByteCode_Concat			= 41,	// concat
	ByteCode_LT			= 42,	// lt
	ByteCode_LE			= 43,	// le
	ByteCode_GE			= 44,	// ge
	ByteCode_GT			= 45,	// gt
	ByteCode_Equal			= 46,	// eq
	ByteCode_Equivalent		= 47,	// equiv
	ByteCode_NEqual			= 48,	// neq
	ByteCode_NEquivalent		= 49,	// nequiv
	ByteCode_Or			= 50,	// or
	ByteCode_And			= 51,	// and
	ByteCode_Add			= 52,	// add
	ByteCode_Subtract		= 53,	// sub
	ByteCode_Multiply		= 54,	// mul
	ByteCode_Divide			= 55,	// div

	ByteCode_Dispatch		= 96,	// dispatch
	ByteCode_DispatchMagic		= 97,	// dispatchMagic mwi	[ubyte]

	ByteCode_NextMessageParameter	= 100,	// nextp

	ByteCode_Value			= 101,	// value
	ByteCode_Intension		= 102,	// intension

	ByteCode_KnownUnaryValue	= 128,	// knownuv	ksi	[ubyte]
						//		mwi	[ubyte]
	ByteCode_KnownUnaryIntension	= 129,	// knownui	ksi	[ubyte]
						//		mwi	[ubyte]

	ByteCode_UnaryValue		= 130,	// unaryv	ssi	[ubyte]
						//		mwi	[ubyte]
	ByteCode_UnaryValueS		= 131,	// unaryvs	ssi	[ushort]
						//		mwi	[ubyte]
	ByteCode_UnaryIntension		= 133,	// unaryi	ssi	[ubyte]
						//		mwi	[ubyte]
	ByteCode_UnaryIntensionS	= 134,	// unaryis	ssi	[ushort]
						//		mwi	[ubyte]

	ByteCode_WrapupUnaryValue	= 136,	// wrapuv
	ByteCode_WrapupUnaryIntension	= 137,	// wrapui

	ByteCode_StoreMagic		= 160,	// stmagic	mwi	[ubyte]
	ByteCode_StoreLexBinding	= 161,	// stlexb	pli	[ubyte]
	ByteCode_StoreLexBindingS	= 162,	// stlexbs	pli	[ushort]

	ByteCode_StoreInteger		= 176,	// stint	value	[int]
	ByteCode_StoreDouble		= 177,	// stdbl	value	[double]
	ByteCode_StoreString		= 178,	// ststr	sli	[ubyte]
	ByteCode_StoreStringS		= 179,	// ststrs	sli	[ushort]
	ByteCode_StoreSelector		= 181,	// stsel	ssi	[ubyte]
						//		valence	[ubyte]
	ByteCode_StoreSelectorS		= 182,	// stsels	ssi	[ushort]
						//		valence	[ubyte]
	ByteCode_StoreKnownSelector	= 184,	// stksel	ksi	[ubyte]
	ByteCode_StoreNoValue		= 185,	// stnoval

	ByteCode_Exit			= 192,	// exit

	ByteCode_Error			= 255	// error
    };



/******************************************
 ******************************************
 *****  The Byte Code Dispatch Cases  *****
 ******************************************
 ******************************************/
/*---------------------------------------------------------------------------
 * This enumerated type defines the cases of the virtual machine's internal
 * byte code switch.
 *---------------------------------------------------------------------------
 */
public:
    enum DispatchCase {
	DispatchCase_Unimplemented,

	DispatchCase_NoOp,

	DispatchCase_AcquireParameter,
	DispatchCase_AcquireKnownParameter,

	DispatchCase_UnaryMessage,
	DispatchCase_KeywordMessage,
	DispatchCase_KnownMessage,
	DispatchCase_OptimizedMessage,

	DispatchCase_Dispatch,
	DispatchCase_DispatchMagic,
	DispatchCase_NextMessageParameter,

	DispatchCase_Value,
	DispatchCase_Intension,
	DispatchCase_KnownUnaryValue,
	DispatchCase_KnownUnaryIntension,
	DispatchCase_UnaryValue,
	DispatchCase_UnaryIntension,
	DispatchCase_WrapupUnaryValue,
	DispatchCase_WrapupUnaryIntension,

	DispatchCase_StoreMagic,
	DispatchCase_StoreContext,
	DispatchCase_StoreInteger,
	DispatchCase_StoreDouble,
	DispatchCase_StoreSelector,
	DispatchCase_StoreKnownSelector,
	DispatchCase_StoreString,
	DispatchCase_StoreNoValue,

	DispatchCase_Exit
    };


/************************************************
 ************************************************
 *****  The Byte Code Parameter Structures  *****
 ************************************************
 ************************************************/
/*---------------------------------------------------------------------------
 * This enumerated type defines the set of possible byte code parameter
 * arrangements.
 *---------------------------------------------------------------------------
 */
public:
    enum ParameterStructure {
	ParameterStructure_Unknown,

	ParameterStructure_None,

	ParameterStructure_Pad,		/* ignored ubyte */
	ParameterStructure_PadS,	/* ignored ushort */

	ParameterStructure_Tag,		/* ubyte */
	ParameterStructure_TagS,	/* ushort */
	ParameterStructure_TagInt,	/* xint */

	ParameterStructure_KSI,		/* ubyte */
	ParameterStructure_KSI_MWI,	/* ubyte ubyte */

	ParameterStructure_MWI,		/* ubyte */

	ParameterStructure_SSI,		/* ubyte */
	ParameterStructure_SSI_Valence,	/* ubyte ubyte */
	ParameterStructure_SSI_MWI,	/* ubyte ubyte */

	ParameterStructure_SSIS,	/* ushort */
	ParameterStructure_SSIS_Valence,/* ushort ubyte */
	ParameterStructure_SSIS_MWI,	/* ushort ubyte */

	ParameterStructure_SLI,		/* ubyte */
	ParameterStructure_SLIS,	/* ushort */

	ParameterStructure_PLI,		/* ubyte */
	ParameterStructure_PLIS,	/* ushort */
	ParameterStructure_PPIS,	/* ushort */

	ParameterStructure_Int,		/* int */
	ParameterStructure_Double	/* double */
    };


//  Construction
public:
    VByteCodeDescriptor (
	ByteCode		xCode,
	ParameterStructure	xParameterStructure,
	DispatchCase		xDispatchCase,
	unsigned int		xAssociatedKSI,
	char const*		pName,
	bool			fSuspendSafe = false
    );

//  Access
public:
    unsigned int associatedKSI () const {
	return m_xAssociatedKSI;
    }
    ByteCode byteCode () const {
	return (ByteCode)m_xCode;
    }
    DispatchCase dispatchCase () const {
	return (DispatchCase)m_xDispatchCase;
    }
    ParameterStructure parameterStructure () const {
	return (ParameterStructure)m_xParameterStructure;
    }
    bool suspendSafe () const {
	return m_fSuspendSafe;
    }

//  State
protected:
    DispatchCase const		m_xDispatchCase		: 8;
    ParameterStructure const	m_xParameterStructure	: 8;
    unsigned int const		m_xAssociatedKSI	: 8;
    unsigned int const		m_fSuspendSafe		: 1;

//  Globals
public:
    static inline VByteCodeDescriptor const *Description (
	ByteCode xCode, VByteCodeDescriptor const *pNotFoundResult
    );
    static inline VByteCodeDescriptor const *Description (
	char const* pName, VByteCodeDescriptor const *pNotFoundResult
    );
    static inline VByteCodeDescriptor const *Description (ByteCode xCode);
    static inline VByteCodeDescriptor const *Description (char const* pName);

    static VByteCodeDescriptor const DefaultDescription;
};


/****************************
 ****************************
 *****  The Code Table  *****
 ****************************
 ****************************/

typedef VCodeTable<
    VByteCodeDescriptor::ByteCode, VByteCodeDescriptor, (size_t)256
> VByteCodeDescriptorCodeTable;

extern VByteCodeDescriptorCodeTable const VByteCodeDescriptor__CodeTable;


/******************************
 ******************************
 *****  InLine Functions  *****
 ******************************
 ******************************/

VByteCodeDescriptor const *VByteCodeDescriptor::Description (
    ByteCode xCode, VByteCodeDescriptor const *pNotFoundResult
) {
    return VByteCodeDescriptor__CodeTable.entry (xCode, pNotFoundResult);
}

VByteCodeDescriptor const *VByteCodeDescriptor::Description (
    char const* pName, VByteCodeDescriptor const *pNotFoundResult
) {
    return VByteCodeDescriptor__CodeTable.entry (pName, pNotFoundResult);
}

VByteCodeDescriptor const *VByteCodeDescriptor::Description (ByteCode xCode) {
    return Description (xCode, &DefaultDescription);
}

VByteCodeDescriptor const *VByteCodeDescriptor::Description (char const* pName) {
    return Description (pName, &DefaultDescription);
}

#endif

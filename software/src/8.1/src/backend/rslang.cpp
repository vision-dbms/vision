/*****  Research System Language Compiler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName RSLANG

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vmagic.h"
#include "vstdio.h"
#include "viobj.h"
#include "vutil.h"

#include "VByteCodeScanner.h"
#include "VSelector.h"

#include "RTblock.h"
#include "RTdictionary.h"
#include "RTdsc.h"
#include "RTstring.h"

/*****	Self	*****/
#include "rslang.h"


/****************************
 *****  Common Strings  *****
 ****************************/

PrivateVarDef char const
    *StringSpaceAvailabilityMessage = "Checking String Storage availability\n",
    *StringSpaceParametersMessage   = "\t\tSpace = %08X, Ptr = %08X, Size = %u",
    *StringSpaceUsedMessage	    = " Used = %u\n",
    *StringSpaceAdjustmentMessage   = " Adjustment = %d\n",
    *StringSpaceAddMessage	    = "... Adding Storage\n",
    *StringSpaceReallocateMessage   = "Reallocated Space\n",
    *BufferAllocationMessage	    = "Buffer alloc.  Buffer = %08X, Size = %u, used = %u\n",
    *OneParameterBCTraceMessage	    = "\t%u ",
    *TagBCTraceMessage		    = "\tVMACHINE_BC_Tag %c\n",
    *StringSpaceTraceMessage	    = "%u(%s)\n",
    *StringSpaceTraceMessage2	    = "%u(%s) %u\n",
    *StackAllocationMessage	    = "Stack alloc.  Stack = %08X, Size = %d\n",
    *StackPushMessage		    = "RSLANG[DCStack_Push]: n:%d ByteCode:%u String:%.*s SP:%d\n",
    *StackPopMessage		    = "RSLANG[DCStack_Pop]: SP:%d\n";


/***********************************
 *****  Parse Tree Node Types  *****
 ***********************************/

enum TokenTypes {
    _Error,
    _Identifier,
    _IntensionalIdentifier,
    _ConcatOp,
    _IntensionalConcatOp,
    _Keyword,
    _IntensionalKeyword,
    _LeftAssignment,
    _IntensionalLeftAssignment,
    _RightAssignment,
    _IntensionalRightAssignment,
    _LessThanP,
    _IntensionalLessThanP,
    _LessThanOrEqualP,
    _IntensionalLessThanOrEqualP,
    _GreaterThanOrEqualP,
    _IntensionalGreaterThanOrEqualP,
    _GreaterThanP,
    _IntensionalGreaterThanP,
    _EqualP,
    _IntensionalEqualP,
    _IdenticalP,
    _IntensionalIdenticalP,
    _NotEqualP,
    _IntensionalNotEqualP,
    _NotIdenticalP,
    _IntensionalNotIdenticalP,
    _Or,
    _IntensionalOr,
    _And,
    _IntensionalAnd,
    _Plus,
    _IntensionalPlus,
    _Minus,
    _IntensionalMinus,
    _Times,
    _IntensionalTimes,
    _Divide,
    _IntensionalDivide,
    _Integer,
    _NegInteger,
    _HexInteger,
    _NegHexInteger,
    _Real,
    _NegReal,
    _String,
    _OpenKeywordExpression,
    _ClosedKeywordExpression,
    _UnaryExpression,
    _BinaryExpression,
    _Root,
    _EmptyExpression,
    _SimpleBlock,
    _HeadedBlock,
    _Magic,
    _LocalId,
    _UnarySelector,
    _BinarySelector,
    _KeywordSelector,
    _UnaryMessageTemplate,
    _BinaryMessageTemplate,
    _KeywordMessageTemplate,
    _ParametricMessageTemplate
};


/******************************
 *****  Parser Utilities  *****
 ******************************/

#define DoNothing

/*****  Parse Tree Node List Terminator  *****/
/*---------------------------------------------------------------------------
 * The parse tree node list terminator is passed as the last argument of
 * routines that expect a variable list of parse tree nodes.
 *---------------------------------------------------------------------------
 */
#define pTreeNLT   -1

/****************************************
 *****  Parse Tree Node Structures  *****
 ****************************************/

struct ParseTreeNode {

    unsigned is_a_terminal_node	    :    1;
    unsigned parenthesized	    :	 1;
    unsigned precedingSemi_colon    :    1;
    unsigned trailingPeriod	    :    1;
    unsigned node_type		    :   28;

    int threading_cell;

    union node_info_t {
	struct terminal_info_t {
	    int line_number,
		origin,
	        length;
	} terminal_info;

    	struct non_terminal_info_t {
	    int subtree_head,
		subtree_tail;
	} non_terminal_info;
    } node_info;

};

#define InitialTreeSize 256
#define TreeSizeIncrement 256

#define NULL_NODE -1

struct TreeTemplate {
    unsigned int	NodeCount;
    ParseTreeNode	NodeArray[InitialTreeSize];
};

PrivateVarDef TreeTemplate *ParseTree;
PrivateVarDef int FreeList = NULL_NODE, rsROOT;
PrivateVarDef char const* rsSOURCE;
PrivateVarDef char const* sourceBASE;


/*---------------------------------------------------------------------------
 * Debugger Trace Switches
 *---------------------------------------------------------------------------
 */
PrivateVarDef bool
    retainingBlockForBrowsing	= false,
    tracingCompile		= false,
    tracingCodeGeneration	= false,
    tracingDecompile		= false,
    tracingStackOps		= false,
    tracingStringSpaceOps	= false,
    tracingNodeTypeChange	= false,
    tracingNodeAppend		= false,
    tracingNodeAlloc		= false,
    tracingTerminalCreation	= false,
    tracingNonTerminalCreation	= false;

PrivateVarDef IOBJ_IObject blockIObj;


/***************************
 *****  Access Macros  *****
 ***************************/

#define nodeCount ParseTree->NodeCount

#define nodeArray(i) &(ParseTree->NodeArray[i])

#define Is_a_terminal_node(node)\
    ((nodeArray (node))->is_a_terminal_node)

#define Parenthesized(node)\
    ((nodeArray (node))->parenthesized)

#define PrecedingSemi_Colon(node)\
    ((nodeArray (node))->precedingSemi_colon)

#define TrailingPeriod(node)\
    ((nodeArray (node))->trailingPeriod)

#define NodeType(node)\
    ((nodeArray (node))->node_type)

#define NodeThreadingCell(node)\
    ((nodeArray (node))->threading_cell)

#define LineNumber(node)\
    (((nodeArray (node))->node_info).terminal_info.line_number)

#define TerminalNode_Origin(node)\
    (((nodeArray (node))->node_info).terminal_info.origin)

#define TerminalNode_Length(node)\
    (((nodeArray (node))->node_info).terminal_info.length)

#define NonTerminal_SubtreeHead(node)\
    (((nodeArray (node))->node_info).non_terminal_info.subtree_head)

#define NonTerminal_SubtreeTail(node)\
    (((nodeArray (node))->node_info).non_terminal_info.subtree_tail)

/*******************************
 *****  Functional Macros  *****
 *******************************/

#define  Thread_ParseNode(head, tail)\
    NodeThreadingCell (head) = (tail)

#define Free_Node(node)\
    Thread_ParseNode (node,  FreeList);\
    FreeList = (node)


PrivateFnDef void Expand_NodeStorage () {
    int oldFree = FreeList;

    ParseTree = (TreeTemplate *)UTIL_Realloc (
	ParseTree,
	sizeof (int) + (nodeCount += TreeSizeIncrement) * sizeof(ParseTreeNode)
    );

    for (unsigned int i = nodeCount - TreeSizeIncrement; i < nodeCount; i++) {
        Free_Node (i);
    }

    if (tracingNodeAlloc) IO_printf (
	"...RSLANG[Extend_NodeStorage] PT:%0p NC: %d FL:%d OFL:%d\n",
	ParseTree, nodeCount, FreeList, oldFree
    );

    return;
}


PrivateFnDef void Alloc_ParseTree_Node (int* node_ptr) {
    if (FreeList == NULL_NODE)
 	Expand_NodeStorage ();

    *node_ptr = FreeList;
    FreeList =  NodeThreadingCell (FreeList);
    return;
}

PrivateFnDef void Initialize_ParseTree_Nodes () {
    FreeList = NULL_NODE;

    if (IsNil (ParseTree)) {
        ParseTree = (TreeTemplate *)UTIL_Malloc (sizeof (TreeTemplate));
	nodeCount = InitialTreeSize;
    }

    for (unsigned int i = 0; i < nodeCount; i++) {
       	Free_Node (i);
    }

    if (tracingNodeAlloc) IO_printf (
	"...RSLANG[Initialize_ParseTree_Nodes] PT:%0p NC: %d FL:%d\n",
	ParseTree, nodeCount, FreeList
    );
}


/***********************************************
 *****  Parse Tree Node Creation Routines  *****
 ***********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create and return a new terminal symbol parse tree node.
 *
 *  Arguments:
 *	nodeType		- the logical type of the node being created.
 *	origin			- the index of the start of the symbol in the
 *				  input stream.
 *	extent			- the length of the input symbol.
 *	lineNumber		- the line number in the source at the end of
 *				  this Terminal Symbol.
 *  Returns:
 *	An integer representing the index of the parse tree node created.
 *
 *****/
PrivateFnDef int pTreeTerminal (
    int				nodeType,
    int				origin,
    int				extent,
    int				lineNumber
)
{
    int node;

    Alloc_ParseTree_Node (&node);
    Is_a_terminal_node (node) = true;
    Parenthesized (node) = false;
    PrecedingSemi_Colon (node) = false;
    TrailingPeriod (node) = false;
    NodeType (node) = nodeType;
    NodeThreadingCell (node) = NULL_NODE;
    LineNumber (node) = lineNumber;

    TerminalNode_Origin (node) = origin;
    TerminalNode_Length (node) = extent;

    if (tracingTerminalCreation) IO_printf (
	"T:%d, o:%d, e:%d, node:%d, ln:%d\n",
	(int)NodeType (node), origin, extent, node, lineNumber
    );

    return node;

}


/*---------------------------------------------------------------------------
 *****  Routine to create and return a new non-terminal symbol parse tree node.
 *
 *  Arguments:
 *	nodeType		- the logical type of the node to be created.
 *	subTree1, ...		- the indices of the parse tree nodes which are
 *				  to become the initial subtrees of this node.
 *	pTreeNLT		- the constant 'pTreeNLT'.  This constant is
 *				  guaranteed to be an invalid parse tree node
 *				  index; its presence terminates the list.
 *
 *  Returns:
 *	The index of the parse tree node created.
 *
 *****/
PrivateFnDef int __cdecl pTreeNonTerminal (int nodeType, ...) {
    int		node,
		subnode,
		first_node,
		previous_node,
		node_num;

    Alloc_ParseTree_Node (&node);
    Is_a_terminal_node (node) = false;
    Parenthesized (node) = false;
    PrecedingSemi_Colon (node) = false;
    TrailingPeriod (node) = false;
    NodeThreadingCell (node)  = NULL_NODE;

    NodeType (node) = nodeType;

    V_VARGLIST (iArgList, nodeType);
    previous_node = pTreeNLT;
    for (node_num = 0; (subnode = (iArgList.arg<int>())) != pTreeNLT; node_num++) {
	if (node_num == 0)
    	    first_node = subnode;
	else
	    Thread_ParseNode (previous_node, subnode);

	previous_node = subnode;
    }
    NonTerminal_SubtreeHead (node) = first_node;
    NonTerminal_SubtreeTail (node) = previous_node;

    if (tracingNonTerminalCreation) IO_printf (
	"NT:%d, H:%d, T:%d, node:%d\n",
	(int)NodeType (node), first_node, previous_node, node
    );

    return node;
}

/***************************************************
 *****  Parse Tree Node Modification Routines  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to append a collection of subtrees to a non-terminal node.
 *
 *  Arguments:
 *	node			- the index of a Non Terminal parse tree node
 *				  to which the subtrees are to be appended.
 *	subTree1, ...		- the indices of the parse tree nodes of the
 *				  roots of the subtrees to be appended.
 *	pTreeNLT		- the constant 'pTreeNLT'.  This constant is
 *				  guaranteed to be an invalid parse tree node
 *				  index; its presence terminates the list.
 *
 *  Returns:
 *	'node'
 *
 *  Errors Signalled:
 *	This routine will signal 'ERR_InvalidParseTreeNodeForOperation' if
 *	'node' is not a terminal node.
 *
 *****/
PrivateFnDef int __cdecl pTreeAppendSubTrees (int node, ...) {
    int previous, subnode;

    if (tracingNodeAppend) IO_printf ("Type %u:Append", (int)NodeType (node));

    if (Is_a_terminal_node (node))
	return node; /* ERR_InvalidParseTreeNodeForOperation; */

    V_VARGLIST (iArgList, node);
    for (previous = NonTerminal_SubtreeTail (node); ((subnode = iArgList.arg<int>())) != pTreeNLT; previous = subnode) {
	Thread_ParseNode (previous, subnode);
	if (tracingNodeAppend) IO_printf ("\t%d to %d\n", subnode, previous);
    }

    NonTerminal_SubtreeTail (node) = previous;

    return node;
}


/*---------------------------------------------------------------------------
 *****  Routine to change the logical type of a parse tree node.
 *
 *  Arguments:
 *	node			- the index of the parse tree node whose
 *				  logical type is to be changed.
 *	newType			- the new logical type of the node.
 *
 *  Returns:
 *	'node'
 *
 *****/
PrivateFnDef int pTreeChangeNodeType (
    int				node,
    int				newType
)
{
    if (tracingNodeTypeChange) IO_printf (
	"Node:%d Type change from %d to %d\n",node,(int)NodeType(node),newType
    );

    NodeType (node) = newType;

    return node;
}


/*******************************************************************
 *****  Lexical and Syntactic Analysis Global State Variables  *****
 *******************************************************************/

extern int rslanglineno;	/***** Must be declared 'extern'	*****
				 ***** to deal with VAX C 'extern'	*****
				 ***** brain damage.			*****/

class ScanPointer {
//  Construction
public:
    ScanPointer () : m_xScan (0), m_xDisplay (0) {
    }

//  Access
public:
    operator int () const {
	return m_xScan;
    }
    int displayOffset () const {
	return m_xDisplay;
    }

//  Update
public:
    void reset () {
	m_xDisplay = m_xScan = 0;
    }

    int operator++ () {		//  Prefix
	int result = ++m_xScan;
	m_xDisplay = m_xScan;
	return result;
    }
    int operator-- () {		//  Prefix
	int result = --m_xScan;
	m_xDisplay = m_xScan;
	return result;
    }

    int operator++ (int) {	//  Postfix
	int result = m_xScan++;
	m_xDisplay = m_xScan;
	return result;
    }
    int operator-- (int) {	//  Postfix
	int result = m_xScan--;
	m_xDisplay = m_xScan;
	return result;
    }

    void setDisplayOffsetTo (int xDisplay) {
	m_xDisplay = xDisplay;
    }

//  State
protected:
    int m_xScan;
    int m_xDisplay;
};

PrivateVarDef ScanPointer CurrentScanPointer;

PrivateVarDef int IntensionalOp;


/*************************
 *****  Yacc Macros  *****
 *************************/

/*****  Parse Tree Node Generating Macros  *****/
#define Percolate(r, ntype, sub)\
    r = pTreeNonTerminal ((int)ntype, sub, pTreeNLT)

#define Extend(r, a, b)\
    r = pTreeAppendSubTrees (a, b, pTreeNLT);

#define Empty(r)\
    r = pTreeTerminal ((int)_EmptyExpression, 0, 0, rslanglineno)

#define Root(r, exp)\
    r = pTreeNonTerminal ((int)_Root, exp, pTreeNLT)

#define HeadedBlock(r, head, body)\
    r = pTreeNonTerminal ((int)_HeadedBlock, head, body, pTreeNLT)

#define UnaryOp(r, a, op)\
    r = pTreeNonTerminal ((int)_UnaryExpression, a, op, pTreeNLT)

#define BinaryOp(r, a, op, b)\
    r = pTreeNonTerminal ((int)_BinaryExpression, a, op, b, pTreeNLT)

#define BinaryMessage(r, a, b)\
    r = pTreeNonTerminal ((int)_BinaryMessageTemplate, a, b, pTreeNLT)

#define KeywordTerm(r, a, keyword, b)\
    if (NodeType (a) == (int)_OpenKeywordExpression)\
	r = pTreeAppendSubTrees (a, keyword, b, pTreeNLT);\
    else\
	r = pTreeNonTerminal ((int)_OpenKeywordExpression,\
				 a, keyword, b, pTreeNLT)

#define ClosingKeywordTerm(r, a, keyword, b)\
{\
    if (NodeType (a) == (int)_OpenKeywordExpression)\
    {\
        pTreeAppendSubTrees (a, keyword, b, pTreeNLT);\
	r = pTreeChangeNodeType (a, (int)_ClosedKeywordExpression);\
    }\
    else\
	r = pTreeNonTerminal\
	        ((int)_ClosedKeywordExpression, a, keyword, b, pTreeNLT);\
    TrailingPeriod (r) = true;\
}

#define CloseParens(r, exp) {\
    if (NodeType (exp) == (int)_OpenKeywordExpression)\
	r = pTreeChangeNodeType ((exp), (int)_ClosedKeywordExpression);\
    else\
	r = exp;\
    Parenthesized (r) = true;\
}

#define SetPrecedingSemi_Colon(exp) {\
    PrecedingSemi_Colon (exp) = true;\
}

/*****  Error Handler  *****/
PrivateVarDef char *MessageBuffer;
PrivateVarDef int
    MessageBufferMax,
    *ErrorLine,
    *ErrorCharacter;

#if defined(_AIX) && defined(__cplusplus) && !defined(_CPP_IOSTREAMS)
extern "C"
#endif

void rslangerror (char const *s) {
/*****  Just fill the error message buffer if one was supplied, ...  *****/
    if (IsntNil (MessageBuffer)) {
	sprintf (MessageBuffer, "%.*s", MessageBufferMax - 1, s);
	*ErrorLine	= rslanglineno + 1;
	*ErrorCharacter	= CurrentScanPointer.displayOffset ();
	return;
    }

/*****  Otherwise, create a pointer to the current scan position in the input, ...  *****/
    char const* scanPointer = rsSOURCE + CurrentScanPointer.displayOffset () - 1;
    if (scanPointer < rsSOURCE)
	scanPointer = rsSOURCE;

/*****  ... locate the beginning of the area to be displayed, ...  *****/
    char const* displayBase = rsSOURCE;
    char const* newDisplayBase = strchr (displayBase, '\n');
    while (newDisplayBase && newDisplayBase < scanPointer - 200) {
	displayBase = newDisplayBase + 1;
	newDisplayBase = strchr (displayBase, '\n');
    }

/*****  ... and print a description of the error, ...  *****/
    IO_printf (
	">>> '%s' near source line %d, character %d <<<\n",
	s, rslanglineno + 1, CurrentScanPointer.displayOffset ()
    );

/*****  ... along with some context:  *****/
    char const* displayLimit = displayBase + strlen (displayBase);
    if (scanPointer + 100 < displayLimit)
        displayLimit = scanPointer + 100;

    do {
	newDisplayBase = strchr (displayBase, '\n');
	if (IsNil (newDisplayBase))
	    newDisplayBase = displayBase + strlen (displayBase);

	IO_printf (" %.*s\n", newDisplayBase - displayBase, displayBase);

	if (scanPointer >= displayBase && scanPointer <= newDisplayBase
	) IO_printf (
	    "*%-*.*s^\n",
	    scanPointer - displayBase,
	    scanPointer - displayBase,
	    "*"
	);

	displayBase = newDisplayBase + 1;
    } while (displayBase < displayLimit);

    if (scanPointer >= displayBase) IO_printf (
	"\n*%-*.*s^\n",
	scanPointer - displayBase,
	scanPointer - displayBase,
	"*"
    );
}


/**********************************
 *****  The Lexical Analyzer  *****
 **********************************/

/*****  Token Generating Macros  *****/
#define ReturnNode(nodeType, tokenType)\
    rslanglval = pTreeTerminal (nodeType,\
			        CurrentScanPointer - (rslangleng),\
				rslangleng, rslanglineno);\
    return tokenType

#define ReturnSelector(nType, intensionalNType, tType, intensionalTType)\
    if (IntensionalOp) {\
        IntensionalOp = false;\
	ReturnNode (intensionalNType, intensionalTType);\
    }\
    ReturnNode (nType, tType)

/*****  I/O and Wrap-up Routines  *****/
#ifdef __cplusplus
extern "C" {
#endif

extern int rslangwrap () {
    return 1;
}

extern int rslanglex ();

#ifdef __cplusplus
}
#endif


/*****  'yacc' Output  *****/
#include "rslang.yo"

/*****  'lex' Output  *****/
#include "rslang.lo"


/**************************************************************
 **************************************************************
 *****		Code Generator Routines			  *****
 **************************************************************
 **************************************************************/

/**************************************
 *****  Internal Data Structures  *****
 **************************************/

typedef struct bufferType {
    int		size;
    pointer_t	allocated_area,
		current_ptr;
} stringSpaceType;

struct PLVectorType {
    int			size;
    M_CPD**		allocated_area;
    unsigned short	index;
} ;

struct ELEVectorType {
    int			size;
    int *		allocated_area;
    unsigned short	index;
} ;

/***************************
 *****  Access Macros  *****
 ***************************/

#define Storage(x)\
    ((x)->allocated_area)

#define Size(x)\
    ((x)->size)

#define Ptr(x)\
    ((x)->current_ptr)

#define Index(x)\
    ((x)->index)

#define stringSpaceIndex(x)\
    (Ptr (x) - Storage (x))

#define bufferIndex(x) (unsigned int)stringSpaceIndex(x)


/*******************************
 *****  Functional Macros  *****
 *******************************/

#define AlignShort(bufferPtr)\
    (bufferPtr) += *(bufferPtr) = (unsigned char)(1 - ((pointer_size_t)(bufferPtr) & 1))

#define AlignInt(bufferPtr)\
    (bufferPtr) += *(bufferPtr) = (unsigned char)(3 - ((pointer_size_t)(bufferPtr) & 3))

#define bCodePtr(cpd)\
    rtBLOCK_CPD_PC(cpd)

/***********************
 *****  Constants  *****
 ***********************/

#define MAX_BYTE_SIZE 255
#define INITIAL_BUFFER_SIZE 128
#define INITIAL_STRING_SPACE 1024
#define INITIAL_PLVECTOR_SIZE 16
#define INITIAL_ELEVECTOR_SIZE 16
#define bufferINCREMENT 128
#define stringSpaceINCREMENT 512
#define BCVectorINCREMENT 512
#define PLVectorINCREMENT 8
#define ELEVectorINCREMENT 8
#define MAX_BYTES_TO_ADD 32
#define PUSHNewLine 1


PrivateFnDef VByteCodeDescriptor::ByteCode getBinaryByteCode (
    TokenTypes nodetype, int *intensional
) {
    *intensional = false;

    switch (nodetype) {
    case _IntensionalLeftAssignment:
	*intensional = true;
    case _LeftAssignment:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_LAssign\n");
	return (VByteCodeDescriptor::ByteCode_LAssign);

    case _IntensionalRightAssignment:
	*intensional = true;
    case _RightAssignment:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_RAssign\n");
	return (VByteCodeDescriptor::ByteCode_RAssign);

    case _IntensionalConcatOp:
	*intensional = true;
    case _ConcatOp:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Concat\n");
	return (VByteCodeDescriptor::ByteCode_Concat);

    case _IntensionalLessThanP:
	*intensional = true;
    case _LessThanP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_LT\n");
	return (VByteCodeDescriptor::ByteCode_LT);

    case _IntensionalLessThanOrEqualP:
	*intensional = true;
    case _LessThanOrEqualP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_LE\n");
	return (VByteCodeDescriptor::ByteCode_LE);

    case _IntensionalGreaterThanOrEqualP:
	*intensional = true;
    case _GreaterThanOrEqualP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_GE\n");
	return (VByteCodeDescriptor::ByteCode_GE);

    case _IntensionalGreaterThanP:
	*intensional = true;
    case _GreaterThanP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_GT\n");
	return (VByteCodeDescriptor::ByteCode_GT);

    case _IntensionalEqualP:
	*intensional = true;
    case _EqualP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Equal\n");
	return (VByteCodeDescriptor::ByteCode_Equal);

    case _IntensionalIdenticalP:
	*intensional = true;
    case _IdenticalP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Equivalent\n");
	return (VByteCodeDescriptor::ByteCode_Equivalent);

    case _IntensionalNotEqualP:
	*intensional = true;
    case _NotEqualP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_NEqual\n");
	return (VByteCodeDescriptor::ByteCode_NEqual);

    case _IntensionalNotIdenticalP:
	*intensional = true;
    case _NotIdenticalP:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_NEquivalent\n");
	return (VByteCodeDescriptor::ByteCode_NEquivalent);

    case _IntensionalOr:
	*intensional = true;
    case _Or:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Or\n");
	return (VByteCodeDescriptor::ByteCode_Or);

    case _IntensionalAnd:
	*intensional = true;
    case _And:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_And\n");
	return (VByteCodeDescriptor::ByteCode_And);

    case _IntensionalPlus:
	*intensional = true;
    case _Plus:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Add\n");
	return (VByteCodeDescriptor::ByteCode_Add);

    case _IntensionalMinus:
	*intensional = true;
    case _Minus:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Subtract\n");
	return (VByteCodeDescriptor::ByteCode_Subtract);

    case _IntensionalTimes:
	*intensional = true;
    case _Times:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Multiply\n");
	return (VByteCodeDescriptor::ByteCode_Multiply);

    case _IntensionalDivide:
	*intensional = true;
    case _Divide:
	if (tracingCodeGeneration) IO_printf ("\tVMACHINE_BC_Divide\n");
	return (VByteCodeDescriptor::ByteCode_Divide);

    default:
        break;
    }

    return VByteCodeDescriptor::ByteCode_Pad0;
}


/********************************************
 *****	Symbol Table State and Utility	*****
 ********************************************/

PrivateVarDef unsigned short stringIndex;
PrivateVarDef pointer_t stringSpaceBase;

/*---------------------------------------------------------------------------
 *****  Function passed to the HP_Unix provided twalk routine.
 *****  See documentation for tsearch(3C) in HP_Unix manual.
 *****/
PrivateFnDef void findIndex (treenode *node, STD_VISIT order, int level) {
    if (level == 0 && (order == STD_preorder || order == STD_leaf))

        stringIndex = (unsigned short)((char *)STD_tdata (node) - stringSpaceBase);
}

PrivateVarDef pointer_diff_t ptrAdjustment;

/*---------------------------------------------------------------------------
 *****  Function passed to the HP_Unix provided twalk routine.
 *****  See documentation for tsearch(3C) in HP_Unix manual.
 *****/
PrivateFnDef void adjustTableValues (treenode *node, STD_VISIT order, int Unused(level)) {
    if (order == STD_preorder || order == STD_leaf)
        *((char **)&STD_tdata (node)) += ptrAdjustment;
}

PrivateVarDef rtDICTIONARY_Handle::Reference LocalMDCPD;

/*---------------------------------------------------------------------------
 *****  Function passed to the HP_Unix provided twalk routine.
 *****  See documentation for tsearch(3C) in HP_Unix manual.
 *****/
PrivateFnDef void InsertMethodDEntries (treenode *node, STD_VISIT order, int Unused(level)) {
    switch (order) {
    case STD_preorder:
    case STD_leaf: {
	    static M_KOT *g_pPropertySpecificationKOT = NilOf (M_KOT*);
	    static DSC_Descriptor *g_pPropertySpecification = NilOf (DSC_Descriptor*);

	    M_KOT *pMDKOT = LocalMDCPD->KOT ();
	    if (g_pPropertySpecificationKOT != pMDKOT) {
		g_pPropertySpecificationKOT = NilOf (M_KOT*);

		if (g_pPropertySpecification)
		    g_pPropertySpecification->clear ();
		else {
		    static DSC_Descriptor g_iPropertySpecification;
		    g_pPropertySpecification = &g_iPropertySpecification;
		}

		static_cast<rtDSC_Handle*>(pMDKOT->TheDefaultProperty.ObjectHandle ())->getValue (
		    *g_pPropertySpecification
		);

		g_pPropertySpecificationKOT = pMDKOT;
	    }

	    VSelectorGenerale selector((char const*)STD_tdata (node));
	    LocalMDCPD->define (selector, *g_pPropertySpecification);
	}
	break;
    default:
	break;
    }
}


/*******************************************
 ****	String Space Management Macros	****
 *******************************************/

PrivateFnDef void InitializeStringSpace (
    stringSpaceType		*stringSpace
)
{
    Ptr	    (stringSpace) =
    Storage (stringSpace) = (char*)UTIL_Malloc (INITIAL_STRING_SPACE);
    Size (stringSpace) = INITIAL_STRING_SPACE;
    if (tracingStringSpaceOps) IO_printf (
	"StringSpace alloc.  StringSpace = %08X, Size = %u, used = %u\n",
	Ptr (stringSpace), Size (stringSpace),
	stringSpaceIndex (stringSpace)
    );
}

#define WalkTable(table)\
    if (IsntNil (table)) STD_twalk (*(table), adjustTableValues)

#define ConditionalGrow(stringSpace,stringLength,walkIdTable,walkEnvTable)\
{\
    pointer_t oldSpace;\
\
    if (tracingStringSpaceOps)\
    {\
	IO_printf (StringSpaceAvailabilityMessage);\
	IO_printf (\
	    StringSpaceParametersMessage,\
	    Storage (stringSpace), Ptr (stringSpace), Size (stringSpace)\
	);\
	IO_printf (StringSpaceUsedMessage, stringSpaceIndex (stringSpace));\
    }\
    int spaceNeeded = stringSpaceIndex (stringSpace) + (stringLength) + 1;\
    if (spaceNeeded > Size (stringSpace)) {\
	if (tracingStringSpaceOps)\
	    IO_printf (StringSpaceAddMessage);\
	oldSpace = Storage (stringSpace);\
	Storage (stringSpace) = (char*)UTIL_Realloc (\
	    Storage (stringSpace), Size (stringSpace) += V_Max (spaceNeeded, stringSpaceINCREMENT)\
	);\
	ptrAdjustment = (pointer_size_t)Storage (stringSpace) - (pointer_size_t)oldSpace;\
	Ptr (stringSpace) += ptrAdjustment;\
	walkIdTable;\
	walkEnvTable;\
	if (tracingStringSpaceOps) {\
	    IO_printf (StringSpaceReallocateMessage);\
	    IO_printf (\
		StringSpaceParametersMessage,\
		Storage (stringSpace), Ptr (stringSpace), Size (stringSpace)\
	    );\
	    IO_printf (StringSpaceAdjustmentMessage, ptrAdjustment);\
	}\
    }\
}


/****************************************************
 *****	String Space Management Routines	*****
 ****************************************************/

/*---------------------------------------------------------------------------
 ****  Internal Routine to check whether a string is currently in stringSpace
 ****  and add it if it isnt there.  In either case, set the variable
 ****  stringIndex to the string's offset in stringSpace.
 *
 *  Arguments:
 *	stringSpace	- the string space being checked (and added to?).
 *	idTable		- the root of the current symbol table.
 *	envTable	- the root of the current environment table.
 *	stringOrigin	- the address of the string to be found or added.
 *	stringLength	- the length of the string to be found or added.
 *	removeBackslash - Boolean which when true indicates that backslash
 *			  characters should be removed before adding to string
 *			  space.
 *
 *  Returns:
 *	0 if successful; -1 if failed (stringSpace too big)
 *
 ****/
PrivateFnDef int addToStringSpace (
    stringSpaceType		*stringSpace,
    void			**idTable,
    void			**envTable,
    char const*			stringOrigin,
    int				stringLength,
    int				removeBackslash
)
{
    char
	*SSptr,
	*tNodeLocation;
    int
	x;

    ConditionalGrow (
	stringSpace, stringLength, WalkTable (idTable), WalkTable (envTable)
    );

    if (tracingStringSpaceOps)
	IO_printf ("Inserting String in String Space\n");

    if (removeBackslash)
    {
	SSptr = Ptr (stringSpace);
	for (x = 0; x < stringLength; x++)
	{
	    if (*stringOrigin == '\\' && x != stringLength - 1)
	    {
		stringOrigin++;
		stringLength--;
	    }
	    *SSptr++ = *stringOrigin++;
	}
	*SSptr = '\0';
    }
    else
    {
	memcpy (Ptr (stringSpace), stringOrigin, stringLength);
	Ptr (stringSpace) [stringLength] = '\0';
    }

    if (tracingStringSpaceOps)
    {
	IO_printf ("Inserting String '%s' in Idtable\n", Ptr (stringSpace));
    }

    if (IsNil (tNodeLocation = (char *)STD_tfind (Ptr (stringSpace), idTable, (VkComparator)strcmp))) {
	if (tracingStringSpaceOps)
	    IO_printf ("Not Present, so Insert ...");

	(void)STD_tsearch (Ptr (stringSpace), idTable, (VkComparator)strcmp);
	if (tracingStringSpaceOps)
	    IO_printf ("Adjust StringSpace pointer\n");
	if (stringSpaceIndex (stringSpace) > USHRT_MAX) {
	    rslangerror (
		UTIL_FormatMessage (
		    "String space index of %u for string '%.*s%s' exceeds %u maximum",
		    stringSpaceIndex (stringSpace),
		    V_Min (128, stringLength),
		    Ptr (stringSpace),
		    stringLength > 128 ? "..." : "",
		    USHRT_MAX
		)
	    );
	    return -1;
	}

	stringIndex = (unsigned short)(stringSpaceIndex (stringSpace));
	Ptr (stringSpace) += (stringLength + 1);
    }
    else
    {
	stringSpaceBase = Storage (stringSpace);
	STD_twalk (tNodeLocation, findIndex);
    }
    return 0;
}


/***************************************************
 ****	Temporary Buffer Management Macros	****
 ***************************************************/

PrivateFnDef void InitializeBuffer (
    bufferType			*buffer
)
{
    if (IsNil (Storage (buffer)))
    {
	Storage (buffer) = (char*)UTIL_Malloc (INITIAL_BUFFER_SIZE);
	Size (buffer) = INITIAL_BUFFER_SIZE;
	if (tracingStringSpaceOps)
	    IO_printf (
		BufferAllocationMessage, Ptr (buffer), Size (buffer), bufferIndex (buffer)
	    );
    }
    Ptr (buffer) = Storage (buffer);
}

PrivateFnDef void ConditionalGrowBuffer (
    bufferType			*buffer,
    int				stringLength
)
{
    char *oldStorage;

    if (tracingStringSpaceOps)
    {
	IO_printf (StringSpaceAvailabilityMessage);
	IO_printf (
	    StringSpaceParametersMessage,
	    Storage (buffer), Ptr (buffer), Size (buffer)
	);
	IO_printf (StringSpaceUsedMessage, bufferIndex (buffer));
    }
    while ((int)bufferIndex (buffer) + stringLength + 1 > Size (buffer))
    {
	if (tracingStringSpaceOps) IO_printf (StringSpaceAddMessage);
	oldStorage = Storage (buffer);
	Storage (buffer) = (char*)UTIL_Realloc (
	    Storage (buffer), Size (buffer) += bufferINCREMENT
	);
	ptrAdjustment = (pointer_size_t)Storage (buffer) - (pointer_size_t)oldStorage;
	Ptr (buffer) += ptrAdjustment;
	if (tracingStringSpaceOps)
	{
	    IO_printf (StringSpaceReallocateMessage);
	    IO_printf (
		StringSpaceParametersMessage,
		Storage (buffer), Ptr (buffer), Size (buffer)
	    );
	    IO_printf (StringSpaceAdjustmentMessage, ptrAdjustment);
	}
    }
}

PrivateFnDef void WriteToBuffer (
    bufferType* buffer, char const*source, int sourceLength
)
{
    ConditionalGrowBuffer (buffer, sourceLength);
    memcpy (Ptr (buffer), source, sourceLength);
    Ptr (buffer) += sourceLength;
    *(Ptr (buffer)) = '\0';
}

/*****  Define Dynamic Holding Buffers  *****/
PrivateVarDef bufferType
	selector,
	parameter,
	tagbuffer,
	asciiNumber;


/*****	Byte Code Output Macros	*****/
PrivateFnDef void output1ParamByteCode (
    unsigned char		**PC,
    unsigned char		shortCode,
    unsigned char		byteCode,
    stringSpaceType		*stringSpace
)
{
    if (stringIndex > MAX_BYTE_SIZE)
    {
	AlignShort (*PC);
	*(*PC)++ = shortCode;
	if (tracingCodeGeneration) IO_printf (
	    OneParameterBCTraceMessage, shortCode
	);
	*(unsigned short *)(*PC) = stringIndex;
	(*PC) += sizeof (short);
    }
    else
    {
	*(*PC)++ = byteCode;
	if (tracingCodeGeneration) IO_printf (
	    OneParameterBCTraceMessage, byteCode
	);
	*(*PC)++ = (unsigned char)stringIndex;
    }
    if (tracingCodeGeneration) IO_printf (
	StringSpaceTraceMessage,
	stringIndex,
	&Storage (stringSpace)[stringIndex]
    );
}

PrivateFnDef void output2ParamByteCode (
    unsigned char		**PC,
    unsigned char		shortCode,
    unsigned char		byteCode,
    unsigned char		param2,
    stringSpaceType		*stringSpace
)
{
    if (stringIndex > MAX_BYTE_SIZE)
    {
	AlignShort (*PC);
	*(*PC)++ = shortCode;
	if (tracingCodeGeneration) IO_printf (
	    OneParameterBCTraceMessage, shortCode
	);
	*(unsigned short *)(*PC) = stringIndex;
	(*PC) += sizeof (short);
    }
    else
    {
	*(*PC)++ = byteCode;
	if (tracingCodeGeneration) IO_printf (
	    OneParameterBCTraceMessage, byteCode
	);
	*(*PC)++ = (unsigned char)stringIndex;
    }
    *(*PC)++ = param2;
    if (tracingCodeGeneration) IO_printf (
	StringSpaceTraceMessage2,
	stringIndex, &Storage (stringSpace)[stringIndex], param2
    );
}

PrivateFnDef void outputTag (
    unsigned char		**PC,
    unsigned char		val
)
{
    *(*PC)++ = VByteCodeDescriptor::ByteCode_Tag;
    *(*PC)++ = val;
    if (tracingCodeGeneration) IO_printf (TagBCTraceMessage, val);
}

PrivateFnDef void conditionallyTagNewline (
    unsigned char		**PC,
    int				subTree
)
{
    if (Is_a_terminal_node (subTree))
    {
        if (LineNumber (subTree) != rslanglineno)
	{
	    outputTag (PC, '\n');
	    rslanglineno = LineNumber (subTree);
	}
	CurrentScanPointer.setDisplayOffsetTo (
	    TerminalNode_Origin (subTree) + TerminalNode_Length(subTree)
	);
    }
}


/***************************************
 *****	Selector Binding Routine   *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****   Inserts selector binding into block
 *
 *   Arguments:
 *	cpd		- the address of the CPD for the block into which
 *			  the selector is being inserted.
 *	stringSpace	- the string space being added to.
 *	idTable		- the root of the current symbol table.
 *	envTable	- the root of the current environment table.
 *	selectorPtr	- pointer to the location of the selector name.
 *
 *   Returns:
 *	0 if successful; -1 if failed;
 *
 *****/
PrivateFnDef int insertSelectorBinding (
    M_CPD			*cpd,
    stringSpaceType		*stringSpace,
    void			**idTable,
    void			**envTable,
    pointer_t			selectorPtr
)
{
    int index = SELECTOR_StringToKSI (selectorPtr);

    rtBLOCK_BlockType* blockPtr = rtBLOCK_CPDBase (cpd);

    if (index < 0) {	/**  is not known  **/
    /** put selector in string space and initialize pointer to it **
     ** addToStringSpace puts pointer in static variable 'stringIndex' **/

	if (-1 == addToStringSpace (
	    stringSpace, idTable, envTable,
	    selectorPtr, strlen (selectorPtr), true
	)) return -1;

	rtBLOCK_SelectorType (blockPtr) = rtBLOCK_C_DefinedSelector;
	rtBLOCK_SelectorIndex (blockPtr) = stringIndex;

    }
    else {	/**  is a known selector  **/
        rtBLOCK_SelectorType (blockPtr) = rtBLOCK_C_KnownSelector;
	rtBLOCK_SelectorIndex (blockPtr) = (short)index;

    }
    return 0;
}


PrivateFnDef void extractSelector (
    int				node,
    bufferType			*selector,
    int				*intension,
    unsigned char		*valence
)
{
    int subnode;

    for (subnode = NonTerminal_SubtreeHead (node);;
	 subnode = NodeThreadingCell (subnode))
    {

        switch (NodeType (subnode))
	{
        case _IntensionalKeyword:
	    *intension = true;
        case _Keyword:
	    (*valence)++;
	    WriteToBuffer (
		selector, &sourceBASE [TerminalNode_Origin (subnode)],
		TerminalNode_Length (subnode)
	    );
            break;
	default:
	    break;
        }
	if (subnode == NonTerminal_SubtreeTail (node))
	    break;
    }
}


/*---------------------------------------------------------------------------
 *****  'buildProgram' forward declaration
 *---------------------------------------------------------------------------
 */
PrivateFnDef M_CPD *buildProgram (
    M_ASD *pContainerSpace, int root, rtDICTIONARY_Handle *pDictionary, int Header
);


/*---------------------------------------------------------------------------
 *****	Routine inserts parameters into the block in reverse order  *****
 *
 *  Arguments:
 *	subTree			-  the root node of the subtree to be walked
 *				   for this phase of code generation.
 *	PC			-  position in Byte Code vector.
 *	stringSpace		-  an area, which may be enlarged, containing
 *				   strings, identifiers and selectors. The
 *				   position of identifiers and selectors is
 *				   kept in the idTable.
 *	envTable		-  the table used to temporarily store
 *				   variables until they are inserted into the
 *				   local environment.
 *	idTable			-  the symbol table to be used for lookup and
 *				   to be added to if necessary.
 *	nextSubTree		-  next parse tree node in chain.
 *	programCPD		-  CPD of the block being built.
 *	skipSelector		-  true if no selector, false to skip over
 *				   selectors in parse tree node chain.
 *
 *  Returns:
 *	0 if successful; -1 if failed
 *****/
PrivateFnDef int acquireParameterLoop (
    int				subTree,
    unsigned char		**PC,
    stringSpaceType		*stringSpace,
    void			**envTable,
    void			**idTable,
    int				nextSubTree,
    M_CPD			*programCPD,
    int				skipSelector
) {
    if (skipSelector)
	nextSubTree = NodeThreadingCell (nextSubTree);

    if (nextSubTree != NonTerminal_SubtreeTail (subTree)) {
	if ( -1 == acquireParameterLoop (
	    subTree, PC, stringSpace, envTable, idTable,
	    NodeThreadingCell (nextSubTree), programCPD, skipSelector
	)) return -1;
    }

    conditionallyTagNewline (PC, nextSubTree);
    InitializeBuffer (&parameter);
    WriteToBuffer (&parameter, &sourceBASE [TerminalNode_Origin (nextSubTree)],
			    TerminalNode_Length (nextSubTree));

    /****	If it is a known parameter ...	****/
    int messageIndex = SELECTOR_StringToKSI (Storage (&parameter));
    if (messageIndex >= 0) {
	*(*PC)++ = VByteCodeDescriptor::ByteCode_AcquireKnownParameter;
        *(*PC)++ = (unsigned char)messageIndex;

        if (tracingCodeGeneration) IO_printf (
	    "\tVMACHINE_BC_AcquireKnownParameter %u(%s) 0\n",
	    messageIndex, Storage (&parameter)
	);

    /*****  although it may appear logical to use parameter instead of
	    KS__ToString, the program will bomb if you do
	    because parameter is stored on the stack and tsearch puts
	    that address in the envTable instead of the string.
	    Consequently when tsearch (in buildProgram) tries to access
	    the stack address,  only garbage will be there.  The same
	    is true in the tsearch code below.		           *****/

	(void)STD_tsearch (KS__ToString (messageIndex), envTable, (VkComparator)strcmp);
    }

    /*****	otherwise it must be a user defined parameter	*****/
    else {
	if (-1 == addToStringSpace (
	    stringSpace, idTable, envTable,
	    &sourceBASE [TerminalNode_Origin (nextSubTree)],
	    TerminalNode_Length (nextSubTree), true)
	) return -1;

        (void)STD_tsearch (&Storage (stringSpace)[stringIndex], envTable, (VkComparator)strcmp);

        output1ParamByteCode (PC,
			      VByteCodeDescriptor::ByteCode_AcquireParameterS,
			      VByteCodeDescriptor::ByteCode_AcquireParameter,
			      stringSpace);
    }

/*****  Check to ensure that there is enough space for a set *****
 *****  of byte codes. If not, call GrowContainer    	     *****/

    if ((size_t) (
	    (*PC) - M_CPD_ContainerBaseAsType (programCPD, unsigned char)
	) + MAX_BYTES_TO_ADD > M_CPD_Size (programCPD)
    )
    {
	if (tracingCompile) IO_printf (
	    "Increasing Program Size by %d\n", BCVectorINCREMENT
	);

        programCPD->GrowContainer (BCVectorINCREMENT);
	rtBLOCK_CPDEnd (programCPD) += BCVectorINCREMENT;
    }

    return 0;
}


/*---------------------------------------------------------------------------
 *****	routine takes selector and parameters and inserts them into the  *****
 *****  block.
 *
 *  Arguments:
 *	subTree			-  the root node of the subtree to be walked
 *				   for this phase of code generation.
 *	programCPD		-  the block being built.
 *	idTable			-  the symbol table to be used for lookup and
 *				   to be added to if necessary.
 *	envTable		-  the table used to temporarily store
 *				   variables until they are inserted into the
 *				   local environment.
 *	stringSpace		-  an area, which may be enlarged, containing
 *				   strings, identifiers and selectors. The
 *				   position of identifiers and selectors is
 *				   kept in the idTable.
 *  Returns:
 *	0 if successful, -1 if failed.
 *
 *****/
PrivateFnDef int acquireParameters (
    int				subTree,
    M_CPD			*programCPD,
    void			**idTable,
    void			**envTable,
    stringSpaceType		*stringSpace
)
{
    unsigned char
	**PC = (unsigned char **)(&(M_CPD_Pointers (programCPD) [rtBLOCK_CPx_PC])),
	valence;
    int
	Intensional,
	xOperator,
	nextSubTree;

    if (tracingCompile)
    {
        IO_printf ("Entered acquireParameters.");
	IO_printf ("subTree = %08X, NodeType = %d\n",
				subTree, NodeType (subTree));
    }

    /*****  initialize selector variable.  *****/
    /*****  selector placed here before being inserted into the block ****/
    InitializeBuffer (&selector);

    switch (NodeType (subTree))
    {
    case _UnaryMessageTemplate:
	subTree = NonTerminal_SubtreeHead (subTree);
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));
	break;

    case _BinaryMessageTemplate:
        outputTag (PC, '|');
	xOperator = NonTerminal_SubtreeHead (subTree);
	nextSubTree = NonTerminal_SubtreeTail (subTree);
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (xOperator)],
			    TerminalNode_Length (xOperator));

	if ( -1 == acquireParameterLoop (
	    subTree, PC, stringSpace, envTable,
	    idTable, nextSubTree, programCPD, false)
	) return -1;

        outputTag (PC, '|');
	break;

    case _KeywordMessageTemplate:
        outputTag (PC, '|');
	extractSelector (subTree, &selector, &Intensional, &valence);
	nextSubTree = NonTerminal_SubtreeHead (subTree);

	if ( -1 == acquireParameterLoop (
	    subTree, PC, stringSpace, envTable,
	    idTable, nextSubTree, programCPD, true)
	) return -1;

        outputTag (PC, '|');
	break;

    case _ParametricMessageTemplate:
	outputTag (PC, '|');
	nextSubTree = NonTerminal_SubtreeHead (subTree);

	if ( -1 == acquireParameterLoop (
	    subTree, PC, stringSpace, envTable,
	    idTable, nextSubTree, programCPD, false)
	) return -1;

        outputTag (PC, '|');
        break;
    default:
	/*****	Error or do nothing  *****/
	break;
    }
    /*****	insert selector into the block	*****/
    return Ptr (&selector) != Storage (&selector) && -1 == insertSelectorBinding (
	programCPD, stringSpace, idTable, envTable, Storage (&selector)
    ) ? -1 : 0;
}


/************************************
 ****  Magic Word Helper Macros  ****
 ************************************/

typedef unsigned char PostMagicWord_t;

PrivateVarDef PostMagicWord_t PostMagicWord	= 0200;
PrivateVarDef PostMagicWord_t PostMagicWordMask	= 0177;

#define MarkTagAsPostMagicWord(tag)\
    ((tag) |= PostMagicWord)

#define TagIsPostMagicWord(tag)\
    ((tag) & PostMagicWord)

#define ConvertPostMagicWordTag(target, source)\
    ((target) = (PostMagicWord_t)((source) & PostMagicWordMask))


/*---------------------------------------------------------------------------
 *****  Internal Routine to control the building of the program. It calls
 *****  itself recursively as it 'walks' the parse tree.
 *
 *  Arguments:
 *	subTree			-  the root node of the subtree to be walked
 *				   for this phase of code generation.
 *	programCPD		-  the block being built.
 *	idTable			-  the symbol table to be used for lookup and
 *				   to be added to if necessary.
 *	envTable		-  the table used to temporarily store
 *				   variables until they are inserted into the
 *				   local environment.
 *	stringSpace		-  an area, which may be enlarged, containing
 *				   strings, identifiers and selectors. The
 *				   position of identifiers and selectors is
 *				   kept in the idTable.
 *	physicalLiteralVector	-  an area which may also be enlarged that
 *				   contains POP's for the blocks immediately
 *				   nested in the one passed to this routine.
 *	evaledLiteralVector	-  an area which again may be enlarged that
 *				   contains (will contain) entry points for
 *				   evaled Literals in this block's bytecode
 *				   stream.
 *
 *  Returns:
 *	0 if successful; -1 if failed
 *****/
PrivateFnDef int GenerateCode (
    M_ASD			*pContainerSpace,
    int				 subTree,
    M_CPD			*programCPD,
    void			**idTable,
    void			**envTable,
    stringSpaceType		*stringSpace,
    PLVectorType		*physicalLiteralVector,
    ELEVectorType		*evaledLiteralVector
) {
#define wrapUpCase\
    justStoredMagicWord = false;\
    break

    VByteCodeDescriptor::ByteCode xByteCode;
    int
	messageIndex;

    unsigned char
	**PC = (unsigned char **)(&(M_CPD_Pointers (programCPD)
			    [rtBLOCK_CPx_PC])),
	paren = false,
	period = false,
	valence,
	magicWord;

    PrivateVarDef int
	justStoredMagicWord = false;

    int
	x,
	length = 0,
	Intensional,
	result,
	integer_val,
	header = NULL_NODE,
	xOperator,
	receiver,
	nextSubTree;

    double
	real_val;

    M_CPD
	*newProgram;
    rtDICTIONARY_Handle::Reference pNewDictionary;

    InitializeBuffer (&selector);
    InitializeBuffer (&tagbuffer);
    InitializeBuffer (&asciiNumber);


    if (Parenthesized (subTree))
        paren = true;

    if (PrecedingSemi_Colon (subTree))
	outputTag (PC, ';');

    if (TrailingPeriod (subTree))
        period = true;

    if (tracingCompile) IO_printf (
	"Entered GenerateCode. subTree = %08X, NodeType = %d\n",
	subTree, NodeType (subTree)
    );

/*****  Check to ensure that there is enough space for a set of byte codes.
	if not, call GrowContainer
 *****/
    if ((size_t) (
	    (*PC) - M_CPD_ContainerBaseAsType (programCPD, unsigned char)
	) + MAX_BYTES_TO_ADD > M_CPD_Size (programCPD)
    )
/*
    if ((unsigned int) ((*PC)
	  - (unsigned char *)M_CPD_ContainerBase (programCPD)
	  + MAX_BYTES_TO_ADD)
		> M_CPD_Size (programCPD))
*/
    {
	if (tracingCompile) IO_printf (
	    "Increasing Program Size by %d\n", BCVectorINCREMENT
	);

        programCPD->GrowContainer (BCVectorINCREMENT);
	rtBLOCK_CPDEnd (programCPD) += BCVectorINCREMENT;
    }

    conditionallyTagNewline (PC, subTree);

    if (paren)
        outputTag (PC, '(');

    switch (NodeType (subTree)) {
    case _Identifier:
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));

	if (justStoredMagicWord) {
	    for (x = -2;; x -= 2)
		if ((*PC) [x] != VByteCodeDescriptor::ByteCode_Tag) break;
		else MarkTagAsPostMagicWord ((*PC) [x+1]);

	    (*PC) += x;
	    magicWord = (*PC) [1];
	    length = (-2) - x;
	    WriteToBuffer (&tagbuffer, (char *)(*PC + 2), length);
	}
	else magicWord = (unsigned char)VMagicWord_Current;

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_KnownUnaryValue;
	    *(*PC)++ = (unsigned char)messageIndex;
	    *(*PC)++ = magicWord;
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_KnownUnaryValue %u(%s) 0\n",
		messageIndex, Storage (&selector)
	    );
	}
	else {

        /****	check Identifier Table.  If Not present, append to string
	   	space and add to Id table. Else, get string space index.
	****/
	    if ( -1 == addToStringSpace (
		stringSpace, idTable, envTable,
		&sourceBASE [TerminalNode_Origin (subTree)],
		TerminalNode_Length (subTree), true)
	    ) return -1;

	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_UnaryValueS,
				  VByteCodeDescriptor::ByteCode_UnaryValue,
				  magicWord, 
				  stringSpace);
	}

	if (length != 0)
	{
	    memcpy (*PC, Storage (&tagbuffer), length);
	    *PC += length;
	}

        wrapUpCase;

    case _IntensionalIdentifier:
	WriteToBuffer (
	    &selector,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);

	if (justStoredMagicWord)
	{
	    for (x = -2;; x -= 2)
		if ((*PC) [x] != VByteCodeDescriptor::ByteCode_Tag) break;
		else MarkTagAsPostMagicWord ((*PC) [x+1]);

	    (*PC) += x;
	    magicWord = (*PC) [1];
	    length = (-2) - x;
	    WriteToBuffer (&tagbuffer, (char *)(*PC + 2), length);
	}
	else magicWord = (unsigned char)VMagicWord_Current;

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_KnownUnaryIntension;
	    *(*PC)++ = (unsigned char)messageIndex;
	    *(*PC)++ = magicWord;
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_KnownUnaryIntension %u(%s) 0\n",
		messageIndex,
		Storage (&selector)
	    );
	}
	else {

	    /**** check Identifier Table.  If Not present, append to string
	   	  space and add to Id table. Else, get string space index.
	    ****/
	    if ( -1 == addToStringSpace (
		stringSpace, idTable, envTable,
		&sourceBASE [TerminalNode_Origin (subTree)],
		TerminalNode_Length (subTree), true)
	    ) return -1;

	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_UnaryIntensionS,
				  VByteCodeDescriptor::ByteCode_UnaryIntension,
				  magicWord,
				  stringSpace);
	}

	if (length != 0)
	{
	    memcpy (*PC, Storage (&tagbuffer), length);
	    *PC += length;
	}
        wrapUpCase;

    case _Integer:
	/****	Grab value at SourceString offset and convert it to an
	   	integer.
	****/
	WriteToBuffer (
	    &asciiNumber,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);
	real_val = atof (Storage (&asciiNumber));
	integer_val = (int)real_val;
	AlignInt ((*PC));
	if (real_val != integer_val)
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreDouble;
	    *(double *)(*PC) = real_val;
	    (*PC) += sizeof (double);
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_StoreDouble %g\n", real_val
	    );
	}
	else
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreInteger;
	    *(int *)(*PC) = integer_val;
	    (*PC) += sizeof (int);
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_StoreInteger %d\n", integer_val
	    );
	}

        wrapUpCase;

    case _NegInteger:
	/****	Grab value at SourceString offset and convert it to an
	   	integer.
	****/
	WriteToBuffer (
	    &asciiNumber,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);
	real_val = -atof (Storage (&asciiNumber));
	integer_val = (int)real_val;
	AlignInt ((*PC));
	if (real_val != integer_val)
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreDouble;
	    *(double *)(*PC) = real_val;
	    (*PC) += sizeof (double);
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_StoreDouble %g\n", real_val
	    );
	}
	else
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreInteger;
	    *(int *)(*PC) = integer_val;
	    (*PC) += sizeof (int);
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_StoreInteger %d\n", integer_val
	    );
	}

        wrapUpCase;

    case _HexInteger:
	/****	Grab value at SourceString offset and convert it to an
	   	integer.
	****/
	WriteToBuffer (
	    &asciiNumber,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);
	STD_sscanf (Storage (&asciiNumber), "0x%x", &integer_val);
	AlignInt ((*PC));
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreInteger;
	*(int *)(*PC) = integer_val;
	(*PC) += sizeof (int);
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_StoreInteger %d\n", integer_val);

        wrapUpCase;

    case _NegHexInteger:
	/****	Grab value at SourceString offset and convert it to an
	   	integer.
	****/
	WriteToBuffer (
	    &asciiNumber,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);
	STD_sscanf (Storage (&asciiNumber), "0x%x", &integer_val);
	AlignInt ((*PC));
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreInteger;
	*(int *)(*PC) = -integer_val;
	(*PC) += sizeof (int);
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_StoreInteger %d\n", integer_val);

        wrapUpCase;

    case _Real:
	/****	Grab value at SourceString offset and convert it to a
	   	double.
	****/
	WriteToBuffer (
	    &asciiNumber,
	    &sourceBASE [TerminalNode_Origin (subTree)],
	    TerminalNode_Length (subTree)
	);
	real_val = atof (Storage (&asciiNumber));
	AlignInt ((*PC));
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreDouble;
	*(double *)(*PC) = real_val;
	(*PC) += sizeof (double);
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_StoreDouble %g\n", real_val);

        wrapUpCase;

    case _NegReal:
	/****	Grab value at SourceString offset and convert it to a
	   	double.
	****/
	WriteToBuffer (&asciiNumber, &sourceBASE [TerminalNode_Origin (subTree)],
		    TerminalNode_Length (subTree));
	real_val = -atof (Storage (&asciiNumber));
	AlignInt ((*PC));
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreDouble;
	*(double *)(*PC) = real_val;
	(*PC) += sizeof (double);
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_StoreDouble %g\n", real_val);

        wrapUpCase;

    case _String:
        /****	Copy string from SourceString offset to the end of
	   	StringSpace, first removing any backslashes preceding
		quotes.
	****/
	if ( -1 == addToStringSpace (
	    stringSpace, idTable, envTable,
	    &sourceBASE [TerminalNode_Origin (subTree) + 1],
	    TerminalNode_Length (subTree) - 2, true)
	) return -1;

	output1ParamByteCode (PC,
			      VByteCodeDescriptor::ByteCode_StoreStringS,
			      VByteCodeDescriptor::ByteCode_StoreString,
			      stringSpace);

        wrapUpCase;

    case _BinaryExpression:
	/****	Get BinaryOperator and look up known message index. Determine
		whether this is an Intensional or a value message.
	****/

	nextSubTree = subTree = NonTerminal_SubtreeHead (subTree);

	while (!Is_a_terminal_node (nextSubTree))
	    nextSubTree = NonTerminal_SubtreeTail (nextSubTree);

	if (LineNumber (NodeThreadingCell (subTree))
		!= LineNumber (nextSubTree))
	    outputTag (PC, PUSHNewLine);

	nextSubTree = NodeThreadingCell (subTree);
	xByteCode = getBinaryByteCode ((TokenTypes)NodeType (nextSubTree), &Intensional);
	*(*PC)++ = (unsigned char)(unsigned int)xByteCode;

	/****	Eval recipient	****/
	if (result = GenerateCode (
		pContainerSpace, subTree, programCPD, idTable, envTable, stringSpace,
		physicalLiteralVector, evaledLiteralVector
	    )
	) return result;

	subTree = NodeThreadingCell (nextSubTree);


	/****	Dispatch	****/
	if (justStoredMagicWord) {
	    if (Intensional) outputTag (PC, ':');
	    for (x = -2;; x -= 2)
		if ((*PC) [x] != VByteCodeDescriptor::ByteCode_Tag) break;
		else MarkTagAsPostMagicWord ((*PC) [x+1]);

	    (*PC) [x] = VByteCodeDescriptor::ByteCode_DispatchMagic;
	    if (tracingCodeGeneration) IO_printf (
		"\tVMACHINE_BC_DispatchMagic\n"
	    );
	    justStoredMagicWord = false;
	}
	else
	{

	    if (Intensional) outputTag (PC, ':');
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Dispatch;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Dispatch\n");
	}


	/****	Evaluate Parameter	****/
	if (result = GenerateCode (
		pContainerSpace, subTree, programCPD, idTable, envTable, stringSpace,
		physicalLiteralVector, evaledLiteralVector
	    )
	) return result;

	if (paren)
	{
	    outputTag (PC, ')');
	    paren = false;
	}

	*(*PC)++ = VByteCodeDescriptor::ByteCode_NextMessageParameter;
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_NextMessageParameter\n");

	/****	Evaluate Binary Expression	****/
	if (Intensional)
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Intension;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Intension\n");
	}
	else
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Value;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Value\n");
	}
	wrapUpCase;

    case _OpenKeywordExpression:
    case _ClosedKeywordExpression:
        /****	Determine Selector and if Intensional	****/
	valence = '\0';
	Intensional = false;

	extractSelector (subTree, &selector, &Intensional, &valence);
	nextSubTree = subTree = NonTerminal_SubtreeHead (subTree);
	while (!Is_a_terminal_node (nextSubTree))
	    nextSubTree = NonTerminal_SubtreeTail (nextSubTree);

	if (LineNumber (NodeThreadingCell (subTree))
		!= LineNumber (nextSubTree))
	    outputTag (PC, PUSHNewLine);

	/****	If it is a known selector ...	****/
	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_KnownMessage;
	    *(*PC)++ = (unsigned char)messageIndex;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_KnownMessage %d(%s)\n",
					    messageIndex, Storage (&selector));
	}
	/****	...  Otherwise, Append to stringSpace and	****/
	else {
	    if ( -1 == addToStringSpace (stringSpace, idTable, envTable,
			      Storage (&selector),
			      strlen (Storage (&selector)), true)
	    ) return -1;

	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_KeywordMessageS,
				  VByteCodeDescriptor::ByteCode_KeywordMessage,
				  valence,
				  stringSpace);

	}


	/****	Evaluate Recipient    ****/
	if (result = GenerateCode (
		pContainerSpace, subTree, programCPD, idTable, envTable, stringSpace,
		physicalLiteralVector, evaledLiteralVector
	    )
	) return result;

	/****	Dispatch	****/
	if (justStoredMagicWord) {
	    if (Intensional) outputTag (PC, ':');
	    for (x = -2;; x -= 2)
		if ((*PC) [x] != VByteCodeDescriptor::ByteCode_Tag) break;
		else MarkTagAsPostMagicWord ((*PC) [x+1]);

	    (*PC) [x] = VByteCodeDescriptor::ByteCode_DispatchMagic;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_DispatchMagic\n");
	}
	else
	{
	    if (Intensional) outputTag (PC, ':');
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Dispatch;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Dispatch\n");
	}

	for (x = 0; x < (int) valence; x++)
	{
	    justStoredMagicWord = false;
	    /****	Get Parameter (skipping selector)	****/
	    subTree = NodeThreadingCell (subTree);
	    rslanglineno = LineNumber (subTree);
	    CurrentScanPointer.setDisplayOffsetTo (
		TerminalNode_Origin (subTree) + TerminalNode_Length(subTree)
	    );

	    subTree = NodeThreadingCell (subTree);
	    /****	Evaluate Parameter	****/
	    if (result = GenerateCode (
		    pContainerSpace, subTree, programCPD, idTable, envTable, stringSpace,
		    physicalLiteralVector, evaledLiteralVector
		)
	    ) return result;

	    if (period && x + 1 == valence)
	    {
	        outputTag (PC, '.');
		period = false;
	    }

	    if (paren && x + 1 == valence)
	    {
	        outputTag (PC, ')');
		paren = false;
	    }

	    *(*PC)++ = VByteCodeDescriptor::ByteCode_NextMessageParameter;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_NextMessageParameter\n");

	}

	/****	Evaluate Keyword Expression	****/
	if (Intensional)
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Intension;
		  if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Intension\n");
	}
	else
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_Value;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_Value\n");
	}
        wrapUpCase;
    case _UnaryExpression:
        /****	Determine selector, if it is known, and if it is
		intensional.
	****/
	receiver = NonTerminal_SubtreeHead (subTree);
	xOperator = NonTerminal_SubtreeTail (subTree);
	if ((TokenTypes)NodeType (receiver) == _Magic)
	{
	    if (result = GenerateCode (
		    pContainerSpace, receiver, programCPD, idTable, envTable, stringSpace,
		    physicalLiteralVector, evaledLiteralVector
		)
	    ) return result;
	    if (result = GenerateCode (
		    pContainerSpace, xOperator, programCPD, idTable, envTable, stringSpace,
		    physicalLiteralVector, evaledLiteralVector
		)
	    ) return result;
	    wrapUpCase;
	}

	Intensional =
		(TokenTypes)NodeType (xOperator) == _IntensionalIdentifier;

	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (xOperator)],
			    TerminalNode_Length (xOperator));

	nextSubTree = receiver;
	while (!Is_a_terminal_node (nextSubTree))
	    nextSubTree = NonTerminal_SubtreeTail (nextSubTree);

	if (LineNumber (xOperator)
		!= LineNumber (nextSubTree))
	    outputTag (PC, PUSHNewLine);

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_KnownMessage;
	    *(*PC)++ = (unsigned char)messageIndex;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_KnownMessage %d(%s)\n",
					    messageIndex, Storage (&selector));
	}
	else {
	    /****	Append Selector to StringSpace	****/
	    if ( -1 == addToStringSpace (stringSpace, idTable, envTable,
			      Storage (&selector),
			      strlen (Storage (&selector)), true)
	    ) return -1;

	    output1ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_UnaryMessageS,
				  VByteCodeDescriptor::ByteCode_UnaryMessage,
				  stringSpace);
	}

	/****	Evaluate Recipient	****/
	if (result = GenerateCode (
		pContainerSpace, receiver, programCPD, idTable, envTable, stringSpace,
		physicalLiteralVector, evaledLiteralVector
	    )
	) return result;

	if (Intensional)
	{
	    outputTag (PC, ':');
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_WrapupUnaryIntension;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_WrapupUnaryIntension\n");
	}
	else
	{
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_WrapupUnaryValue;
	    if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_WrapupUnaryValue\n");
	}
        wrapUpCase;

    case _EmptyExpression:
	/****	Output NOP bytecode	****/
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreNoValue;
        wrapUpCase;

    case _Root:
        /****	Loop through children, generating code for each expression
	****/

	for (nextSubTree = NonTerminal_SubtreeHead (subTree);
	     nextSubTree != NonTerminal_SubtreeTail (subTree);
	     nextSubTree = NodeThreadingCell (nextSubTree))
	{
	    if (result = GenerateCode (
		    pContainerSpace, nextSubTree, programCPD, idTable, envTable,
		    stringSpace, physicalLiteralVector, evaledLiteralVector
		)
	    ) return result;
	    justStoredMagicWord = false;
	}

	if (result = GenerateCode (
		pContainerSpace, nextSubTree, programCPD, idTable, envTable, stringSpace,
		physicalLiteralVector, evaledLiteralVector
	    )
	) return result;

        wrapUpCase;
    case _HeadedBlock:
	header = NonTerminal_SubtreeHead (subTree);

    case _SimpleBlock:
        /****	Create an 'empty' environment and link to the current
	   	environment.
	****/

        pNewDictionary.setTo (new rtDICTIONARY_Handle (pContainerSpace));

	/****	Arrange to get a new program built for this block	****/

	justStoredMagicWord = false;
	newProgram = buildProgram (
	    pContainerSpace, NonTerminal_SubtreeTail (subTree), pNewDictionary, header
	);

	/****	Cleanup dictionary  ****/
	pNewDictionary->alignAll ();

	if (IsNil (newProgram))
	    return -1;

	/****	Insert in Physical Literal Table	****/
	Storage (physicalLiteralVector)[Index (physicalLiteralVector)] =
					    newProgram;

	/****	Output Code	****/
	if (Index (physicalLiteralVector) > MAX_BYTE_SIZE)
	{
		  AlignShort ((*PC));
		  *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreLexBindingS;
		  if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_StoreLexBindingS ");
		  *(unsigned short *)(*PC) =
					    Index (physicalLiteralVector)++;
		  (*PC) += 2;
	}
	else
	{
		  *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreLexBinding;
		  if (tracingCodeGeneration)
		      IO_printf ("\tVMACHINE_BC_StoreLexBinding ");
		  *(*PC)++ = (unsigned char)Index (physicalLiteralVector)++;
	}
	if (tracingCodeGeneration)
	    IO_printf ("%u\n", Index (physicalLiteralVector) - 1);

	/****	Determine if the PLVector is still big enough	****/
	if ((int) Index (physicalLiteralVector) >= Size (physicalLiteralVector))
	    Storage (physicalLiteralVector) = (M_CPD **)UTIL_Realloc (
		Storage (physicalLiteralVector), (
		    Size (physicalLiteralVector) += PLVectorINCREMENT
		) * sizeof (M_CPD*)
	    );

        wrapUpCase;

    case _Magic:
	/****	Get the magic word index	****/
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));

	if ((result = MAGIC_StringToMWI (Storage (&selector))) < 0) {
	    /**** error ****/
	    rslangerror (
		UTIL_FormatMessage ("Unrecognized magic word '%s'", Storage (&selector))
	    );
	    return -1;
	}

	/****	Output Code	****/
	*(*PC)++ = VByteCodeDescriptor::ByteCode_StoreMagic;
	*(*PC)++ = (unsigned char)result;
	if (tracingCodeGeneration)
	    IO_printf ("\tVMACHINE_BC_StoreMagic %d(%s)\n", result, Storage (&selector));

	justStoredMagicWord = true;

        break;
    case _LocalId:

	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));

        outputTag (PC, '!');

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	/*****  Add to the environment table  *****/
	    (void)STD_tsearch (KS__ToString (messageIndex), envTable, (VkComparator)strcmp);
	/****  Process as an intension.	****/
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_KnownUnaryIntension;
	    *(*PC)++ = (unsigned char)messageIndex;
	    *(*PC)++ = (unsigned char) VMagicWord_Current;
	    if (tracingCodeGeneration)
	       IO_printf ("\tVMACHINE_BC_KnownUnaryIntension %u(%s) 0\n",
				     messageIndex,
				     Storage (&selector));
	}
	else {
        /****	Add to the Id table and to the environment Table.
	****/
	    if ( -1 == addToStringSpace (stringSpace, idTable, envTable,
			      &sourceBASE [TerminalNode_Origin (subTree)],
			      TerminalNode_Length (subTree), true)
	    ) return -1;

	    (void)STD_tsearch (&Storage (stringSpace)[stringIndex], envTable,(VkComparator)strcmp);

	/****  Process as an intension.	****/
	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_UnaryIntensionS,
				  VByteCodeDescriptor::ByteCode_UnaryIntension,
				  (unsigned char) VMagicWord_Current,
				  stringSpace);
	}
        wrapUpCase;
    case _UnarySelector:
	subTree = NonTerminal_SubtreeHead (subTree);
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));

        conditionallyTagNewline (PC, subTree);

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreKnownSelector;
	    *(*PC)++ = (unsigned char)messageIndex;
	    if (tracingCodeGeneration)
	       IO_printf ("\tVMACHINE_BC_StoreKnownSelector %u(%s)\n", messageIndex,
				     Storage (&selector));
	}
	else {
	    /****	Append Selector to StringSpace	****/
	    if ( -1 == addToStringSpace (stringSpace, idTable, envTable,
			      Storage (&selector),
			      strlen (Storage (&selector)), true)
	    ) return -1;

	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_StoreSelectorS,
				  VByteCodeDescriptor::ByteCode_StoreSelector,
				  0,
				  stringSpace);

	}
	wrapUpCase;



    case _BinarySelector:
	/****  Binaries are all known	****/
	subTree = NonTerminal_SubtreeHead (subTree);
	WriteToBuffer (&selector, &sourceBASE [TerminalNode_Origin (subTree)],
			    TerminalNode_Length (subTree));
        conditionallyTagNewline (PC, subTree);

	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreKnownSelector;
	    *(*PC)++ = (unsigned char)messageIndex;
	    if (tracingCodeGeneration)
	       IO_printf ("\tVMACHINE_BC_StoreKnownSelector %u(%s)\n", messageIndex,
				     Storage (&selector));
	}
	else
	    /****	Error	****/;
	    wrapUpCase;

    case _KeywordSelector:
        /****	Determine Selector	****/
	valence = '\0';

	extractSelector (subTree, &selector, &Intensional, &valence);

	/****	If it is a known selector ...	****/
	if ((messageIndex = SELECTOR_StringToKSI (Storage (&selector))) >= 0) {
	    *(*PC)++ = VByteCodeDescriptor::ByteCode_StoreKnownSelector;
	    *(*PC)++ = (unsigned char)messageIndex;
	    if (tracingCodeGeneration)
	       IO_printf ("\tVMACHINE_BC_StoreKnownSelector %u(%s)\n", messageIndex,
				     Storage (&selector));
	}
	/****	...  Otherwise, Append to stringSpace and	****/
	else {
	    if (-1 == addToStringSpace (stringSpace, idTable, envTable,
			      Storage (&selector),
			      strlen (Storage (&selector)), true)
	    ) return -1;

	    output2ParamByteCode (PC,
				  VByteCodeDescriptor::ByteCode_StoreSelectorS,
				  VByteCodeDescriptor::ByteCode_StoreSelector,
				  valence,
				  stringSpace);

	}

        wrapUpCase;
    default:
        /****   Error  ****/
	rslangerror (
	    UTIL_FormatMessage ("Unknown Node Type: %u", NodeType (subTree))
	);
	return -1;

    }
    if (period)
        outputTag (PC, '.');

    if (paren)
        outputTag (PC, ')');


/*****  Check to ensure that there is enough space for a set of byte codes.
	if not, call GrowContainer.  This check at the end is for the space
	that will be needed when it returns from one level of recursion and
	finishes up by emitting the rest of the byte code.
 *****/
    if ((size_t) (
	    (*PC) - M_CPD_ContainerBaseAsType (programCPD, unsigned char)
	) + MAX_BYTES_TO_ADD > M_CPD_Size (programCPD)
    )
/*
    if ((unsigned int) ((*PC)
	  - (unsigned char *)M_CPD_ContainerBase (programCPD)
	  + MAX_BYTES_TO_ADD)
		> M_CPD_Size (programCPD))
*/
    {
	if (tracingCompile)
	    IO_printf ("Increasing Program Size by %d\n", BCVectorINCREMENT);

        programCPD->GrowContainer (BCVectorINCREMENT);
	rtBLOCK_CPDEnd (programCPD) += BCVectorINCREMENT;
    }

/*****  Completed Successfully  *****/
    return 0;

}


/*---------------------------------------------------------------------------
 *****  Internal Routine to actually build an instance of a block
 *
 *  Arguments:
 *	pContainerSpace		- the address of the ASD for the object space
 *				  in which to create the block's container.
 *	root			- the block's parse tree root node index.
 *	pDictionary		- the address of the block's dictionary.
 *	Header			- the Header for the block (Nil) if it
 *				  is a simple block.
 *  Returns:
 *	The CPD pointer for the block or an error description
 *
 *****/
PrivateFnDef M_CPD *buildProgram (
    M_ASD *pContainerSpace, int root, rtDICTIONARY_Handle *pDictionary, int Header
) {
    int
	x;

    stringSpaceType
	stringSpace;

    PLVectorType
	plVector;

    ELEVectorType
	eleVector;

    void
	*envirTable = NilOf (void *),
	*symbolTable = NilOf (void *);

    if (tracingCompile)
        IO_printf ("Entered buildProgram. root = %08X, Header = %08X\n", root,  Header);
    if (tracingCodeGeneration)
	IO_printf ("[\n");

    InitializeStringSpace (&stringSpace);

    /****	Initialize plVector	****/
    Storage (&plVector) = (M_CPD **)UTIL_Malloc (INITIAL_PLVECTOR_SIZE * sizeof (M_CPD *));
    Size (&plVector) = INITIAL_PLVECTOR_SIZE;
    Index (&plVector) = 0;

    /****	Initialize eleVector	****/
    Storage (&eleVector) = (int *)UTIL_Malloc (INITIAL_ELEVECTOR_SIZE * sizeof (int));
    Size (&eleVector) = INITIAL_ELEVECTOR_SIZE;
    Index (&eleVector) = 0;

    /*****  Create an rtBLOCK instance  *****/
    if (tracingCompile)
        IO_printf ("Calling rtBLOCK_New\n");

    M_CPD *programCPD = rtBLOCK_New (pContainerSpace);

    /*****  Store dictionary reference in block  *****/
    if (tracingCompile)
        IO_printf ("Inserting Environment\n");

    programCPD->StoreReference (rtBLOCK_CPx_LocalEnv, pDictionary);

    /*****  Aquire Parameters  *****/
    if (NULL_NODE != Header) {
	if (tracingCompile) IO_printf ("Acquiring Parameters\n");
	if (-1 == acquireParameters (
	    Header, programCPD, &symbolTable, &envirTable, &stringSpace))
	{
	    UTIL_Free (Storage (&stringSpace));
	    UTIL_Free (Storage (&plVector));
	    UTIL_Free (Storage (&eleVector));
	    programCPD->release ();
	    
	    return NilOf (M_CPD*);
	}
    }

    if (tracingCompile)
        IO_printf ("Root Code Generation\n");

    if (-1 == GenerateCode (
	    pContainerSpace, root, programCPD, &symbolTable, &envirTable,
	    &stringSpace, &plVector, &eleVector
	)
    ) {
	for (x = 0; x < (int) Index (&plVector); x++)
	    (Storage (&plVector) [x])->release ();

	UTIL_Free (Storage (&stringSpace));
	UTIL_Free (Storage (&plVector));
	UTIL_Free (Storage (&eleVector));
        programCPD->release ();

	return NilOf (M_CPD*);
    }
    *(bCodePtr (programCPD))++ = VByteCodeDescriptor::ByteCode_Exit;
    if (tracingCodeGeneration)
        IO_printf ("\tVMACHINE_BC_Exit\n");


    /*****  Remove 'air' from Block	*****/
    programCPD->ShiftContainerTail (
	rtBLOCK_CPx_End,
	0,
	M_CPD_PointerToByte (programCPD, rtBLOCK_CPx_PC) -
	M_CPD_PointerToByte (programCPD, rtBLOCK_CPx_End),
	true
    );

    if (tracingCompile)
	IO_printf ("Updating local environment.\n");
    /*****  Update local environment with local Variables	*****/
    LocalMDCPD = pDictionary;

    if (tracingCompile)
	IO_printf ("preparing to walk tree.\n");

    STD_twalk (envirTable, InsertMethodDEntries);

    if (tracingCompile)
        IO_printf ("Appending Components\n");

    /*****  Append stringSpace to Program  *****/
    rtBLOCK_AppendStringSpace (programCPD, Storage (&stringSpace),
			 stringSpaceIndex (&stringSpace));

    /*****  Append plVector to Program	*****/
    rtBLOCK_AppendPLVector (programCPD, Storage (&plVector), Index (&plVector));

    /*****  Append eleVector to Program	    *****/
    rtBLOCK_AppendELEVector (programCPD,
		Storage (&eleVector), Index (&eleVector));

    /*****  Initialize CPD pointers  *****/
    rtBLOCK_InitStdCPD (programCPD);

    for (x = 0; x < (int) Index (&plVector); x++)
        (Storage (&plVector) [x])->release ();

    UTIL_Free (Storage (&stringSpace));
    UTIL_Free (Storage (&plVector));
    UTIL_Free (Storage (&eleVector));

    if (retainingBlockForBrowsing)
	blockIObj = RTYPE_QRegister (programCPD->GetCPD ());

    if (tracingCodeGeneration)
	IO_printf ("]\n");

    return programCPD;
}


/**************************
 *****  The Compiler  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to compile a source string into an executable program.
 *
 *  Argument:
 *	source			- the address of the string to compile.
 *	dictionary		- a CPD for the dictionary in which new top
 *				  level local variables will be created.
 *	pMessageBuffer		- the address of a buffer which will be
 *				  initialized with the text of any error
 *				  messages associated with the compilation.
 *				  If 'nil', the compiler will print an
 *				  error message to standard output.
 *	sMessageBuffer		- the maximum size of the supplied message
 *				  buffer.
 *	errorLine		- the address of an integer variable which
 *				  will be set to the line number of the first
 *				  compilation error encountered.
 *	errorCharacter		- the address of an integer variable which
 *				  will be set to the character position of the
 *				  first compilation error encountered.
 *
 *  Returns:
 *	The address of a CPD for:
 *	    the created program if the parse was successful
 *	OR:
 *	    Nil if the parse was unsuccessful.
 *
 *  Notes:
 *	This program should not display any explicit error messages; rather,
 *	it should quietly return a CPD for an object describing in some
 *	structured way what it finds objectionable in the input it was asked
 *	to compile.
 *****/
PublicFnDef M_CPD *RSLANG_Compile (
    M_ASD		*pContainerSpace,
    char const		*source,
    rtDICTIONARY_Handle	*dictionary,
    char		*pMessageBuffer,
    unsigned int	 sMessageBuffer,
    int			*errorLine,
    int			*errorCharacter
) {
    if (tracingCompile)
        IO_printf ("Starting ... ");

    rslanglineno = 0;
    CurrentScanPointer.reset ();
    IntensionalOp = false;

    sourceBASE		=
    rsSOURCE		= source;

    MessageBuffer	= pMessageBuffer;
    MessageBufferMax	= sMessageBuffer;
    ErrorLine		= errorLine;
    ErrorCharacter	= errorCharacter;

    if (tracingCompile)
        IO_printf ("Parsing ... ");

    Initialize_ParseTree_Nodes ();
    if (rslangparse ())
        return NilOf (M_CPD*);
    rslanglineno = 0;

    if (tracingCompile)
        IO_printf ("GO!!!\n");

    return buildProgram (pContainerSpace, rsROOT, dictionary, NULL_NODE);
}


/*****************************
 *****  The De-Compiler  *****
 *****************************/

/*---------------------------------------------------------------------------
 * The De_Compiler runs as a stack machine. A block's byte code vector is read
 * and various push, pop and output operations are performed based on the byte
 * code's defined decompile operation and the values of the byte code's
 * parameters.	Some provision is made to output the decompiled program in
 * properly indented, easily read style.
 *---------------------------------------------------------------------------
 */

/********************************************
 *****	Decompiler Stack Declarations	*****
 ********************************************/

struct RSLangDC_StackFrameType {
    VByteCodeDescriptor::ByteCode
	byteCode;
    char const
	*stringVal;
    int
	valLength;
    unsigned int
	m_iOutputOffset;
    unsigned char
	newline,
	colon,
	m_iMoreParameters,
	nextpOutput;
};

/****  The Stack	****/
struct RSLandDC_StackType {
    int
	size;
    RSLangDC_StackFrameType
        *stack;
};
PrivateVarDef RSLandDC_StackType RSLangDC_Stack;

/****  The Stack Pointer (actually an index to the next free stack frame) ****/
PrivateVarDef int SP;

/*****	Access Macros	*****/

#define rslangSFNewLine(frame)\
    RSLangDC_Stack.stack [frame].newline

#define rslangSFIntensional(frame)\
    RSLangDC_Stack.stack [frame].colon

#define rslangSFMorep(frame)\
    RSLangDC_Stack.stack [frame].m_iMoreParameters

#define rslangSFNextp(frame)\
    RSLangDC_Stack.stack [frame].nextpOutput

#define rslangSFByteCode(frame)\
    RSLangDC_Stack.stack [frame].byteCode

#define rslangSFStringVal(frame)\
    RSLangDC_Stack.stack [frame].stringVal

#define rslangSFValLength(frame)\
    RSLangDC_Stack.stack [frame].valLength

#define rslangSFOutputOffset(frame)\
    RSLangDC_Stack.stack [frame].m_iOutputOffset

#define topOfStackVal (SP > 0 ? rslangSFStringVal (SP - 1) : NilOf (char *))
#define topOfStack (SP > 0 ? rslangSFByteCode (SP - 1) : 0)

/*****  Constants	*****/

#define InitialStackSize 128
#define StackIncrement 32


/***********************************************
 ****	Output Operations and Structures    ****
 ***********************************************/

/****  Pretty Printer Variables  ****/
PrivateVarDef unsigned int IndentationIndex = 0;
PrivateVarDef char const *headMark   = "|";
PrivateVarDef char const *blockClose = "]";
PrivateVarDef char const *blockOpen  = "[";
PrivateVarDef char const *magic      = "^";
PrivateVarDef char const *evaledLit  = "$";
PrivateVarDef char const *selMark    = "\'";
PrivateVarDef char const *colon      = ":";
PrivateVarDef char const *quote      = "\"";
PrivateVarDef char const *backslash  = "\\";
PrivateVarDef char const *newline    = "\n";
PrivateVarDef char const *tab	       = "    ";

PrivateVarDef stringSpaceType decompiledProgram;


/*****  Utilities to copy outputVal to the string space holding the decompiled
 *****	      program being produced.
 *
 *  Argument:
 *	outputVal	- a pointer to a null terminated string to be appended
 *			  to the decompiled program.
 *
 *****/
PrivateFnDef void DoBasicOutput (char const *outputVal, int len) {
    if (0 == len)
	return;
    ConditionalGrow (&decompiledProgram, len, DoNothing, DoNothing);
    memcpy (Ptr (&decompiledProgram), outputVal, len);
    if (tracingDecompile) IO_printf (
	"RSLANG[DoBasicOutput]:%.*s o:%u co:%d\n", 
	len, Ptr (&decompiledProgram),
	bufferIndex (&decompiledProgram),
	SP > 0 ? rslangSFOutputOffset (SP - 1) : UINT_MAX
    );
    Ptr (&decompiledProgram) += len;
}

#define DoBlank DoBasicOutput (tab, 1)

#define Output(outputVal, len, doBlank) {\
    DoBasicOutput (outputVal, len);\
    doBlank;\
}


/********************************************
 *****	Internal Stack Handling Macros  *****
 ********************************************/

PrivateFnDef void outputStkVal (
    void
)
{
    if (SP > 0)
    {
	if (rslangSFNewLine (SP - 1))
	{
	    unsigned int x;

	    Output (newline, 1, DoNothing);
	    for (x = 1; x <= IndentationIndex; x++) Output (tab, 4, DoNothing);
	    rslangSFNewLine (SP - 1) = false;
	}
	if (rslangSFIntensional (SP - 1))
	{
	    Output (colon, 1, DoNothing);
	    rslangSFIntensional (SP - 1) = false;
	}
	if (IsntNil (topOfStackVal))
	{
	    if (tracingStackOps) IO_printf (
		"...RSLANG[outputStkVal]: SP:[%d]\n", SP
	    );
	    Output (
		rslangSFStringVal (SP - 1), rslangSFValLength (SP - 1), DoBlank
	    );
	    rslangSFStringVal (SP - 1) = NilOf (char *);
	}
    }
}

PrivateFnDef void Grow_DCStack (
    void
)
{
    if (SP >= Size (&RSLangDC_Stack))
    {
	if (tracingStackOps) IO_printf ("... Growing DC Stack\n");
	RSLangDC_Stack.stack = (RSLangDC_StackFrameType *)UTIL_Realloc (
	    RSLangDC_Stack.stack,
	    (Size (&RSLangDC_Stack) += StackIncrement)
		* sizeof (RSLangDC_StackFrameType)
	);
	if (tracingStackOps) IO_printf (
	    "Reallocated Stack\n\t\tStack = %08X, Size = %u",
	    RSLangDC_Stack.stack,
	    Size (&RSLangDC_Stack)
	);
    }
}

PrivateFnDef void DCStack_Initialize () {
    RSLangDC_Stack.stack = (RSLangDC_StackFrameType *)UTIL_Malloc (
	InitialStackSize * sizeof (RSLangDC_StackFrameType)
    );
    Size (&RSLangDC_Stack) = InitialStackSize;
    SP = 0;
    if (tracingStackOps) IO_printf (
	StackAllocationMessage,
	RSLangDC_Stack.stack, Size (&RSLangDC_Stack)
    );
}

PrivateFnDef void DCStack_Push (VByteCodeDescriptor::ByteCode byteCode, char const *string, unsigned int length) {
    Grow_DCStack ();
    if (tracingStackOps) IO_printf (
	StackPushMessage, length, byteCode, length, string, SP
    );
    rslangSFNewLine	(SP) = false;
    rslangSFIntensional	(SP) = false;
    rslangSFMorep	(SP) = false;
    rslangSFNextp	(SP) = false;
    rslangSFValLength	(SP) = length;
    rslangSFByteCode	(SP) = byteCode;
    rslangSFStringVal	(SP) = string;
    rslangSFOutputOffset(SP) = bufferIndex (&decompiledProgram);

    SP++;
}

PrivateFnDef void DCStack_Push (VByteCodeDescriptor::ByteCode byteCode, char const *string) {
    DCStack_Push (byteCode, string, strlen (string));
}

PrivateFnDef void DCStack_Pop () {
    if (tracingStackOps)
	IO_printf (StackPopMessage, SP);
    --SP;
}


/************************************
 *****  Selector Decomposition  *****
 ************************************/

/*---------------------------------------------------------------------------
 ***** routine to pull the index'th keyword out of a selector.
 *
 *  Arguments:
 *	cpd		- The cpd for a block containing a selector.
 *	keyword		- The address of a character pointer which will be set
 *			  to the desired keyword.
 *	index		- which keyword to get from the selector.
 *
 *  Returns:
 *	Length of keyword.
 *
 *****/
PrivateFnDef int getSelector (M_CPD *cpd, char const **keyword, int index) {
    if (rtBLOCK_CPD_SelectorIsKnown (cpd)) {
	*keyword = KS__ToString (rtBLOCK_CPD_SelectorIndex (cpd));
	if (1 == SELECTOR_KSValence (rtBLOCK_CPD_SelectorIndex (cpd))) {
	    if (tracingDecompile)
	        IO_printf ("Known Selector: %s(%d)\n", *keyword, strlen (*keyword));
	    return strlen (*keyword);
	}
    }
    else
        *keyword = rtBLOCK_CPD_SelectorName (cpd);

    int  x;
    for (x = 0; x < index;)
        if (*(*keyword)++ == ':') x++;

    for (x = 0; (*keyword)[x] && (*keyword)[x] != ':'; x++);

    if (tracingDecompile)
        IO_printf ("User Selector: %*s(%d)\n", (*keyword)[x] ? x + 1 : x,
		    *keyword, (*keyword)[x] ? x + 1 : x);

    return (*keyword)[x]
	    ? x + 1
	    : x;
}


/*******************************
 *****  Decompiler Kernel  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to convert a compiled program into a string.
 *
 *  Argument:
 *	pBlockCPD		- the address of a CPD which identifies a
 *				  block for the program to be de-compiled.
 *
 *  Returns:
 *	Nothing
 *
 *  Note:
 *	No pointer index is passed.  This is assumed to be the CPD wanted.
 *
 *****/
PrivateFnDef void reproduceSource (
    M_CPD *pBlockCPD, M_CPD *pReferenceCPD, unsigned int *pDecompiledPCOffset
) {
    M_CPD *nestedBlockCPD;

    unsigned int x;
    bool parametersInStack = false, finished = false, needToGetSelector = false;
    int length, pcount, sindex;

    bool pushNewLine = false;
    char const *selectorPtr, *tptr;
    char
	tagString [2],
	outputString [128];

    tagString [1] = '\0';

    if (!(rtBLOCK_CPD_NoSelector (pBlockCPD))) {
	needToGetSelector = true;
        if (rtBLOCK_CPD_SelectorIsKnown (pBlockCPD)) {
	    if (0 == SELECTOR_KSValence (rtBLOCK_CPD_SelectorIndex (pBlockCPD)))
	    {

		Output (headMark, 1, DoNothing);
		length = strlen (KS__ToString
				     (rtBLOCK_CPD_SelectorIndex (pBlockCPD)));
		Output (
		    KS__ToString (rtBLOCK_CPD_SelectorIndex (pBlockCPD)),
		    length,
		    DoNothing
		);
		Output (headMark, 1, DoNothing);
	    }
	}
	else {
	    if (IsNil (strchr (rtBLOCK_CPD_SelectorName (pBlockCPD), ':'))) {

		Output (headMark, 1, DoNothing);
		length = strlen (rtBLOCK_CPD_SelectorName (pBlockCPD));
		Output (
		    rtBLOCK_CPD_SelectorName (pBlockCPD), length, DoNothing
		);
		Output (headMark, 1, DoNothing);
	    }
        }
    }

    VByteCodeScanner iByteCodeScanner (pBlockCPD);
    if (pReferenceCPD)  {
	*pDecompiledPCOffset = bufferIndex (&decompiledProgram);
	if (tracingDecompile)
	    IO_printf ("Reference PC: %d\n", rtBLOCK_CPD_PC (pReferenceCPD));
    }
    do {
/*****  Fetch the next instruction...  *****/
	iByteCodeScanner.fetchInstruction (true);

	if (tracingDecompile) IO_printf (
	    "Instruction[%d]:%s  \n",
	    iByteCodeScanner.bcAddress (),
	    iByteCodeScanner.fetchedByteCodeName ()
	);

	VByteCodeDescriptor::ByteCode xByteCode = iByteCodeScanner.fetchedByteCode ();
	switch (xByteCode) {
	default:
	case VByteCodeDescriptor::ByteCode_Pad0:
	case VByteCodeDescriptor::ByteCode_Pad1:
	case VByteCodeDescriptor::ByteCode_Pad2:
	case VByteCodeDescriptor::ByteCode_Value:
	case VByteCodeDescriptor::ByteCode_Intension:
	case VByteCodeDescriptor::ByteCode_Comment:
	case VByteCodeDescriptor::ByteCode_CommentS:

	    break;

	case VByteCodeDescriptor::ByteCode_Tag:
	    if (iByteCodeScanner.fetchedLiteral () == PUSHNewLine) {
		pushNewLine = true;
		break;
	    }
	    switch (outputString [0] = (char)iByteCodeScanner.fetchedLiteral ()) {
	    case '\n':
		Output (outputString, 1, DoNothing);
		for (x = 0; x < IndentationIndex; x++)
		    Output (tab, 4, DoNothing);
		break;
	    case '|':
		if (!parametersInStack) {
		    parametersInStack = true;
		    pcount = 0;
		}
		else {
		    for (sindex = 0; sindex < pcount; sindex++) {
			if (needToGetSelector) {
			    length = getSelector (pBlockCPD, &selectorPtr, sindex);
			    Output (selectorPtr, length, DoBlank);
			}
			else
			    Output (colon, 1, DoNothing);

			outputStkVal ();
			DCStack_Pop ();
		    }

		    parametersInStack = false;
		}

	    case ';':
	    case ')':
		if (Ptr (&decompiledProgram) > Storage(&decompiledProgram) && *(Ptr (&decompiledProgram) - 1) == ' ') {
		    Ptr (&decompiledProgram)--;
		    if (pReferenceCPD && *pDecompiledPCOffset > bufferIndex (&decompiledProgram))
			*pDecompiledPCOffset = bufferIndex (&decompiledProgram);
		}

		Output (outputString, 1, DoBlank);

		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD) && ';' == outputString[0])
		    *pDecompiledPCOffset = bufferIndex (&decompiledProgram);
		break;

	    case ':':
		if (SP > 0)
		    rslangSFIntensional (SP - 1) = true;
		break;

	    case '.':
	    case '(':
	    case '!':
		Output (outputString, 1, DoNothing);
		break;

	    default:
		break;
	    }

	    break;

	case VByteCodeDescriptor::ByteCode_AcquireParameter:
	case VByteCodeDescriptor::ByteCode_AcquireParameterS:
	    DCStack_Push (
		xByteCode, iByteCodeScanner.fetchedSelectorName ()
	    );
	    pcount++;
	    break;

	case VByteCodeDescriptor::ByteCode_AcquireKnownParameter:
	    DCStack_Push (
		xByteCode, KS__ToString (iByteCodeScanner.fetchedSelectorIndex ())
	    );
	    pcount++;
	    break;

	case VByteCodeDescriptor::ByteCode_UnaryMessage:
	case VByteCodeDescriptor::ByteCode_UnaryMessageS:
	    DCStack_Push (
		xByteCode, iByteCodeScanner.fetchedSelectorName ()
	    );
	    if (pushNewLine)
	    {
		pushNewLine = false;
		rslangSFNewLine (SP - 1) = true;
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_KeywordMessage:
	case VByteCodeDescriptor::ByteCode_KeywordMessageS:
	    {
		unsigned int valence = iByteCodeScanner.fetchedSelectorValence ();
		selectorPtr = iByteCodeScanner.fetchedSelectorName ();
		if (tracingDecompile)
		    IO_printf ("Selector -- '%s'\n", selectorPtr);
		tptr = selectorPtr + strlen (selectorPtr) - 1;
		for (x = 0; x < valence; x++)
		{
		    length = 1;
		    while (*--tptr != ':' && tptr != selectorPtr) length++;
		    length += tptr == selectorPtr ? 1 : 0;
		    if (tracingDecompile) IO_printf (
			"Keyword(%d) -- '%.*s'\n",
			x,
			length,
			tptr == selectorPtr ? tptr : tptr + 1
		    );

		    DCStack_Push (
			xByteCode, tptr == selectorPtr ? tptr : tptr + 1, length
		    );
		    rslangSFMorep (SP - 1) = x > 0;
		    if (x + 1 < valence)
			rslangSFNextp (SP - 1) = true;
		    if (pushNewLine)
		    {
			pushNewLine = false;
			rslangSFNewLine (SP - 1) = true;
		    }
		}
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_KnownMessage:
	    {
		unsigned int ksi = iByteCodeScanner.fetchedSelectorIndex ();
		unsigned int valence = SELECTOR_KSValence (ksi);

		selectorPtr = KS__ToString (ksi);
		if (tracingDecompile)
		    IO_printf ("Selector -- '%s'\n", selectorPtr);
		length = strlen (selectorPtr);
		tptr = selectorPtr + length - 1;
		if (valence == 0)
		{
		    if (tracingDecompile)
			IO_printf ("Keyword(%d) -- '%.*s'\n", 0, length, selectorPtr);

		    DCStack_Push (xByteCode, selectorPtr, length);
		    if (pushNewLine)
		    {
			pushNewLine = false;
			rslangSFNewLine (SP - 1) = true;
		    }
		}
		else for (x = 0; x < valence; x++)
		{
		    length = 1;
		    while (*--tptr != ':' && tptr != selectorPtr) length++;
		    length += tptr == selectorPtr ? 1 : 0;
		    if (tracingDecompile)
			IO_printf ("Keyword(%d) -- '%.*s'\n", x, length,
					 tptr == selectorPtr ? tptr : tptr + 1);

		    DCStack_Push (
			xByteCode, tptr == selectorPtr ? tptr : tptr + 1, length
		    );
		    rslangSFMorep (SP - 1) = x > 0;
		    if (x + 1 < valence)
			rslangSFNextp (SP - 1) = true;

		    if (pushNewLine)
		    {
			pushNewLine = false;
			rslangSFNewLine (SP - 1) = true;
		    }
		}
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_LAssign:
	case VByteCodeDescriptor::ByteCode_RAssign:
	case VByteCodeDescriptor::ByteCode_Concat:
	case VByteCodeDescriptor::ByteCode_LT:
	case VByteCodeDescriptor::ByteCode_LE:
	case VByteCodeDescriptor::ByteCode_GE:
	case VByteCodeDescriptor::ByteCode_GT:
	case VByteCodeDescriptor::ByteCode_Equal:
	case VByteCodeDescriptor::ByteCode_Equivalent:
	case VByteCodeDescriptor::ByteCode_NEqual:
	case VByteCodeDescriptor::ByteCode_NEquivalent:
	case VByteCodeDescriptor::ByteCode_Or:
	case VByteCodeDescriptor::ByteCode_And:
	case VByteCodeDescriptor::ByteCode_Add:
	case VByteCodeDescriptor::ByteCode_Subtract:
	case VByteCodeDescriptor::ByteCode_Multiply:
	case VByteCodeDescriptor::ByteCode_Divide:
	    DCStack_Push (
		xByteCode, KS__ToString (
		    iByteCodeScanner.fetchedAssociatedKSI ()
		)
	    );
	    break;

	case VByteCodeDescriptor::ByteCode_Dispatch:
	    //  Selector Tracking
	    rslangSFOutputOffset (SP - 1) = bufferIndex (&decompiledProgram);

	    outputStkVal ();
	    break;

	case VByteCodeDescriptor::ByteCode_DispatchMagic:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		if (tracingDecompile)
		    IO_printf ("'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);

		Output (magic, 1, DoNothing);
		length = strlen (MAGIC_MWIToString (mwi));
		Output (MAGIC_MWIToString (mwi), length, DoBlank);
		for (
		    x = 0;
		    iByteCodeScanner.getUByte (x++) == VByteCodeDescriptor::ByteCode_Tag;
		    x++
		) if (TagIsPostMagicWord (iByteCodeScanner.getUByte (x)))
		{
		    ConvertPostMagicWordTag (tagString [0], iByteCodeScanner.getUByte (x));
		    Output (tagString, 1, DoBlank);
		}

		//  Selector Tracking
		rslangSFOutputOffset (SP - 1) = bufferIndex (&decompiledProgram);

		outputStkVal ();
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_NextMessageParameter: {
		unsigned int iMoreParameters = rslangSFMorep (SP - 1);
		unsigned int iSelectorOffset = rslangSFOutputOffset (SP - 1);

		DCStack_Pop ();

		if (iMoreParameters)
		    rslangSFOutputOffset (SP - 1) = iSelectorOffset;

		if (SP > 0 && rslangSFNextp (SP - 1))
		    outputStkVal ();

		//  Selector Tracking
		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		    *pDecompiledPCOffset = iSelectorOffset;
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_KnownUnaryValue:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		unsigned int ksi = iByteCodeScanner.fetchedSelectorIndex ();
		if (tracingDecompile)
		    IO_printf ("[magic]'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);
		if (mwi != (unsigned)VMagicWord_Current || (
			iByteCodeScanner.getUByte (0) == VByteCodeDescriptor::ByteCode_Tag &&
			TagIsPostMagicWord (iByteCodeScanner.getUByte (1))
		    )
		)
		{
		    Output (magic, 1, DoNothing);
		    length = strlen (MAGIC_MWIToString (mwi));
		    Output (MAGIC_MWIToString (mwi), length, DoBlank);
		}
		for (
		    x = 0;
		    iByteCodeScanner.getUByte (x++) == VByteCodeDescriptor::ByteCode_Tag;
		    x++
		) if (TagIsPostMagicWord (iByteCodeScanner.getUByte (x)))
		{
		    ConvertPostMagicWordTag (tagString [0], iByteCodeScanner.getUByte (x));
		    Output (tagString, 1, DoBlank);
		}

		//  Selector Tracking
		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		    *pDecompiledPCOffset = bufferIndex (&decompiledProgram);

		length = strlen (KS__ToString (ksi));
		Output (KS__ToString (ksi), length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_KnownUnaryIntension:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		unsigned int ksi = iByteCodeScanner.fetchedSelectorIndex ();
		if (tracingDecompile)
		    IO_printf ("[magic]'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);
		if (mwi != (unsigned)VMagicWord_Current || (
			iByteCodeScanner.getUByte (0) == VByteCodeDescriptor::ByteCode_Tag &&
			TagIsPostMagicWord (iByteCodeScanner.getUByte (1))
		    )
		)
		{
		    Output (magic, 1, DoNothing);
		    length = strlen (MAGIC_MWIToString (mwi));
		    Output (MAGIC_MWIToString (mwi), length, DoBlank);
		}

		for (
		    x = 0;
		    iByteCodeScanner.getUByte (x++) == VByteCodeDescriptor::ByteCode_Tag;
		    x++
		) if (TagIsPostMagicWord (iByteCodeScanner.getUByte (x)))
		{
		    ConvertPostMagicWordTag (tagString [0], iByteCodeScanner.getUByte (x));
		    Output (tagString, 1, DoBlank);
		}

		if (Ptr (&decompiledProgram) == Storage (&decompiledProgram) ||
		    *(Ptr (&decompiledProgram) - 1) != '!'
		) Output (colon, 1, DoNothing);

		//  Selector Tracking
		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		    *pDecompiledPCOffset = bufferIndex (&decompiledProgram);

		length = strlen (KS__ToString (ksi));
		Output (KS__ToString (ksi), length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_UnaryValue:
	case VByteCodeDescriptor::ByteCode_UnaryValueS:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		if (tracingDecompile)
		    IO_printf ("[magic]'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);
		if (mwi != (unsigned)VMagicWord_Current || (
			iByteCodeScanner.getUByte (0) == VByteCodeDescriptor::ByteCode_Tag &&
			TagIsPostMagicWord (iByteCodeScanner.getUByte  (1))
		    )
		)
		{
		    Output (magic, 1, DoNothing);
		    length = strlen (MAGIC_MWIToString (mwi));
		    Output (MAGIC_MWIToString (mwi), length, DoBlank);
		}
		for (
		    x = 0;
		    iByteCodeScanner.getUByte (x++) == VByteCodeDescriptor::ByteCode_Tag;
		    x++
		) if (TagIsPostMagicWord (iByteCodeScanner.getUByte (x)))
		{
		    ConvertPostMagicWordTag (tagString [0], iByteCodeScanner.getUByte (x));
		    Output (tagString, 1, DoBlank);
		}

		//  Selector Tracking
		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		    *pDecompiledPCOffset = bufferIndex (&decompiledProgram);

		selectorPtr = iByteCodeScanner.fetchedSelectorName ();
		length = strlen (selectorPtr);
		Output (selectorPtr, length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_UnaryIntension:
	case VByteCodeDescriptor::ByteCode_UnaryIntensionS:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		if (tracingDecompile)
		    IO_printf ("[magic]'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);
		if (mwi != (unsigned)VMagicWord_Current || (
			iByteCodeScanner.getUByte (0) == VByteCodeDescriptor::ByteCode_Tag &&
			TagIsPostMagicWord (iByteCodeScanner.getUByte (1))
		    )
		)
		{
		    Output (magic, 1, DoNothing);
		    length = strlen (MAGIC_MWIToString (mwi));
		    Output (MAGIC_MWIToString (mwi), length, DoBlank);
		}

		for (
		    x = 0;
		    iByteCodeScanner.getUByte (x++) == VByteCodeDescriptor::ByteCode_Tag;
		    x++
		) if (TagIsPostMagicWord (iByteCodeScanner.getUByte (x)))
		{
		    ConvertPostMagicWordTag (tagString [0], iByteCodeScanner.getUByte (x));
		    Output (tagString, 1, DoBlank);
		}

		if (Ptr (&decompiledProgram) == Storage (&decompiledProgram) ||
		    *(Ptr (&decompiledProgram) - 1) != '!'
		) Output (colon, 1, DoNothing);

		//  Selector Tracking
		if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		    *pDecompiledPCOffset = bufferIndex (&decompiledProgram);

		selectorPtr = iByteCodeScanner.fetchedSelectorName ();
		length = strlen (selectorPtr);
		Output (selectorPtr, length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_WrapupUnaryIntension:
	case VByteCodeDescriptor::ByteCode_WrapupUnaryValue:
	    //  Selector Tracking
	    if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		*pDecompiledPCOffset = bufferIndex (&decompiledProgram);

	    outputStkVal ();
	    DCStack_Pop ();
	    break;

	case VByteCodeDescriptor::ByteCode_StoreMagic:
	    {
		unsigned int mwi = iByteCodeScanner.fetchedLiteral ();
		if (tracingDecompile)
		    IO_printf ("[magic]'%s' (%d)\n", MAGIC_MWIToString (mwi), mwi);
		Output (magic, 1, DoNothing);
		length = strlen (MAGIC_MWIToString (mwi));
		Output (MAGIC_MWIToString (mwi), length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_StoreLexBinding:
	case VByteCodeDescriptor::ByteCode_StoreLexBindingS:
	    DCStack_Push (xByteCode, NilOf (char *), 0);
	    Output (blockOpen, 1, DoNothing);
	    IndentationIndex++;

	    /****  produce code for nested block (uses nestedBlockCPD) ****/
	    nestedBlockCPD = iByteCodeScanner.fetchedBlockCPD ();
	    reproduceSource (nestedBlockCPD, 0, 0);
	    nestedBlockCPD->release ();

	    Output (blockClose, 1, DoNothing);
	    break;

	case VByteCodeDescriptor::ByteCode_StoreInteger:
	    STD_sprintf (outputString, "%d", iByteCodeScanner.fetchedInteger ());
	    length = strlen (outputString);
	    Output (outputString, length, DoBlank);
	    break;

	case VByteCodeDescriptor::ByteCode_StoreDouble:
	    {
		double dp = iByteCodeScanner.fetchedDouble ();
		if (fabs (dp) > 1e20 || (fabs (dp) < 1e-20 && dp != 0.0))
		    STD_sprintf (outputString, "%.20g", dp);
		else
		    STD_sprintf (outputString, "%f", dp);
		length = strlen (outputString);
		Output (outputString, length, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_StoreString:
	case VByteCodeDescriptor::ByteCode_StoreStringS:
	    Output (quote, 1, DoNothing);
	    tptr = iByteCodeScanner.fetchedStringText ();
	    while (IsntNil (selectorPtr = strchr (tptr, '\"')))
	    {
		Output (tptr, selectorPtr - tptr, DoNothing);
		Output (backslash, 1, DoNothing);
		Output (quote, 1, DoNothing);
		tptr = selectorPtr + 1;
	    }

	    length = strlen (tptr);
	    Output (tptr, length, DoNothing);
	    Output (quote, 1, DoBlank);
	    break;

	case VByteCodeDescriptor::ByteCode_StoreSelector:
	case VByteCodeDescriptor::ByteCode_StoreSelectorS:
	    Output (selMark, 1, DoNothing);
	    selectorPtr = iByteCodeScanner.fetchedSelectorName ();
	    length = strlen (selectorPtr);
	    Output (selectorPtr, length, DoNothing);
	    Output (selMark, 1, DoBlank);
	    break;

	case VByteCodeDescriptor::ByteCode_StoreKnownSelector: {
		unsigned int ksi = iByteCodeScanner.fetchedSelectorIndex ();
		Output (selMark, 1, DoNothing);
		length = strlen	(KS__ToString (ksi));
		Output (KS__ToString (ksi), length, DoNothing);
		Output (selMark, 1, DoBlank);
	    }
	    break;

	case VByteCodeDescriptor::ByteCode_StoreNoValue:
	    /****	Output Nothing	****/
	    break;

	case VByteCodeDescriptor::ByteCode_Exit:
	    if (topOfStack == VByteCodeDescriptor::ByteCode_StoreLexBinding ||
		topOfStack == VByteCodeDescriptor::ByteCode_StoreLexBindingS
	    )
	    {
		outputStkVal ();
		DCStack_Pop ();
		IndentationIndex--;
	    }
	    if (pReferenceCPD && iByteCodeScanner.bcAddress () <= rtBLOCK_CPD_PC (pReferenceCPD))
		*pDecompiledPCOffset = bufferIndex (&decompiledProgram);
	    finished = true;
	    break;
	}
	if (tracingDecompile && pReferenceCPD)
	    IO_printf (" DecompiledPCOffset = %d\n", *pDecompiledPCOffset);
    } while (!finished);

    /*****	Remove trailing space if there is one	*****/
    if (Ptr (&decompiledProgram) > Storage(&decompiledProgram) && *(Ptr (&decompiledProgram) - 1) == ' ')
	Ptr (&decompiledProgram)--;

    if (pReferenceCPD && *pDecompiledPCOffset > bufferIndex (&decompiledProgram))
	*pDecompiledPCOffset = bufferIndex (&decompiledProgram);
}


/*---------------------------------------------------------------------------
 *****  Routine to convert a compiled program into a string.
 *
 *  Arguments:
 *	pBlockCPD		- a CPD for the block to decompile.
 *	pDecompiledPCOffset	- the optional address of an 'unsigned int'
 *				  which will be set to the character offset
 *				  of the program counter of 'pBlockCPD' in
 *				  its decompiled representation.
 *	xPCOffset		- the optional (UINT_MAX if absent) program
 *				  counter to be used in the computation of
 *				  *pDecompiledPCOffset
 *
 *  Returns:
 *	A CPD for a text string representing the de-compiled version of
 *	the program.
 *
 *****/
PublicFnDef void RSLANG_Decompile (
    VString		&rSourceReturn,
    M_CPD		*pBlockCPD,
    unsigned int	*pDecompiledPCOffset,
    unsigned int	 xPCOffset
) {
    M_CPD *pReferenceCPD;
    if (IsNil (pDecompiledPCOffset))
	pReferenceCPD = 0;
    else if (UINT_MAX == xPCOffset) 
	pReferenceCPD = pBlockCPD;
    else {
	pReferenceCPD = pBlockCPD->GetCPD (-1);
	rtBLOCK_CPD_PC (pReferenceCPD) = rtBLOCK_CPD_ByteCodeVector (pReferenceCPD) + xPCOffset;
    }

    pBlockCPD = pBlockCPD->GetCPD (-1);

    /*****  Initialize the string space, ...  *****/
    InitializeStringSpace (&decompiledProgram);

    /*****  Initialize the decompiler stack, ...  *****/
    DCStack_Initialize ();

    /*****  Interpret the byte codes, ...  *****/
    reproduceSource (pBlockCPD, pReferenceCPD, pDecompiledPCOffset);
    *Ptr (&decompiledProgram) = '\0';

    rSourceReturn.setTo (Storage (&decompiledProgram));

    /*****  Cleanup the string space, ...  *****/
    UTIL_Free (Storage (&decompiledProgram));

    /*****  Cleanup the decompiler stack, ... *****/
    UTIL_Free (RSLangDC_Stack.stack);

    /*****  Release the block and reference CPDs if copies were made, ...  *****/
    pBlockCPD->release ();
    if (pReferenceCPD && UINT_MAX > xPCOffset)
	pReferenceCPD->release ();
}


/**************************************************
 *****  Facility I-Object Method Definitions  *****
 **************************************************/

/*********************************
 *  Trace Switch Access Methods  *
 *********************************/

IOBJ_DefineUnaryMethod (TracesOn) {
    tracingNodeAlloc		=
    tracingNodeTypeChange	=
    tracingNodeAppend		=
    tracingNonTerminalCreation	=
    tracingTerminalCreation	= true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOff) {
    tracingNodeAlloc		=
    tracingNodeTypeChange	=
    tracingNodeAppend		=
    tracingNonTerminalCreation	=
    tracingTerminalCreation	= false;


    return self;
}

IOBJ_DefineNilaryMethod (NodeChangeTrace) {
    return IOBJ_SwitchIObject (&tracingNodeTypeChange);
}

IOBJ_DefineNilaryMethod (NodeAllocTrace) {
    return IOBJ_SwitchIObject (&tracingNodeAlloc);
}

IOBJ_DefineNilaryMethod (AppendTrace) {
    return IOBJ_SwitchIObject (&tracingNodeAppend);
}

IOBJ_DefineNilaryMethod (NonTerminalTrace) {
    return IOBJ_SwitchIObject (&tracingNonTerminalCreation);
}

IOBJ_DefineNilaryMethod (TerminalTrace) {
    return IOBJ_SwitchIObject (&tracingTerminalCreation);
}

IOBJ_DefineUnaryMethod (CompilerTracesOn) {
    tracingCompile		=
    tracingStringSpaceOps	=
    tracingCodeGeneration	= true;

    return self;
}

IOBJ_DefineUnaryMethod (CompilerTracesOff) {
    tracingCompile		=
    tracingStringSpaceOps	=
    tracingCodeGeneration	= false;

    return self;
}

IOBJ_DefineNilaryMethod (CompileTrace) {
    return IOBJ_SwitchIObject (&tracingCompile);
}

IOBJ_DefineNilaryMethod (StringSpaceTrace) {
    return IOBJ_SwitchIObject (&tracingStringSpaceOps);
}

IOBJ_DefineNilaryMethod (CodeGenerationTrace) {
    return IOBJ_SwitchIObject (&tracingCodeGeneration);
}

IOBJ_DefineNilaryMethod (SaveBlock) {
    return IOBJ_SwitchIObject (&retainingBlockForBrowsing);
}

IOBJ_DefineNilaryMethod (DCStackTrace) {
    return IOBJ_SwitchIObject (&tracingStackOps);
}

IOBJ_DefineNilaryMethod (DecompileTrace) {
    return IOBJ_SwitchIObject (&tracingDecompile);
}

IOBJ_DefineNilaryMethod (GetBlock) {
    return blockIObj;
}

IOBJ_DefineUnaryMethod (OneStepRecursivePrint) {
    RTYPE_RPrint (RTYPE_QRegisterCPD (blockIObj), -1);

    return self;
}

IOBJ_DefineUnaryMethod (OneStepPrint) {
    RTYPE_Print (RTYPE_QRegisterCPD (blockIObj), -1);

    return self;
}

IOBJ_DefineNilaryMethod (Decompile) {
    VString iSource;
    RSLANG_Decompile (iSource, RTYPE_QRegisterCPD (blockIObj));
    return RTYPE_QRegister (rtSTRING_New (iSource));
}

IOBJ_DefineNewaryMethod (GetSource) {
    VString iSource;
    RSLANG_Decompile (iSource, RTYPE_QRegisterCPD (parameterArray [0]));
    return RTYPE_QRegister (rtSTRING_New (iSource));
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("parseTracesOn"	, TracesOn)
	IOBJ_MDE ("parseTracesOff"	, TracesOff)
	IOBJ_MDE ("nodeChangeTrace"	, NodeChangeTrace)
	IOBJ_MDE ("nodeAllocTrace"	, NodeAllocTrace)
	IOBJ_MDE ("appendTrace"	    	, AppendTrace)
	IOBJ_MDE ("nonTerminalTrace"	, NonTerminalTrace)
	IOBJ_MDE ("terminalTrace"	, TerminalTrace)
	IOBJ_MDE ("compilerTracesOn"	, CompilerTracesOn)
	IOBJ_MDE ("compilerTracesOff"	, CompilerTracesOff)
	IOBJ_MDE ("compileTrace"	, CompileTrace)
	IOBJ_MDE ("stringSpaceTrace"	, StringSpaceTrace)
	IOBJ_MDE ("codeGenerationTrace"	, CodeGenerationTrace)
	IOBJ_MDE ("saveBlock"		, SaveBlock)
	IOBJ_MDE ("dcStackTrace"	, DCStackTrace)
	IOBJ_MDE ("decompileTrace"	, DecompileTrace)
	IOBJ_MDE ("getBlock"		, GetBlock)
	IOBJ_MDE ("P"			, OneStepPrint)
	IOBJ_MDE ("R"			, OneStepRecursivePrint)
	IOBJ_MDE ("getSource:"		, GetSource)
	IOBJ_MDE ("decompile"		, Decompile)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (RSLANG);
    FAC_FDFCase_FacilityDescription ("The Compiler/Decompiler");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    case FAC_ReturnFacilityMD:
	FAC_MDResultField = methodDictionary;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    case FAC_DoStartupInitialization:
        blockIObj = IOBJ_TheNilIObject;
	rslangout   = stderr;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rslang.c 1 replace /users/mjc/system
  860226 16:26:59 mjc create

 ************************************************************************/
/************************************************************************
  rslang.c 2 replace /users/mjc/system
  860317 18:12:38 mjc Replace parser to allow release of new lex/yacc make rules

 ************************************************************************/
/************************************************************************
  rslang.c 3 replace /users/jck
  860318 13:04:15 jck Yacc and lex names beginning with 'yy' changed to begin with 'rslang'

 ************************************************************************/
/************************************************************************
  rslang.c 4 replace /users/mjc/system
  860324 12:51:38 mjc Returned parser to library for jck\'s use

 ************************************************************************/
/************************************************************************
  rslang.c 6 replace /users/jck/source
  860418 00:03:14 jck Code Generator version release

 ************************************************************************/
/************************************************************************
  rslang.c 7 replace /users/jck/system
  860418 11:27:40 jck Bug with Binary operations fixed

 ************************************************************************/
/************************************************************************
  rslang.c 8 replace /users/jck/system
  860424 11:11:59 jck Miscellaneous bugs fixed

 ************************************************************************/
/************************************************************************
  rslang.c 11 replace /users/jck/system
  860429 10:28:22 jck Decompiler completed except for 'pretty printing'

 ************************************************************************/
/************************************************************************
  rslang.c 12 replace /users/jck/system
  860430 17:42:12 jck Problems with parse tree allocation fixed

 ************************************************************************/
/************************************************************************
  rslang.c 13 replace /users/jad/system
  860509 15:11:56 jad fixed the recursive bug

 ************************************************************************/
/************************************************************************
  rslang.c 14 replace /users/jck/system
  860522 12:09:26 jck Incorporated Method Dictionaries

 ************************************************************************/
/************************************************************************
  rslang.c 15 replace /users/jck/system
  860523 15:12:41 jck Use of rtVECTOR_USVD_Type parameter revised

 ************************************************************************/
/************************************************************************
  rslang.c 16 replace /users/mjc/system
  860527 18:00:08 mjc Changed 'rtBLOCK_CPD_PCCounter' to 'rtBLOCK_CPD_PC'

 ************************************************************************/
/************************************************************************
  rslang.c 17 replace /users/mjc/system
  860531 10:42:57 mjc Changed 'RSLANG_Compile' to pick up its method dictionary from 'envir'

 ************************************************************************/
/************************************************************************
  rslang.c 18 replace /users/mjc/system
  860531 17:57:15 mjc Release new known CPD names

 ************************************************************************/
/************************************************************************
  rslang.c 19 replace /users/spe/system
  860612 16:19:26 spe Implemented NOP bytecode in GenerateCode and reproduceSource.

 ************************************************************************/
/************************************************************************
  rslang.c 20 replace /users/spe/system
  860613 09:33:37 spe Corrected _EmptyExpression case in generateSource.

 ************************************************************************/
/************************************************************************
  rslang.c 21 replace /users/spe/system
  860613 11:35:01 spe Added SetPrecedingSemi_Colon macro and replaced trailingSemi_Colon bit with precedingSemi_Colon bit in parse tree node.

 ************************************************************************/
/************************************************************************
  rslang.c 23 replace /users/spe/system
  860624 16:57:59 spe Modified buildProgram so selector is installed into the block.

 ************************************************************************/
/************************************************************************
  rslang.c 24 replace /users/spe/system
  860703 11:37:40 spe Modified acquire parameter bytecode operations and removed all UNWIND_LongJumpTrap calls.

 ************************************************************************/
/************************************************************************
  rslang.c 25 replace /users/mjc/maketest
  860708 12:46:16 mjc Deleted references to 'RTselb'

 ************************************************************************/
/************************************************************************
  rslang.c 26 replace /users/mjc/rsystem
  860717 21:05:07 mjc Make 'ENVIR_KCPD_TheDefaultProp' the default property

 ************************************************************************/
/************************************************************************
  rslang.c 27 replace /users/jck/system
  860804 16:42:54 jck Constant 'MAX_BYTE_SIZE' changed from 256 to 255.
 bug fixed in handling of environment table

 ************************************************************************/
/************************************************************************
  rslang.c 28 replace /users/jck/system
  860805 16:43:45 jck Fixed bug related to magic word optimizations
		Fixed bug in decompiler related to known unary messages

 ************************************************************************/
/************************************************************************
  rslang.c 29 replace /users/jck/system
  860806 13:53:35 jck Corrected some decompiler problems

 ************************************************************************/
/************************************************************************
  rslang.c 30 replace /users/jck/system
  860807 16:52:02 jck  Fixed bug in code generated when magic word
 	was recipient of keyword or binary message

 ************************************************************************/
/************************************************************************
  rslang.c 31 replace /users/jck/system
  860811 11:51:04 jck Changed _EmptyExpression to be a terminal node
	in the parse tree

 ************************************************************************/
/************************************************************************
  rslang.c 32 replace /users/jck/system
  860812 07:40:00 jck Fixed bug occurring when magic word was the
	parameter of a keyword message

 ************************************************************************/
/************************************************************************
  rslang.c 33 replace /users/mjc/system
  861002 19:15:42 mjc Release of changes in support of list architecture

 ************************************************************************/
/************************************************************************
  rslang.c 34 replace /usr/rs/versions/5
  861107 17:23:12 mjc Fixed bug caused by randomly initialized value of 'IntensionalOp' flag

 ************************************************************************/
/************************************************************************
  rslang.c 35 replace /users/jck/system
  861110 12:19:08 jck Added some error recognition to GenerateCode and
fixed 'localId generation' to take advantage of Known selectors

 ************************************************************************/
/************************************************************************
  rslang.c 36 replace /users/mjc/system
  861203 13:11:13 mjc Implemented a more reasonable error reporting mechanism

 ************************************************************************/
/************************************************************************
  rslang.c 37 replace /users/mjc/system
  861224 11:28:47 mjc Add form feed to whitespace set, add escaped characters to selectors

 ************************************************************************/
/************************************************************************
  rslang.c 38 replace /users/mjc/translation
  870524 09:41:03 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rslang.c 39 replace /users/mjc/system
  870527 12:51:18 mjc Moved initialization of lex 'output' file to module initialization (VAX portability)

 ************************************************************************/
/************************************************************************
  rslang.c 40 replace /users/mjc/system
  870607 03:14:41 mjc Begin conversion to 'stdoss' utilities

 ************************************************************************/
/************************************************************************
  rslang.c 41 replace /users/mjc/system
  870607 13:11:14 mjc Changed definition of 'rslanglineno' from 'PublicVarDef int' to 'int' to deal with VAX C 'extern' brain damage

 ************************************************************************/
/************************************************************************
  rslang.c 42 replace /users/mjc/system
  870607 22:07:54 mjc Corrected improper correction of 'rslang.c' declaration of 'rslanglineno'

 ************************************************************************/
/************************************************************************
  rslang.c 43 replace /users/mjc/system
  870614 20:15:23 mjc Altered 'twalk' action routines to access node contents indirectly

 ************************************************************************/
/************************************************************************
  rslang.c 44 replace /users/jck/system
  880114 16:06:17 jck Replaced calls to strncat with calls to memcpy

 ************************************************************************/
/************************************************************************
  rslang.c 45 replace /users/mjc/Workbench/system
  880210 15:12:50 mjc Moved recognition of negative numbers from lexical to syntactic analysis

 ************************************************************************/
/************************************************************************
  rslang.c 46 replace /users/jck/system
  880328 10:52:55 jck Fixed some decompiler bugs

 ************************************************************************/
/************************************************************************
  rslang.c 47 replace /users/jck/system
  880708 12:15:04 jck Fixed another decompiler bug

 ************************************************************************/
/************************************************************************
  rslang.c 48 replace /users/jck/system
  881011 14:05:12 jck Made the handling of large vision inputs more robust

 ************************************************************************/
/************************************************************************
  rslang.c 49 replace /users/jck/system
  881018 07:54:30 jck Fixed pointer problem in decompiler's 'getSelector' routine.

 ************************************************************************/
/************************************************************************
  rslang.c 50 replace /users/m2/backend
  890503 15:29:05 m2 Changed printf() to IO_printf(), STD_printf() changes

 ************************************************************************/

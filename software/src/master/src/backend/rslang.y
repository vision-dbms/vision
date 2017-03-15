
/*****  Sentence Symbol  *****/
%start Sentence

/*****  Terminal Symbols  *****/
%token	TS_Error
%token	TS_FinalComment			/*  Any last words...  */
%token	TS_Number
%token	TS_String			/*  "xyz\"abc"  */
%token	TS_LeftAssignmentOp		/*   <-  */
%token	TS_IntensionalLeftAssignmentOp	/*  :<-  */
%token	TS_RightAssignmentOp		/*   ->  */
%token	TS_IntensionalRightAssignmentOp	/*  :->  */
%token	TS_ConcatOp			/*  ,  */
%token	TS_IntensionalConcatOp		/*  :,  */
%token	TS_Keyword			/*   xyz:  */
%token	TS_IntensionalKeyword		/*  :xyz:  */
%token	TS_RelationalOp			/*   <   <=   >=   >   =   <>  */
%token	TS_IntensionalRelationalOp	/*  :<  :<=  :>=  :>  :=  :<>  */
%token	TS_DisjunctiveOp		/*   ||  */
%token	TS_IntensionalDisjunctiveOp	/*  :||  */
%token	TS_ConjunctiveOp		/*   &&  */
%token	TS_IntensionalConjunctiveOp	/*  :&&  */
%token	TS_AdditiveOp			/*   +   -  */
%token	TS_IntensionalAdditiveOp	/*  :+  :-  */
%token	TS_MultiplicativeOp		/*   *   /  */
%token	TS_IntensionalMultiplicativeOp	/*  :*  :/  */
%token	TS_Identifier			/*   xyz  */
%token	TS_IntensionalIdentifier	/*  :xyz  */

/*****  Non-Terminal Symbols  *****/

/*****  Precedence Declarations  *****/
%right	TS_LeftAssignmentOp TS_IntensionalLeftAssignmentOp
%left	TS_RightAssignmentOp TS_IntensionalRightAssignmentOp
%left	TS_Keyword TS_IntensionalKeyword
%left	TS_ConcatOp TS_IntensionalConcatOp
%left	TS_DisjunctiveOp TS_IntensionalDisjunctiveOp
%left	TS_ConjunctiveOp TS_IntensionalConjunctiveOp
%left	TS_RelationalOp TS_IntensionalRelationalOp
%left	TS_AdditiveOp TS_IntensionalAdditiveOp
%left	TS_MultiplicativeOp TS_IntensionalMultiplicativeOp


%%
/*******************
 *****  Rules  *****
 *******************/

Sentence		: ExpressionList
			    { rsROOT = $1; }	/*  Return parse tree  */
			| ExpressionList TS_FinalComment
			    {Extend ($$, $1, $2); rsROOT = $1;}	/*  Return parse tree  */
			;
 
ExpressionList		: ExpressionListTerm
			    {Root ($$, $1);}
			| ExpressionList ';' ExpressionListTerm
			    {SetPrecedingSemi_Colon ($3);
			     Extend ($$, $1, $3);
			     }	/*  Append to expression list  */
			;

ExpressionListTerm	:	/*  EMPTY  */
			    { Empty ($$);}	/*  Generate node for an empty expression  */
			| Expression
			;

Expression		: UnaryExpression

			| Expression TS_LeftAssignmentOp Expression
			    {BinaryOp($$,$1,$2,$3);}

			| Expression TS_RightAssignmentOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalLeftAssignmentOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalRightAssignmentOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_ConcatOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalConcatOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_Keyword Expression
			    {KeywordTerm ($$, $1, $2, $3);}

			| Expression TS_IntensionalKeyword Expression
			    {KeywordTerm ($$, $1, $2, $3);}

			| Expression TS_RelationalOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalRelationalOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_DisjunctiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalDisjunctiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_ConjunctiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalConjunctiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_AdditiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalAdditiveOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_MultiplicativeOp Expression
			    {BinaryOp ($$, $1, $2, $3);}

			| Expression TS_IntensionalMultiplicativeOp Expression
			    {BinaryOp ($$, $1, $2, $3);}
			;

UnaryExpression		: Primary
			| UnaryOperator
			| UnaryExpression UnaryOperator
			    {UnaryOp ($$, $1, $2);}	/*  Explicit Unary Expression  */
			;

UnaryOperator		: TS_Identifier
			| TS_IntensionalIdentifier
			;

Primary			: SimplePrimary
			| Expression TS_Keyword Expression '.'
			    {ClosingKeywordTerm ($$, $1, $2, $3);}
			| Expression TS_IntensionalKeyword Expression '.'
			    {ClosingKeywordTerm ($$, $1, $2, $3);}
			;

SimplePrimary		: LocalIdentifier
			| MagicWord
			| Block
			| Selector
			| TS_String
			| TS_Number
			| TS_AdditiveOp TS_Number
			    {
				$$ =
				    (TokenTypes)NodeType ($1) != _Minus
				    ? $2
				    : (TokenTypes)NodeType ($2) == _Integer
				    ? pTreeChangeNodeType ($2, _NegInteger)
				    : (TokenTypes)NodeType ($2) == _HexInteger
				    ? pTreeChangeNodeType ($2, _NegHexInteger)
				    : pTreeChangeNodeType ($2, _NegReal);
			    }
			| '(' Expression ')' 
			    { CloseParens ($$, $2);}
			;

LocalIdentifier		: '!' TS_Identifier
			    {$$ = pTreeChangeNodeType ($2, _LocalId);}
			;

MagicWord		: '^' TS_Identifier
			    {
				char magic[128];
				int length = TerminalNode_Length ($2);
				
				memcpy
				    (magic,
				     sourceBASE + TerminalNode_Origin ($2),
				     length);
				magic[length] = '\0';
				if (MAGIC_StringToMWI (magic) < 0)
				{
				    yyerror
				    (
					UTIL_FormatMessage
					(
					   "Unrecognized magic word '^%.*s'",
					   length, 
					   sourceBASE + TerminalNode_Origin($2)
					)
				    );
				    YYERROR;
				}
				else
				{
				    $$ = pTreeChangeNodeType ($2, _Magic);
				}
			    }
			;

Block			: '[' ExpressionList ']'
			    { Percolate ($$, (int)_SimpleBlock, $2);}
			| '[' BlockHeader '|' ExpressionList ']'
			    { HeadedBlock ($$, $2, $4); }
			;

BlockHeader		: ParameterList
			| MessageTemplate
			;

MessageTemplate		: UnaryMessageTemplate
			| BinaryMessageTemplate
			| KeywordMessageTemplate
			;

UnaryMessageTemplate	: '|' TS_Identifier
			    { Percolate ($$, (int)_UnaryMessageTemplate, $2); }
			;

BinaryMessageTemplate	: BinaryMessageHeader TS_Identifier
			    { BinaryMessage ($$, $1, $2); }
			;

BinaryMessageHeader	: '|' TS_LeftAssignmentOp
			    { $$ = $2; }
			| '|' TS_RightAssignmentOp
			    { $$ = $2; }
			| '|' TS_ConcatOp
			    { $$ = $2; }
			| '|' TS_RelationalOp
			    { $$ = $2; }
			| '|' TS_DisjunctiveOp
			    { $$ = $2; }
			| '|' TS_ConjunctiveOp
			    { $$ = $2; }
			| '|' TS_AdditiveOp
			    { $$ = $2; }
			| '|' TS_MultiplicativeOp
			    { $$ = $2; }
			;

KeywordMessageTemplate	: KeywordMessageHeader TS_Identifier
			    { Extend ($$, $1, $2); }
			;

KeywordMessageHeader	: '|' TS_Keyword
			    { Percolate ($$, 
					 (int)_KeywordMessageTemplate, 
					 $2); 
			    }
			| KeywordMessageTemplate TS_Keyword
			    { Extend ($$, $1, $2); }
			;

ParameterList		: '|' TS_IntensionalIdentifier
			    { Percolate ($$, 
					 (int)_ParametricMessageTemplate, 
					 $2); 
			    }
			| ParameterList TS_IntensionalIdentifier
			    { Extend ($$, $1, $2); }
			;

Selector		: '\'' UnarySelector '\''
			    { $$ = $2; }
			| '\'' BinarySelector '\''
			    { $$ = $2; }
			| '\'' KeywordSelector '\''
			    { $$ = $2; }
			;

UnarySelector		: TS_Identifier
			    { Percolate($$, _UnarySelector, $1); }
			;

BinarySelector		: TS_LeftAssignmentOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_RightAssignmentOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_ConcatOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_RelationalOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_DisjunctiveOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_ConjunctiveOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_AdditiveOp
			    { Percolate($$, _BinarySelector, $1); }
			| TS_MultiplicativeOp
			    { Percolate($$, _BinarySelector, $1); }
			;

KeywordSelector		: TS_Keyword
			    { Percolate($$, _KeywordSelector, $1); }
			| KeywordSelector TS_Keyword
			    { Extend ($$, $1, $2); }
			;

%%

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rslang.y 1 replace /users/mjc/system
  860226 16:27:04 mjc create

 ************************************************************************/
/************************************************************************
  rslang.y 2 replace /users/mjc/system
  860317 18:12:45 mjc Replace parser to allow release of new lex/yacc make rules

 ************************************************************************/
/************************************************************************
  rslang.y 3 replace /users/mjc/system
  860324 12:51:41 mjc Returned parser to library for jck\'s use

 ************************************************************************/
/************************************************************************
  rslang.y 4 replace /users/jck/system
  860404 17:29:25 jck Partially completed release

 ************************************************************************/
/************************************************************************
  rslang.y 5 replace /users/jck/source
  860418 00:03:20 jck Code Generator version release

 ************************************************************************/
/************************************************************************
  rslang.y 6 replace /users/mjc/system
  860424 10:26:59 mjc Changed '.' closed keyword expressions to reduce to a primary

 ************************************************************************/
/************************************************************************
  rslang.y 7 replace /users/jck/system
  860429 10:27:37 jck Adjusted closeExp to save parenthesis and semi_colon/ information for decompiling purposes

 ************************************************************************/
/************************************************************************
  rslang.y 8 replace /users/jck/system
  860430 17:42:16 jck Problems with parse tree allocation fixed

 ************************************************************************/
 /************************************************************************
  rslang.y 9 replace /users/spe/system
  860613 11:03:58 spe Added SetPrecedingSemi_Colon statement in ExpressionList actions.

 ************************************************************************/
/************************************************************************
  rslang.y 10 replace /users/mjc/system
  860826 08:57:35 mjc Gave ',' higher precedence than keywords

 ************************************************************************/
/************************************************************************
  rslang.y 11 replace /users/mjc/system
  861002 18:05:00 mjc Correct omission of ',' from block header syntax

 ************************************************************************/
/************************************************************************
  rslang.y 12 replace /users/mjc/system
  861203 12:53:29 mjc Fixed 'magic' word action rule to report bad magic words as syntax errors

 ************************************************************************/
/************************************************************************
  rslang.y 13 replace /users/mjc/translation
  870524 09:43:19 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rslang.y 14 replace /users/mjc/system
  870701 14:45:50 mjc Added 'TS_ConcatOp' to list of binary selectors

 ************************************************************************/
/************************************************************************
  rslang.y 15 replace /users/jck/system
  880114 16:08:38 jck Replaced calls to strncat with calls to memcpy

 ************************************************************************/
/************************************************************************
  rslang.y 16 replace /users/jck/system
  880120 21:12:19 jck Added omitted action for TS_MultiplicativeOp in
BinaryMessageHeader

 ************************************************************************/
/************************************************************************
  rslang.y 17 replace /users/mjc/Workbench/system
  880210 15:19:27 mjc Moved recognition of negative numbers from lexical to syntactic analysis

 ************************************************************************/

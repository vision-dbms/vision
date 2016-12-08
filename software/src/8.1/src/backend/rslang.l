/*****  Research System Language Lexical Analyzer  *****/

%{

/*---------------------------------------------------------------------------
 * The following redefintion of yyLMAX is an attempt to avoid a hard limit on
 * the size of a token. Note that it moves the limit well beyond the range of
 * a reasonable size for a token, but that a hard limit still exists. A
 * programmer who attempts to compile a vision program with an identifier,
 * say, of more than yyLMAX characters will find a segmentation fault to be
 * the likely result.  Short of writing our own lexical analyzer, this limit
 * cannot be removed, only enlarged (which has a cost in memory).
 *---------------------------------------------------------------------------
 */

#undef yyLMAX
#define yyLMAX 1024

/*---------------------------------------------------------------------------
 * The redefinitions of input and unput effectively remove a hard limit on the
 * length of an input line which is imposed by lex
 *---------------------------------------------------------------------------
 */

#undef input
#define input() ((yytchar = rsSOURCE [CurrentScanPointer++]) == '\n'\
		 ? yylineno++, yytchar : yytchar)

#undef unput
#define unput(c)\
{\
    (c);\
    if (rsSOURCE [--CurrentScanPointer] == '\n')\
	 rslanglineno--;\
}


%}
 /*****************************************
  *****  Character Class Definitions  *****
  *****************************************/

ALPHA		([a-zA-Z_]|\\.)
DIGIT		[0-9]
HEXDIGIT	[0-9A-Fa-f]
ALPHANUM	({ALPHA}|{DIGIT})
EXPONENT	[Ee][-+]?{DIGIT}+

 /*******************
  *****  Rules  *****
  *******************/
%%


":"				{
	/*****  Intensional Message Qualifier  *****/
	if (IntensionalOp)
	{
	    ReturnNode (_Error, TS_Error);
	}
	IntensionalOp = true;
    }

{ALPHA}{ALPHANUM}*		{
	/*****  Identifier  *****/
	ReturnSelector
	    (_Identifier,
	     _IntensionalIdentifier,
	     TS_Identifier,
	     TS_IntensionalIdentifier);
    }

{ALPHA}{ALPHANUM}*:		{
	/*****  Keyword  *****/
	ReturnSelector
	    (_Keyword,
	     _IntensionalKeyword,
	     TS_Keyword,
	     TS_IntensionalKeyword);
    }

"<-"				{
	ReturnSelector
	    (_LeftAssignment,
	     _IntensionalLeftAssignment,
	     TS_LeftAssignmentOp,
	     TS_IntensionalLeftAssignmentOp);
    }

"->"				{

	ReturnSelector
	    (_RightAssignment,
	     _IntensionalRightAssignment,
	     TS_RightAssignmentOp,
	     TS_IntensionalRightAssignmentOp);
    }

","				{
	ReturnSelector
	    (_ConcatOp,
	     _IntensionalConcatOp,
	     TS_ConcatOp,
	     TS_IntensionalConcatOp);
    }

"<"				{
	ReturnSelector
	    (_LessThanP,
	     _IntensionalLessThanP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"<="				{
	ReturnSelector
	    (_LessThanOrEqualP,
	     _IntensionalLessThanOrEqualP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

">="				{
	ReturnSelector
	    (_GreaterThanOrEqualP,
	     _IntensionalGreaterThanOrEqualP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

">"				{
	ReturnSelector
	    (_GreaterThanP,
	     _IntensionalGreaterThanP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"="				{
	ReturnSelector
	    (_EqualP,
	     _IntensionalEqualP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"=="				{
	ReturnSelector
	    (_IdenticalP,
	     _IntensionalIdenticalP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"!="				{
	ReturnSelector
	    (_NotEqualP,
	     _IntensionalNotEqualP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"!=="				{
	ReturnSelector
	    (_NotIdenticalP,
	     _IntensionalNotIdenticalP,
	     TS_RelationalOp,
	     TS_IntensionalRelationalOp);
    }

"||"				{
	ReturnSelector
	    (_Or,
	     _IntensionalOr,
	     TS_DisjunctiveOp,
	     TS_IntensionalDisjunctiveOp);
    }

"&&"				{
	ReturnSelector
	    (_And,
	     _IntensionalAnd,
	     TS_ConjunctiveOp,
	     TS_IntensionalConjunctiveOp);
    }

"+"				{
	ReturnSelector
	    (_Plus,
	     _IntensionalPlus,
	     TS_AdditiveOp,
	     TS_IntensionalAdditiveOp);
    }

"-"				{
	ReturnSelector
	    (_Minus,
	     _IntensionalMinus,
	     TS_AdditiveOp,
	     TS_IntensionalAdditiveOp);
    }

"*"				{
	ReturnSelector
	    (_Times,
	     _IntensionalTimes,
	     TS_MultiplicativeOp,
	     TS_IntensionalMultiplicativeOp);
    }

"/"				{
	ReturnSelector
	    (_Divide,
	     _IntensionalDivide,
	     TS_MultiplicativeOp,
	     TS_IntensionalMultiplicativeOp);
    }

{DIGIT}+			{
	/*****  Integers  *****/
	if (IntensionalOp)
	{
	    ReturnNode (_Error, TS_Error);
	}
	ReturnNode (_Integer, TS_Number);
    }

"0x"{HEXDIGIT}+			{
	/*****  Hexadecimal Integers  *****/
	if (IntensionalOp)
	{
	    ReturnNode (_Error, TS_Error);
	}
	ReturnNode (_HexInteger, TS_Number);
    }

{DIGIT}+"."{DIGIT}*({EXPONENT})?	|
{DIGIT}*"."{DIGIT}+({EXPONENT})?	|
{DIGIT}+{EXPONENT}			{
	/*****  Reals  *****/
	if (IntensionalOp)
	{
	    ReturnNode (_Error, TS_Error);
	}
	ReturnNode (_Real, TS_Number);
    }

\"			{
	/*****  Strings *****/
	yyleng = CurrentScanPointer - 1;

	for (;;)
	{
	    	while (input () != '\"' &&
		       rsSOURCE [CurrentScanPointer - 1] != '\0');

		if (rsSOURCE [CurrentScanPointer - 1] == '\0') 
		{
		    yyerror ("End of File encountered in String");
		    ReturnNode (_Error, TS_Error);
		}
		if (rsSOURCE [CurrentScanPointer - 2] != '\\') break;
	}
	

	yyleng = CurrentScanPointer - yyleng;
	

	    if (IntensionalOp)
	    {
		ReturnNode (_Error, TS_Error);
	    }
    	    ReturnNode (_String, TS_String);

    }

#			{
	/*****  Comments *****/

        /*****
	 *    Eat up the characters explicitly. 
         *    Don't let lex accumulate as a token since comments 
         *    can become too big (greater than yyLMAX) 
         *****/
	yyleng = CurrentScanPointer - 1;
    	while (input () != '\n' &&
		       rsSOURCE [CurrentScanPointer - 1] != '\0');
	yyleng = CurrentScanPointer - yyleng;
	
    }

[^ \t\n\14\15]			{
	/*****  Special Symbols  *****/
	if (IntensionalOp)
	{
	    ReturnNode (_Error, TS_Error);
	}
	return yytext[0];
    }

[ \t\n\14\15]+			{
	/******  Whitespace  *****/
    }
%%
/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rslang.l 1 replace /users/mjc/system
  860226 16:27:02 mjc create

 ************************************************************************/
/************************************************************************
  rslang.l 2 replace /users/mjc/system
  860226 17:27:31 mjc Added %% to terminate rules section

 ************************************************************************/
/************************************************************************
  rslang.l 3 replace /users/mjc/system
  860317 18:12:42 mjc Replace parser to allow release of new lex/yacc make rules

 ************************************************************************/
/************************************************************************
  rslang.l 4 replace /users/jck/system
  860404 17:29:22 jck Partially completed release

 ************************************************************************/
/************************************************************************
  rslang.l 5 replace /users/jck/source
  860418 00:03:17 jck Code Generator version release

 ************************************************************************/
/************************************************************************
  rslang.l 6 replace /users/jck/system
  860430 17:44:01 jck Customized string recognizer to allow arbitrarily
long strings -- circumventing use of yytext buffer

 ************************************************************************/
/************************************************************************
  rslang.l 7 replace /users/mjc/system
  861224 11:28:43 mjc Add form feed to whitespace set, add escaped characters to selectors

 ************************************************************************/
/************************************************************************
  rslang.l 8 replace /users/mjc/translation
  870524 09:43:15 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rslang.l 9 replace /users/mjc/Workbench/system
  880210 15:17:34 mjc Moved recognition of negative numbers from lexical to syntactic analysis

 ************************************************************************/
/************************************************************************
  rslang.l 10 replace /users/jck/system
  881011 14:10:10 jck Made the handling of large vision inputs more robust

 ************************************************************************/

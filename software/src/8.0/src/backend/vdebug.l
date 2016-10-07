/*****  Debugger Language Lexical Analyzer  *****/

%{

/** Make the lex string buffer yytext larger **/
#undef	    YYLMAX
#define	    YYLMAX 512

#undef	    input
#define	    input() (\
    (\
	debugtchar = debugsptr > debugsbuf ? U(*--debugsptr) : NextCharacter ()\
    )==10\
    ? (debuglineno++, debugtchar)\
    : debugtchar\
)

%}
 /*****************************************
  *****  Character Class Definitions  *****
  *****************************************/
SIGN			[-+]?
DIGIT			[0-9]
EXPONENT		[Ee]{SIGN}{DIGIT}+
ALPHA			[a-zA-Z_]
ALPHANUM		[a-zA-Z_0-9]
%%

{ALPHA}{ALPHANUM}*			{
					    strncpy (
						yylval.name, (char *)yytext, 47
					    );
					    return (IDENTIFIER);
					}

{SIGN}{DIGIT}+				{
					    yylval.iobject = IOBJ_IntIObject (
						atoi ((char *)yytext)
					    );
					    return (NUMBER);
					}

{SIGN}{DIGIT}+"."{DIGIT}*({EXPONENT})?	|
{SIGN}{DIGIT}*"."{DIGIT}+({EXPONENT})?	|
{SIGN}{DIGIT}+{EXPONENT}		{
					    yylval.iobject = IOBJ_DoubleIObject (
						atof ((char *)yytext)
					    );
					    return (NUMBER);
					}

":"{ALPHANUM}+				{
					    strncpy (
						yylval.name, (char *)yytext, 47
					    );
					    return (UNARYSELECTOR);
					}

{ALPHANUM}+":"				{
					    strncpy (yylval.name, (char *)yytext, 47);
					    return (KEYWORD);
					}

"<-"					{
					    return (ASSIGNMENT_OP);
					}

\"[^"]*					{
					    if (yytext[yyleng - 1] == '\\')
					    {
						yytext[--yyleng] = '\0';
						yymore ();
					    }
					    else
					    {
						input ();

						yylval.iobject=RTYPE_QRegister(
						    rtSTRING_New (
							(char *)(yytext + 1)
						    )
						);
						return STRING;
					    }
					}

"$"(.|\n)				{
					    yylval.iobject =
						IOBJ_CharIObject (yytext[1]);
					    return CHARACTER;
					}

[(),;#]					{
					    return yytext[0];
					}

[ \t\n\14\15]				;

.					{
					    return (ERRORTOKEN);
					}

%%

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  debug.l 1 replace /users/jck/system
  860318 12:54:14 jck The lex input for the debugger language

 ************************************************************************/
/************************************************************************
  debug.l 2 replace /users/jck/system
  860318 13:40:59 jck Syntax fixed so that history comments don't cause lex to bomb

 ************************************************************************/
/************************************************************************
  debug.l 3 replace /users/mjc/system
  860328 12:52:37 mjc Added 'expression . expression' and empty expression syntax

 ************************************************************************/
/************************************************************************
  debug.l 4 replace /users/mjc/system
  860412 12:32:58 mjc Added string syntax and objects

 ************************************************************************/
/************************************************************************
  debug.l 5 replace /users/jck/source
  860418 00:02:04 jck Expression separator replaced with comma
Continue prompt removed, YACC bug fixed (ExpressionList type),
hooks added for editor

 ************************************************************************/
/************************************************************************
  debug.l 6 replace /users/jad/system
  860425 12:08:51 jad changed 'input' to call the prompting input routine

 ************************************************************************/
/************************************************************************
  debug.l 7 replace /users/mjc/system
  860508 18:15:44 mjc Added 'character' syntax

 ************************************************************************/
/************************************************************************
  debug.l 8 replace /users/ees/system
  860528 18:13:27 ees modified call to input routine: ees 5-28-86

 ************************************************************************/
/************************************************************************
  debug.l 9 replace /users/jad/system
  860909 15:05:46 jad make the lex string buffer larger

 ************************************************************************/
/************************************************************************
  debug.l 10 replace /users/mjc/translation
  870524 09:43:06 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  debug.l 11 replace /users/m2/backend
  890503 15:03:55 m2 Fix for preprocessor

 ************************************************************************/

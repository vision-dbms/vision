%{

PrivateVarDef char selector [512], temp_string [512];
PrivateVarDef IOBJ_IObject *parameters;
PrivateVarDef IdTableEntry idtab, *idtabp;
PrivateVarDef LTEntry ltk, *ltp;

double atof ();

%}

%start expressionList

%union	{
	int ival;
	char name [48];
	IOBJ_IObject iobject;
	}

%token	ERRORTOKEN
%token	<iobject> NUMBER
%token	<iobject> STRING
%token	<iobject> CHARACTER
%token	<name> IDENTIFIER
%token	<name> UNARYSELECTOR
%token	<name> KEYWORD
%token	ASSIGNMENT_OP

%type <iobject> expressionList
%type <iobject> expression
%type <ival> assignmentList
%type <iobject> valueGenerator
%type <iobject> simpleObject
%type <iobject> primary
%type <iobject> unaryExpression
%type <iobject> keywordExpression
%type <ival> keywordTermList
%type <iobject> cascadedExpression
%type <iobject> cascadedExpressionRoot
%type <ival> cascadedKeywordTermList

%%

expressionList		:
			    { $$ = IOBJ_TheNilIObject;}
			| expression
			| expressionList ','
			| expressionList ',' expression
			;

expression		: assignmentList valueGenerator
			    {
				while ($1--)
				{
				    
				    strcpy (idtab.name, PopString ());
				    idtabp = (IdTableEntry *)bsearch (
					(char *)&idtab,
					(char *)IdTable, 
					IdsInUse, 
					sizeof (IdTableEntry), 
					(VkComparator)CompareIds
				    );
				    idtabp->value = $2;
				    
				}
				
				$$ = $2; 
			        /*** printf ("expression\n"); ***/    
			    }
			    
			|  valueGenerator
			    {	
				$$ = $1;
				/*** printf ("expression\n"); ***/    
			    }
			;

assignmentList		: assignment
			    {
				$$ = 1;
			        /*** printf ("assignmentList : assignment\n"); ***/
			    }

			|  assignmentList assignment
			    {
				$$ = $1 + 1;
			        /*** printf ("assignmentList :assignmentList assignment\n"); ***/
			    }
			;

assignment		: IDENTIFIER ASSIGNMENT_OP
			    {
				    PushString ($1);
				    strcpy (idtab.name, $1);
				    idtabp = (IdTableEntry *)bsearch (
					(char *)&idtab,
					(char *)IdTable, 
					IdsInUse, 
					sizeof (IdTableEntry), 
					(VkComparator)CompareIds
				    );
				if (IsNil (idtabp))
				{
				    AddToIdTable ($1, IOBJ_TheNilIObject );
				}
				
			        /*** printf ("assignment\n"); ***/
			    }
			;

valueGenerator		: simpleObject
			    {
				$$ = $1;
			        /*** printf ("valueGenerator\n"); ***/
			    }
			|  keywordExpression
			    {
				$$ = $1;
			        /*** printf ("valueGenerator\n"); ***/
			    }
			|  cascadedExpression
			    {
				$$ = $1;
			        /*** printf ("valueGenerator\n"); ***/
			    }
			;

simpleObject		: primary
			    {
				$$ = $1;
			        /*** printf ("simpleObject\n"); ***/
			    }
			|  unaryExpression
			    {
				$$ = $1;
			        /*** printf ("simpleObject\n"); ***/
			    }
			;

primary			: IDENTIFIER
			    {
				    strcpy (idtab.name, $1);
				    idtabp = (IdTableEntry *)bsearch (
					(char *)&idtab,
					(char *)IdTable, 
					IdsInUse, 
					sizeof (IdTableEntry), 
					(VkComparator)CompareIds
				    );
				if (IsNil (idtabp)){
				    IO_fprintf
				        (stderr,
					 "** Undefined Identifier: %s\n",
					 idtab.name);
				    return 1;
				    }
				else $$ = idtabp->value;
				
 
			        /*** printf ("primary\n"); ***/
			    }
			|  '#' IDENTIFIER
			    {
				    ltk.name = $2;
				    ltp = (LTEntry *)bsearch (
					(char *)&ltk,
					(char *)LT, 
					LiteralsInUse, 
					sizeof (LTEntry), 
					(VkComparator)CompareLiterals
				    );
				if (IsNil (ltp))
				{
				    IO_fprintf
					(stderr,
					 "** Unknown Literal: #%s.\n",
					 ltk.name);
				    return 1;
				}
				else $$ = ltp->value;
			        /*** printf ("primary\n"); ***/
			    }
			|  NUMBER
			|  STRING
			|  CHARACTER
			|  '(' expression ')'
			    {
				$$ = $2;
			        /*** printf ("primary\n"); ***/
			    }
			;

unaryExpression		: primary UNARYSELECTOR
			    {
				$$ = IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
			        /*** printf ("unaryExpression\n"); ***/
			    }
			|  primary IDENTIFIER
			    {
				$$ = IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
			        /*** printf ("unaryExpression\n"); ***/
			    }
			|  unaryExpression UNARYSELECTOR
			    {
				$$ = IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
			        /*** printf ("unaryExpression\n"); ***/
			    }
			|  unaryExpression IDENTIFIER
			    {
				$$ = IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
			        /*** printf ("unaryExpression\n"); ***/
			    }
			;

keywordExpression	: keywordTermList simpleObject
			    {
				Push ($2);
				selector [0] = '\0';
				parameters = PopN ($1);
				while ($1--)
				{
				    strcpy (temp_string, PopString ());
				    strcat (temp_string, selector);
				    strcpy (selector, temp_string);
				}
				    
				    
				    
				$$ = IOBJ_SendMessage (Pop (), selector, parameters);
			        /*** printf ("keywordExpression\n"); ***/
			    }
			;

keywordTermList		: keywordTerm
			    {
				$$ = 1;
			        /*** printf ("keywordTermList :keywordTerm\n"); ***/
			    }
			|  keywordTermList keywordTerm
			    {
				$$ = $1 + 1;
			        /*** printf ("keywordTermList :keywordTermlist keywordTerm\n"); ***/
			    }
			;

keywordTerm		: simpleObject KEYWORD
			    {
				Push ($1);
				PushString ($2);
			        /*** printf ("keywordTerm\n"); ***/
			    }
			;

cascadedExpression	: cascadedExpressionRoot UNARYSELECTOR
			    {
				IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
				$$ = $1;
			        /*** printf ("cascadedExpression\n"); ***/
			    }
			| cascadedExpressionRoot IDENTIFIER
			    {
				IOBJ_SendMessage ($1,  $2,  NilOf (Ref_IOBJ_IObject));
				$$ = $1;
			        /*** printf ("cascadedExpression\n"); ***/
			    }
			| cascadedKeywordTermList simpleObject
			    {
				Push ($2);
				selector [0] = '\0';
				parameters = PopN ($1);
				while ($1--)
				{
				    strcpy (temp_string, PopString ());
				    strcat (temp_string, selector);
				    strcpy (selector, temp_string);
				}
				    
				IOBJ_SendMessage
				    ($$ = Pop (), selector, parameters);
			        /*** printf ("cascadedExpression\n"); ***/
			    }
			;

cascadedExpressionRoot	: unaryExpression ';'
			    {
				$$ = $1;
			        /*** printf ("cascadedExpressionRoot\n"); ***/
			    }
			|  keywordExpression ';'
			    {
				$$ = $1;
			        /*** printf ("cascadedExpressionRoot\n"); ***/
			    }
			|  cascadedExpression ';'
			    {
				$$ = $1;
			        /*** printf ("cascadedExpressionRoot\n"); ***/
			    }
			;

cascadedKeywordTermList
			: cascadedExpressionRoot KEYWORD
			    {
				Push ($1);
				PushString ($2);
				$$ = 1;
			        /*** printf ("cascadedKeywordTermList KEYWORD\n"); ***/
			    }
			|  cascadedKeywordTermList keywordTerm
			    {
    				$$ = $1 + 1;
			        /*** printf ("cascadedKeywordTermList keywordTerm\n"); ***/
			    }
			;


%%

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  debug.y 1 replace /users/jck/system
  860318 12:54:52 jck The yacc input for the debugger language

 ************************************************************************/
/************************************************************************
  debug.y 2 replace /users/mjc/system
  860325 16:09:15 mjc Removed requirement for ':' in unary selectors

 ************************************************************************/
/************************************************************************
  debug.y 3 replace /users/mjc/system
  860328 12:52:41 mjc Added 'expression . expression' and empty expression syntax

 ************************************************************************/
/************************************************************************
  debug.y 4 replace /users/mjc/system
  860412 12:33:01 mjc Added string syntax and objects

 ************************************************************************/
/************************************************************************
  debug.y 5 replace /users/jck/source
  860418 00:02:07 jck Expression separator replaced with comma
Continue prompt removed, YACC bug fixed (ExpressionList type),
hooks added for editor

 ************************************************************************/
/************************************************************************
  debug.y 6 replace /users/jad/system
  860425 12:09:52 jad added a print stmt for the error: no id table

 ************************************************************************/
/************************************************************************
  debug.y 7 replace /users/mjc/system
  860508 18:15:47 mjc Added 'character' syntax

 ************************************************************************/
/************************************************************************
  debug.y 8 replace /users/mjc/maketest
  860713 17:09:33 mjc Fixed cascading to make it Smalltalk like

 ************************************************************************/
/************************************************************************
  debug.y 9 replace /users/mjc/translation
  870524 09:43:10 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  debug.y 10 replace /users/m2/backend
  890927 15:24:36 m2 dummyiobj added

 ************************************************************************/

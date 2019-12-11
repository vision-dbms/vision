/*---------------------------------------------------------------------------
 * Utility program to split an input file containing an unnormalized relation
 * into a collection of relationally normalized output file.  A specification
 * file contains the definitions of each output file field as well as any NA
 * substitutions that must be performed to satisfy the preprocessor.
 *
 * A specification file consists of an ordered collection of tagged records.
 * The types and contents of those records are described in the following
 * table:
 *
 *	Tag	Description	Parameters
 *	---	-----------	----------
 *	IF	Input File	inputFileName inputRecordSize
 *	NA	NA Pattern	inputOrigin fieldWidth {L,R,F} pattern
 *	OF	Output File	outputFilePrefix outputRecordSize
 *	OR	Output Record
 *	CR	Cond. Record	registerIndex
 *	FC	Field Copy	outputOrigin fieldWidth inputOrigin
 *	FN	Field NA	outputOrigin fieldWidth
 *	FV	Field Value	outputOrigin fieldWidth fieldValue
 *	PL	Push Last	inputOrigin fieldWidth
 *	PC	Push Current	inputOrigin fieldWidth
 *	PV	Push Value	value
 *	PR	Push Register	registerIndex
 *	ST	Store		registerIndex
 *	DO	Do Operation	{==, !=, <, <=, >=, >, &&, ||, !}
 *
 * The following conventions apply to the interpretation of the these
 * specification records:
 *	- Exactly one 'IF' MUST appear in a specification file.  That 'IF'
 *	  MUST precede any specification record that contains an input
 *	  origin.
 *	- An 'OF' MUST appear before an 'OR' or 'CR' can be used.
 *	- Each 'OR' starts a new output record in the current 'OF'.
 *	- Each 'CR' starts a new output record in the current 'OF' if the
 *	  contents of the registerIndex'th register is non-NIL.
 *	- An 'OR' MUST appear before an 'FC', 'FN', or 'FV'.
 *	- Each 'FC', 'FN', and 'FV' applies to the nearest preceding 'OF' and
 *	  'OR' pair.
 *	- The 'L', 'R', and 'F' of record type 'NA' refer to Left justified,
 *	  Right justified, and Fill pattern, respectively.  'NA' records
 *	  specify conversion to standard 'preproc2' NA input conventions.
 *	- All output records will be blank initialized.
 *	- All specification record text contained after the last meaningful
 *	  field of the record is treated as descriptive and is ignored.
 *---------------------------------------------------------------------------
 */

/********************************************************
 *****  Header Files and Miscellaneous Definitions  *****
 ********************************************************/

#include "Vk.h"

enum {
    BufferSize = 256
};

#define NAString	"NA"


/*****************************
 *****  Error Processor  *****
 *****************************/

static void error (char const *fmt, ...) {
    va_list ap;

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end (ap);

    exit (ErrorExitValue);
}

/***********************
 *****  Allocator  *****
 ***********************/

static char* Allocate (
    size_t			bytes
)
{
    char *result;

    if (IsNil (result = (char*)malloc (bytes)))
        error ("Error Allocating %d Bytes\n", bytes);

    return result;
}

/***************************************
 *****  Specification Definitions  *****
 ***************************************/

/***********
 *  Datum  *
 ***********/

struct Datum {
    char const *		text;
    int				size;
};

/**************
 *  Operator  *
 **************/

enum Operator {
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_LE,
    OP_GE,
    OP_GT,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_PUSH,
    OP_PUSHR,
    OP_POP,
    OP_UNKNOWN
};

/*****************
 *  Computation  *
 *****************/

struct Computation {
    Computation*		nextComputation;
    Operator			xOperator;
    Datum*			operand;
    Datum**			rindex;
};

/****************
 *  NA Pattern  *
 ****************/

struct NAPattern {
    NAPattern*			nextPattern;
    char*			pattern;
    char*			source;
    char*			destination;
    int				size;
};

/******************
 *  Output Field  *
 ******************/

struct OutputField {
    OutputField*		nextField;
    char*			source;
    char*			destination;
    int				size;
};

/*******************
 *  Output Record  *
 *******************/

struct OutputRecord {
    OutputRecord*		nextRecord;
    Datum**			condition;
    OutputField*		fieldList;
};

/*****************
 *  Output File  *
 *****************/

struct OutputFile {
    OutputFile*			nextFile;
    OutputRecord*		recordList;
    int    			fd;
    int				recordSize;
};

/*******************
 *  Specification  *
 *******************/

struct Spec {
    Computation*		computationList;
    NAPattern*			naPatternList;
    OutputFile*			outputFileList;
    int				fd;
    int				recordSize;
    char*			inputBuffer;
    char*			lastInput;
    char*			outputBuffer;
};


/**********************************
 *****  Specification Loader  *****
 **********************************/

/******************
 *  Spec Creator  *
 ******************/

static Spec* NewSpecification (
    void
)
{
    Spec* spec;

    spec = (Spec*)Allocate (sizeof (Spec));

    spec->computationList	= NilOf (Computation*);
    spec->naPatternList		= NilOf (NAPattern*);
    spec->outputFileList	= NilOf (OutputFile*);

    spec->fd			=
    spec->recordSize		= -1;

    spec->inputBuffer		=
    spec->lastInput		=
    spec->outputBuffer		= NilOf (char *);

    return spec;
}


/***************************
 *  Interpreter Utilities  *
 ***************************/

#define RegisterCount	256
#define MaxStackDepth	256

static Datum		TDatum	= {"T", 1};

static Datum*		T	= &TDatum;
static Datum*		NIL	= NilOf (Datum*);
static Datum*		Registers   [RegisterCount];
static Datum*		Stack	    [MaxStackDepth];

static Datum**		SP	= Stack;
static Datum**		SL	= Stack + MaxStackDepth;

static void Push (
    Datum*			p
)
{
    if (SP >= SL)
        error ("Stack Overflow\n");

    *SP++ = p;
}

static Datum* Pop (
    void
)
{
    if (SP <= Stack)
	error ("Stack Underflow\n");

    return *--SP;
}

static int Compare (
    Datum*			a,
    Datum*			b
)
{
    return
	a == b
	? 0
	: IsNil (a)
	? -1
	: IsNil (b)
	? 1
	: memcmp
	    (a->text,
	     b->text,
	     a->size < b->size ? a->size : b->size);
}


/*******************************
 *  Dispatch Vector Utilities  *
 *******************************/

#define DispatchVector		struct DispatchVector

typedef void (*DispatchHandler) (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
);

DispatchVector {
    void (*ifHandler) (
	Spec*			spec,
	V::pointer_t		state,
	char*			fname,
	int			recordSize
    );
    void (*naHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			inputOrigin,
	int			fieldWidth,
	char			patternType,
	char*			pattern
    );
    void (*ofHandler) (
	Spec*			spec,
	V::pointer_t		state,
	char*			fname,
	int			recordSize
    );
    void (*orHandler) (
	Spec*			spec,
	V::pointer_t		state
    );
    void (*crHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			registerIndex
    );
    void (*fcHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			outputOrigin,
	int			fieldWidth,
	int			inputOrigin
    );
    void (*fnHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			outputOrigin,
	int			fieldWidth
    );
    void (*fvHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			outputOrigin,
	int			fieldWidth,
	char*			value
    );
    void (*plHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			inputOrigin,
	int			fieldWidth
    );
    void (*pcHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			inputOrigin,
	int			fieldWidth
    );
    void (*pvHandler) (
	Spec*			spec,
	V::pointer_t		state,
	char*			value
    );
    void (*prHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			registerIndex
    );
    void (*stHandler) (
	Spec*			spec,
	V::pointer_t		state,
	int			registerIndex
    );
    void (*doHandler) (
	Spec*			spec,
	V::pointer_t		state,
	Operator		xOperator
    );
};

static void DefaultHandler (
    void
)
{
}

static void InitializeDispatchVector (
    DispatchVector*		dv
)
{
    dv->ifHandler = (
	void (*) (Spec*, V::pointer_t, char*, int)
    ) DefaultHandler;

    dv->naHandler = (
	void (*) (Spec*, V::pointer_t, int, int, char, char*)
    ) DefaultHandler;

    dv->ofHandler = (
	void (*) (Spec*, V::pointer_t, char*, int)
    ) DefaultHandler;

    dv->orHandler = (
	void (*) (Spec*, V::pointer_t)
    ) DefaultHandler;

    dv->crHandler = (
	void (*) (Spec*, V::pointer_t, int)
    ) DefaultHandler;

    dv->fcHandler = (
	void (*) (Spec*, V::pointer_t, int, int, int)
    ) DefaultHandler;

    dv->fnHandler = (
	void (*) (Spec*, V::pointer_t, int, int)
    ) DefaultHandler;

    dv->fvHandler = (
	void (*) (Spec*, V::pointer_t, int, int, char*)
    ) DefaultHandler;

    dv->plHandler = (
	void (*) (Spec*, V::pointer_t, int, int)
    ) DefaultHandler;

    dv->pcHandler = (
	void (*) (Spec*, V::pointer_t, int, int)
    ) DefaultHandler;

    dv->pvHandler = (
	void (*) (Spec*, V::pointer_t, char*)
    ) DefaultHandler;

    dv->prHandler = (
	void (*) (Spec*, V::pointer_t, int)
    ) DefaultHandler;

    dv->stHandler = (
	void (*) (Spec*, V::pointer_t, int)
    ) DefaultHandler;
    
    dv->doHandler = (
	void (*) (Spec*, V::pointer_t, Operator)
    ) DefaultHandler;
}

/***********************
 *  Record Processors  *
 ***********************/

static void CheckParse (
    char const *		what,
    char const *		data,
    int				expected,
    int				actual
)
{
    if (actual != expected)
    {
	error
	    ("Improper Format in '%s' Record In Parameter %d:\n\t%s\n",
	     what, actual + 1, data);
    }
}

static void Process_IF (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    char fname[BufferSize];
    int recordSize;

    CheckParse
	("IF", specRecord, 2,
	 sscanf (specRecord, " %*s %s %d", fname, &recordSize));

    (*dv->ifHandler)(spec, state, fname, recordSize);
}

static void Process_NA (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    char patternType[2], pattern[BufferSize];
    int inputOrigin, fieldWidth;

    CheckParse
	("NA", specRecord, 4,
	 sscanf (specRecord, " %*s %d %d %1s %s",
		 &inputOrigin, &fieldWidth, patternType, pattern));

    (*dv->naHandler)
	(spec, state, inputOrigin, fieldWidth, *patternType, pattern);
}

static void Process_OF (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    char fname[BufferSize];
    int recordSize;

    CheckParse
	("OF", specRecord, 2,
	  sscanf (specRecord, " %*s %s %d", fname, &recordSize));

    (*dv->ofHandler)(spec, state, fname, recordSize);
}

static void Process_OR (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    CheckParse ("OR", specRecord, 0, sscanf (specRecord, " %*s"));

    (*dv->orHandler)(spec, state);
}

static void Process_CR (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int registerIndex;

    CheckParse
	("CR", specRecord, 1,
	 sscanf (specRecord, " %*s %d", &registerIndex));

    if (registerIndex < 0 || registerIndex >= RegisterCount)
        error
	    ("Register Index %d not in range [0:%d] in:\n\t%s\n",
	     registerIndex, RegisterCount, specRecord);

    (*dv->crHandler)(spec, state, registerIndex);
}

static void Process_FC (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int outputOrigin, fieldWidth, inputOrigin;

    CheckParse
	("FC", specRecord, 3,
	 sscanf (specRecord, " %*s %d %d %d",
		 &outputOrigin, &fieldWidth, &inputOrigin));

    (*dv->fcHandler)(spec, state, outputOrigin, fieldWidth, inputOrigin);
}

static void Process_FN (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int outputOrigin, fieldWidth;

    CheckParse
	("FN", specRecord, 2,
	 sscanf (specRecord, " %*s %d %d", &outputOrigin, &fieldWidth));

    (*dv->fnHandler)(spec, state, outputOrigin, fieldWidth);
}

static void Process_FV (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int outputOrigin, fieldWidth;
    char value[BufferSize];

    CheckParse
	("FV", specRecord, 3,
	 sscanf (specRecord, " %*s %d %d %s",
		 &outputOrigin, &fieldWidth, value));

    (*dv->fvHandler)(spec, state, outputOrigin, fieldWidth, value);
}

static void Process_PL (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int inputOrigin, fieldWidth;

    CheckParse
	("PL", specRecord, 2,
	 sscanf (specRecord, " %*s %d %d", &inputOrigin, &fieldWidth));

    (*dv->plHandler)(spec, state, inputOrigin, fieldWidth);
}

static void Process_PC (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int inputOrigin, fieldWidth;

    CheckParse
	("PC", specRecord, 2,
	 sscanf (specRecord, " %*s %d %d", &inputOrigin, &fieldWidth));

    (*dv->pcHandler)(spec, state, inputOrigin, fieldWidth);
}

static void Process_PV (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    char value[BufferSize];

    CheckParse
	("PV", specRecord, 1,
	 sscanf (specRecord, " %*s %s", value));

    (*dv->pvHandler)(spec, state, value);
}

static void Process_PR (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int registerIndex;

    CheckParse
	("PR", specRecord, 1,
	 sscanf (specRecord, " %*s %d", &registerIndex));

    if (registerIndex < 0 || registerIndex >= RegisterCount)
        error
	    ("Register Index %d not in range [0:%d] in:\n\t%s\n",
	     registerIndex, RegisterCount, specRecord);

    (*dv->prHandler)(spec, state, registerIndex);
}

static void Process_ST (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    int registerIndex;

    CheckParse
	("ST", specRecord, 1,
	 sscanf (specRecord, " %*s %d", &registerIndex));

    if (registerIndex < 0 || registerIndex >= RegisterCount)
        error
	    ("Register Index %d not in range [0:%d] in:\n\t%s\n",
	     registerIndex, RegisterCount, specRecord);

    (*dv->stHandler)(spec, state, registerIndex);
}

static void Process_DO (
    char*			specRecord,
    DispatchVector*		dv,
    Spec*			spec,
    V::pointer_t		state
)
{
    char operatorName[BufferSize];
    Operator operatorCode;

    CheckParse
	("DO", specRecord, 1,
	 sscanf (specRecord, " %*s %s", operatorName));

    operatorCode =
	0 == strcmp (operatorName, "==")
	? OP_EQ
	: 0 == strcmp (operatorName, "!=")
	? OP_NE
	: 0 == strcmp (operatorName, "<")
	? OP_LT
	: 0 == strcmp (operatorName, "<=")
	? OP_LE
	: 0 == strcmp (operatorName, ">=")
	? OP_GE
	: 0 == strcmp (operatorName, ">")
	? OP_GT
	: 0 == strcmp (operatorName, "&&")
	? OP_AND
	: 0 == strcmp (operatorName, "||")
	? OP_OR
	: 0 == strcmp (operatorName, "!")
	? OP_NOT
	: OP_UNKNOWN;

    if (OP_UNKNOWN == operatorCode)
        error
	    ("Unrecognized 'DO' operator '%s' in:\n\t%s\n",
	     operatorName, specRecord);

    (*dv->doHandler)(spec, state, operatorCode);
}

static void Process_Unknown (
    char*			specRecord,
    DispatchVector*		Unused(dv),
    Spec*			Unused(spec),
    V::pointer_t		Unused(state)
)
{
    error ("Unrecognized Spec Record:\n\t%s\n", specRecord);
}


/***********************
 *  Record Dispatcher  *
 ***********************/

static void Dispatch (
    char*			specRecord,
    Spec*			spec,
    DispatchVector*		dv,
    V::pointer_t		state
)
{
    char tag[BufferSize];
    DispatchHandler handler;

    handler =
	  1 != sscanf (specRecord, " %s", tag)
	? Process_Unknown
	: 0 == strcmp (tag, "IF")
	? Process_IF
	: 0 == strcmp (tag, "NA")
	? Process_NA
	: 0 == strcmp (tag, "OF")
	? Process_OF
	: 0 == strcmp (tag, "OR")
	? Process_OR
	: 0 == strcmp (tag, "CR")
	? Process_CR
	: 0 == strcmp (tag, "FC")
	? Process_FC
	: 0 == strcmp (tag, "FN")
	? Process_FN
	: 0 == strcmp (tag, "FV")
	? Process_FV
	: 0 == strcmp (tag, "PL")
	? Process_PL
	: 0 == strcmp (tag, "PC")
	? Process_PC
	: 0 == strcmp (tag, "PV")
	? Process_PV
	: 0 == strcmp (tag, "PR")
	? Process_PR
	: 0 == strcmp (tag, "ST")
	? Process_ST
	: 0 == strcmp (tag, "DO")
	? Process_DO
	: Process_Unknown;

    (*handler)(specRecord, dv, spec, state);
}

/*****************
 *  Loader Pass  *
 *****************/

static void DoSpecFilePass (
    FILE*			sfile,
    Spec*			spec,
    DispatchVector*		dv,
    V::pointer_t		state
)
{
    char
	specRecord [BufferSize];

    rewind (sfile);
    while (specRecord == fgets (specRecord, sizeof specRecord, sfile))
    {
	Dispatch (specRecord, spec, dv, state);
    }
}


/*******************
 *  Loader Pass 1  *
 *******************/
/*---------------------------------------------------------------------------
 * The first pass of the spec file loader validates offsets and allocates
 * buffers.
 *---------------------------------------------------------------------------
 */

static struct Pass1State_t {
    int inputBufferSize, currentOutputSize, outputBufferSize;
} Pass1State;

static void Pass1IFHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    char*			Unused(fname),
    int				recordSize
)
{
    if (Pass1State.inputBufferSize > 0)
        error ("Multiple 'IF' Records Found\n");

    Pass1State.inputBufferSize = recordSize;
}

static void Pass1NAHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth,
    char			Unused(patternType),
    char*			Unused(pattern)
)
{
    if (inputOrigin < 0 ||
        fieldWidth < 0 ||
	inputOrigin + fieldWidth > Pass1State.inputBufferSize)
    {
	error
	    ("Unreasonable 'NA' parameters: Origin:%d, Width:%d\n",
	     inputOrigin, fieldWidth);
    }
}

static void Pass1OFHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    char*			Unused(fname),
    int				recordSize
)
{
    if (recordSize > Pass1State.outputBufferSize)
        Pass1State.outputBufferSize = recordSize;

    Pass1State.currentOutputSize = recordSize;
}

static void Pass1FCHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth,
    int				inputOrigin
)
{
    if (inputOrigin < 0 ||
        fieldWidth < 0 ||
	outputOrigin < 0 ||
	inputOrigin + fieldWidth > Pass1State.inputBufferSize ||
	outputOrigin + fieldWidth > Pass1State.currentOutputSize)
    {
	error
	    (
"Unreasonable 'FC' parameters: OutOrigin:%d, Width:%d, InOrigin:%d\n",
	     outputOrigin, fieldWidth, inputOrigin);
    }
}

static void Pass1FNHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth
)
{
    if (fieldWidth < 0 ||
	outputOrigin < 0 ||
	outputOrigin + fieldWidth > Pass1State.currentOutputSize)
    {
	error
	    ("Unreasonable 'FN' parameters: OutOrigin:%d, Width:%d\n",
	     outputOrigin, fieldWidth);
    }
}

static void Pass1FVHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth,
    char*			Unused(value)
)
{
    if (fieldWidth < 0 ||
	outputOrigin < 0 ||
	outputOrigin + fieldWidth > Pass1State.currentOutputSize)
    {
	error
	    ("Unreasonable 'FV' parameters: OutOrigin%d, Width:%d\n",
	     outputOrigin, fieldWidth);
    }
}

static void Pass1PLHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth
)
{
    if (inputOrigin < 0 ||
        fieldWidth < 0 ||
	inputOrigin + fieldWidth > Pass1State.inputBufferSize)
    {
	error
	    ("Unreasonable 'PL' parameters: Origin:%d, Width:%d\n",
	     inputOrigin, fieldWidth);
    }
}

static void Pass1PCHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth
)
{
    if (inputOrigin < 0 ||
        fieldWidth < 0 ||
	inputOrigin + fieldWidth > Pass1State.inputBufferSize)
    {
	error
	    ("Unreasonable 'PC' parameters: Origin:%d, Width:%d\n",
	     inputOrigin, fieldWidth);
    }
}


static void SpecLoaderPass1 (
    FILE*			sfile,
    Spec*			spec
)
{
    DispatchVector dv;

    InitializeDispatchVector (&dv);
    dv.ifHandler = Pass1IFHandler;
    dv.naHandler = Pass1NAHandler;
    dv.ofHandler = Pass1OFHandler;
    dv.fcHandler = Pass1FCHandler;
    dv.fnHandler = Pass1FNHandler;
    dv.fvHandler = Pass1FVHandler;
    dv.plHandler = Pass1PLHandler;
    dv.pcHandler = Pass1PCHandler;

    Pass1State.inputBufferSize		=
    Pass1State.currentOutputSize	=
    Pass1State.outputBufferSize		= 0;

    DoSpecFilePass (sfile, spec, &dv, NilOf (V::pointer_t));

    if (Pass1State.inputBufferSize <= 0)
    {
	error ("No Input File Specified\n");
    }

    if (Pass1State.outputBufferSize <= 0)
    {
	error ("No Output Files Specified\n");
    }

    spec->recordSize	= Pass1State.inputBufferSize;
    spec->inputBuffer	= Allocate (Pass1State.inputBufferSize);

    spec->lastInput	= Allocate (Pass1State.inputBufferSize);
    memset (spec->lastInput, ' ', Pass1State.inputBufferSize);

    spec->outputBuffer	= Allocate (Pass1State.outputBufferSize);

    printf
        ("Input Record Size: %d\nMax Output Record Size: %d\n",
	 Pass1State.inputBufferSize,
	 Pass1State.outputBufferSize);
}


/*******************
 *  Loader Pass 2  *
 *******************/
/*---------------------------------------------------------------------------
 * The second pass of the spec file loader creates descriptions of the NA
 * substitutions and output records.
 *---------------------------------------------------------------------------
 */

static struct Pass2State_t {
    Computation*		computation;
    OutputFile*			outputFile;
    OutputRecord*		outputRecord;
} Pass2State;

static void Pass2IFHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    char*			fname,
    int				Unused(recordSize)
)
{
    if ((spec->fd = open (fname, O_RDONLY, 0)) < 0)
    {
	error ("Error Opening Input File '%s'\n", fname);
    }
}

static void Pass2NAHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth,
    char			patternType,
    char*			pattern
)
{
    NAPattern* nap;

    nap = (NAPattern*) Allocate (sizeof (NAPattern));

    nap->nextPattern	= spec->naPatternList;
    nap->pattern	= Allocate (fieldWidth + 1);
    nap->source		= Allocate (fieldWidth + 1);
    nap->destination	= spec->inputBuffer + inputOrigin;
    nap->size		= fieldWidth;

    switch (patternType)
    {
    case 'L':
	sprintf (nap->pattern, "%-*s", fieldWidth, pattern);
        break;
    case 'R':
	sprintf (nap->pattern, "%*s", fieldWidth, pattern);
        break;
    case 'F':
	memset (nap->pattern, pattern[0], fieldWidth);
        break;
    default:
        error ("Unrecognized 'NA' Pattern Type '%c'\n", patternType);
        break;
    }

    sprintf (nap->source, "%-*s", fieldWidth, NAString);

    spec->naPatternList = nap;
}

static void Pass2OFHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    char*			fname,
    int				recordSize
)
{
    OutputFile* of;

    of = (OutputFile*) Allocate (sizeof (OutputFile));

    of->nextFile	= spec->outputFileList;
    of->recordList	= NilOf (OutputRecord*);
    of->fd		= creat (fname, 0666);
    of->recordSize	= recordSize;

    if ((int)of->fd < 0)
    {
	error ("Error Creating Output File '%s'\n", fname);
    }

    Pass2State.outputRecord	= NilOf (OutputRecord*);
    Pass2State.outputFile	=
    spec->outputFileList	= of;
}

static void Pass2ORHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state)
)
{
    OutputRecord* pOR;

    if (IsNil (Pass2State.outputFile))
	error ("'OR' Record Encountered Outside the Scope of an 'OF'\n");

    pOR = (OutputRecord*) Allocate (sizeof (OutputRecord));

    pOR->nextRecord	= Pass2State.outputFile->recordList;
    pOR->condition	= &T;
    pOR->fieldList	= NilOf (OutputField*);

    Pass2State.outputFile->recordList	=
    Pass2State.outputRecord		= pOR;
}

static void Pass2CRHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				registerIndex
)
{
    OutputRecord* pOR;

    if (IsNil (Pass2State.outputFile))
	error ("'OR' Record Encountered Outside the Scope of an 'OF'\n");

    pOR = (OutputRecord*) Allocate (sizeof (OutputRecord));

    pOR->nextRecord	= Pass2State.outputFile->recordList;
    pOR->condition	= Registers + registerIndex;
    pOR->fieldList	= NilOf (OutputField*);

    Pass2State.outputFile->recordList	=
    Pass2State.outputRecord		= pOR;
}

static void Pass2FCHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth,
    int				inputOrigin
)
{
    OutputField* ofield;

    if (IsNil (Pass2State.outputRecord))
	error ("'FC' Record Encountered Outside the Scope of an 'OR'\n");

    ofield = (OutputField*) Allocate (sizeof (OutputField));

    ofield->nextField	= Pass2State.outputRecord->fieldList;
    ofield->source	= spec->inputBuffer + inputOrigin;
    ofield->destination	= spec->outputBuffer + outputOrigin;
    ofield->size	= fieldWidth;

    Pass2State.outputRecord->fieldList = ofield;
}

static void Pass2FNHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth
)
{
    OutputField* ofield;

    if (IsNil (Pass2State.outputRecord))
	error ("'FN' Record Encountered Outside the Scope of an 'OR'\n");

    ofield = (OutputField*) Allocate (sizeof (OutputField));

    ofield->nextField	= Pass2State.outputRecord->fieldList;
    ofield->source	= Allocate (fieldWidth + 1);
    ofield->destination	= spec->outputBuffer + outputOrigin;
    ofield->size	= fieldWidth;

    sprintf (ofield->source, "%*s", fieldWidth, NAString);

    Pass2State.outputRecord->fieldList = ofield;
}

static void Pass2FVHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				outputOrigin,
    int				fieldWidth,
    char*			value
)
{
    OutputField* ofield;

    if (IsNil (Pass2State.outputRecord))
	error ("'FV' Record Encountered Outside the Scope of an 'OR'\n");

    ofield = (OutputField*) Allocate (sizeof (OutputField));

    ofield->nextField	= Pass2State.outputRecord->fieldList;
    ofield->source	= Allocate (fieldWidth + 1);
    ofield->destination	= spec->outputBuffer + outputOrigin;
    ofield->size	= fieldWidth;

    sprintf (ofield->source, "%*s", fieldWidth, value);

    Pass2State.outputRecord->fieldList = ofield;
}

static void Pass2PLHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth
)
{
    Datum* operand;
    Computation* comp;

    operand = (Datum*)Allocate (sizeof (Datum));
    operand->text = spec->lastInput + inputOrigin;
    operand->size = fieldWidth;

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= OP_PUSH;
    comp->operand		= operand;
    comp->rindex		= NilOf (Datum**);

    Pass2State.computation	= comp;
}

static void Pass2PCHandler (
    Spec*			spec,
    V::pointer_t		Unused(state),
    int				inputOrigin,
    int				fieldWidth
)
{
    Datum* operand;
    Computation* comp;

    operand = (Datum*)Allocate (sizeof (Datum));
    operand->text = spec->inputBuffer + inputOrigin;
    operand->size = fieldWidth;

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= OP_PUSH;
    comp->operand		= operand;
    comp->rindex		= NilOf (Datum**);

    Pass2State.computation	= comp;
}

static void Pass2PVHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    char*			value
)
{
    Datum* operand;
    Computation* comp;

    operand = (Datum*)Allocate (sizeof (Datum));
    operand->size = strlen (value);
    operand->text = strcpy ((char *)Allocate (operand->size + 1), value);

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= OP_PUSH;
    comp->operand		= operand;
    comp->rindex		= NilOf (Datum**);

    Pass2State.computation	= comp;
}

static void Pass2PRHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				registerIndex
)
{
    Computation* comp;

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= OP_PUSHR;
    comp->operand		= NilOf (Datum*);
    comp->rindex		= Registers + registerIndex;

    Pass2State.computation	= comp;
}

static void Pass2STHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    int				registerIndex
)
{
    Computation* comp;

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= OP_POP;
    comp->operand		= NilOf (Datum*);
    comp->rindex		= Registers + registerIndex;

    Pass2State.computation	= comp;
}

static void Pass2DOHandler (
    Spec*			Unused(spec),
    V::pointer_t		Unused(state),
    Operator			xOperator
)
{
    Computation* comp;

    comp = (Computation*) Allocate (sizeof (Computation));
    comp->nextComputation	= Pass2State.computation;
    comp->xOperator		= xOperator;
    comp->operand		= NilOf (Datum*);
    comp->rindex		= NilOf (Datum**);

    Pass2State.computation	= comp;
}


static void UpdateComputationList (
    Spec*			spec
)
{
    Computation* comp;

/*****  Reverse the chain of computations...  *****/
    while (IsntNil (Pass2State.computation))
    {
        comp			= Pass2State.computation;
	Pass2State.computation	= comp->nextComputation;
	comp->nextComputation	= spec->computationList;
	spec->computationList	= comp;
    }
}


static void SpecLoaderPass2 (
    FILE*			sfile,
    Spec*			spec
)
{
    DispatchVector dv;

    InitializeDispatchVector (&dv);
    dv.ifHandler = Pass2IFHandler;
    dv.naHandler = Pass2NAHandler;
    dv.ofHandler = Pass2OFHandler;
    dv.orHandler = Pass2ORHandler;
    dv.crHandler = Pass2CRHandler;
    dv.fcHandler = Pass2FCHandler;
    dv.fnHandler = Pass2FNHandler;
    dv.fvHandler = Pass2FVHandler;
    dv.plHandler = Pass2PLHandler;
    dv.pcHandler = Pass2PCHandler;
    dv.pvHandler = Pass2PVHandler;
    dv.prHandler = Pass2PRHandler;
    dv.doHandler = Pass2DOHandler;
    dv.stHandler = Pass2STHandler;

    Pass2State.computation	= NilOf (Computation*);
    Pass2State.outputFile	= NilOf (OutputFile*);
    Pass2State.outputRecord	= NilOf (OutputRecord*);

    DoSpecFilePass (sfile, spec, &dv, NilOf (V::pointer_t));

    UpdateComputationList (spec);
}


/*****************
 *  Loader Main  *    
 *****************/

static Spec* Specifications (
    char*			specFile
)
{
    Spec*
	spec;
    FILE
	*sfile;

    spec = NewSpecification ();

    if (NULL == (sfile = fopen (specFile, "r")))
        error ("Error Opening Specification File '%s'\n", specFile);

    SpecLoaderPass1 (sfile, spec);
    SpecLoaderPass2 (sfile, spec);

    fclose (sfile);

    return spec;
}


/****************************
 *****  Input Processor  ****
 ****************************/

/*****************************
 *  Computation Interpreter  *
 *****************************/

static void PerformComputations (
    Spec*			spec
)
{
    Computation* comp;
    Datum*	a;
    Datum*	b;

    for (comp = spec->computationList;
	 IsntNil (comp);
	 comp = comp->nextComputation)
    {
	switch (comp->xOperator)
	{
	case OP_EQ:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) == 0 ? T : NIL);
	    break;
	case OP_NE:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) != 0 ? T : NIL);
	    break;
	case OP_LT:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) <  0 ? T : NIL);
	    break;
	case OP_LE:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) <= 0 ? T : NIL);
	    break;
	case OP_GE:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) >= 0 ? T : NIL);
	    break;
	case OP_GT:
	    b = Pop ();
	    a = Pop ();
	    Push (Compare (a, b) >  0 ? T : NIL);
	    break;
	case OP_AND:
	    b = Pop ();
	    a = Pop ();
	    Push (a && b ? T : NIL);
	    break;
	case OP_OR:
	    b = Pop ();
	    a = Pop ();
	    Push (a || b ? T : NIL);
	    break;
	case OP_NOT:
	    Push (Pop () ? NIL : T);
	    break;
	case OP_PUSH:
	    Push (comp->operand);
	    break;
	case OP_PUSHR:
	    Push (*comp->rindex);
	    break;
	case OP_POP:
	    *comp->rindex = Pop ();
	    break;
	default:
	    break;
	}
    }
}


/******************
 *  Input Editor  *
 ******************/

static void EditInput (
    Spec*			spec
)
{
    NAPattern* nap;

    for (nap = spec->naPatternList; IsntNil (nap); nap = nap->nextPattern)
    {
	if (0 == memcmp (nap->destination, nap->pattern, nap->size))
	    memcpy (nap->destination, nap->source, nap->size);
    }
}

/**********************
 *  Output Generator  *
 **********************/

static void GenerateOutput (
    Spec*			spec
)
{
    OutputFile*
	ofile;
    OutputRecord*
	orecord;
    OutputField*
	ofield;

    for (ofile = spec->outputFileList;
         IsntNil (ofile);
	 ofile = ofile->nextFile)
    {
	for (orecord = ofile->recordList;
	     IsntNil (orecord);
	     orecord = orecord->nextRecord)
	{
	    if (IsNil (*orecord->condition))
	        continue;

	    memset (spec->outputBuffer, ' ', ofile->recordSize);
	    for (ofield = orecord->fieldList;
	         IsntNil (ofield);
		 ofield = ofield->nextField)
	    {
		memcpy
		    (ofield->destination, ofield->source, ofield->size);
	    }
	    if (ofile->recordSize !=
		write (ofile->fd, spec->outputBuffer, ofile->recordSize))
	    {
	        error ("Error Generating Output Record\n");
	    }
	}
    }
}


/************************
 *  Last Input Manager  *
 ************************/

static void UpdateLastInput (
    Spec*			spec
)
{
    memcpy (spec->lastInput, spec->inputBuffer, spec->recordSize);
}


/********************
 *  Processor Main  *
 ********************/

static void Process (
    Spec*			spec
)
{
    int nbytes;

    while (spec->recordSize ==
	    (nbytes = read (spec->fd, spec->inputBuffer, spec->recordSize)))
    {
	PerformComputations (spec);
        EditInput (spec);
	GenerateOutput (spec);
	UpdateLastInput (spec);
    }

    if (nbytes < 0)
    {
	perror ("");
        error ("Error Reading Input File\n");
    }
}


/**************************
 *****  Main Program  *****
 **************************/

int main (
    int				argc,
    char*			argv[]
)
{
    if (argc != 2)
        error ("Usage:\n\t%s specFile\n", argv[0]);

    Process (Specifications (argv[1]));

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  normalize.c 1 replace /users/mjc/util
  870802 19:02:25 mjc Release initial version of a file normalization tool

 ************************************************************************/
/************************************************************************
  normalize.c 2 replace /users/mjc/util
  870803 18:32:24 mjc Release interpreter

 ************************************************************************/
/************************************************************************
  normalize.c 3 replace /users/insyte/MJC
  870803 23:52:30 insyte Make 'NA' output string left justified

 ************************************************************************/
/************************************************************************
  normalize.c 4 replace /users/m2/dbup
  880426 12:12:14 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  normalize.c 5 replace /users/m2/dbup
  880512 13:48:40 m2 Removed unneeded SEG_ calls

 ************************************************************************/
/************************************************************************
  normalize.c 6 replace /users/jck/dbupdate
  891006 15:00:56 jck Adding Standard Exit Codes

 ************************************************************************/

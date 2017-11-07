/*****  Utilities Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfUTIL

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

#include "VkRadixListSorter.h"

/*****  Facility  *****/
#include "batchvision.h"

#include "iom.h"
#include "m.h"
#include "ps.h"
#include "rslang.h"
#include "selector.h"
#include "uvector.h"

#include "vdbupdate.h"
#include "vdates.h"
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vprimfns.h"
#include "vstdio.h"
#include "vunwind.h"
#include "vutil.h"
#include "vmachine.h"

#include "RTblock.h"
#include "RTclosure.h"
#include "RTcontext.h"
#include "RTdictionary.h"
#include "RTdsc.h"
#include "RTindex.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTmethod.h"
#include "RTptoken.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

#include "VfGuardTool.h"

#include "VFragment.h"
#include "VTransientServices.h"

#include "V_VArgList.h"
#include "V_VString.h"

/*****  External Object Support *****/
#include "VSNFTaskHolder.h"

/*****  Self  *****/
#include "PFutil.h"


/*******************************
 *******************************
 *****  Primitive Indices  *****
 *******************************
 *******************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that assign indices to the primitives.
 *
 * The numeric indices associated in this section with each of the primitives
 * are the system wide 'indices' by which the primitives are known.  These
 * indices are stored in permanent object memory;  consequently, once assigned,
 * they must not be changed or deleted.  To avoid an fatal error at system
 * startup, the largest index assigned in this section must be less than
 * 'V_PF_MaxPrimitiveIndex' (see "vprimfns.h").
 * If necessary this maximum can be increased.
 *---------------------------------------------------------------------------
 */
/*======================================*
 *  This module currently has indices:  *
 *	   0 - 119 (except 85,86)       *
 *======================================*/


/********************************
 *****  Utility Primitives  *****
 ********************************/

#define XPanic					0

#define XCollectGarbage				1
#define XCollectSessionGarbage			4

#define XSetSessionAttribute			2
#define XUpdateNetwork				115

#define XExitSession				3

#define XReclaimResources			5

#define XFederate				6
#define XClientObject				7
#define XExternalObject				8

/******************************************
 *****  Time Stamp Access Primitives  *****
 ******************************************/

#define XAccessResourceTimeStamp		14
#define XAccessTimeStamp			15
#define XDecodeTZTimeStamp			16
#define XDecodeGMTimeStamp			17
#define XFormatTZTimeStamp			18
#define XFormatGMTimeStamp			19


/***********************************
 *****  Evaluation Primitives  *****
 ***********************************/

#define XStringEvaluate				35

#define XEvaluateInLocalContext			36

#define XSendForExecution			37
#define XSendForValue				38
#define XSendForEnvironment			39

#define XEvaluateForValue			40
#define XEvaluateForEnvironment			41
#define XEvaluateUltimateParameter		42
#define XEvaluatePenultimateParameter		43
#define XForDateEvalUltimateParameter		44

#define XWhileTrue				45

#define XByDaysForDateEval			50
#define XByBDaysForDateEval			51
#define XByMonthsForDateEval			52
#define XByMonthBeginningsForDateEval		53
#define XByMonthEndsForDateEval			54
#define XByQtrsForDateEval			55
#define XByQtrBeginningsForDateEval		56
#define XByQtrEndsForDateEval			57
#define XByYearsForDateEval			58
#define XByYearBeginningsForDateEval		59
#define XByYearEndsForDateEval			60


/*****************************************
 *****  Class Conversion Primitives  *****
 *****************************************/

#define XSelectorToString			65

/*******************************************
 *****  Closure and Method Primitives  *****
 *******************************************/

#define XClosureSelector			70
#define XMethodSelector				71
#define XClosureAsMethod			72
#define XMethodAsClosure			73
#define XClosureSelf				74
#define XClosureCurrent				75
#define XClosureOrigin				76

/***********************************
 *****  Assignment Primitives  *****
 ***********************************/

#define XAssign					80


/*********************************************
 *****  Property Maintenance Primitives  *****
 *********************************************/

#define XCleanStore				109
#define XCleanDictionary			110
#define XAlignStore				116

/*********************************************
 *****  Incorporator Support Primitives  *****
 *********************************************/

#define XDBUpdateReplaceDump			117
#define XDBUpdateAppendDump			118


/***********************************
 ***********************************
 *****  Primitive Definitions  *****
 ***********************************
 ***********************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that define the code bodies of the primitives.
 *---------------------------------------------------------------------------
 */


/********************************
 *****  Utility Primitives  *****
 ********************************/

/*---------------------------------------------------------------------------
 *****  'Panic' primitive.
 *
 *  Invocation Synopsis:
 *	anObject anyMessageOfAnyValence
 *
 *  Returns:
 *	This primitive never returns.
 *
 *****/
V_DefinePrimitive (Panic) {
    pTask->raiseException (
	EC__UsageError,
	"!!!!!  Primitive # %u Not Implemented In This Version  !!!!!",
	V_TOTSC_Primitive
    );
}

V_DefinePrimitive (CollectGarbage) {
    bool fSuccess = false;

    switch (V_TOTSC_Primitive) {
    case XCollectGarbage:
	fSuccess = pTask->codDatabase ()->DisposeOfNetworkGarbage ();
	break;

    case XCollectSessionGarbage:
	fSuccess = /*pTask->codDatabase ()->*/M_DisposeOfSessionGarbage ();
	break;

    default:
	pTask->raiseUnimplementedAliasException ("CollectGarbage");
	break;
    }

    pTask->loadDucWithBoolean (fSuccess);
}

V_DefinePrimitive (ExitSession) {
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	pTask->scheduler()->restartProcess ();
	break; // like this is necessary ...
    case 1:
	IOM_PutBufferedData ();
	UNWIND_Exit (pTask->scheduler()->getExitValue ());
	break; // or this, for that matter.
    default:
	pTask->raiseUnimplementedAliasException ("ExitSession");
	break;
    }
}

V_DefinePrimitive (ReclaimResources) {
    switch (V_TOTSC_PrimitiveFlags) {
    case 1:
	M_FlushCachedResources ();
	pTask->loadDucWithTrue ();
	break;
    case 2:
	pTask->loadDucWithInteger (M_ReclaimSegments ());
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ReclaimResources");
	break;
    }
}


/***********************
 *----  XFederate  ----*
 ***********************/

PrivateFnDef void AccessDBRoot (VPrimitiveTask *pTask, char const *pDatabaseSpec) {
    VString iDatabaseSpec;

    unsigned int xSpace = 3;
    char const *pColon = strchr (pDatabaseSpec, ':');
    if (pColon) {
	sscanf (pColon + 1, "%u", &xSpace);

	VString iDatabaseSpecPrefix;
	iDatabaseSpecPrefix.setTo (pDatabaseSpec, pColon - pDatabaseSpec);
	iDatabaseSpec.setTo (iDatabaseSpecPrefix);
	pDatabaseSpec = iDatabaseSpec;
    }

    VString	iVersionSpec;
    char const *pVersionSpec = 0;
    char const *pComma = strchr (pDatabaseSpec, ',');
    if (pComma) {
	iVersionSpec.setTo (pComma + 1);
	pVersionSpec = iVersionSpec;

	VString iDatabaseSpecPrefix;
	iDatabaseSpecPrefix.setTo (pDatabaseSpec, pComma - pDatabaseSpec);
	iDatabaseSpec.setTo (iDatabaseSpecPrefix);
	pDatabaseSpec = iDatabaseSpec;
    }

    M_AND *pDB = ENVIR_Session ()->Activate (pDatabaseSpec, pVersionSpec);
    if (!pDB)
	pTask->loadDucWithNA ();
    else {
	DSC_Descriptor iTLE;
	pDB->AccessTheTLE (xSpace, iTLE);
	iTLE.codSpace ()->Distinguish ();
	pTask->loadDucWithMoved (iTLE);
    }
}

V_DefinePrimitive (Federate) {
    if (!pTask->isScalar ()) pTask->raiseUnimplementedOperationException (
	"Federate: Non-Scalar Operation Not Supported"
    );

/*****  Access the string store, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    M_CPD *pStringStore = rCurrent.storeCPD ();

/*****  ... and compile the string.  *****/
    if (RTYPE_C_Block == M_CPD_RType (pStringStore))
	AccessDBRoot (pTask, V_BlockString (pStringStore, DSC_Descriptor_Scalar_Int (rCurrent)));
    else {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rCurrent));

	M_CPD *pCharUV = rtLSTORE_CPD_ContentStringsCPD (pStringStore);
	AccessDBRoot (
	    pTask, V_LStoreString (pStringStore, pCharUV, DSC_Descriptor_Scalar_Int (rCurrent))
	);

	pCharUV->release ();
    }
}


/***************************
 *----  XClientObject  ----*
 ***************************/

V_DefinePrimitive (ClientObject) {
    pTask->accessClientObject ();
}

/*****************************
 *----  XExternalObject  ----*
 *****************************/

V_DefinePrimitive (ExternalObject) {
    if (!pTask->isScalar ()) pTask->raiseUnimplementedOperationException (
	"ExternalObject: Non-Scalar Operation Not Supported"
    );

    bool bUsingDirectory = false;
    switch (pTask->flags ()) {
    case 0:	//  ... address:port
	break;
    case 1:	//  ... directory
	bUsingDirectory = true;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ExternalObject");
	break;
    }

/*****  Access the string store for the external object address  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    M_CPD *pStringStore = rCurrent.storeCPD ();

/*****  ... and compile the string.  *****/
    if (RTYPE_C_Block == M_CPD_RType (pStringStore))
	pTask->accessExternalObject (
	    V_BlockString (pStringStore, DSC_Descriptor_Scalar_Int (rCurrent)), bUsingDirectory
	);
    else {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rCurrent));

	M_CPD *pCharUV = rtLSTORE_CPD_ContentStringsCPD (pStringStore);
	pTask->accessExternalObject (
	    V_LStoreString (pStringStore, pCharUV, DSC_Descriptor_Scalar_Int (rCurrent)), bUsingDirectory
	);
	pCharUV->release ();
    }
}


/******************************************
 *****  Time Stamp Access Primitives  *****
 ******************************************/

/************
 *  Access  *
 ************/

V_DefinePrimitive (AccessTimeStamp) {
    struct time time;

/*****  Obtain the timestamp, ...  *****/
    bool notHandled = false;
    switch (V_TOTSC_Primitive) {
    case XAccessResourceTimeStamp:
	switch (V_TOTSC_PrimitiveFlags) {
	case 1:
	    time = PS_NVD_CommitTimeStamp (
		pTask->codDatabase ()->PhysicalAND ()->UpdateNVD ()
	    );
	    break;
	default:
	    notHandled = true;
	    break;
	}
	break;

    case XAccessTimeStamp:
	gettime (&time);
	break;

    default:
	notHandled = true;
	break;
    }
    if (notHandled)
        pTask->raiseUnimplementedAliasException ("AccessTimeStamp");

/*****  ... and pass it to the recipient block.  *****/
    pTask->beginValueCall (2);

    if (pTask->parameterCount () > 0)
	pTask->loadDucWithNextParameter ();
    else
	pTask->loadDucWithCurrent ();

    pTask->commitRecipient ();

    pTask->loadDucWithInteger ((int)time.seconds);
    pTask->commitParameter ();

    pTask->loadDucWithInteger ((int)time.microseconds);
    pTask->commitParameter ();

    pTask->commitCall ();
}


/**************
 *  Decoding  *
 **************/

extern "C" {
    typedef struct tm *(__cdecl *TimeStampDecoder)(time_t const *, struct tm*);
}

V_DefinePrimitive (DecodeTimeStamp) {
/*****  Determine the 'decoder' function, ...  *****/
    TimeStampDecoder decoder;

    switch (V_TOTSC_Primitive) {
    case XDecodeTZTimeStamp:
	decoder = localtime_r;
	break;
    case XDecodeGMTimeStamp:
	decoder = gmtime_r;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("DecodeTimeStamp");
	break;
    }

/*****  ... begin the value value call, ...  *****/
    pTask->beginValueCall (9);

/*****  ... obtain the 'return' block, ...  *****/
    pTask->loadDucWithNextParameter ();
    pTask->commitRecipient ();

/*****  ... obtain the time stamp, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... and decode it:  *****/
    struct tm iTimeParts;
    if (rCurrent.isScalar ()) {
	time_t iTime = DSC_Descriptor_Scalar_Int (rCurrent);
	struct tm *time = decoder (&iTime, &iTimeParts);

	pTask->loadDucWithInteger (time->tm_year);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_mon);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_mday);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_hour);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_min);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_sec);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_yday);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_wday);
	pTask->commitParameter ();

	pTask->loadDucWithInteger (time->tm_isdst);
	pTask->commitParameter ();
    }


    else {
	M_CPD		*cpd_isdst	, *cpd_wday	, *cpd_yday,
			*cpd_sec	, *cpd_min	, *cpd_hour,
			*cpd_mday	, *cpd_mon	, *cpd_year,
			*cpd_src;
	int		*ptr_isdst	, *ptr_wday	, *ptr_yday,
			*ptr_sec	, *ptr_min	, *ptr_hour,
			*ptr_mday	, *ptr_mon	, *ptr_year,
			*ptr_src	, *ptr_limit;

	cpd_isdst	= pTask->NewIntUV ();
	cpd_wday	= pTask->NewIntUV ();
	cpd_yday	= pTask->NewIntUV ();
	cpd_sec		= pTask->NewIntUV ();
	cpd_min		= pTask->NewIntUV ();
	cpd_hour	= pTask->NewIntUV ();
	cpd_mday	= pTask->NewIntUV ();
	cpd_mon		= pTask->NewIntUV ();
	cpd_year	= pTask->NewIntUV ();

	ptr_isdst	= rtINTUV_CPD_Array (cpd_isdst	);
	ptr_wday	= rtINTUV_CPD_Array (cpd_wday	);
	ptr_yday	= rtINTUV_CPD_Array (cpd_yday	);
	ptr_sec		= rtINTUV_CPD_Array (cpd_sec	);
	ptr_min		= rtINTUV_CPD_Array (cpd_min	);
	ptr_hour	= rtINTUV_CPD_Array (cpd_hour	);
	ptr_mday	= rtINTUV_CPD_Array (cpd_mday	);
	ptr_mon		= rtINTUV_CPD_Array (cpd_mon	);
	ptr_year	= rtINTUV_CPD_Array (cpd_year	);

	cpd_src		= DSC_Descriptor_Value (rCurrent);
	ptr_src		= rtINTUV_CPD_Array (cpd_src);
	ptr_limit	= ptr_src + UV_CPD_ElementCount (cpd_src);

	while (ptr_src < ptr_limit) {
	    time_t iTime = *ptr_src++;
	    struct tm *time = decoder (&iTime, &iTimeParts);
	    *ptr_isdst++= time->tm_isdst;
	    *ptr_wday++	= time->tm_wday;
	    *ptr_yday++	= time->tm_yday;
	    *ptr_sec++	= time->tm_sec;
	    *ptr_min++	= time->tm_min;
	    *ptr_hour++	= time->tm_hour;
	    *ptr_mday++	= time->tm_mday;
	    *ptr_mon++	= time->tm_mon;
	    *ptr_year++	= time->tm_year;
	}

	pTask->loadDucWithMonotype (cpd_year);
	cpd_year->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_mon);
	cpd_mon->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_mday);
	cpd_mday->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_hour);
	cpd_hour->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_min);
	cpd_min->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_sec);
	cpd_sec->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_yday);
	cpd_yday->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_wday);
	cpd_wday->release ();
	pTask->commitParameter ();

	pTask->loadDucWithMonotype (cpd_isdst);
	cpd_isdst->release ();
	pTask->commitParameter ();
    }
    
    pTask->commitCall ();
}


/****************
 *  Formatting  *
 ****************/

PrivateFnDef char const *TimeStampString (bool reset, va_list ap) {
    static TimeStampDecoder	decoder;
    static int			*ptr, *limit;
    static char			buffer[256];

    if (reset) {
	V::VArgList iArgList (ap);
	decoder	= iArgList.arg<TimeStampDecoder>();
	ptr	= iArgList.arg<int *>();
	limit	= iArgList.arg<int *>();
	return NilOf (char const*);
    }
    if (ptr >= limit)
	return NilOf (char const*);

    struct tm iTimeParts;
    time_t iTime = *ptr++;
    struct tm*pTimeParts = decoder (&iTime, &iTimeParts);
    if (!pTimeParts || 0 == strftime (buffer, sizeof (buffer), "%a %b %d %X %Z %Y", pTimeParts))
	buffer[0] = '\0';

    return buffer;
}


V_DefinePrimitive (FormatTimeStamp) {
/*****  Determine the 'decoder' function, ...  *****/
    TimeStampDecoder decoder;

    switch (V_TOTSC_Primitive) {
    case XFormatTZTimeStamp:
	decoder = localtime_r;
	break;
    case XFormatGMTimeStamp:
	decoder = gmtime_r;
	break;
    default:
        pTask->raiseUnimplementedAliasException ("FormatTimeStamp");
	break;
    }

/*****  ... obtain the time stamp, ...  *****/
    pTask->loadDucWithCurrent ();

/*****  ... and decode and return it:  *****/
    pTask->loadDucWithListOrStringStore (
	DucIsAScalar ? rtLSTORE_NewStringStore (
	    pTask->codScratchPad (),
	    TimeStampString,
	    decoder,
	    &DSC_Descriptor_Scalar_Int (ADescriptor),
	    &DSC_Descriptor_Scalar_Int (ADescriptor) + 1
	) : rtLSTORE_NewStringStore (
	    pTask->codScratchPad (),
	    TimeStampString,
	    decoder,
	    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor)),
	    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor))
	    + UV_CPD_ElementCount (DSC_Descriptor_Value (ADescriptor))
	)
    );
}


/***********************************
 *****  Evaluation Primitives  *****
 ***********************************/

/*********************************
 *  String Evaluation Primitive  *
 *********************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to evaluate a string.
 *
 *  Argument:
 *	source			- the source string to execute.
 *
 *  Notes:
 *	This routine expects to find the dictionary supplying object in
 *	the duc.
 *
 *****/
PrivateFnDef void CompileAndRun (VPrimitiveTask *pTask, char const *source) {
/*****  Access the local definition dictionary, ...  *****/
    VCPDReference pDictionary (0, pTask->ducDictionary ());

/*****  ... and compile the string.  *****/
    char messageBuffer[256];
    int	 errorLine, errorCharacter;
    VCPDReference pCompiledBlock (
	0, RSLANG_Compile (
	    pTask->codScratchPad (),
	    source,
	    pDictionary,
	    messageBuffer,
	    sizeof (messageBuffer),
	    &errorLine,
	    &errorCharacter
	)
    );

/*****  Schedule the block if it compiled, ...  *****/
    if (pCompiledBlock.isntNil ()) {
	pTask->beginBoundCall	(pCompiledBlock);
	pTask->commitRecipient	();
	pTask->commitCall	();
    }


/*****  ... otherwise, schedule the compilation error handler:  *****/
    else {
	pTask->beginValueCall (3);
	pTask->loadDucWithNextParameter();
	pTask->commitRecipient ();

	pTask->loadDucWithCurrent	();
	pTask->commitParameter		();	/*  compiled string  */

	pTask->loadDucWithInteger	(errorLine);
	pTask->commitParameter		();	/*  error line number  */

	pTask->loadDucWithInteger	(errorCharacter);
	pTask->commitParameter ();	/*  error character position  */

	pTask->commitCall ();
    }
}


/*---------------------------------------------------------------------------*
 *****  String Evaluate Subtask Generator.
 *---------------------------------------------------------------------------*/

class VStringEvaluateSubtaskGenerator : public VTransient {
//  Construction
public:
    VStringEvaluateSubtaskGenerator (VPrimitiveTask *pTask);

//  Destruction
public:
    ~VStringEvaluateSubtaskGenerator () {
	if (m_pSubtaskSubset)
	    m_pSubtaskSubset->release ();
    }

//  Subtask Creation
public:
    void openSubtask ();
    void appendToSubtask (unsigned int element, unsigned int count);
    void closeSubtask ();

    void scheduleSubtasks ();

//  State
protected:
    VPrimitiveTask *const	m_pTask;
    unsigned int		m_iSubtaskSize;
    rtLINK_CType*		m_pSubtaskSubset;
    DSC_Descriptor		m_iEmpty;
    VDescriptor			m_iDictionarySource;
    VFragmentation*		m_pDucFragmentation;
};


/*---------------------------------------------------------------------------
 *****  String evaluate subtask generator constructor.  This routine expects
 *****	to find the dictionary supplying object in the duc.
 *****/
VStringEvaluateSubtaskGenerator::VStringEvaluateSubtaskGenerator (VPrimitiveTask *pTask)
: m_pTask (pTask), m_iSubtaskSize (0), m_pSubtaskSubset (0)
{
    m_iEmpty.construct ();

    m_iDictionarySource.setToMoved (pTask->duc ());
    m_pDucFragmentation = &pTask->loadDucWithFragmentation ();
}


void VStringEvaluateSubtaskGenerator::openSubtask () {
    closeSubtask ();
    m_pSubtaskSubset = m_pTask->NewSubset ();
}

void VStringEvaluateSubtaskGenerator::appendToSubtask (
    unsigned int element, unsigned int count
) {
    rtLINK_AppendRange (m_pSubtaskSubset, element, count);
    m_iSubtaskSize += count;
}

void VStringEvaluateSubtaskGenerator::closeSubtask () {
    if (IsNil (m_pSubtaskSubset))
	return;

    M_CPD *pSubtaskPToken = m_pTask->NewDomPToken (m_iSubtaskSize);
    m_pSubtaskSubset->Close (pSubtaskPToken);

    m_pTask->blockContext ()->retain (); m_pSubtaskSubset->retain ();
    DSC_Descriptor iParent;
    iParent.constructLink (m_pTask->blockContext (), m_pSubtaskSubset);

    m_pDucFragmentation->createFragment (m_pSubtaskSubset)->datum().setToIdentity (
	new rtCLOSURE_Constructor (
	    new rtCONTEXT_Constructor (
		m_iEmpty, m_iEmpty, m_iEmpty, iParent
	    ), m_pTask->primitive ()
	), pSubtaskPToken
    );

    m_iSubtaskSize = 0;
    m_pSubtaskSubset = NilOf (rtLINK_CType*);
}

void VStringEvaluateSubtaskGenerator::scheduleSubtasks () {
    closeSubtask ();

    m_pTask->beginValueCall	(2);
    m_pTask->commitRecipient	();

    m_pTask->loadDucWithNextParameter ();
    m_pTask->commitParameter	();		/* compilation error handler */

    m_pTask->loadDucWithMoved	(m_iDictionarySource);
    m_pTask->commitParameter	();		/* dictionary source */

    m_pTask->commitCall		();
}


V_DefinePrimitive (StringEvaluate) {
/*****  Access the dictionary supplying object, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->beginMessageCall (KS__WithCompilationErrorHandlerEvaluate);
	pTask->commitRecipient ();

	pTask->loadDucWithNextParameter ();
	pTask->commitParameter ();

	pTask->loadDucWithSelf ();
	pTask->commitParameter ();

	pTask->commitCall ();
	return;
    }

    pTask->normalizeDuc ();

/*****  Access the string store, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pStringStore = rCurrent.storeCPD ();

/*****  ... and compile the string.  *****/
    if (RTYPE_C_Block == M_CPD_RType (pStringStore)) {
	if (rCurrent.isScalar ()) CompileAndRun (
	    pTask, V_BlockString (
		pStringStore, DSC_Descriptor_Scalar_Int (rCurrent)
	    )
	);
	else if (DSC_Descriptor_Value (rCurrent).isACoercedScalar ()) CompileAndRun (
	    pTask, V_BlockString (
		pStringStore, rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent))[0]
	    )
	);
	else {
	    unsigned int elementCount = pTask->cardinality ();
	    unsigned int const *stringIndices = (unsigned int *)rtINTUV_CPD_Array (
		DSC_Descriptor_Value (rCurrent)
	    );
	    unsigned int *sortIndices = (unsigned int *)UTIL_Malloc (
		elementCount * sizeof (unsigned int)
	    );
	    UTIL_RadixListSort (stringIndices, sortIndices, elementCount);

	    VStringEvaluateSubtaskGenerator iGenerator (pTask);

	    unsigned int stringIndex = 0; // ... keep the compiler happy (false ref before use warning)
	    for (unsigned int elementIndex = 0; elementIndex < elementCount; elementIndex++) {
		if (elementIndex < 1 || stringIndex != stringIndices[sortIndices[elementIndex]]) {
		    iGenerator.openSubtask ();
		    iGenerator.appendToSubtask (sortIndices[elementIndex], 1);
		    stringIndex = stringIndices[sortIndices[elementIndex]];
		}
		else iGenerator.appendToSubtask (sortIndices[elementIndex], 1);
	    }
	    iGenerator.scheduleSubtasks ();

	    UTIL_Free (sortIndices);
	}
    }


    else {
	M_CPD *pCharUV = rtLSTORE_CPD_ContentStringsCPD (pStringStore);

	if (rCurrent.isScalar ()) {
	    rtREFUV_AlignReference (&DSC_Descriptor_Scalar (rCurrent));
	    CompileAndRun (
		pTask, V_LStoreString (
		    pStringStore, pCharUV, DSC_Descriptor_Scalar_Int (rCurrent)
		)
	    );
	}
	else {
	    bool aBlockHasBeenScheduled = false;

	    rtLINK_CType *linkc = DSC_Descriptor_Link(rCurrent)->Align();
	    if (1 == linkc->ElementCount () ||
		1 == rtLINK_LC_RRDCount (linkc) && rtLINK_LC_HasRepeats (linkc)
	    ) {
#		define handleAnyCase(position,count,origin) {\
		    if (aBlockHasBeenScheduled) pTask->raiseException (\
			EC__InternalInconsistency,\
			"StringEvaluate: Non-Unique String Encountered"\
		    );\
		    else CompileAndRun (\
			pTask, V_LStoreString (pStringStore, pCharUV, origin)\
		    );\
		    aBlockHasBeenScheduled = true;\
		}
#ifdef __VMS
#pragma __message __save
#pragma __message __disable(SETBUTNOTUSED)
#endif
		rtLINK_TraverseRRDCList (linkc, handleAnyCase, handleAnyCase, handleAnyCase);
#ifdef __VMS
#pragma __message __restore
#endif
#		undef handleAnyCase
	    }
	    else {
#		define handleRange(position,count,origin) while (count-- > 0) {\
		    iGenerator.openSubtask ();\
		    iGenerator.appendToSubtask (position++, 1);\
		}

#		define handleRepeat(position,count,origin) {\
		    iGenerator.openSubtask ();\
		    iGenerator.appendToSubtask (position, count);\
		}

		VStringEvaluateSubtaskGenerator iGenerator (pTask);
		rtLINK_TraverseRRDCList (
		    DSC_Descriptor_Link (rCurrent), handleRepeat, handleRepeat, handleRange
		);
		iGenerator.scheduleSubtasks ();

#		undef handleRange
#		undef handleRepeat
	    }
	}

	pCharUV->release ();
    }
}


/********************************************
 *  Context Switched Evaluation Primitives  *
 ********************************************/

/***************************
 *----  Continuations  ----*
 ***************************/

V_DefinePrimitive (SendForExecutionContinuation) {
    pTask->loadDucWithSelf ();
}

V_DefinePrimitive (SendContinuation) {
    VCall::ReturnCase xReturnCase = VCall::Return_Value;

    switch (V_TOTSC_Primitive) {
    case XSendForExecution:
	pTask->setContinuationTo (SendForExecutionContinuation);
	break;
    case XSendForValue:
	break;
    case XSendForEnvironment:
	xReturnCase = VCall::Return_Current;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("Send");
	break;
    }

    pTask->beginValueCall   (pTask->parameterCount () - 1);
    pTask->commitRecipient  ();
    pTask->commitParameters (1);
    pTask->commitCall	    (xReturnCase);
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (Send) {
    pTask->setContinuationTo (SendContinuation);

    pTask->loadDucWithParameter (0);
    if (!pTask->ConvertDucVectorsToMonotypeIfPossible () || RTYPE_C_Closure != pTask->ducStoreRType ())
	pTask->sendBinaryConverseWithSelf (KS__AsClosure);
    else {
/*****  Initialize 'my' and access the block or primitive:  *****/
	DSC_Descriptor iMy;
	iMy.construct ();

	pTask->normalizeDuc ();
	DSC_Descriptor& rDucMonotype = pTask->ducMonotype ();

	rtBLOCK_Handle *pBlock = 0; unsigned int xPrimitive = 0;
	M_CPD *closureCPD; rtCLOSURE_Constructor *closure = static_cast<rtCLOSURE_Constructor*> (
	    rDucMonotype.storePOIfAvailable ()
	);
	if (closure) {
	    if (closure->isAPrimitiveClosure ())
		xPrimitive = closure->primitive ();
	    else {
		rtCONTEXT_Constructor *context = closure->context ();
		DSC_Descriptor& rClosureCurrent = context->getCurrent ();
		iMy.constructComposition (
		    DSC_Descriptor_Pointer (rDucMonotype), rClosureCurrent
		);
		pBlock = closure->block ();
	    }
	}
	else if (rtCLOSURE_CPD_IsAPrimClosure (closureCPD = rDucMonotype.storeCPDIfAvailable ()))
	    xPrimitive = rtCLOSURE_CPD_Primitive (closureCPD);
	else {
	    DSC_Descriptor dsc; {
		VCPDReference pContext (
		    closureCPD, rtCLOSURE_CPx_Context, RTYPE_C_Context
		);
		VCPDReference pDescriptor (
		    pContext, rtCONTEXT_CPx_Current, RTYPE_C_Descriptor
		);
		rtDSC_Unpack (pDescriptor, &dsc);
	    }
	    iMy.constructComposition (DSC_Descriptor_Pointer (rDucMonotype), dsc);
	    dsc.clear ();

	    pBlock = static_cast<rtBLOCK_Handle*>(closureCPD->GetContainerHandle (rtCLOSURE_CPx_Block, RTYPE_C_Block));
	}

/*****  Initialize 'self', 'current', and 'parent':  *****/
	DSC_Descriptor& rSelf = pTask->getSelf ();
	DSC_Descriptor	iSelf;
	iSelf.construct (rSelf);

	DSC_Descriptor	iCurrent;
	iCurrent.construct (iSelf);

/*****  Create the task descriptor:  *****/
	rtCONTEXT_Constructor *pContext = new rtCONTEXT_Constructor (iSelf, iCurrent, iMy);
	pTask->loadDucWithIdentity (
	    pBlock ? new rtCLOSURE_Constructor (
		pContext, pBlock, rDucMonotype.storeMask ()
	    ) : new rtCLOSURE_Constructor (
		pContext, xPrimitive, rDucMonotype.storeMask ()
	    )
	);
    }
}


/**************************************
 *  '^current' Evaluation Primitives  *
 **************************************/

V_DefinePrimitive (Evaluate) {
   switch (pTask->flags ()) {
    case 1:
	pTask->newDescendentProfiler ();
	break;
    }
    pTask->beginValueCall	(pTask->parameterCount ());
    pTask->commitRecipient	(VMagicWord_Current);
    pTask->commitParameters	();
    pTask->commitCall (
	XEvaluateForEnvironment == V_TOTSC_Primitive
	    ? VCall::Return_Current
	    : VCall::Return_Value
    );
}


/****************************************
 *  Local Context Evaluation Primitive  *
 ****************************************/

V_DefinePrimitive (EvaluateInLocalContext) {
    pTask->setLocalContextTo (pTask->getSelf ());

    pTask->beginValueCall (0);
    pTask->loadDucWithNextParameter ();
    pTask->commitRecipient ();
    pTask->commitCall ();
}


/************************************
 *  Parameter Evaluation Primitive  *
 ************************************/

V_DefinePrimitive (EvaluateParameter) {
    switch (V_TOTSC_Primitive) {
    case XEvaluatePenultimateParameter:
	pTask->skipParameter ();
	break;
    case XEvaluateUltimateParameter:
	break;
    default:
	pTask->raiseUnimplementedAliasException ("EvaluateParameter");
        break;
    }

    pTask->beginValueCall (0);
    pTask->loadDucWithNextParameter ();
    pTask->commitRecipient ();
    pTask->commitCall ();
}


/*****************************************
 *****  Dated Evaluation Primitives  *****
 *****************************************/

V_DefinePrimitive (ForDateEvalUltimateParameter) {
/*****  Obtain the parameter to this primitive...  *****/
    pTask->beginValueCall  (0);
    pTask->loadDucWithNextParameter ();
    pTask->commitRecipient ();

/*****  and schedule it to run in the appropriate temporal context.  *****/
    rtINDEX_Key *pTemporalContext = new rtINDEX_Key (
	V_TOTSC_PToken, &pTask->getCurrent ()
    );
    pTask->commitCall (pTemporalContext);
    pTemporalContext->release ();
}

V_DefinePrimitive (ForDatesInRangeContinuation) {
    pTask->loadDucWithNA ();
}

PrivateFnDef void SendConverseByToEval (
    VPrimitiveTask*		pTask,
    unsigned int		xSelector,
    VDescriptor&		rTarget,
    VDescriptor&		rDate,
    VDescriptor&		rEvaluable
)
{
/*****  Dispatch the message to 'rTarget' ...  *****/
    pTask->beginMessageCall (xSelector);
    pTask->loadDucWithMoved (rTarget);
    pTask->commitRecipient ();

/*****  Load 'by' date increment ... *****/
    pTask->loadDucWithCurrent ();
    pTask->commitParameter ();

/*****  Load either the initial or final date ...  *****/
/*****  (whichever isn't the recipient)            *****/
    pTask->loadDucWithMoved (rDate);
    pTask->commitParameter ();

/*****  Load the function to evaluate ... *****/
    pTask->loadDucWithMoved (rEvaluable);
    pTask->commitParameter ();

/*****  And schedule the call ...  *****/
    pTask->commitCall ();
}


V_DefinePrimitive (ForDatesInRangeEvaluate) {
    VDescriptor		functionXDsc,
			initialDateXDsc,
			finalDateXDsc;
    DSC_Descriptor	keyDsc;
    M_CPD		*ptoken, *dateUV;
    int			dateIncrementType;

#define initialDateDsc	initialDateXDsc.contentAsMonotype ()
#define finalDateDsc	finalDateXDsc.contentAsMonotype ()

/*****  Decode the alias by which this primitive was invoked...  *****/
    switch (V_TOTSC_Primitive) {
    case XByDaysForDateEval:
	dateIncrementType = DATES_DateIncr_DAY;
	break;
    case XByBDaysForDateEval:
	dateIncrementType = DATES_DateIncr_BDAY;
	break;
    case XByMonthsForDateEval:
	dateIncrementType = DATES_DateIncr_MONTH;
	break;
    case XByMonthBeginningsForDateEval:
	dateIncrementType = DATES_DateIncr_MONTHBEGIN;
	break;
    case XByMonthEndsForDateEval:
	dateIncrementType = DATES_DateIncr_MONTHEND;
	break;
    case XByQtrsForDateEval:
	dateIncrementType = DATES_DateIncr_QUARTER;
	break;
    case XByQtrBeginningsForDateEval:
	dateIncrementType = DATES_DateIncr_QUARTERBEGIN;
	break;
    case XByQtrEndsForDateEval:
	dateIncrementType = DATES_DateIncr_QUARTEREND;
	break;
    case XByYearsForDateEval:
	dateIncrementType = DATES_DateIncr_YEAR;
	break;
    case XByYearBeginningsForDateEval:
	dateIncrementType = DATES_DateIncr_YEARBEGIN;
	break;
    case XByYearEndsForDateEval:
	dateIncrementType = DATES_DateIncr_YEAREND;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ForDatesInRangeEvaluate");
	break;
    }

/*****  Obtain the increment amount ... *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... obtain the function to be evaluated, ...  *****/
    pTask->loadDucWithNextParameter ();
    functionXDsc.setToMoved (pTask->duc ());

/*****  ... obtain the final date ...  *****/
    pTask->loadDucWithNextParameter ();
    finalDateXDsc.setToMoved (pTask->duc ());

/*****  ... obtain the initial date ...  *****/
    pTask->loadDucWithNextParameter ();
    initialDateXDsc.setToMoved (pTask->duc ());

/*****
 *  If either of the date arguments are polymorphic, or not 'dates'
 *  then send the converse message.
 *****/
    if (!initialDateXDsc.isStandard() || initialDateDsc.storeCPD()->DoesntNameTheDateClass()) {
	SendConverseByToEval (
	    pTask, KS__ByToEvaluate, initialDateXDsc, finalDateXDsc, functionXDsc
	);
	return;
    }

    if (!finalDateXDsc.isStandard() || finalDateDsc.storeCPD()->DoesntNameTheDateClass()) {
	SendConverseByToEval (
	    pTask, KS__ByFromEvaluate, finalDateXDsc, initialDateXDsc, functionXDsc
	);
	return;
    }

    initialDateXDsc.normalize ();
    finalDateXDsc.normalize ();

/*****
 *  Create a key and an expansion link for the enumeration of the date ranges
 *****/
    rtLINK_CType* pExpansionLink = pTask->NewSubset ();

/*****  Scalar case ... *****/
    if (initialDateDsc.isScalar () || initialDateDsc.holdsAScalarIdentity ()) {
	rtINTUV_ElementType	*uvp;

/*****  Create the integer uvector to hold all the dates ... *****/
	unsigned int totalDateCount = DATES_EnumerateDatesInRange (  /*Count??? */
	    DSC_Descriptor_Scalar_Int (initialDateDsc),
	    DSC_Descriptor_Scalar_Int (finalDateDsc),
	    DSC_Descriptor_Scalar_Int (rCurrent),
	    dateIncrementType,
	    NilOf (rtINTUV_ElementType *),
	    true
	);

	ptoken = pTask->NewCodPToken (totalDateCount);
	dateUV = pTask->NewDateUV (ptoken);
	uvp = rtINTUV_CPD_Array (dateUV);

	unsigned int dateCount = DATES_EnumerateDatesInRange (
	    DSC_Descriptor_Scalar_Int (initialDateDsc),
	    DSC_Descriptor_Scalar_Int (finalDateDsc),
	    DSC_Descriptor_Scalar_Int (rCurrent),
	    dateIncrementType,
	    uvp,
	    false
	);

        /*****  Fill in 'pExpansionLink' ... *****/
	rtLINK_AppendRepeat (pExpansionLink, 0, dateCount);

	if (dateCount != totalDateCount) pTask->raiseException (
	    EC__InternalInconsistency,
	    "PFutil: DateRangeEnumerate - Total date counts do not match"
	);

	pExpansionLink->Close (ptoken);
    }


/***** ... non-scalar case ... *****/
    else {
	M_CPD		*initialDateUV = DSC_Descriptor_Value (initialDateDsc),
			*finalDateUV   = DSC_Descriptor_Value (finalDateDsc),
			*incrementUV   = DSC_Descriptor_Value (rCurrent);
	rtINTUV_ElementType
			*uvp,
			*idp = rtINTUV_CPD_Array (initialDateUV),
			*fdp = rtINTUV_CPD_Array (finalDateUV),
			*incp = rtINTUV_CPD_Array (incrementUV),
			*incpl = incp + UV_CPD_ElementCount (incrementUV);


/*****  Create the integer uvector to hold all the dates ... *****/
	unsigned int totalDateCount1 = 0;
	while (incp < incpl) totalDateCount1 += DATES_EnumerateDatesInRange (
	    *idp++,
	    *fdp++,
	    *incp++,
	    dateIncrementType,
	    NilOf (rtINTUV_ElementType *),
	    true
	);

	ptoken = pTask->NewCodPToken (totalDateCount1);
	dateUV = pTask->NewDateUV (ptoken);
	uvp = rtINTUV_CPD_Array (dateUV);

/*****  Reinitialize all the pointers ... *****/
	idp = rtINTUV_CPD_Array (initialDateUV);
	fdp = rtINTUV_CPD_Array (finalDateUV);
	incp = rtINTUV_CPD_Array (incrementUV);

/******  Enumerate the date ranges ... *****/
	unsigned int totalDateCount2 = 0;
	unsigned int listCount = 0;
	while (incp < incpl) {
	    /*****  Fill in the date uvector ... *****/
	    unsigned int dateCount = DATES_EnumerateDatesInRange (
		*idp++, *fdp++, *incp++, dateIncrementType, uvp, false
	    );
	    uvp += dateCount;
	    totalDateCount2 += dateCount;

	    /*****  Fill in 'pExpansionLink' ... *****/
	    rtLINK_AppendRepeat (pExpansionLink, listCount, dateCount);
	    listCount++;
	}

	if (totalDateCount1 != totalDateCount2) pTask->raiseException (
	    EC__InternalInconsistency,
	    "PFutil: DateRangeEnumerate - Total date counts do not match"
	);

	pExpansionLink->Close (ptoken);
    }

/*****  Make dateUV into a key ... *****/
    keyDsc.constructMonotype (dateUV);
    dateUV->release ();

    rtINDEX_Key *pTemporalContext = new rtINDEX_Key (ptoken, &keyDsc);
    ptoken->release ();
    keyDsc.clear ();

/*****  Set the continuation, ... *****/
    pTask->setContinuationTo (ForDatesInRangeContinuation);


/*****  Send a value message to the function, ... *****/
    pTask->beginMessageCall (KS__Value, pExpansionLink);
    pTask->loadDucWithMoved (functionXDsc);
    pTask->restrictDuc (pExpansionLink);
    pTask->commitRecipient ();

/*****  ... and run the tasks created:  *****/
    pTask->commitCall (pTemporalContext);
    pExpansionLink->release ();
    pTemporalContext->release ();
}


/*****************************************
 *****  Class Conversion Primitives  *****
 *****************************************/

PrivateFnDef char const *SelectorTraverseStrings (bool reset, va_list ap) {
    V::VArgList iArgList (ap);
    int* beginPtr = iArgList.arg<int *>();
    int* endPtr   = iArgList.arg<int *>();
    int**workPtr  = iArgList.arg<int **>();

    if (reset) {
	*workPtr = beginPtr;
	return NilOf (char const*);
    }
    if (*workPtr < endPtr)
	return KS__ToString (*(*workPtr)++);

    return NilOf (char const*);
}


V_DefinePrimitive (SelectorToString) {
/*****  Obtain the selector(s) ... *****/
    pTask->loadDucWithCurrent ();

/*****  Scalar Case  *****/
    RTYPE_Type rtype = pTask->ducStoreRType ();

    if (DucIsAScalar) {
	switch (rtype) {
	/***  Known Selectors  ***/
	case RTYPE_C_ValueStore:
	    pTask->loadDucWithListOrStringStore (
		rtLSTORE_NewStringStore (
		    pTask->codScratchPad (), KS__ToString (
			DSC_Descriptor_Scalar_Int (ADescriptor)
		    )
		)
	    );
	    break;

	/***  User Selectors  ***/
	case RTYPE_C_Block:
	    rtype = ADescriptor.pointerRType ();

	    /***  Block String ***/
	    if (rtype == RTYPE_C_IntUV) {
		DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (ADescriptor))->release ();

		M_CPD *pRPT = pTask->ducStore ()->TheStringClass ().PTokenCPD ();
		pRPT->retain ();
		DSC_Scalar_RefPToken (DSC_Descriptor_Scalar (ADescriptor)) = pRPT;
	    }
	    /*** LStore String - return as is ***/
	    break;

	default:
	    pTask->raiseException (
		EC__InternalInconsistency,
		"SelectorToString: A '%s' Cannot Hold Selectors",
		RTYPE_TypeIdAsString (rtype)
	    );
	    break;
	}
    }

/*****  Non-scalar case  *****/
    else {
	int *ksip;

	switch (rtype) {
	/***  Known Selectors  ***/
	case RTYPE_C_ValueStore:
	    pTask->loadDucWithListOrStringStore (
		rtLSTORE_NewStringStore (
		    pTask->codScratchPad (),
		    SelectorTraverseStrings,
		    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor)),
		    rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor))
		    + UV_CPD_ElementCount (DSC_Descriptor_Value (ADescriptor)),
		    &ksip
		)
	    );
	    break;

        /***  User Selectors  ***/
	case RTYPE_C_Block:
	    rtype = ADescriptor.pointerRType ();

	    /***  Block Strings  ***/
	    if (rtype == RTYPE_C_IntUV) {
		DSC_Descriptor_Value (ADescriptor)->StoreReference (
		    UV_CPx_RefPToken, pTask->ducStore()->TheStringClass().PTokenCPD ()
		);
	    }
	    /***  LStore strings - OK as is ***/
	    break;

	default:
	    pTask->raiseException (
		EC__InternalInconsistency,
		"SelectorToString: A '%s' Cannot Hold Selectors",
		RTYPE_TypeIdAsString (rtype)
	    );
	    break;
	}
    }
}


/*******************************************
 *****  Closure and Method Primitives  *****
 *******************************************/

/****************************************
 *  Closure / Method Access Primitives  *
 ****************************************/


/*---------------------------------------------------------------------------
 *****  Internal utility to load the accumulator with a descriptor for the
 *****  selector associated with a block.
 *
 *  Arguments:
 *	block			- a standard CPD for the block whose selector
 *				  is to be accessed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void LoadAWithBlockSelector (VPrimitiveTask *pTask, M_CPD *block) {
    if (IsNil (block) || rtBLOCK_CPD_NoSelector (block))
	pTask->loadDucWithNA ();
    else if (rtBLOCK_CPD_SelectorIsKnown (block))
        pTask->loadDucWithSelector (rtBLOCK_CPD_SelectorIndex (block));
    else
        pTask->loadDucWithSelector (block, rtBLOCK_CPD_SelectorIndex (block));
}


/*---------------------------------------------------------------------------
 *****  Closure selector access primitive.
 *
 *  Invocation Synopsis:
 *	aClosure selector
 *
 *  Returns:
 *	'NA' or a descriptor for closure block's selector.
 *
 *****/
V_DefinePrimitive (ClosureSelector) {
/*****  Obtain '^current'...  *****/
    pTask->loadDucWithCurrent ();

/*****  Obtain the block associated with this closure...  *****/
    rtBLOCK_Handle::Reference block; unsigned int primitive;
    pTask->decodeClosureInDuc (block, primitive);

/*****  ... and load the accumulator with an 'undefined' or the selector:  *****/
    VCPDReference pBlockCPD (0, block ?  block->GetCPD () : 0);
    LoadAWithBlockSelector (pTask, pBlockCPD);
}


/*---------------------------------------------------------------------------
 *****  Method selector primitive.
 *
 *  Invocation Synopsis:
 *	aMethod selector
 *
 *  Returns:
 *	'NA' or a descriptor for the method block's selector.
 *
 *****/
V_DefinePrimitive (MethodSelector)
{
/*****  Obtain '^current'...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Obtain a CPD for the block associated with this method...  *****/
    M_CPD *method = rCurrent.storeCPD ();
    M_CPD *block = rtMETHOD_CPD_BlockCPD (method);

/*****  ... load the accumulator with an 'undefined' or the selector...  *****/
    LoadAWithBlockSelector (pTask, block);

/*****  ... and clean up.  *****/
    block->release ();
}


/*---------------------------------------------------------------------------
 *****  Closure to method conversion primitive.
 *
 *  Invocation Synopsis:
 *	aClosure asMethod
 *
 *  Returns:
 *	A method constructor from the closure.
 *
 *****/
V_DefinePrimitive (ClosureAsMethod) {
//  Get the closure and decode it, ...
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    VReference<rtBLOCK_Handle> pBlock; unsigned int xPrimitive; rtCONTEXT_Constructor *pContext;
    rCurrent.decodeClosure (pBlock, xPrimitive, &pContext);

//  If the closure is a block context...  *****/
    if (pBlock) {
    /*****  ... construct and return a method:  *****/
	DSC_Descriptor& rClosureCurrent = pContext->getCurrent ();

	DSC_Descriptor iMethod;
	iMethod.constructComposition (DSC_Descriptor_Pointer (rCurrent), rClosureCurrent);

	M_CPD *pMethod = rtMETHOD_New (
	    pTask->codScratchPad (), pBlock, iMethod.storeCPD ()
	);
	pMethod->setAttentionMaskTo (rCurrent.storeMask ());
	iMethod.setStoreTo (pMethod);

	pTask->loadDucWithMoved (iMethod);
    }
    else {
    /*****  ... otherwise, return the index of the primitive:  *****/
        pTask->loadDucWithPrimitive (xPrimitive);
    }

//  And clean-up:
    pContext->release ();
}


/*---------------------------------------------------------------------------
 *****  Method as closure primitive.
 *
 *  Invocation Synopsis:
 *	aMethod asClosure: anObject
 *
 *  Returns:
 *	A closure in which 'anObject' fills the roles of 'self' and 'current'.
 *
 *****/
V_DefinePrimitive (MethodAsClosure) {
/*****
 *  Send this primitive's converse message to the parameter if the
 *  parameter is polymorphic...
 *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
        pTask->sendBinaryConverseWithCurrent (KS__ClosureOf);
	return;
    }

/*****  Set up the context registers...  *****/
    pTask->normalizeDuc ();

    M_CPD *distribution = ADescriptor.factor ();

    DSC_Descriptor iSelf;
    iSelf.construct ();
    iSelf.setToMoved (ADescriptor);

    DSC_Descriptor iCurrent;
    iCurrent.construct (iSelf);

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    DSC_Descriptor iMy;
    iMy.construct (rCurrent);

    if (distribution)
	iMy.reorder (distribution);

/*****  Obtain a reference to the positional P-Token of the context...  *****/
    M_CPD *posPToken = iCurrent.PPT ();

/*****  Obtain a CPD for the method...  *****/
    M_CPD *method = iMy.storeCPD ();
    method->retain ();

/*****
 *  Change 'origin's store to the store associated with the method...
 *****/
    iMy.setStoreTo (rtMETHOD_CPD_OriginCPD (method));

/*****  Create a descriptor for the new closure...  *****/
    ADescriptor.constructIdentity (
	new rtCLOSURE_Constructor (
	    new rtCONTEXT_Constructor (iSelf, iCurrent, iMy),
	    static_cast<rtBLOCK_Handle*> (
		method->GetContainerHandle (rtMETHOD_CPx_Block, RTYPE_C_Block)
	    ), method->attentionMask ()
	), posPToken
    );
    method->release ();

/*****
 *  And adjust its order if a distribution was factored from the recipient...
 *****/
    if (distribution) {
	pTask->distributeDuc (distribution);
	distribution->release ();
    }
}


/*---------------------------------------------------------------------------
 *****  Closure '^self' Access Primitive
 *
 *  Invocation Synopsis:
 *	aClosure self
 *
 *  Returns:
 *	The value of '^self' in the closure.
 *
 *****/
V_DefinePrimitive (ClosureSelf) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    DSC_Descriptor result;
    rtCLOSURE_Constructor *closureC = static_cast<rtCLOSURE_Constructor*> (rCurrent.storePOIfAvailable ());
    if (IsntNil (closureC)) {
	rtCONTEXT_Constructor *contextC = closureC->context ();
	DSC_Descriptor& rClosureSelf = contextC->getSelf ();
	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), rClosureSelf);
    }
    else {
	M_CPD *contextCPD = rtCLOSURE_CPD_ContextCPD (rCurrent.storeCPDIfAvailable ());
	M_CPD *pDescriptor = rtCONTEXT_CPD_SelfCPD (contextCPD);

	DSC_Descriptor dsc;
	rtDSC_Unpack (pDescriptor, &dsc);

	contextCPD->release ();
	pDescriptor->release ();

	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), dsc);
	dsc.clear ();
    }

    pTask->loadDucWithMoved (result);
}


/*---------------------------------------------------------------------------
 *****  Closure '^current' Access Primitive
 *
 *  Invocation Synopsis:
 *	aClosure current
 *
 *  Returns:
 *	The value of '^current' in the closure.
 *
 *****/
V_DefinePrimitive (ClosureCurrent) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    DSC_Descriptor result;
    rtCLOSURE_Constructor *closureC = static_cast<rtCLOSURE_Constructor*> (rCurrent.storePOIfAvailable ());
    if (IsntNil (closureC)) {
	rtCONTEXT_Constructor *contextC = closureC->context ();
	DSC_Descriptor& rClosureCurrent = contextC->getCurrent ();
	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), rClosureCurrent);
    }
    else {
	M_CPD *contextCPD = rtCLOSURE_CPD_ContextCPD (rCurrent.storeCPDIfAvailable ());
	M_CPD *pDescriptor = rtCONTEXT_CPD_CurrentCPD (contextCPD);

	DSC_Descriptor dsc;
	rtDSC_Unpack (pDescriptor, &dsc);

	contextCPD->release ();
	pDescriptor->release ();

	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), dsc);
	dsc.clear ();
    }

    pTask->loadDucWithMoved (result);
}


/*---------------------------------------------------------------------------
 *****  Closure '^origin' Access Primitive
 *
 *  Invocation Synopsis:
 *	aClosure current
 *
 *  Returns:
 *	The value of '^origin' in the closure.
 *
 *****/
V_DefinePrimitive (ClosureOrigin) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    DSC_Descriptor result;
    rtCLOSURE_Constructor *closureC = static_cast<rtCLOSURE_Constructor*> (rCurrent.storePOIfAvailable ());
    if (IsntNil (closureC)) {
	rtCONTEXT_Constructor *contextC = closureC->context ();
	DSC_Descriptor& rClosureOrigin = contextC->getMy ();

	if (rClosureOrigin.isEmpty ()) {
	    pTask->loadDucWithNA ();
	    return;
	}

	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), rClosureOrigin);
    }
    else {
	M_CPD *contextCPD = rtCLOSURE_CPD_ContextCPD (rCurrent.storeCPDIfAvailable ());
	M_CPD *pDescriptor = rtCONTEXT_CPD_OriginCPD (contextCPD);

	DSC_Descriptor	dsc;
	rtDSC_Unpack (pDescriptor, &dsc);

	contextCPD->release ();
	pDescriptor->release ();

	if (dsc.isEmpty ()) {
	    pTask->loadDucWithNA ();
	    return;
	}

	result.constructComposition (DSC_Descriptor_Pointer (rCurrent), dsc);
	dsc.clear ();
    }

    pTask->loadDucWithMoved (result);
}


/***********************************
 *****  Assignment Primitives  *****
 ***********************************/

V_DefinePrimitive (Assign) {
/*****  Obtain the vector, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_CPD *pVector = rCurrent.storeCPD ();

/*****  ... obtain the parameter, ...  *****/
    pTask->loadDucWithNextParameter ();

/*****  ... and perform the assignment.  *****/
    pTask->duc ().assignTo (DSC_Descriptor_Pointer (rCurrent), pVector);
}


/*********************************************
 *****  Property Maintenance Primitives  *****
 *********************************************/

V_DefinePrimitive (CleanStore) {
    bool deletingEmptyUSegments;

    switch (V_TOTSC_Primitive) {
    case XCleanStore:
	deletingEmptyUSegments = true;
	break;
    case XAlignStore:
	deletingEmptyUSegments = false;
        break;
    default:
	pTask->raiseUnimplementedAliasException ("CleanAndAlignStore");
	break;
    }

/*****  rtVSTORE_AlignAll does the r-type switch, ...  *****/
    pTask->loadDucWithBoolean (
	rtVSTORE_AlignAll (pTask->getSelf ().storeCPD (), deletingEmptyUSegments)
    );
}

V_DefinePrimitive (CleanDictionary) {
    M_CPD *pDictionaryCPD = pTask->getSelf ().dictionaryCPD ();
    pTask->loadDucWithBoolean (rtDICTIONARY_AlignAll (pDictionaryCPD, true));
    pDictionaryCPD->release ();
}


/******************************************
 *****  Update Parameter Maintenance  *****
 ******************************************/

/*****  Update Parameter Type Definition  *****/
struct SessionAttribute {
#if Vk_DataFormatNative == Vk_DataFormatBEndian
    unsigned short		spaceIndex,
				parameterIndex;
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    unsigned short		parameterIndex,
				spaceIndex;
#endif
};

/*****  Database Update Parameter Indices  *****/
#define NP_UpdateAnnotation	0
#define NP_DoingCompaction	1
#define NP_MakingBaseVersion	2
#define NP_TracingCompaction	3
#define NP_TracingUpdate	4
#define NP_ValidatingChecksums	5
#define NP_MaxWriteChunk	6

#define NP_License01		128
#define NP_License02		129
#define NP_License03		130
#define NP_License04		131
#define NP_License05		132
#define NP_License06		133
#define NP_License07		134
#define NP_License08		135
#define NP_License09		136
#define NP_License10		137
#define NP_License11		138
#define NP_License12		139
#define NP_License13		140
#define NP_License14		141
#define NP_License15		142
#define NP_License16		143
#define NP_License17		144
#define NP_License18		145
#define NP_License19		146
#define NP_License20		147
#define NP_License21		148
#define NP_License22		149
#define NP_License23		150
#define NP_License24		151
#define NP_License25		152
#define NP_License26		153
#define NP_License27		154
#define NP_License28		155
#define NP_License29		156
#define NP_License30		157
#define NP_License31		158
#define NP_License32		159

/*****  Environment Parameter Indices *****/
#define EP_VisionMappingLimit		256
#define EP_VisionAddressThreshold	257
#define EP_VisionNSyncRetries		258
#define EP_VisionSSyncRetries		259
#define EP_VisionSOpenRetries		260
#define EP_VisionSORetryDelay		261
#define EP_VisionNetOpenTrace		262
#define EP_VisionLargeTaskSize		263
#define EP_VisionStackDump		264
#define EP_VisionGRMEnabled		265
#define EP_VisionGRMTrace		266
#define EP_VisionAttemptDefragment	267
#define EP_VisionRestartOnError		268
#define EP_VisionExitOnError		269
#define EP_VerboseSelectorNotFound	270
#define EP_VisionMaxSeriousErrors	271
#define EP_CachingDictionaries		272
#define	EP_SendingValueMessages		273
#define EP_CachingContainerHandles	274
#define EP_VXWspsRowCount		275
#define EP_VXWspsColumnCount		276
#define EP_VisionImplicitCleanEnabled	277
#define EP_VisionCommitDisabled		278
#define EP_VisionRefAlignCDRatio	279
#define EP_VisionUSegCountThreshold	280
#define EP_VisionUsageLogPath		281
#define EP_VisionUsageLogTag		282
#define EP_VisionUsageLogDelimiter	283
#define EP_VisionPre7Ordering		284
#define EP_VxaAdapterType		285
#define EP_VisionUsageLogSize		286
#define EP_VisionUsageLogBackups	287
#define EP_VisionGCTimeout		288
#define EP_VisionGCThreshold		289
#define EP_VisionGCSpread		290

/*****  General Session Parameter Indices  *****/

#define GP_SetExitValue	      512

/*****  Space Update Parameter Indices  *****/
#define SP_ForceUpdate		0
#define SP_InCompaction		1
#define SP_MSSOverride		2
#define SP_CopyCoefficient	3
#define SP_ReclaimCoefficient	4
#define SP_MaxCompactionSegs	5
#define SP_MaxSegmentSize	6
#define SP_LargeContainerSize   7
#define SP_CompactSmallCntnrs   8


PrivateFnDef void SetSessionAttributeToDouble (
    VPrimitiveTask *pTask, SessionAttribute *parameter, double value
) {
    M_ASD *asd;

    if (0 == parameter->spaceIndex) switch (parameter->parameterIndex) {
    case EP_VisionRefAlignCDRatio:
	rtREFUV_SetCodomainDomainRatio (value);
	break;
    default:
    case SP_CopyCoefficient:
	break;
    }
    else if (
	IsntNil (asd = pTask->codDatabase ()->AccessSpace (parameter->spaceIndex))
    ) switch (parameter->parameterIndex) {
    case SP_MSSOverride:
	asd->PhysicalASD ()->SetMSSOverrideTo (value);
	break;
    case SP_CopyCoefficient:
	asd->PhysicalASD ()->SetCopyCoefficientTo (value);
	break;
    case SP_ReclaimCoefficient:
	asd->PhysicalASD ()->SetReclaimCoefficientTo (value);
	break;
    case SP_MaxCompactionSegs:
	asd->PhysicalASD ()->SetMaxCompactionSegmentsTo (value);
	break;
    case SP_MaxSegmentSize:
	asd->PhysicalASD ()->SetMaxSegmentSizeTo (value);
	break;
    case SP_LargeContainerSize:
	asd->PhysicalASD ()->SetLargeContainerSizeTo (value);
	break;
    default:
	break;
    }
}

PrivateFnDef void SetSessionAttributeToInteger (
    VPrimitiveTask *pTask, SessionAttribute *parameter, int value
) {
    M_ASD *asd;

    if (0 == parameter->spaceIndex) switch (parameter->parameterIndex) {
    case NP_DoingCompaction:
	pTask->codDatabase ()->SetDoingCompactionTo (value ? true : false);
	break;
    case NP_MakingBaseVersion:
	pTask->codDatabase ()->SetMakingBaseVersionTo (value ? true : false);
	break;
    case NP_TracingCompaction:
	pTask->codDatabase ()->SetTracingCompactionTo (value ? true : false);
	break;
    case NP_TracingUpdate:
	pTask->codDatabase ()->SetTracingUpdateTo (value ? true : false);
	break;
    case NP_ValidatingChecksums:
	pTask->codDatabase ()->SetValidatingChecksumsTo (value ? true : false);
	break;
    case NP_MaxWriteChunk:
	PS_SetMaxWriteChunk (static_cast<unsigned int> (value));
	break;

    case NP_License01:
    case NP_License02:
    case NP_License03:
    case NP_License04:
    case NP_License05:
    case NP_License06:
    case NP_License07:
    case NP_License08:
    case NP_License09:
    case NP_License10:
    case NP_License11:
    case NP_License12:
    case NP_License13:
    case NP_License14:
    case NP_License15:
    case NP_License16:
    case NP_License17:
    case NP_License18:
    case NP_License19:
    case NP_License20:
    case NP_License21:
    case NP_License22:
    case NP_License23:
    case NP_License24:
    case NP_License25:
    case NP_License26:
    case NP_License27:
    case NP_License28:
    case NP_License29:
    case NP_License30:
    case NP_License31:
    case NP_License32:
	pTask->codDatabase ()->License ().setValue (
	    parameter->parameterIndex - NP_License01, value
	);
	break;

    case EP_VisionMappingLimit:
	PS_SetMappingLimit (value);
	break;
    case EP_VisionAddressThreshold:
	PS_SetAddressThreshold (static_cast<unsigned int> (value));
	break;
    case EP_VisionNSyncRetries:
	PS_SetNSyncRetries (value);
	break;
    case EP_VisionSSyncRetries:
	PS_SetSSyncRetries (value);
	break;
    case EP_VisionSOpenRetries:
	PS_SetSOpenRetries (value);
	break;
    case EP_VisionSORetryDelay:
	PS_SetSORetryDelay (value);
	break;
    case EP_VisionNetOpenTrace:
	PS_SetNetOpenTrace (value ? true : false);
	break;
    case EP_VisionLargeTaskSize:
	V_SetLargeTaskSize (static_cast<unsigned int> (value));
	break;
    case EP_VisionStackDump:
	ThisProcess.enableStackDumps (value ? true : false);
	break;
    case EP_VisionAttemptDefragment:
	V_SetDefragmentAttempt (value ? true : false);
	break;
    case EP_VisionRestartOnError:
	UNWIND_SetRestart (value ? true : false);
	break;
    case EP_VisionExitOnError:
	UNWIND_SetExit (value ? true : false);
	break;
    case EP_VerboseSelectorNotFound:
	VComputationUnit::Context::g_fVerboseSelectorNotFound = value ? true : false;
	break;
    case EP_CachingContainerHandles:
	VContainerHandle::g_fPreservingHandles = value ? true : false;
	break;
    case EP_CachingDictionaries:
	rtDICTIONARY_UsingCache = value ? true : false;
	break;
    case EP_SendingValueMessages:
	VCall::g_bSendingValueMessages = value ? true : false;
	break;
    case EP_VisionMaxSeriousErrors:
	UNWIND_SetMaximumNumberOfLongjumps (static_cast<unsigned int> (value));
	break;
    case EP_VXWspsRowCount:
        break;
    case EP_VXWspsColumnCount:
        break;
    case EP_VisionImplicitCleanEnabled:
	rtVECTOR_SetImplicitCleanupFlag (value ? true : false);
	break;
    case EP_VisionCommitDisabled:
	PS_DisableCommits (value ? true : false);
	break;
    case EP_VisionRefAlignCDRatio:
        rtREFUV_SetCodomainDomainRatio (value);
        break;
    case EP_VisionUSegCountThreshold:
	rtVECTOR_SetUSegmentCountThreshold (static_cast<unsigned int> (value));
	break;
    case EP_VisionPre7Ordering:
	rtVECTOR_EnablePOPOrdering (value ? true : false);
	break;
    case EP_VxaAdapterType:
	VSNFTaskHolder::g_iAdapterType = static_cast<unsigned int> (value);
	break;
    case EP_VisionUsageLogSize:
	pTask->transientServicesProvider ()->updateUsageLogSize (static_cast<unsigned int> (value));
	break;
    case EP_VisionUsageLogBackups:
	pTask->transientServicesProvider ()->updateUsageLogBackups (static_cast<unsigned int> (value));
	break;
    case EP_VisionGCTimeout:
	pTask->scheduler()->setGCTimeout (static_cast<unsigned int> (value));
	break;
    case EP_VisionGCThreshold:
	ThisProcess.setGCThreshold (static_cast<unsigned int> (value));
	break;
    case EP_VisionGCSpread:
	ThisProcess.setGCSpread (static_cast<unsigned int> (value));
	break;
    case GP_SetExitValue:
	pTask->scheduler()->setExitValue (value);
	break;
    default:
	break;
    }
    else if (
	IsntNil (asd = pTask->codDatabase ()->AccessSpace (parameter->spaceIndex))
    ) switch (parameter->parameterIndex) {
    case SP_ForceUpdate:
	if (value)
	    asd->CompelUpdate ();
	break;
    case SP_InCompaction:
	asd->PhysicalASD ()->SetInCompactionTo (value ? true : false);
	break;
    case SP_MSSOverride:
	asd->PhysicalASD ()->SetMSSOverrideTo (value);
	break;
    case SP_CopyCoefficient:
	asd->PhysicalASD ()->SetCopyCoefficientTo (value);
	break;
    case SP_ReclaimCoefficient:
	asd->PhysicalASD ()->SetReclaimCoefficientTo (value);
	break;
    case SP_MaxCompactionSegs:
	asd->PhysicalASD ()->SetMaxCompactionSegmentsTo (value);
	break;
    case SP_MaxSegmentSize:
	asd->PhysicalASD ()->SetMaxSegmentSizeTo (value);
	break;
    case SP_LargeContainerSize:
	asd->PhysicalASD ()->SetLargeContainerSizeTo (value);
	break;
    case SP_CompactSmallCntnrs:
	asd->PhysicalASD ()->SetCompactingSmallTo (value ? true : false);
	break;
    default:
	break;
    }
}

PrivateFnDef void SetSessionAttributeToString (
    VPrimitiveTask *pTask, SessionAttribute *parameter, char const *value
) {
    if (0 == parameter->spaceIndex) switch (parameter->parameterIndex) {
    case NP_UpdateAnnotation:
	pTask->codDatabase ()->SetUpdateAnnotationTo (value);
	break;
    case EP_VisionUsageLogPath:
	if (strlen (value) == 0) {
	    pTask->transientServicesProvider ()->updateUsageLogSwitch (false);
	} else {
	    pTask->transientServicesProvider ()->updateUsageLogSwitch (true);
	    pTask->transientServicesProvider ()->updateUsageLogFilePath (value);
	    pTask->transientServicesProvider ()->updateUsageLogFile ();
	}
	break;
    case EP_VisionUsageLogTag:
	pTask->transientServicesProvider ()->updateUsageLogTag (value);
	break;
    case EP_VisionUsageLogDelimiter:
	pTask->transientServicesProvider ()->updateUsageLogDelimiter (value);
	break;
    default:
	break;
    }
    else if (
	IsntNil (pTask->codDatabase ()->AccessSpace (parameter->spaceIndex))
    ) switch (parameter->parameterIndex) {
    default:
    case NP_UpdateAnnotation:
	break;
    }
}


PrivateFnDef void SetSessionAttributeToDouble (VPrimitiveTask *pTask) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    SessionAttribute *pp, *pl;
    double *dp;

    if (DucIsAScalar) SetSessionAttributeToDouble (
	pTask,
	(SessionAttribute*)&DSC_Descriptor_Scalar_Int (rCurrent),
	DSC_Descriptor_Scalar_Double (ADescriptor)
    );
    else for (
	pp = (SessionAttribute*)rtINTUV_CPD_Array (
	    DSC_Descriptor_Value (rCurrent)
	),
	pl = pp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent)),
	dp = rtDOUBLEUV_CPD_Array (DSC_Descriptor_Value (ADescriptor));
	pp < pl;
	pp++, dp++
    ) SetSessionAttributeToDouble (pTask, pp, *dp);
}

PrivateFnDef void SetSessionAttributeToInteger (VPrimitiveTask *pTask) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    SessionAttribute *pp, *pl;
    int *ip;

    if (DucIsAScalar) SetSessionAttributeToInteger (
	pTask,
	(SessionAttribute*)&DSC_Descriptor_Scalar_Int (rCurrent),
	DSC_Descriptor_Scalar_Int (ADescriptor)
    );
    else for (
	pp = (SessionAttribute*)rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent)),
	pl = pp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent)),
	ip = rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor));
	pp < pl;
	pp++, ip++
    ) SetSessionAttributeToInteger (pTask, pp, *ip);
}

PrivateFnDef void SetSessionAttributeToBoolean (VPrimitiveTask *pTask, bool value) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    SessionAttribute *pp, *pl;

    if (DucIsAScalar) SetSessionAttributeToInteger (
	pTask, (SessionAttribute*)&DSC_Descriptor_Scalar_Int (rCurrent), value
    );
    else for (
	pp = (SessionAttribute*)rtINTUV_CPD_Array (
	    DSC_Descriptor_Value (rCurrent)
	),
	pl = pp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent));
	pp < pl;
	pp++
    ) SetSessionAttributeToInteger (pTask, pp, value);
}

PrivateFnDef void SetSessionAttributeToBlockString (VPrimitiveTask *pTask, M_CPD *pStringStore) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    SessionAttribute *pp, *pl;
    int *ip;

    if (DucIsAScalar) SetSessionAttributeToString (
	pTask,
	(SessionAttribute*)&DSC_Descriptor_Scalar_Int (rCurrent),
	V_BlockString (pStringStore, DSC_Descriptor_Scalar_Int (ADescriptor))
    );
    else for (
	pp = (SessionAttribute*)rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent)),
	pl = pp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent)),
	ip = rtINTUV_CPD_Array (DSC_Descriptor_Value (ADescriptor));
	pp < pl;
	pp++, ip++
    ) SetSessionAttributeToString (pTask, pp, V_BlockString (pStringStore, *ip));
}

PrivateFnDef void SetSessionAttributeToLStoreString (VPrimitiveTask *pTask, M_CPD *pStringStore) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    rtLSTORE_Align (pStringStore);
    M_CPD *charuv = rtLSTORE_CPD_ContentStringsCPD (pStringStore);
    if (DucIsAScalar) {
	rtREFUV_AlignReference (&DSC_Descriptor_Scalar (ADescriptor));
	SetSessionAttributeToString (
	    pTask,
	    (SessionAttribute*)&DSC_Descriptor_Scalar_Int (rCurrent),
	    V_LStoreString (pStringStore, charuv, DSC_Descriptor_Scalar_Int (ADescriptor))
	);
    }
    else {
	M_CPD *refuv = ADescriptor.asUVector ();
	SessionAttribute *pp, *pl;
	rtREFUV_ElementType *rp;
	for (pp = (SessionAttribute*) rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent)),
	     pl = pp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent)),
	     rp = rtREFUV_CPD_Array (refuv);
	     pp < pl;
	     pp++, rp++
	) SetSessionAttributeToString (pTask, pp, V_LStoreString (pStringStore, charuv, *rp));
	refuv->release ();
    }
    charuv->release ();
}


V_DefinePrimitive (SetSessionAttribute) {
/*****  Send the converse message if the operand is polymorphic ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (KS__RAssign);
	return;
    }

/*****  Otherwise, acquire both operands of the binary ... *****/
    pTask->normalizeDuc ();

    M_CPD *argType = pTask->ducStore ();
    if (argType->NamesTheDoubleClass ())
	SetSessionAttributeToDouble (pTask);
    else if (argType->NamesTheIntegerClass ())
	SetSessionAttributeToInteger (pTask);
    else if (argType->NamesTheTrueClass ())
	SetSessionAttributeToBoolean (pTask, true);
    else if (argType->NamesTheFalseClass ())
	SetSessionAttributeToBoolean (pTask, false);
    else if (RTYPE_C_Block == (RTYPE_Type)M_CPD_RType (argType))
	SetSessionAttributeToBlockString (pTask, argType);
    else if (
	RTYPE_C_ListStore == (RTYPE_Type)M_CPD_RType (argType) &&
	rtLSTORE_CPD_StringStore (argType)
    ) SetSessionAttributeToLStoreString (pTask, argType);
}


/*****************************************
 *****  Data Base Update Primitives  *****
 *****************************************/

PrivateFnDef void __cdecl UpdateNetworkOutputHandler (
    VPrimitiveTask *pTask, char const *pFormat, ...
) {
    V_VARGLIST (ap, pFormat);
    pTask->vprintf (256, pFormat, ap);
}

V_DefinePrimitive (UpdateNetwork) {
    pTask->loadDucWithInteger (
	pTask->codDatabase ()->SaveTheTLE (
	    pTask, (M_AND::UpdateOutputHandler)UpdateNetworkOutputHandler
	)
    );
}


/**********************************
 *****  Incorporator Support  *****
 **********************************/

PrivateFnDef void dumpTheTable (
    VPrimitiveTask *pTask, char const *pFilename, bool replace, M_CPD *pObjectCPD
) {
    IO_printf ("+++ Opening file %s\n", pFilename);

    if ((DBUPDATE__DumpFile = fopen (pFilename, "w")) == NULL) pTask->raiseException (
	EC__InternalInconsistency, "DBUPDATE: Error opening the DB Dump File."
    );
    DBUPDATE__DumpFileIsOpen = true;

/***** Write the Object Space line ... *****/
    IO_printf ("+++ Writing the Header Record ... \n");
    pObjectCPD->WriteDBUpdateInfo (replace);

/*****  Output the VStore or LStore ... *****/
    RTYPE_Type xObjectRType = (RTYPE_Type)M_CPD_RType (pObjectCPD);
    if (xObjectRType == RTYPE_C_ValueStore) {
	IO_printf ("+++ Writing the ValueStore Records ... \n");
	rtVSTORE_WriteDBUpdateInfo (pObjectCPD);
    }
    else if (
	xObjectRType == RTYPE_C_ListStore && rtLSTORE_CPD_StringStore (pObjectCPD)
    ) {
	IO_printf ("+++ Writing the ListStore records ... \n");
	rtLSTORE_WriteDBUpdateInfo (pObjectCPD);
    }
    else pTask->raiseException (
	EC__UsageError,
	"DBUpdateDump: The object must be either a VStore or a string LStore"
    );

/***** Close the file ... *****/
    IO_printf ("+++ Closing the DB Dump File ... \n");
    fclose (DBUPDATE__DumpFile);
    DBUPDATE__DumpFileIsOpen = false;

    IO_printf ("+++ DB Dump File Complete! +++\n");
} 


V_DefinePrimitive (DBUpdateDump) {
    char const *filename;
    bool	replace;
    int		converseMessageSelector;

/*****   Define Internal macros  *****/
#define handleBlockStrings(value) {\
    filename = V_BlockString (pStringStore, value);\
    dumpTheTable (pTask, filename, replace, object);\
}

#define handleLStoreStrings(value) {\
    filename = V_LStoreString (pStringStore, charCPD, value);\
    dumpTheTable (pTask, filename, replace, object);\
}

/*---------------------------------------------------------------------------
 * 			Code Body of DBUpdateDump
 *---------------------------------------------------------------------------
 */

/*****  Determine which alias was invoked ... *****/
    switch (V_TOTSC_Primitive) {
    case XDBUpdateReplaceDump:
	converseMessageSelector = KS__DBUpdateReplaceFile;
	replace = true;
	break;
    case XDBUpdateAppendDump:
	converseMessageSelector = KS__DBUpdateAppendFile;
	replace = false;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DBUpdateDump");
	break;
    }

/***** Acquire the operands ... *****/
/***** Send the binary converse message if the operand is polymorphic ... *****/
    if (!pTask->loadDucWithNextParameterAsMonotype ()) {
	pTask->sendBinaryConverseWithCurrent (converseMessageSelector);
	return;
    }

/*****  Otherwise, acquire both operands of the binary ... *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    pTask->normalizeDuc ();

/*****  Get the object to dump ... *****/
    M_CPD *object = pTask->ducStore ();

/*****  Determine the file(s) to create ... *****/
    M_CPD *pStringStore = rCurrent.storeCPD ();
    RTYPE_Type xStoreRType = (RTYPE_Type)M_CPD_RType (pStringStore);
    if (xStoreRType == RTYPE_C_Block) {
	if (DucIsAScalar) {
	    handleBlockStrings (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, handleBlockStrings);
	}
    }
    else if (xStoreRType == RTYPE_C_ListStore && rtLSTORE_CPD_StringStore (pStringStore)) {
	M_CPD *charCPD = rtLSTORE_CPD_ContentStringsCPD (pStringStore);
	if (DucIsAScalar) {
	    handleLStoreStrings (DSC_Descriptor_Scalar_Int (rCurrent));
	}
	else {
	    DSC_Traverse (rCurrent, handleLStoreStrings);
	}

	charCPD->release ();
    }

#undef handleBlockStrings
#undef handleLStoreStrings
}


/************************************
 ************************************
 *****  Primitive Descriptions  *****
 ************************************
 ************************************/
/*---------------------------------------------------------------------------
 * Three entries must be made in this file for every primitive in the system -
 * one to assign the primitive an index, one to define its code body, and one
 * to describe it to the virtual machine and debugger.  This section contains
 * the entries that describe the primitives to the virtual machine and
 * debugger.
 *---------------------------------------------------------------------------
 */

BeginDescriptions


/********************************
 *****  Utility Primitives  *****
 ********************************/

    PD (XPanic,
	"Panic",
	Panic)

    PD (XCollectGarbage,
	"CollectGarbage",
	CollectGarbage)

    PD (XCollectSessionGarbage,
	"CollectSessionGarbage",
	CollectGarbage)

    PD (XExitSession,
	"ExitSession",
	ExitSession)

    PD (XReclaimResources,
	"ReclaimResources",
	ReclaimResources)

    PD (XFederate,
	"Federate",
	Federate)
    PD (XClientObject,
	"ClientObject",
	ClientObject)
    PD (XExternalObject,
	"ExternalObject",
	ExternalObject)

    PD (XSetSessionAttribute,
	"SetSessionAttribute", 
	SetSessionAttribute)

    PD (XUpdateNetwork,
	"UpdateNetwork",
	UpdateNetwork)

/******************************************
 *****  Time Stamp Access Primitives  *****
 ******************************************/

    PD (XAccessResourceTimeStamp,
	"AccessResourceTimeStamp",
	AccessTimeStamp)
    PD (XAccessTimeStamp,
	"AccessTimeStamp",
	AccessTimeStamp)
    PD (XDecodeTZTimeStamp,
	"DecodeTZTimeStamp",
	DecodeTimeStamp)
    PD (XDecodeGMTimeStamp,
	"DecodeGMTimeStamp",
	DecodeTimeStamp)
    PD (XFormatTZTimeStamp,
	"FormatTZTimeStamp",
	FormatTimeStamp)
    PD (XFormatGMTimeStamp,
	"FormatGMTimeStamp",
	FormatTimeStamp)


/***********************************
 *****  Evaluation Primitives  *****
 ***********************************/

    PD (XStringEvaluate,
	"StringEvaluate",
	StringEvaluate)

    PD (XSendForExecution,
	"SendForExecution",
	Send)
    PD (XSendForValue,
	"SendForValue",
	Send)
    PD (XSendForEnvironment,
	"SendForEnvironment",
	Send)

    PD (XEvaluateForValue,
	"EvaluateForValue",
	Evaluate)
    PD (XEvaluateForEnvironment,
	"EvaluateForEnvironment",
	Evaluate)
    PD (XEvaluateUltimateParameter,
	"EvaluateUltimateParameter",
	EvaluateParameter)
    PD (XEvaluatePenultimateParameter,
	"EvaluatePenultimateParameter",
	EvaluateParameter)
    PD (XForDateEvalUltimateParameter,
	"ForDateEvaluateUltimateParameter",
	ForDateEvalUltimateParameter)

    PD (XEvaluateInLocalContext,
	"EvaluateInLocalContext",
	EvaluateInLocalContext)

    PD (XWhileTrue,
	"WhileTrue",
	VPrimitiveDescriptor::IClass_WhileTrueController)


    PD (XByDaysForDateEval,
	"ByDaysForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByBDaysForDateEval,
	"ByBDaysForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByMonthsForDateEval,
	"ByMonthsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByMonthBeginningsForDateEval,
	"ByMonthBeginningsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByMonthEndsForDateEval,
	"ByMonthEndsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByQtrsForDateEval,
	"ByQuartersForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByQtrBeginningsForDateEval,
	"ByQuarterBeginningsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByQtrEndsForDateEval,
	"ByQuarterEndsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByYearsForDateEval,
	"ByYearsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByYearBeginningsForDateEval,
	"ByYearBeginningsForDatesEvaluate",
	ForDatesInRangeEvaluate)
    PD (XByYearEndsForDateEval,
	"ByYearEndsForDatesEvaluate",
	ForDatesInRangeEvaluate)


/*****************************************
 *****  Class Conversion Primitives  *****
 *****************************************/

    PD (XSelectorToString,
	"SelectorToString",
	SelectorToString)


/*******************************************
 *****  Closure and Method Primitives  *****
 *******************************************/

    PD (XClosureSelector,
	"ClosureSelector",
	ClosureSelector)
    PD (XMethodSelector,
	"MethodSelector",
	MethodSelector)
    PD (XClosureAsMethod,
	"ClosureAsMethod",
	ClosureAsMethod)
    PD (XMethodAsClosure,
	"MethodAsClosure",
	MethodAsClosure)
    PD (XClosureSelf,
	"ClosureSelf",
	ClosureSelf)
    PD (XClosureCurrent,
	"ClosureCurrent",
	ClosureCurrent)
    PD (XClosureOrigin,
	"ClosureOrigin",
	ClosureOrigin)

/***********************************
 *****  Assignment Primitives  *****
 ***********************************/

    PD (XAssign,
	"Assign",
	Assign)

/*********************************************
 *****  Property Maintenance Primitives  *****
 *********************************************/

    PD (XCleanStore,
	"CleanStore",
	CleanStore)
    PD (XAlignStore,
	"AlignStore",
	CleanStore)
    PD (XCleanDictionary,
	"CleanDictionary",
	CleanDictionary)


/*********************************************
 *****  Incorporator Support Primitives  *****
 *********************************************/

    PD (XDBUpdateReplaceDump,
	"DBUpdateReplaceDump",
	DBUpdateDump)
    PD (XDBUpdateAppendDump,
	"DBUpdateAppendDump",
	DBUpdateDump)

EndDescriptions


/*******************************************************
 *****  The Primitive Dispatch Vector Initializer  *****
 *******************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a portion of the primitive function
 *****	dispatch vector at system startup.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void pfUTIL_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfUTIL);
    FAC_FDFCase_FacilityDescription ("Utility Primitive Function Services");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  PFutil.c 1 replace /users/jad/system
  861116 16:02:23 jad new utitlity primitive functions

 ************************************************************************/
/************************************************************************
  PFutil.c 2 replace /users/jad/system
  861119 12:42:12 jad added ln, exp, and sqrt primitives

 ************************************************************************/
/************************************************************************
  PFutil.c 3 replace /users/jad/system
  861119 14:05:50 jad moved the log,exp, and sqrt to PFnumeric

 ************************************************************************/
/************************************************************************
  PFutil.c 4 replace /users/jad/system
  861201 18:27:53 jad added PrintWithCommasOf primitives

 ************************************************************************/
/************************************************************************
  PFutil.c 5 replace /users/mjc/system
  861226 12:25:47 mjc Implemented 'super'

 ************************************************************************/
/************************************************************************
  PFutil.c 6 replace /users/mjc/system
  870104 22:53:24 lcn Added 'Locate' and 'LocateOrAdd' primitives

 ************************************************************************/
/************************************************************************
  PFutil.c 7 replace /users/jad/system
  870119 12:54:39 jad implemented lstore strings

 ************************************************************************/
/************************************************************************
  PFutil.c 8 replace /users/jad/system
  870120 15:00:33 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  PFutil.c 9 replace /users/jad/system
  870123 16:12:25 jad implemented the list rowsInStore primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 10 replace /users/jad/system
  870128 19:02:33 jad 1) added delete and contents methodd primitives.
2) fixed PointerAsValue primitive.

 ************************************************************************/
/************************************************************************
  PFutil.c 11 replace /users/jad/system
  870204 15:26:50 jad 1) made print: primitive work with selector and \implemented sprint. 2) make NewInstance primitive not make a new for values

 ************************************************************************/
/************************************************************************
  PFutil.c 12 replace /users/jad/system
  870205 14:56:22 jad added specializeClass primitive & split out the print primitives into another module

 ************************************************************************/
/************************************************************************
  PFutil.c 13 replace /users/mjc/rsystem
  870210 13:07:40 mjc Fixed list case bug in 'IntegerAsReferenceTo'

 ************************************************************************/
/************************************************************************
  PFutil.c 14 replace /users/jad/system
  870210 17:49:27 jad added selectorToString primitive, and fixed bug in lookup primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 15 replace /users/mjc/system
  870215 23:01:33 lcn Eliminate 'SAVER' calls

 ************************************************************************/
/************************************************************************
  PFutil.c 16 replace /users/jad/system
  870220 15:50:42 jad implemented SelectedRowsInsStore primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 17 replace /users/mjc/system
  870302 09:58:21 mjc Modified selector deletion primitive to correct erroneous call to 'Lookup'

 ************************************************************************/
/************************************************************************
  PFutil.c 18 replace /users/jck/system
  870420 13:34:43 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  PFutil.c 19 replace /users/mjc/translation
  870524 09:31:20 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  PFutil.c 20 replace /users/jck/system
  870605 16:07:56 jck Generalized interface to RTindex

 ************************************************************************/
/************************************************************************
  PFutil.c 21 replace /users/jck/system
  870612 13:28:34 jck AMD now knows about ALists

 ************************************************************************/
/************************************************************************
  PFutil.c 22 replace /users/jck/system
  870616 15:20:59 jck Fixed bug in 'LocateOrAddInDictionaryOf' primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 23 replace /users/jck/system
  870706 08:18:28 jck Implemented a generic deletion primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 24 replace /users/jck/system
  870824 15:05:31 jck Added a primitive to align the columns of a store and
a primitive to update the network (equivalent of ?w)

 ************************************************************************/
/************************************************************************
  PFutil.c 25 replace /users/jad/system
  870921 10:44:48 jad implemented echoing to an output file

 ************************************************************************/
/************************************************************************
  PFutil.c 26 replace /users/jck/system
  871007 13:39:37 jck Release implementation of Global Persistent Garbage Collector

 ************************************************************************/
/************************************************************************
  PFutil.c 27 replace /users/jad/system
  871215 14:00:51 jad rewrite the 'ForDateRangeEvaluate' primitive for effeciency

 ************************************************************************/
/************************************************************************
  PFutil.c 28 replace /users/jad/system
  871215 17:29:33 jad fix typo & remove old code

 ************************************************************************/
/************************************************************************
  PFutil.c 29 replace /users/jad/system
  871216 11:48:28 jad fix calls to 'RTlink' to be consistent
with 'RTlink' before sequences

 ************************************************************************/
/************************************************************************
  PFutil.c 30 replace /users/mjc/Software/system
  871224 15:20:50 mjc Eliminated restrictions on 'alignStore' primitive

 ************************************************************************/
/************************************************************************
  PFutil.c 31 replace /users/jck/system
  880331 08:19:19 jck Added missing case to PointerAsValuePrimitive

 ************************************************************************/
/************************************************************************
  PFutil.c 32 replace /users/jad/system
  880525 13:17:06 jad Added two network primitives:
objectSpace and containerIndex

 ************************************************************************/
/************************************************************************
  PFutil.c 33 replace /users/jad/system
  880707 14:24:13 jad Added missing return after converse

 ************************************************************************/
/************************************************************************
  PFutil.c 34 replace /users/jck/system
  890411 13:35:58 jck Corrections to 'ValueAsPointerTo' and 'DeleteInstance'

 ************************************************************************/
/************************************************************************
  PFutil.c 35 replace /users/jck/system
  890808 15:46:23 jck Added Non-scalar case to the dumpTables primitives

 ************************************************************************/

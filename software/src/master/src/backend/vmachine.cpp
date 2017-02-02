/*****  Virtual Machine  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName VMACHINE

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

#include "vmachine.h"

/************************
 *****  Supporting  *****
 ************************/

#include "batchvision.h"

#include "gopt.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTclosure.h"

#include "VBoundCall.h"
#include "VEvaluationCall.h"
#include "VPrimaryCall.h"
#include "VSecondaryCall.h"

#include "VBlockTask.h"
#include "VPrimitiveTask.h"
#include "VTopTask.h"
#include "VUtilityTask.h"

#include "VChannelController.h"
#include "VListEnumerationController.h"
#include "VReadEvalPrint.h"
#include "VWhileTrueController.h"

#include "VFragment.h"


/******************************
 ******************************
 *****  Global Interface  *****
 ******************************
 ******************************/

PublicFnDef void V_SetLargeTaskSize (unsigned int size) {
    if (size > 0)
	VComputationUnit::Context::g_iLargeTaskSize = size;
}

PublicFnDef void V_SetDefragmentAttempt (unsigned int flag) {
    VDescriptor::DefragmentationAttemptEnabled = flag ? true : false;
}


/**********************************
 **********************************
 *****  Facility Definitions  *****
 **********************************
 **********************************/

/**************************************************
 *****  Facility I-Object Method Definitions  *****
 **************************************************/

/*********************************
 *  Trace Switch Access Methods  *
 *********************************/

IOBJ_DefineUnaryMethod (TracesOnDM) {
    VTask::TracingExecution			=
    VFragmentation::TracingFragmentationOps	= true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM) {
    VTask::TracingExecution			=
    VFragmentation::TracingFragmentationOps	= false;

    return self;
}

IOBJ_DefineNilaryMethod (ExecutionTraceDM) {
    return IOBJ_SwitchIObject (&VTask::TracingExecution);
}

IOBJ_DefineNilaryMethod (FragmentationOpsTraceDM) {
    return IOBJ_SwitchIObject (&VFragmentation::TracingFragmentationOps);
}

IOBJ_DefineNilaryMethod (VerboseSelectorNotFoundDM) {
    return IOBJ_SwitchIObject (&VComputationUnit::Context::g_fVerboseSelectorNotFound);
}

IOBJ_DefineNilaryMethod (SendingValueMessagesDM) {
    return IOBJ_SwitchIObject (&VCall::g_bSendingValueMessages);
}


/*************************************
 *  Tuning Parameter Update Methods  *
 *************************************/

IOBJ_DefineMethod (SetTaskPollingIntervalDM) {
    int input = IOBJ_NumericIObjectValue (parameterArray[0], int);

    if (input > 0)
	Batchvision::SetPollingIntervalTo (static_cast<unsigned int>(input));
    else {
        IO_printf ("Invalid value (%d), Parameter Not Changed.\n", input);
    }

    return self;
}


/***************************
 *  State Display Methods  *
 ***************************/

IOBJ_DefineUnaryMethod (DisplayConstantsDM) {
    IO_printf ("Sizes of Execution Apropos Types:\n");
    UTIL_DisplaySizeofType (VComputationScheduler     );
    UTIL_DisplaySizeofType (VComputationUnit          );
    UTIL_DisplaySizeofType (VComputationUnit::Context );
    IO_printf ("\n");
    UTIL_DisplaySizeofType (VCall                     );
    UTIL_DisplaySizeofType (VBoundCall                );
    UTIL_DisplaySizeofType (VEvaluationCall           );
    UTIL_DisplaySizeofType (VPrimaryCall              );
    UTIL_DisplaySizeofType (VSecondaryCall            );
    IO_printf ("\n");
    UTIL_DisplaySizeofType (VTask                     );
    UTIL_DisplaySizeofType (VBlockTask                );
    UTIL_DisplaySizeofType (VPrimitiveTaskBase        );
    UTIL_DisplaySizeofType (VPrimitiveTask            );
    UTIL_DisplaySizeofType (VTopTask                  );
    UTIL_DisplaySizeofType (VUtilityTask              );
    IO_printf ("\n");
    UTIL_DisplaySizeofType (VChannelController        );
    UTIL_DisplaySizeofType (VListEnumerationController);
    UTIL_DisplaySizeofType (VReadEvalPrintController  );
    UTIL_DisplaySizeofType (VWhileTrueController      );
    IO_printf ("\n");
    UTIL_DisplaySizeofType (VConstructor              );
    UTIL_DisplaySizeofType (rtCONTEXT_Constructor     );
    UTIL_DisplaySizeofType (rtCLOSURE_Constructor     );
    IO_printf ("\n");
    UTIL_DisplaySizeofType (VDescriptor               );
    UTIL_DisplaySizeofType (VFragmentation            );
    UTIL_DisplaySizeofType (VFragment                 );
    UTIL_DisplaySizeofType (DSC_Descriptor            );

    return self;
}

/*---------------------------------------------------------------------------*
 *  'DisplayParameters' supersedes 'DisplayState'.  'DisplayState' has been
 *  retained to simplify the comparison of boot strap log files.
 *---------------------------------------------------------------------------*/
IOBJ_DefineUnaryMethod (DisplayParametersDM) {
    IO_printf ("Tuning Parameters:\n");
    IO_printf ("DefragmentAttemptEnabled  = %u\n", VDescriptor::DefragmentationAttemptEnabled);
    IO_printf ("BlockingWait              = %u\n", Batchvision::BlockingWait ());
    IO_printf ("TaskPollingInterval       = %u\n", Batchvision::PollingInterval ());
    IO_printf ("LargeTaskSize             > %u\n", VComputationUnit::Context::g_iLargeTaskSize);

    return self;
}

IOBJ_DefineUnaryMethod (DisplayStateDM)
{
    IO_printf ("Stack State:\n");
    IO_printf ("DefragmentAttemptEnabled  = %u\n", VDescriptor::DefragmentationAttemptEnabled);

    return self;
}

extern unsigned int g_cMessages;
extern unsigned int g_cSteppings;
extern unsigned int g_sSteppings;
extern unsigned int g_cSteppingsOptimized;
extern unsigned int g_sSteppingsOptimized;
extern unsigned int g_cSteppingsSuppressed;
extern unsigned int g_sSteppingsSuppressed;

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    IO_printf ("\nVMACHINE Usage Counts\n");
    IO_printf (  "---------------------\n");

    IO_printf ("MakeFragFromVCCount       = %u\n", VFragmentation::MakeFragFromVCCount);
    IO_printf ("MakeFragFromDscCount      = %u\n", VFragmentation::MakeFragFromDscCount);
    IO_printf ("MakeVectorFromFragCount   = %u\n", VFragmentation::MakeVectorFromFragCount);
    IO_printf ("HomogenizeVCCount         = %u\n", VDescriptor::HomogenizeVCCount);
    IO_printf ("HomogenizeVectorCount     = %u\n", VDescriptor::HomogenizeVectorCount);
    IO_printf ("FlattenFragCount          = %u\n", VFragmentation::FlattenFragCount);
    IO_printf ("CoalesceAttemptCount      = %u\n", VFragmentation::CoalesceAttemptCount);
    if (VFragmentation::CoalesceNonDJUCount) 
	IO_printf (
	       "CoalesceSuccessCount      = %u(%u)\n", VFragmentation::CoalesceSuccessCount
						     , VFragmentation::CoalesceNonDJUCount
	);
    else
	IO_printf (
	       "CoalesceSuccessCount      = %u\n", VFragmentation::CoalesceSuccessCount
	);
    IO_printf ("CoalesceExamineCount      = %u\n", VFragmentation::CoalesceExamineCount);
    IO_printf ("CoalesceCandidateCount    = %u\n", VFragmentation::CoalesceCandidateCount);
    IO_printf ("CoalesceReductionCount    = %u\n", VFragmentation::CoalesceReductionCount);
    IO_printf ("\n");

    IO_printf ("LargeTaskSize             > %u\n", VComputationUnit::Context::g_iLargeTaskSize);
    IO_printf ("LargeTaskCount            = %u\n", VTask::LargeTaskCount);
    IO_printf ("MaxTaskSize               = %u\n", VTask::MaxTaskSize);
    IO_printf ("PrimitiveTaskCount        = %u\n", VTask::PrimitiveTaskCount);
    IO_printf ("BlockTaskCount            = %u\n", VTask::BlockTaskCount);
    IO_printf ("\n");

    IO_printf ("Messages                   = %u\n", g_cMessages);
    IO_printf ("... Steppings              = %u\n", g_cSteppings);
    IO_printf ("                           = %u\n", g_sSteppings);
    if (g_cSteppingsOptimized) {
	IO_printf ("... Steppings Optimized    = %u (%g%%)\n", g_cSteppingsOptimized, 100 * (double)g_cSteppingsOptimized / g_cSteppings);
	IO_printf ("                           = %u (%g%%)\n", g_sSteppingsOptimized, 100 * (double)g_sSteppingsOptimized / g_sSteppings);
    }
    if (g_cSteppingsSuppressed) {
	IO_printf ("... Steppings Suppressed   = %u (%g%%)\n", g_cSteppingsSuppressed, 100 * (double)g_cSteppingsSuppressed / g_cSteppings);
	IO_printf ("                           = %u (%g%%)\n", g_sSteppingsSuppressed, 100 * (double)g_sSteppingsSuppressed / g_sSteppings);
    }
    IO_printf ("\n");

    return self;
}

IOBJ_DefineUnaryMethod (ClearCountsDM)
{
    VFragmentation::MakeFragFromVCCount = 0;
    VFragmentation::MakeFragFromDscCount = 0;
    VFragmentation::MakeVectorFromFragCount = 0;
    VDescriptor::HomogenizeVCCount = 0;
    VDescriptor::HomogenizeVectorCount = 0;
    VFragmentation::FlattenFragCount = 0;
    VFragmentation::CoalesceAttemptCount = 0;
    VFragmentation::CoalesceSuccessCount = 0;
    VFragmentation::CoalesceNonDJUCount = 0;
    VFragmentation::CoalesceExamineCount = 0;
    VFragmentation::CoalesceCandidateCount = 0;
    VFragmentation::CoalesceReductionCount = 0;

    VTask::LargeTaskCount = 0;
    VTask::MaxTaskSize = 0;
    VTask::PrimitiveTaskCount = 0;
    VTask::BlockTaskCount = 0;

    return self;
}


PrivateFnDef void DisplayPrimitiveTable (
    int				allPrimitives
)
{
    IO_printf ("Primitive Table Dump:\n\n");
    IO_printf (
	"Index     Prim Fn Name                 Code Address  Exec.Count\n"
    );
    IO_printf (
	"-----     -------------------------    ------------  ----------\n");

    for (unsigned int i=0; i< V_PF_MaxPrimitiveIndex; i++) {
	VPrimitiveDescriptor* pd = VPrimitiveTask::PrimitiveDescriptor (i);

	if (IsntNil (pd) && (allPrimitives || pd->useCount () > 0))
	    IO_printf (
		"%6d    %-25.25s      %8x  %10u\n",
		i, pd->name (), pd->continuation (), pd->useCount ()
	    );
    }
}

IOBJ_DefineUnaryMethod (DisplayAllPrimitivesDM) {
    DisplayPrimitiveTable (true);
    return self;
}

IOBJ_DefineUnaryMethod (DisplayExecutedPrimitivesDM) {
    DisplayPrimitiveTable (false);
    return self;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    static bool alreadyInitialized = false;
    char *		estring;
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("displayConstants"		, DisplayConstantsDM)
	IOBJ_MDE ("displayParameters"		, DisplayParametersDM)
	IOBJ_MDE ("displayState"		, DisplayStateDM)
	IOBJ_MDE ("displayCounts"		, DisplayCountsDM)
	IOBJ_MDE ("clearCounts"			, ClearCountsDM)
	IOBJ_MDE ("displayPrimitives"		, DisplayAllPrimitivesDM)
	IOBJ_MDE ("displayExecutedPrimitives"	, DisplayExecutedPrimitivesDM)
	IOBJ_MDE ("tracesOn"			, TracesOnDM)
	IOBJ_MDE ("tracesOff"			, TracesOffDM)
	IOBJ_MDE ("executionTrace"		, ExecutionTraceDM)
	IOBJ_MDE ("fragmentationOpsTrace"	, FragmentationOpsTraceDM)
	IOBJ_MDE ("verboseSelectorNotFound"	, VerboseSelectorNotFoundDM)
	IOBJ_MDE ("sendingValueMessages"	, SendingValueMessagesDM)
	IOBJ_MDE ("setTaskPollingIntervalTo:"	, SetTaskPollingIntervalDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (V);
    FAC_FDFCase_FacilityDescription ("The Virtual Machine");
    FAC_FDFCase_FacilityMD (methodDictionary);

    case FAC_DoStartupInitialization:
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}

/*****  Obtain the large task size paramter value...  *****/
	V_SetLargeTaskSize (
	    strtoul (GOPT_GetValueOption ("LargeTaskSize"), (char **) NULL, 0)
	);

/*****  Turn on defragmentation if requested ...  *****/
	if (IsntNil (estring = getenv ("VisionAttemptDefragment")))
	    VDescriptor::DefragmentationAttemptEnabled = strtoul (
		estring, (char**)0, 0
	    ) ? true : false;

/*****  Obtain other switch values ...  *****/
	VCall::g_bSendingValueMessages = IsntNil (getenv ("VisionSendingValueMessages"));

/*****  ... and indicate that the initialization succeeded.  *****/
	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  vmachine.c 1 replace /users/mjc/system
  860325 11:32:31 mjc Virtual Machine

 ************************************************************************/
/************************************************************************
  vmachine.c 2 replace /users/mjc/system
  860410 19:20:51 mjc Restructured dispatch/added 'FetchInstruction'

 ************************************************************************/
/************************************************************************
  vmachine.c 3 replace /users/mjc/system
  860412 18:14:12 mjc Changed 'RTpvector' references to 'RTparray'

 ************************************************************************/
/************************************************************************
  vmachine.c 4 replace /users/jad/system
  860422 15:16:39 jad use new io module

 ************************************************************************/
/************************************************************************
  vmachine.c 5 replace /users/jck/system
  860429 10:37:36 jck Export Internal Known Selector translation
capability for the benefit of rslang

 ************************************************************************/
/************************************************************************
  vmachine.c 6 replace /users/jck/system
  860522 13:15:50 jck Changed #include RTenvir.h to RTdictionary.h

 ************************************************************************/
/************************************************************************
  vmachine.c 7 replace /users/mjc/system
  860526 20:32:23 mjc Preliminary release to free up other modules

 ************************************************************************/
/************************************************************************
  vmachine.c 8 replace /users/mjc/system
  860527 18:00:29 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.c 9 replace /users/mjc/system
  860531 10:53:24 mjc Eliminated 'VMACHINE_TopLevel...' variable

 ************************************************************************/
/************************************************************************
  vmachine.c 10 replace /users/mjc/system
  860601 18:26:43 mjc Release virtual machine through byte code 4

 ************************************************************************/
/************************************************************************
  vmachine.c 11 replace /users/mjc/system
  860602 15:00:05 mjc Release changes to use new value descriptor format

 ************************************************************************/
/************************************************************************
  vmachine.c 12 replace /users/mjc/system
  860604 19:24:52 mjc Intermediate release of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.c 13 replace /users/mjc/system
  860606 13:37:31 mjc Release initial working version of virtual machine

 ************************************************************************/
/************************************************************************
  vmachine.c 14 replace /users/mjc/system
  860607 12:13:57 mjc Fix a few bugs

 ************************************************************************/
/************************************************************************
  vmachine.c 15 replace /users/mjc/system
  860610 11:40:09 mjc Added 'StoreNoValue' byte code

 ************************************************************************/
/************************************************************************
  vmachine.c 16 replace /users/mjc/system
  860615 19:01:37 mjc Deleted reference to 'RTintensn'

 ************************************************************************/
/************************************************************************
  vmachine.c 17 replace /users/mjc/system
  860623 10:21:47 mjc Converted to use new 'valuedsc' macros

 ************************************************************************/
/************************************************************************
  vmachine.c 18 replace /users/mjc/system
  860703 11:52:24 mjc Changed declaration of 'acqp' byte codes, added support for '^current'

 ************************************************************************/
/************************************************************************
  vmachine.c 19 replace /users/mjc/system
  860709 10:16:32 mjc Release new format value descriptors

 ************************************************************************/
/************************************************************************
  vmachine.c 20 replace /users/mjc/system
  860712 09:33:23 mjc Delete 'RTlexb' R-Type

 ************************************************************************/
/************************************************************************
  vmachine.c 21 replace /users/mjc/system
  860728 13:56:57 mjc Prerelease for db update

 ************************************************************************/
/************************************************************************
  vmachine.c 22 replace /users/mjc/system
  860803 00:18:58 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  vmachine.c 23 replace /usr/rs
  860803 09:17:34 mjc Change 'InitTaskOutputBuffer' to 'InitBuffer'

 ************************************************************************/
/************************************************************************
  vmachine.c 24 replace /users/mjc/system
  860803 11:25:54 mjc Release parameter acquisition byte codes

 ************************************************************************/
/************************************************************************
  vmachine.c 25 replace /users/mjc/system
  860803 12:51:59 lcn Temporarily bypassed 'lio' for error messages

 ************************************************************************/
/************************************************************************
  vmachine.c 26 replace /users/mjc/system
  860805 18:47:14 mjc Return system for rebuild

 ************************************************************************/
/************************************************************************
  vmachine.c 27 replace /users/mjc/system
  860807 18:05:39 mjc Release basic time operations

 ************************************************************************/
/************************************************************************
  vmachine.c 28 replace /users/mjc/system
  860811 13:50:12 mjc Completed initial implementation of date offsets

 ************************************************************************/
/************************************************************************
  vmachine.c 29 replace /users/jad/system
  860813 19:01:46 jad added a memory manager state trace

 ************************************************************************/
/************************************************************************
  vmachine.c 30 replace /users/mjc/rsystem
  860814 02:46:57 mjc Fixed cleanup bug dispatching to context constructors

 ************************************************************************/
/************************************************************************
  vmachine.c 31 replace /users/jad/system
  860820 15:29:34 jad added counters

 ************************************************************************/
/************************************************************************
  vmachine.c 32 replace /users/mjc/system
  860826 19:58:34 mjc Fixed erroneous P-Token in 'StoreNoValue' byte code

 ************************************************************************/
/************************************************************************
  vmachine.c 33 replace /users/mjc/system
  861002 19:22:59 mjc Release of new list architecture

 ************************************************************************/
/************************************************************************
  vmachine.c 34 replace /users/mjc/system
  861003 12:38:25 mjc Fixed '^current' creation bug in block activation

 ************************************************************************/
/************************************************************************
  vmachine.c 35 replace /users/mjc/system
  861008 18:37:44 mjc Take out the trash in 'DispatchContext' inheritance stepping and 'StoreNextBlockParameter'

 ************************************************************************/
/************************************************************************
  vmachine.c 36 replace /users/jck/system
  861012 10:25:41 mjc Release corrections to scheduler in support of more general properties

 ************************************************************************/
/************************************************************************
  vmachine.c 37 replace /users/mjc/system
  861015 23:53:37 lcn Release of sorting, grouping, ranking, and formating primitives

 ************************************************************************/
/************************************************************************
  vmachine.c 38 replace /users/mjc/system
  861016 18:35:00 mjc Fixed property store creator to discard inappropriate stores

 ************************************************************************/
/************************************************************************
  vmachine.c 39 replace /users/jad/system
  861024 15:35:14 jad made link descriptors
contain a linkc and/or a linkcpd to eliminate some conversions

 ************************************************************************/
/************************************************************************
  vmachine.c 40 replace /users/jad/system
  861106 17:57:48 jad added several routines for fragmentations

 ************************************************************************/
/************************************************************************
  vmachine.c 41 replace /users/jad/system
  861112 10:53:26 jad add HomoginizeDescriptorIfPossible to
speed things up

 ************************************************************************/
/************************************************************************
  vmachine.c 42 replace /users/jad/system
  861116 16:00:32 jad in order to split up primfns, needed to make the
primitive function array owned by vmachine

 ************************************************************************/
/************************************************************************
  vmachine.c 43 replace /users/jad/system
  861119 12:42:16 jad added ln, exp, and sqrt primitives

 ************************************************************************/
/************************************************************************
  vmachine.c 44 replace /users/jad/system
  861201 18:33:46 jad added a dumpPrimitiveTable debug method

 ************************************************************************/
/************************************************************************
  vmachine.c 45 replace /users/mjc/system
  861201 21:04:14 mjc Fixed destruction of 'distribution' bug in 'HomogenizeDescriptorIfPossible'

 ************************************************************************/
/************************************************************************
  vmachine.c 46 replace /users/mjc/system
  861226 12:25:50 mjc Implemented 'super'

 ************************************************************************/
/************************************************************************
  vmachine.c 47 replace /users/mjc/system
  870105 10:24:55 mjc Fixed failure to pop parameter stack for parametric intensional messages

 ************************************************************************/
/************************************************************************
  vmachine.c 48 replace /users/jad/system
  870128 19:06:42 jad modified LoadAccumulatorWithPredicateResult routine to be more general

 ************************************************************************/
/************************************************************************
  vmachine.c 49 replace /users/jad/system
  870205 14:59:00 jad moved DecodeClosureInAccumulator from PFutil to vmachine

 ************************************************************************/
/************************************************************************
  vmachine.c 50 replace /users/jad/system
  870210 14:20:12 jad implement LoadAccumulatorWithStringStore routine

 ************************************************************************/
/************************************************************************
  vmachine.c 51 replace /users/jad/system
  870220 18:58:01 jad implement LoadAccumulatorWithDomainLimitedDateFnResult routine for date na's

 ************************************************************************/
/************************************************************************
  vmachine.c 52 replace /users/jad/system
  870317 12:29:55 jad add interrupt handling

 ************************************************************************/
/************************************************************************
  vmachine.c 53 replace /users/jck/system
  870420 13:34:14 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  vmachine.c 54 replace /users/mjc/translation
  870524 09:42:53 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  vmachine.c 55 replace /users/jck/system
  870605 16:03:07 jck Generalized interface to RTindex

 ************************************************************************/
/************************************************************************
  vmachine.c 56 replace /users/mjc/system
  870607 03:15:18 mjc Fixed VAX compilation errors

 ************************************************************************/
/************************************************************************
  vmachine.c 57 replace /users/jck/system
  870611 10:21:53 jck Corrected a bug in the evaluation of time series

 ************************************************************************/
/************************************************************************
  vmachine.c 58 replace /users/mjc/system
  870627 10:54:27 mjc Implemented 'SendBinaryConverseToCurrent'

 ************************************************************************/
/************************************************************************
  vmachine.c 59 replace /users/jad/system
  871215 14:10:07 jad release a public V_NormalizeDescriptor function

 ************************************************************************/
/************************************************************************
  vmachine.c 60 replace /users/jck/system
  880107 08:42:13 jck Added omitted final arguments to rtVECTOR_*SubsetInStore calls

 ************************************************************************/
/************************************************************************
  vmachine.c 61 replace /users/jad/system
  880509 12:16:43 jad Implement 'sprint' for Blocks, Methods, and Closures

 ************************************************************************/
/************************************************************************
  vmachine.c 62 replace /users/jad/system
  880711 16:39:03 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  vmachine.c 63 replace /users/jck/system
  881011 14:12:55 jck Made the handling of large vision inputs more robust

 ************************************************************************/
/************************************************************************
  vmachine.c 64 replace /users/jck/system
  890413 13:53:24 jck Added the ability to split homomorphic tasks into subtasks

 ************************************************************************/
/************************************************************************
  vmachine.c 65 replace /users/jck/system
  890419 09:53:54 jck Finished making vmachine's handling of missing parameters neater

 ************************************************************************/
/************************************************************************
  vmachine.c 66 replace /users/jck/system
  890421 21:26:13 jck Fixed the warning messages for missing parameters

 ************************************************************************/
/************************************************************************
  vmachine.c 67 replace /users/m2/backend
  890503 15:36:11 m2 STD_sprintf() change

 ************************************************************************/
/************************************************************************
  vmachine.c 68 replace /users/jck/system
  890808 16:03:00 jck Fixed bug in fault recording

 ************************************************************************/
/************************************************************************
  vmachine.c 69 replace /users/jck/system
  890808 21:15:14 jck Removed STD_sprintf call temporarily

 ************************************************************************/
/************************************************************************
  vmachine.c 70 replace /users/jck/system
  890828 16:35:51 jck Replaced STD_sprintf call in most recent version

 ************************************************************************/

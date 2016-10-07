/*****  The Transient Storage Manager  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName TS

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

/*****  Shared  *****/
#include "ts.h"


/*********************************
 *********************************
 *****  Profiling Utilities  *****
 *********************************
 *********************************/

/**************************************
 *****  Debugging Trace Switches  *****
 **************************************/

PrivateVarDef bool TracingAllocator	= false;

/****************************
 *****  Usage Counters  *****
 ****************************/


/**********************************************************************
 **********************************************************************
 *****  Container Allocation, Copying, and Reallocation Routines  *****
 **********************************************************************
 **********************************************************************/

/*********************************
 *****  Size Rounding Macro  *****
 *********************************/

#define RoundContainerSize(size) ((int)(size) + 3 & ~3)


/*********************************
 *****  Container Allocator  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to allocate a container in transient storage.
 *
 *  Arguments:
 *	type			- the representation type of the new container.
 *	size			- the size in bytes of the new container.  The
 *				  actual size of the container may be adjusted
 *				  upward as needed.
 *	xOS			- the index of the container's object space.
 *	xCTE			- the index of the container in that space's
 *				  container table.
 *
 *  Returns:
 *	A pointer to the preamble of the allocated container.
 *
 *****/
PublicFnDef M_CPreamble* TS_AllocateContainer (
    RTYPE_Type type, size_t size, unsigned int xOS, unsigned int xCTE
)
{
    if (size > UINT_MAX) {
	ERR_SignalFault (
	    EC__PrivateMemoryError,
	    "Requested Size exceeds Maximum (4Gb)"
	);
    }
    size = RoundContainerSize (size);

    M_CPreamble* result = (M_CPreamble*) UTIL_Malloc (
	sizeof (M_CPreamble) + sizeof(M_CEndMarker) + size
    );

    M_CPreamble_NSize			(result) = size;
    M_CPreamble_RType			(result) = (unsigned int)(type);
    M_CPreamble_OSize			(result) = 0;
    M_CPreamble_POPObjectSpace		(result) = xOS;
    M_CPreamble_POPContainerIndex	(result) = xCTE;

    M_CEndMarker_Size (M_CPreamble_EndMarker (result)) = size;

    if (TracingAllocator) IO_printf (
	"...TS_AllocateContainer: [%u:%u] -> %08X + %u.\n",
	M_CPreamble_POPObjectSpace (result),
	M_CPreamble_POPContainerIndex (result),
	result,
	M_CPreamble_NSize (result)
    );

    return result;
}

PublicFnDef M_CPreamble* TS_AllocateContainer (
    RTYPE_Type type, size_t size, M_POP iPOP
)
{
    return TS_AllocateContainer (
	type, size, M_POP_D_ObjectSpace (iPOP), M_POP_D_ContainerIndex (iPOP)
    );
}


/******************************
 *****  Container Copier  *****
 ******************************/

/*---------------------------------------------------------------------------
 *****  Routine to copy a container.
 *
 *  Argument:
 *	oldPreamblePtr		- a pointer to the preamble of an already
 *				  existing container
 *
 *  Returns:
 *	A pointer to the preamble of the copied container.
 *
 *****/
PublicFnDef M_CPreamble* TS_CopyContainer (M_CPreamble* oldPreamblePtr) {
    size_t			size = (size_t)M_CPreamble_Size (oldPreamblePtr)
					+ sizeof (M_CPreamble)
					+ sizeof (M_CEndMarker);
    M_CPreamble*		result = (M_CPreamble*) UTIL_Malloc (size);

    memcpy (result, oldPreamblePtr, size);

    M_CPreamble_FixSizes (result);

    if (TracingAllocator) IO_printf (
	"...TS_CopyContainer: %08X -> %08X.\n", oldPreamblePtr, result
    );

    return result;
}


/***********************************
 *****  Container Reallocator  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to re-size a container in transient storage.
 *
 *  Arguments:
 *	containerAddress	- the address of the preamble of the container
 *				  to be resized.
 *	newSize			- the new size the container in bytes.
 *
 *  Returns:
 *	The new address of the container.
 *
 *****/
PublicFnDef M_CPreamble* TS_ReallocateContainer (
    M_CPreamble* containerAddress, size_t newSize
)
{
    M_CPreamble_ValidateFraming (containerAddress);

    if (newSize > UINT_MAX) {
	ERR_SignalFault (
	    EC__PrivateMemoryError,
	    "Requested Size exceeds Maximum (4Gb)"
	);
    }
    newSize = RoundContainerSize (newSize);

    M_CPreamble* result = (M_CPreamble*) UTIL_Realloc (
	(pointer_t)containerAddress,
	sizeof (M_CPreamble) + sizeof (M_CEndMarker) + newSize
    );

    M_CPreamble_NSize (result) = newSize;
    M_CEndMarker_Size (M_CPreamble_EndMarker (result)) = newSize;

    if (TracingAllocator) IO_printf (
	"...TS_ReallocateContainer: NS:%d, NP:%08X, OP:%08X\n",
	newSize, result, containerAddress
    );

    return result;
}


/***********************************
 *****  Container Deallocator  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Routine to free a container in transient storage.
 *
 *  Argument:
 *	containerAddress	- the address of the transient container
 *				  to be freed.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void TS_DeallocateContainer (M_CPreamble* containerAddress) {
/*****  Check for reasonable container framing, ...  *****/
    M_CPreamble_ValidateFraming (containerAddress);

/*****  ... report the freeing of the container, ...  *****/
    if (TracingAllocator) IO_printf (
	"...TS_DeallocateContainer: Container at %08X freed.\n",
	containerAddress
    );

/*****  ... and free it.  *****/
    UTIL_Free ((pointer_t)containerAddress);
}


/*******************************
 *******************************
 *****  Facility Services  *****
 *******************************
 *******************************/

/******************************
 *****  Debugger Methods  *****
 ******************************/

IOBJ_DefineUnaryMethod (TracesOnDM) {
    TracingAllocator	= true;

    return self;
}

IOBJ_DefineUnaryMethod (TracesOffDM) {
    TracingAllocator	= false;

    return self;
}

IOBJ_DefineNilaryMethod (AllocatorTraceDM) {
    return IOBJ_SwitchIObject (&TracingAllocator);
}

/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    static bool alreadyInitialized = false;

    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"		, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("tracesOn"		, TracesOnDM)
	IOBJ_MDE ("tracesOff"		, TracesOffDM)
	IOBJ_MDE ("allocatorTrace"	, AllocatorTraceDM)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (TS);
    FAC_FDFCase_FacilityDescription ("The Transient Storage Manager");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    FAC_FDFCase_FacilityMD (methodDictionary);

    case FAC_DoStartupInitialization:
    /*****  Check for multiple initialization requests  *****/
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}

	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
        break;
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  ts.c 1 replace /users/mjc/system
  870216 00:08:42 lcn Release of new memory manager

 ************************************************************************/
/************************************************************************
  ts.c 2 replace /users/mjc/translation
  870524 09:42:03 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  ts.c 3 replace /users/jck/system
  890411 13:43:29 jck Added a check against maximum container size to the container allocation routines

 ************************************************************************/
/************************************************************************
  ts.c 4 replace /users/m2/backend
  890828 17:09:25 m2 Moved ValidateContainerFraming macro from ts.c

 ************************************************************************/

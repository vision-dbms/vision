/*****  System Statistics Primitive Function Services Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfSTATS

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "batchvision.h"
#include "m.h"
#include "rslang.h"
#include "ps.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTblock.h"
#include "RTclosure.h"
#include "RTcontext.h"
#include "RTdictionary.h"
#include "RTdsc.h"
#include "RTindex.h"
#include "RTlink.h"
#include "RTlstore.h"
#include "RTmethod.h"
#include "RTparray.h"
#include "RTptoken.h"
#include "RTstring.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"

#include "VfGuardTool.h"

#include "VOutputBuffer.h"

#include "V_VAllocator.h"

/*****  Self  *****/
#include "PFstats.h"


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
 *	  85 -  86                      *
 *	 490 - 499			*
 *	 600 - 624			*
 *======================================*/


/********************************************
 *****  Session Information Primitives  *****
 ********************************************/

#define XTotalNetworkAllocation		       600
#define XSpacesInNetwork		       601
#define XAccessedNetworkVersion		       602
#define XNetworkUpdateThread		       603
#define XProcessID			       604
#define XSpacesInSession		       605

#define XVersionInformation		       606
/******************************************
 *****  Space Information Primitives  *****
 ******************************************/

#define XContainerRefCount			609
#define XTotalSpaceAllocation			610
#define XContainerTableSize			611
#define XAccessContainer			612
#define XContainerSize				613
#define XContainerType				614
#define XContainerSegment			615
#define XContainerSegmentOffset			616
#define XContainerAddrType			617
#define XContainerPOPCnt			618
#define XPackPOPWithContainer			619

/***********************************
 *****  POP Access Primitives  *****
 ***********************************/

#define XPOPExtractObjectSpace			620
#define XPOPExtractContainerIndex		621
#define XPOPGetNthPOP				622
#define XCleanDictionary			623
#define XCleanDescriptor			624

/********************************************
 *****  Cluster Information Primitives  *****
 ********************************************/

#define XObjectSpace 				85
#define XContainerIndex				86

#define XClusterID				490
#define XClusterType				491
#define XClusterSize				492
#define XClusterReferenceMapSize		493
#define XClusterSuperPointerType		494
#define XClusterSuperPointerSize		495
#define XClusterSchemaID			496
#define XClusterBoolean				497

#define XClusterReferenceMapData		499


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


/*********************************
 *****  Version Information  *****
 *********************************/
#ifdef VisionBuild
// The following struct is defined in maketools/templates/auditshell.c
PublicVarDecl struct program_description_t {
    char const *auditFile, *auditEntry, *compilationLogin, *compilationDate, *platform, *release;
} ProgramDescription;

V_DefinePrimitive (VersionInformation) {

    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.auditFile
	    )
	);
	break;
    case 1:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.auditEntry
	    )
	);
	break;
    case 2:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.compilationLogin
	    )
	);
	break;
    case 3:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.compilationDate
	    )
	);
	break;
    case 4:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.platform
	    )
	);
	break;
    case 5:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.release
	    )
	);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("VersionInformation");
	break;
    }

}
#else
V_DefinePrimitive (VersionInformation) {
    pTask->loadDucWithNA ();
}
#endif

/**************************************
 *****  Total Network Allocation  *****
 **************************************/

V_DefinePrimitive (TotalNetworkAllocation) {
    double result = 0;

    switch (V_TOTSC_PrimitiveFlags) {
    case 0:
	result = ENVIR_Session ()->CurrentTransientAllocationLevel ();
	break;
    case 1:
	result = ENVIR_Session ()->TransientAllocationHighWaterMark ();
	break;
    case 2:
	result = static_cast<double> (ThisProcess.memoryAllocationLimit ());
	pTask->loadDucWithDouble (result);
	return;
    case 3:
	double placeHolder;
	M_AccumulateAllocationStatistics (&placeHolder, &result);
	pTask->loadDucWithDouble (result);
	return;
    default:
	pTask->raiseUnimplementedAliasException ("TotalNetworkAllocation");
	break;
    }

    result += UTIL_FreeListTotal;
    result += VOutputBuffer::cellAllocatorCurrentAllocationAmount ();

    pTask->loadDucWithDouble (result);
}


/*********************************
 *****  Session Information  *****
 *********************************/

V_DefinePrimitive (SessionInformation) {
    unsigned int result = 0, placeHolder = 0;

    switch (V_TOTSC_Primitive) {
    case XSpacesInNetwork:
	result = pTask->codDatabase()->SpaceCount ();
	break;
    case XAccessedNetworkVersion:
	result = pTask->codDatabase()->PhysicalAND ()->AccessFO ();
	break;
    case XNetworkUpdateThread:
	result = pTask->codDatabase()->PhysicalAND ()->UpdateFO ();
	break;
    case XProcessID:
	result = getpid ();
	break;
    case XSpacesInSession:
	result = pTask->codDatabase()->ActiveSpaceCount ();
	break;
    default:
	pTask->raiseUnimplementedAliasException ("SessionInformation");
	break;
    }

    pTask->loadDucWithInteger (result);
}


/*******************************
 *****  Space Information  *****
 *******************************/

/***********************
 *----  Accessors  ----*
 ***********************/

PrivateFnDef double GetAllocation (
    M_AND *pNetwork, unsigned int xSpace, bool *spaceValid
) {
    M_ASD* asd = pNetwork->AccessSpace (xSpace, spaceValid);
    return asd ? static_cast<double>(asd->TransientAllocation ()) : 0;
}

PrivateFnDef unsigned int GetCTEntryCount (
    M_AND *pNetwork, unsigned int xSpace, bool *spaceValid
) {
    M_ASD* asd = pNetwork->AccessSpace (xSpace);
    *spaceValid = IsntNil (asd);
    return *spaceValid ? asd->cteCount () : 0;
}


/*******************
 *----  Types  ----*
 *******************/

typedef unsigned int (*SpaceAccessor) (
    M_AND *pNetwork, unsigned int xSpace, bool *spaceValid
);

typedef double (*SpaceAllocAccessor) (
    M_AND *pNetwork, unsigned int xSpace, bool *spaceValid
);

/*********************
 *----  Helpers  ----*
 *********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize an 'IntUV' with accessed
 *****  object space values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  a CPD for the source u-vector, a pointer to
 *				  the guard tool used to return partial
 *				  results, and an accessor function.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeIntUVWithSpaceAccessor (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    M_CPD		*pSource = iArgList.arg<M_CPD*>();
    VfGuardTool		*pGuardTool = iArgList.arg<VfGuardTool*>();
    SpaceAccessor	accessor = iArgList.arg<SpaceAccessor>();

    M_AND *pNetwork = pSource->ReferencedNetwork (UV_CPx_RefPToken);

    rtINTUV_ElementType	*tp = rtINTUV_CPD_Array (resultCPD);
    rtINTUV_ElementType	*sp = rtINTUV_CPD_Array (pSource),
			*sl = sp + nelements;

    while (sp < sl) {
	bool spaceValid = false;
	*tp++ = (rtINTUV_ElementType) accessor (pNetwork, *sp++, &spaceValid);
	if (spaceValid)
	    pGuardTool->addNext ();
	else
	    pGuardTool->skipNext ();
    }
}

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize an 'IntUV' with accessed
 *****  object space values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  a CPD for the source u-vector, a pointer to
 *				  the guard tool used to return partial
 *				  results, and an accessor function.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeDoubleUVWithSpaceAccessor (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    M_CPD		*pSource = iArgList.arg<M_CPD*>();
    VfGuardTool		*pGuardTool = iArgList.arg<VfGuardTool*>();
    SpaceAllocAccessor	accessor = iArgList.arg<SpaceAllocAccessor>();

    M_AND *pNetwork = pSource->ReferencedNetwork (UV_CPx_RefPToken);

    rtDOUBLEUV_ElementType	*tp = rtDOUBLEUV_CPD_Array (resultCPD);
    rtINTUV_ElementType		*sp = rtINTUV_CPD_Array (pSource),
				*sl = sp + nelements;

    while (sp < sl) {
	bool spaceValid = false;
	*tp++ = (rtDOUBLEUV_ElementType) accessor (pNetwork, *sp++, &spaceValid);
	if (spaceValid)
	    pGuardTool->addNext ();
	else
	    pGuardTool->skipNext ();
    }
}


/************************
 *----  Primitives  ----*
 ************************/

V_DefinePrimitive (SpaceAllocation) {
/*****  Decode the alias by which this primitive was invoked...  *****/
    SpaceAllocAccessor accessor;
    switch (V_TOTSC_Primitive) {
    case XTotalSpaceAllocation:
	accessor = GetAllocation;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("SpaceInformation");
	break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    if (rCurrent.isScalar ()) {
	bool spaceValid = false;
	double result = accessor (
	    rCurrent.codDatabase (), DSC_Descriptor_Scalar_Int (rCurrent), &spaceValid
	);
	if (spaceValid)
	    pTask->loadDucWithDouble (result);
	else
	    pTask->loadDucWithNA ();
    }
    else {
	VfGuardTool iGuardTool (V_TOTSC_PToken);
	pTask->loadDucWithPartialFunction (
	    iGuardTool, rtDOUBLEUV_New (
		V_TOTSC_PToken, pTask->codKOT()->TheDoublePTokenHandle (),
		InitializeDoubleUVWithSpaceAccessor,
		(M_CPD*)DSC_Descriptor_Value (rCurrent), &iGuardTool, accessor
	    )
	);
    }
}

V_DefinePrimitive (SpaceInformation) {
/*****  Decode the alias by which this primitive was invoked...  *****/
    SpaceAccessor accessor;
    switch (V_TOTSC_Primitive) {
    case XContainerTableSize:
	accessor = GetCTEntryCount;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("SpaceInformation");
	break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    if (rCurrent.isScalar ()) {
	bool spaceValid = false;
	unsigned int result = accessor (
	    rCurrent.codDatabase (), DSC_Descriptor_Scalar_Int (rCurrent), &spaceValid
	);
	if (spaceValid)
	    pTask->loadDucWithInteger (result);
	else
	    pTask->loadDucWithNA ();
    }
    else {
	VfGuardTool iGuardTool (V_TOTSC_PToken);
	pTask->loadDucWithPartialFunction (
	    iGuardTool, rtINTUV_New (
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenHandle (),
		InitializeIntUVWithSpaceAccessor,
		(M_CPD*)DSC_Descriptor_Value (rCurrent), &iGuardTool, accessor
	    )
	);
    }
}


/************************
 *****  POP Packer  *****
 ************************/

/********************
 *----  Packer  ----*
 ********************/

bool POPPacker (M_POP *pPOP, unsigned int xSpace, unsigned int xContainer) {
    if (xSpace <= M_POP_MaxObjectSpace  && xContainer <= M_POP_MaxContainerIndex) {
	M_POP_ObjectSpace    (pPOP) = xSpace;
	M_POP_ContainerIndex (pPOP) = xContainer;
	return true;
    }
    return false;
}


/********************
 *----  Helper  ----*
 ********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a 'IntUV' routine with accessed
 *****  container values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  1) a CPD for the space u-vector,
 *				  2) a CPD for the container index u-vector,
 *				  2) a guard tool used to record partial results,
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeIntUVWithPackedPOP (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    rtINTUV_ElementType	*tp = rtINTUV_CPD_Array (resultCPD);
    rtINTUV_ElementType	*sp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>()),
			*sl = sp + nelements,
			*cp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>());
    VfGuardTool *pGuardTool = iArgList.arg<VfGuardTool*>();

    while (sp < sl) {
	M_POP iPOP;
	if (POPPacker (&iPOP, *sp++, *cp++)) {
	    *tp++ = M_POP_D_AsInt (iPOP);
	    pGuardTool->addNext ();
	}
	else {
	    *tp++ = 0;
	    pGuardTool->skipNext ();
	}
    }
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (PackPOP) {
/*****  Determine which alias was invoked ... *****/
    switch (V_TOTSC_Primitive) {
    case XPackPOPWithContainer:
	break;
    default:
	pTask->raiseUnimplementedAliasException ("PackPOP");
	break;
    }

/*****  Acquire the operand...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype () ||
	pTask->ducStore ()->DoesntNameTheIntegerClass ()
    ) {
	pTask->sendBinaryConverseWithCurrent ("_packPOPWithSpace:");
	return;
    }

    pTask->normalizeDuc ();

    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    if (rCurrent.isScalar ()) {
	M_POP iPOP;
	unsigned int xSpace = DSC_Descriptor_Scalar_Int (rCurrent);
	unsigned int xCntnr = DSC_Descriptor_Scalar_Int (ADescriptor);
	if (POPPacker (&iPOP, xSpace, xCntnr))
    	    pTask->loadDucWithInteger (M_POP_D_AsInt (iPOP));
	else
	    pTask->loadDucWithNA ();
    }
    else {
	VfGuardTool iGuardTool (V_TOTSC_PToken);
	pTask->loadDucWithPartialFunction (
	    iGuardTool, rtINTUV_New (
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenHandle (),
		InitializeIntUVWithPackedPOP,
		(M_CPD *)DSC_Descriptor_Value (rCurrent),
		(M_CPD *)DSC_Descriptor_Value (ADescriptor),
		&iGuardTool
	    )
	);
    }
}


/**************************
 *****  POP Unpacker  *****
 **************************/

/***********************
 *----  Accessors  ----*
 ***********************/

PrivateFnDef unsigned int ExtractObjectSpace (M_POP const *pPOP) {
    return M_POP_ObjectSpace (pPOP);
}

PrivateFnDef unsigned int ExtractContainerIndex (M_POP const *pPOP) {
    return M_POP_ContainerIndex (pPOP);
}


/*******************
 *----  Types  ----*
 *******************/

typedef unsigned int (*POPUnpacker) (M_POP const *pPOP);

/********************
 *----  Helper  ----*
 ********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize an 'IntUV' with accessed
 *****  object space values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  a CPD for the source u-vector, and
 *				  a POP unpacker.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeIntUVWithPOPPart (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    rtINTUV_ElementType	*tp = rtINTUV_CPD_Array (resultCPD);
    rtINTUV_ElementType	*sp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>()),
			*sl = sp + nelements;

    POPUnpacker unpacker = iArgList.arg<POPUnpacker>();

    while (sp < sl) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = *sp++;
	*tp++ = (rtINTUV_ElementType) unpacker (&iPOP);
    }
}


/************************
 *----  Primitives  ----*
 ************************/

V_DefinePrimitive (UnpackPOP) {
/*****  Determine which alias was invoked ... *****/
    POPUnpacker unpacker;

    switch (V_TOTSC_Primitive) {
    case XPOPExtractObjectSpace:
	unpacker = ExtractObjectSpace;
	break;
    case XPOPExtractContainerIndex:
	unpacker = ExtractContainerIndex;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("UnpackPOP");
	break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);
    	pTask->loadDucWithInteger (unpacker (&iPOP));
    }
    else if (rCurrent.holdsNonScalarValues ()) {
	M_CPD *resultUV = rtINTUV_New (
	    V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenHandle (),
	    InitializeIntUVWithPOPPart, (M_CPD *)DSC_Descriptor_Value (rCurrent), unpacker
	);
	pTask->loadDucWithMonotype (resultUV);
	resultUV->release ();
    }
    else
	pTask->loadDucWithNA ();
}


/***********************************
 *****  Container Information  *****
 ***********************************/

/***********************
 *----  Accessors  ----*
 ***********************/

PrivateFnDef unsigned int GetContainerSize (M_AND *pNetwork, M_POP const *pPOP) {
    VContainerHandle::Reference pContainerHandle (pNetwork->GetContainerHandle (pPOP));
    return pContainerHandle->containerSize () + M_SizeOfPreambleType + M_SizeOfEndMarker;
}

PrivateFnDef unsigned int GetContainerType (M_AND *pNetwork, M_POP const *pPOP) {
    VContainerHandle::Reference pContainerHandle (pNetwork->GetContainerHandle (pPOP));
    return pContainerHandle->RType ();
}

PrivateFnDef unsigned int GetContainerSegment (M_AND *pNetwork, M_POP const *pPOP) {
    PS_CTE iCTE;
    return pNetwork->AccessSpace (pPOP)->GetLiveCTE (M_POP_ContainerIndex (pPOP), iCTE)
	? PS_CTE_Segment (iCTE) : UINT_MAX;
}

PrivateFnDef unsigned int GetContainerSegmentOffset (M_AND *pNetwork, M_POP const *pPOP) {
    PS_CTE iCTE;
    return pNetwork->AccessSpace (pPOP)->GetLiveCTE (M_POP_ContainerIndex (pPOP), iCTE)
	? PS_CTE_Offset (iCTE) : UINT_MAX;
}

PrivateFnDef unsigned int GetContainerAddrType (M_AND *pNetwork, M_POP const *pPOP) {
    M_DCTE const &cte = *pNetwork->AccessSpace (pPOP)->cte (M_POP_ContainerIndex (pPOP));
    unsigned int addrType = cte.addressType ();

    if (M_CTEAddressType_CPCC == addrType) {
	addrType = cte.addressAsContainerHandle ()->hasAReadWriteContainer ()
		   ? M_CTEAddressType_RWContainer
		   : M_CTEAddressType_ROContainer;
    }

    return addrType;
}

PrivateFnDef unsigned int GetContainerRefCount (M_AND *pNetwork, M_POP const *pPOP) {
    return pNetwork->AccessSpace (pPOP)->cte (
	M_POP_ContainerIndex (pPOP)
    )->referenceCount ();
}


PrivateFnDef unsigned int GetContainerPOPCnt (M_AND *pNetwork, M_POP const *pPOP) {
    VContainerHandle::Reference pHandle (pNetwork->GetContainerHandle (pPOP));
    return pHandle->containerAddress () ? pHandle->getPOPCount (): 0;
}


/*******************
 *----  Types  ----*
 *******************/

typedef unsigned int (*ContainerAccessor) (M_AND *pNetwork, M_POP const *pPOP);
typedef bool (M_AND::*ContainerValidator) (M_POP const *pPOP);

/********************
 *----  Helper  ----*
 ********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a 'IntUV' routine with accessed
 *****  container values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  1) a CPD for the POP u-vector, 
 *				  2) a guard tool used to record partial results,
 *				  3) a container accessor function, and
 *				  4) a validator function.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeIntUVWithContainerAccessor (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    M_CPD		*pSource	= iArgList.arg<M_CPD*>();
    VfGuardTool		*pGuardTool	= iArgList.arg<VfGuardTool*>();
    ContainerAccessor	 accessor	= iArgList.arg<ContainerAccessor>();
    ContainerValidator	 validator	= iArgList.arg<ContainerValidator>();

    M_AND *pNetwork = pSource->ReferencedNetwork (UV_CPx_RefPToken);

    rtINTUV_ElementType	*tp = rtINTUV_CPD_Array (resultCPD);
    rtINTUV_ElementType	*sp = rtINTUV_CPD_Array (pSource),
			*sl = sp + nelements;

    while (sp < sl) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = *sp++;
	if ((pNetwork->*validator) (&iPOP))  {
	    *tp++ = (rtINTUV_ElementType) accessor (pNetwork, &iPOP);
	    pGuardTool->addNext ();
	}
	else {
	    *tp++ = 0;
	    pGuardTool->skipNext ();
	}
    }
}


/************************
 *----  Primitives  ----*
 ************************/

V_DefinePrimitive (ContainerInformation) {
/*****  Determine which alias was invoked ... *****/
    ContainerAccessor accessor;
    ContainerValidator validator;
    switch (V_TOTSC_Primitive) {
    case XContainerSize:
	accessor = GetContainerSize;
	validator = &M_AND::ContainerExists;
	break;
    case XContainerType:
	accessor = GetContainerType;
	validator = &M_AND::ContainerExists;
	break;
    case XContainerSegment:
	accessor = GetContainerSegment;
	validator = &M_AND::ContainerPersists;
	break;
    case XContainerSegmentOffset:
	accessor = GetContainerSegmentOffset;
	validator = &M_AND::ContainerPersists;
	break;
    case XContainerRefCount:
	accessor = GetContainerRefCount;
	validator = &M_AND::ContainerExists;
	break;
    case XContainerAddrType:
	accessor = GetContainerAddrType;
	validator = &M_AND::ContainerExists;
	break;
    case XContainerPOPCnt:
	accessor = GetContainerPOPCnt;
	validator = &M_AND::ContainerExists;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ContainerInformation");
	break;
    }

/*****  Acquire the operand...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    M_AND *pNetwork = pTask->codDatabase ();

    M_POP iPOP;
    if (rCurrent.isScalar ()) {
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);
	if ((pNetwork->*validator) (&iPOP))
    	    pTask->loadDucWithInteger (accessor (pNetwork, &iPOP));
	else
	    pTask->loadDucWithNA ();
    }
    else {
	VfGuardTool iGuardTool (V_TOTSC_PToken);
	pTask->loadDucWithPartialFunction (
	    iGuardTool, rtINTUV_New (
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenHandle (),
		InitializeIntUVWithContainerAccessor,
		(M_CPD *)DSC_Descriptor_Value (rCurrent), &iGuardTool, accessor, validator
	    )
	);
    }
}


/***************************
 *****  NthPOP Access  *****
 ***************************/

/***********************
 *----  Accessors  ----*
 ***********************/

bool rtDSC_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *storePOP ();
	break;
    case 1:
	*pResult = *pointerPOP ();
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtLINK_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *pptPOP ();
	break;
    case 1:
	*pResult = *rptPOP ();
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtUVECTOR_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *pptPOP ();
	break;
    case 1:
	*pResult = *rptPOP ();
	break;
    case 2:
	*pResult = *auxPOP ();
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtBLOCK_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *dictionaryPOP ();
	break;
    default:
	if (xReference <= nestedBlockCount ())
	    *pResult = *addressOfPOP (xReference - 1);
	else
	    bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtCLOSURE_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *m_pContext->containerIdentity ();
	break;
    case 1:
	*pResult = *m_pBlock->containerIdentity ();
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtPARRAY_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = xReference < elementCount ();
    if (bResultValid)
	*pResult = *element (xReference);
    return bResultValid;
}

bool rtPOPVECTOR_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = xReference < elementCount ();
    if (bResultValid)
	*pResult = *element (xReference);
    return bResultValid;
}

bool rtVECTOR_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *ptokenPOP ();
	break;
    default:
	if (xReference <= rtVECTOR_USD_POPsPerUSD * segmentArraySize ())
	    *pResult = *((M_POP*)segmentArray () + xReference - 1);
	else
	    bResultValid = false;
	break;
    }
    return bResultValid;
}

bool rtLSTORE_Handle::getPOP (M_POP *pResult, unsigned int xReference) const {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rowPTokenPOP ();
	break;
    case 1:
	*pResult = *contentPOP ();
	break;
    case 2:
	*pResult = *contentPTokenPOP ();
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}


PrivateFnDef bool GetNthPOPFromPOP (
    M_AND *pNetwork, M_POP const *pPOP, unsigned int xReference, M_POP *pResult
) {
    VContainerHandle::Reference pHandle (pNetwork->SafeGetContainerHandle (pPOP));
    return pHandle && pHandle->containerAddress () && pHandle->getPOP (pResult, xReference);
}


/********************
 *----  Helper  ----*
 ********************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a 'IntUV' routine with accessed
 *****  container values.
 *
 *  Arguments:
 *	resultCPD		- a initialized standard CPD for the u-vector
 *				  being created.
 *	nelements		- the number of elements in the u-vectors.
 *	ap			- a <stdargs.h> argument pointer referencing
 *				  1) a CPD for the space Index u-vector, 
 *				  2) a CPD for the container Index u-vector, 
 *				  3) a guard tool for returning partial results
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void InitializeIntUVWithNthPOP (
    M_CPD *resultCPD, size_t nelements, va_list ap
) {
    V::VArgList iArgList (ap);
    M_CPD		*pSource = iArgList.arg<M_CPD*>();
    rtINTUV_ElementType	*pp = rtINTUV_CPD_Array (iArgList.arg<M_CPD*>());
    VfGuardTool		*pGuardTool = iArgList.arg<VfGuardTool*>();

    M_AND *pNetwork = pSource->ReferencedNetwork (UV_CPx_RefPToken);

    rtINTUV_ElementType	*tp = rtINTUV_CPD_Array (resultCPD);
    rtINTUV_ElementType	*sp = rtINTUV_CPD_Array (pSource),
			*sl = sp + nelements;

    while (sp < sl) {
	M_POP iSource, iResult;
	M_POP_D_AsInt (iSource) = *sp++;
	if (GetNthPOPFromPOP (pNetwork, &iSource, *pp++, &iResult)) {
	    *tp++ = M_POP_D_AsInt (iResult);
	    pGuardTool->addNext ();
	}
	else {
	    *tp++ = 0;
	    pGuardTool->skipNext ();
	}
    }
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (GetNthPOP) {
/*****  Acquire the operand...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype () ||
	pTask->ducStore ()->DoesntNameTheIntegerClass ()
    ) {
	pTask->sendBinaryConverseWithCurrent ("_nthPOPReferencedBy:");
	return;
    }

    pTask->normalizeDuc ();
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    
    if (rCurrent.isScalar ()) {
	M_POP iSource, iResult;
	M_POP_D_AsInt (iSource) = DSC_Descriptor_Scalar_Int (rCurrent);
	unsigned int xReference = DSC_Descriptor_Scalar_Int (ADescriptor);

	if (GetNthPOPFromPOP (rCurrent.codDatabase (), &iSource, xReference, &iResult))
    	    pTask->loadDucWithInteger (M_POP_D_AsInt (iResult));
	else
	    pTask->loadDucWithNA ();
    }
    else {
	VfGuardTool iGuardTool (V_TOTSC_PToken);
	pTask->loadDucWithPartialFunction (
	    iGuardTool, rtINTUV_New (
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenHandle (),
		InitializeIntUVWithNthPOP,
		(M_CPD *)DSC_Descriptor_Value (rCurrent),
		(M_CPD *)DSC_Descriptor_Value (ADescriptor),
		&iGuardTool
	    )
	);
    }
}


/***********************************
 *****  Container Maintenance  *****
 ***********************************/

V_DefinePrimitive (ContainerCleanup) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();
    M_AND *pNetwork = rCurrent.codDatabase ();

    if (rCurrent.isScalar ()) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);
	pTask->loadDucWithBoolean (pNetwork->alignAll (&iPOP));
    }
    else {
	M_CPD *pSource = DSC_Descriptor_Value (rCurrent);
	rtINTUV_ElementType *sp = rtINTUV_CPD_Array (pSource),
			    *sl = sp + UV_CPD_ElementCount (pSource);

	VfGuardTool iGuardTool (V_TOTSC_PToken);
	while (sp < sl) {
	    M_POP iPOP;
	    M_POP_D_AsInt (iPOP) = *sp++;
	    if (pNetwork->alignAll (&iPOP))
		iGuardTool.addNext ();
	    else
		iGuardTool.skipNext ();
	}
	pTask->loadDucWithBoolean (iGuardTool.commit ());
    }
}


/***************************
 *****  Object Access  *****
 ***************************/

/********************
 *----  Helper  ----*
 ********************/

PrivateFnDef bool ConvertPOPToDsc (
    VPrimitiveTask *pTask, M_POP const *pPOP, DSC_Descriptor &rResult
) {
    bool bResultValid = false;
    bool bReferenceFirstElement = false;
    VContainerHandle::Reference pStoreHandle (pTask->codDatabase ()->SafeGetContainerHandle (pPOP));
    if (pStoreHandle) {
	if (pStoreHandle->RType () == RTYPE_C_Block) {
	    VString iSource; {
		M_CPD *pStoreCPD = pStoreHandle->GetCPD ();
		RSLANG_Decompile (iSource, pStoreCPD);
		pStoreCPD->release ();
	    }
	    pStoreHandle.setTo (
		rtLSTORE_NewStringStoreWithDelm (pTask->codScratchPad (), "[", "]", iSource)
	    );
	    // This is the implementation of POP asObject.
	    // Normally, we want to return the referenceNil for the store,
	    // but in the case of a block, we want to return the source code for the block
	    // so we special case this type to point at the string and not the referenceNil
	    bReferenceFirstElement = true;
	}

/*****
 *  Special case for the 'NA' store. It must be an undefined descriptor
 *  rather than a reference descriptor ...
 *****/
	Vdd::Store::Reference pStore (pStoreHandle->getStore ());
	if (pStore) {
	    bResultValid = true;
	    rtPTOKEN_Handle::Reference pRefPToken (pStore->getPToken ());
	    if (pStore->NamesTheNAClass ())
		rResult.constructScalar (pStore, pRefPToken);
	    else rResult.constructReferenceScalar (
		pStore, pRefPToken, bReferenceFirstElement ? 0 : pRefPToken->cardinality ()
	    );
	}
    }
    return bResultValid;
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (AccessContainer) {
    DSC_Descriptor &rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);

	DSC_Descriptor referenceDsc;
	if (ConvertPOPToDsc (pTask, &iPOP, referenceDsc))
	    pTask->loadDucWithMoved (referenceDsc);
	else
	    pTask->loadDucWithNA ();
    }
    else {
	rtVECTOR_Handle::Reference references (new rtVECTOR_Handle (V_TOTSC_PToken));

	DSC_Scalar pReference;
	pReference.constructReference (V_TOTSC_PToken, 0);

	int *sp = rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent));
	int *sl = sp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent));
	while (sp < sl) {
	    M_POP iPOP;
	    M_POP_D_AsInt (iPOP) = *sp++;

	    DSC_Descriptor referenceDsc;
	    if (ConvertPOPToDsc (pTask, &iPOP, referenceDsc)) {
		references->setElements (pReference, referenceDsc);
		referenceDsc.clear ();
	    }
	    DSC_Scalar_Int (pReference)++;
	}

/*****  Ensure that references' undefined usegment is aligned... *****/
	references->alignAll (true);

/*****  And 'return' result.  *****/
	pTask->loadDucWithVector (references);
    }
}


/********************************************
 *****  Cluster Information Primitives  *****
 ********************************************/

/**************************
 *----  Size Helpers  ----*
 **************************/

/***********************************
 *  'Context' Cluster Size Helper  *
 ***********************************/

//  Warning::  This routine doesn't account for the size of the descriptor's it really uses.
unsigned __int64 rtCONTEXT_Handle::getClusterSize () {
    static unsigned int const xElements[] = {rtCONTEXT_CPx_Current, rtCONTEXT_CPx_Self, rtCONTEXT_CPx_Origin};

    unsigned __int64 result = BaseClass::getClusterSize ();

    for (unsigned int xElement = 0; xElement < sizeof (xElements) / sizeof (xElements[0]); xElement++) {
	if (ReferenceIsntNil (xElements[xElement])) {
	    VContainerHandle::Reference pComponent (elementHandle (xElements[xElement]));
	    result += pComponent->getClusterSize ();
	}
    }

    return result;
}


/***********************************
 *  'Closure' Cluster Size Helper  *
 ***********************************/

unsigned __int64 rtCLOSURE_Handle::getClusterSize () {
    return BaseClass::getClusterSize ()
	 + m_pContext->getClusterSize ()
	 + (m_pBlock ? m_pBlock->getClusterSize () : 0);
}

/**************************************
 *  'Descriptor' Cluster Size Helper  *
 **************************************/

unsigned __int64 rtDSC_Handle::getClusterSize () {
    unsigned __int64 result = BaseClass::getClusterSize ();

    switch (pointerType ()) {
    case DSC_PointerType_Value:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference: {
	    VContainerHandle::Reference pPointerHandle (pointerHandle ());
	    result += pPointerHandle->getClusterSize ();
	}
	break;
    }

    return result;
}


/**************************************
 *  'Dictionary' Cluster Size Helper  *
 **************************************/

unsigned __int64 rtDICTIONARY_Handle::getClusterSize () {
    unsigned __int64 result = BaseClass::getClusterSize (); {
	rtSELUV_Handle::Reference pComponent (selectors ());
	result += pComponent->getClusterSize ();
    } {
	rtVECTOR_Handle::Reference pComponent (values ());
	result += pComponent->getClusterSize ();
    } {
	rtLINK_Handle::Reference pComponent (propertySubset ());
	result += pComponent->getClusterSize ();
    } {
	rtVECTOR_Handle::Reference pComponent (propertyPrototypes ());
	result += pComponent->getClusterSize ();
    }
    return result;
}


/*********************************
 *  'Index' Cluster Size Helper  *
 *********************************/

unsigned __int64 rtINDEX_Handle::getClusterSize () {
    unsigned __int64 result = BaseClass::getClusterSize (); {
	VContainerHandle::Reference pComponent (keySetHandle ());
	result += pComponent->getClusterSize ();
    } {
	VContainerHandle::Reference pComponent (keyMapHandle ());
	result += pComponent->getClusterSize ();
    } {
	rtLSTORE_Handle::Reference pComponent (listStoreHandle ());
	result += pComponent->getClusterSize ();
    }
    return result;
}

/**********************************
 *  'LStore' Cluster Size Helper  *
 **********************************/

unsigned __int64 rtLSTORE_Handle::getClusterSize () {
    unsigned __int64 result = BaseClass::getClusterSize (); {
	VContainerHandle::Reference pComponent (contentHandle ());
	result += pComponent->getClusterSize ();
    }
    return result;
}


/**********************************
 *  'Method' Cluster Size Helper  *
 **********************************/

unsigned __int64 rtMETHOD_Handle::getClusterSize () {
    unsigned __int64 result = BaseClass::getClusterSize (); {
	rtBLOCK_Handle::Reference pComponent (blockHandle ());
	result += pComponent->getClusterSize ();
    }
    return result;
}

/**********************************
 *  'Vector' Cluster Size Helper  *
 **********************************/

unsigned __int64 rtVECTOR_Handle::getClusterSize () {
    align ();

    unsigned __int64 result = BaseClass::getClusterSize ();
    unsigned int uSegmentCount = segmentArraySize ();

    for (unsigned int uSegmentIndex = 0; uSegmentIndex < uSegmentCount; uSegmentIndex++) {
	if (segmentInUse (uSegmentIndex)) {
	    VContainerHandle::Reference pComponent (segmentPointerHandle (uSegmentIndex));
	    result += pComponent->getClusterSize ();
	}
    }

    return result;
}


/**********************************
 *  'VStore' Cluster Size Helper  *
 **********************************/

unsigned __int64 rtVSTORE_Handle::getClusterSize () {
    align ();

    unsigned __int64 result = BaseClass::getClusterSize ();

    if (hasAnInheritance ()) {
	VContainerHandle::Reference pComponent (inheritancePointerHandle ());
	result += pComponent->getClusterSize ();
    }

    unsigned int n = rtVSTORE_CPx_Column + rtPTOKEN_BaseElementCount (this, element (rtVSTORE_CPx_ColumnPToken));
    for (unsigned int i = rtVSTORE_CPx_Column; i < n; i++) {
	if (ReferenceIsntNil (i)) {
	    VContainerHandle::Reference pComponent (elementHandle (i));
	    result += pComponent->getClusterSize ();
	}
    }

    return result;
}


/************************************************
 *----  Cluster Reference Map Size Helpers  ----*
 ************************************************/

/********************************************
 *  'Dictionary' Reference Map Size Helper  *
 ********************************************/

unsigned int rtDICTIONARY_Handle::getClusterReferenceMapSize () {
    return 2;
}

/***************************************
 *  'Index' Reference Map Size Helper  *
 ***************************************/

unsigned int rtINDEX_Handle::getClusterReferenceMapSize () {
    return isATimeSeries () ? 1 : 2;
}

/****************************************
 *  'LStore' Reference Map Size Helper  *
 ****************************************/

unsigned int rtLSTORE_Handle::getClusterReferenceMapSize () {
    return isAStringStore () ? 0 : 1;
}

/****************************************
 *  'Vector' Reference Map Size Helper  *
 ****************************************/

unsigned int rtVECTOR_Handle::getClusterReferenceMapSize () {
    align ();
    return segmentIndexSize ();
}

/****************************************
 *  'VStore' Reference Map Size Helper  *
 ****************************************/

unsigned int rtVSTORE_Handle::getClusterReferenceMapSize () {
    unsigned int result = 0;

    align ();

    unsigned int n = rtVSTORE_CPx_Column + rtPTOKEN_BaseElementCount (this, element (rtVSTORE_CPx_ColumnPToken));

    for (unsigned int i = rtVSTORE_CPx_Column; i < n; i++) {
	if (ReferenceIsntNil (i))
	    result++;
    }

    return result;
}


/*****************************************
 *----  Cluster V-Store Equivalents  ----*
 *****************************************/

PrivateFnDef RTYPE_Type ClusterSuperPointerType (DSC_Descriptor const &rSelf) {
    RTYPE_Type xSuperPointerType = RTYPE_C_Undefined;

    rtVSTORE_Handle::Reference pStoreEquivalent;
    rSelf.store ()->getCanonicalization (pStoreEquivalent, rSelf.Pointer ());

    if (pStoreEquivalent && pStoreEquivalent->hasAnInheritance ()) {
	VContainerHandle::Reference pSuperPointer;
	pStoreEquivalent->getInheritancePointer (pSuperPointer);

	xSuperPointerType = pSuperPointer->RType ();
    }

    return xSuperPointerType;
}

PrivateFnDef double ClusterSuperPointerSize (DSC_Descriptor const &rSelf) {
    double superPointerSize = 0.0;

    rtVSTORE_Handle::Reference pStoreEquivalent;
    rSelf.store ()->getCanonicalization (pStoreEquivalent, rSelf.Pointer ());

    if (pStoreEquivalent && pStoreEquivalent->hasAnInheritance ()) {
	VContainerHandle::Reference pSuperPointer;
	pStoreEquivalent->getInheritancePointer (pSuperPointer);

	superPointerSize = pSuperPointer->containerSize ();
    }

    return superPointerSize;
}


/************************************
 *----  Cluster Data Primitive  ----*
 ************************************/

V_DefinePrimitive (ClusterData) {
    enum {
	isLong, isDouble
    } resultType = isLong;
    union cursor_t {
	unsigned int		asLong;
	double			asDouble;
    } result;

/*****  Obtain the selector(s) ... *****/
    DSC_Descriptor const &rSelf = pTask->getSelf ();

    VContainerHandle::Reference pStoreHandle;
    rSelf.store ()->getContainerHandle (pStoreHandle);

/*****  Obtain the result ...  *****/
    switch (V_TOTSC_Primitive) {
    case XObjectSpace:
	result.asLong = pStoreHandle->spaceIndex ();
	break;
    case XContainerIndex:
	result.asLong = pStoreHandle->containerIndex ();
	break;
    case XClusterID:
	result.asLong = M_POP_AsInt (pStoreHandle->containerIdentity ());
	break;
    case XClusterType:
	result.asLong = pStoreHandle->RType ();
	break;
    case XClusterSize:
	resultType = isDouble;
	result.asDouble = static_cast<double>(pStoreHandle->getClusterSize ());
	break;
    case XClusterReferenceMapSize:
	result.asLong = pStoreHandle->getClusterReferenceMapSize ();
	break;
    case XClusterSuperPointerType:
	result.asLong = ClusterSuperPointerType (rSelf);
	break;
    case XClusterSuperPointerSize:
	resultType = isDouble;
	result.asDouble = ClusterSuperPointerSize (rSelf);
	break;
    case XClusterSchemaID:
	pTask->loadDucWithSelf (); {
	    rtDICTIONARY_Handle::Reference pDictionary;
	    pTask->getDucDictionary (pDictionary);
	    result.asLong = M_POP_AsInt (pDictionary->containerIdentity ());
	}
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ClusterData");
	break;
    }

    switch (resultType) {
    case isLong:
	pTask->loadDucWithInteger ((int)result.asLong);
	break;
    case isDouble:
	pTask->loadDucWithDouble (result.asDouble);
	break;
    default:
	pTask->loadDucWithNA ();
        break;
    }
}


V_DefinePrimitive (ClusterBoolean) {
    rtLSTORE_Handle *pLStore = dynamic_cast<rtLSTORE_Handle*> (
	(V_TOTSC_PrimitiveFlags > 100 ? pTask->getCurrent () : pTask->getSelf ()).store ()
    );

    switch (V_TOTSC_PrimitiveFlags) {
    case  50:
    case 150:	// Is an l-store string cluster
	pTask->loadDucWithBoolean (pLStore && pLStore->isAStringStore ());
	break;

    case  51:
    case 151:	// Is an associative string cluster
	pTask->loadDucWithBoolean (pLStore && pLStore->isAStringSet ());
	break;

    default:
	pTask->raiseUnimplementedAliasException ("ClusterBoolean");
	break;
    }
}


/*****************************************
 *----  Reference Map Entry Helpers  ----*
 *****************************************/

/****************************************
 *  Reference Map Data Type Definition  *
 ****************************************/

class VContainerHandle::MapEntryData {
//  Construction
public:
    MapEntryData (VTask *pTask);

//  Access
public:
    M_KOT *codKOT () const;
    M_KOT *domKOT () const;

    VTask *task () const {
	return m_pTask;
    }

//  State
protected:
    VTask*		const	m_pTask;
public:
    DSC_Descriptor		m_reference;
    RTYPE_Type			m_referenceType;
    double			m_referenceSize;
    unsigned int		m_subsetCardinality;
    unsigned int		m_internalSlot;
};

typedef VContainerHandle::MapEntryData MapEntryData;

#define ReferenceTypeDirect	RTYPE_C_MaxType

MapEntryData::MapEntryData (VTask *pTask)
: m_pTask		(pTask)
, m_referenceType	(ReferenceTypeDirect)
, m_referenceSize	(0.0)
, m_subsetCardinality	(0)
, m_internalSlot	(0)
{
    m_reference.construct ();
}

M_KOT *MapEntryData::codKOT () const {
    return m_pTask->codKOT ();
}
M_KOT *MapEntryData::domKOT () const {
    return m_pTask->ptoken ()->KOT ();
}


/***************************************
 *  'Index' Reference Map Data Helper  *
 ***************************************/

void rtDICTIONARY_Handle::getClusterReferenceMapData (MapEntryData &rMapEntryData, unsigned int xEntry) {
    rtVECTOR_Handle::Reference referenceStore;
    switch (xEntry) {
    case 1:	/* Values */
	rMapEntryData.m_internalSlot = rtDICTIONARY_CPx_Values;
	getValues (referenceStore);
	break;
    case 2:	/* Property Prototypes */
	rMapEntryData.m_internalSlot = rtDICTIONARY_CPx_PropertyPrototypes;
	getPropertyPrototypes (referenceStore);
	break;
    default:
	return;
    }

    rtPTOKEN_Handle::Reference referencePToken (referenceStore->getPToken ());

#if 0
    rMapEntryData.m_referenceType	= ReferenceTypeDirect;
    rMapEntryData.m_referenceSize	= referenceStore->getClusterSize ();
    rMapEntryData.m_subsetCardinality	= referencePToken->cardinality ();
#endif

    rMapEntryData.m_reference.constructReferenceScalar (referenceStore, referencePToken, 0);
}


/***************************************
 *  'Index' Reference Map Data Helper  *
 ***************************************/

void rtINDEX_Handle::getClusterReferenceMapData (MapEntryData &rMapEntryData, unsigned int xEntry) {
    Vdd::Store::Reference referenceStore;
    switch (xEntry) {
    case 1:	/* Values */
	rMapEntryData.m_internalSlot = rtINDEX_CPx_Lists;
	getListStore (referenceStore);
	break;
    case 2:	/* Keys */
	rMapEntryData.m_internalSlot  = rtINDEX_CPx_KeyValues;
	if (!getKeySet (referenceStore))
	    return;
	break;
    default:
	return;
    }

    rtPTOKEN_Handle::Reference referencePToken (referenceStore->getPToken ());

#if 0
    rMapEntryData.m_referenceType	= ReferenceTypeDirect;
    rMapEntryData.m_referenceSize	= referenceStore->getClusterSize ();
    rMapEntryData.m_subsetCardinality	= referencePToken->cardinality ();
#endif

    rMapEntryData.m_reference.constructReferenceScalar (referenceStore, referencePToken, 0);
}


/****************************************
 *  'LStore' Reference Map Data Helper  *
 ****************************************/

void rtLSTORE_Handle::getClusterReferenceMapData (MapEntryData &rMapEntryData, unsigned int xEntry) {
    if (isAStringStore () || xEntry != 1)
	return;

    Vdd::Store::Reference referenceStore;
    getContent (referenceStore);

    rMapEntryData.m_internalSlot = rtLSTORE_CPx_Content;

    rtPTOKEN_Handle::Reference referencePToken;
    getContentPToken (referencePToken);

#if 0
    rMapEntryData.m_referenceType	= ReferenceTypeDirect;
    rMapEntryData.m_referenceSize	= referenceStore->getClusterSize ();
    rMapEntryData.m_subsetCardinality	= referencePToken->cardinality ();
#endif

    rMapEntryData.m_reference.constructReferenceScalar (referenceStore, referencePToken, 0);
}


/****************************************
 *  'Vector' Reference Map Data Helper  *
 ****************************************/

void rtVECTOR_Handle::getClusterReferenceMapData (MapEntryData &rMapEntryData, unsigned int xEntry) {
    align ();

    if (--xEntry >= segmentIndexSize ())
	return;

    unsigned int xSegment = segmentIndexElement (xEntry);

    rtPTOKEN_Handle::Reference pSegmentPToken;
    getSegmentPToken (pSegmentPToken, xSegment);

    M_CPD *pSegmentPointer;
    getSegmentPointer (pSegmentPointer, xSegment);

    rtPTOKEN_Handle::Reference pSegmentPointerRPT (
	static_cast<rtUVECTOR_Handle*>(pSegmentPointer->containerHandle ())->rptHandle ()
    );

    Vdd::Store::Reference pSegmentStore;
    getSegmentStore (pSegmentStore, xSegment);

    rMapEntryData.m_referenceType	= pSegmentPointer->RType ();
    rMapEntryData.m_referenceSize	= static_cast<double>(pSegmentPointer->containerHandle ()->getClusterSize ());
    rMapEntryData.m_subsetCardinality	= pSegmentPToken->cardinality ();
    rMapEntryData.m_internalSlot	= xSegment;

    rMapEntryData.m_reference.constructZero (
	pSegmentStore,
	pSegmentPointer->RType (),
	rMapEntryData.domKOT ()->TheScalarPTokenHandle (),
	pSegmentPointerRPT
    );

    pSegmentPointer->release ();
}


/****************************************
 *  'VStore' Reference Map Data Helper  *
 ****************************************/

void rtVSTORE_Handle::getClusterReferenceMapData (MapEntryData &rMapEntryData, unsigned int xEntry) {
    align ();

/*****  Locate the requested column, ...  *****/
    unsigned int n = rtPTOKEN_BaseElementCount (this, element (rtVSTORE_CPx_ColumnPToken));
    if (xEntry > n)
	return;

    n += rtVSTORE_CPx_Column;
    unsigned int i = rtVSTORE_CPx_Column;
    while (i < n) {
	if (ReferenceIsntNil (i) && 0 == --xEntry)
	    break;
	i++;
    }
    if (xEntry > 0)
	return;

/*****  ... and retrieve the reference map data:  *****/
    VContainerHandle::Reference pColumn (elementHandle (i));
    Vdd::Store::Reference referenceStore (pColumn->getStore ());

    rtPTOKEN_Handle::Reference referencePToken (referenceStore->getPToken ());

#if 0
    rMapEntryData.m_referenceType	= ReferenceTypeDirect;
    rMapEntryData.m_referenceSize	= referenceStore->getClusterSize ();
    rMapEntryData.m_subsetCardinality	= referencePToken->cardinality ();
#endif
    rMapEntryData.m_internalSlot	= i - rtVSTORE_CPx_Column + 1;

    rMapEntryData.m_reference.constructReferenceScalar (referenceStore, referencePToken, 0);
}


/**********************************
 *  Reference Map Data Primitive  *
 **********************************/

V_DefinePrimitive (ClusterReferenceMapData) {
/*****  Get the store to be analyzed, ...  *****/
    VContainerHandle::Reference pStoreHandle;
    pTask->getSelf ().store ()->getContainerHandle (pStoreHandle);

/*****  Get the collector block argument, ...  *****/
    pTask->loadDucWithNextParameter ();
    VDescriptor iCollector;
    iCollector.setToMoved (pTask->duc ());

/*****  ... get the entry index argument, ...  *****/
/*****  ... require it to be integral, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype () || RTYPE_C_IntUV != ADescriptor.pointerRType ()) {
        pTask->loadDucWithNA ();
	return;
    }

/*****  ... move it someplace permanent, ...  *****/
    pTask->normalizeDuc ();
    VDescriptor iEntryIndices;
    iEntryIndices.setToMoved (pTask->duc ());
    DSC_Descriptor& rEntryIndicesMonotype = iEntryIndices.contentAsMonotype ();

/*****  ... begin the collector call-back, ...  *****/
    pTask->beginValueCall (5);
    pTask->loadDucWithMoved (iCollector);
    pTask->commitRecipient ();

    if (rEntryIndicesMonotype.isScalar ()) {
/*****  ... process the store, ...  *****/
	MapEntryData entryData (pTask);
	pStoreHandle->getClusterReferenceMapData (entryData, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype));

/*****  ... push the reference, ...  *****/
	if (rEntryIndicesMonotype.isEmpty ())
	    pTask->loadDucWithNA ();
	else
	    pTask->loadDucWithMoved (entryData.m_reference);
	pTask->commitParameter ();

/*****  ... reference type, ...  *****/
	pTask->loadDucWithInteger (entryData.m_referenceType);
	pTask->commitParameter ();

/*****  ... reference size, ...  *****/
	pTask->loadDucWithDouble (entryData.m_referenceSize);
	pTask->commitParameter ();

/*****  ... subset cardinality, ...  *****/
	pTask->loadDucWithInteger ((int)entryData.m_subsetCardinality);
	pTask->commitParameter ();

/*****  ... and internal slot onto the stack, ...  *****/
	pTask->loadDucWithInteger ((int)entryData.m_internalSlot);
	pTask->commitParameter ();
    }
    else {
	rtVECTOR_Handle::Reference references (new rtVECTOR_Handle (V_TOTSC_PToken));
	DSC_Scalar pReference;
	pReference.constructReference (V_TOTSC_PToken, 0);

	M_CPD *referenceTypes = pTask->NewIntUV ();
	int *pReferenceType = rtINTUV_CPD_Array (referenceTypes);
	
	M_CPD *referenceSizes	= pTask->NewDoubleUV ();
	double *pReferenceSize = rtDOUBLEUV_CPD_Array (referenceSizes);
	
	M_CPD *subsetCardinalities = pTask->NewIntUV ();
	int* pSubsetCardinality = rtINTUV_CPD_Array (subsetCardinalities);

	M_CPD *internalSlots = pTask->NewIntUV ();
	int* pInternalSlot = rtINTUV_CPD_Array (internalSlots);

	int *xp, *xl;

	for (
	    xp = rtINTUV_CPD_Array (DSC_Descriptor_Value (rEntryIndicesMonotype)),
	    xl = xp + UV_CPD_ElementCount (DSC_Descriptor_Value (rEntryIndicesMonotype));

	    xp < xl;

	    xp++
	) {
	    MapEntryData entryData (pTask);
	    pStoreHandle->getClusterReferenceMapData (entryData, *xp);
	    if (entryData.m_reference.isntEmpty ()) {
		references->setElements (pReference, entryData.m_reference);
		entryData.m_reference.clear ();
	    }
	    DSC_Scalar_Int (pReference)++;
	    *pReferenceType++		= entryData.m_referenceType;
	    *pReferenceSize++		= entryData.m_referenceSize;
	    *pSubsetCardinality++	= (int)entryData.m_subsetCardinality;
	    *pInternalSlot++		= (int)entryData.m_internalSlot;
	}

/*****  ... push the reference, ...  *****/
	pTask->loadDucWithVector (references);
	pTask->commitParameter ();

/*****  ... reference type, ...  *****/
	pTask->loadDucWithMonotype (referenceTypes);
	referenceTypes->release ();
	pTask->commitParameter ();

/*****  ... reference size, ...  *****/
	pTask->loadDucWithMonotype (referenceSizes);
	referenceSizes->release ();
	pTask->commitParameter ();

/*****  ... subset cardinality, ...  *****/
	pTask->loadDucWithMonotype (subsetCardinalities);
	subsetCardinalities->release ();
	pTask->commitParameter ();

/*****  ... and internal slot onto the stack, ...  *****/
	pTask->loadDucWithMonotype (internalSlots);
	internalSlots->release ();
	pTask->commitParameter ();
    }

/*****  ... and run the collector block:  *****/
    pTask->commitCall ();
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


/*********************************
 *****  Session Information  *****
 *********************************/

    PD (XTotalNetworkAllocation,
	"TotalNetworkAllocation",
	TotalNetworkAllocation)
    PD (XSpacesInNetwork,
	"SpacesInNetwork",
	SessionInformation)
    PD (XAccessedNetworkVersion,
	"AccessedNetworkVersion",
	SessionInformation)
    PD (XNetworkUpdateThread,
	"NetworkUpdateThread",
	SessionInformation)
    PD (XProcessID,
	"ProcessID",
	SessionInformation)
    PD (XSpacesInSession,
	"SpacesInSession",
	SessionInformation)

    PD (XVersionInformation,
	"VersionInformation",
	VersionInformation)

/*******************************
 *****  Space Information  *****
 *******************************/

    PD (XTotalSpaceAllocation,
	"TotalSpaceAllocation",
	SpaceAllocation)
    PD (XContainerTableSize,
	"ContainerTableSize",
	SpaceInformation)

/******************************
 *****  POP Manipulation  *****
 ******************************/

    PD (XPackPOPWithContainer,
	"PackPOPWithContainer",
	PackPOP)

    PD (XPOPExtractObjectSpace,
	"POPExtractObjectSpace",
	UnpackPOP)
    PD (XPOPExtractContainerIndex,
	"POPExtractContainerIndex",
	UnpackPOP)

/***********************************
 *****  Container Information  *****
 ***********************************/

    PD (XContainerRefCount,
	"ContainerRefCount",
	ContainerInformation)
    PD (XContainerSize,
	"ContainerSize",
	ContainerInformation)
    PD (XContainerType,
	"ContainerType",
	ContainerInformation)
    PD (XContainerSegment,
	"ContainerSegment",
	ContainerInformation)
    PD (XContainerSegmentOffset,
	"ContainerSegmentOffset",
	ContainerInformation)
    PD (XContainerAddrType,
	"ContainerAddrType",
	ContainerInformation)
    PD (XContainerPOPCnt,
	"ContainerPOPCnt",
	ContainerInformation)

    PD (XPOPGetNthPOP,
	"POPGetNthPOP",
	GetNthPOP)

/***********************************
 *****  Container Maintenance  *****
 ***********************************/

    PD (XCleanDictionary,
	"CleanDictionary",
	ContainerCleanup)
    PD (XCleanDescriptor,
	"CleanDescriptor",
	ContainerCleanup)

/******************************
 *****  Container Access  *****
 ******************************/

    PD (XAccessContainer,
	"AccessContainer",
	AccessContainer)

/********************************************
 *****  Cluster Information Primitives  *****
 ********************************************/

    PD (XObjectSpace,
	"ObjectSpace",
	ClusterData)
    PD (XContainerIndex,
	"ContainerIndex",
	ClusterData)

    PD (XClusterID,
	"ClusterID",
	ClusterData)
    PD (XClusterType,
	"ClusterType",
	ClusterData)
    PD (XClusterSize,
	"ClusterSize",
	ClusterData)
    PD (XClusterReferenceMapSize,
        "ClusterReferenceMapSize",
	ClusterData)
    PD (XClusterSuperPointerType,
        "ClusterSuperPointerType",
	ClusterData)
    PD (XClusterSuperPointerSize,
        "ClusterSuperPointerSize",
	ClusterData)
    PD (XClusterSchemaID,
	"ClusterSchemaID",
	ClusterData)
    PD (XClusterBoolean,
	"ClusterBoolean",
	ClusterBoolean)

    PD (XClusterReferenceMapData,
        "ClusterReferenceMapData",
	ClusterReferenceMapData)

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
PublicFnDef void pfSTATS_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (pfSTATS);
    FAC_FDFCase_FacilityDescription ("System Statistics Primitive Function Services");
    default:
        break;
    }
}

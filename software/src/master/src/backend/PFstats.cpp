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


/***************************
 *----  M_AND Helpers  ----*
 ***************************/

bool M_AND::ContainerExists (M_POP const *pReference) {
    unsigned int const xContainer = M_POP_ContainerIndex (pReference);
    M_ASD* pASD = 0;
    return AccessSpace (pASD, pReference)
	&& xContainer < pASD->cteCount ()
	&& pASD->cte (xContainer)->isReferenced ();
}

bool M_AND::ContainerPersists (M_POP const *pReference) {
    PS_ASD *pASD = 0;
    PS_CTE iCTE;
    return M_POP_ObjectSpace (pReference) < SpaceCount ()
        && m_pPhysicalAND->AccessSpace(pASD, M_POP_ObjectSpace (pReference))
        && pASD->GetLiveCTE (M_POP_ContainerIndex (pReference), iCTE);
}

M_CPD *M_AND::SafeGetCPD (M_POP const *pReference) {
    return ContainerExists (pReference) ? GetCPD (pReference) : 0;
}

M_CPD *M_AND::SafeGetCPDOfType (M_POP const *pReference, RTYPE_Type xType) {
    M_CPD *pContainerCPD = SafeGetCPD (pReference);
    if (pContainerCPD && pContainerCPD->RTypeIsnt (xType)) {
	pContainerCPD->release ();
	pContainerCPD = 0;
    }
    return pContainerCPD;
}


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
    char const *auditFile, *auditEntry, *compilationLogin, *compilationDate, *platform, *release, *source;
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
    case 6:
	pTask->loadDucWithListOrStringStore (
	    rtLSTORE_NewStringStore (
		pTask->codScratchPad (), ProgramDescription.source
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
    return asd ? asd->TransientAllocation () : 0;
}

PrivateFnDef unsigned int GetCTEntryCount (
    M_AND *pNetwork, unsigned int xSpace, bool *spaceValid
) {
    M_ASD* asd = 0;
    return (*spaceValid = pNetwork->AccessSpace (asd, xSpace)) ? asd->cteCount () : 0;
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
		V_TOTSC_PToken, pTask->codKOT()->TheDoublePTokenCPD (), -1,
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
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenCPD (), -1,
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
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenCPD (), -1,
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
	    V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenCPD (), -1,
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
    M_CPD *pContainerCPD = pNetwork->GetCPD (pPOP);
    unsigned int result = M_CPD_Size (pContainerCPD)
			+ M_SizeOfPreambleType
			+ M_SizeOfEndMarker;

    pContainerCPD->release ();

    return result;
}

PrivateFnDef unsigned int GetContainerType (M_AND *pNetwork, M_POP const *pPOP) {
    M_CPD *pContainerCPD = pNetwork->GetCPD (pPOP);
    unsigned int result = M_CPD_RType (pContainerCPD);

    pContainerCPD->release ();

    return result;
}

PrivateFnDef unsigned int GetContainerSegment (M_AND *pNetwork, M_POP const *pPOP) {
    M_ASD* pASD;
    PS_CTE iCTE;
    return pNetwork->AccessSpace(pASD, pPOP) && pASD->GetLiveCTE(M_POP_ContainerIndex (pPOP), iCTE)
	? PS_CTE_Segment (iCTE) : UINT_MAX;
}

PrivateFnDef unsigned int GetContainerSegmentOffset (M_AND *pNetwork, M_POP const *pPOP) {
    M_ASD* pASD;
    PS_CTE iCTE;
    return pNetwork->AccessSpace(pASD, pPOP) && pASD->GetLiveCTE(M_POP_ContainerIndex (pPOP), iCTE)
	? PS_CTE_Offset (iCTE) : UINT_MAX;
}

PrivateFnDef unsigned int GetContainerAddrType (M_AND *pNetwork, M_POP const *pPOP) {
    M_ASD *pASD = 0;
    if (!pNetwork->AccessSpace (pASD, pPOP))
        return M_CTEAddressType_ForwardingPOP;

    M_DCTE const &cte = *pASD->cte (M_POP_ContainerIndex (pPOP));
    unsigned int addrType = cte.addressType ();

    if (M_CTEAddressType_CPCC == addrType) {
	addrType = cte.addressAsContainerHandle ()->IsReadWrite ()
		   ? M_CTEAddressType_RWContainer
		   : M_CTEAddressType_ROContainer;
    }

    return addrType;
}

PrivateFnDef unsigned int GetContainerRefCount (M_AND *pNetwork, M_POP const *pPOP) {
    M_ASD *pASD;
    return pNetwork->AccessSpace (pASD, pPOP)
        ? pASD->cte (M_POP_ContainerIndex (pPOP))->referenceCount ()
        : 0;
}


PrivateFnDef unsigned int GetContainerPOPCnt (M_AND *pNetwork, M_POP const *pPOP) {
    M_CPD *pContainerCPD = pNetwork->GetCPD (pPOP);
    unsigned int result;

    switch (pContainerCPD->RType ()) {
    default:
	result = 0;
	break;

    case RTYPE_C_Descriptor:
	result = 2;
	break;

    case RTYPE_C_PArray:
	result = rtPARRAY_CPD_ElementCount (pContainerCPD);
	break;

    case RTYPE_C_PToken:
	result = rtPTOKEN_TT_Cartesian == rtPTOKEN_CPD_TokenType (pContainerCPD) ? 3 : 1;
	break;

    case RTYPE_C_Link:
	result = 2;
	break;

    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_SelUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
	result = 3;
	break;

    case RTYPE_C_Dictionary:
    case RTYPE_C_Method:
    case RTYPE_C_Context:
    case RTYPE_C_ValueStore:
    case RTYPE_C_Index:
	result = POPVECTOR_CPD_ElementCount (pContainerCPD);
	break;

    case RTYPE_C_Block:
	result = 1 +  /* LocalEnvironment */
		 rtBLOCK_PLVector_Count (
			rtBLOCK_CPD_PLiteralVector (pContainerCPD)
		 );
	break;

    case RTYPE_C_Closure:
	result = 2;
	break;

    case RTYPE_C_Vector:
	result = 1 +  /* ptoken */
		 (rtVECTOR_USD_POPsPerUSD * 
		  rtVECTOR_CPD_USegArraySize (pContainerCPD));
	break;

    case RTYPE_C_ListStore:
	result = 3;
	break;

    }
    pContainerCPD->release ();
    return result;
}


/*******************
 *----  Types  ----*
 *******************/

typedef unsigned int (*ContainerAccessor) (M_AND *pNetwork, M_POP const *pPOP);
typedef bool (*ContainerCleaner) (M_AND *pNetwork, M_POP const *pPOP);
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
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenCPD (), -1,
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

PrivateFnDef bool rtDSC_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtDSC_CPD_Store(cpd);
	break;
    case 1:
	*pResult = *rtDSC_CPD_Pointer(cpd);
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtPTOKEN_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtPTOKEN_CPD_NextGeneration (cpd);
	break;
    case 1:
	if (rtPTOKEN_TT_Cartesian == rtPTOKEN_CPD_TokenType (cpd))
	    *pResult = *rtPTOKEN_CPD_RowPToken (cpd);
	else
	    bResultValid = false;
	break;
    case 2:
	if (rtPTOKEN_TT_Cartesian == rtPTOKEN_CPD_TokenType (cpd))
	    *pResult = *rtPTOKEN_CPD_ColPToken (cpd);
	else
	    bResultValid = false;
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtLINK_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtLINK_CPD_PosPToken (cpd);
	break;
    case 1:
	*pResult = *rtLINK_CPD_RefPToken (cpd);
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool UVECTOR_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *UV_CPD_PToken (cpd);
	break;
    case 1:
	*pResult = *UV_CPD_RefPToken (cpd);
	break;
    case 2:
	*pResult = *UV_CPD_AuxillaryPOP (cpd);
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtBLOCK_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtBLOCK_CPD_LocalEnvironment (cpd);
	break;
    default:
	if (xReference <= (unsigned int)rtBLOCK_PLVector_Count (rtBLOCK_CPD_PLiteralVector (cpd)))
	    *pResult = *(rtBLOCK_CPD_PLVectorElement(cpd) + xReference - 1);
	else
	    bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtCLOSURE_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtCLOSURE_CPD_Context (cpd);
	break;
    case 1:
	*pResult = *rtCLOSURE_CPD_Block (cpd);
	break;
    default:
	bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtVECTOR_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtVECTOR_CPD_PToken (cpd);
	break;
    default:
	if (xReference <= rtVECTOR_USD_POPsPerUSD * (unsigned int)rtVECTOR_CPD_USegArraySize (cpd))
	    *pResult = *((M_POP*) rtVECTOR_CPD_USegArray (cpd) + xReference - 1);
	else
	    bResultValid = false;
	break;
    }
    return bResultValid;
}

PrivateFnDef bool rtLSTORE_GetNthPOP (M_CPD *cpd, unsigned int xReference, M_POP *pResult) {
    bool bResultValid = true;
    switch (xReference) {
    case 0:
	*pResult = *rtLSTORE_CPD_RowPToken (cpd);
	break;
    case 1:
	*pResult = *rtLSTORE_CPD_Content (cpd);
	break;
    case 2:
	*pResult = *rtLSTORE_CPD_ContentPToken (cpd);
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
    bool bResultValid = false;

    M_CPD *pContainerCPD = pNetwork->SafeGetCPD (pPOP);
    if (pContainerCPD) {
	switch (pContainerCPD->RType ()) {
	default:
	    break;

	case RTYPE_C_Descriptor:
	    bResultValid = rtDSC_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_PArray:
	    if (bResultValid = xReference < rtPARRAY_CPD_ElementCount (pContainerCPD))
		*pResult = *(rtPARRAY_CPD_ElementArray (pContainerCPD) + xReference);
	    break;

	case RTYPE_C_PToken:
	    bResultValid = rtPTOKEN_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_Link:
	    bResultValid = rtLINK_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_CharUV:
	case RTYPE_C_DoubleUV:
	case RTYPE_C_FloatUV:
	case RTYPE_C_IntUV:
	case RTYPE_C_RefUV:
	case RTYPE_C_SelUV:
	case RTYPE_C_UndefUV:
	case RTYPE_C_Unsigned64UV:
	case RTYPE_C_Unsigned96UV:
	case RTYPE_C_Unsigned128UV:
	    bResultValid = UVECTOR_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_Dictionary:
	case RTYPE_C_Method:
	case RTYPE_C_Context:
	case RTYPE_C_ValueStore:
	case RTYPE_C_Index:
	    if (bResultValid = xReference < POPVECTOR_CPD_ElementCount (pContainerCPD))
		*pResult = *(POPVECTOR_CPD_Array (pContainerCPD) + xReference);
	    break;

	case RTYPE_C_Block:
	    bResultValid = rtBLOCK_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_Closure:
	    bResultValid = rtCLOSURE_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_Vector:
	    bResultValid = rtVECTOR_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;

	case RTYPE_C_ListStore:
	    bResultValid = rtLSTORE_GetNthPOP (pContainerCPD, xReference, pResult);
	    break;
	}

	pContainerCPD->release ();
    }

    return bResultValid;
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
		V_TOTSC_PToken, pTask->codKOT()->TheIntegerPTokenCPD (), -1,
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

/*************************
 *----  Maintainers  ----*
 *************************/

PrivateFnDef bool CleanDictionary (M_AND *pNetwork, M_POP const *pPOP) {
    M_CPD *pCPD = pNetwork->SafeGetCPDOfType (pPOP, RTYPE_C_Dictionary);
    if (pCPD) {
	bool result = rtDICTIONARY_AlignAll (pCPD, true);
	pCPD->release ();
	return result;
    }
    return false;
}

PrivateFnDef bool CleanDescriptor (M_AND *pNetwork, M_POP const *pPOP) {
    M_CPD *pCPD = pNetwork->SafeGetCPDOfType (pPOP, RTYPE_C_Descriptor);
    if (pCPD) {
	bool result = rtDSC_Align (pCPD);
	pCPD->release ();
	return result;
    }
    return false;
}


/************************
 *----  Primitives  ----*
 ************************/

V_DefinePrimitive (ContainerCleanup) {
/*****  Determine which alias was invoked ... *****/
    ContainerCleaner cleaner;
    switch (V_TOTSC_Primitive) {
    case XCleanDictionary:
	cleaner = CleanDictionary;
	break;
    case XCleanDescriptor:
	cleaner = CleanDescriptor;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("ContainerCleanup");
	break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    M_AND *pNetwork = rCurrent.codDatabase ();

    if (rCurrent.isScalar ()) {
	M_POP iPOP;
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);
	pTask->loadDucWithBoolean (cleaner (pNetwork, &iPOP));
    }
    else {
	M_CPD *pSource = DSC_Descriptor_Value (rCurrent);
	rtINTUV_ElementType *sp = rtINTUV_CPD_Array (pSource),
			    *sl = sp + UV_CPD_ElementCount (pSource);

	VfGuardTool iGuardTool (V_TOTSC_PToken);
	while (sp < sl) {
	    M_POP iPOP;
	    M_POP_D_AsInt (iPOP) = *sp++;
	    if (cleaner (pNetwork, &iPOP))
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

PrivateFnDef void ConvertPOPToDsc (
    VPrimitiveTask *pTask, M_POP const *pPOP, DSC_Descriptor *pResultDsc
) {
    M_CPD *pStoreCPD = pTask->codDatabase ()->SafeGetCPD (pPOP);
    if (pStoreCPD) {
	M_CPD *pRefPTokenCPD;
	switch (pStoreCPD->RType ()) {
	case RTYPE_C_ValueStore:
	    pRefPTokenCPD = rtVSTORE_CPD_RowPTokenCPD (pStoreCPD);
	    break;
	case RTYPE_C_ListStore:
	    pRefPTokenCPD = rtLSTORE_CPD_RowPTokenCPD (pStoreCPD);
	    break;
	case RTYPE_C_Index: {
		M_CPD *pTmpCPD = rtINDEX_CPD_ListStoreCPD (pStoreCPD);
		pRefPTokenCPD = rtLSTORE_CPD_RowPTokenCPD (pTmpCPD);
		pTmpCPD->release ();
	    }
	    break;
	case RTYPE_C_Vector:
	    pRefPTokenCPD = rtVECTOR_CPD_RowPTokenCPD (pStoreCPD);
	    break;

	case RTYPE_C_Closure:
	case RTYPE_C_Method:
	    V_TOTSC_PToken->retain ();
	    pResultDsc->constructIdentity (pStoreCPD, V_TOTSC_PToken);
	    return;

	case RTYPE_C_Block: {
		VString iSource;
		RSLANG_Decompile (iSource, pStoreCPD);
		pStoreCPD->release ();
		pStoreCPD = rtLSTORE_NewStringStoreWithDelm (
		    pTask->codScratchPad (), "[", "]", iSource
		);
		pRefPTokenCPD = rtLSTORE_CPD_RowPTokenCPD (pStoreCPD);
	    }
	    pResultDsc->constructReferenceScalar (pStoreCPD, pRefPTokenCPD, 0);
	    return;
	default:
	    pStoreCPD->release ();
	    pStoreCPD = NilOf (M_CPD*);
	    break;
	}

/*****
 *  Special case for the 'NA' store. It must be an undefined descriptor
 *  rather than a reference descriptor ...
 *****/
	if (pStoreCPD && pStoreCPD->NamesTheNAClass ())
	    pResultDsc->constructScalar (pStoreCPD, pRefPTokenCPD);
	else if (pStoreCPD) {
	    pResultDsc->constructReferenceScalar (
		pStoreCPD, pRefPTokenCPD, rtPTOKEN_CPD_BaseElementCount(pRefPTokenCPD)
	    );
	}
    }
}


/***********************
 *----  Primitive  ----*
 ***********************/

V_DefinePrimitive (AccessContainer) {
    DSC_Descriptor referenceDsc;
    referenceDsc.construct ();

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    M_POP iPOP;
    if (rCurrent.isScalar ()) {
	M_POP_D_AsInt (iPOP) = DSC_Descriptor_Scalar_Int (rCurrent);

	ConvertPOPToDsc (pTask, &iPOP, &referenceDsc);

	if (referenceDsc.isEmpty ())
	    pTask->loadDucWithNA ();
	else
	    pTask->loadDucWithMoved (referenceDsc);
    }
    else {
	M_CPD *references = rtVECTOR_New (V_TOTSC_PToken);

	DSC_Scalar pReference;
	DSC_InitReferenceScalar (pReference, V_TOTSC_PToken, 0);

	int *sp = rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent));
	int *sl = sp + UV_CPD_ElementCount (DSC_Descriptor_Value (rCurrent));
	while (sp < sl) {
	    M_POP_D_AsInt (iPOP) = *sp++;

	    ConvertPOPToDsc (pTask, &iPOP, &referenceDsc);

	    if (referenceDsc.isntEmpty ()) {
		rtVECTOR_Assign (references, &pReference, &referenceDsc);
		referenceDsc.clear ();
	    }
	    DSC_Scalar_Int (pReference)++;
	}

/*****  Ensure that references' undefined usegment is aligned... *****/
	rtVECTOR_AlignAll (references, true);

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

PrivateFnDef double ClusterSizeHelper (M_CPD *self);

/***********************************
 *  'Context' Cluster Size Helper  *
 ***********************************/

PrivateFnDef double rtCONTEXT_ClusterSize (M_CPD *self) {
    double result = M_CPD_Size (self);

    M_CPD *component = rtCONTEXT_CPD_CurrentCPD (self);
    result += ClusterSizeHelper (component);
    component->release ();

    component = rtCONTEXT_CPD_SelfCPD (self);
    result += ClusterSizeHelper (component);
    component->release ();

    component = rtCONTEXT_CPD_OriginCPD	(self);
    result += ClusterSizeHelper (component);
    component->release ();

    return result;
}


/***********************************
 *  'Closure' Cluster Size Helper  *
 ***********************************/

PrivateFnDef double rtCLOSURE_ClusterSize (M_CPD *self) {
    double result = M_CPD_Size (self);

    M_CPD *component = rtCLOSURE_CPD_ContextCPD (self);
    result += ClusterSizeHelper (component);
    component->release ();

    if (rtCLOSURE_CPD_IsABlockClosure (self)) {
	component = rtCLOSURE_CPD_BlockCPD (self);
	result += ClusterSizeHelper (component);
	component->release ();
    }

    return result;
}

/**************************************
 *  'Descriptor' Cluster Size Helper  *
 **************************************/

PrivateFnDef double rtDSC_ClusterSize (M_CPD *self) {
    double result = M_CPD_Size (self);
    switch (rtDSC_CPD_PointerType (self)) {
    case DSC_PointerType_Value:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference: {
	    M_CPD *component = self->GetCPD (rtDSC_CPx_Pointer);
	    result += M_CPD_Size (component);
	    component->release ();
	}
	break;
    default:
	break;
    }

    return result;
}


/**************************************
 *  'Dictionary' Cluster Size Helper  *
 **************************************/

PrivateFnDef double rtDICTIONARY_ClusterSize (M_CPD *self) {
    double result = M_CPD_Size (self);

    M_CPD *pComponentCPD = rtDICTIONARY_CPD_SelectorsCPD (self);
    result += ClusterSizeHelper (pComponentCPD);
    pComponentCPD->release ();

    pComponentCPD = rtDICTIONARY_CPD_ValuesCPD (self);
    result += ClusterSizeHelper (pComponentCPD);
    pComponentCPD->release ();

    pComponentCPD = rtDICTIONARY_CPD_PropertySubsetCPD (self);
    result += ClusterSizeHelper (pComponentCPD);
    pComponentCPD->release ();

    pComponentCPD = rtDICTIONARY_CPD_PropertyPrototypesCPD (self);
    result += ClusterSizeHelper (pComponentCPD);
    pComponentCPD->release ();

    return result;
}


/*********************************
 *  'Index' Cluster Size Helper  *
 *********************************/

PrivateFnDef double rtINDEX_ClusterSize (M_CPD *self) {
    M_CPD *keys		= self->GetCPD (rtINDEX_CPx_KeyValues);
    M_CPD *map		= self->GetCPD (rtINDEX_CPx_Map);
    M_CPD *values	= self->GetCPD (rtINDEX_CPx_Lists);

    double result	= M_CPD_Size (self)
			+ ClusterSizeHelper (keys)
			+ ClusterSizeHelper (map)
			+ ClusterSizeHelper (values);

    keys->release ();
    map->release ();
    values->release ();

    return result;
}

/**********************************
 *  'LStore' Cluster Size Helper  *
 **********************************/

PrivateFnDef double rtLSTORE_ClusterSize (M_CPD *self) {
    M_CPD *values = rtLSTORE_CPD_ContentCPD (self);
    double result = M_CPD_Size (self) + ClusterSizeHelper (values);
    values->release ();

    return result;
}


/**********************************
 *  'Method' Cluster Size Helper  *
 **********************************/

PrivateFnDef double rtMETHOD_ClusterSize (M_CPD *self) {
    M_CPD *block = rtMETHOD_CPD_BlockCPD (self);
    double result = M_CPD_Size (self) + ClusterSizeHelper (block);
    block->release ();

    return result;
}

/**********************************
 *  'Vector' Cluster Size Helper  *
 **********************************/

PrivateFnDef double rtVECTOR_ClusterSize (M_CPD *self) {
    rtVECTOR_Align (self);

    double result = M_CPD_Size (self);

    unsigned int uSegmentCount = rtVECTOR_CPD_USegArraySize (self);

    for (unsigned int uSegmentIndex = 0; uSegmentIndex < uSegmentCount; uSegmentIndex++) {
	rtVECTOR_CPD_USD (self) = rtVECTOR_CPD_USegArray (self) + uSegmentIndex;

	rtVECTOR_CPD_SetUSDCursor (self, rtVECTOR_USD_PToken);

//	if (M_POPIsNil (&rtVECTOR_CPD_USD_PToken (self)))
	if (self->ReferenceIsNil (&rtVECTOR_CPD_USD_PToken (self)))
	    continue;

	rtVECTOR_CPD_SetUSDCursor (self, rtVECTOR_USD_Values);
	M_CPD *uvector = self->GetCPD (rtVECTOR_CPx_USDCursor);
	result += ClusterSizeHelper (uvector);
	uvector->release ();
    }

    return result;
}


/**********************************
 *  'VStore' Cluster Size Helper  *
 **********************************/

PrivateFnDef double rtVSTORE_ClusterSize (M_CPD *self) {
    rtVSTORE_Align (self);

    double result = M_CPD_Size (self);
    if (self->ReferenceIsntNil (rtVSTORE_CPx_InheritancePtr)) {
	M_CPD *column = rtVSTORE_CPD_InheritancePointerCPD (self);
	result += M_CPD_Size (column);
	column->release ();
    }

    unsigned int n = rtPTOKEN_BaseElementCount (self, rtVSTORE_CPx_ColumnPToken);

    VContainerHandle *pStoreHandle = self->containerHandle ();
    M_POP const* pColumnArray = rtVSTORE_CPD_ColumnArray (self);
    for (unsigned int i = 0; i < n; i++) {
	M_POP const *pColumnPOP = &pColumnArray[i];
	if (pStoreHandle->ReferenceIsntNil (pColumnPOP)) {
	    M_CPD *column = pStoreHandle->GetCPD (pColumnPOP);
	    result += ClusterSizeHelper (column);
	    column->release ();
	}
    }

    return result;
}


/*********************************
 *  Generic Cluster Size Helper  *
 *********************************/

PrivateFnDef double ClusterSizeHelper (M_CPD *self) {
    switch ((RTYPE_Type)M_CPD_RType (self)) {
    case RTYPE_C_Context:
	return rtCONTEXT_ClusterSize	(self);
    case RTYPE_C_Closure:
	return rtCLOSURE_ClusterSize	(self);
    case RTYPE_C_Descriptor:
	return rtDSC_ClusterSize	(self);
    case RTYPE_C_Dictionary:
	return rtDICTIONARY_ClusterSize	(self);
    case RTYPE_C_Index:
	return rtINDEX_ClusterSize	(self);
    case RTYPE_C_ListStore:
	return rtLSTORE_ClusterSize	(self);
    case RTYPE_C_Method:
	return rtMETHOD_ClusterSize	(self);
    case RTYPE_C_Vector:
	return rtVECTOR_ClusterSize	(self);
    case RTYPE_C_ValueStore:
	return rtVSTORE_ClusterSize	(self);
    default:
	return M_CPD_Size (self);
    }
}


/************************************************
 *----  Cluster Reference Map Size Helpers  ----*
 ************************************************/

/*****************************************
 *  'Closure' Reference Map Size Helper  *
 *****************************************/

PrivateFnDef unsigned int rtCLOSURE_ClusterReferenceMapSize (M_CPD *Unused(self)) {
    return 0;
}

/********************************************
 *  'Dictionary' Reference Map Size Helper  *
 ********************************************/

PrivateFnDef unsigned int rtDICTIONARY_ClusterReferenceMapSize (M_CPD *Unused(self)) {
    return 2;
}

/***************************************
 *  'Index' Reference Map Size Helper  *
 ***************************************/

PrivateFnDef unsigned int rtINDEX_ClusterReferenceMapSize (M_CPD *self) {
    return rtINDEX_CPD_IsATimeSeries (self) ? 1 : 2;
}

/****************************************
 *  'LStore' Reference Map Size Helper  *
 ****************************************/

PrivateFnDef unsigned int rtLSTORE_ClusterReferenceMapSize (M_CPD *self) {
    return rtLSTORE_CPD_StringStore (self) ? 0 : 1;
}

/****************************************
 *  'Method' Reference Map Size Helper  *
 ****************************************/

PrivateFnDef unsigned int rtMETHOD_ClusterReferenceMapSize (M_CPD *Unused(self)) {
    return 0;
}


/****************************************
 *  'Vector' Reference Map Size Helper  *
 ****************************************/

PrivateFnDef unsigned int rtVECTOR_ClusterReferenceMapSize (M_CPD *self) {
    rtVECTOR_Align (self);
    return rtVECTOR_CPD_USegIndexSize (self);
}

/****************************************
 *  'VStore' Reference Map Size Helper  *
 ****************************************/

PrivateFnDef unsigned int rtVSTORE_ClusterReferenceMapSize (M_CPD *self) {
    unsigned int result = 0;

    rtVSTORE_Align (self);

    unsigned int n = rtPTOKEN_BaseElementCount (self, rtVSTORE_CPx_ColumnPToken);

    VContainerHandle *pStoreHandle = self->containerHandle ();
    M_POP const *pColumnArray = rtVSTORE_CPD_ColumnArray (self);
    for (unsigned int i = 0; i < n; i++) {
	if (pStoreHandle->ReferenceIsntNil (pColumnArray + i))
	    result++;
    }

    return result;
}


/***************************************
 *  Generic Reference Map Size Helper  *
 ***************************************/

PrivateFnDef unsigned int ClusterReferenceMapSizeHelper (M_CPD *self) {
    switch ((RTYPE_Type)M_CPD_RType (self)) {
    case RTYPE_C_Closure:
	return rtCLOSURE_ClusterReferenceMapSize    (self);
    case RTYPE_C_Dictionary:
	return rtDICTIONARY_ClusterReferenceMapSize (self);
    case RTYPE_C_Index:
	return rtINDEX_ClusterReferenceMapSize	    (self);
    case RTYPE_C_ListStore:
	return rtLSTORE_ClusterReferenceMapSize	    (self);
    case RTYPE_C_Method:
	return rtMETHOD_ClusterReferenceMapSize	    (self);
    case RTYPE_C_Vector:
	return rtVECTOR_ClusterReferenceMapSize	    (self);
    case RTYPE_C_ValueStore:
	return rtVSTORE_ClusterReferenceMapSize	    (self);
    default:
	return 0;
    }
}


/****************************************
 *----  Cluster V-Store Equivalent  ----*
 ****************************************/

PrivateFnDef M_CPD *ClusterVStoreEquivalent (VPrimitiveTask* pTask, M_CPD *self) {
    M_CPD *vstoreEquivalent;
    
    switch ((RTYPE_Type)M_CPD_RType (self)) {
    case RTYPE_C_Block:
	/*****  Use the following approximation...  *****/
	vstoreEquivalent = self->TheStringClass ();
	break;
    case RTYPE_C_Closure:
	vstoreEquivalent = self->TheClosureClass ();
	break;
    case RTYPE_C_Dictionary:
	vstoreEquivalent = self->TheFixedPropertyClass ();
	break;
    case RTYPE_C_Index:
	vstoreEquivalent = rtINDEX_CPD_IsATimeSeries (self)
	    ? self->TheTimeSeriesClass () : self->TheIndexedListClass ();
	break;
    case RTYPE_C_ListStore:
	vstoreEquivalent = rtLSTORE_CPD_StringStore (self)
	    ? self->TheStringClass () : self->TheListClass ();
	break;
    case RTYPE_C_Method:
	vstoreEquivalent = self->TheMethodClass ();
	break;
    case RTYPE_C_Vector:
	vstoreEquivalent = self->TheFixedPropertyClass ();
	break;
    case RTYPE_C_ValueStore:
	vstoreEquivalent = self;
	break;
    default:
	pTask->raiseException (
	    EC__InternalInconsistency,
	    "ClusterData: %s Not Recognized As A Valid Store Type",
	    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (self))
	);
	break;
    }
    return vstoreEquivalent;
}


PrivateFnDef RTYPE_Type ClusterSuperPointerType (VPrimitiveTask* pTask, M_CPD *self) {
    M_CPD *storeEquivalent = ClusterVStoreEquivalent (pTask, self);
    M_CPD *superPointer = rtVSTORE_CPD_InheritancePointerCPD (storeEquivalent);
    RTYPE_Type superPointerType = (RTYPE_Type)M_CPD_RType (superPointer);
    superPointer->release ();

    return superPointerType;
}

PrivateFnDef double ClusterSuperPointerSize (VPrimitiveTask* pTask, M_CPD *self) {
    M_CPD *storeEquivalent = ClusterVStoreEquivalent (pTask, self);
    double superPointerSize = 0;

    if (storeEquivalent->ReferenceIsntNil (rtVSTORE_CPx_InheritancePtr)) {
	M_CPD *superPointer = rtVSTORE_CPD_InheritancePointerCPD (storeEquivalent);

	superPointerSize = M_CPD_Size (superPointer);
	
	superPointer->release ();
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
    M_CPD *store = pTask->getSelf ().storeCPD ();

/*****  Obtain the result ...  *****/
    switch (V_TOTSC_Primitive) {
    case XObjectSpace:
	result.asLong = store->SpaceIndex ();
	break;
    case XContainerIndex:
	result.asLong = store->ContainerIndex ();
	break;
    case XClusterID:
	result.asLong = M_POP_D_AsInt (M_CPD_POP (store));
	break;
    case XClusterType:
	result.asLong = M_CPD_RType (store);
	break;
    case XClusterSize:
	resultType = isDouble;
	result.asDouble = ClusterSizeHelper (store);
	break;
    case XClusterReferenceMapSize:
	result.asLong = ClusterReferenceMapSizeHelper (store);
	break;
    case XClusterSuperPointerType:
	result.asLong = ClusterSuperPointerType (pTask, store);
	break;
    case XClusterSuperPointerSize:
	resultType = isDouble;
	result.asDouble = ClusterSuperPointerSize (pTask, store);
	break;
    case XClusterSchemaID:
	pTask->loadDucWithSelf (); {
	    M_CPD *dictionary = pTask->ducDictionary ();
	    result.asLong = M_POP_D_AsInt (M_CPD_POP (dictionary));
	    dictionary->release ();
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
    M_CPD *pStore = (
	V_TOTSC_PrimitiveFlags > 100 ? pTask->getCurrent () : pTask->getSelf ()
    ).storeCPD ();

    switch (V_TOTSC_PrimitiveFlags) {
    case  50:
    case 150:	// Is an l-store string cluster
	pTask->loadDucWithBoolean (
	    RTYPE_C_ListStore == (RTYPE_Type)M_CPD_RType (pStore)
	    && rtLSTORE_CPD_StringStore (pStore)
	);
	break;

    case  51:
    case 151:	// Is an associative string cluster
	pTask->loadDucWithBoolean (
	    RTYPE_C_ListStore == (RTYPE_Type)M_CPD_RType (pStore)
	    && rtLSTORE_StringSet::IsAStringSet (pStore)
	);
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

class MapEntryData {
//  Construction
public:
    MapEntryData (VTask *pTask);

//  Access
public:
    M_KOT *codKOT () const {
	return m_pTask->codKOT ();
    }
    M_KOT *domKOT () const {
	return m_pTask->ptoken ()->KOT ();
    }

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


/*****************************************
 *  'Closure' Reference Map Data Helper  *
 *****************************************/

PrivateFnDef void rtCLOSURE_ClusterReferenceMapData (
    M_CPD*			Unused(self),
    unsigned int		Unused(entryIndex),
    MapEntryData		*Unused(pMapEntryData)
)
{
}


/***************************************
 *  'Index' Reference Map Data Helper  *
 ***************************************/

PrivateFnDef void rtDICTIONARY_ClusterReferenceMapData (
    M_CPD*			self,
    unsigned int		entryIndex,
    MapEntryData		*pMapEntryData
)
{
    M_CPD *referenceStore;
    M_CPD *referencePToken;

    switch (entryIndex) {
    case 1:	/* Values */
	pMapEntryData->m_internalSlot  = rtDICTIONARY_CPx_Values;
	referenceStore  = rtDICTIONARY_CPD_ValuesCPD (self);
	referencePToken	= rtVECTOR_CPD_RowPTokenCPD (referenceStore);
	break;
    case 2:	/* Property Prototypes */
	pMapEntryData->m_internalSlot  = rtDICTIONARY_CPx_PropertyPrototypes;
	referenceStore  = rtDICTIONARY_CPD_PropertyPrototypesCPD (self);
	referencePToken = rtVECTOR_CPD_RowPTokenCPD (referenceStore);
	break;
    default:
	return;
    }

#if 0
    pMapEntryData->m_referenceType	= ReferenceTypeDirect;
    pMapEntryData->m_referenceSize	= ClusterSizeHelper (referenceStore);
    pMapEntryData->m_subsetCardinality	= rtPTOKEN_CPD_BaseElementCount (referencePToken);
#endif

    pMapEntryData->m_reference.constructReferenceScalar (
	referenceStore, referencePToken, 0
    );
}


/***************************************
 *  'Index' Reference Map Data Helper  *
 ***************************************/

PrivateFnDef void rtINDEX_ClusterReferenceMapData (
    M_CPD*			self,
    unsigned int		entryIndex,
    MapEntryData		*pMapEntryData
)
{
    M_CPD *referenceStore;
    M_CPD *referencePToken;

    switch (entryIndex)
    {
    case 1:	/* Values */
	pMapEntryData->m_internalSlot  = rtINDEX_CPx_Lists;
	referenceStore  = rtINDEX_CPD_ListStoreCPD (self);
	referencePToken	= rtLSTORE_CPD_RowPTokenCPD (referenceStore);
	break;
    case 2:	/* Keys */
	pMapEntryData->m_internalSlot  = rtINDEX_CPx_KeyValues;
	referenceStore  = self->GetCPD (rtINDEX_CPx_KeyValues, RTYPE_C_Vector);
	referencePToken = rtVECTOR_CPD_RowPTokenCPD (referenceStore);
	break;
    default:
	return;
    }

#if 0
    pMapEntryData->m_referenceType	= ReferenceTypeDirect;
    pMapEntryData->m_referenceSize	= ClusterSizeHelper (referenceStore);
    pMapEntryData->m_subsetCardinality	= rtPTOKEN_CPD_BaseElementCount (referencePToken);
#endif

    pMapEntryData->m_reference.constructReferenceScalar (
	referenceStore, referencePToken, 0
    );
}


/****************************************
 *  'LStore' Reference Map Data Helper  *
 ****************************************/

PrivateFnDef void rtLSTORE_ClusterReferenceMapData (
    M_CPD*			self,
    unsigned int		entryIndex,
    MapEntryData		*pMapEntryData
)
{
    if (rtLSTORE_CPD_StringStore (self) || entryIndex != 1)
	return;

    pMapEntryData->m_internalSlot  = rtLSTORE_CPx_Content;
    M_CPD *referenceStore	= rtLSTORE_CPD_ContentCPD	(self);
    M_CPD *referencePToken	= rtLSTORE_CPD_ContentPTokenCPD	(self);

#if 0
    pMapEntryData->m_referenceType	= ReferenceTypeDirect;
    pMapEntryData->m_referenceSize	= ClusterSizeHelper (referenceStore);
    pMapEntryData->m_subsetCardinality	= rtPTOKEN_CPD_BaseElementCount (
	referencePToken
    );
#endif

    pMapEntryData->m_reference.constructReferenceScalar (
	referenceStore, referencePToken, 0
    );
}


/****************************************
 *  'Method' Reference Map Data Helper  *
 ****************************************/

PrivateFnDef void rtMETHOD_ClusterReferenceMapData (
    M_CPD*			Unused(self),
    unsigned int		Unused(entryIndex),
    MapEntryData		*Unused(pMapEntryData)
)
{
}


/****************************************
 *  'Vector' Reference Map Data Helper  *
 ****************************************/

PrivateFnDef void rtVECTOR_ClusterReferenceMapData (
    M_CPD*			self,
    unsigned int		entryIndex,
    MapEntryData		*pMapEntryData
)
{
    rtVECTOR_Align (self);
    if (--entryIndex >= (unsigned int)rtVECTOR_CPD_USegIndexSize (self))
	return;

    rtVECTOR_CPD_USD (self) = &rtVECTOR_CPD_USegArray (self)[
	rtVECTOR_CPD_USegIndex (self)[entryIndex]
    ];

    rtVECTOR_CPD_SetUSDCursor (self, rtVECTOR_USD_PToken);
    M_CPD *subsetPToken = self->GetCPD (rtVECTOR_CPx_USDCursor, RTYPE_C_PToken);

    rtVECTOR_CPD_SetUSDCursor (self, rtVECTOR_USD_Values);
    M_CPD *referencePointer	= self->GetCPD (rtVECTOR_CPx_USDCursor);
    M_CPD *referencePToken	= UV_CPD_RefPTokenCPD (referencePointer);

    rtVECTOR_CPD_SetUSDCursor (self, rtVECTOR_USD_VStore);
    M_CPD *referenceStore	= self->GetCPD (rtVECTOR_CPx_USDCursor);

    pMapEntryData->m_referenceType	= (RTYPE_Type)M_CPD_RType (referencePointer);
    pMapEntryData->m_referenceSize	= ClusterSizeHelper (referencePointer);
    pMapEntryData->m_subsetCardinality	= rtPTOKEN_CPD_BaseElementCount (subsetPToken);
    pMapEntryData->m_internalSlot	= rtVECTOR_CPD_USegIndex (self)[entryIndex];

    pMapEntryData->m_reference.constructZero (
	referenceStore,
	(RTYPE_Type)M_CPD_RType (referencePointer),
	pMapEntryData->domKOT ()->TheScalarPToken,
	referencePToken
    );

    subsetPToken->release ();
    referencePointer->release ();
}


/****************************************
 *  'VStore' Reference Map Data Helper  *
 ****************************************/

PrivateFnDef void rtVSTORE_ClusterReferenceMapData (
    M_CPD*			self,
    unsigned int		entryIndex,
    MapEntryData		*pMapEntryData
)
{
    rtVSTORE_Align (self);

/*****  Locate the requested column, ...  *****/
    unsigned int n = rtPTOKEN_BaseElementCount (self, rtVSTORE_CPx_ColumnPToken);
    if (entryIndex > n)
	return;

    VContainerHandle *pStoreHandle = self->containerHandle ();
    M_POP const *pColumn = rtVSTORE_CPD_ColumnArray (self);
    unsigned int i = 0;
    while (i < n && entryIndex > 0) {
	if (pStoreHandle->ReferenceIsNil (pColumn) || --entryIndex > 0)
	    pColumn++;
	i++;
    }
    if (entryIndex > 0)
	return;

/*****  ... and retrieve the reference map data:  *****/
    M_CPD *referenceStore  = pStoreHandle->GetCPD (pColumn);
    M_CPD *referencePToken = rtVSTORE_CPD_RowPTokenCPD (self);

#if 0
    pMapEntryData->m_referenceType	= ReferenceTypeDirect;
    pMapEntryData->m_referenceSize	= ClusterSizeHelper (referenceStore);
    pMapEntryData->m_subsetCardinality	= rtPTOKEN_CPD_BaseElementCount (
	referencePToken
    );
#endif
    pMapEntryData->m_internalSlot	= i;

    pMapEntryData->m_reference.constructReferenceScalar (
	referenceStore, referencePToken, 0
    );
}


/**********************************
 *  Reference Map Data Primitive  *
 **********************************/

V_DefinePrimitive (ClusterReferenceMapData) {
/*****  Get the store to be analyzed, ...  *****/
    M_CPD *store = pTask->getSelf ().storeCPD ();

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
	switch ((RTYPE_Type)M_CPD_RType (store)) {
	case RTYPE_C_Closure:
	    rtCLOSURE_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_Dictionary:
	    rtDICTIONARY_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_Index:
	    rtINDEX_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_ListStore:
	    rtLSTORE_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_Method:
	    rtMETHOD_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_Vector:
	    rtVECTOR_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	case RTYPE_C_ValueStore:
	    rtVSTORE_ClusterReferenceMapData (
		store, DSC_Descriptor_Scalar_Int (rEntryIndicesMonotype), &entryData
	    );
	    break;
	default:
	    break;
	}

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
	M_CPD *references = rtVECTOR_New (V_TOTSC_PToken);
	DSC_Scalar pReference;
	DSC_InitReferenceScalar (pReference, V_TOTSC_PToken, 0);

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
	)
	{
	    MapEntryData entryData (pTask);
	    switch ((RTYPE_Type)M_CPD_RType (store)) {
	    case RTYPE_C_Closure:
		rtCLOSURE_ClusterReferenceMapData	(store, *xp, &entryData);
		break;
	    case RTYPE_C_Index:
		rtINDEX_ClusterReferenceMapData		(store, *xp, &entryData);
		break;
	    case RTYPE_C_ListStore:
		rtLSTORE_ClusterReferenceMapData	(store, *xp, &entryData);
		break;
	    case RTYPE_C_Method:
		rtMETHOD_ClusterReferenceMapData	(store, *xp, &entryData);
		break;
	    case RTYPE_C_Vector:
		rtVECTOR_ClusterReferenceMapData	(store, *xp, &entryData);
		break;
	    case RTYPE_C_ValueStore:
		rtVSTORE_ClusterReferenceMapData	(store, *xp, &entryData);
		break;
	    default:
		break;
	    }
	    if (entryData.m_reference.isntEmpty ()) {
		rtVECTOR_Assign (
		    references, &pReference, &entryData.m_reference
		);
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

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (pfSTATS);
    FAC_FDFCase_FacilityDescription ("System Statistics Primitive Function Services");
    default:
        break;
    }
}

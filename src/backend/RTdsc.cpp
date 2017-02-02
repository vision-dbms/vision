/*****  Descriptor Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtDSC

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "ts.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTlink.h"
#include "RTptoken.h"
#include "RTrefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"
#include "RTundefuv.h"
#include "RTvstore.h"

/*****  Self  *****/
#include "RTdsc.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtDSC_Handle);


/*************************************************
 *****  Standard CPD Initialization Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize a standard 'Descriptor' CPD.
 *
 *  Argument:
 *	cpd			- the address of an uninitialized standard CPD.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PrivateFnDef void InitStdCPD (M_CPD* cpd) {
    rtDSC_Descriptor* base = rtDSC_CPD_Base (cpd);

    rtDSC_CPD_Store	(cpd) = &rtDSC_Descriptor_Store		(base);
    rtDSC_CPD_Pointer	(cpd) = &rtDSC_Descriptor_Pointer	(base);
}


/****************************************
 *****  Descriptor Packing Routine  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to pack a descriptor into a container.
 *
 *  Argument:
 *	dscPtr			- the address of the descriptor to pack.
 *
 *  Returns:
 *	The address of a standard CPD for the packed descriptor container.
 *
 *****/
PublicFnDef M_CPD *rtDSC_Pack (M_ASD *pContainerSpace, DSC_Descriptor *dscPtr) {
    bool hasAStore = true;

/*****
 *  Create and basically initialize a new instance of R-Type 'descriptor', ...
 *****/
    M_CPD *dscCPD = pContainerSpace->CreateContainer (
	RTYPE_C_Descriptor, sizeof (rtDSC_Descriptor)
    );

    InitStdCPD (dscCPD);

    dscCPD->constructReference (rtDSC_CPx_Store);
    dscCPD->constructReference (rtDSC_CPx_Pointer);

/*****  ...decode the descriptor's pointer, ...  *****/
    switch (dscPtr->pointerType ()) {
    case DSC_PointerType_Empty:
        hasAStore = false;
        break;
    case DSC_PointerType_Scalar:
	rtDSC_CPD_RType(dscCPD) = DSC_Scalar_RType (dscPtr->m_iPointer.m_iContent.as_iScalar);
//	rtDSC_CPD_ScalarValue (dscCPD) = DSC_Scalar_Value (dscPtr->m_iPointer.m_iContent.as_iScalar);
	memcpy (
	    &rtDSC_CPD_ScalarValue (dscCPD),
	    &DSC_Scalar_Value (dscPtr->m_iPointer.m_iContent.as_iScalar),
	    sizeof (rtDSC_CPD_ScalarValue(dscCPD))
	);
        dscCPD->StoreReference (
	    rtDSC_CPx_Pointer, DSC_Scalar_RefPToken (dscPtr->m_iPointer.m_iContent.as_iScalar)
	);
        break;
    case DSC_PointerType_Value:
        dscCPD->StoreReference (rtDSC_CPx_Pointer, dscPtr->m_iPointer.m_iContent.as_iValue);
        break;
    case DSC_PointerType_Identity:
        dscCPD->StoreReference (
	    rtDSC_CPx_Pointer, dscPtr->m_iPointer.m_iContent.as_iIdentity.PToken ()
	);
        break;
    case DSC_PointerType_Link:
    //  Changed to share rather than create a new link container (mjc, 3/2/01):
	dscCPD->StoreReference (
	    rtDSC_CPx_Pointer, dscPtr->m_iPointer.m_iContent.as_iLink.realizedContainer ()
	);
        break;
    case DSC_PointerType_Reference:
        dscCPD->StoreReference (
	    rtDSC_CPx_Pointer, dscPtr->m_iPointer.m_iContent.as_iReference
	);
        break;
    default:
	dscCPD->release ();
        dscPtr->complainAboutBadPointerType ("rtDSC_Pack");
        break;
    }

/*****  ... install a POP for the store, ...  *****/
    if (hasAStore)
	dscCPD->StoreReference (rtDSC_CPx_Store, dscPtr->storeCPD ());

/*****  ... set the descriptor type, ...  *****/
    rtDSC_CPD_PointerType (dscCPD) = dscPtr->m_iPointer.m_xType;

/*****  ... and return.  *****/
    return dscCPD;
}


/******************************************
 *****  Descriptor Unpacking Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to unpack a descriptor container into a 'Descriptor'
 *
 *  Arguments:
 *	dscCPD			- a standard CPD for the descriptor to be
 *				  unpacked.
 *	dscPtr			- the address to be initialized as a
 *				  'Descriptor'.  This location is assumed to
 *				  NOT contain a currently valid descriptor.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PublicFnDef void rtDSC_Unpack (M_CPD *dscCPD, DSC_Descriptor *dscPtr) {
    bool hasAStore = true;

/*****  Decode the descriptor's type, ...  *****/
    switch (dscPtr->m_iPointer.m_xType = rtDSC_CPD_PointerType (dscCPD)) {
    case DSC_PointerType_Empty:
        hasAStore = false;
        break;
    case DSC_PointerType_Scalar: {
	    DSC_ScalarValue iValueFromContainer;
	    memcpy (&iValueFromContainer, &rtDSC_CPD_ScalarValue (dscCPD), sizeof (iValueFromContainer));
	    DSC_InitScalar (
		dscPtr->m_iPointer.m_iContent.as_iScalar,
		dscCPD->GetCPD (rtDSC_CPx_Pointer, RTYPE_C_PToken),
		rtDSC_CPD_RType (dscCPD), DSC_Scalar_Value, iValueFromContainer
	    );
	} break;
    case DSC_PointerType_Value:
	dscPtr->m_iPointer.m_iContent.as_iValue.construct (dscCPD->GetCPD (rtDSC_CPx_Pointer));
        break;
    case DSC_PointerType_Identity:
	dscPtr->m_iPointer.m_iContent.as_iIdentity.construct (
	    dscCPD->GetCPD (rtDSC_CPx_Pointer, RTYPE_C_PToken)
	);
        break;
    case DSC_PointerType_Link:
	dscPtr->m_iPointer.m_iContent.as_iLink.construct (
	    dscCPD->GetCPD (rtDSC_CPx_Pointer, RTYPE_C_Link)
	);
        break;
    case DSC_PointerType_Reference:
	dscPtr->m_iPointer.m_iContent.as_iReference.construct (
	    dscCPD->GetCPD (rtDSC_CPx_Pointer, RTYPE_C_RefUV)
	);
        break;
    default:
        dscPtr->complainAboutBadPointerType ("rtDSC_Unpack");
        break;
    }

/*****  ... and unpack the store and return.  *****/
    if (hasAStore)
        dscPtr->store ().construct (dscCPD->GetCPD (rtDSC_CPx_Store));
    else
        dscPtr->store ().construct ();
}


/******************************************
 *****  Descriptor Alignment Routine  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a 'Descriptor'
 *
 *  Arguments:
 *	dscCPD			- a standard CPD for the descriptor to be
 *				  aligned.
 *
 *  Returns:
 *	true if the descriptor required alignment. false if not.
 *
 *****/
PublicFnDef bool rtDSC_Align (M_CPD* dscCPD) {
    M_CPD* tmpCPD;
    bool result = false;

/*****  Validate Argument R-Type  *****/
    RTYPE_MustBeA ("rtDsc_Align", M_CPD_RType (dscCPD), RTYPE_C_Descriptor);

    M_CPD* pPointerCPD = dscCPD->GetCPD (rtDSC_CPx_Pointer);

    switch (M_CPD_RType (pPointerCPD)) {
    case RTYPE_C_PToken:
	if (rtPTOKEN_IsCurrent (pPointerCPD, -1))
	    break;
	result = true;
	if (RTYPE_C_RefUV != rtDSC_CPD_RType(dscCPD))
	    tmpCPD = rtPTOKEN_BasePToken (pPointerCPD, -1);
	else {
	    rtREFUV_Type_Reference ref;
	    pPointerCPD->retain ();
	    rtREFUV_Ref_D_RefPTokenCPD (ref) = pPointerCPD;
	    rtREFUV_Ref_D_Element (ref) =
		DSC_ScalarValue_Int (rtDSC_CPD_ScalarValue (dscCPD));
	    rtREFUV_AlignReference (&ref);
	    tmpCPD = rtREFUV_Ref_D_RefPTokenCPD (ref);
	    dscCPD->EnableModifications ();
	    DSC_ScalarValue_Int (rtDSC_CPD_ScalarValue (dscCPD)) =
		rtREFUV_Ref_D_Element (ref);
	}
	dscCPD->StoreReference (rtDSC_CPx_Pointer, tmpCPD, -1);
	tmpCPD->release ();
	break;
    case RTYPE_C_Link:
	rtLINK_AlignLink (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_RefUV:
	rtREFUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_IntUV:
	rtINTUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_CharUV:
	rtCHARUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_UndefUV:
	rtUNDEFUV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Align (pPointerCPD);
	result = pPointerCPD->IsReadWrite ();
	break;
    default:
	break;
    }

    pPointerCPD->release ();
    return result;
}


/****************************************
 *****  I-Object Unpacking Routine  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to attempt to unpack an I-Object into a descriptor.
 *
 *  Arguments:
 *	iObject			- the I-Object to be unpacked.
 *	dscPtr			- the address of an uninitialized or empty
 *				  descriptor to be unpacked.
 *
 *  Returns:
 *	true if the I-Object was successfully unpacked, false otherwise.
 *
 *****/
PublicFnDef bool rtDSC_UnpackIObject (
    M_ASD *pContainerSpace, IOBJ_IObject iObject, DSC_Descriptor *dscPtr
) {
    M_CPD *sourceCPD;
    bool result = true;

    if (IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Int)) dscPtr->constructScalar (
	pContainerSpace->KOT (), IOBJ_IObjectValueAsInt (iObject)
    );
    else if (IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Double)) dscPtr->constructScalar (
	pContainerSpace->KOT (), IOBJ_IObjectValueAsDouble (iObject)
    );
    else if (IOBJ_IsAnInstanceOf (iObject, PRIMFNS_PrimitiveIType)) dscPtr->constructScalar (
	pContainerSpace->ThePrimitiveClass (),
	(int)PRIMFNS_UnpackPrimitiveIObject (iObject)
    );
    else switch (M_CPD_RType (sourceCPD = RTYPE_QRegisterCPD (iObject))) {
    case RTYPE_C_Descriptor:
        rtDSC_Unpack (sourceCPD, dscPtr);
        break;
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
	dscPtr->constructMonotype (sourceCPD);
        break;
    default:
	result = false;
        break;
    }

    return result;
}


/********************************************************************
 *****  Standard Representation Type Services Handler Routines  *****
 ********************************************************************/

/***************************
 *  Container Reclamation  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	preambleAddress		- the address of the container to be reclaimed.
 *	ownerASD		- the ASD of the space which owns the container
 *
 *****/
PrivateFnDef void ReclaimContainer (
    M_ASD *ownerASD, M_CPreamble const *preambleAddress
) {
    rtDSC_Descriptor const *dsc = (rtDSC_Descriptor const*)M_CPreamble_ContainerBase (
	preambleAddress
    );

    ownerASD->Release (&rtDSC_Descriptor_Store	(dsc));
    ownerASD->Release (&rtDSC_Descriptor_Pointer(dsc));
}


/***********
 *  Saver  *
 ***********/

/*---------------------------------------------------------------------------
 *****  Routine to 'save' a Descriptor.
 *****/
bool rtDSC_Handle::PersistReferences () {
    rtDSC_Descriptor *dsc = (rtDSC_Descriptor*)ContainerContent ();

    return Persist (&rtDSC_Descriptor_Store   (dsc))
	&& Persist (&rtDSC_Descriptor_Pointer (dsc));
}


/*************************************************
 *****  Standard Descriptor Marking Routine  *****
 *************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to 'mark' containers referenced by a Descriptor.
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void MarkContainers (M_ASD* pSpace, M_CPreamble const *pContainer) {
    rtDSC_Descriptor const *dsc = (rtDSC_Descriptor const *)M_CPreamble_ContainerBase (
	pContainer
    );

    pSpace->Mark (&rtDSC_Descriptor_Store	(dsc));
    pSpace->Mark (&rtDSC_Descriptor_Pointer	(dsc));
}


/*************
 *  Printer  *
 *************/

/*---------------------------------------------------------------------------
 *****  Internal routine to print a descriptor.
 *
 *  Arguments:
 *	cpd			- a standard CPD for the descriptor to be
 *				  printed.
 *	full			- a boolean which, when true, requests
 *				  recursive display of the descriptor.
 *
 *  Returns:
 *	NOTHING - Executed for side effect on the user only.
 *
 *****/
PrivateFnDef void PrintDescriptor (M_CPD* cpd, int full) {
    IO_printf ("#%s{\nStore:   ", RTYPE_TypeIdAsString (RTYPE_C_Descriptor));
    if (full)
        RTYPE_Print (cpd, rtDSC_CPx_Store);
    else
	RTYPE_QPrint (cpd, rtDSC_CPx_Store);

    IO_printf ("\nPointer: ");
    switch (rtDSC_CPD_PointerType (cpd)) {
    case DSC_PointerType_Empty:
        IO_printf ("-");
        break;
    case DSC_PointerType_Scalar:
        IO_printf (
	    "#%s(SCALAR):", RTYPE_TypeIdAsString (rtDSC_CPD_RType (cpd))
	);
	switch (rtDSC_CPD_RType (cpd))
	{
	case RTYPE_C_CharUV:
	    IO_printf ("%c", rtDSC_CPD_ScalarValue (cpd).asChar);
	    break;
	case RTYPE_C_DoubleUV:
	    IO_printf ("%g", rtDSC_CPD_ScalarValue (cpd).asDouble);
	    break;
	case RTYPE_C_FloatUV:
	    IO_printf ("%g", rtDSC_CPD_ScalarValue (cpd).asFloat);
	    break;
	case RTYPE_C_IntUV:
	case RTYPE_C_Link:
	case RTYPE_C_RefUV:
	    IO_printf ("%d", rtDSC_CPD_ScalarValue (cpd).asInt);
	    break;
	case RTYPE_C_UndefUV:
	    IO_printf ("-");
	    break;
	default:
	    IO_printf ("???");
	    break;
	}
        break;
    case DSC_PointerType_Value:
    case DSC_PointerType_Identity:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference:
        RTYPE_Print (cpd, rtDSC_CPx_Pointer);
        break;
    default:
        IO_printf ("???");
        break;
    }
    IO_printf ("}");
}


/***************
 *  Converter  *
 ***************/

PrivateFnDef M_CPreamble* ConvertOldDescriptor (
    M_ASD *Unused(pASD), M_CPreamble const *oldDescriptorPreamble
) {
    M_CPreamble* newDescriptorPreamble = TS_AllocateContainer (
	RTYPE_C_Descriptor,
	sizeof (rtDSC_Descriptor),
	M_CPreamble_POP (oldDescriptorPreamble)
    );
    rtDSC_Descriptor* newDescriptor = M_CPreamble_ContainerBaseAsType (
	newDescriptorPreamble, rtDSC_Descriptor
    );
    int const *oldDescriptor = M_CPreamble_ContainerBaseAsType (
	oldDescriptorPreamble, int const
    );

    switch (M_CPreamble_RType (oldDescriptorPreamble)) {
    case RTYPE_C_Descriptor0:	/* Pre-5.7 Format */
	memcpy (
	    &rtDSC_Descriptor_PointerType (newDescriptor),
	    &oldDescriptor[0],
	    sizeof (int)
	);
	memcpy (
	    &rtDSC_Descriptor_Store (newDescriptor),
	    &oldDescriptor[1],
	    sizeof (int)
	);
	memcpy (
	    &rtDSC_Descriptor_Pointer (newDescriptor),
	    &oldDescriptor[2],
	    sizeof (int)
	);
	switch (M_CPreamble_Size (oldDescriptorPreamble))
	{
	case 28:
	    memcpy (
		&rtDSC_Descriptor_RType (newDescriptor),
		&oldDescriptor[3],
		sizeof (int)
	    );
	    memcpy (
		&rtDSC_Descriptor_ScalarValue (newDescriptor),
		&oldDescriptor[5],
		sizeof (DSC_ScalarValue)
	    );
	    break;
	case 32:
	    memcpy (
		&rtDSC_Descriptor_RType (newDescriptor),
		&oldDescriptor[4],
		sizeof (int)
	    );
	    memcpy (
		&rtDSC_Descriptor_ScalarValue (newDescriptor),
		&oldDescriptor[6],
		sizeof (DSC_ScalarValue)
	    );
	    break;
	default:
	    ERR_SignalFault (
		EC__InternalInconsistency,
		UTIL_FormatMessage (
		"ConvertOldDescriptor: Descriptor [%u:%u] Size Invalid: %u",
		    M_CPreamble_POPObjectSpace (oldDescriptorPreamble),
		    M_CPreamble_POPContainerIndex (oldDescriptorPreamble),
		    M_CPreamble_Size (oldDescriptorPreamble)
		)
	    );
	    break;
	}
	break;

    case RTYPE_C_Descriptor1:	/* Pre-6 Format */
	memcpy (newDescriptor, oldDescriptor, M_CPreamble_Size (oldDescriptorPreamble));
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"ConvertOldDescriptor: Descriptor [%u:%u] Type Invalid: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPreamble_RType (oldDescriptorPreamble))
	    )
	);
	break;
    }

    return newDescriptorPreamble;
}


/***************************************
 *****  Internal Debugger Methods  *****
 ***************************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM) {
    DSC_Descriptor descriptor;
    if (rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[0], &descriptor)) {
        M_CPD *result = rtDSC_Pack (IOBJ_ScratchPad, &descriptor);
	descriptor.clear ();
	return RTYPE_QRegister (result);
    }

    return IOBJ_TheNilIObject;
}


IOBJ_DefineNewaryMethod (NewInDM) {
/*****  Validate that the 'in:' parameter is a value store...  *****/
/*---------------------------------------------------------------------------
 * N.B.:
 *	If this test is removed, the scalar decoding cases below may fail.
 *---------------------------------------------------------------------------
 */
    M_CPD *store = RTYPE_QRegisterCPD (parameterArray[1]);
    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Char)   ||
	IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Double) ||
	IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)    ||
	IOBJ_IsAnInstanceOf (parameterArray[0], PRIMFNS_PrimitiveIType)
    )
    {
	RTYPE_MustBeA (
	    "rtDSC[new:in:]", M_CPD_RType (store), RTYPE_C_ValueStore
	);
    }

/*****  Decode the I-Type of the 'new:' parameter...  *****/
    DSC_Descriptor descriptor;
    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Char)) {
	descriptor.constructScalar (
	    store, rtVSTORE_CPD_RowPTokenCPD (store),
	    (char)IOBJ_IObjectValueAsInt (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Double)) {
	store->retain ();
	descriptor.constructScalar (
	    store, rtVSTORE_CPD_RowPTokenCPD (store),
	    IOBJ_IObjectValueAsDouble (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
	store->retain ();
	descriptor.constructScalar (
	    store, rtVSTORE_CPD_RowPTokenCPD (store),
	    IOBJ_IObjectValueAsInt (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], PRIMFNS_PrimitiveIType)) {
	store->retain ();
	descriptor.constructScalar (
	    store, rtVSTORE_CPD_RowPTokenCPD (store),
	    (int)PRIMFNS_UnpackPrimitiveIObject (parameterArray[0])
	);
    }
    else {
	store->retain ();
	descriptor.constructMonotype (store, RTYPE_QRegisterCPD (parameterArray[0]));
    }

/*****  ...pack the resulting value descriptor, ...  *****/
    M_CPD *result = rtDSC_Pack (IOBJ_ScratchPad, &descriptor);

/*****  ...cleanup and return.  *****/
    descriptor.clear ();

    return RTYPE_QRegister (result);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (StoreDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtDSC_CPx_Store);
}

IOBJ_DefineUnaryMethod (PointerDM) {
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtDSC_CPx_Pointer);
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
    DSC_DisplayCounts ();
    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtDSC_ConversionHandler) {
    M_RTD* rtd;
    switch (handlerOperation) {
    case RTYPE_InitializeMData:
        rtd = iArgList.arg<M_RTD*>();
	M_RTD_ConvertFn	(rtd) = ConvertOldDescriptor;
        break;
    default:
        return -1;
    }
    return 0;
}


RTYPE_DefineHandler (rtDSC_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"		, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"		, NewDM)
	IOBJ_MDE ("new:in:"		, NewInDM)
	IOBJ_MDE ("displayCounts"	, DisplayCountsDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	RTYPE_StandardDMDEPackage
	IOBJ_MDE ("store"		, StoreDM)
	IOBJ_MDE ("pointer"		, PointerDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtDSC_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtDSC_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD*>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD*>() = instanceMD;
        break;
    case RTYPE_PrintObject:
        PrintDescriptor (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
        PrintDescriptor (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtDSC);
    FAC_FDFCase_FacilityDescription
        ("Descriptor Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTdsc.c 1 replace /users/mjc/system
  861002 17:59:34 mjc New descriptor modules

 ************************************************************************/
/************************************************************************
  RTdsc.c 2 replace /users/jck/system
  861017 21:17:34 jck Brought error check in NewInDebugMethod more into line with what is allowed in descriptors

 ************************************************************************/
/************************************************************************
  RTdsc.c 3 replace /users/jad/system
  861024 15:35:03 jad made link descriptors
contain a linkc and/or a linkcpd to eliminate some conversions

 ************************************************************************/
/************************************************************************
  RTdsc.c 4 replace /users/mjc/system
  870204 17:16:24 mjc Add '#include saver.h' statement

 ************************************************************************/
/************************************************************************
  RTdsc.c 5 replace /users/jck/system
  870415 09:53:12 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTdsc.c 6 replace /users/mjc/translation
  870524 09:32:33 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTdsc.c 7 replace /users/jck/system
  871007 13:34:56 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTdsc.c 8 replace /users/jck/system
  880612 21:50:29 jck Apollo version pointed out that rtDSC_Unpack did not initalize the descriptor in all cases

 ************************************************************************/

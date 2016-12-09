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


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to pack a descriptor into a container.
 *
 *  Argument:
 *	rDsc			- the address of the descriptor to pack.
 *
 *  Returns:
 *	The address of a standard CPD for the packed descriptor container.
 *
 *****/
rtDSC_Handle::rtDSC_Handle (
    M_ASD *pSpace, DSC_Descriptor &rDsc
) : BaseClass (pSpace, RTYPE_C_Descriptor, sizeof (rtDSC_Descriptor)) {
    bool hasAStore = true;

/*****
 *  Create and basically initialize a new instance of R-Type 'descriptor', ...
 *****/
    constructReference (storePOP ());
    constructReference (pointerPOP ());

/*****  ...decode the descriptor's pointer, ...  *****/
    switch (rDsc.pointerType ()) {
    case DSC_PointerType_Empty:
        hasAStore = false;
        break;
    case DSC_PointerType_Scalar:
	setScalarTo (rDsc.m_iPointer.m_iContent.as_iScalar);
        break;
    case DSC_PointerType_Value:
        setPointerTo (rDsc.m_iPointer.m_iContent.as_iValue);
        break;
    case DSC_PointerType_Identity:
	setPointerTo (rDsc.m_iPointer.m_iContent.as_iIdentity.PToken ());
        break;
    case DSC_PointerType_Link:
    //  Changed to share rather than create a new link container (mjc, 3/2/01):
	setPointerTo (rDsc.m_iPointer.m_iContent.as_iLink.realizedContainer ());
        break;
    case DSC_PointerType_Reference:
        setPointerTo (rDsc.m_iPointer.m_iContent.as_iReference);
        break;
    default:
        rDsc.complainAboutBadPointerType ("rtDSC_Pack");
        break;
    }

/*****  ... install a POP for the store, ...  *****/
    if (hasAStore)
	setStoreTo (rDsc.store ());

/*****  ... set the descriptor type, ...  *****/
    setPointerTypeTo (rDsc.m_iPointer.m_xType);
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

rtDSC_Handle::~rtDSC_Handle () {
}


/***********************
 ***********************
 *****  Alignment  *****
 ***********************
 ***********************/

bool rtDSC_Handle::align () {
    bool bAlignmentsDone = false;

    rtPTOKEN_Handle::Reference pPToken;

    switch (pointerType ()) {
    case DSC_PointerType_Scalar: {
	    DSC_Scalar iScalar;
	    getScalar (iScalar);
	    if (iScalar.RType () != RTYPE_C_RefUV)
		pPToken.setTo (iScalar.RPT ());
	    else {
		rtPTOKEN_Handle::Reference pOldRPT (iScalar.RPT ());
		rtREFUV_AlignReference (&iScalar);
		if (iScalar.RPT () != pOldRPT) {
		    EnableModifications ();
		    setScalarTo (iScalar);
		    bAlignmentsDone = true;
		}
	    }
	    iScalar.destroy ();
	}
	break;
    case DSC_PointerType_Identity:
	pPToken.setTo (static_cast<rtPTOKEN_Handle*>(pointerHandle ()));
	break;
    case DSC_PointerType_Value:
    case DSC_PointerType_Link:
    case DSC_PointerType_Reference: {
	    VContainerHandle::Reference pPointer;
	    getPointer (pPointer);
	    bAlignmentsDone = pPointer->align ();
	}
	break;
    default:
	break;
    }

    if (pPToken) {
	rtPTOKEN_Handle::Reference pBasePToken (pPToken->basePToken ());
	if (pPToken->DoesntName (pBasePToken)) {
	    EnableModifications ();
	    setPointerTo (pBasePToken);
	    bAlignmentsDone = true;
	}
    }

    return bAlignmentsDone;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*---------------------------------------------------------------------------
 *****  Routine to unpack a descriptor container into a 'Descriptor'
 *
 *  Arguments:
 *	dscCPD			- a standard CPD for the descriptor to be
 *				  unpacked.
 *	rDsc			- the address to be initialized as a
 *				  'Descriptor'.  This location is assumed to
 *				  NOT contain a currently valid descriptor.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
void rtDSC_Handle::getValue (DSC_Descriptor &rDsc) {
    rDsc.construct ();

/*****  Decode the descriptor's type, ...  *****/
    bool hasAStore = true;

    switch (rDsc.m_iPointer.m_xType = pointerType ()) {
    case DSC_PointerType_Empty:
        hasAStore = false;
        break;
    case DSC_PointerType_Scalar:
	getScalar (rDsc.m_iPointer.m_iContent.as_iScalar);
        break;
    case DSC_PointerType_Value:
	rDsc.m_iPointer.m_iContent.as_iValue.construct (pointerCPD ());
        break;
    case DSC_PointerType_Identity:
	rDsc.m_iPointer.m_iContent.as_iIdentity.construct (
	    static_cast<rtPTOKEN_Handle*>(pointerHandle (RTYPE_C_PToken))
	);
        break;
    case DSC_PointerType_Link:
	rDsc.m_iPointer.m_iContent.as_iLink.construct (pointerCPD (RTYPE_C_Link));
        break;
    case DSC_PointerType_Reference:
	rDsc.m_iPointer.m_iContent.as_iReference.construct (pointerCPD (RTYPE_C_RefUV));
        break;
    default:
        rDsc.complainAboutBadPointerType ("rtDSC_Handle::getValue");
        break;
    }

/*****  ... and unpack the store and return.  *****/
    if (hasAStore) {
        rDsc.setStoreTo (storeHandle ()->getStore ());
    }
}


/****************************************
 *****  I-Object Unpacking Routine  *****
 ****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to attempt to unpack an I-Object into a descriptor.
 *
 *  Arguments:
 *	iObject			- the I-Object to be unpacked.
 *	rDsc			- the address of an uninitialized or empty
 *				  descriptor to be unpacked.
 *
 *  Returns:
 *	true if the I-Object was successfully unpacked, false otherwise.
 *
 *****/
PublicFnDef bool rtDSC_UnpackIObject (
    M_ASD *pContainerSpace, IOBJ_IObject iObject, DSC_Descriptor &rDsc
) {
    M_CPD *sourceCPD;
    bool result = true;

    if (IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Int)) rDsc.constructScalar (
	pContainerSpace->KOT (), IOBJ_IObjectValueAsInt (iObject)
    );
    else if (IOBJ_IsAnInstanceOf (iObject, IOBJ_IType_Double)) rDsc.constructScalar (
	pContainerSpace->KOT (), IOBJ_IObjectValueAsDouble (iObject)
    );
    else if (IOBJ_IsAnInstanceOf (iObject, PRIMFNS_PrimitiveIType)) rDsc.constructScalar (
	pContainerSpace->ThePrimitiveClass (),
	(int)PRIMFNS_UnpackPrimitiveIObject (iObject)
    );
    else switch ((sourceCPD = RTYPE_QRegisterCPD (iObject))->RType ()) {
    case RTYPE_C_Descriptor:
        static_cast<rtDSC_Handle*>(sourceCPD->containerHandle ())->getValue (rDsc);
        break;
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
	rDsc.constructMonotype (sourceCPD);
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

/********************************
 *----  CPD Initialization  ----*
 ********************************/

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

/***********************************
 *----  Container Reclamation  ----*
 ***********************************/

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


/*******************
 *----  Saver  ----*
 *******************/

/*---------------------------------------------------------------------------
 *****  Routine to 'save' a Descriptor.
 *****/
bool rtDSC_Handle::PersistReferences () {
    rtDSC_Descriptor *dsc = typecastContent ();

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
PrivateFnDef void MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD* pSpace, M_CPreamble const *pContainer) {
    rtDSC_Descriptor const *dsc = (rtDSC_Descriptor const *)M_CPreamble_ContainerBase (
	pContainer
    );

    pGCV->Mark (pSpace, &rtDSC_Descriptor_Store	(dsc));
    pGCV->Mark (pSpace, &rtDSC_Descriptor_Pointer	(dsc));
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
    M_ASD *pASD, M_CPreamble const *oldDescriptorPreamble
) {
    M_CPreamble *newDescriptorPreamble = pASD->AllocateContainer (
	RTYPE_C_Descriptor,
	sizeof (rtDSC_Descriptor),
	M_CPreamble_POPContainerIndex (oldDescriptorPreamble)
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
    if (rtDSC_UnpackIObject (IOBJ_ScratchPad, parameterArray[0], descriptor)) {
	rtDSC_Handle::Reference result (new rtDSC_Handle (IOBJ_ScratchPad, descriptor));
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
    VContainerHandle *pStoreHandle = RTYPE_QRegisterHandle (parameterArray[1]);
    Vdd::Store::Reference pStore;
    if (!pStoreHandle->getStore (pStore)				||
	IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Char)	||
	IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Double)	||
	IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)		||
	IOBJ_IsAnInstanceOf (parameterArray[0], PRIMFNS_PrimitiveIType)
    ) {
	RTYPE_MustBeA (
	    "rtDSC[new:in:]", pStoreHandle->RType (), RTYPE_C_ValueStore
	);
    }

    rtPTOKEN_Handle::Reference pStorePToken (pStore->getPToken ());

/*****  Decode the I-Type of the 'new:' parameter...  *****/
    DSC_Descriptor descriptor;
    if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Char)) {
	descriptor.constructScalar (
	    pStore, pStorePToken, (char)IOBJ_IObjectValueAsInt (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Double)) {
	descriptor.constructScalar (
	    pStore, pStorePToken, IOBJ_IObjectValueAsDouble (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], IOBJ_IType_Int)) {
	descriptor.constructScalar (
	    pStore, pStorePToken, IOBJ_IObjectValueAsInt (parameterArray[0])
	);
    }
    else if (IOBJ_IsAnInstanceOf (parameterArray[0], PRIMFNS_PrimitiveIType)) {
	descriptor.constructScalar (
	    pStore, pStorePToken, (int)PRIMFNS_UnpackPrimitiveIObject (parameterArray[0])
	);
    }
    else {
	descriptor.constructMonotype (pStore, RTYPE_QRegisterCPD (parameterArray[0]));
    }

/*****  ...pack the resulting value descriptor, ...  *****/
    rtDSC_Handle::Reference result (new rtDSC_Handle (IOBJ_ScratchPad, descriptor));

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

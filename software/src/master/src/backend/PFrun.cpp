/*****  Run Time Support Primitives Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName pfRUN

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"
#include "VkSocketAddress.h"

/*****  Facility  *****/
#include "m.h"
#include "selector.h"
#include "uvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vprimfns.h"
#include "vutil.h"

#include "RTblock.h"
#include "RTlink.h"
#include "RTptoken.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"

#include "RTlstore.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "IOMHandle.h"

#include "IOMFile.h"
#include "IOMService.h"
#include "IOMStream.h"

#include "VOutputGenerator.h"
#include "VSuspension.h"
#include "VTopTask.h"

#include "Vca_ITrigger.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VDeviceFactory.h"
#include "Vca_VListener.h"
#include "Vca_VProcess.h"
#include "Vca_VTimer.h"

/*****  Self  *****/
#include "PFrun.h"


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
 *	500 - 599                       *
 *======================================*/


/******************************
 *****  Input Processing  *****
 ******************************/

// #define XReadEvalPrintController	IOMDriver::XReadEvalPrintController	// == 500

/*******************************
 *****  Execution Control  *****
 *******************************/

#define XMutex				501
#define XRelinquishTimeSlice		502

/*******************
 *****  Magic  *****
 *******************/

#define XBlockMagic			505
#define XGroundMagic			506

/********************************************
 *****  Process Information Primitives  *****
 ********************************************/

#define XGetProcessString		510

#define XActiveDriver			512
#define XGetProcessInteger		513


/***********************************************
 *****  I/O Driver Description Primitives  *****
 ***********************************************/

#define XIOMStateDescription		520
#define XVkStatusTypeDescription	521
#define XVkStatusCodeDescription	522
#define XIOMChannelTypeDescription	523
#define XIOMOptionDescription		524
#define XIOMPromptFormatDescription	525
#define XIOMBinaryFormatDescription	526
#define XIOMTrimFormatDescription	527


/********************************************
 *****  I/O Driver Lifetime Primitives  *****
 ********************************************/

#define XCreate				530
#define XAccept				531

#define XWait				535

#define XEndTransmission		536
#define XEndReception			537

#define XClose				538


/******************************************
 *****  I/O Driver Access Primitives  *****
 ******************************************/

#define XAccess				539

/*****************************************
 *****  I/O Driver Query Primitives  *****
 *****************************************/

#define XGetIndex			540
#define XGetState			541
#define XGetStatus			542
#define XGetError			543
#define XGetHandler			544

#define XGetErrorOutput			545

#define XGetOptionIndex			546
#define XGetOptionValue			547
#define XGetSeekOffset			548

#define XGetErrorInput			549

#define XGetChannelType			550
#define XGetSocketName			551
#define XGetPeerName			552

/*******************************************
 *****  I/O Driver Control Primitives  *****
 *******************************************/

#define XRetain				560
#define XRelease			561

#define XEnableHandler			562
#define XDisableHandler			563
#define XSetHandler			564

#define XSetErrorOutput			565

#define XSetOptionIndex			566
#define XSetOptionValue			567
#define XSetSeekOffset			568

#define XSetErrorInput			569


/*****************************************
 *****  I/O Driver Input Primitives  *****
 *****************************************/

#define XGetByteCount			570

#define XGetSignedByte			571
#define XGetUnsignedByte		572
#define XGetSignedShort			573
#define XGetUnsignedShort		574
#define XGetLong			575
#define XGetFloat			576
#define XGetDouble			577

#define XGetString			580
#define XGetFixedString			581
#define XGetLine			582


/******************************************
 *****  I/O Driver Output Primitives  *****
 ******************************************/

#define XFlushOutput			590

#define XPutByte			591

#define XPutShort			593

#define XPutLong			595
#define XPutFloat			596
#define XPutDouble			597
#define XPutString			598

/******************************************
 *****  Output Management Primitives  *****
 ******************************************/

#define XFilterOutput			81
#define XWriteOutput			82
#define XAppendOutput			83

#define XDivertDelimitedOutput		84
#define XDivertOutput			87

#define XEchoToFile			88
#define XStopEchoToFile	    		89

#define XFilterOutput2			99


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

/**********************************
 *****  Argument Acquisition  *****
 **********************************/

PrivateFnDef bool GetArguments (VPrimitiveTask* pTask, RTYPE_Type xExpectedRType) {
/*****  Get the handle argument parameter, which must be a monotype, ...  *****/
    if (!pTask->loadDucWithNextParameterAsMonotype () || ADescriptor.pointerRType() != xExpectedRType)
	return false;

/*****  ... and normalized upon return: *****/
    pTask->normalizeDuc ();
    return true;
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

/*******************************
 *****  Execution Control  *****
 *******************************/

V_DefinePrimitive (RelinquishTimeSlice) {
    pTask->yield ();
    pTask->loadDucWithSelf ();
}


/*************************
 *****  Block Magic  *****
 *************************/

V_DefinePrimitive (BlockMagic) {
//  Do the magic, ...
    VTopTask* pDetachedTask = 0;
    switch (V_TOTSC_PrimitiveFlags) {
    case 0:	// Top
    case 10:	// Top		(suspended)
	pDetachedTask = new VTopTask (0, pTask->channel ()); {
	    DSC_Descriptor iDelegateMonotype;
	    iDelegateMonotype.construct (pTask->getSelf ());

	    DSC_Pointer& rDelegatePointer = DSC_Descriptor_Pointer (iDelegateMonotype);
	    rtPTOKEN_Handle::Reference pDelegateRPT (rDelegatePointer.RPT ());

	    rDelegatePointer.clear ();
	    rDelegatePointer.constructReferenceScalar (pDelegateRPT, 0);

	    pDetachedTask->loadDucWithMoved (iDelegateMonotype);
	}
	break;

    case 1:	// Background
    case 11:	// Background	(suspended)
	pDetachedTask = new VTopTask (pTask, pTask->channel ());
	pDetachedTask->loadDucWithCopied (pTask->getSelf ());
	break;

    default:
	pTask->raiseUnimplementedAliasException ("BlockMagic");
	break;
    }

//  ... determine the appropriate ground object to return, ...
    VInternalGroundStore* pReturnedGround = 0;
    switch (pTask->flags ()) {
    case 0:	// Top
    case 1:	// Background
	pReturnedGround = pDetachedTask;
	break;

    case 10:	// Top		(suspended)
    case 11:	// Background	(suspended)
	pReturnedGround = new VSuspension (pDetachedTask);
	break;

    default:
	break;
    }

//  ... return the ground object appropriately, ...
    switch (pTask->flags ()) {
    case 0:	// Top
    case 10:	// Top		(suspended)
	pTask->loadDucWithRepresentative (pReturnedGround);
	break;

    case 1:	// Background
    case 11:	// Background	(suspended)
	pTask->loadDucWith (pReturnedGround);
	break;

    default:
	break;
    }

//  ... and start any task created by this primitive.
    if (pDetachedTask)
	pDetachedTask->start ();
}


/**************************
 *****  Ground Magic  *****
 **************************/

V_DefinePrimitive (GroundMagic) {
    DSC_Descriptor const *pRecipient;

    switch (V_TOTSC_PrimitiveFlags) {
    case 0:	// doesGroundMagicAtCurrent
	pRecipient = &pTask->getCurrent();
	break;

    case 1:	// doesGroundMagicAtSelf
	pRecipient = &pTask->getSelf ();
	break;

    default:
	pRecipient = NilOf (DSC_Descriptor*);

	pTask->raiseUnimplementedAliasException ("BlockMagic");
	break;
    }
    pTask->loadDucWithBoolean (pRecipient->storeTransientExtensionIsA (VGroundStore::RTT));
}


/*******************************************
 *****  Process Information Primitive  *****
 *******************************************/

V_DefinePrimitive (ProcessInformation) {
    char iBuffer[257];

    switch (V_TOTSC_Primitive) {
    case XGetProcessString:
	switch (V_TOTSC_PrimitiveFlags) {
	case 0:		//  Host Name
	    pTask->loadDucWith (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));
	    break;
	case 1:
	    pTask->loadDucWith (Vk_username (iBuffer, sizeof (iBuffer)));
	    break;
	case 2:
	    pTask->loadDucWith (pTask->codDatabase()->NDFPathName ());
	    break;
	case 3:
	    pTask->loadDucWith (pTask->codDatabase()->OSDPathName ());
	    break;
	case 5:
	    pTask->loadDucWith (pTask->getSelf ().codDatabase ()->NDFPathName ());
	    break;
	case 6:
	    pTask->loadDucWith (pTask->getSelf ().codDatabase ()->OSDPathName ());
	    break;
	default:
	    pTask->loadDucWithNA ();
	    break;
	}
	break;

    case XActiveDriver: {
	    IOMHandle handle;
	    handle.construct (pTask->channel ());
	    pTask->loadDucWithDouble (*(double *)&handle);
	}
	break;

    case XGetProcessInteger:
	switch (V_TOTSC_PrimitiveFlags) {
	case 0:
	    pTask->loadDucWithInteger (IOM_DriverTableSize ());
	    break;
	case 1:
	    pTask->loadDucWithInteger (gethostid ());
	    break;
	default:
	    pTask->loadDucWithNA ();
	    break;
	}
	break;

    default:
	pTask->raiseUnimplementedAliasException ("ProcessInformation");
	break;
    }
}


/**********************************************
 *****  I/O Driver Description Primitive  *****
 **********************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

typedef char const* (*Ref_Describer) (int);

PrivateFnDef char const *IOMDescriptionHelper (bool reset, va_list argList) {
    static Ref_Describer	accessFunction;
    static int			*iuvp, *iuvl;

    if (reset) {
	V::VArgList iArgList (argList);
	VPrimitiveTask *pTask = iArgList.arg<VPrimitiveTask*>();
	accessFunction = iArgList.arg<Ref_Describer>();

	DSC_Descriptor &rCurrent = pTask->getCurrent ();
	iuvp = rCurrent.isScalar ()
	? &DSC_Descriptor_Scalar_Int (rCurrent)
	: rtINTUV_CPD_Array (DSC_Descriptor_Value (rCurrent));
	iuvl = iuvp + pTask->cardinality ();
    }
    else if (iuvp < iuvl)
	return accessFunction (*iuvp++);

    return NilOf (char const*);
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (IOMDescription) {
    rtLSTORE_Handle *stringStore = NilOf (rtLSTORE_Handle*);

    pTask->getCurrent ();

    switch (V_TOTSC_Primitive) {
    case XIOMStateDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_StateDescription
	);
	break;
    case XVkStatusTypeDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask,
	    (char const*(*)(VkStatusType))&VkStatus::DescriptionFor
	);
	break;
    case XVkStatusCodeDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask,
	    (char const*(*)(VkStatusCode))&VkStatus::DescriptionFor
	);
	break;
    case XIOMChannelTypeDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_ChannelTypeDescription
	);
	break;
    case XIOMOptionDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_OptionDescription
	);
	break;
    case XIOMPromptFormatDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_PromptFormatDescription
	);
	break;
    case XIOMBinaryFormatDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_BinaryFormatDescription
	);
	break;
    case XIOMTrimFormatDescription:
	stringStore = rtLSTORE_NewStringStore (
	    pTask->codScratchPad (), IOMDescriptionHelper, pTask, IOM_TrimFormatDescription
	);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("IOMDescription");
        break;
    }

    pTask->loadDucWithListOrStringStore (stringStore);
}


/*******************************************
 *****  I/O Driver Creation Primitive  *****
 *******************************************/

/*!!!!!!!!!!!!!!!!!!!!!*
 *!!!  File Helper  !!!*
 *!!!!!!!!!!!!!!!!!!!!!*/

PrivateFnDef void CreateFileDriver (
    IOMHandle*			pHandle,
    IOMDriver::Options const&	rOptions,
    char const*			pOptionsString,
    char const*			pFileNameString
)
{
    long tmpLong;
    char*tmpTail;

/*****  Parse file 'pOptionsString' argument, ...  *****/
    IOMFile::MappingMode mappingMode = IOMFile::MappingMode_ReadOnly;
    int oflags		= 0;
    int omode		= 0666;
    bool oread		= false;
    bool owrite		= false;
    bool omapped	= false;
    bool otrunc		= false;
    size_t truncateSize	= 0;

    while (*pOptionsString) {
	switch (toupper (*pOptionsString++)) {
	case 'M':
	    omapped = !(oflags & O_APPEND);
	    break;
	case 'R':
	    oread   = true;
	    break;
	case 'W':
	    owrite  = true;
	    mappingMode = IOMFile::MappingMode_ReadWrite;
	    break;
	case 'A':
	    omapped = false;
	    owrite  = true;
	    oflags |= O_APPEND;
	    break;
	case 'C':
	    oflags |= O_CREAT;
	    owrite  = true;
	    tmpLong = strtol (pOptionsString, &tmpTail, 8);
	    if (tmpTail != pOptionsString) {
		omode = (int)tmpLong;
		pOptionsString = tmpTail;
	    }
	    break;
	case 'T':
	    oflags |= O_TRUNC;
	    otrunc  =
	    owrite  = true;
	    tmpLong = strtol (pOptionsString, &tmpTail, 0);
	    if (tmpTail != pOptionsString) {
		truncateSize = (size_t)tmpLong;
		pOptionsString = tmpTail;
	    }
	    break;
	case 'X':
	    oflags |= O_EXCL;
	    break;
#if !defined(_WIN32)
	case 'S':
	    oflags |= O_SYNC;
	    break;
#endif
	default:
	    break;
	}
    }

    if (oread && owrite)
	oflags |= O_RDWR;
    else if (owrite)
	oflags |= O_WRONLY;
    else
	oflags |= O_RDONLY;

/*****  ... open the file, ...  *****/
    int fd = open (pFileNameString, oflags, omode);
    if (fd < 0) {
	VkStatus iStatus;
	iStatus.MakeErrorStatus ();
        pHandle->construct (&iStatus);
	return;
    }
#if !defined(_WIN32)
    fcntl (fd, F_SETFD, FD_CLOEXEC);
#endif

/*****  ... pre-allocate it, ...  *****/
    if (otrunc)
	ftruncate (fd, truncateSize);

/*****  ... and create and return a driver for the file:  *****/
    pHandle->construct (
	new IOMFile (
	    rOptions, fd, pFileNameString, omapped ? mappingMode : IOMFile::MappingMode_None
	)
    );
}


/*!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  Filter Helper  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!*/

PrivateFnDef void CreateProcessDriver (
    IOMHandle*			pHandle,
    IOMDriver::Options const&	rOptions,
    char const*			pOptionsString,
    char const*			pCommandString
) {
/*****  Parse the 'pOptionsString' argument, ...  *****/
    bool bCreatingAnErrorChannel = false;
    while (*pOptionsString) {
	switch (toupper (*pOptionsString++)) {
	case 'E':
	    bCreatingAnErrorChannel = true;
	    break;
	default:
	    break;
	}
    }

/*****  ... create the process, ...  *****/
    VkStatus iStatus;
    Vca::VProcess::Reference pProcess;
    Vca::VBSConsumer::Reference pStdBSToPeer;
    Vca::VBSProducer::Reference pStdBSToHere;
    Vca::VBSProducer::Reference pErrBSToHere;

    if (!Vca::VDeviceFactory::Supply (
	    iStatus,
	    pProcess,
	    pStdBSToPeer,
	    pStdBSToHere,
	    bCreatingAnErrorChannel ? pErrBSToHere : pStdBSToHere,
	    pCommandString
	)
    ) {
	pHandle->construct (&iStatus);
    }
    else {
	IOMStream* pDataDriver = new IOMStream (rOptions, pStdBSToHere, pStdBSToPeer);
	pHandle->construct (pDataDriver);

	if (bCreatingAnErrorChannel) pDataDriver->SetErrorInputDriver (
	    new IOMStream (rOptions, pErrBSToHere, NilOf (Vca::VBSConsumer*))
	);
    }
}


/*!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  Socket Helper  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!*/

PrivateFnDef void CreateSocketDriver (
    IOMHandle*			pHandle,
    IOMDriver::Options const&	rOptions,
    char const*			pOptionsString,
    char const*			pProtocol,
    char const*			pName
)
{
/*****  Parse the 'pOptionsString' argument, ...  *****/
    bool bPassive = false;
//    bool bBinding = false;
    bool bNoDelay = false;

    while (*pOptionsString) {
	switch (toupper (*pOptionsString++)) {
	case 'P':
	    bPassive = true;
	    break;
	case 'B':
//	    bBinding = true;
	    break;
	case 'N':
	    bNoDelay = true;
	    break;
	default:
	    break;
	}
    }

    if (bPassive) {
	VkStatus iStatus; Vca::VListener::Reference pListener;
	if (!Vca::VDeviceFactory::Supply (iStatus, pListener, pName, pProtocol, bNoDelay))
	    pHandle->construct (&iStatus);
	else {
	    Vca::IListener::Reference pIListener;
	    pListener->getRole (pIListener);
	    pHandle->construct (new IOMService (rOptions, pIListener));
	}
    }
    else {
#if !defined(__VMS) && !defined(_WIN32)
    //  Allow listeners but not connections from inside the jail...
	static bool bInJail = true;
	static bool bInJailUninitialized = true;

	if (bInJailUninitialized) {
	    struct stat iStats;
	    bInJail = stat ("/usr/bin/telnet", &iStats) < 0 || (
		iStats.st_mode & (S_IXUSR + S_IXGRP + S_IXOTH)
	    ) == 0;
	    bInJailUninitialized = false;
	}

	if (bInJail) {
	    VkStatus iStatus;
	    iStatus.MakeFailureStatus ();
	    pHandle->construct (&iStatus);
	    return;
	}
#endif
	VkStatus iStatus; Vca::VBSConsumer::Reference pBSToPeer; Vca::VBSProducer::Reference pBSToHere;
	if (Vca::VDeviceFactory::Supply (iStatus, pBSToHere, pBSToPeer, pName, pProtocol))
	    pHandle->construct (new IOMStream (rOptions, pBSToHere, pBSToPeer));
	else
	    pHandle->construct (&iStatus);
    }
}


/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void CreateHelper (
    IOMHandle *pHandle, IOMDriver::Options const &rOptions, char const *openTarget
) {
    VkStatus iStatus;
    iStatus.MakeFailureStatus ();
    pHandle->construct (&iStatus);

    bool notOk = false;
    int details = 0;
    char service[65], options[257];
    if (2 == STD_sscanf (
	    openTarget, " %64[^,], %256[^:]: %n", service, options, &details
	) && details > 0
    );
    else if (1 == STD_sscanf (
	    openTarget, " %64[^:]: %n", service, &details
	) && details > 0
    ) strcpy (options, "");
    else notOk = true;

    if (notOk);
    else if (0 == strcmp (service, "file")) CreateFileDriver (
	pHandle, rOptions, options, openTarget + details
    );
    else if (0 == strcmp (service, "process")) CreateProcessDriver (
	pHandle, rOptions, options, openTarget + details
    );
    else CreateSocketDriver (
	pHandle, rOptions, options, service, openTarget + details
    );
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (Create) {
#define handleString(xString) {\
    CreateHelper (handleCursor++, options, pStrings[xString]);\
}

/*****  Initialize the default connection driver options, ...  *****/
    IOMDriver::Options options;

/*****  ... process the command strings, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;
    IOMHandle* handleCursor;
    if (rCurrent.isScalar ()) {
	IOMHandle handle;
	handleCursor = &handle;

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}

	pTask->loadDucWithDouble (*(double *)&handle);
    }
    else {
	M_CPD* handles = pTask->NewDoubleUV ();
	handleCursor = (IOMHandle*)rtDOUBLEUV_CPD_Array (handles);

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}

	pTask->loadDucWithMonotype (handles);
	handles->release ();
    }

#undef handleString
}


/*****************************************
 *****  I/O Driver Accept Primitive  *****
 *****************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void AcceptHelper (
    IOMHandle*			pHandle,
    IOMHandle*			pService
)
{
    IOMDriver* pDriver = pService->getConnection ();

    if (pDriver)
	pHandle->construct (pDriver);
    else {
	VkStatus iStatus;
	iStatus.MakeFailureStatus ();
	pHandle->construct (&iStatus);
    }
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (Accept) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	IOMHandle handle;

	AcceptHelper (
	    &handle,
	    (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent)
	);

	pTask->loadDucWithDouble (*(double *)&handle);
    }
    else {
	M_CPD* huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	M_CPD* ruv = pTask->NewDoubleUV ();
	IOMHandle* ruvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (ruv);

	while (huvp < huvl) {
	    AcceptHelper (ruvp++, huvp++);
	}

	pTask->loadDucWithMonotype (ruv);
	ruv->release ();
    }
}


/*****************************************
 *****  I/O Driver Access Primitive  *****
 *****************************************/

V_DefinePrimitive (Access) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	IOMHandle handle;

	handle.construct (DSC_Descriptor_Scalar_Int (rCurrent));

	pTask->loadDucWithDouble (*(double *)&handle);
    }
    else {
	M_CPD* iuv = DSC_Descriptor_Value (rCurrent);
	int *iuvp = rtINTUV_CPD_Array (iuv);
	int *iuvl = iuvp + UV_CPD_ElementCount (iuv);

	M_CPD* ruv = pTask->NewDoubleUV ();
	IOMHandle* ruvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (ruv);

	while (iuvp < iuvl) {
	    ruvp++->construct (*iuvp++);
	}

	pTask->loadDucWithMonotype (ruv);
	ruv->release ();
    }
}


/****************************************************
 *****  I/O Driver (Handle)->Double Primitive  *****
 ****************************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef double DriverHDPrimitiveHelper (
    VPrimitiveTask* pTask, IOMHandle* pHandle
)
{
    double result;

    switch (V_TOTSC_Primitive) {
    case XGetByteCount:
	result = pHandle->getByteCount ();
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DriverHDPrimitive");
	break;
    }

    return result;
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (DriverHDPrimitive) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	pTask->loadDucWithDouble (
	    DriverHDPrimitiveHelper (
		pTask, (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent)
	    )
	);
    }
    else {
	M_CPD* huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	M_CPD* ruv = pTask->NewDoubleUV ();
	double *ruvp = rtDOUBLEUV_CPD_Array (ruv);

	while (huvp < huvl)
	    *ruvp++ = DriverHDPrimitiveHelper (pTask, huvp++);

	pTask->loadDucWithMonotype (ruv);
	ruv->release ();
    }
}


/****************************************************
 *****  I/O Driver (Handle)->Integer Primitive  *****
 ****************************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef int DriverHIPrimitiveHelper (
    VPrimitiveTask* pTask, IOMHandle* pHandle
)
{
    int result;

    switch (V_TOTSC_Primitive) {
    case XGetIndex:
	result = pHandle->index ();
	break;
    case XGetState:
	result = pHandle->state ();
	break;
    case XGetStatus:
	result = pHandle->status ();
	break;
    case XGetError:
	result = pHandle->statusCode ();
	break;
    case XGetOptionIndex:
	result = pHandle->optionIndex ();
	break;
    case XGetOptionValue:
	result = pHandle->optionValue ();
	break;
    case XGetSeekOffset:
	result = pHandle->seekOffset ();
	break;
    case XGetChannelType:
	result = pHandle->channelType ();
	break;
    case XRetain:
	result = pHandle->retain ();
	break;
    case XRelease:
	result = pHandle->release ();
	break;
    case XEnableHandler:
	result = pHandle->enableHandler (pTask);
	break;
    case XDisableHandler:
	result = pHandle->disableHandler ();
	break;
    case XEndTransmission:
	result = pHandle->endTransmission ();
	break;
    case XEndReception:
	result = pHandle->endReception ();
	break;
    case XClose:
	result = pHandle->close ();
	break;
    case XFlushOutput:
	result = pHandle->putBufferedData ();
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DriverHIPrimitive");
	break;
    }

    return result;
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (DriverHIPrimitive) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	pTask->loadDucWithInteger (
	    DriverHIPrimitiveHelper (
		pTask, (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent)
	    )
	);
    }
    else {
	M_CPD* huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	M_CPD* ruv = pTask->NewIntUV ();
	int *ruvp = rtINTUV_CPD_Array (ruv);

	while (huvp < huvl)
	    *ruvp++ = DriverHIPrimitiveHelper (pTask, huvp++);

	pTask->loadDucWithMonotype (ruv);
	ruv->release ();
    }
}


/***************************************************
 *****  I/O Driver (Handle)->Handle Primitive  *****
 ***************************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void DriverHHPrimitiveHelper (VPrimitiveTask* pTask, IOMHandle* pHandle) {
    IOMDriver*			pDriver;
    VkStatus			iStatus;

    switch (V_TOTSC_Primitive) {
    case XGetErrorInput:
	if (IsntNil (pDriver = pHandle->errorInputDriver ()))
	    pHandle->construct (pDriver);
	else {
	    iStatus.MakeFailureStatus ();
	    pHandle->construct (&iStatus);
	}
	break;
    case XGetErrorOutput:
	if (IsntNil (pDriver = pHandle->errorOutputDriver ()))
	    pHandle->construct (pDriver);
	else {
	    iStatus.MakeFailureStatus ();
	    pHandle->construct (&iStatus);
	}
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DriverHHPrimitive");
	break;
    }
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (DriverHHPrimitive) {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	DriverHHPrimitiveHelper (
	    pTask, (IOMHandle*)&DSC_Descriptor_Scalar_Double (
		rCurrent
	    )
	);
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	while (huvp < huvl)
	{
	    DriverHHPrimitiveHelper (pTask, huvp++);
	}
    }

    pTask->loadDucWithCurrent ();
}


/************************************************************
 *****  I/O Driver (Handle, Integer)->Handle Primitive  *****
 ************************************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void DriverHIHPrimitiveHelper (
    VPrimitiveTask*		pTask,
    IOMHandle*			pHandle,
    int				argument
)
{
    switch (V_TOTSC_Primitive) {
    case XSetOptionIndex:
	pHandle->setOptionIndex ((IOMOption)argument);
	break;
    case XSetOptionValue:
	pHandle->setOptionValue (argument);
	break;
    case XSetSeekOffset:
	pHandle->setSeekOffset (argument);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DriverHIHPrimitive");
	break;
    }
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (DriverHIHPrimitive) {
    if (!GetArguments (pTask, RTYPE_C_IntUV)) {
	switch (V_TOTSC_Primitive) {
	case XSetOptionIndex:
	    pTask->sendBinaryConverseWithCurrent ("becomeOptionIndexOf:");
	    break;
	case XSetOptionValue:
	    pTask->sendBinaryConverseWithCurrent ("becomeOptionValueOf:");
	    break;
	case XSetSeekOffset:
	    pTask->sendBinaryConverseWithCurrent ("becomeSeekOffsetOf:");
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("DriverHIHPrimitive");
	    break;
	}
	return;
    }

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {

	DriverHIHPrimitiveHelper (
	    pTask, (IOMHandle*)&DSC_Descriptor_Scalar_Double (
		rCurrent
	    ),
	    DSC_Descriptor_Scalar_Int (ADescriptor)
	);
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	M_CPD *auv = DSC_Descriptor_Value (ADescriptor);

	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	int *auvp = rtINTUV_CPD_Array (auv);
	while (huvp < huvl)
	{
	    DriverHIHPrimitiveHelper (pTask, huvp++, *auvp++);
	}
    }

    pTask->loadDucWithCurrent ();
}

/*****************************************************************
 *****  I/O Driver (Handle, Integer)->Handle WAIT Primitive  *****
 *****************************************************************/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  VWaitTaskTrigger  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/******************************************************************************
 * The VWaitTaskTrigger class, which is an ITrigger Implementation, is used 
 * to suspend a primitive task and trigger it at a later timer with a timeout. 
 * This class also has the option of attaching a VTimer as a timeout 
 * for the trigger. Created solely for use in the DriverHIHWaitPrimitive 
 * implementation for suspending a task with timeout option....
******************************************************************************/

class VWaitTaskTrigger : public Vca::VRolePlayer {
    DECLARE_CONCRETE_RTT (VWaitTaskTrigger, Vca::VRolePlayer);

//  Aliases and Enumerations
public:
    typedef Vca::ITrigger ITrigger;

    enum State {
	State_Initialized,
	State_Triggerable,
	State_Triggered
    };

//  Construction
public:
    VWaitTaskTrigger (VTask *pTask);

//  Destruction
private:
    ~VWaitTaskTrigger (); 

//  Query
public:
    bool isTriggerable () const {
	return m_xState == State_Triggerable;
    }

//  Startup
public:
    void suspendTask (Vca::U64 sMicrosecondTimeout);

//  Base Roles
public:
    using BaseClass::getRole;

//  ITrigger Role
private:
    Vca::VRole<ThisClass,Vca::ITrigger> m_pITrigger;
public:
    void getRole (ITrigger::Reference&rpRole) {
	m_pITrigger.getRole (rpRole);
    }

//  ITrigger Method
public:
    void Process (ITrigger *pRole); 

//  State
protected:
    VTask::Reference const	m_pTask;
    Vca::VTimer::Reference	m_pTimer;	    
    State			m_xState;
};

/******************************************************************************
 * Note:
 *  VWaitTaskTrigger has a reference to VTimer and VTimer has a refernce to
 * VWaitTaskTrigger, thereby creating a reference cycle. This refernce cycle
 * is broken when the VTimer is triggered and the reference to VWaitTaskTrigger
 * is cleared after the triggering process.
******************************************************************************/

/***************************
 *****  Run Time Type  *****
 ***************************/

DEFINE_CONCRETE_RTT (VWaitTaskTrigger);

/**************************
 *****  Construction  *****
 **************************/

VWaitTaskTrigger::VWaitTaskTrigger (
    VTask *pTask
) : m_pTask (pTask), m_xState (State_Initialized), m_pITrigger (this) {
    traceInfo ("Creating Trigger");
}

/*************************
 *****  Destruction  *****
 *************************/

VWaitTaskTrigger::~VWaitTaskTrigger () {
    traceInfo ("Destroying Trigger");
}

/**********************************
 *****  Wait Task Operations  *****
 **********************************/

void VWaitTaskTrigger::suspendTask (Vca::U64 sMicrosecondTimeout) {
    m_pTask->suspend ();

    ITrigger::Reference pRole;
    getRole (pRole);
    m_pTimer.setTo (new Vca::VTimer (pRole, sMicrosecondTimeout));

    m_pTimer->start ();
    m_xState = State_Triggerable;
}

/***************************************
 ***** ITrigger Interface Methods  *****
 ***************************************/

/******************************************************************************
 * Routine: Process
 *	This method is invoked either from the IOMDriver's OnEvent () or 
 * from the Vca::VTimer's timeout handler. In either case does the operation of 
 * deleting this trigger from the triggerset of IOMDriver(s) into which this  
 * might have been inserted when the task was suspended. Also clears the VTask 
 * from this trigger which could lead to garbage collection of the trigger/task
******************************************************************************/

void VWaitTaskTrigger::Process (ITrigger *pRole) {
    if (!isTriggerable ()) 
	return;

    //  resume the suspended task...
    m_xState = State_Triggered;
    m_pTimer->cancel ();
    m_pTask->resume ();

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = m_pTask->getCurrent ();

    //  remove this trigger from other drivers.....
    if (rCurrent.isScalar ()) {
	IOMHandle *pHandle = (IOMHandle*) &DSC_Descriptor_Scalar_Double (rCurrent);
	IOMDriver *pDriver = pHandle->driver ();
	if (pDriver)
	    pDriver->deleteFromTriggerSet (pRole);
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);

	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	while (huvp < huvl) {
	    IOMDriver *pDriver = huvp->driver ();
	    if (pDriver)
		pDriver->deleteFromTriggerSet (pRole);
	    huvp++;
	}
    }
}

/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

/******************************************************************************
 * Routine: DriverHIHWaitPrimitive
 *	Creates an VWaitTaskTrigger object with the task that might have to be
 * suspended. If the task needs to wait on multiple drivers...the method iterates 
 * through the set of IOMDrivers, computing the minimum timeout value...
 * also inserts the trigger into the triggerset of IOMDrivers that are waiting 
 * for input . Also sets the timer on the trigger if required. If the task needs
 * wait on single IOMDriver (scalar) does the above things on the single IOMDriver.
******************************************************************************/

V_DefinePrimitive (DriverHIHWaitPrimitive) {
    if (!GetArguments (pTask, RTYPE_C_IntUV)) {
	pTask->sendBinaryConverseWithCurrent ("waitForInputOn:");
	return;
    }

    size_t sTimeOut = Vca_InfiniteWait;

    VWaitTaskTrigger::Reference const pTrigger (new VWaitTaskTrigger (pTask));
    Vca::ITrigger::Reference pTriggerRole;
    pTrigger->getRole (pTriggerRole);

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	IOMHandle *pHandle = (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent);
	IOMDriver *pDriver = pHandle->driver ();
	size_t iWaitTimeOut = (size_t)DSC_Descriptor_Scalar_Int (ADescriptor);
	sTimeOut = sTimeOut > iWaitTimeOut ? iWaitTimeOut : sTimeOut;
	    
	if (pDriver && !pDriver->isClosing () 
	    && sTimeOut > 0
	    && pDriver->InputOperationStatus () == VkStatusType_Blocked
	) {
	    pDriver->insertIntoTriggerSet (pTriggerRole);
	    pTrigger->suspendTask (sTimeOut);
	}
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	M_CPD *auv = DSC_Descriptor_Value (ADescriptor);

	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	int *auvp = rtINTUV_CPD_Array (auv);
	bool isTaskSuspendable = false;
	while (huvp < huvl) {
	    IOMDriver *pDriver = huvp->driver ();
	    size_t iWaitTimeOut = (size_t)(*auvp);
	    sTimeOut = sTimeOut > iWaitTimeOut ? iWaitTimeOut : sTimeOut;
		
	    if (pDriver && !pDriver->isClosing () 
		&& sTimeOut > 0
		&& pDriver->InputOperationStatus () == VkStatusType_Blocked
	    ) {
		isTaskSuspendable = true;
		pDriver->insertIntoTriggerSet (pTriggerRole);
	    }
	    auvp++; huvp++;
	}
	if (isTaskSuspendable) {
	    pTrigger->suspendTask (sTimeOut);
	}
    }
    pTask->loadDucWithCurrent (); 
}

/***********************************************************
 *****  I/O Driver (Handle, Handle)->Handle Primitive  *****
 ***********************************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void DriverHHHPrimitiveHelper (
    VPrimitiveTask*		pTask,
    IOMHandle*			pHandle,
    IOMHandle*			pArgument
)
{
    switch (V_TOTSC_Primitive) {
    case XSetErrorOutput:
	pHandle->setErrorOutput (pArgument);
	break;
    case XSetErrorInput:
	pHandle->setErrorInput (pArgument);
	break;
    default:
	pTask->raiseUnimplementedAliasException ("DriverHHHPrimitive");
	break;
    }
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (DriverHHHPrimitive) {
    if (!GetArguments (pTask, RTYPE_C_DoubleUV)) {
	switch (V_TOTSC_Primitive) {
	case XSetErrorOutput:
	    pTask->sendBinaryConverseWithCurrent ("becomeErrorOutputOf:");
	    break;
	case XSetErrorInput:
	    pTask->sendBinaryConverseWithCurrent ("becomeErrorInputOf:");
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("DriverHHHPrimitive");
	    break;
	}
	return;
    }

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	DriverHHHPrimitiveHelper (
	    pTask,
	    (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent),
	    (IOMHandle*)&DSC_Descriptor_Scalar_Double (ADescriptor)
	);
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	M_CPD *auv = DSC_Descriptor_Value (ADescriptor);

	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	IOMHandle* auvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (auv);

	while (huvp < huvl) DriverHHHPrimitiveHelper (pTask, huvp++, auvp++);
    }

    pTask->loadDucWithCurrent ();
}


/*******************************************
 *****  I/O Driver Handler Management  *****
 *******************************************/

/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef void InitializeHandlerPointer (
    DSC_Pointer &rHandlerPointer, VPrimitiveTask *pTask, bool bSettingHandler
) {
/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	IOMHandle *pHandle = (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent);
	unsigned int xDriver = pHandle->index ();

	rHandlerPointer.constructReferenceScalar (
	    IOM_HandlerPToken, xDriver > 0 ? xDriver - 1 : IOM_HandlerPToken->cardinality ()
	);

	if (bSettingHandler)
	    pHandle->setWiredHandler (false);
    }
    else {
	M_CPD* huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	M_CPD *ruv = rtREFUV_New (V_TOTSC_PToken, IOM_HandlerPToken);
	int *ruvp = rtREFUV_CPD_Array (ruv);

	while (huvp < huvl) {
	    IOMHandle *pHandle = huvp++;
	    unsigned int xDriver = pHandle->index ();

	    *ruvp++ = xDriver > 0 ? xDriver - 1 : IOM_HandlerPToken->cardinality ();

	    if (bSettingHandler)
		pHandle->setWiredHandler (false);
	}
	rHandlerPointer.construct (ruv);
	ruv->release ();
    }
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  Registration Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (SetHandler) {
    pTask->loadDucWithNextParameter ();

    DSC_Pointer iHandlerPointer;
    InitializeHandlerPointer (iHandlerPointer, pTask, true);

    pTask->duc ().assignTo (iHandlerPointer, IOM_HandlerVector);
    iHandlerPointer.clear ();

    IOM_HandlerVector->alignAll (true);

    pTask->loadDucWithSelf ();
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  Query Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetHandler) {
    DSC_Descriptor iHandlerDescriptor; {
	DSC_Pointer iHandlerPointer;
	InitializeHandlerPointer (iHandlerPointer, pTask, false);
        iHandlerDescriptor.construct (IOM_HandlerVector, iHandlerPointer);
	iHandlerPointer.clear ();
    }

    pTask->beginValueCall   (0);
    pTask->loadDucWithMoved (iHandlerDescriptor);
    pTask->commitRecipient  ();
    pTask->commitCall	    ();
}


/************************************************
 *****  I/O Driver Numeric Input Primitive  *****
 ************************************************/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  Type Definitions  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!*/

union GetNumberResult {
    unsigned char	asByte;
    unsigned short	asShort;
    unsigned int	asInt;
    float		asFloat;
    double		asDouble;
};


/*!!!!!!!!!!!!!!!!*
 *!!!  Helper  !!!*
 *!!!!!!!!!!!!!!!!*/

PrivateFnDef bool GetNumberHelper (
    VPrimitiveTask *pTask, IOMHandle *pHandle, GetNumberResult *pResult	
)
{
    bool resultIsValid;

    switch (V_TOTSC_Primitive) {
    case XGetSignedByte:
	resultIsValid = pHandle->getByte (&pResult->asByte);
	pResult->asInt = (char)pResult->asByte;
	break;
    case XGetUnsignedByte:
	resultIsValid = pHandle->getByte (&pResult->asByte);
	pResult->asInt = (unsigned char)pResult->asByte;
	break;
    case XGetSignedShort:
	resultIsValid = pHandle->getShort (&pResult->asShort);
	pResult->asInt = (short)pResult->asShort;
	break;
    case XGetUnsignedShort:
	resultIsValid = pHandle->getShort (&pResult->asShort);
	pResult->asInt = (unsigned short)pResult->asShort;
	break;
    case XGetLong:
	resultIsValid = pHandle->getLong (&pResult->asInt);
	break;
    case XGetFloat:
	resultIsValid = pHandle->getFloat (&pResult->asFloat);
	break;
    case XGetDouble:
	resultIsValid = pHandle->getDouble (&pResult->asDouble);
	break;
    default:
	resultIsValid = false;
	break;
    }

    return resultIsValid;
}


/*!!!!!!!!!!!!!!!!!!!*
 *!!!  Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetNumber) {
    switch (V_TOTSC_Primitive) {
    case XGetSignedByte:
    case XGetUnsignedByte:
    case XGetSignedShort:
    case XGetUnsignedShort:
    case XGetLong:
    case XGetFloat:
    case XGetDouble:
	break;
    default:
	pTask->raiseUnimplementedAliasException ("GetNumber");
	break;
    }

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    GetNumberResult result;

    if (rCurrent.isScalar ()) {
	IOMHandle* pHandle = (IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent);

	if (!GetNumberHelper (pTask, pHandle, &result))
	    pTask->loadDucWithNA ();
	else switch (V_TOTSC_Primitive) {
	    case XGetSignedByte:
	    case XGetUnsignedByte:
	    case XGetSignedShort:
	    case XGetUnsignedShort:
	    case XGetLong:
		pTask->loadDucWithInteger ((int)result.asInt);
		break;
	    case XGetFloat:
		pTask->loadDucWithFloat (result.asFloat);
		break;
	    case XGetDouble:
		pTask->loadDucWithDouble (result.asDouble);
		break;
	    default:
	        pTask->loadDucWithNA ();
		break;
	}
    }
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	rtLINK_CType *resultLinkC = NilOf (rtLINK_CType *);
	unsigned int totalRun = 0;
	unsigned int validRun = 0;

	M_CPD *resultUVector = 0;
	switch (V_TOTSC_Primitive) {
	case XGetSignedByte:
	case XGetUnsignedByte:
	case XGetSignedShort:
	case XGetUnsignedShort:
	case XGetLong:
	    resultUVector = pTask->NewIntUV ();
	    break;
	case XGetFloat:
	    resultUVector = pTask->NewFloatUV ();
	    break;
	case XGetDouble:
	    resultUVector = pTask->NewDoubleUV ();
	    break;
	default:
	    resultUVector = pTask->NewUndefUV ();
	    break;
	}

	union cursor_t {
	    void *asVoid;
	    int *asInt;
	    float *asFloat;
	    double *asDouble;
	} pResult;

	pResult.asVoid = UV_CPD_Array (resultUVector, void);

	while (huvp < huvl) {
	    bool resultValid = GetNumberHelper (pTask, huvp++, &result);

	    if (resultValid) {
		switch (V_TOTSC_Primitive) {
		case XGetSignedByte:
		case XGetUnsignedByte:
		case XGetSignedShort:
		case XGetUnsignedShort:
		case XGetLong:
		    *pResult.asInt++   = result.asInt;
		    break;
		case XGetFloat:
		    *pResult.asFloat++  = result.asFloat;
		    break;
		case XGetDouble:
		    *pResult.asDouble++ = result.asDouble;
		    break;
		default:
		    resultValid = false;
		    break;
		}
	    }

	    if (resultValid)
		validRun++;
	    else if (validRun > 0) {
		if (IsNil (resultLinkC))
		    resultLinkC = pTask->NewSubset ();
		rtLINK_AppendRange (resultLinkC, totalRun - validRun, validRun);
		validRun = 0;
	    }

	    totalRun++;
	}

	if (validRun > 0 && validRun < totalRun && IsNil (resultLinkC))
	    resultLinkC = pTask->NewSubset ();

	if (IsntNil (resultLinkC)) {	/* ... partial function ... */
	    if (validRun > 0) rtLINK_AppendRange (
		resultLinkC, totalRun - validRun, validRun
	    );
	    resultLinkC->Close (pTask->NewDomPToken (resultLinkC->ElementCount ()));

	    pTask->loadDucWithPartialFunction (resultLinkC, resultUVector);

	    resultLinkC->release ();
	}
	else if (validRun == totalRun) 	/* ... total function ... */
	    pTask->loadDucWithMonotype (resultUVector);
	else				/* ... undefined function ... */
	    pTask->loadDucWithNA ();

	resultUVector->release ();
    }
}


/************************************************
 *****  I/O Driver String Input Primitives  *****
 ************************************************/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  String Cache Manager  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*********************
 *  Type Definition  *
 *********************/

class StringCache : public VTransient {
protected:
    struct StringDescriptor {
	size_t			m_length;
	char *			m_string;
    };

//  Construction
public:
    StringCache (VPrimitiveTask* pTask);

//  Access
public:
    M_KOT *codKOT () const {
	return m_pTask->codKOT ();
    }

//  Use
public:
    void AddName (
	IOMHandle* pHandle, int (IOMHandle::*nameGrabber) (char**, size_t*) const
    );
    void AddString (IOMHandle* pHandle, size_t cbMinLength, size_t cbMaxLength);
    void AddString (IOMHandle* pHandle, size_t sString) {
	AddString (pHandle, sString, sString);
    }
    void AddLine (IOMHandle* pHandle, char const* zPrompt);

    void Next	();
    void Commit	();

//  State
protected:
    VPrimitiveTask	* const	m_pTask;
    size_t			m_tcount;	/* Entry Count (==V_TOTSC_PToken) */
    size_t			m_scount;	/* String Count */
    size_t			m_ccount;	/* Character Count */
    size_t			m_totalRun;	/* Total Run (== cursor - content)*/
    size_t			m_validRun;	/* Valid Run */
    StringDescriptor*		m_content;
    StringDescriptor*		m_cursor;
    StringDescriptor*		m_limit;
    rtLINK_CType*		m_pSubset;
};

/******************
 *  Construction  *
 ******************/

StringCache::StringCache (VPrimitiveTask *pTask) : m_pTask (pTask) {
    m_tcount	= m_pTask->ptoken ()->cardinality ();
    m_scount	=
    m_ccount	=
    m_totalRun	=
    m_validRun	= 0;
    m_content	=
    m_cursor	= (StringDescriptor*)allocate (m_tcount * sizeof (StringDescriptor));
    m_limit	= m_content + m_tcount;
    m_pSubset	= NilOf (rtLINK_CType *);
}


/**********************
 *  'AddName' Helper  *
 **********************/

void StringCache::AddName (
    IOMHandle* pHandle, int (IOMHandle::*nameGrabber) (char**, size_t*) const
)
{
    StringDescriptor* pString = m_cursor;

    (pHandle->*nameGrabber)(&pString->m_string, &pString->m_length);

    Next ();
}


/************************
 *  'AddString' Helper  *
 ************************/

void StringCache::AddString (
    IOMHandle* pHandle, size_t cbMinLength, size_t cbMaxLength
)
{
    StringDescriptor* pString = m_cursor;

    pHandle->getString (
	cbMinLength, cbMaxLength, &pString->m_string, &pString->m_length
    );

    Next ();
}

/**********************
 *  'AddLine' Helper  *
 **********************/

void StringCache::AddLine (IOMHandle* pHandle, char const*zPrompt) {
    StringDescriptor* pString = m_cursor;

    pHandle->getLine (
	zPrompt, &pString->m_string, &pString->m_length
    );

    Next ();
}


/**********
 *  Next  *
 **********/

void StringCache::Next () {
    StringDescriptor* pString = m_cursor++;

    if (IsntNil (pString->m_string)) {
	m_scount++;
	m_ccount += pString->m_length;
	m_validRun++;
    }
    else if (m_validRun > 0) {
	if (IsNil (m_pSubset))
	    m_pSubset = m_pTask->NewSubset ();
	rtLINK_AppendRange (m_pSubset, m_totalRun - m_validRun, m_validRun);
	m_validRun = 0;
    }
    m_totalRun++;
}


/************
 *  Commit  *
 ************/

void StringCache::Commit () {
/*****  Create the subset link if necessary, ...  *****/
    if (m_validRun > 0 && m_validRun < m_totalRun && IsNil (m_pSubset))
	m_pSubset = m_pTask->NewSubset ();

/*****  Close the subset link if it was created, ...  *****/
    if (m_pSubset) {
	if (m_validRun > 0) rtLINK_AppendRange (
	    m_pSubset, m_totalRun - m_validRun, m_validRun
	);
	m_pSubset->Close (m_pTask->NewDomPToken (m_scount));
    }

/*****  Construct and return the string store, ...  *****/
    if (m_scount > 0) {
	M_CPD *uvector = m_pTask->NewCharUV (m_scount + m_ccount);
	char *cursor = rtCHARUV_CPD_Array (uvector);

	m_cursor = m_content;
	while (m_cursor < m_limit) {
	    StringDescriptor* pString = m_cursor++;

	    if (IsntNil (pString->m_string)) {
		strcpy (cursor, pString->m_string);
		cursor += pString->m_length;
		*cursor++ = '\0';
		deallocate (pString->m_string);
	    }
	}

	m_pTask->loadDucWithPartialCorrespondence (
	    rtLSTORE_NewStringStoreFromUV (uvector), m_pSubset
	);

	if (m_pSubset)
	    m_pSubset->release ();
	uvector->release ();
    }
/*****  ... or return NA if no strings are in the cache:  *****/
    else
	m_pTask->loadDucWithNA ();

/*****  Finally, delete the cache content vector and return.  *****/
    deallocate (m_content);
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  'GetName' Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetName) {
/*****  Decode the name being accessed, ...  *****/
    int (IOMHandle::*nameGrabber) (char **, size_t *) const;
    switch (V_TOTSC_Primitive)
    {
    case XGetSocketName:
	nameGrabber = &IOMHandle::getSocketNameString;
	break;
    case XGetPeerName:
	nameGrabber = &IOMHandle::getPeerNameString;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("GetName");
	break;
    }

/*****  Get the arguments to this primitive, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... initialize the string cache, ...  *****/
    StringCache stringCache (pTask);
    if (rCurrent.isScalar ()) stringCache.AddName (
	(IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent), nameGrabber
    );
    else {
	M_CPD* huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	while (huvp < huvl)
	    stringCache.AddName (huvp++, nameGrabber);
    }

/*****  ... and create and return the string store:  *****/
    stringCache.Commit ();
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  'GetString' Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetString) {
/*****  Get the arguments to this primitive, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... initialize the string cache, ...  *****/
    StringCache stringCache (pTask);
    if (rCurrent.isScalar ()) stringCache.AddString (
	(IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent), 1, 0
    );
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	while (huvp < huvl)
	    stringCache.AddString (huvp++, 1, 0);
    }

/*****  ... and create and return the string store:  *****/
    stringCache.Commit ();
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  'GetFixedString' Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetFixedString) {
/*****  Get the arguments to this primitive, ...  *****/
    if (!GetArguments (pTask, RTYPE_C_IntUV)) {
	pTask->sendBinaryConverseWithCurrent ("getStringFrom:");
	return;
    }

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  ... initialize the string cache, ...  *****/
    StringCache stringCache (pTask);
    if (rCurrent.isScalar ()) stringCache.AddString (
	(IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent),
	(size_t)DSC_Descriptor_Scalar_Int (ADescriptor)
    );
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);
	rtINTUV_ElementType *luvp = rtINTUV_CPD_Array (
	    DSC_Descriptor_Value (ADescriptor)
	);

	while (huvp < huvl) {
	    stringCache.AddString (huvp++, (size_t)(*luvp));
	    luvp++;
	}
    }

/*****  ... and create and return the string store:  *****/
    stringCache.Commit ();
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
 *!!!  'GetLine' Primitive  !!!*
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

V_DefinePrimitive (GetLine) {
/*****  Get the arguments to this primitive, ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    
/*****  ... initialize the string cache, ...  *****/
    StringCache stringCache (pTask);
    if (rCurrent.isScalar ()) stringCache.AddLine (
	(IOMHandle*)&DSC_Descriptor_Scalar_Double (rCurrent),
	NilOf (char const*)
    );
    else {
	M_CPD *huv = DSC_Descriptor_Value (rCurrent);
	IOMHandle* huvp = (IOMHandle*)rtDOUBLEUV_CPD_Array (huv);
	IOMHandle* huvl = huvp + UV_CPD_ElementCount (huv);

	while (huvp < huvl)
	    stringCache.AddLine (huvp++, NilOf (char const*));
    }

/*****  ... and create and return the string store:  *****/
    stringCache.Commit ();
}


/******************************************
 *****  I/O Driver Output Primitives  *****
 ******************************************/

/***************
 *  PutNumber  *
 ***************/

V_DefinePrimitive (PutNumber) {
    char const *converse;
    switch (V_TOTSC_Primitive) {
    case XPutByte:
	converse = "putByte:";
	break;
    case XPutShort:
	converse = "putShort:";
	break;
    case XPutLong:
	converse = "putLong:";
	break;
    case XPutFloat:
	converse = "putFloat:";
	break;
    case XPutDouble:
	converse = "putDouble:";
	break;
    default:
	pTask->raiseUnimplementedAliasException ("PutNumber");
        break;
    }

    if (!GetArguments (pTask, RTYPE_C_DoubleUV)) {
	pTask->sendBinaryConverseWithCurrent (converse);
	return;
    }

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    if (rCurrent.isScalar ()) {
	int result;

	IOMHandle *pHandle = (IOMHandle*)&DSC_Descriptor_Scalar_Double (ADescriptor);

	switch (V_TOTSC_Primitive) {
	case XPutByte:
	    result = pHandle->putByte (
		(unsigned char)DSC_Descriptor_Scalar_Int (rCurrent)
	    );
	    break;
	case XPutShort:
	    result = pHandle->putShort (
		(unsigned short)DSC_Descriptor_Scalar_Int (rCurrent)
	    );
	    break;
	case XPutLong:
	    result = pHandle->putLong (
		DSC_Descriptor_Scalar_Int (rCurrent)
	    );
	    break;
	case XPutFloat:
	    result = pHandle->putFloat (
		DSC_Descriptor_Scalar_Float (rCurrent)
	    );
	    break;
	case XPutDouble:
	    result = pHandle->putDouble (
		DSC_Descriptor_Scalar_Double (rCurrent)
	    );
	    break;
	default:
	    pTask->raiseUnimplementedAliasException ("PutNumber");
	    break;
	}
	
	pTask->loadDucWithInteger (result);
    }
    else {
	M_CPD *resultUVector = pTask->NewIntUV ();
	int *pResultCursor = rtINTUV_CPD_Array (resultUVector);
	int *pResultLimit  = pResultCursor + UV_CPD_ElementCount (resultUVector);
	union cursor_t {
	    void *asVoid;
	    int *asInt;
	    float *asFloat;
	    double *asDouble;
	} pNumber;

	IOMHandle *pHandle = (IOMHandle*)rtDOUBLEUV_CPD_Array (DSC_Descriptor_Value (ADescriptor));
	pNumber.asVoid = UV_CPD_Array (DSC_Descriptor_Value (rCurrent), void);

	while (pResultCursor < pResultLimit)
	    switch (V_TOTSC_Primitive) {
	    case XPutByte:
		*pResultCursor++ = pHandle++->putByte   ((unsigned char)*pNumber.asInt++);
		break;
	    case XPutShort:
		*pResultCursor++ = pHandle++->putShort  ((unsigned short)*pNumber.asInt++);
		break;
	    case XPutLong:
		*pResultCursor++ = pHandle++->putLong   (*pNumber.asInt++);
		break;
	    case XPutFloat:
		*pResultCursor++ = pHandle++->putFloat  (*pNumber.asFloat++);
		break;
	    case XPutDouble:
		*pResultCursor++ = pHandle++->putDouble (*pNumber.asDouble++);
		break;
	    default:
		pTask->raiseUnimplementedAliasException ("PutNumber");
		break;
	    }

	pTask->loadDucWithMonotype (resultUVector);
	resultUVector->release ();
    }
}


/***************
 *  PutString  *
 ***************/

V_DefinePrimitive (PutString) {
    IOMHandle*			pHandle;
    int *			pResult;

#define handleString(stringIndex) {\
    *pResult++ = pHandle++->putString (pStrings[stringIndex]);\
}

    if (!GetArguments (pTask, RTYPE_C_DoubleUV)) {
	pTask->sendBinaryConverseWithCurrent ("putString:");
	return;
    }

/*****  Get the data argument in '^current', ...  *****/
    DSC_Descriptor& rCurrent = pTask->getCurrent ();

    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

    if (rCurrent.isScalar ()) {
	int result;

	pResult = &result;
	pHandle = (IOMHandle*)&DSC_Descriptor_Scalar_Double (ADescriptor);

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}

	pTask->loadDucWithInteger (result);
    }
    else {
	M_CPD *resultUVector = pTask->NewIntUV ();

	pResult = rtINTUV_CPD_Array (resultUVector);
	pHandle = (IOMHandle*)rtDOUBLEUV_CPD_Array (DSC_Descriptor_Value (ADescriptor));

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}

	pTask->loadDucWithMonotype (resultUVector);
	resultUVector->release ();
    }

#undef handleString
}


/******************************************
 *****  Output Management Primitives  *****
 ******************************************/

/*******************************
 *  File->Output Copy Handler  *
 *******************************/

PrivateFnDef int CopyFileToOutputBuffer (
    VOutputGenerator& rOutputGenerator, char const *pFileName
)
{
    FILE	*file;
    char	buffer[1024];

    if (IsNil (file = fopen (pFileName, "r")))
	return errno;

    while (IsntNil (fgets (buffer, sizeof (buffer), file)))
	rOutputGenerator.putString (buffer);

    fclose (file);

    rOutputGenerator.advance ();

    return 0;
}


/*******************************
 *  Output->File Copy Handler  *
 *******************************/
 
PrivateFnDef int CopyOutputToFileGeneral (
    VOutputGenerator&		rOutputGenerator,
    char const*			pOutput,
    char const*			pFileName,
    int				openFlags,
    int				unlinkOnError,
	int 			fd
) {
    if (fd < 0) {
	if (IsNil (pOutput)) // do something with data in the buffer
	    rOutputGenerator.outputBuffer()->moveOutputTo ();
	return errno ;
    }
    if (IsNil (pOutput)) { // output still in lio buffers
	FILE *stream = fdopen (fd, openFlags & O_TRUNC ? "w" : "a");
	if (IsNil (stream)) {
	    close (fd);
	    return errno ;
	}
	rOutputGenerator.outputBuffer()->moveOutputTo (stream, "\377");
	fclose (stream);
	return 0;
    }
    else while (*pOutput) { // output is contained in this string
	switch (*pOutput) {
	case '\377':
	    pOutput++;
	    break;
	default: {
		int span = write (fd, pOutput, strcspn (pOutput, "\377"));
		if (span < 0) {
		    close (fd);
		    if (unlinkOnError)
			unlink (pFileName);
		    return errno ;
		}
		pOutput += span;
	    }
	    break;
	}
    }
    close (fd);
    return 0;
}
#ifndef _WIN32
// currently filterOutputOf:[] does not work on Windows ... To make it work, we will have to provide a Windows version of
// mkstemp ... possibly using GetTempFileName
PrivateFnDef int CopyOutputToFile (
    VOutputGenerator&		rOutputGenerator,
    char const*			pOutput,
    char*			pFileName,
    int				openFlags,
    int				unlinkOnError
) {
    int fd = mkstemp (pFileName);
    return CopyOutputToFileGeneral (rOutputGenerator, pOutput, pFileName, openFlags, unlinkOnError, fd);
}
#endif

PrivateFnDef int CopyOutputToFile (
    VOutputGenerator&		rOutputGenerator,
    char const*			pOutput,
    char const*			pFileName,
    int				openFlags,
    int				unlinkOnError
) {
    int fd = open (pFileName, openFlags, 0666);
    return CopyOutputToFileGeneral (rOutputGenerator, pOutput, pFileName, openFlags, unlinkOnError, fd);
}


/*********************
 *  Filter Handlers  *
 *********************/

#if defined(_WIN32)
#define FilterOutput1 FilterOutput2

#elif defined(__VMS)
PrivateFnDef int FilterOutput1 (
    VOutputGenerator&		rOutputGenerator,
    char const*			output,
    char const*			command,
    int				Unused(openFlags),
    int				Unused(unlinkOnError)
) {
    return ENOTSUP;
}

#else
PrivateFnDef int FilterOutput1 (
    VOutputGenerator&		rOutputGenerator,
    char const*			output,
    char const*			command,
    int				Unused(openFlags),
    int				Unused(unlinkOnError)
) {
/*****  Parse the command...  *****/
    Vca::VProcess::Command iCommand (command);
    char **argv = iCommand.argv ();
    if (IsNil (argv[0]))
	return 0;

/*****  Create the input file ...  *****/
    char inputFileName [1024];
    strncpy (inputFileName, "/tmp/fileXXXXXX", 1024);
    int result = CopyOutputToFile (
	rOutputGenerator, output, inputFileName, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, true
    );
    if (result != 0)
	return result;

/*****  ... and re-open it for input:  *****/
    int ifd = open (inputFileName, O_RDONLY, 0666);
    if (ifd < 0) {
	unlink	(inputFileName);
	return	errno ;
    }

/*****  Create the output file ...  *****/
    char outputFileName [1024];
    strncpy (outputFileName, "/tmp/XXXXXX", 1024);
    int ofd = mkstemp (outputFileName);
    if (ofd < 0) {
	close	(ifd);
	unlink	(inputFileName);
	return errno ;
    }

/*****  Fork and 'exec' the filter command:  *****/
    pid_t pid = vfork ();
    if (pid < 0) {
	close (ifd);
	close (ofd);
	unlink (inputFileName);
	unlink (outputFileName);
	return errno ;
    }
    else if (pid == 0) {
      /****
       * establish the input and output files created above as stdin, stdout
       * and stderr for the command to be 'exec'ed ...
       * if stdin, stdout or stderr have been closed in the parent, it is
       * possible that one of these files is assigned a file descriptor
       * normally used for stdio. Use dup to ensure we don't accidentally
       * close the file before we setup stdio for the child.
       ****/

	while (ifd < 3 && ifd >= 0) ifd = dup (ifd);
	while (ofd < 3 && ofd >= 0) ofd = dup (ofd);

	close (0);
	close (1);
	close (2);

	dup2 (ifd, 0); /* stdin  */
	close (ifd);

	dup2 (ofd, 1); /* stdout */
	dup2 (ofd, 2); /* stderr */
	close (ofd);

	execvp (argv[0], argv);
	_exit (errno);
    }

    close (ifd);
    close (ofd);

    waitpid (pid, &result, 0);
    result <<= 16;

/*****  Collect the output of the filter process ...  *****/
    result |= CopyFileToOutputBuffer (rOutputGenerator, outputFileName);

    unlink (inputFileName);
    unlink (outputFileName);

    return result;
}
#endif


PrivateFnDef int FilterOutput2 (
    VOutputGenerator&		rOutputGenerator,
    char const*			pOutput,
    char const*			pCommandLine,
    int				Unused(openFlags),
    int				Unused(unlinkOnError)
)
{
    int result = 512;  //  copying Solaris behavior (7/98) 

#if 0
    VkStatus iStatus;
    VkProcessData iProcessData;
    VkStream *pOutputStream = NilOf (VkStream*);
    VkStream *pInputStream  = NilOf (VkStream*);
    if (iProcessData.construct (iStatus, pCommandLine, &pOutputStream, &pInputStream)) {
    //  Create windows, ...
	VkStreamReader *pReader = new VkStreamReader (pInputStream);
	VkStreamWriter *pWriter = new VkStreamWriter (pOutputStream);

    //  ... transfer data, ...
	VkEventSet  iEventSet;
	bool	    fOutputToSend = true;

	do {
	    iEventSet.clear ();

	    /*****  Receive input...  *****/
	    VkStatus iStatus;
	    char *pInputString; size_t sInputString;
	    while (
		pReader->GetString (1, 0, &pInputString, &sInputString, &iStatus)
	    ) {
		rOutputGenerator.putString  (pInputString);
		pReader->deallocate (pInputString);
	    }
	    switch (iStatus.Type ()) {
	    case VkStatusType_Closed:
	    case VkStatusType_Failed:
		if (fOutputToSend) {
		    pWriter->EndTransmission (&iStatus);
		    fOutputToSend = false;
		}
		break;

	    default:
		iEventSet.Add (pReader->Event ());
		break;
	    }

	    if (fOutputToSend) {
		while (
		    '\377' == (
			pOutput += pWriter->PutData (
			    strcspn (pOutput, "\377"), pOutput, &iStatus
			)
		    )[0]
		) pOutput++;
		if ('\0' == *pOutput && pWriter->PutBufferedData (&iStatus)) {
		    pWriter->EndTransmission (&iStatus);
		    fOutputToSend = false;
		}
		else
		    iEventSet.Add (pWriter->Event ());
	    }

	} while (IsntNil (iEventSet.getNextTriggeredEvent (Vca_InfiniteWait)));

    //  ... delete windows, ...
	delete pReader;
	delete pWriter;

    //  ... and clean-up process:
	iProcessData.waitForTermination (&result);
    }
#endif

    return result << 16;
}


/**********************************
 *  Output Director Continuation  *
 **********************************/

V_DefinePrimitive (OutputDirectorContinuation) {
#define handleString(stringIndex) {\
    *errorCodeCursor++ = handler (\
	iOutputGenerator,\
	outputString,\
	pStrings[stringIndex],\
	handlerOpenFlags,\
	false\
    );\
    if (IsntNil (outputString)) outputString += strlen (outputString) + 1;\
}

    char* outputString = NULL;
    M_CPD* outputStrings;

    DSC_Descriptor& rCurrent = pTask->getCurrent ();

/*****  Access the output to be processed, ...  *****/
    switch (V_TOTSC_Primitive) {
    default:
	if (rCurrent.isScalar ())
	    break;

    case XFilterOutput:
    case XFilterOutput2:
	outputStrings = pTask->NewCharUV (pTask->outputBufferCharacterCount ('\0'));
	outputString = rtCHARUV_CPD_Array (outputStrings);
	pTask->popOutputDiversion (outputString, '\0');
	break;
    }

/*****  ... determine the output processor to apply, ...  *****/
    int handlerOpenFlags;
    int (*handler) (VOutputGenerator&, char const*, char const*, int, int);
    switch (V_TOTSC_Primitive) {
    case XFilterOutput:
	handler			= FilterOutput1;
	handlerOpenFlags	= 0;
	break;
    case XFilterOutput2:
	handler			= FilterOutput2;
	handlerOpenFlags	= 0;
	break;
    case XWriteOutput:
	handler			= CopyOutputToFile;
	handlerOpenFlags	= O_WRONLY | O_CREAT | O_TRUNC;
	break;
    case XAppendOutput:
	handler			= CopyOutputToFile;
	handlerOpenFlags	= O_WRONLY | O_CREAT | O_APPEND;
	break;
    default:
	pTask->raiseUnimplementedAliasException ("OutputDirector");
	break;
    }

/*****  ... process the command strings, ...  *****/
    rtBLOCK_Handle::Strings pBlockStrings;
    rtLSTORE_Handle::Strings pLStoreStrings;

    VOutputGenerator iOutputGenerator (pTask);
    if (rCurrent.isScalar ()) {
	int errorCode;
	int* errorCodeCursor = &errorCode;

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    handleString (DSC_Descriptor_Scalar_Int (rCurrent));
#undef pStrings
	}

	pTask->loadDucWithInteger (errorCode);
    }
    else {
	M_CPD* errorCodes = pTask->NewIntUV ();
	int *errorCodeCursor = rtINTUV_CPD_Array (errorCodes);

	if (pBlockStrings.setTo (rCurrent.store ())) {
#define pStrings pBlockStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}
	else if (pLStoreStrings.setTo (rCurrent.store ())) {
#define pStrings pLStoreStrings
	    DSC_Traverse (rCurrent, handleString);
#undef pStrings
	}

	pTask->loadDucWithMonotype (errorCodes);
	errorCodes->release ();
    }

/*****  ... cleanup, ...  *****/
    switch (V_TOTSC_Primitive) {
    default:
	if (rCurrent.isScalar ())
	    break;
    case XFilterOutput:
    case XFilterOutput2:
	outputStrings->release ();
	break;
    }
/*****  ... and return.  *****/

#undef handleString
}


/*******************************
 *  Output Director Primitive  *
 *******************************/

V_DefinePrimitive (OutputDirector) {
    if (pTask->parameterCount () < 1) pTask->raiseException (
	EC__UsageError, "Output Redirector Requires At Least 1 Parameter"
    );

    pTask->setContinuationTo (OutputDirectorContinuation);
    pTask->pushOutputDiversion ();

    pTask->beginValueCall	(pTask->parameterCount () - 1);
    pTask->loadDucWithNextParameter ();
    pTask->commitRecipient	();

    pTask->commitParameters	();

    pTask->commitCall		();
}


/********************************
 *  Output Diversion Primitive  *
 ********************************/

V_DefinePrimitive (DivertOutputContinuation) {
    char outputDelimiter = V_TOTSC_Primitive == XDivertDelimitedOutput
	? '\377'	/* (char)255 */
	: '\0';

    M_CPD* charuv = pTask->NewCharUV (pTask->outputBufferCharacterCount (outputDelimiter));

    pTask->popOutputDiversion (rtCHARUV_CPD_Array (charuv), outputDelimiter);

    pTask->loadDucWithListOrStringStore (rtLSTORE_NewStringStoreFromUV (charuv));
    charuv->release ();
}

V_DefinePrimitive (DivertOutput) {
    pTask->setContinuationTo (DivertOutputContinuation);
    pTask->pushOutputDiversion ();

    pTask->beginValueCall	(pTask->parameterCount ());
    pTask->commitRecipient	(VMagicWord_Current);

    pTask->commitParameters	();

    pTask->commitCall		();
}


/*****************************
 *  Echo to File Primitives  *
 *****************************/

PrivateFnDef int EchoToFileErrorHandler (ERR_ErrorDescription *errorDescription) {
    ERR_ReportErrorOnStderr ("", errorDescription);

    return 0;
}


V_DefinePrimitive (EchoToFile) {
/*****  Obtain the filename string ...  *****/
    pTask->loadDucWithNextParameter ();
    pTask->normalizeDuc ();

/*****  If its non-scalar then error *****/
    if (!DucIsAScalar) {
	ERR_PushErrorHandler (EC__UsageError, EchoToFileErrorHandler);
	ERR_SignalWarning (
	    EC__UsageError, "EchoToFile: non scalar case is not implemented"
	);
	ERR_PopErrorHandler (EC__UsageError);
	pTask->loadDucWithSelf ();
	return;
    }

/*****  Determine the output filename ... *****/
    char const* pFileName = 0;
    switch (ADescriptor.pointerRType ()) {
    case RTYPE_C_IntUV: {
	    rtBLOCK_Handle::Strings pStrings;
	    if (pStrings.setTo (pTask->ducStore ()))
		pFileName = pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)];
	}
	break;
    case RTYPE_C_Link: {
	    rtLSTORE_Handle::Strings pStrings;
	    if (pStrings.setTo (pTask->ducStore ()))
		pFileName = pStrings[DSC_Descriptor_Scalar_Int (ADescriptor)];
	}
	break;
    }
    if (pFileName)
	IO_EchoToFile (pFileName);
    else {
	ERR_PushErrorHandler (EC__UsageError, EchoToFileErrorHandler);
	ERR_SignalWarning (EC__UsageError, "EchoToFile: argument must be a string");
	ERR_PopErrorHandler (EC__UsageError);
    }
    pTask->loadDucWithSelf ();
}


V_DefinePrimitive (StopEchoToFile) {
    IO_StopEchoToFile ();

    pTask->loadDucWithSelf ();
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


/******************************
 *****  Input Processing  *****
 ******************************/

    PD (IOMDriver::XReadEvalPrintController,
	"ReadEvalPrintController",
	VPrimitiveDescriptor::IClass_ReadEvalPrintController)

/*******************************
 *****  Execution Control  *****
 *******************************/

    PD (XMutex,
	"Mutex",
	VPrimitiveDescriptor::IClass_MutexController)
    PD (XRelinquishTimeSlice,
	"RelinquishTimeSlice",
	RelinquishTimeSlice)

/*******************
 *****  Magic  *****
 *******************/

    PD (XBlockMagic,
	"BlockMagic",
	BlockMagic)
    PD (XGroundMagic,
	"GroundMagic",
	GroundMagic)

/********************************************
 *****  Process Information Primitives  *****
 ********************************************/

    PD (XGetProcessString,
	"GetProcessString", 
	ProcessInformation)

    PD (XActiveDriver,
	"ActiveDriver", 
	ProcessInformation)

    PD (XGetProcessInteger,
	"GetProcessInteger", 
	ProcessInformation)

    PD (XAccess,
	"Access", 
	Access)


/***********************************************
 *****  I/O Driver Description Primitives  *****
 ***********************************************/

    PD (XIOMStateDescription,
	"IOMStateDescription",
	IOMDescription)
    PD (XVkStatusTypeDescription,
	"VkStatusTypeDescription",
	IOMDescription)
    PD (XVkStatusCodeDescription,
	"VkStatusCodeDescription",
	IOMDescription)
    PD (XIOMChannelTypeDescription,
	"IOMChannelTypeDescription",
	IOMDescription)
    PD (XIOMOptionDescription,
	"IOMOptionDescription",
	IOMDescription)
    PD (XIOMPromptFormatDescription,
	"IOMPromptFormatDescription",
	IOMDescription)
    PD (XIOMBinaryFormatDescription,
	"IOMBinaryFormatDescription",
	IOMDescription)
    PD (XIOMTrimFormatDescription,
	"IOMTrimFormatDescription",
	IOMDescription)


/********************************************
 *****  I/O Driver Lifetime Primitives  *****
 ********************************************/

    PD (XCreate,
	"Create",
	Create)
    PD (XAccept,
	"Accept",
	Accept)

    PD (XWait,
        "Wait",
	DriverHIHWaitPrimitive)

    PD (XEndTransmission,
	"EndTransmission", 
	DriverHIPrimitive)
    PD (XEndReception,
	"EndReception", 
	DriverHIPrimitive)

    PD (XClose,
	"Close",
	DriverHIPrimitive)

/*****************************************
 *****  I/O Driver Query Primitives  *****
 *****************************************/

    PD (XGetIndex,
	"GetIndex",
	DriverHIPrimitive)
    PD (XGetState,
	"GetState",
	DriverHIPrimitive)
    PD (XGetStatus,
	"GetStatus",
	DriverHIPrimitive)
    PD (XGetError,
	"GetError",
	DriverHIPrimitive)

    PD (XGetHandler,
	"GetHandler", 
	GetHandler)

    PD (XGetErrorOutput, 
        "GetErrorOutput",
	DriverHHPrimitive)
    PD (XGetErrorInput,
	"GetErrorOutput",
	DriverHHPrimitive)

    PD (XGetOptionIndex,
	"GetOptionIndex",
	DriverHIPrimitive)
    PD (XGetOptionValue,
	"GetOptionValue",
	DriverHIPrimitive)
    PD (XGetSeekOffset,
	"GetSeekOffset",
	DriverHIPrimitive)

    PD (XGetChannelType,
	"GetChannelType",
	DriverHIPrimitive)

    PD (XGetSocketName,
	"GetSocketName",
	GetName)
    PD (XGetPeerName,
	"GetPeerName",
	GetName)


/*******************************************
 *****  I/O Driver Control Primitives  *****
 *******************************************/

    PD (XRetain,
	"Retain",
	DriverHIPrimitive)
    PD (XRelease,
	"Release",
	DriverHIPrimitive)

    PD (XEnableHandler,
	"EnableHandler",
	DriverHIPrimitive)
    PD (XDisableHandler,
	"DisableHandler",
	DriverHIPrimitive)

    PD (XSetHandler,
	"SetHandler",
	SetHandler)

    PD (XSetErrorOutput,
	"SetErrorOutput",
	DriverHHHPrimitive)
    PD (XSetErrorInput,
	"SetErrorInput",
	DriverHHHPrimitive)

    PD (XSetOptionIndex,
	"SetOptionIndex",
	DriverHIHPrimitive)
    PD (XSetOptionValue,
	"SetOptionValue",
	DriverHIHPrimitive)
    PD (XSetSeekOffset,
	"SetSeekOffset",
	DriverHIHPrimitive)


/*****************************************
 *****  I/O Driver Input Primitives  *****
 *****************************************/

    PD (XGetByteCount,
	"GetByteCount", 
	DriverHDPrimitive)

    PD (XGetSignedByte,
	"GetSignedByte",
	GetNumber)
    PD (XGetUnsignedByte,
	"GetUnsignedByte",
	GetNumber)
    PD (XGetSignedShort,
	"GetSignedShort",
	GetNumber)
    PD (XGetUnsignedShort,
	"GetUnsignedShort",
	GetNumber)
    PD (XGetLong,
	"GetLong",
	GetNumber)
    PD (XGetFloat,
	"GetFloat",
	GetNumber)
    PD (XGetDouble,
	"GetDouble",
	GetNumber)
    PD (XGetString,
	"GetString",
	GetString)
    PD (XGetFixedString, 
        "GetFixedString",
	GetFixedString)
    PD (XGetLine, 
        "GetLine", 
	GetLine)


/******************************************
 *****  I/O Driver Output Primitives  *****
 ******************************************/

    PD (XFlushOutput,
	"FlushOutput", 
	DriverHIPrimitive)

    PD (XPutByte,
	"PutByte",
	PutNumber)
    PD (XPutShort,
	"PutShort",
	PutNumber)
    PD (XPutLong,
	"PutLong",
	PutNumber)
    PD (XPutFloat,
	"PutFloat",
	PutNumber)
    PD (XPutDouble,
	"PutDouble",
	PutNumber)

    PD (XPutString,
	"PutString",
	PutString)


/******************************************
 *****  Output Management Primitives  *****
 ******************************************/

    PD (XFilterOutput,
	"FilterOutput",
	OutputDirector)
    PD (XFilterOutput2,
	"FilterOutput2",
	OutputDirector)
    PD (XWriteOutput,
	"WriteOutput",
	OutputDirector)
    PD (XAppendOutput,
	"AppendOutput",
	OutputDirector)
    PD (XDivertDelimitedOutput,
	"DivertDelimitedOutput",
	DivertOutput)
    PD (XDivertOutput,
	"DivertOutput",
	DivertOutput)

    PD (XEchoToFile,
	"EchoToFile",
	EchoToFile)
    PD (XStopEchoToFile,
	"StopEchoToFile",
	StopEchoToFile)

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
PublicFnDef void pfRUN_InitDispatchVector () {
    VPrimitiveTask::InstallPrimitives (PrimitiveDescriptions, PrimitiveDescriptionCount);
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (pfRUN);
    FAC_FDFCase_FacilityDescription ("Run Time Support Primitives");

    case FAC_DoShutdownCleanup:
	FAC_CompletionCodeField = FAC_RequestSucceeded;
	break;

    default:
	break;
    }
}







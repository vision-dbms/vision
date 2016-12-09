/*****  Vision Kernel Status Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#if defined(__VMS)
#include <descrip.h>
#include <ssdef.h>
#include <starlet.h>
#include <stsdef.h>

#ifndef $DESCRIPTOR64
#define $DESCRIPTOR64 $DESCRIPTOR
#endif

#endif

/******************
 *****  Self  *****
 ******************/

#include "VkStatus.h"

#if defined(__VMS)
#undef VkStatusCode_Invalid
#undef VkStatusCode_Success

#define VkStatusCode_Invalid SS$_BADPARAM
#define VkStatusCode_Success SS$_NORMAL
#endif

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"


/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

/******************
 *****  Type  *****
 ******************/

char const *VkStatus::DescriptionFor (VkStatusType xType) {
    switch (xType) {
    case VkStatusType_Completed:
	return "Completed";
    case VkStatusType_Failed:
	return "Failed";
    case VkStatusType_Blocked:
	return "Blocked";
    case VkStatusType_Closed:
	return "Closed";
    case VkStatusType_Unimplemented:
	return "Unimplemented";
    default:
	return "???";
    }
}


/******************
 *****  Code  *****
 ******************/

/***********************************************************************
 *>>>>  This routine is not thread safe - use 'getDescription'!!!  <<<<*
 ***********************************************************************/
char const *VkStatus::DescriptionFor (VkStatusCode xCode) {
    VkStatus iStatus;
    iStatus.MakeErrorStatus (xCode);

    static VString iMessage;	//  ... bad!!!
    iStatus.getDescription(iMessage);
    return iMessage;
}

void VkStatus::getDescription (VString &rMessage) const {
#if defined(__VMS)
    unsigned short sMessage;
    char iMessage[257];
    $DESCRIPTOR64 (iMessageDesc, iMessage);
    unsigned char iExtra[4] = {0};
    if ($VMS_STATUS_SUCCESS (sys$getmsg (m_xCode, &sMessage, &iMessageDesc, 0xf, iExtra))) {
	rMessage.setTo (iMessage, sMessage);
    }
#elif defined(_WIN32)
    char iMessage[257];
    if (FormatMessage (
	   /*FORMAT_MESSAGE_ALLOCATE_BUFFER |*/FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	   0, m_xCode, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)iMessage, sizeof (iMessage), 0
       )
    ) {
	rMessage.setTo (iMessage);
    }
#else
    char const *pMessage = strerror (m_xCode);
    if (pMessage) {
	rMessage.setToStatic (pMessage);
    }

#endif
    else {
	rMessage.clear ();
	rMessage.printf ("Error %u (%08X)\n", m_xCode, m_xCode);
    }
}


/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

bool VkStatus::MakeBlockedStatus () {
    m_xType = VkStatusType_Blocked;
#if defined(__VMS)
    m_xCode = SS$_SUSPENDED;
#elif defined(_WIN32)
    m_xCode = ERROR_IO_PENDING;
#else
    m_xCode = EWOULDBLOCK;
#endif
    return true;	// ... blocked is a form of 'normal' completion
}

bool VkStatus::MakeClosedStatus () {
    m_xType = VkStatusType_Closed;
#if defined(__VMS)
    m_xCode = SS$_ENDOFFILE;
#elif defined(_WIN32)
    m_xCode = ERROR_HANDLE_EOF;
#else
    m_xCode = EPIPE;
#endif
    return false;
}

bool VkStatus::MakeErrorStatus () {
#if defined(__VMS)
    switch (errno) {
    case 0:
	return MakeSuccessStatus ();
    case EWOULDBLOCK:
	return MakeBlockedStatus ();
    case EPIPE:
	return MakeClosedStatus ();
    case EVMSERR:
	return MakeErrorStatus (vaxc$errno);
    default:
	return MakeFailureStatus ();
    }
#elif defined(_WIN32)
    return MakeErrorStatus (GetLastError ());
#else
    return MakeErrorStatus (errno);
#endif
}

bool VkStatus::MakeErrorStatus (VkStatusCode xCode, VkStatusType xSuccessType) {
    m_xCode = xCode;
#if defined(__VMS)
    m_xType = $VMS_STATUS_SUCCESS (xCode) ? xSuccessType : VkStatusType_Failed;
#elif defined(_WIN32)
    switch (xCode) {
    case NO_ERROR:
	m_xType = xSuccessType;
	break;
    case ERROR_IO_PENDING:
    case ERROR_IO_INCOMPLETE:
	m_xType = VkStatusType_Blocked;
	break;
    case ERROR_BROKEN_PIPE:
    case ERROR_HANDLE_EOF:
    case ERROR_PIPE_NOT_CONNECTED:
	m_xType = VkStatusType_Closed;
	break;
    default:
	m_xType = VkStatusType_Failed;
	break;
    }
#else
    switch (xCode) {
    case 0:
	m_xType = xSuccessType;
	break;
    case EWOULDBLOCK:
	m_xType = VkStatusType_Blocked;
	break;
    case EPIPE:
    case ECONNRESET:
	m_xType = VkStatusType_Closed;
	break;
    default:
	m_xType = VkStatusType_Failed;
	break;
    }
#endif
    return isCompleted () || isBlocked ();
}

bool VkStatus::MakeFailureStatus () {
    m_xType = VkStatusType_Failed;
    m_xCode = VkStatusCode_Invalid;
    return false;
}

bool VkStatus::MakeSuccessStatus () {
    m_xType = VkStatusType_Completed;
    m_xCode = VkStatusCode_Success;
    return true;
}

bool VkStatus::MakeUnimplementedStatus () {
    m_xType = VkStatusType_Unimplemented;
    m_xCode = VkStatusCode_Invalid;
    return false;
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void VkStatus::setTo (VkStatus const &rStatus) {
    m_xType = rStatus.Type ();
    m_xCode = rStatus.Code ();
}

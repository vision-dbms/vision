/*****  Vision Kernel Stream Implementation  *****/

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

#include "VkStream.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkEvent.h"
#include "VkSocketAddress.h"
#include "VkStatus.h"

#if defined(_WIN32)
#pragma comment (lib, "ws2_32.lib")
#endif


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VkStream);


/*******************************
 *******************************
 *****  Module Management  *****
 *******************************
 *******************************/

/*********************
 *****  Globals  *****
 *********************/

#if defined(_WIN32)

#ifndef SD_RECEIVE
#define SD_RECEIVE 0
#endif

#ifndef SD_SEND
#define SD_SEND 1
#endif

#endif


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VkStream::VkStream (
    HANDLE hStream, VkStreamType xType, VkStreamMode xMode
) : m_hStream (hStream), m_xType (xType), m_xMode (xMode) {
/*****
 *  Stream handles must be made private this process (i.e., not-inheritable)
 *  to keep the children creating by this process from inheriting handles to
 *  its open sockets, pipes, and files.  Many bad things will happen (rotten
 *  kids) if this principle is not respected.
 *****/
#if defined (_WIN32)
#else
    fcntl (hStream, F_SETFD, FD_CLOEXEC);
#endif
    traceInfo ("Creating VkStream");
}


/**********************************
 **********************************
 *****  Construction Helpers  *****
 **********************************
 **********************************/

void VkStream::GetHandleTypeAndMode (
    HANDLE hStream, VkStreamType &rxType, VkStreamMode &rxMode
) {
#if defined(_WIN32)
    switch (GetFileType (hStream)) {
    case FILE_TYPE_DISK:
	rxType	= VkStreamType_File;
	rxMode	= VkStreamMode_Synchronous;
	break;

    case FILE_TYPE_CHAR:
	rxType	= VkStreamType_Console;
	rxMode	= VkStreamMode_Threaded;
	break;

    case FILE_TYPE_PIPE:
	rxType	= VkStreamType_Pipe;
	rxMode	= VkStreamMode_Threaded;
	break;

    default:
	rxType	= VkStreamType_Unknown;
	rxMode	= VkStreamMode_Threaded;
	break;
    }
#else
    rxType = VkStreamType_Unknown;
    rxMode = VkStreamMode_Threaded;
#endif
}


VkStream *VkStream::New (HANDLE hStream) {
    VkStreamType xType; VkStreamMode xMode;
    GetHandleTypeAndMode (hStream, xType, xMode);
    return new VkStream (hStream, xType, xMode);
}

VkStream *VkStream::New (VkStandardHandle xHandle) {
    HANDLE hStream;

#if defined(_WIN32)
    switch (xHandle) {
    case VkStandardHandle_Input:
	hStream = GetStdHandle (STD_INPUT_HANDLE);
	break;
    case VkStandardHandle_Output:
	hStream = GetStdHandle (STD_OUTPUT_HANDLE);
	break;
    case VkStandardHandle_Error:
	hStream = GetStdHandle (STD_ERROR_HANDLE);
	break;
    default:
	SetLastError (ERROR_INVALID_HANDLE);
	hStream = INVALID_HANDLE_VALUE;
	break;
    }
    if (FileHandleIsInvalid (hStream))
	return NilOf (VkStream*);

#else
    if (xHandle < VkStandardHandle_Input || xHandle > VkStandardHandle_Error) {
	errno = EINVAL;
	return NilOf (VkStream*);
    }
    hStream = xHandle;
#endif

    return VkStream::New (hStream);
}


VkStream *VkStream::New (char const *pFileName) {
#if defined(_WIN32)
    HANDLE hFile = CreateFile (
	pFileName,
	GENERIC_READ,
	FILE_SHARE_READ,
	NULL,
	OPEN_EXISTING,
	FILE_FLAG_SEQUENTIAL_SCAN,	/*  Not efficient for random access  */
	NULL
    );
#else
    HANDLE hFile = open (pFileName, O_RDONLY);
#endif

    return FileHandleIsValid (hFile) ? VkStream::New (hFile) : NilOf (VkStream*);
}


VkStream *VkStream::New (SOCKET hSocket, bool fPassive, bool fNoDelay) {
    if (SocketHandleIsInvalid (hSocket))
	return NilOf (VkStream*);

    VkStream *pStream = new VkStream (
	(HANDLE)hSocket,
	VkStreamType_Socket,
	fPassive ? VkStreamMode_Threaded : VkStreamMode_Overlapped
    );

    if (fNoDelay) {
	VkStatus iStatus;
	pStream->SetTcpNodelay (&iStatus, fNoDelay);
    }

    return pStream;
}


VkStream *VkStream::New (
    VkSocketAddress const*	pAddress,
    int				xType,
    int				xProtocol,
    bool			bBindOnly,
    bool			fPassive,
    bool			fNoDelay,
    VkStatus*			pStatusReturn
)
{
/*****  ... create the socket, ...  *****/
    SOCKET hSocket = socket (pAddress->family (), xType, xProtocol);
    if (SocketHandleIsInvalid (hSocket)) {
	pStatusReturn->MakeErrorStatus ();
	return NilOf (VkStream*);
    }

/*****  ... bind or connect it, ...  *****/
    if (bBindOnly ? pAddress->bind (hSocket) < 0 : pAddress->connect (hSocket) < 0) {
	pStatusReturn->MakeErrorStatus ();
	return NilOf (VkStream*);
    }

/*****  ... note the creation of a unix domain socket, ...  *****/
    bool bUnixDomainBinding = bBindOnly && pAddress->familyIsUnix ();

/*****  ... enable listening on passive sockets, ...  *****/
    if (fPassive && listen (hSocket, 5) < 0) {
	pStatusReturn->MakeErrorStatus ();
#if defined(_WIN32)
	closesocket (hSocket);
#else
	close (hSocket);
#endif
	if (bUnixDomainBinding)
	    unlink (pAddress->unixPathName ());

	return NilOf (VkStream*);
    }

/*****  ...  create a stream for the socket, ...  *****/
    VkStream *pStream = VkStream::New (hSocket, fPassive, fNoDelay);
    if (IsNil (pStream)) {
	pStatusReturn->MakeErrorStatus ();
#if defined(_WIN32)
	closesocket (hSocket);
#else
	close (hSocket);
#endif
	if (bUnixDomainBinding)
	    unlink (pAddress->unixPathName ());

	return NilOf (VkStream*);
    }

/*****  ... and return the new stream.  *****/
    return pStream;
}


VkStream *VkStream::New (
    char const *pProtocolString,
    char const *pAddressString,
    bool	fBindOnly,
    bool	fPassive,
    bool	fNoDelay,
    VkStatus	*pStatusReturn
)
{
    if (fPassive)
	fBindOnly = true;

    VkSocketAddress iAddress; int xType, xProtocol;

    return iAddress.construct (
	pProtocolString, pAddressString, fBindOnly, &xType, &xProtocol, pStatusReturn
    ) ? VkStream::New (
	&iAddress, xType, xProtocol, fBindOnly, fPassive, fNoDelay, pStatusReturn
    ) : NilOf (VkStream*);
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VkStream::~VkStream () {
    traceInfo ("Destroying VkStream");

    if (FileHandleIsValid (m_hStream))
#if defined(_WIN32)
	CloseHandle (m_hStream);
#else
	close (m_hStream);
#endif
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

size_t VkStream::GetByteCount (VkStatus *pStatusReturn){
    size_t bytesAvailable;

#if defined(_WIN32)
    size_t sFile, oFile;

    switch (m_xType) {
    case VkStreamType_File:
	if (0xffffffff == (sFile = GetFileSize (m_hStream, NULL))
	||  0xffffffff == (oFile = SetFilePointer (m_hStream, 0, NULL, FILE_CURRENT))
	)
	{
	    pStatusReturn->MakeErrorStatus ();
	    bytesAvailable = 0;
	}
	else
	    bytesAvailable = sFile - oFile;
	break;

    case VkStreamType_Socket:
	if (ioctlsocket ((SOCKET)m_hStream, FIONREAD, (u_long*)&bytesAvailable) < 0) {
	    pStatusReturn->MakeErrorStatus ();
	    bytesAvailable = 0;
	}
	break;

//  case VkStreamType_Pipe:
//	if (!PeekNamedPipe (m_hStream, NULL, 0, NULL, &bytesAvailable, NULL)) {
//	    pStatusReturn->MakeErrorStatus ();
//	    bytesAvailable = 0;
//	}
//	break;

    default:
	bytesAvailable = 0;
	break;
    }
#elif defined(sun)
    if (ioctl (m_hStream, I_NREAD, &bytesAvailable) < 0) {
	pStatusReturn->MakeErrorStatus ();
	bytesAvailable = 0;
    }

#else
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
    if (ioctl (m_hStream, FIONREAD, &bytesAvailable) < 0) {
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	pStatusReturn->MakeErrorStatus ();
	bytesAvailable = 0;
    }
#endif

    return bytesAvailable;
}


SOCKET VkStream::GetSocketHandle () const {
    return this && VkStreamType_Socket == m_xType ? (SOCKET)m_hStream : INVALID_SOCKET;
}

int VkStream::GetSocketName (
    VkStatus *pStatusReturn, char **ppString, size_t *psString
) const {
    VkSocketAddress iAddress;

    if (iAddress.GetSocketName (GetSocketHandle (), pStatusReturn) < 0)
	return -1;

    if (iAddress.Format (ppString, psString) < 0) {
	pStatusReturn->MakeFailureStatus ();
	return -1;
    }

    return 0;
}

int VkStream::GetSocketPeerName (
    VkStatus *pStatusReturn, char **ppString, size_t *psString
) const {
    VkSocketAddress iAddress;

    if (iAddress.GetPeerName (GetSocketHandle (), pStatusReturn) < 0)
	return -1;

    if (iAddress.Format (ppString, psString) < 0) {
	pStatusReturn->MakeFailureStatus ();
	return -1;
    }

    return 0;
}


int VkStream::GetTcpNodelay (VkStatus *pStatusReturn, int *fOnOff) const {
    if (SocketHandleIsValid (GetSocketHandle ())) {
	int answer; socklen_t answerSize = sizeof (answer);
	if (0 > getsockopt (GetSocketHandle (), IPPROTO_TCP, TCP_NODELAY, (char*)&answer, &answerSize)) {
	    pStatusReturn->MakeErrorStatus ();
	    *fOnOff = false;
	    return -1;
	}
	*fOnOff = answer ? true : false;
    }
    else {
	*fOnOff = false;
    }
    return 0;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

int VkStream::EndTransmission (VkStatus *pStatusReturn) {
    int result = 0;
#if defined(_WIN32)
    switch (m_xType) {
    case VkStreamType_Socket:
	if (shutdown ((SOCKET)m_hStream, SD_SEND) < 0) {
	    pStatusReturn->MakeErrorStatus ();
	    result = -1;
	}
	break;
    case VkStreamType_Pipe:
	if (!CloseHandle (m_hStream)) {
	    pStatusReturn->MakeErrorStatus ();
	    result = -1;
	}
	m_hStream = INVALID_HANDLE_VALUE;
	break;
    default:
	pStatusReturn->MakeFailureStatus ();
	result = -1;
	break;
    }

#else
    if (shutdown (m_hStream, SHUT_WR) < 0) {
	pStatusReturn->MakeErrorStatus ();
	result = -1;
    }
#endif

    return result;
}

int VkStream::EndReception (VkStatus *pStatusReturn) {
    int result = 0;
#if defined(_WIN32)
    switch (m_xType) {
    case VkStreamType_Socket:
	if (shutdown ((SOCKET)m_hStream, SD_RECEIVE) < 0) {
	    pStatusReturn->MakeErrorStatus ();
	    result = -1;
	}
	break;
    case VkStreamType_Pipe:
	if (!CloseHandle (m_hStream)) {
	    pStatusReturn->MakeErrorStatus ();
	    result = -1;
	}
	m_hStream = INVALID_HANDLE_VALUE;
	break;
    default:
	pStatusReturn->MakeFailureStatus ();
	result = -1;
	break;
    }

#else
    if (shutdown (m_hStream, SHUT_RD) < 0) {
	pStatusReturn->MakeErrorStatus ();
	result = -1;
    }
#endif

    return result;
}


int VkStream::SetTcpNodelay (VkStatus *pStatusReturn, int fOnOff) const {
    int nodelay = fOnOff ? true : false;

//  Some systems (i.e., windows) want a 'char*':
    if (0 > setsockopt (
	    GetSocketHandle (), IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof (nodelay)
	)
    ) {
	pStatusReturn->MakeErrorStatus ();
	return -1;
    }

    return 0;
}

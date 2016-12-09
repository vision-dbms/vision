/*****  Vision Kernel Stream Interface  *****/
#ifndef VkStream_Interface
#define VkStream_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkSocketAddress;
class VkStatus;

/*************************
 *****  Definitions  *****
 *************************/

/******************************
 *----  VkStandardHandle  ----*
 ******************************/

enum VkStandardHandle {
    VkStandardHandle_Input,
    VkStandardHandle_Output,
    VkStandardHandle_Error
};


/********************
 *----  Stream  ----*
 ********************/

enum VkStreamMode {
    VkStreamMode_Synchronous,
    VkStreamMode_Overlapped,
    VkStreamMode_Threaded
};

enum VkStreamType {
    VkStreamType_File,
    VkStreamType_Socket,
    VkStreamType_Pipe,
    VkStreamType_Console,
    VkStreamType_Unknown
};

class VkStream : public VReferenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VkStream, VReferenceable);

//  Friends
    friend class VkStreamAgent;

//  Construction
protected:
    VkStream (HANDLE hStream, VkStreamType xType, VkStreamMode xMode);

    static void GetHandleTypeAndMode (
	HANDLE hStream, VkStreamType &rxType, VkStreamMode &rxMode
    );

public:
    static VkStream *New (HANDLE hStream);
    static VkStream *New (VkStandardHandle xHandle);
    static VkStream *New (char const *pFileName);
    static VkStream *New (SOCKET hSocket, bool fPassive, bool fNoDelay);
    static VkStream *New (
	VkSocketAddress const*	pAddress,
	int		xType,
	int		xProtocol,
	bool		fBindOnly,
	bool		fPassive,
	bool		fNoDelay,
	VkStatus*	pStatusReturn
    );
    static VkStream *New (
	char const*	pProtocolString,
	char const*	pAddressString,
	bool		fBindOnly,
	bool		fPassive,
	bool		fNoDelay,
	VkStatus*	pStatusReturn
    );

//  Destruction
protected:
    ~VkStream ();

//  Access
public:
    VkStreamType Type () const {
	return m_xType;
    }
    VkStreamMode Mode () const {
	return m_xMode;
    }

//  Query
public:
    size_t GetByteCount (VkStatus *pStatusReturn);

    SOCKET GetSocketHandle () const;

    int GetSocketName (
	VkStatus *pStatusReturn, char **ppString, size_t *psString
    ) const;

    int GetSocketPeerName (
	VkStatus *pStatusReturn, char **ppString, size_t *psString
    ) const;

    int GetTcpNodelay (VkStatus *pStatusReturn, int *fOnOff) const;

//  Control
public:
    int EndTransmission (VkStatus *pStatusReturn);

    int EndReception (VkStatus *pStatusReturn);

    int SetTcpNodelay (VkStatus *pStatusReturn, int fOnOff) const;

//  State
protected:
    HANDLE		m_hStream;
    VkStreamType const	m_xType;
    VkStreamMode const	m_xMode;
};


#endif

#ifndef VkStreamAgent_Interface
#define VkStreamAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkStatus.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VkEvent.h"

#include "VkStream.h"

#include "VpSocket.h"

/*************************
 *****  Definitions  *****
 *************************/

/***************************
 *----  VkStreamAgent  ----*
 ***************************/

class VkStreamAgent : public virtual VTransient {
    DECLARE_FAMILY_MEMBERS (VkStreamAgent, VTransient);

    friend class VkStreamThread;

//  Construction
protected:
    VkStreamAgent (VkStream *pStream);

//  Destruction
public:
    void Destroy ();

protected:
    ~VkStreamAgent ();

//  Access
public:
    VkEvent const *Event () const {
	return &m_iEvent;
    }
    VkEvent *Event () {
	return &m_iEvent;
    }

    int GetSocketName (
	VkStatus *pStatusReturn, char **ppString, size_t *psString
    ) const {
	return m_pStream
            ? m_pStream->GetSocketName (pStatusReturn, ppString, psString)
            : MakeFailureStatus (pStatusReturn);
    }

    int GetSocketPeerName (
	VkStatus *pStatusReturn, char **ppString, size_t *psString
    ) const {
	return m_pStream
            ? m_pStream->GetSocketPeerName (pStatusReturn, ppString, psString)
            : MakeFailureStatus (pStatusReturn);
    }

    int GetTcpNodelay (VkStatus *pStatusReturn, int *fOnOff) const {
	return m_pStream
            ? m_pStream->GetTcpNodelay (pStatusReturn, fOnOff)
            : MakeFailureStatus (pStatusReturn);
    }

    SOCKET SocketHandle () const {
	return m_pStream ? m_pStream->GetSocketHandle () : INVALID_SOCKET;
    }
    VkStream *Stream () const {
	return m_pStream;
    }

protected:
    static int MakeFailureStatus (VkStatus *pStatusReturn) {
        pStatusReturn->MakeFailureStatus ();
        return -1;
    }
    HANDLE StreamHandle () const {
	return m_pStream->m_hStream;
    }

    VkStreamMode StreamMode () const {
	return m_pStream->Mode ();
    }
    VkStreamType StreamType () const {
	return m_pStream->Type ();
    }

//  Control
public:
    void start (Vca::ITrigger *pEventHandler);

protected:
    bool StartOperation (VkEvent::State xEventState, VkStatus *pStatusReturn);

#   if defined(_WIN32)
protected:
    virtual bool CompleteOverlappedOperation () {
	return true;
    }
    void CompleteThreadedOperation ();
#   endif

protected:
    virtual void OnCompletion () = 0;

public:
    bool Complete (
	VkStatus *pStatusReturn, VkEvent::State xPartialSuccessState = VkEvent::State_Signaled
    );

    int EndReception (VkStatus *pStatusReturn) const {
	return m_pStream
            ? m_pStream->EndReception (pStatusReturn)
            : MakeFailureStatus (pStatusReturn);
    }
    int EndTransmission (VkStatus *pStatusReturn) const {
	return m_pStream
            ? m_pStream->EndTransmission (pStatusReturn)
            : MakeFailureStatus (pStatusReturn);
    }

    int SetTcpNodelay (VkStatus *pStatusReturn, int fOnOff) const {
	return m_pStream
            ? m_pStream->SetTcpNodelay (pStatusReturn, fOnOff)
            : MakeFailureStatus (pStatusReturn);
    }

//  State
protected:
    VkEvent			m_iEvent;
    VkEvent::HandlerHolder	m_iEventHH;
    VkStatus			m_iDeferredStatus;
    VReference<VkStream>	m_pStream;
    void*			m_pData;
    size_t			m_sData;
};


/******************************
 *----  VkStreamListener  ----*
 ******************************/

class VkStreamListener : public VkStreamAgent {
    DECLARE_FAMILY_MEMBERS (VkStreamListener, VkStreamAgent);

//  Construction
public:
    VkStreamListener (VkStream *pStream)
	: VkStreamAgent (pStream), m_hSocket (INVALID_SOCKET)
    {
    }

//  Destruction
public:
    void Destroy ();

//  Use
protected:
    void OnCompletion ();

public:
    VkStream *GetConnection (VkStatus *pStatusReturn);

//  State
protected:
    SOCKET m_hSocket;
};


/*******************************
 *----  VkStreamDataAgent  ----*
 *******************************/

class VkStreamDataAgent : public VkStreamAgent {
    DECLARE_FAMILY_MEMBERS (VkStreamDataAgent, VkStreamAgent);

//  Construction
protected:
    VkStreamDataAgent (VkStream *pStream);

//  Destruction
protected:

//  Control
protected:
#   if defined(_WIN32)
    bool RegisterOverlappedOperation (
	VkEvent::State xEventState, VkStatus *pStatusReturn
    );
    bool CompleteOverlappedOperation () {
	return GetOverlappedResult (
	    StreamHandle (), &m_iOverlapped, (LPDWORD)&m_sData, false
	) ? true : false;
    }
#   endif

//  State
protected:
#if defined(_WIN32)
    OVERLAPPED m_iOverlapped;
#endif
};


/************************************
 *----  VkStreamReceivingAgent  ----*
 ************************************/

class VkStreamReceivingAgent : public VkStreamDataAgent {
    DECLARE_FAMILY_MEMBERS (VkStreamReceivingAgent, VkStreamDataAgent);

//  Construction
protected:
    VkStreamReceivingAgent (VkStream *pStream) : VkStreamDataAgent (pStream) {
    }

//  Input
protected:
    bool StartRead (void *pData, size_t sData, VkStatus *pStatusReturn);

public:
    size_t GetByteCount (VkStatus *pStatusReturn) const {
	return m_pStream->GetByteCount (pStatusReturn);
    }

//  State
protected:
};


/***********************************
 *----  VkStreamShippingAgent  ----*
 ***********************************/

class VkStreamShippingAgent : public VkStreamDataAgent {
    DECLARE_FAMILY_MEMBERS (VkStreamShippingAgent, VkStreamDataAgent);

//  Construction
protected:
    VkStreamShippingAgent (VkStream *pStream) : VkStreamDataAgent (pStream) {
    }

//  Output
protected:
    bool StartWrite (void const *pData, size_t sData, VkStatus *pStatusReturn);

//  State
protected:
};


#endif

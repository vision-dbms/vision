/*****  Vision Kernel Message Interface  *****/
#ifndef VkRemote_Interface
#define VkRemote_Interface

/***********************
 ***********************
 *****  Components *****
 ***********************
 ***********************/

#include "VTransient.h"

#include "VkStreamAgent.h"


/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

typedef unsigned int VkMessageTargetIndex;

class VkMessage;
class VkMessageReceivingAgent;
class VkMessageShippingAgent;


/******************************************
 ******************************************
 *****  Message Controller Interface  *****
 ******************************************
 ******************************************/

/************************
 *****  Definition  *****
 ************************/

class VkMessageController {
//  Friends
    friend class VkMessage;

//  Callbacks
protected:
    virtual void PrepareIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );
    virtual void ReceiveIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );
    virtual void PrepareOutgoingMessage (
	VkMessage *pMessage, VkMessageShippingAgent *pAgent
    );
    virtual void DiscardOutgoingMessage (
	VkMessage *pMessage, VkMessageShippingAgent *pAgent
    );
};


/**********************************
 **********************************
 *****  Message Target Index  *****
 **********************************
 **********************************/

/***********************
 *****  Constants  *****
 ***********************/

#define VkMessageTargetIndex_Nil	((VkMessageTargetIndex)-1)
#define VkMessageTargetIndex_Root	((VkMessageTargetIndex) 0)

/************************
 *****  Conversion  *****
 ************************/

PublicFnDecl bool VkMessageTargetIndex_Convert (
    VkMessageTargetIndex *pThis, unsigned int xDataFormat
);


/**********************************
 **********************************
 *****  Message Target Table  *****
 **********************************
 **********************************/

/************************
 *****  Definition  *****
 ************************/

class VkMessageTargetTable : public VTransient {
//  Types
protected:
    class Entry {
    //  Friends
	friend class VkMessageTargetTable;

    //  Construction
    public:
	void construct (VkMessageController *pController) {
	    m_pController = pController;
	}
	void construct (unsigned int xNextFree) {
	    m_pController = 0;
	    m_xNextFree = xNextFree;
	}

    //  Destruction
    public:

    //  Access
    public:
	VkMessageController *Controller () const {
	    return m_pController;
	}

    //  Query
    public:
	bool IsntValid () const {
	    return IsNil (m_pController);
	}
	bool IsValid () const {
	    return IsntNil (m_pController);
	}

    //  State
    protected:
	VkMessageController *m_pController;
	unsigned int	     m_xNextFree;
    };


//  Construction
public:
    VkMessageTargetTable (VkMessageController *pRootObject);

//  Destruction
public:
    void Destroy ();

//  Target Update
public:
    VkMessageTargetIndex Add (VkMessageController *pController);

    void Remove (VkMessageTargetIndex xTarget);

//  Access
public:
    bool GetTargetMessageController (
	VkMessageTargetIndex xTarget, VkMessageController **pTargetMessageController
    ) const;

//  State
protected:
    VkMessageTargetIndex m_xFirstFree;
    unsigned int	 m_sActiveArea;
    unsigned int	 m_sTable;
    Entry		*m_pTable;
};


/****************************
 ****************************
 *****  Message Header  *****
 ****************************
 ****************************/

/************************
 *****  Definition  *****
 ************************/

class VkMessageHeader {
//  Construction
public:
    void construct (
	VkMessageTargetIndex	xSender,
	VkMessageTargetIndex	xTarget,
	unsigned short		xType,
	unsigned int		sBody
    );

//  Access
public:
    VkMessageTargetIndex Sender () const {
	return m_xSender;
    }
    VkMessageTargetIndex Target () const {
	return m_xTarget;
    }
    unsigned int Format () const {
	return m_xFormat;
    }
    unsigned int Type () const {
	return m_xType;
    }
    unsigned int Size () const {
	return m_sBody;
    }

//  Conversion
public:
    void Convert ();

//  Update
public:
    void SetTypeTo (unsigned int xType) {
	m_xType = xType;
    }
    void SetSizeTo (unsigned int sBody) {
	m_sBody = sBody;
    }

//  State
protected:
    VkMessageTargetIndex	m_xSender;
    VkMessageTargetIndex	m_xTarget;
    unsigned int		m_xFormat	:  8,
				m_fUnused	:  8,
				m_xType		: 16;
    unsigned int		m_sBody;
};


/*********************
 *********************
 *****  Message  *****
 *********************
 *********************/

/************************
 *****  Definition  *****
 ************************/

class VkMessage : public VTransient {
//  Construction
public:
    void construct (
	VkMessageController		*pController,
	VkMessageTargetIndex		 xSender,
	VkMessageTargetIndex		 xTarget,
	unsigned short			 xType,
	unsigned int			 sBody,
	void const			*pBody
    );

    VkMessage &operator= (VkMessage const &rOther) {
	memcpy (this, &rOther, sizeof (VkMessage));
	return *this;
    }

//  Destruction
public:
    void Destroy ();

//  Callbacks
public:
    void PrepareIncomingMessage (VkMessageReceivingAgent *pAgent);
    void ReceiveIncomingMessage (VkMessageReceivingAgent *pAgent);
    void PrepareOutgoingMessage (VkMessageShippingAgent *pAgent);
    void DiscardOutgoingMessage (VkMessageShippingAgent *pAgent);

//  Access
public:
    VkMessageTargetIndex Sender () const {
	return m_iHeader.Sender ();
    }
    VkMessageTargetIndex Target () const {
	return m_iHeader.Target ();
    }
    unsigned int Format () const {
	return m_iHeader.Format ();
    }
    unsigned int Type () const {
	return m_iHeader.Type ();
    }
    unsigned int Size () const {
	return m_iHeader.Size ();
    }
    void *Body () const {
	return m_pBody;
    }

//  Update
public:
    void AttachBody (void *pBody);
    void DetachBody ();
    bool RealizeBody ();

    void SetTypeTo (unsigned int xType) {
	m_iHeader.SetTypeTo (xType);
    }
    void SetSizeTo (unsigned int sBody) {
	m_iHeader.SetSizeTo (sBody);
    }

//  Conversion
public:
    void Convert ();

//  State
public:
    VkMessageController*	m_pController;
    VkMessageHeader		m_iHeader;
    void*			m_pBody;
    unsigned int		m_fBodyAllocated;
};


/************************************
 ************************************
 *****  Message Shipping Agent  *****
 ************************************
 ************************************/

class VkMessageShippingAgent : public VkStreamShippingAgent {
//  Types
protected:
    class QueueEntry : public VTransient {
    //  Construction
    public:
	QueueEntry (
	    VkMessageController *pController,
	    VkMessageTargetIndex xSender,
	    VkMessageTargetIndex xTarget,
	    unsigned short	 xType,
	    unsigned int	 sBody,
	    void const		*pBody
	);

    //  State
    public:
	QueueEntry	*m_pNext;
	VkMessage	 m_iOutgoingMessage;
    };

    enum State {
	State_Closed,
	State_Idle,
	State_Ready,
	State_HeadDeparting,
	State_HeadDeparted,
	State_HeadAborted,
	State_BodyDeparting,
	State_BodyDeparted,
	State_BodyAborted
    };

//  Construction
public:
    VkMessageShippingAgent (
	VkStream *pOutgoingStream, unsigned int sOutgoingChunk
    );

//  Destruction
public:

//  Event Processing
public:
    void ProcessEvent ();

//  Transmission
protected:
    void OnCompletion ();

    bool WriteData ();

public:
    bool Send (
	VkMessageController*	pController,
	VkMessageTargetIndex	xSender,
	VkMessageTargetIndex	xTarget,
	unsigned short		xType,
	unsigned int		sBody,
	void const*		pBody
    );

//  State
public:
    unsigned int  const	m_sOutgoingChunk;
    State		m_xOutgoingState;
    VkMessage		m_iOutgoingMessage;
    pointer_t		m_pOutgoingCursor;
    pointer_t		m_pOutgoingLimit;
    QueueEntry	       *m_pOutgoingQueueHead;
    QueueEntry	       *m_pOutgoingQueueTail;
};


/*************************************
 *************************************
 *****  Message Receiving Agent  *****
 *************************************
 *************************************/

class VkMessageReceivingAgent : public VkStreamReceivingAgent {
//  Types
protected:
    enum State {
	State_Closed,
	State_Ready,
	State_HeadArriving,
	State_HeadArrived,
	State_HeadAborted,
	State_BodyArriving,
	State_BodyArrived,
	State_BodyAborted
    };

//  Construction
public:
    VkMessageReceivingAgent (
	VkStream*		pIncomingStream,
	unsigned int		sIncomingChunk,
	VkStream*		pOutgoingStream,
	unsigned int		sOutgoingChunk,
	VkMessageTargetTable*	pIncomingTargets
    );

//  Destruction
public:

//  Access
public:
    VkMessageShippingAgent *ShippingAgent () const {
	return m_pShippingAgent;
    }

//  Event Processing
public:
    bool ProcessEvent ();

//  Reception
protected:
    void OnCompletion ();

    bool ReadData ();

//  Transmission
public:
    bool Send (
	VkMessageController*	pController,
	VkMessageTargetIndex	xSender,
	VkMessageTargetIndex	xTarget,
	unsigned short		xType,
	unsigned int		sBody,
	void const*		pBody
    ) {
	return m_pShippingAgent->Send (
	    pController, xSender, xTarget, xType, sBody, pBody
	);
    }

//  State
public:
    VkMessageShippingAgent*const m_pShippingAgent;
    unsigned int	   const m_sIncomingChunk;
    State			 m_xIncomingState;
    VkMessage			 m_iIncomingMessage;
    pointer_t			 m_pIncomingCursor;
    pointer_t			 m_pIncomingLimit;
    VkMessageTargetTable  *const m_pIncomingTargets;
};


#endif

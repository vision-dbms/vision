/*****  Data Base Converter Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VkMemory.h"
#include "VkMessage.h"

#include "V_VArgList.h"
#include "V_VString.h"
#include "V_VTime.h"
#include "VTransient.h"
#include "VTransientServices.h"

#include "Vca.h"

#include "gopt.h"
#include "rtype.d"
#include "ps.d"
#include "m.d"

#include "RTblock.d"
#include "RTcharuv.d"
#include "RTclosure.d"
#include "RTcontext.d"
#include "RTdictionary.d"
#include "RTdoubleuv.d"
#include "RTdsc.d"
#include "RTfloatuv.d"
#include "RTindex.d"
#include "RTintuv.d"
#include "RTlink.d"
#include "RTlstore.d"
#include "RTmethod.d"
#include "RTparray.d"
#include "RTpct.d"
#include "RTptoken.d"
#include "RTrefuv.d"
#include "RTseluv.d"
#include "RTstring.d"
#include "RTtct.d"
#include "RTundefuv.d"
#include "RTvector.d"
#include "RTvstore.d"

#ifndef S_ISDIR
#define S_ISDIR(m) ((m) & _S_IFDIR)
#endif


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

static VkMemory::Share g_xMappingMode = VkMemory::Share_Private;


/****************************
 ****************************
 *****  The Byte Codes  *****
 ****************************
 ****************************/

/***********************************
 *****  Byte Code Definitions  *****
 ***********************************/
/*---------------------------------------------------------------------------
 * The following #define's attach numeric values to the byte codes.  Each byte
 * code is accompanied by a synopsis of the form:
 *
 *	operator parameterName[parameterType] parameterName[parameterType] ...
 *
 * where 'parameterName' is one of:
 *
 *	eli			- evaluated literal index
 *	ksi			- known selector index
 *	mwi			- magic word index
 *	pli			- physical literal index
 *	ppi			- primitive procedure index
 *	skip			- unsigned pc-relative offset.
 *	ssi			- string space index
 *	tag			- tag value
 *	value			- numeric value
 *	valence			- selector valence
 *	void			- ignored numeric value
 *
 * and 'parameterType' is one of:
 *
 *	byte			- signed byte
 *	ubyte			- unsigned byte
 *	short			- aligned, signed short
 *	ushort			- aligned, unsigned short
 *	xshort			- un-aligned, signed short
 *	xushort			- un-aligned, unsigned short
 *	int			- aligned, signed int
 *	xint			- un-aligned, signed int
 *	double			- double
 *
 *---------------------------------------------------------------------------
 */


#define V_BC_Pad0		     1	/* pad0 */
#define V_BC_Pad1		     2	/* pad1 void[byte] */
#define V_BC_Pad2		     3	/* pad2 void[xshort] */

#define V_BC_Tag		     4	/* tag tag[byte] */

#define V_BC_Comment		     5	/* comment ssi[ubyte] */
#define V_BC_CommentS		     6	/* comments ssi[xushort] */

#define V_BC_AcquireParameter	    16	/* acqp ssi[ubyte] */
#define V_BC_AcquireParameterS	    17	/* acqps ssi[ushort] */
#define V_BC_AcquireKnownParameter  19	/* acqkp ksi[ubyte] */

#define V_BC_UnaryMessage	    32	/* unarymsg ssi[ubyte] */
#define V_BC_UnaryMessageS	    33	/* unarymsgs ssi[ushort] */
#define V_BC_KeywordMessage	    35	/* keywordmsg ssi[ubyte]
							      valence[ubyte] */
#define V_BC_KeywordMessageS	    36	/* keywordmsgs ssi[ushort]
							       valence[ubyte]*/

#define V_BC_KnownMessage	    38	/* knownmsg ksi[ubyte] */

#define V_BC_LAssign		    39	/* lassign */
#define V_BC_RAssign		    40	/* rassign */
#define V_BC_Concat		    41	/* concat */
#define V_BC_LT			    42	/* lt */
#define V_BC_LE			    43	/* le */
#define V_BC_GE			    44	/* ge */
#define V_BC_GT			    45	/* gt */
#define V_BC_Equal		    46	/* eq */
#define V_BC_Equivalent		    47	/* equiv */
#define V_BC_NEqual		    48	/* neq */
#define V_BC_NEquivalent	    49	/* nequiv */
#define V_BC_Or			    50	/* or */
#define V_BC_And		    51	/* and */
#define V_BC_Add		    52	/* add */
#define V_BC_Subtract		    53	/* sub */
#define V_BC_Multiply		    54	/* mul */
#define V_BC_Divide		    55	/* div */

#define V_BC_Dispatch		    96	/* dispatch */
#define V_BC_DispatchMagic	    97	/* dispatchMagic mwi[ubyte] */

#define V_BC_NextMessageParameter  100	/* nextp */

#define V_BC_Value		   101	/* value */
#define V_BC_Intension		   102	/* intension */

#define V_BC_KnownUnaryValue	   128	/* knownuv ksi[ubyte]
							   mwi[ubyte] */
#define V_BC_KnownUnaryIntension   129	/* knownui ksi[ubyte]
							   mwi[ubyte] */

#define V_BC_UnaryValue		   130	/* unaryv ssi[ubyte]
							  mwi[ubyte] */
#define V_BC_UnaryValueS	   131	/* unaryvs ssi[ushort]
							   mwi[ubyte] */
#define V_BC_UnaryIntension	   133	/* unaryi ssi[ubyte]
							  mwi[ubyte] */
#define V_BC_UnaryIntensionS	   134	/* unaryis ssi[ushort]
							   mwi[ubyte] */

#define V_BC_WrapupUnaryValue	   136	/* wrapuv */
#define V_BC_WrapupUnaryIntension  137	/* wrapui */

#define V_BC_StoreMagic		   160	/* stmagic mwi[ubyte] */
#define V_BC_StoreLexBinding	   161	/* stlexb pli[ubyte] */
#define V_BC_StoreLexBindingS	   162  /* stlexbs pli[ushort] */

#define V_BC_StoreInteger	   176	/* stint value[int] */
#define V_BC_StoreDouble	   177	/* stdbl value[double] */
#define V_BC_StoreString	   178	/* ststr ssi[ubyte] */
#define V_BC_StoreStringS	   179	/* ststrs ssi[ushort] */
#define V_BC_StoreSelector	   181	/* stsel ssi[ubyte]
							 valence[ubyte] */
#define V_BC_StoreSelectorS	   182	/* stsels ssi[ushort]
							  valence[ubyte] */
#define V_BC_StoreKnownSelector	   184	/* stksel ksi[ubyte] */
#define V_BC_StoreNoValue	   185  /* stnoval */

#define V_BC_Exit		   192	/* exit */


/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

class VNetworkHandle;
class VSegmentHandle;
class VSpaceHandle;


/*************************
 *************************
 *****  Definitions  *****
 *************************
 *************************/

/***************************
 *****  Schedule Entry *****
 ***************************/

class VScheduleEntry {
//  Friends
    friend class VSchedule;

//  Construction
public:
    VScheduleEntry (VScheduleEntry* pNext, PS_Type_FO oStructure);

//  Query
public:
    static int __cdecl Compare (VScheduleEntry const* pThis, VScheduleEntry const* pOther);

//  State
protected:
    VScheduleEntry*		m_pNext;
    PS_Type_FO			m_oStructure;
};

/**********************
 *****  Schedule  *****
 **********************/

class VSchedule : public VTransient {
//  Construction
public:
    int Construct ();

//  Update
public:
    void Add (
	PS_Type_FO oStructure, PS_Type_FO oStructureLimit, char const *pStructureName
    );
    int Next (PS_Type_FO* pResult);

//  State
protected:
    VScheduleEntry*		m_pSchedule;
    void*			m_pSearchTree;
};

/******************************
 *****  Segment Identity  *****
 ******************************/

class VSegmentIdentity {
//  Construction
public:
    VSegmentIdentity (
	unsigned int xSpace, unsigned int xSegment, PS_TID const &rTID, PS_RID const &rRID
    );

//  Comparison
public:
    bool operator== (VSegmentIdentity const& pOther) const;

//  Conversion
public:
    bool Convert (unsigned int xDataFormat);

//  State
public:
    unsigned int	m_xSpace;
    unsigned int	m_xSegment;
    PS_TID		m_iTID;
    PS_RID		m_iRID;
};


/****************************
 *****  Network Handle  *****
 ****************************/

class VNetworkHandle : public VTransient, public VkMessageController {
//  Construction
protected:
    VNetworkHandle () {
    }

    bool Construct (char const *pPathName, bool fReadOnly);

public:
    static VNetworkHandle *New (char const *pPathName, bool fReadOnly);

//  Destruction
public:
    void Destroy ();

//  Access
public:
    VkMessageTargetIndex MySelf () const {
	return m_xMySelf;
    }
    unsigned int IncomingSegmentCount () const {
	return m_cIncomingSegments;
    }

//  Segment Count Maintenance
public:
    void IncrementIncomingSegmentCount () {
	++m_cIncomingSegments;
    }
    void DecrementIncomingSegmentCount () {
	--m_cIncomingSegments;
    }

//  NDF Access / Update
protected:
    void Get (PS_Type_FO oDatum, void *pDatum, unsigned int sDatum) const {
	memcpy (pDatum, (pointer_t)m_iMapping.RegionAddress () + oDatum, sDatum);
    }
    void Put (PS_Type_FO oDatum, void const*pDatum, unsigned int sDatum) const {
	memcpy ((pointer_t)m_iMapping.RegionAddress () + oDatum, pDatum, sDatum);
    }
    void Sync ();

//  Directory Creation and Cleanup
protected:
    int FindOrCreateDirectory () const;

public:
    void CleanupOSD () const;

//  Segment Access and Modeling
public:
    VSegmentHandle* SegmentHandle (unsigned int xSpace, unsigned int xSegment) const;

    void FindOrRetrieveSegments ();

protected:
    void BuildSegmentModel ();

//  Scheduling
protected:
    void ProcessSchedule (
	VSchedule*	pSchedule,
	bool		(VNetworkHandle::*pElementProcessor)(PS_Type_FO),
	char const*	pElementTypeName
    );
    void ScheduleSD  (PS_Type_FO oSD);
    void ScheduleSVD (PS_Type_FO oSVD);
    void ScheduleNVD (PS_Type_FO oNVD);

//  Component Conversion
protected:
    void ConvertString (PS_Type_FO oString);

    bool ConvertSD  (PS_Type_FO oSD);
    bool ConvertSVD (PS_Type_FO oSVD);

    void ConvertSRV (PS_Type_FO oSRV);
    
    bool ConvertNVD (PS_Type_FO oNVD);

    void ConvertNDF ();

//  State
protected:
    VkMessageTargetIndex	m_xMySelf;
    char const*			m_pNDFPathName;
    char const*			m_pOSDPathName;
    unsigned int		m_cIncomingSegments;
    VSchedule			m_iNVDSchedule;
    VSchedule			m_iSVDSchedule;
    VSchedule			m_iSDSchedule;
public:
    VkMemory			m_iMapping;
protected:
    PS_NDH			m_iNDH;
    VSpaceHandle*		m_pSpaceHandle[M_POP_MaxObjectSpace + 1];
};


/**************************
 *****  Space Handle  *****
 **************************/

class VSpaceHandle : public VTransient, public VkMessageController {
    friend class VNetworkHandle;

//  Globals
private:
    static char const *const g_pDSegsDirectoryName;

//  Construction
public:
    VSpaceHandle (
	VNetworkHandle *pNetworkHandle, char const *pOSDPathName, unsigned int xSpace
    );

//  Destruction
public:
    ~VSpaceHandle ();

//  Access
public:
    VkMessageTargetIndex MySelf () const {
	return m_xMySelf;
    }

    VNetworkHandle *NetworkHandle () const {
	return m_pNetworkHandle;
    }

    unsigned int SegmentModelSize () const {
	return m_xMaxModelSegment + 1 - m_xMinModelSegment;
    }

protected:
    unsigned int SegmentVectorSize () const {
	return m_xMaxVectorSegment + 1 - m_xMinVectorSegment;
    }

public:
    VSegmentHandle* SegmentHandle (unsigned int xSegment) const;

    unsigned int SpaceIndex () const {
	return m_xSpace;
    }

//  Directory Creation and Cleanup
public:
    int FindOrCreateDirectory () const;

    void CleanupOSD () const;

//  Model Building
public:
    void UpdateModelBounds (PS_SVD* pSVD);

    VSegmentHandle* DeclareSegment (PS_SD* pSD);

    void DecrementIncomingSegmentCount () {
	m_pNetworkHandle->DecrementIncomingSegmentCount ();
    }
    void IncrementIncomingSegmentCount () {
	m_pNetworkHandle->IncrementIncomingSegmentCount ();
    }

//  State
protected:
    VNetworkHandle* const	m_pNetworkHandle;
    VkMessageTargetIndex	m_xMySelf;
    char const*			m_pPathName;
    unsigned int		m_xSpace;
    PS_Type_FO			m_oUndeclaredSDChain;
    unsigned int		m_xMinModelSegment;
    unsigned int		m_xMaxModelSegment;
    unsigned int		m_xMinVectorSegment;
    unsigned int		m_xMaxVectorSegment;
    VSegmentHandle**		m_pSegmentVector;
};


/****************************
 *****  Segment Handle  *****
 ****************************/

class VSegmentHandle : public VTransient, public VkMessageController {
//  Message Controller
protected:
    void PrepareIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );
    void ReceiveIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );
    void PrepareOutgoingMessage (
	VkMessage *pMessage, VkMessageShippingAgent *pAgent
    );
    void DiscardOutgoingMessage (
	VkMessage *pMessage, VkMessageShippingAgent *pAgent
    );

//  Construction
public:
    VSegmentHandle (VSpaceHandle* pSpace, PS_SD* pSD, char const *pPathName);

//  Destruction
public:
    ~VSegmentHandle ();

//  Access
public:
    VkMessageTargetIndex MySelf () const {
	return m_xMySelf;
    }
    VNetworkHandle *NetworkHandle () const {
	return m_pSpaceHandle->NetworkHandle ();
    }
    VSpaceHandle* SpaceHandle () const {
	return m_pSpaceHandle;
    }

//  Mapping
protected:
    void Unmap (bool fUnlock);

    void Sync ();

    bool Map (bool fReadOnly, bool fLockMapping);

    bool MapNew (unsigned int sSegment);

//  Conversion
protected:
    bool ConvertSegment ();

//  Retrieval
public:
    bool FindOrRetrieveSegment ();

//  Segment Count Maintenance
protected:
    void DecrementIncomingSegmentCount () {
	m_pSpaceHandle->DecrementIncomingSegmentCount ();
    }
    void IncrementIncomingSegmentCount () {
	m_pSpaceHandle->IncrementIncomingSegmentCount ();
    }

//  State
protected:
    VkMessageTargetIndex	m_xMySelf;
    VSpaceHandle*		m_pSpaceHandle;
    char const*			m_pPathName;
    char const*			m_pSavedPathName;
    unsigned int		m_fSegmentValid		: 1,
				m_fMappingValid		: 1,
				m_fReadOnly		: 1;
    VSegmentIdentity		m_iIdentity;
    VkMemory			m_iMapping;
    unsigned int		m_cMappingLocks;
};


/***************************************
 *****  Database Conversion Agent  *****
 ***************************************/

class VConversionAgent : public VTransient, public VkMessageController {
//  Message Controller
protected:
    void PrepareIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );
    void ReceiveIncomingMessage (
	VkMessage *pMessage, VkMessageReceivingAgent *pAgent
    );

//  Construction
public:
    VConversionAgent ();

    void Construct ();

    VkMessageTargetIndex Add (VkMessageController *pController) {
	return m_iMessageTargets.Add (pController);
    }

//  Destruction
public:
    void Destroy ();

//  Access
public:
    VSegmentHandle* SegmentHandle (unsigned int xSpace, unsigned int xSegment) const;

//  Peer Connection
protected:
    void ConnectPeer ();

//  Data Transfer
public:
    bool Send (
	VkMessageController*	pController,
	VkMessageTargetIndex	xSender,
	VkMessageTargetIndex	xTarget,
	unsigned short		xType,
	unsigned int		sBody,
	void const*		pBody
    );

//  Mapping
protected:
    bool MapNew (unsigned int sMapping);

    void Sync	();
    void Unmap	();

//  Execution
protected:
    void ProcessSlave ();

public:
    void Run ();

//  State
protected:
    VkMessageTargetTable	m_iMessageTargets;
    char const			*m_pSlavePathName;
    char const			*m_pMasterPathName;
    VkMessageReceivingAgent	*m_pMessageAgent;
    VNetworkHandle		*m_pNetworkHandle;
    unsigned int		m_fNotDone	: 1,
				m_fMappingValid	: 1;
    VkMemory			m_iMapping;
};


/************************************
 ************************************
 *****  Remote Message Indices  *****
 ************************************
 ************************************/

enum MessageIndex {
    MessageIndex_OpenNetwork,		/* C-->S: <Body>={ndfPathName\0}	*/
    MessageIndex_OpenSegment,		/* C-->S: <Body>=VSegmentIdentity	*/
    MessageIndex_ObjectAck,		/* S-->C: <Body>=VkMessageTargetIndex	*/
    MessageIndex_ObjectNack,		/* S-->C: <Body>=<null>			*/
    MessageIndex_GetContent,		/* C-->S: <Body>=<null>			*/
    MessageIndex_SetContent		/* S-->C: <Body>={segment content}	*/
};


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

PrivateVarDef VConversionAgent g_iConversionAgent;

PrivateVarDef unsigned int MessageChunkSize;

PrivateVarDef struct time OperationStartTime;
PrivateVarDef struct time OperationEndTime;

PrivateVarDef bool NoCleanup	= false;
PrivateVarDef bool Verbose	= false;
PrivateVarDef bool CleanupOnly	= false;


/*************************************
 *************************************
 *****  Operation Rate and Time  *****
 *************************************
 *************************************/

#define VkTime_AsDouble(pThis) (\
    (double)(pThis)->seconds + (double)(pThis)->microseconds / 1e6\
)

static double OperationTime () {
    return VkTime_AsDouble (&OperationEndTime)
	 - VkTime_AsDouble (&OperationStartTime);
}

static double OperationRate (unsigned int sTransfer) {
    double sDelay = OperationTime ();
    return sDelay != 0 ? (double)sTransfer / sDelay / 1.e3 : 0.0;
}


/******************************
 ******************************
 *****  Error Management  *****
 ******************************
 ******************************/

/***********************
 *****  Utilities  *****
 ***********************/

PrivateFnDef char const *LastErrorDescription () {
    VkStatus iStatus;
    iStatus.MakeErrorStatus ();
    return iStatus.CodeDescription ();
}

PrivateFnDef char const *VMakeMessage (char const *pFormat, V::VArgList pArguments) {
    static char buffer[1024];

    vsprintf (buffer, pFormat, pArguments);

    return buffer;
}

PrivateFnDef char const *MakeMessage (char const *pFormat, ...) {
    V_VARGLIST (pArguments, pFormat);
    return VMakeMessage (pFormat, pArguments);
}


/*********************
 *****  Display  *****
 *********************/

enum MessageType {
    MessageType_Info,
    MessageType_Warning,
    MessageType_Error
};

PrivateFnDef char const *MessageType_Flag (MessageType xMessageType) {
    switch (xMessageType) {
    case MessageType_Info:
	return "-->";
    case MessageType_Warning:
	return "+++";
    case MessageType_Error:
	return ">>>";
    default:
	return "???";
    }
}

PrivateFnDef void MessageType_VDisplayApplicationMessage (
    MessageType xMessageType, char const *pFormat, V::VArgList const &rArguments
) {
    V::VTime iNow; 
    VString iTimestring;
    iNow.asString (iTimestring);

    fflush  (stdout);
    fprintf (
	stderr, "%s:%s %s\n",
	iTimestring.content (),
	MessageType_Flag (xMessageType),
	VMakeMessage (pFormat, rArguments)
    );
    fflush  (stderr);
    if (MessageType_Error == xMessageType)
	V::SetExitCodeToError ();
}

PrivateFnDef void MessageType_VDisplaySystemMessage (
    MessageType xMessageType, char const *pFormat, V::VArgList const &rArguments
) {
    V::VTime iNow; 
    VString iTimestring;
    iNow.asString (iTimestring);

    fflush  (stdout);
    fprintf (
	stderr, "%s:%s %s:%s\n",
	iTimestring.content (),
	MessageType_Flag (xMessageType),
	LastErrorDescription (),
	VMakeMessage (pFormat, rArguments)
    );
    fflush  (stderr);
    if (MessageType_Error == xMessageType)
	V::SetExitCodeToError ();
}

PrivateFnDef void MessageType_DisplayApplicationMessage (
    MessageType xMessageType, char const *pFormat, ...
) {
    V_VARGLIST (iArgList, pFormat);

    MessageType_VDisplayApplicationMessage (xMessageType, pFormat, iArgList);
}

PrivateFnDef void MessageType_DisplaySystemMessage (
    MessageType xMessageType, char const *pFormat, ...
) {
    V_VARGLIST (pArguments, pFormat);
    MessageType_VDisplaySystemMessage (xMessageType, pFormat, pArguments);
}


/************************
 *****  Generation  *****
 ************************/

PrivateFnDef void RaiseApplicationException (char const *pFormat, ...) {
    V_VARGLIST (pArguments, pFormat);
    MessageType_VDisplayApplicationMessage (MessageType_Error, pFormat, pArguments);

    exit (ErrorExitValue);
}

PrivateFnDef void RaiseSystemException (char const *pFormat, ...) {
    V_VARGLIST (pArguments, pFormat);
    MessageType_VDisplaySystemMessage (MessageType_Error, pFormat, pArguments);

    exit (ErrorExitValue);
}


/*******************************
 *******************************
 *****  Stream Management  *****
 *******************************
 *******************************/

PrivateFnDef void StartStreamManager () {
    VString iMessage;

    if (Vca::Start (&iMessage))
	return;

    RaiseApplicationException (
	"Stream Subsystem Initialization Error: %s", iMessage.content ()
    );
}

PrivateFnDef void StopStreamManager () {
    Vca::Stop ();
}


/********************************
 ********************************
 *****  Directory Creation  *****
 ********************************
 ********************************/

PrivateFnDef int FindOrCreateDirectory (char const *pPathName) {
    struct stat iPathStats;

    if (0 == stat (pPathName, &iPathStats)) {
	if (S_ISDIR (iPathStats.st_mode))
	    return true;

	remove (pPathName);
    }

    return 0 == mkdir (pPathName, 0777);
}


/**********************************
 **********************************
 *****  Conversion Utilities  *****
 **********************************
 **********************************/

PrivateFnDef bool IsAReversedSignature (unsigned int signature) {
    static unsigned int ReversedSignature = 0;
    if (0 == ReversedSignature)
    {
	ReversedSignature = PS_NetworkDirectorySignature;
	Vk_ReverseSizeable (&ReversedSignature);
    }

    return signature == ReversedSignature;
}

PrivateFnDef bool IsAReversedOSDOffset (PS_Type_FO offset) {
    static PS_Type_FO ReversedOffset = 0;
    if (0 == ReversedOffset) {
	ReversedOffset = sizeof (PS_NDH);
	Vk_ReverseSizeable (&ReversedOffset);
    }

    return offset == ReversedOffset;
}

PrivateFnDef bool IsAReversedNilOffset (PS_Type_FO offset) {
    static PS_Type_FO ReversedOffset = 0;
    if (0 == ReversedOffset) {
	ReversedOffset = PS_FO_Nil;
	Vk_ReverseSizeable (&ReversedOffset);
    }

    return offset == ReversedOffset;
}


/*****************
 *****************
 *****  TID  *****
 *****************
 *****************/

PrivateFnDef void TID_Convert (PS_TID *pThis) {
    Vk_ReverseSizeable (&pThis->high);
    Vk_ReverseSizeable (&pThis->low);
    Vk_ReverseSizeable (&pThis->sequence);
}

/*****************
 *****************
 *****  RID  *****
 *****************
 *****************/

PrivateFnDef void RID_Convert (PS_RID *pThis) {
    Vk_ReverseSizeable (&pThis->type);
    Vk_ReverseSizeable (&pThis->sequence);
}

/************************
 ************************
 *****  Time Stamp  *****
 ************************
 ************************/

PrivateFnDef void TS_Convert (PS_TS *pThis) {
    Vk_ReverseSizeable (&pThis->seconds);
    Vk_ReverseSizeable (&pThis->microseconds);
}

/*****************
 *****************
 *****  UID  *****
 *****************
 *****************/

PrivateFnDef void UID_Convert (PS_UID *pThis) {
    Vk_ReverseSizeable (&pThis->realUID);
    Vk_ReverseSizeable (&pThis->effectiveUID);
    Vk_ReverseSizeable (&pThis->realGID);
    Vk_ReverseSizeable (&pThis->effectiveGID);
}


/*******************
 *******************
 *****  Block  *****
 *******************
 *******************/

PrivateFnDef void Block_Convert (
    rtBLOCK_BlockType*		pThis,
    unsigned int		Unused(sThis)
)
{
/*****  Convert the block preamble, ...  *****/
    Vk_ReverseSizeable (&pThis->compilerVersion);
    Vk_ReverseSizeable (&pThis->selector.selectorType);
    Vk_ReverseSizeable (&pThis->selector.selectorIndex);
    Vk_ReverseSizeable (&pThis->preamble.localEnvironment);
    Vk_ReverseSizeable (&pThis->preamble.byteCodeVector);
    Vk_ReverseSizeable (&pThis->preamble.stringSpace);
    Vk_ReverseSizeable (&pThis->preamble.physicalLiteralVector);
    Vk_ReverseSizeable (&pThis->preamble.evaledLEntryVector);
    Vk_ReverseSizeable (&pThis->localEnvironmentPOP);

/*****  ... byte code vector, ...  *****/
    if (pThis->preamble.byteCodeVector > 0)
    {
    	unsigned char* pBC = (unsigned char*)(
	    (pointer_t)pThis + pThis->preamble.byteCodeVector
	);
	bool notDone = true;
	do {
	    switch (*pBC++)
	    {
	    default:
	    case V_BC_Pad0:
	    case V_BC_LAssign:
	    case V_BC_RAssign:
	    case V_BC_Concat:
	    case V_BC_LT:
	    case V_BC_LE:
	    case V_BC_GE:
	    case V_BC_GT:
	    case V_BC_Equal:
	    case V_BC_Equivalent:
	    case V_BC_NEqual:
	    case V_BC_NEquivalent:
	    case V_BC_Or:
	    case V_BC_And:
	    case V_BC_Add:
	    case V_BC_Subtract:
	    case V_BC_Multiply:
	    case V_BC_Divide:
	    case V_BC_Dispatch:
	    case V_BC_NextMessageParameter:
	    case V_BC_Value:
	    case V_BC_Intension:
	    case V_BC_WrapupUnaryValue:
	    case V_BC_WrapupUnaryIntension:
	    case V_BC_StoreNoValue:
		break;

	    case V_BC_Pad1:
	    case V_BC_Tag:
	    case V_BC_Comment:
	    case V_BC_AcquireParameter:
	    case V_BC_AcquireKnownParameter:
	    case V_BC_UnaryMessage:
	    case V_BC_KnownMessage:
	    case V_BC_DispatchMagic:
	    case V_BC_StoreMagic:
	    case V_BC_StoreLexBinding:
	    case V_BC_StoreString:
	    case V_BC_StoreKnownSelector:
		pBC++;
		break;

	    case V_BC_Pad2:
	    case V_BC_CommentS:
	    case V_BC_AcquireParameterS:
	    case V_BC_UnaryMessageS:
	    case V_BC_StoreLexBindingS:
	    case V_BC_StoreStringS:
		Vk_ReverseSizeable ((unsigned short *)pBC);
		pBC += sizeof (unsigned short);
		break;

	    case V_BC_KeywordMessage:
	    case V_BC_KnownUnaryValue:
	    case V_BC_KnownUnaryIntension:
	    case V_BC_UnaryValue:
	    case V_BC_UnaryIntension:
	    case V_BC_StoreSelector:
 		pBC += 2;
		break;

	    case V_BC_KeywordMessageS:
	    case V_BC_UnaryValueS:
	    case V_BC_UnaryIntensionS:
	    case V_BC_StoreSelectorS:
		Vk_ReverseSizeable ((unsigned short *)pBC);
		pBC += sizeof (unsigned short) + 1;
		break;

	    case V_BC_StoreInteger:
		Vk_ReverseSizeable ((unsigned int *)pBC);
		pBC += sizeof (unsigned int);
		break;

	    case V_BC_StoreDouble:
		Vk_ReverseSizeable ((double *)pBC);
		pBC += sizeof (double);
		break;

	    case V_BC_Exit:
		notDone = false;
		break;
	    }
	} while (notDone);
    }

/*****  ... POP vector, ...  *****/
    if (pThis->preamble.physicalLiteralVector > 0)
    {
	rtBLOCK_PLVectorType* pVector = (rtBLOCK_PLVectorType*)(
	    (pointer_t)pThis + pThis->preamble.physicalLiteralVector
	);
	int iPOP;

	Vk_ReverseSizeable (&pVector->popCount);
	for (iPOP = 0; iPOP < pVector->popCount; iPOP++)
	    Vk_ReverseSizeable (pVector->physicalLiterals + iPOP);
    }

/*****  ... and unused Evaled literal entry vector, ...  *****/
    if (pThis->preamble.evaledLEntryVector > 0)
    {
	rtBLOCK_ELEVectorType* pVector = (rtBLOCK_ELEVectorType*)(
	    (pointer_t)pThis + pThis->preamble.evaledLEntryVector
	);
	int iEntry;

	Vk_ReverseSizeable (&pVector->entryCount);
	for (iEntry = 0; iEntry < pVector->entryCount; iEntry++)
	    Vk_ReverseSizeable (pVector->entryPoints + iEntry);
    }
}


/*********************
 *********************
 *****  Closure  *****
 *********************
 *********************/

PrivateFnDef void Closure_Convert (
    struct rtCLOSURE_Closure*	pThis,
    unsigned int		Unused(sThis)
)
{
    Vk_ReverseSizeable (&pThis->context);
    Vk_ReverseSizeable (&pThis->block);
    Vk_ReverseSizeable (&pThis->primitive);
}


/************************
 ************************
 *****  Descriptor  *****
 ************************
 ************************/

PrivateFnDef void Descriptor_Convert (
    struct rtDSC_Descriptor*	pThis,
    unsigned int		Unused(sThis)
)
{
    Vk_ReverseSizeable (&pThis->pointerType);
    Vk_ReverseSizeable (&pThis->rType);
    Vk_ReverseSizeable (&pThis->store);
    Vk_ReverseSizeable (&pThis->pointer);
    switch (pThis->rType)
    {
    case RTYPE_C_CharUV:
	break;
    case RTYPE_C_DoubleUV:
	Vk_ReverseSizeable (&pThis->scalarValue.asDouble);
	break;
    case RTYPE_C_FloatUV:
	Vk_ReverseSizeable (&pThis->scalarValue.asFloat);
	break;
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_Link:
	Vk_ReverseSizeable (&pThis->scalarValue.asInt);
	break;
    case RTYPE_C_Unsigned64UV:
        Vk_ReverseSizeable (&pThis->scalarValue.asUnsigned64);
        break;
    case RTYPE_C_Unsigned96UV:
        Vk_ReverseSizeable (&pThis->scalarValue.asUnsigned96);
        break;
    case RTYPE_C_Unsigned128UV:
        Vk_ReverseSizeable (&pThis->scalarValue.asUnsigned128);
        break;
    default:
	break;
    }
}


/******************
 ******************
 *****  Link  *****
 ******************
 ******************/

PrivateFnDef void Link_Convert (
    rtLINK_Type*		pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int	cRRD;
    rtLINK_RRDType*	pRRD;
    union linkFields_t {
	struct forward_t {
	    unsigned int	tracking	: 1,
				hasRepeats	: 1,
				hasRanges	: 1,
				isInconsistent	: 1,
				rrdCount	: 28;
	}		    inStructOrder;
	struct reverse_t {
	    unsigned int	rrdCount	: 28,
				isInconsistent	: 1,
				hasRanges	: 1,
				hasRepeats	: 1,
				tracking	: 1;
	}		    inReversedOrder;
    }			linkFields;
    union rrdFields_t {
	struct forward_t {
	    unsigned int	repeatedRef	: 1,
				referenceOrigin	: 31;
	}		    inStructOrder;
	struct reverse_t {
	    unsigned int
				referenceOrigin	: 31,
				repeatedRef	: 1;
	}		    inReversedOrder;
    }			rrdFields;

    Vk_ReverseSizeable (&pThis->posPToken);
    Vk_ReverseSizeable (&pThis->refPToken);

    linkFields.inStructOrder.tracking		= pThis->tracking;
    linkFields.inStructOrder.hasRepeats		= pThis->hasRepeats;
    linkFields.inStructOrder.hasRanges		= pThis->hasRanges;
    linkFields.inStructOrder.isInconsistent	= pThis->isInconsistent;
    linkFields.inStructOrder.rrdCount		= pThis->rrdCount;
    Vk_ReverseSizeable (&linkFields);
    pThis->tracking		= linkFields.inReversedOrder.tracking;
    pThis->hasRepeats		= linkFields.inReversedOrder.hasRepeats;
    pThis->hasRanges		= linkFields.inReversedOrder.hasRanges;
    pThis->isInconsistent	= linkFields.inReversedOrder.isInconsistent;
    pThis->rrdCount		= linkFields.inReversedOrder.rrdCount;

    cRRD = pThis->rrdCount = pThis->rrdCount;
    pRRD = rtLINK_L_RRDArray (pThis);

    do {
	Vk_ReverseSizeable (&pRRD->linkCumulative);

	rrdFields.inStructOrder.repeatedRef	= pRRD->repeatedRef;
	rrdFields.inStructOrder.referenceOrigin	= pRRD->referenceOrigin;
	Vk_ReverseSizeable (&rrdFields);
	pRRD->repeatedRef	= rrdFields.inReversedOrder.repeatedRef;
	pRRD->referenceOrigin	= rrdFields.inReversedOrder.referenceOrigin;

	pRRD++;
    } while (cRRD-- > 0);
}


/*********************
 *********************
 *****  L-Store  *****
 *********************
 *********************/

PrivateFnDef void LStore_Convert (
    rtLSTORE_Type_LStore*	pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int cBPT, iBPT;
    union lstoreFields_t {
	struct forward_t {
	    unsigned int    	stringStore	:  1,
				isInconsistent	:  1, 
				breakpointCount	: 30;
	} inStructOrder;
	struct reverse_t {
	    unsigned int
				breakpointCount	: 30,
				isInconsistent	:  1, 
	    			stringStore	:  1;
	} inReversedOrder;
    } lstoreFields;

    Vk_ReverseSizeable (&pThis->rowPToken);
    Vk_ReverseSizeable (&pThis->content);
    Vk_ReverseSizeable (&pThis->contentPToken);

    lstoreFields.inStructOrder.stringStore	= pThis->stringStore;
    lstoreFields.inStructOrder.isInconsistent	= pThis->isInconsistent;
    lstoreFields.inStructOrder.breakpointCount	= pThis->breakpointCount;
    Vk_ReverseSizeable (&lstoreFields);
    pThis->stringStore		= lstoreFields.inReversedOrder.stringStore;
    pThis->isInconsistent	= lstoreFields.inReversedOrder.isInconsistent;
    pThis->breakpointCount	= lstoreFields.inReversedOrder.breakpointCount;

    cBPT = pThis->breakpointCount;

    for (iBPT = 0; iBPT <= cBPT; iBPT++)
	Vk_ReverseSizeable (pThis->breakpointArray + iBPT);
}


/*********************
 *********************
 *****  P-Array  *****
 *********************
 *********************/

PrivateFnDef void PArray_Convert (
    M_POP*			pThis,
    unsigned int		sThis
)
{
    unsigned int cPOP, iPOP;

    cPOP = sThis / sizeof (M_POP);
    for (iPOP = 0; iPOP < cPOP; iPOP++)
	Vk_ReverseSizeable (pThis + iPOP);
}


/****************************************
 ****************************************
 *****  Persistent Container Table  *****
 ****************************************
 ****************************************/

PrivateFnDef void PCT_Convert (
    PS_CT*			pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int	cCTE;
    PS_CTE*		pCTE;
    union cteFields_t {
	struct forward_t {
	    unsigned int	m_bCopy		:  1,
				m_bFree		:  1,
				m_xSegment	: 30;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	m_xSegment	: 30,
				m_bFree		:  1,
				m_bCopy		:  1;
	} inReversedOrder;
    } cteFields;

    Vk_ReverseSizeable (&pThis->spaceIndex);
    Vk_ReverseSizeable (&pThis->segmentIndex);
    Vk_ReverseSizeable (&pThis->entryCount);
    Vk_ReverseSizeable (&pThis->freeList);

    cCTE = pThis->entryCount;
    pCTE = pThis->entries;
    while (cCTE-- > 0)
    {
	cteFields.inStructOrder.m_bCopy	   = pCTE->m_bCopy;
	cteFields.inStructOrder.m_bFree	   = pCTE->m_bFree;
	cteFields.inStructOrder.m_xSegment = pCTE->m_xSegment;
	Vk_ReverseSizeable (&cteFields);
	pCTE->m_bCopy	= cteFields.inReversedOrder.m_bCopy;
	pCTE->m_bFree	= cteFields.inReversedOrder.m_bFree;
	pCTE->m_xSegment= cteFields.inReversedOrder.m_xSegment;

	Vk_ReverseSizeable (&pCTE->reference);

	pCTE++;
    }
}


/*********************
 *********************
 *****  P-Token  *****
 *********************
 *********************/

PrivateFnDef void PToken_Convert (
    rtPTOKEN_Type*		pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int		cSDsExamined;
    rtPTOKEN_SDType*		pSD;
    rtPTOKEN_CartesianType*	pCData;
    union pTokenFields_t {
	struct forward_t {
	    unsigned int	independent	:  1,
				tokenType	:  6,
				closed		:  1,
				count		: 24;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	count		: 24,
				closed		:  1,
				tokenType	:  6,
				independent	:  1;
	} inReversedOrder;
    } pTokenFields;

    Vk_ReverseSizeable (&pThis->nextGeneration);

    pTokenFields.inStructOrder.independent	= pThis->independent;
    pTokenFields.inStructOrder.tokenType	= pThis->tokenType;
    pTokenFields.inStructOrder.closed		= pThis->closed;
    pTokenFields.inStructOrder.count		= pThis->count;
    Vk_ReverseSizeable (&pTokenFields);
    pThis->independent	= pTokenFields.inReversedOrder.independent;
    pThis->tokenType	= pTokenFields.inReversedOrder.tokenType;
    pThis->closed	= pTokenFields.inReversedOrder.closed;
    pThis->count	= pTokenFields.inReversedOrder.count;

    Vk_ReverseSizeable (&pThis->baseElementCount);

    switch (pThis->tokenType)
    {
    case rtPTOKEN_TT_Base:
	break;
    case rtPTOKEN_TT_Shift:
	cSDsExamined = pThis->count;
	pSD = rtPTOKEN_PT_ChangeArray (pThis, rtPTOKEN_SDType);
	while (cSDsExamined-- > 0)
	{
	    Vk_ReverseSizeable (&pSD->origin);
	    Vk_ReverseSizeable (&pSD->shift);
	    pSD++;
	}
	break;
    case rtPTOKEN_TT_Cartesian:
	pCData = rtPTOKEN_PT_CartesianPTokens (pThis);
	Vk_ReverseSizeable (&pCData->rowPToken);
	Vk_ReverseSizeable (&pCData->colPToken);
	break;
    default:
	break;
    }
}


/************************
 ************************
 *****  POP-Vector  *****
 ************************
 ************************/

PrivateFnDef void POPVector_Convert (
    POPVECTOR_PVType*		pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int cPOP, iPOP;
    M_POP* pPOPArray;
    union popvectorFields_t {
	struct forward_t {
	    unsigned int	isInconsistent : 1,
				elementCount   :31;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	elementCount   :31,
				isInconsistent : 1;
	} inReversedOrder;
    } popvectorFields;

    popvectorFields.inStructOrder.isInconsistent = pThis->isInconsistent;
    popvectorFields.inStructOrder.elementCount	 = pThis->elementCount;
    Vk_ReverseSizeable (&popvectorFields);
    pThis->isInconsistent	= popvectorFields.inReversedOrder.isInconsistent;
    pThis->elementCount		= popvectorFields.inReversedOrder.elementCount;

    cPOP = pThis->elementCount;
    pPOPArray = POPVECTOR_PV_Array (pThis);

    for (iPOP = 0; iPOP < cPOP; iPOP++)
	Vk_ReverseSizeable (pPOPArray + iPOP);
}


/**********************
 **********************
 *****  U-Vector  *****
 **********************
 **********************/

PrivateFnDef void UVector_Convert (
    UV_PreambleType*		pThis,
    unsigned int		Unused(sThis),
    int				isASelectorUVector
)
{
    unsigned int  cElement;
    unsigned int  sElement;
    pointer_t pElement;
    union uvectorFields_t {
	struct forward_t {
#if defined(_AIX)
		unsigned int
#else
		unsigned short
#endif
				isInconsistent	: 1,
				isASetUV	: 1,
				padding		: 14;
	} inStructOrder;
	struct reverse_t {
#if defined(_AIX)
		unsigned int
#else
		unsigned short
#endif
				padding		: 14,
				isASetUV	: 1,
				isInconsistent	: 1;
	} inReversedOrder;
    } uvectorFields;
    union selectorFields_t {
	struct forward_t {
	    unsigned int	type	: 4,
				index	: 28;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	index	: 28,
	        		type	: 4;
	} inReversedOrder;
    } selectorFields;

    Vk_ReverseSizeable (&pThis->pToken);
    Vk_ReverseSizeable (&pThis->refPToken);
    Vk_ReverseSizeable (&pThis->auxiliaryPOP);
    Vk_ReverseSizeable (&pThis->elementCount);
    Vk_ReverseSizeable (&pThis->granularity);

    uvectorFields.inStructOrder.isInconsistent	= pThis->isInconsistent;
    uvectorFields.inStructOrder.isASetUV	= pThis->isASetUV;
    uvectorFields.inStructOrder.padding		= pThis->padding;
    Vk_ReverseSizeable (&uvectorFields);
    pThis->isInconsistent	= uvectorFields.inReversedOrder.isInconsistent;
    pThis->isASetUV		= uvectorFields.inReversedOrder.isASetUV;
    pThis->padding		= uvectorFields.inReversedOrder.padding;

    cElement = pThis->elementCount;
    sElement = pThis->granularity;
    pElement = (pointer_t)(pThis + 1);
    if (isASelectorUVector) while (cElement-- > 0) {
    	rtSELUV_Type_Element* pSelector = (rtSELUV_Type_Element*)pElement;

	selectorFields.inStructOrder.type	= pSelector->type;
	selectorFields.inStructOrder.index	= pSelector->index;
	Vk_ReverseSizeable (&selectorFields);
	pSelector->type		= selectorFields.inReversedOrder.type;
	pSelector->index	= selectorFields.inReversedOrder.index;

	pElement += sElement;
    }
    else while (cElement-- > 0) {
	Vk_ReverseArray (pElement, sElement);
	pElement += sElement;
    }
}


/********************
 ********************
 *****  Vector  *****
 ********************
 ********************/

PrivateFnDef void Vector_Convert (
    rtVECTOR_Type*		pThis,
    unsigned int		Unused(sThis)
)
{
    unsigned int	sArray;
    rtVECTOR_PMRDType*	pMapElement;
    rtVECTOR_USDType*	pUSegArrayElement;
    int*		pUSegIndexElement;
    union vectorFields_t {
	struct forward_t {
	    unsigned int	isASet		: 1,
				isInconsistent  : 1,
				pMapSize	: 30;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	pMapSize	: 30,
				isInconsistent  :  1,
				isASet		:  1;
	} inReversedOrder;
    } vectorFields;

/*****  Convert the preamble, ...  *****/
    Vk_ReverseSizeable (&pThis->pToken);

    vectorFields.inStructOrder.isASet		= pThis->isASet;
    vectorFields.inStructOrder.isInconsistent	= pThis->isInconsistent;
    vectorFields.inStructOrder.pMapSize		= pThis->pMapSize;
    Vk_ReverseSizeable (&vectorFields);
    pThis->isASet		= vectorFields.inReversedOrder.isASet;
    pThis->isInconsistent	= vectorFields.inReversedOrder.isInconsistent;
    pThis->pMapSize		= vectorFields.inReversedOrder.pMapSize;

    Vk_ReverseSizeable (&pThis->pMapTransition);
    Vk_ReverseSizeable (&pThis->uSegArraySize);
    Vk_ReverseSizeable (&pThis->uSegFreeBound);
    Vk_ReverseSizeable (&pThis->uSegIndexSize);

/*****  ... p-map, ...  *****/
    sArray = pThis->pMapSize;
    pMapElement = rtVECTOR_V_PMap (pThis);
    do {
	Vk_ReverseSizeable (&pMapElement->vectorOrigin);
	Vk_ReverseSizeable (&pMapElement->segmentIndex);
	Vk_ReverseSizeable (&pMapElement->segmentOrigin);
	pMapElement++;
    } while (sArray-- > 0);

/*****  ... u-seg array, ...  *****/
    sArray = pThis->uSegArraySize;
    pUSegArrayElement = rtVECTOR_V_USegArray (pThis);
    while (sArray-- > 0)
    {
	Vk_ReverseSizeable (&pUSegArrayElement->pToken);
	Vk_ReverseSizeable (&pUSegArrayElement->vStore);
	Vk_ReverseSizeable (&pUSegArrayElement->values);
	pUSegArrayElement++;
    }

/*****  ... and u-seg index:  *****/
    sArray = pThis->uSegIndexSize;
    pUSegIndexElement = rtVECTOR_V_USegIndex (pThis);
    while (sArray-- > 0)
    {
	Vk_ReverseSizeable (pUSegIndexElement);
	pUSegIndexElement++;
    }
}


/***********************
 ***********************
 *****  Container  *****
 ***********************
 ***********************/

PrivateFnDef M_CPreamble* Container_Convert (
    M_CPreamble*		pThis,
    const M_CPreamble*		pLimit,
    int*			pChecksum
)
{
    union preambleFields_t {
	struct forward_t {
	    unsigned int	rType	: 8,
				oSize	: 24;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	oSize	: 24,
				rType	: 8;
	} inReversedOrder;
    } preambleFields;

    /*****  Convert the preamble, ...  *****/
    Vk_ReverseSizeable (&pThis->nSize);

    preambleFields.inStructOrder.rType	= pThis->rType;
    preambleFields.inStructOrder.oSize	= pThis->oSize;
    Vk_ReverseSizeable (&preambleFields);
    pThis->rType    = preambleFields.inReversedOrder.rType;
    pThis->oSize    = preambleFields.inReversedOrder.oSize;

    Vk_ReverseSizeable (&pThis->pop);

    /*****  ... validate the end-marker address, ...  *****/
    M_CEndMarker *pEndMarker = M_CPreamble_EndMarker (pThis);
    if ((pointer_t)pEndMarker >= (pointer_t)pLimit)
    {
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Container [%04u:%07u]: Size %u Exceeds %u Bytes Remaining In Segment",
	    M_CPreamble_POPObjectSpace		(pThis),
	    M_CPreamble_POPContainerIndex	(pThis),
	    M_CPreamble_Size			(pThis),
	    (unsigned int)(pLimit - pThis)
	);
	return NilOf (M_CPreamble*);
    }

    /*****  ... convert the content, ...  *****/
    switch ((RTYPE_Type)M_CPreamble_RType (pThis))
    {
    case RTYPE_C_String:
    case RTYPE_C_TCT:
    case RTYPE_C_Undefined:
    case RTYPE_C_Undefined0:
	break;

    case RTYPE_C_Context:
    case RTYPE_C_Dictionary:
    case RTYPE_C_Index:
    case RTYPE_C_Method:
    case RTYPE_C_MethodD:
    case RTYPE_C_ValueStore:
	POPVector_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, POPVECTOR_PVType),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_RefUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
	UVector_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, UV_PreambleType),
	    M_CPreamble_Size (pThis),
	    false
	);
	break;

    case RTYPE_C_Block:
	Block_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, rtBLOCK_BlockType),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_Closure:
	Closure_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, struct rtCLOSURE_Closure),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_Descriptor1:
    case RTYPE_C_Descriptor:
	Descriptor_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, struct rtDSC_Descriptor),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_Link:
	Link_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, rtLINK_Type),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_ListStore:
	LStore_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, rtLSTORE_Type_LStore),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_PArray:
	PArray_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, M_POP),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_PCT:
	PCT_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, PS_CT),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_PToken:
	PToken_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, rtPTOKEN_Type),
	    M_CPreamble_Size (pThis)
	);
	break;

    case RTYPE_C_SelUV:
	UVector_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, UV_PreambleType),
	    M_CPreamble_Size (pThis),
	    true
	);
	break;

    case RTYPE_C_Vector:
	Vector_Convert (
	    M_CPreamble_ContainerBaseAsType (pThis, rtVECTOR_Type),
	    M_CPreamble_Size (pThis)
	);
	break;

    default:
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Container [%04u:%07u]: Unrecognized Type Code %d\n",
	    M_CPreamble_POPObjectSpace		(pThis),
	    M_CPreamble_POPContainerIndex	(pThis),
	    M_CPreamble_RType			(pThis)
	);
	return NilOf (M_CPreamble*);
    }

    /*****  ... convert the end marker, ...  *****/
    Vk_ReverseSizeable (pEndMarker);

    /*****  ... validate the container framing, ...  *****/
    if (M_CPreamble_NSize (pThis) != M_CEndMarker_Size (pEndMarker))
    {
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Container [%04u:%07u]: Framing Error %08x:%08x %u:%u",
	    M_CPreamble_POPObjectSpace		(pThis),
	    M_CPreamble_POPContainerIndex	(pThis),
	    M_CPreamble_NSize			(pThis),
	    M_CEndMarker_Size			(pEndMarker),
	    M_CPreamble_NSize			(pThis),
	    M_CEndMarker_Size			(pEndMarker)
	);
	return NilOf (M_CPreamble*);
    }

    /*****  ... determine the address of the next container, ...  *****/
    M_CPreamble *pSuccessor = (M_CPreamble*)(pEndMarker + 1);

    /*****  ... add this container to the current checksum, ... *****/
    int *pCSCursor = (int*)pThis;
    int cs = *pChecksum;
    while (pCSCursor < (int*)pSuccessor)
	cs ^= *pCSCursor++;
    *pChecksum = cs;

    /*****  ... and return the address of the next container:  *****/
    return pSuccessor;
}


/*********************
 *********************
 *****  Segment  *****
 *********************
 *********************/

PrivateFnDef bool Segment_Convert (
    PS_SH *pSegment, unsigned int sSegment, char const *pSegmentPathName
) {
    M_CPreamble*		pNextContainer;
    M_CPreamble*		pNullContainer;

    switch (PS_SH_DataFormat (pSegment)) {
    case Vk_DataFormatNative:
	break;

    case Vk_DataFormatReversed:
	gettime (&OperationStartTime);

	pNextContainer = (M_CPreamble*)(pSegment + 1);
	pNullContainer = (M_CPreamble*)((pointer_t)pSegment + sSegment);

	pSegment->m_dataFormat = Vk_DataFormatChanging;

	Vk_ReverseSizeable (&pSegment->m_ctOffset);
	Vk_ReverseSizeable (&pSegment->m_softwareVersion);
	Vk_ReverseSizeable (&pSegment->m_ctSize);
	TID_Convert (&pSegment->m_tid);
	RID_Convert (&pSegment->m_rid);

	pSegment->m_checksum = 0;
	while (IsntNil (pNextContainer) && pNextContainer < pNullContainer) {
            if (0xffffffff != pNextContainer->nSize)
                pNextContainer = Container_Convert (pNextContainer, pNullContainer, &pSegment->m_checksum);
            else {
                pSegment->m_checksum ^= pNextContainer->nSize;
                pNextContainer = reinterpret_cast<M_CPreamble*>(&pNextContainer->nSize + 1);
            }
 	}
	if (IsNil (pNextContainer)) {
	    MessageType_DisplayApplicationMessage (
		MessageType_Error, "Segment %s: Conversion Error", pSegmentPathName
	    );
	    return false;
	}

	pSegment->m_dataFormat = Vk_DataFormatNative;

	gettime (&OperationEndTime);

	if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "Segment %s: Converted %9.3f sec.",
	    pSegmentPathName,
	    OperationTime ()
	);
	break;

    case Vk_DataFormatChanging:
	MessageType_DisplayApplicationMessage (
	    MessageType_Error, "Segment %s: Previous Conversion Incomplete", pSegmentPathName
	);
	return false;

    default:
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Segment %s: Unrecognized Data Format %u",
	    pSegmentPathName,
	    PS_SH_DataFormat (pSegment)
	);
	return false;
    }

    return true;
}


/*********************************
 *********************************
 *****  Explicit Conversion  *****
 *********************************
 *********************************/

PrivateFnDef bool ExplicitConverter () {
    bool	fExplicitSegmentsSpecified = false;
    char const*	pSegmentPathName;
    VkMemory	iSegmentMapping;

    GOPT_SeekExtraArgument (0, 0);
    while (IsntNil (pSegmentPathName = GOPT_GetExtraArgument ())) {
	bool fSegmentForRemoval = false;
	fExplicitSegmentsSpecified = true;

	if (IsntNil (GOPT_GetValueOption ("SlaveNDFPathName")) ||
	    IsntNil (GOPT_GetValueOption ("MasterNDFPathName"))) {
	    RaiseApplicationException (
	       "Mode Ambiguity: NDF option specified and explicit segment (%s) conversion requested -- Exiting.",
	       pSegmentPathName
	   );
	}
	
	if (chmod (pSegmentPathName, 0644) < 0) MessageType_DisplaySystemMessage (
	    MessageType_Warning, "Segment %s: Could Not Mark Read-Write", pSegmentPathName
	);
	if (!iSegmentMapping.Map (pSegmentPathName, false, g_xMappingMode)) {
	    MessageType_DisplaySystemMessage (
		MessageType_Error, "Segment %s: Mapping Error", pSegmentPathName
	    );
	    continue;
	}

	if (!Segment_Convert (
		(PS_SH*)iSegmentMapping.RegionAddress (),
		iSegmentMapping.RegionSize (),
		pSegmentPathName
	    )
	) fSegmentForRemoval = true;

	iSegmentMapping.Sync ();
	iSegmentMapping.Destroy ();

	if (chmod (pSegmentPathName, 0444) < 0) MessageType_DisplaySystemMessage (
	    MessageType_Warning, "Segment %s: Could Not Mark Read-Only", pSegmentPathName
	);

	if (fSegmentForRemoval) {
	    remove (pSegmentPathName);
	    MessageType_DisplayApplicationMessage (
		MessageType_Info,
		"Segment %s: %s",
		pSegmentPathName,
		0 == access (pSegmentPathName, 0) ? "Removal Failed" : "Removed"
	    );
	}
    }

    return fExplicitSegmentsSpecified;
}


/***************************
 ***************************
 *****  Schedule Entry *****
 ***************************
 ***************************/

/************************
 *****  Comparison  *****
 ************************/

int __cdecl VScheduleEntry::Compare (VScheduleEntry const* pThis, VScheduleEntry const* pOther) {
    return pThis->m_oStructure < pOther->m_oStructure
	? -1
	: pThis->m_oStructure > pOther->m_oStructure
	? 1
	: 0;
}

/**************************
 *****  Construction  *****
 **************************/

VScheduleEntry::VScheduleEntry (VScheduleEntry* pNext, PS_Type_FO oStructure) {
    m_pNext		= pNext;
    m_oStructure	= oStructure;
}


/**********************
 **********************
 *****  Schedule  *****
 **********************
 **********************/

/****************************
 *****  Element Update  *****
 ****************************/

void VSchedule::Add (
    PS_Type_FO oStructure, PS_Type_FO oStructureLimit, char const *pStructureName
)
{
    static VScheduleEntry* pFreeList	= NilOf (VScheduleEntry*);
    static VScheduleEntry* pFreeLimit	= NilOf (VScheduleEntry*);

    /*****  Ignore 'nil' offsets, ...  *****/
    if (PS_FO_IsNil (oStructure) || 0 == oStructure)
	return;

    /*****  Verify the offset, ...  *****/
    if (oStructure >= oStructureLimit) {
	RaiseApplicationException (
	    "Invalid %s Offset %u", pStructureName, oStructure
	);
	return;
    }

    /*****  First look for the node, ...  *****/
    VScheduleEntry iKey (m_pSchedule, oStructure);
    if (STD_tfind (&iKey, &m_pSearchTree, (VkComparator)&VScheduleEntry::Compare))
	return;

    /*****  ... inserting it only if it wasn't found:  *****/
    if (pFreeList >= pFreeLimit) {
	pFreeList  = (VScheduleEntry*)allocate (4096 * sizeof (VScheduleEntry));
	pFreeLimit = pFreeList + 4096;
    }
    VScheduleEntry *pKey = pFreeList++;
    *pKey = iKey;
    STD_tsearch (pKey, &m_pSearchTree, (VkComparator)&VScheduleEntry::Compare);
    m_pSchedule = pKey;
}


/****************************
 *****  Element Access  *****
 ****************************/

int VSchedule::Next (PS_Type_FO* pResult) {
    VScheduleEntry* pNextEntry;

    if (IsNil (pNextEntry = m_pSchedule))
	return false;

    m_pSchedule = pNextEntry->m_pNext;
    pNextEntry->m_pNext	= NilOf (VScheduleEntry*);

    *pResult = pNextEntry->m_oStructure;
    return true;
}


/**************************
 *****  Construction  *****
 **************************/

int VSchedule::Construct () {
    m_pSchedule		= NilOf (VScheduleEntry*);
    m_pSearchTree	= NilOf (void*);

    return true;
}


/******************************
 ******************************
 *****  Segment Identity  *****
 ******************************
 ******************************/

/*******************
 *****  Query  *****
 *******************/

bool VSegmentIdentity::operator == (VSegmentIdentity const& rOther) const {
    return m_xSpace	== rOther.m_xSpace
    &&	   m_xSegment	== rOther.m_xSegment
    &&	   m_iTID	== rOther.m_iTID
    &&	   m_iRID	== rOther.m_iRID;
}

/************************
 *****  Conversion  *****
 ************************/

bool VSegmentIdentity::Convert (unsigned int xDataFormat) {
    switch (xDataFormat) {
    case Vk_DataFormatReversed:
	Vk_ReverseSizeable (&m_xSpace);
	Vk_ReverseSizeable (&m_xSegment);
	TID_Convert (&m_iTID);
	RID_Convert (&m_iRID);
	return true;

    case Vk_DataFormatNative:
	return true;

    default:
	return false;
    }
}

/**************************
 *****  Construction  *****
 **************************/

VSegmentIdentity::VSegmentIdentity (
    unsigned int xSpace, unsigned int xSegment, PS_TID const &rTID, PS_RID const &rRID
) : m_xSpace (xSpace), m_xSegment (xSegment), m_iTID (rTID), m_iRID (rRID)
{
}


/****************************
 ****************************
 *****  Segment Handle  *****
 ****************************
 ****************************/

/************************
 *****  Un-Mapping  *****
 ************************/

void VSegmentHandle::Unmap (bool fUnlock) {
    if (!m_fMappingValid)
	return;

    if (fUnlock)
	m_cMappingLocks--;

    if (m_cMappingLocks > 0)
	return;

    m_fMappingValid = false;
    m_iMapping.Destroy ();
}

/*****************************
 *****  Synchronization  *****
 *****************************/

void VSegmentHandle::Sync () {
    if (!m_fMappingValid)
	return;

    gettime (&OperationStartTime);
    m_iMapping.Sync ();
    gettime (&OperationEndTime);
    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info,
	"Segment %s: Committed %9.3f sec.",
	IsNil (m_pSavedPathName)
	    ? m_pPathName
	    : m_pSavedPathName,
	OperationTime ()
    );
}


/*********************
 *****  Mapping  *****
 *********************/

bool VSegmentHandle::Map (bool fReadOnly, bool fLockMapping) {
    if (m_fMappingValid) {
	if (fReadOnly || !m_fReadOnly) {
	/*****  ... current mapping OK:  *****/
	    if (fLockMapping)
		m_cMappingLocks++;
	    return true;
	}

	if (m_cMappingLocks > 0)
	/*****  ... current mapping not OK but locked:  *****/
	    return false;

	m_fMappingValid = false;
	m_iMapping.Destroy ();
    }
    if (!fReadOnly){ /** Make the file owner writeable if we can ... */
	if (chmod (m_pPathName, 0644) < 0)
	    MessageType_DisplaySystemMessage
		(MessageType_Warning, "Segment %s: Could Not Mark Read-Write", m_pPathName);
    }
    if (m_iMapping.Map (m_pPathName, fReadOnly, g_xMappingMode)) {
	m_fReadOnly	= fReadOnly;
	m_fMappingValid	= true;

	if (fLockMapping)
	    m_cMappingLocks++;
	return true;
    }

    return false;
}


bool VSegmentHandle::MapNew (unsigned int sSegment) {
    Unmap (false);

    if (m_fMappingValid || !m_iMapping.MapNew (m_pPathName, sSegment, g_xMappingMode))
	return false;

    m_fReadOnly		= false;
    m_fMappingValid	= true;
    m_cMappingLocks++;

    return true;
}


/************************
 *****  Conversion  *****
 ************************/

bool VSegmentHandle::ConvertSegment () {
    bool aMatch			= false;
    bool anUnconvertedMatch	= false;
    bool segmentDamaged		= false;

/*****  Return if the segment has already been verified, ...  *****/
    if (m_fSegmentValid)
	return true;

/*****  Attempt to find and verify the segment, ...  *****/
    if (Map (true, false)) {
	PS_SH *pSH = (PS_SH*)m_iMapping.RegionAddress ();
	PS_TID tid;
	PS_RID rid;

	switch (PS_SH_DataFormat (pSH)) {
	case Vk_DataFormatNative:
	    aMatch = PS_SH_TID (pSH) == m_iIdentity.m_iTID
		  && PS_SH_RID (pSH) == m_iIdentity.m_iRID;
	    break;

	case Vk_DataFormatReversed:
	    tid = PS_SH_TID (pSH);
	    rid = PS_SH_RID (pSH);

	    TID_Convert (&tid);
	    RID_Convert (&rid);

	    anUnconvertedMatch = tid == m_iIdentity.m_iTID && rid == m_iIdentity.m_iRID;
	    break;
	
	case Vk_DataFormatChanging:
	    MessageType_DisplayApplicationMessage (
		MessageType_Error,
		"Segment %s: Previous Conversion Incomplete",
		m_pPathName
	    );
	    segmentDamaged = true;
	    break;

	default:
	    MessageType_DisplayApplicationMessage (
		MessageType_Error,
		"Segment %s: Unrecognized Data Format %u",
		m_pPathName,
		PS_SH_DataFormat (pSH)
	    );
	    segmentDamaged = true;
	    break;
	}

	Unmap (false);
    }

/*****  If the segment exists and is unconverted, convert it, ...  *****/
    if (!anUnconvertedMatch) {
    }
    else if (Map (false, false)) {
	aMatch = Segment_Convert (
	    (PS_SH*)m_iMapping.RegionAddress (),
	    m_iMapping.RegionSize (),
	    IsNil (m_pSavedPathName) ? m_pPathName : m_pSavedPathName
	);
	segmentDamaged = !aMatch;

	Sync  ();
	Unmap (false);
    }
    else MessageType_DisplaySystemMessage (
	MessageType_Error, "Segment %s: Mapping Error", m_pPathName
    );

/*****  If the segment matches, we're done, ...  *****/
    if (aMatch) {
#if !defined(__VMS)
	if (chmod (m_pPathName, 0444) < 0) MessageType_DisplaySystemMessage (
	    MessageType_Warning, "Segment %s: Could Not Mark Read-Only", m_pPathName
	);
#endif

	m_fSegmentValid = true;
	return true;
    }

/*****  ... otherwise remove damaged segments, ...  *****/
    if (segmentDamaged)
	remove (m_pPathName);

/*****  ... and return.  *****/
    return false;
}


/*******************************
 *****  Segment Retrieval  *****
 *******************************/

bool VSegmentHandle::FindOrRetrieveSegment () {
/*****  If the segment matches and can be converted, we're done, ...  *****/
    if (ConvertSegment ())
	return true;

/*****  ... otherwise, try to get it from the master...  *****/
    if (!g_iConversionAgent.Send (
	    this,
	    m_xMySelf,
	    VkMessageTargetIndex_Root,
	    MessageIndex_OpenSegment,
	    sizeof (m_iIdentity),
	    &m_iIdentity
	)
    ) return false;

/*****  ... and report if the attempt was begun:  *****/
    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info, "Segment %s: Requested From Server", m_pPathName
    );

    IncrementIncomingSegmentCount ();
    return true;
}


/******************************
 *****  Message Handlers  *****
 ******************************/

void VSegmentHandle::PrepareIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *Unused(pAgent)
) {
    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_SetContent:
	/** receive into temp file, in case transmission is interrupted **/
	m_pSavedPathName = m_pPathName;
	m_pPathName = strdup (MakeMessage ("%s%s", m_pSavedPathName,"x"));

	if (MapNew (pMessage->Size ()))
	    pMessage->AttachBody ((pointer_t)m_iMapping.RegionAddress ());
	else RaiseApplicationException (
	    "Segment %s: Cannot Create", m_pPathName
	);

	if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "Segment %s: Receiving %9u Bytes",
	    m_pSavedPathName,
	    m_iMapping.RegionSize ()
	);
	gettime (&OperationStartTime);
	break;
    }
}


void VSegmentHandle::ReceiveIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *pAgent
) {
    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_ObjectAck:
	if (!pAgent->Send (
		this,
		pMessage->Target (),
		pMessage->Sender (),
		MessageIndex_GetContent,
		0,
		NilOf (void const*)
	    )
	) {
	    DecrementIncomingSegmentCount ();
	}
	else if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info, "Segment %s: Content Requested", m_pPathName
	);
	break;

    case MessageIndex_ObjectNack:
	DecrementIncomingSegmentCount ();
	MessageType_DisplayApplicationMessage (
	    MessageType_Error, "Segment %s: Not Available From Server", m_pPathName
	);
	break;

    case MessageIndex_GetContent:
	pAgent->Send (
	    this,
	    pMessage->Target (),
	    pMessage->Sender (),
	    MessageIndex_SetContent,
	    0,			/*  Body setup deferred until transmission.  */
	    NilOf (void const*)
 	);
	break;

    case MessageIndex_SetContent:
	gettime (&OperationEndTime);
	if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "Segment %s: Received  %9.3f seconds, %10.3f KB/second",
	    m_pSavedPathName,
	    OperationTime (),
	    OperationRate (m_iMapping.RegionSize ())
	);
	pMessage->DetachBody ();

	DecrementIncomingSegmentCount ();
	ConvertSegment ();
	Unmap (true);

	/**  Transmission complete ... move segment into permanent location **/
	remove (m_pSavedPathName);
	if (rename (m_pPathName, m_pSavedPathName) < 0) MessageType_DisplaySystemMessage (
	    MessageType_Error, "Segment %s: Could Not be renamed", m_pPathName
	);
#if defined(__VMS)
	if (chmod (m_pSavedPathName, 0444) < 0) MessageType_DisplaySystemMessage (
	    MessageType_Warning, "Segment %s: Could Not Mark Read-Only", m_pPathName
	);
#endif
	deallocate ((void*)m_pPathName);
	m_pPathName = m_pSavedPathName;
	m_pSavedPathName = (char *) NULL;
	break;
    }
}


void VSegmentHandle::PrepareOutgoingMessage (
    VkMessage *pMessage, VkMessageShippingAgent *Unused(pAgent)
) {
    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_SetContent:
	if (Map (true, true)) {
	    if (Verbose) MessageType_DisplayApplicationMessage (
		MessageType_Info, "Segment %s: Preparing to Send %u Bytes",
		m_pPathName, m_iMapping.RegionSize ()
	    );
	    pMessage->SetSizeTo (m_iMapping.RegionSize ());
	    pMessage->AttachBody ((pointer_t)m_iMapping.RegionAddress ());
	}
	else {
	    if (Verbose) MessageType_DisplayApplicationMessage (
		MessageType_Info, "Segment %s: Could Not Map", m_pPathName
	    );
	    pMessage->SetTypeTo (MessageIndex_ObjectNack);
	    pMessage->SetSizeTo (0);
	}
	break;
    }
}

void VSegmentHandle::DiscardOutgoingMessage (
    VkMessage *pMessage, VkMessageShippingAgent *Unused(pAgent)
) {
    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_SetContent:
	pMessage->DetachBody ();
	Unmap (true);
	break;
    }
}


/*************************
 *****  Destruction  *****
 *************************/

VSegmentHandle::~VSegmentHandle () {
    if (m_fMappingValid) {
	m_fMappingValid = false;
	m_iMapping.Destroy ();
    }

    deallocate ((void*)m_pPathName);
}

/**************************
 *****  Construction  *****
 **************************/

VSegmentHandle::VSegmentHandle (
    VSpaceHandle *pSpace, PS_SD *pSD, char const *pPathName
) : m_iIdentity (pSpace->SpaceIndex (), pSD->segment, pSD->tid, pSD->rid)
{
    m_xMySelf = g_iConversionAgent.Add (this);

    m_pSpaceHandle	= pSpace;
    m_pPathName		= strdup (pPathName);
    m_pSavedPathName	= NilOf (char *);
    m_fSegmentValid	=
    m_fMappingValid	= false;
    m_cMappingLocks	= 0;
}


/**************************
 **************************
 *****  Space Handle  *****
 **************************
 **************************/

/********************
 *****  Access  *****
 ********************/

VSegmentHandle* VSpaceHandle::SegmentHandle (unsigned int xSegment) const {
    return xSegment >= m_xMinVectorSegment && xSegment <= m_xMaxVectorSegment
	? m_pSegmentVector[xSegment - m_xMinVectorSegment]
	: NilOf (VSegmentHandle*);
}


/********************************
 *****  Directory Creation  *****
 ********************************/

int VSpaceHandle::FindOrCreateDirectory () const {
    return ::FindOrCreateDirectory (m_pPathName);
}


/*******************************
 *****  Directory Cleanup  *****
 *******************************/

#ifdef __VMS
char const * const VSpaceHandle::g_pDSegsDirectoryName = "_dsegs";
#else
char const * const VSpaceHandle::g_pDSegsDirectoryName = ".dsegs";
#endif

void VSpaceHandle::CleanupOSD () const {
    DIR *pDirEnumerator;
    if (!::FindOrCreateDirectory (MakeMessage ("%s/%s", m_pPathName, g_pDSegsDirectoryName)) ||
	IsNil (pDirEnumerator = opendir (m_pPathName)))
    {
	MessageType_DisplaySystemMessage (
	    MessageType_Error, "Space %s: Cannot Access '%s' Directory", m_pPathName, g_pDSegsDirectoryName
	);
	return;
    }

    STD_DirectoryEntryType const *pDirEntry;
    while (IsntNil (pDirEntry = readdir (pDirEnumerator))) {
	char const *pFileName = STD_DirectoryEntryName (pDirEntry);
	unsigned int sFileName = strspn (pFileName, "0123456789");
#ifdef __VMS
	if (pFileName[sFileName] == '.')
	    sFileName++;
#endif
	if (sFileName == STD_DirectoryEntryNameLen (pDirEntry) && (unsigned int)atoi (pFileName) < m_xMinModelSegment) {
	    char iOldName[1024];
	    sprintf (iOldName, "%s/%*.*s", m_pPathName, sFileName, sFileName, pFileName);

	    char iNewName[1024];
	    sprintf (iNewName, "%s/%s/%*.*s", m_pPathName, g_pDSegsDirectoryName, sFileName, sFileName, pFileName);

	    remove (iNewName);
	    rename (iOldName, iNewName);
	}
    }
    closedir (pDirEnumerator);
}


/*********************************
 *****  Model Bounds Update  *****
 *********************************/

void VSpaceHandle::UpdateModelBounds (PS_SVD* pSVD) {
    m_xMinModelSegment = V_Min (
	m_xMinModelSegment, (unsigned int)pSVD->minSegment
    );
    m_xMaxModelSegment = V_Max (
	m_xMaxModelSegment, (unsigned int)pSVD->maxSegment
    );
    if (PS_FO_IsNil (m_oUndeclaredSDChain) && PS_NetworkDirectorySignature == pSVD->signature)
	m_oUndeclaredSDChain = pSVD->sd;
}


/*********************************
 *****  Segment Declaration  *****
 *********************************/

VSegmentHandle* VSpaceHandle::DeclareSegment (PS_SD* pSD) {
    if (IsNil (m_pSegmentVector)) {
	m_xMinVectorSegment = m_xMinModelSegment;
	m_xMaxVectorSegment = m_xMaxModelSegment;

	if (m_xMinVectorSegment > pSD->segment)
	    m_xMinVectorSegment = pSD->segment;
	if (m_xMaxVectorSegment < pSD->segment)
	    m_xMaxVectorSegment = pSD->segment;

	unsigned int sSegmentVector = SegmentVectorSize ();
	VSegmentHandle**pSegmentVector = (VSegmentHandle**)allocate (
	    sizeof (VSegmentHandle*) * sSegmentVector
	);
	for (unsigned int xSegment = 0; xSegment < sSegmentVector; xSegment++)
	    pSegmentVector[xSegment] = NilOf (VSegmentHandle*);

	m_pSegmentVector = pSegmentVector;
    }
    else if (m_xMinVectorSegment > pSD->segment) {
	unsigned int sSegmentVectorDisplacement = m_xMinVectorSegment - pSD->segment;
	unsigned int sSegmentVector = (unsigned int)(m_xMaxVectorSegment - pSD->segment + 1);
	m_pSegmentVector = (VSegmentHandle**)reallocate (
	    m_pSegmentVector, sizeof (VSegmentHandle*) * sSegmentVector
	);
	memmove (
	    m_pSegmentVector + sSegmentVectorDisplacement,
	    m_pSegmentVector ,
	    SegmentVectorSize () * sizeof (VSegmentHandle*)
	);
	for (unsigned int xSegment = 0; xSegment < sSegmentVectorDisplacement; xSegment++)
	    m_pSegmentVector [xSegment] = NilOf (VSegmentHandle*);

	m_xMinVectorSegment = pSD->segment;
    }
    else if (m_xMaxVectorSegment < pSD->segment) {
	unsigned int sSegmentVector = (unsigned int)(pSD->segment - m_xMinVectorSegment + 1);
	m_pSegmentVector = (VSegmentHandle**)reallocate (
	    m_pSegmentVector, sizeof (VSegmentHandle*) * sSegmentVector
	);
	for (
	    unsigned int xSegment = SegmentVectorSize (); 
	    xSegment < sSegmentVector;
	    xSegment++
	) m_pSegmentVector[xSegment] = NilOf (VSegmentHandle*);

	m_xMaxVectorSegment = pSD->segment;
    }

    VSegmentHandle *pSegmentHandle = m_pSegmentVector[pSD->segment - m_xMinVectorSegment];
    if (IsNil (pSegmentHandle)) {
	pSegmentHandle = new VSegmentHandle (
	    this, pSD, MakeMessage ("%s/%u", m_pPathName, pSD->segment)
	);
	m_pSegmentVector[pSD->segment - m_xMinVectorSegment] = pSegmentHandle;
    }

    return pSegmentHandle;
}


/*************************
 *****  Destruction  *****
 *************************/

VSpaceHandle::~VSpaceHandle () {
    if (m_pSegmentVector) {
	unsigned int sSegmentVector = SegmentVectorSize ();

	for (unsigned int xSegment = 0; xSegment < sSegmentVector; xSegment++) {
	    VSegmentHandle* pSegmentHandle = m_pSegmentVector[xSegment];
	    if (pSegmentHandle) {
		m_pSegmentVector[xSegment] = NilOf (VSegmentHandle*);

		delete pSegmentHandle;
	    }
	}

	deallocate (m_pSegmentVector);
	m_pSegmentVector = NilOf (VSegmentHandle**);
    }

    deallocate ((void*)m_pPathName);
    m_pPathName = NilOf (char const*);
}


/**************************
 *****  Construction  *****
 **************************/

VSpaceHandle::VSpaceHandle (
    VNetworkHandle *pNetworkHandle, char const *pOSDPathName, unsigned int xSpace
) : m_pNetworkHandle (pNetworkHandle)
{
    char	iSpacePathName[1024], iLinkPathName[1024], *nlp;
    FILE*	ls;

/*****  First, determine the path name of this space, ...  *****/
    sprintf (iSpacePathName, "%s/%u", pOSDPathName, xSpace);

    while (sprintf (iLinkPathName, "%s/LINK", iSpacePathName), access (iLinkPathName, 0) == 0)
    {
	if (NULL == (ls = fopen (iLinkPathName, "r")))
	    RaiseSystemException ("Space Link %s Open", iLinkPathName);
	if (iSpacePathName != fgets (iSpacePathName, 1024, ls))
	    RaiseSystemException ("Space Link %s Read", iLinkPathName);
	fclose (ls);

	if (IsntNil (nlp = strchr (iSpacePathName, '\n')))
	    *nlp = '\0';
    }

/*****  ... and initialize the structure:  *****/
    m_xMySelf = g_iConversionAgent.Add (this);

    m_pPathName		= strdup (iSpacePathName);
    m_xSpace		= xSpace;
    m_xMinModelSegment	=
    m_xMinVectorSegment	= UINT_MAX;
    m_xMaxModelSegment	=
    m_xMaxVectorSegment	= 0;
    m_oUndeclaredSDChain= PS_FO_Nil;
    m_pSegmentVector	= NilOf (VSegmentHandle**);
}


/****************************
 ****************************
 *****  Network Handle  *****
 ****************************
 ****************************/

/*********************************
 *****  NDF Synchronization  *****
 *********************************/

void VNetworkHandle::Sync () {
    gettime (&OperationStartTime);
    m_iMapping.Sync ();
    gettime (&OperationEndTime);

    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info,
	"NDF %s: Committed %9.3f sec.",
	m_pNDFPathName,
	OperationTime ()
    );
}


/********************************
 *****  Directory Creation  *****
 ********************************/

int VNetworkHandle::FindOrCreateDirectory () const {
    return ::FindOrCreateDirectory (m_pOSDPathName);
}


/*******************************
 *****  Directory Cleanup  *****
 *******************************/

void VNetworkHandle::CleanupOSD () const {
    unsigned int		xSpace;
    VSpaceHandle*		pSpaceHandle;

    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info,
	"Starting OSD Cleanup ... (move segments to dsegs)"
    );

    for (xSpace = 0; xSpace <= M_POP_MaxObjectSpace; xSpace++)
	if (IsntNil (pSpaceHandle = m_pSpaceHandle[xSpace]))
	    pSpaceHandle->CleanupOSD ();
}


/****************************
 *****  Segment Access  *****
 ****************************/

VSegmentHandle* VNetworkHandle::SegmentHandle (
    unsigned int xSpace, unsigned int xSegment
) const {
    VSpaceHandle*		pSpaceHandle;
    VSegmentHandle*		pSegmentHandle;

    if (IsNil (pSpaceHandle = m_pSpaceHandle[xSpace]))
	return NilOf (VSegmentHandle*);

    if (IsNil (pSegmentHandle = pSpaceHandle->SegmentHandle (xSegment))) {
	PS_SD iSD;

	iSD.segment = xSegment + 1;
	while (PS_FO_IsntNil (pSpaceHandle->m_oUndeclaredSDChain) && iSD.segment != xSegment)
	{
	    Get (pSpaceHandle->m_oUndeclaredSDChain, &iSD, sizeof (iSD));
	    pSegmentHandle = pSpaceHandle->DeclareSegment (&iSD);
	    pSpaceHandle->m_oUndeclaredSDChain = iSD.predecessor;
	}
    }

    return pSegmentHandle;
}


/*******************************
 *****  Segment Retrieval  *****
 *******************************/

void VNetworkHandle::FindOrRetrieveSegments () {
    unsigned int		xSpace;
    unsigned int		xSegment;
    VSpaceHandle*		pSpaceHandle;
    VSegmentHandle*		pSegmentHandle;

    if (FindOrCreateDirectory ()			/* OS directory must exist */
    ) for (
	xSpace = 0; xSpace <= M_POP_MaxObjectSpace; xSpace++
    ) if (
	IsntNil (					/* space must exist */
	    pSpaceHandle = m_pSpaceHandle[xSpace]
	) && pSpaceHandle->FindOrCreateDirectory (	/* space directory must exist */
	)
    ) for (
	xSegment = pSpaceHandle->m_xMinModelSegment; xSegment <= pSpaceHandle->m_xMaxModelSegment; xSegment++
    ) if (
	IsntNil (					/* segment must be declared */
	    pSegmentHandle = SegmentHandle (xSpace, xSegment)
	) && pSegmentHandle->FindOrRetrieveSegment (	/* segment must exist or be retrievable */
	)
    );
    else if (
	Verbose
    ) MessageType_DisplayApplicationMessage (
	MessageType_Info, "Segment Needed: %u/%u", xSpace, xSegment
    );
}


/*******************************
 *****  Segment Modelling  *****
 *******************************/

void VNetworkHandle::BuildSegmentModel () {
    unsigned int		cBaseVersionsToModel;
    int				notDone;
    PS_Type_FO			oNVD;
    PS_NVD			iNVD;
    unsigned int		xSpace;

/*****  ... determine the number of compactions to overlook, ...  *****/
    cBaseVersionsToModel = atoi (GOPT_GetValueOption ("BaseVersionCount"));

/*****  ... determine the range of segments required in each space, ... *****/
    oNVD    = PS_NDH_CurrentNetworkVersion (m_iNDH);
    notDone = true;
    while (PS_FO_IsntNil (oNVD) && notDone) {
	Get (oNVD, &iNVD, sizeof (iNVD));
	if (PS_NVD_SoftwareVersion (iNVD) >= 2) {
	    VSpaceHandle*	pSpace;
	    int			xSpace;
	    int			cSRD;
	    PS_Type_FO		oSRD = iNVD.srv + sizeof (int);
	    PS_SRD		iSRD;
	    PS_SVD		iSVD;

	    Get (iNVD.srv, &cSRD, sizeof (cSRD));
	    for (xSpace = 0; xSpace < cSRD; xSpace++) {
		Get (oSRD, &iSRD, sizeof (iSRD));
		switch (iSRD.role) {
		case PS_Role_Nonextant:
		    break;
		default:
		    Get (iSRD.svd, &iSVD, sizeof (iSVD));

		    if (IsNil (pSpace = m_pSpaceHandle[iSVD.spaceIndex])) {
			pSpace = m_pSpaceHandle[iSVD.spaceIndex] = new VSpaceHandle (
			    this, m_pOSDPathName, xSpace
			);
		    }

		    pSpace->UpdateModelBounds (&iSVD);
		    break;
		}
		oSRD += sizeof (iSRD);
	    }

	    if (PS_NVD_BaseVersion (iNVD))
		notDone = cBaseVersionsToModel-- > 0;
	}
	else RaiseApplicationException (
	    "Network Version %d Produced By Outdated Version %d Backend -- Segment Analysis May Be Incomplete.",
	    oNVD,
	    iNVD.softwareVersion
	);

	oNVD = PS_NVD_PreviousVersion (iNVD);
    }

/*****  ... and display it if in verbose mode.  *****/
    if (Verbose) for (xSpace = 0; xSpace <= M_POP_MaxObjectSpace; xSpace++)
    {
	VSpaceHandle* pSpaceHandle = m_pSpaceHandle[xSpace];

	if (IsntNil (pSpaceHandle)) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "Space %4u: Min Segment = %6u Max Segment = %6u",
	    xSpace,
	    pSpaceHandle->m_xMinModelSegment,
	    pSpaceHandle->m_xMaxModelSegment
	);
    }
}


/********************************************
 *****  Conversion Schedule Processing  *****
 ********************************************/

void VNetworkHandle::ProcessSchedule (
    VSchedule*	pSchedule,
    bool	(VNetworkHandle::*pElementProcessor)(PS_Type_FO),
    char const*	pElementTypeName
)
{
    PS_Type_FO	oStructure;
    unsigned int	cExamined;
    unsigned int	cConverted;

    cExamined = cConverted = 0;
    while (pSchedule->Next (&oStructure)) {
	if ((this->*pElementProcessor) (oStructure))
	    cConverted++;
	cExamined++;
    }
    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info,
	"%6u %18.18s%s Examined, %6u Converted",
	cExamined,
	pElementTypeName,
	1 != cExamined ? "s" : "",
	cConverted
    );
}


/***********************************
 *****  Conversion Scheduling  *****
 ***********************************/

void VNetworkHandle::ScheduleSD (PS_Type_FO oSD) {
    m_iSDSchedule.Add (oSD, m_iMapping.RegionSize (), "SD");
}

void VNetworkHandle::ScheduleSVD (PS_Type_FO oSVD) {
    m_iSVDSchedule.Add (oSVD, m_iMapping.RegionSize (), "SVD");
}

void VNetworkHandle::ScheduleNVD (PS_Type_FO oNVD) {
    m_iNVDSchedule.Add (oNVD, m_iMapping.RegionSize (), "NVD");
}


/*******************************
 *****  String Conversion  *****
 *******************************/

void VNetworkHandle::ConvertString (PS_Type_FO oString) {
    int cChars;

    Get (oString, &cChars, sizeof (cChars));
    Vk_ReverseSizeable (&cChars);
    Put (oString, &cChars, sizeof (cChars));
}


/***************************
 *****  SD Conversion  *****
 ***************************/

bool VNetworkHandle::ConvertSD (PS_Type_FO oSD) {
    bool bConverted = false;

    PS_SD iSD;
    Get (oSD, &iSD, sizeof (iSD));
    if (IsAReversedNilOffset (iSD.extensions)) {
	bConverted = true;

	Vk_ReverseSizeable (&iSD.predecessor);
	TID_Convert	   (&iSD.tid);
	RID_Convert	   (&iSD.rid);
	Vk_ReverseSizeable (&iSD.segment);
	Vk_ReverseSizeable (&iSD.extensions);

	Put (oSD, &iSD, sizeof (iSD));
    }

    ScheduleSD (iSD.predecessor);

    return bConverted;
}


/****************************
 *****  SVD Conversion  *****
 ****************************/

bool VNetworkHandle::ConvertSVD (PS_Type_FO oSVD) {
    bool bConverted = false;

    PS_SVD iSVD;
    Get (oSVD, &iSVD, sizeof (iSVD));
    if (IsAReversedOSDOffset (iSVD.spacePathName)) {
	bConverted = true;

	Vk_ReverseSizeable (&iSVD.spaceIndex);
	Vk_ReverseSizeable (&iSVD.minSegment);
	Vk_ReverseSizeable (&iSVD.maxSegment);
	Vk_ReverseSizeable (&iSVD.rootSegment);
	Vk_ReverseSizeable (&iSVD.origRootSegment);
	Vk_ReverseSizeable (&iSVD.ctOffset);
	Vk_ReverseSizeable (&iSVD.previousSVD);
	Vk_ReverseSizeable (&iSVD.spacePathName);
    }

    if (IsAReversedSignature (iSVD.signature)) {
	Vk_ReverseSizeable (&iSVD.signature);
	Vk_ReverseSizeable (&iSVD.sd);
	Vk_ReverseSizeable (&iSVD.mp);
	Vk_ReverseSizeable (&iSVD.extensions);
    }

    if (bConverted)
	Put (oSVD, &iSVD, sizeof (iSVD));

    ScheduleSVD (iSVD.previousSVD);

    if (PS_NetworkDirectorySignature == iSVD.signature) {
	ScheduleSD  (iSVD.sd);
	ScheduleSVD (iSVD.mp);
    }
    
    return bConverted;
}


/****************************
 *****  SRV Conversion  *****
 ****************************/

void VNetworkHandle::ConvertSRV (PS_Type_FO oSRV) {
    int		cSRD;
    PS_Type_FO	oSRD = oSRV + sizeof (int);
    PS_SRD	iSRD;

    Get (oSRV, &cSRD, sizeof (cSRD));
    Vk_ReverseSizeable (&cSRD);
    Put (oSRV, &cSRD, sizeof (cSRD));

    while (cSRD-- > 0) {
	union fields_t {
	    struct forward_t {
		unsigned int	role	: 32 - PS_FO_FieldWidth,
				svd	: PS_FO_FieldWidth;
	    }		    inStructOrder;
	    struct reverse_t {
		unsigned int	svd	: PS_FO_FieldWidth,
				role	: 32 - PS_FO_FieldWidth;
	    }		    inReversedOrder;
	} fields;

	Get (oSRD, &iSRD, sizeof (iSRD));

	fields.inStructOrder.role	= iSRD.role;
	fields.inStructOrder.svd	= iSRD.svd;
	Vk_ReverseSizeable (&fields);
	iSRD.role	= fields.inReversedOrder.role;
	iSRD.svd	= fields.inReversedOrder.svd;

	Put (oSRD, &iSRD, sizeof (iSRD));

	oSRD += sizeof (iSRD);
    }
}


/****************************
 *****  NVD Conversion  *****
 ****************************/

bool VNetworkHandle::ConvertNVD (PS_Type_FO oNVD) {
    bool bConverted = false;
    union flags_t {
	struct forward_t {
	    unsigned int	baseVersion	:  1,
				unusedFlags	: 31;
	}		    inStructOrder;
	struct reverse_t {
	    unsigned int	unusedFlags	: 31,
				baseVersion	:  1;
	}		    inReversedOrder;
    } flags;

    PS_NVD iNVD;
    Get (oNVD, &iNVD, sizeof (iNVD));
    if (IsAReversedOSDOffset (iNVD.newSpacePathName)) {
	bConverted = true;

	UID_Convert	   (&iNVD.creator);
	TS_Convert	   (&iNVD.commitTimeStamp);
	Vk_ReverseSizeable (&iNVD.softwareVersion);
	Vk_ReverseSizeable (&iNVD.srv);
	Vk_ReverseSizeable (&iNVD.newSpacePathName);
	Vk_ReverseSizeable (&iNVD.previousVersion);
	Vk_ReverseSizeable (&iNVD.previousNVDChain);
	Vk_ReverseSizeable (&iNVD.updateThread);
	Vk_ReverseSizeable (&iNVD.accessedVersion);
	Vk_ReverseSizeable (&iNVD.directoryVersion);
    }

    if (IsAReversedSignature (iNVD.signature)) {
	Vk_ReverseSizeable (&iNVD.signature);

	flags.inStructOrder.baseVersion = iNVD.baseVersion;
	flags.inStructOrder.unusedFlags = iNVD.unusedFlags;
	Vk_ReverseSizeable (&flags);
	iNVD.baseVersion = flags.inReversedOrder.baseVersion;
	iNVD.unusedFlags = flags.inReversedOrder.unusedFlags;

	TID_Convert	   (&iNVD.tid);
	Vk_ReverseSizeable (&iNVD.td);
	Vk_ReverseSizeable (&iNVD.mp);
	Vk_ReverseSizeable (&iNVD.annotation);
	Vk_ReverseSizeable (&iNVD.extensions);
    }

    if (bConverted) {
	Put (oNVD, &iNVD, sizeof (iNVD));

	if (PS_NVD_SoftwareVersion (iNVD) >= 2)
	    ConvertSRV (iNVD.srv);
	else MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Data Base Version %d Produced By Outdated Version %d Backend -- Version Not Converted.",
	    oNVD,
	    iNVD.softwareVersion
	);

	if (PS_NetworkDirectorySignature == iNVD.signature && PS_FO_IsntNil (iNVD.annotation))
	    ConvertString (iNVD.annotation);
    }

    ScheduleNVD (iNVD.previousVersion);
    ScheduleNVD (iNVD.previousNVDChain);
    ScheduleNVD (iNVD.updateThread);
    ScheduleNVD (iNVD.accessedVersion);

    if (PS_NetworkDirectorySignature == iNVD.signature)
	ScheduleNVD (iNVD.mp);

    if (iNVD.softwareVersion >= 2)
    {
	int		cSRD;
	PS_Type_FO	oSRD = iNVD.srv + sizeof (int);
	PS_SRD		iSRD;

	Get (iNVD.srv, &cSRD, sizeof (cSRD));
	while (cSRD-- > 0) {
	    Get (oSRD, &iSRD, sizeof (iSRD));
	    switch (iSRD.role) {
	    case PS_Role_Nonextant:
		break;
	    default:
		ScheduleSVD (iSRD.svd);
		break;
	    }
	    oSRD += sizeof (iSRD);
	}
    }
    else MessageType_DisplayApplicationMessage (
	MessageType_Error,
	"Network Version %d Produced By Outdated Version %d Backend -- Version Not Traversed.",
	oNVD,
	iNVD.softwareVersion
    );

    return bConverted;
}


/****************************
 *****  NDF Conversion  *****
 ****************************/

void VNetworkHandle::ConvertNDF () {
    if (IsAReversedSignature (m_iNDH.components.signature))
    {
	Vk_ReverseSizeable (&m_iNDH.components.signature);
	Vk_ReverseSizeable (&m_iNDH.components.ndfVersion);
	Vk_ReverseSizeable (&m_iNDH.components.directoryVersion);
	TS_Convert	   (&m_iNDH.components.modificationTimeStamp);
	Vk_ReverseSizeable (&m_iNDH.components.currentNetworkVersion);
	Vk_ReverseSizeable (&m_iNDH.components.alternateVersionChainHead);
	Vk_ReverseSizeable (&m_iNDH.components.alternateVersionChainMark);
	Vk_ReverseSizeable (&m_iNDH.components.signature2);
	Vk_ReverseSizeable (&m_iNDH.components.layoutType);
	Vk_ReverseSizeable (&m_iNDH.components.generation);

    	Put (0, &m_iNDH, sizeof (m_iNDH));
	ConvertString (sizeof (PS_NDH));
    }

/*****  Schedule the current network version for conversion, ...  *****/
    ScheduleNVD (m_iNDH.components.currentNetworkVersion);

/*****  ... schedule the rollback version for conversion, ...  *****/
    if (m_iNDH.components.alternateVersionChainMark ==
	m_iNDH.components.currentNetworkVersion
    ) ScheduleNVD (m_iNDH.components.alternateVersionChainHead);

/*****  ... build and process the conversion schedules ...  *****/
    if (Verbose) MessageType_DisplayApplicationMessage (
	MessageType_Info, "Processing NDF: %s", m_pNDFPathName
    );

    ProcessSchedule (
	&m_iNVDSchedule, &VNetworkHandle::ConvertNVD, "Network Version"
    );
    ProcessSchedule (
	&m_iSVDSchedule, &VNetworkHandle::ConvertSVD, "Space Version"
    );
    ProcessSchedule (
	&m_iSDSchedule , &VNetworkHandle::ConvertSD , "Segment Descriptor"
    );

/*****  and force all changes to disk:  *****/
    Sync ();
}


/*************************
 *****  Destruction  *****
 *************************/

 void VNetworkHandle::Destroy () {
    m_iMapping.Destroy ();

    for (unsigned int xSpace = 0; xSpace <= M_POP_MaxObjectSpace; xSpace++) {
	VSpaceHandle* pSpace = m_pSpaceHandle[xSpace];
	if (pSpace) {
	    delete pSpace;
	    m_pSpaceHandle[xSpace] = NilOf (VSpaceHandle*);
	}
    }
}


/**************************
 *****  Construction  *****
 **************************/

bool VNetworkHandle::Construct (char const *pPathName, bool fReadOnly) {
    unsigned int		ndfVersion;
    unsigned int		xSpace;
    char			pLinkBuffer[1024], *pNewLine;
    FILE*			pLinkStream;

/*****  Verify that the NDF can be opened, ...  *****/
    if (!m_iMapping.Map (pPathName, fReadOnly, g_xMappingMode)) {
	MessageType_DisplaySystemMessage (
	    MessageType_Error, "NDF %s: Mapping Error", pPathName
	);
	return false;
    }
    else if (!m_iMapping.Refresh ()) {
	MessageType_DisplaySystemMessage (
	    MessageType_Error, "NDF %s: Mapping Refresh Error", pPathName
	);
	return false;
    }

/*****  ... that it is an NDF, ...  *****/
    Get (0, &m_iNDH, sizeof (m_iNDH));

    if (PS_NetworkDirectorySignature == PS_NDH_Signature (m_iNDH))
	ndfVersion = PS_NDH_NDFVersion (m_iNDH);
    else if (IsAReversedSignature (PS_NDH_Signature (m_iNDH))) {
	ndfVersion = PS_NDH_NDFVersion (m_iNDH);
	Vk_ReverseSizeable (&ndfVersion);
    }
    else {
	MessageType_DisplayApplicationMessage (
	    MessageType_Error, "NDF %s: Not An NDF", pPathName
	);
	m_iMapping.Destroy ();
	return false;
    }

/*****  ... and that it can be processed by this software.  *****/
    if (PS_NDFVersionNumber != ndfVersion) {
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "NDF %s: File Version %d Incompatible With Software Version %d",
	    pPathName,
	    ndfVersion,
	    PS_NDFVersionNumber
	);
	m_iMapping.Destroy ();
	return false;
    }

/*****  If the NDF is real, initialize this structure, ...  *****/
    m_xMySelf = g_iConversionAgent.Add (this);

    m_pNDFPathName = pPathName;
    m_pOSDPathName = (char const*)m_iMapping.RegionAddress ()
		   + sizeof (PS_NDH)
		   + sizeof (int);
    m_cIncomingSegments	= 0;

    m_iNVDSchedule.Construct ();
    m_iSVDSchedule.Construct ();
    m_iSDSchedule.Construct ();

    for (xSpace = 0; xSpace <= M_POP_MaxObjectSpace; xSpace++)
	m_pSpaceHandle[xSpace] = NilOf (VSpaceHandle*);

/*****  ... adjust the OSD path name if appropriate, ...  *****/
    sprintf (pLinkBuffer, "%s.OSDPATH", pPathName);

#if defined(_WIN32)
    pLinkStream = fopen (pLinkBuffer, "rt");
#else
    pLinkStream = fopen (pLinkBuffer, "r");
#endif
/*****  If NDF.OSDPATH doesn't exist, assume the directory in which the NDF lives, ... *****/
    if (IsNil (pLinkStream)) {
	char const* pLastSlash = strrchr (m_pNDFPathName, '/');
	if (!pLastSlash)
	    m_pOSDPathName = strdup ("");
	else {
	    size_t const sDirname = static_cast<size_t>(pLastSlash - m_pNDFPathName);
	    char  *const pOSDPathName = strdup (m_pNDFPathName);
	    pOSDPathName[sDirname] = '\0';
	    m_pOSDPathName = pOSDPathName;
	}
    } else if (IsntNil (fgets (pLinkBuffer, sizeof (pLinkBuffer), pLinkStream))) {
	if (IsntNil (pNewLine = strchr (pLinkBuffer, '\n')))
	    *pNewLine = '\0';
	m_pOSDPathName = strdup (pLinkBuffer);

	fclose (pLinkStream);
    }
    else fclose (pLinkStream);

/*****  ... convert the NDF if opened with write permission, ...  *****/
    if (!fReadOnly)
	ConvertNDF ();

/*****  ... build the segment model, ...  *****/
    BuildSegmentModel ();

/*****  ... and return.  *****/
    return true;
}


VNetworkHandle *VNetworkHandle::New (char const *pPathName, bool fReadOnly) {
    VNetworkHandle *pResult = new VNetworkHandle ();
    if (pResult->Construct (pPathName, fReadOnly))
	return pResult;

    delete pResult;
    return 0;
}


/******************************
 ******************************
 *****  VConversionAgent  *****
 ******************************
 ******************************/

/**********************************
 *****  Message Transmission  *****
 **********************************/

bool VConversionAgent::Send (
    VkMessageController*	pController,
    VkMessageTargetIndex	xSender,
    VkMessageTargetIndex	xTarget,
    unsigned short		xType,
    unsigned int		sBody,
    void const*			pBody
)
{
    return m_pMessageAgent && m_pMessageAgent->Send (
	pController, xSender, xTarget, xType, sBody, pBody
    );
}


/*******************************
 *****  Transient Mapping  *****
 *******************************/

void VConversionAgent::Sync () {
    if (m_fMappingValid)
	m_iMapping.Sync ();
}

void VConversionAgent::Unmap () {
    if (m_fMappingValid) {
	m_fMappingValid = false;
	m_iMapping.Destroy ();
    }
}

bool VConversionAgent::MapNew (unsigned int sMapping) {
    Unmap ();

    if (!m_iMapping.MapNew (m_pSlavePathName, sMapping, g_xMappingMode))
	return false;

    m_fMappingValid = true;

    return true;
}


/*****************************
 *****  Peer Connection  *****
 *****************************/

void VConversionAgent::ConnectPeer () {
/*****  If no slave to maintain, run in server mode, ...  *****/
    if (IsNil (m_pSlavePathName)) {
	VkStream* pIncomingStream;
	VkStream* pOutgoingStream;

	if (IsNil (pIncomingStream = VkStream::New (VkStandardHandle_Input))) {
	}
	else if (IsNil (pOutgoingStream = VkStream::New (VkStandardHandle_Output))) {
	    pIncomingStream->discard ();
	}
	else {
	    m_pMessageAgent = new VkMessageReceivingAgent (
		pIncomingStream,
		MessageChunkSize,
		pOutgoingStream,
		MessageChunkSize,
	    	&m_iMessageTargets
	    );
	}

	return;
    }

/*****  ... otherwise, try to open a connection to the server, ...  *****/
    char const*		pServerAddress;
    VkStream*		pServerStream;
    VkStatus		iStatus;

    if (IsNil (m_pMasterPathName) || IsNil (
	    pServerAddress = GOPT_GetValueOption ("ServerAddress")
	) || IsNil (
	    pServerStream = VkStream::New (
		"inet/stream", pServerAddress, false, false, false, &iStatus
	    )
	)
    ) return;

    m_pMessageAgent = new VkMessageReceivingAgent (
	pServerStream,
	MessageChunkSize,
	pServerStream,
	MessageChunkSize,
	&m_iMessageTargets
    );
}


/******************************
 *****  Slave Processing  *****
 ******************************/

void VConversionAgent::ProcessSlave () {
    if (IsNil (m_pNetworkHandle = VNetworkHandle::New (m_pSlavePathName, false))) {
	m_fNotDone = false;
	return;
    }

    if (!CleanupOnly)
	m_pNetworkHandle->FindOrRetrieveSegments ();

    if (NoCleanup)
	return;

    m_pNetworkHandle->CleanupOSD ();
}


/****************************
 *****  Segment Access  *****
 ****************************/

VSegmentHandle* VConversionAgent::SegmentHandle (unsigned int xSpace, unsigned int xSegment) const {
    return IsntNil (m_pNetworkHandle)
	? m_pNetworkHandle->SegmentHandle (xSpace, xSegment)
	: NilOf (VSegmentHandle*);
}


/***********************
 *****  Execution  *****
 ***********************/

void VConversionAgent::Run () {
    if (IsNil (m_pMessageAgent))
	return;

    VkMessageShippingAgent *pShippingAgent = m_pMessageAgent->ShippingAgent ();

    VkEventSet iEventSet;
    do {
	if (!m_pMessageAgent->ProcessEvent ()) {
	    MessageType_DisplayApplicationMessage (
		MessageType_Error,
		"Can't Process Incoming Event [target:%d, type:%d, size:%d]",
		m_pMessageAgent->m_iIncomingMessage.Target (),
		m_pMessageAgent->m_iIncomingMessage.Type (),
		m_pMessageAgent->m_iIncomingMessage.Size ()
	    );
	    return;
	}
	pShippingAgent->ProcessEvent ();

	iEventSet.clear ();
	iEventSet.Add (m_pMessageAgent->Event ());
	iEventSet.Add (pShippingAgent->Event ());
    } while (
	m_fNotDone && (
	    IsNil (m_pSlavePathName)	||			/* ... a server */
	    IsNil (m_pNetworkHandle)	||			/* ... waiting for network */
	    m_pNetworkHandle->IncomingSegmentCount () > 0	/* ... more segments to come */
	) && IsntNil (iEventSet.getNextTriggeredEvent (Vk_InfiniteWait))
    );
}


/******************************
 *****  Message Handlers  *****
 ******************************/

void VConversionAgent::PrepareIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *Unused(pAgent)
) {
    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_SetContent:
	if (MapNew (pMessage->Size ()))
	    pMessage->AttachBody ((pointer_t)m_iMapping.RegionAddress ());
	else RaiseApplicationException (
	    "NDF %s: Cannot Create", m_pSlavePathName
	);

	if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "NDF %s: Receiving %9u Bytes",
	    m_pSlavePathName,
	    m_iMapping.RegionSize ()
	);
	gettime (&OperationStartTime);
	break;
    }
}


void VConversionAgent::ReceiveIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *pAgent
) {
    VSegmentIdentity*		pSegmentIdentity;
    VSegmentHandle*		pSegmentHandle;

    switch (pMessage->Type ()) {
    default:
	break;
    case MessageIndex_ObjectAck:		/* Received By Client */
	if (!pAgent->Send (
		this,
		pMessage->Target (),
		pMessage->Sender (),
		MessageIndex_GetContent,
		0,
		NilOf (void const*)
	    )
	)
	{
	    MessageType_DisplayApplicationMessage (
		MessageType_Error,
		"Master NDF %s: Content Could Not Be Requested",
		m_pMasterPathName
	    );
	    m_fNotDone = false;
	}
	else if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "Master NDF %s: Content Requested",
	    m_pMasterPathName
	);
	break;

    case MessageIndex_ObjectNack:		/* Received By Client */
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Master NDF %s: Not Available From Server",
	    m_pMasterPathName
	);
	m_fNotDone = false;
	break;

    case MessageIndex_OpenNetwork:		/* Received By Server */
	if (IsNil (m_pNetworkHandle) && IsntNil (
		m_pNetworkHandle = VNetworkHandle::New (
		    strdup ((char const*)pMessage->Body ()), true
		)
	    )
	) {
	    pAgent->Send (
		this,
		pMessage->Target (),
		pMessage->Sender (),
		MessageIndex_ObjectAck,
		0,
		NilOf (void const*)
	    );
	}
	else pAgent->Send (
	    this,
	    pMessage->Target (),
	    pMessage->Sender (),
	    MessageIndex_ObjectNack,
	    0,
	    NilOf (void const*)
	);
	break;

    case MessageIndex_OpenSegment:		/* Received By Server */
	pSegmentIdentity = (VSegmentIdentity*)pMessage->Body ();

	if (pSegmentIdentity->Convert (pMessage->Format ()) && IsntNil (
		pSegmentHandle = SegmentHandle (
		    pSegmentIdentity->m_xSpace, pSegmentIdentity->m_xSegment
		)
	    )
	) pAgent->Send (
	    pSegmentHandle,
	    pSegmentHandle->MySelf (),
	    pMessage->Sender (),
	    MessageIndex_ObjectAck,
	    0,
	    NilOf (void const*)
	);
	else pAgent->Send (
	    this,
	    pMessage->Target (),
	    pMessage->Sender (),
	    MessageIndex_ObjectNack,
	    0,
	    NilOf (void const*)
	);
	break;

    case MessageIndex_GetContent:		/* Received By Server */
	if (IsntNil (m_pNetworkHandle)) pAgent->Send (
	    this,
	    pMessage->Target (),
	    pMessage->Sender (),
	    MessageIndex_SetContent,
	    m_pNetworkHandle->m_iMapping.RegionSize (),
	    m_pNetworkHandle->m_iMapping.RegionAddress ()
 	);
	else pAgent->Send (
	    this,
	    pMessage->Target (),
	    pMessage->Sender (),
	    MessageIndex_ObjectNack,
	    0,
	    NilOf (void const*)
	);
	break;

    case MessageIndex_SetContent:		/* Received By Client */
	gettime (&OperationEndTime);
	if (Verbose) MessageType_DisplayApplicationMessage (
	    MessageType_Info,
	    "NDF %s: Received  %9.3f seconds %10.3f kb/second",
	    m_pSlavePathName,
	    OperationTime (),
	    OperationRate (m_iMapping.RegionSize ())
	);
	pMessage->DetachBody ();

	Sync		();
	Unmap		();
	ProcessSlave	();
	break;
    }
}


/*************************
 *****  Destruction  *****
 *************************/

void VConversionAgent::Destroy () {
    m_iMessageTargets.Destroy ();
    if (m_pMessageAgent)
	m_pMessageAgent->Destroy ();
    if (m_pNetworkHandle)
	m_pNetworkHandle->Destroy ();
    if (m_fMappingValid)
	m_iMapping.Destroy ();
}

/**************************
 *****  Construction  *****
 **************************/

VConversionAgent::VConversionAgent ()
: m_iMessageTargets	(this)
, m_pMessageAgent	(0)
, m_pSlavePathName	(0)
, m_pMasterPathName	(0)
, m_pNetworkHandle	(0)
, m_fNotDone		(true)
, m_fMappingValid	(false)
{
}

void VConversionAgent::Construct () {
/*****  Initialize the fixed slots of this structure, ...  *****/
    m_pSlavePathName	= GOPT_GetValueOption ("SlaveNDFPathName");
    m_pMasterPathName	= GOPT_GetValueOption ("MasterNDFPathName");

/*****  Construct the message agent if appropriate, ...  *****/
    ConnectPeer ();

/*****  If this is a server, do nothing more for now, ...  *****/
    if (IsNil (m_pSlavePathName)) {
#if defined(__hp9000s800) || defined(__hp9000s700)
	syslog (LOG_INFO, "Server starting ...");
#endif
	/* make sure we don't write errors out our communication pipe */
	freopen (GOPT_GetValueOption ("ServerErrorLog"), "a", stderr);
    }

/*****  If a master wasn't specified, process the slave locally, ...  *****/
    else if (IsNil (m_pMasterPathName)) {
	ProcessSlave ();
    }

/*****  ... otherwise, attempt to access the master, ...  *****/
    else if (
	!Send (
	    this,
	    VkMessageTargetIndex_Root,
	    VkMessageTargetIndex_Root,
	    MessageIndex_OpenNetwork,
	    strlen (m_pMasterPathName) + 1,
	    m_pMasterPathName
	)
    )
    {
	MessageType_DisplayApplicationMessage (
	    MessageType_Error,
	    "Master NDF %s: Open Could Not Be Requested",
	    m_pMasterPathName
	);
	m_fNotDone = false;
    }
}


/*********************
 *********************
 *****  Options  *****
 *********************
 *********************/

GOPT_BeginOptionDescriptions

    GOPT_ValueOptionDescription (
	"SlaveNDFPathName"	, 'n', NilOf (char*)	, NilOf (char*)
    )
    GOPT_ValueOptionDescription (
	"MasterNDFPathName"	, 'N', NilOf (char*)	, NilOf (char*)
    )
    GOPT_ValueOptionDescription (
	"ServerAddress"		, 'a', NilOf (char*)	, NilOf (char*)
    )
    GOPT_ValueOptionDescription (
	"LogFacility"		, 'l', NilOf (char*)	, NilOf (char*)
    )
    GOPT_ValueOptionDescription (
	"BaseVersionCount"	, 'b', NilOf (char*)	, "0"
    )
    GOPT_ValueOptionDescription (
	"MessageChunkSize"	, 'm', NilOf (char*)	, "0"
    )
    GOPT_ValueOptionDescription (
	"ServerErrorLog"	, 's', NilOf (char*)	, "/dev/null"
    )
    GOPT_SwitchOptionDescription (
	"NoCleanup"		, 'p', NilOf (char*)
    )
    GOPT_SwitchOptionDescription (
	"VerboseMode"		, 'v', NilOf (char*)
    )
    GOPT_SwitchOptionDescription (
	"CleanupOnly"		, 'c', NilOf (char*)
    )

GOPT_EndOptionDescriptions;


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char* argv[]) {
//  Initialize static transients...
    VStaticTransient::InitializeAllInstances ();

/*****  Start the stream manager, ...  *****/
    StartStreamManager ();

/*****  ... acquire and cache program options, ...  *****/
    GOPT_AcquireOptions (argc, argv);

/*****  ... Global Reclamation Manager Initialization, ...  *****/
    VkMemory::StartGRM (
	argv[0],
	GOPT_GetValueOption ("LogFacility"),
	(VkMemoryTraceCallback) printf, 
	(VkMemoryErrorCallback) RaiseSystemException
    );

    MessageChunkSize = atoi (GOPT_GetValueOption ("MessageChunkSize"));
    if (0 == MessageChunkSize) MessageChunkSize = 1024 * 1024;

    NoCleanup	= GOPT_GetSwitchOption ("NoCleanup") > 0;
    Verbose	= GOPT_GetSwitchOption ("VerboseMode") > 0;
    CleanupOnly = GOPT_GetSwitchOption ("CleanupOnly") > 0;

    if (NoCleanup && CleanupOnly)
	RaiseApplicationException ("NoCleanup(-p) and CleanupOnly(-c) both specified");

    if (!ExplicitConverter ()) {
	g_iConversionAgent.Construct ();
	g_iConversionAgent.Run ();
	g_iConversionAgent.Destroy ();
    }

/*****  ... stop the stream manager, ...  *****/
    StopStreamManager ();

/*****  ... and return:  *****/
    return NormalExitValue;
}

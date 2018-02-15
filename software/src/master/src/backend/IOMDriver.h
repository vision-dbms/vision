#ifndef IOMDriver_Interface
#define IOMDriver_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VTriggerSet.h"

#include "VkStatus.h"

#include "VComputationTrigger.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "iom.h"

class DSC_Descriptor;
class IOMHandle;

class VChannelController;
class VComputationUnit;
class VMutexClaim;
class VReadEvalPrintController;
class VTask;

#include "Vca_VTrigger.h"

#include "VEvaluatorPump.h"


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT IOMDriver : public Vca::VRolePlayer {
    DECLARE_ABSTRACT_RTT (IOMDriver, Vca::VRolePlayer);

//  Friends
    friend class VChannelController;
    friend class VReadEvalPrintController;

/*------------------------------*
 *----  IOMDriver::Options  ----*
 *------------------------------*/
public:
    class Options : public VTransient {
    //  Construction
    public:
	Options (Options const& rOther);
	Options ();

    //  Destruction
    public:
	~Options ();

    //  Access
    public:
	VEvaluatorPump *evaluator () const {
	    return m_pEvaluator;
	}
	bool getEvaluation (Vsa::VEvaluation::Reference &rpEvaluation) const {
	    if (m_pEvaluator)
		rpEvaluation.setTo (m_pEvaluator->incomingClient ());
	    return rpEvaluation.isntNil ();
	}
	unsigned int optionValue (
	    IOMHandle const* pHandle, IOMDriver const* pDriver
	) const;

	bool autoMutex () const {
	    return m_fAutoMutex;
	}
	IOMBinaryFormat binaryFormat () const {
	    return (IOMBinaryFormat)m_xBinaryFormat;
	}
	IOMPromptFormat promptFormat () const {
	    return (IOMPromptFormat)m_xPromptFormat;
	}

    //  Update
    public:
	void SetOptionValue (
	    IOMHandle* pHandle, IOMDriver* pDriver, unsigned int optionValue
	);

	void SetAutoMutexTo (bool fAutoMutex) {
	    m_fAutoMutex = fAutoMutex;
	}
	void SetEvaluatorTo (VEvaluatorPump *pEvaluator);
	void SetBinaryFormatTo (IOMBinaryFormat xBinaryFormat) {
	    m_xBinaryFormat = xBinaryFormat;
	}
	void SetPromptFormatTo (IOMPromptFormat xPromptFormat) {
	    m_xPromptFormat = xPromptFormat;
	}

    //  State
    protected:
	VEvaluatorPump::Reference	m_pEvaluator;
	unsigned int			m_fAutoMutex		: 1;
	unsigned int			m_xPromptFormat		: 1;
	unsigned int			m_xBinaryFormat		: 2;
    };


/*---------------------*
 *----  IOMDriver  ----*
 *---------------------*/

//  Constants
public:
    enum {
	XReadEvalPrintController = 500
    };

//  Driver Table Management
private:
    void AttachToDriverTable	();
    void DetachFromDriverTable	();

//  Construction
protected:
    IOMDriver (Options const& rOptions);

    void CreateOutputEventHandler ();    //  ... called from concrete class constructor

//  Destruction
protected:
    ~IOMDriver ();

//  Access
public:
    bool autoMutex () const {
	return m_iOptions.autoMutex ();
    }

    IOMBinaryFormat binaryFormat () const {
	return m_iOptions.binaryFormat ();
    }

    virtual IOMChannelType channelType () const = 0;

    virtual char const* consumedStartupExpression ();

    VChannelController* controller () const {
	return m_pController;
    }

    IOMDriver* errorInputDriver	() const {
	return m_pErrorInputDriver;
    }
    IOMDriver* errorOutputDriver () const {
	return m_pErrorOutputDriver;
    }

    bool getInputHandler (DSC_Descriptor& rInputHandler);
    bool getEvaluation (Vsa::VEvaluation::Reference &rpEvaluation) const {
	return m_iOptions.getEvaluation (rpEvaluation);
    }

    virtual int	getPeerNameString   (char** ppString, size_t* psString);
    virtual int getSocketNameString (char** ppString, size_t* psString);
    virtual int getTcpNodelay	    (int*fOnOff) = 0;

    bool handlerWired () const {
	return m_fHandlerWired;
    }

    unsigned int index () const {
	return m_xDriver;
    }

    unsigned int optionValue (IOMHandle const* pHandle) const {
	return m_iOptions.optionValue (pHandle, this);
    }

    IOMPromptFormat promptFormat () const {
	return m_iOptions.promptFormat ();
    }

    IOMState state () const {
	return (IOMState)m_xState;
    }

    VkStatusType statusType () const {
	return m_iStatus.Type ();
    }
    VkStatusCode statusCode () const {
	return m_iStatus.Code ();
    }

    unsigned int useCount () const {
	return m_iUseCount;
    }

//  Query
public:
    bool isReady () const {
	return m_xState == IOMState_Ready;
    }
    bool isStarting () const {
	return m_xState == IOMState_Starting;
    }
    bool isRestarting () const {
	return m_xState == IOMState_Restarting;
    }
    bool isRunning () const {
	return m_xState == IOMState_Running;
    }
    bool isClosing () const {
	return m_xState == IOMState_Closing;
    }

//  Auto Mutex Support
public:
    bool acquireAutoMutex (VMutexClaim& rClaim, VComputationUnit* pSupplicant);

//  Channel Controller Management
private:
    void detach (VChannelController const *pController) {
	if (m_pController.referent () == pController)
	    m_pController.clear ();
    }

//  Control
public:
    virtual void Close		() = 0;
    virtual int	EndTransmission	() = 0;
    virtual int	EndReception	() = 0;

    unsigned int IncrementUseCount ();
    unsigned int DecrementUseCount ();

    int EnableHandler	(VTask* pSpawningTask = 0);
    int DisableHandler	();

protected:
    void SetStateTo (IOMState xState);

    void SetStateToClosing () {
	SetStateTo (IOMState_Closing);
    }

    void SetStateToRunning () {
	SetStateTo (IOMState_Running);
    }

public:
    void SetErrorOutputDriver (IOMDriver* pDriver) {
	m_pErrorOutputDriver.setTo (pDriver);
    }

    void SetErrorInputDriver (IOMDriver* pDriver) {
	m_pErrorInputDriver.setTo (pDriver);
    }

    void SetOptionValue (IOMHandle* pHandle, unsigned int optionValue) {
	m_iOptions.SetOptionValue (pHandle, this, optionValue);
    }

    void SetAutoMutexTo (bool fAutoMutex) {
	m_iOptions.SetAutoMutexTo (fAutoMutex);
    }
    void SetBinaryFormatTo (IOMBinaryFormat xBinaryFormat) {
	m_iOptions.SetBinaryFormatTo (xBinaryFormat);
    }
    void SetPromptFormatTo (IOMPromptFormat xPromptFormat) {
	m_iOptions.SetPromptFormatTo (xPromptFormat);
    }

    virtual int SetTcpNodelay (int fOnOff) = 0;

    void SetWiredHandler (int value) {
	m_fHandlerWired = value;
    }

// Data Conversion
protected:
    void ConvertByteArray (void *pArray, size_t sArray);

    void ConvertShort	(unsigned short	*pValue) { ConvertByteArray (pValue, sizeof (*pValue)); }
    void ConvertLong	(unsigned int	*pValue) { ConvertByteArray (pValue, sizeof (*pValue)); }
    void ConvertFloat	(float		*pValue) { ConvertByteArray (pValue, sizeof (*pValue)); }
    void ConvertDouble	(double		*pValue) { ConvertByteArray (pValue, sizeof (*pValue)); }

//  Input
public:
    virtual IOMDriver *GetConnection ();

    virtual VkStatusType InputOperationStatus () const;

    virtual size_t GetByteCount (IOMHandle const* pHandle) = 0;

    virtual size_t GetData (
	IOMHandle const*	pHandle,
	void *			pBuffer,
	size_t			sMinLength,
	size_t			sMaxLength
    ) = 0;
    size_t GetData (IOMHandle const *pHandle, void *pBuffer, size_t sBuffer) {
	return GetData (pHandle, pBuffer, sBuffer, sBuffer);
    }

    virtual VkStatusType GetString (
	IOMHandle const*	pHandle,
	size_t			sMinLength,
	size_t			sMaxLength,
	char **			ppString,
	size_t *		psString
    ) = 0;
    virtual VkStatusType GetLine (
	IOMHandle const*	pHandle,
	char const*		pPrompt,
	char **			ppLine,
	size_t *		psLine
    ) = 0;

    bool GetByte   (IOMHandle const* pHandle, unsigned char *value);
    bool GetShort  (IOMHandle const* pHandle, unsigned short *value);
    bool GetLong   (IOMHandle const* pHandle, unsigned int *value);
    bool GetFloat  (IOMHandle const* pHandle, float *value);
    bool GetDouble (IOMHandle const* pHandle, double *value);

    void Wait ();

//  Output
public:
    virtual void OpenOutputFile	 (char const *pFileName, bool fOutputRedirected);
    virtual void CloseOutputFile ();

    virtual bool PutBufferedData () = 0;

    size_t	    PutByte	(IOMHandle const* pHandle, unsigned char value);
    size_t	    PutShort	(IOMHandle const* pHandle, unsigned short value);
    size_t	    PutLong	(IOMHandle const* pHandle, unsigned int value);
    size_t	    PutFloat	(IOMHandle const* pHandle, float value);
    size_t	    PutDouble	(IOMHandle const* pHandle, double value);

    virtual size_t  PutData	(IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer) = 0;
    virtual size_t  PutString	(char const* pString) = 0;

    size_t  __cdecl Print	(size_t sData, char const* fmt, ...);
    size_t  __cdecl Report	(size_t sData, char const* fmt, ...);

    virtual size_t  VPrint	(size_t sData, char const* pFormat, va_list pFormatArguments) = 0;
    virtual size_t  VReport	(size_t sData, char const* pFormat, va_list pFormatArguments);

//  Event Handling and Notification
public:
    void untilInputArrivesSuspend (VComputationUnit* pSuspendable);

//  Tracing and Debugging
public:
    void enableTracing () {
	m_fTracingEnabled = true;
    }
    void disableTracing () {
	m_fTracingEnabled = false;
    }
    bool toggleTracing () {
	return m_fTracingEnabled = !m_fTracingEnabled;
    }

//  CheckPointing
public:
    virtual void createCheckPoint (bool bBlocking = false, Vca::ITrigger *pTrigger = 0) {
    }
    virtual void releaseBlockingCheckPoint (Vca::VTrigger<IOMDriver> *pTrigger) {
    }
protected:
    bool checkpointsEnabled () {
	return m_fCheckpointsEnabled;
    }
    void enableCheckpoints () {
	m_fCheckpointsEnabled = true;
    }

//  Triggering Suspended ComputationUnits
public:
    void triggerAll ();

//  TriggerSet Operations
public:
    bool insertIntoTriggerSet (Vca::ITrigger *pTrigger);
    bool deleteFromTriggerSet (Vca::ITrigger *pTrigger);

//  State
protected:
    Options			m_iOptions;
    VkStatus			m_iStatus;
    unsigned int		m_xDriver;
    unsigned int		m_iUseCount;
    IOMState			m_xState		: 8;
    unsigned int		m_fTracingEnabled	: 1,
				m_fCheckpointsEnabled	: 1,
				m_fHandlerWired		: 1;
    Reference			m_pErrorInputDriver;
    Reference			m_pErrorOutputDriver;
    V::VPointer<VChannelController>
				m_pController;
    VComputationTrigger		m_iTrigger;
    Vca::VTriggerSet		m_iTriggerSet;
};

/******************************************************************************
 * Note:
 *  An IOMDriver can have many computation units suspended on it waiting for 
 * input/output data. IOMDriver currently supports 2 structures for this
 * 1. "VComputationTrigger" is a direct list of suspended computation units. 
 * 2. "VTriggerSet" is another indirect structure which is a set of ITriggers. 
 *     Each of this ITrigger can do the operation of resuming common VComputation 
 *     Units which are suspended over a set of IOMDrivers 
 *     (used by wait: primitive)
*****************************************************************************/

#endif

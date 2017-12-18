/*****  I/O Manager Stream Driver Interface  *****/
#ifndef IOMStream_Interface
#define IOMStream_Interface

/************************
 *****  Components  *****
 ************************/

#include "IOMDriver.h"

/**************************
 *****  Declarations  *****
 **************************/

class IOMStreamProducer;
class IOMStreamConsumer;

template<class Actor, typename Datum> class VReceiver;

namespace Vca {
    class IError;

    class VBSConsumer;
    class VBSProducer;

    class VCheckPoint;
}


/*************************
 *****  Definitions  *****
 *************************/

/***********************
 *----  IOMStream  ----*
 ***********************/

class IOMStream : public IOMDriver {
//  Run Time Type
    DECLARE_CONCRETE_RTT (IOMStream, IOMDriver);

//  Producer/Consumer Types
public:
    typedef IOMStreamProducer Producer;
    typedef IOMStreamConsumer Consumer;

    typedef Vca::VBSProducer VBSProducer;
    typedef Vca::VBSConsumer VBSConsumer;

    typedef VReceiver<ThisClass,VBSProducer*> VBSProducerSink;
    typedef VReceiver<ThisClass,VBSConsumer*> VBSConsumerSink;

//  WindowFrame Type
public:
    class WindowFrame : public VTransient {
    //  Construction
    public:
	WindowFrame (WindowFrame *pPredecessor, VReference<Producer> &rpProducer);

    //  Destruction
    protected:
	~WindowFrame ();
    public:
	WindowFrame *Pop (VReference<Producer> &rpProducer);

    //  State
    protected:
	WindowFrame*		const m_pPredecessor;
	VReference<Producer>	const m_pProducer;
    };


//  Construction
public:
    IOMStream (Options const &rOptions, VBSProducer *pBSToHere, VBSConsumer *pBSToPeer);

//  Destruction
protected:
    ~IOMStream ();

//  Plumbing
private:
    void onConnect (VBSProducerSink *pSink, VBSProducer *pBS);
    void onConnect (VBSConsumerSink *pSink, VBSConsumer *pBS);

    void onError (VBSProducerSink *pSink, Vca::IError *pError, VString const &rMessage);
    void onError (VBSConsumerSink *pSink, Vca::IError *pError, VString const &rMessage);

    void plumb (VBSProducer *pBS);
    void plumb (VBSConsumer *pBS);

//  Query
public:
    IOMChannelType channelType () const;

    char const* consumedStartupExpression ();

public:
    int getTcpNodelay (int* fOnOff);

//  Control
protected:
    void PushInput ();
    void PopInput  ();
    void PopAllInput ();

    int Seek (IOMHandle const* pHandle);

public:
    void Close ();
    int EndTransmission	();
    int EndReception	();

    int SetTcpNodelay (int fOnOff);

    void SetStartupExpressionTo (char const* pStartupExpression) {
	m_pStartupExpression = pStartupExpression;
    }

//  Input
public:
    VkStatusType InputOperationStatus () const; 

    size_t GetByteCount (IOMHandle const* pHandle);
    size_t GetData	(IOMHandle const* pHandle, void *pBuffer, size_t cbMin, size_t cbMax);

    VkStatusType GetString (
	IOMHandle const*pHandle,
	size_t		cbMin,
	size_t		cbMax,
	char **		ppString,
	size_t *	psString
    );

    VkStatusType GetLine (
	IOMHandle const*pHandle,
	char const*	zPrompt,
	char **		ppLine,
	size_t *	psLine
    );

//  Output
public:
    void OpenOutputFile	 (char const *filename, bool fOutputRedirected);
    void CloseOutputFile ();

    bool PutBufferedData ();

    size_t PutData (IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer);

    size_t PutString (char const *string);

    size_t VPrint  (size_t sData, char const *fmt, va_list ap);
    size_t VReport (size_t sData, char const *fmt, va_list ap);

//  CheckPoint
public:
    void createCheckPoint (bool bBlocking, Vca::ITrigger *pTrigger);
    void releaseBlockingCheckPoint (Vca::VTrigger<IOMDriver>* pTrigger);

//  State
protected:
    WindowFrame*		m_pInputStack;
    VReference<Producer>	m_pProducer;
    VReference<Consumer>	m_pConsumer;
    unsigned int		m_cGetLineFailures;
    FILE *			m_pOutputFile;
    char const*			m_pStartupExpression;
    bool			m_bOutputGenerated;
    bool			m_bOutputRedirected;
    bool			m_bPromptNeeded;
};


#endif

/*****  I/O Service Driver Interface  *****/
#ifndef IOMService_Interface
#define IOMService_Interface

/************************
 *****  Components  *****
 ************************/

#include "IOMDriver.h"

#include "Vca_IPipeSourceClient.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    class IListener;

    class VBSConsumer;
    class VBSProducer;
}

/*************************
 *****  Definitions  *****
 *************************/

class IOMService : public IOMDriver {
//  Run Time Type
    DECLARE_CONCRETE_RTT (IOMService, IOMDriver);

//  Aliases
public:
    typedef Vca::IClient		IClient;
    typedef Vca::IError			IError;
    typedef Vca::IListener		IListener;
    typedef Vca::IPipeSourceClient	IPipeSourceClient;
    typedef Vca::ITrigger		ITrigger;
    typedef Vca::VBSConsumer		VBSConsumer;
    typedef Vca::VBSProducer		VBSProducer;

//  Construction
public:
    IOMService (Options const& rOptions, IListener *pListener);

//  Destruction
protected:
    ~IOMService ();

//  Base Roles
public:
    using BaseClass::getRole;

//  IPipeSourceClient Role
private:
    Vca::VRole<ThisClass,Vca::IPipeSourceClient> m_pIPipeSourceClient;
public:
    void getRole (VReference<IPipeSourceClient>&rpRole) {
	m_pIPipeSourceClient.getRole (rpRole);
    }

//  IPipeSourceClient Methods
public:
    void OnData (
	IPipeSourceClient *pRole,
	VBSConsumer *pStdBSToPeer,
	VBSProducer *pStdBSToHere,
	VBSProducer *pErrBSToHere
    );

//  IClient Methods
    void OnError (IClient *pRole, IError *pIError, VString const &rMessage);

//  Query
public:
    IOMChannelType channelType () const;

    int getPeerNameString	(char** ppString, size_t* psString);
    int getSocketNameString	(char** ppString, size_t* psString);

    int getTcpNodelay		(int* fOnOff);

//  Control
public:
    void Close ();

    int EndTransmission ();
    int EndReception ();

    int SetTcpNodelay (int fOnOff);

//  Input
public:
    IOMDriver *GetConnection ();

    size_t GetByteCount (IOMHandle const* pHandle);

    size_t GetData (IOMHandle const* pHandle, void* pBuffer, size_t cbMin, size_t cbMax);

    VkStatusType GetString (
	IOMHandle const*	pHandle,
	size_t			cbMin,
	size_t			cbMax,
	char **			ppString,
	size_t *		psString
    );

    VkStatusType GetLine (
	IOMHandle const*	pHandle,
	char const*		zPrompt,
	char**			ppLine,
	size_t *		psLine
    );

//  Output
public:
    bool PutBufferedData ();

    size_t PutData (IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer);

    size_t PutString (char const* string);

    size_t VPrint (size_t sData, char const* pFormat, va_list pFormatArguments);

//  State
protected:
    VReference<IListener>	m_pListener;
    VReference<VBSConsumer>	m_pAcceptedStdBSToPeer;
    VReference<VBSProducer>	m_pAcceptedStdBSToHere;
    VReference<VBSProducer>	m_pAcceptedErrBSToHere;
};


#endif

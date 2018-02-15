/*****  I/O Service Driver Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IOMServiceFacility

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "IOMService.h"

/*****  Supporting *****/
#include "vfac.h"

#include "IOMStream.h"

#include "Vca_IListener.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (IOMService);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMService::IOMService (
    Options const& rOptions, IListener *pListener
) : IOMDriver (rOptions), m_pListener (pListener), m_pIPipeSourceClient (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IOMService::~IOMService () {
    Close ();
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

IOMChannelType IOMService::channelType () const {
    return IOMChannelType_Service;
}

int IOMService::getTcpNodelay (int* fOnOff) {
    *fOnOff = false;
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void IOMService::Close () {
    if (isClosing ())
	return;

//  Begin the close process, ...
    SetStateToClosing ();

//  ... and release resources:
    if (m_pListener) {
	m_pListener->Close ();
	m_pListener.clear ();
    }
}

int IOMService::EndTransmission () {
    m_iStatus.MakeFailureStatus ();
    return -1;
}

int IOMService::EndReception () {
    m_iStatus.MakeFailureStatus ();
    return -1;
}

int IOMService::SetTcpNodelay (int fOnOff) {
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/*******************
 *******************
 *****  Input  *****
 *******************
 *******************/

void IOMService::OnData (
    Vca::IPipeSourceClient *pRole,
    Vca::VBSConsumer *pStdBSToPeer,
    Vca::VBSProducer *pStdBSToHere,
    Vca::VBSProducer *pErrBSToHere
) {
    m_pAcceptedStdBSToPeer.setTo (pStdBSToPeer);
    m_pAcceptedStdBSToHere.setTo (pStdBSToHere);
    m_pAcceptedErrBSToHere.setTo (pErrBSToHere);
    triggerAll ();
}

void IOMService::OnError (
    Vca::IClient *pRole, Vca::IError *pIError, VString const &rMessage
) {
    m_iStatus.MakeFailureStatus ();
    if (m_pListener) {
	m_pListener->Close ();
	m_pListener.clear ();
    }

    triggerAll ();
}

/*---------------------------------------------------------------------------
 *****  'GetConnection' Handler.
 *
 *  Arguments:
 *
 *  Returns:
 *	The address of the accepted connection or NULL if no connection
 *	was available to be accepted.
 *
 *****/
IOMDriver* IOMService::GetConnection () {
    if (m_pListener.isNil ())
	return NilOf (IOMDriver*);

    if (m_pAcceptedStdBSToHere.isNil () || m_pAcceptedStdBSToPeer.isNil ()) {
	m_iStatus.MakeBlockedStatus ();
	VReference<Vca::IPipeSourceClient> pRole;
	getRole (pRole);
	m_pListener->SupplyByteStreams (pRole, 1);
	return NilOf (IOMDriver*);
    }

    IOMStream *pStream = new IOMStream (m_iOptions, m_pAcceptedStdBSToHere, m_pAcceptedStdBSToPeer);
    m_pAcceptedErrBSToHere.clear ();
    m_pAcceptedStdBSToHere.clear ();
    m_pAcceptedStdBSToPeer.clear ();

    return pStream;
}


/*---------------------------------------------------------------------------
 *****  'GetByteCount' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *
 *  Returns:
 *	The number of bytes actually available for reading.  Always zero for this
 *	driver.
 *
 *****/
size_t IOMService::GetByteCount (IOMHandle const* Unused(pHandle)) {
    return 0;
}

/*---------------------------------------------------------------------------
 *****  'GetData' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *	pBuffer			- the address of the buffer to fill.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns zero if at least 'cbMin'
 *				  bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.
 *
 *  Returns:
 *	The actual number of bytes returned in '*pBuffer'.  Always zero for this
 *	driver.
 *
 *****/
size_t IOMService::GetData (
    IOMHandle const*		Unused(pHandle),
    void*			Unused(pBuffer),
    size_t			Unused(cbMin),
    size_t			Unused(cbMax)
)
{
    m_iStatus.MakeFailureStatus ();
    return 0;
}


/*---------------------------------------------------------------------------
 *****  'GetString' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns a failure code if at least
 *				  'cbMin' bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.  If 'cbMax' is less than 'cbMin', all
 *				  currently available data will be read and
 *				  and returned.
 *	ppString		- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  data read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'deallocate' on its address when it is no
 *				  longer needed.  '*ppString' is set to NULL if
 *				  no data was read.
 *	psString		- the length of the string returned in
 *				  '**ppString', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	The driver status at the end of the call.  Always 'Failed' in this driver.
 *
 *****/
VkStatusType IOMService::GetString (
    IOMHandle const*		Unused(pHandle),
    size_t			Unused(cbMin),
    size_t			Unused(cbMax),
    char **			ppString,
    size_t *			psString
)
{
    *ppString = NilOf (char *);
    *psString = 0;

    m_iStatus.MakeFailureStatus ();
    return VkStatusType_Failed;
}


/*---------------------------------------------------------------------------
 *****  'GetLine' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.
 *	zPrompt			- the address of a NULL terminated string used
 *				  as an input prompt.  May be NULL if no prompt
 *				  is desired.
 *	ppLine			- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  line read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'deallocate' on its address when it is no
 *				  longer needed.  '*ppLine' is set to NULL if
 *				  no data was read.
 *	psLine			- the length of the string returned in
 *				  '**ppLine', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	The driver status at the end of the call.  Always 'Failed' in this driver.
 *
 *****/
VkStatusType IOMService::GetLine (
    IOMHandle const*		Unused(pHandle),
    char const*			Unused(zPrompt),
    char**			ppLine,
    size_t *			psLine
)
{
    *ppLine = NilOf (char *);
    *psLine = 0;

    m_iStatus.MakeFailureStatus ();
    return VkStatusType_Failed;
}


/********************
 ********************
 *****  Output  *****
 ********************
 ********************/

/*******************
 *****  Flush  *****
 *******************/

bool IOMService::PutBufferedData () {
    return true;
}

/*****************
 *****  Raw  *****
 *****************/

size_t IOMService::PutData (
    IOMHandle const* Unused(pHandle), void const* Unused(pBuffer), size_t Unused(sBuffer)
) {
    return (size_t)0;
}

/***********************
 *****  Character  *****
 ***********************/

size_t IOMService::PutString (char const* Unused(string)) {
    return (size_t)0;
}

size_t IOMService::VPrint (
    size_t Unused(sData), char const* Unused(pFormat), va_list Unused(pFormatArguments)
) {
    return (size_t)0;
}


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (IOMService);
    FAC_FDFCase_FacilityDescription ("I/O Service Driver");
    default:
        break;
    }
}

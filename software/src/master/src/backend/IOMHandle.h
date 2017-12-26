#ifndef IOMHandle_Interface
#define IOMHandle_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "IOMDriver.h"


/*************************
 *****  Definitions  *****
 *************************/

class IOMHandle {
//  Initialization
protected:
    void construct ();

public:
    void construct (VkStatus const* pStatus);
    void construct (IOMDriver const* pDriver);
    void construct (unsigned int xDriver);

//  Query
public:

//  Access
public:
    IOMChannelType channelType () const;

    IOMDriver* driver () const;

    IOMDriver* errorInputDriver () const;
    IOMDriver* errorOutputDriver () const;

    unsigned int index () const {
	return m_xDriverValid ? m_xDriver + 1 : 0;
    }

    IOMOption optionIndex () const {
	return (IOMOption)m_xOption;
    }
    unsigned int optionValue () const;

    size_t seekOffset () const {
	return m_iSeekOffset;
    }

    IOMState	 state	    () const;
    VkStatusType status	    () const;
    VkStatusCode statusCode () const;

    int getPeerNameString   (char **ppString, size_t *psString) const;
    int getSocketNameString (char **ppString, size_t *psString) const;

    int getTcpNodelay (int* fOnOff) const;

    IOMTrimFormat trimFormat () const {
	return (IOMTrimFormat)m_xTrimFormat;
    }

//  Control and Update
public:
    unsigned int retain ();
    unsigned int release ();

    void setErrorInput  (IOMHandle const* pHandle) const;
    void setErrorOutput (IOMHandle const* pHandle) const;

    void setOptionIndex (IOMOption xOption) {
	m_xOption = xOption;
    }

    void setOptionValue (unsigned int optionValue);

    void setSeekOffset (size_t iSeekOffset) {
	m_iSeekOffset = iSeekOffset;
    }

    int setTcpNodelay (int fOnOff) const;

    void setTrimFormat (IOMTrimFormat xTrimFormat) {
	m_xTrimFormat = xTrimFormat;
    }

    void setWiredHandler (int value) const;

    int enableHandler (VTask* pSpawningTask) const;
    int disableHandler () const;

//  Control
    int endReception	() const;
    int endTransmission	() const;

    unsigned int close	() const;

//  Input
protected:
    VkStatusType trimString (
	char **ppString, size_t *psString, VkStatusType resultStatus
    ) const;
    VkStatusType trimLine (
	char **ppString, size_t *psString, VkStatusType resultStatus
    ) const;

public:
    IOMDriver* getConnection () const;

    size_t getByteCount () const;

    bool getByte (unsigned char *value) const;

    bool getShort (unsigned short *value) const;

    bool getLong (unsigned int *value) const;

    bool getFloat (float *value) const;

    bool getDouble (double *value) const;

    VkStatusType getString (
	size_t sMinLength, size_t sMaxLength, char ** ppString, size_t *psString
    ) const;

    VkStatusType getLine (
	char const *pPrompt, char **ppLine, size_t *psLine
    ) const;

//  Output
public:
    size_t putByte (unsigned char value) const;

    size_t putShort (unsigned short value) const;

    size_t putLong (unsigned int value) const;

    size_t putFloat (float value) const;

    size_t putDouble (double value) const;

    size_t putString (char const *pString) const;

    int putBufferedData () const;

//  State
protected:
    unsigned int	m_xOption		: 3,
			m_xTrimFormat		: 2,
						: 2,
			m_xDriverValid		: 1,
			m_xDriver		: 24;
    unsigned int	m_iSeekOffset;
};


#endif

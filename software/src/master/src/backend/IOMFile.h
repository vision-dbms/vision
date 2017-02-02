/*****  I/O Manager File Driver Interface  *****/
#ifndef IOMFile_Interface
#define IOMFile_Interface

/************************
 *****  Components  *****
 ************************/

#include "IOMDriver.h"
#include "VkMemory.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class IOMFile : public IOMDriver {
//  Run Time Type
    DECLARE_CONCRETE_RTT (IOMFile, IOMDriver);

//  MappingMode Enumeration
public:
    enum MappingMode {
	MappingMode_None,
	MappingMode_ReadOnly,
	MappingMode_ReadWrite
    };

//  Construction
public:
    IOMFile (
	Options const& rOptions, int fd, char const *pFileName, MappingMode mappingMode
    );

//  Destruction
protected:
    ~IOMFile ();

//  Query
public:
    IOMChannelType channelType () const;

    int getTcpNodelay (int* fOnOff);
    
//  Control
public:
    void Close		 ();

    int	 EndTransmission ();
    int	 EndReception	 ();

protected:
    int Seek (IOMHandle const* pHandle, size_t *pOffset);

public:
    int SetTcpNodelay (int fOnOff);

//  Input
public:
    size_t GetByteCount (IOMHandle const* pHandle);

    size_t GetData (IOMHandle const* pHandle, void *pBuffer, size_t cbMin, size_t cbMax);

    VkStatusType GetString (
	IOMHandle const*pHandle,
	size_t		cbMin,
	size_t		cbMax,
	char **		ppString,
	size_t *	psString
    );

    VkStatusType GetLine (
	IOMHandle const*pHandle,
	char const*	pPrompt,
	char **		ppLine,
	size_t *	psLine
    );

//  Output
public:
    bool PutBufferedData();

    size_t PutData	(IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer);

    size_t PutString	(char const *string);

    size_t VPrint	(size_t sData, char const* pFormat, va_list pFormatArguments);

//  State
protected:
    int				m_filefd;
    MappingMode			m_mappingMode;
    size_t			m_mappingSize;
    pointer_t			m_mappingBase;
    pointer_t			m_mappingCursor;
    VkMemory			m_fileMapping;
};


#endif

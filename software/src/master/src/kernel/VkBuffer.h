#ifndef VkBuffer_Interface
#define VkBuffer_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class V_API VkBuffer : public VTransient {
//  Constants
public:
    enum {
	BufferGrain = 1024,
	InitialBufferSize = 4 * BufferGrain
    };

//  Construction
public:
    VkBuffer ();

//  Destruction
public:
    ~VkBuffer ();

//  Access
public:
    char const *ConsumerRegion () const {
	return m_pConsumerRegion;
    }
    size_t ConsumerRegionSize () const {
	return (size_t)(m_pProducerRegion - m_pConsumerRegion);
    }

    pointer_t ProducerRegion () const {
	return m_pProducerRegion;
    }
    size_t ProducerRegionSize () const {
	return (size_t)(m_pStorage + m_sStorage - m_pProducerRegion);
    }

//  Control
public:
    bool CompactAndGuarantee (size_t sMinimumProducerRegion);

//  Update
public:
    void Consume (size_t sData) {
	m_pConsumerRegion += sData;
    }
    void Consume (void *pData, size_t sData) {
	memcpy (pData, m_pConsumerRegion, sData);
	Consume (sData);
    }

    void Produce (size_t sData) {
	m_pProducerRegion += sData;
    }
    void UnProduce (size_t sData) {
	m_pProducerRegion -= sData;
    }
    void Produce (void const *pData, size_t sData) {
	memcpy (m_pProducerRegion, pData, sData);
	Produce (sData);
    }

//  State
protected:
    pointer_t	m_pConsumerRegion;
    pointer_t	m_pProducerRegion;
    pointer_t	m_pStorage;
    size_t	m_sStorage;
};


#endif

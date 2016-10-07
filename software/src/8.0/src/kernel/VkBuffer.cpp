/*****  VkBuffer Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VkBuffer.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VkBuffer::VkBuffer ()
: m_pStorage ((pointer_t)allocate (InitialBufferSize))
, m_sStorage (InitialBufferSize)
{
    m_pConsumerRegion = m_pProducerRegion = m_pStorage;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VkBuffer::~VkBuffer () {
    deallocate (m_pStorage);
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

bool VkBuffer::CompactAndGuarantee (size_t sMinimumProducerRegion) {
    size_t sConsumerRegion = ConsumerRegionSize ();
    if (m_pStorage < m_pConsumerRegion) {
	memcpy (m_pStorage, m_pConsumerRegion, sConsumerRegion);
	m_pConsumerRegion = m_pStorage;
	m_pProducerRegion = m_pStorage + sConsumerRegion;
    }

    size_t sProducerRegion = ProducerRegionSize ();
    if (sProducerRegion < sMinimumProducerRegion) {
	m_sStorage = (sConsumerRegion + sMinimumProducerRegion + BufferGrain - 1)
		   / BufferGrain * BufferGrain;
	m_pStorage = (pointer_t)reallocate (m_pStorage, m_sStorage);
	m_pConsumerRegion = m_pStorage;
	m_pProducerRegion = m_pStorage + sConsumerRegion;
    }

    return true;
}

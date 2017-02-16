/*****  V_VFifoLite Implementation  *****/

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

#include "V_VFifoLite.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  V::VFifoLite  *****
 *****                *****
 **************************
 **************************/

/***************************
 ***************************
 *****  Data Movement  *****
 ***************************
 ***************************/

size_t V::VFifoLite::move (char *&rpVoid, char const *&rpData, size_t sMove) {
    if (sMove > 0) {
	if (rpVoid != rpData)
	    memcpy (rpVoid, rpData, sMove);

	rpVoid += sMove;
	rpData += sMove;
    }
    return sMove;
}

size_t V::VFifoLite::moveDataTo (VFifoLite &rConsumer) {
    size_t sMove = consumerAreaSize ();
    if (sMove > rConsumer.producerAreaSize ())
	sMove = rConsumer.producerAreaSize ();
    return move (rConsumer.m_pPPoint, m_pCPoint, sMove);
}

size_t V::VFifoLite::moveDataFrom (VFifoLite &rProducer) {
    size_t sMove = producerAreaSize ();
    if (sMove > rProducer.consumerAreaSize ())
	sMove = rProducer.consumerAreaSize ();
    return move (m_pPPoint, rProducer.m_pCPoint, sMove);
}

/*****  VUtilities::ZlibCompressor Implementation  *****/

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

#include "VUtilities_ZlibCompressor.h"

/************************
 *****  Supporting  *****
 ************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VUtilities::ZlibCompressor);


void VUtilities::ZlibCompressor::init () {
    m_iState.zalloc = 0;  
    m_iState.zfree  = 0;   
    m_iState.opaque = 0;  
    VUtilities::Zlib_DeflateInit (&m_iState, Z_DEFAULT_COMPRESSION);
}


void VUtilities::ZlibCompressor::onTransfer (size_t sTransfer) {
    m_sVoid += sTransfer;
    
    if (m_sData > 0 && m_sVoid > 0) {
	size_t sCopy = m_sVoid;
	if (sCopy > m_sData)
	    sCopy = m_sData;
	
	display ("\nSent COMPRESSED Data Size %d", sTransfer);
	displayDataAtTransferPoint (m_pData, sCopy, "O");

	m_sVoid -= sCopy;
	m_sData -= sCopy;
	m_pData += sCopy;
	m_iBlob.incrementContentLowerBoundBy (sCopy);

	if (m_sData == 0) {
	    m_iBlob.setContentLowerBoundTo (0);
	    m_iBlob.setContentUpperBoundTo (0);	    
	}
    }
    
    if (m_sData == 0 && m_bPending)
   	flushPendingData ();
}

void VUtilities::ZlibCompressor::transferData () {

    while (m_sData == 0 && 
	   m_bPending == false &&
	   m_sOriginalSize == 0 ? getArea (m_pOriginalArea, m_sOriginalSize) : true 
    ) {
	Vca::U32 sConsumed = compressBufferToBlob ();
	if (sConsumed) {
	    m_pData = m_iBlob.storage ();
	    m_sData = m_iBlob.contentSize ();

	    VReference<Vca::IBSClient> pIBSClient;
	    getRole (pIBSClient);
	    onTransfer (m_pConsumer->put (pIBSClient, m_pData, m_sData));
	    onPutCompletion (sConsumed);    
	}
    }
}

Vca::U32 VUtilities::ZlibCompressor::compressBufferToBlob () {

    Vca::U32 sOrigTotalIn = m_iState.total_in;
    Vca::U32 sOrigTotalOut = m_iState.total_out;

    m_iState.next_in = (unsigned char *) m_pOriginalArea;
    m_iState.avail_in = m_sOriginalSize;

    m_iState.next_out = (unsigned char *) m_iBlob.contentArea ();
    m_iState.avail_out = m_iBlob.storageSize ();

    int result = VUtilities::Zlib_Deflate (&m_iState, Z_FINISH);

    if (result == Z_OK  && m_iState.avail_out==0) 
	m_bPending = true;

    if (result == Z_STREAM_END ) 
	m_bPending = false;


    Vca::U32 sConsumed = m_iState.total_in - sOrigTotalIn;
    m_sOriginalSize -=  sConsumed;
    m_pOriginalArea +=  sConsumed;

    m_iBlob.incrementContentUpperBoundBy (m_iState.total_out - sOrigTotalOut);

    if (result == Z_STREAM_END) {
	VUtilities::Zlib_DeflateEnd (&m_iState);
	VUtilities::Zlib_DeflateInit (&m_iState, Z_DEFAULT_COMPRESSION);
    }
    return sConsumed;
}

void VUtilities::ZlibCompressor::flushPendingData () {
    Vca::U32 sOrigTotalIn = m_iState.total_in;
    Vca::U32 sOrigTotalOut = m_iState.total_out;

    m_iState.next_out = (unsigned char *) m_iBlob.contentArea ();
    m_iState.avail_out = m_iBlob.storageSize ();

    int result = VUtilities::Zlib_Deflate (&m_iState, Z_FINISH);
    if (result == Z_OK && m_iState.avail_out==0) 
	m_bPending = true;

    if (result == Z_STREAM_END) 
	m_bPending = false;

    m_iBlob.incrementContentUpperBoundBy (m_iState.total_out - sOrigTotalOut);

    if (result == 1) {
	VUtilities::Zlib_DeflateEnd  (&m_iState);
	VUtilities::Zlib_DeflateInit (&m_iState, Z_DEFAULT_COMPRESSION);
    }

    m_pData = m_iBlob.storage ();
    m_sData = m_iBlob.contentSize ();

    VReference<Vca::IBSClient> pIBSClient;
    getRole (pIBSClient);
    onTransfer (m_pConsumer->put (pIBSClient, m_pData, m_sData));
}

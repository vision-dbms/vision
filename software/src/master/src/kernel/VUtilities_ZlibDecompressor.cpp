/*****  VUtilities::ZlibDecompressor Implementation  *****/

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

#include "VUtilities_ZlibDecompressor.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************************
 ******************************************
 *****			    	      *****
 *****  VUtilities::ZlibDecompressor  *****
 *****                                *****
 ******************************************
 ******************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VUtilities::ZlibDecompressor);


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VUtilities::ZlibDecompressor::ZlibDecompressor (
    Vca::VBSProducer *pProducer
)   : m_pProducer (pProducer), m_pIBSClient (this)
    , m_pOriginalArea (0), m_sOriginalSize (0)
    , m_iBlob (4096) {
    init ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VUtilities::ZlibDecompressor::~ZlibDecompressor () {
    VUtilities::Zlib_InflateEnd  (&m_iState);
}

void VUtilities::ZlibDecompressor::init () {
    m_iState.zalloc = 0;  
    m_iState.zfree  = 0;   
    m_iState.opaque = 0;  
    VUtilities::Zlib_InflateInit (&m_iState);    
}

void VUtilities::ZlibDecompressor::onTransfer (size_t sTransfer) {
    display ("\nObtained COMPRESSED Data Size %d", sTransfer);
    displayDataAtTransferPoint (m_iBlob.postContentArea (), sTransfer, "I");
    m_iBlob.incrementContentUpperBoundBy (sTransfer);
}


void VUtilities::ZlibDecompressor::transferData () {

    if (m_iBlob.contentSize () > 0 && m_pOriginalArea) 
	decompressBlobToBuffer ();

    while (m_pOriginalArea ==0 && getArea (m_pOriginalArea, m_sOriginalSize)) {

	if (m_iBlob.contentSize () > 0) 
	    decompressBlobToBuffer ();
	else {
	    VReference<Vca::IBSClient> pClient;
	    getRole (pClient);
	    
	    Vca::U32 sData = m_pProducer->get (
		    pClient, m_iBlob.storage (), 1, m_iBlob.storageSize  ()
		);
	    if (sData >0) {
		onTransfer (sData);
		decompressBlobToBuffer ();
	    }
	}
    }
}


void VUtilities::ZlibDecompressor::decompressBlobToBuffer () {

    Vca::U32 sOrigTotalIn = m_iState.total_in;

    //  set the input and output area
    m_iState.next_in = (unsigned char *) m_iBlob.contentArea ();
    m_iState.avail_in = m_iBlob.contentSize ();

    m_iState.next_out = (unsigned char*) m_pOriginalArea;
    m_iState.avail_out = m_sOriginalSize;


    int iResult;
    for (;;) {
	if ((iResult = VUtilities::Zlib_Inflate (&m_iState,  Z_SYNC_FLUSH)) != Z_OK) 
	    break;
    }


    Vca::U32 sDecompressed = m_sOriginalSize - m_iState.avail_out;
    if (sDecompressed > 0) {

	display ("\nDECOMPRESSED SIZE %d", sDecompressed);
	onGetCompletion (sDecompressed);

	m_sOriginalSize = 0;
	m_pOriginalArea = 0;

	m_iBlob.incrementContentLowerBoundBy (m_iState.total_in - sOrigTotalIn);

	if (m_iBlob.contentSize () == 0) {
	    m_iBlob.setContentLowerBoundTo (0);
	    m_iBlob.setContentUpperBoundTo (0);
	}
    }

    if (iResult == Z_STREAM_END) {
	VUtilities::Zlib_InflateEnd  (&m_iState);
	VUtilities::Zlib_InflateInit (&m_iState);
    }
}



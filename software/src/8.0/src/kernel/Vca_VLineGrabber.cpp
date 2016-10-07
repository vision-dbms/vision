/*****  Vca_VLineGrabber Implementation  *****/

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

#include "Vca_VLineGrabber.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSProducer.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VLineGrabber  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VLineGrabber::VLineGrabber (VBSProducer *pBSToHere) : m_pBS (pBSToHere), m_pBSClient (this), m_bNotStarted (true) {
//  Make sure the input buffer is large enough (leave room to add a trailing '\0')...
    m_iInputBuffer.CompactAndGuarantee (2);
}

void Vca::VLineGrabber::start () {
//  start the first read...
    if (m_bNotStarted) {
	m_bNotStarted = false;
	retain (); {
	    IBSClient::Reference pBSClient;
	    getRole (pBSClient);
	    OnTransfer (
		pBSClient, m_pBS->get (
		    pBSClient,
		    m_iInputBuffer.ProducerRegion (),
		    1,
		    m_iInputBuffer.ProducerRegionSize () - 1
		)
	    );
	} untain ();
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VLineGrabber::~VLineGrabber () {
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

/************************
 *****  OnTransfer  *****
 ************************/

void Vca::VLineGrabber::OnTransfer (IBSClient *pRole, size_t sTransfer) {
    while (sTransfer > 0) {
    //  'Move' the now valid data from the buffer's producer to consumer region, ...
	m_iInputBuffer.Produce (sTransfer);

    //  ... output, but don't 'produce', a '\0' to make the string functions usable, ...
	m_iInputBuffer.ProducerRegion ()[0] = '\0';

    //  ... find and process lines, ...
	bool bNotDone = true;
	do {
	    char const *pData = m_iInputBuffer.ConsumerRegion ();
	    size_t sData = strcspn (m_iInputBuffer.ConsumerRegion (), "\n");
	    if (!pData[sData])	//  ... we're done when we find the trailing '\0'
		bNotDone = false;
	    else {
	    //  ... trim trailing '\r', ...
		size_t sLine = sData > 0 && '\r' == pData[sData - 1] ? sData - 1 : sData;

	    //  ... and process the line:
		if (!onData_(pData, sLine)) {
		    onDone ();
		    bNotDone = false;
		}

		m_iInputBuffer.Consume (sData + 1);
	    }
	} while (bNotDone);

    //  ... and ask for more if we haven't closed our connection.
	if (m_pBS.isNil ())
	    sTransfer = 0;
	else {
	    // ... leave room for at least one char and a trailing '\0'
	    m_iInputBuffer.CompactAndGuarantee (2);
	    sTransfer = m_pBS->get (
		pRole,
		m_iInputBuffer.ProducerRegion (),
		1,
		m_iInputBuffer.ProducerRegionSize () - 1
	    );
	}
    }
}


/*******************
 *****  OnEnd  *****
 *******************/

void Vca::VLineGrabber::OnEnd (IClient *pRole) {
    onDone ();
}

/*********************
 *****  OnError  *****
 *********************/

void Vca::VLineGrabber::OnError (IClient *pRole, IError *pError, VString const &rMessage) {
    BaseClass::OnError (pRole, pError, rMessage);
    onDone ();
}

/********************
 *****  onDone  *****
 ********************/

void Vca::VLineGrabber::onDone () {
    m_pBS.clear ();
    onDone_();
}

/*****  Vca::VBSProducerBitBucket Implementation  *****/

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

#include "Vca_VBSProducerBitBucket.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::VBSProducerBitBucket  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VBSProducerBitBucket::VBSProducerBitBucket (VBSProducer *pBS) : m_pBS (pBS), m_pIBSClient (this) {
    retain (); {
	IBSClient::Reference pRole;
	getRole (pRole);
	OnTransfer (pRole, 0);
    } untain ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VBSProducerBitBucket::OnTransfer (IBSClient *pRole, size_t sTransfer) {
    do {
	sTransfer = m_pBS->get (pRole, m_iBucketBuffer, 1, sizeof (m_iBucketBuffer));
    } while (sTransfer > 0);
}

void Vca::VBSProducerBitBucket::OnError (IClient *pRole, IError *pError, VString const &rMessage) {
    if (m_pBS) {
	m_pBS->Close ();
	m_pBS.clear ();
    }
}

void Vca::VBSProducerBitBucket::OnEnd (IClient *pRole) {
    if (m_pBS) {
	m_pBS->Close ();
	m_pBS.clear ();
    }
}

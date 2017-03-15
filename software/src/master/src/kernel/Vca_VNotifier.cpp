/*****  Vca_VNotifier Implementation  *****/

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

#include "Vca_VNotifier.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaGofer.h"
#include "VReceiver.h"

#include "V_VArgList.h"
#include "V_VString.h"

#include "VTransientServices.h"
#include "VkSocketAddress.h"

#if defined(__VMS)
#include "sys_info.h"
#endif


/***************************
 ***************************
 *****      Globals    *****
 ***************************
 ***************************/

Vca_API Vca::VGoferInterface<Vca::IInfoServer>::Reference g_pInfoServerGofer;

/********************************
 ********************************
 *****                      *****
 *****  Vca::InfoDataObject *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (Vca::InfoDataObject);
DEFINE_CONCRETE_RTT (Vca::Notifier);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::InfoDataObject::InfoDataObject (bool bWaitingForResp, VGoferInterface<IInfoServer> *pInfoServerGofer, VString const &rTarget, int iPriority, char const *pFormat, va_list ap)
    : m_bWaitingForResp (bWaitingForResp), m_iTarget (rTarget), m_iPriority (iPriority), m_pIClient (this) {
    m_pContent.vsafeprintf (pFormat, ap);
    retain (); {
	if (pInfoServerGofer) {
	    pInfoServerGofer->supplyMembers (this, &ThisClass::onInfoServer, &ThisClass::onInfoServerError);
	}
    } untain ();
}

Vca::InfoDataObject::~InfoDataObject () {
}

void Vca::InfoDataObject::onInfoServer (Vca::IInfoServer *pInfoServer) {

    static VString iUserName;
    static VString iHostName;
    static VString iProcessId;
    static VString iProcessName;
    static VString iImageName;

    if (iUserName.isEmpty ()) {
	char iBuffer[256];
	iUserName.setTo (Vk_username (iBuffer, sizeof (iBuffer)));
    }
    if (iHostName.isEmpty ()) {
	char iBuffer[256];
	iHostName.setTo (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));
    }
    if (iProcessId.isEmpty ()) {
	char iBuffer[256];
#if defined(__VMS)
	sprintf (iBuffer,"%X", getpid ());  
#elif defined (__linux__)
	sprintf (iBuffer,"%d", getpid ());  
#endif

	iProcessId.setTo (iBuffer);
    }
    if (iProcessName.isEmpty ()) {
	char iBuffer[256];
#if defined(__VMS)
	get_process_name (iBuffer, sizeof (iBuffer));
#elif defined (__linux__)
	ctermid(iBuffer);
#endif
	iProcessName.setTo (iBuffer);
    }
    if (iImageName.isEmpty ()) {
	char iBuffer[256];
#if defined(__VMS)
	get_image_name (iBuffer, sizeof (iBuffer));
#elif defined (__linux__)
	int iSize = sizeof (iBuffer);
	ssize_t iCount = readlink("/proc/self/exe", iBuffer, iSize);
 
	if( iCount != -1 ) {
	    // Adding the terminating character.
	    if( iCount < iSize ) {
		iBuffer[iCount] = '\0';
	    } else { 
		iBuffer[iSize - 1] = '\0';
	    }
	}
#endif
	iImageName.setTo (iBuffer);
    }

    VkDynamicArrayOf<VString> iInfoArray (7);
    iInfoArray[0].setTo (m_pContent.content ());
    //    iInfoArray[1].setTo (transientServicesProvider()->infoSubscriber ().content ());
    iInfoArray[1].setTo (m_iTarget.content ());
    iInfoArray[2].setTo (iUserName.content ());
    iInfoArray[3].setTo (iHostName.content ());
    iInfoArray[4].setTo (iProcessId.content ());
    iInfoArray[5].setTo (iProcessName.content ());
    iInfoArray[6].setTo (iImageName.content ());

    if (m_bWaitingForResp) {
	IClient::Reference pClient;
	getRole (pClient);
	pInfoServer -> Notify (pClient, m_iPriority, iInfoArray);
    } else {
	pInfoServer -> Notify (0, m_iPriority, iInfoArray);
    }
}

/*
void Vca::InfoDataObject::OnEnd_() {
    exit (NormalExitValue);
}
*/

Vca::Notifier::Notifier (VGoferInterface<IInfoServer> *pInfoServerGofer) : m_pGofer (pInfoServerGofer) {
} 

Vca::Notifier::~Notifier () {
}

void Vca::VTransientServicesForNotification::notify_ (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap) {
    VString iNSName; 
    V::GetEnvironmentString (iNSName, "NSName", "");
	ActionInfos_t iActionInfos[5];
    NSEntry::Reference pNSEntry;
    unsigned int xEntry;
    if (m_iNSEntries.Locate (iNSName, xEntry)) {
	pNSEntry.setTo (m_iNSEntries[xEntry]);
	if (pNSEntry->infoServer ().isntEmpty ()) {
	    pNSEntry->getActionInfos (xEvent, iActionInfos);
	    for (int xType = 0; xType < 5; xType++) {
		V::U32 iSize = iActionInfos[xType].cardinality ();
		if (iSize > 0) {
		    for (V::U32 i = 0; i < iSize; i++) 
			InfoDataObject::Reference const pInfoDataObj (new InfoDataObject (bWaitingForResp, g_pInfoServerGofer, iActionInfos[xType][i], xType, pFormat, ap));
		}
	    }
	}
    }
}


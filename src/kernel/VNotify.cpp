/*****  VNotify Application  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"
#include "Vca_VClientApplication.h"

#include "Vca_VNotifier.h"
#include "Vca_VDirectory.h"
#include "VTransientServices.h"
#include "Vca_IInfoServer.h"
#include "Vca_VGoferInterface.h"
#include "VkSocketAddress.h"
#if defined(__VMS)
#include "sys_info.h"
#endif


#include "VTransientServices.h"

typedef VkSetOf<VString, VString const&, char const*> ActionInfos_t;
extern     Vca::VGoferInterface<Vca::IInfoServer>::Reference g_pInfoServerGofer;

/*****************************
 *****************************
 *****                   *****
 *****  VcaApps::VNotify  *****
 *****                   *****
 *****************************
 *****************************/

namespace VcaApps {
    class VNotify : public Vca::VClientApplication {
	DECLARE_CONCRETE_RTT (VNotify, Vca::VClientApplication);

    //  Construction
    public:
	VNotify (Context *pContext);

    //  Destruction
    private:
	~VNotify ();

    //  Startup
    private:
	bool start_();

    public:
	void sendNotice (Vca::IInfoServer *pInterface, int xType, VString  rTarget);

    private:
	VkDynamicArrayOf<VString> m_iInfoArray;
    };
}


namespace {
    using namespace VcaApps;
    class Notice : public VNotify::Activity {
	DECLARE_CONCRETE_RTTLITE (Notice, VNotify::Activity);

    //  Aliases
    public:
	typedef Vca::VGoferInterface<Vca::IInfoServer> gofer_t;
	typedef void (VNotify::*member_t)(Vca::IInfoServer*, int xType, VString const iTarget);

    //  Construction
    public:
	Notice (
	    VNotify *pApplication, member_t pMember, int xType, VString iTarget, gofer_t *pGofer
	) : BaseClass (pApplication), m_pMember (pMember), m_xType (xType), m_iTarget (iTarget) {
	    retain (); {
		if (pGofer) {
		    pGofer->supplyMembers (this, &ThisClass::onValue, &ThisClass::onError);
		} 

	    } untain ();
	}

    //  Destruction
    private:
	~Notice () {
	}

    //  Access
    public:
	VNotify *thisapp () const {
	    return static_cast<VNotify*>(application ());
	}

    //  Callbacks
    private:
	void onValue (Vca::IInfoServer *pInterface) {
	    (thisapp ()->*m_pMember)(pInterface, m_xType, m_iTarget);
	    onDone ();
	}
	void onError (Vca::IError *pInterface, VString const &rMessage) {
	    fprintf(stderr, "%s", rMessage.content ());
	    onDone ();
	}

    //  State
    private:
	member_t const m_pMember;
	int const m_xType;
	VString const  m_iTarget;
    };
}

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VcaApps::VNotify);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VcaApps::VNotify::VNotify (Context *pContext)
: BaseClass (pContext)
{
    m_iInfoArray.Append (7);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VcaApps::VNotify::~VNotify () {
}


/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool VcaApps::VNotify::start_() {
    if (!BaseClass::start_())
	return false;

    char const *pEventNumber;
    Vca::U32 xEvent;
    char const *pEventNotes;
    VString iNotes;
    if (pEventNumber = commandStringValue ("eventNumber")) {
	xEvent = atoi (pEventNumber);
	if (xEvent < 500 || xEvent > 1000) {
	    iNotes << "Invalid event number: " << xEvent << " (Range: 500-1000); ";
	    xEvent = 499;
	}
    } else {
	iNotes << "Event number not specified; (Range: 500-1000)";
	xEvent = 499;
    }
    if (!(pEventNotes = commandStringValue ("eventNotes")))
	iNotes << "No event description.";
    else
	iNotes << pEventNotes << ".";
    
    VString iContent;
    iContent << "#" << xEvent << ": " << iNotes << "\n";
	
    VString iUserName;
    VString iHostName;
    VString iProcessId;
    VString iProcessName;
    VString iImageName;
    char iBuffer[256];

    iUserName.setTo (Vk_username (iBuffer, sizeof (iBuffer)));
    iHostName.setTo (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));

#if defined(__VMS)
    sprintf (iBuffer,"%X", getpid ());  
#elif defined (__linux__)
    sprintf (iBuffer,"%d", getpid ());  
#endif	    
    iProcessId.setTo (iBuffer);

#if defined(__VMS)
    get_process_name (iBuffer, sizeof (iBuffer));
#elif defined (__linux__)
    ctermid(iBuffer);
#endif
    iProcessName.setTo (iBuffer);

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

    m_iInfoArray[0].setTo (iContent.content ());
    m_iInfoArray[2].setTo (iUserName.content ());
    m_iInfoArray[3].setTo (iHostName.content ());
    m_iInfoArray[4].setTo (iProcessId.content ());
    m_iInfoArray[5].setTo (iProcessName.content ());
    m_iInfoArray[6].setTo (iImageName.content ());

    VTransientServices *pTSP = transientServicesProvider ();
    if (pTSP) {
	ActionInfos_t iActionInfos[5];
	VString const iNSName (getenv ("NSName"));
	VTransientServices::NSEntry::Reference pNSEntry;
	if (pTSP->getNSEntry (iNSName, pNSEntry)) {
	    if (pNSEntry.isntEmpty () && pNSEntry->infoServer ().isntEmpty ()) {
		pNSEntry->getActionInfos (xEvent, iActionInfos);
		for (int xType = 0; xType < 5; xType++) {
		    V::U32 iSize = iActionInfos[xType].cardinality ();
		    if (iSize>0) {
			for (V::U32 i = 0; i < iSize; i++) {
			    (new Notice (this, &ThisClass::sendNotice, xType, iActionInfos[xType][i], g_pInfoServerGofer))->discard ();
			}			
		    }
		}	

	    }
	}
    }
    return isStarting ();
}

void VcaApps::VNotify::sendNotice (Vca::IInfoServer *pInterface, int xType, VString iTarget) {
    if (pInterface) {
	VkDynamicArrayOf<VString> iInfoArray(7);
	for (V::U32 i=0; i < 7; i++) {
	    iInfoArray[i].setTo (m_iInfoArray[i].content ());
	}
	iInfoArray[1].setTo (iTarget.content ());
	pInterface -> Notify (0, xType, iInfoArray);
    }
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<VcaApps::VNotify> iMain (argc, argv);
    return iMain.processEvents ();
}


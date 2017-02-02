/*****  Vca Info Server  *****/

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
#include "Vca_VServerApplication.h"
#include "Vca_IInfoServer.h"
#include "V_VLogger.h"
#if !defined(sun)
#ifdef __VMS
#include <curl_root/include/curl/curl.h>
#else
#include <curl/curl.h>
#endif
#endif
#include <stdio.h>
#include <stdlib.h>

#include "VConfig.h"

/*********************************
 *********************************
 *****                       *****
 *****    Vca::VInfoServer   *****
 *****                       *****
 *********************************
 *********************************/

namespace Vca {
    class VInfoServer : public Vca::VServerApplication {
	DECLARE_CONCRETE_RTT (VInfoServer, Vca::VServerApplication);

    //  Construction
    public:
	VInfoServer (Context *pContext);

    //  Destruction
    private:
	~VInfoServer ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IInfoServer
    private:
	Vca::VRole<ThisClass,IInfoServer> m_pIInfoServer;
    public:
	void getRole (IInfoServer::Reference &rpRole) {
	    m_pIInfoServer.getRole (rpRole);
	}

    //  IInfoServer Callbacks
    public:
	void Notify (
	    IInfoServer *pRole, IClient *pClient, int iPriority, VkDynamicArrayOf<VString> const &rInfos
	);

    //  Startup
    private:
	bool start_();
    };
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (Vca::VInfoServer);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VInfoServer::VInfoServer (Context *pContext) : BaseClass (pContext), m_pIInfoServer (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VInfoServer::~VInfoServer () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VInfoServer::Notify (
    IInfoServer *pRole, IClient *pClient, int iPriority, VkDynamicArrayOf<VString> const &rInfos
) {
    // iPriority: 0-log; 1-mail; 2-page; 3-event manager
    V::VTime iNow;
    char iTimeStringBuffer[26];
    char const *timestring = iNow.ctime (iTimeStringBuffer);

    VString iMessage;
    VString iTime;
    iNow.asString (iTime);
    
    V::U32 cEntries = rInfos.cardinality ();
    V::U32 cExpected = 7;
    if (cEntries == cExpected) {
        iMessage << "|" << rInfos[3].content ();
        iMessage << "|" << rInfos[2].content ();
        iMessage << "|" << rInfos[4].content ();
        iMessage << "|" << rInfos[6].content ();
        iMessage << "|" << rInfos[0].content ();
    } else {
        for (V::U32 i = 0; i < cEntries; i++) 
            iMessage << rInfos[i].content () << "\n";
    }

    if (rInfos[1].isntEmpty ()) {
	if (iPriority == 0) {
	    V::VLogger const iLogger (rInfos[1]);
	    iLogger.printf ("---------------------------------------------------\n");
	    iLogger.printf ("%s", iMessage.content ());
	} else if (iPriority == 1) {
	    VString iSubject ("NS Event ");
	    VString iEvent, iOthers;
	    rInfos[0].getPrefix (':', iEvent, iOthers);
	    iSubject << iEvent.content () << " ";
	    iSubject << rInfos[3].content () << " ";
	    iSubject << rInfos[2].content () << " ";
	    iSubject << rInfos[4].content () << " ";
	    iSubject << rInfos[6].content ();
	    VString command = "/bin/mailx -s '";
	    command << iSubject;
	    command << "' ";
	    command << rInfos[1].content ();
	    FILE *mailService = popen (command.content (), "w");
	    fprintf (mailService, "%s%s", iTime.content (), iMessage.content ());
	    pclose (mailService);
        } else if (iPriority == 2) {
            VString command = "/bin/page ";
            command << rInfos[1].content ();
            command << " \"";
            command << iMessage.content ();
            command << "\"";

            FILE *pPageService = popen(command, "r");
            pclose (pPageService);
	} else if (iPriority == 3 || iPriority == 4) {
#if !defined(sun) 
	    CURL *pCurl;
	    CURLcode iResult;

	    VString iMessage;
	    rInfos[0].replaceSubstring ("\n", " ", iMessage);
	    VString iUserEnvironment;
	    if (rInfos[1]=="devel")
		iUserEnvironment.setTo ("3");
	    else if (rInfos[1]=="staging")
		iUserEnvironment.setTo ("2");
	    else if (rInfos[1]=="prod")
		iUserEnvironment.setTo ("1");
	    VString iProcessId ("ProcessID: ");
	    iProcessId << rInfos[4];

	    unsigned int const cHeaders = 3;
	    VkDynamicArrayOf<VString> iHeaders (cHeaders);
	    iHeaders[0].setTo ("Accept: application/json");
	    iHeaders[1].setTo ("Content-Type: application/json");
	    iHeaders[2].setTo ("charsets: utf-8");

	    unsigned int const cData = 11;
	    VkDynamicArrayOf<VString> iKey (cData);
	    VkDynamicArrayOf<VString> iValue (cData);

	    iKey[0].setTo ("Name");
	    iKey[1].setTo ("Type");
	    iKey[2].setTo ("ProductId");
	    iKey[3].setTo ("Status");
	    iKey[4].setTo ("StatusMsg");
	    iKey[5].setTo ("AlertModeName");
	    iKey[6].setTo ("UserEnvironment");
	    iKey[7].setTo ("CIVersion");
	    iKey[8].setTo ("Cluster");
	    iKey[9].setTo ("SendMonster");
	    iKey[10].setTo ("SendRPD");

	    iValue[0].setTo (rInfos[6]);
	    iValue[1].setTo (iProcessId);
	    iValue[2].setTo ("3320");
	    iValue[3].setTo ("RAISE");
            iValue[4].setTo ("Check event information above");
	    iValue[5].setTo (rInfos[1]);
	    iValue[6].setTo (iUserEnvironment);
	    iValue[7].setTo ("3");
            VString iCluster (rInfos[3]);
            iCluster << " - (Event) - " << iMessage.content();
            iValue[8].setTo (iCluster);

	    if (iPriority == 3) {
		iValue[9].setTo ("1");
		iValue[10].setTo ("0");
	    } else {
		iValue[9].setTo ("0");
		iValue[10].setTo ("1");
	    }

	    struct curl_slist *pHeaders = NULL; 
	    for (unsigned int xElement = 0; xElement < cHeaders; xElement++) {
		pHeaders = curl_slist_append (pHeaders, iHeaders[xElement].content ());
	    }

	    VString iData ("{\"");
	    for (unsigned int xElement = 0; xElement < cData-1; xElement++) {
		iData << iKey[xElement] << "\":\"";
		iData << iValue[xElement] << "\", \"";
	    }
	    iData << iKey[cData-1] << "\":\"" << iValue[cData-1] << "\"}";

	    VString iAgent ("curl/7.15.5 (x86_64-redhat-linux-gnu) libcurl/7.15.5 OpenSSL/0.9.8b zlib/1.2.3 libidn/0.6.5");
#if defined(V_USING_DEFAULT_INFO_URL_DEFINED)
	    V_DEFAULT_INFO_URL;
#else
	    VString iUrl ("");
#endif

	    curl_global_init (CURL_GLOBAL_ALL);
	    pCurl = curl_easy_init ();

	    if (pCurl) {
		FILE *pSink;
#if defined (_WIN32)
		pSink = fopen ("NUL:", "w+");
#else
		pSink = fopen ("/dev/null", "w+");
#endif
		curl_easy_setopt (pCurl, CURLOPT_HTTPHEADER, pHeaders);
		curl_easy_setopt (pCurl, CURLOPT_USERAGENT, iAgent.content ()); 
		curl_easy_setopt (pCurl, CURLOPT_URL, iUrl.content ());
		curl_easy_setopt (pCurl, CURLOPT_POST, 1);
		curl_easy_setopt (pCurl, CURLOPT_POSTFIELDS, iData.content ());
		curl_easy_setopt (pCurl, CURLOPT_POSTFIELDSIZE, (long)strlen (iData.content ()));
		curl_easy_setopt (pCurl, CURLOPT_WRITEHEADER, pSink);
		curl_easy_setopt (pCurl, CURLOPT_WRITEDATA, pSink);

		iResult = curl_easy_perform (pCurl);
		if (iResult != CURLE_OK)
		    fprintf (stderr, "Vca Notification failed in curl posting: %s\n", curl_easy_strerror (iResult));
		fclose (pSink);
		curl_easy_cleanup (pCurl);
	    }
#endif
        }
    }
}



/*********************
 *********************
 *****           *****
 *****  Startup  *****
 *****           *****
 *********************
 *********************/

bool Vca::VInfoServer::start_() {
    if (!BaseClass::start_())
	return false;
    CommandCursor iCommandCursor (this);
    char const *const pListenerAddress = iCommandCursor.nextToken ();
    if (pListenerAddress) 
     	offerSelf (pListenerAddress);
    else {
	fprintf (stderr, "Usage: No address to offer object.\n");
	setExitStatus (ErrorExitValue);
	stop ();
    }
    return isStarting ();
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vca::VInfoServer> iMain (argc, argv);
    return iMain.processEvents ();
}

/*****  Vca::VSiteInfo Implementation  *****/

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

#include "Vca_VSiteInfo.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_IProcessInfoSink.h"

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::SiteInfo  *****
 *****                 *****
 ***************************
 ***************************/

/*********************************
 *********************************
 *****  AuthoritativeSource  *****
 *********************************
 *********************************/

namespace Vca {
    namespace SiteInfo {
	AuthoritativeSource::AuthoritativeSource () {
	}
	AuthoritativeSource::~AuthoritativeSource () {
	}
	void AuthoritativeSource::onValueNeeded () {
	    onAuthoritativeSourceNeeded_();
	}
    }
}

/*****************
 *****************
 *****  PID  *****
 *****************
 *****************/

namespace Vca {
    namespace SiteInfo {
	PID::PID () {
	}
	PID::~PID () {
	}
	void PID::onValueNeeded () {
	    onPIDNeeded_();
	}
    }
}

/*****************
 *****************
 *****  UID  *****
 *****************
 *****************/

namespace Vca {
    namespace SiteInfo {
	UID::UID () {
	}
	UID::~UID () {
	}
	void UID::onValueNeeded () {
	    onUIDNeeded_();
	}
    }
}

/*****************
 *****************
 *****  GID  *****
 *****************
 *****************/

namespace Vca {
    namespace SiteInfo {
	GID::GID () {
	}
	GID::~GID () {
	}
	void GID::onValueNeeded () {
	    onGIDNeeded_();
	}
    }
}

/**********************
 **********************
 *****  HostName  *****
 **********************
 **********************/

namespace Vca {
    namespace SiteInfo {
	HostName::HostName () {
	}
	HostName::~HostName () {
	}
	void HostName::onValueNeeded () {
	    onHostNameNeeded_();
	}
    }
}

/**********************
 **********************
 *****  UserName  *****
 **********************
 **********************/

namespace Vca {
    namespace SiteInfo {
	UserName::UserName () {
	}
	UserName::~UserName () {
	}
	void UserName::onValueNeeded () {
	    onUserNameNeeded_();
	}
    }
}

/***********************
 ***********************
 *****  GroupName  *****
 ***********************
 ***********************/

namespace Vca {
    namespace SiteInfo {
	GroupName::GroupName () {
	}
	GroupName::~GroupName () {
	}
	void GroupName::onValueNeeded () {
	    onGroupNameNeeded_();
	}
    }
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VSiteInfo::ProcessInfoRequest  *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSiteInfo::ProcessInfoRequest::ProcessInfoRequest (
    VSiteInfo* pSiteInfo
) : m_pSiteInfo (pSiteInfo), m_cRequests (0), m_iPID (0), m_iUID (0), m_iGID (0) {
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSiteInfo::ProcessInfoRequest::~ProcessInfoRequest () {
}


/********************************
 ********************************
 *****  Request Initiators  *****
 ********************************
 ********************************/

void Vca::VSiteInfo::ProcessInfoRequest::requestData () {
    onRequest ();

    requestPID ();
    requestUID ();
    requestGID ();
    requestHostName ();
    requestUserName ();
    requestGroupName ();

    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::requestPID () {
    onRequest ();
    m_pSiteInfo->supplyPID (this, &ThisClass::onPID, &ThisClass::onError);
}

void Vca::VSiteInfo::ProcessInfoRequest::requestUID () {
    onRequest ();
    m_pSiteInfo->supplyUID (this, &ThisClass::onUID, &ThisClass::onError);
}

void Vca::VSiteInfo::ProcessInfoRequest::requestGID () {
    onRequest ();
    m_pSiteInfo->supplyGID (this, &ThisClass::onGID, &ThisClass::onError);
}

void Vca::VSiteInfo::ProcessInfoRequest::requestHostName () {
    onRequest ();
    m_pSiteInfo->supplyHostName (this, &ThisClass::onHostName, &ThisClass::onError);
}

void Vca::VSiteInfo::ProcessInfoRequest::requestUserName () {
    onRequest ();
    m_pSiteInfo->supplyUserName (this, &ThisClass::onUserName, &ThisClass::onError);
}

void Vca::VSiteInfo::ProcessInfoRequest::requestGroupName () {
    onRequest ();
    m_pSiteInfo->supplyGroupName (this, &ThisClass::onGroupName, &ThisClass::onError);
}


/********************************
 ********************************
 *****  Response Callbacks  *****
 ********************************
 ********************************/

void Vca::VSiteInfo::ProcessInfoRequest::onReply () {
    if (0 == --m_cRequests) {
	onData (m_pSiteInfo, m_iPID, m_iUID, m_iGID, m_iHostName, m_iUserName, m_iGroupName);
    }
}

void Vca::VSiteInfo::ProcessInfoRequest::onPID (ProcessInfo::pid_t iValue) {
    m_iPID = iValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onUID (ProcessInfo::pid_t iValue) {
    m_iUID = iValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onGID (ProcessInfo::pid_t iValue) {
    m_iGID = iValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onHostName (VString const& rValue) {
    m_iHostName = rValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onUserName (VString const& rValue) {
    m_iUserName = rValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onGroupName (VString const& rValue) {
    m_iGroupName = rValue;
    onReply ();
}

void Vca::VSiteInfo::ProcessInfoRequest::onError (IError* pInterface, VString const& rMessage) {
    onReply ();
}


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::VSiteInfo::ProcessInfoSinkRequest  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

namespace Vca {
    class VSiteInfo::ProcessInfoSinkRequest : public ProcessInfoRequest {
	DECLARE_CONCRETE_RTTLITE (ProcessInfoSinkRequest, ProcessInfoRequest);

    //  Construction
    public:
	template <typename sink_t> ProcessInfoSinkRequest (
	    VSiteInfo* pSiteInfo, sink_t* pInfoSink
	) : BaseClass (pSiteInfo), m_pInfoSink (pInfoSink) {
	    retain (); {
		requestData ();
	    } untain ();
	}

    //  Destruction
    private:
	~ProcessInfoSinkRequest ();

    //  Response Callbacks
    private:
	void onData (
	    VSiteInfo*		pSiteInfo,
	    ProcessInfo::pid_t	iPID,
	    ProcessInfo::uid_t	iUID,
	    ProcessInfo::gid_t	iGID,
	    VString const&	rHostName,
	    VString const&	rUserName,
	    VString const&	rGroupName
	);

    //  State
    private:
	IClient::Reference const m_pInfoSink;
    };
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VSiteInfo::ProcessInfoSinkRequest::~ProcessInfoSinkRequest () {
}

/************************
 ************************
 *****  Completion  *****
 ************************
 ************************/

void Vca::VSiteInfo::ProcessInfoSinkRequest::onData (
    VSiteInfo*		pSiteInfo,
    ProcessInfo::pid_t	iPID,
    ProcessInfo::uid_t	iUID,
    ProcessInfo::gid_t	iGID,
    VString const&	rHostName,
    VString const&	rUserName,
    VString const&	rGroupName
) {
    if (IProcessInfoSink *const pInfoSink = dynamic_cast<IProcessInfoSink*>(m_pInfoSink.referent ())) {
	IProcessInfo::Reference pIProcessInfo;
	pSiteInfo->getRole (pIProcessInfo);
	pInfoSink->OnBasics (pIProcessInfo, iPID, iUID, iGID, rHostName, rUserName, rGroupName);
    } else if (IVReceiver<VString const&> *const pInfoSink = dynamic_cast<IVReceiver<VString const&>*>(m_pInfoSink.referent ())) {
	VString iProcessInfo; {
	    VkUUIDHolder const iSiteUUID (siteUUID ());
	    iSiteUUID.GetString (iProcessInfo);
	}
	iProcessInfo << " [" << rUserName << "@" << rHostName << ", " << iPID << "]";
	pInfoSink->OnData (iProcessInfo);
    }
}


/****************************
 ****************************
 *****                  *****
 *****  Vca::VSiteInfo  *****
 *****                  *****
 ****************************
 ****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VSiteInfo::VSiteInfo (
    VcaPeerCharacteristics const& rCharacteristics
) : m_iCharacteristics (rCharacteristics), m_pIProcessInfo (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VSiteInfo::~VSiteInfo () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void Vca::VSiteInfo::cacheProcessInfo () {
//  Initiate a data request if the PID is hasn't been requested yet...
    if (static_cast<PID*>(this)->holdsNothing ())
	(new ProcessInfoSinkRequest (this, static_cast<IProcessInfoSink*>(0)))->discard ();
}

void Vca::VSiteInfo::supplyProcessInfo (IProcessInfoSink* pInfoSink) {
    if (pInfoSink) {
	(new ProcessInfoSinkRequest (this, pInfoSink))->discard ();
    }
}

void Vca::VSiteInfo::supplyProcessInfo (IVReceiver<VString const&>* pInfoSink) {
    if (pInfoSink) {
	(new ProcessInfoSinkRequest (this, pInfoSink))->discard ();
    }
}

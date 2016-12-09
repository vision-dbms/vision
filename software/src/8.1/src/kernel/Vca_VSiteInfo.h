#ifndef Vca_VSiteInfo_Interface
#define Vca_VSiteInfo_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VInstanceSource.h"

#include "Vca_IGetter_Ex2.h"
#include "Vca_IProcessInfo.h"

#include "Vca_VcaPeerCharacteristics.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IProcessInfoSink;

    /********************************
     *----  namespace SiteInfo  ----*
     ********************************/
    namespace SiteInfo {

	/*------------------------------*
	 *----  AuthoritativeSource ----*
	 *------------------------------*/
	class AuthoritativeSource : public VInstanceSource<IProcessInfo*> {
	    DECLARE_FAMILY_MEMBERS (AuthoritativeSource, VInstanceSource<IProcessInfo*>);
	public:
	    AuthoritativeSource ();
	    ~AuthoritativeSource ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onAuthoritativeSourceNeeded_() = 0;
	};

	/*--------------*
	 *----  PID ----*
	 *--------------*/
	class PID : public VInstanceSource<ProcessInfo::pid_t> {
	    DECLARE_FAMILY_MEMBERS (PID, VInstanceSource<ProcessInfo::pid_t>);
	public:
	    PID ();
	    ~PID ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onPIDNeeded_() = 0;
	};

	/*--------------*
	 *----  UID ----*
	 *--------------*/
	class UID : public VInstanceSource<ProcessInfo::uid_t> {
	    DECLARE_FAMILY_MEMBERS (UID, VInstanceSource<ProcessInfo::uid_t>);
	public:
	    UID ();
	    ~UID ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onUIDNeeded_() = 0;
	};

	/*--------------*
	 *----  GID ----*
	 *--------------*/
	class GID : public VInstanceSource<ProcessInfo::gid_t> {
	    DECLARE_FAMILY_MEMBERS (GID, VInstanceSource<ProcessInfo::gid_t>);
	public:
	    GID ();
	    ~GID ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onGIDNeeded_() = 0;
	};

	/*-------------------*
	 *----  HostName ----*
	 *-------------------*/
	class HostName : public VInstanceSource<VString const&> {
	    DECLARE_FAMILY_MEMBERS (HostName, VInstanceSource<VString const&>);
	public:
	    HostName ();
	    ~HostName ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onHostNameNeeded_() = 0;
	};

	/*-------------------*
	 *----  UserName ----*
	 *-------------------*/
	class UserName : public VInstanceSource<VString const&> {
	    DECLARE_FAMILY_MEMBERS (UserName, VInstanceSource<VString const&>);
	public:
	    UserName ();
	    ~UserName ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onUserNameNeeded_() = 0;
	};

	/*--------------------*
	 *----  GroupName ----*
	 *--------------------*/
	class GroupName : public VInstanceSource<VString const&> {
	    DECLARE_FAMILY_MEMBERS (GroupName, VInstanceSource<VString const&>);
	public:
	    GroupName ();
	    ~GroupName ();
	private:
	    virtual void onValueNeeded ();
	    virtual void onGroupNameNeeded_() = 0;
	};
    }

/*****************************
 *----  class VSiteInfo  ----*
 *****************************/
    class Vca_API VSiteInfo
	: public VRolePlayer
	, /*protected*/ public SiteInfo::AuthoritativeSource
	, /*protected*/ public SiteInfo::PID
	, /*protected*/ public SiteInfo::UID
	, /*protected*/ public SiteInfo::GID
	, /*protected*/ public SiteInfo::HostName
	, /*protected*/ public SiteInfo::UserName
	, /*protected*/ public SiteInfo::GroupName
    {
	DECLARE_ABSTRACT_RTTLITE (VSiteInfo, VRolePlayer);

    //  ProcessInfoRequest
    public:
	class Vca_API ProcessInfoRequest : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (ProcessInfoRequest, VRolePlayer);

	//  Construction
	protected:
	    ProcessInfoRequest (VSiteInfo* pSiteInfo);

	//  Destruction
	protected:
	    ~ProcessInfoRequest ();

	//  Access
	public:
	    VSiteInfo *site () const {
		return m_pSiteInfo;
	    }
	    VcaPeerCharacteristics const &siteCharacteristics () const {
		return site ()->siteCharacteristics ();
	    }
	    uuid_t const &siteUUID () const {
		return site ()->siteUUID ();
	    }

	//  Request Initiators
	protected:
	    void requestData ();
	private:
	    void onRequest () {
		m_cRequests++;
	    }

	    void requestPID ();
	    void requestUID ();
	    void requestGID ();
	    void requestHostName ();
	    void requestUserName ();
	    void requestGroupName ();

	//  Response Callbacks
	private:
	    void onReply ();

	    void onPID (ProcessInfo::pid_t iValue);
	    void onUID (ProcessInfo::uid_t iValue);
	    void onGID (ProcessInfo::gid_t iValue);
	    void onHostName (VString const& rValue);
	    void onUserName (VString const& rValue);
	    void onGroupName (VString const& rValue);

	    void onError (IError* pInterface, VString const& rMessage);

	    virtual void onData (
		VSiteInfo*		pSiteInfo,
		ProcessInfo::pid_t	iPID,
		ProcessInfo::uid_t	iUID,
		ProcessInfo::gid_t	iGID,
		VString const&		rHostName,
		VString const&		rUserName,
		VString const&		rGroupName
	    ) = 0;

	//  State
	private:
	    VSiteInfo::Reference const	m_pSiteInfo;
	    unsigned int		m_cRequests;
	    ProcessInfo::pid_t		m_iPID;
	    ProcessInfo::uid_t		m_iUID;
	    ProcessInfo::gid_t		m_iGID;
	    VString			m_iHostName;
	    VString			m_iUserName;
	    VString			m_iGroupName;
	};
	class ProcessInfoSinkRequest;

    //  Construction
    protected:
	VSiteInfo (VcaPeerCharacteristics const& rPeerCharacteristics);

    //  Destruction
    protected:
	~VSiteInfo ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  IProcessInfo Role
    private:
	VRole<ThisClass,IProcessInfo> m_pIProcessInfo;
    public:
	void getRole (IProcessInfo::Reference& rpRole) {
	    m_pIProcessInfo.getRole (rpRole);
	}

    //  Site Info
    public:
	VcaPeerCharacteristics const& siteCharacteristics () const {
	    return m_iCharacteristics;
	}
	uuid_t const& siteUUID () const {
	    return m_iCharacteristics.uuid ();
	}

	void cacheProcessInfo ();

	void supplyProcessInfo (IProcessInfoSink* pInfoSink);
	void supplyProcessInfo (IVReceiver<VString const&>* pInfoSink);

        /**
         * Called by a consumer interested in obtaining the AuthoritativeSource for the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the AuthoritativeSource has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyAuthoritativeSource (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(AuthoritativeSource::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        AuthoritativeSource::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<AuthoritativeSource*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the PID of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the PID has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyPID (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(PID::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        PID::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<PID*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the UID of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the UID has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyUID (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(UID::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        UID::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<UID*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the GID of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the GID has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyGID (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(GID::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        GID::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<GID*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the HostName of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the HostName has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyHostName (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(HostName::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        HostName::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<HostName*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the UserName of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the UserName has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyUserName (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(UserName::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        UserName::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<UserName*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

        /**
         * Called by a consumer interested in obtaining the GroupName of the site represented by this object.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be when the GroupName has been retrieved.
         * @param pErrorMember the callback that should be invoked if the request has failed.
         */
	template <typename class_t> void supplyGroupName (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(GroupName::val_t),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
	 *  The Sun compiler doesn't like:
	 *
	 *        GroupName::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	 *
	 *  but is happy with the static_cast<...>:
	 *
	 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	    static_cast<GroupName*>(this)->supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}

    //  State
    private:
	VcaPeerCharacteristics const m_iCharacteristics;
    };
}

#endif

#ifndef Vca_VDevice__Interface
#define Vca_VDevice__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDevice.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/


/**************************************
 *****                            *****
 *****  VDevice_<Implementation>  *****
 *****                            *****
 **************************************/

/****************************************************************************
 *>>>>									<<<<*
 *>>>>	The 'static_cast<BaseType*>(this)->inherited ()' constructs	<<<<*
 *>>>>	below work around a very serious bug in MSVC6.  Left to its	<<<<*
 *>>>>	own devices, MSVC6 routinely passes the method being called a	<<<<*
 *>>>>  'this' pointer using the vtable it has in hand instead of a	<<<<*
 *>>>>	vtable appropriate to the base class of method being invoked.	<<<<*
 *>>>>	Unfortunately, while this workaround solves one very bad	<<<<*
 *>>>>	problem, it also steps 'outside' the semantics of inheritance	<<<<*
 *>>>>	so that the base class must either make these members public	<<<<*
 *>>>>	or define friend relationships for every class derived from	<<<<*
 *>>>>	the base (BAD MICROSOFT, BAD, MICROSOFT, BAD DOG, BAD!!!!).	<<<<*
 *>>>>									<<<<*
 ****************************************************************************/

namespace Vca {
    // Forward declarations.
    template <class T> class VBSPoll_;
    template <class T> class VBSMove_;

    template <class Implementation> class VDevice_ : public VDevice, public Implementation {
	DECLARE_ABSTRACT_RTTLITE (VDevice_<Implementation>, VDevice);

    /**
     * @name Type Bundles
     * These structs bundle together types by their intended consumer type. Each type bundle below provides the same typedef names with different typedef values, each appropriate for a particular direction of traffic.
     */
    //@{
    public:
        /** Type bundle for use with producer devices. */
	struct ProducerTypes {
            /** The buffer type over which byte stream operations occur. */
	    typedef VDeviceBSReadArea			Area;
            /** The Use for this type of device. */
	    typedef VDeviceBSRead			Use;
            /** The User for this type of device. */
	    typedef VDeviceBSReader			User;
            /** The Implementation of this device. */
	    typedef Implementation			ImplDev;
            /** The Implementation Use of this device. */
	    typedef typename Implementation::BSGet	ImplUse;
            typedef VDevicePoll                         PollUse;
            typedef typename Implementation::ReadPoll   PollImplUse;
	};

        /** Type bundle for use with consumer devices. */
	struct ConsumerTypes {
            /** The buffer type over which byte stream operations occur. */
	    typedef VDeviceBSWriteArea			Area;
            /** The Use for this type of device. */
	    typedef VDeviceBSWrite			Use;
            /** The User for this type of device. */
	    typedef VDeviceBSWriter			User;
            /** The Implementation of this device. */
	    typedef Implementation			ImplDev;
            /** The Implementation Use of this device. */
	    typedef typename Implementation::BSPut	ImplUse;
            typedef VDevicePoll                         PollUse;
            typedef typename Implementation::WritePoll  PollImplUse;
	};
    //@}

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;
        typedef VBSPoll_<ProducerTypes> ReadPoll;
        typedef VBSPoll_<ConsumerTypes> WritePoll;
	typedef VBSMove_<ProducerTypes> Get;
	typedef VBSMove_<ConsumerTypes> Put;

    //  Construction
    protected:
	VDevice_(
	    ImplementationData &rDeviceData
	) : BaseClass (rDeviceData.cohort ()), Implementation (this, rDeviceData) {
	    static_cast<Implementation*>(this)->makeDeviceFriendly ();
	}

    //  Destruction
    protected:
	~VDevice_() {
	}

    //  Face Implementation
    public:
        /** Used to retrieve the device name via the Implementation delegate. */
	bool getName (VkStatus &rStatus, VString &rName);
#ifndef __VMS
        bool getSocket (VkStatus &rStatus, SOCKET& rSocket);
    private:
        virtual bool getSocket_ (VkStatus &rStatus, SOCKET& rSocket);
#endif
    };

    template<class Implementation> bool VDevice_<Implementation>::getName (VkStatus &rStatus, VString &rName) {
	return static_cast<Implementation*>(this)->getName (rStatus, rName);
    }
#ifndef __VMS
    template<class Implementation> bool VDevice_<Implementation>::getSocket (VkStatus &rStatus, SOCKET& rSocket) {
	return static_cast<Implementation*>(this)->getSocket (rStatus, rSocket);
    }
    template<class Implementation> bool VDevice_<Implementation>::getSocket_ (VkStatus &rStatus, SOCKET& rSocket) {
	return getSocket (rStatus, rSocket);
    }
#endif
}


/**********************************
 *****                        *****
 *****  Device Use Templates  *****
 *****                        *****
 **********************************/

/****************************************************************************
 *>>>>	VBSMove_, VBSConnect_, etc. ought to be nested types of the	<<<<*
 *>>>>	VDevice_ template.  They cannot be written that way because	<<<<*
 *>>>>	MSVC6 neglects to include definitions for the static data	<<<<*
 *>>>>	members of nested types in a template.				<<<<*
 ****************************************************************************/

/*****************************
 *----  VBSMove_<Types>  ----*
 *****************************/

namespace Vca {
    /*************************************************************************
     *>>>>  The VBSMoveBase_<Types> template is another workaround for   <<<<*
     *>>>>  the benefit of MSVC 6.  This fine Microsoft product gets     <<<<*
     *>>>>  quite confused when given too many things to keep track of   <<<<*
     *>>>>  at once.  Sort of like walking and chewing gum.              <<<<*
     *************************************************************************
     *                                                                       *
     *                        >>>>>  WALK  <<<<<                             *
     *                                                                       *
     *************************************************************************/
    template<class Types> class VBSMoveBase_ : public Types::Use {
    protected:
	VBSMoveBase_() {
	}
	~VBSMoveBase_() {
	}
    };

    /*************************************************************************
     *                                                                       *
     *                        >>>>>  CHEW  <<<<<                             *
     *                                                                       *
     *************************************************************************/
    /**
     * Supports movement of data to/from a byte stream.
     *
     * @todo Get rid of VBSMoveBase_ entirely, publically inheriting directly from Types::Use right here; we're no longer using MSVC6, so hopefully the bug that VBSMoveBase_ was created to solve isn't of concern anymore.
     */
    template<class Types> class VBSMove_ : public VBSMoveBase_<Types>, private Types::ImplUse {
	DECLARE_CONCRETE_RTTLITE (VBSMove_<Types>, VBSMoveBase_<Types>);

    //  Aliases
    public:
	typedef typename Types::Area	Area;
	typedef typename Types::User	User;
	typedef typename Types::ImplDev DevImpl;
	typedef typename Types::ImplUse	UseImpl;

    //  Construction
    public:
	VBSMove_(VDevice_<DevImpl> *pDevice) : UseImpl (this, pDevice) {
	}

    //  Destruction
    private:
	~VBSMove_() {
	}

    //  Access
    private:
        /**
         * @todo: remove static_cast here, was put in to avoid a dominance bug in MSVC.
         */
	virtual VDeviceUse *use_() OVERRIDE {
	    return static_cast<BaseClass*>(this);
	}
	virtual VDevice::User *user_() OVERRIDE {
	    return static_cast<BaseClass*>(this)->user ();
	}
    public:
	ThisClass *use () {
	    return this;
	}

    //  Startup
    public:
//	'using BaseClass::start' doesn't work in MSVC6:
	bool start (VkStatus &rStatus, User *pUser, Area const &rArea) {
	    return static_cast<BaseClass*>(this)->start (rStatus, pUser, rArea);
	}
    private:
	virtual bool start_(VkStatus &rStatus, Area const &rArea) OVERRIDE {
	    return static_cast<UseImpl*>(this)->start (rStatus, rArea);
	}

    //  Callback
    private:
	virtual void triggerUser_() OVERRIDE {
	    static_cast<BaseClass*>(this)->triggerUser ();
	}

    //  Completion
    private:
	virtual bool onData_(size_t sTransfer, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (sTransfer, bKeep);
	}
	virtual bool onData_(VDevice *pDevice, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (pDevice, bKeep);
	}
	virtual bool onError_(VkStatus const &rStatus, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onError (rStatus, bKeep);
	}

    //  Recycling
    private:
	virtual void recycle_() OVERRIDE {
	    BaseClass::recycle ();
	}
    };
}


/***************************
 ***** VBSPoll_<Types> *****
 ***************************/
namespace Vca {
    template<class Types> class VBSPoll_ : public Types::PollUse, private Types::PollImplUse {
	DECLARE_CONCRETE_RTTLITE (VBSPoll_<Types>, typename Types::PollUse);

    //  Aliases
    public:
	typedef typename Types::User	User;
	typedef typename Types::ImplDev DevImpl;
	typedef typename Types::PollImplUse UseImpl;

    //  Construction
    public:
	VBSPoll_(VDevice_<DevImpl> *pDevice) : UseImpl (this, pDevice) {
	}

    //  Destruction
    private:
	~VBSPoll_() {
	}

    //  Access
    private:
	virtual VDeviceUse *use_() OVERRIDE {
	    return static_cast<BaseClass*>(this);
	}
	virtual VDevice::User *user_() OVERRIDE {
	    return static_cast<BaseClass*>(this)->user ();
	}
    public:
	ThisClass *use () {
	    return this;
	}

    //  Startup
    public:
	bool start (VkStatus &rStatus, User *pUser) {
	    return static_cast<BaseClass*>(this)->start (rStatus, pUser);
	}
    private:
	virtual bool start_(VkStatus &rStatus) OVERRIDE {
	    return static_cast<UseImpl*>(this)->start (rStatus);
	}

    //  Callback
    private:
	virtual void triggerUser_() OVERRIDE {
	    static_cast<BaseClass*>(this)->triggerUser ();
	}

    //  Completion
    private:
	virtual bool onData_(size_t sTransfer, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (sTransfer, bKeep);
	}
	virtual bool onData_(VDevice *pDevice, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (pDevice, bKeep);
	}
	virtual bool onError_(VkStatus const &rStatus, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onError (rStatus, bKeep);
	}

    //  Recycling
    private:
	virtual void recycle_() OVERRIDE {
	    BaseClass::recycle ();
	}
    };
}


/********************************************
 *----  VAccept_<DeviceImplementation>  ----*
 ********************************************/

namespace Vca {
    template<class Impl> class VAccept_ : public VDeviceListen, private Impl::AcceptUse {
	DECLARE_CONCRETE_RTTLITE (VAccept_<Impl>, VDeviceListen);

    //  Aliases
    public:
	typedef VDeviceListener			User;
	typedef Impl				DevImpl;
	typedef typename Impl::AcceptUse	UseImpl;

    //  Construction
    public:
	VAccept_(VDevice_<DevImpl> *pDevice) : UseImpl (this, pDevice) {
	}

    //  Destruction
    private:
	~VAccept_() {
	}

    //  Access
    private:
	virtual VDeviceUse *use_() OVERRIDE {
	//  The static cast is needed to avoid a dominance bug in MSVC:
	    return static_cast<BaseClass*>(this);
	}
	virtual VDeviceUser *user_() OVERRIDE {
	    return static_cast<BaseClass*>(this)->user ();
	}
    public:
	ThisClass *use () {
	    return this;
	}

    //  Startup
    public:
//	'using BaseClass::start' doesn't work in MSVC6:
	bool start (VkStatus &rStatus, User *pUser) {
	    return static_cast<BaseClass*>(this)->start (rStatus, pUser);
	}
    private:
	virtual bool start_(VkStatus &rStatus) OVERRIDE {
	    return static_cast<UseImpl*>(this)->start (rStatus);
	}

    //  Callback
    private:
	virtual void triggerUser_() OVERRIDE {
	    static_cast<BaseClass*>(this)->triggerUser ();
	}

    //  Completion
    private:
	virtual bool onData_(size_t sTransfer, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (sTransfer, bKeep);
	}
	virtual bool onData_(VDevice *pDevice, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onData (pDevice, bKeep);
	}
	virtual bool onError_(VkStatus const &rStatus, bool bKeep) OVERRIDE {
	    return static_cast<BaseClass*>(this)->onError (rStatus, bKeep);
	}

    //  Recycling
    private:
	virtual void recycle_() OVERRIDE {
	    recycle ();
	}
    };
}
#endif

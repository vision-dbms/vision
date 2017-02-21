/*****  Vca_VDeviceFactory Implementation  *****/

#ifdef __VMS

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#endif

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VkSocketAddress.h"

typedef char const *pointer_const_t;

#if defined(__VMS)
/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

#ifdef INVALID_SOCKET
#undef INVALID_SOCKET
#endif
#ifdef SOCKET
#undef SOCKET
#endif

#include "cast_32.h"		// omit:Linux omit:SunOS

#define V_USING_CUSTOM_DESCRIPTOR 1
#include "VConfig.h"

#include <cmbdef.h>
#include <clidef.h>
#include <dcdef.h>
#include <dvidef.h>
#include <efndef.h>
#include <gen64def.h>
#include <iledef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <jpidef.h>
#include <lib$routines.h>
#include <libdtdef.h>
#include <prcdef.h>
#include <starlet.h>
#include <ssdef.h>
#include <stsdef.h>
#include <tis.h>

#include "multinet_inetiodef.h"	// omit:Linux omit:SunOS

extern "C" {
    void *decc$malloc (size_t sAllocation);
    void *decc$free (void *pMemory);
}

#pragma __required_pointer_size __save
#pragma __required_pointer_size __long

extern "C" {
    typedef void (*astptr_t)(__unknown_params);
}

#pragma __required_pointer_size __restore

#pragma __pointer_size __save
#pragma __pointer_size __short

extern "C" {
    typedef void (*astptr32_t)(__unknown_params);
}

#pragma __pointer_size __restore

#define TCPIP$C_IPOPT 0
#define TCPIP$C_SOCKOPT 1
#define TCPIP$C_IOCTL 2
#define TCPIP$C_SOCK_NAME 4
#define TCPIP$C_TCPOPT 6

#define TCPIP$C_DSC_RCV 0
#define TCPIP$C_DSC_SND 1
#define TCPIP$C_DSC_ALL 2

#define TCPIP$C_IP_OPTIONS 1            /* buf/ip_opts; set/get IP per-packet options  */
#define TCPIP$C_IP_HDRINCL 2            /* int; header is included with data (raw)  */
#define TCPIP$C_IP_TOS 3                /* int; IP type of service and precedence */
#define TCPIP$C_IP_TTL 4                /* int; IP time to live             */
#define TCPIP$C_IP_RECVOPTS 5           /* bool; receive all IP options w/datagram  */
#define TCPIP$C_IP_DUMMY1 6             /* reserve space                    */
#define TCPIP$C_IP_RECVDSTADDR 7        /* bool; receive IP dst addr w/datagram  */
#define TCPIP$C_IP_MULTICAST_IF 16      /* set/get IP multicast interface   */
#define TCPIP$C_IP_MULTICAST_TTL 17     /* set/get IP multicast timetolive  */
#define TCPIP$C_IP_MULTICAST_LOOP 18    /* set/get IP multicast loopback    */
/* For binary compatability with UCX 4.n, TCPIP kernel accepts both 12 & 19 */
/* for IP_ADD_MEMBERSHIP.                                                   */
#define TCPIP$C_IP_ADD_MEMBERSHIP 19    /* add  an IP group membership      */
#define TCPIP$C_IP_DROP_MEMBERSHIP 20   /* drop an IP group membership      */
#define TCPIP$C_IP_MULTICAST_VIF 21     /* set/get IP mcast vir. interface  */

#define TCPIP$C_TCP_NODELAY 1
#define TCPIP$C_TCP_MAXSEG 2
#define TCPIP$C_TCP_KEEPIDLE 4
#define TCPIP$C_TCP_KEEPINTVL 5
#define TCPIP$C_TCP_KEEPCNT 6
#define TCPIP$C_TCP_KEEPINIT 7
#define TCPIP$C_TCP_NODELACK 9
#define TCPIP$C_TCP_TSOPTENA 16
#define TCPIP$C_TCP_PAWS 32
#define TCPIP$C_TCP_SACKENA 64
#define TCPIP$C_TCP_PROBE_IDLE 128
#define TCPIP$C_TCP_DROP_IDLE 129

#define TCPIP$C_SO_SNDBUF 4097          /* 0x1001 send buffer size          */
#define TCPIP$C_SO_RCVBUF 4098          /* 0x1002 receive buffer size       */
#define TCPIP$C_SO_SNDLOWAT 4099        /* 0x1003 send low-water mark       */
#define TCPIP$C_SO_RCVLOWAT 4100        /* 0x1004 receive low-water mark    */
#define TCPIP$C_SO_SNDTIMEO 4101        /* 0x1005 send timeout              */
#define TCPIP$C_SO_RCVTIMEO 4102        /* 0x1006 receive timeout           */
#define TCPIP$C_SO_ERROR 4103           /* 0x1007 get error status and clear */
#define TCPIP$C_SO_TYPE 4104            /* 0x1008 get socket type           */
#define TCPIP$C_SO_SHARE 4105           /* 0x1009 ovms Share between processes */
#define TCPIP$C_SO_CCL 4106             /* 0x100a ovms Carriage Control socket */
#define TCPIP$C_SO_STATE 4107           /* 0x100b get socket state bits     */
#define TCPIP$C_SO_FAMILY 4108          /* 0x100c get socket address family */
#define TCPIP$C_SO_XSE 4109             /* 0x100d _XOPEN_SOURCE_EXTENDED socket */
#define TCPIP$C_SO_NO_RCV_CHKSUM 16384
#define TCPIP$C_SO_NO_SND_CHKSUM 32768
#define TCPIP$C_SO_NO_CHKSUM 49152

#define SOCKOPT$C_SO_DEBUG 1            /* turn on event logging, not used  */
#define SOCKOPT$C_ACCEPTCONN 2          /* socket has had LISTEN            */
#define SOCKOPT$C_REUSEADDR 4           /* allow local address reuse        */
#define SOCKOPT$C_KEEPALIVE 8           /* keep connection alive            */
#define SOCKOPT$C_DONTROUTE 16          /* use only the interface addr      */
#define SOCKOPT$C_BROADCAST 32          /* allow broadcasting               */
#define SOCKOPT$C_USELOOPBACK 64        /* loopback interface, not used     */
#define SOCKOPT$C_LINGER 128            /* linger at close                  */
#define SOCKOPT$C_OOBINLINE 256         /* leave received OOB data in line  */
#define SOCKOPT$C_REUSEPORT 512         /* allow local address and port reuse */
#define SOCKOPT$C_FULL_DUPLEX_CLOSE 8192 /* full duplex close               */
#define SOCKOPT$C_NO_RCV_CHKSUM 16384   /* no receive checksum calculation  */
#define SOCKOPT$C_NO_SND_CHKSUM 32768   /* no send checksum calculation     */

#define TCPIP$C_SO_DEBUG 1              /* turn on event logging, not used  */
#define TCPIP$C_ACCEPTCONN 2            /* socket has had LISTEN            */
#define TCPIP$C_REUSEADDR 4             /* allow local address reuse        */
#define TCPIP$C_KEEPALIVE 8             /* keep connection alive            */
#define TCPIP$C_DONTROUTE 16            /* use only the interface addr      */
#define TCPIP$C_BROADCAST 32            /* allow broadcasting               */
#define TCPIP$C_USELOOPBACK 64          /* loopback interface, not used     */
#define TCPIP$C_LINGER 128              /* linger at close                  */
#define TCPIP$C_OOBINLINE 256           /* leave received OOB data in line  */
#define TCPIP$C_REUSEPORT 512           /* allow local address and port reuse */
#define TCPIP$C_FULL_DUPLEX_CLOSE 8192  /* full duplex close                */
#define TCPIP$C_NO_RCV_CHKSUM 16384     /* no receive checksum calculation  */
#define TCPIP$C_NO_SND_CHKSUM 32768     /* no send checksum calculation     */

#define SOCKOPT$C_SNDBUF 4097           /* send buffer size                 */
#define SOCKOPT$C_RCVBUF 4098           /* receive buffer size              */
#define SOCKOPT$C_SNDLOWAT 4099         /* send low-water mark              */
#define SOCKOPT$C_RCVLOWAT 4100         /* receive low-water mark           */
#define SOCKOPT$C_SNDTIMEO 4101         /* send timeout                     */
#define SOCKOPT$C_RCVTIMEO 4102         /* receive timeout                  */
#define SOCKOPT$C_ERROR 4103            /* get error status and clear       */
#define SOCKOPT$C_TYPE 4104             /* get socket type                  */
#define SOCKOPT$C_SHARE 4105            /* shared between processes         */
#define SOCKOPT$C_CCL 4106              /* carriage control added           */
#define SOCKOPT$C_STATE 4107            /* get socket state bits            */
#define SOCKOPT$C_FAMILY 4108           /* get socket address family        */

#define TCPIP$C_SNDBUF 4097             /* send buffer size                 */
#define TCPIP$C_RCVBUF 4098             /* receive buffer size              */
#define TCPIP$C_SNDLOWAT 4099           /* send low-water mark              */
#define TCPIP$C_RCVLOWAT 4100           /* receive low-water mark           */
#define TCPIP$C_SNDTIMEO 4101           /* send timeout                     */
#define TCPIP$C_RCVTIMEO 4102           /* receive timeout                  */
#define TCPIP$C_ERROR 4103              /* get error status and clear       */
#define TCPIP$C_TYPE 4104               /* get socket type                  */
#define TCPIP$C_SHARE 4105              /* shared between processes         */
#define TCPIP$C_CCL 4106                /* carriage control added           */
#define TCPIP$C_STATE 4107              /* get socket state bits            */
#define TCPIP$C_FAMILY 4108             /* get socket address family        */


namespace {
    bool const UCX = IsntNil (getenv ("tcpip$network"));
    bool const SocketDriverIs64Bit = IsNil (getenv ("SocketDriverIs32Bit"));
    bool const UsingSocketOptionLimits = IsNil (getenv ("SocketDeviceInfoLimits"));
    size_t const SocketOptionLimitOverride = UCX ? V::GetEnvironmentInteger ("SocketOptionLimitOverride", 0) : 0;
}

//  true => do Socket IO$_SELECT first, false => go straight to I/O operation.
static bool GetDoingSelect () {
    return !UCX && IsntNil (getenv ("VcaDoingSelect"));
}
static bool const DoingSelect = GetDoingSelect ();

static char const *const g_pNullDeviceName = "/dev/null";

static pid_t const xThisProcess = getpid ();


#elif defined(_WIN32)
/*-----------------*
 *----  Win32  ----*
 *-----------------*/

#pragma comment (lib, "ws2_32.lib")

static char const *const g_pNullDeviceName = "NUL:";

#else
/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

#include "VkDynamicArrayOf.h"

#include <poll.h>

static char const *const g_pNullDeviceName = "/dev/null";

#endif


/******************
 *****  Self  *****
 ******************/

#include "Vca_VDeviceFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "V_VArgList.h"
#include "V_VManagedThread.h"

#include "Vca_VcaThreadState.h"
#include "Vca_VCohort.h"

#include "Vca_VBSProducer.h"
#include "Vca_VBSProducerBitBucket.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSConsumerDevice_.h"
#include "Vca_VBSProducerDevice_.h"
#include "Vca_VBSProducerConsumerDevice_.h"

#include "Vca_VConnection.h"

#include "Vca_VDeviceManager.h"
#include "Vca_VDeviceImplementation.h"

#include "Vca_VFDIODeviceImplementation.h"

#include "Vca_VListener.h"
#include "Vca_VListeningDevice_.h"

#include "Vca_VProcess.h"
#include "Vca_VStatus.h"

namespace {
    unsigned int s_cSocketReaders = 0;
    unsigned int s_cSocketWriters = 0;
    void ReportUser (char const* pWhat, unsigned int cUsers, int xStatus = 0) {
	Vca::VCohort::DefaultLogger().printf (xStatus ? "+++ On %s: %u %u\n" : "+++ On %s: %u\n", pWhat, cUsers, errno);
    }
}
/*************************************************/


/********************************************
 ********************************************
 *****                                  *****
 *****  Platform Independent Utilities  *****
 *****                                  *****
 ********************************************
 ********************************************/

static void doTrace (char const *pFormat, ...) {
    static FILE *const files[] = {stdout, stderr};
    static char const *const names[] = {"stdout", "stderr"};

    for (unsigned int xFile = 0; xFile < sizeof (files) / sizeof (files[0]); xFile++) {
	FILE *pFile = files [xFile];
	fprintf (pFile, " \n+++ %s: ", names[xFile]);

	V_VARGLIST (pArgs, pFormat);
	vfprintf (pFile, pFormat, pArgs);

	fflush (pFile);
    }
}

static void redirectToNull (int xFD, int xMode) {
    int xNullFD = open (g_pNullDeviceName, xMode, 0);
    if (xNullFD >= 0) {
	dup2 (xNullFD, xFD);
	close (xNullFD);
    }
}

static void redirectToNull (FILE *pFile, int xMode) {
    if (pFile)
	redirectToNull (fileno (pFile), xMode);
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VDatagramDevice_  *****
 *****                         *****
 ***********************************
 ***********************************/

namespace Vca {
    template <class Implementation> class VDatagramDevice_
	: public VDevice_<Implementation>
	, private VDevice::ListenFace
	, private VDevice::BSReadFace
	, private VDevice::BSWriteFace
    {
	DECLARE_CONCRETE_RTTLITE (VDatagramDevice_<Implementation>, VDevice_<Implementation>);
    public:
	typedef typename BaseClass::Referenceable Referenceable;

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;

	typedef VAccept_<Implementation> Accept;

	typedef typename BaseClass::ProducerTypes ProducerTypes;
	typedef typename BaseClass::ConsumerTypes ConsumerTypes;
        typedef typename BaseClass::WritePoll WritePoll;
        typedef typename BaseClass::ReadPoll ReadPoll;
	typedef VBSMove_<ProducerTypes> Get;
	typedef VBSMove_<ConsumerTypes> Put;

    //  Construction
    public:
	VDatagramDevice_(ImplementationData &rDeviceData)
	    : BaseClass (rDeviceData), VDevice::BSReadFace (this), VDevice::BSWriteFace (this), VDevice::ListenFace (this)
	{
	}

    //  Destruction
    private:
	~VDatagramDevice_() {
	}

    //  Face Implementation
    private:
	VDevice *device_() {
	    return this;
	}
	bool getName_(VkStatus &rStatus, VString &rName) {
	    return static_cast<BaseClass*>(this)->getName (rStatus, rName);
	}
	bool start_(
	    VkStatus &rStatus, VDeviceBSReader *pUser, VDeviceBSReadArea const &rArea
	) {
	    typename Get::Reference pUse (new Get (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
	bool start_(
	    VkStatus &rStatus, VDeviceBSWriter *pUser, VDeviceBSWriteArea const &rArea
	) {
	    typename Put::Reference pUse (new Put (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
        bool start_(
	    VkStatus &rStatus, VDeviceBSWriter *pUser
        ) {
	    VReference<WritePoll> pUse (new WritePoll (this));
	    return pUse->start (rStatus, pUser);
        }
        bool start_(
	    VkStatus &rStatus, VDeviceBSReader *pUser
        ) {
	    VReference<ReadPoll> pUse (new ReadPoll (this));
	    return pUse->start (rStatus, pUser);
        }
	bool start_(VkStatus &rStatus, VDevice::Listener *pUser) {
	    typename Accept::Reference pUse (new Accept (this));
	    return pUse->start (rStatus, pUser);
	}

    //  User Accounting
    private:
	void onFirstUser_(VDevice::BSReadFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstReader ();
	}
	void onFinalUser_(VDevice::BSReadFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalReader ();
	}

	void onFirstUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstWriter ();
	}
	void onFinalUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalWriter ();
	}

	void onFirstUser_(VDevice::ListenFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstListener ();
	}
	void onFinalUser_(VDevice::ListenFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalListener ();
	}

    //  User Creation
    private:
	bool supplyBSProducer_(VBSProducer::Reference &rpUser) {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
	bool supplyBSConsumer_(VBSConsumer::Reference &rpUser) {
	    return static_cast<VDevice::BSWriteFace*>(this)->supply (rpUser);
	}
	bool supplyConnection_(VConnection::Reference &rpUser) {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
	bool supplyListener_(VListener::Reference &rpUser) {
	    return static_cast<VDevice::ListenFace*>(this)->supply (rpUser);
	}

    //  The Good Stuff
    public:
	bool supplyConnectionTo (VkStatus &rStatus, VDevice::Reference &rpDevice, VkSocketAddress const &rAddress) {
	    return rStatus.MakeUnimplementedStatus ();
	}
    };
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::OS::ManagedDevice  *****
 *****                          *****
 ************************************
 ************************************/

namespace Vca {
    namespace OS {
	class DeviceManager;

	typedef VDeviceBSReadArea	BSReadArea;
	typedef VDeviceBSWriteArea	BSWriteArea;

        /**
         * Base class for devices that register themselves with device dispatchers. Generic for all operating systems.
         *
         * @see Vca::VDeviceManager
         */
	class ManagedDevice : public VDeviceImplementation {
	    DECLARE_FAMILY_MEMBERS (ManagedDevice, VDeviceImplementation);

	//  Manager
	public:
	    typedef DeviceManager Manager;

	//  ManagedUse
	public:
	    typedef BaseClass::AbstractUse AbstractUse;
            /**
             * Base class for managed device Use classes. Generic for all operating systems.
             */
	    class ManagedUse : public AbstractUse {
		DECLARE_FAMILY_MEMBERS (ManagedUse, AbstractUse);

	    //  Construction
	    protected:
		ManagedUse (VReferenceable *pContainer) : BaseClass (pContainer) {
		}

	    //  Destruction
	    protected:
		~ManagedUse () {
		}

	    //  Access
	    private:
		virtual ManagedDevice *device_() const = 0;

	    //  Completion
	    protected:
		void decrementUseCount () {
		    device_()->decrementUseCount ();
		}
	    protected:
		bool onData (size_t sTransfer, bool bKeep = false) {
		    if (!bKeep)
			decrementUseCount ();
		    return BaseClass::onData (sTransfer, bKeep);
		}
		bool onData (VDevice *pDevice, bool bKeep = false) {
		    if (!bKeep)
			decrementUseCount ();
		    return BaseClass::onData (pDevice, bKeep);
		}
		bool onError (VkStatus const &rStatus, bool bKeep = false) {
		    if (!bKeep)
			decrementUseCount ();
		    return BaseClass::onError (rStatus, bKeep);
		}
	    };
	    friend class ManagedUse;

	//  Construction
	protected:
	    ManagedDevice (VReferenceable *pReferenceable, Manager *pManager);
	    ManagedDevice (VReferenceable *pReferenceable, ManagedDevice const &rOther);

	//  Destruction
	protected:
	    ~ManagedDevice ();

	//  Access
	private:
	    virtual VCohort *cohort_() const {
		return cohort ();
	    }
	public:
	    VCohort *cohort () const;
	    Manager *manager () const {
		return m_pManager;
	    }
	    unsigned int useCount () const;

	//  Device Use
	protected:
	    void onUseCountWasZero_();
	    void onUseCountIsZero_();

	//  State
	protected:
	    VReference<Manager> const m_pManager;
	};
    }
}


#if defined(__VMS)

/*****************************
 *****************************
 *****                   *****
 *****  Vca::OS::Device  *****
 *****                   *****
 *****************************
 *****************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	typedef unsigned short Handle;

	static bool assign (
	    VkStatus &rStatus, Handle &rhDevice, dsc$descriptor_s const &rDeviceName
	);
	static bool assign (VkStatus &rStatus, Handle &rhSocket);

	/*>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<*
	 *>>>>  template<typename Stuff> class Area32_  <<<<*
	 *>>>>     (32-bit stuff in a 64-bit world)     <<<<*
	 *>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<*/

	template <typename Stuff> class Area32_ : public virtual VTransient, public Stuff {
	//  Aliases
	public:
	    typedef Area32_<Stuff> Container;

	    typedef void *new_delete_void_ptr_t;

#pragma __pointer_size __save
#pragma __pointer_size __short
	    typedef Stuff *StuffAddress;
	    typedef Container *ContainerAddress;
#pragma __pointer_size __restore

	//  Memory Management
	public:
	    new_delete_void_ptr_t operator new (size_t sObject) {
		return _malloc32 (sObject);
	    }
	    void operator delete (new_delete_void_ptr_t pObject, size_t sObject) {
		decc$free (pObject);
	    }

	//  Pointer
	public:
	    class Pointer {
	    //  Construction
	    public:
		Pointer () : m_pContainer (0) {
		}

	    //  Destruction
	    public:
		~Pointer () {
		    if (m_pContainer)
			delete m_pContainer;
		}

	    //  Access
	    public:
		StuffAddress stuff () const {
		    return m_pContainer;
		}
		StuffAddress stuff () {
		    if (!m_pContainer)
			m_pContainer = static_cast<ContainerAddress>(new Container ());
		    return m_pContainer;
		}
		operator StuffAddress () const {
		    return stuff ();
		}
		operator StuffAddress () {
		    return stuff ();
		}
		StuffAddress operator-> () const {
		    return stuff ();
		}
		StuffAddress operator-> () {
		    return stuff ();
		}

	    //  Query
	    public:
		bool isNil () const {
		    return m_pContainer ? false : true;
		}
		bool isntNil () const {
		    return m_pContainer ? true : false;
		}

	    //  State
	    private:
		ContainerAddress m_pContainer;
	    };
	    friend class Pointer;

	//  Construction
	private:
	    Area32_() {
	    }

	//  Destruction
	private:
	    ~Area32_() {
	    }
	};


	/*>>>>>>>><<<<<<<<<*
	 *>>>>  XIOSB  <<<<*
	 *>>>>>>>><<<<<<<<<*/

	class XIOSB : public _iosb {
	    DECLARE_FAMILY_MEMBERS (XIOSB, _iosb);

	//  Construction
	public:
	    XIOSB (XIOSB const &rOther) {
		setTo (rOther);
	    }
	    XIOSB (_iosb const &rOther) {
		setTo (rOther);
	    }
	    XIOSB () {
		iosb$w_status = SS$_NORMAL;
	    }

	//  Destruction
	public:
	    ~XIOSB () {
	    }

	//  Access
	public:
	    unsigned int status () const {
		return iosb$w_status;
	    }
	    bool statusIsSuccess () const {
		return $VMS_STATUS_SUCCESS (iosb$w_status);
	    }

	    void supply (XIOSB &rIOSB) const {
		rIOSB = *this;
	    }
	    unsigned short terminator () const {
		return static_cast<unsigned short>(iosb$l_dev_depend);
	    }
	    unsigned short terminatorSize () const {
		return iosb$w_dev_depend_high;
	    }

	//  Update
	public:
	    ThisClass &operator= (XIOSB const &rOther) {
		setTo (rOther);
		return *this;
	    }
	    ThisClass &operator= (_iosb const &rOther) {
		setTo (rOther);
		return *this;
	    }
	    void setTo (_iosb const &rOther) {
		memcpy (static_cast<_iosb*>(this), &rOther, sizeof (rOther));
	    }
	};


	/*>>>>>>>>>><<<<<<<<<<*
	 *>>>>  XBuffer_  <<<<*
	 *>>>>>>>>>><<<<<<<<<<*/

	template <unsigned int Size> class XBuffer_ : public XIOSB {
	    DECLARE_FAMILY_MEMBERS (XBuffer_<Size>, XIOSB);

	//  Construction
	public:
	    XBuffer_() {
	    }

	//  Destruction
	public:
	    ~XBuffer_() {
	    }

	//  Access
	public:
	    pointer_const_t storage () const {
		return m_iStorage;
	    }
	    static unsigned int storageSize () {
		return Size;
	    }
	    using BaseClass::supply;

	//  Update
	public:
	    void supply (XIOSB *&rpIOSB, pointer_t &rpArea, size_t &rsArea, BSReadArea const &rSource) {
		rpIOSB = this;
		rpArea = m_iStorage;
		rsArea = rSource.size ();
		if (rsArea > Size)
		    rsArea = Size;
	    }
	    void supply (XIOSB *&rpIOSB, pointer_const_t &rpArea, size_t &rsArea, BSWriteArea const &rSource) {
		rpIOSB = this;
		rpArea = m_iStorage;
		rsArea = rSource.size ();
		if (rsArea > Size)
		    rsArea = Size;
		memcpy (m_iStorage, rSource.base (), rsArea);
	    }
	    void supply (BSReadArea const &rArea) const {
		memcpy (rArea.base (), m_iStorage, iosb$w_bcnt);
	    }

	//  State
	private:
	    char m_iStorage[Size];
	};


	/*>>>>>>>>><<<<<<<<<*
	 *>>>>  Device  <<<<*
	 *>>>>>>>>><<<<<<<<<*/

	class Device : public ManagedDevice {
	    DECLARE_FAMILY_MEMBERS (Device, ManagedDevice);

	//  Data
	public:
	    typedef ThisClass Data;

	//  Device Information Keys
	public:
	    enum {
		Device_Class,
		Device_BufferSize,
//		Device_Char2,
//		Device_Depend,
//		Device_Depend2,

		DEVICE_INFO_COUNT
	    };
	    typedef unsigned int DeviceClass;
	    typedef unsigned int DeviceInfoItem;
	    typedef DeviceInfoItem DeviceInfo[DEVICE_INFO_COUNT];

	//  Device Reference
	public:
	    typedef VReference<ThisClass> DeviceReference;

	//  Invalid Class/Invalid Handle
	public:
	    enum {
		InvalidClass = UINT_MAX
	    };
	    enum {
		InvalidHandle = 0
	    };

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	//  Use
	public:
	    class Use : public ManagedUse {
		DECLARE_FAMILY_MEMBERS (Use, ManagedUse);

	    //  Friends
		friend class Device;
		friend class DeviceManager;

	    //  Reference
	    public:
		typedef VReference<ThisClass> Reference;

	    //  Construction
	    protected:
		Use (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer), m_pDevice (pDevice) {
		}

	    //  Destruction
	    protected:
		~Use () {
		}

	    //  Access
	    private:
		ManagedDevice *device_() const {
		    return device ();
		}
	    public:
		Device *device () const {
		    return m_pDevice;
		}
		bool deviceDriverIs32Bit () const {
		    return m_pDevice->deviceDriverIs32Bit ();
		}
		bool deviceDriverIs64Bit () const {
		    return m_pDevice->deviceDriverIs64Bit ();
		}
		Manager *manager () const {
		    return m_pDevice->manager ();
		}

	    //  Completion
	    protected:
		bool onError (VkStatus const &rStatus, bool bKeep = false) {
		    m_pDevice->onErrorDetect (this, rStatus);
		    return BaseClass::onError (rStatus, bKeep);
		}
		bool onError (unsigned int xStatus, bool bKeep = false);

	    //  Event Processing
	    protected:
		//  AST level event processing:
		static void OnAST (ThisClass *pUse);
	    private:
		void onAST () {
		    m_pDevice->onAST (this);
		}

		//  Non-AST level event processing:
		virtual void onEventUpdateIOSB_() {
		}
		virtual bool onEvent_() = 0;
		void onEvent ();

	    //  Resource Display
	    protected:
		void displayQuotas (char const *pWhere) {
		    m_pDevice->displayQuotas (pWhere);
		}

	    //  State
	    protected:
		DeviceReference const	m_pDevice;
		Pointer			m_pQueueSuccessor;
		XIOSB			m_iIOSB;
	    };
	    friend class Use;

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	//  BSMove
	public:
	    class BSMove : public Use {
		DECLARE_FAMILY_MEMBERS (BSMove, Use);

		friend class Device;

	    //  Construction
	    protected:
		BSMove (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSMove () {
		}

	    //  Access
	    public:
		XIOSB *iosb (bool bIn32BitMemory) {
		    return bIn32BitMemory ? m_pStuff32 : &m_iIOSB;
		}

	    //  Event Processing
	    private:
		virtual void onEventUpdateIOSB_() {
		    if (m_pStuff32.isntNil ())
			m_pStuff32->supply (m_iIOSB);
		}

	    //  State
	    protected:
		Area32_<XBuffer_<16536> >::Pointer m_pStuff32;
	    };
	    friend class BSMove;

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	//  BSGet
	public:
	    class BSGet : public BSMove {
		DECLARE_FAMILY_MEMBERS (BSGet, BSMove);

		friend class Device;

	    //  Area
	    public:
		typedef BSReadArea Area;

	    //  Construction
	    protected:
		BSGet (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSGet () {
		}

	    //  Access
	    public:
		virtual void supply (
		    XIOSB *&rpIOSB, pointer_t &rpArea, size_t &rsArea, U64 &riP3, U64 &riP4, U64 &riP5, bool bIn32BitMemory
		) {
		    riP3 = riP4 = riP5 = 0;
		    if (bIn32BitMemory) {
			m_pStuff32->supply (rpIOSB, rpArea, rsArea, m_iArea);
			return;
		    }
		    rpIOSB = &m_iIOSB;
		    rpArea = static_cast<pointer_t>(m_iArea.base ());
		    rsArea = m_iArea.size ();
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, Area const &rArea) {
		    m_iArea.setTo (rArea);
		    return doTransfer (rStatus);
		}
	    protected:
		bool doTransfer (VkStatus &rStatus) {
		    return m_pDevice->doRead (rStatus, this) || onError (rStatus);
		}

	    //  Event Processing
	    private:
		void onEventUpdateArea () {
		    if (m_pStuff32.isntNil ())
			m_pStuff32->supply (m_iArea);
		}
	    protected:
		bool onEvent_();

	    //  State
	    protected:
		Area m_iArea;
	    };
	    friend class BSGet;

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	//  BSPut
	public:
	    class BSPut : public BSMove {
		DECLARE_FAMILY_MEMBERS (BSPut, BSMove);

		friend class Device;

	    //  Area
	    public:
		typedef BSWriteArea Area;

	    //  Construction
	    protected:
		BSPut (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice), m_sCarriageControlPrefix (0) {
		}

	    //  Destruction
	    protected:
		~BSPut () {
		}

	    //  Access
	    public:
		virtual void supply (
		    XIOSB *&rpIOSB, pointer_const_t &rpArea, size_t &rsArea, U64 &riP3, U64 &riP4, U64 &riP5, bool bIn32BitMemory
		) {
		    riP3 = riP4 = riP5 = 0;
		    if (bIn32BitMemory) {
			m_pStuff32->supply (rpIOSB, rpArea, rsArea, m_iArea);
			return;
		    }
		    rpIOSB = &m_iIOSB;
		    rpArea = static_cast<pointer_const_t>(m_iArea.base ());
		    rsArea = m_iArea.size ();
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, BSWriteArea const &rArea) {
		    m_iArea.setTo (rArea);
		    return doTransfer (rStatus);
		}
	    protected:
		bool doTransfer (VkStatus &rStatus) {
		    return m_pDevice->doWrite (rStatus, this) || onError (rStatus);
		}

	    //  Event Processing
	    private:
		void onEventUpdateArea () {
		}
	    protected:
		bool onEvent_();

	    //  Control
	    private:
		void setCarriageControlPrefix (size_t sCarriageControlPrefix) {
		    m_sCarriageControlPrefix = sCarriageControlPrefix;
		}

	    //  State
	    protected:
		Area m_iArea;
		size_t m_sCarriageControlPrefix;
	    };
	    friend class BSPut;

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	public:
	    class Limits {
		DECLARE_FAMILY_MEMBERS (Limits, void);

	    //  Construction
	    public:
		Limits (ThisClass const &rOther) {
		    setTo (rOther);
		}
		Limits () {
		    reset ();
		}

	    //  Destruction
	    public:
		~Limits () {
		}

	    //  Access
	    public:
		size_t getGetSizeLimit () const {
		    return m_sGetLimit;
		}
		size_t getPutSizeLimit () const {
		    return m_sPutLimit;
		}

	    //  Update
	    public:
		void claim (ThisClass &rOther) {
		    setTo (rOther);
		    rOther.reset ();
		}

		void reset () {
		    m_sGetLimit = m_sPutLimit = UINT_MAX;
		}

		void setTo (ThisClass const &rOther) {
		    m_sGetLimit = rOther.m_sGetLimit;
		    m_sPutLimit = rOther.m_sPutLimit;
		}
		void setGetLimitTo (size_t sLimit) {
		    m_sGetLimit = sLimit;
		}
		void setPutLimitTo (size_t sLimit) {
		    m_sPutLimit = sLimit;
		}
		void setTransferLimitTo (size_t sLimit) {
		    m_sGetLimit = m_sPutLimit = sLimit;
		}

		ThisClass& operator= (ThisClass const &rOther) {
		    setTo (rOther);
		    return *this;
		}

	    //  Use
	    public:
		size_t adjustedGetSize (size_t sTransfer) const {
		    return sTransfer < m_sGetLimit ? sTransfer : m_sGetLimit;
		}
		size_t adjustedPutSize (size_t sTransfer) const {
		    return sTransfer < m_sPutLimit ? sTransfer : m_sPutLimit;
		}

	    //  State
	    private:
		size_t m_sGetLimit;
		size_t m_sPutLimit;
	    };

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	//  Construction Utilities
	private:
	    void claimInfo (ThisClass &rOther) {
		m_iDeviceLimits.claim (rOther.m_iDeviceLimits);
		memcpy (m_iDeviceInfo, rOther.m_iDeviceInfo, sizeof (m_iDeviceInfo));
		rOther.clearInfo ();
	    }
	    void clearInfo () {
		memset (m_iDeviceInfo, 0, sizeof (m_iDeviceInfo));
	    }

	//  Construction
	public:
	    Device (
		VReferenceable *pContainer, Manager *pManager, Handle &rhDevice
	    ) : BaseClass (pContainer, pManager), m_hDevice (grab (rhDevice)) {
		clearInfo ();
		classifyDevice ();
	    }
	    Device (
		VReferenceable *pContainer, Manager *pManager
	    ) : BaseClass (pContainer, pManager), m_hDevice (InvalidHandle) {
		clearInfo ();
	    }
	    Device (
		VReferenceable *pContainer, Device &rOther
	    ) : BaseClass (pContainer, rOther), m_hDevice (rOther.grab ()) {
		claimInfo (rOther);
	    }

	//  Destruction
	public:
	    ~Device () {
		close ();
	    }

	//  Access
	public:
	    operator Handle () const {
		return m_hDevice;
	    }
	    DeviceClass deviceClass () const {
		return m_iDeviceInfo[Device_Class];
	    }
	    size_t deviceBufferSize () const {
		return m_iDeviceInfo[Device_BufferSize];
	    }
	    bool deviceDriverIs32Bit () const {
		return !deviceDriverIs64Bit ();
	    }
	    virtual bool deviceDriverIs64Bit () const {
		return false;
	    }
	    Limits const &deviceLimits () const {
		return m_iDeviceLimits;
	    }
	    Handle grab () {
		return grab (m_hDevice);
	    }
	    static Handle grab (Handle &rhDevice) {
		Handle hDevice = rhDevice;
		rhDevice = InvalidHandle;
		return hDevice;
	    }

	//  Update
	public:
	    void claim (Device &rOther) {
		close ();
		m_hDevice = rOther.grab ();
		claimInfo (rOther);
	    }
	    void claim (Handle &rhDevice) {
		close ();
		m_hDevice = grab (rhDevice);
		classifyDevice ();
	    }
	    void close () {
		close (m_hDevice);
	    }
	    static void close (Handle &rhDevice) {
		if (rhDevice != InvalidHandle) {
		    sys$dassgn (grab (rhDevice));
		}
	    }

	//  Device Classification
	private:
	    void classifyDevice ();
	public:
	    bool deviceClassUnderstood () const;

	//  Device Control
	public:
	    bool makeDeviceFriendly () const {
		return makePrivate ();
	    }
	    bool makePublic () const {
		return false;
	    }
	    bool makePrivate () const {
		return true;
	    }

	//  Device Limits
	protected:
	    void setDeviceLimitsTo (Limits const &rLimits) {
		m_iDeviceLimits.setTo (rLimits);
// 		printf (
// 		    "+++ %5u: Device : %u %u\n", m_hDevice, m_iDeviceLimits.getGetSizeLimit (), m_iDeviceLimits.getPutSizeLimit ()
// 		);
	    }

	//  QIO Helpers
	private:
	    size_t qioSize (size_t sArea) const;

	//  Device Use
	protected:
	    bool doRead  (VkStatus &rStatus, BSGet *pUse);
	    bool doWrite (VkStatus &rStatus, BSPut *pUse);

	    bool onEvent (
		VkStatus &rStatus, size_t &rsTransfer, XIOSB const &rIOSB, BSGet::Area const &rArea
	    ) const;
	    bool onEvent (
		VkStatus &rStatus, size_t &rsTransfer, XIOSB const &rIOSB, BSPut::Area const &rArea
	    ) const;

	    bool makeBlockedStatus (VkStatus &rStatus, unsigned int xStatus, XIOSB const *pIOSB, char const *pWhat) const;

	//  Event Processing
	private:
	    //  AST level event processing:
	    void onAST (Use *pUse);

	//  Resource Display
	public:
	    void displayQuotas (char const *pWhere);

	//  State
	private:
	    Handle	m_hDevice;
	    DeviceInfo	m_iDeviceInfo;
	    Limits	m_iDeviceLimits;
	};
	typedef Device::Use DeviceUse;
    }
}
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::Device>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::Device>::ProducerTypes>;

template class Vca::VBSConsumerDevice_<Vca::OS::Device>;
template class Vca::VBSProducerDevice_<Vca::OS::Device>;
template class Vca::VBSProducerConsumerDevice_<Vca::OS::Device>;

template class Vca::VDevice_<Vca::OS::Device>;


/********************************
 ********************************
 *****  Channel Assignment  *****
 ********************************
 ********************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::assign (
    VkStatus &rStatus, Handle &rhDevice, dsc$descriptor_s const &rDeviceName
) {
    return rStatus.MakeSuccessStatus (sys$assign ((void*)&rDeviceName, &rhDevice, 0, 0, 0));
}

bool Vca::OS::assign (VkStatus &rStatus, Handle &rhDevice) {
    static char aUCXDeviceName[] = "TCPIP$DEVICE";
    static char aMultinetDeviceName[] = "INET0:";
    static $DESCRIPTOR (iUCXDeviceName, aUCXDeviceName);		// ... UCX Device Name
    static $DESCRIPTOR (iMultinetDeviceName, aMultinetDeviceName);	// ... Multinet Device Name
    return assign (rStatus, rhDevice, UCX ? iUCXDeviceName : iMultinetDeviceName);
}


/***********************************
 ***********************************
 *****  Device Classification  *****
 ***********************************
 ***********************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

void Vca::OS::Device::classifyDevice () {
    static unsigned short xDVIItem[DEVICE_INFO_COUNT];
    if (!xDVIItem[0]) {
	xDVIItem[Device_Class		] = DVI$_DEVCLASS;
	xDVIItem[Device_BufferSize	] = DVI$_DEVBUFSIZ;
    }

    DeviceInfo iDeviceInfo;
    _ile3 iItemList[DEVICE_INFO_COUNT + 1];
    memset (iItemList, 0, sizeof (iItemList));

#pragma __pointer_size __save
#pragma __pointer_size __short
    unsigned short sItem[DEVICE_INFO_COUNT];
    unsigned int xItem = 0;
    while (xItem < DEVICE_INFO_COUNT) {
	iItemList[xItem].ile3$w_length = sizeof (iDeviceInfo[xItem]);
	iItemList[xItem].ile3$w_code = xDVIItem[xItem];
	iItemList[xItem].ile3$ps_bufaddr = static_cast<DeviceInfoItem*>(&iDeviceInfo[xItem]);
	iItemList[xItem].ile3$ps_retlen_addr = static_cast<unsigned short*>(&sItem[xItem]);

	sItem[xItem++] = 0;
    }

    XIOSB iIOSB;
    bool bGood = $VMS_STATUS_SUCCESS (
	sys$getdvi (
	    EFN$C_ENF, m_hDevice, NULL, static_cast<_ile3*>(iItemList), cast_32_truncate(&iIOSB), (astptr32_t)&tis_io_complete, 0, NULL
	)
    );
#pragma __pointer_size __restore

    if (bGood && $VMS_STATUS_SUCCESS (tis_synch (EFN$C_ENF, &iIOSB)) && iIOSB.statusIsSuccess ()) {
	memcpy (m_iDeviceInfo, iDeviceInfo, sizeof (DeviceInfo));

	switch (deviceClass ()) {
	case DC$_TERM:
	    m_iDeviceLimits.setTransferLimitTo (32718);
	    break;
	default:
	    m_iDeviceLimits.setTransferLimitTo (deviceBufferSize ());
	    break;
	}
    }
}

bool Vca::OS::Device::deviceClassUnderstood () const {
    switch (deviceClass ()) {
    case DC$_TERM:
    case DC$_MAILBOX:
	return true;
    default:
	return false;
    }
}


/*************************
 *************************
 *****  QIO Helpers  *****
 *************************
 *************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

size_t Vca::OS::Device::qioSize (size_t sArea) const {
    size_t sLimit = UINT_MAX;
    switch (deviceClass ()) {
    default:
	sLimit = deviceBufferSize ();
	break;
    case DC$_TERM:
	sLimit = 32718;
	break;
    }
    return sArea < sLimit ? sArea : sLimit;
}

/******************************
 ******************************
 *****  Resource Display  *****
 ******************************
 ******************************/

void Vca::OS::Device::displayQuotas (char const *pWhere) {
    unsigned int astcnt, astlm, biocnt, biolm, diocnt, diolm;

    unsigned __int64 cbReturned;
    _ileb_64 iItemList[] = {
	{ 1, JPI$_ASTCNT, -1, sizeof(astcnt), &astcnt, &cbReturned},
	{ 1, JPI$_ASTLM , -1, sizeof(astlm ), &astlm , &cbReturned},
	{ 1, JPI$_BIOCNT, -1, sizeof(biocnt), &biocnt, &cbReturned},
	{ 1, JPI$_BIOLM , -1, sizeof(biolm ), &biolm , &cbReturned},
	{ 1, JPI$_DIOCNT, -1, sizeof(diocnt), &diocnt, &cbReturned},
	{ 1, JPI$_DIOLM , -1, sizeof(diolm ), &diolm , &cbReturned},
	{ 0 }
    };
    _iosb iIOSB;
    iIOSB.iosb$w_status = SS$_NORMAL;
    if ($VMS_STATUS_SUCCESS (sys$getjpi(EFN$C_ENF, 0, 0, iItemList, &iIOSB, (astptr_t)&tis_io_complete, 0)) &&
	$VMS_STATUS_SUCCESS (tis_synch (EFN$C_ENF, &iIOSB)) &&
	$VMS_STATUS_SUCCESS (iIOSB.iosb$w_status)
    ) fprintf (
	stderr, "+++ %s: U:%4u A:%4u %4u, B:%4u %4u, D:%4u %4u\n", pWhere, useCount (), astcnt, astlm, biocnt, biolm, diocnt, diolm
    );
}


/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::Device::doRead (VkStatus &rStatus, BSGet *pUse) {
    incrementUseCount ();

    XIOSB *pIOSB; pointer_t pArea; size_t sArea; unsigned __int64 iP3, iP4, iP5;
    pUse->supply (pIOSB, pArea, sArea, iP3, iP4, iP5, deviceDriverIs32Bit ());
    sArea = m_iDeviceLimits.adjustedGetSize (sArea);

    unsigned int xQIOP = IO$_READVBLK;
    switch (deviceClass ()) {
    case DC$_MAILBOX:
	xQIOP |= IO$M_WRITERCHECK | IO$M_STREAM;
	break;
    default:
	break;
    }

//    return rStatus.MakeBlockedStatus (
    return makeBlockedStatus (
	rStatus, sys$qio (
	    EFN$C_ENF, m_hDevice, xQIOP, pIOSB,
	    (astptr_t)&Use::OnAST, reinterpret_cast<__int64>(pUse),
	    pArea, sArea, iP3, iP4, iP5, 0
	), pIOSB, "read"
    );
}

bool Vca::OS::Device::doWrite (VkStatus &rStatus, BSPut *pUse) {
    incrementUseCount ();

    XIOSB *pIOSB; pointer_const_t pArea; size_t sArea; unsigned __int64 iP3, iP4, iP5;
    pUse->supply (pIOSB, pArea, sArea, iP3, iP4, iP5, deviceDriverIs32Bit ());
    sArea = m_iDeviceLimits.adjustedPutSize (sArea);

    unsigned int xQIOP = IO$_WRITEVBLK;
    switch (deviceClass ()) {
    case DC$_MAILBOX:
	xQIOP |= IO$M_READERCHECK;
	break;
    case DC$_TERM:
	xQIOP |= IO$M_BREAKTHRU;
	if (sArea > 0) {
	    //  count the number of leading newlines ('\n')...
	    unsigned int sCarriageControlPrefix = 0;
	    while (sArea > 0 && '\n' == pArea[0] && sCarriageControlPrefix < 0x7f) {
		sCarriageControlPrefix++;
		pArea++;
		sArea--;
	    }
	    pUse->setCarriageControlPrefix (sCarriageControlPrefix);
	    iP4 = sCarriageControlPrefix << 16;

	    //  ... find the boundary of the next CarriageControl, ...
	    char const *pNL = static_cast<char*>(memchr (pArea, '\n', sArea));
	    if (pNL)
		sArea = pNL - pArea;
	}
	break;
    default:
	break;
    }

//    return rStatus.MakeBlockedStatus (
    return makeBlockedStatus (
	rStatus, sys$qio (
	    EFN$C_ENF, m_hDevice, xQIOP, pIOSB,
	    (astptr_t)&Use::OnAST, reinterpret_cast<__int64>(pUse),
	    const_cast <pointer_t>(pArea), sArea, iP3, iP4, iP5, 0
	), pIOSB, "write"
    );
}


/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::Device::onEvent (
    VkStatus &rStatus, size_t &rsTransfer, XIOSB const &rIOSB, BSGet::Area const &rArea
) const {
    rsTransfer = rIOSB.iosb$w_bcnt;

    switch (deviceClass ()) {
    case DC$_TERM:
	if (rIOSB.terminatorSize () > 0) {
	    switch (rIOSB.terminator ()) {
	    default:
		break;
	    case '\r':
		static_cast<char*>(rArea.base ())[rsTransfer] = '\n';
		break;
	    case 26:	// ^z
		rStatus.MakeClosedStatus ();
		return false;
	    }
	    rsTransfer += rIOSB.terminatorSize ();
	}
	break;
    default:
	if (0 == rsTransfer) {
	    rStatus.MakeClosedStatus ();
	    return false;
	}
	break;
    }
    return true;
}

bool Vca::OS::Device::onEvent (
    VkStatus &rStatus, size_t &rsTransfer, XIOSB const &rIOSB, BSPut::Area const &rArea
) const {
    rsTransfer = rIOSB.iosb$w_bcnt;
    if (0 == rsTransfer) {
	VCohort::DefaultLogger().printf ("+++ Vca::VDeviceImplementation[%llp]::onEvent BSPut 0.\n", this);
    }
    return true;
}

bool Vca::OS::Device::makeBlockedStatus (VkStatus &rStatus, unsigned int xStatus, XIOSB const *pIOSB, char const *pWhat) const {
    if (pIOSB->status () > 1) VCohort::DefaultLogger().printf (
	"+++ Vca::VDeviceImplementation[%llp]::makeBlockedStatus: %08x %04x %04x%s\n", this, xStatus, pIOSB->status (), pIOSB->iosb$w_bcnt, pWhat
    );
    
    return rStatus.MakeBlockedStatus (xStatus);
}


/**********************************
 **********************************
 *****                        *****
 *****  Vca::OS::Device::Use  *****
 *****                        *****
 **********************************
 **********************************/

/************************
 ************************
 *****  Completion  *****
 ************************
 ************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::Device::Use::onError (unsigned int xStatus, bool bKeep) {
    VkStatus iStatus;
    iStatus.MakeErrorStatus (xStatus);
    return onError (iStatus, bKeep);
}

/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

//  WARNING: This routine is run at AST level:
void Vca::OS::Device::Use::OnAST (Use *pUse) {
    pUse->onAST ();
}

void Vca::OS::Device::Use::onEvent () {
    onEventUpdateIOSB_();

    unsigned int xStatus = sys$synch (EFN$C_ENF, &m_iIOSB);
    if ($VMS_STATUS_SUCCESS (xStatus))
	xStatus = m_iIOSB.iosb$w_status;

    if ($VMS_STATUS_SUCCESS (xStatus) ? onEvent_() : onError (xStatus))
	triggerUser ();
}

bool Vca::OS::Device::BSGet::onEvent_() {
    onEventUpdateArea ();

    VkStatus iStatus; size_t sTransfer;
    return m_pDevice->onEvent (
	iStatus, sTransfer, m_iIOSB, m_iArea
    ) ? onData (sTransfer) : onError (iStatus);
}

bool Vca::OS::Device::BSPut::onEvent_() {
    onEventUpdateArea ();

    VkStatus iStatus; size_t sTransfer;
    if (!m_pDevice->onEvent (iStatus, sTransfer, m_iIOSB, m_iArea))
	return onError (iStatus);
    if (m_sCarriageControlPrefix > 0 && sTransfer > 0)
	sTransfer--;
    return onData (sTransfer);
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::OS::MailboxDevice  *****
 *****                          *****
 ************************************
 ************************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	class MailboxDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (MailboxDevice, Device);

	//  Aliases
	public:
	    typedef Pointer DevicePointer;

	//  Data
	public:
	    typedef ThisClass Data;

	//  Status
	public:
	    class Status : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Status, VReferenceable);

		friend class MailboxDevice;

	    //  Construction
	    public:
		Status (MailboxDevice *pMailbox)
		    : m_pMailbox (pMailbox) 
		    , m_bSubprocessStdin (false)
		    , m_bSubprocessStdout (false)
		    , m_bSubprocessStderr (false)
		    , m_bNoReader (false)
		    , m_bNoWriter (false)
		{
		}

	    //  Destruction
	    private:
		~Status () {
		}

	    //  Access
	    public:
		bool hasNoReader () const {
		    return m_bNoReader;
		}
		bool hasNoWriter  () const {
		    return m_bNoWriter;
		}
		bool hasReader () const {
		    return !m_bNoReader;
		}
		bool hasWriter () const {
		    return !m_bNoWriter;
		}

		bool isSubprocessStdin () const {
		    return m_bSubprocessStdin;
		}
		bool isSubprocessStdout () const {
		    return m_bSubprocessStdout;
		}
		bool isSubprocessStderr () const {
		    return m_bSubprocessStderr;
		}

	    //  Update
	    public:
		void markAsStdinOf (VProcess *pProcess, char const *pCommand);
		void markAsStdoutOf (VProcess *pProcess) {
		    if (m_pProcess.setIfNil (pProcess) || m_pProcess.referent () == pProcess) {
			m_bSubprocessStdout = true;
		    }
		}
		void markAsStderrOf (VProcess *pProcess) {
		    if (m_pProcess.setIfNil (pProcess) || m_pProcess.referent () == pProcess) {
			m_bSubprocessStderr = true;
		    }
		}

		void onError (VkStatus const &rStatus);

		void onFinalReader ();
		void onFinalWriter ();

	    //  State
	    private:
		DevicePointer		m_pMailbox;	//  possibly used to send initial command to a sys$creprc created processes. 
		VProcess::Reference	m_pProcess;
		bool m_bSubprocessStdin;
		bool m_bSubprocessStdout;
		bool m_bSubprocessStderr;
		bool m_bNoReader;
		bool m_bNoWriter;
	    };
	    friend class Status;

	//  Construction
	public:
	    MailboxDevice (VReferenceable *pContainer, Manager *pManager, Handle &rhDevice);
	    MailboxDevice (VReferenceable *pContainer, Manager *pManager);
	    MailboxDevice (VReferenceable *pContainer, ThisClass &rOther);


	//  Destruction
	public:
	    ~MailboxDevice ();

	//  Access
	public:
	    bool deviceDriverIs64Bit () const {
		return true;
	    }

	//  Device Use
	private:
	    void launch (char const *pCommand);

	//  Event Processing
	protected:
	    void onErrorDetect_(AbstractUse *pUse, VkStatus const &rStatus);

	//  Status Access
	public:
	    Status *status () const {
		return m_pStatus;
	    }
	    Status *status () {
		if (m_pStatus.isNil ())
		    m_pStatus.setTo (new Status (this));
		return m_pStatus;
	    }

	public:
	    bool hasNoReader () const {
		return m_pStatus && m_pStatus->hasNoReader ();
	    }
	    bool hasNoWriter  () const {
		return m_pStatus && m_pStatus->hasNoWriter ();
	    }
	    bool hasReader () const {
		return m_pStatus.isNil () || m_pStatus->hasReader ();
	    }
	    bool hasWriter () const {
		return m_pStatus.isNil () || m_pStatus->hasWriter ();
	    }

	    bool isSubprocessStdin () const {
		return m_pStatus && m_pStatus->isSubprocessStdin ();
	    }
	    bool isSubprocessStdout () const {
		return m_pStatus && m_pStatus->isSubprocessStdout ();
	    }
	    bool isSubprocessStderr () const {
		return m_pStatus && m_pStatus->isSubprocessStderr ();
	    }

	//  User Accounting
	public:
	    void onFinalReader ();
	    void onFinalWriter ();

	//  State
	private:
	    Status::Reference m_pStatus;
	};
    }
}
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::MailboxDevice>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::MailboxDevice>::ProducerTypes>;

template class Vca::VBSConsumerDevice_<Vca::OS::MailboxDevice>;
template class Vca::VBSProducerDevice_<Vca::OS::MailboxDevice>;

template class Vca::VDevice_<Vca::OS::MailboxDevice>;


/********************************************
 *------------------------------------------*
 *----                                  ----*
 *----  Vca::OS::MailboxDevice::Status  ----*
 *----                                  ----*
 *------------------------------------------*
 ********************************************/

/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

void Vca::OS::MailboxDevice::Status::onError (VkStatus const &rStatus) {
    switch (rStatus.Code ()) {
    case SS$_NOREADER:
	m_bNoReader = true;
	break;
    case SS$_NOWRITER:
	m_bNoWriter = true;
	break;
    }
}

/****************************
 ****************************
 *****  Use Accounting  *****
 ****************************
 ****************************/

void Vca::OS::MailboxDevice::Status::markAsStdinOf (VProcess *pProcess, char const *pCommand) {
    if (m_pProcess.setIfNil (pProcess) || m_pProcess.referent () == pProcess) {
	m_bSubprocessStdin = true;
	if (m_pMailbox)
	    m_pMailbox->launch (pCommand);
    }
}

/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

void Vca::OS::MailboxDevice::Status::onFinalReader () {
    if (m_pProcess) {
	if (m_bSubprocessStdout)
	    m_pProcess->onClosedStdout ();
	if (m_bSubprocessStderr)
	    m_pProcess->onClosedStderr ();
    }
}

void Vca::OS::MailboxDevice::Status::onFinalWriter () {
    if (m_pProcess && m_bSubprocessStdin)
	m_pProcess->onClosedStdin ();
}


/************************************
 *----------------------------------*
 *----                          ----*
 *----  Vca::OS::MailboxDevice  ----*
 *----                          ----*
 *----------------------------------*
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::OS::MailboxDevice::MailboxDevice (
    VReferenceable *pContainer, Manager *pManager, Handle &rhDevice
) : BaseClass (pContainer, pManager, rhDevice) {
}

Vca::OS::MailboxDevice::MailboxDevice (
    VReferenceable *pContainer, Manager *pManager
) : BaseClass (pContainer, pManager) {
}

Vca::OS::MailboxDevice::MailboxDevice (
    VReferenceable *pContainer, ThisClass &rOther
) : BaseClass (pContainer, rOther), m_pStatus (rOther.m_pStatus) {
    if (m_pStatus)
	m_pStatus->m_pMailbox.setTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::OS::MailboxDevice::~MailboxDevice () {
    if (m_pStatus)
	m_pStatus->m_pMailbox.clearIf (this);
}

/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

void Vca::OS::MailboxDevice::launch (char const *pCommand) {
    if (pCommand) {
	VkStatus iStatus; IOSB iIOSB;
	iStatus.MakeSuccessStatus (
	    sys$qio (
		EFN$C_ENF, operator Handle (), IO$_WRITEVBLK, &iIOSB,
		(astptr_t)&tis_io_complete, 0,
		const_cast<char*>(pCommand), strlen (pCommand), 0, 0, 0, 0
	    )
	) && iStatus.MakeSuccessStatus (
	    tis_synch (EFN$C_ENF, &iIOSB)
	);
    }
}
 
/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

void Vca::OS::MailboxDevice::onErrorDetect_(AbstractUse *pUse, VkStatus const &rStatus) {
    status ()->onError (rStatus);
    BaseClass::onErrorDetect_(pUse, rStatus);
}


/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

void Vca::OS::MailboxDevice::onFinalReader () {
    if (hasWriter () && (isSubprocessStdout () || isSubprocessStderr ())) {
	VBSProducer::Reference pLeftovers; {
	    VDevice::Reference pLeftoversDevice (new VBSProducerDevice_<MailboxDevice>(*this));
	    pLeftoversDevice->supply (pLeftovers);
	}
	VBSProducerBitBucket::Reference pGarbageDisposal (new VBSProducerBitBucket (pLeftovers));
    } else if (m_pStatus)
	m_pStatus->onFinalReader ();

    BaseClass::onFinalReader ();
}

void Vca::OS::MailboxDevice::onFinalWriter () {
//  Write one EOF for the program...
    VkStatus iStatus; XIOSB iIOSB;
    iStatus.MakeSuccessStatus (
	sys$qio (
	    EFN$C_ENF, operator Handle (),
	    IO$_WRITEOF + IO$M_READERCHECK + IO$M_NOW,
	    &iIOSB, (astptr_t)&tis_io_complete,
	    0, 0, 0, 0, 0, 0, 0
	)
    ) && iStatus.MakeSuccessStatus (
	tis_synch (EFN$C_ENF, &iIOSB)
    );
// ... and one for the troll that lives under the bridge:
    if (isSubprocessStdin ()) iStatus.MakeSuccessStatus (
	sys$qio (
	    EFN$C_ENF, operator Handle (),
	    IO$_WRITEOF + IO$M_READERCHECK + IO$M_NOW,
	    &iIOSB, (astptr_t)&tis_io_complete,
	    0, 0, 0, 0, 0, 0, 0
	)
    ) && iStatus.MakeSuccessStatus (
	tis_synch (EFN$C_ENF, &iIOSB)
    );

    if (m_pStatus)
	m_pStatus->onFinalWriter ();

    BaseClass::onFinalWriter ();
}


/*****************************
 *****************************
 *****                   *****
 *****  Vca::OS::Socket  *****
 *****                   *****
 *****************************
 *****************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

/*>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<*/
/*>>  VMS_LINUX Socket Class Workalike  <<*/
/*>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<*/

namespace Vca {
    namespace OS {
	class Socket {
	private:
	    enum {
		InvalidHandle = 0
	    };
	public:
	    Socket () : m_hSocket (InvalidHandle) {
	    }
	    ~Socket () {
		Close ();
	    }

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    void Attach (Handle hSocket);
	    bool Create (VkStatus &rStatus, int xType);

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    Handle Detach () {
		Handle hSocket = m_hSocket;
		m_hSocket = InvalidHandle;
		return hSocket;
	    }
	    void Close ();

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    bool Bind (VkStatus &rStatus, sockaddr_in const &rAddress);

	    bool Connect (VkStatus &rStatus, sockaddr_in const &rAddress);

	    bool Listen (VkStatus &rStatus, int backlog = 5);

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    Handle GetChannel () const {
		return m_hSocket;
	    }
	    bool GetPeerName (VkStatus &rStatus, void *addr,int len) const;
	    bool GetSockName (VkStatus &rStatus, void *addr,int len) const;

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    bool SetNoDelay (VkStatus &rStatus, bool bValue = true) const {
		return SetOptionValue (rStatus, IPPROTO_TCP, TCP_NODELAY, bValue);
	    }
	    bool GetNoDelay (VkStatus &rStatus, Pointer32<bool>::type pValue) const {
		return GetOptionValue (rStatus, IPPROTO_TCP, TCP_NODELAY, pValue);
	    }

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    void GetLimits (Device::Limits &rLimits, char const *pWhere) const;

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    template <typename T> bool GetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, T pValue) const {
		size_t sValue = sizeof (*pValue);
		return GetOptionValue (rStatus, xProtocol, xOption, pValue, cast_32_truncate (&sValue));
	    }
	    bool GetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, Pointer32<bool>::type pValue) const {
		int iValue = 0;
		if (GetOptionValue (rStatus, xProtocol, xOption, cast_32_truncate (&iValue))) {
		    *pValue = iValue ? true : false;
		    return true;
		}
		return false;

	    };
	    bool GetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, __void_ptr32 pVoid, Pointer32<size_t>::type psVoid) const;

	    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	    template <typename T> bool SetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, T iValue) const {
		return SetOptionValue (rStatus, xProtocol, xOption, cast_32_truncate (&iValue), sizeof(T));
	    }
	    bool SetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, bool bValue) const {
		int const iValue = bValue ? 1 : 0;
		return SetOptionValue (rStatus, xProtocol, xOption, iValue);
	    }
	    bool SetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, __const_void_ptr32 pData, size_t sData) const;

	private:
	    static void ConvertCodes (int &rxProtocol, int &rxOption);

	private:
	    Handle m_hSocket;
	};


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	void Socket::Attach (Handle hSocket) {
	    if (m_hSocket != hSocket) {
		Close ();
		m_hSocket = hSocket;
	    }
	}

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::Create (VkStatus &rStatus, int xType) {
	    Close ();

	    if (!assign (rStatus, m_hSocket))
		return false;

	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;
	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_SOCKET, &iIOSB, (astptr_t)&tis_io_complete, 0,
		(void*)AF_INET, xType, 0, 0, 0, 0
	    );
	    else {
		struct sockchar {
		    unsigned short m_xProtocol;
		    unsigned char m_xType;
		    unsigned char m_xFamily;
		} iCharactistics = {IPPROTO_TCP, static_cast<unsigned char>(xType), AF_INET};
		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SETMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    &iCharactistics, 0, 0, 0, 0, 0
		);
	    }
	    bool const bOK = rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);

	    if (bOK)
		return true;

	    Close ();

	    return false;
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::Bind (VkStatus &rStatus, sockaddr_in const &rAddress) {
	    sockaddr_in sin;
	    memcpy (&sin, &rAddress, sizeof (sin));

	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_BIND, &iIOSB, (astptr_t)&tis_io_complete, 0, &sin, sizeof (sin), 0, 0, 0, 0
	    );
	    else {
		_ile2 iItemList[2];
		memset(&iItemList, 0, sizeof(iItemList));

		iItemList[0].ile2$w_code = TCPIP$C_SOCK_NAME;
		iItemList[0].ile2$w_length = sizeof (sin);
#pragma __pointer_size __save
#pragma __pointer_size __short
		iItemList[0].ile2$ps_bufaddr = static_cast<sockaddr_in*>(&sin);
#pragma __pointer_size __restore

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SETMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, (unsigned __int64)&iItemList, 0, 0, 0
		);
	    }

	    return rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::Connect(VkStatus &rStatus, sockaddr_in const &rAddress) {
	    sockaddr_in sin;
	    memcpy (&sin, &rAddress, sizeof (sin));

	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_CONNECT, &iIOSB, (astptr_t)&tis_io_complete, 0, &sin, sizeof (sin), 0, 0, 0, 0
	    );
	    else {
		_ile2 iItemList[2];
		memset(&iItemList, 0, sizeof(iItemList));

		iItemList[0].ile2$w_code = TCPIP$C_SOCK_NAME;
		iItemList[0].ile2$w_length = sizeof (sin);
#pragma __pointer_size __save
#pragma __pointer_size __short
		iItemList[0].ile2$ps_bufaddr = static_cast<sockaddr_in*>(&sin);
#pragma __pointer_size __restore

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_ACCESS, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, (unsigned __int64)&iItemList, 0, 0, 0
		);
	    }

	    return rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool  Socket::Listen (VkStatus &rStatus, int backlog) {
	    XIOSB iIOSB;
	    unsigned int xStatus = UCX ? sys$qio (
		EFN$C_ENF, m_hSocket, IO$_SETMODE, &iIOSB, (astptr_t)&tis_io_complete, 0, /*p1*/0, 0, 0, backlog, 0, 0
	    ) : sys$qio (
		EFN$C_ENF, m_hSocket, IO$_LISTEN, &iIOSB, (astptr_t)&tis_io_complete, 0, /*p1*/(void*)backlog, 0, 0, 0, 0, 0
	    );
	    return rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::GetPeerName(VkStatus &rStatus, void *addr,int len) const {
	    sockaddr iName; union u1632 {
		U32 asU32;
		U16 asU16;
	    } sName;
	    sName.asU32 = sizeof (iName);

	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_GETPEERNAME, &iIOSB, (astptr_t)&tis_io_complete, 0,
		&iName, (unsigned __int64)&sName, 0, 0, 0, 0
	    );
	    else {
		_ile3 iItemList[2];
		memset (iItemList, 0, sizeof (iItemList));

		iItemList[0].ile3$w_code = TCPIP$C_SOCK_NAME;
		iItemList[0].ile3$w_length = sizeof (sockaddr);
#pragma __pointer_size __save
#pragma __pointer_size __short
		iItemList[0].ile3$ps_bufaddr = static_cast<sockaddr*>(&iName);
		iItemList[0].ile3$ps_retlen_addr = static_cast<U16*>(&sName.asU16);
#pragma __pointer_size __restore

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SENSEMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, 0, (unsigned __int64)&iItemList /* p4 <==> peername */, 0, 0
		);
	    }

	    bool bGood = rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);

	    if (bGood)
		memcpy (addr, &iName, len < sName.asU16 ? len : sName.asU16);

	    return bGood;
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::GetSockName(VkStatus &rStatus, void *addr,int len) const {
	    sockaddr iName; union u1632 {
		U32 asU32;
		U16 asU16;
	    } sName;
	    sName.asU32 = sizeof (iName);

	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_GETSOCKNAME, &iIOSB, (astptr_t)&tis_io_complete, 0,
		&iName, (unsigned __int64)&sName, 0, 0, 0, 0
	    );
	    else {
		_ile3 iItemList[2];
		memset (iItemList, 0, sizeof (iItemList));

		iItemList[0].ile3$w_code = TCPIP$C_SOCK_NAME;
		iItemList[0].ile3$w_length = sizeof (sockaddr);
#pragma __pointer_size __save
#pragma __pointer_size __short
		iItemList[0].ile3$ps_bufaddr = static_cast<sockaddr*>(&iName);
		iItemList[0].ile3$ps_retlen_addr = static_cast<U16*>(&sName.asU16);
#pragma __pointer_size __restore

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SENSEMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, (unsigned __int64)&iItemList /* p3 <==> sockname */, 0, 0, 0
		);
	    }

	    bool bGood = rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);

	    if (bGood)
		memcpy (addr, &iName, len < sName.asU16 ? len : sName.asU16);

	    return bGood;
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::SetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, __const_void_ptr32 pData, size_t sData) const {
	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) xStatus = sys$qio (
		EFN$C_ENF, m_hSocket, IO$_SETSOCKOPT, &iIOSB, (astptr_t)&tis_io_complete, 0,
		reinterpret_cast<void*>(xProtocol), xOption, reinterpret_cast<__int64>(pData), sData, 0, 0
	    ); else {
		ConvertCodes (xProtocol, xOption);

		_ile2 iValueILE;
		iValueILE.ile2$w_code = xOption;
		iValueILE.ile2$w_length = sData;
		iValueILE.ile2$ps_bufaddr = const_cast<__void_ptr32>(pData);

		_ile2 iOptionILE;
		iOptionILE.ile2$w_code = xProtocol;
		iOptionILE.ile2$w_length = sizeof (iValueILE);
 		iOptionILE.ile2$ps_bufaddr = cast_32_truncate (&iValueILE);

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SETMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, 0, 0, reinterpret_cast<__int64>(&iOptionILE), 0
		);
	    }

	    return rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	bool Socket::GetOptionValue (VkStatus &rStatus, int xProtocol, int xOption, __void_ptr32 pData, size_t* psData) const {
	    XIOSB iIOSB;
	    unsigned int xStatus = SS$_UNSUPPORTED;

	    if (!UCX) {
		unsigned int sData = *psData;
		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_GETSOCKOPT, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    reinterpret_cast<void*>(xProtocol), xOption, reinterpret_cast<__int64>(pData), reinterpret_cast<__int64>(&sData), 0, 0
		);
		*psData = sData;
	    } else {
		ConvertCodes (xProtocol, xOption);

		unsigned short sData = *psData;
		_ile3 iValueILE;
		iValueILE.ile3$w_code = xOption;
		iValueILE.ile3$w_length = sData;
		iValueILE.ile3$ps_bufaddr = pData;
		iValueILE.ile3$ps_retlen_addr = cast_32_truncate (&sData);

		_ile2 iOptionILE;
		iOptionILE.ile2$w_code = xProtocol;
		iOptionILE.ile2$w_length = sizeof (iValueILE);
 		iOptionILE.ile2$ps_bufaddr = cast_32_truncate (&iValueILE);

		xStatus = sys$qio (
		    EFN$C_ENF, m_hSocket, IO$_SENSEMODE, &iIOSB, (astptr_t)&tis_io_complete, 0,
		    0, 0, 0, 0, 0, reinterpret_cast<__int64>(&iOptionILE)
		);
		*psData = sData;
	    }

	    return rStatus.MakeSuccessStatus (xStatus)
		&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
		&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
	}


	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	void Socket::ConvertCodes (int &rxProtocol, int &rxOption) {
	    if (UCX) switch (rxProtocol) {
	    case SOL_SOCKET:
		rxProtocol = TCPIP$C_SOCKOPT;
		break;
	    case IPPROTO_TCP:
		rxProtocol = TCPIP$C_TCPOPT;
		break;
	    case IPPROTO_IP:
		rxProtocol = TCPIP$C_IPOPT;
		break;
	    }
	}

	void Socket::GetLimits (Device::Limits &rLimits, char const *pWhere) const {
	    if (UCX) {
		if (SocketOptionLimitOverride > 0)
		    rLimits.setTransferLimitTo (SocketOptionLimitOverride);
		else {
		    VkStatus iStatus; size_t sLimit;
		    if (GetOptionValue (iStatus, SOL_SOCKET, SO_RCVBUF, cast_32_truncate (&sLimit)))
			rLimits.setGetLimitTo (sLimit);

		    if (GetOptionValue (iStatus, SOL_SOCKET, SO_SNDBUF, cast_32_truncate (&sLimit)))
			rLimits.setPutLimitTo (sLimit);
		}
	    }
//	    printf ("+++ %5u: %s: %u %u\n", m_hSocket, pWhere, rLimits.getGetSizeLimit (), rLimits.getPutSizeLimit ());
	}

	/*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
	void Socket::Close () {
	    if (m_hSocket != InvalidHandle) {
		sys$dassgn (Detach ());
	    }
	}
    }
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::OS::SocketDevice  *****
 *****                         *****
 ***********************************
 ***********************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	class SocketDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (SocketDevice, Device);

	//  Data
	public:
	    typedef ThisClass Data;

	//  MultiStackAddress
	public:
	    struct MultiStackAddress {
		union {
		    unsigned short as_u16;
		    unsigned long  as_u32;
		} m_sAddress;
		sockaddr m_iAddress;
	    };

	//  SocketUse_<DeviceUse>
	public:
	    template <class DeviceUse> class SocketUse_ : public DeviceUse {
		DECLARE_FAMILY_MEMBERS (SocketUse_<DeviceUse>, DeviceUse);

		friend class SocketDevice;

	    //  Construction
	    protected:
		SocketUse_(VReferenceable *pContainer, SocketDevice *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~SocketUse_() {
		}

	    //  Access
	    protected:
		SocketDevice *device () const {
		    return static_cast<SocketDevice*>(BaseClass::m_pDevice.referent ());
		}
	    };

	//  AcceptUse
	public:
	    class AcceptUse : public SocketUse_<Use> {
		DECLARE_FAMILY_MEMBERS (AcceptUse, SocketUse_<Use>);

	    //  Friends
		friend class SocketDevice;

	    //  Aliases
	    public:
#pragma __pointer_size __save
#pragma __pointer_size __short
		typedef XIOSB *xiosb_ptr32_t;
		typedef _ile3 &ile3_ref32_t;
		typedef Handle &handle_ref32_t;
		typedef MultiStackAddress *multistackaddress_ptr32_t;
#pragma __pointer_size __restore

	    //  Stuff
	    public:
		struct Stuff {
		    MultiStackAddress	m_iPeerAddress;
		    _ile3		m_iPeerAddressILE;
		    XIOSB		m_iIOSB;
		    Handle		m_hNewSocket;
		};

	    //  Construction
	    protected:
		AcceptUse (VReferenceable *pContainer, SocketDevice *pDevice) : BaseClass (pContainer, pDevice) {
		    pDevice->grab (newSocket ());

		    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
		    if (UCX) {
			multistackaddress_ptr32_t pPeerAddress = peerAddress ();
			ile3_ref32_t rPeerAddressILE = peerAddressILE ();

			rPeerAddressILE.ile3$w_length = sizeof (pPeerAddress->m_iAddress);
			rPeerAddressILE.ile3$w_code = TCPIP$C_SOCK_NAME;
			rPeerAddressILE.ile3$ps_bufaddr = &pPeerAddress->m_iAddress;
			rPeerAddressILE.ile3$ps_retlen_addr = &pPeerAddress->m_sAddress.as_u16;
		    }
		    /*>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<*/
		}

	    //  Destruction
	    protected:
		~AcceptUse () {
		    device ()->close (newSocket ());
		}

	    //  Access
	    public:
#pragma __pointer_size __save
#pragma __pointer_size __short
		xiosb_ptr32_t iosb () {
		    return &m_pAcceptStuff->m_iIOSB;
		}
		ile3_ref32_t peerAddressILE () {
		    return m_pAcceptStuff->m_iPeerAddressILE;
		}
		handle_ref32_t newSocket () {
		    return m_pAcceptStuff->m_hNewSocket;
		}
		multistackaddress_ptr32_t peerAddress () {
		    return &m_pAcceptStuff->m_iPeerAddress;
		}
#pragma __pointer_size __restore

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus) {
		    return device ()->doAccept (rStatus, this) || onError (rStatus);
		}

	    //  Event Processing
	    protected:
		bool onEvent_();

	    //  State
	    private:
		Area32_<Stuff>::Pointer m_pAcceptStuff;
	    };
	    friend class AcceptUse;


	//  template <class DeviceUse> class SelectedUse_
	public:
	    template <class DeviceUse> class SelectedUse_ : public SocketUse_<DeviceUse> {
		DECLARE_FAMILY_MEMBERS (SelectedUse_<DeviceUse>, SocketUse_<DeviceUse>);

		friend class SocketDevice;

	    //  Construction
	    protected:
		SelectedUse_(
		    VReferenceable *pContainer, SocketDevice *pDevice
		) : BaseClass (pContainer, pDevice), m_bDoingSelect (DoingSelect) {
		}

	    //  Destruction
	    protected:
		~SelectedUse_() {
		}

	    //  Access
	    protected:
		using BaseClass::device;

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
 		bool start (VkStatus &rStatus, typename DeviceUse::Area const &rArea) {
//		    BaseClass::displayQuotas ("preStart");
		    bool bResult = m_bDoingSelect ? doSelect (rStatus, rArea) : BaseClass::start (rStatus, rArea);
//		    BaseClass::displayQuotas ("onStart ");
		    return bResult;
		}
	    private:
		bool doSelect (VkStatus &rStatus, typename DeviceUse::Area const &rArea) {
		    BaseClass::m_iArea.setTo (rArea);
 		    return device()->doSelect (rStatus, this) || BaseClass::onError (rStatus);
 		}

	    //  Event Processing
	    protected:
		bool onEvent_() {
//		    BaseClass::displayQuotas ("onEvent ");
		    return m_bDoingSelect ? onSelect () : BaseClass::onEvent_();
		}
	    private:
		bool onSelect () {
		    BaseClass::decrementUseCount ();
		    m_bDoingSelect = false; VkStatus iStatus;
		    return BaseClass::doTransfer (iStatus) && iStatus.isntBlocked ();
		}

	    //  State
	    private:
		bool m_bDoingSelect;
	    };
	    friend class SelectedUse_<BaseClass::BSGet>;
	    typedef SelectedUse_<BaseClass::BSGet> BSGet;

//	Write IO$_SELECT's trash the stack in 64-bit land.  Since they're suspect there, don't use them anywhere.
//	    friend class SelectedUse_<BaseClass::BSPut>;
//	    typedef SelectedUse_<BaseClass::BSPut> BSPut;

	//  Construction
	public:
	    SocketDevice (
		VReferenceable *pContainer, Manager *pManager, Handle &rhDevice
	    ) : BaseClass (pContainer, pManager) {
		claim (rhDevice);
	    }
	    SocketDevice (
		VReferenceable *pContainer, Manager *pManager, Socket &rhSocket
	    ) : BaseClass (pContainer, pManager) {
		claim (rhSocket);
	    }
	    SocketDevice (VReferenceable *pContainer, Manager *pManager) : BaseClass (pContainer, pManager) {
	    }
	    SocketDevice (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther.manager ()) {
		claim (rOther);
	    }

	//  Destruction
	public:
	    ~SocketDevice () {
		close ();
	    }

	//  Update
	private:
	    void claimHandle (Handle &rhSocket);
	    void claim (Handle &rhSocket, Limits const &rLimits);
	    void claim (Handle &rhSocket, Socket const *pSocket = 0);
	    void claim (Socket &rhSocket);
	    void claim (ThisClass &rOther);
	public:
	    using BaseClass::grab;
	    Handle grab () {
		m_hSocket.Detach ();
		return BaseClass::grab ();
	    }

	//  Access
	public:
	    bool deviceDriverIs64Bit () const {
		return SocketDriverIs64Bit;
	    }
	    bool getName (VkStatus &rStatus, VString &rName) const;

	//  Device Control
	public:
	    using BaseClass::close;
	    void close () {
		m_hSocket.Detach ();
		BaseClass::close ();
	    }
	    void shutdown (int how);

	    bool setNoDelay (VkStatus &rStatus, bool bValue = true) const {
		return m_hSocket.SetNoDelay (rStatus, bValue);
	    }

	    template <typename T> bool setOptionValue (VkStatus &rStatus, int xProtocol, int xOption, T iValue) const {
		return m_hSocket.SetOptionValue (rStatus, xProtocol, xOption, iValue);
	    }

	//  Device Query
	public:
	    template <typename T> bool getOptionValue (VkStatus &rStatus, int xProtocol, int xOption, T pValue) const {
		return m_hSocket.GetOptionValue (rStatus, xProtocol, xOption, pValue);
	    }

	//  Device Use
	protected:
	    bool doAccept (VkStatus &rStatus, AcceptUse *pUse);
	    bool doSelect (VkStatus &rStatus, BSGet *pUse) {
		return doSelect (rStatus, pUse, 1 << 1);
	    }
	    bool doSelect (VkStatus &rStatus, BSPut *pUse) {
		return doSelect (rStatus, pUse, 1 << 2);
	    }
	private:
	    bool doSelect (VkStatus &rStatus, BSMove *pUse, unsigned int xSelectMode);

	//  Event Processing
	private:
	    bool onEvent (
		VkStatus &rStatus, VDevice::Reference &rpDevice, Handle &rhSocket, sockaddr const &rPeerAddress, socklen_t sPeerAddress
	    ) const;

	//  User Accounting
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    void onFirstReader () {
		ReportUser ("first socket reader", ++s_cSocketReaders);
		BaseClass::onFirstReader ();
	    }
	    void onFirstWriter () {
		ReportUser ("first socket writer", ++s_cSocketWriters);
		BaseClass::onFirstWriter ();
	    }
	    void onFinalReader () {
		shutdown (0 /*SHUT_RD*/);
		BaseClass::onFinalReader ();
	    }
	    void onFinalWriter () {
		shutdown (1 /*SHUT_WR*/);
		BaseClass::onFinalWriter ();
	    }
	private:
	    void onUserCountIsZero_() {
		close ();
	    }

	//  State
	private:
	    Socket m_hSocket;
	};
    }
}
template class Vca::VAccept_<Vca::OS::SocketDevice>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ProducerTypes>;

template class Vca::VBSProducerConsumerDevice_<Vca::OS::SocketDevice>;
template class Vca::VListeningDevice_<Vca::OS::SocketDevice>;
template class Vca::VDevice_<Vca::OS::SocketDevice>;


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::SocketDevice::getName (VkStatus &rStatus, VString &rName) const {
    sockaddr iSockAddress;
    if (!m_hSocket.GetSockName (rStatus, &iSockAddress, sizeof (iSockAddress)))
	return false;

    VkSocketAddress iAddress (iSockAddress);
    return iAddress.getName (rName);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

void Vca::OS::SocketDevice::claimHandle (Handle &rhSocket) {
    close ();
    Handle hSocket = rhSocket;
    BaseClass::claim (rhSocket);
    m_hSocket.Attach (hSocket);
}

void Vca::OS::SocketDevice::claim (Handle &rhSocket, Limits const &rLimits) {
    claimHandle (rhSocket);
    setDeviceLimitsTo (rLimits);
}

void Vca::OS::SocketDevice::claim (Handle &rhSocket, Socket const *pSocket) {
    claimHandle (rhSocket);

    if (UsingSocketOptionLimits) {
	Limits iLimits (deviceLimits ());
	m_hSocket.GetLimits (iLimits, "Claim  ");
	setDeviceLimitsTo (iLimits);
    }
}

void Vca::OS::SocketDevice::claim (Socket &rhSocket) {
    Handle hSocket = rhSocket.Detach ();
    claim (hSocket, &rhSocket);
}

void Vca::OS::SocketDevice::claim (ThisClass &rOther) {
    Handle hSocket = rOther.grab ();
    claim (hSocket, rOther.deviceLimits ());
}


/****************************
 ****************************
 *****  Device Control  *****
 ****************************
 ****************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

void Vca::OS::SocketDevice::shutdown (int how) {
    unsigned int xStatus; XIOSB iIOSB;
    if (!UCX) xStatus = sys$qio (
	EFN$C_ENF,
	m_hSocket.GetChannel (), IO$_SHUTDOWN, &iIOSB,
	(astptr_t)&tis_io_complete, 0,
	(void*) how, 0, 0, 0, 0, 0
    );
    else {
	int flags = TCPIP$C_DSC_SND;	// case 1:
	switch (how) {
	case 0:
	    flags = TCPIP$C_DSC_RCV;
	    break;
	case 2:
	    flags = TCPIP$C_DSC_ALL;
	    break;
	}
	xStatus = sys$qio (
	    EFN$C_ENF,
	    m_hSocket.GetChannel (), IO$_DEACCESS|IO$M_SHUTDOWN, &iIOSB,
	    (astptr_t)&tis_io_complete, 0,
	    0, 0, 0, flags, 0, 0
	);
    }
    if ($VMS_STATUS_SUCCESS (xStatus))
	tis_synch (EFN$C_ENF, &iIOSB);
}


/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::SocketDevice::doAccept (VkStatus &rStatus, AcceptUse *pUse) {
    incrementUseCount ();

    return assign (rStatus, pUse->newSocket ()) && rStatus.MakeBlockedStatus (
	UCX ? sys$qio (		// ... UCX
	    EFN$C_ENF,				// event flag
	    m_hSocket.GetChannel (),		// listener channel
	    IO$_ACCESS|IO$M_ACCEPT,		// function
	    pUse->iosb (),			// iosb
	    (astptr_t)&Use::OnAST,		// ast callback
	    (__int64)pUse,			// ast argument
	    0,					// p1
	    0,					// p2
	    (__int64)&pUse->peerAddressILE (),	// address return
	    (__int64)&pUse->newSocket (),	// channel 'return'
	    0,					// p5
	    0					// p6
	) : sys$qio (		// ... Multinet
	    EFN$C_ENF,				// Event flag
	    pUse->newSocket (),			// new socket channel
	    IO$_ACCEPT,				// function
	    pUse->iosb (),			// iosb
	    (astptr_t)&Use::OnAST,		// ast callback
	    (__int64)pUse,			// ast argument
	    pUse->peerAddress (),		// address return
	    sizeof (MultiStackAddress),		// address length
	    m_hSocket.GetChannel (),		// listener channel
	    0,					// p4
	    0,					// p5
	    0					// p6
	)
    );
}

bool Vca::OS::SocketDevice::doSelect (VkStatus &rStatus, BSMove *pUse, unsigned int xSelectMode) {
    incrementUseCount ();

    bool bResult =  UCX ? rStatus.MakeUnimplementedStatus () : rStatus.MakeBlockedStatus (
	sys$qio (
	    EFN$C_ENF,
	    m_hSocket.GetChannel (), IO$_SELECT, pUse->iosb (false),
	    (astptr_t)&Use::OnAST, reinterpret_cast<__int64>(pUse),
	    &xSelectMode, 0, 0, 0, 0, 0
	)
    );
//    displayQuotas ("onSelect");

    return bResult;
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::SocketDevice::onEvent (
    VkStatus &rStatus, VDevice::Reference &rpDevice, Handle &rhSocket, sockaddr const &rPeerAddress, socklen_t sPeerAddress
) const {
    SocketDevice::Data iDD (0, m_pManager, rhSocket);
    rpDevice.setTo (new VBSProducerConsumerDevice_<SocketDevice> (iDD));
    return true;
}

bool Vca::OS::SocketDevice::AcceptUse::onEvent_() {
    VkStatus iStatus; VDevice::Reference pDevice;
    return device ()->onEvent (
	iStatus, pDevice, newSocket (), peerAddress ()->m_iAddress, peerAddress ()->m_sAddress.as_u16
    ) ? onData (pDevice) : onError (iStatus);
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::OS::DatagramSocket  *****
 *****                           *****
 *************************************
 *************************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

/*>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<*/
/*>>>>>  Datagram Socket Device  <<<<<*/
/*>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<*/

namespace Vca {
    namespace OS {
	class DatagramSocket : public SocketDevice {
	    DECLARE_FAMILY_MEMBERS (DatagramSocket, SocketDevice);

	//  Data
	public:
	    typedef ThisClass Data;

	//  PeerAddress
	public:
	    class PeerAddress {
		DECLARE_FAMILY_MEMBERS (PeerAddress, void);

	    //  Construction
	    public:
		PeerAddress ();

	    //  Destruction
	    public:
		~PeerAddress ();
	    };

	//  BSGet
	public:
	    typedef BaseClass::BSGet BSGetBase;
	    class BSGet : public BSGetBase {
		DECLARE_FAMILY_MEMBERS (BSGet, BSGetBase);

	    //  Construction
	    protected:
		BSGet (VReferenceable *pContainer, DatagramSocket *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSGet () {
		}

	    //  Access
	    public:
		virtual /*override*/ void supply (
		    XIOSB *&rpIOSB, pointer_t &rpArea, size_t &rsArea, U64 &riP3, U64 &riP4, U64 &riP5, bool bIn32BitMemory
		) {
		    BaseClass::supply (rpIOSB, rpArea, rsArea, riP3, riP4, riP5, bIn32BitMemory);
		    if (UCX) {
		    }
		    else {
		    }
		}
	    };

	//  BSPut
	public:
	    typedef BaseClass::BSPut BSPutBase;
	    class BSPut : public BSPutBase {
		DECLARE_FAMILY_MEMBERS (BSPut, BSPutBase);

	    //  Construction
	    protected:
		BSPut (VReferenceable *pContainer, DatagramSocket *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSPut () {
		}

	    //  Access
	    public:
		virtual /*override*/ void supply (
		    XIOSB *&rpIOSB, pointer_const_t &rpArea, size_t &rsArea, U64 &riP3, U64 &riP4, U64 &riP5, bool bIn32BitMemory
		) {
		    BaseClass::supply (rpIOSB, rpArea, rsArea, riP3, riP4, riP5, bIn32BitMemory);
		    if (UCX) {
		    }
		    else {
		    }
		}
	    };

	//  Construction
	public:
	    DatagramSocket (VReferenceable *pContainer, Manager *pManager, Handle &rhDevice) : BaseClass (pContainer, pManager, rhDevice) {
	    }
	    DatagramSocket (VReferenceable *pContainer, Manager *pManager, Socket &rhSocket) : BaseClass (pContainer, pManager, rhSocket) {
	    }
	    DatagramSocket (VReferenceable *pContainer, Manager *pManager) : BaseClass (pContainer, pManager) {
	    }
	    DatagramSocket (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther) {
	    }

	//  Destruction
	public:
	    ~DatagramSocket () {
	    }

	};
    }
}
template class Vca::VAccept_<Vca::OS::SocketDevice>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ProducerTypes>;

template class Vca::VBSProducerConsumerDevice_<Vca::OS::SocketDevice>;
template class Vca::VListeningDevice_<Vca::OS::SocketDevice>;
template class Vca::VDevice_<Vca::OS::SocketDevice>;

template class Vca::VAccept_<Vca::OS::DatagramSocket>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::DatagramSocket>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::DatagramSocket>::ProducerTypes>;

template class Vca::VBSProducerConsumerDevice_<Vca::OS::DatagramSocket>;
template class Vca::VListeningDevice_<Vca::OS::DatagramSocket>;

template class Vca::VDatagramDevice_<Vca::OS::DatagramSocket>;
template class Vca::VDevice_<Vca::OS::DatagramSocket>;


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::OS::<x>_descriptor_s  *****
 *****                             *****
 ***************************************
 ***************************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	using namespace V::OS;
    }
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::OS::DeviceName  *****
 *****                       *****
 *********************************
 *********************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

static char const g_iNullDeviceName[] = "nl:";

namespace Vca {
    namespace OS {
	class DeviceName {
	//  Aliases
	public:
	    typedef char storage_t[64];
	    typedef unsigned short length_t;

	//  strdsc_t
	public:
	    typedef str32_descriptor_s<sizeof(storage_t)> strdsc_t;
	    typedef strdsc_t::r_t strdsc_r_t;
	    typedef strdsc_t::const_r_t strdsc_const_r_t;

	//  Construction
	public:
	    DeviceName (strdsc_const_r_t rOther) : m_sName (rOther.stringLength ()) {
		memcpy (m_iName, rOther.string (), sizeof (m_iName));
	    }
	    DeviceName (DeviceName const &rOther) : m_sName (rOther.m_sName) {
		memcpy (m_iName, rOther.m_iName, sizeof (m_iName));
	    }
	    DeviceName () : m_sName (sizeof (g_iNullDeviceName) - 1) {
		memcpy (m_iName, g_iNullDeviceName, m_sName);
	    }

	//  Destruction
	public:
	    ~DeviceName () {
	    }

	//  Access
	public:
	    char const *string () const {
		return m_iName;
	    }
	    void supply (strdsc_r_t rDescriptor) {
#pragma __message __save
#pragma __message __disable(INCARGTYP)
		rDescriptor.setTo (m_iName, m_sName);
#pragma __message __restore
	    }

	//  Query
	public:
	    bool namesTheNullDevice () const {
		return strncmp (m_iName, g_iNullDeviceName, m_sName) == 0;
	    }

	//  Update
	public:
	    bool setFrom (VkStatus &rStatus, Handle hDevice);

	//  State
	private:
	    storage_t	m_iName;
	    length_t	m_sName;
	};
    }
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::DeviceName::setFrom (VkStatus &rStatus, Handle hDevice) {
#pragma __pointer_size __save
#pragma __pointer_size __short
    storage_t iName, *piName = static_cast<storage_t*>(&iName);
    length_t  sName, *psName = static_cast<length_t *>(&sName);

    _ile3 iItemList[2];
    memset (iItemList, 0, sizeof (iItemList));

    iItemList[0].ile3$w_length = sizeof (storage_t);
    iItemList[0].ile3$w_code = DVI$_DEVNAM;
    iItemList[0].ile3$ps_bufaddr = piName;
    iItemList[0].ile3$ps_retlen_addr = psName;

    XIOSB iIOSB;
    int xStatus = sys$getdvi (
	EFN$C_ENF, hDevice, NULL, static_cast<_ile3*>(iItemList), cast_32_truncate(&iIOSB), (astptr32_t)&tis_io_complete, 0, NULL
    );
#pragma __pointer_size __restore
    memcpy (m_iName, iName, sizeof (iName));
    m_sName = sName;

    return rStatus.MakeSuccessStatus (xStatus)
	&& rStatus.MakeSuccessStatus (tis_synch (EFN$C_ENF, &iIOSB))
	&& rStatus.MakeSuccessStatus (iIOSB.iosb$w_status);
}


/********************************
 ********************************
 *****                      *****
 *****  Vca::OS::FarHandle  *****
 *****                      *****
 ********************************
 ********************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	class FarHandle : public ManagedDevice {
	    DECLARE_FAMILY_MEMBERS (FarHandle, ManagedDevice);

	//  Aliases
	public:
	    typedef DeviceName::strdsc_t strdsc_t;
	    typedef strdsc_t::r_t strdsc_r_t;
	    typedef MailboxDevice::Status mailbox_status_t;

	//  Construction
	public:
	    FarHandle (VReferenceable *pContainer, DeviceManager *pManager);

	//  Destruction
	public:
	    ~FarHandle () {
	    }

	//  Access
	public:
	    char const *deviceName () const {
		return m_iDeviceName.string ();
	    }

	    void supply (strdsc_r_t rDescriptor) {
		m_iDeviceName.supply (rDescriptor);
	    }

	//  Query
	public:
	    bool namesTheNullDevice () const {
		return m_iDeviceName.namesTheNullDevice ();
	    }

	//  Update
	public:
	    bool setFrom (VkStatus &rStatus, Handle hDevice) {
		return m_iDeviceName.setFrom (rStatus, hDevice);
	    }

	    void linkTo (mailbox_status_t *pStatus) {
		m_pMailboxStatus.setTo (pStatus);
	    }
	    void markAsStdinOf (VProcess *pProcess, char const *pCommand) const {
		if (m_pMailboxStatus)
		    m_pMailboxStatus->markAsStdinOf (pProcess, pCommand);
		else
		    pProcess->onClosedStdin ();
	    }
	    void markAsStdoutOf (VProcess *pProcess) const {
		if (m_pMailboxStatus)
		    m_pMailboxStatus->markAsStdoutOf (pProcess);
		else
		    pProcess->onClosedStdout ();
	    }
	    void markAsStderrOf (VProcess *pProcess) const {
		if (m_pMailboxStatus)
		    m_pMailboxStatus->markAsStderrOf (pProcess);
		else
		    pProcess->onClosedStderr ();
	    }

	//  State
	private:
	    DeviceName m_iDeviceName;
	    mailbox_status_t::Reference m_pMailboxStatus;
	};
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

Vca::OS::FarHandle::FarHandle (
    VReferenceable *pContainer, DeviceManager *pDeviceManager
) : BaseClass (pContainer, pDeviceManager) {
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::OS::DeviceManager  *****
 *****                          *****
 ************************************
 ************************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

namespace Vca {
    namespace OS {
	class DeviceManager : public VDeviceManager {
	    DECLARE_CONCRETE_RTTLITE (DeviceManager, VDeviceManager);

	//  Aliases
	public:
	    typedef VFDIODeviceImplementation FDIODevice;

	//  Friends
	    friend class Device;

	//  EventFlag
	public:
	    typedef unsigned int EventFlag;

	//  Construction
	public:
	    DeviceManager (VCohort *pCohort);

	//  Destruction
	private:
	    ~DeviceManager ();

	//  Event Processing
	private:
	    void linkQueues ();						// ... AST level ONLY !!!

	    void lockQueue () {						// ... non-AST level ONLY !!!
		__LOCK_LONG (&m_iQueueLock);
	    }
	    bool lockQueue (unsigned int nTries) {			// ... any level
		return __LOCK_LONG_RETRY (&m_iQueueLock, nTries);
	    }
	    void unlockQueue () {					// ... any level
#pragma message save
#pragma message disable UNAVOLACC
		__UNLOCK_LONG (&m_iQueueLock);
#pragma message restore
	    }

	    static void OnEventAST (ThisClass *pDeviceManager);		// ... AST level ONLY
	    static void OnQueueAST (ThisClass *pDeviceManager);		// ... AST level ONLY

	    void onAST (DeviceUse *pUse);				// ... AST level ONLY
	    void onAST ();						// ... AST level ONLY

	    //  Non-AST level event processing:
	    bool processCurrentEvents (bool bClearEF);				// ... non-AST level ONLY !!!

	private:
	    virtual /*override*/ bool postInterrupt_();
	    virtual /*override*/ bool processEvent_(size_t sTimeout);	// ... non-AST level ONLY !!!

	//  Suppliers
	public:
	    //  ... subprocess
	    bool supply (
		VkStatus&		rStatus,
		VProcess::Reference&	rpProcess,
		FarHandle&		rhProcessStdin,
		FarHandle&		rhProcessStdout,
		FarHandle&		rhProcessStderr, // & == &rhProcessStdout -> stdout device device
		char const*		pCommand
	    );

	    //  ... subprocesses pipes
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+
		FarHandle&		rhFarsideProducer, // <-+
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+
	    );
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+
		FarHandle&		rhFarsideProducer  // <-+
	    );
	    bool supply (
		VkStatus&		rStatus,
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+
	    );

	    //  ... standard pipes
	    bool supplyStdToHere (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdin
	    );
	    bool supplyStdToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdout
	    );
	    bool supplyErrToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stderr
	    );

	    //  ... sockets
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference&	rpDevice,
		VkSocketAddress const&	rAddress,
		int			xProtocol,
		int			xType,
		bool			bNoDelay,
		bool			bPassive
	    );

	    //  ... files
	    bool supply (
		VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
	    );
	    bool supply (
		VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
	    );

	//  State
	private:
	    EventFlag			m_xEventFlag;
	    DeviceUse::Pointer		m_pProducerQueueHead;	// ... new events are added here by ASTs, ...
	    DeviceUse::Pointer		m_pProducerQueueTail;
	    unsigned int		m_iQueueLock;		// ... and, when the lock allows, ...
	    DeviceUse::Pointer		m_pQueueHead;		// ... transferred here, ...
	    DeviceUse::Pointer		m_pQueueTail;
	    DeviceUse::Pointer		m_pConsumerQueueHead;	// ... and finally here for processing.
	    DeviceUse::Pointer		m_pConsumerQueueTail;
	};
    }
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::OS::DeviceManager::DeviceManager (
    VCohort *pCohort
) : BaseClass (pCohort), m_xEventFlag (EFN$C_ENF), m_iQueueLock (0) {
    lib$get_ef (&m_xEventFlag);

    joinCohort ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::OS::DeviceManager::~DeviceManager () {
    exitCohort ();

    lib$free_ef (&m_xEventFlag);
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/*-------------------------------*
 *----  OpenVMS (AST Level)  ----*
 *-------------------------------*/

void Vca::OS::Device::onAST (Use *pUse) {
    /*============================================================*
     *----  WARNING: This routine MUST be called at AST level.
     *============================================================*/

    m_pManager->onAST (pUse);
}

//  WARNING: This routine is called at AST level:
void Vca::OS::DeviceManager::onAST (DeviceUse *pUse) {
    /*============================================================*
     *----  WARNING: This routine MUST be called at AST level.
     *============================================================*/
    /*--------------------------------------------------------------------*
     *  This code is thread safe because...
     *	... the AST queue is manipulated only at user mode AST level.
     *	... no more than one user mode AST can ever be active at a time
     *	    in a process, regardless of the number of threads.
     *  This code is AST safe because...
     *	... it never blocks and calls no routines that do.
     *--------------------------------------------------------------------*/

//  Add the use to the AST queue, ...
    if (m_pProducerQueueTail.isntNil ())
	m_pProducerQueueTail->m_pQueueSuccessor.setTo (pUse);
    else
	m_pProducerQueueHead.setTo (pUse);
    m_pProducerQueueTail.setTo (pUse);

//  ... and try to link the queues, setting the event flag if appropriate:
    linkQueues ();
}

//  WARNING: This routine is called at AST level:
void Vca::OS::DeviceManager::OnEventAST (ThisClass *pDeviceManager) {
    pDeviceManager->onAST ();
}

//  WARNING: This routine is called at AST level:
void Vca::OS::DeviceManager::OnQueueAST (ThisClass *pDeviceManager) {
    pDeviceManager->linkQueues ();
}

//  WARNING: This routine is called at AST level:
void Vca::OS::DeviceManager::onAST () {
    sys$setef (m_xEventFlag);
}

void Vca::OS::DeviceManager::linkQueues () {
    /*============================================================*
     *----  WARNING: This routine MUST be called at AST level.
     *============================================================*/

    if (lockQueue (1)) {
	bool bTriggeringEvent = false;

	//  If the queue lock was acquired, link the queues, ...
	if (m_pQueueTail.isntNil ())
	    m_pQueueTail->m_pQueueSuccessor.claim (m_pProducerQueueHead);
	else {
	    m_pQueueHead.claim (m_pProducerQueueHead);
	    bTriggeringEvent = m_pQueueHead.isntNil ();
	}
	m_pQueueTail.claim (m_pProducerQueueTail);

	//  ... unlock the queue, ...
	unlockQueue ();

	//  ... and signal an event if the main queue was extended:
	if (bTriggeringEvent)
	    onAST ();
    }
}


/*-----------------------------*
 *----  OpenVMS (Non-AST)  ----*
 *-----------------------------*/

bool Vca::OS::DeviceManager::postInterrupt_() {
    sys$dclast ((astptr_t)&ThisClass::OnEventAST, (__int64)this, 0);
    return true;
}

//  Returns true if at an event was processed, false otherwise:
bool Vca::OS::DeviceManager::processCurrentEvents (bool bClearEF) {

  if (bClearEF) {
    //  Acknowledge (clear) the signal that got us here, ...
    sys$clref (m_xEventFlag);
  }

    //  While the event queue isn't empty, ...
    bool bEventsProcessed = false;
    while (m_pQueueHead.isntNil ()) {
	//  ... move its contents to the consumer queue, ...
	lockQueue ();
	if (m_pConsumerQueueTail.isntNil ())
	    m_pConsumerQueueTail->m_pQueueSuccessor.claim (m_pQueueHead);
	else
	    m_pConsumerQueueHead.claim (m_pQueueHead);
	m_pConsumerQueueTail.claim (m_pQueueTail);
	unlockQueue ();

	//  ... and process the events:
	while (m_pConsumerQueueHead.isntNil ()) {
	    DeviceUse::Pointer pDequeued (m_pConsumerQueueHead);
	    m_pConsumerQueueHead.claim (pDequeued->m_pQueueSuccessor);
	    if (m_pConsumerQueueHead.isNil ())
		m_pConsumerQueueTail.clear ();

	    pDequeued->onEvent ();
	    bEventsProcessed = true;
	}
    }

    //  If more events have arrived, get them for next time, ...
    if (m_pProducerQueueHead.isntNil ()) {
	sys$dclast ((astptr_t)&ThisClass::OnQueueAST, (__int64)this, 0);
    }

    //  ... and return:
    return bEventsProcessed;
}


bool Vca::OS::DeviceManager::processEvent_(
    size_t sTimeout /* in milliseconds */
) {
//  Process events currently in the queue, ...
    if (processCurrentEvents (false))
	return true;	//  ... some events.

//  Otherwise, wait for the appropriate amount of time, ...
    if (Vca_InfiniteWait == sTimeout)
	sys$waitfr (m_xEventFlag);
    else if (sTimeout > 0) {
	unsigned __int64 xTimerRequest = reinterpret_cast<unsigned __int64>(this);
	unsigned int xOperation = LIB$K_DELTA_SECONDS_F;
	__f_float iTimeoutFraction = static_cast<__f_float>(sTimeout) / 1000;
	_generic_64 iTimerTime;
	//  lib$cvtf_to_internal_time requires an f-float.  In ANSI64 bit land, 'float' is IEEE.
	//  The __f_float declaration for iTimeoutFraction handles the first part while the
	//  reinterpret_cast convinces the compiler that the '__f_float' we've got is really the
	//  'float' required by lib$cvtf_to_internal_time's signature.
	unsigned int xStatus = lib$cvtf_to_internal_time (
	    &xOperation, reinterpret_cast<float*>(&iTimeoutFraction), &iTimerTime.gen64$q_quadword
	);
	if (!$VMS_STATUS_SUCCESS (xStatus)) {
	    return false;	//  ... no timer -> no new events.
	}
	xStatus = sys$setimr (EFN$C_ENF, &iTimerTime, (astptr_t)&ThisClass::OnEventAST, xTimerRequest, 0);
	if (!$VMS_STATUS_SUCCESS (xStatus)) {
	    return false;	//  ... no timer -> no new events.
	}

	sys$waitfr (m_xEventFlag);
	sys$cantim (xTimerRequest, 0);
    }

//  ... and process any events that occurred:
    return processCurrentEvents (true);
}


/********************************
 ********************************
 *****  Resource Suppliers  *****
 ********************************
 ********************************/

/************************
 *****  Subprocess  *****
 ************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    FarHandle&			rhProcessStdin,
    FarHandle&			rhProcessStdout,
    FarHandle&			rhProcessStderr, // & == &rhProcessStdout -> stdout device device
    char const*			pCommand
) {
//  Parse options from command string...
    bool bUsingCreateProcess = false;
    bool bDetached = false;
    bool bStderrSuppressed = false;
    bool bTerminatingOnClosedStdio = false;
    bool bProcessingOptions = pCommand && '{' == pCommand[0];
    while (bProcessingOptions) {
	switch (*++pCommand) {
	case '2':
	    bStderrSuppressed = true;
	    break;
	case 'c':
	case 'C':
	    bUsingCreateProcess = bStderrSuppressed = true;
	    break;
	case 'd':
	case 'D':
	    bDetached = true;
	    break;
	case '9':
	    bUsingCreateProcess = false;
	    bDetached = true;
	    break;
	case '.':
	    bTerminatingOnClosedStdio = true;
	    break;
	case '}':
	    bProcessingOptions = false;
	    pCommand++;
	    break;
	case '\0':
	    bProcessingOptions = false;
	    break;
	}
    }
    pCommand += strspn (pCommand, " \t");

//  Determine if 'stderr' needs to be created...
    bool const bStderrRequested = &rhProcessStderr != &rhProcessStdout;
    bool const bStderrInUse = bUsingCreateProcess && bStderrRequested && !bStderrSuppressed;

#pragma __message __save
#pragma __message __disable(INCARGTYP)

#pragma __pointer_size __save
#pragma __pointer_size __short
    typedef FarHandle::strdsc_t strdsc_t;

    strdsc_t iStdin, iStdout, iStderr;

    strdsc_t &rStdin  = iStdin;
    strdsc_t &rStdout = iStdout;
    strdsc_t &rStderr = iStderr;

    strdsc_t *const pStdin  = static_cast<strdsc_t*>(&iStdin );
    strdsc_t *const pStdout = static_cast<strdsc_t*>(&iStdout);
    strdsc_t *const pStderr = bStderrInUse ? static_cast<strdsc_t*>(&iStderr) : pStdout;

#pragma __pointer_size __restore
    rhProcessStdin .supply (rStdin);
    rhProcessStdout.supply (rStdout);
    if (bStderrInUse)
	rhProcessStderr.supply (rStderr);

#pragma __pointer_size __save
#pragma __pointer_size __short
    char *const pCommand32 = pCommand[0] ? _strdup32 (pCommand) : 0;

    char *const pLoginout = "sys$system:loginout.exe";
    char *const pImageName = pCommand32 && !bUsingCreateProcess ? pCommand32 : pLoginout;
    dsc32_descriptor_s iImageName (pImageName);

    int xStatus = SS$_NORMAL; unsigned int iPID;
    if (bUsingCreateProcess) {
	static int xCurrentPriority = 0; static unsigned int xNextCreation = 0;
	if (xCurrentPriority == 0) {
	    _generic_64 iTimeBase;
	    int xJPIValue = JPI$_LOGINTIM;
	    xStatus = lib$getjpi (&xJPIValue,NULL,NULL,&iTimeBase,NULL,NULL);

	    if ($VMS_STATUS_SUCCESS (xStatus)) {
		_generic_64 iTimeNow;
		xStatus = sys$gettim (&iTimeNow);
		xNextCreation = static_cast<unsigned int>((iTimeNow.gen64$q_quadword - iTimeBase.gen64$q_quadword) / 100000000);
	    }

	    if ($VMS_STATUS_SUCCESS (xStatus)) {
		xJPIValue = JPI$_PRIB;
		xStatus = lib$getjpi (&xJPIValue,NULL,NULL,&xCurrentPriority,NULL,NULL);
	    }
	}

	unsigned int const iFlags = bDetached ? PRC$M_DETACH : 0;

	if ($VMS_STATUS_SUCCESS (xStatus)) do {
	    char iProcessNameString[32];
	    sprintf (iProcessNameString, "V%08X%06X", xThisProcess, xNextCreation++);
	    dsc32_descriptor_s iProcessName (static_cast<char*>(iProcessNameString));

	    xStatus =  sys$creprc (
		static_cast<unsigned int*>(&iPID),		// PID Return
		static_cast<dsc32_descriptor_s*>(&iImageName),	// Image Name
		pStdin,						// Stdin
		pStdout,					// Stdout
		pStderr,					// Stderr
		0,						// Privilege
		0,						// Quota
		static_cast<dsc32_descriptor_s*>(&iProcessName),// Process Name
		xCurrentPriority,				// Priority
		0,						// UIC
		0,						// Termination Mailbox
		iFlags,						// Status Flag
		0,						// Item List
		0						// Node
	    );
	} while (SS$_DUPLNAM == xStatus);
    } else {
	unsigned int iFlags = CLI$M_TRUSTED + CLI$M_NOWAIT + (bDetached ? CLI$M_DETACHED : 0);
	xStatus = lib$spawn  (
	    &iImageName,		// Image Name
	    pStdin,			// Stdin
	    pStdout,			// Stdout
	    &iFlags,			// Flags
	    0,				// Process Name
	    &iPID,			// PID
	    0,				// Completion Status Address
	    0,				// Event Flag Number
	    0,				// AST Address
	    0,				// AST Argument
	    0,				// Prompt String
	    0,				// CLI
	    0				// Table
	);
    }
#pragma __pointer_size __restore
#pragma __message __restore
    if ($VMS_STATUS_SUCCESS (xStatus)) {
	rpProcess.setTo (new VProcess (iPID));
	rpProcess->setTerminateOnClosedStdio (bTerminatingOnClosedStdio);

	rhProcessStdin.markAsStdinOf (rpProcess, bUsingCreateProcess ? pCommand32 : 0);
	rhProcessStdout.markAsStdoutOf (rpProcess);
	if (bStderrInUse)
	    rhProcessStderr.markAsStderrOf (rpProcess);
	else
	    rpProcess->onClosedStderr ();
    }
    if (pCommand32)
	decc$free (pCommand32);

    return rStatus.MakeSuccessStatus (xStatus);
}

/******************************
 *****  Subprocess Pipes  *****
 ******************************/

/*-------------------*
 *----  OpenVMS  ----*
 *-------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer, // <-+
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    return supply (rStatus, ppOursideConsumer, rhFarsideProducer)
	&& supply (rStatus, rhFarsideConsumer, ppOursideProducer);
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer  // <-+
) {
    if (!ppOursideConsumer)
	return true;

    Handle hDevice;
    bool bDone = rStatus.MakeSuccessStatus (
#pragma __pointer_size __save
#pragma __pointer_size __short
	sys$crembx (
	    0,					// temporary mailbox
	    static_cast<Handle*>(&hDevice),	// channel return
	    4 * 4095,				// max message
	    4 * 4095,				// bufquo
	    0,					// protection
	    0,					// access
	    0,					// logical name
	    CMB$M_WRITEONLY			// channel access
	)
#pragma __pointer_size __restore
    ) && rhFarsideProducer.setFrom (rStatus, hDevice);
    if (bDone) {
	MailboxDevice::Data iDD (0, this, hDevice);
	rhFarsideProducer.linkTo (iDD.status ());
	ppOursideConsumer->setTo (new VBSConsumerDevice_<MailboxDevice>(iDD));
    }
    return bDone;
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    if (!ppOursideProducer)
	return true;

    Handle hDevice;
    bool bDone = rStatus.MakeSuccessStatus (
#pragma __pointer_size __save
#pragma __pointer_size __short
	sys$crembx (
	    0,					// temporary mailbox
	    static_cast<Handle*>(&hDevice),	// channel return
	    4 * 4095,				// max message
	    4 * 4095,				// bufquo
	    0,					// protection
	    0,					// access
	    0,					// logical name
	    CMB$M_READONLY			// channel access
	)
#pragma __pointer_size __restore
    ) && rhFarsideConsumer.setFrom (rStatus, hDevice);
    if (bDone) {
	MailboxDevice::Data iDD (0,this, hDevice);
	rhFarsideConsumer.linkTo (iDD.status ());
	ppOursideProducer->setTo (new VBSProducerDevice_<MailboxDevice>(iDD));
    }
    return bDone;
}


/****************************
 *****  Standard Pipes  *****
 ****************************/

/*---------------------*
 *-----  OpenVMS  -----*
 *---------------------*/

bool Vca::OS::DeviceManager::supplyStdToHere ( // stdin
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Try to do this using devices, ...
    static char aDeviceName[] = "sys$input";
    static $DESCRIPTOR (iDeviceName, aDeviceName);

    Handle hChannel;
    if (assign (rStatus, hChannel, iDeviceName)) {
	Device::Data iDD (0, this, hChannel);
	if (iDD.deviceClassUnderstood ()) {
	    rpDevice.setTo (new VBSProducerDevice_<Device> (iDD));

	//  ... redirect the original handle to the null device:
	    redirectToNull (stdin, O_RDONLY);

	    return rpDevice.isntNil ();
	}
    }

//  ... but, if that fails, use Unix (should be asychronous RMS) I/O :
    int hDevice = dup (fileno (stdin));
    FDIODevice iDD (0, hDevice, O_RDONLY);
    rpDevice.setTo (new VBSProducerDevice_<FDIODevice> (iDD));

//  ... redirect the original handle to the null device:
    redirectToNull (stdin, O_RDONLY);

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyStdToPeer ( // stdout
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Try to do this using devices and qio's, ...
    static char aDeviceName[] = "sys$output";
    static $DESCRIPTOR (iDeviceName, aDeviceName);

    Handle hChannel;
    if (assign (rStatus, hChannel, iDeviceName)) {
	Device::Data iDD (0, this, hChannel);
	if (iDD.deviceClassUnderstood ()) {
	    rpDevice.setTo (new VBSConsumerDevice_<Device> (iDD));

	//  ... redirect the original handle to the null device:
	    redirectToNull (stdout, O_WRONLY);

	    return rpDevice.isntNil ();
	}
    }

//  ... but, if that fails, use Unix (should be asychronous RMS) I/O :
    int hDevice = dup (fileno (stdout));
    FDIODevice iDD (0, hDevice, O_WRONLY);
    rpDevice.setTo (new VBSConsumerDevice_<FDIODevice> (iDD));

//  ... redirect the original handle to the null device:
    redirectToNull (stdout, O_WRONLY);

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyErrToPeer (
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Try to do this using devices and qio's, ...
    static char aDeviceName[] = "sys$error";
    static $DESCRIPTOR (iDeviceName, aDeviceName);

    Handle hChannel;
    if (assign (rStatus, hChannel, iDeviceName)) {
	Device::Data iDD (0, this, hChannel);
	if (iDD.deviceClassUnderstood ()) {
	    rpDevice.setTo (new VBSConsumerDevice_<Device> (iDD));

	//  ... redirect the original handle to the null device:
	    redirectToNull (stderr, O_WRONLY);

	    return rpDevice.isntNil ();
	}
    }

//  ... but, if that fails, use Unix (should be asychronous RMS) I/O :
    int hDevice = dup (fileno (stderr));
    FDIODevice iDD (0, hDevice, O_WRONLY);
    rpDevice.setTo (new VBSConsumerDevice_<FDIODevice> (iDD));

//  ... redirect the original handle to the null device:
    redirectToNull (stderr, O_WRONLY);

//  ... and return the device.
    return rpDevice.isntNil ();
}


/*********************
 *****  Sockets  *****
 *********************/

/*---------------------*
 *-----  OpenVMS  -----*
 *---------------------*/

/*>>>>DGRAM<<<<*/
bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay,
    bool			bPassive
) {
    //  Get the address, ...
    sockaddr_in iAddress;
    if (!rAddress.getAddress (iAddress))
	return rStatus.MakeFailureStatus ();

    //  ... create a socket, ...
    Socket iSocket;
    if (!iSocket.Create (rStatus, xType))
	return false;

    if (SOCK_STREAM == xType) {
    // connect should be asynchronous:
	if (bPassive) {
	    if (rAddress.specifiesPort ()) {
	    //  Set SO_REUSEADDR, errors not critical...
		VkStatus iStatus;
		iSocket.SetOptionValue (iStatus, SOL_SOCKET, SO_REUSEADDR, true);
	    }
	    if (!iSocket.Bind (rStatus, iAddress) || !iSocket.Listen (rStatus))
		return false;
	} else if (!iSocket.Connect (rStatus, iAddress))
	    return false;

    //  set no-delay socket option if appropriate
	if (bNoDelay) {
	// Setting TCP_NODELAY improves the latency of some multi-hop transactions, but isn't required, ...
	    VkStatus iStatus;
	    iSocket.SetNoDelay (iStatus);  // ... so we can safely errors we get here.
	}

    //  create the new device
	SocketDevice::Data iDD (0, this, iSocket);
	if (!bPassive)
	    rpDevice.setTo (new VBSProducerConsumerDevice_<SocketDevice>(iDD));
	else {
	    rpDevice.setTo (new VListeningDevice_<SocketDevice>(iDD));
	}
    }
    else if (SOCK_DGRAM == xType) {
	if (bPassive && !iSocket.Bind (rStatus, iAddress))
	    return false;

	DatagramSocket::Data iDD (0, this, iSocket);
	VDatagramDevice_<DatagramSocket>::Reference pDGMD (new VDatagramDevice_<DatagramSocket>(iDD));

	if (bPassive)
	    rpDevice.setTo (pDGMD);
	else {
	    pDGMD->supplyConnectionTo (rStatus, rpDevice, rAddress);
	}
    }
    else {
	rpDevice.clear ();
	return rStatus.MakeUnimplementedStatus ();
    }

    return rpDevice.isntNil ();
}


/*******************
 *****  Files  *****
 *******************/

/*******************
 *----  Input  ----*
 *******************/

/*---------------------*
 *-----  OpenVMS  -----*
 *---------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
) {
//  BS < file
    int hFile = open (rName, O_RDONLY);
    FDIODevice iDD (0, hFile, O_RDONLY);

    VDevice::Reference pDevice (new VBSProducerDevice_<FDIODevice> (iDD));
    return pDevice->supply (rpBS);
}

/********************
 *----  Output  ----*
 ********************/

/*---------------------*
 *-----  OpenVMS  -----*
 *---------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
) {
//  BS > file
    int hFile = open (rName, O_WRONLY);
    FDIODevice iDD (0, hFile, O_WRONLY);

    VDevice::Reference pDevice (new VBSConsumerDevice_<FDIODevice> (iDD));
    return pDevice->supply (rpBS);
}


#elif defined(_WIN32)

/*****************************
 *****************************
 *****                   *****
 *****  Vca::OS::Device  *****
 *****                   *****
 *****************************
 *****************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	typedef HANDLE Handle;

	class Device : public ManagedDevice {
	    DECLARE_FAMILY_MEMBERS (Device, ManagedDevice);

	//  Data
	public:
	    typedef ThisClass Data;

	//  Device Reference
	public:
	    typedef VReference<ThisClass> DeviceReference;

	//  Cursor
	public:
	    class Cursor : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Cursor, VReferenceable);

	    public:
		Cursor () : m_iOffset (0) {}
		Cursor (U64 iOffset) : m_iOffset (iOffset) {}

	    protected:
		~Cursor () {}

	    public:
		U64 offset () {
		    return m_iOffset;
		}
		void incrementOffset (U64 sTransfer) {
		    m_iOffset += sTransfer;
		}
		void initOVERLAPPED (OVERLAPPED &rOverlapped) {
		    *((U64*) &rOverlapped.Offset) = m_iOffset;
		}

	    protected:
		U64 m_iOffset;
	    };


	//  Basic Use
	public:
	    typedef BaseClass::ManagedUse ManagedUse;
	    class BasicUse : public ManagedUse {
		DECLARE_FAMILY_MEMBERS (BasicUse, ManagedUse);

	    //  Friends
		friend class OS::DeviceManager;

	    //  Overlapped
	    public:
		struct Overlapped : public OVERLAPPED {
		//  Construction
		public:
		    Overlapped () {
			Internal = InternalHigh = Offset = OffsetHigh = 0;
			hEvent = NULL;
		    }
		//  Destruction
		public:
		    ~Overlapped () {
		    }
		};

	    //  XOverlapped
	    public:
		struct XOverlapped : public Overlapped {
		    DECLARE_FAMILY_MEMBERS (XOverlapped, Overlapped);

		//  Construction
		public:
		    XOverlapped (BasicUse *pUse) : m_pUse (pUse) {
		    }

		//  Destruction
		public:
		    ~XOverlapped () {
		    }

		//  Access
		public:
		    BasicUse *use () const {
			return m_pUse;
		    }

		//  State
		private:
		    V::VPointer<BasicUse> const m_pUse;
		};

	    //  Construction
	    protected:
		BasicUse (VReferenceable *pContainer, Device *pDevice)
		    : BaseClass (pContainer), m_pDevice (pDevice), m_iOverlappedData (this)
		{
		}

	    //  Destruction
	    protected:
		~BasicUse () {
		}

	    //  Access
	    private:
		ManagedDevice *device_() const {
		    return device ();
		}
	    protected:
		Device *device () const {
		    return m_pDevice;
		}
		Handle handle () const {
		    return m_pDevice->operator Handle ();
		}
		Manager *manager () const {
		    return m_pDevice->manager ();
		}
		OVERLAPPED &overlappedData () {
		    return m_iOverlappedData;
		}
		OVERLAPPED const &overlappedData () const {
		    return m_iOverlappedData;
		}

	    //  Update
	    public:
		void setCursorTo (Cursor *pCursor) {
		    m_iCursor.setTo (pCursor);
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, BSReadArea const &rArea);
		bool start (VkStatus &rStatus, BSWriteArea const &rArea);

	    //  Callback
	    protected:
		//  Overlapped event processing done here:
		virtual void onEvent (VkStatus const &rStatus, size_t sTransfer);

	    protected:
		bool onData (DWORD sTransfer, bool bKeep = false) {
		    if (m_iCursor.isntNil ())
			m_iCursor->incrementOffset (sTransfer);
		    return BaseClass::onData (sTransfer, bKeep);
		}
		bool onData (VDevice *pDevice, bool bKeep = false) {
		    return BaseClass::onData (pDevice, bKeep);
		}


	    //  Event Posting
	    protected:
		void postCompletion (size_t sTransfer = 0) {
		    m_pDevice->postCompletion (this, sTransfer);
		}

	    //  State
	    protected:
		DeviceReference const m_pDevice;
		XOverlapped m_iOverlappedData;
		Cursor::Reference m_iCursor;
	    };
	    friend class BasicUse;

	public:
	    virtual Cursor* getReadCursor () const { return NULL; }
	    virtual Cursor* getWriteCursor () const { return NULL; }

	//  Threaded Use
	public:
	    class ThreadedUse : public BasicUse {
		DECLARE_FAMILY_MEMBERS (ThreadedUse, BasicUse);

	    //  Use Pointer
	    public:
		typedef Pointer UsePointer;

	    //  Worker
	    public:
		class Worker : public V::VManagedThread {
		    DECLARE_CONCRETE_RTTLITE (Worker, V::VManagedThread);

		    friend class ThreadedUse;

		//  Thread Procedure
		private:
		    pthread_procedure_result_t run_();

		//  Construction
		private:
		    Worker ();

		//  Destruction
		private:
		    ~Worker ();

		//  Access
		public:
		    bool isValid () const {
			return SystemHandleIsValid (m_hSemaphore) && BaseClass::isInitialized ();
		    }
		    bool isntValid () const {
			return !isValid ();
		    }

		//  Scheduling
		private:
		    static Reference g_pFreeListHead;

		    static bool Process (VkStatus &rStatus, ThreadedUse *pUse);
		    bool process (VkStatus &rStatus, ThreadedUse *pUse);

		    void recycle ();

		//  State
		protected:
		    HANDLE		m_hSemaphore;
		    Reference		m_pFreeListLink;
		    UsePointer		m_pUse;
		};
		typedef Worker::Reference WorkerReference;
		friend class Worker;

	    //  Construction
	    protected:
		ThreadedUse (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~ThreadedUse () {
		}

	    //  Access
	    public:
		VkStatus const &postedStatus () const {
		    return m_iPostedStatus;
		}

	    //  Startup
	    public:
		bool start (VkStatus &rStatus);

	    //  Execution
	    protected:
		void recycleWorker ();

	    private:
		virtual void run_() = 0;
		void run () {
		    run_();
		}

	    //  Callback
	    protected:
		//  Threaded event processing done here:
		void onEvent (VkStatus const &rStatus, size_t sTransfer);

	    //  Posting
	    protected:
		void postClosed () {
		    m_iPostedStatus.MakeClosedStatus ();
		    postCompletion ();
		}
		void postError () {
		    m_iPostedStatus.MakeErrorStatus ();
		    postCompletion ();
		}
		void postError (VkStatus const &rStatus) {
		    m_iPostedStatus.setTo (rStatus);
		    postCompletion ();
		}
		void postSuccess () {
		    //  ... used to post successful zero sized transfers:
		    m_iPostedStatus.MakeSuccessStatus ();
		    postCompletion ();
		}

	    //  State
	    private:
		WorkerReference	m_pWorker;
		VkStatus	m_iPostedStatus;
	    };
	    friend class ThreadedUse;

	//  ThreadedGet
	public:
	    class ThreadedGet : public ThreadedUse {
		DECLARE_FAMILY_MEMBERS (ThreadedGet, ThreadedUse);

	    //  Construction
	    protected:
		ThreadedGet (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~ThreadedGet () {
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, BSReadArea const &rArea);

	    //  Execution
	    private:
		void run_();

	    //  State
	    protected:
		BSReadArea m_iArea;
	    };

	//  ThreadedPut
	public:
	    class ThreadedPut : public ThreadedUse {
		DECLARE_FAMILY_MEMBERS (ThreadedPut, ThreadedUse);

	    //  Construction
	    protected:
		ThreadedPut (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~ThreadedPut () {
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, BSWriteArea const &rArea);

	    //  Execution
	    private:
		void run_();

	    //  State
	    protected:
		BSWriteArea m_iArea;
	    };

	//  Construction
	public:
	    Device (VReferenceable *pContainer, Manager *pManager, Handle &rhDevice)
		: BaseClass	(pContainer, pManager)
		, m_hDevice	(grab (rhDevice))
		, m_xDeviceType	(GetFileType(m_hDevice))
		, m_bRegistered	(false)
	    {
	    }
	    Device (VReferenceable *pContainer, ThisClass &rOther)
		: BaseClass	(pContainer, rOther)
		, m_hDevice	(rOther.grab ())
		, m_xDeviceType	(rOther.grabType ())
		, m_bRegistered	(false)
	    {
	    }
	    Device (VReferenceable *pContainer, Manager *pManager) : BaseClass (pContainer, pManager), m_bRegistered (false) {
		grab	 (m_hDevice);
		grabType (m_xDeviceType);
	    }

	//  Destruction
	public:
	    ~Device () {
		close ();
	    }

	//  Access
	public:
	    operator Handle () const {
		return m_hDevice;
	    }
	    DWORD deviceType () const {
		return m_xDeviceType;
	    }
	    Handle grab () {
		return grab (m_hDevice);
	    }
	    static Handle grab (Handle &rhDevice) {
		Handle hDevice = rhDevice;
		rhDevice = INVALID_HANDLE_VALUE;
		return hDevice;
	    }
	private:
	    DWORD grabType () {
		return grabType (m_xDeviceType);
	    }
	    static DWORD grabType (DWORD &rxType) {
		DWORD xType = rxType;
		rxType = FILE_TYPE_UNKNOWN;
		return xType;
	    }

	//  Update
	public:
	    void claim (Handle &rhDevice) {
		close ();
		m_hDevice = grab (rhDevice);
	    }
	    void claim (Device &rOther) {
		close ();
		m_hDevice = rOther.grab ();
	    }
	    void close () {
		close (m_hDevice);
	    }
	    static void close (Handle &rhDevice) {
		if (FileHandleIsValid (rhDevice)) {
		    CloseHandle (grab (rhDevice));
		}
	    }

	//  Device Classification
	public:
	    bool isACharacterDevice () const {
		return m_xDeviceType == FILE_TYPE_CHAR;
	    }
	    bool isADiskFile () const {
		return m_xDeviceType == FILE_TYPE_DISK;
	    }
	    bool isAPipeOrSocket () const {
		return m_xDeviceType == FILE_TYPE_PIPE;
	    }

	//  Device Control
	public:
	    bool makeDeviceFriendly () {
		return makePrivate ();
	    }
	    bool makePublic () {
		return setInheritabilityTo (true);
	    }
	    bool makePrivate () {
		return setInheritabilityTo (false);
	    }
	    bool setInheritabilityTo (bool bInheritable);
	    bool supplyDuplicate (Handle &rhDuplicate) const;
	    bool supplyDuplicate (Handle &rhDuplicate, bool bInheritable) const;

	//  Device Registration
	protected:
	    bool registerDevice ();

	//  Device Use
//	protected:
//	    using BaseClass::incrementUseCount;
	public:
	    bool doAccept (VkStatus &rStatus, SOCKET &rhSocket);

	    bool doRead (VkStatus &rStatus, DWORD &rsTransfer, BSReadArea const &rArea);
	    bool doRead (
		VkStatus &rStatus, DWORD &rsTransfer, BSReadArea const &rArea, OVERLAPPED &rOverlapped
	    );
	    bool doRead (
		VkStatus &rStatus, DWORD &rsTransfer, WSABUF &rArea, OVERLAPPED &rOverlapped
	    );

	    bool doWrite (VkStatus &rStatus, DWORD &rsTransfer, BSWriteArea const &rArea);
	    bool doWrite (
		VkStatus &rStatus, DWORD &rsTransfer, BSWriteArea const &rArea, OVERLAPPED &rOverlapped
	    );
	    bool doWrite (
		VkStatus &rStatus, DWORD &rsTransfer, WSABUF &rArea, OVERLAPPED &rOverlapped
	    );

	//  Event Posting
	protected:
	    void postCompletion (BasicUse *pUse, size_t sTransfer = 0);

	//  State
	protected:
	    Handle	m_hDevice;
	    DWORD	m_xDeviceType;
	    bool	m_bRegistered;
	};

	//  Far Handle
	typedef Device FarHandle;
    }
}


/****************************
 ****************************
 *****  Device Control  *****
 ****************************
 ****************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::setInheritabilityTo (bool bInheritable) {
    DWORD iFlags;
    if (!GetHandleInformation (m_hDevice, &iFlags))
	return false;

    bool bInheritableNow = (iFlags & HANDLE_FLAG_INHERIT) ? true : false;
    if (bInheritableNow == bInheritable)
	return true;

    HANDLE hDuplicate;
    if (!supplyDuplicate (hDuplicate, bInheritable))
	return false;

    claim (hDuplicate);

    return true;
}

bool Vca::OS::Device::supplyDuplicate (Handle &rhDuplicate) const {
    DWORD iFlags;
    return GetHandleInformation (m_hDevice, &iFlags) && supplyDuplicate (
	rhDuplicate, (iFlags & HANDLE_FLAG_INHERIT) ? true : false
    );
}

bool Vca::OS::Device::supplyDuplicate (Handle &rhDuplicate, bool bInheritable) const {
    HANDLE hProcess = GetCurrentProcess ();
    return DuplicateHandle (
	hProcess, m_hDevice, hProcess, &rhDuplicate, 0, bInheritable, DUPLICATE_SAME_ACCESS
    ) ? true : false;
}


/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::doAccept (VkStatus &rStatus, SOCKET &rhSocket) {
    rhSocket = accept ((SOCKET)m_hDevice, 0, 0);
    return SocketHandleIsValid (rhSocket) || rStatus.MakeErrorStatus ();
}

bool Vca::OS::Device::doRead (
    VkStatus &rStatus, DWORD &rsTransfer, BSReadArea const &rArea
) {
    return ReadFile (m_hDevice, rArea.base (), rArea.size (), &rsTransfer, 0)
	|| rStatus.MakeErrorStatus ();
}

bool Vca::OS::Device::doRead (
    VkStatus &rStatus, DWORD &rsTransfer, BSReadArea const &rArea, OVERLAPPED &rOverlapped
) {
    registerDevice ();
    incrementUseCount ();
    return ReadFile (m_hDevice, rArea.base (), rArea.size (), &rsTransfer, &rOverlapped)
	|| rStatus.MakeErrorStatus ();
}

bool Vca::OS::Device::doRead (
    VkStatus &rStatus, DWORD &rsTransfer, WSABUF &rArea, OVERLAPPED &rOverlapped
) {
    registerDevice ();
    incrementUseCount ();
    DWORD iFlags = 0;
    bool bResult = WSARecv((SOCKET)m_hDevice, &rArea, 1, &rsTransfer, &iFlags, &rOverlapped, 0) == 0
	|| rStatus.MakeErrorStatus (WSAGetLastError ());
    return bResult;
}

bool Vca::OS::Device::doWrite (
    VkStatus &rStatus, DWORD &rsTransfer, BSWriteArea const &rArea
) {
/********************************************************************
 *>>>>	Synchronous (Console) WriteFile must be limited to 32k  <<<<*
 *>>>>	to prevent ERROR_NOT_ENOUGH_MEMORY errors.		<<<<*
 ********************************************************************/
    DWORD sArea = rArea.size ();
    if (sArea > 0x8000 && isACharacterDevice ())
	sArea = 0x8000;

    return WriteFile (m_hDevice, rArea.base (), sArea, &rsTransfer, 0)
	|| rStatus.MakeErrorStatus ();
}

bool Vca::OS::Device::doWrite (
    VkStatus &rStatus, DWORD &rsTransfer, BSWriteArea const &rArea, OVERLAPPED &rOverlapped
) {
    registerDevice ();
    incrementUseCount ();
    return WriteFile (m_hDevice, rArea.base (), rArea.size (), &rsTransfer, &rOverlapped)
	|| rStatus.MakeErrorStatus ();
}

bool Vca::OS::Device::doWrite (
    VkStatus &rStatus, DWORD &rsTransfer, WSABUF &rArea, OVERLAPPED &rOverlapped
) {
    registerDevice ();
    incrementUseCount ();
    return WSASend((SOCKET)m_hDevice, &rArea, 1, &rsTransfer, 0, &rOverlapped, 0) == 0
	|| rStatus.MakeErrorStatus (WSAGetLastError ());
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::OS::Device::BasicUse  *****
 *****                             *****
 ***************************************
 ***************************************/

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool Vca::OS::Device::BasicUse::start (VkStatus &rStatus, BSReadArea const &rArea) {
    DWORD sTransfer = 0;

    //  get the cursor if any and store it
    if (m_pDevice->isADiskFile ()) {
	Cursor::Reference pCursor (m_pDevice->getReadCursor ());
	setCursorTo (pCursor);
	pCursor->initOVERLAPPED (m_iOverlappedData);
    }

    if (!m_pDevice->doRead (rStatus, sTransfer, rArea, m_iOverlappedData))
	return onError (rStatus);

    if (rStatus.isBlocked ())
	return true;

    if (sTransfer> 0)
	return onData (sTransfer, true);

    rStatus.MakeClosedStatus ();
    return onError (rStatus, true);
}

bool Vca::OS::Device::BasicUse::start (VkStatus &rStatus, BSWriteArea const &rArea) {
    DWORD sTransfer = 0;

    //  get the cursor if any and store it
    if (m_pDevice->isADiskFile ()) {
	VReference <Cursor> pCursor (m_pDevice->getWriteCursor ());
	setCursorTo (pCursor);
	pCursor->initOVERLAPPED (m_iOverlappedData);
    }

    return !m_pDevice->doWrite (
	rStatus, sTransfer, rArea, m_iOverlappedData
    ) ? onError (rStatus) : rStatus.isBlocked () || onData (sTransfer, true);
}


/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::Device::BasicUse::onEvent (
    VkStatus const &rStatus, size_t sTransfer
) {
//  Generate the appropriate notifications:
    bool bTriggeringUser = sTransfer > 0 || rStatus.isCompleted ()
	? onData (sTransfer) : onError (rStatus);
    if (bTriggeringUser)
	triggerUser ();
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::OS::Device::ThreadedUse  *****
 *****                                *****
 ******************************************
 ******************************************/

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::ThreadedUse::start (VkStatus &rStatus) {
    m_pDevice->incrementUseCount ();
    return Worker::Process (rStatus, this);
}

void Vca::OS::Device::ThreadedUse::recycleWorker () {
//  Recycle the worker that did the work...
    m_pWorker->recycle ();
    m_pWorker.clear ();
}

/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::Device::ThreadedUse::onEvent (
    VkStatus const &rStatus, size_t sTransfer
) {
//  Recycle the worker that did the work...
    recycleWorker ();

//  ... and generate the appropriate notifications:
    bool bTriggeringUser = sTransfer > 0 || m_iPostedStatus.isCompleted ()
	? onData (sTransfer) : onError (m_iPostedStatus);
    if (bTriggeringUser)
	triggerUser ();
}


/**************************************************
 **************************************************
 *****                                        *****
 *****  Vca::OS::Device::ThreadedUse::Worker  *****
 *****                                        *****
 **************************************************
 **************************************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	Device::ThreadedUse::Worker::Reference Device::ThreadedUse::Worker::g_pFreeListHead;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

Vca::OS::Device::ThreadedUse::Worker::Worker () : m_hSemaphore (CreateSemaphore (NULL, 0, 1, NULL)) {
    retain (); {
	start ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

Vca::OS::Device::ThreadedUse::Worker::~Worker () {
    CloseHandle (m_hSemaphore);
}


/******************************
 ******************************
 *****  Thread Procedure  *****
 ******************************
 ******************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

pthread_procedure_result_t Vca::OS::Device::ThreadedUse::Worker::run_() {
    bool bNotDone = true;
    while (bNotDone) {
	switch (WaitForSingleObject (m_hSemaphore, INFINITE)) {
	case WAIT_OBJECT_0:
	    //  If there's work to be done, get it done:
	    if (m_pUse.isntNil ()) {

	/************************************************************
	 *----  Failure to clear (claim) m_pUse before calling  ----*
	 *----  'run' results in a race with the main thread.   ----*
	 ************************************************************/
		UsePointer pThisUse;
		pThisUse.claim (m_pUse);
		pThisUse->run ();
	    }
	    break;
	case WAIT_ABANDONED:
	    //  Broken mutex.  Abandon ship:
	    bNotDone = false;
	    break;
	case WAIT_TIMEOUT:
	    //  Time's up!!! (see above)
	    break;
	default:
	    //  Now what?  Abandon hope:
	    bNotDone = false;
	    break;
	}
    }
    CloseHandle (m_hSemaphore);
    m_hSemaphore = NULL;
    return 0;
}


/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::ThreadedUse::Worker::Process (VkStatus &rStatus, ThreadedUse *pUse) {
    WorkerReference pWorker;

//  Try to find an existing worker to do the job, ...
    do {
	pWorker.claim (g_pFreeListHead);
	if (pWorker.isntNil ())
	    g_pFreeListHead.claim (pWorker->m_pFreeListLink);
    } while (pWorker.isntNil () && !pWorker->process (rStatus, pUse));

    if (pWorker.isntNil ())
	return true;

//  ... but if none is available, create and try a new one:
    pWorker.setTo (new Worker ());
    return pWorker->isntValid ()
	? rStatus.MakeErrorStatus ()
	: rStatus.MakeSuccessStatus () && pWorker->process (rStatus, pUse);
}

bool Vca::OS::Device::ThreadedUse::Worker::process (VkStatus &rStatus, ThreadedUse *pUse) {
//  Register the activity, ...
    pUse->m_pWorker.setTo (this);
    m_pUse.setTo (pUse);

//  ... release the thread's semaphore, ...
    if (ReleaseSemaphore (m_hSemaphore, 1, NULL))
	return true;

//  ... or note that it couldn't be released, ...
    rStatus.MakeErrorStatus ();

//  ... reverse the activity registration, ...
    m_pUse.clear ();
    pUse->m_pWorker.clear ();

//  ... stop the worker's thread, ...
    stop ();

//  ... and return a failure indication:
    return false;
}

void Vca::OS::Device::ThreadedUse::Worker::recycle () {
    m_pFreeListLink.claim (g_pFreeListHead);
    g_pFreeListHead.setTo (this);
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::OS::Device::ThreadedGet  *****
 *****                                *****
 ******************************************
 ******************************************/

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::ThreadedGet::start (
    VkStatus &rStatus, BSReadArea const &rArea
) {
    m_iArea.setTo (rArea);
    return BaseClass::start (rStatus);
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::Device::ThreadedGet::run_() {
    VkStatus iStatus; DWORD sTransfer = 0;
    if (!m_pDevice->doRead (iStatus, sTransfer, m_iArea))
	postError (iStatus);

    else if (sTransfer > 0)
	postCompletion (sTransfer);

    /*-----------------------------------------------------------------------*
     *  ERROR_OPERATION_ABORTED is returned in response to Control-C/Break.  *
     *  It isn't an error in those cases...                                  *
     *-----------------------------------------------------------------------*/
    else if (iStatus.Code () == ERROR_OPERATION_ABORTED)
	postSuccess ();

    else
	postClosed ();
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::OS::Device::ThreadedPut  *****
 *****                                *****
 ******************************************
 ******************************************/

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::Device::ThreadedPut::start (
    VkStatus &rStatus, BSWriteArea const &rArea
) {
    m_iArea.setTo (rArea);
    return BaseClass::start (rStatus);
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::Device::ThreadedPut::run_() {
    VkStatus iStatus; DWORD sTransfer = 0;
    if (m_pDevice->doWrite (iStatus, sTransfer, m_iArea))
	postCompletion (sTransfer);
    else
	postError (iStatus);
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::OS::OverlappedDevice  *****
 *****                             *****
 ***************************************
 ***************************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	class OverlappedDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (OverlappedDevice, Device);

	//  Uses
	public:
	    typedef BasicUse BSGet;
	    typedef BasicUse BSPut;

	//  Construction
	public:
	    OverlappedDevice (
		VReferenceable *pContainer, Manager *pManager, Handle &rhDevice
	    ) : BaseClass (pContainer, pManager, rhDevice), m_iReadCursor (new Cursor (0)), m_iWriteCursor (new Cursor (0)) {
	    }
	    OverlappedDevice (VReferenceable *pContainer, ThisClass &rOther) 
		: BaseClass (pContainer, rOther), m_iReadCursor (new Cursor (0)), m_iWriteCursor (new Cursor (0)) {
	    }
	    OverlappedDevice (VReferenceable *pContainer, Data &rDeviceData)
		: BaseClass (pContainer, rDeviceData), m_iReadCursor (new Cursor (0)), m_iWriteCursor (new Cursor (0)) {
	    }

	//  Destruction
	public:
	    ~OverlappedDevice () {
	    }

	//  Access
	public:
	    Cursor *getReadCursor () const {
		return m_iReadCursor;
	    }
	    Cursor *getWriteCursor () const {
		return m_iWriteCursor;
	    }

	//  State
	protected:

	    VReference <Cursor> m_iReadCursor;
	    VReference <Cursor> m_iWriteCursor;
	};
    }
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::OS::SocketDevice  *****
 *****                         *****
 ***********************************
 ***********************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	class SocketDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (SocketDevice, Device);

	//  Handle
	public:
	    typedef SOCKET Socket;

	//  Data
	public:
	    typedef ThisClass Data;

	//  AcceptUse
	public:
	    class AcceptUse : public Device::ThreadedUse {
		DECLARE_FAMILY_MEMBERS (AcceptUse, Device::ThreadedUse);

	    //  Construction
		AcceptUse (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
		~AcceptUse () {
		}

	    //  Access
	    protected:
		SOCKET handle () const {
		    return (SOCKET)BaseClass::handle ();
		}

	    //  Execution
	    private:
		void run_();

	    //  Callback
	    protected:
		//  Threaded event processing done here:
		void onEvent (VkStatus const &rStatus, size_t sTransfer);

	    //  State
	    protected:
		SOCKET m_hNewSocket;
	    };

	//  BSMove
	public:
	    class BSMove : public Device::BasicUse {
		DECLARE_FAMILY_MEMBERS (BSMove, Device::BasicUse);

	    //  Construction
	    protected:
		BSMove (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSMove () {
		}

	    //  Access
	    protected:
		SOCKET handle () const {
		    return (SOCKET)BaseClass::handle ();
		}

	    //  Startup
	    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
		bool start (VkStatus &rStatus, BSReadArea const &rArea);
		bool start (VkStatus &rStatus, BSWriteArea const &rArea);

	    //  Callback
	    protected:
		//  Overlapped event processing done here:
		void onEvent (VkStatus const &rStatus, size_t sTransfer);

	    //  State
	    protected:
		WSABUF m_iWSABUF;
	    };
	    typedef BSMove BSGet;
	    typedef BSMove BSPut;

	//  Construction
	public:
	    SocketDevice (
		VReferenceable *pContainer, Manager *pManager, Socket &rhSocket
	    ) : BaseClass (pContainer, pManager, *(Handle*)&rhSocket) {
	    }
	    SocketDevice (VReferenceable *pContainer, SocketDevice &rOther) : BaseClass (pContainer, rOther) {
	    }

	//  Destruction
	public:
	    ~SocketDevice () {
		close ();
	    }

	//  Access
	public:
	    operator Socket () const {
		return reinterpret_cast<Socket>(operator Handle ());
	    }
	    bool getName (VkStatus &rStatus, VString &rName) const;
            bool getSocket (VkStatus &rStatus, SOCKET& rSocket) const;

	//  Update
	public:
	    void claim (Socket &rhSocket) {
		close ();
		BaseClass::claim (*(Handle*)&rhSocket);
	    }
	    void claim (ThisClass &rOther) {
		close ();
		BaseClass::claim (rOther);
	    }
	    void close () {
		Socket hSocket = grab ();
		close (hSocket);
	    }
	    static void close (Socket &rhSocket) {
		if (SocketHandleIsValid (rhSocket)) {
		    closesocket (grab (rhSocket));
		}
	    }

	    Socket grab () {
		return reinterpret_cast<Socket>(BaseClass::grab ());
	    }
	    static Socket grab (Socket &rhSocket) {
		Socket hSocket = rhSocket;
		rhSocket = INVALID_SOCKET;
		return hSocket;
	    }

	//  User Accounting
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    void onFirstReader () {
		ReportUser ("first socket reader", ++s_cSocketReaders);
		BaseClass::onFirstReader ();
	    }
	    void onFirstWriter () {
		ReportUser ("first socket writer", ++s_cSocketWriters);
		BaseClass::onFirstWriter ();
	    }
	    void onFinalReader () {
		ReportUser ("final socket reader", --s_cSocketReaders, shutdown (operator Socket(), SD_RECEIVE));
		BaseClass::onFinalReader ();
	    }
	    void onFinalWriter () {
		ReportUser ("final socket writer", --s_cSocketWriters, shutdown (operator Socket(), SD_SEND));
		BaseClass::onFinalWriter ();
	    }
	    void onFinalListener () {
		close ();
		BaseClass::onFinalListener ();
	    }
	};
    }
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::SocketDevice::getName (VkStatus &rStatus, VString &rName) const {
    VkSocketAddress iAddress;
    return iAddress.GetSocketName (operator Socket (), &rStatus) >= 0 && iAddress.getName (rName);
}

bool Vca::OS::SocketDevice::getSocket (VkStatus &rStatus, SOCKET& rSocket) const {
    rSocket = operator Socket ();
    return true;
}



/**********************************************
 **********************************************
 *****                                    *****
 *****  Vca::OS::SocketDevice::AcceptUse  *****
 *****                                    *****
 **********************************************
 **********************************************/

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::SocketDevice::AcceptUse::run_() {
    VkStatus iStatus;
    if (m_pDevice->doAccept (iStatus, m_hNewSocket))
	postCompletion ();
    else
	postError ();
}

/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::SocketDevice::AcceptUse::onEvent (
    VkStatus const &rStatus, size_t sTransfer
) {
//  Recycle the worker that did the work...
    recycleWorker ();

    bool bTriggeringUser = false;
    if (SocketHandleIsInvalid (m_hNewSocket))
	bTriggeringUser = onError (postedStatus ());
    else {
	//  TO DO: copy socket options
	SocketDevice::Data iDD (0, manager (), m_hNewSocket);
	VDevice::Reference pDevice (new VBSProducerConsumerDevice_<SocketDevice>(iDD));
	bTriggeringUser = onData (pDevice);
    }
    if (bTriggeringUser)
	triggerUser ();
}


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vca::OS::SocketDevice::BSMove  *****
 *****                                 *****
 *******************************************
 *******************************************/

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

bool Vca::OS::SocketDevice::BSMove::start (
    VkStatus &rStatus, BSReadArea const &rArea
) {
    m_iWSABUF.buf = (char*)rArea.base ();
    m_iWSABUF.len = rArea.size ();
    DWORD sTransfer = 0;
    if (!m_pDevice->doRead (rStatus, sTransfer, m_iWSABUF, m_iOverlappedData))
	return onError (rStatus);

    if (rStatus.isBlocked ())
	return true;

    if (sTransfer > 0)
	return onData (sTransfer, true);

    rStatus.MakeClosedStatus ();
    return onError (rStatus, true);
}

bool Vca::OS::SocketDevice::BSMove::start (
    VkStatus &rStatus, BSWriteArea const &rArea
) {
    m_iWSABUF.buf = (char*)rArea.base ();
    m_iWSABUF.len = rArea.size ();
    DWORD sTransfer = 0;
    return !m_pDevice->doWrite (
	rStatus, sTransfer, m_iWSABUF, m_iOverlappedData
    ) ? onError (rStatus) : rStatus.isBlocked () || onData (sTransfer, true);
}

void Vca::OS::SocketDevice::BSMove::onEvent (
    VkStatus const &rStatus, size_t sTransfer
) {
/****************************************************************************
 * An obscure bit of Windows lore described on the Web and supposedly
 * somewhere to be found in one of the 'Returns' sections of the Win32
 * documentation:
 *
 *  If a socket handle associated with a completion port is closed,
 *  GetQueuedCompletionStatus returns ERROR_SUCCESS, with *lpOverlaped
 *  non-NULL and lpNumberOfBytes equal to zero. 
 *
 * The code that follows is, of course, a socket.  The other conditions were
 * satisfied to get us here in the first place.
 *
 *****************************************************************************/
    VkStatus iStatus (rStatus);
    if (sTransfer == 0 && iStatus.isCompleted ()) {
	iStatus.MakeClosedStatus ();
    }
    BaseClass::onEvent (iStatus, sTransfer);
}


/************************************
 ************************************
 *****  Datagram Socket Device  *****
 ************************************
 ************************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	class DatagramSocket : public SocketDevice {
	    DECLARE_FAMILY_MEMBERS (DatagramSocket, SocketDevice);

	//  Data
	public:
	    typedef ThisClass Data;

	//  BSGet
	public:
	    typedef BaseClass::BSGet BSGetBase;
	    class BSGet : public BSGetBase {
		DECLARE_FAMILY_MEMBERS (BSGet, BSGetBase);

	    //  Construction
	    protected:
		BSGet (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSGet () {
		}

	    //  Access
	    public:
	    };

	//  BSPut
	public:
	    typedef BaseClass::BSPut BSPutBase;
	    class BSPut : public BSPutBase {
		DECLARE_FAMILY_MEMBERS (BSPut, BSPutBase);

	    //  Construction
	    protected:
		BSPut (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSPut () {
		}

	    //  Access
	    public:
	    };

	//  Construction
	public:
	    DatagramSocket (VReferenceable *pContainer, Manager *pManager, Socket &rhSocket)
		: BaseClass (pContainer, pManager, rhSocket)
	    {
	    }
	    DatagramSocket (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther) {
	    }

	//  Destruction
	public:
	    ~DatagramSocket () {
	    }

	};
    }
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::OS::ThreadedDevice  *****
 *****                           *****
 *************************************
 *************************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	class ThreadedDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (ThreadedDevice, Device);

	//  Uses
	public:
	    typedef ThreadedGet BSGet;
	    typedef ThreadedPut BSPut;

	//  Construction
	protected:
	    ThreadedDevice (VReferenceable *pContainer, Manager *pManager, Handle &rhDevice)
		: BaseClass (pContainer, pManager, rhDevice)
	    {
	    }
	    ThreadedDevice (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther) {
	    }
	    ThreadedDevice (VReferenceable *pContainer, Data &rDeviceData) : BaseClass (pContainer, rDeviceData) {
	    }

	//  Destruction
	protected:
	    ~ThreadedDevice () {
	    }
	};
    }
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::OS::DeviceManager  *****
 *****                          *****
 ************************************
 ************************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

namespace Vca {
    namespace OS {
	class DeviceManager : public VDeviceManager {
	    DECLARE_CONCRETE_RTTLITE (DeviceManager, VDeviceManager);

	//  Aliases
	public:
	    typedef Device::BasicUse BasicUse;
	    typedef BasicUse::XOverlapped XOverlapped;

	//  Construction
	public:
	    DeviceManager (VCohort *pCohort) : BaseClass (pCohort), m_hPort (
		CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL, 0, 0)
	    ) {
		joinCohort ();
	    }

	//  Destruction
	private:
	    ~DeviceManager () {
		exitCohort ();

		CloseHandle (m_hPort);
	    }

	//  Device Registration
	public:
	    bool registerDevice (Device *pDevice);

	//  Event Processing
	public:
	    bool postCompletion (BasicUse *pUse, size_t sTransfer);
	private:
	    virtual /*override*/ bool postInterrupt_();
	    virtual /*override*/ bool processEvent_(size_t sTimeout);

	//  Suppliers
	public:
	    //  ... subprocess
	    bool supply (
		VkStatus&		rStatus,
		VProcess::Reference&	rpProcess,
		FarHandle&		rhProcessStdin,
		FarHandle&		rhProcessStdout,
		FarHandle&		rhProcessStderr, // & == &rhProcessStdout -> stdout device device
		char const*		pCommand
	    );

	    //  ... subprocesses pipes
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+	(null -> null device)
		FarHandle&		rhFarsideProducer, // <-+
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+	(null -> null device)
	    );
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+	(null -> null device)
		FarHandle&		rhFarsideProducer  // <-+
	    );
	    bool supply (
		VkStatus&		rStatus,
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+	(null -> null device)
	    );

	    //  ... standard pipes
	    bool supplyStdToHere (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdin
	    );
	    bool supplyStdToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdout
	    );
	    bool supplyErrToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stderr
	    );

	    //  ... sockets
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference&	rpDevice,
		VkSocketAddress const&	rAddress,
		int			xProtocol,
		int			xType,
		bool			bNoDelay,
		bool			bPassive
	    );

	    //  ... files
	    bool supply (
		VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
	    );
	    bool supply (
		VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
	    );

	//  State
	private:
	    HANDLE const m_hPort;
	};
    }
}


/*********************************
 *********************************
 *****  Device Registration  *****
 *********************************
 *********************************/

bool Vca::OS::Device::registerDevice () {
    return m_bRegistered || (m_bRegistered = m_pManager->registerDevice (this));
}

bool Vca::OS::DeviceManager::registerDevice (Device *pDevice) {
    return CreateIoCompletionPort (
	pDevice->operator Handle (), m_hPort, (ULONG_PTR)pDevice, 0
    ) != NULL;
}

/***************************
 ***************************
 *****  Event Posting  *****
 ***************************
 ***************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

void Vca::OS::Device::postCompletion (BasicUse *pUse, size_t sTransfer) {
    m_pManager->postCompletion (pUse, sTransfer);
}

bool Vca::OS::DeviceManager::postCompletion (BasicUse *pUse, size_t sTransfer) {
    return PostQueuedCompletionStatus (
	m_hPort, sTransfer, (ULONG_PTR)pUse->device (), &pUse->overlappedData ()
    ) ? true : false;
}

bool Vca::OS::DeviceManager::postInterrupt_() {
    return PostQueuedCompletionStatus (m_hPort, 0, 0, 0) ? true : false;
}

/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/*-----------------*
 *----  Win32  ----*
 *-----------------*/

//  Returns true if at an event was processed, false otherwise:
bool Vca::OS::DeviceManager::processEvent_(
    size_t sTimeout /* in milliseconds */
) {
    OVERLAPPED iOverlapped;
    DWORD sTransfer = 0;
    Device *pDevice = 0; LPOVERLAPPED pOverlapped = &iOverlapped;

    VkStatus iStatus;
    if (!GetQueuedCompletionStatus (m_hPort, &sTransfer, (PULONG_PTR)&pDevice, &pOverlapped, sTimeout)) {
	iStatus.MakeErrorStatus ();
	if (iStatus.Code () == WAIT_TIMEOUT)
	    return false;
    }
    if (pOverlapped && pOverlapped != &iOverlapped) {
	BasicUse *pUse = static_cast<XOverlapped*>(pOverlapped)->use ();
	pUse->onEvent (iStatus, sTransfer);
    }
    return true;
}


/********************************
 ********************************
 *****  Resource Suppliers  *****
 ********************************
 ********************************/

/************************
 *****  Subprocess  *****
 ************************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

/**
 * @detail
 * Process creation flags may be provided at the beginning of the command string. The set of flags provided must be enclosed in curly braces. The following flags are recognized on Windows:
 *  - b: @c CREATE_BREAKAWAY_FROM_JOB
 *  - c: @c CREATE_NEW_CONSOLE
 *  - g: @c CREATE_NEW_PROCESS_GROUP
 *  - w: @c CREATE_NO_WINDOW
 *  - a: @c CREATE_PRESERVE_CODE_AUTHZ_LEVEL
 *  - s: @c CREATE_SUSPENDED
 *  - u: @c CREATE_UNICODE_ENVIRONMENT
 *  - d: @c DEBUG_PROCESS
 *  - o: @c DEBUG_ONLY_THIS_PROCESS
 *  - e: @c DETACHED_PROCESS
 */
bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    FarHandle&			rhProcessStdin,
    FarHandle&			rhProcessStdout,
    FarHandle&			rhProcessStderr, // & == &rhProcessStdout -> stdout device device
    char const*			pCommand
) {
    DWORD dwCreationFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SEPARATE_WOW_VDM; // removing DETACHED_PROCESS eliminated undesired console creations by some spawned processes (7/98)
//  Parse the command, ...
    bool bProcessingOptions = pCommand && '{' == pCommand[0];
    while (bProcessingOptions) {
	switch (*++pCommand) {
        case 'b':
            dwCreationFlags |= CREATE_BREAKAWAY_FROM_JOB;
            break;
        case 'c':
            dwCreationFlags |= CREATE_NEW_CONSOLE;
            break;
        case 'g':
            dwCreationFlags |= CREATE_NEW_PROCESS_GROUP;
            break;
        case 'w':
            dwCreationFlags |= CREATE_NO_WINDOW;
            break;
        case 'a':
            dwCreationFlags |= CREATE_PRESERVE_CODE_AUTHZ_LEVEL;
            break;
        case 's':
            dwCreationFlags |= CREATE_SUSPENDED;
            break;
        case 'u':
            dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
            break;
        case 'd':
            dwCreationFlags |= DEBUG_PROCESS;
            break;
        case 'o':
            dwCreationFlags |= DEBUG_ONLY_THIS_PROCESS;
            break;
        case 'e':
            dwCreationFlags |= DETACHED_PROCESS;
            break;
	case '}':
	    bProcessingOptions = false;
	    pCommand++;
	    break;
	case '\0':
	    bProcessingOptions = false;
	    break;
	}
    }
    pCommand += strspn (pCommand, " \t");

//  Determine if stderr needs to be created ...
    bool const bCreatingStderr = &rhProcessStderr != &rhProcessStdout;

//  Make the farside handles inheritable, ...
    if (!rhProcessStdin.makePublic ())
	return false;
    if (!rhProcessStdout.makePublic ())
	return false;
    if (bCreatingStderr && !rhProcessStderr.makePublic ())
	return false;

//  ... create a STARTUPINFO structure for CreateProcess, ...
    STARTUPINFO iStartupInfo;
    iStartupInfo.cb		= sizeof (STARTUPINFO);
    iStartupInfo.lpReserved	= NULL;
    iStartupInfo.lpDesktop	= NULL;
    iStartupInfo.lpTitle	= NULL;
    iStartupInfo.dwFlags	= STARTF_USESTDHANDLES;
    iStartupInfo.cbReserved2	= 0;
    iStartupInfo.lpReserved2	= NULL;

    iStartupInfo.hStdInput  = rhProcessStdin .grab ();
    iStartupInfo.hStdOutput = rhProcessStdout.grab ();
    iStartupInfo.hStdError  = bCreatingStderr ? rhProcessStderr.grab () : iStartupInfo.hStdOutput;

    PROCESS_INFORMATION iProcessInfo;
    BOOL bCreated = CreateProcess (
	NULL,				/*  module name		*/
	const_cast<char*>(pCommand),	/*  command line	*/
	NULL,				/*  process security	*/
	NULL,				/*  thread security	*/
	true,				/*  inherit handles	*/
	dwCreationFlags,
	NULL,				/*  current environment	*/
	NULL,				/*  current directory	*/
	&iStartupInfo,
	&iProcessInfo
    );
    CloseHandle (iStartupInfo.hStdInput);
    CloseHandle (iStartupInfo.hStdOutput);
    if (bCreatingStderr)
	CloseHandle (iStartupInfo.hStdError);

    if (bCreated) {
	rpProcess.setTo (new VProcess (iProcessInfo));
	return true;
    }

    return false;
}


/******************************
 *****  Subprocess Pipes  *****
 ******************************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer, // <-+
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    return supply (rStatus, ppOursideConsumer, rhFarsideProducer)
	&& supply (rStatus, rhFarsideConsumer, ppOursideProducer);
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer  // <-+
) {
    if (ppOursideConsumer) {
	HANDLE hBSProducer, hBSConsumer;
	if (!CreatePipe (&hBSProducer, &hBSConsumer, NULL, 0))
	    return rStatus.MakeErrorStatus ();
	rhFarsideProducer.claim (hBSProducer);

	ThreadedDevice::Data iDD (0, this, hBSConsumer);
	ppOursideConsumer->setTo (new VBSConsumerDevice_<OS::ThreadedDevice> (iDD));
    } else {
	HANDLE hBSProducer = CreateFile (
	    "NUL",
	    GENERIC_READ,
	    FILE_SHARE_READ + FILE_SHARE_WRITE,
	    NULL,
	    OPEN_EXISTING,
	    FILE_FLAG_SEQUENTIAL_SCAN,
	    NULL
	);
	if (FileHandleIsInvalid (hBSProducer))
	    return rStatus.MakeErrorStatus ();
	rhFarsideProducer.claim (hBSProducer);
    }
    return true;
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    if (ppOursideProducer) {
	HANDLE hBSProducer, hBSConsumer;
	if (!CreatePipe (&hBSProducer, &hBSConsumer, NULL, 0)) {
	    return rStatus.MakeErrorStatus ();
	}
	rhFarsideConsumer.claim (hBSConsumer);

	ThreadedDevice::Data iDD (0, this, hBSProducer);
	ppOursideProducer->setTo (new VBSProducerDevice_<ThreadedDevice>(iDD));
    } else {
	HANDLE hBSConsumer = CreateFile (
	    "NUL",
	    GENERIC_WRITE,
	    FILE_SHARE_READ + FILE_SHARE_WRITE,
	    NULL,
	    OPEN_EXISTING,
	    FILE_FLAG_SEQUENTIAL_SCAN,
	    NULL
	);
	if (FileHandleIsInvalid (hBSConsumer))
	    return rStatus.MakeErrorStatus ();
	rhFarsideConsumer.claim (hBSConsumer);
    }
    return true;
}


/****************************
 *****  Standard Pipes  *****
 ****************************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

bool Vca::OS::DeviceManager::supplyStdToHere (	// stdin
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = GetStdHandle (STD_INPUT_HANDLE);

    ThreadedDevice::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stdin, O_RDONLY);

//  ... create and return the requested device:
    ThreadedDevice::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSProducerDevice_<ThreadedDevice> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyStdToPeer (	// stdout
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = GetStdHandle (STD_OUTPUT_HANDLE);

    ThreadedDevice::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stdout, O_WRONLY);

//  ... create and return the requested device:
    ThreadedDevice::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSConsumerDevice_<OS::ThreadedDevice> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyErrToPeer (	// stderr
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = GetStdHandle (STD_ERROR_HANDLE);

    ThreadedDevice::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stderr, O_WRONLY);

//  ... create and return the requested device:
    ThreadedDevice::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSConsumerDevice_<OS::ThreadedDevice> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}


/*********************
 *****  Sockets  *****
 *********************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

/*>>>>DGRAM<<<<*/
bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay,
    bool			bPassive
) {
    SOCKET hSocket = socket (rAddress.family (), xType, xProtocol);
    if (SocketHandleIsInvalid (hSocket))
	return rStatus.MakeErrorStatus ();

    if (SOCK_STREAM == xType) {
	bool bFailed = true;
	if (!bPassive)
	    bFailed = rAddress.connect (hSocket) < 0;
	else {
	    if (rAddress.specifiesPort ()) {
	    //  Set SO_REUSEADDR, errors not critical...
		int const bSet = 1;
		setsockopt (hSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char const*>(&bSet), sizeof (bSet));
	    }
	    bFailed = rAddress.bind (hSocket) < 0 || listen (hSocket, 5) < 0;
	}
	if (bFailed) {
	    rStatus.MakeErrorStatus ();
	    closesocket (hSocket);
	    return false;
	}

    //  set no-delay socket option if appropriate
	if (bNoDelay) {
	    int const bSet = bNoDelay ? 1 : 0;
	    setsockopt (hSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char const*>(&bSet), sizeof (bSet));
	}

    //  create the new device
	SocketDevice::Data iDD (0, this, hSocket);
	if (!bPassive)
	    rpDevice.setTo (new VBSProducerConsumerDevice_<SocketDevice>(iDD));
	else {
	    rpDevice.setTo (new VListeningDevice_<SocketDevice>(iDD));
	}
    }
    else if (SOCK_DGRAM == xType) {
	if (bPassive && rAddress.bind (hSocket) < 0)
	    return rStatus.MakeErrorStatus ();

	DatagramSocket::Data iDD (0, this, hSocket);
	VDatagramDevice_<DatagramSocket>::Reference pDGMD (new VDatagramDevice_<DatagramSocket>(iDD));

	if (bPassive)
	    rpDevice.setTo (pDGMD);
	else {
	    pDGMD->supplyConnectionTo (rStatus, rpDevice, rAddress);
	}
    }
    else {
	rpDevice.clear ();
	return rStatus.MakeUnimplementedStatus ();
    }

    return rpDevice.isntNil ();
}


/*******************
 *****  Files  *****
 *******************/

/*******************
 *----  Input  ----*
 *******************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
) {
//  BS < file
    HANDLE hFile = CreateFile (
	rName,
	GENERIC_READ,
	FILE_SHARE_READ,
	NULL,
	OPEN_EXISTING,
	FILE_FLAG_SEQUENTIAL_SCAN,
	NULL
    );
    if (FileHandleIsInvalid (hFile)) {
	return rStatus.MakeErrorStatus ();
    }

    ThreadedDevice::Data iDD (0, this, hFile);
    VDevice::Reference pDevice (new VBSProducerDevice_<ThreadedDevice> (iDD));

    return pDevice->supply (rpBS);
}

/********************
 *----  Output  ----*
 ********************/

/*-------------------*
 *-----  Win32  -----*
 *-------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
) {
//  BS > file
    rStatus.MakeFailureStatus ();
    return false;
}


#else

/**********************************
 **********************************
 *****                        *****
 *****  Vca::OS::DeviceIndex  *****
 *****                        *****
 **********************************
 **********************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

namespace Vca {
    namespace OS {
	class DeviceIndex {
	//  Construction
	public:
	    DeviceIndex () : m_xGeneration (0), m_xPosition (0) {
	    }

	//  Destruction
	public:
	    ~DeviceIndex () {
	    }

	//  Access
	public:
	    operator unsigned int () const {
		return m_xPosition;
	    }
	    unsigned int generation () const {
		return m_xGeneration;
	    }
	    unsigned int position () const {
		return m_xPosition;
	    }

	//  Update
	public:
	    void set (DeviceIndex &rIndex) {
		rIndex.setFrom (*this);
	    }
	    void setFrom (DeviceIndex &rGenerator) {
		rGenerator.set (m_xGeneration, m_xPosition);
	    }
	private:
	    void set (unsigned int &rxGeneration, unsigned int &rxPosition) {
		if (rxGeneration != m_xGeneration) {
		    rxGeneration = m_xGeneration;
		    rxPosition = m_xPosition++;
		}
	    }
	public:
	    void incrementGeneration () {
		m_xGeneration++;
		m_xPosition = 0;
	    }

	//  State
	private:
	    unsigned int m_xGeneration;
	    unsigned int m_xPosition;
	};
    }
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::OS::DevicePoll  *****
 *****                       *****
 *********************************
 *********************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

namespace Vca {
    namespace OS {
	typedef int Handle;

	class DevicePoll {
	//  Construction
	public:
	    DevicePoll () : m_iQuery (0), m_iReply (0) {
	    }

	//  Destruction
	public:
	    ~DevicePoll () {
	    }

	//  'struct pollfd' Interchange
	public:
	    void set (pollfd &rPoll) const {
		rPoll.events = m_iQuery;
	    }
	    void get (pollfd const &rPoll) {
		m_iReply = rPoll.revents;
	    }

	//  Device Polling
	private:
	    void doPoll (Handle hDevice);
	public:
	    bool pollAccept (Handle hDevice) {
		return pollRead (hDevice);
	    }
	    bool pollConnect (Handle hDevice) {
		return pollWrite (hDevice);
	    }
	    bool pollRead (Handle hDevice);
	    bool pollWrite (Handle hDevice);

	//  Device Query
	public:
	    bool cant () const {
		return m_iReply == 0;
	    }
	    bool canAccept () const {
		return canRead ();
	    }
	    bool canConnect () const {
		return canWrite ();
	    }
	    bool canRead () const {
		return (m_iReply & POLLIN) != 0 || endOrError ();
	    }
	    bool canWrite () const {
		return (m_iReply & POLLOUT) != 0 || endOrError ();
	    }

	    bool end () const {
		return (m_iReply & POLLHUP) != 0;
	    }
	    bool error () const {
		return (m_iReply & (POLLERR + POLLNVAL)) != 0;
	    }
	    bool endOrError () const {
		return (m_iReply & (POLLERR + POLLHUP + POLLNVAL)) != 0;
	    }
	    void onClosed () {
	      m_iReply |= POLLERR;
	    }

	//  Device Use (true returned on operation success, false on failure):
	private:
	    void claimAccept () {
		claimRead ();
	    }
	    void claimConnect () {
		claimWrite ();
	    }
        public:
	    void claimRead () {
		m_iQuery &= ~POLLIN;
		m_iReply &= ~POLLIN;
	    }
	    void claimWrite () {
		m_iQuery &= ~POLLOUT;
		m_iReply &= ~POLLOUT;
	    }

	public:
	    bool doAccept (VkStatus &rStatus, Handle &rhNewDevice, Handle hDevice);
	    bool doConnect (VkStatus &rStatus, Handle hDevice);
	    bool doRead (
		VkStatus &rStatus, ssize_t &rsTransfer, BSReadArea const &rArea, Handle hDevice
	    );
	    bool doWrite (
		VkStatus &rStatus, ssize_t &rsTransfer, BSWriteArea const &rArea, Handle hDevice
	    );

	//  State
	private:
	    short m_iQuery, m_iReply;
	};
    }
}


/****************************
 ****************************
 *****  Device Polling  *****
 ****************************
 ****************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

bool Vca::OS::DevicePoll::pollRead (Handle hDevice) {
    if (canRead ())
	return true;
    m_iQuery |= POLLIN;
    doPoll (hDevice);
    return canRead ();
}

bool Vca::OS::DevicePoll::pollWrite (Handle hDevice) {
    if (canWrite ())
	return true;
    m_iQuery |= POLLOUT;
    doPoll (hDevice);
    return canWrite ();
}

void Vca::OS::DevicePoll::doPoll (Handle hDevice) {
    pollfd iPoll;
    iPoll.fd = hDevice;
    set (iPoll);
/*----------------------------------------------------------------------*
 *	What to do with errors?  The only meaningful errors indicate	*
 *	temporary limits or signals, both of which should be retried.	*
 *----------------------------------------------------------------------*/
    poll (&iPoll, 1, 0);
    get (iPoll);
}


/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

//  True returned on operation success, false on failure:
bool Vca::OS::DevicePoll::doAccept (VkStatus &rStatus, Handle &rhNewDevice, Handle hDevice) {
//  Clear the poll's accept indication, ...
    claimAccept ();

//  ... and do the accept:
    rhNewDevice = accept (hDevice, 0, 0);
    return rhNewDevice >= 0 || rStatus.MakeErrorStatus ();
}

//  True returned on operation success, false on failure:
bool Vca::OS::DevicePoll::doConnect (VkStatus &rStatus, Handle hDevice) {
//  Claim the poll's connect indication, ...
    claimConnect ();

    return true;
}

//  True returned on operation success, false on failure:
bool Vca::OS::DevicePoll::doRead (
    VkStatus &rStatus, ssize_t &rsTransfer, BSReadArea const &rArea, Handle hDevice
) {
//  Claim the poll's read indication, ...
    claimRead ();

//  ... and do the read:
    size_t sRequest = rArea.size ();
    if (sRequest > SSIZE_MAX)
	sRequest = SSIZE_MAX;

    rsTransfer = read (hDevice, rArea.base (), sRequest);
    return rsTransfer > 0 || (
	rsTransfer < 0 ? rStatus.MakeErrorStatus () : rStatus.MakeClosedStatus ()
    );
}

//  True returned on operation success, false on failure:
bool Vca::OS::DevicePoll::doWrite (
    VkStatus &rStatus, ssize_t &rsTransfer, BSWriteArea const &rArea, Handle hDevice
) {
//  Claim the poll's write indication, ...
    claimWrite ();

//  ... and do the write:
    size_t sRequest = rArea.size ();
    if (sRequest > SSIZE_MAX)
	sRequest = SSIZE_MAX;

    rsTransfer = write (hDevice, rArea.base (), sRequest);

    return rsTransfer >= 0 || rStatus.MakeErrorStatus ();
}


/*****************************
 *****************************
 *****                   *****
 *****  Vca::OS::Device  *****
 *****                   *****
 *****************************
 *****************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

namespace Vca {
    namespace OS {
	typedef int Handle;

	class Device : public ManagedDevice {
	    DECLARE_FAMILY_MEMBERS (Device, ManagedDevice);

	// Friends
	    friend class DeviceManager;

	//  Data
	public:
	    typedef ThisClass Data;

	//  Device Reference
	public:
	    typedef VReference<ThisClass> DeviceReference;

	//  Use
	public:
	    class Use : public ManagedUse {
		DECLARE_FAMILY_MEMBERS (Use, ManagedUse);

	    //  Friends
		friend class Device;
		friend class DeviceManager;

	    //  Reference
	    public:
		typedef VReference<ThisClass> Reference;

	    //  Construction
	    protected:
		Use (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer), m_pDevice (pDevice) {
		}

	    //  Destruction
	    protected:
		~Use () {
		}

	    //  Access
	    private:
		ManagedDevice *device_() const {
		    return device ();
		}
	    protected:
		Device *device () const {
		    return m_pDevice;
		}
		DeviceManager *manager () const {
		    return m_pDevice->manager ();
		}

	    //  Blocking and Callback
	    private:
		virtual bool attempt_(VkStatus &rStatus) = 0;
	    protected:
		bool block (VkStatus &rStatus) {
		    rStatus.MakeBlockedStatus ();
		    m_pDevice->block (this);
		    return true;
		}
	    private:
		void push (Reference &rpUseListRest) {
		    m_pUseListRest.claim (rpUseListRest);
		    rpUseListRest.setTo (this);
		}
		ThisClass *setPollIndex () const {
		    m_pDevice->setPollIndex ();
		    return m_pUseListRest;
		}
		ThisClass *setPollData () const {
		    m_pDevice->setPollData ();
		    return m_pUseListRest;
		}
		ThisClass *getPollData () const {
		    m_pDevice->getPollData ();
		    return m_pUseListRest;
		}
	    private:
		void unblock (Reference &rpUseListRest) {
		    rpUseListRest.claim (m_pUseListRest);
		    VkStatus iStatus;
		    if (m_pDevice->cant ())
			m_pDevice->block (this);
		    else if (attempt_(iStatus) && !iStatus.isBlocked ())
			triggerUser ();
		}

	    //  State
	    protected:
		DeviceReference const	m_pDevice;
	    private:
		Reference		m_pUseListRest;
	    };
	    typedef Use::Reference UseReference;
	    friend class Use;

	//  Accept
	public:
	    class Accept : public Use {
		DECLARE_FAMILY_MEMBERS (Accept, Use);

	    //  Construction
	    protected:
		Accept (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~Accept () {
		}

	    //  Start
	    public:
		bool start (VkStatus &rStatus) {
		    m_pDevice->incrementUseCount ();
		    return attempt (rStatus);
		}

	    //  Attempt
	    private:
		bool attempt_(VkStatus &rStatus) {
		    return attempt (rStatus);
		}
		bool attempt (VkStatus &rStatus) {
		    return device()->pollAccept () ? finish (rStatus) : block (rStatus);
		}

	    //  Finish
	    private:
		bool finish (VkStatus &rStatus) {
		    VDevice::Reference pNewDevice;
		    return device()->doAccept (
			rStatus, pNewDevice
		    ) ? onData (pNewDevice) : onError (rStatus);
		}
	    };
	    friend class Accept;

	//  BSGet
	public:
	    class BSGet : public Use {
		DECLARE_FAMILY_MEMBERS (BSGet, Use);

	    //  Construction
	    protected:
		BSGet (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSGet () {
		}

	    //  Start
	    public:
		bool start (VkStatus &rStatus, BSReadArea const &rArea) {
		    m_iArea.setTo (rArea);
		    m_pDevice->incrementUseCount ();
		    return attempt (rStatus);
		}

	    //  Attempt
	    private:
		bool attempt_(VkStatus &rStatus) {
		    return attempt (rStatus);
		}
		bool attempt (VkStatus &rStatus) {
		    return device()->pollRead () ? finish (rStatus) : block (rStatus);
		}

	     /****
	      * Solaris Note: If a read system call fails with a closed status (EPIPE or ECONNRESET)
	      * we explicitly change the DevicePoll object's status to POLLERRed, so that if a write
	      * is attempted independently on the same device it doesnt have to poll the device to know that
	      * it has errored. This proactive step is done as a work around to solve the problem with solaris poll system
	      * call problem not returning POLLERR in cases taking to terminated Windows connections.
	      ****/

	    //  Finish
	    private:
		bool finish (VkStatus &rStatus) {
		    ssize_t sTransfer = 0;
		    if (!device()->doRead (rStatus, sTransfer, m_iArea)) {
#if defined(sun)
		      if (rStatus.isClosed ())
			device ()->onClosed ();
#endif
		      return onError (rStatus);
		    }
		    return onData (sTransfer);
		}

	    //  State
	    private:
		BSReadArea m_iArea;
	    };
	    friend class BSGet;

	//  BSPut
	public:
	    class BSPut : public Use {
		DECLARE_FAMILY_MEMBERS (BSPut, Use);

	    //  Construction
	    protected:
		BSPut (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSPut () {
		}

	    //  Start
	    public:
		bool start (VkStatus &rStatus, BSWriteArea const &rArea) {
		    m_iArea.setTo (rArea);
		    m_pDevice->incrementUseCount ();
		    return attempt (rStatus);
		}

	    //  Attempt
	    private:
		bool attempt_(VkStatus &rStatus) {
		    return attempt (rStatus);
		}
		bool attempt (VkStatus &rStatus) {
		    return device()->pollWrite () ? finish (rStatus) : block (rStatus);
		}

	     /****
	      * Solaris Note: If a write system call fails with a closed status (EPIPE or ECONNRESET)
	      * we explicitly change the DevicePoll object's status to POLLERRed, so that if a read
	      * is attempted independently on the same device it doesnt have to poll the device to know that
	      * it has errored. This proactive step is done as a work around to solve the problem with solaris poll system
	      * call problem not returning POLLERR in cases taking to terminated Windows connections.
	      ****/

	    //  Finish
	    private:
		bool finish (VkStatus &rStatus) {
		    ssize_t sTransfer = 0;
		    if (!device()->doWrite (rStatus, sTransfer, m_iArea)) {
#if defined(sun)
		      if (rStatus.isClosed ())
			  device ()->onClosed ();
#endif
		      return onError (rStatus);
		    }
		    return onData (sTransfer);
		}

	    // State
	    private:
		BSWriteArea m_iArea;
	    };
	    friend class BSPut;

	//  ReadPoll
	public:
	    class ReadPoll : public Use {
		DECLARE_FAMILY_MEMBERS (ReadPoll, Use);

	    //  Construction
	    protected:
		ReadPoll (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~ReadPoll () {
		}

	    //  Start
	    public:
		bool start (VkStatus &rStatus) {
		    m_pDevice->incrementUseCount ();
		    return attempt (rStatus);
		}

	    //  Attempt
	    private:
		bool attempt_(VkStatus &rStatus) {
		    return attempt (rStatus);
		}
		bool attempt (VkStatus &rStatus) {
		    return device()->pollRead () ? finish (rStatus) : block (rStatus);
		}

	    //  Finish
	    private:
		bool finish (VkStatus &rStatus) {
                    device()->claimRead();
                    return true;
		}
	    };
	    friend class ReadPoll;

	//  WritePoll
	public:
	    class WritePoll : public Use {
		DECLARE_FAMILY_MEMBERS (WritePoll, Use);

	    //  Construction
	    protected:
		WritePoll (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~WritePoll () {
		}

	    //  Start
	    public:
		bool start (VkStatus &rStatus) {
		    m_pDevice->incrementUseCount ();
		    return attempt (rStatus);
		}

	    //  Attempt
	    private:
		bool attempt_(VkStatus &rStatus) {
		    return attempt (rStatus);
		}
		bool attempt (VkStatus &rStatus) {
		    return device()->pollWrite () ? finish (rStatus) : block (rStatus);
		}

	    //  Finish
	    private:
		bool finish (VkStatus &rStatus) {
                    device()->claimWrite();
                    return true;
		}
	    };
	    friend class WritePoll;

	//  Construction
	public:
	    Device (
		VReferenceable *pContainer, Manager *pManager, Handle &rhDevice
	    ) : BaseClass (pContainer, pManager), m_hDevice (grab (rhDevice)) {
	    }
	    Device (VReferenceable *pContainer, Manager *pManager) : BaseClass (pContainer, pManager) {
		grab (m_hDevice);
	    }
	    Device (
		VReferenceable *pContainer, Device &rOther
	    ) : BaseClass (pContainer, rOther), m_hDevice (rOther.grab ()) {
	    }

	//  Destruction
	public:
	    ~Device () {
		close ();
	    }

	//  Access
	public:
	    operator Handle () const {
		return m_hDevice;
	    }
	    Handle grab () {
		return grab (m_hDevice);
	    }
	    static Handle grab (Handle &rhDevice) {
		Handle hDevice = rhDevice;
		rhDevice = -1;
		return hDevice;
	    }

	//  Update
	public:
	    void claim (Device &rOther) {
		close ();
		m_hDevice = rOther.grab ();
	    }
	    void claim (Handle &rhDevice) {
		close ();
		m_hDevice = grab (rhDevice);
	    }
	    void close () {
		close (m_hDevice);
	    }
	    static void close (Handle &rhDevice) {
		if (rhDevice >= 0) {
		    ::close (grab (rhDevice));
		}
	    }

	//  Control
	public:
	    bool makeDeviceFriendly () const {
		return makePrivate (); // && makeNonblocking ();
/*****************
 *  2010-07-19:
 *     In aggressive testing of the Vca and pool features involved in multi-hop connectivity,
 *     I've consistently discovered that the middle-men of that connectivity (e.g., pools)
 *     stuck in blocking __read_nocancel and __accept_nocancel calls when they try to exit.
 *
 *     To the best of my knowledge, we haven't seen this before but may have always been
 *     vulnerable.  Specifically, even though we make blocking calls, we rely on poll to
 *     tell us whether the blocking call will complete before we attempt it.  When poll
 *     misleads us as it has been doing, most probably because of an underlying state
 *     change, we block.
 *
 *  - Mike
 *
		return makePrivate () && makeNonblocking ();
 ****************/
	    }
	    bool makeBlocking () const {
		return setNonBlockingTo (false);
	    }
	    bool makeNonblocking () const {
		return setNonBlockingTo (true);
	    }
	    bool makePublic () const {
		return setInheritabilityTo (true);
	    }
	    bool makePrivate () const {
		return setInheritabilityTo (false);
	    }
	    bool setInheritabilityTo (bool bInheritable) const;
	    bool setNonBlockingTo (bool bNonBlocking) const;
	    bool supplyDuplicate (Handle &rhDuplicate) const;
	    bool supplyDuplicate (Handle &rhDuplicate, bool bInheritable) const;

	//  Poll Registration
	public:
	    void block (Use *pUse);

	    void setPollIndex ();
	    void setPollData ();
	    void getPollData ();

	//  Device Query
	public:
	    bool cant () const {
		return m_iDevicePoll.cant ();
	    }
	    bool canAccept () const {
		return m_iDevicePoll.canAccept ();
	    }
	    bool canConnect () const {
		return m_iDevicePoll.canConnect ();
	    }
	    bool canRead () const {
		return m_iDevicePoll.canRead ();
	    }
	    bool canWrite () const {
		return m_iDevicePoll.canWrite ();
	    }
	    void onClosed () {
	      m_iDevicePoll.onClosed ();
	    }

	//  Device Polling
	public:
	    bool pollAccept () {
		return m_iDevicePoll.pollAccept (m_hDevice);
	    }
	    bool pollConnect () {
		return m_iDevicePoll.pollConnect (m_hDevice);
	    }
	    bool pollRead () {
		return m_iDevicePoll.pollRead (m_hDevice);
	    }
	    bool pollWrite () {
		return m_iDevicePoll.pollWrite (m_hDevice);
	    }
            void claimRead () {
                m_iDevicePoll.claimRead ();
            }
            void claimWrite () {
                m_iDevicePoll.claimWrite ();
            }

	//  Device Use (true returned on operation success, false on failure):
	protected:
	    using BaseClass::incrementUseCount;
	public:
	    virtual bool doAccept (VkStatus &rStatus, VDevice::Reference &rpNewDevice) {
		return rStatus.MakeFailureStatus ();
	    }
	    bool doConnect (VkStatus &rStatus) {
		return m_iDevicePoll.doConnect (rStatus, m_hDevice);
	    }
	    bool doRead (
		VkStatus &rStatus, ssize_t &rsTransfer, BSReadArea const &rArea
	    ) {
		return m_iDevicePoll.doRead (rStatus, rsTransfer, rArea, m_hDevice);
	    }
	    bool doWrite (
		VkStatus &rStatus, ssize_t &rsTransfer, BSWriteArea const &rArea
	    ) {
		return m_iDevicePoll.doWrite (rStatus, rsTransfer, rArea, m_hDevice);
	    }

	//  State
	protected:
	    Handle	m_hDevice;
	    DevicePoll	m_iDevicePoll;
	    DeviceIndex	m_iDeviceIndex;
	};
	typedef Device::Use		DeviceUse;
	typedef DeviceUse::Reference	DeviceUseReference;

	//  Far Handle
	typedef Device FarHandle;
    }
}
#ifdef VMS_LINUX_EXPLICIT_COMPAT
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::Device>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::Device>::ProducerTypes>;
template class Vca::VBSConsumerDevice_<Vca::OS::Device>;
template class Vca::VBSProducerDevice_<Vca::OS::Device>;
template class Vca::VBSProducerConsumerDevice_<Vca::OS::Device>;
template class Vca::VDevice_<Vca::OS::Device>;
#endif


/****************************
 ****************************
 *****  Device Control  *****
 ****************************
 ****************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

bool Vca::OS::Device::setInheritabilityTo (bool bInheritable) const {
    return fcntl (m_hDevice, F_SETFD, bInheritable ? 0 : FD_CLOEXEC) != -1;
}

bool Vca::OS::Device::setNonBlockingTo (bool bNonBlocking) const {
    int iFlagsNow = fcntl (m_hDevice, F_GETFL);
    int iFlagsNew = bNonBlocking ? iFlagsNow | O_NONBLOCK : iFlagsNow & ~O_NONBLOCK;
    return iFlagsNow != -1 && (iFlagsNow == iFlagsNew || fcntl (m_hDevice, F_SETFL, iFlagsNew) != -1);
}

bool Vca::OS::Device::supplyDuplicate (Handle &rhDuplicate) const {
    rhDuplicate = dup (m_hDevice);
    return rhDuplicate >= 0;
}

bool Vca::OS::Device::supplyDuplicate (Handle &rhDuplicate, bool bInheritable) const {
    if (!supplyDuplicate (rhDuplicate))
	return false;

    Device iDuplicate (0, m_pManager, rhDuplicate);
    if (!iDuplicate.setInheritabilityTo (bInheritable))
	return false;

    rhDuplicate = iDuplicate.grab ();
    return true;
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::OS::SocketDevice  *****
 *****                         *****
 ***********************************
 ***********************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

namespace Vca {
    namespace OS {
	class SocketDevice : public Device {
	    DECLARE_FAMILY_MEMBERS (Device, Device);

	//  AcceptUse
	public:
	    typedef Accept AcceptUse;

	//  Construction
	public:
	    SocketDevice (
		VReferenceable *pContainer, Manager *pManager, Handle &rhSocket
	    ) : BaseClass (pContainer, pManager, rhSocket) {
	    }
	    SocketDevice (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther) {
	    }

	//  Destruction
	public:
	    ~SocketDevice () {
	    }

	//  Access
	public:
	    bool getName (VkStatus &rStatus, VString &rString) const;
            bool getSocket (VkStatus &rStatus, SOCKET& rSocket) const;

	//  Device Use (true returned on operation success, false on failure):
	public:
	    virtual bool doAccept (VkStatus &rStatus, VDevice::Reference &rpNewDevice);

	//  User Accounting
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    void onFirstReader () {
		ReportUser ("first socket reader", ++s_cSocketReaders);
		BaseClass::onFirstReader ();
	    }
	    void onFirstWriter () {
		ReportUser ("first socket writer", ++s_cSocketWriters);
		BaseClass::onFirstWriter ();
	    }
	    void onFinalReader () {
		ReportUser ("final socket reader", --s_cSocketReaders, shutdown (operator Handle (), SHUT_RD));
		BaseClass::onFinalReader ();
	    }
	    void onFinalWriter () {
		ReportUser ("final socket writer", --s_cSocketWriters, shutdown (operator Handle (), SHUT_WR));
		BaseClass::onFinalWriter ();
	    }
	    void onFinalListener ();
	};
    }
}
#ifdef VMS_LINUX_EXPLICIT_COMPAT
template class Vca::VAccept_<Vca::OS::SocketDevice>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::SocketDevice>::ProducerTypes>;
template class Vca::VBSProducerConsumerDevice_<Vca::OS::SocketDevice>;
template class Vca::VDevice_<Vca::OS::SocketDevice>;
template class Vca::VListeningDevice_<Vca::OS::SocketDevice>;
#endif


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

bool Vca::OS::SocketDevice::getName (VkStatus &rStatus, VString &rName) const {
    VkSocketAddress iAddress;
    return iAddress.GetSocketName (m_hDevice, &rStatus) >= 0 && iAddress.getName (rName);
}

bool Vca::OS::SocketDevice::getSocket (VkStatus &rStatus, SOCKET& rSocket) const {
    rSocket = m_hDevice;
    return true;
}

/************************
 ************************
 *****  Device Use  *****
 ************************
 ************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

bool Vca::OS::SocketDevice::doAccept (VkStatus &rStatus, VDevice::Reference &rpNewDevice) {
    Handle hNewDevice;
    if (m_iDevicePoll.doAccept (rStatus, hNewDevice, m_hDevice)) {
	SocketDevice::Data iDD (0, m_pManager, hNewDevice);
	rpNewDevice.setTo (new VBSProducerConsumerDevice_<SocketDevice> (iDD));
	return rpNewDevice.isntNil ();
    }
    return false;
}

/*****************************
 *****************************
 *****  User Accounting  *****
 *****************************
 *****************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

void Vca::OS::SocketDevice::onFinalListener () {
    VkSocketAddress iAddress; VkStatus iStatus;
    if (iAddress.GetSocketName (operator Handle (), &iStatus) >= 0 && iAddress.familyIsUnix ())
	unlink (iAddress.unixPathName ());
    close ();
    BaseClass::onFinalListener ();
}


/************************************
 ************************************
 *****  Datagram Socket Device  *****
 ************************************
 ************************************/

/*----------------------*
 *----  Linux/Unix  ----*
 *----------------------*/

namespace Vca {
    namespace OS {
	class DatagramSocket : public SocketDevice {
	    DECLARE_FAMILY_MEMBERS (DatagramSocket, SocketDevice);

	//  Data
	public:
	    typedef ThisClass Data;

	//  BSGet
	public:
	    typedef BaseClass::BSGet BSGetBase;
	    class BSGet : public BSGetBase {
		DECLARE_FAMILY_MEMBERS (BSGet, BSGetBase);

	    //  Construction
	    protected:
		BSGet (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSGet () {
		}

	    //  Access
	    public:
	    };

	//  BSPut
	public:
	    typedef BaseClass::BSPut BSPutBase;
	    class BSPut : public BSPutBase {
		DECLARE_FAMILY_MEMBERS (BSPut, BSPutBase);

	    //  Construction
	    protected:
		BSPut (VReferenceable *pContainer, Device *pDevice) : BaseClass (pContainer, pDevice) {
		}

	    //  Destruction
	    protected:
		~BSPut () {
		}

	    //  Access
	    public:
	    };

	//  Construction
	public:
	    DatagramSocket (VReferenceable *pContainer, Manager *pManager, Handle &rhDevice) : BaseClass (pContainer, pManager, rhDevice) {
	    }
	    DatagramSocket (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer, rOther) {
	    }

	//  Destruction
	public:
	    ~DatagramSocket () {
	    }

	};
    }
}
#ifdef VMS_LINUX_EXPLICIT_COMPAT
template class Vca::VAccept_<Vca::OS::DatagramSocket>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::DatagramSocket>::ConsumerTypes>;
template class Vca::VBSMove_<Vca::VDevice_<Vca::OS::DatagramSocket>::ProducerTypes>;
template class Vca::VBSProducerConsumerDevice_<Vca::OS::DatagramSocket>;
template class Vca::VListeningDevice_<Vca::OS::DatagramSocket>;
template class Vca::VDatagramDevice_<Vca::OS::DatagramSocket>;
template class Vca::VDevice_<Vca::OS::DatagramSocket>;
#endif


/************************************
 ************************************
 *****                          *****
 *****  Vca::OS::DeviceManager  *****
 *****                          *****
 ************************************
 ************************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

namespace Vca {
    namespace OS {
	class DeviceManager : public VDeviceManager {
	    DECLARE_CONCRETE_RTTLITE (DeviceManager, VDeviceManager);

	//  Poll Vector
	public:
	    typedef VkDynamicArrayOf<pollfd> PollVector;

	//  Construction
	public:
	    DeviceManager (VCohort *pCohort);

	//  Destruction
	private:
	    ~DeviceManager ();

	//  Poll Registration
	public:
	    void block (DeviceUse *pUse) {
		pUse->push (m_pBlockedUses);
	    }
	    void set (DeviceIndex &rDeviceIndex) {
		rDeviceIndex.setFrom (m_iDeviceIndex);
	    }
	    void set (
		DeviceIndex const &rDeviceIndex,
		DevicePoll  const &rDevicePoll,
		Handle hDevice
	    ) {
		pollfd &rPollfd = m_iPollVector[rDeviceIndex];
		rDevicePoll.set (rPollfd);
		rPollfd.fd = hDevice;
		if (hDevice < 0) {
		    rPollfd.revents = POLLERR;
		    m_bValidPoll = false;
		}
	    }
	    void get (DeviceIndex const &rDeviceIndex, DevicePoll &rDevicePoll) const {
		rDevicePoll.get (m_iPollVector[rDeviceIndex]);
	    }

	//  Event Processing
	private:
	    virtual /*override*/ bool postInterrupt_();
	    virtual /*override*/ bool processEvent_(size_t sTimeout);

	//  Suppliers
	public:
	    //  ... subprocess
	    bool supply (
		VkStatus&		rStatus,
		VProcess::Reference&	rpProcess,
		FarHandle&		rhProcessStdin,
		FarHandle&		rhProcessStdout,
		FarHandle&		rhProcessStderr, // & == &rhProcessStdout -> stdout device device
		char const*		pCommand
	    );

	    //  ... subprocesses pipes
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+	(null -> null device)
		FarHandle&		rhFarsideProducer, // <-+
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+	(null -> null device)
	    );
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference*	ppOursideConsumer, // --+	(null -> null device)
		FarHandle&		rhFarsideProducer  // <-+
	    );
	    bool supply (
		VkStatus&		rStatus,
		FarHandle&		rhFarsideConsumer, // --+
		VDevice::Reference*	ppOursideProducer  // <-+	(null -> null device)
	    );

	    //  ... standard pipes
	    bool supplyStdToHere (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdin
	    );
	    bool supplyStdToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stdout
	    );
	    bool supplyErrToPeer (
		VkStatus &rStatus, VDevice::Reference &rpDevice // stderr
	    );

	    //  ... sockets
	    bool supply (
		VkStatus&		rStatus,
		VDevice::Reference&	rpDevice,
		VkSocketAddress const&	rAddress,
		int			xProtocol,
		int			xType,
		bool			bNoDelay,
		bool			bPassive
	    );
	    bool supply_(
		VkStatus&		rStatus,
		VDevice::Reference&	rpDevice,
		VkSocketAddress const&	rAddress,
		int			xProtocol,
		int			xType,
		bool			bNoDelay,
		bool			bPassive
	    );

	    //  ... files
	    bool supply (
		VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
	    );
	    bool supply (
		VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
	    );

	// socketpair reimplementation
	private:
	    static int getSocketPair (int sockets[2]);

	//  State
	private:
	    DeviceUseReference	m_pBlockedUses;
	    DeviceIndex		m_iDeviceIndex;
	    PollVector		m_iPollVector;
	    bool		m_bValidPoll;
	    pthread_mutex_t	m_iMutex;
	    pthread_cond_t	m_iCondvar;
	    enum {
		State_Running,
		State_CondWait,
		State_PollWait,
		State_Signalled
	    }			m_xState;
	    int			m_iPSPair[2];	//  Poll Signalling Socket Pair
	    DevicePoll		m_iPSPoll;
	};
    }
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::OS::DeviceManager::DeviceManager (VCohort *pCohort) : BaseClass (pCohort), m_bValidPoll (false), m_xState (State_Running) {
    m_iPSPair[0] = m_iPSPair[1] = -1;

    pthread_mutex_init (&m_iMutex, 0);
    pthread_cond_init  (&m_iCondvar, 0);

    joinCohort ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::OS::DeviceManager::~DeviceManager () {
    exitCohort ();

    pthread_mutex_destroy (&m_iMutex);
    pthread_cond_destroy  (&m_iCondvar);

    for (unsigned int xPSPair = 0; xPSPair < sizeof (m_iPSPair) / sizeof (m_iPSPair[0]); xPSPair++) {
	if (m_iPSPair[xPSPair] >= 0)
	    close (m_iPSPair[xPSPair]);
    }
}


/*******************************
 *******************************
 *****  Poll Registration  *****
 *******************************
 *******************************/

/*------------------------*
 *-----  Linux/Unix  -----*
 *------------------------*/

void Vca::OS::Device::block (Use *pUse) {
    m_pManager->block (pUse);
}

void Vca::OS::Device::setPollIndex () {
    m_pManager->set (m_iDeviceIndex);
}

void Vca::OS::Device::setPollData () {
    m_pManager->set (m_iDeviceIndex, m_iDevicePoll, m_hDevice);
}

void Vca::OS::Device::getPollData () {
    m_pManager->get (m_iDeviceIndex, m_iDevicePoll);
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

/*----------------------*
 *----  Linux/Unix  ----*
 *----------------------*/

bool Vca::OS::DeviceManager::postInterrupt_() {
    bool bSignalingCondvar = false;

    pthread_mutex_lock (&m_iMutex);
    switch (m_xState) {
    case State_CondWait:
	bSignalingCondvar = true;
	break;

    case State_PollWait:
	if (m_iPSPair[1] >= 0) {
	    static char iByte = 0;
	    write (m_iPSPair[1], &iByte, sizeof (iByte));
	}
	break;
    }
    m_xState = State_Signalled;
    pthread_mutex_unlock (&m_iMutex);
    if (bSignalingCondvar)
	pthread_cond_signal (&m_iCondvar);

    return true;
}

//  Returns true if at an event was processed, false otherwise:
bool Vca::OS::DeviceManager::processEvent_(
    size_t sTimeout /* in milliseconds */
) {
//  Start a new poll, ...
    m_iDeviceIndex.incrementGeneration ();
    m_bValidPoll = true;

//  Obtain poll positions for the devices being polled, ...
    DeviceUseReference pBlockedUses (m_pBlockedUses);
    while (pBlockedUses.isntNil ()) {
	pBlockedUses.setTo (pBlockedUses->setPollIndex ());
    }

//  Initialize the poll vector, ...
    size_t sPoll = m_iDeviceIndex.position ();
    if (sPoll > 0) {
	m_iPollVector.Guarantee (sPoll + 1);
	pBlockedUses.setTo (m_pBlockedUses);
	while (pBlockedUses.isntNil ()) {
	    pBlockedUses.setTo (pBlockedUses->setPollData ());
	}
    }
    else if (0 == sTimeout)
	return false;

//  Do the poll if all handles are valid, ...
    if (m_bValidPoll) {
	int cEvents = 0;
	if (sPoll > 0) {
	    int xSignalFD = -1;
	    if (sTimeout > 0) {
		pthread_mutex_lock (&m_iMutex);
		if (m_xState == State_Signalled)
		    sTimeout = 0;
		else {
		    xSignalFD = m_iPSPair[0];
		    if (xSignalFD < 0 && getSocketPair (m_iPSPair) == 0) {
			xSignalFD = m_iPSPair[0];
			m_iPSPoll.pollRead (xSignalFD);	//  prime the signal reader
		    }
		    if (xSignalFD >= 0) {
			set (m_iDeviceIndex, m_iPSPoll, xSignalFD);
			sPoll++;
			m_xState = State_PollWait;
		    }
		    else if (sTimeout > 1000) {
			sTimeout = 1000;
		    }
		}
		pthread_mutex_unlock (&m_iMutex);
	    }
	    cEvents = poll (m_iPollVector.elementArray (), sPoll, sTimeout);
	    if (m_xState == State_PollWait) {
		get (m_iDeviceIndex, m_iPSPoll);
		if (m_iPSPoll.canRead ()) {
		    //  Drain the signal reader...
		    char iByte[1]; BSReadArea iByteArea (iByte, sizeof(iByte));
		    VkStatus iStatus; ssize_t sTransfer;
		    while (m_iPSPoll.doRead (iStatus, sTransfer, iByteArea, xSignalFD) && m_iPSPoll.pollRead (xSignalFD));
		}
	    }
	    m_xState = State_Running;
	}
	else if (sTimeout < UINT_MAX) {
	    timespec iTimespec;
#ifdef __linux__
	    timeval iTimeval;
	    gettimeofday (&iTimeval, 0);
	    iTimespec.tv_sec = iTimeval.tv_sec;
	    iTimespec.tv_nsec = iTimeval.tv_usec * 1000;
#else
	    clock_gettime (CLOCK_REALTIME, &iTimespec);
#endif

	    iTimespec.tv_sec += sTimeout / 1000;
	    unsigned int iNS = iTimespec.tv_nsec + (sTimeout % 1000) * 1000000000;
	    if (iNS < 1000000000)
		iTimespec.tv_nsec = iNS;
	    else {
		iTimespec.tv_sec += iNS / 1000000000;
		iTimespec.tv_nsec = iNS % 1000000000;
	    }

	    pthread_mutex_lock (&m_iMutex);
	    if (m_xState != State_Signalled) {
		m_xState = State_CondWait;
		while (pthread_cond_timedwait (&m_iCondvar, &m_iMutex, &iTimespec) == 0 && m_xState != State_Signalled);
	    }
	    m_xState = State_Running;
	    pthread_mutex_unlock (&m_iMutex);
	}
	else {
	    pthread_mutex_lock (&m_iMutex);
	    if (m_xState != State_Signalled) {
		m_xState = State_CondWait;
		while (pthread_cond_wait (&m_iCondvar, &m_iMutex) == 0 && m_xState != State_Signalled);
	    }
	    m_xState = State_Running;
	    pthread_mutex_unlock (&m_iMutex);
	}

	if (cEvents == 0)	// timeout
	    return false;
	if (cEvents < 0) {	// error or interrupt
	    VkStatus iStatus;
	    iStatus.MakeErrorStatus ();
	    if (iStatus.Code () != EINTR) raiseSystemException (
		"Vca::OS::DeviceManager::processEvent: Polling Error: %s", iStatus.CodeDescription ()
	    );
	    return false;
	}
    }

//  Return the poll results to their devices, ...
    pBlockedUses.setTo (m_pBlockedUses);
    while (pBlockedUses.isntNil ()) {
	pBlockedUses.setTo (pBlockedUses->getPollData ());
    }

//  And process the events:
    pBlockedUses.claim (m_pBlockedUses);
    while (pBlockedUses.isntNil ()) {
	DeviceUseReference pThisUse (pBlockedUses);
	pThisUse.setTo (pBlockedUses);
	pThisUse->unblock (pBlockedUses);
    }

    return true;
}


/********************************
 ********************************
 *****  Resource Suppliers  *****
 ********************************
 ********************************/

/************************
 *****  Subprocess  *****
 ************************/

/*----------------------*
 *----  Linux/Unix  ----*
 *----------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    FarHandle&			rhProcessStdin,
    FarHandle&			rhProcessStdout,
    FarHandle&			rhProcessStderr, // & == &rhProcessStdout -> stdout device device
    char const*			pCommand
) {
//  Parse the command, ...
    bool bProcessingOptions = pCommand && '{' == pCommand[0];
    while (bProcessingOptions) {
	switch (*++pCommand) {
	case '}':
	    bProcessingOptions = false;
	    pCommand++;
	    break;
	case '\0':
	    bProcessingOptions = false;
	    break;
	}
    }
    pCommand += strspn (pCommand, " \t");

    VProcess::Command iCommand (pCommand);
    char **argv = iCommand.argv();
    if (IsNil (argv[0])) {
	errno = EINVAL;
	return false;
    }

//  ... obtain handles for the child's standard input, output, and error, ...
    if (!rhProcessStdin.makePublic ())
	return false;
    if (!rhProcessStdout.makePublic ())
	return false;
    if (&rhProcessStderr != &rhProcessStdout && !rhProcessStderr.makePublic ())
	return false;

    int hProcessStdin  = rhProcessStdin .grab ();
    int hProcessStdout = rhProcessStdout.grab ();
    int hProcessStderr = &rhProcessStderr != &rhProcessStdout ? rhProcessStderr.grab () : -1;

//  ... fork, ...
    pid_t hProcess = vfork ();

//  ... and setup the child:
    if (0 == hProcess) {
      /****
       * if stdin, stdout or stderr have been closed in the parent, it is
       * possible that one of these files is assigned a file descriptor
       * normally used for stdio. Use dup to ensure we don't accidentally
       * close the file before we setup stdio for the child.
       ****/
	while (hProcessStdin >= 0 && hProcessStdin < 3)
	    hProcessStdin = dup (hProcessStdin);
	while (hProcessStdout >= 0 && hProcessStdout < 3)
	    hProcessStdout = dup (hProcessStdout);
	while (hProcessStderr >= 0 && hProcessStderr < 3)
	    hProcessStderr = dup (hProcessStderr);

      /****
       * establish the input and output files created above as stdin, stdout
       * and stderr for the command to be 'exec'ed ...
       ****/
	dup2 (hProcessStdin , 0);	// ... stdin
	dup2 (hProcessStdout, 1);	// ... stdout
	if (hProcessStderr < 0)		// ... stderr
	    dup2 (hProcessStdout, 2);
	else {
	    dup2 (hProcessStderr, 2);
	    close (hProcessStderr);
	}
	close (hProcessStdout);
	close (hProcessStdin);

	execvp (argv[0], argv);
	_exit (errno);
    }

/*****  ... and the parent:  *****/
    if (hProcessStderr >= 0)
	close (hProcessStderr);
    close (hProcessStdout);
    close (hProcessStdin);

    if (hProcess > 0) {
	rpProcess.setTo (new VProcess (hProcess));
	return true;
    }

    return false;
}

#if defined (__linux__)

/***************************
 *****  getSocketPair  *****
 ***************************/

/********************************************************************************
 * Note: Blocking socket descriptors created using Linux's socketpair ()
 *       has problems (hangs) when writing large chunks of date. To avoid it
 *       we have written this local getSocketPair () which is called with AF_INET
 *       family, to create connected pair of INET domain sockets
 ********************************************************************************/

int Vca::OS::DeviceManager::getSocketPair (int sockets[2]) {
  int s, len;
  struct sockaddr_in addr_srvr, addr_clnt;

  //  create a listener socket
  int const domain = AF_INET;
  int const type = SOCK_STREAM;
  int const protocol = 0;

  if ((s = socket (domain, type, protocol)) == -1)
    return -1;


  //  set the server socket address
  memset (&addr_srvr, 0, sizeof (addr_srvr));
  addr_srvr.sin_family = domain;
  addr_srvr.sin_port = htons (0);
  addr_srvr.sin_addr.s_addr = INADDR_ANY;

  //  bind the server socket with the address
  if (bind (s, (struct sockaddr*) &addr_srvr, sizeof (addr_srvr)) == -1)
    return -1;

  if (listen (s, 5) < 0)
    return -1;


  len = sizeof (addr_srvr);
  if (getsockname (s, (struct sockaddr*) &addr_srvr, &len) < 0)
    return -1;


  //  create the client socket
  if ((sockets [1] = socket (domain, type, protocol)) == -1)
    return -1;


  //  put the client socket in non-blocking mode
  fcntl (sockets[1], F_SETFL, fcntl (sockets[1], F_GETFL, 0) | O_NONBLOCK);

  if (connect(sockets[1], (struct sockaddr*) &addr_srvr, sizeof (addr_srvr)) <0) {
    if (errno != EINPROGRESS)
      return -1;
  }

  int t = sizeof (addr_clnt);
  if ((sockets[0] = accept (s, (struct sockaddr*) &addr_clnt, (socklen_t*)&t)) == -1)
    return -1;

  //  revert the client socket back to blocking mode
  fcntl (sockets [1], F_SETFL, fcntl (sockets [1], F_GETFL, 0) & ~O_NONBLOCK);

  close (s);
  return 0;
}
#else

int Vca::OS::DeviceManager::getSocketPair (int sockets[2]) {
    return socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
}
#endif


/******************************
 *****  Subprocess Pipes  *****
 ******************************/

/*----------------------*
 *----  Linux/Unix  ----*
 *----------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer, // <-+
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    if (!ppOursideConsumer || !ppOursideProducer)
	return supply (
	    rStatus, ppOursideConsumer, rhFarsideProducer
	) && supply (rStatus, rhFarsideConsumer, ppOursideProducer);

    if (!supply (rStatus, ppOursideConsumer, rhFarsideProducer))
	return false;

    Handle hFarsideConsumer;
    if (!rhFarsideProducer.supplyDuplicate (hFarsideConsumer))
	return false;

    rhFarsideConsumer.claim (hFarsideConsumer);
    ppOursideProducer->setTo (*ppOursideConsumer);
    return true;
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference*		ppOursideConsumer, // --+	(null -> null device)
    FarHandle&			rhFarsideProducer  // <-+
) {
    if (ppOursideConsumer) {
	Handle sockets[2];
	if (getSocketPair(sockets) < 0)
	    return rStatus.MakeErrorStatus ();

	rhFarsideProducer.claim (sockets[0]);

	SocketDevice::Data iDD (0, this, sockets[1]);
	ppOursideConsumer->setTo (new VBSProducerConsumerDevice_<SocketDevice>(iDD));
    } else {
	Handle hNull = open (g_pNullDeviceName, O_RDWR, 0);
	if (hNull < 0)
	    return rStatus.MakeErrorStatus ();
	rhFarsideProducer.claim (hNull);
    }
    return true;
}

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    FarHandle&			rhFarsideConsumer, // --+
    VDevice::Reference*		ppOursideProducer  // <-+	(null -> null device)
) {
    return supply (rStatus, ppOursideProducer, rhFarsideConsumer);
}


/****************************
 *****  Standard Pipes  *****
 ****************************/

/*----------------------*
 *-----  Linux/Unix-----*
 *----------------------*/

bool Vca::OS::DeviceManager::supplyStdToHere ( // stdin
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = fileno (stdin);
    Device::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stdin, O_RDONLY);

//  ... create and return the requested device:
    Device::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSProducerDevice_<Device> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyStdToPeer ( // stdout
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = fileno (stdout);
    Device::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stdout, O_WRONLY);

//  ... create and return the requested device:
    Device::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSConsumerDevice_<Device> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}

bool Vca::OS::DeviceManager::supplyErrToPeer (
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
//  Obtain and duplicate the standard handle, ...
    HANDLE hDevice = fileno (stderr);
    Device::Data iDD1 (0, this, hDevice);
    iDD1.supplyDuplicate (hDevice, false);

//  ... redirect the original handle to the null device:
    redirectToNull (stderr, O_WRONLY);

//  ... create and return the requested device:
    Device::Data iDD2 (0, this, hDevice);
    rpDevice.setTo (new VBSConsumerDevice_<Device> (iDD2));

//  ... reset iDD1 so that it doesn't close the standard channel
    hDevice = iDD1.grab ();

//  ... and return the device.
    return rpDevice.isntNil ();
}


/*********************
 *****  Sockets  *****
 *********************/

/*----------------------*
 *-----  Linux/Unix-----*
 *----------------------*/

/****
 * Note: Very occasionally in solaris, we see connection refused errors when the CPU is on a high load.
 *       This connection refused error disappears immediately on retries. To prevent this kind of false
 *       connection refused behaviour we have a retry limit of 3 for ECONNREFUSED errors.
 ****/

bool Vca::OS::DeviceManager::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay,
    bool			bPassive
) {
  bool bResult = false;
  Vca::U32 cRetries = 3; //  internal connection refused retry limit

  while (cRetries-- > 0 &&
	 rStatus.MakeSuccessStatus () && //  reset the error status for retrying again
	 !(bResult = supply_ (rStatus, rpDevice, rAddress, xProtocol, xType, bNoDelay, bPassive)) && 
	 !bPassive && 
	 rStatus.Code () == ECONNREFUSED
  );
  return bResult;
}

/*>>>>DGRAM<<<<*/
bool Vca::OS::DeviceManager::supply_(
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay,
    bool			bPassive
) {
    int hSocket = socket (rAddress.family (), xType, xProtocol);
    if (SocketHandleIsInvalid (hSocket))
	return rStatus.MakeErrorStatus ();

    if (SOCK_STREAM == xType) {
    //  connect should be asynchronous:
	bool bFailed = true;
	if (!bPassive)
	    bFailed = rAddress.connect (hSocket) < 0;
	else {
	    if (rAddress.specifiesPort ()) {
	    //  Set SO_REUSEADDR, errors not critical...
		int const bSet = 1;
		setsockopt (hSocket, SOL_SOCKET, SO_REUSEADDR, &bSet, sizeof (bSet));
	    }
	    bFailed = rAddress.bind (hSocket) < 0 || listen (hSocket, 5) < 0;
	}
	if (bFailed) {
	    rStatus.MakeErrorStatus ();
	    close (hSocket);
	    return false;
	}

    //  set no-delay socket option if appropriate
	if (bNoDelay) {
	    int iSetting = bNoDelay ? 1 : 0;
	    setsockopt (hSocket, IPPROTO_TCP, TCP_NODELAY, &iSetting, sizeof (iSetting));
	}

    //  create the new device
	SocketDevice::Data iDD (0, this, hSocket);
	if (!bPassive)
	    rpDevice.setTo (new VBSProducerConsumerDevice_<SocketDevice>(iDD));
	else {
	    rpDevice.setTo (new VListeningDevice_<SocketDevice>(iDD));
	}
    }
    else if (SOCK_DGRAM == xType) {
	if (bPassive && rAddress.bind (hSocket) < 0)
	    return rStatus.MakeErrorStatus ();

	DatagramSocket::Data iDD (0, this, hSocket);
	VDatagramDevice_<DatagramSocket>::Reference pDGMD (new VDatagramDevice_<DatagramSocket>(iDD));

	if (bPassive)
	    rpDevice.setTo (pDGMD);
	else {
	    pDGMD->supplyConnectionTo (rStatus, rpDevice, rAddress);
	}
    }
    else {
	rpDevice.clear ();
	return rStatus.MakeUnimplementedStatus ();
    }

    return rpDevice.isntNil ();
}


/*******************
 *****  Files  *****
 *******************/

/*******************
 *----  Input  ----*
 *******************/

/*----------------------*
 *-----  Linux/Unix-----*
 *----------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
) {
//  BS < file
    int hFile = open (rName, O_RDONLY);
    if (hFile < 0) {
	return rStatus.MakeErrorStatus ();
    }
    Device::Data iDD (0, this, hFile);
    VDevice::Reference pDevice (new VBSProducerDevice_<Device> (iDD));
    return pDevice->supply (rpBS);
}

/********************
 *----  Output  ----*
 ********************/

/*----------------------*
 *-----  Linux/Unix-----*
 *----------------------*/

bool Vca::OS::DeviceManager::supply (
    VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
) {
//  BS > file
    rStatus.MakeFailureStatus ();
    return false;
}
#endif


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>>>>>>>  Deferred Definitions  <<<<<<<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

Vca::OS::ManagedDevice::ManagedDevice (
    VReferenceable *pContainer, Manager *pManager
) : BaseClass (pContainer), m_pManager (pManager) {
}

Vca::OS::ManagedDevice::ManagedDevice (
    VReferenceable *pContainer, ManagedDevice const &rOther
) : BaseClass (pContainer), m_pManager (rOther.m_pManager) {
}

Vca::OS::ManagedDevice::~ManagedDevice () {
}

Vca::VCohort *Vca::OS::ManagedDevice::cohort () const {
    return m_pManager->cohort ();
}

unsigned int Vca::OS::ManagedDevice::useCount () const {
    return m_pManager->useCount ();
}
void Vca::OS::ManagedDevice::onUseCountWasZero_() {
    m_pManager->incrementUseCount ();
}
void Vca::OS::ManagedDevice::onUseCountIsZero_() {
    m_pManager->decrementUseCount ();
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VDeviceFactory  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDeviceFactory::VDeviceFactory (VCohort *pCohort) : BaseClass (pCohort) {
    joinCohortAs (CohortIndex ());
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDeviceFactory::~VDeviceFactory () {
    exitCohortAs (CohortIndex ());
}

/**************************
 **************************
 *****  Cohort Index  *****
 **************************
 **************************/

Vca::VCohortIndex const &Vca::VDeviceFactory::CohortIndex () {
    static VCohortIndex iIndex;
    return iIndex;
}

/*****************************
 *****************************
 *****  Cohort Instance  *****
 *****************************
 *****************************/

bool Vca::VDeviceFactory::Supply (Reference &rpInstance) {
    VCohort *pCohort = VCohort::Vca ();
    rpInstance.setTo (reinterpret_cast<ThisClass*> (pCohort->value (CohortIndex ())));
    if (rpInstance.isNil ())
	rpInstance.setTo (new ThisClass (pCohort));
    return rpInstance.isntNil ();
}


/**************************************
 **************************************
 *****  Device Manager Suppliers  *****
 **************************************
 **************************************/

bool Vca::VDeviceManager::Supply (DeviceManager::Reference &rpInstance, VCohort *pCohort) {
    rpInstance.setTo (static_cast<DeviceManager*> (pCohort->deviceManager ()));
    if (rpInstance.isNil ())
	rpInstance.setTo (new DeviceManager (pCohort));
    return rpInstance.isntNil ();
}

bool Vca::VDeviceManager::Supply (Reference &rpInstance, VCohort *pCohort) {
    DeviceManager::Reference pInstance;
    if (Supply (pInstance, pCohort))
	rpInstance.setTo (pInstance);
    return rpInstance.isntNil ();
}

bool Vca::VDeviceFactory::supply (DeviceManager::Reference&rpDeviceManager) {
    return VDeviceManager::Supply (rpDeviceManager, cohort ());
}


/******************************
 ******************************
 *****  Device Suppliers  *****
 ******************************
 ******************************/

/**************************
 *****  Pipe Devices  *****
 **************************/

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VDevice::Reference*		ppStdToPeer, // null -> null device
    VDevice::Reference*		ppStdToHere, // null -> null device
    VDevice::Reference*		ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
    char const*			pCommand
) {
    DeviceManager::Reference pDeviceManager;
    if (!supply (pDeviceManager))
	return false;

    OS::FarHandle
	hProcessStdin	(this, pDeviceManager),
	hProcessStdout	(this, pDeviceManager),
	hProcessErrout	(this, pDeviceManager);

//  Error output is going to standard output if ppErrToHere == ppStdToHere...
    bool const bErroutIsStdout = ppErrToHere == ppStdToHere;

    return pDeviceManager->supply (
	rStatus, ppStdToPeer, hProcessStdin, hProcessStdout, ppStdToHere
    ) && ( // supply an error device only when it's needed...
	bErroutIsStdout || pDeviceManager->supply (rStatus, hProcessErrout, ppErrToHere)
    ) && pDeviceManager->supply (
	rStatus,
	rpProcess,
	hProcessStdin,
	hProcessStdout,
	bErroutIsStdout ? hProcessStdout : hProcessErrout,
	pCommand
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VDevice::Reference&		rpStdToPeer,
    VDevice::Reference&		rpStdToHere,
    VDevice::Reference&		rpErrToHere,
    char const*			pCommand
) {
    return supply (rStatus, rpProcess, &rpStdToPeer, &rpStdToHere, &rpErrToHere, pCommand);
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpStdToHere,
    VDevice::Reference&		rpStdToPeer,
    VDevice::Reference&		rpErrToPeer
) {
    return supplyStdToHere (rStatus, rpStdToHere)
	&& supplyStdToPeer (rStatus, rpStdToPeer)
	&& supplyErrToPeer (rStatus, rpErrToPeer);
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpStdToHere,
    VDevice::Reference&		rpStdToPeer
) {
    return supplyStdToHere (rStatus, rpStdToHere)
	&& supplyStdToPeer (rStatus, rpStdToPeer);
}

bool Vca::VDeviceFactory::supplyStdToHere ( // stdin
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager)
	&& pDeviceManager->supplyStdToHere (rStatus, rpDevice);
}

bool Vca::VDeviceFactory::supplyStdToPeer ( // stdout
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager)
	&& pDeviceManager->supplyStdToPeer (rStatus, rpDevice);
}

bool Vca::VDeviceFactory::supplyErrToPeer (
    VkStatus &rStatus, VDevice::Reference &rpDevice
) {
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager)
	&& pDeviceManager->supplyErrToPeer (rStatus, rpDevice);
}


/*********************
 *****  Sockets  *****
 *********************/

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay,
    bool			bPassive
) {
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager) && pDeviceManager->supply (
	rStatus, rpDevice, rAddress, xProtocol, xType, bNoDelay, bPassive
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VDevice::Reference&		rpDevice,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay,
    bool			bPassive
) {
    VkSocketAddress iAddress; int xType, xProtocol;
    return iAddress.construct (
	pProtocol, pName, bPassive, &xType, &xProtocol, &rStatus
    ) && supply (rStatus, rpDevice, iAddress, xProtocol, xType, bNoDelay, bPassive);
}

bool Vca::VDeviceFactory::supply (
    VkStatus &rStatus, VDevice::Reference &rpDevice, char const *pName, bool bNoDelay
) {
    return supply (rStatus, rpDevice, pName, "-/stream", bNoDelay, false);
}


/**********************************
 **********************************
 *****  Device Use Suppliers  *****
 **********************************
 **********************************/

/*******************************
 *****  Pipe Byte Streams  *****
 *******************************/

/************************
 *----  Input File  ----*
 ************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpBS, rName);
}

bool Vca::VDeviceFactory::supply (
    VkStatus& rStatus, VBSProducer::Reference &rpBS, VString const &rName
) {
//  BS < file
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager) && pDeviceManager->supply (rStatus, rpBS, rName);
}

/*************************
 *----  Output File  ----*
 *************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpBS, rName);
}

bool Vca::VDeviceFactory::supply (
    VkStatus &rStatus, VBSConsumer::Reference &rpBS, VString const &rName
) {
//  BS > file
    DeviceManager::Reference pDeviceManager;
    return supply (pDeviceManager) && pDeviceManager->supply (rStatus, rpBS, rName);
}


/****************************
 *----  Standard Pipes  ----*
 ****************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference*	ppStdToPeer, // null -> null device
    VBSProducer::Reference*	ppStdToHere, // null -> null device
    VBSProducer::Reference*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
    char const*			pCommand
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpProcess, ppStdToPeer, ppStdToHere, ppErrToHere, pCommand
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference*	ppStdToPeer, // null -> null device
    VBSProducer::Reference*	ppStdToHere, // null -> null device
    VBSProducer::Reference*	ppErrToHere, // null -> null device, ==ppStdToHere -> stdout device
    char const*			pCommand
) {
    VDevice::Reference pStddevToPeer, pStddevToHere, pErrdevToHere;
    VDevice::Reference *const ppErrdevToHere = ppErrToHere == ppStdToHere ? &pStddevToHere : &pErrdevToHere;

    return supply (
	rStatus, rpProcess,
	ppStdToPeer ? &pStddevToPeer : 0,
	ppStdToHere ? &pStddevToHere : 0,
	ppErrToHere ? ppErrdevToHere : 0,
	pCommand
    ) && (
	!ppStdToPeer || pStddevToPeer->supply (*ppStdToPeer)
    ) && (
	!ppStdToHere || pStddevToHere->supply (*ppStdToHere)
    ) && (
	!ppErrToHere || ppErrdevToHere == &pStddevToHere || pErrdevToHere->supply (*ppErrToHere)
    );
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference&	rpStdToPeer,
    VBSProducer::Reference&	rpStdToHere,
    VBSProducer::Reference&	rpErrToHere,
    char const*			pCommand
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpProcess, rpStdToPeer, rpStdToHere, rpErrToHere, pCommand
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VProcess::Reference&	rpProcess,
    VBSConsumer::Reference&	rpStdToPeer,
    VBSProducer::Reference&	rpStdToHere,
    VBSProducer::Reference&	rpErrToHere,
    char const*			pCommand
) {
    return supply (rStatus, rpProcess, &rpStdToPeer, &rpStdToHere, &rpErrToHere, pCommand);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpStdToHere,
    VBSConsumer::Reference&	rpStdToPeer,
    VBSConsumer::Reference&	rpErrToPeer
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpStdToHere, rpStdToPeer, rpErrToPeer);
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpStdToHere,
    VBSConsumer::Reference&	rpStdToPeer,
    VBSConsumer::Reference&	rpErrToPeer
) {
    VDevice::Reference pStdToHere, pStdToPeer, pErrToPeer;
    return supply (rStatus, pStdToHere, pStdToPeer, pErrToPeer)
	&& pStdToHere->supply (rpStdToHere)
	&& pStdToPeer->supply (rpStdToPeer)
	&& pErrToPeer->supply (rpErrToPeer);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpStdToHere,
    VBSConsumer::Reference&	rpStdToPeer
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpStdToHere, rpStdToPeer);
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpStdToHere,
    VBSConsumer::Reference&	rpStdToPeer
) {
    VDevice::Reference pStdToHere, pStdToPeer;
    return supply (rStatus, pStdToHere, pStdToPeer)
	&& pStdToHere->supply (rpStdToHere)
	&& pStdToPeer->supply (rpStdToPeer);
}

bool Vca::VDeviceFactory::SupplyStdToHere (	//  stdin
    VkStatus &rStatus, VBSProducer::Reference &rpBS
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supplyStdToHere (rStatus, rpBS);
}

bool Vca::VDeviceFactory::supplyStdToHere (	//  stdin
    VkStatus &rStatus, VBSProducer::Reference &rpBS
) {
    VDevice::Reference pDevice;
    return supplyStdToHere (rStatus, pDevice) && pDevice->supply (rpBS);
}

bool Vca::VDeviceFactory::SupplyStdToPeer (	//  stdout
    VkStatus &rStatus, VBSConsumer::Reference &rpBS
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supplyStdToPeer (rStatus, rpBS);
}

bool Vca::VDeviceFactory::supplyStdToPeer (	//  stdout
    VkStatus &rStatus, VBSConsumer::Reference &rpBS
) {
    VDevice::Reference pDevice;
    return supplyStdToPeer (rStatus, pDevice) && pDevice->supply (rpBS);
}

bool Vca::VDeviceFactory::SupplyErrToPeer (	//  stderr
    VkStatus &rStatus, VBSConsumer::Reference &rpBS
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supplyErrToPeer (rStatus, rpBS);
}

bool Vca::VDeviceFactory::supplyErrToPeer (	//  stderr
    VkStatus &rStatus, VBSConsumer::Reference &rpBS
) {
    VDevice::Reference pDevice;
    return supplyErrToPeer (rStatus, pDevice) && pDevice->supply (rpBS);
}


/*********************************
 *****  Socket Byte Streams  *****
 *********************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpBSProducer, rpBSConsumer, rAddress, xProtocol, xType, bNoDelay
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, rAddress, xProtocol, xType, bNoDelay, false
    ) && pDevice->supply (rpBSProducer, rpBSConsumer);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpBSProducer, rpBSConsumer, pName, pProtocol, bNoDelay
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, pName, pProtocol, bNoDelay, false
    ) && pDevice->supply (rpBSProducer, rpBSConsumer);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    char const*			pName,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpBSProducer, rpBSConsumer, pName, bNoDelay);
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VBSProducer::Reference&	rpBSProducer,
    VBSConsumer::Reference&	rpBSConsumer,
    char const*			pName,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, pName, bNoDelay
    ) && pDevice->supply (rpBSProducer, rpBSConsumer);
}


/********************************
 *****  Socket Connections  *****
 ********************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VConnection::Reference&	rpConnection,
    VkSocketAddress const&	rAddress,
    int				xType,
    int				xProtocol,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpConnection, rAddress, xType, xProtocol, bNoDelay
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VConnection::Reference&	rpConnection,
    VkSocketAddress const&	rAddress,
    int				xType,
    int				xProtocol,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, rAddress, xType, xProtocol, bNoDelay, false
    ) && pDevice->supply (rpConnection);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VConnection::Reference&	rpConnection,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpConnection, pName, pProtocol, bNoDelay
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VConnection::Reference&	rpConnection,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, pName, pProtocol, bNoDelay, false
    ) && pDevice->supply (rpConnection);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus &rStatus, VConnection::Reference &rpConnection, char const *pName, bool bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpConnection, pName, bNoDelay);
}

bool Vca::VDeviceFactory::supply (
    VkStatus &rStatus, VConnection::Reference &rpConnection, char const *pName, bool bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (rStatus, pDevice, pName, bNoDelay) && pDevice->supply (rpConnection);
}


/************************
 *****   Listeners  *****
 ************************/

bool Vca::VDeviceFactory::Supply (
    VkStatus&			rStatus,
    VListener::Reference&	rpListener,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (
	rStatus, rpListener, rAddress, xProtocol, xType, bNoDelay
    );
}

bool Vca::VDeviceFactory::supply (
    VkStatus&			rStatus,
    VListener::Reference&	rpListener,
    VkSocketAddress const&	rAddress,
    int				xProtocol,
    int				xType,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, rAddress, xProtocol, xType, bNoDelay, true
    ) && pDevice->supply (rpListener);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus &			rStatus,
    VListener::Reference&	rpListener,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpListener, pName, pProtocol, bNoDelay);
}

bool Vca::VDeviceFactory::supply (
    VkStatus &			rStatus,
    VListener::Reference&	rpListener,
    char const*			pName,
    char const*			pProtocol,
    bool			bNoDelay
) {
    VDevice::Reference pDevice;
    return supply (
	rStatus, pDevice, pName, pProtocol, bNoDelay, true
    ) && pDevice->supply (rpListener);
}

bool Vca::VDeviceFactory::Supply (
    VkStatus &rStatus, VListener::Reference &rpListener, char const *pName, bool bNoDelay
) {
    Reference pFactory;
    return Supply (pFactory) && pFactory->supply (rStatus, rpListener, pName, bNoDelay);
}

bool Vca::VDeviceFactory::supply (
    VkStatus &rStatus, VListener::Reference &rpListener, char const *pName, bool bNoDelay
) {
    return supply (rStatus, rpListener, pName, "-/stream", bNoDelay);
}

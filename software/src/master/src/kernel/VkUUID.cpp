/*****  VkUUID Implementation  *****/

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

#include "VkUUID.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"


/*******************************************
 *******************************************
 *****  Platform Specific Definitions  *****
 *******************************************
 *******************************************/

typedef V::uuid_t::u32	u32;
typedef V::uuid_t::u16	u16;
typedef V::uuid_t::u8	u8;

/***********************************
 *****  DCE Compliant Systems  *****
 ***********************************/

#if defined(_WIN32)
#define USING_WINDOWS_DCE
#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")

namespace {
    uuid_t* UCast (V::uuid_t const &rUUID) {
	return (uuid_t*)(&rUUID);
    };
    uuid_t* UCast (V::uuid_t &rUUID) {
	return (uuid_t*)(&rUUID);
    };
}

#elif defined(__APPLE__) || defined(__linux__) || defined(sun)
#define USING_LIBUUID_DCE
#include <uuid/uuid.h>

namespace {
    u8* UCast (V::uuid_t const &rUUID) {
	return (u8*)(&rUUID);
    };
    u8* UCast (V::uuid_t &rUUID) {
	return (u8*)(&rUUID);
    };
}

#elif defined(_AIX) || defined(__hpux)
#define USING_LEGACY_DCE

extern "C" {
    void rpc_string_free	(unsigned char**, u32*);

    void uuid_create		(uuid_t*, u32*);
    void uuid_to_string		(uuid_t const*, unsigned char**, u32*);
    void uuid_from_string	(unsigned char const*, uuid_t*, u32*);
};


/***************************
 *****  Other Systems  *****
 ***************************/

#elif defined(__VMS)
//lif defined(__VMS) || defined (__linux__) || defined(sun) 
#define USING_OWN_IMPLEMENTATION

#ifdef __VMS

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#include <starlet.h>
#include <ssdef.h>
#include <stsdef.h>

typedef __char_ptr32 ifc_buf_t;

#else // !__VMS

typedef caddr_t ifc_buf_t;

#endif

typedef V::uuid_t::u32	u32;
typedef V::uuid_t::u16	u16;
typedef V::uuid_t::u8	u8;

#include "VpSocket.h"

#define get_socket_ioctl_data ioctl

/*---------------------------------
 *----  If UCX ever comes back...
 *---------------------------------
static int get_socket_ioctl_data (SOCKET iSD, unsigned int xOperation, __void_ptr32 pResult) {
    static bool bUCX = getenv ("tcpip$network") ? true : false;
    return bUCX ? ioctl (iSD, xOperation, pResult) : socket_ioctl (iSD, xOperation, pResult);
}
*----------------------------------*/

#if defined(sun)
#include <sys/sockio.h>
#endif
#include <net/if.h>
#include <net/if_arp.h>

#define ifreq_size(i) sizeof(struct ifreq)

extern "C" {
    long random ();
    void srandom (unsigned int seed);
}

PrivateFnDef void get_random_bytes(void *buf, unsigned int nbytes) {
    char *cp = (char *) buf;
    for (unsigned int i=0; i < nbytes; i++) 
      *cp++ = (char)((random () >> 16) & 0xFF);
}


PrivateFnDef bool get_node_id(unsigned char *node_id) {
    int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sd < 0)
	return false;

    char buf[1024];
    memset(buf, 0, sizeof(buf));

#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
    struct ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = static_cast<ifc_buf_t>(buf);
    if (get_socket_ioctl_data (sd, SIOCGIFCONF, static_cast<void*>(&ifc)) < 0) {
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	close(sd);
	return false;
    }

    int n = ifc.ifc_len;
    struct ifreq ifr;
    for (int i = 0; i < n; i+= ifreq_size(ifr) ) {
	struct ifreq *ifrp = (struct ifreq *)((char *) ifc.ifc_buf+i);
	strncpy(ifr.ifr_name, ifrp->ifr_name, IFNAMSIZ);
#if defined(SIOCGIFHWADDR)

#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
	if (get_socket_ioctl_data(sd, SIOCGIFHWADDR, static_cast<void*>(&ifr)) < 0)
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	    continue;
	unsigned char *a = (unsigned char *) &ifr.ifr_hwaddr.sa_data;

#elif defined(SIOCGARP)
	struct arpreq arp;
	arp.arp_pa.sa_family = AF_INET;
	arp.arp_ha.sa_family = AF_INET;
	((struct sockaddr_in*)&arp.arp_pa)->sin_addr.s_addr=
	  ((struct sockaddr_in*)(&ifrp->ifr_addr))->sin_addr.s_addr;

#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
	if (get_socket_ioctl_data(sd, SIOCGARP, static_cast<void*>(&arp)) < 0)
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	    continue;
	unsigned char *a = (unsigned char *) &arp.arp_ha.sa_data;

#elif defined(SIOCGENADDR)
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
	if (get_socket_ioctl_data(sd, SIOCGENADDR, static_cast<void*>(&ifr)) < 0)
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	    continue;
	unsigned char *a = (unsigned char *) ifr.ifr_enaddr;
#elif defined(SIOCRPHYSADDR)
	struct ifdevea iDeviceAddr;
	strncpy(iDeviceAddr.ifr_name, ifrp->ifr_name, IFNAMSIZ);
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
	if (get_socket_ioctl_data (sd, SIOCRPHYSADDR, static_cast<void*>(&iDeviceAddr)) < 0)
#ifdef __VMS
#pragma __pointer_size __restore
#endif
	    continue;
	unsigned char *a = (unsigned char *) iDeviceAddr.default_pa;
#else
    /*  No way to to get the hardware address...  */
	close(sd);
	return false;
#endif /* SIOCGENADDR */ /* SIOCGIFHWADDR */

	if (!a[0] && !a[1] && !a[2] && !a[3] && !a[4] && !a[5])
	    continue;
	if (node_id) {
	    memcpy(node_id, a, 6);
	    close(sd);
	    return true;
	}
    }
    close(sd);
    return false;
}


/* Assume that the gettimeofday() has microsecond granularity */
#define MAX_ADJUSTMENT 10

PrivateFnDef void get_clock(u32 *clock_high, u32 *clock_low, u16 *ret_clock_seq) {
	static int			adjustment = 0;
	static struct timeval		last = {0, 0};
	static u16			clock_seq;
	struct timeval 			tv;
	unsigned __int64		clock_reg;
	
try_again:
	gettimeofday(&tv, 0);
	if ((last.tv_sec == 0) && (last.tv_usec == 0)) {
		get_random_bytes(&clock_seq, sizeof(clock_seq));
		clock_seq &= (u16)0x1FFF;
		last = tv;
		last.tv_sec--;
	}
	if ((tv.tv_sec < last.tv_sec) ||
	    ((tv.tv_sec == last.tv_sec) &&
	     (tv.tv_usec < last.tv_usec))) {
		clock_seq = (u16)((clock_seq+1) & 0x1FFF);
		adjustment = 0;
		last = tv;
	} else if ((tv.tv_sec == last.tv_sec) &&
	    (tv.tv_usec == last.tv_usec)) {
		if (adjustment >= MAX_ADJUSTMENT)
			goto try_again;
		adjustment++;
	} else {
		adjustment = 0;
		last = tv;
	}
		
	clock_reg = tv.tv_usec*10 + adjustment;
	clock_reg += ((unsigned __int64) tv.tv_sec)*10000000;
	clock_reg += (((unsigned __int64) 0x01B21DD2) << 32) + 0x13814000;

	*clock_high = (u32)(clock_reg >> 32);
	*clock_low = (u32)clock_reg;
	*ret_clock_seq = clock_seq;
}
#endif	/* defined(sun) */


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

bool VkUUID::GetUUID (uuid_t &rUUID, char const *pString) {
#if defined(USING_LIBUUID_DCE)
    return 0 == uuid_parse (const_cast<char*>(pString), UCast(rUUID));

#elif defined(USING_LEGACY_DCE)
    u32 iStatus;
    uuid_from_string (reinterpret_cast<unsigned char const*>(pString), &rUUID, &iStatus);
    return iStatus == 0;

#elif defined(USING_WINDOWS_DCE)
    return RPC_S_OK == UuidFromString ((unsigned char*)pString, UCast(rUUID));

#elif defined(USING_OWN_IMPLEMENTATION)
    if (strlen(pString) != 36)
	return false;

    int i; char const *cp;
    for (i=0, cp = pString; i < 36; i++,cp++) {
	switch (i) {
	case 8:
	case 13:
	case 18:
	case 23:
	    if (*cp != '-')
		return false;
	    break;
	default:
	    if (!isxdigit(*cp))
		return false;
	    break;
	}
    }

    rUUID.time_low = (u32)strtoul(pString, NULL, 16);
    rUUID.time_mid = (u16)strtoul(pString+9, NULL, 16);
    rUUID.time_hi_and_version = (u16)strtoul(pString+14, NULL, 16);

    u16 clock_seq = (u16)strtoul (pString+19, NULL, 16);
    rUUID.clock_seq_hi_and_reserved = (u8)(clock_seq >> 8);
    rUUID.clock_seq_low = (u8)clock_seq;

    cp = pString+24;

    char buf[3];
    buf[2] = 0;
    for (i=0; i < 6; i++) {
	buf[0] = *cp++;
	buf[1] = *cp++;
	rUUID.node[i] = (u8)strtoul(buf, NULL, 16);
    }

    return true;

#else
    return false;

#endif
}


bool VkUUID::GetUUID (uuid_t &rUUID) {
#if defined(USING_LIBUUID_DCE)
    uuid_generate (UCast(rUUID));
    return true;

#elif defined(USING_LEGACY_DCE)
#if defined(_AIX)
    ::fprintf (stderr, "+++VkUUID::GetUUID: UUID Creation May Fail With A Segmentation Violation.\n");
#endif
    u32 iStatus = 0;
    uuid_create (&rUUID, &iStatus);
    return iStatus == 0;

#elif defined(USING_WINDOWS_DCE)
    switch (UuidCreate (UCast(rUUID))) {
    case RPC_S_OK:
    case RPC_S_UUID_LOCAL_ONLY:
	return true;

    default:
	break;
    }
    UuidCreateNil (UCast(rUUID));
    return true;

#elif defined(USING_LOCAL_IMPLEMENTATION)

#if defined (__VMS)
#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size __short
#endif
    unsigned int uid[4];
    unsigned int xStatus = sys$create_uid (static_cast<unsigned int*>(uid));
#ifdef __VMS
#pragma __pointer_size __restore
#endif
    if ($VMS_STATUS_SUCCESS (xStatus)) {
	memcpy (&rUUID, uid, sizeof (rUUID));
	return true;
    }
#endif // defined(__VMS)

    static bool notInitialized = true;
    static unsigned char node_id[6];

/* Seed and crank the random number generator, ...  */
    pid_t iPID = getpid ();
    uid_t iUID = getuid ();
    time_t iTime = time (0);
    unsigned int iSeed = (iPID << 16) ^ iUID ^ iTime;
    srandom(iSeed);

    for (int i = time(0) & 0x1F; i > 0; i--)
	random ();

/*  Obtain the node ID if it hasn't been obtained yet, ...  */
    if (notInitialized) {
	if (!get_node_id(node_id)) {
	    get_random_bytes(node_id, 6);
	/*
	 * Set multicast bit, to prevent conflicts
	 * with IEEE 802 addresses obtained from
	 * network cards
	 */
	    node_id[0] |= (u8)0x80;
	}
	notInitialized = false;
    }

    u32 clock_mid;
    u16 clock_seq;
    get_clock(&clock_mid, &rUUID.time_low, &clock_seq);

    rUUID.clock_seq_low = (u8)clock_seq;
    rUUID.clock_seq_hi_and_reserved = (u8)((clock_seq >> 8) | 0x80);

    rUUID.time_mid = (u16)clock_mid;
    rUUID.time_hi_and_version = (u16)((clock_mid >> 16) | 0x1000);
    memcpy(rUUID.node, node_id, 6);
    return true;

#else
    return false;

#endif
}


V::uuid_t VkUUID::ReturnUUID (
    unsigned int iD1,
    unsigned short iD2, unsigned short iD3,
    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
) {
    uuid_t iUUID;
    iUUID.time_low			= iD1;
    iUUID.time_mid			= iD2;
    iUUID.time_hi_and_version		= iD3;
    iUUID.clock_seq_hi_and_reserved	= iD4_0;
    iUUID.clock_seq_low			= iD4_1;
    iUUID.node[0]			= iD4_2;
    iUUID.node[1]			= iD4_3;
    iUUID.node[2]			= iD4_4;
    iUUID.node[3]			= iD4_5;
    iUUID.node[4]			= iD4_6;
    iUUID.node[5]			= iD4_7;

    return iUUID;
}

V::uuid_t VkUUID::ReturnUUID (char const *pString) {
    uuid_t iUUID;
    GetUUID (iUUID, pString);
    return iUUID;
}

V::uuid_t VkUUID::ReturnUUID () {
    uuid_t iUUID;
    GetUUID (iUUID);
    return iUUID;
}


VkUUID::VkUUID (uuid_t const &rUUID) : m_iUUID (rUUID) {
}

VkUUID::VkUUID (
    unsigned int   iD1,
    unsigned short iD2, unsigned short iD3,
    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
) : m_iUUID (ReturnUUID (iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7))
{
}

VkUUID::VkUUID (char const *pString) : m_iUUID (ReturnUUID (pString)) {
}

VkUUID::VkUUID () {
    memset (&m_iUUID, 0, sizeof (m_iUUID));
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VkUUID::GetString (VString &rString) const {
#if defined(USING_LIBUUID_DCE)
    uuid_unparse (UCast(m_iUUID), rString.storage (36));
    return true;

#elif defined(USING_LEGACY_DCE)
    unsigned char *pString = 0;
    u32 iStatus;
    uuid_to_string (&m_iUUID, &pString, &iStatus);
    if (iStatus)
	return false;
    rString.setTo ((char const*)pString);
    rpc_string_free (&pString, &iStatus);

    return true;

#elif defined(USING_WINDOWS_DCE)
    unsigned char *pString;
    if (RPC_S_OK != UuidToString (UCast(m_iUUID), &pString))
	return false;

    rString.setTo ((char const*)pString);
    RpcStringFree (&pString);
    return true;

#elif defined(USING_LOCAL_IMPLEMENTATION)
    rString.printf (
	"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	m_iUUID.time_low, m_iUUID.time_mid, m_iUUID.time_hi_and_version,
	m_iUUID.clock_seq_hi_and_reserved, m_iUUID.clock_seq_low,
	m_iUUID.node[0], m_iUUID.node[1], m_iUUID.node[2],
	m_iUUID.node[3], m_iUUID.node[4], m_iUUID.node[5]
    );

    return true;

#else
    return false;

#endif
}

bool VkUUID::GetString (VString &rString, const uuid_t &rUUID) {
    VkUUID dispUUID (rUUID);
    return dispUUID.GetString (rString);
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VkUUID::isNil () const {
#if defined(USING_LIBUUID_DCE)
    return uuid_is_null (UCast(m_iUUID));
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidIsNil (UCast(m_iUUID), &iStatus) ? true : false;
#else
    return 0 == m_iUUID.time_low
	&& 0 == m_iUUID.time_mid
	&& 0 == m_iUUID.time_hi_and_version
	&& 0 == m_iUUID.clock_seq_hi_and_reserved
	&& 0 == m_iUUID.clock_seq_low
	&& 0 == m_iUUID.node[0]
	&& 0 == m_iUUID.node[1]
	&& 0 == m_iUUID.node[2]
	&& 0 == m_iUUID.node[3]
	&& 0 == m_iUUID.node[4]
	&& 0 == m_iUUID.node[5];
#endif
}

bool VkUUID::lt (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) < 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidCompare (UCast(rUUID1), UCast(rUUID2), &iStatus) < 0;
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) < 0;
#endif
}

bool VkUUID::le (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) <= 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidCompare (UCast(rUUID1), UCast(rUUID2), &iStatus) <= 0;
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) <= 0;
#endif
}

bool VkUUID::eq (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) == 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidEqual (UCast(rUUID1), UCast(rUUID2), &iStatus) ? true : false;
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) == 0;
#endif
}

bool VkUUID::ne (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) != 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return !UuidEqual (UCast(rUUID1), UCast(rUUID2), &iStatus);
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) != 0;
#endif
}

bool VkUUID::ge (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) >= 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidCompare (UCast(rUUID1), UCast(rUUID2), &iStatus) >= 0;
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) >= 0;
#endif
}

bool VkUUID::gt (uuid_t const &rUUID1, uuid_t const &rUUID2) {
#if defined(USING_LIBUUID_DCE)
    return uuid_compare (UCast(rUUID1), UCast(rUUID2)) > 0;
#elif defined(USING_WINDOWS_DCE)
    RPC_STATUS iStatus;
    return UuidCompare (UCast(rUUID1), UCast(rUUID2), &iStatus) > 0;
#else
    return memcmp (&rUUID1, &rUUID2, sizeof (uuid_t)) > 0;
#endif
}


/*********************************
 *****  Byte Order Reversal  *****
 *********************************/

void VkUUID::reverseByteOrder (uuid_t &rUUID) {
    Vk_ReverseSizeable (&rUUID.time_low);
    Vk_ReverseSizeable (&rUUID.time_mid);
    Vk_ReverseSizeable (&rUUID.time_hi_and_version);
}

/*********************
 *****  Display  *****
 *********************/

void VkUUID::displayInfo (char const *pWhat, uuid_t const &rUUID) {
    VkUUID dispUUID (rUUID);
    dispUUID.displayInfo (pWhat);
}


void VkUUID::displayInfo (char const *pWhat) {
    VString uuidString;
    GetString(uuidString);
    printf ("%-20.20s: %s ", pWhat, uuidString.content ());
}

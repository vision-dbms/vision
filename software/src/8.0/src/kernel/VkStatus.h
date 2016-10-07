/*****  Vision Kernel Status Interface  *****/
#ifndef VkStatus_Interface
#define VkStatus_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

class VString;

#if defined(_WIN32)
typedef DWORD VkStatusCode;
#else
typedef int VkStatusCode;
#endif

/*************************
 *****  Definitions  *****
 *************************/

#if defined(_WIN32)
#define VkStatusCode_Success	NO_ERROR
#define VkStatusCode_Invalid	ERROR_INVALID_DATA
#else
#define VkStatusCode_Success	0
#define VkStatusCode_Invalid	EINVAL
#endif

enum VkStatusType {
    VkStatusType_Completed	= 1,
    VkStatusType_Closed,
    VkStatusType_Blocked	= 4,
    VkStatusType_Failed,
    VkStatusType_Unimplemented
};

class V_API VkStatus {
//  Description
public:
    static char const *DescriptionFor (VkStatusCode xCode);
    static char const *DescriptionFor (VkStatusType xType);

//  Construction/Initialization
public:
    VkStatus (VkStatus const &rOther) : m_xCode (rOther.m_xCode), m_xType (rOther.m_xType) {
    }
    VkStatus () {
	MakeSuccessStatus ();
    }

    bool MakeBlockedStatus ();		// ... returns true
    bool MakeBlockedStatus (		// ... returns true on success/blocked, false otherwise
	VkStatusCode xCode
    ) {
	return MakeErrorStatus (xCode, VkStatusType_Blocked);
    }

    bool MakeClosedStatus ();		// ... returns false

    bool MakeErrorStatus ();		// ... returns true on success/blocked, false otherwise
    bool MakeErrorStatus (		// ... returns true on success/blocked, false otherwise
	VkStatusCode xCode
    ) {
	return MakeErrorStatus (xCode, VkStatusType_Completed);
    }
    bool MakeErrorStatus (		// ... returns true on success/blocked, false otherwise
	VkStatusCode xCode, VkStatusType xSuccessType
    );


    bool MakeFailureStatus ();		// ... returns false

    bool MakeSuccessStatus ();		// ... returns true
    bool MakeSuccessStatus (		// ... returns true on success/blocked, false otherwise
	VkStatusCode xCode
    ) {
	return MakeErrorStatus (xCode, VkStatusType_Completed);
    }

    bool MakeUnimplementedStatus ();	// ... returns false

//  Access
public:
    VkStatusCode Code () const {
	return m_xCode;
    }

    char const *CodeDescription () const {
	return DescriptionFor (m_xCode);
    }

    VkStatusType Type () const {
	return m_xType;
    }
    char const *TypeDescription () const {
	return DescriptionFor (m_xType);
    }

    void getDescription (VString &rText) const;

//  Query
public:
    bool isBlocked () const {
	return m_xType == VkStatusType_Blocked;
    }
    bool isClosed () const {
	return m_xType == VkStatusType_Closed;
    }
    bool isCompleted () const {
	return m_xType == VkStatusType_Completed;
    }
    bool isFailed () const {
	return m_xType == VkStatusType_Failed;
    }
    bool isUnimplemented () const {
	return m_xType == VkStatusType_Unimplemented;
    }

    bool isntBlocked () const {
	return !isBlocked ();
    }
    bool isntClosed () const {
	return !isClosed ();
    }
    bool isntCompleted () const {
	return !isCompleted ();
    }
    bool isntFailed () const {
	return !isFailed ();
    }

//  Update
public:
    void setTo (VkStatus const &rStatus);

//  State
protected:
    VkStatusType m_xType;
    VkStatusCode m_xCode;
};


#endif

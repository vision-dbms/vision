/*****  Vision Kernel Event Interface  *****/
#ifndef VkEvent_Interface
#define VkEvent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca.h"

#include "VTransient.h"

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    class ITrigger;
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

#if defined(_WIN32)
#define Vk_InfiniteWait		INFINITE
#else
#define Vk_InfiniteWait		((size_t)UINT_MAX)
#endif


/*********************
 *********************
 *****  VkEvent  *****
 *********************
 *********************/

class VkEvent : public VTransient {
    DECLARE_FAMILY_MEMBERS (VkEvent, VTransient);

//  Friends
    friend class VkEventSet;

//  Handler Holder
public:
    typedef VReference<Vca::ITrigger> HandlerHolder;

//  State
public:
    enum State {
	State_Failed,
	State_Signaled,
	State_AcceptWait,
	State_ReadWait,
	State_WriteWait
    };

//  State Name
public:
    static char const *nameFor (State xState);

//  Construction
public:
    VkEvent (HANDLE hEvent);

//  Destruction
public:
    ~VkEvent ();

//  Access
public:
    HANDLE handle () const {
	return m_hEvent;
    }
    State state () const {
	return m_xState;
    }
    char const *stateName () const {
	return nameFor (m_xState);
    }

//  Query
public:
    bool isSignaled (size_t sTimeout);

    bool isTriggered () const {
	return m_bTriggered;
    }

//  Control
public:
    void callHandler ();

    void claimHandler (HandlerHolder &rEHH, State xState);
    void clearHandler (HandlerHolder &rEHH, State xState);
    void clearHandler (HandlerHolder &rEHH) {
	clearHandler (rEHH, m_xState);
    }

    void setStateTo (State xState) {
	m_xState = xState;
    }
    void setTrigger () {
	m_bTriggered = true;
    }

//  State
protected:
    State		m_xState;
    HANDLE		m_hEvent;
    HandlerHolder	m_iHH;
    VkEvent*		m_pPredecessor;
    VkEvent*		m_pSuccessor;
    bool		m_bTriggered;
};


/************************
 ************************
 *****  VkEventSet  *****
 ************************
 ************************/

class VkEventSet {
//  Construction
public:
    VkEventSet () : m_sEventSet (0) {
    }

//  Destruction
public:
    ~VkEventSet () {
    }

//  Update
public:
    unsigned int AddActiveEvents ();

    bool Add (VkEvent *pEvent);

    void clear () {
	m_sEventSet = 0;
    }

//  Access/Query
public:
    size_t size () const {
	return m_sEventSet;
    }
    bool isEmpty () const {
	return 0 == m_sEventSet;
    }
    bool isntEmpty() const {
	return 0 != m_sEventSet;
    }

//  Selection
public:
    enum Got {
	Got_Event,
	Got_EmptySet,
	Got_Timeout,
	Got_Message,
	Got_Error
    };
    Got getNextEvent (size_t sTimeout, VkEvent *&rpEvent);

    VkEvent *getNextTriggeredEvent (size_t sTimeout);

private:
    VkEvent *remove (unsigned int xEvent);

//  State
protected:
    size_t			m_sEventSet;
    VkEvent*			m_pEvent[MAXIMUM_WAIT_OBJECTS];
#if defined(_WIN32)
    HANDLE			m_hEvent[MAXIMUM_WAIT_OBJECTS];
#endif
};

#endif

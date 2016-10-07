/*****  Vca_VEventRecorder Implementation  *****/

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

#include "Vca_VEventRecorder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************************************
 *************************************************
 *****                                       *****
 *****  Vca::VEventRecorder::InceptionEvent  *****
 *****                                       *****
 *************************************************
 *************************************************/

namespace Vca {
    /**
     * Event generated upon VEventRecorder creation. Should never be dismissed.
     */
    class VEventRecorder::InceptionEvent : public VEvent {
	DECLARE_CONCRETE_RTTLITE (InceptionEvent, VEvent);

    //  Construction
    public:
	InceptionEvent () {
	}

    //  Destruction
    private:
	~InceptionEvent () {
	}

    //  Lifetime
    private:
	bool dismissed_() const {
	    return false;
	}
    };
}


/*********************************
 *********************************
 *****                       *****
 *****  Vca::VEventRecorder  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VEventRecorder::VEventRecorder (event_sink_gofer_t* pEventSinkGofer) :
    m_pEventSinkGofer (pEventSinkGofer),
    m_pEventListHead (new InceptionEvent ()),
    m_pEventListTail(m_pEventListHead) {
    traceInfo ("Creating VEventRecorder");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VEventRecorder::~VEventRecorder () {
    traceInfo ("Destroying VEventRecorder");
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VEventRecorder::onEventSink (IEventSink* pEventSink) {
}

/********************************
 ********************************
 *****  Event Registration  *****
 ********************************
 ********************************/

void Vca::VEventRecorder::registerEvent(VEvent *pEvent) {
    m_pEventListTail->append(m_pEventListTail, pEvent);
}

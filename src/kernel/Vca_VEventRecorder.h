#ifndef Vca_VEventRecorder_Interface
#define Vca_VEventRecorder_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VEvent.h"

#include "Vca_VGoferInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IEventSink.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VEventRecorder : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VEventRecorder, VRolePlayer);

    //  Aliases
    public:
	typedef VGoferInterface<IEventSink> event_sink_gofer_t;

    //  InceptionEvent
    public:
	class InceptionEvent;

    //  Construction
    public:
	VEventRecorder (event_sink_gofer_t* pEventSinkGofer);

    //  Destruction
    protected:
	~VEventRecorder ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  Event Registration
    public:
        /**
         * Registers an event with this VEventRecorder.
         *
         * @param pEvent the event to be registered.
         */
        void registerEvent(VEvent *pEvent);

    //  Callbacks
    private:
	void onEventSink (IEventSink* pEventSink);

    //  State
    private:
	/** A gofer for the event sink used to process the events recorded by this VEventRecorder.  */
	event_sink_gofer_t::Reference const m_pEventSinkGofer;

        /** The head of our event list. */
	VEvent::Reference m_pEventListHead;

        /** The tail of our event list. */
        VEvent::Reference m_pEventListTail;
    };
}


#endif

#ifndef Vca_IEventSink_Interface
#define Vca_IEventSink_Interface

#ifndef Vca_IEventSink
#define Vca_IEventSink extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IEvent.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IEventSink)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
    * @class Vca::IEventSink
    *
    * Interface representing an event receiver.  Provides the receiver with knowledge of the event's
    * occurrence and inception timestamp.  Implementations of this interface can query the event for
    * additional information.
    */
    VcaINTERFACE (IEventSink, IClient)
	/**
	 *  Receive an event.
	 *
	 *  @param p1 The IEvent interface for the original event.
	 *  @param p2 The IEvent interface for the source of this information.
	 *  @param p3 The time of occurrence of the event.
	 *  @param p4 A brief description of the event.
	 */
	VINTERFACE_METHOD_4 (OnEvent, IEvent*, IEvent*, V::VTime const&, VString const&);

	/**
	 *  Receive an end event.
	 *
	 *  @param p1 The IEvent interface for the original event.
	 *  @param p2 The IEvent interface for the source of this information.
	 *  @param p3 The time of occurrence of the event.
	 */
	VINTERFACE_METHOD_3 (OnEndEvent, IEvent*, IEvent*, V::VTime const&);

	/**
	 *  Receive an error event.
	 *
	 *  @param p1 The IEvent interface for the original event.
	 *  @param p2 The IEvent interface for the source of this information.
	 *  @param p3 The time of occurrence of the event.
	 *  @param p4 The IError representing the error.
	 *  @param p5 A brief ASCII description of the error.
	 */
	VINTERFACE_METHOD_5 (OnErrorEvent, IEvent*, IEvent*, V::VTime const&, IError*, VString const&);

	/**
	 *  Receive a failure (end + error)  event.
	 *
	 *  @param p1 The IEvent interface for the original event.
	 *  @param p2 The IEvent interface for the source of this information.
	 *  @param p3 The time of occurrence of the event.
	 *  @param p4 The IError representing the error.
	 *  @param p5 A brief ASCII description of the error.
	 */
	VINTERFACE_METHOD_5 (OnFailureEvent, IEvent*, IEvent*, V::VTime const&, IError*, VString const&);

    VINTERFACE_END

    VINTERFACE_ROLE (IEventSink, IClient)
	VINTERFACE_ROLE_4 (OnEvent, IEvent*, IEvent*, V::VTime const&, VString const&);
	VINTERFACE_ROLE_3 (OnEndEvent, IEvent*, IEvent*, V::VTime const&);
	VINTERFACE_ROLE_5 (OnErrorEvent, IEvent*, IEvent*, V::VTime const&, IError*, VString const&);
	VINTERFACE_ROLE_5 (OnFailureEvent, IEvent*, IEvent*, V::VTime const&, IError*, VString const&);
    VINTERFACE_ROLE_END
}


#endif

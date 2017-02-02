#ifndef Vca_VEvent_Interface
#define Vca_VEvent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IEvent.h"

#include "V_VTime.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Event descriptor class.
     */
    class Vca_API VEvent : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VEvent, VRolePlayer);

    //  Construction
    protected:
	VEvent ();

    //  Destruction
    protected:
	~VEvent ();

    //  Roles
    public:
	using BaseClass::getRole;

    /** @name IEventImplementation IEvent Implementation
     *  VEvent supports the basic capabilities of IEvent.  Subclasses of VEvent are free to support
     *  additional interfaces derived from IEvent to expose additional details.  VRolePlayer's interface
     *  query algorithm will respond with the subclass' IEvent specialization if one exists.
     *@{
     */
    private:
	VRole<ThisClass,IEvent> m_pIEvent;
    public:
	void getRole (IEvent::Reference& rpRole) {
	    m_pIEvent.getRole (rpRole);
	}

    //  Methods
    public:
	void GetDescription (IEvent* pRole, IVReceiver<VString const&>* pResultReceiver);
	void GetTimestamp (IEvent* pRole, IVReceiver<V::VTime const&>* pResultReceiver);
	void GetSSID (IEvent* pRole, IVReceiver<ssid_t>* pClient);
    //@}

    //  Access
    protected:
        /**
         * Virtual overridden by VEvent subclasses to return a brief description of this event.
	 * The default implementation of getDescription_ does nothing.
         *
         * @param rsDescription the VString that will receive a brief description of this event.
         */
	virtual void getDescription_(VString& rsDescription) const;
    public:
        /**
         * Used to retrieve a brief description of this event.
         *
         * @param rsDescription the VString that will receive a brief description of this event.
         */
	void getDescription (VString& rsDescription) const {
	    getDescription_(rsDescription);
	}

        /**
         * Used to retrieve the timestamp of this event.
         *
         * @return the timestamp of this event.
         */
	V::VTime const& timestamp () const {
	    return m_iTimestamp;
	}

    //  State
    private:
        /** The time at which this event occurred. */
	V::VTime const m_iTimestamp;
    };
}


#endif

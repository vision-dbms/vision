#ifndef Vca_VInterfaceEKG_Interface
#define Vca_VInterfaceEKG_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VTriggerTicket.h"

#include "Vca_ITrigger.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Triggers callbacks on another object when a particular interface is disconnected.
     * StakeHolder_T is the class of the object that callbacks are triggered on.
     * Interface_T is the type of interface that's being monitored.
     * In order for monitoring to continue, the instance of VInterfaceEKG responsible for initiating said monitoring must still be alive. In other words, keep a reference to the VInterfaceEKG after setting it up!
     */
    template <typename StakeHolder_T, typename Interface_T> class VInterfaceEKG : public VRolePlayer {
    public:
        typedef VInterfaceEKG<StakeHolder_T, Interface_T> this_t;
        DECLARE_CONCRETE_RTTLITE (this_t, VRolePlayer);

    //  Aliases
    public:
        typedef Interface_T interface_t;

        typedef StakeHolder_T stakeholder_t;
        typedef void (StakeHolder_T::*stakeholder_member_t)();

    //  Construction
    public:
        /**
         * @param pStakeHolder the instance of the stakeholder on which to make the callback.
         * @param pStakeHolderMember the method address on pStakeHolder to invoke. Expected to be parameter-less.
         * @param pInterface the interface instance that we're monitoring for life.
         */
        VInterfaceEKG (
            stakeholder_t* pStakeHolder, stakeholder_member_t pStakeHolderMember, interface_t *pInterface = 0
        ) : m_pITrigger (this), m_pStakeHolder(pStakeHolder), m_pStakeHolderMember (pStakeHolderMember) {
	    if (pInterface) {
		retain (); {
		    monitor (pInterface);
		} untain ();
	    }
        }

    //  Destruction
    protected:
        ~VInterfaceEKG () {
        }

    //  Roles
    public:
        using BaseClass::getRole;

    //  ITrigger Role
    private:
        VRole<ThisClass,ITrigger> m_pITrigger;
    public:
        void getRole (ITrigger::Reference& rpRole) {
            m_pITrigger.getRole (rpRole);
        }

    //  ITrigger Callbacks
    public:
        void Process (ITrigger* pRole) {        //  ... called when interface is un-restorably disconnected
            if (m_pStakeHolder) {
		(m_pStakeHolder.referent()->*m_pStakeHolderMember)();
                m_pTicket.clear ();
            }
        }

    //  Access
    public:
        interface_t* interfase () const {
            return m_pInterface;
        }
        stakeholder_t* stakeHolder () const {
            return m_pStakeHolder;
        }

    //  Monitor
    public:
        /**
         * Commence monitoring on a given interface.
         * First cancels monitoring on any existing interface that's being monitored by this VInterfaceEKG, then begins monitoring another (given) interface.
         * 
         * @param pInterface the interface to begin monitoring.
         */
        void monitor (interface_t *pInterface) {
            cancel ();

            m_pInterface.setTo (pInterface);
	    if (pInterface) {
		ITrigger::Reference pTrigger;
		getRole (pTrigger);
		pInterface->requestNoRouteToPeerCallback (m_pTicket, pTrigger);
	    }
        }

        /**
         * To be called when monitoring of the given interface is no longer necessary/useful.
         */
        void cancel () {
            if (m_pTicket) {
                m_pTicket->cancel ();
                m_pTicket.clear ();
            }
        }

    //  State
    private:
        const typename stakeholder_t::Reference m_pStakeHolder;
        const stakeholder_member_t              m_pStakeHolderMember;
        typename interface_t::Reference         m_pInterface;
        VTriggerTicket::Reference               m_pTicket;
    };
}


#endif

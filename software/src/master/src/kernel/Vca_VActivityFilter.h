#ifndef Vca_VActivityFilter_Interface
#define Vca_VActivityFilter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivitySink.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VGoferInstance.h"
#include "Vca_VTimer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     *  An activity sink that finds and forwards activities that encounter
     *  errors or do not finish when expected.
     */
    class Vca_API VActivityFilter : public VActivitySink {
	DECLARE_CONCRETE_RTTLITE (VActivityFilter, VActivitySink);

    //  Aliases
    public:
	typedef VGoferInstance<IActivitySink*> consumer_gofer_t;

    //  Constants
    public:
	static VTimer::microseconds_t g_sHarvestingInterval;

    //********************************************************************************************/
    //  Harvester
    public:
	class Harvester : public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Harvester, VRolePlayer);

	//  Aliases
	public:
	    typedef Reference HarvesterReference;

	//********************************************************************************************/
	//  Item
	public:
	    class Item : public virtual VReferenceable {
		DECLARE_ABSTRACT_RTTLITE (Item, VReferenceable);

		friend class Harvester;

	    //  Construction
	    protected:
		Item ();

	    //  Destruction
	    protected:
		~Item ();

	    //  Access
	    protected:
		virtual void getDescription_(VString& rResult) const;
	    public:
		void getDescription (VString& rResult) const;

		ThisClass* predecessor () const {
		    return m_pPredecessor;
		}
		ThisClass* successor () const {
		    return m_pSuccessor;
		}

	    //  Linkage
	    private:
		bool add (ThisClass* pNewItem);
		bool added (ThisClass* pNewItem, Reference &rpNextItem);
	    private:
		void attachHere (Harvester* pHarvester, Reference& rpListHead, Reference& rpPredecessor);
		void detachFrom (Harvester* pHarvester, Reference& rpListHead, Reference& rpListTail);
	    public:
		void detach ();

	    //  Ripening
	    public:
		virtual bool ripenedBy (ThisClass const* pItem, Reference &rpNextItem) = 0;
		virtual bool ripenedBy (VTime const& rTime, Reference &rpNextItem) = 0;

		virtual bool ripensBefore (Item const* pItem) const = 0;
		virtual bool ripensAfter  (Item const* pItem) const = 0;

		virtual bool ripensBefore (VTime const& rTime) const = 0;
		virtual bool ripensAfter  (VTime const& rTime) const = 0;

		virtual bool startedBefore (VTime const& rTime) const = 0;
		virtual bool startedAfter  (VTime const& rTime) const = 0;

	    //  State
	    private:
		HarvesterReference	m_pHarvester;
		Reference		m_pPredecessor;
		Reference		m_pSuccessor;
	    };
	    friend class Item;

	//********************************************************************************************/
	//  Construction
	public:
	    Harvester ();

	//  Destruction
	private:
	    ~Harvester () {
	    }

	//  Query
	public:
	    bool isEmpty () const {
		return m_pItemListHead.isNil ();
	    }
	    bool isntEmpty () const {
		return m_pItemListHead.isntNil ();
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
	    void Process (ITrigger* pRole);

	//  Linkage
	public:
	    void attach (Item* pItem, VTimer* pHarvestTimer);
	    void detach (Item* pItem);
	private:
	    void insert (Item* pNewItem, Item::Reference& rpPredecessor);

	//  Harvest Management
	private:
	    void scheduleNextHarvest ();

	//  State
	private:
	    VTimer::Reference m_pHarvestTimer;
	    Item::Reference	m_pItemListHead;
	    Item::Reference	m_pItemListTail;
	};
	typedef Harvester::Item Item;

    //********************************************************************************************/
    //  ActivityMonitor
    public:
	class ActivityMonitor;

    //********************************************************************************************/
    //  Construction
    private:
	VTimer* HarvestTimer ();
    public:
	VActivityFilter (consumer_gofer_t* pConsumerGofer);

    //  Destruction
    private:
	~VActivityFilter ();

    //  Monitoring
    public:
	void createMonitorFor (VActivity* pActivity) const OVERRIDE;

    //  State
    private:
	consumer_gofer_t::Reference	const m_pConsumerGofer;
	Harvester::Reference		const m_pHarvester;
	VTimer::Reference		const m_pHarvestTimer;
    };
}


#endif

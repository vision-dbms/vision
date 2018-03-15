/*****  Vxa_VTicketManager Implementation  *****/

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

#include "Vxa_VTicketProducer.h"
#include "Vxa_VTicketConsumer.h"

#include "VkMapOf.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkMapOf.h"
#include "VkUUID.h"

#include "Vxa_VCollectableCollection.h"
#include "Vxa_VCollectableObject.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vxa::VTicketRegistry  *****
 *****                        *****
 **********************************
 **********************************/

namespace {
    using namespace Vxa;

/**********************************
 *---- class VTicketRegistry  ----*
 **********************************/
    class VTicketRegistry {
    //  class Entry
    public:
        class Entry : public VReferenceable {
            DECLARE_CONCRETE_RTTLITE (Entry, VReferenceable);

        //  Construction
        public:
            Entry (VString const &rUUID, cluster_t *pCluster);

        //  Destruction
        private:
            ~Entry () {
            }

        //  State
        private:
            VString              const m_iUUID;
            cluster_t::Reference const m_pCluster;
            cardinality_t m_cOutstandingTickets;
            cardinality_t m_cOutstandingHandles;
        };
        typedef VkMapOf<VString,VString const&,VString const&,Entry::Reference> EntryMap;

    //  Construction
    public:
        VTicketRegistry () {
        }

    //  Destruction
    public:
        ~VTicketRegistry () {
        }

    //  Ticket Production
    public:
        bool getTicket (
            VString &rTicket, VString &rUUID, cluster_t *pCluster, cluster_index_t xObject, bool bSingleUse
        ) {
            rTicket << "<!*:" << rUUID << ":" << xObject << (bSingleUse ? ":T" : ":H") << "!*!>";
            return true;
        }
    };
/*----------------*/
    VTicketRegistry::Entry::Entry (
        VString const &rUUID, cluster_t *pCluster
    ) : m_iUUID (rUUID), m_pCluster (pCluster), m_cOutstandingTickets (0), m_cOutstandingHandles (0) {
    }

/*----------------*/
    VTicketRegistry &Registry () {
        static VTicketRegistry g_iTicketRegistry;
        return g_iTicketRegistry;
    }
}


/**********************************
 **********************************
 *****                        *****
 *****  Vxa::VTicketProducer  *****
 *****                        *****
 **********************************
 **********************************/

/*******************************
 *******************************
 *****  Ticket Generation  *****
 *******************************
 *******************************/

bool Vxa::VTicketProducer::readyToUse () {
    return m_iUUID.isntEmpty () || VkUUID::GetString (m_iUUID, VkUUID::ReturnUUID ());
}

bool Vxa::VTicketProducer::getTicket (
    VString &rTicket, cluster_t *pCluster, cluster_index_t xObject, bool bSingleUse
) {
    return readyToUse () && Registry ().getTicket (rTicket, m_iUUID, pCluster, xObject, bSingleUse);
}


/**********************************
 **********************************
 *****                        *****
 *****  Vxa::VTicketConsumer  *****
 *****                        *****
 **********************************
 **********************************/

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

namespace {
    using namespace Vxa;

    class Consumer : public VTicketConsumer {
        DECLARE_CONCRETE_RTTLITE (Consumer, VTicketConsumer);

    //  Construction
    public:
        Consumer () {
        }

    //  Destruction
    private:
        ~Consumer () {
        }

    //  Use
    public:
        virtual bool getObject (VCollectableObject* &rpObject, VString const &rObjectTicket) OVERRIDE {
            return getObject (rpObject);
        }
        virtual bool getObject (VCollectableObject* &rpObject) OVERRIDE {
            rpObject = m_pObject;
            return m_pObject.isntNil ();
        }

    //  State
    private:
        VCollectableObject::Reference m_pObject;
    };
}

bool Vxa::VTicketConsumer::Get (Reference &rpTicketConsumer) {
    if (rpTicketConsumer.isNil ())
        rpTicketConsumer.setTo (new Consumer ());
    return rpTicketConsumer.isntNil ();
}

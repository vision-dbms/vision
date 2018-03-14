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

/************************
 *****  Supporting  *****
 ************************/

#include "VkUUID.h"

#include "Vxa_VCollectableObject.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vxa::VTicketManager  *****
 *****                       *****
 *********************************
 *********************************/

namespace {
    using namespace Vxa;

    bool GetUUID (VString &rUUID) {
        return rUUID.isntEmpty () || VkUUID::GetString (rUUID, VkUUID::ReturnUUID ());
    }
    bool GetTicket (
        VString &rTicket, VString &rUUID, cluster_t *pCluster, cluster_index_t xObject, bool bSingleUse
    ) {
        if (!GetUUID (rUUID))
            return false;
        rTicket << "<!*:" << rUUID << ":" << xObject << (bSingleUse ? ":T" : ":H") << "!*!>";
        return true;
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

bool Vxa::VTicketProducer::getTicket (
    VString &rTicket, cluster_t *pCluster, cluster_index_t xObject, bool bSingleUse
) {
    return GetTicket (rTicket, m_iUUID, pCluster, xObject, bSingleUse);
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

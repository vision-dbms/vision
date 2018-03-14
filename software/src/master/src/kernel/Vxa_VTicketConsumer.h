#ifndef Vxa_VTicketConsumer_Interface
#define Vxa_VTicketConsumer_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCollectableObject;

    class Vxa_API VTicketConsumer : public VReferenceable {
        DECLARE_ABSTRACT_RTTLITE (VTicketConsumer, VReferenceable);

    //  Instantiation
    public:
        static bool Get (Reference &rpTicketConsumer);

    //  Construction/Destruction
    protected:
        VTicketConsumer () {
        }
        ~VTicketConsumer () {
        }

    //  Use
    public:
        template <typename object_value_t> bool getObject (
            object_value_t &rpObject, VString const &rObjectTicket
        ) {
            VCollectableObject *pObject = 0;
            return getObject (pObject, rObjectTicket)
                && (rpObject = dynamic_cast<object_value_t>(pObject));
        }
        template <typename object_value_t> bool getObject (
            object_value_t &rpObject
        ) {
            VCollectableObject *pObject = 0;
            return getObject (pObject)
                && (rpObject = dynamic_cast<object_value_t>(pObject));
        }

        virtual bool getObject (VCollectableObject* &rpObject, VString const &rObjectTicket) = 0;
        virtual bool getObject (VCollectableObject* &rpObject) = 0;
    };
} //  namespace Vxa


#endif

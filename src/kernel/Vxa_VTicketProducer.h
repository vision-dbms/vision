#ifndef Vxa_VTicketProducer_Interface
#define Vxa_VTicketProducer_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VString.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCollectableCollection;

    class Vxa_API VTicketProducer {
    //  Aliases
    public:
        typedef VCollectableCollection cluster_t;
        typedef cardinality_t          cluster_index_t;

    //  Construction
    public:
        VTicketProducer () {
        }

    //  Destruction
    public:
        ~VTicketProducer () {
        }

    //  Use
    public:
        bool getTicketKey (VString &rKey) const;
        bool getTicket (
            VString &rTicket, cluster_t *pCluster, cluster_index_t xObject, bool bSingleUse
        ) const;

    //  State
    private:
        VString mutable m_iKey;
    };
} //  namespace Vxa


#endif

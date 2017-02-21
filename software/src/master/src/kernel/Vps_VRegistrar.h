// #pragma once  /* BUILD directive - do not remove this line */

#ifndef _H_VPS_VREGISTRAR
#define _H_VPS_VREGISTRAR

#include "Vps.h"

namespace Vca {
    namespace Vps {
      class VRelayPublication;
      class Vps_API RelayPublicationRegistrar {
        public:
        virtual ~RelayPublicationRegistrar();
        virtual void install(Vca::Vps::VRelayPublication * ) = 0;
      }; 
    }
}

#endif /* _H_VPS_VREGISTRAR */

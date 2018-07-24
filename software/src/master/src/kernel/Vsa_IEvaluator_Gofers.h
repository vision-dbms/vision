/**
 * @file
 * Declaration for collection of gofers for IEvaluator interface instances.
 */

#ifndef Vsa_IEvaluator_Gofers_Interface
#define Vsa_IEvaluator_Gofers_Interface

/*************************
 *****  Definitions  *****
 *************************/

#ifndef Vsa_IEvaluator_Gofers
#define Vsa_IEvaluator_Gofers extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca.h"
#include "Vca_VGoferInterface.h"
#include "Vca_IPipeSource.h"
#include "Vca_VInstanceOfInterface.h"
#include "Vsa_IEvaluator.h"

namespace Vsa {
    DECLARE_GOFER_INTERFACE (Vsa_IEvaluator_Gofers, Vsa_API, IEvaluator)

    namespace IEvaluator_Gofer {

        /** Base class for all IEvaluator_Gofer classes. */
        typedef Vca::VGoferInterface<IEvaluator> gofer_base_t;

        /** IEvaluator receiver type. */
        typedef IVReceiver<IEvaluator*> IEvaluatorSink;

        /** IEvaluator data source type. */
        typedef Vca::IDataSource<IEvaluator*> IEvaluatorSource;

        /** Gofer for IEvaluators given an IPipeSource. */
        class Vsa_API FromPipeSource : public gofer_base_t {
            DECLARE_CONCRETE_RTTLITE (FromPipeSource, gofer_base_t);

        private:
            typedef Vca::VInstanceOfInterface<Vca::IPipeSource> PipeSourceInstance;

        // Construction
        public:
            /**
             * Default constructor expects IPipeSource.
             *
             * @param p the IPipeSource (or a gofer for one) to use to fetch an IEvaluator.
             */
            template <typename ipipesource_t> FromPipeSource (ipipesource_t p) : m_pIPipeSource (p), m_pIEvaluatorSink (this) {}

        // Destruction
        private:
            /** Empty destructor restricts scope. */
            ~FromPipeSource () {}

        /// @name Gofer callbacks
        //@{
        private:
            virtual void onNeed () OVERRIDE;
            virtual void onData () OVERRIDE;
        //@}

        /// @name IEvaluatorSink implementation.
        //@{
        public:
            /**
             * Used to retrieve the IPipeSource role for this VEvaluatorSinkWaiter.
             *
             * @param pRole the reference by which the IPipeSource role should be returned.
             */
            void getRole (IEvaluatorSink::Reference &pRole) {
                m_pIEvaluatorSink.getRole (pRole);
            }

            /**
             * Implements IEvaluatorSink::OnData().
             */
            void OnData (IEvaluatorSink *pRole, IEvaluator *pEvaluator);

        private:
            /** IEvaluatorSink role. */
            Vca::VRole<ThisClass,IEvaluatorSink> m_pIEvaluatorSink;
        //@}


        // State
        private:
            /** The IPipeSource that an IEvaluator will be fetched from. */
            PipeSourceInstance m_pIPipeSource;
        };

    }

}

#endif


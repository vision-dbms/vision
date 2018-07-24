#ifndef Vsa_IEvaluatorPool_Ex3_Interface
#define Vsa_IEvaluatorPool_Ex3_Interface

#ifndef Vsa_IEvaluatorPool_Ex3
#define Vsa_IEvaluatorPool_Ex3 extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPool_Ex2.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool_Ex3)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool_Ex3, IEvaluatorPool_Ex2)
        VINTERFACE_ROLE_3 (TakeWorkerOffline, VString const &, bool, IVReceiver<VString const &>*);
        VINTERFACE_ROLE_3 (MakeWorkerOnline, VString const &, bool, IVReceiver<VString const &>*);
        VINTERFACE_ROLE_4 (EvaluateUsingOfflineWorker, VString const &, VString const &, bool, IEvaluatorClient *);
        VINTERFACE_ROLE_3 (RetireOfflineWorker, VString const &, bool, IVReceiver<VString const &>*);

        VINTERFACE_ROLE_3 (DumpPoolQueue, VString const &, Vca::U32, IVReceiver<VString const &>*);
        VINTERFACE_ROLE_2 (DumpWorkingWorkers, VString const &, IVReceiver<VString const &>*);
        VINTERFACE_ROLE_4 (DumpWorker, VString const &, bool, VString const &, IVReceiver<VString const &>*);

        VINTERFACE_ROLE_3 (GetWorker_Ex, VString const &, bool, IVReceiver<IVUnknown*>*);
        VINTERFACE_ROLE_1 (GetWorkersStatistics, IVReceiver<VString const&>*);
        VINTERFACE_ROLE_2 (GetGenerationStatistics, Vca::U32, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool_Ex3
     *
     * Pool interface extension 3; adds several worker and stats operations.
     */
    VsaINTERFACE (IEvaluatorPool_Ex3, IEvaluatorPool_Ex2);
        /**
         * Sends a directive to take a worker offline to the pool.
         *
         * @param p1 the worker ID.
         * @param p2 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p3 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_3 (TakeWorkerOffline, VString const &, bool, IVReceiver<VString const &>*);
        /**
         * Sends a directive to make an offline worker online to a pool.
         *
         * @param p1 the worker ID.
         * @param p2 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p3 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_3 (MakeWorkerOnline, VString const &, bool, IVReceiver<VString const &>*);
        /**
         * Requests that the pool use a particular offline worker to evaluate a given query.
         *
         * @param p1 the query string.
         * @param p2 the worker ID.
         * @param p3 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p4 the IEvaluatorClient that the pool should send query results to.
         */
        VINTERFACE_METHOD_4 (EvaluateUsingOfflineWorker, VString const &, VString const &, bool, IEvaluatorClient *);
        /**
         * Sends a directive to retire an offline worker to the pool.
         *
         * @param p1 the worker ID.
         * @param p2 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p3 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_3 (RetireOfflineWorker, VString const &, bool, IVReceiver<VString const &>*);

        /**
         * Dumps this pool's queue information to a file.
         *
         * @param p1 the filename to dump to.
         * @param p2 currently unused.
         * @param p3 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_3 (DumpPoolQueue, VString const &, Vca::U32, IVReceiver<VString const &>*);
        /**
         * Dumps this pool's working worker statistics to a file.
         *
         * @param p1 the filename to dump to.
         * @param p2 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_2 (DumpWorkingWorkers, VString const &, IVReceiver<VString const &>*);
        /**
         * Dumps a worker's statistics to a file.
         *
         * @param p1 the worker ID.
         * @param p2 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p3 the filename to dump to.
         * @param p4 receiver for the return message from the pool.
         */
        VINTERFACE_METHOD_4 (DumpWorker, VString const &, bool, VString const &, IVReceiver<VString const &>*);

        /**
         * Retrieves a worker interface given a worker ID.
         *
         * @param p1 the worker ID.
         * @param p2 true if P2 is specified using a PID, false if pool-internal worker ID format is used.
         * @param p3 receiver for the interface to be returned by the pool.
         */
        VINTERFACE_METHOD_3 (GetWorker_Ex, VString const &, bool, IVReceiver<IVUnknown*>*);
        /**
         * Requests worker statistics from the pool.
         *
         * @param p1 receiver for the worker statistics to be returned by the pool.
         */
        VINTERFACE_METHOD_1 (GetWorkersStatistics, IVReceiver<VString const&>*);
        /**
         * Requests generation statistics from the pool.
         *
         * @param p1 receiver for the generation statistics to be returned by the pool.
         */
        VINTERFACE_METHOD_2 (GetGenerationStatistics, Vca::U32, IVReceiver<VString const&>*);
    VINTERFACE_END
}


#endif

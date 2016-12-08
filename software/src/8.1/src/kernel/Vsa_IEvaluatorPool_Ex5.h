#ifndef Vsa_IEvaluatorPool_Ex5_Interface
#define Vsa_IEvaluatorPool_Ex5_Interface

#ifndef Vsa_IEvaluatorPool_Ex5
#define Vsa_IEvaluatorPool_Ex5 extern
#endif 

/*********************
 *****  Library  *****
 *********************/ 

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_IEvaluatorPool_Ex4.h"

/**************************
 *****  Declarations  *****
 **************************/
class VString;
VINTERFACE_TEMPLATE_INSTANTIATIONS (Vsa,IEvaluatorPool_Ex5)

/*************************
 *****  Definitions  *****
 *************************/ 

 /*==========================================================================*
 *==========================================================================*/

namespace Vsa {
    VINTERFACE_ROLE (IEvaluatorPool_Ex5, IEvaluatorPool_Ex4)
        VINTERFACE_ROLE_4 (StatSum, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*); 
        VINTERFACE_ROLE_5 (StatDivide, VString, VString, Vca::U64, Vca::U64, IVReceiver<Vca::F64>*);
	VINTERFACE_ROLE_5 (StatMax, VString, Vca::U64, Vca::U64, IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*);
	VINTERFACE_ROLE_5 (StatMin, VString, Vca::U64, Vca::U64, IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*);
	VINTERFACE_ROLE_4 (StatMaxString, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_4 (StatMinString, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
	VINTERFACE_ROLE_6 (AllStatValues, VString, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, IVReceiver<VkDynamicArrayOf<V::VTime> const &>*, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, IVReceiver<VkDynamicArrayOf<Vca::U64>  const &>*, IVReceiver<VkDynamicArrayOf<Vca::F64> const &>*);
    VINTERFACE_ROLE_END

    /**
     * @class IEvaluatorPool_Ex5
     *
     * Pool interface extension 5; adds several worker and stats operations.
     */
    VsaINTERFACE (IEvaluatorPool_Ex5, IEvaluatorPool_Ex4);

    /** 
     * Takes the history of the given stat and returns the summation of its values in the given time range
     * Use this to get the total change in something in a time period, such as 'total number of queries in the last two hours'
     * @param p1 the string name of the stat
     * @param p2 the amount of time into the past we care about (a value of 0 is the present, higher values represent points in time further into the past)
     * @param p3 the size of the time-range of interest 
     */
    VINTERFACE_METHOD_4(StatSum, VString,  Vca::U64, Vca::U64,  IVReceiver<VString const &>*);

    /**
     * Divides the sum of a stat by the sum of another stat in a given time range.
     * Use this to get a value rate, such as 'time spent in queue per query in the last two hours'
     * @param p1 the string name of the first stat
     * @param p2 the string name of the second stat
     * @param p3 the amount of time into the past we care about (a value of 0 is the present, higher values represent points in time further into the past)
     * @param p4 the size of the time-range of interest
     */
    VINTERFACE_METHOD_5(StatDivide, VString, VString, Vca::U64, Vca::U64,  IVReceiver<Vca::F64>*);

    /**
     * Returns the maximum value of the given stat in the given time range
     * @param p1 the string name of the stat
     * @param p2 the amount of time into the past we care about (a value of 0 is the present, higher values represent points in time further into the past)
     * @param p3 the size of the time-range of interest
     */
    VINTERFACE_METHOD_5(StatMax, VString, Vca::U64, Vca::U64,  IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*); 

    /**
     * Returns the minimum value of the given stat in the given time range
     * @param p1 the string name of the stat
     * @param p2 the amount of time into the past we care about (a value of 0 is the present, higher values represent points in time further into the past)
     * @param p3 the size of the time-range of interest
     */
    VINTERFACE_METHOD_5(StatMin, VString, Vca::U64, Vca::U64,  IVReceiver<Vca::U32>*, IVReceiver<V::VTime const &>*);

    VINTERFACE_METHOD_4 (StatMaxString, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);
    VINTERFACE_METHOD_4 (StatMinString, VString, Vca::U64, Vca::U64, IVReceiver<VString const &>*);

    /**
     * Returns the whole history of all values the pool has
     */
    VINTERFACE_METHOD_6 (AllStatValues, VString, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, IVReceiver<VkDynamicArrayOf<V::VTime> const &>*, IVReceiver<VkDynamicArrayOf<Vca::U64> const &>*, IVReceiver<VkDynamicArrayOf<Vca::U64>  const &>*, IVReceiver<VkDynamicArrayOf<Vca::F64> const &>*);

    // TODO: IMPLEMENT THE FOLLOWING

    // A couple of emergency methods

    /**
     * Erase history on all stats
     */
    // VINTERFACE_METHOD_0(EraseStats);

    /**
     * Stop pushing stats to history
     */
    //VINTERFACE_METHOD_0(StatTrackingOff);

    /**
     * Resume pushing stats to history
     */
    //VINTERFACE_METHOD_0(StatTrackingOn);

    VINTERFACE_END
}


#endif

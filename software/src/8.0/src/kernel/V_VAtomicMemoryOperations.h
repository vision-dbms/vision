#ifndef V_VAtomicMemoryOperations_Interface
#define V_VAtomicMemoryOperations_Interface

/**
 * @file
 * @brief Defines a number of atomic and memory barrier operations.
 * For details, see @ref AtomicMemoryOperationsPage.
 */

/**
 * @page AtomicMemoryOperationsPage Atomic Memory Operations
 *
 * <h2>Memory Barriers</h2>
 * The memory barrier operations defined here implement a standard set of barrier uses.
 * For the most part, they mirror the fine grained barrier logic of Sparc processors and
 * the acquire/release barrier semantics required by many lock operations and provided
 * in the semantics of IA64(Itanium) and Opteron load and store instructions.
 *
 * <h2>Notes</h2>
 *   @anchor AtomicMemoryOperationsPage_Note1
 *   -# Unnecessary on this processor because of its architecture.
 *   @anchor AtomicMemoryOperationsPage_Note2
 *   -# The 'ld.acq' instruction performs the equivalent of a 'load' followed by a
 *      load:load+load:store barrier to ensure that no loads or stores will be
 *      performed until after the load instructions preceeding the barrier
 *      (presumably the loads used to test the lock) have completed.
 *   @anchor AtomicMemoryOperationsPage_Note3
 *   -# The 'st.rel' instruction performs a load:store+store:store barrier followed
 *      by a store.  This semantics insures that all loads and stores preceeding
 *      the barrier complete before the store following the barrier (presumably the
 *      store used to release the lock) is attempted.
 *
 * @see V::VAtomicMemoryOperations
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierAcquire()
 *
 * Designed to FOLLOW the successful acquisition of a spin lock. This barrier blocks
 * ALL loads and stores following the barrier until ALL loads preceding the barrier
 * (i.e., the loads used to test the lock) have completed.  The following table shows
 * the implementation for the processors currently supported by this implementation:
 *
 * <table>
 *   <tr>
 *     <th>Processor</th>
 *     <th>Implementation</th>
 *   </tr>
 *   <tr>
 *     <td>Alpha</td>
 *     <td><code>mb</code></td>
 *   </tr>
 *   <tr>
 *     <td>x86</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>x86-64</td>
 *     <td><code>lfence</code></td>
 *   </tr>
 *   <tr>
 *     <td>Itanium</td>
 *     <td><code><code>ld.acq</code> @ref AtomicMemoryOperationsPage_Note2 "(see note 2)"</td>
 *   </tr>
 *   <tr>
 *     <td>Sparc</td>
 *     <td><code>membar #loadload | #loadstore</code></td>
 *   </tr>
 * </table>
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierRelease()
 *
 * Designed to PRECEED the store instruction that releases a lock.  Ensures that all
 * access and update performed prior to the lock's release has completed before any
 * other processor is allowed to modify the protected resource.  By processor:
 *
 * <table>
 *   <tr>
 *     <th>Processor</th>
 *     <th>Implementation</th>
 *   </tr>
 *   <tr>
 *     <td>Alpha</td>
 *     <td><code>mb</code></td>
 *   </tr>
 *   <tr>
 *     <td>x86</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>x86-64</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>Itanium</td>
 *     <td><code>st.rel</code> @ref AtomicMemoryOperationsPage_Note3 "(see note 3)"</td>
 *   </tr>
 *   <tr>
 *     <td>Sparc</td>
 *     <td><code>membar #loadstore | #storestore</code></td>
 *   </tr>
 * </table>
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierProduce()
 *
 * Designed to PRECEED the store instruction that makes newly produced data visible
 * to other threads in producer/consumer sharing model.  This is a classic write
 * memory barrier scenario requiring that the data just produced be visible to
 * other processors before the data is made visible.  By processor:
 *
 * <table>
 *   <tr>
 *     <th>Processor</th>
 *     <th>Implementation</th>
 *   </tr>
 *   <tr>
 *     <td>Alpha</td>
 *     <td><code>wmb</code></td>
 *   </tr>
 *   <tr>
 *     <td>x86</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>x86-64</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>Itanium</td>
 *     <td><code>st.rel</code> @ref AtomicMemoryOperationsPage_Note3 "(see note 3)"</td>
 *   </tr>
 *   <tr>
 *     <td>Sparc</td>
 *     <td><code>membar #storestore</code></td>
 *   </tr>
 * </table>
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierConsume()
 *
 * Designed to PRECEED the store instruction that makes resources (e.g., buffers)
 * available for re-use in a producer/consumer sharing model.  This barrier assures
 * that the current thread has successful fetched the data contained in those
 * resources before allowing a producer to overwrite them.  By processor:
 *
 * <table>
 *   <tr>
 *     <th>Processor</th>
 *     <th>Implementation</th>
 *   </tr>
 *   <tr>
 *     <td>Alpha</td>
 *     <td><code>mb</code></td>
 *   </tr>
 *   <tr>
 *     <td>x86</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>x86-64</td>
 *     <td>N/A @ref AtomicMemoryOperationsPage_Note1 "(see note 1)"</td>
 *   </tr>
 *   <tr>
 *     <td>Itanium</td>
 *     <td><code>st.rel</code> @ref AtomicMemoryOperationsPage_Note3 "(see note 3)"</td>
 *   </tr>
 *   <tr>
 *     <td>Sparc</td>
 *     <td><code>membar #loadstore</code></td>
 *   </tr>
 * </table>
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierLoadLoad()
 *
 * Free floating memory barrier instruction that mirrors the Sparc memory barriers and is appropriately emulated on other architectures.
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierLoadStore()
 *
 * @copybrief V::VAtomicMemoryOperations::MemoryBarrierLoadLoad()
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierStoreStore()
 *
 * @copybrief V::VAtomicMemoryOperations::MemoryBarrierLoadLoad()
 */

/**
 * @fn static void V::VAtomicMemoryOperations::MemoryBarrierStoreLoad()
 *
 * @copybrief V::VAtomicMemoryOperations::MemoryBarrierLoadLoad()
 */


// Doxygen tags.

/**
 * @class V::VAtomicMemoryOperations
 *
 * Provides memory barrier definitions for subclasses.
 *
 * @warning NEVER use this class directly.
 *
 * @see V::VAtomicMemoryOperations_
 * @see AtomicMemoryOperationsPage
 */

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/**************************
 *****  Declarations  *****
 **************************/

#if defined(__arm__)
#define V_VAtomicMemoryOperations_Disabled

#elif defined(__VMS)
#include <builtins.h>

#ifdef __ia64
#   define __WMB() __MB ()
#else
#   include <c_asm.h>
#   define __WMB() asm ("wmb")
#endif

#elif defined(_WIN32)
#pragma comment (lib, "kernel32.lib")

#if defined(USING_MSVC7)
extern "C" {
   LONG  __cdecl _InterlockedIncrement(LONG volatile *Addend);
   LONG  __cdecl _InterlockedDecrement(LONG volatile *Addend);
   LONG  __cdecl _InterlockedCompareExchange(LONG volatile *Dest, LONG Exchange, LONG Comp);
   LONG  __cdecl _InterlockedExchange(LONG volatile *Target, LONG Value);
   LONG  __cdecl _InterlockedExchangeAdd(LONG volatile *Addend, LONG Value);
}

#pragma intrinsic (_InterlockedCompareExchange)
#define InterlockedCompareExchange _InterlockedCompareExchange

#pragma intrinsic (_InterlockedExchange)
#define InterlockedExchange _InterlockedExchange 

#pragma intrinsic (_InterlockedExchangeAdd)
#define InterlockedExchangeAdd _InterlockedExchangeAdd

#pragma intrinsic (_InterlockedIncrement)
#define InterlockedIncrement _InterlockedIncrement

#pragma intrinsic (_InterlockedDecrement)
#define InterlockedDecrement _InterlockedDecrement

#endif

#elif defined(sun)

////////////////////////////////////////////////////////
//  The assembly language implementations of these routines can be found in the 'sun.*.il' file
//  appropriate for the architecture targeted by the compilation (v8plus = 32bit, v9 = 64 bit).
//
//  These declarations are NOT declarations for external symbols - they are prototypes for
//  routines whose implementation is taken from the '.il' file mentioned above and directly
//  inlined by the Sun compiler's code generator (see 'man inline' for more information).
//
////////////////////////////////////////////////////////

extern "C" {
    unsigned __int32 compareAndSwap32 (
        unsigned __int32 volatile *pValue, unsigned __int32 iExpected, unsigned __int32 iReplacement
    );
    unsigned __int64 compareAndSwap64 (
        unsigned __int64 volatile *pValue, unsigned __int64 iExpected, unsigned __int64 iReplacement
    );

    unsigned __int32 fetchAndAdd32 (
        unsigned __int32 volatile &rValue, unsigned __int32 iIncrement
    );
    unsigned __int64 fetchAndAdd64 (
        unsigned __int64 volatile &rValue, unsigned __int64 iIncrement
    );

    void membar_acquire    ();  // #loadload  | #loadstore
    void membar_release    ();  // #loadstore | #storestore
    void membar_produce    ();  // #storestore
    void membar_consume    ();  // #loadload

    void membar_loadload   ();
    void membar_loadstore  ();
    void membar_storeload  ();
    void membar_storestore ();
}

#endif


/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /*---------------------------*
     *----  No Interlocking  ----*
     *---------------------------*/

    class VAtomicMemoryNilOperations {
    //  Aliases
    public:
        typedef VAtomicMemoryNilOperations ThisClass;

    //  Operations
    public:
        static void MemoryBarrierAcquire () {   //  lock acquire, no read ahead.
        }
        static void MemoryBarrierRelease () {   //  lock release, no write behind.
        }
        static void MemoryBarrierProduce () {   //  data produce, no write behind.
        }
        static void MemoryBarrierConsume () {   //  data consume, no read ahead.
        }

        static void MemoryBarrierLoadLoad () {
        }
        static void MemoryBarrierLoadStore () {
        }
        static void MemoryBarrierStoreStore () {
        }
        static void MemoryBarrierStoreLoad () {
        }
    };


    /*---------------------------*
     *----  No Interlocking  ----*
     *---------------------------*/

#if defined(V_VAtomicMemoryOperations_Disabled)
    typedef VAtomicMemoryNilOperations VAtomicMemoryOperations;


#elif defined(__VMS)
    /*-------------------------*
     *----  OpenVMS/Alpha  ----*
     *-------------------------*/

    class VAtomicMemoryOperations {
    //  Aliases
    public:
        typedef VAtomicMemoryOperations ThisClass;

    //  Operations
    public:
        static void MemoryBarrierAcquire () {   //  lock acquire, no read ahead.
            __MB ();
        }
        static void MemoryBarrierRelease () {   //  lock release, no write behind.
            __MB ();
        }
        static void MemoryBarrierProduce () {   //  data produce, no write behind.
            __WMB ();
        }
        static void MemoryBarrierConsume () {   //  data consume, no read ahead.
            __MB ();
        }

        static void MemoryBarrierLoadLoad () {
            __MB ();
        }
        static void MemoryBarrierLoadStore () {
            __MB ();
        }
        static void MemoryBarrierStoreStore () {
            __WMB ();
        }
        static void MemoryBarrierStoreLoad () {
            __MB ();
        }
    };


#elif defined(_WIN32)
    /*-----------------------*
     *----  Windows/x86  ----*
     *-----------------------*/

    class VAtomicMemoryOperations {
    //  Aliases
    public:
        typedef VAtomicMemoryOperations ThisClass;

    //  Operations
    public:
        static void MemoryBarrierAcquire () {   //  lock acquire, no read ahead.
        }
        static void MemoryBarrierRelease () {   //  lock release, no write behind.
        }
        static void MemoryBarrierProduce () {   //  data produce, no write behind.
        }
        static void MemoryBarrierConsume () {   //  data consume, no read ahead.
        }

        static void MemoryBarrierLoadLoad () {
        }
        static void MemoryBarrierLoadStore () {
        }
        static void MemoryBarrierStoreStore () {
        }
        static void MemoryBarrierStoreLoad () {
        }
    };


#elif defined(__linux__)
    /*---------------------*
     *----  Linux/x86  ----*
     *---------------------*/

    class VAtomicMemoryOperations {
    //  Aliases
    public:
        typedef VAtomicMemoryOperations ThisClass;

    //  Operations
    public:
        static void MemoryBarrierAcquire () {   //  lock acquire, no read ahead.
        }
        static void MemoryBarrierRelease () {   //  lock release, no write behind.
        }
        static void MemoryBarrierProduce () {   //  data produce, no write behind.
        }
        static void MemoryBarrierConsume () {   //  data consume, no read ahead.
        }

        static void MemoryBarrierLoadLoad () {
        }
        static void MemoryBarrierLoadStore () {
        }
        static void MemoryBarrierStoreStore () {
        }
        static void MemoryBarrierStoreLoad () {
        }
    };


#elif defined(sun)
    /*-------------------------*
     *----  Solaris/Sparc  ----*
     *-------------------------*/

    class VAtomicMemoryOperations {
    //  Aliases
    public:
        typedef VAtomicMemoryOperations ThisClass;

    //  Operations
    public:
        static void MemoryBarrierAcquire () {   //  lock acquire, no read ahead.
            membar_acquire ();  // #loadload | #loadstore
        }
        static void MemoryBarrierRelease () {   //  lock release, no write behind.
            membar_release ();
        }
        static void MemoryBarrierProduce () {   //  data produce, no write behind.
            membar_produce ();
        }
        static void MemoryBarrierConsume () {   //  data consume, no read ahead.
            membar_consume ();
        }

        static void MemoryBarrierLoadLoad () {
            membar_loadload ();
        }
        static void MemoryBarrierLoadStore () {
            membar_loadstore ();
        }
        static void MemoryBarrierStoreStore () {
            membar_storestore ();
        }
        static void MemoryBarrierStoreLoad () {
            membar_storeload ();
        }
    };

#endif
}


#endif

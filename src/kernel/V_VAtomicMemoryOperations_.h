#ifndef V_VAtomicMemoryOperations__Interface
#define V_VAtomicMemoryOperations__Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAtomicMemoryOperations.h"

/**************************
 *****  Declarations  *****
 **************************/

/***********************
 *****  Templates  *****
 ***********************/

namespace V {
    /*---------------------------*
     *----  No Interlocking  ----*
     *---------------------------*/

    /**
     * Class serving as home for static methods used for atomic memory operations. Used as a cover for processor/platform-specific atomic memory instructions.
     */
    template<unsigned int Size> class VAtomicMemoryNilOperations_ : public VAtomicMemoryNilOperations {
    };

    /**
     * NEVER use this class directly.
     * @private
     */
    template<> class VAtomicMemoryNilOperations_<(size_t)4> : public VAtomicMemoryNilOperations {
    //  Family Values
    public:
	typedef __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    *pMemory = iNew;
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    if (*pMemory != iExpected)
		return false;
	    *pMemory = iNew;
	    return true;
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if !defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif
    };

    template<> class VAtomicMemoryNilOperations_<(size_t)8> : public VAtomicMemoryNilOperations {
    //  Family Values
    public:
	typedef __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    *pMemory = iNew;
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    if (*pMemory != iExpected)
		return false;
	    *pMemory = iNew;
	    return true;
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif
    };


    /*---------------------------*
     *----  No Interlocking  ----*
     *---------------------------*/

#if defined(V_VAtomicMemoryOperations_Disabled)
    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryNilOperations_<Size> {
    };


#elif defined(__APPLE__)
    /*-----------------*
     *----  Apple  ----*
     *-----------------*/

    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryOperations {
    };

    template<> class VAtomicMemoryOperations_<(size_t)4> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return OSAtomicCompareAndSwap32 (
		static_cast<int32_t>(iExpected),
		static_cast<int32_t>(iNew),
		reinterpret_cast<int32_t volatile*>(pMemory)
	    );
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if !defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif // !defined(_LP64)
    };

    template<> class VAtomicMemoryOperations_<(size_t)8> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return OSAtomicCompareAndSwap64 (
		static_cast<int64_t>(iExpected),
		static_cast<int64_t>(iNew),
		reinterpret_cast<OSAtomic_int64_aligned64_t volatile*>(pMemory)
	    );
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif // defined(_LP64)
    };


#elif defined(__VMS)
    /*-------------------------*
     *----  OpenVMS/Alpha  ----*
     *-------------------------*/

    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryOperations {
    };

    template<> class VAtomicMemoryOperations_<(size_t)4> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return __ATOMIC_EXCH_LONG (pMemory, iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    do {
		if (__CMP_SWAP_LONG (pMemory, iExpected, iNew))
		    return true;
	    } while (iExpected == *reinterpret_cast<value_t volatile*>(pMemory));
	    return false;
	}

#if !defined(_LP64)
	static void *interlockedExchange (void volatile *pMemory, void *iNew) {
	    return (void*)interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    do {
		if (__CMP_SWAP_LONG (pMemory, (value_t)iExpected, (value_t)iNew))
		    return true;
	    } while ((value_t)iExpected == *reinterpret_cast<value_t volatile*>(pMemory));
	    return false;
	}
#endif
    };

    template<> class VAtomicMemoryOperations_<(size_t)8> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return __ATOMIC_EXCH_QUAD (pMemory, iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    do {
		if (__CMP_SWAP_QUAD (pMemory, iExpected, iNew))
		    return true;
	    } while (iExpected == *reinterpret_cast<value_t volatile*>(pMemory));
	    return false;
	}
#if defined(_LP64)
	static void *interlockedExchange (void volatile *pMemory, void *iNew) {
	    return (void*)interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    do {
		if (__CMP_SWAP_QUAD (pMemory, (value_t)iExpected, (value_t)iNew))
		    return true;
	    } while ((value_t)iExpected == *reinterpret_cast<value_t volatile*>(pMemory));
	    return false;
	}
#endif
    };


#elif defined(_WIN32)
    /*-------------------------*
     *----  Microsoft/x86  ----*
     *-------------------------*/

    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryOperations {
    };

    template<> class VAtomicMemoryOperations_<(size_t)4> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return InterlockedExchange ((LONG volatile*)pMemory, iNew);
	}
	static void *interlockedExchange (void *volatile *pMemory, void *iNew) { // TODO: should iNew be pNew?
#ifdef __ATLCONV_H__
		// Direct from atlconv.h -- we can't call InterlockedExchangePointer() from there because of volatile->non-volatile casts.
		return( reinterpret_cast<void*>(static_cast<LONG_PTR>(::InterlockedExchange(reinterpret_cast<LONG volatile *>(pMemory), static_cast<LONG>(reinterpret_cast<LONG_PTR>(iNew))))) );
#else
		return InterlockedExchangePointer (pMemory, iNew);
#endif
	}

	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return InterlockedCompareExchange ((LONG volatile*)pMemory, iNew, iExpected) == iExpected;
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return InterlockedCompareExchangePointer (
		(PVOID volatile*)pMemory, iNew, (PVOID)iExpected
	    ) == iExpected;
	}
    };

    template<> class VAtomicMemoryOperations_<(size_t)8> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return InterlockedExchange64 ((LONGLONG volatile*)pMemory, iNew);
	}
	static void *interlockedExchange (void *volatile *pMemory, void *iNew) {
#ifdef __ATLCONV_H__
		// Direct from atlconv.h -- we can't call InterlockedExchangePointer() from there because of volatile->non-volatile casts.
		return( reinterpret_cast<void*>(static_cast<LONG_PTR>(::InterlockedExchange(reinterpret_cast<LONG volatile *>(pMemory), static_cast<LONG>(reinterpret_cast<LONG_PTR>(iNew))))) );
#else
		return InterlockedExchangePointer (pMemory, iNew);
#endif
	}

	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return InterlockedCompareExchange64 (
		(LONGLONG volatile*)pMemory, iNew, iExpected
	    ) == iExpected;
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return InterlockedCompareExchangePointer (
		(PVOID volatile*)pMemory, iNew, (PVOID)iExpected
	    ) == iExpected;
	}
    };


#elif defined(__linux__)
    /*---------------------*
     *----  Linux/x86  ----*
     *---------------------*/

    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryOperations {
    };

    template<> class VAtomicMemoryOperations_<(size_t)4> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (
	    value_t volatile *pMemory, value_t iExpected, value_t iNew
	) {
	    value_t iOld;
	    __asm__ __volatile__(
		"lock; cmpxchgl %1,(%2)"
		: "=a"(iOld)
		: "r"(iNew), "r"(pMemory), "0"(iExpected)
		: "cc", "memory"
	    );
	    return iOld == iExpected;
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if !defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif
    };

    template<> class VAtomicMemoryOperations_<(size_t)8> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (
	    value_t volatile *pMemory, value_t iExpected, value_t iNew
	) {
	/*-------------------------------------------------------------------------------*
	 *>>>>>>>>>>>>>>>>>>>>  'cmpxchgq' is an AMD64 INSTRUCTION.  <<<<<<<<<<<<<<<<<<<<*
	 *>>>>>>>>>>>>>>>>>>>>   CHECK FOR __x86_64__ CORRECTNESS.   <<<<<<<<<<<<<<<<<<<<*
	 *-------------------------------------------------------------------------------*/
	    value_t iOld;
	    __asm__ __volatile__(
		"lock; cmpxchgq %1,(%2)"
		: "=a"(iOld)
		: "r"(iNew), "r"(pMemory), "0"(iExpected)
		: "cc", "memory"
	    );
	    return iOld == iExpected;
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif
    };


#elif defined(sun)
    /*-----------------------*
     *----  Sun/Solaris  ----*
     *-----------------------*/

    template<unsigned int Size> class VAtomicMemoryOperations_ : public VAtomicMemoryOperations {
    };

    template<> class VAtomicMemoryOperations_<(size_t)4> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int32 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
#ifdef USING_LEGACY_SPARC_ATOMICS
	    return compareAndSwap32 (pMemory, iExpected, iNew) == iExpected;
#else
	    return atomic_cas_32 (pMemory, iExpected, iNew) == iExpected;
#endif
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if !defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif // !defined(_LP64)
    };

    template<> class VAtomicMemoryOperations_<(size_t)8> : public VAtomicMemoryOperations {
    //  Family Values
    public:
	typedef unsigned __int64 value_t;
	enum {
	    Size = sizeof (value_t)
	};

    //  Operations
    public:
	static value_t interlockedExchange (value_t volatile *pMemory, value_t iNew) {
	    value_t iOld = *pMemory;
	    while (!interlockedSetIf (pMemory, iOld, iNew)) {
		iOld = *pMemory;
	    }
	    return iOld;
	}
	static value_t interlockedExchange (void volatile *pMemory, value_t iNew) {
	    return interlockedExchange ((value_t volatile*)pMemory, iNew);
	}

	static bool interlockedSetIf (value_t volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
#ifdef USING_LEGACY_SPARC_ATOMICS
	    return compareAndSwap64 (pMemory, iExpected, iNew) == iExpected;
#else
	    return atomic_cas_64 (reinterpret_cast<uint64_t volatile*>(pMemory), iExpected, iNew) == iExpected;
#endif
	}
	static bool interlockedSetIf (void volatile *pMemory, value_t iExpected, value_t iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf ((value_t volatile*)pMemory, iExpected, iNew);
	}
#if defined(_LP64)
	static value_t interlockedExchange (void volatile *pMemory, void *iNew) {
	    return interlockedExchange (pMemory, (value_t)iNew);
	}
	static bool interlockedSetIf (void volatile *pMemory, void const *iExpected, void *iNew) {
	    //  returns true if memory set, false otherwise
	    return interlockedSetIf (pMemory, (value_t)iExpected, (value_t)iNew);
	}
#endif // defined(_LP64)
    };

#endif
}


#endif

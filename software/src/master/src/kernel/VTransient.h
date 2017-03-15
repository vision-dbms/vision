#ifndef VTransient_Interface
#define VTransient_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

class VTransientServices;

#define ENABLE_MEMORY_MANAGER_EXCEPTIONS
#include "VConfig.h"

#if defined(MEMORY_MANAGER_CONTROL_OVERRIDES)
DECLARE_MEMORY_MANAGER_CONTROL_CLASS;
#endif

#include "verrdef.d"	    //  ... a concession to batchvision


/*************************
 *****  Definitions  *****
 *************************/

/************************
 *----  VTransient  ----*
 ************************/

///////////////////////////////////////////////////////////////////////
//  Do NOT define any 'virtual' functions at this class.  They will
//  double the size of all transient data structures.  Especially do
//  NOT define a virtual destructor.  It will impose a significant
//  performance penalty on all destructors.
///////////////////////////////////////////////////////////////////////

class V_API VTransient {
//  Construction
protected:
//    VTransient () {
//    }

private: //  Disable default copy constructors...
//    VTransient (VTransient const &rOther) {
//    }

//  Destruction
protected:
//    ~VTransient () {
//    }

private: //  Disable default assignment operators...
    VTransient& operator= (VTransient const &rOther) {
	return *this;
    }

//  Memory Allocation
private:
    static void *malloc (size_t cBytes) {
	return 0;
    }
    static void *realloc (void *pMemory, size_t cBytes) {
	return 0;
    }
    static void free (void *pMemory) {
    }

public:
    static void *allocate	(size_t cBytes);
    static void *reallocate	(void *pMemory, size_t cBytes);
    static void  deallocate	(void *pMemory);

    static void *copy (void const *pData, size_t sData) {
	void *pCopy = allocate (sData);
	memcpy (pCopy, pData, sData);
	return pCopy;
    }

    void *operator new (size_t sObject);
    void operator delete (void *pObject, size_t sObject);

    void *operator new (size_t sObject, void *pRegion, size_t sRegion) {
	return pRegion;
    }
    void operator delete (void *pObject, void *pRegion, size_t sRegion) {
    }

    void *operator new (size_t sObject, void *pRegion) {
	return pRegion;
    }
    void operator delete (void *pObject, void *pRegion) {
    }

    #if defined(MEMORY_MANAGER_CONTROL_OVERRIDES)
    DEFINE_MEMORY_MANAGER_NEW_OVERRIDES
    #endif

    static void DisplayAllocationStatistics ();

//  Message Display and Formatting
public:
    static int __cdecl display (char const *pMessage, ...);
    static int vdisplay (char const *pMessage, va_list pArguments);

    static char const *__cdecl string (char const *pMessage, ...);
    static char const *vstring (char const *pMessage, va_list pArguments);

//  Resource Reclamation
public:
    //  Routines return true when resources are reclaimed, false otherwise.
    bool garbageCollected ();
    bool mappingsReclaimed ();

//  Exception Generation
public:
    void __cdecl raiseApplicationException (char const *pMessage, ...) const;
    void vraiseApplicationException (char const *pMessage, va_list pArguments) const;

    void __cdecl raiseSystemException (char const *pMessage, ...) const;
    void vraiseSystemException (char const *pMessage, va_list pArguments) const;

    void __cdecl raiseUnimplementedOperationException (char const *pMessage, ...) const;
    void vraiseUnimplementedOperationException (char const *pMessage, va_list pArguments) const;

    void __cdecl raiseException (ERRDEF_ErrorCode xErrorCode, char const *pMessage, ...) const;
    void vraiseException (ERRDEF_ErrorCode xErrorCode, char const *pMessage, va_list pArguments) const;

//  Transient Services Provider
private:
    static VTransientServices *g_pTSP;

protected:
    static void attachTransientServicesProvider (VTransientServices *pTSP);
    static void detachTransientServicesProvider (VTransientServices *pTSP);

public:
    static VTransientServices *transientServicesProvider () {
	return g_pTSP;
    }

//  Notification Service
public:

    void notify (int xEvent, char const *pFormat, ...) const;

    void notify (bool bWaitingForResp, int xEvent, char const *pFormat, ...) const ;

    void notify (bool bWaitingForResp, int xEvent, char const *pFormat, va_list ap) const;


//  Logging 
public:
    void log (char const *pFormat, ...) const;
    void vlog (char const *pFormat, va_list ap) const;

  //  Trace Control
private:
  enum ObjectTracing {
    Tracing_NotInitialized,
    Tracing_On,
    Tracing_Off
  };
  static ObjectTracing g_bTracingObjects;

  static bool initializeTraceOption () {
    if (g_bTracingObjects == Tracing_NotInitialized) {
      char const *pEnvValue = getenv ("TraceObjects");
      g_bTracingObjects = pEnvValue ? (strcasecmp (pEnvValue, "TRUE")? Tracing_Off : Tracing_On) : Tracing_Off;
    }
    return true;
  }

  //  Tracing Objects
public:
    void traceInfo (char const *pWhat) const;

    static void setObjectsTrace (bool onOffSwitch) {
        g_bTracingObjects = onOffSwitch ? Tracing_On : Tracing_Off;
    }
    static bool objectsTrace () {
       return initializeTraceOption () && g_bTracingObjects == Tracing_On;
    }   

};


/***************************
 *****  Pointer Types  *****
 ***************************/

#include "V_VPointer_NRK.h"


#endif

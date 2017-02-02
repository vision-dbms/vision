#ifndef V_Semaphore_Interface
#define V_Semaphore_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VSynchronizationObject.h"

#if defined(_WIN32)
#define V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES

#elif defined(sun) || defined(__linux__) || defined(__APPLE__)
#include <semaphore.h>
#define V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES

#else
#include <pthread.h>
#define V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS

#endif

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class VTime;

    class V_API VSemaphore : public VSynchronizationObject {
	DECLARE_FAMILY_MEMBERS (VSemaphore, VSynchronizationObject);

    //  Aliases
    public:
	typedef unsigned int value_t;

    //  Construction
    public:
	VSemaphore (value_t iInitialValue = 0);

    //  Destruction
    public:
	~VSemaphore ();

    //  Operation
    public:
	void produce ();
	void consume ();
	bool consumeTry ();
	WaitStatus consumeTry (VTime const &rLimit);

    //  State
    private:
#   if defined(V_SEMAPHORE_IMPLEMENTED_USING_WIN32_SEMAPHORES)
	HANDLE m_hSemaphore;
#   elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_SEMAPHORES)
	sem_t m_iSemaphore;
#   elif defined(V_SEMAPHORE_IMPLEMENTED_USING_POSIX_PTHREADOBJECTS)
	value_t		m_iValue;
	pthread_mutex_t	m_iMutex;
	pthread_cond_t	m_iCondvar;
#   endif
    };
}


#endif

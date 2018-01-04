#ifndef VkRunTimes_Interface
#define VkRunTimes_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#if !defined(__VMS) && !defined(_WIN32)
#include <sys/resource.h>
#endif

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class V_API VkRunTimes {
//  Aliases and Helpers
public:
#if defined(__VMS)
    typedef unsigned __int64 wall_tick_t;
    typedef unsigned long proc_tick_t;
#elif defined(_WIN32)
    typedef __int64 tick_count_t;
    typedef tick_count_t wall_tick_t;
    typedef tick_count_t proc_tick_t;
#else
    typedef unsigned __int64 tick_count_t;
    typedef tick_count_t wall_tick_t;
    typedef tick_count_t proc_tick_t;

    /*=====================*
     *==== xtimeval_t  ====*
     *=====================*/
    class V_API xtimeval_t : public timeval {
    //  USECPERSEC
    private:
	enum {USECPERSEC = 1000000};

    // Construction
    public:
	xtimeval_t (time_t sec, unsigned int usec) {
	    tv_sec = sec;
	    tv_usec = usec;
	}
	xtimeval_t (wall_tick_t iTicks) {
	    setTo (iTicks);
	}
	xtimeval_t (timeval const &rOther) {
	    tv_sec  = rOther.tv_sec;
	    tv_usec = rOther.tv_usec;
	}
	xtimeval_t () {
	    refresh ();
	}

    // Destruction
    public:
	~xtimeval_t () {
	}

    //  Access
    public:
	static tick_count_t toTicks (timeval const &rTimeval) {
	    return static_cast<tick_count_t>(rTimeval.tv_sec) * USECPERSEC + rTimeval.tv_usec;
	}
	tick_count_t toTicks () const {
	    return toTicks (*this);
	}
	operator tick_count_t () const {
	    return toTicks ();
	}

	tick_count_t operator+ (tick_count_t iTicks) const {
	    return toTicks () + iTicks;
	}
	tick_count_t operator- (tick_count_t iTicks) const {
	    return toTicks () - iTicks;
	}

    //  Update
    public:
	void refresh ();

	void setTo (tick_count_t iTicks) {
	    tv_sec  = static_cast<time_t>(iTicks / USECPERSEC);
	    tv_usec = static_cast<unsigned int>(iTicks % USECPERSEC);
	}
	void setTo (timeval const &rOther) {
	    tv_sec = rOther.tv_sec;
	    tv_usec = rOther.tv_usec;
	}

	xtimeval_t &operator= (tick_count_t iTicks) {
	    setTo (iTicks);
	    return *this;
	}
	xtimeval_t &operator= (timeval const &rOther) {
	    setTo (rOther);
	    return *this;
	}

	xtimeval_t &operator+= (tick_count_t iTicks) {
	    setTo (toTicks () + iTicks);
	    return *this;
	}
	xtimeval_t &operator-= (tick_count_t iTicks) {
	    setTo (toTicks () - iTicks);
	    return *this;
	}
    };


    /*=====================*
     *====  xrusage_t  ====*
     *=====================*/

    class V_API xrusage_t : public rusage {
    //  USECPERSEC
    private:
	enum {USECPERSEC = 1000000};

    // Construction
    public:
	xrusage_t (rusage const *pOther) {
	    setTo (pOther);
	}
	xrusage_t (rusage const &rOther) {
	    setTo (rOther);
	}
	xrusage_t () {
	    refresh ();
	}

    // Destruction
    public:
	~xrusage_t () {
	}

    //  Update
    public:
	void refresh ();

	void setTo (rusage const *pOther);
	void setTo (rusage const &rOther);

	xrusage_t &operator= (rusage const *pOther) {
	    setTo (pOther);
	    return *this;
	}
	xrusage_t &operator= (rusage const &rOther) {
	    setTo (rOther);
	    return *this;
	}

	xrusage_t &operator+= (rusage const &rOther);
	xrusage_t &operator-= (rusage const &rOther);
    };
#endif


    /*=========================*
     *====  The Real Deal  ====*
     *=========================*/

//  Factors
public:
    static double const g_iElapsedTicksPerSecond;
    static double const g_iProcessorTicksPerSecond;

//  Construction
public:
#if defined(__VMS)
    VkRunTimes (wall_tick_t iETime,
		proc_tick_t iUTime = 0,
		unsigned long iBufIO = 0,
		unsigned long iDirIO = 0,
		unsigned long iFilCnt = 0,
		unsigned long iPageFlts = 0
    );
#elif defined(_WIN32)
    VkRunTimes (wall_tick_t iETime,
		proc_tick_t iUTime = 0,
		proc_tick_t iSTime = 0
    );
#else
    VkRunTimes (wall_tick_t iETime,
		rusage *pUsage = 0
    );
#endif

    VkRunTimes (VkRunTimes const &rOther);

    VkRunTimes () {
	refresh ();
    }

//  Destruction
public:
    ~VkRunTimes () {
    }

//  Access
public:
    /**********************************/
    wall_tick_t elapsedTicks () const {
	return m_iETime;
    }
    double elapsed () const {
	return elapsedTicks () / g_iElapsedTicksPerSecond;
    }

    /**********************************/
    proc_tick_t userTicks () const {
#if defined(__VMS) || defined(_WIN32)
	return m_iUTime;
#else
	return xtimeval_t::toTicks (m_iUsage.ru_utime);
#endif
    }
    double user () const {
	return userTicks () / g_iProcessorTicksPerSecond;
    }

    /**********************************/
#if defined(__VMS)
    static proc_tick_t systemTicks () {
	return 0;
    }
    static double system () {
	return 0.0;
    }
#else
    proc_tick_t systemTicks () const {
# if defined(_WIN32)
	return m_iSTime;
# else
	return xtimeval_t::toTicks (m_iUsage.ru_stime);
# endif
    }
    double system () const {
	return systemTicks () / g_iProcessorTicksPerSecond;
    }
#endif
	
#if defined(__VMS)
    unsigned int bufio () const {
	return m_iBufIO;
    }
    unsigned int dirio () const {
	return m_iDirIO;
    }
    unsigned int filcnt () const {
	return m_iFilCnt;
    }
    unsigned int pageflts () const {
	return m_iPageFlts;
    }
    static unsigned int minflts () {
	return 0;
    }
    static unsigned int majflts () {
	return 0;
    }
#else
    static unsigned int bufio () {
	return 0;
    }
    static unsigned int dirio () {
	return 0;
    }
    static unsigned int filcnt () {
	return 0;
    }
# if defined(_WIN32)
    static unsigned int pageflts () {
	return 0;
    }
    static unsigned int minflts () {
	return 0;
    }
    static unsigned int majflts () {
	return 0;
    }
# else
    unsigned int pageflts () const {
	return m_iUsage.ru_majflt + m_iUsage.ru_minflt;
    }
    unsigned int majflts () const {
	return m_iUsage.ru_majflt;
    }
    unsigned int minflts () const {
	return m_iUsage.ru_minflt;
    }
# endif	// ?defined(_WIN32)
#endif	// ?defined(__VMS)

//  Update
public:
    VkRunTimes &operator= (VkRunTimes const& rOther);
    VkRunTimes &operator+= (VkRunTimes const& rOther);
    VkRunTimes &operator-= (VkRunTimes const& rOther);

    void refresh ();

//  State
private:
#if defined(__VMS)
    wall_tick_t m_iETime;
    proc_tick_t m_iUTime;
    unsigned long m_iBufIO;   // JPI$_BUFIO
    unsigned long m_iDirIO;   // JPI$_DIRIO
    unsigned long m_iFilCnt;  // JPI$_FILCNT
    unsigned long m_iPageFlts;// JPI$_PAGEFLTS
#elif defined(_WIN32)
    wall_tick_t m_iETime;
    proc_tick_t m_iUTime;
    proc_tick_t m_iSTime;
#else
    xtimeval_t m_iETime;
    xrusage_t m_iUsage;
#endif
};


/***********************************
 ***********************************
 *****  Timing Display Macros  *****
 ***********************************
 ***********************************/

#define UTIL_TimeDelta(start, end, field) ((end).field () - (start).field ())

#define UTIL_PrintDelta(start, end, field, fmt) IO_printf (\
    #field ": " fmt "\n", UTIL_TimeDelta (start, end, field)\
)

#define UTIL_PrintTimeDelta(start, end, field) UTIL_PrintDelta (start, end, field, "%g")

#define UTIL_PrintStatDelta(start, end, field) UTIL_PrintDelta (start, end, field, "%u")


#endif

#ifndef V_VAdaptiveAllocator_Interface
#define V_VAdaptiveAllocator_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VAdaptiveAllocator : public VTransient {
	DECLARE_FAMILY_MEMBERS (VAdaptiveAllocator, VTransient);

    //  Globals
    public:
        static size_t g_sIncrementDelay;
        static size_t g_sIncrementMin;
        static size_t g_sIncrementMax;
        static size_t g_sIncrementMultiplier;

    //  Parameters
    public:
	class V_API Parameters : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Parameters, VTransient);

	//  Construction
	public:
	    Parameters () :
		m_sIncrementDelay	(g_sIncrementDelay),
		m_sIncrementMin		(g_sIncrementMin),
		m_sIncrementMax		(g_sIncrementMax),
		m_sIncrementMultiplier	(g_sIncrementMultiplier)
	    {
	    }

	//  Destruction
	public:
	    ~Parameters () {
	    }

	//  Access
	public:
	    size_t incrementDelay () const {
		return m_sIncrementDelay;
	    }
	    size_t incrementMin () const {
		return m_sIncrementMin;
	    }
	    size_t incrementMax () const {
		return m_sIncrementMax;
	    }
	    size_t incrementMultiplier () const {
		return m_sIncrementMultiplier;
	    }

	//  Update
	public:
	    void setIncrementDelay (size_t iValue) {
		m_sIncrementDelay = iValue;
	    }
	    void setIncrementMin (size_t iValue) {
		m_sIncrementMin = iValue;
	    }
	    void setIncrementMax (size_t iValue) {
		m_sIncrementMax = iValue;
	    }
	    void setIncrementMultiplier (size_t iValue) {
		m_sIncrementMultiplier = iValue;
	    }

	//  State
	private:
	    size_t m_sIncrementDelay;
	    size_t m_sIncrementMin;
	    size_t m_sIncrementMax;
	    size_t m_sIncrementMultiplier;
	};

    //  Construction
    public:
	VAdaptiveAllocator (Parameters *pParameters) {
	    reset (pParameters);
	}
	VAdaptiveAllocator () : m_pParameters (0) {
	    reset ();
	}

    //  Destruction
    public:
	~VAdaptiveAllocator () {
	}

    //  Parameter Access
    public:
        size_t incrementDelay () const {
            return m_pParameters ? m_pParameters->incrementDelay() : g_sIncrementDelay;
        }
        size_t incrementMin () const {
            return m_pParameters ? m_pParameters->incrementMin() : g_sIncrementMin;
        }
        size_t incrementMax () const {
            return m_pParameters ? m_pParameters->incrementMax() : g_sIncrementMax;
        }
        size_t incrementMultiplier () const {
            return m_pParameters ? m_pParameters->incrementMultiplier() : g_sIncrementMultiplier;
        }

    //  Use
    public:
	void reset (Parameters *pParameters) {
	    m_pParameters = pParameters;
	    reset ();
	}
	void reset ();

    //  Use
    public:
	size_t nextSize (size_t sRequired);

    //  State
    private:
	Parameters*	m_pParameters;
	size_t		m_sCurrentIncrement;
	unsigned int	m_sTimeToNextAdaptiveCall;
    };
}


#endif

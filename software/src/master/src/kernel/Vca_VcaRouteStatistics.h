#ifndef VcaRouteStatistics_Interface
#define VcaRouteStatistics_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VcaRouteStatistics {
    //  Construction
    public:
	VcaRouteStatistics ();
	VcaRouteStatistics (unsigned int cHops);
	VcaRouteStatistics (VcaRouteStatistics const &rOther);

    //  Destruction
    public:
	~VcaRouteStatistics () {
	}

    //  Assignment 
    public:
	VcaRouteStatistics &operator= (VcaRouteStatistics const &rOther) {
	    setTo (rOther);
	    return *this;
	}

    //  Access
    public:
	unsigned int hopCount () const {
	    return m_cHops;
	}

    //  Update
    public:
	void setHopCount (unsigned int cHops) {
	    m_cHops = cHops;
	}
	void setTo (VcaRouteStatistics const &rOther) {
	    m_cHops = rOther.m_cHops;
	}

    //  State
    protected:
	unsigned int m_cHops;
    };
}


#endif

#ifndef Vca_VPipeSource_Interface
#define Vca_VPipeSource_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VPipeSource : public VRolePlayer {
    //  Run Time Type
	DECLARE_ABSTRACT_RTTLITE (VPipeSource, VRolePlayer);

    //  Construction
    protected:
	VPipeSource ();

    //  Destruction
    protected:
	~VPipeSource () {
	}

    //  IObjectSource Role
    private:
	VRole<ThisClass,IObjectSource> m_pIObjectSource;
    public:
	void getRole (VReference<IObjectSource>&rpRole) {
	    m_pIObjectSource.getRole (rpRole);
	}

    //  IPipeSource Role
    private:
	VRole<ThisClass,IPipeSource> m_pIPipeSource;
    public:
	void getRole (VReference<IPipeSource>&rpRole) {
	    m_pIPipeSource.getRole (rpRole);
	}

    //  IObjectSource Methods
    public:
	void Supply (IObjectSource *pRole, IObjectSink *pClient);

    //  IPipeSource Methods
    private:
	virtual void supply_(IPipeSourceClient *pClient) = 0;
    public:
	void Supply (IPipeSource *pRole, IPipeSourceClient *pClient);
    };
}


#endif

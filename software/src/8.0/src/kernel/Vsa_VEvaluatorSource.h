#ifndef Vsa_VEvaluatorSource_Interface
#define Vsa_VEvaluatorSource_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IDataSource.h"

#include "Vsa_IEvaluatorSourceControl.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IEvaluator.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    typedef IVReceiver<IEvaluator*> IEvaluatorSink;
    typedef Vca::IDataSource<IEvaluator*> IEvaluatorSource;

    class VEvaluator;

    class Vsa_API VEvaluatorSource : public Vca::VRolePlayer {
    //  Run Time Type
	DECLARE_ABSTRACT_RTTLITE (VEvaluatorSource, Vca::VRolePlayer);

    //  Starter
    public:
	class Starter;
	friend class Starter;

    //  Construction
    protected:
	VEvaluatorSource (VString const &rStartupQuery);
	VEvaluatorSource ();

    //  Destruction
    protected:
	~VEvaluatorSource () {
	}

    //  IEvaluatorSource Role
    protected:
	Vca::VRole<ThisClass,IEvaluatorSource> m_pIEvaluatorSource;
    public:
	void getRole (IEvaluatorSource::Reference &rpRole) {
	    m_pIEvaluatorSource.getRole (rpRole);
	}

    //  IEvaluatorSource Methods
    public:
	virtual void Supply (IEvaluatorSource *pRole, IEvaluatorSink *pSink) = 0;

    //  IEvaluatorSourceControl Role
    private:
	Vca::VRole<ThisClass,IEvaluatorSourceControl> m_pIEvaluatorSourceControl;
    public:
	void getRole (IEvaluatorSourceControl::Reference &rpRole) {
	    m_pIEvaluatorSourceControl.getRole (rpRole);
	}

    //  IEvaluatorSourceControl Methods
    public:
	void GetStartupQuery (
	    IEvaluatorSourceControl *pRole, IVReceiver<VString const &>*
	);
	void SetStartupQuery (
	    IEvaluatorSourceControl *pRole, VString const &
	);

    //  Update
    public:
	void setStartupQueryTo (VString const &rQuery) {
    	    m_iStartupQuery = rQuery;
        }
    
    //  Access
    public:
	VString const &startupQuery () const {
	    return m_iStartupQuery;
	}

    //  Factory Callbacks
    protected:
	void startEvaluator (IEvaluatorSink *pSink, IEvaluator *pEvaluator);

    //  State
    protected:
	VString m_iStartupQuery;
    };
}


#endif

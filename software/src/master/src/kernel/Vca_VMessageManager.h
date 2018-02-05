#ifndef Vca_VMessageManager_Interface
#define Vca_VMessageManager_Interface

/************************
 *****  Components  ***** 
 ************************/

#include "Vca_VManager.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class ABSTRACT Vca_API VMessageManager : public VManager {
	DECLARE_ABSTRACT_RTTLITE (VMessageManager, VManager);

    //  Construction
    protected:
	VMessageManager ();

    //  Destruction
    protected:
	~VMessageManager ();

    //  Callbacks
    public:
	virtual void onSent ();
	virtual void onTrigger ();
    };

    template <class StakeHolder> class VMessageMonitor : public VMessageManager {
	DECLARE_CONCRETE_RTTLITE (VMessageMonitor<StakeHolder>, VMessageManager);

    //  Construction
    public:
	VMessageMonitor (StakeHolder *pStakeHolder) : m_pStakeHolder (pStakeHolder) {
	}

    //  Destruction
    protected:
	~VMessageMonitor () {
	}

    //  Callbacks
    private:
	void onEnd () OVERRIDE {
	    if (m_pStakeHolder)
		m_pStakeHolder->onEnd ();
	}
	void onError (IError *pInterface, VString const &rMessage) OVERRIDE {
	    if (m_pStakeHolder)
		m_pStakeHolder->onError (pInterface, rMessage);
	}
	void onSent () OVERRIDE {
	    if (m_pStakeHolder)
		m_pStakeHolder->onSent ();
	}
	void onTrigger () OVERRIDE {
	    if (m_pStakeHolder)
		m_pStakeHolder->onTrigger ();
	}

    //  State
    private:
	typename StakeHolder::Reference const m_pStakeHolder;
    };
}


#endif

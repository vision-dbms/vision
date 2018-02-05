#ifndef Vca_VOneAppMain_Interface
#define Vca_VOneAppMain_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VMainProgram.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class App> class VOneAppMain_ : public VMainProgram {
	DECLARE_FAMILY_MEMBERS (VOneAppMain_<App>, VMainProgram);

    //  Aliases
    public:
	typedef VMainProgram::Application ApplicationBase;

    //  DefaultApplication
    public:
	class DefaultApplication : public ApplicationBase {
	    DECLARE_CONCRETE_RTTLITE (DefaultApplication, ApplicationBase);

	//  Aliases
	public:
	    typedef BaseClass::RunState RunState;

	//  Construction
	public:
	    DefaultApplication (VMainProgram *pMain) : BaseClass (pMain, "Default") {
	    }

	//  Destruction
	protected:
	    ~DefaultApplication () {
	    }

	//  Access
	public:
	    RunState runState () const OVERRIDE {
		return m_pImplementation ? m_pImplementation->runState () : RunState_AwaitingStart;
	    }
	    App* implementation () const {
		return m_pImplementation;
	    }

	//  Control
	private:
	    bool start_() OVERRIDE {
	    //  Create, ...
		m_pImplementation.setTo (new App (appContext ()));

	    //  ... register, ...
		registerApplication (m_pImplementation);

	    //  ... and start the application:
		m_pImplementation->start ();

		return true;
	    }

	//  State
	private:
	    typename App::Reference m_pImplementation;
	};

    //  Construction
    public:
	VOneAppMain_(
	    int argc, argv_t argv
	) : BaseClass (argc, argv), m_pTheApp (new DefaultApplication (this)) {
	}

    //  Destruction
    public:
	~VOneAppMain_() {
	}

    //  Access
    public:
	App* theApp () const {
	    return m_pTheApp ? m_pTheApp->implementation () : static_cast<App*>(0);
	}

    //  State
    private:
	typename DefaultApplication::Reference const m_pTheApp;
    };
}



#endif

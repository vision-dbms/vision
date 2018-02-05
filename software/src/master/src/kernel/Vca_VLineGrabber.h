#ifndef Vca_VLineGrabber_Interface
#define Vca_VLineGrabber_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IBSClient.h"

#include "VkBuffer.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**************************
     *----  VLineGrabber  ----*
     **************************/
    class Vca_API VLineGrabber : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VLineGrabber, VRolePlayer);

    //  Aliases
    public:
	typedef VBSProducer BS;

    //  Construction
    protected:
	VLineGrabber (BS *pBSToHere);
    //  ... start must be called from the constructor of the most derived class.
	void start ();

    //  Destruction
    protected:
	~VLineGrabber ();

    //  Roles
    private:
	VRole<ThisClass,IBSClient> m_pBSClient;
	void getRole (VReference<IBSClient>&rpRole) {
	    m_pBSClient.getRole (rpRole);
	}

    //  Callbacks
    public/*private*/:
	void OnEnd (IClient *pRole);
	void OnError (IClient *pRole, IError *pError, VString const &rMessage);
	void OnTransfer (IBSClient *pRole, size_t sTransfer);

    //  Event Generation
    protected:
	virtual bool onData_(char const *pLine, size_t sLine) = 0;
	virtual void onDone_() = 0;
    private:
	void onDone ();

    //  State
    private:
	VReference<BS>	m_pBS;
	VkBuffer	m_iInputBuffer;
	bool		m_bNotStarted;
    };


    /******************************
     *----  VLineGrabber_<T>  ----*
     ******************************/

    template <class T> class VLineGrabber_ : public VLineGrabber {
	DECLARE_CONCRETE_RTTLITE (VLineGrabber_<T>, VLineGrabber);

    //  Construction
    public:
	VLineGrabber_(typename BaseClass::BS *pBS, T *pInputProcessor) : BaseClass (pBS), m_pInputProcessor (pInputProcessor) {
	    start ();
	}

    //  Destruction
    private:
	~VLineGrabber_() {
	}

    //  Callbacks
    private:
	virtual bool onData_(char const *pLine, size_t sLine) OVERRIDE {
	    return m_pInputProcessor->onInputLine (pLine, sLine);
	}
	virtual void onDone_() OVERRIDE {
	    m_pInputProcessor->onInputDone ();
	}

//  State
private:
	typename T::Reference const m_pInputProcessor;
    };
}


#endif

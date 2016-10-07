#ifndef Vca_VMessageStarter_Interface
#define Vca_VMessageStarter_Interface 

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "V_VAddin.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VMessage;

    class VMessageStarter : public V::VAddin {
	DECLARE_FAMILY_MEMBERS (VMessageStarter, V::VAddin);

	friend class VMessage;

    //  Aliases
    public:
	typedef ThisClass		MessageStarter;
	typedef VReference<ThisClass>	Reference;

    //  Construction
    protected:
	VMessageStarter (VReferenceable *pReferenceable) : BaseClass (pReferenceable) {
	}

    //  Destruction
    protected:
	~VMessageStarter () {
	}

    //  Use
    private:
	virtual unsigned int nextMessageIndex () {
	    return m_xNextMessage++;
	}
	virtual void start_(VMessage *pMessage) = 0;
    public:
	void start (VMessage *pMessage) {
	    start_(pMessage);
	}

    //  State
    private:
	counter_t m_xNextMessage;
    };
}


#endif

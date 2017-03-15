#ifndef Vca_VTriggerableTicket_Interface
#define Vca_VTriggerableTicket_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VCallbackTicket.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class ITrigger;

    class Vca_API VTriggerableTicket : public VCallbackTicket {
	DECLARE_ABSTRACT_RTTLITE (VTriggerableTicket, VCallbackTicket);

    //  List
    public:
	typedef VCallbackTicket::List CallbackList;
	class Vca_API List : public CallbackList {
	    DECLARE_FAMILY_MEMBERS (List, CallbackList);

	//  Construction
	public:
	    List () {
	    }

	//  Destruction
	public:
	    ~List () {
	    }

	//  Access
	public:
	    VTriggerableTicket *head () const {
		return static_cast<VTriggerableTicket*>(BaseClass::head ());
	    }

	//  Use
	public:
	    void triggerAll ();
	};
	friend class List;

    //  Construction
    protected:
	VTriggerableTicket (
	    VReferenceable *pListOwner, List &rList
	);

    //  Destruction
    protected:
	~VTriggerableTicket ();

    //  Access
    public:
	ThisClass *successor () const {
	    return static_cast<ThisClass*>(BaseClass::successor ());
	}

    //  Triggering
    private:
	void triggerFromList (ThisClass::Reference &rpSuccessor);
    private:
	virtual void trigger () const = 0;
    };
}


#endif

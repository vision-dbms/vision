#ifndef Vca_VCallbackTicket_Interface
#define Vca_VCallbackTicket_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VSpinlock.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VCallbackTicket : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (VCallbackTicket, VReferenceable);

    //  List
    public:
	class Vca_API List : protected Pointer {
	    DECLARE_FAMILY_MEMBERS (List,void);

	    friend class VCallbackTicket;

	//  Mutex
	public:
	    class Mutex : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Mutex, VReferenceable);

	    //  Aliases
	    public:
		typedef V::VSpinlock Implementation;

	    //  Construction
	    public:
		Mutex ();

	    //  Destruction
	    private:
		~Mutex ();

	    //  Access
	    public:
		Implementation& implementation() {
		    return m_iImplementation;
		}

	    //  State
	    private:
		Implementation m_iImplementation;
	    };
	    typedef Mutex::Implementation MutexImplementation;
	    typedef MutexImplementation::Claim MutexClaim;

	//  Construction
	public:
	    List () {
	    }

	//  Destruction
	public:
	    ~List () {
	    }

	//  Access
	private:
	    MutexImplementation& mutex ();

	protected:
	    VCallbackTicket *head () const {
		return *this;
	    }

	//  Update
	private:
	    void insert (VCallbackTicket *pTicket);
	    void remove (VCallbackTicket *pTicket);

	//  State
	private:
	    Mutex::Reference m_pMutex;
	};
	friend class List;

    //  Status Enumeration
    public:
	enum Status {
	    Status_Active, Status_Canceled
	};

    //  Construction
    protected:
	VCallbackTicket (VReferenceable *pListOwner, List &rList);
    
    //  Destruction
    protected:
	~VCallbackTicket ();

    //  Access/Query
    public:
	bool isActive () const {
	    return m_xStatus == Status_Active;
	}
	bool isntActive () const {
	    return m_xStatus != Status_Active;
	}
	bool isCanceled () const {
	    return m_xStatus == Status_Canceled;
	}
	bool isntCanceled () const {
	    return m_xStatus != Status_Canceled;
	}

    //  Control
    public:
	void cancel ();

    //  Linkage
    private:
	void linkTo (Pointer &rListHead);
	void unlinkFrom (Pointer &rListHead);
    protected:
	ThisClass *successor () const {
	    return m_pSuccessor;
	}
	ThisClass *predecessor () const {
	    return m_pPredecessor;
	}

    //  State
    protected:
	VReferenceable::Reference const m_pListOwner;
	List&	m_rList;
	Pointer	m_pPredecessor;
	Pointer	m_pSuccessor;
        Status	m_xStatus;
    };
}


#endif

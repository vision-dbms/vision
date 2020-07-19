#ifndef VStaticTransient_Interface
#define VStaticTransient_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VEternal.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class V_API VStaticTransient : public virtual VTransient {
    DECLARE_FAMILY_MEMBERS (VStaticTransient, VTransient);

//****************
//  Aliases
public:
    typedef ThisClass Instance;
    typedef Pointer InstancePointer;

//****************
//  class Link
public:
    class V_API Link {
	DECLARE_NUCLEAR_FAMILY (Link);

	friend class VStaticTransient;

    //  Construction
    public:
	Link (Instance *pInstance);

    //  Destruction
    public:
	~Link ();

    //  Access
    public:
	Instance *instance () const {
	    return m_pInstance;
	}

    //  Use
    private:
	bool detach (Instance *pInstance) {
	    return m_pInstance.clearIf (pInstance);
	}

    //  State
    private:
	InstancePointer	m_pInstance;
    public:
	Pointer		m_pListLink;
    };
    typedef V::VEternal<Link> EternalLink;

//****************
//  class List
public:
    class V_API List {
	DECLARE_NUCLEAR_FAMILY (List);

    //  Construction
    public:
	List ();

    //  Destruction
    public:
	~List ();

    //  Use
    public:
	void interlockedPush (Instance *pInstance);
	bool interlockedPop (InstancePointer &rpInstance);

    //  State
    private:
	V::VAtomicallyLinkablePointer<Link> m_pListHead;
    };
    friend class List;
    typedef V::VEternal<List> EternalList;

//****************
//  Construction
protected:
    VStaticTransient ();

//  Destruction
protected:
    ~VStaticTransient ();

//  Initialization
public:
    static void InitializeAllInstances ();
private:
    virtual void initialize () = 0;

//  Access
private:
    static List &listHead () {
	return g_pListHead;
    }
    Link &listLink() {
	return m_pListLink;
    }

//  State
private:
    static EternalList  g_pListHead;
    EternalLink		m_pListLink;
};


#endif

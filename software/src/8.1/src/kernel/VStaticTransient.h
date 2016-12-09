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
	DECLARE_FAMILY_MEMBERS (Link,void);

	friend class VStaticTransient;

    //  Construction
    public:
	Link (Instance* pInstance = 0);

    //  Destruction
    public:
	~Link ();

    //  Access
    public:
	Instance* instance () const {
	    return m_pInstance;
	}

    //  Use
    public:
	void interlockedPush (Instance* pInstance);
	bool interlockedPop (InstancePointer& rpInstance);
    private:
	bool detach (Instance* pInstance) {
	    return m_pInstance.clearIf (pInstance);
	}

    //  State
    private:
	InstancePointer	m_pInstance;
	Pointer		m_pListLink;
    };
    typedef V::VEternal<Link> EternalLink;

    friend class Link;

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
    static Link& listHead () {
	return g_pListHead;
    }
    Link& listLink() {
	return m_pListLink;
    }

//  State
private:
    static EternalLink  g_pListHead;
    EternalLink		m_pListLink;
};


#endif

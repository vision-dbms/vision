#ifndef V_VScopedResource_Interface
#define V_VScopedResource_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "VComponentReference.h"

#include "VkSetOfReferences.h"

#include "V_VAtom.h"

#include "V_VXRef.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /*****************************
     *----  VScopedResource  ----*
     *****************************/

    class V_API ABSTRACT VScopedResource : public VReferenceable {
    //  Run Time Type
	DECLARE_ABSTRACT_RTT (VScopedResource, VReferenceable);

    //  Scope
    public:
	typedef VAtom Scope;

    //  ScopeSet
    public:
	typedef VkSetOfReferences<Scope> ScopeSet;

    //  Construction
    protected:
	VScopedResource ();

    //  Destruction
    protected:
	~VScopedResource ();

    //  Scope Management
    public:
	ScopeSet const &scopeSet () const {
	    return m_iScopeSet;
	}

	bool addScope	(Scope *pScope);
	bool deleteScope(Scope *pScope);

    private:
	virtual void onScopeAddition (Scope *pScope) = 0;
	virtual void onScopeDeletion (Scope *pScope) = 0;

    //  State
    private:
	ScopeSet m_iScopeSet;
    };

    /******************************
     *----  VScopedResource_  ----*
     ******************************/

    template <typename DerivedClass>
    class ABSTRACT VScopedResource_ : public VScopedResource {
    //  Run Time Type
    //	DECLARE_ABSTRACT_RTT (VScopedResource_<DerivedClass>, VScopedResource);

    //  Registry
    public:
	class Registry : public virtual VTransient {
	    DECLARE_FAMILY_MEMBERS (Registry, VTransient);

	//  Scope
	public:
	    typedef VAtom Scope;

	//  ScopeSet
	public:
	    typedef VkSetOfReferences<Scope> ScopeSet;

	//  XRef
	public:
	    typedef VXRef<
		VReference<DerivedClass>, DerivedClass*, DerivedClass const*,
		VReference<Scope>, Scope*, Scope const*
	    > XRef;

	//  Scope Query Result
	public:
	    typedef typename XRef::SetOf1sQueryResult ScopeQueryResult;

	//  Construction
	public:
	    Registry () {
	    }

	//  Deletion
	public:
	    ~Registry () {
	    }

	//  Access
	public:
	    bool queryScope (Scope *pScope, ScopeQueryResult &rResult) const {
		return m_iXRef.locateK2 (pScope, rResult);
	    }

	//  Update
	public:
	    bool insert (DerivedClass *pScopedObject) {
		return m_iXRef.insertKS (pScopedObject, pScopedObject->scope ());
	    }
	    bool insert (DerivedClass *pScopedObject, Scope *pScopeAtom) {
		return m_iXRef.insertKK (pScopedObject, pScopeAtom);
	    }
	    bool insert (DerivedClass *pScopedObject, ScopeSet const &rScopeSet) {
		return m_iXRef.insertKS (pScopedObject, rScopeSet);
	    }

	    bool remove (DerivedClass *pScopedObject) {
		return m_iXRef.removeKS (pScopedObject, pScopedObject->scope ());
	    }
	    bool remove (DerivedClass *pScopedObject, Scope *pScopeAtom) {
		return m_iXRef.removeKK (pScopedObject, pScopeAtom);
	    }
	    bool remove (DerivedClass *pScopedObject, ScopeSet const &rScopeSet) {
		return m_iXRef.removeKS (pScopedObject, rScopeSet);
	    }

	//  State
	private:
	    XRef m_iXRef;
	};

    //  Scope Query Result
    public:
	typedef typename Registry::ScopeQueryResult ScopeQueryResult;

    //  Construction
    protected:
	VScopedResource_() {
	}

    //  Destruction
    protected:
	~VScopedResource_() {
	}

    //  Registration
    public:
	void moveToRegistry (Registry *pRegistry, VReferenceable *pContainer = 0) {
	    if (pRegistry != m_pRegistry) {
		removeFromRegistry ();
		pRegistry->insert (static_cast<DerivedClass*>(this), scopeSet ());
		m_pRegistry.setTo (pContainer, pRegistry);
	    }
	}

	void removeFromRegistry () {
	    if (m_pRegistry) {
		m_pRegistry->remove (static_cast<DerivedClass*>(this), scopeSet ());
		m_pRegistry.clear ();
	    }
	}

	bool isRegistered () const {
	    return m_pRegistry ? true : false;
	}

    //  Scope Query
    public:
	bool queryScope (Scope *pScope, ScopeQueryResult &rResult) const {
	    return m_pRegistry.isntNil () && m_pRegistry.queryScope (pScope, rResult);
	}

    //  Scope Management
    private:
	void onScopeAddition (Scope *pScope) {
	    if (m_pRegistry)
		m_pRegistry->insert (static_cast<DerivedClass*>(this), pScope);
	}
	void onScopeDeletion (Scope *pScope) {
	    if (m_pRegistry)
		m_pRegistry->remove (static_cast<DerivedClass*>(this), pScope);
	}

    //  State
    private:
	VComponentReference<Registry> m_pRegistry;
    };
}


#endif

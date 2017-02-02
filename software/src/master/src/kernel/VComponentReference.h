#ifndef VComponentReference_Interface
#define VComponentReference_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReference.h"

#include "V_VPointer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VReferenceable.h"

/***********************
 *****  Templates  *****
 ***********************/

template<class ComponentClass, class ContainerClass = ::VReferenceable>
class VComponentReference : public VTransient {
//  Family
public:
    typedef VComponentReference<ComponentClass,ContainerClass> ThisClass;
    typedef VTransient BaseClass;

//  Pointer
public:
    typedef VReference<ContainerClass> ContainerReference;
    typedef V::VPointer<ComponentClass> ComponentPointer;

//  Construction
public:
    VComponentReference (ThisClass const &rOther)
    : m_pContainer (rOther.m_pContainer), m_pComponent (rOther.m_pComponent)
    {
    }

    VComponentReference (ContainerClass *pContainer, ComponentClass *pComponent)
    : m_pContainer (pContainer), m_pComponent (pComponent)
    {
    }

    VComponentReference () {
    }

//  Destruction
public:
    ~VComponentReference () {
    }

//  Access
public:
    ContainerClass *container () const {
	return m_pContainer;
    }
    ComponentClass *component () const {
	return m_pComponent;
    }

    ComponentClass &operator* () const {
	return *m_pComponent;
    }
    ComponentClass *operator-> () const {
	return m_pComponent;
    }
    operator ComponentClass* () const {
	return m_pComponent;
    }

//  Query
public:
    bool isEmpty () const {
	return m_pComponent.isNil ();
    }
    bool isntEmpty () const {
	return m_pComponent.isntNil ();
    }

    bool isNil () const {
	return m_pComponent.isNil ();
    }
    bool isntNil () const {
	return m_pComponent.isntNil ();
    }

    bool ownsContainer () const {
	return m_pContainer.ownsReferent ();
    }
    bool sharesContainer () const {
	return m_pContainer.sharesReferent ();
    }

//  Update
public:
    void claim (ThisClass &rOther) {
	m_pContainer.claim (rOther.m_pContainer);
	m_pComponent.claim (rOther.m_pComponent);
    }
    void claim (ContainerClass *pContainer, ComponentClass *pComponent) {
	m_pContainer.claim (pContainer);
	m_pComponent.setTo (pComponent);
    }

    void clear () {
	m_pComponent.clear ();
	m_pContainer.clear ();
    }
    bool clearIf (ComponentClass *pOld) {
	if (!m_pComponent.clearIf (pOld))
	    return false;
	m_pContainer.clear ();
	return true;
    }

    void setTo (ThisClass const &rOther) {
	m_pContainer.setTo (rOther.m_pContainer);
	m_pComponent.setTo (rOther.m_pComponent);
    }
    void setTo (ContainerClass *pContainer, ComponentClass *pComponent) {
	m_pContainer.setTo (pContainer);
	m_pComponent.setTo (pComponent);
    }

    ThisClass &operator= (ThisClass const &rOther) {
	setTo (rOther);
	return *this;
    }

//  Interlocked Update
public:

//  State
protected:
    ContainerReference	m_pContainer;
    ComponentPointer	m_pComponent;
};


#endif

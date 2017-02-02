#ifndef V_VAddin_Interface
#define V_VAddin_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VReferenceable.h"
namespace V {
    /**
     * Base class for objects that serve to provide additional functionality, by way of (multiple) inheritance, to other classes.
     * Overcomes compiler issues supporting virtual base classes -- specifically, inheriting from multiple Referenceable classes -- by delegating Referenceable operations to another Referenceable.
     */
    template <class Referenceable> class VAddin_{
	DECLARE_NUCLEAR_FAMILY (VAddin_<Referenceable>);

    //  Construction
    protected:
        /**
         * Contsructor expects a delegate for Referenceable operations.
         *
         * @param pContainer the delegate for this VAddin_.
         */
	VAddin_(Referenceable *pContainer) : m_pContainer (pContainer) {
	}

    //  Destruction
    protected:
	~VAddin_() {
	}

    /// Referenceable Operations
    /// All of these operations must be defined for Referenceable classes. We (re)define them here to delegate to our Referenceable. Any class that supports these operations is substitutable for VReferenceable (or, can be used as the template parameter for both this class and VReference).
    //@{
    public:
        /** Delegates to m_pContainer. */
	Referenceable const *referenceable () const {
	    return m_pContainer;
	}
        /** @copybrief referenceable() */
	Referenceable *referenceable () {
	    return m_pContainer;
	}
        /** @copybrief referenceable() */
	unsigned int referenceCount () const {
	    return m_pContainer->referenceCount ();
	}
        /** @copybrief referenceable() */
	void retain () {
	    m_pContainer->retain ();
	}
        /** @copybrief referenceable() */
	void release () {
	    m_pContainer->release ();
	}
        /** @copybrief referenceable() */
	void untain () {
	    m_pContainer->untain ();
	}
        /** @copybrief referenceable() */
	void discard () {
	    m_pContainer->discard ();
	}
    //@}

    //  State
    private:
        /** Our delegate. */
	typename Referenceable::Pointer const m_pContainer;
    };
    /** Generic VAddins expect the delegate to be a VReferenceable. */
    typedef VAddin_<VReferenceable> VAddin;
    /** Non-interlocked VAddin. */
    typedef VAddin_<VReferenceableNIL> VAddinNIL;
}


#endif

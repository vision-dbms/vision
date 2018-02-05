#ifndef Vca_VcaGofer_Interface
#define Vca_VcaGofer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_VcaOffer.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPassiveCallback.h"
#include "Vca_IPipeSource.h"

#include "Vca_Namespace_VTraits.h"

#include "Vca_VInterfaceQuery.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IDirectory;
    class IPipeFactory;

    /**
     * Holds Standard Vca-provided gofers and their tools.
     */
    namespace Gofer {
	typedef VcaOffer::Offering offering_t;

	Vca_API void Supply (VInterfaceQuery const &rQuery, offering_t const &rCounterOffer, VString const &rObjectName, IDirectory *pDirectory);
	Vca_API void Supply (VInterfaceQuery const &rQuery, offering_t const &rCounterOffer, VString const &rObjectName, IPipeFactory *pPipeFactory);
	Vca_API void Supply (VInterfaceQuery const &rQuery, offering_t const &rCounterOffer, VString const &rObjectName, VString const &rLibraryName);
	Vca_API void Supply (VInterfaceQuery const &rQuery, offering_t const &rCounterOffer, VString const &rObjectName);
	Vca_API void Supply (VInterfaceQuery const &rQuery, offering_t const &rCounterOffer, IPipeSource *pPipeSource);

	Vca_API void Supply (VInterfaceQuery const &rQuery, VString const &rObjectName);
	Vca_API void Supply (VInterfaceQuery const &rQuery, IPipeSource *pPipeSource);


    /***************************************
     *----  InstanceOfInterface Gofer  ----*
     ***************************************/

	template <class Interface_T> class InstanceOfInterface : public VGoferInterface<Interface_T> {
	    DECLARE_ABSTRACT_RTTLITE (InstanceOfInterface<Interface_T>, VGoferInterface<Interface_T>);

	//  Aliases
	public:
	    typedef Interface_T interface_t;
	    typedef VQueryInterface<Interface_T> query_t;

	//  Construction
	protected:
	    InstanceOfInterface (offering_t const &rCounterOffer) : m_iCounter (rCounterOffer), m_pIObjectSink (this) {
	    }
	    InstanceOfInterface () : m_pIObjectSink (this) {
	    }

	//  Destruction
	protected:
	    ~InstanceOfInterface () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IObjectSink
	private:
	    VRole<ThisClass,IObjectSink> m_pIObjectSink;
	    void getRole (IObjectSink::Reference &rpRole) {
		m_pIObjectSink.getRole (rpRole);
	    }

	//  IObjectSink Methods
	public:
	    void OnData (IObjectSink *pRole, IVUnknown *pObject) {
		BaseClass::setTo (dynamic_cast<interface_t*>(pObject));
	    }

	//  Callbacks and Triggers
	private:
            /**
             * Invokes a VQueryInterface query using the obtained data, then delegates to the derived class using supply_().
             */
	    void onData () OVERRIDE {
		IObjectSink::Reference pObjectSink;
		getRole (pObjectSink);
		VQueryInterface<Interface_T> iQuery (pObjectSink);

                // Delegate to derived class.
		supply_(iQuery, m_iCounter);
	    }
	private:
	    virtual void supply_(query_t const &rQuery, offering_t const &rCounterOffer) = 0;

	//  State
	private:
	    offering_t const m_iCounter;
	};


    /*********************************
     *----  Linked Object Gofer  ----*
     *********************************/

        /**
         * Finds a named interface given a library (namespace) and an entry point.
         *
         * @tparam Interface_T the required interface type.
         */
	template <class Interface_T> class Linked : public InstanceOfInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Linked, InstanceOfInterface<Interface_T>);

	//  Construction
	public:
	    template <typename entry_name_t, typename image_name_t> Linked (
		entry_name_t rEntryName, image_name_t rImageName
	    ) : m_iEntryName (rEntryName), m_iImageName (rImageName) {
	    }

	//  Destruction
	private:
	    ~Linked () {
	    }

	//  Callbacks
	private:
            /**
             * @copybrief Vca::VGofer::onNeed()
             * Materializes the object name and namespace interface.
             */
	    void onNeed () OVERRIDE {
		m_iEntryName.materializeFor (this);
		m_iImageName.materializeFor (this);
		BaseClass::onNeed ();
	    }

	    void supply_(typename BaseClass::query_t const &rQuery, offering_t const &rCounterOffer) OVERRIDE {
		Gofer::Supply (rQuery, rCounterOffer, m_iEntryName, m_iImageName);
	    }

	//  State
	private:
	    VInstanceOf_String m_iImageName;
	    VInstanceOf_String m_iEntryName;
	};


    /********************************
     *----  Named Object Gofer  ----*
     ********************************/

        /**
         * Finds a named interface from within a namespace, which must also be an interface.
         *
         * @tparam Interface_T the required interface type.
         * @tparam Namespace_T the namespace within which to find the required interface.
         */
	template <class Interface_T, class Namespace_T> class Named : public InstanceOfInterface<Interface_T> {
	    typedef Named<Interface_T,Namespace_T> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, InstanceOfInterface<Interface_T>);

	//  Aliases
	public:
	    typedef Interface_T interface_t;
	    typedef typename Namespace::VTraits<Namespace_T>::interface_t namespace_t;
	    typedef typename Namespace::VTraits<Namespace_T>::name_t name_t;

	//  Construction
	public:
            /**
             * @name Internal Constructors
             * @warning These constructors are for internal use only.
             */
            //@{
	    template <typename namme_t, typename namespase_t> Named (
		offering_t const &rCounterOffer, namme_t iObjectName, namespase_t pNamespace
	    ) : BaseClass (rCounterOffer), m_iObjectName (iObjectName), m_pNamespace (pNamespace) {
	    }
	    template <typename namme_t> Named (
		offering_t const &rCounterOffer, namme_t iObjectName
	    ) : BaseClass (rCounterOffer), m_iObjectName (iObjectName), m_pNamespace (static_cast<namespace_t*>(0)) {
	    }
            //@}

            /**
             * Constructor for Named gofers, expects a namespace instance to be provided.
             *
             * @param iObjectName the name (or a gofer for the name) of the required interface.
             * @param pNamespace pointer to (or gofer for) the namespace instance to use for finding the required interface.
             */
	    template <typename namme_t, typename namespase_t> Named (
		namme_t iObjectName, namespase_t pNamespace
	    ) : m_iObjectName (iObjectName), m_pNamespace (pNamespace) {
	    }

            /**
             * Constructor allowing for the use of a default instance of the provided Namespace_T type.
             *
             * @param iObjectName the name (or a gofer for the name) of the required interface.
             */
	    template <typename namme_t> Named (
		namme_t iObjectName
	    ) : m_iObjectName (iObjectName), m_pNamespace (static_cast<namespace_t*>(0)) {
	    }

	//  Destruction
	private:
	    ~Named () {
	    }

	//  Callbacks and Triggers
	private:
            /**
             * @copybrief Vca::VGofer::onNeed()
             * Materializes the object name and namespace interface.
             */
	    void onNeed () OVERRIDE {
		m_iObjectName.materializeFor (this);
		m_pNamespace.materializeFor (this);
		BaseClass::onNeed ();
	    }
            /**
             * Delegates to Gofer::Supply().
             */
	    void supply_(typename BaseClass::query_t const &rQuery, offering_t const &rCounterOffer) OVERRIDE {
		Gofer::Supply (rQuery, rCounterOffer, m_iObjectName, m_pNamespace);
	    }

	//  State
	private:
            /**
             * Refers to an instance of, or gofer for, a VString.
             */
	    VInstanceOf<name_t> m_iObjectName;

            /**
             * Refers to an instance of, or gofer for, a Namespace_T.
             */
	    VInstanceOfInterface<Namespace_T> m_pNamespace;
	};


    /**********************************
     *----  Passive Object Gofer  ----*
     **********************************/

	template <class Interface_T> class Passive : public InstanceOfInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Passive<Interface_T>, InstanceOfInterface<Interface_T>);

	//  Aliases
	public:
	    typedef Interface_T interface_t;

	//  Construction
	public:
	    template <class callback_t> Passive (
		offering_t const &rCounterOffer, callback_t *pCallback
	    ) : BaseClass (rCounterOffer), m_pCallback (pCallback) {
	    }
	    template <class callback_t> Passive (
		callback_t *pCallback
	    ) : m_pCallback (pCallback) {
	    }

	//  Destruction
	private:
	    ~Passive () {
	    }

	//  Callbacks and Triggers
	private:
	    void onNeed () OVERRIDE {
		m_pCallback.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void supply_(typename BaseClass::query_t const &rQuery, offering_t const &rCounterOffer) OVERRIDE {
		rQuery.getObject (m_pCallback);
	    }

	//  State
	private:
	    VInstanceOfInterface<IPassiveCallback> m_pCallback;
	};


    /********************************
     *----  Piped Object Gofer  ----*
     ********************************/

	template <class Interface_T> class Piped : public InstanceOfInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Piped<Interface_T>, InstanceOfInterface<Interface_T>);

	//  Construction
	public:
	    template <typename pipe_source_t> Piped (
		offering_t const &rCounterOffer, pipe_source_t pPipeSource
	    ) : BaseClass (rCounterOffer), m_pPipeSource (pPipeSource) {
	    }
	    template <typename pipe_source_t> Piped (
		pipe_source_t pPipeSource
	    ) : m_pPipeSource (pPipeSource) {
	    }

	//  Destruction
	protected:
	    ~Piped () {
	    }

	//  Callbacks and Triggers
	private:
	    void onNeed () OVERRIDE {
		m_pPipeSource.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void supply_(typename BaseClass::query_t const &rQuery, offering_t const &rCounterOffer) OVERRIDE {
		Gofer::Supply (rQuery, rCounterOffer, m_pPipeSource);
	    }

	//  State
	private:
	    VInstanceOfInterface<IPipeSource> m_pPipeSource;
	};


    /**********************************
     *----  Queried Object Gofer  ----*
     **********************************/

	template <class Interface_T, class Source_T = IVUnknown> class Queried : public InstanceOfInterface<Interface_T> {
	    typedef Queried<Interface_T,Source_T> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, InstanceOfInterface<Interface_T>);

	//  Construction
	public:
	    template <typename object_source_t> Queried (
		offering_t const &rCounterOffer, object_source_t pObjectSource
	    ) : BaseClass (rCounterOffer), m_pObjectSource (pObjectSource) {
	    }
	    template <typename object_source_t> Queried (
		object_source_t pObjectSource
	    ) : m_pObjectSource (pObjectSource) {
	    }

	//  Destruction
	protected:
	    ~Queried () {
	    }

	//  Callbacks and Triggers
	private:
	    void onNeed () OVERRIDE {
		m_pObjectSource.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void supply_(typename BaseClass::query_t const &rQuery, offering_t const &rCounterOffer) OVERRIDE {
		rQuery.getObject (m_pObjectSource);
	    }

	//  State
	private:
	    VInstanceOfInterface<Source_T> m_pObjectSource;
	};


    /**********************************
     *----  Sourced Object Gofer  ----*
     **********************************

     *********************************************************************************************
     *  Can be generalized to any object, not just interfaces, provided interfaces continue to
     *  inherit from VGoferInterface<T> (non-interfaces must inherit from VGoferInstance<T>).
     *********************************************************************************************/

	template <typename Interface_T> class Sourced : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Sourced<Interface_T>, VGoferInterface<Interface_T>);

	//  Aliases
	public:
	    typedef typename BaseClass::IDataReceiver IObjectReceiver;
	    typedef typename BaseClass::IDataSupplier IObjectSupplier;

	//  Construction
	public:
	    template <typename source_t> Sourced (source_t iSource) : m_iSource (iSource), m_pIObjectReceiver (this) {
	    }

	//  Destruction
	private:
	    ~Sourced () {
	    }

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IObjectSink Role
	private:
	    VRole<ThisClass,IObjectReceiver> m_pIObjectReceiver;
	    void getRole (typename IObjectReceiver::Reference &rpRole) {
		m_pIObjectReceiver.getRole (rpRole);
	    }

	//  Callbacks and Triggers
	public:
	    void OnData (IObjectReceiver *pRole, Interface_T *pInstance) {
		BaseClass::setTo (pInstance);
	    }
	private:
	    void onData () OVERRIDE {
		if (!m_iSource)
		    BaseClass::setTo (static_cast<Interface_T*>(0));
		else {
		    typename IObjectReceiver::Reference pSink;
		    getRole (pSink);
		    m_iSource->Supply (pSink);
		}
	    }
	    void onNeed () OVERRIDE {
		m_iSource.materializeFor (this);
		BaseClass::onNeed ();
	    }

	//  State
	private:
	    VInstanceOfInterface<IObjectSupplier> m_iSource;
	};


    /****************************
     *----  Identity Gofer  ----*
     ****************************

     *********************************************************************************************
     *  Can be generalized to any object, not just interfaces, provided interfaces continue to
     *  inherit from VGoferInterface<T> (non-interfaces must inherit from VGoferInstance<T>).
     *********************************************************************************************/

	template <typename Interface_T> class Identity : public VGoferInterface<Interface_T> {
	    DECLARE_CONCRETE_RTTLITE (Identity<Interface_T>, VGoferInterface<Interface_T>);

	//  Construction
	public:
	    template <typename source_t> Identity (source_t iSource) : m_iSource (iSource) {
	    }

	//  Destruction
	private:
	    ~Identity () {
	    }

	//  Callbacks and Triggers
	private:
	    void onData () OVERRIDE {
		BaseClass::setTo (m_iSource);
	    }
	    void onNeed () OVERRIDE {
		m_iSource.materializeFor (this);
		BaseClass::onNeed ();
	    }

	//  State
	private:
	    VInstanceOfInterface<Interface_T> m_iSource;
	};
    }  //  namespace Gofer

} // namespace Vca


#endif

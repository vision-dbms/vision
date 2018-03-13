#ifndef Vxa_VCallAgent_Interface
#define Vxa_VCallAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_IVSNFTaskImplementation3.h"
#include "Vxa_IVSNFTaskImplementation3NC.h"

#include "Vxa_VAny.h"
#include "Vxa_VPack.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VImportable.h"
#include "Vxa_VParameter.h"
#include "Vxa_VTask.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VImportableType;

/***************************************
 *----  class VTicketObjectHolder  ----*
 ***************************************/
    class VCollectableObject;
    class VObjectHolder : public VReferenceable {
        DECLARE_ABSTRACT_RTTLITE (VObjectHolder, VReferenceable);

    protected:
        VObjectHolder () {
        }
        ~VObjectHolder () {
        }
    public:
        template <typename object_value_t> bool getObject (
            object_value_t &rpObject, VString const &rObjectTicket
        ) {
            VCollectableObject *pObject = 0;
            if (getObject (pObject, rObjectTicket)) {
                rpObject = dynamic_cast<object_value_t>(pObject);
                return true;
            }
            return false;
        }
        bool getObject (VCollectableObject* &rpObject, VString const &rObjectTicket) {
            return false;
        }
    };

/******************************
 *----  class VCallAgent  ----*
 ******************************/
    class VCallAgent : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VCallAgent, VRolePlayer);

    //  Aliases
    public:
	typedef Vxa::IVSNFTaskImplementation3   ICallType1Implementation_T;
	typedef Vxa::IVSNFTaskImplementation3NC ICallType1ImplementationNC_T;

	typedef Vxa::IVSNFTaskImplementation3NC ICallType2Implementation_T;

    /*********************************************************
     *----  template <typename storage_t> class Wrapped  ----*
     *********************************************************/
    public:
        template <typename storage_t> class Wrapped {
        //  Aliases
        public:
            typedef typename storage_t::element_t element_t;

        //  Construction
        public:
            Wrapped (
                storage_t const &rData, VCallAgent *pAgent
            ) : m_pCursor (pAgent->taskCursor ()), m_iData (rData) {
            }
            Wrapped (
                Wrapped const &rOther
            ) : m_pCursor (rOther.m_pCursor), m_iData (rOther.m_iData) {
            }

        //  Destruction
        public:
            ~Wrapped () {
            }

        //  Access
        public:
            template <typename consumer_t> bool supply (consumer_t &rConsumer) const {
                return rConsumer.consume (value ());
            }
            operator element_t const& () const {
                return value ();
            }
            element_t const& value () const {
                return m_iData[m_pCursor->position()];
            }

        //  State
        private:
            VCursor::Reference	const m_pCursor;
            storage_t		const m_iData;
        };

    /*********************************************************
     *----  template <typename storage_t> class Boolean  ----*
     *********************************************************/
    public:
        template <typename storage_t> class Boolean {
        //  Construction
        public:
            template <typename wrapped_storage_t> Boolean (
                wrapped_storage_t const &rData, VCallAgent *pAgent
            ) : m_iData (storage_t (rData, pAgent)) {
            }
            Boolean (storage_t const &rData) : m_iData (rData) {
            }
            Boolean (Boolean const &rOther) : m_iData (rOther.m_iData) {
            }

        //  Destruction
        public:
            ~Boolean () {
            }

        //  Access
        public:
            template <typename consumer_t> bool supply (consumer_t &rConsumer) const {
                return rConsumer.consume (value ());
            }
            operator bool () const {
                return value ();
            }
            bool value () const {
                return m_iData.value () ? true : false;
            }

        //  State
        private:
            storage_t const m_iData;
        };

    /****************************************************************
     *----  template <typename storage_t> class TicketedObject  ----*
     ****************************************************************/
    public:
        template <typename storage_t,typename object_value_t> class TicketedObject {
        //  Aliases
        public:
            typedef object_value_t element_t;

        //  Construction
        public:
            template <typename wrapped_storage_t> TicketedObject (
                wrapped_storage_t const &rData, VCallAgent *pAgent
            ) : m_iData (storage_t (rData, pAgent)) {
            }
            TicketedObject (
                storage_t const &rData
            ) : m_iData (rData) {
            }
            TicketedObject (
                TicketedObject const &rOther
            ) : m_iData (rOther.m_iData) {
            }

        //  Destruction
        public:
            ~TicketedObject () {
            }

        //  Access
        private:
            bool getObject (object_value_t &rpObject) const {
                return m_pObjectHolder && m_pObjectHolder->getObject (rpObject, m_iData);
            }
        protected:
            template <typename consumer_t> bool supplyAlways (consumer_t &rConsumer) const {
                return supply (rConsumer) || m_iData.supply (rConsumer);
            }
        public:
            template <typename consumer_t> bool supply (consumer_t &rConsumer) const {
                object_value_t pObject = 0;
                return getObject (pObject) && rConsumer.consume (pObject);
            }
            operator object_value_t () const {
                return value ();
            }
            object_value_t value () const {
                object_value_t pObject = 0;
                getObject (pObject);
                return pObject;
            }

        //  State
        private:
            storage_t		       const m_iData;
            VObjectHolder::Reference mutable m_pObjectHolder;
        };

    /*********************************************************************
     *----  template <typename storage_t> class MaybeTicketedObject  ----*
     *********************************************************************/
    public:
        template <typename storage_t,typename object_value_t> class MaybeTicketedObject
            : public TicketedObject<storage_t,object_value_t>
        {
        //  Aliases
        public:
            typedef TicketedObject<storage_t,object_value_t> BaseClass;

        //  Construction
        public:
            template <typename wrapped_storage_t> MaybeTicketedObject (
                wrapped_storage_t const &rData, VCallAgent *pAgent
            ) : BaseClass (rData, pAgent) {
            }
            MaybeTicketedObject (
                storage_t const &rData
            ) : BaseClass (rData) {
            }
            MaybeTicketedObject (
                MaybeTicketedObject const &rOther
            ) : BaseClass (rOther) {
            }

        //  Destruction
        public:
            ~MaybeTicketedObject () {
            }

        //  Access
        public:
            template <typename consumer_t> bool supply (consumer_t &rConsumer) const {
                return this->supplyAlways (rConsumer);
            }
        };


    /******************************
     *----  ParameterFactory  ----*
     ******************************/
    public:
	class ParameterFactory : public VReferenceable {
	    DECLARE_ABSTRACT_RTTLITE (ParameterFactory, VReferenceable);

	//  Construction
	protected:
	    ParameterFactory (VImportableType *pType) {
	    }

	//  Destruction
	protected:
	    ~ParameterFactory () {
	    }

	//  Access
	public:
	    VImportableType *type () const {
		return m_pType;
	    }

	//  Callbacks
	public:
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) {
		return pAgent->raiseParameterTypeException (typeid(*this), typeid (rValues));
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) {
		return pAgent->raiseParameterTypeException (typeid(*this), typeid (rValues));
	    }
	    virtual bool createFromStrings (str_array_t const &rValues, VCallAgent *pAgent) {
		return pAgent->raiseParameterTypeException (typeid(*this), typeid (rValues));
	    }

	//  State
	private:
	    VImportableType::Pointer const m_pType;
	};
	typedef ParameterFactory::Reference factory_reference_t;
	typedef VkDynamicArrayOf<factory_reference_t> factory_vector_t;


    /*************************************************************************
     *----  template <typename scalar_return_t> class ParameterFactory_  ----*
     *************************************************************************/
    public:
	template <typename scalar_return_t> class ParameterFactory_ : public ParameterFactory {
	    DECLARE_ABSTRACT_RTTLITE (ParameterFactory_<scalar_return_t>, ParameterFactory);

	//  Aliases
	public:
	    typedef typename scalar_return_t::ReferencedClass scalar_t;
            typedef typename scalar_t::value_t scalar_value_t;

	//  Construction
	protected:
	    ParameterFactory_(
		VImportableType *pType, scalar_return_t &rpResult
	    ) : BaseClass (pType), m_rpResult (rpResult) {
	    }

	//  Destruction
	protected:
	    ~ParameterFactory_() {
	    }

	//  Update
	protected:
            template <typename storage_t> bool setResultTo (
                storage_t const &rData
            ) const {
                m_rpResult.setTo (
                    new VParameter<storage_t,scalar_value_t> (this->type (), rData)
                );
                return true;
            }

	//  State
	private:
	    scalar_return_t &m_rpResult;
	};


    /***************************************************************************
     *----  template <typename scalar_return_t> class AnyParameterFactory  ----*
     ***************************************************************************/
    public:
	template <typename scalar_return_t> class AnyParameterFactory
            : public ParameterFactory_<scalar_return_t>
        {
	    DECLARE_CONCRETE_RTTLITE (AnyParameterFactory<scalar_return_t>, ParameterFactory_<scalar_return_t>);

        //  Aliases
        public:
            typedef typename BaseClass::scalar_value_t scalar_value_t;

        //  Any Storage
        public:
            template <typename storage_t> class Any : public VAny {
            //  Aliases
            public:
                typedef typename storage_t::element_t value_t;

            //  Construction
            public:
                template <typename wrapped_storage_t> Any (
                    wrapped_storage_t const &rData, VCallAgent *pAgent
                ) : m_iData (storage_t (rData, pAgent)) {
                }
                Any (
                    storage_t const &rData
                ) : m_iData (rData) {
                }
                Any (
                    Any const &rOther
                ) : m_iData (rOther.m_iData) {
                }

            //  Destruction
            public:
                ~Any () {
                }

            //  Access
            public:
                operator VAny const& () const {
                    return value ();
                }
                VAny const & value () const {
                    return *this;
                }
                virtual void supply (Client &rClient) const OVERRIDE {
                    m_iData.supply (rClient);
                }

            //  State
            private:
                storage_t m_iData;
            };

	//  Construction
	public:
	    AnyParameterFactory (
                VImportableType *pType, scalar_return_t &rpResult
            ) : BaseClass (pType, rpResult) {
	    }

	//  Destruction
	protected:
	    ~AnyParameterFactory () {
	    }

	//  Callbacks
	private:
            template <typename storage_t> bool createUsingWrapped (
                storage_t const &rValues, VCallAgent *pAgent
            ) {
                return this->setResultTo (Any<Wrapped<storage_t> > (rValues, pAgent));
            }
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
                return createUsingWrapped (rValues, pAgent);
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
                return createUsingWrapped (rValues, pAgent);
	    }
	    virtual bool createFromStrings (str_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (
                    Any<MaybeTicketedObject<Wrapped<str_array_t>,VCollectableObject*> >(rValues, pAgent)
                );

	    }
	};


    /*******************************************************************************
     *----  template <typename scalar_return_t> class BooleanParameterFactory  ----*
     *******************************************************************************/

    public:
	template <typename scalar_return_t> class BooleanParameterFactory : public ParameterFactory_<scalar_return_t> {
	    DECLARE_CONCRETE_RTTLITE (BooleanParameterFactory<scalar_return_t>, ParameterFactory_<scalar_return_t>);

	//  Construction
	public:
	    BooleanParameterFactory (VImportableType *pType, scalar_return_t &rpResult) : BaseClass (pType, rpResult) {
	    }

	//  Destruction
	protected:
	    ~BooleanParameterFactory () {
	    }

	//  Callbacks
	private:
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
                return this->setResultTo (Boolean<Wrapped<i32_array_t> >(rValues, pAgent));
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (Boolean<Wrapped<f64_array_t> >(rValues, pAgent));
	    }
	};


    /*******************************************************************************
     *----  template <typename scalar_return_t> class NumericParameterFactory  ----*
     *******************************************************************************/

    public:
	template <typename scalar_return_t> class NumericParameterFactory : public ParameterFactory_<scalar_return_t> {
	    DECLARE_CONCRETE_RTTLITE (NumericParameterFactory<scalar_return_t>, ParameterFactory_<scalar_return_t>);

	//  Construction
	public:
	    NumericParameterFactory (VImportableType *pType, scalar_return_t &rpResult) : BaseClass (pType, rpResult) {
	    }

	//  Destruction
	protected:
	    ~NumericParameterFactory () {
	    }

	//  Callbacks
	private:
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (Wrapped<i32_array_t> (rValues, pAgent));
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (Wrapped<f64_array_t> (rValues, pAgent));
	    }
	};


    /*******************************************************************************
     *----  template <typename scalar_return_t> class ObjectParameterFactory  ----*
     *******************************************************************************/

    public:
	template <typename scalar_return_t> class ObjectParameterFactory : public ParameterFactory_<scalar_return_t> {
	    DECLARE_CONCRETE_RTTLITE (ObjectParameterFactory<scalar_return_t>, ParameterFactory_<scalar_return_t>);

        //  Aliases
        public:
            typedef typename BaseClass::scalar_value_t object_value_t;

	//  Construction
	public:
	    ObjectParameterFactory (VImportableType *pType, scalar_return_t &rpResult) : BaseClass (pType, rpResult) {
	    }

	//  Destruction
	protected:
	    ~ObjectParameterFactory () {
	    }

	//  Callbacks
	private:
	    virtual bool createFromStrings (str_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (
                    TicketedObject<Wrapped<str_array_t>,object_value_t>(rValues, pAgent)
                );
	    }
	};


    /******************************************************************************
     *----  template <typename scalar_return_t> class StringParameterFactory  ----*
     ******************************************************************************/

    public:
	template <typename scalar_return_t> class StringParameterFactory : public ParameterFactory_<scalar_return_t> {
	    DECLARE_CONCRETE_RTTLITE (StringParameterFactory<scalar_return_t>, ParameterFactory_<scalar_return_t>);

	//  Construction
	public:
	    StringParameterFactory (VImportableType *pType, scalar_return_t &rpResult) : BaseClass (pType, rpResult) {
	    }

	//  Destruction
	protected:
	    ~StringParameterFactory () {
	    }

	//  Callbacks
	private:
	    virtual bool createFromStrings (str_array_t const &rValues, VCallAgent *pAgent) OVERRIDE {
		return this->setResultTo (Wrapped<str_array_t> (rValues, pAgent));
	    }
	};

    /*****************************************************************/
    //  Construction
    protected:
	VCallAgent (VTask *pTask, unsigned int cParameters);

    //  Destruction
    protected:
	~VCallAgent ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  Role IVSNFTaskImplementation
    private:
	Vca::VRole<ThisClass,ICallType1Implementation_T> m_pIVSNFTaskImplementation;
    public:
	void getRole (ICallType1Implementation_T::Reference &rpRole) {
	    m_pIVSNFTaskImplementation.getRole (rpRole);
	}

    //  Role IVSNFTaskImplementationNC
    private:
	Vca::VRole<ThisClass,ICallType1ImplementationNC_T> m_pIVSNFTaskImplementationNC;
    public:
	void getRole (ICallType1ImplementationNC_T::Reference &rpRole) {
	    m_pIVSNFTaskImplementationNC.getRole (rpRole);
	}

    //  IVSNFTaskImplementation Methods
    public:
	void SetToInteger (
	    IVSNFTaskImplementation *, unsigned int xParameter, i32_array_t const &rValue
	);
	void SetToDouble (
	    IVSNFTaskImplementation *, unsigned int xParameter, f64_array_t const &rValue
	);
	void SetToVString (
	    IVSNFTaskImplementation *, unsigned int xParameter, str_array_t const &rValue
	);
	void SetToObjects (
	    IVSNFTaskImplementation *, unsigned int xParameter, obj_array_t const &rValue
	);

    //  once used as an interface member, but now a dummy member...
	void PopulateVariantCompleted (IVSNFTaskImplementation * pRole) {
	};

    //  IVSNFTaskImplementation2 Methods
    public:
	void SetToS2Integers (
	    IVSNFTaskImplementation2*, unsigned int xParameter, i32_s2array_t const &rValue
	);

    //  IVSNFTaskImplementation3 Methods
    public:
	void OnParameterError (
	    IVSNFTaskImplementation3*, unsigned int xParameter, VString const &rMsg
	);

    //  IVSNFTaskImplementation3NC Methods
    public:
	void OnParameterError (
	    IVSNFTaskImplementation3NC*, unsigned int xParameter, VString const &rMsg
	);

    private:
	void OnParameterError (
	    unsigned int xParameter, VString const &rMsg
	);

    //  Access/Query
    public:
	bool moreToDo () const {
	    return m_pTask->moreToDo ();
	}

        cardinality_t parameterIndex () const {
            return m_xParameter;
        }
        cardinality_t parameterCountRemaining () const {
            return m_cParameters - m_xParameter;
        }
            
	VTask *task () const {
	    return m_pTask;
	}
	VTaskCursor *taskCursor () const {
	    return m_pTask->cursor ();
	}

    //  Parameter Acquisition
    private:
	virtual bool onParameterRequest (VTask *pTask, unsigned int xParameter) const = 0;
	virtual bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const = 0;

	bool getParameterFactory (factory_reference_t &rpParameterFactory, unsigned int xParameter);
	bool setParameterFactory (ParameterFactory *pParameterFactory);
    public:
	template <typename scalar_result_t> bool getAnyParameter (VImportableType *pType, scalar_result_t &rpResult) {
	    return moreToDo () && setParameterFactory (
		new AnyParameterFactory<scalar_result_t> (pType, rpResult)
	    );
	}
	template <typename scalar_result_t> bool getBooleanParameter (VImportableType *pType, scalar_result_t &rpResult) {
	    return moreToDo () && setParameterFactory (
		new BooleanParameterFactory<scalar_result_t> (pType, rpResult)
	    );
	}
	template <typename scalar_result_t> bool getNumericParameter (VImportableType *pType, scalar_result_t &rpResult) {
	    return moreToDo () && setParameterFactory (
		new NumericParameterFactory<scalar_result_t> (pType, rpResult)
	    );
	}
	template <typename scalar_result_t> bool getObjectParameter (VImportableType *pType, scalar_result_t &rpResult) {
	    return moreToDo () && setParameterFactory (
		new ObjectParameterFactory<scalar_result_t> (pType, rpResult)
	    );
	}
	template <typename scalar_result_t> bool getStringParameter (VImportableType *pType, scalar_result_t &rpResult) {
	    return moreToDo () && setParameterFactory (
		new StringParameterFactory<scalar_result_t> (pType, rpResult)
	    );
	}

    //  Exception Generation
    private:
	virtual bool returnError_(VString const &rMessage) const = 0;
    public:
	virtual bool raiseParameterTypeException (
	    std::type_info const &rOriginatingType, std::type_info const &rResultType
	) const = 0;
	virtual bool raiseUnimplementedOperationException (
	    std::type_info const &rOriginatingType, char const *pWhere
	) const = 0;
	
	bool returnError (VString const &rMessage) const;

    //  Reporting
    private:
	void onFailure (Vca::IError *pInterface, VString const &rMessage);

	virtual void reportCompletion () const = 0;

    //  State
    private:
	VTask::Reference  const m_pTask;
	unsigned int      const m_cParameters;
	unsigned int		m_xParameter;
	factory_vector_t	m_pFactory;
    };

/****************************************************************/

    template <class Handle_T> class VCallAgentFor : public VCallAgent {
	DECLARE_CONCRETE_RTTLITE (VCallAgentFor<Handle_T>, VCallAgent);

    //  Aliases
    public:
	typedef Handle_T handle_t;

    //  Construction
    public:
	VCallAgentFor (VTask *pTask, handle_t const &rHandle) : BaseClass (pTask, rHandle.parameterCount ()), m_iHandle (rHandle) {
	}

    //  Destruction
    private:
	~VCallAgentFor () {
	}

    //  Implementation
    private:
	virtual bool onParameterRequest (VTask *pTask, unsigned int xParameter) const OVERRIDE {
	    return m_iHandle.onParameterRequest (pTask, xParameter);
	}
	virtual bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const OVERRIDE {
	    return m_iHandle.onParameterReceipt (pTask, xParameter);
	}

	virtual bool raiseParameterTypeException (
	    std::type_info const &rOriginatingType, std::type_info const &rResultType
	) const OVERRIDE {
	    return m_iHandle.raiseParameterTypeException (rOriginatingType, rResultType);
	}
	virtual bool raiseUnimplementedOperationException (
	    std::type_info const &rOriginatingType, char const *pWhere
	) const OVERRIDE {
	    return m_iHandle.raiseUnimplementedOperationException (rOriginatingType, pWhere);
	}

	virtual void reportCompletion () const OVERRIDE {
	    return m_iHandle.reportCompletion ();
	}

	virtual bool returnError_(VString const &rMessage) const OVERRIDE {
	    return m_iHandle.returnError (rMessage);
	}

    //  State
    private:
	handle_t m_iHandle;
    };
}


#endif

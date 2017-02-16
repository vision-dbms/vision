#ifndef Vxa_VCallAgent_Interface
#define Vxa_VCallAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_IVSNFTaskImplementation3.h"
#include "Vxa_IVSNFTaskImplementation3NC.h"

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

    class VCallAgent : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VCallAgent, VRolePlayer);

    //  Aliases
    public:
	typedef Vxa::IVSNFTaskImplementation3   ICallType1Implementation_T;
	typedef Vxa::IVSNFTaskImplementation3NC ICallType1ImplementationNC_T;

	typedef Vxa::IVSNFTaskImplementation3NC ICallType2Implementation_T;

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
	    bool setResultTo (scalar_t *pResult) const {
		m_rpResult.setTo (pResult);
		return true;
	    }

	//  State
	private:
	    scalar_return_t &m_rpResult;
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
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) {
		BaseClass::setResultTo (new VBooleanParameter<i32_array_t> (BaseClass::type (), rValues, pAgent->taskCursor ()));
		return true;
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) {
		BaseClass::setResultTo (new VBooleanParameter<f64_array_t> (BaseClass::type (), rValues, pAgent->taskCursor ()));
		return true;
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
	    virtual bool createFromIntegers (i32_array_t const &rValues, VCallAgent *pAgent) {
		BaseClass::setResultTo (new VParameter<scalar_return_t,i32_array_t> (BaseClass::type (), rValues, pAgent->taskCursor ()));
		return true;
	    }
	    virtual bool createFromDoubles (f64_array_t const &rValues, VCallAgent *pAgent) {
		BaseClass::setResultTo (new VParameter<scalar_return_t,f64_array_t> (BaseClass::type (), rValues, pAgent->taskCursor ()));
		return true;
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
	    virtual bool createFromStrings (str_array_t const &rValues, VCallAgent *pAgent) {
		BaseClass::setResultTo (new VParameter<scalar_return_t,str_array_t> (BaseClass::type (), rValues, pAgent->taskCursor ()));
		return true;
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
	virtual bool onParameterRequest (VTask *pTask, unsigned int xParameter) const {
	    return m_iHandle.onParameterRequest (pTask, xParameter);
	}
	virtual bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const {
	    return m_iHandle.onParameterReceipt (pTask, xParameter);
	}

	virtual bool raiseParameterTypeException (
	    std::type_info const &rOriginatingType, std::type_info const &rResultType
	) const {
	    return m_iHandle.raiseParameterTypeException (rOriginatingType, rResultType);
	}
	virtual bool raiseUnimplementedOperationException (
	    std::type_info const &rOriginatingType, char const *pWhere
	) const {
	    return m_iHandle.raiseUnimplementedOperationException (rOriginatingType, pWhere);
	}

	virtual void reportCompletion () const {
	    return m_iHandle.reportCompletion ();
	}

	virtual bool returnError_(VString const &rMessage) const {
	    return m_iHandle.returnError (rMessage);
	}

    //  State
    private:
	handle_t m_iHandle;
    };
}


#endif

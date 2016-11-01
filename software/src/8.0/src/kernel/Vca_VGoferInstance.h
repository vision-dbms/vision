#ifndef Vca_VGoferInstance_Interface
#define Vca_VGoferInstance_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGofer.h"

#include "Vca_IDataSource.h"

#include "Vca_VInstanceSource.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

#define DECLARE_GOFER_INSTANCE(storageclass,api,type)\
    DECLARE_TEMPLATE_INSTANCE(storageclass,api,Vca::VGoferInstance<type >)

#ifdef _WIN32
#define DECLARE_TEMPLATE_INSTANCE(storageclass,api,t)\
    storageclass template class api t;
#else
#ifdef __VMS
#pragma message disable(EMPMACARG)
#endif
#define DECLARE_TEMPLATE_INSTANCE(storageclass,api,t)
#endif


namespace Vca {

/*******************************************************************************************************************
 *----  template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VGoferInstance  -----*
 *******************************************************************************************************************/

    /**
     * Gofer subclass templated for easy retrieval of object instances of any type.
     * 
     * In general, usage of VGoferInstance simply involves:
     *   - Construction
     *   - Invocation of supplyMembers().
     *   
     * @see supplyMembers()
     */
    template <
	typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t
    > class VGoferInstance : public VGofer, protected VInstanceSource<Val_T,Var_T> {
	typedef BOGUS_TYPENAME VGoferInstance<Val_T,Var_T> this_t;
	DECLARE_ABSTRACT_RTTLITE (this_t, VGofer);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;

        /**
         * Our VInstanceSource superlcass.
         */
	typedef VInstanceSource<Val_T,Var_T> SourceBase;
	typedef typename SourceBase::Datum SourceBaseDatum;
	typedef typename SourceBase::Error SourceBaseError;

	typedef IDataSource<Val_T> IDataSupplier;
	typedef IVReceiver<Val_T>  IDataReceiver;

    //  Construction
    protected:
	VGoferInstance (Val_T iValue) : m_pIDataSupplier (this) {
	    setTo (iValue);
	}
	VGoferInstance () : m_pIDataSupplier (this)  {
	}

    //  Destruction
    protected:
	~VGoferInstance () {
	}

    //  Base Roles
    public:
#ifndef _WIN32
	using BaseClass::getRole;
#endif

    //  IDataSupplier
    private:
	VRole<ThisClass,IDataSupplier> m_pIDataSupplier;
    public:
	void getRole (typename IDataSupplier::Reference &rpRole) {
	    m_pIDataSupplier.getRole (rpRole);
	}

    //  IDataSupplier Callbacks
    public:
	void Supply (IDataSupplier *pRole, IDataReceiver *pReceiver) {
	    supplyReceiver (pReceiver);
	}

    //  Callbacks
    public:
        /**
         * If we have a fallback (ElseUse), delegate supplyMembers responsibility to it; otherwise, handle the error ourselves using SourceBase.
         */
	virtual void onError (IError *pInterface, VString const &rMessage) {
	    if (m_pFallback) {
		m_pFallback->supplyMembers (this, &ThisClass::setTo, &ThisClass::onFallbackError);
	    } else {
		onGoferingFailure (pInterface, rMessage);
		SourceBase::setTo (pInterface, rMessage);
	    }
	}
    private:
	void onFallbackError (IError *pInterface, VString const &rMessage) {
	    onGoferingFailure (pInterface, rMessage);
	    SourceBase::setTo (pInterface, rMessage);
	}
    protected:
	virtual void onNeed () {
	    BaseClass::onNeed ();
	}
	void onReset () {
	    SourceBase::unset ();
	    BaseClass::onReset ();
	}
    private:
	void onValueNeeded () {
	    BaseClass::onValueNeeded ();
	}
	void onValueReset () {
	    BaseClass::onResetNeeded ();
	}

    //  Access
    private:
	ThisClass *fallback () const {
	    return m_pFallback;
	}
    public:
        /**
         * Requests a value from this gofer.
         * This is the most general entry point for consumers of VGoferInstance classes allowing an arbitrary class implementing the following operator calls:
         *  - operator() (Val_T)
         *  - operator() (Vca::IError*, VString const &)
         */
	template <typename callback_t> void supply (callback_t const &rCallback) {
	    SourceBase::supply (rCallback);
	}

        /**
         * Should be called by the consumer in order to ask this Gofer to supply its result.
         * Parameters of this method indicate which callbacks ought to be invoked under resultant conditions.
         * Implemented by calling the SourceBase class' supplyMembers method, which winds up calling onNeed().
         *
         * Usage: The consumer must have the following methods defined (typeically in itself) with no particular stipulations on method names:
         *   - void onGoferResult (Val_T); called when this Gofer's result has been retrieved, with the retrieved value passed in as the method call parameter.
         *   - void onGoferError (IError*, VString const&); called when this Gofer's retrieval process resulted in an error, with the IError and error message as the method call parameters.
         *
         * @param pClassInstance the object instance on which callbacks will be invoked.
         * @param pDataMember the callback that should be invoked when this Gofer's result has been retrieved.
         * @param pErrorMember the callback that should be invoked when this Gofer's retrieval process resulted in an error.
         */
	template <typename class_t> void supplyMembers (
	    class_t *pClassInstance,
	    void (class_t::*pDataMember)(Val_T),
	    void (class_t::*pErrorMember)(IError*, VString const&)
	) {
	    SourceBase::supplyMembers (pClassInstance, pDataMember, pErrorMember);
	}
        /**
         * Winds up calling onNeed().
         */
	void supplyReceiver (IDataReceiver *pReceiver) {
	    SourceBase::supplyReceiver (pReceiver);
	}
        /**
         * Winds up calling onNeed().
         */
	template <typename variable_t> void supplyVariable (VGofer *pDependent, variable_t &rVariable) {
	    VInstanceGoferCallback<VGofer,Val_T,variable_t> iCallback (pDependent, rVariable);
	    SourceBase::supply (iCallback);
	}

    //  Query
    public:
        /**
         * Delegates to SourceBase.
         */
	bool validatesDatum (Val_T iValue) const {
	    return SourceBase::validatesDatum (iValue);
	}
        /**
         * Delegates to SourceBase.
         */
	bool validatesError (IError *pInterface, VString const &rMessage) const {
	    return SourceBase::validatesError (pInterface, rMessage);
	}

    //  Update
    protected:
	void setTo (Val_T iValue) {
	    onGoferingSuccess ();
	    SourceBase::setTo (iValue);
	}

    //  Fallback
    public:
	void setFallbackTo (ThisClass *pFallback) {
	    m_pFallback.setTo (pFallback);
	}

    //  Validation
    protected:
        /**
         * Typically called by SourceBase when validating.
         * Delegates to BaseClass.
         */
	virtual bool validatesError_(IError *pInterface, VString const &rMessage) const {
	    return BaseClass::validatesError (pInterface, rMessage);
	}

    //  State
    private:
        /**
         * Implements ElseUse.
         */
	Reference m_pFallback;
    };


/*********************
 *----  Aliases  ----*
 *********************/

    typedef VGoferInstance<bool> VGoferInstance_bool;

    typedef VGoferInstance<F32>	VGoferInstance_F32;
    typedef VGoferInstance<F64>	VGoferInstance_F64;

    typedef VGoferInstance<S08>	VGoferInstance_S08;
    typedef VGoferInstance<S16>	VGoferInstance_S16;
    typedef VGoferInstance<S32>	VGoferInstance_S32;

    typedef VGoferInstance<U08>	VGoferInstance_U08;
    typedef VGoferInstance<U16>	VGoferInstance_U16;
    typedef VGoferInstance<U32>	VGoferInstance_U32;

    typedef VGoferInstance<VString const&> VGoferInstance_String;
}


#endif

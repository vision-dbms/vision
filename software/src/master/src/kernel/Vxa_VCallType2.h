#ifndef Vxa_VCallType2_Interface
#define Vxa_VCallType2_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallHandle.h"

#include "Vxa_VCollectableCollectionOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICaller.h"

#include "Vxa_VScalar.h"
#include "Vxa_VTaskCursor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <class Handle_T> class VCallAgentFor;
    class VCallAgent;

    class VCallType2Exporter;
    class VCallType2Importer;

    class VImportableType;

    class VTask;

    /******************************
     *----  class VCallType2  ----*
     ******************************/

    class Vxa_API VCallType2 : public VCallHandle {
	DECLARE_FAMILY_MEMBERS (VCallType2, VCallHandle);

	friend class VCallAgentFor<ThisClass>;
	friend class VCallType2Importer;

	friend class VCallType2Exporter;

    //  Aliases
    public:
	typedef VCallType2Exporter Exporter;
	typedef VCallType2Importer Importer;

    //****************************************************************
    //  SelfProvider
    public:
	class Vxa_API SelfProvider : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (SelfProvider, VRolePlayer);

	//  Aliases
	public:
	    typedef IVReceiver<object_reference_t> ISelfReferenceSink;
	    typedef IVReceiver<object_reference_array_t const&> ISelfReferenceArraySink;

	//  Construction
	protected:
	    SelfProvider (ICaller *pCaller, VTask *pTask);

	//  Destruction
	protected:
	    ~SelfProvider ();

	//  Access
	protected:
	    VTaskCursor *taskCursor () const;

	//  Roles
	public:
	    using BaseClass::getRole;

	//  ISelfReferenceSink Role
	private:
	    Vca::VRole<ThisClass,ISelfReferenceSink> m_pISelfReferenceSink;
	public:
	    void getRole (ISelfReferenceSink::Reference &rpRole) {
		m_pISelfReferenceSink.getRole (rpRole);
	    }

	//  ISelfReferenceSink Methods
	public:
	    void OnData (ISelfReferenceSink *pRole, object_reference_t xSelfReference);

	//  ISelfReferenceArraySink Role
	private:
	    Vca::VRole<ThisClass,ISelfReferenceArraySink> m_pISelfReferenceArraySink;
	public:
	    void getRole (ISelfReferenceArraySink::Reference &rpRole) {
		m_pISelfReferenceArraySink.getRole (rpRole);
	    }

	//  ISelfReferenceArraySink Methods
	public:
	    void OnData (ISelfReferenceArraySink *pRole, object_reference_array_t const &rSelfReferences);

	//  IClient Methods
	protected:
	    void OnError_(Vca::IError *pInterface, VString const &rMessage);

	//  Implementation
	private:
	    virtual bool onSelf (object_reference_t xSelfReference) const = 0;
	    virtual bool onSelf (object_reference_array_t const &rSelfReferences) const = 0;

	//  State
	private:
	    VReference<VTask> const m_pTask;
	};

    //****************************************************************
    //  SelfProviderFor
    public:
	template <typename Cluster_T, typename Provider_T> class SelfProviderFor : public SelfProvider {
	    typedef SelfProviderFor<Cluster_T,Provider_T> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, SelfProvider);

	//  Aliases
	public:
	    typedef Cluster_T cluster_t;
	    typedef Provider_T provider_t;

	    typedef typename cluster_t::val_t val_t;
	    typedef typename cluster_t::var_t var_t;

	//****************************************************************
	//  CurrentSelf
	public:
	    class CurrentSelf : public VScalar<val_t> {
		DECLARE_CONCRETE_RTTLITE (CurrentSelf, VScalar<val_t>);

	    //  Construction
	    public:
		CurrentSelf (
		    cluster_t *pCluster, object_reference_array_t const &rSelfReferences, VTaskCursor *pTaskCursor
		) : BaseClass (pCluster->type ()), m_pCluster (pCluster), m_iSelfReferences (rSelfReferences), m_pTaskCursor (pTaskCursor) {
		}

	    //  Destruction
	    private:
		~CurrentSelf () {
		}

	    //  Access
	    public:
		val_t value () {
		    return m_pCluster->element(m_iSelfReferences[m_pTaskCursor->position ()]);
		}

	    //  State
	    private:
		typename cluster_t::Reference const m_pCluster;
		object_reference_array_t const m_iSelfReferences;
		VTaskCursor::Reference const m_pTaskCursor;
	    };

	//****************************************************************
	//  Construction
	public:
	    SelfProviderFor (
		ICaller *pCaller, VTask *pTask, cluster_t *pCluster, provider_t &rpSelfProvider
	    ) : BaseClass (pCaller, pTask), m_pCluster (pCluster), m_rpSelfProvider (rpSelfProvider) {
	    }

	//  Destruction
	private:
	    ~SelfProviderFor () {
	    }

	//  Implementation
	private:
	    bool onSelf (object_reference_t xSelfReference) const {
		m_rpSelfProvider.setTo (
		    new VScalarInstance<val_t, var_t>(
			m_pCluster->type (), m_pCluster->element (xSelfReference)
		    )
		);
		return true;
	    }
	    bool onSelf (object_reference_array_t const &rSelfReferences) const {
		m_rpSelfProvider.setTo (
		    new CurrentSelf (m_pCluster, rSelfReferences, taskCursor ())
		);
		return true;
	    }

	//  State
	private:
	    typename cluster_t::Reference const m_pCluster;
	    provider_t &m_rpSelfProvider;
	};

    //****************************************************************
    //  Construction
    public:
	VCallType2 (cardinality_t cParameters, cardinality_t cTask, ICaller *pTask);
	VCallType2 (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType2 ();

    //  Access
    private:
	IVUnknown *caller () const;

    //  Invocation
    public:
	virtual bool invoke (VMethod *pMethod, VCollection *pCluster) const;
	bool start (VTask *pTask) const;

    //  Parameter Acquisition
    protected:
	void returnImplementationHandle (IVSNFTaskImplementation *pHandle) const;
	bool onParameterRequest (VTask *pTask, unsigned int xParameter) const;
	bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const;
    public:
	template <typename cluster_t, typename provider_t> bool getSelfProviderFor (
	    VTask *pTask, cluster_t *pCluster, provider_t &rpSelfProvider
	) const {
	    (new SelfProviderFor<cluster_t,provider_t> (m_pCaller,pTask,pCluster,rpSelfProvider))->discard ();
	    return true;
	}

    //  Result Return
    public:
    //  ... constants:
	template <typename T> bool returnConstant (T iT) const {
	    return raiseResultTypeException (typeid(*this), typeid (T));
	}
	bool returnConstant (bool iConstant) const;
	bool returnConstant (short iConstant) const;
	bool returnConstant (unsigned short iConstant) const;
	bool returnConstant (int iConstant) const;
	bool returnConstant (unsigned int iConstant) const;
	bool returnConstant (float iConstant) const;
	bool returnConstant (double iConstant) const;
	bool returnConstant (char const *pConstant) const;
	bool returnConstant (VString const &rConstant) const;

    //  ... objects:
	template <typename T> bool returnObjects (VCollection *pCluster, T const &rT) const {
	    return raiseResultTypeException (typeid(*this), typeid (T));
	}
	bool returnObjects (VCollection *pCluster, object_reference_array_t const &rReferences) const;

    //  ... vectors:
	bool returnVector (VkDynamicArrayOf<bool> const &rVector) const;

	bool returnVector (VkDynamicArrayOf<short> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<int> const &rVector) const;

	bool returnVector (VkDynamicArrayOf<unsigned short> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<unsigned int> const &rVector) const;

	bool returnVector (VkDynamicArrayOf<float> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<double> const &rVector) const;

	bool returnVector (VkDynamicArrayOf<VString> const &rVector) const;

    //  ... status:
	bool returnError (VString const &rMessage) const OVERRIDE;
	bool returnSNF () const OVERRIDE;
	bool returnNA () const OVERRIDE;

    //  ... segments:
	template <typename T> bool returnSegment (object_reference_array_t const &rInjection, VCollection *pCluster, T const &rT) const {
	    return raiseResultTypeException (typeid(*this), typeid (T));
	}

	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<bool> const &rValues) const;

	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<char> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<short> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<int> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<__int64> const &rValues) const;

	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned char> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned short> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned int> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned __int64> const &rValues) const;

	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<float> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<double> const &rValues) const;

	bool returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<VString> const &rValues) const;
	bool returnSegment (object_reference_array_t const &rInjection, VCollection *pCluster, object_reference_array_t const &rReferences) const;

	bool returnSegment (object_reference_array_t const &rInjection) const;

	bool returnSegmentCount (cardinality_t cSegments) const;

    //  State
    private:
	ICaller::Reference const m_pCaller;
    };


    /**************************************
     *----  class VCallType2Importer  ----*
     **************************************/

    class Vxa_API VCallType2Importer : public VCallType2 {
	DECLARE_FAMILY_MEMBERS (VCallType2Importer, VCallType2);

	friend class VCallAgent;

    //  Construction
    public:
	VCallType2Importer (VCallType2 const &rCallHandle);
	VCallType2Importer (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType2Importer ();

    //  Parameter Acquisition
    private:
	VCallAgent *agent (VTask *pTask) const;
    public:
	template <typename scalar_return_t> bool getParameter (
	    VTask *pTask, VImportableType *pType, scalar_return_t &rResult
	) {
	    return raiseParameterTypeException (pTask, typeid(*this), typeid (scalar_return_t));
	}
	bool getParameter (VTask *pTask, VImportableType *pType, bool_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, short_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, unsigned_short_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, int_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, unsigned_int_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, float_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, double_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, VString_scalar_return_t &rResult);

    //  Exception Generation
    protected:
	bool raiseParameterTypeException (
	    VTask *pTask, std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
	) const;
    public:
	bool raiseUnimplementedOperationException (
	    VTask *pTask, std::type_info const &rOriginator, char const *pWhere
	) const;

    //  State
    private:
	VReference<VCallAgent> mutable m_pAgent;
    };
}


#endif

#ifndef Vxa_VCallType2_Interface
#define Vxa_VCallType2_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallHandle.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICaller2.h"

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
	    SelfProvider (VTask *pTask, ICaller *pCaller);

	//  Destruction
	protected:
	    ~SelfProvider ();

	//  Access
	public:
            VCollection*        cluster       () const;
            VCollectableObject* clusterObject (object_reference_t xObject) const;
            VClass*             clusterType   () const;
	    VTaskCursor*        taskCursor    () const;

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
	template <typename Provider_T> class SelfProviderFor : public SelfProvider {
	    DECLARE_CONCRETE_RTTLITE (SelfProviderFor<Provider_T>, SelfProvider);

	//  Aliases
	public:
	    typedef Provider_T provider_t;

	    typedef typename provider_t::ReferencedClass::value_t val_t;

	//****************************************************************
	//  CurrentSelf
	public:
	    class CurrentSelf : public VScalar<val_t> {
		DECLARE_CONCRETE_RTTLITE (CurrentSelf, VScalar<val_t>);

	    //  Construction
	    public:
		CurrentSelf (
		    SelfProvider const *pSelfProvider, object_reference_array_t const &rSelfReferences, VTaskCursor *pTaskCursor
		) : BaseClass (pSelfProvider->clusterType ()), m_pCluster (pSelfProvider->cluster ()), m_iSelfReferences (rSelfReferences), m_pTaskCursor (pTaskCursor) {
		}

	    //  Destruction
	    private:
		~CurrentSelf () {
		}

	    //  Access
	    public:
		virtual val_t value () OVERRIDE {
		    return dynamic_cast<val_t>(m_pCluster->object (m_iSelfReferences[m_pTaskCursor->position ()]));
		}

	    //  State
	    private:
                VCollection::Reference   const m_pCluster;
		object_reference_array_t const m_iSelfReferences;
		VTaskCursor::Reference   const m_pTaskCursor;
	    };

	//****************************************************************
	//  Construction
	public:
	    SelfProviderFor (
		VTask *pTask, ICaller *pCaller, provider_t &rpSelfProvider
	    ) : BaseClass (pTask, pCaller), m_rpSelfProvider (rpSelfProvider) {
	    }

	//  Destruction
	private:
	    ~SelfProviderFor () {
	    }

	//  Implementation
	private:
	    virtual bool onSelf (object_reference_t xSelfReference) const OVERRIDE {
                m_rpSelfProvider.setTo (
                    new VScalarInstance<val_t> (
                        clusterType (), dynamic_cast<val_t>(clusterObject (xSelfReference))
		    )
		);
		return true;
	    }
	    virtual bool onSelf (object_reference_array_t const &rSelfReferences) const OVERRIDE {
		m_rpSelfProvider.setTo (
		    new CurrentSelf (this, rSelfReferences, taskCursor ())
		);
                return true;
	    }

	//  State
	private:
	    provider_t &m_rpSelfProvider;
	};

    //****************************************************************
    //  Construction
    public:
        VCallType2 (
            VCollection *pCluster, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask, ICaller *pTask, bool bIntensional
        );
	VCallType2 (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType2 ();

    //  Access
    private:
	virtual IVUnknown *caller () const OVERRIDE;

    //  Invocation
    public:
	virtual bool invokeMethod (VMethod *pMethod) const OVERRIDE;
	bool start (VTask *pTask) const;

    //  Parameter Acquisition
    protected:
	void returnImplementationHandle (IVSNFTaskImplementation *pHandle) const;
	bool onParameterRequest (VTask *pTask, unsigned int xParameter) const;
	bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const;
    public:
	template <typename provider_t> bool getSelfProviderFor (
            provider_t &rpSelfProvider, VTask *pTask
	) const {
	    (new SelfProviderFor<provider_t> (pTask, m_pCaller, rpSelfProvider))->discard ();
	    return true;
	}

    //  Remote Control
    protected:
        Vxa::ICaller2 *getRemoteControlInterface () const;

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
	virtual bool returnError (VString const &rMessage) const OVERRIDE;
	virtual bool returnSNF () const OVERRIDE;
	virtual bool returnNA () const OVERRIDE;

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
	ICaller::Reference  const m_pCaller;
        ICaller2::Reference const m_pCaller2;
    };
}


#endif

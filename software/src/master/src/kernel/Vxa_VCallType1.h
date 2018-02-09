#ifndef Vxa_VCallType1_Interface
#define Vxa_VCallType1_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallHandle.h"

#include "Vxa_VCollectableCollectionOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_IVSNFTaskHolder.h"

#include "Vxa_VScalar.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <class Handle_T> class VCallAgentFor;
    class VCallAgent;

    class VCallType1Exporter;
    class VCallType1Importer;

    class VImportableType;

    class VTask;

    /******************************
     *----  class VCallType1  ----*
     ******************************/

    class Vxa_API VCallType1 : public VCallHandle {
	DECLARE_FAMILY_MEMBERS (VCallType1, VCallHandle);

	friend class VCallAgentFor<ThisClass>;

    //  Aliases
    public:
	typedef VCallType1Exporter Exporter;
	typedef VCallType1Importer Importer;

    //****************************************************************
    //  Construction
    public:
	VCallType1 (
            VString const &rMethodName, cardinality_t cParameters, cardinality_t ocTask, IVSNFTaskHolder *pTask
        );
	VCallType1 (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType1 ();

    //  Access
    private:
	virtual IVUnknown *caller () const OVERRIDE;

    //  Invocation
    public:
	virtual bool invokeMethod (VMethod *pMethod, VCollection *pCluster) const OVERRIDE;

    //  Parameter Acquisition
    protected:
	void returnImplementationHandle (IVSNFTaskImplementation *pHandle) const;
	bool onParameterRequest (VTask *pTask, unsigned int xParameter) const;
	bool onParameterReceipt (VTask *pTask, unsigned int xParameter) const;
    public:
	template <typename cluster_t, typename provider_t> bool getSelfProviderFor (
	    VTask *pTask, cluster_t *pCluster, provider_t &rpProvider
	) const {
	    rpProvider.setTo (
		new VScalarInstance<typename cluster_t::val_t, typename cluster_t::var_t>(
		    pCluster->type (), pCluster->element (0)
		)
	    );
	    return true;
	}

    //  Result Return
    public:
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

	bool returnVector (VkDynamicArrayOf<bool> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<int> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<float> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<double> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<VString> const &rVector) const;
	bool returnVector (VkDynamicArrayOf<ISingleton::Reference> const &rVector) const;

	virtual bool returnError (VString const &rMessage) const OVERRIDE;
	virtual bool returnSNF () const OVERRIDE;

    //  Task Management
    public:
	bool start (VTask *pTask) const;

    //  State
    private:
	IVSNFTaskHolder::Reference const m_pCaller;
    };


    /**************************************
     *----  class VCallType1Importer  ----*
     **************************************/

    class Vxa_API VCallType1Importer : public VCallType1 {
	DECLARE_FAMILY_MEMBERS (VCallType1Importer, VCallType1);

	friend class VCallAgent;

    //  Construction
    public:
	VCallType1Importer (VCallType1 const &rCallHandle);
	VCallType1Importer (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType1Importer ();

    //  Parameter Acquisition
    private:
	VCallAgent *agent (VTask *pTask) const;
    public:
	template <typename scalar_return_t> bool getParameter (
	    VTask *pTask, VImportableType *pType, scalar_return_t &rResult
	) {
	    return raiseParameterTypeException (pTask, typeid(*this), typeid (scalar_return_t));
	}
        bool getParameter (VTask *pTask, VImportableType *pType, any_scalar_return_t &rResult);
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

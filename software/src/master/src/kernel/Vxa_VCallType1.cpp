/*****  Vxa_VCallType1 Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vxa_VCallType1.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_IVSNFTaskHolder2.h"

#include "Vxa_VCLF.h"
#include "Vxa_VCallType1Importer.h"
#include "Vxa_VMethod.h"


/************************
 ************************
 ***                  ***
 *** Data Compression ***
 ***                  ***
 ************************
 ************************/

namespace {
    bool const g_bVxaCompression = IsntNil (getenv ("EnableVxaCompression"));
}


/*****************************
 *****************************
 *****                   *****
 *****  Vxa::VCallType1  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType1::VCallType1 (
    VCollection *pCluster, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask, IVSNFTaskHolder *pCaller
) :  BaseClass (pCluster, rMethodName, cParameters, cTask, false), m_pCaller (pCaller) {
}

Vxa::VCallType1::VCallType1 (
    ThisClass const &rOther
) : BaseClass (rOther), m_pCaller (rOther.m_pCaller) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType1::~VCallType1 () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

IVUnknown *Vxa::VCallType1::caller () const {
    return m_pCaller;
}

/************************
 ************************
 *****  Invocation  *****
 ************************
 ************************/

bool Vxa::VCallType1::invokeMethod (VMethod *pMethod) const {
    reportInvocation ();

    return pMethod->invokeCall (*this);
}


/***********************************
 ***********************************
 *****  Parameter Acquisition  *****
 ***********************************
 ***********************************/

void Vxa::VCallType1::returnImplementationHandle (IVSNFTaskImplementation *pHandle) const {
    VkDynamicArrayOf<ISingleton::Reference> iDeprecatedClientListFactoryArray(1);
    (new VCLF (cluster()))->getRole (iDeprecatedClientListFactoryArray[0]);
    m_pCaller->ReturnImplementationHandle (pHandle, iDeprecatedClientListFactoryArray);
}

bool Vxa::VCallType1::onParameterRequest (VTask *pTask, unsigned int xParameter) const {
    reportParameterRequest (xParameter);

    pTask->suspend ();
    m_pCaller->GetParameter (xParameter);
    return true;
}

bool Vxa::VCallType1::onParameterReceipt (VTask *pTask, unsigned int xParameter) const {
    reportParameterReceipt (xParameter);

    return pTask->resume ();
}

Vxa::VClass *Vxa::VCallType1::TaskObjectClass (VTask *pTask) {
    return pTask->clusterType ();
}

Vxa::VCollectableObject *Vxa::VCallType1::TaskObject (VTask *pTask, object_reference_t xObject) {
    return pTask->clusterObject (xObject);
}



/*******************************
 *******************************
 *****  Result Generation  *****
 *******************************
 *******************************/

/******************************************
 *++++  Singletons (a.k.a Constants)  ++++*
 ******************************************/

bool Vxa::VCallType1::returnConstant (bool iConstant) const {
    reportCompletion ();
    m_pCaller->ReturnInteger (iConstant ?  1 : 0);
    return true;
}

bool Vxa::VCallType1::returnConstant (short iConstant) const {
    reportCompletion ();
    m_pCaller->ReturnInteger (iConstant);
    return true;
}

bool Vxa::VCallType1::returnConstant (unsigned short iConstant) const {
    reportCompletion ();
    m_pCaller->ReturnInteger (iConstant);
    return true;
}

bool Vxa::VCallType1::returnConstant (int iConstant) const {
    reportCompletion ();
    m_pCaller->ReturnInteger (iConstant);
    return true;
}

bool Vxa::VCallType1::returnConstant (unsigned int iConstant) const {
    reportCompletion ();
    m_pCaller->ReturnInteger (iConstant);
    return true;
}

bool Vxa::VCallType1::returnConstant (float iConstant) const {
    reportCompletion ();
    return returnConstant (static_cast<double>(iConstant));
}

bool Vxa::VCallType1::returnConstant (double iConstant) const {
    reportCompletion ();
    IVSNFTaskHolder2::Pointer const pTH2 (
	g_bVxaCompression ? dynamic_cast<IVSNFTaskHolder2*>(m_pCaller.referent ()) : 0
    );
    if (pTH2)
	pTH2->ReturnDouble (iConstant);
    else {
	size_t const nElements = cardinality ();
	VkDynamicArrayOf<double> iResultArray (cardinality ());
	for (unsigned int xElement = 0; xElement < nElements; xElement++)
	    iResultArray[xElement] = iConstant;
	m_pCaller->ReturnDoubleArray (iResultArray);
    }
    return true;
}

bool Vxa::VCallType1::returnConstant (char const *pConstant) const {
    reportCompletion ();
    m_pCaller->ReturnString (pConstant);
    return true;
}

bool Vxa::VCallType1::returnConstant (VString const &rConstant) const {
    reportCompletion ();
    m_pCaller->ReturnString (rConstant);
    return true;
}


/*********************
 *++++  Vectors  ++++*
 *********************/

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<bool> const &rVector) const {
    reportCompletion ();
    IVSNFTaskHolder2::Pointer const pTH2 (
	g_bVxaCompression ? dynamic_cast<IVSNFTaskHolder2*>(m_pCaller.referent ()) : 0
    );
    if (pTH2.isNil ())
	m_pCaller->ReturnBooleanArray (rVector);
    else {
	bool_s2array_t iS2Array (rVector);
	pTH2->ReturnS2Booleans (iS2Array);
    }
    return true;
}

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<int> const &rVector) const {
    reportCompletion ();
    IVSNFTaskHolder2::Pointer const pTH2 (
	g_bVxaCompression ? dynamic_cast<IVSNFTaskHolder2*>(m_pCaller.referent ()) : 0
    );
    if (pTH2.isNil ())
	m_pCaller->ReturnIntegerArray (rVector);
    else {
	i32_s2array_t iS2Array (rVector);
	pTH2->ReturnS2Integers (iS2Array);
    }
    return true;
}

bool Vxa::VCallType1::returnError (VString const &rMessage) const {
    reportCompletion ();

    IVSNFTaskHolder1::Pointer const pITask1 (dynamic_cast<IVSNFTaskHolder1*>(m_pCaller.referent ()));
    if (pITask1)
	pITask1->ReturnError (rMessage);
    else 
	returnSNF ();
    return false; // false -> call failed
}

bool Vxa::VCallType1::returnSNF () const {
    reportCompletion ();

    m_pCaller->TurnBackSNFTask ();
    return false; // false -> call failed
}

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<float> const &rVector) const {
    reportCompletion ();
    m_pCaller->ReturnFloatArray (rVector);
    return true;
}

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<double> const &rVector) const {
    reportCompletion ();
    m_pCaller->ReturnDoubleArray (rVector);
    return true;
}

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<VString> const &rVector) const {
    reportCompletion ();
    m_pCaller->ReturnStringArray (rVector);
    return true;
}

bool Vxa::VCallType1::returnVector (VkDynamicArrayOf<ISingleton::Reference> const &rVector) const {
    reportCompletion ();
    m_pCaller->ReturnObjectArray (rVector);
    return true;
}


/*****************************
 *****************************
 *****  Task Management  *****
 *****************************
 *****************************/

bool Vxa::VCallType1::start (VTask *pTask) const {
    VCallType1Importer iImporter (*this);
    return pTask->startWithMonitorUsing (iImporter);
}


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VCallType1Importer  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType1Importer::VCallType1Importer (VCallType1 const &rHandle) : BaseClass (rHandle) {
}

Vxa::VCallType1Importer::VCallType1Importer (ThisClass const &rOther) : BaseClass (rOther) , m_pAgent (rOther.m_pAgent) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType1Importer::~VCallType1Importer () {
}


/***********************************
 ***********************************
 *****  Parameter Acquisition  *****
 ***********************************
 ***********************************/

Vxa::VCallAgent *Vxa::VCallType1Importer::agent (VTask *pTask) const {
    if (m_pAgent.isNil ()) {
	m_pAgent.setTo (new VCallAgentFor<VCallType1> (pTask, *this));
	if (g_bVxaCompression) {
	    VCallAgent::ICallType1Implementation_T::Reference pIVSNFTaskImplementation;
	    m_pAgent->getRole (pIVSNFTaskImplementation);
	    returnImplementationHandle (pIVSNFTaskImplementation);
	} else {
	    VCallAgent::ICallType1ImplementationNC_T::Reference pIVSNFTaskImplementation;
	    m_pAgent->getRole (pIVSNFTaskImplementation);
	    returnImplementationHandle (pIVSNFTaskImplementation);
	}
    }
    return m_pAgent;
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, any_scalar_return_t &rpResult) {
    return agent (pTask)->getAnyParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, bool_scalar_return_t &rpResult) {
    return agent (pTask)->getBooleanParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, short_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, unsigned_short_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, int_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, unsigned_int_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, float_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, double_scalar_return_t &rpResult) {
    return agent (pTask)->getNumericParameter (pType, rpResult);
}

bool Vxa::VCallType1Importer::getParameter (VTask *pTask, VImportableType *pType, VString_scalar_return_t &rpResult) {
    return agent (pTask)->getStringParameter (pType, rpResult);
}

Vxa::cardinality_t Vxa::VCallType1Importer::getParameterIndex (VTask *pTask) const {
    return agent (pTask)->parameterIndex ();
}

Vxa::cardinality_t Vxa::VCallType1Importer::getParameterCountRemaining (VTask *pTask) const {
    return agent (pTask)->parameterCountRemaining ();
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

bool Vxa::VCallType1Importer::raiseParameterTypeException (
    VTask *pTask, std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
) const {
    return agent (pTask)->raiseParameterTypeException (rOriginatorType, rUnexpectedType);
}

bool Vxa::VCallType1Importer::raiseUnimplementedOperationException (
    VTask *pTask, std::type_info const &rOriginator, char const *pWhere
) const {
    return agent (pTask)->raiseUnimplementedOperationException (rOriginator, pWhere);
}

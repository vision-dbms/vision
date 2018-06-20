/*****  Vxa_VCallType2 Implementation  *****/

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

#include "Vxa_VCallType2.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_ICaller.h"

#include "Vxa_VCLF.h"
#include "Vxa_VCallType2Importer.h"
#include "Vxa_VCollection.h"
#include "Vxa_VMethod.h"

#include "V_VArgList.h"


/*******************************************
 *******************************************
 *****                                 *****
 *****  Vxa::VCallType2::SelfProvider  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType2::SelfProvider::SelfProvider (
    ICaller *pCaller, VTask *pTask
) : m_pTask (pTask), m_pISelfReferenceSink (this), m_pISelfReferenceArraySink (this) {
    pTask->suspend ();
    retain (); {
	if (pTask->cardinality () == 1) {
	    ISelfReferenceSink::Reference pMySelf;
	    getRole (pMySelf);
	    pCaller->GetSelfReference (pMySelf);
	} else {
	    ISelfReferenceArraySink::Reference pMySelf;
	    getRole (pMySelf);
	    pCaller->GetSelfReferences (pMySelf);
	}
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType2::SelfProvider::~SelfProvider () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vxa::VTaskCursor *Vxa::VCallType2::SelfProvider::taskCursor () const {
    return m_pTask->cursor ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vxa::VCallType2::SelfProvider::OnData (ISelfReferenceSink *pRole, object_reference_t xSelfReference) {
    if (onSelf (xSelfReference))
	m_pTask->resume ();
    else {
	VString iMessage;
	iMessage.printf ("Bad Self Reference: %u", xSelfReference);
	OnError_(0, iMessage);
    }
}

void Vxa::VCallType2::SelfProvider::OnData (ISelfReferenceArraySink *pRole, object_reference_array_t const &rSelfReferences) {
    if (onSelf (rSelfReferences))
	m_pTask->resume ();
    else {
	VString iMessage;
	iMessage.printf ("Bad Self Reference Array");
	OnError_(0, iMessage);
    }
}

void Vxa::VCallType2::SelfProvider::OnError_(Vca::IError *pInterface, VString const &rMessage) {
    m_pTask->kill ();
    m_pTask->resume ();
}


/*****************************
 *****************************
 *****                   *****
 *****  Vxa::VCallType2  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType2::VCallType2 (
    VCollection *pCluster, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask, ICaller *pCaller, bool bIntensional
) :  BaseClass (pCluster, rMethodName, cParameters, cTask, bIntensional), m_pCaller (pCaller) {
}

Vxa::VCallType2::VCallType2 (
    ThisClass const &rOther
) : BaseClass (rOther), m_pCaller (rOther.m_pCaller) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType2::~VCallType2 () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

IVUnknown *Vxa::VCallType2::caller () const {
    return m_pCaller;
}

/************************
 ************************
 *****  Invocation  *****
 ************************
 ************************/

bool Vxa::VCallType2::invokeMethod (VMethod *pMethod, VCollection *pCluster) const {
    return pMethod->invoke (*this, pCluster);
}

bool Vxa::VCallType2::start (VTask *pTask) const {
    VCallType2Importer iImporter (*this);
    return pTask->startUsing (iImporter);
}


/***********************************
 ***********************************
 *****  Parameter Acquisition  *****
 ***********************************
 ***********************************/

void Vxa::VCallType2::returnImplementationHandle (IVSNFTaskImplementation *pHandle) const {
    VkDynamicArrayOf<ISingleton::Reference> iDeprecatedClientListFactoryArray(1);
    (new VCLF (cluster()))->getRole (iDeprecatedClientListFactoryArray[0]);
    m_pCaller->ReturnImplementationHandle (pHandle, iDeprecatedClientListFactoryArray);
}

bool Vxa::VCallType2::onParameterRequest (VTask *pTask, unsigned int xParameter) const {
    reportParameterRequest (xParameter);

    pTask->suspend ();
    m_pCaller->GetParameter (xParameter);
    return true;
}

bool Vxa::VCallType2::onParameterReceipt (VTask *pTask, unsigned int xParameter) const {
    reportParameterReceipt (xParameter);

    return pTask->resume ();
}


/***************************
 ***************************
 *****  Result Return  *****
 ***************************
 ***************************/

/******************************************
 *++++  Singletons (a.k.a Constants)  ++++*
 ******************************************/

bool Vxa::VCallType2::returnConstant (bool iConstant) const {
    m_pCaller->ReturnInteger (iConstant ?  1 : 0);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (short iConstant) const {
    m_pCaller->ReturnInteger (iConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (unsigned short iConstant) const {
    m_pCaller->ReturnInteger (iConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (int iConstant) const {
    m_pCaller->ReturnInteger (iConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (unsigned int iConstant) const {
    m_pCaller->ReturnInteger (iConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (float iConstant) const {
    return returnConstant (static_cast<double>(iConstant));
}

bool Vxa::VCallType2::returnConstant (double iConstant) const {
    m_pCaller->ReturnDouble (iConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (char const *pConstant) const {
    m_pCaller->ReturnString (pConstant);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnConstant (VString const &rConstant) const {
    m_pCaller->ReturnString (rConstant);
    reportCompletion ();
    return true;
}


/*********************
 *++++  Objects  ++++*
 *********************/

bool Vxa::VCallType2:: returnObjects (
    VCollection *pCluster, VkDynamicArrayOf<unsigned int> const &rReferences
) const {
    ICollection::Reference pClusterInterface;
    pCluster->getRole (pClusterInterface);
    if (cardinality () == 1) {
	m_pCaller->ReturnObjectReference (pClusterInterface, pCluster->cardinality (), rReferences[0]);
    } else {
	m_pCaller->ReturnObjectReferences (pClusterInterface, pCluster->cardinality (), rReferences);
    }
    reportCompletion ();
    return true;
}


/*********************
 *++++  Vectors  ++++*
 *********************/

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<bool> const &rVector) const {
    m_pCaller->ReturnBooleanArray (rVector);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<short> const &rVector) const {
    VkDynamicArrayOf<int> iTransmissible (rVector.cardinality ());
    for (unsigned int xElement = 0; xElement < rVector.cardinality (); xElement++)
	iTransmissible[xElement] = rVector[xElement];
    return returnVector (iTransmissible);
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<int> const &rVector) const {
    m_pCaller->ReturnIntegerArray (rVector);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<unsigned short> const &rVector) const {
    VkDynamicArrayOf<int> iTransmissible (rVector.cardinality ());
    for (unsigned int xElement = 0; xElement < rVector.cardinality (); xElement++)
	iTransmissible[xElement] = rVector[xElement];
    return returnVector (iTransmissible);
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<unsigned int> const &rVector) const {
    VkDynamicArrayOf<int> iTransmissible (rVector.cardinality ());
    for (unsigned int xElement = 0; xElement < rVector.cardinality (); xElement++)
	iTransmissible[xElement] = rVector[xElement];
    return returnVector (iTransmissible);
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<float> const &rVector) const {
    m_pCaller->ReturnFloatArray (rVector);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<double> const &rVector) const {
    m_pCaller->ReturnDoubleArray (rVector);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnVector (VkDynamicArrayOf<VString> const &rVector) const {
    m_pCaller->ReturnStringArray (rVector);
    reportCompletion ();
    return true;
}


/********************
 *++++  Status  ++++*
 ********************/

bool Vxa::VCallType2::returnError (VString const &rMessage) const {
    m_pCaller->ReturnError (rMessage);
    reportCompletion ();
    return false; // false -> call failed
}

bool Vxa::VCallType2::returnSNF () const {
    m_pCaller->TurnBackSNFTask ();
    reportCompletion ();
    return false; // false -> call failed
}

bool Vxa::VCallType2::returnNA () const {
    m_pCaller->ReturnNA ();
    reportCompletion ();
    return true;
}


/**********************
 *++++  Segments  ++++*
 **********************/

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<bool> const &rValues) const {
    m_pCaller->ReturnBooleanSegment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<char> const &rValues) const{
    m_pCaller->ReturnS08Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<short> const &rValues) const{
    m_pCaller->ReturnS16Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<int> const &rValues) const{
    m_pCaller->ReturnS32Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<__int64> const &rValues) const{
    m_pCaller->ReturnS64Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned char> const &rValues) const{
    m_pCaller->ReturnU08Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned short> const &rValues) const{
    m_pCaller->ReturnU16Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned int> const &rValues) const{
    m_pCaller->ReturnU32Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<unsigned __int64> const &rValues) const{
    m_pCaller->ReturnU64Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<float> const &rValues) const{
    m_pCaller->ReturnF32Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<double> const &rValues) const{
    m_pCaller->ReturnF64Segment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection, VkDynamicArrayOf<VString> const &rValues) const{
    m_pCaller->ReturnStringSegment (rInjection, rValues);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (
    object_reference_array_t const &rInjection, VCollection *pCluster, object_reference_array_t const &rReferences
) const {
    ICollection::Reference pClusterInterface;
    pCluster->getRole (pClusterInterface);
    m_pCaller->ReturnObjectSegment (rInjection, pClusterInterface, pCluster->cardinality (), rReferences);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegment (object_reference_array_t const &rInjection) const {
    m_pCaller->ReturnNASegment (rInjection);
    reportCompletion ();
    return true;
}

bool Vxa::VCallType2::returnSegmentCount (cardinality_t cSegments) const {
    m_pCaller->ReturnSegmentCount (cSegments);
    reportCompletion ();
    return true;
}


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VCallType2Importer  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType2Importer::VCallType2Importer (VCallType2 const &rHandle) : BaseClass (rHandle) {
}

Vxa::VCallType2Importer::VCallType2Importer (ThisClass const &rOther) : BaseClass (rOther), m_pAgent (rOther.m_pAgent) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType2Importer::~VCallType2Importer () {
}


/***********************************
 ***********************************
 *****  Parameter Acquisition  *****
 ***********************************
 ***********************************/

Vxa::VCallAgent *Vxa::VCallType2Importer::agent (VTask *pTask) const {
    if (m_pAgent.isNil ()) {
	m_pAgent.setTo (new VCallAgentFor<VCallType2> (pTask, *this));

	VCallAgent::ICallType2Implementation_T::Reference pIVSNFTaskImplementation;
	m_pAgent->getRole (pIVSNFTaskImplementation);
	returnImplementationHandle (pIVSNFTaskImplementation);
    }
    return m_pAgent;
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, any_scalar_return_t &rResult) {
    return agent (pTask)->getAnyParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, bool_scalar_return_t &rResult) {
    return agent (pTask)->getBooleanParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, short_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, unsigned_short_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, int_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, unsigned_int_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, float_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, double_scalar_return_t &rResult) {
    return agent (pTask)->getNumericParameter (pType, rResult);
}

bool Vxa::VCallType2Importer::getParameter (VTask *pTask, VImportableType *pType, VString_scalar_return_t &rResult) {
    return agent (pTask)->getStringParameter (pType, rResult);
}

Vxa::cardinality_t Vxa::VCallType2Importer::getParameterIndex (VTask *pTask) const {
    return agent (pTask)->parameterIndex ();
}

Vxa::cardinality_t Vxa::VCallType2Importer::getParameterCountRemaining (VTask *pTask) const {
    return agent (pTask)->parameterCountRemaining ();
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

bool Vxa::VCallType2Importer::raiseParameterTypeException (
    VTask *pTask, std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
) const {
    return agent (pTask)->raiseParameterTypeException (rOriginatorType, rUnexpectedType);
}

bool Vxa::VCallType2Importer::raiseUnimplementedOperationException (
    VTask *pTask, std::type_info const &rOriginator, char const *pWhere
) const {
    return agent (pTask)->raiseUnimplementedOperationException (rOriginator, pWhere);
}

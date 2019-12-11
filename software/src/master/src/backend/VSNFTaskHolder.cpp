/*****  VSNFTaskHolder Implementation  *****/

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

#include "VSNFTaskHolder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VTrigger.h"

#include "VExternalGroundStore.h"
#include "VPrimitiveTask.h"
#include "RTblock.h"

#include "Vxa_ICollection.h"
#include "Vxa_IVSNFTaskImplementation3.h"
#include "Vxa_IVSNFTaskImplementation3NC.h"

#include "V_VLogger.h"
#include "VTransientServices.h"



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

/********************
 ********************
 *****          *****
 *****  Logger  *****
 *****          *****
 ********************
 ********************/

namespace {
    V::VLogger g_iLogger (getenv ("VxaCalloutLog"));
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  VSNFTaskHolder::Scheduler::Entry  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSNFTaskHolder::Scheduler::Entry::Entry (VSNFTaskHolder *pTaskHolder) : m_pTaskHolder (pTaskHolder) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSNFTaskHolder::Scheduler::Entry::~Entry () {
}


/***************************************
 ***************************************
 *****                             *****
 *****  VSNFTaskHolder::Scheduler  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSNFTaskHolder::Scheduler::Scheduler (unsigned int sConcurrentTaskLimit) : m_cRunningTasks (0), m_sConcurrentTaskLimit (sConcurrentTaskLimit) {
}

/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

void VSNFTaskHolder::Scheduler::insertTail (Entry *pEntry) {
    if (m_pQueueTail)
	m_pQueueTail->setSuccessorTo (pEntry);
    else
	m_pQueueHead.setTo (pEntry);
    m_pQueueTail.setTo (pEntry);
}

bool VSNFTaskHolder::Scheduler::removeHead (Entry::Reference &rpEntry) {
    if (m_pQueueHead) {
	rpEntry.setTo (m_pQueueHead);
	m_pQueueHead.claim (rpEntry->m_pSuccessor);
	if (m_pQueueHead.isEmpty ())
	    m_pQueueTail.clear ();
	return true;
    }

    rpEntry.clear ();
    return false;
}

/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void VSNFTaskHolder::Scheduler::startNextTask () {
    Entry::Reference pNextTask;
    if (removeHead (pNextTask))
	pNextTask->start ();
}


/****************************
 ****************************
 *****                  *****
 *****  VSNFTaskHolder  *****
 *****                  *****
 ****************************
 ****************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSNFTaskHolder);

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

VSNFTaskHolder::Scheduler VSNFTaskHolder::g_iScheduler (V::GetEnvironmentUnsigned ("VxaClientConcurrency", UINT_MAX));

unsigned int VSNFTaskHolder::g_iAdapterType (V::GetEnvironmentUnsigned ("VisionAdapterType", 0)); 
                                            // 0: Bridge adapter type
                                            // 1: SPlus adapter type

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSNFTaskHolder::~VSNFTaskHolder () {
}


/*********************
 *********************
 *****  Helpers  *****
 *********************
 *********************/

/************************************************************
 *----  VSNFTaskHolder::ReturnArray<Source_T,Result_T>  ----*
 ************************************************************/

template <typename Source_T, typename Result_T> void VSNFTaskHolder::ReturnArray (
    VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Result", rSourceArray);
	m_pSNFTask->ReturnArray (rSourceArray, rpResultArray);
	wrapup ();
    }
}
template void VSNFTaskHolder::ReturnArray<bool,int>          (VkDynamicArrayOf<bool>const&,int const*&);

template void VSNFTaskHolder::ReturnArray<char,int>          (VkDynamicArrayOf<char>const&,int const*&);
template void VSNFTaskHolder::ReturnArray<short,int>         (VkDynamicArrayOf<short>const&,int const*&);
template void VSNFTaskHolder::ReturnArray<int,int>           (VkDynamicArrayOf<int>const&,int const*&);

template void VSNFTaskHolder::ReturnArray<unsigned char,int> (VkDynamicArrayOf<unsigned char>const&,int const*&);
template void VSNFTaskHolder::ReturnArray<unsigned short,int>(VkDynamicArrayOf<unsigned short>const&,int const*&);
template void VSNFTaskHolder::ReturnArray<unsigned int,int>  (VkDynamicArrayOf<unsigned int>const&,int const*&);

template void VSNFTaskHolder::ReturnArray<double,double>     (VkDynamicArrayOf<double>const&,double const*&);
template void VSNFTaskHolder::ReturnArray<float,float>       (VkDynamicArrayOf<float>const&,float const*&);

template void VSNFTaskHolder::ReturnArray<V::VString,V::VString>   (VkDynamicArrayOf<VString>const&,VString const*&);
template void VSNFTaskHolder::ReturnArray<Vxa::ISingleton::Reference,Vxa::ISingleton::Reference> (
    VkDynamicArrayOf<Vxa::ISingleton::Reference>const&,Vxa::ISingleton::Reference const*&
);

/***************************************************
 *----  VSNFTaskHolder::ReturnArray<Source_T>  ----*
 ***************************************************/

template <typename Source_T> void VSNFTaskHolder::ReturnArray (VkDynamicArrayOf<Source_T> const &rSourceArray) {
    Source_T const *pResultArray;
    ReturnArray (rSourceArray, pResultArray);
}
template void VSNFTaskHolder::ReturnArray<double>      (VkDynamicArrayOf<double>const&);
template void VSNFTaskHolder::ReturnArray<float>       (VkDynamicArrayOf<float>const&);
template void VSNFTaskHolder::ReturnArray<int>         (VkDynamicArrayOf<int>const&);
template void VSNFTaskHolder::ReturnArray<V::VString>  (VkDynamicArrayOf<VString>const&);
template void VSNFTaskHolder::ReturnArray<Vxa::ISingleton::Reference> (VkDynamicArrayOf<Vxa::ISingleton::Reference>const&);


/**************************************************************
 *----  VSNFTaskHolder::ReturnSegment<Source_T,Result_T>  ----*
 **************************************************************/

template <typename Source_T, typename Result_T> void VSNFTaskHolder::ReturnSegment (
    object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Result Segment", rSourceArray);
	if (m_pSNFTask->ReturnSegment (rInjector, rSourceArray, rpResultArray))
	    wrapup ();
    }
}
template void VSNFTaskHolder::ReturnSegment<bool,int>          (object_reference_array_t const&,VkDynamicArrayOf<bool>const&,int const*&);

template void VSNFTaskHolder::ReturnSegment<char,int>          (object_reference_array_t const&,VkDynamicArrayOf<char>const&,int const*&);
template void VSNFTaskHolder::ReturnSegment<short,int>         (object_reference_array_t const&,VkDynamicArrayOf<short>const&,int const*&);
template void VSNFTaskHolder::ReturnSegment<int,int>           (object_reference_array_t const&,VkDynamicArrayOf<int>const&,int const*&);

template void VSNFTaskHolder::ReturnSegment<unsigned char,int> (object_reference_array_t const&,VkDynamicArrayOf<unsigned char>const&,int const*&);
template void VSNFTaskHolder::ReturnSegment<unsigned short,int>(object_reference_array_t const&,VkDynamicArrayOf<unsigned short>const&,int const*&);
template void VSNFTaskHolder::ReturnSegment<unsigned int,int>  (object_reference_array_t const&,VkDynamicArrayOf<unsigned int>const&,int const*&);

template void VSNFTaskHolder::ReturnSegment<double,double>     (object_reference_array_t const&,VkDynamicArrayOf<double>const&,double const*&);
template void VSNFTaskHolder::ReturnSegment<float,float>       (object_reference_array_t const&,VkDynamicArrayOf<float>const&,float const*&);

template void VSNFTaskHolder::ReturnSegment<V::VString,V::VString> (object_reference_array_t const&,VkDynamicArrayOf<VString>const&,VString const*&);

/*****************************************************
 *----  VSNFTaskHolder::ReturnSegment<Source_T>  ----*
 *****************************************************/

template <typename Source_T> void VSNFTaskHolder::ReturnSegment (
    object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray
) {
    Source_T const *pResultArray;
    ReturnSegment (rInjector, rSourceArray, pResultArray);
}
template void VSNFTaskHolder::ReturnSegment<double>  (object_reference_array_t const&,VkDynamicArrayOf<double>const&);
template void VSNFTaskHolder::ReturnSegment<float>   (object_reference_array_t const&,VkDynamicArrayOf<float>const&);
template void VSNFTaskHolder::ReturnSegment<int>     (object_reference_array_t const&,VkDynamicArrayOf<int>const&);
template void VSNFTaskHolder::ReturnSegment<V::VString> (object_reference_array_t const&,VkDynamicArrayOf<VString>const&);


/*******************************************************
 *----  VSNFTaskHolder::ReturnSingleton<Source_T>  ----*
 *******************************************************/

template <typename Source_T> void VSNFTaskHolder::ReturnSingleton (Source_T iSingleton) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Coerced Result", iSingleton);
	m_pSNFTask->ReturnSingleton (iSingleton);
	wrapup ();
    }
}
template void VSNFTaskHolder::ReturnSingleton<double>      (double iSingleton);
template void VSNFTaskHolder::ReturnSingleton<float>       (float);
template void VSNFTaskHolder::ReturnSingleton<int>         (int);
template void VSNFTaskHolder::ReturnSingleton<char const*> (char const*);


/***************************************
 ***************************************
 *****  Interface Implementations  *****
 ***************************************
 ***************************************/

/*****************************
 *****  IVSNFTaskHolder  *****
 *****************************/

void VSNFTaskHolder::GetParameter (IVSNFTaskHolder *pRole, unsigned int xParameter) {
    if (m_pSNFTask) {
//  Cache the task's external implementation, ...
	IVSNFTaskImplementation *const pImplementation = m_pSNFTask->implementation ();

//  ... access the parameter, ...
	VDescriptor rVDatum;
	m_pSNFTask->getParameter (xParameter, rVDatum);

//  ... some fragmentation type parameters can be converted to monotype, 
//      and for them we don't want to issue "Unclustered Parameter Not Supported" msg ...
	if (rVDatum.isAFragmentation ()) rVDatum.convertToVector ();

	if (rVDatum.convertVectorsToMonotypeIfPossible ()) {
//  ... and return it ...
	    rVDatum.normalize ();
	    DSC_Descriptor& rDatum = rVDatum.contentAsMonotype ();

	    Vdd::Store *store = rDatum.store ();
	    RTYPE_Type const rtype = rDatum.pointerRType ();
	    RTYPE_Type const xStoreRType = rDatum.storeRType ();
	    unsigned int const elementCount = taskCardinality ();

	    rtBLOCK_Handle::Strings pBlockStrings;
	    rtLSTORE_Handle::Strings pLStoreStrings;

	    VString sWhere;
	    if (g_iLogger.isActive ())
		sWhere.printf ("Parameter %u", xParameter);

	    if (rtype == RTYPE_C_IntUV && store->NamesTheIntegerClass ()) {
    // ... as an integer, ...
		int const *const pParameterArray = rDatum;
		VkDynamicArrayOf<int> dynamicArray(elementCount);
		for (unsigned int xElement = 0; xElement < elementCount; xElement++)
		    dynamicArray[xElement] = pParameterArray[xElement];

		if (g_iLogger.isActive ())
		    g_iLogger.log (taskId (), sWhere, dynamicArray);

		Vxa::IVSNFTaskImplementation2::Pointer const pTI2 (
		    g_bVxaCompression ? dynamic_cast<Vxa::IVSNFTaskImplementation2*>(pImplementation) : 0
		);
		if (pTI2.isNil ())
		    pImplementation->SetToInteger (xParameter, dynamicArray);
		else {
		    Vxa::i32_s2array_t iS2Array (dynamicArray);
		    pTI2->SetToS2Integers (xParameter, iS2Array);
		}
	    } else if (rtype == RTYPE_C_DoubleUV && store->NamesTheDoubleClass ()) {
    // ... double, ...
		double const *const pParameterArray = rDatum;
		VkDynamicArrayOf<Vca::F64> dynamicArray(elementCount);
		for (unsigned int xElement = 0; xElement < elementCount; xElement++)
		    dynamicArray[xElement] = pParameterArray[xElement];

		if (g_iLogger.isActive ())
		    g_iLogger.log (taskId (), sWhere, dynamicArray);

		pImplementation->SetToDouble (xParameter, dynamicArray);
	    } else if (rtype == RTYPE_C_FloatUV && store->NamesTheFloatClass ()) {
    // ... float cast to double, ...
		float const *const pParameterArray = rDatum;
		VkDynamicArrayOf<Vca::F64> dynamicArray(elementCount);
		for (unsigned int xElement = 0; xElement < elementCount; xElement++)
		    dynamicArray[xElement] = pParameterArray[xElement];

		if (g_iLogger.isActive ())
		    g_iLogger.log (taskId (), sWhere, dynamicArray);

		pImplementation->SetToDouble (xParameter, dynamicArray);
    // ... string, ...
	    } else if (pBlockStrings.setTo (store) || pLStoreStrings.setTo (store)) {
		if (m_pSNFTask->isScalar ()) { /*****  Scalar Case  *****/
		    VkDynamicArrayOf<VString> dynamicArray(1);
		    dynamicArray[0].append (
			pBlockStrings.isSet ()
			? pBlockStrings[DSC_Descriptor_Scalar_Int (rDatum)]
			: pLStoreStrings[DSC_Descriptor_Scalar_Int (rDatum)]
		    );
		    pImplementation->SetToVString (xParameter, dynamicArray );
		} else { /*****  Non-scalar case  *****/
		    //	size_t sValue = DSC_Descriptor_Scalar_Int (rDatum);
		    unsigned int arraySize = 0;
		    unsigned int arrayIndex = 0;

		    #define CalcLStoreTotalSize(value)	arraySize ++
                    #define CalcBlockTotalSize(value)	arraySize ++
                    #define LStoreFill(value) {\
			dynamicArray[arrayIndex].append(pLStoreStrings[value]);\
                        arrayIndex++;\
                    }
                    #define BlockFill(value) {\
                        dynamicArray[arrayIndex].append(pBlockStrings[value]);\
                        arrayIndex++;\
                    }

		    RTYPE_Type const rtype = rDatum.pointerRType ();
		    bool const sourceIsBlockString = (rtype == RTYPE_C_IntUV);

		    VkDynamicArrayOf<VString> dynamicArray;

		    
		    if (!sourceIsBlockString) { //  ... LStore strings
		    //  Calculate the size of dynamic array ... 
			DSC_Traverse (rDatum, CalcLStoreTotalSize);

		    //  ... preallocate it, ...
			dynamicArray.Append (arraySize);

		    //  ... and set its content:
			DSC_Traverse (rDatum, LStoreFill);
		    } else { //  ... Block strings
		    //  Calculate the size of dynamic array ...
			DSC_Traverse (rDatum, CalcBlockTotalSize);

		    //  ... pre-allocate it, ...
			dynamicArray.Append (arraySize);

		    //  ... and set its content:
			DSC_Traverse (rDatum, BlockFill);
		    }

                    #undef CalcLStoreTotalSize
                    #undef CalcBlockTotalSize
                    #undef LStoreFill
                    #undef BlockFill

		    if (g_iLogger.isActive ())
			g_iLogger.log (taskId (), sWhere, dynamicArray);

		    pImplementation->SetToVString (xParameter, dynamicArray );
		}
	    } else {
		if (g_iAdapterType == 0) { // Bridge adapter
		    VString iError;
		    // count parameter as 1 based
		    iError.printf ("Parameter %u: Parameters such as List and External Object Not Supported for Bridge Adapter", xParameter + 1); 
		    signalParameterError (pRole, pImplementation, xParameter, iError);
		}
		else if (g_iAdapterType == 1) { // SPlus adapter
    // ... deprecated object array, ...
		    if(rDatum.storeTransientExtensionIsA(VExternalGroundStore::RTT)) {
			VkDynamicArrayOf<ISingleton::Reference> dynamicArray (1);
			dynamicArray[0].setTo (static_cast<VExternalGroundStore*> (
			    rDatum.storeTransientExtensionIfA (VExternalGroundStore::RTT))->getInterface ()
			);
			pImplementation->SetToObjects (xParameter, dynamicArray);
    // ... or deprecated variant, ...
		    } else {
			m_pSNFTask->createAndCopyToVariant (xParameter);
		    }
		} else { // other adapter
		    VString iError ("Adapter Type other than Bridge and SPlus Not Supported"); 
		    signalParameterError (pRole, pImplementation, xParameter, iError);
		}
	    }
	} else {
    // ... or return an error because it's not a monotype:
	    VString iError;
	    iError.printf ("Parameter %u: Unclustered Parameter Not Supported", xParameter + 1); // 1 based
	    signalParameterError (pRole, pImplementation, xParameter, iError);
	}
    }
}

void VSNFTaskHolder::ReturnImplementationHandle (
    IVSNFTaskHolder *					pRole, 
    IVSNFTaskImplementation *				pImplementation, 
    VkDynamicArrayOf<ISingleton::Reference> const &	rRemoteFactoryArray
) {
    if (m_pSNFTask)
	m_pSNFTask->returnImplementationHandle (pImplementation, rRemoteFactoryArray);
}

void VSNFTaskHolder::ReturnBooleanArray (IVSNFTaskHolder *pRole, VkDynamicArrayOf<bool> const & rArray) {
    int const *pResultArray;
    ReturnArray (rArray, pResultArray);
}

void VSNFTaskHolder::ReturnIntegerArray (IVSNFTaskHolder *pRole, VkDynamicArrayOf<int> const & rArray) {
    ReturnArray (rArray);
}

void VSNFTaskHolder::ReturnFloatArray (IVSNFTaskHolder *pRole, VkDynamicArrayOf<Vca::F32> const & rArray) {
    ReturnArray (rArray);
}

void VSNFTaskHolder::ReturnDoubleArray (IVSNFTaskHolder *pRole, VkDynamicArrayOf<Vca::F64> const & rArray) {
    ReturnArray (rArray);
}

void VSNFTaskHolder::ReturnStringArray (IVSNFTaskHolder *pRole, VkDynamicArrayOf<VString> const & rArray) {
    ReturnArray (rArray);
}

void VSNFTaskHolder::ReturnObjectArray (
    IVSNFTaskHolder *pRole, VkDynamicArrayOf<ISingleton::Reference> const &rArray
) {
    ReturnArray (rArray);
}

void VSNFTaskHolder::ReturnInteger (IVSNFTaskHolder *pRole, int result) {
    ReturnSingleton (result);
}

void VSNFTaskHolder::ReturnString (IVSNFTaskHolder *pRole, char const * pName) {
    ReturnSingleton (pName);
}

void VSNFTaskHolder::SetOutput (IVSNFTaskHolder *pRole, VkDynamicArrayOf<VString> const & rArray) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Output", rArray);
	m_pSNFTask->SetOutput (rArray);
    }
}

void VSNFTaskHolder::TurnBackSNFTask (IVSNFTaskHolder *pRole) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "SNF", ">>> SELECTOR NOT FOUND <<<");
	m_pSNFTask->TurnBackSNFTask ();
	wrapup ();
    }
}


/******************************
 *****  IVSNFTaskHolder1  *****
 ******************************/

void VSNFTaskHolder::ReturnError(Vxa::IVSNFTaskHolder1 *pRole, VString const &rMessage) {
    onError (0, rMessage);
}

/******************************
 *****  IVSNFTaskHolder2  *****
 ******************************/

void VSNFTaskHolder::ReturnDouble (IVSNFTaskHolder2 *pRole, double result) {
    ReturnSingleton (result);
}

void VSNFTaskHolder::ReturnS2Booleans (IVSNFTaskHolder2 *pRole, Vxa::bool_s2array_t const &rResult) {
    ReturnBooleanArray (pRole, rResult);
}

void VSNFTaskHolder::ReturnS2Integers (IVSNFTaskHolder2 *pRole, Vxa::i32_s2array_t const &rResult) {
    ReturnIntegerArray (pRole, rResult);
}

/*********************
 *****  ICaller  *****
 *********************/

void VSNFTaskHolder::getRole (ICaller::Reference &rpRole) {
    ICaller2::Reference pRoleX;
    getRole (pRoleX);
    rpRole.setTo (pRoleX);
}

void VSNFTaskHolder::GetSelfReference (ICaller *pRole, IVReceiver<object_reference_t> *pResultSink) {
    if (pResultSink && m_pSNFTask) {
	object_reference_t xSelf;
	if (m_pSNFTask->getSelfReference (xSelf))
	    pResultSink->OnData (xSelf);
	else {
	    pResultSink->OnError (0, "GetSelfReference Not Implemented");
	}
    }
}

void VSNFTaskHolder::GetSelfReferences (ICaller *pRole, IVReceiver<object_reference_array_t const&> *pResultSink) {
    if (pResultSink && m_pSNFTask) {
	object_reference_array_t xSelf;
	if (m_pSNFTask->getSelfReference (xSelf))
	    pResultSink->OnData (xSelf);
	else {
	    pResultSink->OnError (0, "GetSelfReferences Not Implemented");
	}
    }
}

void VSNFTaskHolder::ReturnObjectReference (ICaller *pRole, ICollection *pCluster, object_reference_t sCluster, object_reference_t xObject) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Object Reference", xObject);
	m_pSNFTask->ReturnObject (pCluster, sCluster, xObject);
	wrapup ();
    }
}

void VSNFTaskHolder::ReturnObjectReferences (ICaller *pRole, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Object References", rxObjects);
	m_pSNFTask->ReturnObjects (pCluster, sCluster, rxObjects);
	wrapup ();
    }
}

void VSNFTaskHolder::ReturnS08Array (ICaller *pRole, VkDynamicArrayOf<char> const &rData) {
    int const *pResult;
    ReturnArray (rData, pResult);
}

void VSNFTaskHolder::ReturnU08Array (ICaller *pRole, VkDynamicArrayOf<unsigned char> const &rData) {
    int const *pResult;
    ReturnArray (rData, pResult);
}

void VSNFTaskHolder::ReturnS16Array (ICaller *pRole, VkDynamicArrayOf<short> const &rData) {
    int const *pResult;
    ReturnArray (rData, pResult);
}

void VSNFTaskHolder::ReturnU16Array (ICaller *pRole, VkDynamicArrayOf<unsigned short> const &rData) {
    int const *pResult;
    ReturnArray (rData, pResult);
}

void VSNFTaskHolder::ReturnS32Array (ICaller *pRole, VkDynamicArrayOf<int> const &rData) {
    ReturnArray (rData);
}

void VSNFTaskHolder::ReturnU32Array (ICaller *pRole, VkDynamicArrayOf<unsigned int> const &rData) {
    int const *pResult;
    ReturnArray (rData, pResult);
}

void VSNFTaskHolder::ReturnS64Array (ICaller *pRole, VkDynamicArrayOf<__int64> const &rData) {
    ReturnError (pRole, "64-bit integers not implemented in this version.");
}

void VSNFTaskHolder::ReturnU64Array (ICaller *pRole, VkDynamicArrayOf<unsigned __int64> const &rData) {
    ReturnError (pRole, "64-bit unsigned integers not implemented in this version.");
}

void VSNFTaskHolder::ReturnNA (ICaller *pRole) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "NA", "Singleton");
	m_pSNFTask->ReturnNA ();
	wrapup ();
    }
}

void VSNFTaskHolder::ReturnNASegment (ICaller *pRole, object_reference_array_t const &rInjector) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "NA", "Segment");
	if (m_pSNFTask->ReturnNA (rInjector))
	    wrapup ();
    }
}

void VSNFTaskHolder::ReturnBooleanSegment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<bool> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnS08Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<char> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnU08Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned char> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnS16Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<short> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnU16Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned short> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnS32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<int> const &rData) {
    ReturnSegment (rInjector, rData);
}

void VSNFTaskHolder::ReturnU32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned int> const &rData) {
    int const *pResult;
    ReturnSegment (rInjector, rData, pResult);
}

void VSNFTaskHolder::ReturnS64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<__int64> const &rData) {
    ReturnError (pRole, "64-bit integers not implemented in this version.");
}

void VSNFTaskHolder::ReturnU64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<unsigned __int64> const &rData) {
    ReturnError (pRole, "64-bit unsigned integers not implemented in this version.");
}

void VSNFTaskHolder::ReturnF32Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<float> const &rData) {
    ReturnSegment (rInjector, rData);
}

void VSNFTaskHolder::ReturnF64Segment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<double> const &rData) {
    ReturnSegment (rInjector, rData);
}

void VSNFTaskHolder::ReturnStringSegment (ICaller *pRole, object_reference_array_t const &rInjector, VkDynamicArrayOf<VString> const &rData) {
    ReturnSegment (rInjector, rData);
}

void VSNFTaskHolder::ReturnObjectSegment (
    ICaller *pRole, object_reference_array_t const &rInjector, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rData
) {
    if (m_pSNFTask) {
 	if (g_iLogger.isActive ())
 	    g_iLogger.log (taskId (), "Object Segment", rData);
	if (m_pSNFTask->ReturnObjects (rInjector, pCluster, sCluster, rData))
	    wrapup ();
    }
}

void VSNFTaskHolder::ReturnSegmentCount (ICaller *pRole, cardinality_t cSegments) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Segment Count", cSegments);
	if (m_pSNFTask->SetSegmentCountTo (cSegments))
	    wrapup ();
    }
}

/**********************
 *****  ICaller2  *****
 **********************/

void VSNFTaskHolder::Suspensions (ICaller2 *pRole, IVReceiver<cardinality_t> *pResultSink) {
    if (pResultSink) {
        if (m_pSNFTask)
	    pResultSink->OnData (m_pSNFTask->suspendCount ());
	else {
	    pResultSink->OnError (0, "Task Inactive");
	}
    }
}

void VSNFTaskHolder::Suspend (ICaller2 *pRole) {
    if (m_pSNFTask)
        m_pSNFTask->suspend ();
}

void VSNFTaskHolder::Resume (ICaller2 *pRole) {
    if (m_pSNFTask)
        m_pSNFTask->resume ();
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void VSNFTaskHolder::getDescription_(VString& rResult) const {
    VCall const* const pCall (m_pSNFTask ? m_pSNFTask->call () : 0);
    if (pCall)
	pCall->callerDecompilation (rResult);
    else
	rResult << "\n#### UNKNOWN CALLER ####\n";
}


/************************************
 ************************************
 *****  Monitoring and Control  *****
 ************************************
 ************************************/

void VSNFTaskHolder::start (ISingleton *pExternalObject) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ()) {
	    g_iLogger.printf (
		"+++%5u: %p->%s [%u x %u]\n", taskId (), pExternalObject,
		m_pSNFTask->selectorName (), m_pSNFTask->cardinality (), m_pSNFTask->parameterCount ()
	    );
	}
	try {
	    g_iScheduler.incrementTaskCount ();
	    if (g_bVxaCompression) {
	      IVSNFTaskHolder_T::Reference pIVSNFTaskHolder;
	      getRole (pIVSNFTaskHolder);
	      pExternalObject->ExternalImplementation (
		pIVSNFTaskHolder,
		m_pSNFTask->selectorName (),
		m_pSNFTask->parameterCount (),
		m_pSNFTask->cardinality ()
	      );
	    } else {
	      IVSNFTaskHolderNC_T::Reference pIVSNFTaskHolder;
	      getRole (pIVSNFTaskHolder);
	      pExternalObject->ExternalImplementation (
		pIVSNFTaskHolder,
		m_pSNFTask->selectorName (),
		m_pSNFTask->parameterCount (),
		m_pSNFTask->cardinality ()
	      );
	    }
	} catch (...) {
	    g_iScheduler.decrementTaskCount ();
	    throw;
	}

	Trigger::Reference pDeadPeerTrigger (new Trigger (this, &ThisClass::onDeadPeer));
	pExternalObject->requestNoRouteToPeerCallback (m_pNoRouteToPeerTriggerTicket, pDeadPeerTrigger);
    }
}

void VSNFTaskHolder::start (ICollection *pExternalObject) {
    if (m_pSNFTask) {
	ICaller2::Reference pICaller2;
	getRole (pICaller2);
	if (g_iLogger.isActive ()) {
	    g_iLogger.printf (
		"+++%5u: %p->%s [%u x %u]\n", taskId (), pExternalObject,
		m_pSNFTask->selectorName (), m_pSNFTask->cardinality (), m_pSNFTask->parameterCount ()
	    );
	}
	try {
	    g_iScheduler.incrementTaskCount ();
            if (m_pSNFTask->returnCase () == VComputationUnit::Return_Intension) {
                pExternalObject->Bind (
                    pICaller2, m_pSNFTask->selectorName (), m_pSNFTask->parameterCount (), m_pSNFTask->cardinality ()
                );
            } else {
                pExternalObject->Invoke (
                    pICaller2, m_pSNFTask->selectorName (), m_pSNFTask->parameterCount (), m_pSNFTask->cardinality ()
                );
            }
	} catch (...) {
	    g_iScheduler.decrementTaskCount ();
	    throw;
	}

	Trigger::Reference pDeadPeerTrigger (new Trigger (this, &ThisClass::onDeadPeer));
	pExternalObject->requestNoRouteToPeerCallback (m_pNoRouteToPeerTriggerTicket, pDeadPeerTrigger);
    }
}

void VSNFTaskHolder::onDeadPeer (Trigger *pTrigger) {
    static bool const bSuppressException = V::GetEnvironmentBoolean ("VAcceptExtIntError");

    notify (19, "#19: VSNFTaskHolder::onDeadPeer: Bridge peer process is disconnected\n");
    onError (0, "Object Disconnected");
    if (!bSuppressException)
	raiseException (EC__ExternalInterfaceError, "Object Disconnected");
}

void VSNFTaskHolder::onEnd () {
    onError (0, "EOD");
}

void VSNFTaskHolder::onError (Vca::IError *pInterface, VString const &rMessage) {
    if (m_pSNFTask) {
	if (g_iLogger.isActive ())
	    g_iLogger.log (taskId (), "Error", rMessage);

	onFailure (pInterface, rMessage);

	m_pSNFTask->ReturnError (rMessage);
	wrapup ();
    }
}

void VSNFTaskHolder::onSent () {
}

void VSNFTaskHolder::wrapup () {
    if (m_pNoRouteToPeerTriggerTicket) {
	m_pNoRouteToPeerTriggerTicket->cancel ();
	m_pNoRouteToPeerTriggerTicket.clear ();
    }
    if (m_pSNFTask) {
	onSuccess ();

	g_iScheduler.decrementTaskCount ();
    //	m_pSNFTask.clear ();
    }
}

void VSNFTaskHolder::signalParameterError (
    IVSNFTaskHolder *pRole, 
    IVSNFTaskImplementation *pImplementation, 
    unsigned int xParameter, VString const & rError
) {
    Vxa::IVSNFTaskImplementation3::Pointer const pTI3 (
	dynamic_cast<Vxa::IVSNFTaskImplementation3*>(pImplementation)
    );
    Vxa::IVSNFTaskImplementation3NC::Pointer const pTI3NC (
	dynamic_cast<Vxa::IVSNFTaskImplementation3NC*>(pImplementation)
    );

    if (pTI3)
	pTI3->OnParameterError (xParameter, rError);
    else if (pTI3NC)
	pTI3NC->OnParameterError (xParameter, rError);

    ReturnError (dynamic_cast<Vxa::IVSNFTaskHolder1*>(pRole), rError);
}

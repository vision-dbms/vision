/*****  Vxa_VCollection Implementation  *****/

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

#include "Vxa_VCollection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType1.h"
#include "Vxa_VCallType2.h"

#include "Vxa_VTask.h"


/******************************
 ******************************
 *****                    *****
 *****  Vxa::VCollection  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCollection::VCollection () : m_pICollection (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCollection::~VCollection () {
}


/****************************
 ****************************
 *****  Role Callbacks  *****
 ****************************
 ****************************/

void Vxa::VCollection::Bind (
    ICollection *pRole, ICaller *pCaller, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask
) {
    if (pCaller) {
	VCallType2 iCallHandle (rMethodName, cParameters, cTask, pCaller);
//	invokeMethod_(iCallHandle);
    }
}

void Vxa::VCollection::Invoke (
    ICollection *pRole, ICaller *pCaller, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask
) {
    if (pCaller) {
	VCallType2 iCallHandle (rMethodName, cParameters, cTask, pCaller);
	invokeMethod_(iCallHandle);
    }
}

void Vxa::VCollection::QueryCardinality (ICollection *pRole, IVReceiver<cardinality_t> *pResultReceiver) {
    if (pResultReceiver)
	pResultReceiver->OnData (cardinality ());
}

void Vxa::VCollection::ExternalImplementation (
    ISingleton *pRole, IVSNFTaskHolder *pCaller, VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask
) {
    if (pCaller) {
	VCallType1 iCallHandle (rMethodName, cParameters, cTask, pCaller);
	invokeMethod_(iCallHandle);
    }
}

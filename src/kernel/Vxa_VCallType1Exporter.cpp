/*****  Vxa_VCallType1Exporter Implementation  *****/

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

#include "Vxa_VCallType1Exporter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType1Importer.h"


/************************************************
 ************************************************
 *****                                      *****
 *****  Vxa::VCallType1Exporter::ResultSet  *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType1Exporter::ResultSet::ResultSet () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType1Exporter::ResultSet::~ResultSet () {
}


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VCallType1Exporter  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType1Exporter::VCallType1Exporter (
    VTask *pTask, VCallType1Importer const &rCallImporter
) : BaseClass (pTask), VCallType1 (rCallImporter), m_bDisabled (false) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType1Exporter::~VCallType1Exporter () {
    if (disabled ()) {
    } else if (m_pResultSet) {
	m_pResultSet->transmitUsing (this);
    } else {
	returnNA ();
    }
}


/****************************
 ****************************
 *****  Result Storage  *****
 ****************************
 ****************************/

// bool storage:
bool Vxa::VCallType1Exporter::storeResult (bool_result_set_t::storage_t &rStorage, bool iValue) {
    return storeValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (bool_result_set_t::storage_t &rStorage, int iValue) {
    return promoteAndStoreValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (bool_result_set_t::storage_t &rStorage, float iValue) {
    return promoteAndStoreValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (bool_result_set_t::storage_t &rStorage, double iValue) {
    return promoteAndStoreValue (rStorage, iValue);
}

// int storage:
bool Vxa::VCallType1Exporter::storeResult (int_result_set_t::storage_t &rStorage, bool iValue) {
    return storeValue (rStorage, iValue ? 1 : 0);
}
bool Vxa::VCallType1Exporter::storeResult (int_result_set_t::storage_t &rStorage, int iValue) {
    return storeValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (int_result_set_t::storage_t &rStorage, float iValue) {
    return promoteAndStoreValue (rStorage, static_cast<double>(iValue));
}
bool Vxa::VCallType1Exporter::storeResult (int_result_set_t::storage_t &rStorage, double iValue) {
    return promoteAndStoreValue (rStorage, iValue);
}

// float storage:
bool Vxa::VCallType1Exporter::storeResult (float_result_set_t::storage_t &rStorage, bool iValue) {
    return storeValue (rStorage, static_cast<float>(iValue ? 1 : 0));
}
bool Vxa::VCallType1Exporter::storeResult (float_result_set_t::storage_t &rStorage, int iValue) {
    return promoteAndStoreValue (rStorage, static_cast<double>(iValue));
}
bool Vxa::VCallType1Exporter::storeResult (float_result_set_t::storage_t &rStorage, float iValue) {
    return storeValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (float_result_set_t::storage_t &rStorage, double iValue) {
    return promoteAndStoreValue (rStorage, iValue);
}

// double storage:
bool Vxa::VCallType1Exporter::storeResult (double_result_set_t::storage_t &rStorage, bool iValue) {
    return storeValue (rStorage, iValue ? 1 : 0);
}
bool Vxa::VCallType1Exporter::storeResult (double_result_set_t::storage_t &rStorage, int iValue) {
    return storeValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (double_result_set_t::storage_t &rStorage, float iValue) {
    return storeValue (rStorage, iValue);
}
bool Vxa::VCallType1Exporter::storeResult (double_result_set_t::storage_t &rStorage, double iValue) {
    return storeValue (rStorage, iValue);
}

// string storage:
bool Vxa::VCallType1Exporter::storeResult (string_result_set_t::storage_t &rStorage, VString const &rValue) {
    return storeValue (rStorage, rValue);
}

// object storage:
bool Vxa::VCallType1Exporter::storeResult (object_result_set_t::storage_t &rStorage, ISingleton *pValue) {
    return storeValue (rStorage, pValue);
}


/***************************
 ***************************
 *****  Result Return  *****
 ***************************
 ***************************/

bool Vxa::VCallType1Exporter::makeResultSetOf (VString const &rValue) {
    m_pResultSet.setTo (new string_result_set_t (cardinality (), rValue));
    return true;
}

bool Vxa::VCallType1Exporter::makeResultSetOf (ISingleton *pValue) {
    m_pResultSet.setTo (new object_result_set_t (cardinality (), pValue));
    return true;
}

void Vxa::VCallType1Exporter::logError (VError *pError) const {
    task()->onErrorEvent (pError);
}

bool Vxa::VCallType1Exporter::returnResult (VExportableDatum const &rDatum) {
    return rDatum.returnResultUsing (this);
}

//  conversions:
bool Vxa::VCallType1Exporter::returnResult (VExportableType *pType, short iValue) {
    return returnResult (pType, static_cast<int>(iValue));
}
bool Vxa::VCallType1Exporter::returnResult (VExportableType *pType, unsigned int iValue) {
    return returnResult (pType, static_cast<int>(iValue));
}
bool Vxa::VCallType1Exporter::returnResult (VExportableType *pType, unsigned short iValue) {
    return returnResult (pType, static_cast<int>(iValue));
}
bool Vxa::VCallType1Exporter::returnResult (VExportableType *pType, char const *pValue) {
    VString iValue (pValue);
    return returnResult (pType, iValue);
}
bool Vxa::VCallType1Exporter::returnResult (VExportableType *pType, export_return_t const &rpValue) {
    return returnResult (pType, static_cast<ISingleton*>(rpValue.referent ()));
}

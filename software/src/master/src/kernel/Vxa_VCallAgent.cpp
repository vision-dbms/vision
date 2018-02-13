/*****  Vxa_VCallAgent Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include <iostream>

/******************
 *****  Self  *****
 ******************/

#include "Vxa_VCallAgent.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****                   *****
 *****  Vxa::VCallAgent  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallAgent::VCallAgent (
    VTask *pTask, unsigned int cParameters
) : m_pTask (pTask), m_cParameters (cParameters), m_pFactory (m_cParameters), m_xParameter (0)
  , m_pIVSNFTaskImplementation (this), m_pIVSNFTaskImplementationNC (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallAgent::~VCallAgent () {
}

/***************************************
 ***************************************
 *****  Interface Implementations  *****
 ***************************************
 ***************************************/

/*************************************
 *****  IVSNFTaskImplementation  *****
 *************************************/

void Vxa::VCallAgent::SetToInteger (
    IVSNFTaskImplementation *, unsigned int xParameter, i32_array_t const &rValue
) {
    factory_reference_t pParameterFactory;
    if (getParameterFactory (pParameterFactory, xParameter) && pParameterFactory->createFromIntegers (rValue, this)) {
	onParameterReceipt (m_pTask, xParameter);
    } else {
	VString iMsg ("Vxa Parameters Retrieving Error: ");
	iMsg << xParameter;
	onFailure (0, iMsg);
    }
}

void Vxa::VCallAgent::SetToDouble (
    IVSNFTaskImplementation *, unsigned int xParameter, f64_array_t const &rValue
) {
    factory_reference_t pParameterFactory;
    if (getParameterFactory (pParameterFactory, xParameter) && pParameterFactory->createFromDoubles (rValue, this)) {
	onParameterReceipt (m_pTask, xParameter);
    } else {
	VString iMsg ("Vxa Parameters Retrieving Error: ");
	iMsg << xParameter;
	onFailure (0, iMsg);
    }
}

void Vxa::VCallAgent::SetToVString (
    IVSNFTaskImplementation *, unsigned int xParameter, str_array_t const &rValue
) {
    factory_reference_t pParameterFactory;
    if (getParameterFactory (pParameterFactory, xParameter) && pParameterFactory->createFromStrings (rValue, this)) {
	onParameterReceipt (m_pTask, xParameter);
    } else {
	VString iMsg ("Vxa Parameters Retrieving Error: ");
	iMsg << xParameter;
	onFailure (0, iMsg);
    }
}

void Vxa::VCallAgent::SetToObjects (
    IVSNFTaskImplementation *, unsigned int xParameter, obj_array_t const &rValue
) {
    raiseUnimplementedOperationException (typeid(*this),"SetToObjects");
}

void Vxa::VCallAgent::SetToS2Integers (
    IVSNFTaskImplementation2 *pRole, unsigned int xParameter, i32_s2array_t const &rValue
) {
    SetToInteger (pRole, xParameter, rValue);
}

void Vxa::VCallAgent::OnParameterError (
    unsigned int xParameter, VString const &rMsg
) {
    onParameterReceipt (m_pTask, xParameter);
    reportCompletion ();
    m_pTask->kill ();
    VString iMsg ("Vxa Parameters Retrieving Error: ");
    iMsg << xParameter;
    iMsg << "\n" << rMsg;
    onFailure (0, iMsg);
}

void Vxa::VCallAgent::OnParameterError (
    IVSNFTaskImplementation3 *pRole, unsigned int xParameter, VString const &rMsg
) {
    OnParameterError (xParameter, rMsg);
}

void Vxa::VCallAgent::OnParameterError (
    IVSNFTaskImplementation3NC *pRole, unsigned int xParameter, VString const &rMsg
) {
    OnParameterError (xParameter, rMsg);
}


/***********************************
 ***********************************
 *****  Parameter Acquisition  *****
 ***********************************
 ***********************************/

bool Vxa::VCallAgent::setParameterFactory (ParameterFactory *pParameterFactory) {
    std::cerr
        << this
        << "setParameterFactory["
        << m_xParameter
        << "]: "
        << task ()->methodName ()
        << std::endl;
    m_pFactory[m_xParameter].setTo (pParameterFactory);
    return onParameterRequest (m_pTask, m_xParameter++);
}

bool Vxa::VCallAgent::getParameterFactory (factory_reference_t &rpParameterFactory, unsigned int xParameter) {
    std::cerr
        << this
        << "getParameterFactory["
        << xParameter
        << "/"
        << m_xParameter
        << "]: "
        << task ()->methodName ()
        << std::endl;
    if (xParameter < m_cParameters)
	rpParameterFactory.claim (m_pFactory[xParameter]);
    else
	rpParameterFactory.clear ();
    return rpParameterFactory.isntNil ();
}

bool Vxa::VCallAgent::returnError (VString const &rMessage) const {
    m_pTask->kill ();
    return returnError_(rMessage);
}

/***********************
 ***********************
 *****  Reporting  *****
 ***********************
 ***********************/

void Vxa::VCallAgent::onFailure (Vca::IError *pInterface, VString const &rMessage) {
}

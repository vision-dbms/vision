/*****  M_CPD Implementation  *****/

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

#include "M_CPD.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verr.h"


/***************************************
 ***************************************
 *****  Assertions and Exceptions  *****
 ***************************************
 ***************************************/

void M_CPD::RaiseReferenceCountException (ReferenceCountExceptionCase xExceptionCase) const {
    char const* pMessage;
    switch (xExceptionCase) {
    case Attach:
	pMessage = "M_CPD::GetCPD: CPD not in use";
	break;
    case Reclaim:
	pMessage = "M_CPD::discard: CPD not in use";
	break;
    case POPClearTarget:
	pMessage = "M_CPD::ClearReference: CPD not in use";
	break;
    case POPCopyTarget:
	pMessage = "M_CPD::StoreReference: Destination CPD not in use";
	break;
    case POPCopySource:
	pMessage = "M_CPD::StoreReference: Source CPD not in use";
	break;
    default:
	pMessage = "M_CPD: CPD not in use";
	break;
    }
    ERR_SignalFault (EC__MError, pMessage);
}

void M_CPD::AssertLiveness (ReferenceCountExceptionCase xExceptionCase) const {
    if (m_iReferenceCount < 1)
	RaiseReferenceCountException (xExceptionCase);
}

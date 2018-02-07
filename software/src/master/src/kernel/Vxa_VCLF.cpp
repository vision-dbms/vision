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

#include "Vxa_VCLF.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType1.h"


/***********************
 ***********************
 *****             *****
 *****  Vxa::VCLF  *****
 *****             *****
 ***********************
 ***********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCLF::VCLF () : m_pISingleton (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCLF::~VCLF () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vxa::VCLF::ExternalImplementation (
    ISingleton *pRole, IVSNFTaskHolder *pCaller, VString const &rMessageName, cardinality_t cParameters, cardinality_t cTask
) {
    VString iMessage;
    iMessage.printf ("VCLF::ExternalImplementation(%s): Not Supported", rMessageName.content ());

    VCallType1 iCallHandle (rMessageName, cParameters, cTask, pCaller);
    iCallHandle.returnError (iMessage);
}

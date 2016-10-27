/*****  Vca_VPassiveProcessAgent Implementation *****/

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

#include "Vca_VPassiveProcessAgent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************************
 *********************************************
 *****                                   *****
 *****  class Vca::VPassiveProcessAgent  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPassiveProcessAgent::VPassiveProcessAgent (
    VString const &rDirectivePrefix
) : BaseClass (rDirectivePrefix), m_pDirectiveGofer (new VGoferExogenous_String ())  {
}

/*************************
 ************************* 
 *****  Destruction  *****
 ************************* 
 *************************/

Vca::VPassiveProcessAgent::~VPassiveProcessAgent () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void Vca::VPassiveProcessAgent::processCallbackRequest (CallData const &rCallData) {
//  Make the directive available to the process launcher, ...
    VString iDirective;
    supplyDirective (iDirective, rCallData);

    m_pDirectiveGofer->setTo (iDirective);

//  ... and launch the process:
    BaseClass::processCallbackRequest (rCallData);
}

void Vca::VPassiveProcessAgent::supplyDirective_(
    VString &rDirective, VString const &rDirectivePrefix, CallData const &rCallData
) const {
    VString iCallID;
    rCallData.getCallIDAsString (iCallID);

    rDirective.setTo (rDirectivePrefix);
    rDirective << " -- -callback=" << iCallID << "@" << rCallData.callerAddress ();
}

/*****  Vxa_VCollectableObject Implementation  *****/

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

#include "Vxa_VCollectableObject.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VCollectableObject  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCollectableObject::VCollectableObject () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCollectableObject::~VCollectableObject () {
}

/*******************************
 *******************************
 *****  Ticketing Support  *****
 *******************************
 *******************************/

void Vxa::VCollectableObject::GetHandle (VResultBuilder &rRB) {
    GetTicketImplementation (rRB, false);
}

void Vxa::VCollectableObject::GetTicket (VResultBuilder &rRB) {
    GetTicketImplementation (rRB, true);
}

void Vxa::VCollectableObject::GetTicketImplementation (VResultBuilder &rRB, bool bSingleUse) {
    VString iTicket;
    if (m_iIdentity.getTicket (iTicket, bSingleUse))
        rRB = iTicket;
    else
        rRB = false;
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vxa::VCollectableObject::ClassBuilder  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCollectableObject::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : m_pClass (pClass) {
    defineMethod (".id" , &VCollectableObject::GetTicket);
    defineMethod (".id:", &VCollectableObject::GetTicketImplementation);
}

/**************************************
 **************************************
 *****  Method Definition Helpers *****
 **************************************
 **************************************/

bool Vxa::VCollectableObject::ClassBuilder::defineMethodImpl (VString const &rName, VMethod *pMethod) {
    m_iHelpInfo << rName << "\n";

    return m_pClass->defineMethod (rName, pMethod);
}

bool Vxa::VCollectableObject::ClassBuilder::defineDefaultImpl (VMethod *pMethod) {
    return m_pClass->defineDefault (pMethod);
}

bool Vxa::VCollectableObject::ClassBuilder::defineHelp (VString const &rWhere) {
    VString iHelpInfo;
    iHelpInfo << "The class " << rWhere << " supports the following methods:\nhelp\n" << m_iHelpInfo;
    return defineConstant ("help", iHelpInfo);
}

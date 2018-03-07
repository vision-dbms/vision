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

bool Vxa::VCollectableObject::GetObjectForTicket (
    Reference &rObject, VString const &rTicket
) {
    return false;
}

void Vxa::VCollectableObject::GetHandle (VResultBuilder &rRB) {
    rRB = GetHandleImpl ();
}

void Vxa::VCollectableObject::GetTicket (VResultBuilder &rRB) {
    rRB = GetTicketImpl ();
}

VString Vxa::VCollectableObject::GetTicketImpl (bool bSingleUse) {
    VString iTicket;
    iTicket.printf ("!*%c%p", (bSingleUse ? '.' : '*'), this);
    return iTicket;
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
    defineMethod (".getHandle", &VCollectableObject::GetHandle);
    defineMethod (".getTicket", &VCollectableObject::GetTicket);
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

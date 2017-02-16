/*****  Vca_VInterfaceMember Implementation  *****/

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

#include "Vca_VInterfaceMember.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VTypeInfoHolder.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VInterfaceMember  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VInterfaceMember::VInterfaceMember (char const *pName, unsigned int xMember)
    : m_pName (pName), m_xMember (xMember), m_pSuccessor (0)
{
}

Vca::VInterfaceMember::VInterfaceMember (Initializer const &rInitializer)
    : m_pName (rInitializer.m_pName), m_xMember (rInitializer.m_xMember)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VInterfaceMember::~VInterfaceMember () {
}


/*********************************
 *********************************
 *****  Member Registration  *****
 *********************************
 *********************************/

void Vca::VInterfaceMember::registerWithTypeInfoHolder (
    VTypeInfoHolderInstance &rTypeInfoHolderInstance
) {
    rTypeInfoHolderInstance.registerMember (this);
}

/***************************
 ***************************
 *****  Member Lookup  *****
 ***************************
 ***************************/

Vca::VInterfaceMember const *Vca::VInterfaceMember::firstMemberWithSignature (
    VTypeInfo::ParameterSignature const &rParameterSignature
) const {
    for (VInterfaceMember const *pMember = this; pMember; pMember = pMember->successor ())
	if (pMember->parameterSignature_() == rParameterSignature)
	    return pMember;
    return 0;
}

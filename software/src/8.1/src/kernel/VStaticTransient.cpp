/*****  VStaticTransient Implementation  *****/

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

#include "VStaticTransient.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VMutex.h"


/****************************************
 ****************************************
 *****                              *****
 *****  VStaticTransient::ListLink  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VStaticTransient::Link::Link (Instance* pInstance) : m_pInstance (pInstance) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VStaticTransient::Link::~Link () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VStaticTransient::Link::interlockedPush (Instance* pInstance) {
    m_pListLink.interlockedPush (&pInstance->listLink(), &ThisClass::m_pListLink);
}

bool VStaticTransient::Link::interlockedPop (InstancePointer& rpInstance) {
    Pointer pListLink;
    if (m_pListLink.interlockedPop (pListLink, &ThisClass::m_pListLink)) {
	rpInstance.setTo (pListLink->instance ());
	return true;
    }
    return false;
}


/******************************
 ******************************
 *****                    *****
 *****  VStaticTransient  *****
 *****                    *****
 ******************************
 ******************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

namespace {
    using namespace V;
    typedef VEternal<VMutex> EternalMutex;

    EternalMutex g_iInitializationMutex;
}

VStaticTransient::EternalLink VStaticTransient::g_pListHead;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VStaticTransient::VStaticTransient () : m_pListLink (this) {
    listHead().interlockedPush (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VStaticTransient::~VStaticTransient () {
    listLink().detach (this);
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void VStaticTransient::InitializeAllInstances () {
    V::VMutex::Claim iInitializationLock (g_iInitializationMutex);

    Pointer pInstance;
    while (listHead().interlockedPop (pInstance)) {
	if (pInstance)
	    pInstance->initialize ();
    }
}

void VStaticTransient::initialize () {
}

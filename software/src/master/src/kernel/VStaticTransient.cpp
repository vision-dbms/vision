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


/************************************
 ************************************
 *****                          *****
 *****  VStaticTransient::Link  *****
 *****                          *****
 ************************************
 ************************************/

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


/************************************
 ************************************
 *****                          *****
 *****  VStaticTransient::List  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VStaticTransient::List::List () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VStaticTransient::List::~List () {
}

/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VStaticTransient::List::interlockedPush (Instance* pInstance) {
    m_pListHead.interlockedPush (&pInstance->listLink(), &Link::m_pListLink);
}

bool VStaticTransient::List::interlockedPop (InstancePointer& rpInstance) {
    Link::Pointer pListHead;
    if (m_pListHead.interlockedPop (pListHead, &Link::m_pListLink)) {
	rpInstance.setTo (pListHead->instance ());
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

VStaticTransient::EternalList VStaticTransient::g_pListHead;


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

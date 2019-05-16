/*****  V_VReferenceable Implementation  *****/
#define V_VReferenceable_Instantiations

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

#define V_VReferenceable
#include "VReferenceable.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "V_VThread.h"


/***********************************
 ***********************************
 *****                         *****
 *****  V::VReferenceableBase  *****
 *****                         *****
 ***********************************
 ***********************************/

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void V::VReferenceableBase::MetaMaker () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VReferenceableBase::~VReferenceableBase () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

V::VString V::VReferenceableBase::rttName () const {
    VRTTI const iRTTI (typeid(*this));
    return VString (iRTTI.name ());
}

/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void V::VReferenceableBase::displayInfo (char const *pWhat) const {
    displayInfo ();
    display ("%s", pWhat);
}

void V::VReferenceableBase::displayInfo () const  {
    display ("\n %s %p:", rttName ().content (), this);
}

void V::VReferenceableBase::getInfo (VString &rResult, const VString &rPrefix) const {
    rResult << rPrefix;
    rResult.printf ("%s %p", rttName ().content (), this);
}

void V::VReferenceableBase::reclaimThis () {
    VThread::ReclaimObject (this);
}

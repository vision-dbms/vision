/*****  VReferenceable Implementation  *****/
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

#include "V_VThread.h"
#include "V_VString.h"


/***********************************
 ***********************************
 *****                         *****
 *****  V::ThreadModel::Multi  *****
 *****                         *****
 ***********************************
 ***********************************/

/*************************
 *************************
 *****  Reclamation  *****
 *************************
 *************************/

void V::ThreadModel::Multi::reclaim (VReferenceableBase *pObject) {
    VThread::ReclaimObject (pObject);
}


/************************************
 ************************************
 *****                          *****
 *****  V::ThreadModel::Single  *****
 *****                          *****
 ************************************
 ************************************/

/*************************
 *************************
 *****  Reclamation  *****
 *************************
 *************************/

namespace {
    class Guard {
	DECLARE_FAMILY_MEMBERS (Guard,void);

    //  Globals
    private:
	static bool g_bSet;

    //  Construction
    public:
	Guard () {
	    g_bSet = true;
	}

    //  Destruction
    public:
	~Guard () {
	    g_bSet = false;
	}

    //  Access
    public:
	static bool isSet () {
	    return g_bSet;
	}
	static bool isntSet () {
	    return !g_bSet;
	}
    };

    bool Guard::g_bSet = false;

    V::VReferenceableBase::ReclamationList g_pReclamationList;
}

void V::ThreadModel::Single::reclaim (VReferenceableBase *pObject) {
    if (Guard::isSet ())
	g_pReclamationList.add (pObject);
    else {
	Guard iGuard;
	static_cast<VReferenceableImplementation_<ThisClass>*>(pObject)->die ();
	g_pReclamationList.reap ();
    }
}


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

VString V::VReferenceableBase::rttName () const {
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

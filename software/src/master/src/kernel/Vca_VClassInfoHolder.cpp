/*****  Vca_VClassInfoHolder Implementation  *****/

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

#include "Vca_VClassInfoHolder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vca::VClassInfoHolder::Property  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VClassInfoHolder::Property::Property () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VClassInfoHolder::Property::~Property () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VClassInfoHolder::Property::getTrackableValue (
    IClient *pValueSink, ITicketSink *pTicketSink, VRolePlayer*pContainer
) const {
    return (
	getTrackableImpl (dynamic_cast<IVReceiver<VString const&>*>(pValueSink),pContainer) ||
	getTrackableImpl (dynamic_cast<IVReceiver<unsigned int>*>(pValueSink),pContainer) ||
	getTrackableImpl (dynamic_cast<IVReceiver<unsigned __int64>*>(pValueSink),pContainer) ||
	getTrackableImpl (dynamic_cast<IVReceiver<float>*>(pValueSink),pContainer) ||
	getTrackableImpl (dynamic_cast<IVReceiver<double>*>(pValueSink),pContainer)
    ) && sayNoToSubscription (pTicketSink);
}

bool Vca::VClassInfoHolder::Property::sayNoToSubscription (ITicketSink *pTicketSink) const {
    if (pTicketSink)
	pTicketSink->OnData (0);
    return true;
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VClassInfoHolder  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VClassInfoHolder::VClassInfoHolder (std::type_info const &rTypeInfo) : m_iRTTI (rTypeInfo), m_bIncomplete (true) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VClassInfoHolder::~VClassInfoHolder () {
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool Vca::VClassInfoHolder::getProperty (Property::Reference &rpProperty, char const *pName) const {
    unsigned int xProperty;
    if (m_mProperties.Locate (pName, xProperty)) {
	rpProperty.setTo (m_mProperties[xProperty]);
	return true;
    }
    return false;
}

void Vca::VClassInfoHolder::getProperties (property_map_t &rResultSink) const {
    for (unsigned int xProperty = 0; xProperty < m_mProperties.cardinality (); xProperty++) {
	unsigned int xInsertion;
	rResultSink.Insert (m_mProperties.key (xProperty), xInsertion);
	rResultSink[xInsertion] = m_mProperties[xProperty];
    }
}

V::VString Vca::VClassInfoHolder::baseNames () const {
    VString iResult;
    for (unsigned int xElement = 0; xElement < m_mBases.cardinality (); xElement++)
	iResult << m_mBases[xElement]->typeName () << "\n";
    return iResult;
}

V::VString Vca::VClassInfoHolder::propertyNames () const {
    VString iResult;
    for (unsigned int xElement = 0; xElement < m_mProperties.cardinality (); xElement++) {
	iResult << m_mProperties.key(xElement) << "\n";
    }
    return iResult;
}

V::VString Vca::VClassInfoHolder::summary () const {
    VString iResult;
    iResult
	<< "Class " << typeName () << ":\n\n"
	<< "Base Classes:\n" << baseNames () << "\n\n"
	<< "Properties:\n" << propertyNames ();
    return iResult;
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

Vca::VClassInfoHolder &Vca::VClassInfoHolder::addBase (ThisClass *pInstance) {
    m_mBases.Insert (pInstance);
    pInstance->getProperties (m_mProperties);
    return *this;
}
Vca::VClassInfoHolder &Vca::VClassInfoHolder::addProperty (VString const &rName, Property *pInstance) {
    unsigned int xInstance;
    m_mProperties.Insert (rName, xInstance);
    m_mProperties[xInstance] = pInstance;
    return *this;
}

void Vca::VClassInfoHolder::markCompleted () {
    m_bIncomplete = false;
}

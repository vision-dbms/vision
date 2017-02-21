/*****  Vxa_VCollectableCollection Implementation  *****/

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

#include "Vxa_VCollectableCollection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCollectableObject.h"
#include "Vxa_VMonotypeMapMaker.h"


/*****************************************
 *****************************************
 *****                               *****
 *****  Vxa::VCollectableCollection  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCollectableCollection::VCollectableCollection (VClass *pClass) : m_pClass (pClass), m_sCollection (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCollectableCollection::~VCollectableCollection () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

bool Vxa::VCollectableCollection::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection) {
    return pMapMaker->transmitValues (pExporter, this, rInjection);
}

bool Vxa::VCollectableCollection::transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker) {
    return pMapMaker->transmitValues (pExporter, this);
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vxa::VCollectableCollection::attach (VCollectableObject *pObject) {
    return pObject && pObject->attach (this, m_sCollection++);
}

bool Vxa::VCollectableCollection::detach (VCollectableObject *pObject) {
    return pObject && pObject->detach (this);
}

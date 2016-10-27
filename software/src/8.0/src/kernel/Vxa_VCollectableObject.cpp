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

Vxa::VCollectableObject::VCollectableObject () : m_xObject (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCollectableObject::~VCollectableObject () {
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

bool Vxa::VCollectableObject::attach (VCollectableCollection *pCollection, collection_index_t xObject) {
    if (m_pCollection.interlockedSetIfNil (pCollection)) {
	m_xObject = xObject;
	return true;
    }
    return false;
}

bool Vxa::VCollectableObject::detach (VCollectableCollection *pCollection) {
    return m_pCollection.interlockedClearIf (pCollection);
}
